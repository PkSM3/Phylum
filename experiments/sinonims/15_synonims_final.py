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

	# index authors and sources and pre-index raw-keywords #
	def extractJSONs ( self  , pub):

		if "keywords" in pub:
			for k in pub["keywords"]:
				kw = superunicode.fix_bad_unicode( unicode( str(k.decode('utf-8')) ) )
				if kw not in self.Dict["K_raw"]:
					self.Dict["K_raw"][kw] = 0
				self.Dict["K_raw"][kw] += 1

		return True


	def stemmizeAll( self , mode="normal", whichstemmer="snowball" ):

		from sinonims import Stemmizing
		# grouping = Stemmizing( self.Kcoocs.G )
		grouping = Stemmizing( whichstemmer )
		if mode =="test": 
			MainForms = grouping.do_synonims_test( self.Dict["K_raw"] )

			test_name = "synonim_"+whichstemmer
			sqldbname = test_name+".sqlite"
			conn = sqlite3.connect( sqldbname )
			conn.row_factory = sqlite3.Row# Magic line!
			conn.isolation_level = None
			c = conn.cursor()
			sqlcreate = "DROP TABLE IF EXISTS "+test_name+"; "
			c.execute(  sqlcreate )
			conn.commit()
			sqlcreate = "CREATE TABLE "+test_name+"( main varchar(255) , subforms TEXT ); "
			c.execute(  sqlcreate )
			conn.commit()
			self.InsertInto_modular( connection=conn , cursor=c , tablename=test_name , data=MainForms )
			conn.close()
			return False

		else:
			MainForms = grouping.do_synonims( self.Dict["K_raw"] )

		del self.Dict["K_raw"]
		# for subform in get_mainform:
		# 	print subform , "->", get_mainform[subform]
		self.Dict["K_getMainForm"] = MainForms



	def decide_mainform_test( self , candidates ):
		s = sorted(candidates, key = lambda x: (x[0]) , reverse=True)
		S = []
		for c in s:
			S.append( str(c[0])+"*"+c[1] )
		return s[0][1] , " | ".join( S ) 

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

		print( results )
		return results


	def InsertInto_modular(self , connection , cursor , tablename , data , step=490):

		results = [ tablename, "OK"]
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
					#print e
					print "FAIL:\t",data
					#print (" - - - - -  - - - - ")
			connection.execute('COMMIT TRANSACTION')
			connection.commit()
		print( results )
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



print "|L| =",len(execc.Dict["K_raw"].keys())
stemmers = [ "porter" , "snowball" , "lancaster" , "wordnet" , "porter2" ,"paicehusk"]
from synonims import Stemmizing
grouping = Stemmizing( execc.Dict["K_raw"] )

for whichstemmer in stemmers:
	print "\tdoing: ",whichstemmer
	grouping.do_synonims_test( whichstemmer )
print
print "|V| =",len(grouping.G)
print "|E| =", grouping.G.number_of_edges()
print 

Synonims = {}
print "... finding cliques"
cliques = nx.find_cliques( grouping.G )
# print "|C| =",len(list(cliques))
for clique in cliques:
	candidates = []
	for term in clique: 
		candidates.append( [ execc.Dict["K_raw"][term] , term] )
	# print candidates
	mainForm , sorted_cand = execc.decide_mainform_test( candidates )
	# print sorted_cand
	# print "\t", mainForm
	# print
	Synonims[ mainForm ] = { 
		"main": mainForm,
		"subforms": sorted_cand
	}



test_name = "synonims"
sqldbname = test_name+".sqlite"
conn = sqlite3.connect( sqldbname )
conn.row_factory = sqlite3.Row# Magic line!
conn.isolation_level = None
c = conn.cursor()
sqlcreate = "DROP TABLE IF EXISTS "+test_name+"; "
c.execute(  sqlcreate )
conn.commit()
sqlcreate = "CREATE TABLE "+test_name+"( main varchar(255) , subforms TEXT ); "
c.execute(  sqlcreate )
conn.commit()
execc.InsertInto_modular( connection=conn , cursor=c , tablename=test_name , data=Synonims )
conn.close()

print
print "END"






# AttsCount = {}
# for pub in execc.Dict["RAW"]:
# 	pub = execc.do_Dictionaries( pub )
# 	for k in sorted(pub.keys()):
# 		if k not in AttsCount:
# 			AttsCount[k] = 0
# 		AttsCount[k] += 1
# 	# 	print k,":",pub[k]
# 	# print
# 	# print

# print("")
# print("")
# pprint.pprint( AttsCount )





# for s in execc.Dict["S"]:
# 	print execc.Dict["S"][s]
# 	# print








# for k in execc.Dict["K"]["_i2s_"]:
# 	execc.Dict["K"]["_i2s_"][k]["id"] = k
# 	if execc.Dict["K"]["_i2s_"][k]["c"] > 1:
# 		execc.Dict["K"]["_i2s_"][k]["v"] = round( execc.Dict["K"]["_i2s_"][k]["v"]/float(execc.Dict["K"]["_i2s_"][k]["c"]) , 2)
# 	# print execc.Dict["K"]["_i2s_"][k]["c"] ,"\t", execc.Dict["K"]["_i2s_"][k]["name"] 

# for k in execc.Dict["A"]:
# 	execc.Dict["A"][k]["id"] = k
# 	if execc.Dict["A"][k]["c"] > 1:
# 		execc.Dict["A"][k]["v"] = round( execc.Dict["A"][k]["v"]/float(execc.Dict["A"][k]["c"]) , 2 )




# # execc.InsertInto( tablename="authors" , data=execc.Dict["A"] )
# # execc.InsertInto( tablename="keyword" , data=execc.Dict["K"]["_i2s_"] )
# # execc.InsertInto( tablename="publications" , data=execc.Dict["P"] )
execc.connection.close()

