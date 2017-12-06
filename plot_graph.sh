#!/usr/bin/env bash
#Plots graph using gnuplot + convert

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 PlotFileName" >&2
    exit 1
fi

echo "Start"

gnuplot $1

now=$(date -u +'%Y%m%dT%H%M%S')
read -e -p "File prefix: " prefix

for i in *.eps; 
    do 
        old_file=$i"_"$prefix"_"$now.png;

        echo "Generate: " $old_file
        convert -density 300 $i -resize "1428x1000" -quality 100 -flatten $old_file;
    done

rm -f *.eps

echo "Done!"
