#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import json as jsonold
import time
from InterUnion import Utils
import pprint
import networkx as nx
import math
from collections import defaultdict
import datetime
d = datetime.datetime.now().isoformat().split(".")[0].replace(":",".")

import sqlite3

import multiprocessing
from multiprocessing import Process, Manager, Queue
from os import getpid

class Metrics:

	def __init__( self , DEBUG , pubs_list , occurrences ):
		self.cooccurrencesG = []
		self.occurrences = occurrences
		self.rawdata = pubs_list
		self.stats = {
			"Pubs" : 0,
			"Pubs+Aid" : 0,
			"nodes" : 0,
			"edges" : 0,
			"times" : []
			# "cliques" : len(cliques),
		}
		self.stats_ordered = [
			"Pubs", "Pubs+Aid" , 
			"nodes" , "edges" , 
			"cliques" , "times"
		]
		self.Parallelism = {
			"TTtf": False,
			"R": False
		}
		self.DEBUG = DEBUG


	def calc_COOCs ( self , occs_min = 1 , coocc_min = 1 ):
		# # [ = = = = = = = [ COOCs CALCULATION ] = = = = = = = ] # # 

		if self.DEBUG: print( time.strftime("%H:%M:%S") , "calc_COOCs" )

		self.stats["times"].append( [ time.strftime("%H:%M:%S") , "calc_COOCs" ] )

		start = time.time()


		nwkx = Utils()
		for elems in self.rawdata:
			filtered_elems = []
			for e in elems:
				if self.occurrences[e]>=occs_min:
					filtered_elems.append(e)
			if len(filtered_elems)>1:
				nwkx.addCompleteSubGraph( filtered_elems )

		if coocc_min > 1:
			to_del = []
			for n in nwkx.G.nodes_iter():
				if nwkx.G.degree( n ) <= coocc_min:
					to_del.append( n )
			nwkx.G.remove_nodes_from( to_del )
			nwkx.G.remove_nodes_from( nx.isolates(nwkx.G) )


		self.stats["times"].append( [ self.get_time_s( start ) , "traversing pubs for COOCs" ] )
		self.stats["nodes"] = len( nwkx.G )
		self.stats["edges"] = nwkx.G.number_of_edges()
		self.cooccurrencesG = nwkx.G
		# # [ = = = = = = = [ / COOCs CALCULATION ] = = = = = = = ] # # 


	def normalize_edges( self , G ):

		if self.DEBUG: print( time.strftime("%H:%M:%S") , "normalize_edges" )

		self.stats["times"].append( [ time.strftime("%H:%M:%S") , "normalize_edges" ] )

		start = time.time()

		links = []
		max_w = -1
		for e in G.edges_iter():
			weight = G[e[0]][e[1]]["weight"]
			if weight>max_w: 
				max_w=weight

		for e in G.edges_iter():
			s = e[0]
			t = e[1]
			G[s][t]["weight"] = (G[s][t]["weight"]/max_w)

		self.stats["times"].append( [ self.get_time_s( start ) , "normalizing edges-weights" ] )
		return G

	def automatic_threshold( self , G ):

		if self.DEBUG: print( time.strftime("%H:%M:%S") , "automatic_threshold" )

		self.stats["times"].append( [ time.strftime("%H:%M:%S") , "automatic_threshold" ] )

		start = time.time()

		min_array = []
		for i in G.nodes_iter():
			Vi = G.neighbors( i )
			min_i = 99999
			for j in Vi:
				if G[i][j]["weight"] > 0:
					if G[i][j]["weight"] < min_i:
						min_i = G[i][j]["weight"]
			min_array.append( min_i )

		threshold = max( min_array )
		# print("\t\t","|E|:",G.number_of_edges(), " and   ALL >=",threshold,"  |   will be considered")

		G_n = nx.Graph()
		for e in G.edges_iter():
			if G[e[0]][e[1]]["weight"] >= threshold:
				G_n.add_edge( e[0] , e[1] , weight=G[e[0]][e[1]]["weight"] )

		self.stats["times"].append( [ self.get_time_s( start ) , "applying a automatic threshold" ] )
		# self.stats["nodes"] = len( G_n )
		# self.stats["edges"] = G_n.number_of_edges()
		return G_n

	def get_time_s( self , start ):
		return "{0:.2f}".format((time.time() -  start))


	def calc_distance_PseudoInclusion( self , s_min = 0.00001 , alpha = 1.0 ):

		if self.DEBUG: print( time.strftime("%H:%M:%S") , "calc_distance_PseudoInclusion" )

		self.stats["times"].append( [ time.strftime("%H:%M:%S") , "calc_distance_PseudoInclusion" ] )

		start = time.time()

		alpha = float(alpha)
		if alpha<=0: alpha = 1.0
		exp_i = alpha
		exp_j = (1/alpha)
		exp_final = min( exp_i , exp_j )

		exp_i = exp_i*exp_final
		exp_j = exp_j*exp_final

		G = nx.Graph()
		for e in self.cooccurrencesG.edges_iter():
			nA = e[0]
			nB = e[1]
			cooc = self.cooccurrencesG[ nA ][ nB ]["weight"]
			if exp_i == exp_j:
				score = (cooc*cooc)/( self.occurrences[nA] * self.occurrences[nB] )
			else:
				Ci = math.pow( (cooc/self.occurrences[nA]) , exp_i )
				Cj = math.pow( (cooc/self.occurrences[nB]) , exp_j )
				score = Ci*Cj
			if score >= s_min:
				G.add_edge(nA,nB,weight=score)

		self.stats["times"].append( [ self.get_time_s( start ) , "pseudoinclusion" ] )
		# self.stats["nodes"] = len( G )
		# self.stats["edges"] = G.number_of_edges()
		return G

	# # In construction!
	# def calc_distance_Distributional ( self ):
	# 	return False


	# Could be optimized with CPython
	#  Used by Cosine Similarity 
	#  1st element is the smallest|equal 
	def dot_product( self , A , B ):
		sum_prods = 0
		for v in A:
			if v in B:
				sum_prods += (A[v]*B[v])
				# print("\t[", v, "] , ",A[v],"*",B[v]," =", (A[v]*B[v]) )
		return sum_prods

	# complexity:  O( n )
	def calc_distance_CosineSimilarity_limitbycoocs( self , cooccurrencesG):

		if self.DEBUG: print( time.strftime("%H:%M:%S") , "calc_distance_CosineSimilarity_limitbycoocs" )

		self.stats["times"].append( [ time.strftime("%H:%M:%S") , "calc_distance_CosineSimilarity_limitbycoocs" ] )

		start = time.time()
		
		max_edgeweight = -1

		AdjDict = {}
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
			AdjDict[i] = IDX
			cooccurrencesG.node[i]["sq"] = math.sqrt( sum_powers_i )
			# = = = = = = = = = = = = = = = = #
			# = = = = = = = = = = = = = = = = #
		self.stats["times"].append( [ self.get_time_s( start ) , "    cosinesim reduced, sqrt OK" ] )

		sub_start = time.time()
		for e in cooccurrencesG.edges_iter():
			dot_prod = False
			if len( AdjDict[e[0]].keys() ) < len( AdjDict[e[1]].keys() ):
				dot_prod = self.dot_product ( AdjDict[e[0]] , AdjDict[e[1]] )
			else:
				dot_prod = self.dot_product ( AdjDict[e[1]] , AdjDict[e[0]] )
			cooccurrencesG[e[0]][e[1]]["prod"] = dot_prod
		self.stats["times"].append( [ self.get_time_s( sub_start ) , "    cosinesim reduced, num+denom OK" ] )

		sub_start = time.time()
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
		self.stats["times"].append( [ self.get_time_s( sub_start ) , "    cosinesim reduced, savingweights OK" ] )

		self.stats["times"].append( [ self.get_time_s( start ) , "TOTAL cosinesim reduced" ] )
		# self.stats["nodes"] = len( G )
		# self.stats["edges"] = G.number_of_edges()
		return G


	# complexity:  O( n**2 )
	def calc_distance_CosineSimilarity ( self , cooccurrencesG ):

		if self.DEBUG: print( time.strftime("%H:%M:%S") , "calc_distance_CosineSimilarity" )

		self.stats["times"].append( [ time.strftime("%H:%M:%S") , "calc_distance_CosineSimilarity" ] )

		start = time.time()

		max_edgeweight = -1

		AdjDict = {}
		for i in cooccurrencesG.nodes_iter():
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
			AdjDict[i] = IDX
			cooccurrencesG.node[i]["sq"] = math.sqrt( sum_powers_i )
			# = = = = = = = = = = = = = = = = #
			# = = = = = = = = = = = = = = = = #		start = time.time()

		self.stats["times"].append( [ self.get_time_s( start ) , "    cosinesim serial, sqrt OK" ] )


		T = {}
		max_edgeweight = -1
		nodes = sorted(cooccurrencesG.nodes())


		sub_start = time.time()
		# = = = [ Cosine Similarity: using just adjacency dict ] = = = # 
		for a in range(len(nodes)):

			for b in range( a+1 , len(nodes) ):
				i = nodes[a]
				j = nodes[b]

				A = len( AdjDict[i].keys() )
				B = len( AdjDict[j].keys() )

				if A < B :
					A , B = AdjDict[i] , AdjDict[j]
				else:
					A , B = AdjDict[j] , AdjDict[i]

				dot_prod = 0
				for v in A:
					if v in B:
						dot_prod += ( A[v]*B[v] )


				if dot_prod > 0:
					if i not in T:
						T[i] = {}
					cosine_sim = dot_prod/(cooccurrencesG.node[i]["sq"]*cooccurrencesG.node[j]["sq"])
					T[i][j] = cosine_sim 
					if cosine_sim > max_edgeweight:
						max_edgeweight = cosine_sim

		# = = = [ / Cosine Similarity: using just adjacency dict ] = = = # 
		self.stats["times"].append( [ self.get_time_s( sub_start ) , "    cosinesim serial ALL OK" ] )

		self.stats["times"].append( [ self.get_time_s( start ) , "TOTAL cosinesim serial" ] )
		# self.stats["nodes"] = len( G )
		# self.stats["edges"] = G.number_of_edges()
		return { "edges" : T , "max_edgeweight" : max_edgeweight }


	def calc_T_edgesfilter ( self, edges , max_edgeweight , edge_threshold = 0 ):

		G = nx.Graph()
		# PercentageDist = {}
		for i in edges:
			for j in edges[i]:
				p = round((edges[i][j] / max_edgeweight)*100)
				if p >= edge_threshold:
					G.add_edge( i , j , weight = p) 
				# if p not in PercentageDist:
				# 	PercentageDist[p] = 0
				# PercentageDist[p] += 1

		# for p in sorted( PercentageDist.keys() ):
		# 	print( p , "\t", PercentageDist[p] )

		print ( edge_threshold ,"\t", len(G) ,"\t", G.number_of_edges()  )
		return True



	def dot_product__MultiCore( self, TTtf , subgraph , q):
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


	# complexity:  O( n**2(1/p)  )
	def calc_distance_CosineSimilarity__MultiCore( self ):

		if self.DEBUG: print( time.strftime("%H:%M:%S") , "calc_distance_CosineSimilarity__MultiCore" )
		
		self.stats["times"].append( [ time.strftime("%H:%M:%S") , "calc_distance_CosineSimilarity__MultiCore" ] )

		start = time.time()

		COOCs = self.cooccurrencesG

		T_Txtf = {}
		for i in COOCs:
			Vi = COOCs.neighbors( i )
			sum_powers_i = 0
			IDX = {}
			for j in Vi:
				IDX[ j ] = COOCs[i][j]["weight"]
				sum_powers_i += (IDX[j]*IDX[j])
			T_Txtf[i] = IDX
			COOCs.node[i]["sq"] = math.sqrt( sum_powers_i )
		self.stats["times"].append( [ self.get_time_s( start ) , "    cosinesim parallel, sqrt OK" ] )

		#  Setting domains: Processors and Chunks #
		chunksIDX = []
		N_ = COOCs.number_of_edges()
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
		#  / Setting domains: Processors and Chunks #

		q = Queue()
		jobs = []

		sub_start = time.time()
		Edges = []
		for e in COOCs.edges_iter():
			Edges.append( [e[0], e[1] ] )
		self.stats["times"].append( [ self.get_time_s( sub_start ) , "    cosinesim parallel, list( G.edges() )" ] )

		sub_start = time.time()
		for c in chunksIDX:
			# print(c)
			# print("\t" ,len(Edges[c[0]:c[1]]) )
			j = Process(  target=self.dot_product__MultiCore , args=( T_Txtf , Edges[c[0]:c[1]] , q ) )
			j.start()

		results = []
		for c in chunksIDX:
			results.append(q.get(True))
		 
		for j in jobs:
			j.join()

		self.stats["times"].append( [ self.get_time_s( sub_start ) , "    cosinesim parallel, dot_prod OK" ] )

		sub_start = time.time()
		G = nx.Graph()
		for chunk in results:
			for r in chunk:
				nA = r[0]
				nB = r[1]
				prod = r[2]
				cos_sim_score = (prod/(COOCs.node[nA]["sq"]*COOCs.node[nB]["sq"]))
				if cos_sim_score>0:
					G.add_edge( nA , nB , weight=cos_sim_score  )
		self.stats["times"].append( [ self.get_time_s( sub_start ) , "    cosinesim parallel, savingweights OK" ] )

		self.stats["times"].append( [ self.get_time_s( start ) , "TOTAL cosinesim parallel" ] )
		# self.stats["nodes"] = len( G )
		# self.stats["edges"] = G.number_of_edges()
		return G




