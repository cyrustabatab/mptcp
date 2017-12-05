#!/usr/bin/env bash
#Plots graph using gnuplot + convert

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 PlotFileName" >&2
    exit 1
fi

echo "Start"

gnuplot $1

now=$(date -u +'%Y%m%dT%H%M%S')
read -e -p "File prefix:" prefix

for i in *.eps; 
    do 
        old_file=$i"_"$prefix"_"$now.png;
        new_file=$i.png;

        echo "Generate: " $old_file
        convert $i -resize "1428x1000" -quality 100 $old_file;
        echo "Update: " $new_file
        /bin/cp $old_file $new_file;
    done

rm -f *.eps

echo "Done!"
