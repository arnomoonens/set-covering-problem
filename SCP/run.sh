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
            # instance_name=$("$instance" | xargs -n 1 basename)
            y=${instance%.txt}
            cost=$(eval "$1 --seed $seed --instance $instance --output $output --$a $re")
            if [ -z $re ]
            then
                echo "${y##*/} $cost" >> "$a.txt"
            else
                echo "${y##*/} $cost" >> "$a+re.txt"
            fi
        done
    done
done