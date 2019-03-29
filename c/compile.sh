#!/bin/bash
gcc -Wall -c mIO.c -o mIO.o
ar rcs libmIO.a mIO.o

gcc -Wall test.c -L. -lmIO -o test
