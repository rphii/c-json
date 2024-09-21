#!/bin/bash
set -e

#OPTIMIZE="-DNDEBUG -march=native -O3 -rdynamic"
OPTIMIZE="-DNDEBUG -march=native -O3 -fdevirtualize-at-ltrans -fipa-pta -fno-semantic-interposition -flto=auto -pipe"

JEMALLOC="-I`jemalloc-config --includedir` \
		  -L`jemalloc-config --libdir` -Wl,-rpath,`jemalloc-config --libdir` \
		  -ljemalloc `jemalloc-config --libs`"

CFLAGS="$OPTIMIZE $JEMALLOC -Wall -Wextra -Wimplicit -Wfatal-errors -Wno-unused-parameter"
CC=gcc
$CC $CFLAGS -I../src bench.c ../src/str.c ../src/json.c ../src/file.c ../src/platform.c ../src/info.c ../src/vstr.c -o bench

###valgrind --tool=callgrind --dump-before=KonqMainWindow::slotReload ./bench
###valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes --collect-atstart=no --instr-atstart=no ./bench


#valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes --callgrind-out-file=bench.out.callgrind ./bench
#kcachegrind bench.out.callgrind

./bench

