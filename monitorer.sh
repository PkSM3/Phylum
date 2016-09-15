#!/bin/bash


# echo "   Inspecting ur shitty $1 $2..."
while true; do
	monitorer=`ps aux | grep $1 | grep $2 | grep -v $0 | grep -v color`
	if [[  -z "$monitorer" ]]; then
		break
	fi
	sleep 1
done
