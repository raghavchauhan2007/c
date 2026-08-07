#! /bin/bash

CC="clang"
CFLAGS="-Wall -Wextra -O3 -ggdb"
FILE="dijkstra.c"
OUT="dijkstra"

$CC $CFLAGS $FILE -o $OUT
./$OUT
