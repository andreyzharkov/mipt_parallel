#! /bin/bash
np=4

./game_sock -m $1 $2 -g $3 $3 -np $np &
sleep 5
for i in $(seq 1 $np); do
  echo $i;
  ./game_sock $1 93.175.2.82 &
done
