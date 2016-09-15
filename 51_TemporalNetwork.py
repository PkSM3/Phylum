#!/usr/bin/env python
# -*- coding: utf-8 -*-



# = = = [ EXECUTION EXAMPLE ] = = = ] #
#   be a man and make sure that "experiments/test00" exists.
#
#		     experiment: { input:"DATA/ACM-DL/ACM-DL_2016.sqlite"  year:1980  aggregation:1  occmin:2  cooccmin:1  output:"experiments/test00" }
#
#	    python3  51_TemporalNetwork.py  "DATA/ACM-DL/ACM-DL_2016.sqlite"  1980  1  2  1  "experiments/test00"
#
#
# = = = [ / EXECUTION EXAMPLE ] = = = ] #




import sys
import pprint
import time
import json as jsonold


# = = = = = = [ A R G U M E N T S ] = = = = = = #
INPUT = "ACM-DL.sqlite"
timerange = "1951"
time_A = int(timerange)
window_ = 0
time_B = str( time_A + window_ )
OCCMIN = 1
COOCCMIN = 1
# THRESHOLD = 1
OUTFOLDER = "experiments/test00"
COMMUNITIES = "CPM" # "FPG"

if len(sys.argv)>1 and sys.argv[1]!=None: INPUT=sys.argv[1]

if len(sys.argv)>2 and sys.argv[2]!=None: 
	time_A = int(sys.argv[2])
	time_B = str( time_A + (window_-1) )
	timerange = sys.argv[2]

if len(sys.argv)>3 and sys.argv[3]!=None:
	window_ = int(sys.argv[3])
	if window_ > 0:
		time_B = str( time_A + (window_-1) )
		# timerange = timerange + "-" + time_B

if len(sys.argv)>4 and sys.argv[4]!=None: OCCMIN = int(sys.argv[4])

if len(sys.argv)>5 and sys.argv[5]!=None: COOCCMIN = int(sys.argv[5])
# if len(sys.argv)>6 and sys.argv[6]!=None: THRESHOLD = int(sys.argv[6])

if len(sys.argv)>6 and sys.argv[6]!=None: OUTFOLDER=sys.argv[6]

# = = = = = = [ /  A R G U M E N T S ] = = = = = = #



publist = []
occurrences = {}
# = = = = = = [ CHOOSING DB-SOURCE ] = = = = = = #
if "ACM" in INPUT.upper():
	from DATA.ACMDL_extractor import Extract
	get_ = Extract( DEBUG=False )
	publist , occurrences = get_.calc_ACM_OCCs ( dbfile=INPUT , t = [ str(time_A)+"-01" , str(time_B)+"-12"] )

if "DBLP" in INPUT.upper():
	from DATA.DBLP_extractor import Extract
	get_ = Extract( DEBUG=False )
	publist , occurrences = get_.calc_DBLP_OCCs ( dbfile=INPUT , t = [ str(time_A) , str(time_B) ] )

if "ARXIV" in INPUT.upper():
	print(INPUT,"not ready")
	sys.exit(1)

if "CITESEERX" in INPUT.upper():
	print(INPUT,"not ready")
	sys.exit(1)

if "CERN" in INPUT.upper():
	print(INPUT,"not ready")
	sys.exit(1)

if "PUBMED" in INPUT.upper():
	# from DATA.PubMed_extractor import Extract
	from DATA.PubMed_extractor_test import Extract
	get_  = Extract()
	publist , occurrences = get_.calc_OCCs( INPUT , str(time_A) )
	print( "|P|:", len(publist) )
	print( "|V|:", len(occurrences.keys()) )
	COMMUNITIES = "FPG"
	# sys.exit(1)

if "WOS" in INPUT.upper():
	print(INPUT,"not ready")
	sys.exit(1)

if "REPEC" in INPUT.upper():
	print(INPUT,"not ready")
	sys.exit(1)
# = = = = = = [ / CHOOSING DB-SOURCE ] = = = = = = #




if COMMUNITIES == "CPM":
	print("Clique Percolation Method mode")

	from Phylo import Metrics
	FP = Metrics(DEBUG=False , pubs_list = publist , occurrences = occurrences )
	FP.calc_COOCs( occs_min = OCCMIN , coocc_min = COOCCMIN )
	# L_occs = FP.occurrences
	L_coocs = FP.cooccurrencesG
	# L_coocs = FP.calc_distance_PseudoInclusion(  s_min = 0.0001 , alpha = 1)
	# L_coocs = FP.calc_distance_Distributional() #  DANGER, DO NOT USE!
	# FP.calc_distance_CosineSimilarity_limitbycoocs( FP.cooccurrencesG )
	# L_coocs = FP.calc_distance_CosineSimilarity__MultiCore( )  # Faster when   Pubs > 2.000.000

	# # print( timerange ,"\t", len( FP.cooccurrencesG ) ,"\t", FP.cooccurrencesG.number_of_edges() )
	# # Cosine = FP.calc_distance_CosineSimilarity ( FP.cooccurrencesG )   # then use FP.calc_T_edgesfilter( >threshold )
	# # for s in range(0,30):
	# # 	FP.calc_T_edgesfilter ( Cosine["edges"] , Cosine["max_edgeweight"] , edge_threshold = s )

	# L_coocs = FP.normalize_edges ( FP.cooccurrencesG )
	# L_coocs = FP.automatic_threshold( L_coocs )


	# # [ CLIQUE PERCOLATION METHOD ] # # 
	from CPM import CPM
	cpm = CPM( L_coocs ) # Passing the Graph
	CPM_cliques_time = cpm.find_cliques() # all_cliques calculated internally. returns seconds
	if len(cpm.all_cliques) == 0:
		sys.exit(1)

	min_cliq = 4

	Dist_C = {}
	for clique in cpm.all_cliques:
		len_ = len(clique)
		if len_ not in Dist_C:
			Dist_C[len_] = 0
		Dist_C[len_] += 1

	last_number = -1

	clq_c = 0
	stats_per_k = {}
	CPM_array = {}
	CPM_perc_time = 0
	for k in range( min_cliq , 1000 ):

		start = time.time()
		kcliques = list( cpm.get_percolated_cliques( k ) )
		end = time.time()
		CPM_perc_time+=(end - start)
		time_s = float("{0:.5f}".format((end - start)))

		nb_communities = len( kcliques )

		if   nb_communities > 2   and   nb_communities != last_number:

			outfile =  timerange+"_k"+str(k)+".txt"
			f = open( OUTFOLDER+"/cliques/"+outfile , "w" )

			kcliques_export = []
			nb_terms = 0
			for ci in range(nb_communities):
				kclique = sorted(list( kcliques[ci] ))
				nb_terms += len( kclique )
				# kcliques_export.append( kclique )
				clq_c += 1
				clq_id = timerange+"c"+str(clq_c)
				clq_str = " ".join(list(map(str,kclique)))
				f.write( clq_id+" "+ clq_str+" (1)\n" )
			f.close()

			density = ( nb_terms / nb_communities )

			CPM_array[k] = { 
				"#c" : nb_communities,
				"t" : time_s,
				"density" : float("{0:.1f}".format(density)),
			}
			# print("k:",k)
			# print("#cliques:",nb_communities)
			# print("#density:",float("{0:.1f}".format(density)))
			# print("")
			info = str( CPM_array[k]["#c"] )+"*"+str( CPM_array[k]["density"] )
			stats_per_k[k] = info 

			last_number = nb_communities


		else:
			break

	# # [ / CLIQUE PERCOLATION METHOD ] # # 

	FINAL = {
		"T": timerange,
		"|P|": len(publist),
		"|V|": FP.stats["nodes"],
		"|E|": FP.stats["edges"],
		"|C|": Dist_C,
		"|k|": stats_per_k
	}


	outfile =  OUTFOLDER+"/stats/"+str(timerange)+".json"
	f = open( outfile , "w" )
	f.write( jsonold.dumps( FINAL, indent=1 ) )
	f.close()



if COMMUNITIES == "FPG":
	import os
	SUPP = "20"
	# C = fpg_workflow()
	print("FP-growth mode")

	ff_name = OUTFOLDER+"/trans/"+str(timerange)+".txt"
	ff = open( ff_name , "w" )
	E = publist
	D = {}
	for Ei in E:
		if len(Ei)>=4:
			l_str = list(map(str,sorted(Ei)))
			idx = "_".join(l_str)
			if idx not in D:
				D[idx] = True
				ff.write( " ".join(l_str)+"\n" )
	ff.close()
	P_ = len(D.keys())
	print( "|P'|:", P_ )

	SUPP = int(str(P_/100000.0).split(".")[0])  # int(str(P_)[0])
	SUPP = SUPP*2+4
	print( "supp:", SUPP )

	# "cd fp-growth"
	ff_name_fpg = ff_name+".sup"+str(SUPP)+".txt"
	query = "fp-growth/./fim_maximal "+ff_name+" "+str(SUPP)+" "+ff_name_fpg
	print("DOING  ",query)
	# os.popen( query , 'w' , 1 )
	os.system( query )
	print( "FINISH   ",ff_name_fpg )


	FIs = {}
	fff = open( ff_name_fpg , "r" )
	for line in fff:
		itemset = line.replace("\n","").split(" ")
		nb_items = len(itemset)-1
		# print(nb_items,"\t",itemset)
		if nb_items>=1:
			supp = itemset[-1]
			# supp = int(supp.replace("(","").replace(")",""))
			# print(supp)
			freq_items_ints = sorted(list(map(int,itemset[:-1])))
			len_itemset = len( freq_items_ints )
			if len_itemset >=4:
				# print( supp ,":", freq_items_ints )
				freq_items = list(map(str,freq_items_ints))
				# idx = "_".join( freq_items )
				if len_itemset not in FIs:
					FIs[len_itemset] = []
				itemset_s = (" ".join( freq_items )) +" "+supp 
				FIs[len_itemset].append( itemset_s )
	fff.close()

	stats_per_k = {}
	CPM_array = {}
	c_i = 0
	for k in FIs:
		# print("size",k,": ",len(FIs[k]))
		outfile =  timerange+"_k"+str(k)+".txt"
		ffff = open( OUTFOLDER+"/cliques/"+outfile , "w" )


		CPM_array[k] = { 
			"#c" : len(FIs[k]),
			"density" : float("{0:.1f}".format(float(k))),
		}
		info = str( CPM_array[k]["#c"] )+"*"+str( CPM_array[k]["density"] )
		stats_per_k[k] = info 


		for c in FIs[k]:
			c_i += 1
			c_id = timerange+"c"+str(c_i)
			ffff.write( c_id+" "+ c + "\n")
		ffff.close()



	FINAL = {
		"T": timerange,
		"|P|": len(publist),
		"|V|": len(occurrences.keys()),
		# "|E|": FP.stats["edges"],
		# "|C|": Dist_C,
		"|k|": stats_per_k
	}


	outfile =  OUTFOLDER+"/stats/"+str(timerange)+".json"
	f = open( outfile , "w" )
	f.write( jsonold.dumps( FINAL, indent=1 ) )
	f.close()
