import sys
import os
import tempfile


for i, filename in enumerate(sys.argv[1:]):
  if not os.path.isfile(filename):
    print(f"Non-existent file: {filename}", file=sys.stderr)
    sys.exit(+1)

  name = chr(ord('A') + i)
  print(f'#ifdef _COMPILE_{name}_')
  with open(filename, 'r') as f:
    for line in f:
      print(line, end='')
  print(f'#endif')
