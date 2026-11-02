#!/usr/bin/env python3
"""Install the legacy tools. All build details live under _internal/."""
from pathlib import Path
import subprocess
import sys

root = Path(__file__).resolve().parent
raise SystemExit(subprocess.call([sys.executable, str(root / '_internal/scripts/setup.py')] + sys.argv[1:]))
