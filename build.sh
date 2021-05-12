#!/bin/bash

# This code is licensed under MIT license (see LICENSE for details)
# (c) 2021 Andrew V. Jones

set -eu

cflags="-Wall -Wextra -Wpedantic -Werror -std=c99 -g"
ldflags="-ldw -ldl -lunwind"

gcc $cflags -c crash.c -o crash.o
gcc $cflags -c show_backtrace.c -o show_backtrace.o
gcc $cflags -c main.c -o main.o

gcc crash.o show_backtrace.o main.o $ldflags -o main

# EOF
