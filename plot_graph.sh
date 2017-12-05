#!/usr/bin/env bash
#Plots graph using gnuplot + convert

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 PlotFileName" >&2
    exit 1
fi

echo "Start"

gnuplot $1

now=$(date -u +'%Y%m%dT%H%M%S')

for i in *.eps; 
    do 
        timestamped_file=$i"_"$now.png;
        new_file=$i.png;

        echo "Generate: " $timestamped_file
        convert $i -resize "1428x1000" -quality 100 $timestamped_file;
        echo "Update: " $new_file
        /bin/cp $timestamped_file $new_file;
    done

rm -f *.eps

echo "Done!"
