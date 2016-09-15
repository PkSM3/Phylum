/*----------------------------------------------------------------------
  File    : Zigzag.cpp
  Contents: Class that implement the algorithm ABS.
  Author  : Frédéric Flouvat
----------------------------------------------------------------------*/

#ifndef ZIGZAG_H
#define ZIGZAG_H

#include "apriori.h"
#include "tract.h"

#include "Application.h"


class Zigzag: public Application 
{
protected:
	
	// --------------------------------------------------
	// minimal proportion of non frequents itemsets 
	// compared to the number of candidates itemsets for
	// a level i 
	// (used to determine until which level the
	// levelwise approach is applied)
	// --------------------------------------------------

	double ratioNfC ;

	// ------------------------------------------
	// Maximal distance from the positive border
	// to go through from the top
	// ( error measure threshold )
	// ------------------------------------------

	double eps ;

	// --------------------------------------------
	// set of itemset representing itemsets of the 
	// positive border found by the initialisation
	// --------------------------------------------

	Trie * bdPapriori ;
	
	// ------------------------------
	// Apriori last level
	// ------------------------------

	int initK ;
	
	// -------------------------------------------------
	// variable for retrieving some stistics on results
	// -------------------------------------------------

	// -----------------------------------------------
	// max size of an itemsets of the positive border
	// -----------------------------------------------

	int maxBdP ;

	// ----------------------------------------------------
	// average size of the itemsets of the positive border
	// ----------------------------------------------------

	double avgBdP ;
	
	
	

public:

	// ------------------------
	// default constructor
	// ------------------------

	Zigzag() ;

	// -----------------------
	// default destructor
	// -----------------------

	~Zigzag() ;
	
	// ------------------------------
	// method initializing the min
	// proportion of non frequent
	// according to the candidates
	// for the levelwise approach
	// ------------------------------

	void setRatioNfC( double inRatio ){ ratioNfC = inRatio ; }	

	// ------------------------
	// method initializing Eps
	// ------------------------

	void setEps( double inEps ){ eps = inEps ; }

	// ----------------------------------------
	// method that execute the algrithm zigzag
	// ----------------------------------------

	int run() ;

	// ----------------------------------------------
	// process the optimist positive border
	// all the frequent update Bd+, the non frequent 
	// with an error < "eps" are stored in optIt and 
	// the others are keep in the set.
	// store in stat the number of itemsets by level
	// stat[0] is total number of itemsets of the trie
	// -----------------------------------------------

	int processOptBorder(Trie * complSet, set< Trie >  * optIt, Trie * tr,  Trie * freqTr, vector<unsigned int> * stat );
	
	bool processOptBorder( set<Trie> *items, vector< short unsigned > *itemset, set< Trie > * optIt,
		Trie * complSet, Trie * tr,  Trie * freqTr, vector<unsigned int> * stat );

	int processOptBorder(Trie * complSet, Trie  * opt, Trie * tr,  Trie * freqTr, vector<unsigned int> * stat );
	
	bool processOptBorder( set<Trie> *items, vector< short unsigned > *itemset,Trie * opt,
		Trie * complSet, Trie * tr,  Trie * freqTr, vector<unsigned int> * stat );

	// -------------------------------------------------
	// process the optimist positive border
	// all the frequent update Bd+, the non frequent 
	// with an error < "eps" are stored in optIt and 
	// the others generate the subset of the infrequent
	// itemsets "far" from the positive border.
	// store in stat the number of itemsets by level
	// stat[0] is total number of itemsets of the trie
	// -------------------------------------------------

	int processOptGenSub(Trie * complSet, Trie  * opt, Trie * tr, Trie * freqTr,
							Trie * subSet,  int size, vector<unsigned int> * stat );
	
	bool processOptGenSub( set<Trie> *items, vector< short unsigned > *itemset, Trie * opt,
		Trie * complSet, Trie * tr,  Trie * freqTr ,  Trie * subSet, int size, vector<unsigned int> * stat);

	// -----------------------------------------
	// method that move a transversal that have
	// "generated" a frequent itemset "itemset"
	// into a set of itemset (freqTr) from 
	// the original set of transversal (tr)
	// -----------------------------------------

	void moveTransFreq( vector< short unsigned > *itemset, Trie * tr,  Trie * freqTr ) ;
		
	//-----------------------------------------
	// method updating the positive border
	// with the itemsets of bdPopt and Bdp
	// found by apriori
	//-----------------------------------------

	void updateBdP() ;

	// ----------------------------------------
	// method processing the optimist approach
	// ie from a set of itemset in parameter
	// it found all the maximals frquents 
	// itemsets of the sub lattice 
	// and save the min non frequent, 
	// until the level lvl
	// ----------------------------------------

	void optApproach( set< Trie > * optIt, TATREE * tat, int lvl, vector< unsigned int > * stat ) ;

	void optApproach( set< Trie > * items,  vector< short unsigned > * itemset, Trie * nextLev, vector< unsigned int > * stat ) ;

	void optApproach( Trie * opt, TATREE * tat, int lvl, vector< unsigned int > * stat ) ;

	void initOptApproach( set< Trie > * items,  vector< short unsigned > * itemset, set< Trie > * optIt ) ;

	// -------------------------------
	// prune the infrequent itemset 
	// insert the non frequent in Bd-
	// and update Bd+ of the pess 
	// approach
	// (used in the pess approach)
	// -------------------------------

	int pruneCandidates(Trie * tr, int level);

	int pruneCandidates( int level, set<Trie> *items, int depth, short unsigned *itemset);

	// --------------------------------------------------------------
	// prune the infrequent candidates itemsets, for each infrequent 
	// itemset update/process the transversal min, for each frequent
	// update Bd+ of the pessimist approach
	// --------------------------------------------------------------

	int processCandTrans(Trie * tr, int level, Trie * transv, int n, int lvl, Trie * freqTr);

	int processCandTrans( int level, set<Trie> *items, int depth, vector< unsigned short> *itemset,
						  Trie * transv, int n, int lvl, Trie * freqTr);

	// --------------------------------------------------------
	// prune the frequent itemset without knowing the level
	// for the optimist approach since we go through from the 
	// high level itemsets ie it update the positive border 
	// for this approach
	// return the number of frequent found
	// --------------------------------------------------------

	int pruneCandidatesOpt( Trie * tr, vector< unsigned int > * stat   );
	
	int pruneCandidatesOpt( set<Trie> *items, vector< short unsigned > *itemset, vector< unsigned int > * stat  );
		
	// --------------------------------
	// method that calcul the distance
	// form the positive border
	// --------------------------------

	double error(int supp, int minSup ) ;
 
	// -----------------------------------------------------------------------
	// method that prune the candidates generated from the itemsets found far 
	// from the positive border (stored in tr), ie it delete from all the 
	// subsets of these itemsets at the level k+1, the itemsets included in an
	// itemset of bd
	// -----------------------------------------------------------------------

	void pruneCandBottomUp( Trie * tr, Trie * bd, Trie * opt )  ;
	
	bool pruneCandBottomUp( set< Trie > * items, Trie * bd, Trie * opt, vector< short unsigned > * vect, Trie * tr ) ;

	// ----------------------------------------------------------------------------------
	// method that generates the candidates at the level k+1
	// this method delete all the subsets included into one itemset of bdp
	// subSet : trie with all the subset of size k+1 of the infrequent found by the jump
	//  bdp : set of itemsets of bd+ or near bd+ found by jumps
	// ----------------------------------------------------------------------------------

	void genCand( Trie * subSet, Trie * inbdp ) ;

	
	bool genCand( Trie * subSet, set<Trie> * childbdp, Trie * tr ) ;
};

#endif
