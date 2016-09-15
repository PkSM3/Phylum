# -*- coding: utf-8 -*-

import time
import re
import pprint
import json as jsonold
import glob
import sqlite3
import sys
reload(sys)
sys.setdefaultencoding("utf-8")
import networkx as nx

from InterUnion import Utils
import UnicodeFixer as superunicode


class SQL:

	def __init__(self , dbname):
		self.db = dbname
		self.connection=sqlite3.connect(dbname)
		self.connection.row_factory = sqlite3.Row# Magic line!
		self.connection.isolation_level = None
		self.cursor=self.connection.cursor()
		self.keys = {
			"id": "id",
			"date": "date",
			"title": "name",
			"abstract": "abstract",
			"authors": "authors",
			"keywords": "keywords",
			"c": "c",
			"source_id": "s_id",
			"source_type": "s_type"
		}
		self.Dict = {
			"RAW" : [],
			"P": {} ,
			"A": {} ,
			"K_raw" : {}, # just temporal
			"K": { # without subforms
				"_s2i_": {},
				"_i2s_": {},
				"_c_": 0
			} ,
			"K_getMainForm": False,
			"S": {} ,
			"P2A": {} ,
			"P2K": {} ,
			"NN": -1
		}
		# self.Kcoocs = Utils()


	def index_authors( self , array ):
		authors = []
		auth_len = len(array)
		for a in array:
			if "id" not in a:
				a["id"] = str(self.Dict["NN"])
				self.Dict["NN"] = self.Dict["NN"]-1 
			a["id"] = int(a["id"])			

			if a["id"] not in self.Dict["A"]:
				self.Dict["A"][ a["id"] ] = {
					"name": a["name"],
					"c": 0,
					"v": 0
				}
			self.Dict["A"][ a["id"] ]["c"] += 1
			self.Dict["A"][ a["id"] ]["v"] += auth_len
			
			authors.append( a["id"] )

		return authors


	def index_keywords( self , array ):
		keys = []
		nb_keys = len(array)
		for a in array:
			kw = superunicode.fix_bad_unicode( unicode( str(a.decode('utf-8')) ) )

			# # UNCOMMENT FOR STEMMING # # #
			# if kw in self.Dict["K_getMainForm"]:
			# 	# print "OVERWRITE\t", kw ,"<-", self.Dict["K_getMainForm"][kw]
			# 	kw = self.Dict["K_getMainForm"][kw]
			# # / UNCOMMENT FOR STEMMING # # #

			if kw not in self.Dict["K"]["_s2i_"]:
				self.Dict["K"]["_c_"] += 1
				self.Dict["K"]["_s2i_"][ kw ] = self.Dict["K"]["_c_"]
				self.Dict["K"]["_i2s_"][ self.Dict["K"]["_c_"] ] = {
					"name" : kw,
					"c" : 0,
					"v" : 0
				}
			self.Dict["K"]["_i2s_"][   self.Dict["K"]["_s2i_"][ kw ]   ][ "c" ] += 1
			self.Dict["K"]["_i2s_"][   self.Dict["K"]["_s2i_"][ kw ]   ][ "v" ] += nb_keys

			keys.append( self.Dict["K"]["_s2i_"][ kw ] ) # int-ID of keyword

		# return "|".join(map(str, keys))
		return keys


	# index authors and sources and pre-index raw-keywords #
	def extractJSONs ( self  , pub):

		if "authors" in pub:
			pub["authors"] = self.index_authors( pub["authors"] )
			pub["authors"] = "|".join(map(str, pub["authors"]))
		pub["id"] = int( pub["id"] )
		self.Dict["RAW"].append( pub )

		if "keywords" in pub:
			for k in pub["keywords"]:
				kw = superunicode.fix_bad_unicode( unicode( str(k.decode('utf-8')) ) )
				if kw not in self.Dict["K_raw"]:
					self.Dict["K_raw"][kw] = 0
				self.Dict["K_raw"][kw] += 1

		return True


	def decide_mainform( self , candidates ):
		s = sorted(candidates, key = lambda x: (x[0]) , reverse=True)
		return s[0][1]

	def find_stems_cliques ( self, stemmers ):

		print "|L| =",len(self.Dict["K_raw"].keys())
		from synonims import Stemmizing
		grouping = Stemmizing( self.Dict["K_raw"] )

		for whichstemmer in stemmers:
			# print "\tdoing: ",whichstemmer
			grouping.do_synonims_test( whichstemmer )

		Synonims = {}
		# print "... finding cliques"
		cliques = nx.find_cliques( grouping.G )
		# print "|C| =",len(list(cliques))
		for clique in cliques:
			candidates = []
			for term in clique: 
				candidates.append( [ self.Dict["K_raw"][term] , term] )
			# print candidates
			mainForm = self.decide_mainform( candidates )
			for s in clique:
				if s!=mainForm:
					Synonims[ s ] = mainForm

		# print
		print "|L_v| =",len(grouping.G)
		print "|L_e| =", grouping.G.number_of_edges()
		print 

		self.Dict["K_getMainForm"] = Synonims

		return True


	def do_Dictionaries( self , pub ):

		if "keywords" in pub:
			pub["keywords"] = self.index_keywords( pub["keywords"] )
			pub["keywords"] = "|".join(map(str, pub["keywords"]))

		if "source_id" in pub:
			pub["source_id"] = int( pub["source_id"] )
			if pub["source_id"] not in self.Dict["S"]:
				self.Dict["S"][pub["source_id"]] = {
					"id": pub["source_id"],
					"type": pub["source_type"],
					"name": pub["source"],
					"P": []
				}
			self.Dict["S"][pub["source_id"]]["P"].append( pub["id"] )


		p_i = {}
		for k in pub.keys():
			if k in self.keys:
				p_i[ self.keys[k] ] = pub[k]
		self.Dict["P"][p_i["id"]] = p_i



	def insertFromDict(self , table, d):
		"""Return SQL statement and data vector for insertion into table."""
		fields = []
		for k in d:
			fields.append( k )

		sql = 'INSERT OR IGNORE INTO %s (%s) VALUES(%s)' % ( table, ", ".join(fields), ", ".join("?" for f in fields))
		#if d["c"] > 2:
		#	print( sql, d.values() )
		return sql, d.values()

	# BULK INSERT in SQLite \m| aaarrgh
	def InsertInto(self , tablename , data , step=490):

		results = [ tablename, "OK"]
		print( results )
		from itertools import izip_longest, ifilter

		step_chunks = [data.iteritems()]*step
		chunks = (dict(ifilter(None, v)) for v in izip_longest(*step_chunks))


		for chunk in chunks:
			# self.connection.execute('BEGIN TRANSACTION')
			for item in chunk:
				sql, data = self.insertFromDict( tablename ,  chunk[item])
			# 	try:
			# 		self.cursor.execute(sql, data)
			# 	except Exception as e:
			# 		#print e
			# 		print "FAIL:\t",data
			# 		#print (" - - - - -  - - - - ")
			# self.connection.execute('COMMIT TRANSACTION')
			# self.connection.commit()

		return results


	def InsertInto_modular(self , connection , cursor , tablename , data , step=490):

		print [ tablename , len(data.keys()) ]
		from itertools import izip_longest, ifilter

		step_chunks = [data.iteritems()]*step
		chunks = (dict(ifilter(None, v)) for v in izip_longest(*step_chunks))


		for chunk in chunks:
			connection.execute('BEGIN TRANSACTION')
			for item in chunk:
				sql, data = self.insertFromDict( tablename ,  chunk[item])
				try:
					cursor.execute(sql, data)
				except Exception as e:
					print e
					print "FAIL:\t",data
					print (" - - - - -  - - - - ")
			connection.execute('COMMIT TRANSACTION')
			connection.commit()
		results = [ tablename, "OK"]
		print "\t", results 
		return results




FOLDER = "../../2016-02-08_22-49-35__pubs/2016-"
MODE = "normal"
STEMMER = "porter2"
if len(sys.argv)>1 and sys.argv[1]!=None: FOLDER = sys.argv[1]
if len(sys.argv)>2 and sys.argv[2]!=None: MODE = sys.argv[2]
if len(sys.argv)>3 and sys.argv[3]!=None: STEMMER = sys.argv[3]
execc = SQL( "acm.sqlite" )

for filename in glob.iglob(FOLDER+'*'):
	# print( filename )
	json_data = open( filename , "r")
	jsonfilecontent = json_data.read()
	json_data.close()
	pub = jsonold.loads( jsonfilecontent  )
	execc.extractJSONs( pub ) # index authors and sources and pre-index raw-keywords
# # OUTPUT: Dict["RAW"] -> list for publications


# # UNCOMMENT FOR STEMMING # # #
# # stemmers = [ "porter" , "snowball" , "lancaster" , "wordnet" , "porter2" ,"paicehusk"]
# stemmers = [  "snowball" , "lancaster" , "wordnet" , "porter2" ]
# # stemmers = [ "snowball" ]
# execc.find_stems_cliques( stemmers )
# # UNCOMMENT FOR STEMMING # # #

for pub in execc.Dict["RAW"]:
	p = execc.do_Dictionaries( pub )
del execc.Dict["RAW"]


for s in execc.Dict["S"]:
	execc.Dict["S"][s]["P"] = "|".join(map(str, execc.Dict["S"][s]["P"]))


for k in execc.Dict["K"]["_i2s_"]:
	execc.Dict["K"]["_i2s_"][k]["id"] = k
	if execc.Dict["K"]["_i2s_"][k]["c"] > 1:
		execc.Dict["K"]["_i2s_"][k]["v"] = round( execc.Dict["K"]["_i2s_"][k]["v"]/float(execc.Dict["K"]["_i2s_"][k]["c"]) , 2)
	# print execc.Dict["K"]["_i2s_"][k]["c"] ,"\t", execc.Dict["K"]["_i2s_"][k]["name"] 

for k in execc.Dict["A"]:
	execc.Dict["A"][k]["id"] = k
	if execc.Dict["A"][k]["c"] > 1:
		execc.Dict["A"][k]["v"] = round( execc.Dict["A"][k]["v"]/float(execc.Dict["A"][k]["c"]) , 2 )
	# print execc.Dict["A"][k]["c"] , "\t" , execc.Dict["A"][k]["name"]




test_name = "ACM2016_v0"
sqldbname = test_name+".sqlite"
conn = sqlite3.connect( sqldbname )
conn.row_factory = sqlite3.Row# Magic line!
conn.isolation_level = None
c = conn.cursor()

sql_commands = [
	"DROP TABLE IF EXISTS author;",
	"DROP TABLE IF EXISTS keyword;",
	"DROP TABLE IF EXISTS source;",
	"DROP TABLE IF EXISTS publication;",
	"CREATE TABLE author( id INTEGER PRIMARY KEY , name VARCHAR(255) , c INTEGER , v REAL )",
	"CREATE TABLE keyword( id INTEGER PRIMARY KEY , name VARCHAR(255) , c INTEGER , v REAL )",
	"CREATE TABLE source( id INTEGER PRIMARY KEY , type VARCHAR(100) , name TEXT , P TEXT )",
	"CREATE TABLE publication( id INTEGER PRIMARY KEY , name TEXT , date VARCHAR(100) , abstract TEXT , authors TEXT , keywords TEXT , c INTEGER , s_id INTEGER , s_type VARCHAR(100) )",
]

for command in sql_commands:
	c.execute(  command )
	conn.commit()

execc.InsertInto_modular( connection=conn , cursor=c , tablename="author" , data=execc.Dict["A"] )
execc.InsertInto_modular( connection=conn , cursor=c , tablename="keyword" , data=execc.Dict["K"]["_i2s_"] )
execc.InsertInto_modular( connection=conn , cursor=c , tablename="source" , data=execc.Dict["S"] )
execc.InsertInto_modular( connection=conn , cursor=c , tablename="publication" , data=execc.Dict["P"] )
# # execc.InsertInto( tablename="publications" , data=execc.Dict["P"] )
conn.close()
execc.connection.close()
print
print "END"

