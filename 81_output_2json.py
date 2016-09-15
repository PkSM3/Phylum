
#!/usr/bin/env python
# -*- coding: utf-8 -*-


import json
import sys
import os
import re
import networkx as nx


#
# python3 01_workflow-output_2json.py testMESH/edgesk9 testMESH_formatted
#

INPUT = "../test20160606/edgesk7"
OUTPUT = "../test20160606_formatted"
JCC = 0.0
if len(sys.argv)>1 and sys.argv[1]!=None: INPUT = sys.argv[1]
if len(sys.argv)>2 and sys.argv[2]!=None: OUTPUT = sys.argv[2]
if len(sys.argv)>3 and sys.argv[3]!=None: JCC = float(sys.argv[3])


def writeDict(orig_file , name , array ):
	filename = orig_file+name
	f = open( filename , "w" )
	f.write( json.dumps( array , indent=1) )
	f.close()
	return filename

def writeG( filename , G , Labels ):

	# from networkx import isolates
	# G.remove_nodes_from(isolates(G))

	B = {}
	B["nodes"] =[]
	for n in G.nodes_iter():
		COOCC = G.degree( n )
		node = {
			"attributes":{
				"coocc": COOCC
			},
			"id": n,
			"label": Labels[n],
			"type": "Doc",
			"size": 3 #GraphA.degree(n)
		}
		B["nodes"].append( node )



	B["links"] = []
	for e in G.edges_iter():
		s = e[0]
		t = e[1]
		weight = G[s][t]["weight"]
		info = {  "s": s , "t": t , "w": weight  }
		B["links"].append(info)


	outfile =  filename
	f = open( outfile , "w" )
	f.write( json.dumps( B, indent=1 ) )
	f.close()




def openJSON( filename ):
	# print("reading...")
	data = []
	f = open( filename , "r" )
	# data = json.load( f )
	for line in f:
		lien = line.replace("\n","").split(" ")
		# print(lien)
		data.append( lien )
	f.close()
	return data



def iter_folder( FOLDER ):

	G = nx.Graph()
	suma = 0

	D = {}
	D_c = 0
	D_s2i = {}
	D_i2s = {}

	query = 'ls '+FOLDER+"/*"
	pubs_folder = os.popen(  query  )

	elems = []
	for f_ in pubs_folder:
		filepath = f_.replace("\n","")
		# print(filepath)
		P = openJSON(filepath)
		for i in P:
			nodeA_ID = i[0]
			nodeB_ID = i[1]
			jacc = float(i[2])
			setA = i[3].split(",")
			setB = i[4].split(",")
			# print(jacc)
			if jacc >= JCC:
				the_ID_1 = -1
				if nodeA_ID not in D_s2i:
					D_c += 1
					the_ID_1 = D_c
					D[the_ID_1] = setA
				else:
					the_ID_1 = D_s2i[nodeA_ID]
				D_s2i[nodeA_ID] = the_ID_1
				D_i2s[the_ID_1] = nodeA_ID



				the_ID_2 = -1
				if nodeB_ID not in D_s2i:
					D_c += 1
					the_ID_2 = D_c
					D[the_ID_2] = setB
				else:
					the_ID_2 = D_s2i[nodeB_ID]
				D_s2i[nodeB_ID] = the_ID_2
				D_i2s[the_ID_2] = nodeB_ID



				G.add_edge( the_ID_1 , the_ID_2 , weight= jacc )


	writeDict( OUTPUT+"/phylo/sets" , ".json" , D )
	writeDict( OUTPUT+"/phylo/sets__D_s2i" , ".json" , D_s2i )
	writeDict( OUTPUT+"/phylo/sets__D_i2s" , ".json" , D_i2s )
	writeG( OUTPUT+"/phylo/"+str(JCC)+".json" , G , D_i2s )
	return D



Nodes = iter_folder( INPUT )
print(len( Nodes.keys() ))

# from uploadr.phylo_gexf2json import PhyloGen
# phylolayout = PhyloGen()


# returnvar = phylolayout.process(destination)
# print "returnvar: "+returnvar
