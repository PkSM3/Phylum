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

# Could be optimized with CPython
# 1st element is the smallest|equal 
def dot_product( A , B ):
	sum_prods = 0
	for v in A:
		if v in B:
			sum_prods += (A[v]*B[v])
			# print("\t[", v, "] , ",A[v],"*",B[v]," =", (A[v]*B[v]) )
	return sum_prods


def calc_cosinesimilarity_1thread( cooccurrencesG ):
	start = time.time()

	T_Txtf = {}
	for i in cooccurrencesG:
		Vi = cooccurrencesG.neighbors( i )
		# = = = = = = = = = = = = = = = = #
		# Set IDX[t1] = { t2 : freq(t2) , t4 : freq(t4) , ... }
		# Set  SQ[t1] = squareroot_of( sum of  each neigh_cooc **2 )
		# = = = = = = = = = = = = = = = = #
		sum_powers_i = 0
		IDX = {}
		for j in Vi:
			IDX[ j ] = cooccurrencesG[i][j]["weight"]
			sum_powers_i += (IDX[j]*IDX[j])
		T_Txtf[i] = IDX
		cooccurrencesG.node[i]["sq"] = math.sqrt( sum_powers_i )
		# = = = = = = = = = = = = = = = = #
		# = = = = = = = = = = = = = = = = #

	for e in cooccurrencesG.edges_iter():
		dot_prod = False
		if len( T_Txtf[e[0]].keys() ) < len( T_Txtf[e[1]].keys() ):
			dot_prod = dot_product ( T_Txtf[e[0]] , T_Txtf[e[1]] )
		else:
			dot_prod = dot_product ( T_Txtf[e[1]] , T_Txtf[e[0]] )
		cooccurrencesG[e[0]][e[1]]["prod"] = dot_prod


	G = nx.Graph()
	for e in cooccurrencesG.edges_iter():
		nA = e[0]
		nB = e[1]
		edge = cooccurrencesG[nA][nB]
		# print( nA ,"->", nB,":")
		# print( "\tcooc:" , edge["weight"] )
		# print( "\tprod:" , edge["prod"])
		# print( "\tsqrtA:" ,cooccurrencesG.node[ nA ]["sq"] )
		# print( "\tsqrtB:" ,cooccurrencesG.node[ nB ]["sq"] )
		# print( e[0] ,"->", e[1],":",(edge["prod"]/(cooccurrencesG.node[nA]["sq"]*cooccurrencesG.node[nB]["sq"]))
		# print("")
		cos_sim_score = (edge["prod"]/(cooccurrencesG.node[nA]["sq"]*cooccurrencesG.node[nB]["sq"]))
		if cos_sim_score>0:
			G.add_edge( nA , nB , weight=cos_sim_score  )
			# print( e[0] ,"->", e[1],":",cos_sim_score)

	end = time.time()
	_time =float("{0:.7f}".format((end - start)))
	return {
		"G": G,
		"[s]": _time
	}


results = calc_cosinesimilarity_1thread(cooccurrencesG )
print( results["[s]"] )







# T_Txtf = {}
# for i in cooccurrencesG:
# 	Vi = cooccurrencesG.neighbors( i )
# 	sum_powers_i = 0
# 	IDX = {}
# 	for j in Vi:
# 		IDX[ j ] = cooccurrencesG[i][j]["weight"]
# 		sum_powers_i += (IDX[j]*IDX[j])
# 	T_Txtf[i] = IDX
# 	cooccurrencesG.node[i]["sq"] = math.sqrt( sum_powers_i )


# import multiprocessing
# from multiprocessing import Process, Manager
# def f( subgraph ):
# 	print( "thread:", len(TTtf.keys()) , len(subgraph))
# 	for n in subgraph:
# 		# print("\t",n)
# 		dot_prod = False
# 		if len( TTtf[n[0]].keys() ) < len( TTtf[n[1]].keys() ):
# 			dot_prod = dot_product ( TTtf[n[0]] , TTtf[n[1]] )
# 		else:
# 			dot_prod = dot_product ( TTtf[n[1]] , TTtf[n[0]] )
# 		# print("\t\t",dot_prod)
# 		R.append( [ n[0] , n[1] , dot_prod ] )

# manager = Manager()
# TTtf = manager.dict()
# TTtf = T_Txtf
# R = manager.list()



# chunksIDX = []
# N_ = cooccurrencesG.number_of_edges()
# nb_threads = multiprocessing.cpu_count()
# nb_threads = int(nb_threads/2)
# k_size = int( N_ / nb_threads )
# for i in range(nb_threads):
# 	from_ = ( k_size * i )
# 	to_   = (k_size * (i+1)) -1
# 	if i == (nb_threads-1):
# 		if to_ > N_: to_ = to_-1
# 		else: to_ = N_-1
# 		i+=1
# 	if from_ < (N_-1):
# 		chunksIDX.append( [from_ , to_ ] )


# ChunksData = {}
# c_i = 0
# e_i = 0
# for e in cooccurrencesG.edges_iter():
# 	if c_i == len(chunksIDX):
# 		break

# 	if  chunksIDX[c_i][0] <= e_i <= chunksIDX[c_i][1]:
# 		pass
# 	else:
# 		c_i += 1

# 	if c_i not in ChunksData:
# 		ChunksData[c_i] = []
# 	ChunksData[c_i].append( [ e[0] , e[1] ] )

# 	e_i+=1



# for c in ChunksData:
# 	print(  c,") ",chunksIDX[c],"  :" , len(ChunksData[c] )  )
# 	t = Process(target=f, args=( ChunksData[c] ,))
# 	t.start()
# t.join()

# print("")
# print("")
# print( len(R) )
# G = nx.Graph()
# for r in R:
# 	nA = r[0]
# 	nB = r[1]
# 	prod = r[2]
# 	cos_sim_score = (prod/(cooccurrencesG.node[nA]["sq"]*cooccurrencesG.node[nB]["sq"]))
# 	if cos_sim_score>0:
# 		G.add_edge( nA , nB , weight=cos_sim_score  )
# print ("CHA CHAAAN")
# print("")

# for e in G.edges_iter():
# 	# print(e)
# 	print(e[0],"->",e[1],":",G[e[0]][e[1]]["weight"])

