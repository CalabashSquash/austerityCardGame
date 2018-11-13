#!/bin/bash

#check for //todo in all .c and .h files

grep -nH -in --colour=always todo *.c *.h
