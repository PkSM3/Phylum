#!/bin/bash


# = = = [ EXECUTION EXAMPLE ] = = = ] #
#
# {
#    inputDB: "DATA/ACM-DL/ACM-DL_2016.sqlite"   
#    periodsize: 1  
#    from: 1970  
#    to: 1990  
#    minoccs: 2  
#    mincooccs: 1  
#    output: "experiments/test00" 
# }
#
# 	 nohup  ./50_PeriodSlicing.sh  "DATA/ACM-DL/ACM-DL_2016.sqlite"  1  1970  1990  2  1  "experiments/test00"  &
#
# = = = [ / EXECUTION EXAMPLE ] = = = ] #

INPUT="$1"
OUTPUT="$2"
JCC="$3"

if [[ -z "$INPUT" ]]; then INPUT="exps/edgesk9"; fi
if [[ -z "$OUTPUT" ]]; then OUTPUT="exps/phylo"; fi
if [[ -z "$JCC" ]]; then JCC="0.5"; fi


source envpy3/bin/activate
echo "81_output_2json.py" "$INPUT" "$OUTPUT"  "$JCC"
echo "   OK"
python3 "81_output_2json.py" "$INPUT" "$OUTPUT"  "$JCC"
source virtenv2/bin/activate
echo python2 "82_prune_phylograph.py" "$OUTPUT" "$JCC"
python2 "82_prune_phylograph.py" "$OUTPUT" "$JCC"
echo "   OK"