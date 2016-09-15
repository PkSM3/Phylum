#!/usr/bin/env python
# -*- coding: utf-8 -*-


# = = = [ EXECUTION EXAMPLE ] = = = ] #
#
#      python3 71_available_kcliques.py  input:"test03/stats.json"  k:11
#
# = = = [ / EXECUTION EXAMPLE ] = = = ] #


# import time
import pprint
from itertools import combinations
import json
import sys
import os
LIMIT = 800000

K = 11
FOLDER = "experiments/test20160606"
if len(sys.argv)>1 and sys.argv[1]!=None: K = int(sys.argv[1])
if len(sys.argv)>2 and sys.argv[2]!=None: FOLDER = sys.argv[2]
STATS = FOLDER+"/stats"
CLIQUES = FOLDER+"/cliques"


def openJSON( filename ):
        # print("reading...")
        f = open( filename , "r" )
        data = json.load( f )
        f.close()
        return data

def iter_folder( FOLDER ):

        V = {}
        suma = 0

        query = 'ls '+FOLDER+"/*"
        pubs_folder = os.popen(  query  )

        elems = []
        for i in pubs_folder:
                filepath = i.replace("\n","")
                stats = openJSON(filepath)
                V[stats["T"]] = stats
        return V



D = iter_folder( STATS )
periods = []
last_p = -1
P_C = {}
for p in sorted(D.keys() , reverse=True):
	if "|k|" in D[p]:
		if len(D[p]["|k|"].keys())>0:
			if str(K) in D[p]["|k|"]:
				#print(p)
				period = int(p)
				if last_p==-1  or (period+1)==last_p:
					periods.append( period )
					last_p = period
					P_C[ period ] = int(D[p]["|k|"][str(K)][0])

all_comparisons = combinations( P_C.keys() , 2)

list_chunks = {}
for p in all_comparisons:
	mult = P_C[ p[0] ]*P_C[ p[1] ]
	compID = str(p[0])+"_"+str(p[1])
	if compID not in list_chunks:
		list_chunks[compID] = []

	if mult>LIMIT:
		chunker = round( LIMIT/2 )
		divisor = round(mult/chunker)
		N = -1
		if P_C[ p[0] ] > P_C[ p[1] ] : N = [ p[0] , P_C[p[0]] ]
		else: N = [ p[1] , P_C[p[1]] ]

		# print(  p[0] ,"vs", p[1],":\t",mult )
		# print( "\t","divide " , N, "  in ",divisor)

		k_size = round( N[1] / divisor )
		for i in range(divisor):
			from_ = k_size * i 
			to_   = (k_size * (i+1)) -1 
			# print( "\t  ", str(N[0])+"-"+str(from_)+"-"+str(to_) )
			list_chunks[compID].append( [ N[0] , from_ , to_ ] )

		list_chunks[compID][-1][-1] = (N[1]-1)
		# print( "\t  ", list_chunks[compID][-1] )
		# print("")
	else:
		list_chunks[compID] = False


to_execute = []
for c in sorted(list_chunks.keys()):
	command="Null"
	if list_chunks[c] == False:
		Y = c.split( "_" )
		command = Y[0]+ " " +Y[1]+" "+ str(K)
		to_execute.append( command )
	else:
		Y = c.split( "_" )
		for instance in list_chunks[c]:
			divparams = "-".join( map( str, instance ) )
			command = Y[0]+ " " +Y[1]+" "+ str(K) +" "+divparams
			to_execute.append( command )

for i in to_execute:
	print(i)
