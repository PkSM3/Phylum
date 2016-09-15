/*----------------------------------------------------------------------
  File    : Application.h
  Contents: Class that count the support in a database stored in memory
  Author  : Frédéric Flouvat
----------------------------------------------------------------------*/

#ifndef APPLICATION_H
#define APPLICATION_H

#include "tract.h"

#include <fstream>
#include <time.h>
#include <algorithm>
#include <string>

#include "Trie.h"

using namespace std;

class Element
{
public:
	Element(int iold, int inew=0) : oldid(iold), id(inew){}

	int oldid;
	int id;
	bool operator< (const Element  &e) const { return oldid < e.oldid;}
};


class Application
{
protected :

    // ------------------------------
	// actual level 
	// ------------------------------

	int k ;
	// ---------------------------
	// represent a set of itemset
	// ---------------------------

	Trie * trie;

	// ------------------------
	// minsup of the algorithm
	// ------------------------

	int minsup;

	// -----------------------
	// name of the save file
	// of the positive border
	// -----------------------

	string saveFile ;
	
	// -----------------------
	// name of the data file
	// -----------------------

	char *fileName;

	// ----------------------------------
	// object that store the database
	// in memory
	// ----------------------------------

	TATREE * tatree ;

	// -------------------------------------------------
	// table of int that enable to map the internal
	// id (the index in the table)
	// into the item real name (the data of the table )
	// -------------------------------------------------

	short unsigned * remap;

	// --------------------------------------
	// set of element containing explicitely
	// the mapping between the internal id
	// and the name of the item
	// --------------------------------------

	set< Element > * relist;

	// -------------------------------------
	// boolean that enable the screen print 
	// of information about the processing
	// of the algorithm
	// -------------------------------------

	bool verbose;

	// -----------------------------------------------
	// set of itemset representing de negative border
	// -----------------------------------------------

	Trie * bdN ;

	// -----------------------------------------------
	// set of itemset representing de positive border
	// -----------------------------------------------

	Trie * bdP ;



public:

	// --------------------
	// default constructor
	// --------------------
	
	Application();

	// -----------------------
	// default destructor
	// -----------------------

	virtual ~Application();

	// ------------------------------
	// method initializing the level
	// ------------------------------

	void setLevel( int level ){ k = level ; }

	// --------------------------------------------------
	// method initialising data
	// with in input the name of the file which countain
	// the data base and the foramt of the file
	// --------------------------------------------------

	void setData(char *fn){ fileName = fn;} 

	// -------------------------------
	// method initialising the minsup
	// -------------------------------

	void setMinSup(int ms){minsup=ms;} 

	// ------------------------------------
	// method initialising the name of the
	// save file of the positive border
	// ------------------------------------

	void setSaveFile( char * inSaveFile ) { saveFile = inSaveFile ; }

	// ------------------------------------
	// method that enable the screen print
	// of information about the processing
	// of the algorithm
	// ------------------------------------

	void setVerbose(){verbose=true;} 

	// -------------------------------
	// method initialising trie
	// -------------------------------

	void setTrie(  Trie * inTrie ){ trie = inTrie; } 

	// -------------------------------
	// method initialising relist
	// -------------------------------

	void setRelist(  set< Element > * inList ){ relist = inList; } 

	// -------------------------------
	// method initialising remap
	// -------------------------------

	void setRemap(  short unsigned * inMap ){ remap = inMap; } 

	// --------------------------------------
	// method returning the actual level
	// --------------------------------------	

	int getLevel(){ return k ; }

	// --------------------------------------
	// method returning the list for mapping
	// --------------------------------------	

	set< Element > * getRelist() { return relist ; } 

	// ---------------------------------------
	// method returning the frequent itemsets
	// ---------------------------------------	

	Trie * getTrie() { return trie ; } 

	// -------------------------------------
	// method returning the negative border
	// -------------------------------------

	Trie * getBdN() { return bdN ; } 
	
	// ----------------------------------------
	// method initialising the negative border
	// ----------------------------------------

	void setBdN( Trie * inBdN ) { bdN = inBdN ; } 

	// -------------------------------------
	// method returning the positive border
	// -------------------------------------

	Trie * getBdP() { return bdP ; } 

	// ----------------------------------------
	// method initialising the positive border
	// ----------------------------------------

	void setBdP( Trie * inBdP ) { bdP = inBdP ; } 

	// -----------------------------------
	// method returning the mapping table
	// -----------------------------------

	short unsigned * getRemap(){ return remap ; } 
		
	// ----------------------------------------------------------
	// this method update the support for the set of itemsets tr
	// acording to the database stored in tat
	// only the transaction of size >= min are considered
	// ----------------------------------------------------------

	int countCandidates(Trie * tr,TATREE *tat, int min =0 );

	int count(Trie * tr ,TATREE *tat, int min =0 );

	int countLeaf( int *t, int size, int cnt, Trie * tr, int spos = 0) ;
	
};

#endif
