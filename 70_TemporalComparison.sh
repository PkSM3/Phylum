#!/bin/bash


# = = = [ EXECUTION EXAMPLE ] = = = ] #
#
# {
#    inputfolder: "experiments/test00"
#    k: 11  
# }
#
# 	 nohup  ./70_TemporalComparison  "experiments/test00"  11  nb_threads  &
#./70_TemporalComparison.sh  "experiments/test20160606"  7  40
# = = = [ / EXECUTION EXAMPLE ] = = = ] #


pythonversion="python3"
instance="$1" 
k="$2"
threshold=0.1
cores=$3
decalage=$(( cores * 60 / 100 )) 

INICIO=`date +"%y-%m-%d %H:%M:%S"`

#source virtenv/bin/activate
rm -fR $instance/balance__k$k  && mkdir $instance/balance__k$k
#echo "$pythonversion" 71_balancing_comparisons.py  "$k"  "$instance"

"$pythonversion" 71_balancing_comparisons.py  "$k"  "$instance"  > "$instance/balance_k$k"

split -l $cores "$instance/balance_k$k" "$instance/balance__k$k/liens_"
TOTAL=`ls $instance/balance__k$k/ | wc -l`

COUNTER=1
rm -fR $instance/edgesk$k  && mkdir $instance/edgesk$k
IFS=$'\n'

# # # = = = = [ NOHUP-STRATEGY ] = = = = # # #
# for f in "$instance/balance__k$k/liens_"*; do 
# 	# # echo "$f"; 
# 	echo "$COUNTER / $TOTAL"
# 	for params in $(cat $f)
# 	do
# 		A=`echo $params |cut -d ' ' -f1 `
# 		B=`echo $params |cut -d ' ' -f2 `
# 		C=`echo $params |cut -d ' ' -f3 `
# 		D=`echo $params |cut -d ' ' -f4 `
# 		nohup  "$pythonversion" 72_kcliques-comparison.py  "$instance"  "stats.json"  "cliques" "edgesk$k" $A $B $C $D   >> "$instance/nohup" 2>&1 &
# 	done
# 	while true; do
# 		sleep 0.2s
# 		cthreads=`ps aux | grep $pythonversion | grep 72_kcliques-comparison.py | grep -v $0 | grep -v color | wc -l`
# 		if [[  -z "$cthreads" ]]; then
# 			break
# 		else
# 			if [[ cthreads -lt decalage ]]; then
# 				break
# 			fi
# 		fi
# 	done
# 	COUNTER=$((COUNTER+1)) 
# done
# # # = = = = [ / NOHUP-STRATEGY ] = = = = # # #



# # # = = = = [ WAIT-STRATEGY ] = = = = # # #
for f in "$instance/balance__k$k/liens_"*; do 
	# # echo "$f"; 
	echo "$COUNTER / $TOTAL"
	for params in $(cat $f)
	do
		A=`echo $params |cut -d ' ' -f1 `
		B=`echo $params |cut -d ' ' -f2 `
		C=`echo $params |cut -d ' ' -f3 `
		D=`echo $params |cut -d ' ' -f4 `
		# echo "$pythonversion" 72_kcliques-comparison.py  "$instance"  "stats.json"  "cliques" "edgesk$k" $A $B $C $D
		"$pythonversion" 72_kcliques-comparison.py  "$instance"  "stats.json"  "cliques" "edgesk$k" $A $B $C $D   &
	done
	wait
	COUNTER=$((COUNTER+1)) 
done
# # # = = = = [ / WAIT-STRATEGY ] = = = = # # #

./monitorer.sh  "$pythonversion" 72_kcliques-comparison.py

FINI=`date +"%y-%m-%d %H:%M:%S"`
# echo "$cores cores  : $k : END"
echo "   $INICIO  --  $FINI  jaccard OK";

