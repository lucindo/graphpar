#!/bin/bash

for vertex in `cat sizes.txt`;
	do
		let weight=500*$vertex; 
		for density in 20 30 40;
		do
			for clusters in 4 8 16 32 64;
			do  
				echo "./run bin/htest -v $vertex -w $weight -d $density -c $clusters -C >> data/teste_heuristica/${vertex}_${density}";
				./run bin/htest -v $vertex -w $weight -d $density -c $clusters -C >> data/teste_heuristica/${vertex}_${density};
				echo "./run bin/htest -v $vertex -w $weight -d $density -c $clusters -R >> data/teste_heuristica/${vertex}_${density}";
				./run bin/htest -v $vertex -w $weight -d $density -c $clusters -R >> data/teste_heuristica/${vertex}_${density};
			done
		done
done
