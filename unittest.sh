#!/bin/bash
set -euo pipefail

# Verify that QCC produces valid quines by:
# 1. Transforming the C program specified by first argument
# 2. Compiling it with gcc
# 3. Executing the resulting binary
# 4. Verify that the stdout output is identical to the transformed source.

source="$1"

sans_extension="${source%.*}"

quined_src="${sans_extension}_quined.c"
quined_bin="${sans_extension}_quined"

quine_output=$(mktemp)

./qcc "$source" > "$quined_src"
gcc -o "$quined_bin" "$quined_src"
./"$quined_bin" > "$quine_output"

if diff -q "$quined_src" "$quine_output" > /dev/null
then
    echo "OK"
else
    echo "error: files differ..."
    meld "$quined_src" "$quine_output" &
fi
