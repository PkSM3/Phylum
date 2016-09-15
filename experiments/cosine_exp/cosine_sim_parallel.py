#!/usr/bin/env python
# -*- coding: utf-8 -*-

import networkx as nx
import math
import time
import json


cooccurrencesG = nx.Graph()
f_ = open( "edges_2000.json" , "r" )
json_data = json.load(f_)
for i in json_data:
	cooccurrencesG.add_edge( i[0] , i[1] , weight=i[2] )
f_.close()




start = time.time()




T_Txtf = {}
for i in cooccurrencesG:
	Vi = cooccurrencesG.neighbors( i )
	sum_powers_i = 0
	IDX = {}
	for j in Vi:
		IDX[ j ] = cooccurrencesG[i][j]["weight"]
		sum_powers_i += (IDX[j]*IDX[j])
	T_Txtf[i] = IDX
	cooccurrencesG.node[i]["sq"] = math.sqrt( sum_powers_i )


import multiprocessing
from multiprocessing import Process, Manager, Queue
from os import getpid

def f( TTtf , subgraph , q):
	# print( "\t\t\tSTART","thread:",getpid() )
	# print( "thread:",getpid(),"  #subgraph",  len(subgraph) )
	R = []
	for n in subgraph:
		# print("\t",n)
		dot_prod = False
		# 1st element is the smallest|equal 
		if len( TTtf[n[0]].keys() ) < len( TTtf[n[1]].keys() ):
			dot_prod = 0
			A = TTtf[n[0]]
			B = TTtf[n[1]]
			for v in A:
				if v in B:
					dot_prod += (A[v]*B[v])
		else:
			dot_prod = 0
			A = TTtf[n[1]]
			B = TTtf[n[0]]
			for v in A:
				if v in B:
					dot_prod += (A[v]*B[v])

		R.append( [ n[0] , n[1] , dot_prod ] )
	# print( "\t\t\t\tFINISH","thread:",getpid() )
	q.put(R)


chunksIDX = []
N_ = cooccurrencesG.number_of_edges()
nb_threads = multiprocessing.cpu_count()
nb_threads = int(nb_threads)
k_size = int( N_ / nb_threads )
for i in range(nb_threads):
	from_ = ( k_size * i )
	to_   = (k_size * (i+1)) -1
	if i == (nb_threads-1):
		if to_ > N_: to_ = to_-1
		else: to_ = N_-1
		i+=1
	if from_ < (N_-1):
		chunksIDX.append( [from_ , to_ ] )


Array = []
for e in cooccurrencesG.edges_iter():
	Array.append( [e[0], e[1] , cooccurrencesG[e[0]][e[1]]["weight"]] )
for c in chunksIDX:
	print(c)
	print("\t" ,len(Array[c[0]:c[1]]) )
# L = nx.generate_edgelist(cooccurrencesG,data=['weight'] )
print( len(Array) )

import sys
sys.exit(1)
ChunksData = {}
c_i = 0
e_i = 0
for e in cooccurrencesG.edges_iter():
	if c_i == len(chunksIDX):
		break

	if  chunksIDX[c_i][0] <= e_i <= chunksIDX[c_i][1]:
		pass
	else:
		c_i += 1

	if c_i not in ChunksData:
		ChunksData[c_i] = []
	ChunksData[c_i].append( [ e[0] , e[1] ] )

	e_i+=1




q = Queue()

jobs = []
for c in ChunksData:
	j = Process(  target=f , args=( T_Txtf , ChunksData[c] , q ) )
	jobs.append( j )
	j.start()

results = []
for c in ChunksData:
	results.append(q.get(True))
 
for j in jobs:
	j.join()

print ("results:",len(results))

G = nx.Graph()
for chunk in results:
	for r in chunk:
		nA = r[0]
		nB = r[1]
		prod = r[2]
		cos_sim_score = (prod/(cooccurrencesG.node[nA]["sq"]*cooccurrencesG.node[nB]["sq"]))
		if cos_sim_score>0:
			G.add_edge( nA , nB , weight=cos_sim_score  )


 
print ("CHA CHAAAN" , len(G) , G.number_of_edges())
# print( results )
# print("")

end = time.time()
_time =float("{0:.7f}".format((end - start)))
print( _time )

# for e in G.edges_iter():
# 	# print(e)
# 	print(e[0],"->",e[1],":",G[e[0]][e[1]]["weight"])

