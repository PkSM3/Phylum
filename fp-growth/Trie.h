/*----------------------------------------------------------------------
  File    : Trie.h
  Contents: Class that represent a prefix trie. It is used to represent
            itemsets. Methods to manipulate the trie are given.
            The incremental calculation of the transversal minimal
            of an hypergraph wrt the caracterization
            of Demetrovics et Thi is implemented here
  Author  : Frédéric Flouvat
----------------------------------------------------------------------*/

#ifndef TRIE_H
#define TRIE_H

#include <set>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>

using namespace std;

class Trie
{
protected:
    
    // -----------------------------------
    // identifier of the item in the node
    // -----------------------------------

	short unsigned id;
	
	// --------------------------------
	// support of the current itemset
	// for the root of the tree this 
    // is the number of itemset of the
    // set
	// --------------------------------
	
    int unsigned support;	

    // -----------------------------------------
    // set storing the child nodes of this node    
    // in the prefix tree
    // -----------------------------------------
        
	set<Trie> *children;

public:

	Trie(short unsigned i=-1) : id(i), support(0), children(0) {}
	Trie(short unsigned i, unsigned int supp) : id(i), support(supp), children(0) {}
	Trie(const Trie &i) : id(i.id), support(i.support), children(i.children) {}
	~Trie(){}

	// ---------------------
	// affectation operator
	// ---------------------

	Trie & operator = ( const Trie & inTrie ) ;

	// --------------------------------
	// method returning the attributes
	// --------------------------------
	
	short unsigned getId() const {return id;}
	
	unsigned int getSupport() const {return support;}

	set<Trie> *getChildren() const {return children;}

	// -------------------------------------------------
	// method that increment the support of the itemset
	// represented by the path until that node
	// -------------------------------------------------

	int Increment( unsigned int inc = 1) {return support+=inc;}

	// -------------------------------------------------
	// method that decrement the support of the itemset
	// represented by the path until that node
	// -------------------------------------------------
	
	int Decrement( unsigned int dec = 1) {return support-=dec;}

	// ------------------------------------------
	// method that allocate a set of child nodes
	// ------------------------------------------
		
	set<Trie> *makeChildren();

	// ---------------------------------------------------------
	// method that delete recursively the sub tree of this node
	// ---------------------------------------------------------

	int deleteChildren();

	// ------------------------------------------------------------------
	// Operator < to have a prefix ordered wrt their internal identifier
	// ie increasing support 
	// ------------------------------------------------------------------
		
	bool operator<(const Trie &i) const{return id < i.id;}

	// ------------------------------
	// method that initialize the id 
	// ------------------------------

	void setId( short unsigned inId ) { id = inId ; }  

	// --------------------------------
	// method that initialize children 
	// --------------------------------

	void setChildren( set<Trie> * inChild ) { children = inChild ; }  

	// -----------------------------------
	// method that initialize the support 
	// -----------------------------------

	void setSupport( unsigned int inSup ) { support = inSup ; }  

	// -----------------------------------
	// method searching if one itemset of 
	// items is included into itemset
	// -----------------------------------

	bool includedIn( short unsigned * itemset, int level ) ;

	bool includedIn( set<Trie> * items, short unsigned * itemset, int level, int spos = 0 ) ;

	
	bool includedIn( vector< short unsigned > * itemset ) ;

	bool includedIn( set<Trie> * items, vector< short unsigned > * itemset, int spos = 0 ) ;


	bool includedIn( list< short unsigned > * itemset ) ;

	bool includedIn( set<Trie> * items, list< short unsigned > * itemset, list< short unsigned >::iterator spos ) ;

	// ----------------------------------------------
	// method searching if itemset is include in one
	// of the itemsets of this set of itemset
	// ----------------------------------------------
	
	bool include( vector< short unsigned > * itemset ){ return include( children, itemset, 0 ) ; } ;

	bool include( set<Trie> * items, vector< short unsigned > * itemset, int spos = 0 ) ;


	bool include( short unsigned * itemset, int size ){ return include( children, itemset, size, 0 ) ; } ;

	bool include( set<Trie> * items, short unsigned * itemset, int size, int spos = 0 ) ;

	// ---------------------------------------------------
	// print to screen all the itemset stored in tree
	// remap is a table for mapping the name of the items
	// if remap is 0 it use the internal id
	// ---------------------------------------------------

	void printItemsets( short unsigned * remap = 0 ) ;

	void printItemsets( short unsigned * remap, set< Trie > * items, vector< short unsigned > * vect, int inSup = 0 ) ;

	// ---------------------------------------------------
	// save into a file all the itemset stored in tree
	// remap is a table for mapping the name of the items
	// if remap is 0 it use the internal id
	// ---------------------------------------------------

	void saveItemsets( const char * fileName, short unsigned * remap = 0 ) ;

	void saveItemsets( ofstream & pFile, short unsigned * remap , set< Trie > * items, vector< short unsigned > * vect
		, int inSup = 0) ;

	// ---------------------------------------------------
	// save into a file all the itemset stored in tree
	// remap is a table for mapping the name of the items
	// according the FIMI format of datasets 
	// ---------------------------------------------------

	void saveDataSet( const char * fileName, short unsigned * remap ) ;

	void saveDataSet( ofstream & pFile, short unsigned * remap, set< Trie > * items, list< unsigned >  * lst ) ;

	// ----------------------------------------------------------------
	// method which construct 
	// Tr(i+1) = Si with Tr mininmal transversal
	//				and Si = { X is in Tr(i) | X intersect Ei+1 != 0 }
	//				and H = { E1,..., En } an hypergraph
	// and Bi with Bi = { X is in Tr(i) | X intersect Ei+1 == 0 }
	// Note: here Tr(i) is this, Ei+1 is itemset, level is the size
	//       of Ei+1, bi is Bi  
	// ----------------------------------------------------------------

	void initTrBi( short unsigned * itemset, int level,  Trie * bi ) ;

	void initTrBi( vector< short unsigned > * itemset,  Trie * bi ) ;

	bool initTrBi( set< Trie > * items, short unsigned * itemset, int level,Trie * bi,
		vector< short unsigned > * vect) ;

	bool initTrBi( set< Trie > * items, vector< short unsigned > * itemset,Trie * bi,
			vector< short unsigned > * vect) ;

	// ----------------------------------------------------------------
	// method which construct 
	// Tr(i+1) = Si with Tr mininmal transversal
	//				and Si = { X is in Tr(i) | X intersect Ei+1 != 0 }
	//				and H = { E1,..., En } an hypergraph
	// and Bi with Bi = { X is in Tr(i) | X intersect Ei+1 == 0 }
	// Note: here Tr(i) is this, Ei+1 is itemset, level is the size
	//       of Ei+1, bi is Bi  
	//  this method is optimised for our problem since it don't add
	//  to Bi the transversal of cardinality >= n-k-1 
	//                        with n the number of items and
	//                        k the level of the levelwise approach  
	// ----------------------------------------------------------------

	void initTrBiOpt( vector< short unsigned > * itemset,  Trie * bi, int n, int k ) ;

	bool initTrBiOpt( set< Trie > * items, vector< short unsigned > * itemset,Trie * bi,
		vector< short unsigned > * vect, bool intersect, int n, int k ) ;

	// ------------------------------------------------------
	// method which insert an itemset in this set of itemset
	// NOTE : be careful this method increment the support 
	//        of the objets Trie on which it is applied
	//		  since it must be a root of a trie and in this
	//        case the support is the number of itemset in
	//        the trie
	// ------------------------------------------------------

	void insert( short unsigned * itemset, int level, int inSup = 0 ) ;

	void insert( vector< short unsigned > * itemset, int inSup = 0 ) ;

	void insert( list< short unsigned > * itemset, int inSup = 0 ) ;

	// ------------------------------------------------------
	// method which delete an itemset in this set of itemset
	// NOTE : be careful this method decrement the support 
	//        of the objets Trie on which it is applied
	//		  since it must be a root of a trie and in this
	//        case the support is the number of itemset in
	//        the trie
	// ------------------------------------------------------

	void deleteIt( vector< short unsigned > * itemset ) ;

	int deleteIt( vector< short unsigned > * itemset, set< Trie > * items, int pos=0 ) ;
	
	// ----------------------------------------------------
	// method generating the new itemsets for Tr(i+1) with 
	// Tr(i), Bi, the new edge itemset
	// ----------------------------------------------------

	Trie * newItTr( Trie * bi, short unsigned * itemset, int level ) ;

	void newItTr( set< Trie > * trs, set< Trie > * bis, short unsigned * itemset, int level,
						  list< short unsigned > * lst, Trie * res ) ;
	
	// ----------------------------------------------------
	// method generating the new itemsets for Tr(i+1) with 
	// Tr(i), Bi, the new edge itemset
	// this method is optimised for our problem since it 
	// enable the separation of the minimal transversals
	// that "generate" frequent already tested from the 
	// others, without affecting the process of the 
	// minimals transversals
	// ----------------------------------------------------

	Trie * newItTr( Trie * bi, vector< short unsigned > * itemset, Trie * freqTr = 0 ) ;

	void newItTr( set< Trie > * trs, set< Trie > * bis, vector< short unsigned > * itemset,
						  list< short unsigned > * lst, Trie * res, set< Trie > * freqTrChild = 0 ) ;


	// -------------------------------------------------
	// method merging this sets of itemset 
	// and trie2 into this set 
	// note: the set trie2 is destructed after the copy
	// -------------------------------------------------

	void merge( Trie * trie2 ) ;

	void merge( set< Trie > * trieC1, set< Trie > * trieC2 ) ;

	// -----------------------------------
	// method that delete all the itemset
	// of size < "level"
	// -----------------------------------

	int pruneNodes(int level);

	int pruneNodes(int level, set<Trie> *items, int depth);

	// -----------------------------------------------------------
	// method which calcul the minimal transversal with i+1 edges
	// from the minimal transversal with i edges ( this set ) 
	// and a new edge ( Demetrovics et Thi approach )
	// -----------------------------------------------------------

	void trMinI( short unsigned * itemset, int level );

	void trMinI( vector< short unsigned > * itemset );

	// -------------------------------------------------------------
	// method which calcul the minimal transversal with i+1 edges
	// from the minimal transversal with i edges ( this set ) 
	// and a new edge ( Demetrovics et Thi approach )
	//  this method is optimised for our problem since it delete 
	//  all the transversal of cardinality >= n-k 
	//                        with n the number of items and
	//                        inActLev the actual level of the
	//							levelwise approach  
	// it also enable the separation of the minimal transversals
	// that "generate" frequent already tested from the 
	// others, without affecting the process of the 
	// minimals transversals
	// -------------------------------------------------------------

	void trMinIopt( vector< short unsigned > * itemset, int n, int k, Trie * freqTr=0  );

	// -------------------------------------------------------------
	// method which calcul the minimal transversal with the edges
	// stored in the set in parameter
	// from the minimal transversal with i edges ( this set ) 
	// and a new edge ( Demetrovics et Thi approach )
	//  this method is optimised for our problem since it delete 
	//  all the transversal of cardinality >= n-k 
	//                        with n the number of items and
	//                        inActLev the actual level of the
	//							levelwise approach  
	// it also enable the separation of the minimal transversals
	// that "generate" frequent already tested from the 
	// others, without affecting the process of the 
	// minimals transversals
	// -------------------------------------------------------------

	void trMinIopt( Trie * tr, int n, int k, Trie * freqTr=0  ) ;

	void trMinIopt( set< Trie > * items, vector< short unsigned > * vect, int n, int k, Trie * freqTr  ) ;

	// -----------------------------------------------------------
	// method which calcul the minimal transversal of this set of
	// itemset
	// -----------------------------------------------------------

	Trie * trMin();

	// ------------------------------------------------------------------------
	// method which calcul incrementaly the minimal transversal with the edges
	// stored in the set in parameter
	// from the minimal transversal with i edges ( this set ) 
	// and a new edge ( Demetrovics et Thi approach )
	// ------------------------------------------------------------------------
	
	void trMinI( Trie * tr ) ;

	void trMin( set< Trie > * items, vector< short unsigned > * vect, Trie * res ) ;

	// -----------------------------------------------------------
	// method which calcul the minimal transversal of this set of
	// itemset
	//  this method is optimised for our problem since it delete 
	//  all the transversal of cardinality >= n-k 
	//                        with n the number of items and
	//                        inActLev the actual level of the
	//							levelwise approach  
	// it also enable the separation of the minimal transversals
	// that "generate" frequent already tested from the 
	// others, without affecting the process of the 
	// minimals transversals
	// -----------------------------------------------------------

	Trie * trMinOpt( int n, int k );

	void trMinOpt( set< Trie > * items, vector< short unsigned > * vect, Trie * res, int n, int k ) ;


	// -----------------------------------------------------------------
	// method that construct the complementary of this set of itemset
	// ( res ) since we have all the items ordered and with the internal
	// id begining at 1 to the number of items in the database
	// we only have to use the total number of items to find the
	// complementary
	// -----------------------------------------------------------------

	Trie * complem( int nb ) ;

	void complRec( int nb, set< Trie > * items, vector< short unsigned > * vect, Trie * res ) ; 

	// -------------------------------------
	// method that proces the complement of 
	// an itemset in whom the items are
	// between 1 an nb
	// -------------------------------------

	vector< short unsigned > * complem( vector< short unsigned > * vect, int nb ) ;
	
	// -----------------------------------
	// method that delete all the subsets
	// of items of size "size" and return 
	// the number of subsets deleted
	// -----------------------------------

	int delSubsets( short unsigned *iset, int level, int size) 
		{ return delSubsets( iset, level, children, 0, level - size ) ;  }

	// -------------------------------------
	// method that delete all the subsets
	// of items of size "level-1" (default)  
	// return the number of subsets deleted
	// -------------------------------------

	int delSubsets( short unsigned *iset, int level, set<Trie> *items, int spos = 0, int depth = 1) ;
	
	// -----------------------------------
	// method that delete all the subsets
	// of items of size "size" and return 
	// the number of subsets deleted
	// without knowing the level of iset
	// -----------------------------------

	int delSubsets( vector< short unsigned > * iset, int size) 
		{ return delSubsets( iset, children, 0, iset->size() - size ) ;  }

	// -------------------------------------
	// method that delete all the subsets
	// of items of size "level-1" (default)  
	// return the number of subsets deleted
	// without knowing the level of iset
	// -------------------------------------

	int delSubsets( vector< short unsigned > * iset, set<Trie> *items, int spos = 0, int depth = 1); 

	// ------------------------------------------
	// method that generate into this set 
	// the subsets of size "size" of an itemsets 
	// Note: items must be already alocated
	// -------------------------------------------

	void genSubsets(  vector< short unsigned > * itemset, int size )
	{ genSubsets( itemset, makeChildren(), 0, itemset->size() - size ); }

	void genSubsets(  vector< short unsigned > * itemset, set<Trie> *items, int spos = 0, int depth = 1) ;

	// ------------------------------------------------
	// method that return a set of the subsets of size
	// "size" of this set of itemsets 
	// ------------------------------------------------

	Trie * genSubsets( int size ) ;

	void genSubsets( int size, vector< short unsigned > * itemset, set< Trie > * items, Trie * subset ) ;

};

#endif
