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

input="$1"
periodsize="$2"
FROM="$3"
TO="$4"
occmin="$5"
cooccmin="$6"
instance="$7"
pythonversion="$8"
K=9
C=40


if [[ -z "$input" ]]; then input="DATA/ACM-DL/ACM-DL_2016.sqlite"; fi
if [[ -z "$FROM" ]]; then FROM="1950"; fi
if [[ -z "$TO" ]]; then TO="2015"; fi
if [[ -z "$periodsize" ]]; then periodsize="1"; fi
if [[ -z "$occmin" ]]; then occmin="1"; fi
if [[ -z "$cooccmin" ]]; then cooccmin="1"; fi
if [[ -z "$instance" ]]; then instance="experiments/test00"; fi
if [[ -z "$pythonversion" ]]; then pythonversion="python3"; fi
rm -fR nohup.out "$instance"*
mkdir "$instance"
mkdir "$instance/cliques"
mkdir "$instance/stats"
mkdir "$instance/trans"
mkdir "$instance/phylo"


source envpy3/bin/activate
for ((y=FROM; y<=TO; y++)); do
	# echo $pythonversion 51_TemporalNetwork.py $input $y $periodsize $occmin $cooccmin $instance
	nohup   $pythonversion 51_TemporalNetwork.py $input $y $periodsize $occmin $cooccmin $instance >> "$instance/log" 2>&1 &
done
# exit

echo "parallelism started!"
INIT=`date +"%y-%m-%d %H:%M:%S"`
./monitorer.sh  $pythonversion  51_TemporalNetwork.py
echo "  $INIT -- `date +"%y-%m-%d %H:%M:%S"`cliques per period OK"
echo -e "\n"
echo "temporal alignment started!"
echo "./70_TemporalComparison.sh"  "$instance"  "$K"  "$C"
./70_TemporalComparison.sh  "$instance"  "$K"  "$C"

./80_phyloviz.sh  "$instance/edgesk$K"  "$instance" "0.6"