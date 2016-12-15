#!/bin/bash

declare -i field_size=100
declare -i n_threads=2

rm *.csv

while [ $field_size -le 2000 ]
do
	echo -n "$field_size," >> "size_test.csv"
    ./game_life1/life $field_size $field_size 100 5 >> "size_test.csv"
    echo -n "," >> "size_test.csv"
    mpirun -np 5 ./life_mpi/mpi_life $field_size $field_size 100 >> "size_test.csv"
    echo "" >> "size_test.csv"
    echo "$field_size done"
    let "field_size = field_size + 100"
done

while [ $n_threads -le 15 ]
do 
	echo -n "$n_threads," >> "threads_test.csv"
    ./game_life1/life 500 500 100 $n_threads >> "threads_test.csv"
    echo -n "," >> "threads_test.csv"
    mpirun -np $n_threads ./life_mpi/mpi_life 500 500 100 >> "threads_test.csv"
    echo "" >> "threads_test.csv"
    echo "$n_threads done"
    let "n_threads = n_threads + 1"
done
echo 'OK'
exit 0
