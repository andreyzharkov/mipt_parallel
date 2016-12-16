#!/bin/bash

declare -i field_size=100
declare -i n_threads=2

rm *.csv

while [ $n_threads -le 50 ]
do 
    echo -n "$n_threads," >> "mpi_test.csv"
#    ./game_life1/life 500 500 100 $n_threads >> "threads_test.csv"
#    echo -n "," >> "threads_test.csv"
    mpirun -np $n_threads ./life_mpi/mpi_life 50 50 1000 >> "mpi_test.csv"
    echo "" >> "mpi_test.csv"
    echo "$n_threads done"
    let "n_threads = n_threads + 1"
done
echo 'OK'
exit 0

