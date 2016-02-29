#!/bin/sh

seed=20
output="out.txt"

# $1 is path of scp executable
# $2 is path of folder with instances

#Exercise 1.1
for a in "ch1" "ch2" "ch3" "ch4"; do
    for re in "" "--re"; do
        start=$S
        if [ -z $re ] # if empty string
        then
            configuration="$a"
        else
            configuration="$a+re"
        fi
        for instance in "$2"/*
        do
            y=${instance%.txt} # get part before .txt
            cost=$(eval "$1 --seed $seed --instance $instance --output $output --$a $re")
            filename=${y##*/} # Only keep filename (remove the path)
            length=${#filename}
            first=$(echo ${filename:3:$length-4}  | awk '{print toupper($0)}')
            second=${filename:$length-1:1}
            echo "$first.$second $cost" >> "$configuration.txt"
        done
        duration=$(( S - start ))
        echo "$configuration $duration" >> "ex11_durations.txt"
    done
done


#Exercise 1.2
for a in "ch1" "ch4"; do
    for re in "" "--re"; do
        for imp_algo in "fi" "bi"; do
            start=$S
            if [ -z $re ] # if empty string
            then
                configuration="$a+$imp_algo"
            else
                configuration="$a+re+$imp_algo"
            fi
            for instance in "$2"/*
            do
                y=${instance%.txt} # get part before .txt
                cost=$(eval "$1 --seed $seed --instance $instance --output $output --$a $re --$imp_algo")
                filename=${y##*/} # Only keep filename (remove the path)
                length=${#filename}
                first=$(echo ${filename:3:$length-4}  | awk '{print toupper($0)}')
                second=${filename:$length-1:1}
                echo "$first.$second $cost" >> "$configuration.txt"
            done
            duration=$(( S - start ))
            echo "$configuration $duration" >> "ex12_durations.txt"
        done
    done
done