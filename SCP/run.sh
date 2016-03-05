#!/bin/sh

# $1 is path of scp executable
# $2 is path of folder with instances
# $3 is path of folder to save results

seed=20

#Exercise 1.1
echo "" > "$3/ex11_durations.txt" # Create/overwrite durations file
for a in "ch1" "ch2" "ch3" "ch4"; do
    for re in "" "--re"; do
        start=$SECONDS
        if [ -z $re ] # if empty string
        then
            configuration="$a"
        else
            configuration="$a+re"
        fi
        echo "" > "$3/$configuration.txt" # Create/overwrite results file
        for instance in "$2"/* # Loop over every instance file in the instances folder
        do
            y=${instance%.txt} # get part before .txt
            cost=$(eval "$1 --seed $seed --instance $instance --$a $re")
            filename=${y##*/} # Only keep filename (remove the path)
            length=${#filename}
            first=$(echo ${filename:3:$length-4}  | awk '{print toupper($0)}')
            second=${filename:$length-1:1}
            echo "$first.$second $cost" >> "$3/$configuration.txt"
        done
        duration=$(( SECONDS - start ))
        echo "$configuration $duration" >> "$3/ex11_durations.txt"
    done
done


#Exercise 1.2
echo "" > "$3/ex12_durations.txt" # Create/overwrite durations file
for a in "ch1" "ch4"; do
    for re in "" "--re"; do
        for imp_algo in "fi" "bi"; do
            start=$SECONDS
            if [ -z $re ] # if empty string
            then
                configuration="$a+$imp_algo"
            else
                configuration="$a+re+$imp_algo"
            fi
            echo "" > "$3/$configuration.txt" # Create/overwrite results file
            for instance in "$2"/*
            do
                y=${instance%.txt} # get part before .txt
                cost=$(eval "$1 --seed $seed --instance $instance --$a $re --$imp_algo")
                filename=${y##*/} # Only keep filename (remove the path)
                length=${#filename}
                first=$(echo ${filename:3:$length-4}  | awk '{print toupper($0)}')
                second=${filename:$length-1:1}
                echo "$first.$second $cost" >> "$3/$configuration.txt"
            done
            duration=$(( SECONDS - start ))
            echo "$configuration $duration" >> "$3/ex12_durations.txt"
        done
    done
done