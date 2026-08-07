#! /bin/bash

CC="clang"
CFLAGS="-Wall -Wextra -O3 -ggdb"
FILE="hash.c"
OUT="hash"

$CC $CFLAGS $FILE -o $OUT
./$OUT
