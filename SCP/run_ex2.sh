#!/bin/sh

# $1 is path of lsscp executable
# $2 is path of maxtimes file
# $3 is path of folder with instances
# $4 is path to file to save results

seed=20

printf "" > "$4/ils.txt"
printf "" > "$4/aco.txt"

for instance in "$3"/* # Loop over every instance file in the instances folder
do
    read info
    splitted=($info)
    config=${splitted[0]}
    maxtime=${splitted[1]}
    for algo in "ils" "aco";
    do
        cost=$(eval "$1 --seed $seed --instance $instance --$algo --mt $maxtime")
        echo $cost
        echo "$config $cost" >> "$4/$algo.txt"
    done
done < "$2"
