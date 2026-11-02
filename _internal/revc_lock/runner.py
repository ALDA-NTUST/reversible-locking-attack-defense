"""Run the unchanged experiment stages in isolated processes and directories."""
from pathlib import Path
import hashlib
import json
import os
import shutil
import subprocess
import tempfile
import time

from .real import read_real, validate_original, validate_locked

ROOT = Path(__file__).resolve().parents[1]


def binary_paths(build_dir=None):
    build = Path(build_dir).resolve() if build_dir else ROOT / 'build'
    return build / 'revc-native', build / 'minisat'


def _publish(source, destination, force):
    destination = Path(destination).resolve()
    if destination.exists() and not force:
        raise ValueError('Output already exists; use --force: ' + str(destination))
    destination.parent.mkdir(parents=True, exist_ok=True)
    fd, name = tempfile.mkstemp(prefix='.' + destination.name + '.', dir=str(destination.parent))
    os.close(fd)
    try:
        shutil.copyfile(str(source), name)
        os.replace(name, str(destination))
    finally:
        if os.path.exists(name): os.unlink(name)


def operate(mode, input_path, output_path=None, reference=None, build_dir=None,
            timeout=120, force=False, log_dir=None):
    input_path = Path(input_path).resolve()
    circuit = read_real(input_path)
    if mode in {'lock', 'defend'}:
        validate_original(circuit)
    elif mode == 'attack':
        validate_locked(circuit)
    if reference:
        reference = Path(reference).resolve()
        reference_circuit = read_real(reference)
        if mode == 'attack' and reference_circuit['inputs'] != (circuit['inputs'] - 2) // 4:
            raise ValueError('Reference line count does not match the locked circuit')
    if mode == 'verify' and not reference:
        raise ValueError('verify requires a second circuit')
    if mode not in {'lock', 'defend', 'attack', 'verify'}:
        raise ValueError('Unknown operation: ' + mode)
    if output_path:
        output_path = Path(output_path).resolve()
        if output_path == input_path or (reference and output_path == reference):
            raise ValueError('Output must differ from the input and reference')
        if output_path.exists() and not force:
            raise ValueError('Output already exists; use --force: ' + str(output_path))
    native, minisat = binary_paths(build_dir)
    required = [native] + ([minisat] if mode == 'verify' or reference else [])
    for binary in required:
        if not binary.is_file() or not os.access(str(binary), os.X_OK):
            raise ValueError('Missing executable {}. Run python3 setup.py from the repository root.'.format(binary))
    # EXORCISM scratch files use cwd; a subprocess per request also isolates its
    # global state and C rand() seed. No shell command contains user paths.
    with tempfile.TemporaryDirectory(prefix='revc-job-') as temporary:
        work = Path(temporary)
        for directory in ['benchmark', 'enc_benchmark', 'programs']:
            (work / directory).mkdir()
        shutil.copyfile(str(input_path), str(work / 'benchmark/input.real'))
        if reference:
            shutil.copyfile(str(reference), str(work / 'benchmark/reference.real'))
        if minisat.is_file(): (work / 'programs/minisat').symlink_to(minisat)
        if mode in {'lock', 'defend'}:
            args = [mode, 'benchmark/input.real', 'enc_benchmark/output.real']
            produced = work / 'enc_benchmark/output.real'
        elif mode == 'attack':
            args = [mode, 'benchmark/input.real', 'benchmark/reference.real' if reference else '']
            produced = work / 'RE_version.real'
        else:
            args = [mode, 'benchmark/input.real', 'benchmark/reference.real']
            produced = None
        started = time.monotonic()
        try:
            process = subprocess.run([str(native)] + args, cwd=str(work),
                stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, timeout=timeout)
        except subprocess.TimeoutExpired:
            raise ValueError('Operation exceeded {} seconds; increase --timeout'.format(timeout))
        elapsed = time.monotonic() - started
        if process.returncode:
            raise ValueError('Native stage failed (exit {}): {}'.format(
                process.returncode, (process.stderr or process.stdout).strip()))
        result = dict(operation=mode, input=str(input_path), wall_seconds=elapsed,
                      native_stdout=process.stdout.strip())
        tokens = process.stdout.strip().rstrip(',').split(',')
        if mode in {'lock', 'defend'}:
            if len(tokens) != 6:
                raise ValueError('Unexpected encryption report: ' + process.stdout)
            result['encryption_seconds'] = float(tokens[5])
        elif mode == 'attack':
            result['attack_seconds'] = float(tokens[0])
        if produced is not None:
            if not produced.is_file(): raise ValueError('Native stage did not produce a circuit')
            meta = read_real(produced)
            result.update(inputs=meta['inputs'], gates=meta['gates'],
                          sha256=hashlib.sha256(produced.read_bytes()).hexdigest())
            if output_path:
                _publish(produced, output_path, force)
                result['output'] = str(output_path)
        solver_log = work / 'tmp_result.txt'
        if reference or mode == 'verify':
            if not solver_log.is_file(): raise ValueError('Missing MiniSat result')
            words = solver_log.read_text().split()
            if 'UNSATISFIABLE' in words: result['equivalence'] = 'equivalent'
            elif 'SATISFIABLE' in words: result['equivalence'] = 'non_equivalent'
            else: raise ValueError('MiniSat did not produce SATISFIABLE or UNSATISFIABLE')
        if log_dir:
            logs = Path(log_dir); logs.mkdir(parents=True, exist_ok=True)
            (logs / 'stdout.txt').write_text(process.stdout)
            (logs / 'stderr.txt').write_text(process.stderr)
            if solver_log.is_file(): shutil.copyfile(str(solver_log), str(logs / 'minisat.txt'))
            (logs / 'result.json').write_text(json.dumps(result, indent=2))
        return result
