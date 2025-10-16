#!/usr/bin/bash

SOURCES=(
    "term/term.c"
    "fs/fs.c"
    "input/input.c"
    "main.c"
)
OBJECTS=($(
printf 'src/%s\n' "${SOURCES[@]}" | xargs -n 1 -P "$(nproc)" \
    bash -c 'set -x
for src; do
    obj="out/${src%.c}.o"
    mkdir -p "$(dirname "$obj")"
    gcc -std=gnu2x -g -O0 -Wall -Wextra -pedantic -c "$src" -o "$obj"
    echo "$obj"
done
set +x' bash
))

gcc -o main "${OBJECTS[@]}"