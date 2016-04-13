#!/bin/sh

# $1 is path of lsscp executable
# $2 is path of maxtimes file
# $3 is path of folder with instances
# $4 is path to file to save results


FILE=$2


while read line
do
    splitted=($line)
    maxtime=${splitted[0]}
done < $FILE


for instance in "$3"/* # Loop over every instance file in the instances folder
do
