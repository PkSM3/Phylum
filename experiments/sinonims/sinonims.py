# -*- coding: utf-8 -*-

# import sqlite3

from nltk.stem.porter import PorterStemmer as porter_stem
from nltk.stem.snowball import EnglishStemmer as snowball_stem
from nltk.stem.lancaster import LancasterStemmer as lancaster_stem
from nltk.stem import WordNetLemmatizer as wordnet_lemma

# from stemming.lovins import stem as lovins_stem
from stemming.paicehusk import stem as paicehusk_stem
# from stemming.porter import stem as porter_stem
from stemming.porter2 import stem as porter2_stem


import UnicodeFixer as superunicode
import networkx as nx
from itertools import combinations


class Stemmizing:

	def __init__(self , whichstemmer ):
		self.whichstemmer = whichstemmer
		self.stemmer = self.select_stemmer( whichstemmer )
		self.G = nx.Graph()
		# self.COOCs = coocs

	def select_stemmer( self , stemchoice):

		if stemchoice == "porter":
			st = porter_stem()
			return st.stem

		if stemchoice == "snowball":
			st = snowball_stem()
			return st.stem

		if stemchoice == "lancaster":
			st = lancaster_stem()
			return st.stem

		if stemchoice == "wordnet":
			st = wordnet_lemma()
			return st.lemmatize

		if stemchoice == "paicehusk":
			return paicehusk_stem

		if stemchoice == "porter2":
			return porter2_stem


	def get_stem(self, ngram):

		try:
			stems = list(map(lambda x: self.stemmer(x), ngram.split(' ')))

		except Exception as e:

			if self.whichstemmer=="paicehusk" and "min() arg is an empty sequence" in str(e):
				return ngram

			ngram = superunicode.fix_bad_unicode( unicode(ngram) )
			stems = list(map(lambda x: self.stemmer(x), ngram.split(' ')))


		stems.sort()
		return(str(' '.join(stems)))

	# def get_distances( self , candidates ):
	# 	# self.COOCs
	# 	if len(candidates)>2:
	# 		edges = combinations(candidates, 2)
	# 		for e in edges:
	# 			distance = None
	# 			if self.COOCs.has_edge( e[0] , e[1] ):
	# 				distance = nx.shortest_path_length( self.COOCs , source=e[0] , target=e[1] )
	# 			print "\t", list(e) , distance
	# 	else:
	# 		distance = None
	# 		if self.COOCs.has_edge( candidates[0] , candidates[1] ):
	# 			distance = nx.shortest_path_length( self.COOCs , source=candidates[0] , target=candidates[1] )
	# 		print "\t", candidates  , distance
	# 	print


	def decide_mainform( self , candidates ):
		s = sorted(candidates, key = lambda x: (x[0]) , reverse=True)
		return s[0][1]


	def decide_mainform_test( self , candidates ):
		s = sorted(candidates, key = lambda x: (x[0]) , reverse=True)
		S = []
		for c in s:
			S.append( str(c[0])+"*"+c[1] )
		return s[0][1] , " | ".join( S ) 

	def do_synonims_test( self , L_occs ):
		G2N = {}
		for n in L_occs:
			stem = str(self.get_stem( n ))
			if stem!= n:
				if stem not in G2N:
					G2N[stem] = []
				G2N[stem].append( n )

		Synonims = {}
		for n in G2N:
			if len(G2N[n])>1:
				# print G2N[n] 
				candidates = []
				for s in G2N[n]: 
					candidates.append( [ L_occs[s] , s] )
				mainForm , sorted_cand = self.decide_mainform_test( candidates )
				# self.get_distances( G2N[n] )
				# print mainForm , "\t" ,  sorted_cand
				Synonims[ mainForm ] = { 
					"main": mainForm,
					"subforms": sorted_cand
				}
		
		return Synonims


	def do_synonims( self , L_occs ):
		G2N = {}
		for n in L_occs:
			stem = str(self.get_stem( n ))
			if stem!= n:
				if stem not in G2N:
					G2N[stem] = []
				G2N[stem].append( n )

		Synonims = {}
		for n in G2N:
			if len(G2N[n])>1:
				# print G2N[n] 
				candidates = []
				for s in G2N[n]: 
					candidates.append( [ L_occs[s] , s] )
				mainForm = self.decide_mainform( candidates )
				# # self.get_distances( G2N[n] )
				# print "\t:",mainForm
				for s in G2N[n]:
					if s!=mainForm:
						Synonims[ s ] = mainForm
				# print
		
		return Synonims



# ngrams = []
# dbname = "ngrams.sqlite"
# query='SELECT id,n,c FROM keyword limit 3000'
# connection=sqlite3.connect(dbname)
# connection.row_factory = sqlite3.Row# Magic line!
# connection.isolation_level = None
# cursor=connection.cursor()
# cursor.execute(query)
# rows = cursor.fetchall()

# Ngrams = {}
# for r in rows:
# 	Ngrams[r["id"]] = r["n"]
# 	# ngrams.append( [ r["id"] , r["n"] , r["c"] ] )


# G2N = {}
# for n in Ngrams:
# 	stem = str(self.get_stem(Ngrams[n]))
# 	if stem!=Ngrams[n]:
# 		# n.append(None)
# 		# print( Ngrams[n] , "->" , stem )
# 		if stem not in G2N:
# 			G2N[stem] = []
# 		G2N[stem].append( Ngrams[n] )

# for n in G2N:
# 	if len(G2N[n])>1:
# 		print( G2N[n])
# 		# print("")



# connection.close()
