#!/bin/bash

declare -i field_size=100
declare -i n_threads=2

while [ $field_size -le 1000 ]
do
    ./life $field_size $field_size 100 5 >> "test_by_field_size.csv"
    echo "$field_size done"
    let "field_size = field_size + 100"
done

while [ $n_threads -le 15 ]
do 
    ./life 500 500 100 $n_threads >> "test_by_n_threads.csv"
    echo "$n_threads done"
    let "n_threads = n_threads + 1"
done
echo 'OK'
exit 0
