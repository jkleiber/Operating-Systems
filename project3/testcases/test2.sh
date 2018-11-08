#!/bin/bash

echo ">./zformat"
./zformat

for i in `seq 15`; do
    echo ">./zmkdir"
    ./zmkdir $i
done

echo ">./zmkdir"
./zmkdir 16