#!/bin/sh

# $1 is path of lsscp executable
# $2 is path of folder with instances
# $3 is path to file to save results


seed=20

printf "" > "$3" # Create/overwrite results file
for instance in "$2"/* # Loop over every instance file in the instances folder
do
    times=()
    for run in {1..10} #Run the algorithm for each instance 10 times
    do
        start=$(echo "scale=4;$(gdate +%s%3N)/1000" | bc)
        cost=$(eval "$1 --seed $seed --instance $instance --ch4 --fi")
        stop=$(echo "scale=4;$(gdate +%s%3N)/1000" | bc)
        times+=($(echo "($stop-$start)" | bc))
    done
    count=0
    total=0
    for T in "${times[@]}"
    do
        total=$(echo "scale=4;$total+$T" | bc )
        ((count++))
    done
    #echo "$total $count"
    maxtime=$(echo "scale=4; $total / $count * 100" | bc) #100*average
    y=${instance%.txt} # get part before .txt
    filename=${y##*/} # Only keep filename (remove the path)
    length=${#filename}
    first=$(echo ${filename:3:$length-4}  | awk '{print toupper($0)}')
    second=${filename:$length-1:1}
    echo "$first.$second $maxtime" >> "$3"
done