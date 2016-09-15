#!/usr/bin/env python
# -*- coding: utf-8 -*-


# = = = [ EXECUTION EXAMPLE ] = = = ] #
#
#	   python3.4  72_kcliques-comparison.py input:"experiments/test02" "stats.json" "cliques" outputfolder:"edges_test"  A:2010  B:2014  k:9
#	   	OR
#	   python3.4  72_kcliques-comparison.py input:"experiments/test02" "stats.json" "cliques" outputfolder:"edges_test"  A:2010  B:2014  k:9   chunk:"2010-1590-1907"
#
# = = = [ / EXECUTION EXAMPLE ] = = = ] #


import time
import json
import glob
import sys
from InterUnion import Utils
do_ = Utils()
threshold= 0.2



FOLDER = "dossier"
STATS = "stats.json"
CLIQUES = "cliques_folder"
OUTFOLDER = "edges_folder"
PERIOD_x = "1970"
PERIOD_y = "1990"
K = "12"
DIV = False
if len(sys.argv)>1 and sys.argv[1]!=None: FOLDER = sys.argv[1]
if len(sys.argv)>2 and sys.argv[2]!=None: STATS = sys.argv[2]
if len(sys.argv)>3 and sys.argv[3]!=None: CLIQUES = sys.argv[3]
if len(sys.argv)>4 and sys.argv[4]!=None: OUTFOLDER = sys.argv[4]
if len(sys.argv)>5 and sys.argv[5]!=None: PERIOD_x = sys.argv[5]
if len(sys.argv)>6 and sys.argv[6]!=None: PERIOD_y = sys.argv[6]
if len(sys.argv)>7 and sys.argv[7]!=None: K = sys.argv[7]
if len(sys.argv)>8 and sys.argv[8]!=None: DIV = sys.argv[8]


def compare_periods( cliques_A , cliques_B ):
	start = time.time()
	results = [] 
	c_a = 0 
	for a in cliques_A:
		c_a += 1
		c_b = 0
		a_id = a[0]
		a = sorted( list(map(int,a[1:])) )
		for b in cliques_B:
			c_b += 1
			# a = sorted(a)
			# b = sorted(b)
			b_id = b[0]
			b = sorted( list(map(int,b[1:])) )
			inter = len( do_.intersect( a , b ) )
			if inter > 0:
				union = len( do_.union( a , b ) )
				jaccard = inter/union
				if jaccard >= threshold:
					# print("\t", inter/union )
					results.append(  [ str(round(jaccard,2)) , a_id , b_id , ",".join(map(str,a)) , ",".join(map(str,b)) ]  )
	return results , "{0:.2f}".format((time.time() -  start))

# print("\t",FOLDER+"/"+CLIQUES+"/"+PERIOD_x+"_k"+K+".json")

file_period = open( FOLDER+"/"+CLIQUES+"/"+PERIOD_x+"_k"+K+".txt" , "r")
Ca = []
for line in file_period:
	clqu = list(line.replace("\n","").split(" ")[:-1])
	# print( "A:",line.replace("\n","") )
	# print( "A':",clqu )
	Ca.append( clqu )
Pa = {  "id": PERIOD_x , "cliques": Ca  }
file_period.close()
N = len( Pa["cliques"] )


file_period = open( FOLDER+"/"+CLIQUES+"/"+str(PERIOD_y)+"_k"+K+".txt" , "r")
Cb = []
for line in file_period:
	clqu = list(line.replace("\n","").split(" ")[:-1])
	# print( "B:",line.replace("\n","") )
	# print( "B':",clqu )
	Cb.append( clqu )
Pb = { "id": PERIOD_y , "cliques": Cb  }
file_period.close()
M = len(Pb["cliques"])

# print("\t", "N:",N," x ","M:",M , "\t",Pa["id"],"x",Pb["id"])
OUTFILE = FOLDER+"/"+OUTFOLDER+"/"+PERIOD_x+"-"+PERIOD_y
if DIV!=False:
	param = DIV.split("-")
	period2split = param[0]
	from_ = int(param[1])
	to_ = int(param[2])
	if Pa["id"]==period2split:
		Pa["cliques"] = Pa["cliques"][ from_ : to_ ]
	if Pb["id"]==period2split:
		Pb["cliques"] = Pb["cliques"][ from_ : to_ ]
	# print( "Pa:",len(Pa["cliques"]) ,"vs", "Pb:",len(Pb["cliques"])  )
	OUTFILE = FOLDER+"/"+OUTFOLDER+"/"+PERIOD_x+"-"+PERIOD_y+"__"+DIV


matches, t_s = compare_periods( Pa["cliques"] , Pb["cliques"] )

if len(matches)>0:
	f = open( OUTFILE , "w")
	for jacc in matches:
		tp = [ jacc[1], jacc[2] ,jacc[0] , jacc[3] , jacc[4] ]
		f.write( " ".join(tp) + "\n" )
	f.close()
