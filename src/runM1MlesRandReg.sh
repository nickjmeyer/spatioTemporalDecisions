#!/bin/bash

cd ../bin

NETS=("rand")
SIZE=("100" "500" "1000")

for i in ${NETS[@]}
do
    for j in ${SIZE[@]}
    do
	submit ./runM1Mles "../data/toy/${i}${j}" y;
    done
done
