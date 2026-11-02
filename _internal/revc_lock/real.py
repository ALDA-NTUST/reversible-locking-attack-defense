"""Validate the flat NOT/CNOT/Toffoli/MCT subset used by the ICCD experiments."""
from pathlib import Path
import re


def read_real(path):
    path = Path(path)
    try:
        text = path.read_text(encoding='utf-8-sig')
    except (OSError, UnicodeError) as e:
        raise ValueError('Cannot read circuit {}: {}'.format(path, e))
    header, gates, active, ended = {}, [], False, False
    for line_number, raw in enumerate(text.splitlines(), 1):
        line = raw.split('#', 1)[0].strip()
        if not line:
            continue
        tokens = line.split()
        if tokens[0] == '.begin':
            if active or ended:
                raise ValueError('Duplicate .begin at line {}'.format(line_number))
            active = True
        elif tokens[0] == '.end':
            if not active:
                raise ValueError('Unexpected .end at line {}'.format(line_number))
            active, ended = False, True
        elif active:
            if not re.fullmatch(r't[1-9][0-9]*', tokens[0]):
                raise ValueError('Only flat tN (MCT) gates are supported; line {}'.format(line_number))
            if int(tokens[0][1:]) != len(tokens) - 1:
                raise ValueError('Gate size mismatch at line {}'.format(line_number))
            gates.append(tokens)
        elif tokens[0].startswith('.') and not ended:
            key = tokens[0][1:]
            if key in {'inputbus', 'outputbus', 'state'}:
                # RevLib's optional bus metadata is handled by the upstream reader.
                if len(tokens) < 3:
                    raise ValueError('Malformed .{} at line {}'.format(key, line_number))
                continue
            if key not in {'version', 'numvars', 'variables', 'inputs', 'outputs', 'constants', 'garbage'}:
                raise ValueError('Unsupported directive .{} at line {}'.format(key, line_number))
            if key in header:
                raise ValueError('Duplicate header .{}'.format(key))
            header[key] = tokens[1:]
        else:
            raise ValueError('Unexpected content at line {}'.format(line_number))
    if active or not ended:
        raise ValueError('Circuit must contain one complete .begin/.end block')
    try:
        count = int(header['numvars'][0])
        variables = header['variables']
    except (KeyError, ValueError, IndexError):
        raise ValueError('Missing or invalid .numvars/.variables')
    if count < 1 or len(variables) != count or len(set(variables)) != count:
        raise ValueError('Invalid number of variables or duplicate variable names')
    if any(v.startswith('-') for v in variables):
        raise ValueError('Variable names must not start with a minus sign')
    constants = ''.join(header.get('constants', ['-' * count]))
    garbage = ''.join(header.get('garbage', ['-' * count]))
    if len(constants) != count or set(constants) - set('-01'):
        raise ValueError('Invalid .constants')
    if len(garbage) != count or set(garbage) - set('-01'):
        raise ValueError('Invalid .garbage')
    for key in ['inputs', 'outputs']:
        if key in header and len(header[key]) != count:
            raise ValueError('Wrong number of .{} labels'.format(key))
    for gate in gates:
        wires = [v.lstrip('-') for v in gate[1:]]
        if any(v not in variables for v in wires) or len(set(wires)) != len(wires):
            raise ValueError('Unknown or repeated wire in gate: ' + ' '.join(gate))
        if gate[-1].startswith('-'):
            raise ValueError('Gate targets cannot be negated')
    return dict(inputs=count, gates=len(gates), variables=variables,
                constants=constants, garbage=garbage, gate_tokens=gates)


def validate_original(circuit):
    if not 3 <= circuit['inputs'] <= 249:
        raise ValueError('Legacy encryption supports 3..249 circuit lines (EXORCISM limit)')
    if not circuit['gates']:
        raise ValueError('Legacy encryption requires at least one gate')


def validate_locked(circuit):
    count = circuit['inputs']
    if count < 14 or count > 998 or (count - 2) % 4:
        raise ValueError('Attack expects a legacy 4*n+2-line RevC-Lock circuit')
    n = (count - 2) // 4
    gates, variables = circuit['gate_tokens'], circuit['variables']
    # The legacy attack assumes two n-CNOT + MCT restore blocks. Reject other
    # circuit families instead of passing them to its unchecked iterator loop.
    if len(gates) < 3 * n + 2:
        raise ValueError('Locked circuit is missing the legacy restore/input prefix')
    def block(start):
        if len(gates) <= start + n: return False
        if any(g[0] != 't2' or g[1].startswith('-') for g in gates[start:start + n]): return False
        g = gates[start + n]
        return len(g) == n + 2 and all(c.startswith('-') for c in g[1:-1])
    if not block(0) or not (block(n + 1) or block(2 * n + 1)):
        raise ValueError('Unsupported legacy restore-unit structure')
    return n
