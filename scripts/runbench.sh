#!/bin/bash

if [ -n "$2" ]
then
	./run bin/hbench -v $1 -d 30 -c $2 -r 50 >> data/benchmark/$1_$2.txt
	echo "" >> data/benchmark/$1_$2.txt
	./run bin/hbench -v $1 -d 60 -c $2 -r 50 >> data/benchmark/$1_$2.txt
	echo "" >> data/benchmark/$1_$2.txt
	./run bin/hbench -v $1 -d 90 -c $2 -r 50 >> data/benchmark/$1_$2.txt
fi

