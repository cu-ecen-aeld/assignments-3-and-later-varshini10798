#! /bin/bash

if [[ $# -ne 2 ]] 
then
	echo "All the parameters have not been specified"
	exit 1
fi

if  [ ! -d "$1" ]
then 
	echo "File directory does not exist"
	exit 1
else
	#echo "$(find $1 -type f | wc -l)"
	filesCount=$(find $1 -type f| wc -l)
	linesMatch=$(grep -r "$2" "$1" | wc -l)
	echo "The number of files are $filesCount and the number of matching lines are $linesMatch"
	exit 0
fi

