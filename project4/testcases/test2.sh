#!/bin/bash

echo ">./zformat"
./zformat

for i in `seq 15`; do
    echo ">./zmkdir $i"
    ./zmkdir $i
done

echo ">./zmkdir"
./zmkdir 16

for i in `seq 16`; do
    echo ">./zrmdir $i"
    ./zrmdir $i
done

./zfilez