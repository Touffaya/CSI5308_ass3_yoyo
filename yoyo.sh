#!/bin/bash

CPR="Yoyo-Algorithm  Copyright (C) 2014  Arthur Bourjac\n
This program comes with ABSOLUTELY NO WARRANTY;\n
This is free software, and you are welcome to redistribute it\n
under certain conditions;\n"

USG="Usage : yoyo.sh -f file | -d dir report | -r number report\n
\t-f : process one graph (console display)\n
\t\t\"file\" specifies the file for the graph\n
\t-d : process multiple graphs (WARNING : appends)\n
\t\t\"dir\" specifies the directory for the graphs\n
\t\t\"report\" specifies the file in which to save\n
\t-r : process random graphs (WARNING : appends)\n
\t\t\"number\" specifies the number of graphs to process\n
\t\t\"report\" specifies the file in which to save\n\n"

echo -e $CPR

if [ $# = 2 ] && [ $1 = "-f" ]; then	
	./bin/yoyo.exe -o $2
elif [ $# = 3 ] && [ $1 = "-d" ]; then
	for i in $(ls $2); do
		./bin/yoyo.exe -m $2/$i $3
	done
elif [ $# = 3 ] && [ $1 = "-r" ]; then
	CPT=0
	while [ $CPT -lt $2 ]; do
		rand=$RANDOM
		let rand%=40000
		let rand+=10000
		rand2=$RANDOM
		let rand2%=50
		let rand2+=10
		graph="randgraph"$CPT
		path="graphs/graphs/random/"$graph
		graphs/gengraph_win/distrib $rand 2.5 10 $rand2 | graphs/gengraph_win/graph -v > $path
		./bin/yoyo.exe -m $path $3
		let CPT=CPT+1
	done
	echo -e "\n--DONE--"
else
	echo -e $USG
fi
