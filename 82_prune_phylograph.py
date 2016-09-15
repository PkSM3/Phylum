
#!/usr/bin/env python
# -*- coding: utf-8 -*-


import json
import sys
import os
import re
import networkx as nx





import pygraphviz as pgv
import networkx as nx
import pprint
from itertools import combinations

class PhyloGen:
	def __init__(self , folder , filename , extension):
		self.hola = "mundo"
		self.AG = nx.DiGraph()
		self.PATHFILE = folder+"/phylo/"+filename
		self.EXT = "."+extension

	def combinaisons( self , L ):
		combs = combinations(L, 2)
		return list(combs)


	def verify_descendance( self , nA , nB , family ):

		result = True

		DG = nx.DiGraph()
		family_k = sorted(list(family.keys()))
		pairs = self.combinaisons( family_k )

		for year in pairs:
			for nn1 in family[year[0]]:
				for nn2 in family[year[1]]:
					if self.AG.has_edge( nn1 , nn2 ):
						DG.add_edge( nn1 , nn2 )
		DG.remove_edge( nA["id"] , nB["id"])

		try:
			result = nx.shortest_path(DG,source=nA["id"],target=nB["id"])
			result = True
		except:
			result = False
			pass

		return result


	def processJSON_test( self , G , SetsDict , T ):

		inputfile = "hola"

		Year_CL = {}
		self.AG = nx.DiGraph()
		AG = self.AG

		NodeIDs = {}
		for e in G["links"]:
			# print(e)
			s = e["s"]
			t = e["t"]
			try:
				# if e["w"] >= 0.7:
				ns = G["nodes"][s]["label"]
				nt = G["nodes"][t]["label"]
				ys = int(ns.split("c")[0])
				yt = int(nt.split("c")[0])

				if ys>=T[0] and ys<=T[1] and yt>=T[0] and yt<=T[1]:
					NodeIDs[s] = True
					NodeIDs[t] = True

					if ys not in Year_CL: 
						Year_CL[ys] = {}
					Year_CL[ys][s] = ns

					if yt not in Year_CL: 
						Year_CL[yt] = {}
					Year_CL[yt][t] = nt

			except:
				pass




		 # - - - - - [ Adding yearly-graph ] - - - - - #
		years = Year_CL.keys()
		for y in years:
			AG.add_node(str(y), label=y , fake=True ,shape="plaintext")

		for i in range(len(years)):
			try:
				AG.add_edge(str(years[i]),str(years[i+1]),fake=True)
			except:
				pass
		 # - - - - - [ / Adding yearly-graph ] - - - - - #


		for y in Year_CL:
			for ID in Year_CL[y]:
				if ID in NodeIDs:
					label = Year_CL[y][ID]
					AG.add_node(ID, shape="square" , y=y , label=label, fontsize=7)


		for e in G["links"]:
			# print(e)
			s = e["s"]
			t = e["t"]
			try:
				# if e["w"] >= 0.7:

				ns = G["nodes"][s]["label"]
				nt = G["nodes"][t]["label"]
				ys = int(ns.split("c")[0])
				yt = int(nt.split("c")[0])
				if ys>=T[0] and ys<=T[1] and yt>=T[0] and yt<=T[1]:

					if ys > yt: 
						AG.add_edge(t,s)
						# print G["nodes"][t]["label"] ,"->", G["nodes"][s]["label"]
					else: 
						if ys == yt:
							# print G["nodes"][s]["label"] ,"->", G["nodes"][t]["label"]
							xxx = 10
						else:
							AG.add_edge(s,t)
			except:
				pass


		print(" - - - -- - - - ")
		print "|V|" , len(AG) 
		print "|E|" , AG.number_of_edges()
		print("")
		AG.remove_nodes_from(nx.isolates(AG))


		# A = nx.drawing.nx_agraph.to_agraph(AG)
		# for y in sorted(list(Year_CL.keys())):
		# 	clusters = list(Year_CL[y].keys())
		# 	A.add_subgraph( clusters , rank='same')
		# A.layout(prog='dot')
		# A.draw(self.PATHFILE+'_A.'+self.EXT, prog='dot')


		redundant_ = nx.DiGraph()
		for n in AG.nodes_iter():
			node = AG.node[n]
			if "fake" not in node:
				parents = AG.predecessors( n )
				if len(parents)>=2:
					family = {}
					for p in parents:
						gpy = AG.node[p]["y"]
						if gpy not in family:
							family[ gpy ] = []
						family[ AG.node[p]["y"] ].append( p )
					family[node["y"]] = [ n ]
					family_k = sorted(list(family.keys()))
					family_D = {}
					for i in range(len(family_k)): 
						family_D[ family_k[i] ] = i


					if len(family_k)>=3:
						for p in parents:
							# ancestor = AG.node[p]
							# ego = node
							if ( family_D[node["y"]] - family_D[AG.node[p]["y"]] ) >=2:
								infoA = { "y":AG.node[p]["y"] , "id":p , "label":AG.node[p]["label"] }
								infoB = { "y":node["y"] , "id":n , "label":node["label"] }
								nA_has_descendance = self.verify_descendance( infoA , infoB , family )
								if nA_has_descendance: # then remove the edge
									redundant_.add_edge( infoA["id"] , infoB["id"] )

		AG.remove_edges_from( redundant_.edges() )
		# AG.remove_nodes_from(nx.isolates(AG))
		


		# Remove not-relevant intertemporal-clusters
		clusters_bof = {}
		for n in AG.nodes_iter():
			if "fake" not in AG.node[n]:
				if AG.out_degree(n)==0 and AG.in_degree(n)<=1:
					parent = AG.predecessors( n )[0]
					if AG.out_degree(parent)<=1 and AG.in_degree(parent)==0:
						# print AG.node[parent]["label"] ,",", AG.node[n]["label"]
						if ( AG.node[n]["y"] - AG.node[parent]["y"]) <=999:
							clusters_bof[parent] = True
							clusters_bof[n] = True
		AG.remove_nodes_from( list(clusters_bof.keys()) )
		AG.remove_nodes_from( list(map( str , Year_CL.keys())) )


		print("")
		B = nx.drawing.nx_agraph.to_agraph(AG)
		for y in sorted(list(Year_CL.keys())):
			clusters = list(Year_CL[y].keys())
			clusters.insert(0, str(y) )
			print  y,":", len(clusters)
			# print "\t",clusters
			# print ""
			B.add_subgraph( clusters , rank='same')


		B.layout(prog='dot')
		# A.draw(self.PATHFILE+'_B.'+self.EXT, prog='dot')







		NodesDict = []
		EdgesDict = []

		for i in B:
			# if "fake" not in AG.node[i]:
			n=B.get_node(i)
			coord = n.attr["pos"].split(",")
			# # print " label =", n.attr["label"]# , " : (" , coord[0] , "," , coord[1] , ")"
			# if int(n.attr["y"])==years[-1]:
			#	 print n.attr
			infodict = { "id":i , "label":n.attr["label"] , "shape":"square" , "x":float(coord[0]) , "y":float(coord[1]) }
			NodesDict.append(infodict)


		for e in B.edges_iter():
			s = e[0]
			t = e[1]
			# if "fake" not in AG[s][t]:
			infodict = {"s":s , "t":t , "w":1 , "type":"line" }
			EdgesDict.append(infodict)

		Graph = {  
			"graph":[],
			"directed":False,
			"multigraph":False,
			"nodes": NodesDict,
			"links": EdgesDict
		}

		import json
		f = open( self.PATHFILE+".json","w")
		f.write(json.dumps(Graph,indent=1))
		f.close()




FOLDER = "test20160606"
JCC = "0.5"
if len(sys.argv)>1 and sys.argv[1]!=None: FOLDER = sys.argv[1]
if len(sys.argv)>2 and sys.argv[2]!=None: JCC = sys.argv[2]
NAME = FOLDER.split("/")[-1]+"_"+JCC

def openJSON( filename ):
	# print("reading...")
	f = open( filename , "r" )
	data = json.load( f )
	f.close()
	return data

Phylo = openJSON( FOLDER+"/phylo/"+JCC+".json" )
Sets = openJSON(  FOLDER+"/phylo/"+"sets.json"  )


# from uploadr.phylo_gexf2json import PhyloGen
phylolayout = PhyloGen( FOLDER , NAME ,  "pdf" )
# canon cmap cmapx cmapx_np dot eps fig gd gd2 
# gif gv imap imap_np ismap jpe jpeg jpg pdf pic 
# plain plain-ext png pov ps ps2 svg svgz tk 
# vml vmlz vrml wbmp x11 xdot xdot1.2 xdot1.4 xlib


timerange = [ 1982 , 2007 ]
returnvar = phylolayout.processJSON_test(Phylo , Sets , timerange )
# print "returnvar: "+returnvar
