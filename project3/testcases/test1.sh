#!/bin/bash

echo ">./zformat"
./zformat
echo ">./zinspect -master"
./zinspect -master
echo ">./zfilez"
./zfilez
echo ">./zmkdir foo"
./zmkdir foo
echo ">./zfilez"
./zfilez
echo ">./zinspect -master"
./zinspect -master
echo ">./zmkdir foo"
./zmkdir foo
echo ">./zmkdir"
./zmkdir
echo ">./zmkdir foo bar"
./zmkdir foo bar
echo ">./zmkdir foo/bar"
./zmkdir foo/bar
echo ">./zfilez"
./zfilez
echo ">./zfilez foo"
./zfilez foo
echo ">./zrmdir foo"
./zrmdir foo
echo ">./zrmdir foo/bar"
./zrmdir foo/bar
echo ">./zrmdir foo"
./zrmdir foo
echo ">./zfilez"
./zfilez
echo ">./zinspect -master"
./zinspect -master
