#!/bin/bash


echo "   Inspecting ur shitty $1 $2..."
while true; do
    sleep 1
    
	monitorer=`ps aux | grep $1 | grep $2 | grep -v $0 | grep -v color`
	if [[  -z "$monitorer" ]]; then
		break
	fi

    now=$(date +"%T")
	ps -C "$1" -O rss | gawk '{ count ++; sum += $2 }; END {count --; print "", strftime("%H:%M:%S") ," , " , count, " , ", sum/1024/count, " , " , sum/1024, "" ;};'

done

 # ps -C "python3" -O rss | gawk '{ count ++; sum += $2 }; END {count --; print "[ ",count, " , ", sum/1024/count, " , " , sum/1024, " ]" ;};'
