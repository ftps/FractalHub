#!/bin/bash

rm -f exe

make

if [ -f "exe" ]; then
    echo -e "Starting run . . .\n\n"
    time ./exe $1
fi