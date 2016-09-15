#!/usr/bin/env python
# -*- coding: utf-8 -*-


# = = = [ EXECUTION EXAMPLE ] = = = ] #
#
#      python3 52_csv2json.py input:"stats_file.csv"
#
# = = = [ / EXECUTION EXAMPLE ] = = = ] #


# import time
import json as jsonold
import sys

FILE = "stats.csv"
if len(sys.argv)>1 and sys.argv[1]!=None: FILE = sys.argv[1]





import csv

D = {}
headers = []
c = 0
with open( FILE , 'r') as f:
	reader = csv.reader(f, delimiter=' ')
	for row in reader:
		if c==0:
			headers = row
			c += 1
		else:
			D[ row[0] ] = {}
			k = 6
			for e in range(len(row)):
				try:
					# print( headers[e] , "->" , row[e] )
					D[ row[0] ][ headers[e] ] = row[e]
				except:
					if "k" not in D[ row[0] ]:
						D[ row[0] ][ "k" ] = {}
					D[ row[0] ][ "k" ][k] = row[e].split("*")
					# print( "\t" , "k =",k ,"->", row[e].split("*") )
					k += 1
					pass


f = open( FILE.replace(".csv",".json") , "w" )
f.write( jsonold.dumps( D ) )
f.close()
