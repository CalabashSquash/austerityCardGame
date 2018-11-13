#!/bin/bash
CC=gcc
EXEC=prac1
SRC=prac1.c
if [ $# -lt 1 ]; then
    printf "Usage: %s <input>\n" "$0" >&2
    exit 1
fi

if [ "$SRC" -nt "$EXEC" ]
then
    echo "Rebuilding $EXEC..."
    $CC -o $EXEC $SRC
fi
printValue=`./$EXEC $1`
if [ "$?" == '1' ];
then
    echo "Error!"
    exit
fi

echo "Final = $printValue"
echo "done."




