#!/usr/bin/env bash
#Plots graph using gnuplot + convert

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 PlotFileName" >&2
    exit 1
fi

echo "Start"

gnuplot $1

for i in *.eps; do convert $i $i.png; done

rm -f *.eps

echo "Done!"