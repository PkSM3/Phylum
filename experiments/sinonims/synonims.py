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

	def __init__(self , L_occs ):
		self.whichstemmer = False
		self.stemmer = False 
		self.G = nx.Graph()
		self.L_occs = L_occs
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


	def decide_mainform_test( self , candidates ):
		s = sorted(candidates, key = lambda x: (x[0]) , reverse=True)
		S = []
		for c in s:
			S.append( str(c[0])+"*"+c[1] )
		return s[0][1] , " | ".join( S ) 

	def do_synonims_test( self , whichstemmer ):
		self.stemmer = self.select_stemmer( whichstemmer )
		self.whichstemmer = whichstemmer
		G2N = {}
		for n in self.L_occs:
			stem = str(self.get_stem( n ))
			if stem!= n:
				if stem not in G2N:
					G2N[stem] = []
				G2N[stem].append( n )

		# Synonims = []
		for n in G2N:
			if len(G2N[n])>1:
				edges = combinations(G2N[n], 2)
				for i in edges:
					if not self.G.has_edge( i[0] , i[1] ):
						self.G.add_edge( i[0] , i[1] )
					# Synonims.append( G2N[n] )


		self.stemmer = False
		self.whichstemmer = False
		return True
		# return Synonims


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
