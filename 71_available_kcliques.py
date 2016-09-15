#!/usr/bin/env python
# -*- coding: utf-8 -*-


# = = = [ EXECUTION EXAMPLE ] = = = ] #
#
#      python3 71_available_kcliques.py  input:"test03/stats.json"  k:11
#
# = = = [ / EXECUTION EXAMPLE ] = = = ] #


# import time
import json
import sys
import os

JSONSTATS = "experiments/test00/stats"
K = 11
if len(sys.argv)>1 and sys.argv[1]!=None: JSONSTATS = sys.argv[1]
if len(sys.argv)>2 and sys.argv[2]!=None: K = int(sys.argv[2])


def openJSON( filename ):
	# print("reading...")
	f = open( filename , "r" )
	data = json.load( f )
	f.close()
	return data

def iter_folder( FOLDER ):

	V = {}
	suma = 0

	query = 'ls '+FOLDER+"/*"
	pubs_folder = os.popen(  query  )

	elems = []
	for i in pubs_folder:
		filepath = i.replace("\n","")
		stats = openJSON(filepath)
		V[stats["T"]] = stats
	return V



D = iter_folder( JSONSTATS )
periods = []
last_p = -1
for p in sorted(D.keys() , reverse=True):
	if "|k|" in D[p]:
		if len(D[p]["|k|"].keys())>0:
			if str(K) in D[p]["|k|"]:
				#print(p)
				period = int(p)
				if last_p==-1  or (period+1)==last_p:
					periods.append( period )
					last_p = period

P = sorted( periods )
print( P[0] , P[-1] )
