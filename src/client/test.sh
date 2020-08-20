#!/bin/bash
clear
sleep 2
x=0
while [ $x -le 1000 ]
do
	./client "hello-world-$x"
	x=$(( $x + 1 ))
	sleep 0.3
done

