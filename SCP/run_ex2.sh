#!/bin/sh

# $1 is path of lsscp executable
# $2 is path of maxtimes file
# $3 is path of folder with instances
# $4 is file with best costs
# $5 is path to file to save results

seed=20

algos=(ils aco)

#Part 1
echo "Part 1"
#Clear files of algorithm results
for algo in "${algos[@]}"
do
    printf "" > "$5/$algo.txt"
done

for instance in "$3"/* # Loop over every instance file in the instances folder
do
    read info
    splitted=($info)
    config=${splitted[0]}
    maxtime=${splitted[1]}
    for algo in "${algos[@]}"
    do
        cost=$(eval "$1 --seed $seed --instance $instance --$algo --mt $maxtime")
        echo "$algo on $config: $cost"
        echo "$config $cost" >> "$5/$algo.txt"
    done
done < "$2"

#Part 2
echo "Part 2"
instances=(A.1 B.1 C.1 D.1)
nrofinstances=${#instances[@]}

for algo in "${algos[@]}"; do
    rm -rf "$5/$algo"
    i=0
    for instance in "$3"/*
    do
        if [[ $i == $nrofinstances ]]; then
            break
        fi
        read info
        splitted=($info)
        config=${splitted[0]}
        if [[ $config != ${instances[i]} ]]; then
            continue
        fi
        i=$((i+1))
        while read -u 3 best
        do
            splittedbest=($best)
            bestconfig=${splittedbest[0]}
            if [[ $bestconfig == $config ]]; then
                break
            fi
        done
        destinationfolder="$5/$algo/$config"
        mkdir -p $destinationfolder
        bestcost=${splittedbest[1]}
        maxtime=${splitted[1]}
        maxcost=$(echo "scale=4;$bestcost * 1.02" | bc)
        cutoff=$(echo "scale=4;$maxtime * 10" | bc)
        echo "Running $algo on $config 25 times with max cost $maxcost and cut-off time $cutoff"
        for run in {1..25}
        do
            destinationfile="$destinationfolder/$run.csv"
            cost=$(eval "$1 --seed $seed --instance $instance --$algo --mc $maxcost --co $cutoff --trace $destinationfile")
        done
    done < $2 3<"$4"
done