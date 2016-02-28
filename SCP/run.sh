#!/bin/sh

seed=20
output="out.txt"

# $1 is path of scp executable
# $2 is path of folder with instances

algorithms=("ch1" "ch2" "ch3" "ch4")

for a in "${algorithms[@]}"; do
    for re in "" "--re"; do
        for instance in "$2"/*
        do
            y=${instance%.txt}
            cost=$(eval "$1 --seed $seed --instance $instance --output $output --$a $re")
            filename=${y##*/}
            length=${#filename}
            first=$(echo ${filename:3:$length-4}  | awk '{print toupper($0)}')
            second=${filename:$length-1:1}
            if [ -z $re ]
            then
                echo "$first.$second $cost" >> "$a.txt"
            else
                echo "$first.$second $cost" >> "$a+re.txt"
            fi
        done
    done
done