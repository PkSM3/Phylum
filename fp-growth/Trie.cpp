/*----------------------------------------------------------------------
  File    : Trie.cpp
  Contents: Class that represent a prefix trie. It is used to represent
            itemsets. Methods to manipulate the trie are given.
            The incremental calculation of the transversal minimal
            of an hypergraph wrt the caracterization
            of Demetrovics et Thi is implemented here
  Author  : Frédéric Flouvat
----------------------------------------------------------------------*/

#include "Trie.h"

/**
*  method that allocate a set of child nodes
*/

set<Trie> *Trie::makeChildren() 
{
	if(children) return children;
	return children = new set<Trie>;
}

// ---------------------------------------------------------------------------------------------- 

/**
*  method that delete recursively the sub tree of this node
*/

int Trie::deleteChildren() 
{
	int deleted=0;

	if(children)
	{
		for(set<Trie>::iterator it = children->begin(); it != children->end(); it++)
		{
		    Trie * tmpT = const_cast< Trie * >( &(*it) ) ;
			deleted += tmpT->deleteChildren();
		}
		delete children;
		children = 0;
		deleted++;
	}

	support = 0;

	return deleted;
}

// ---------------------------------------------------------------------------------------------- 

/**
*  affectation operator
*/

Trie & Trie::operator = ( const Trie & inTrie ) 
{
	if( this != &inTrie )
	{
		if( children ) 
			deleteChildren() ;

		id =  inTrie.id ;

		support = inTrie.support ;

		children = inTrie.children ;

	}

	return *this ;

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method searching if one itemset of 
*  this set is included into itemset
*/

bool Trie::includedIn( short unsigned * itemset, int level ) 
{

	return includedIn( children, itemset, level, 0 ) ;

}

bool Trie::includedIn( vector< short unsigned > * itemset ) 
{

	return includedIn( children, itemset, 0 ) ;

}

bool Trie::includedIn( list< short unsigned > * itemset ) 
{

	return includedIn( children, itemset, itemset->begin() ) ;

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method searching if one itemset of 
*  items is included into itemset
*  be carefull itemset MUST BE ordered
*/

bool Trie::includedIn( set<Trie> * items, short unsigned * itemset, int level, int spos ) 
{
	bool res = false ; // result of the inclusion test
	
	bool find ;

	int tmp ;

	set< Trie >::iterator runner ;

	if( items != 0 )
	{
		runner = items->begin() ;

		while( ( res == false ) && ( runner != items->end() ) )
		{

			// test if the id of the current element
			// is included into itemset 
			// without rechecking the passed items 
			// since the itemsets is ordered

			tmp = spos ; // we save the current pos for all the childrens
						 // in the case that one of this item is not
						 // equal to an item of itemset

			find = false ;

			while( ( find == false ) && ( spos < level ) )
			{

				if( runner->getId() == itemset[ spos ] )
					find = true ;

				spos++ ; 
			}

			if( find == true )
				res = includedIn( runner->getChildren(), itemset, level, spos ) ;

			else
			{
				spos = tmp ; ;
				res = false ;
			}

			runner++ ;

		}

	}
	else 
		// we have check all the item of the current itemset
		// of this set
		res = true ;

	return res ;

}

bool Trie::includedIn( set<Trie> * items, vector< short unsigned > * itemset, int spos ) 
{
	bool res = false ; // result of the inclusion test
	
	bool find ;
	
	int tmp ;

	set< Trie >::iterator runner ;

	if( items != 0 )
	{
		runner = items->begin() ;

		while( ( res == false ) && ( runner != items->end() ) )
		{

			// test if the id of the current element
			// is included into itemset 
			// without rechecking the passed items 
			// since the itemsets is ordered

			find = false ;

			tmp = spos ; // we save the current pos for all the childrens
						 // in the case that one of this item is not
						 // equal to an item of itemset

			while( ( find == false ) && ( spos < itemset->size() ) )
			{
				if( runner->getId() == (*itemset)[spos ] )
					find = true ;

				spos++ ; 
			}

			if( find == true )
				res = includedIn( runner->getChildren(), itemset, spos ) ;

			else
			{
				spos = tmp ;
				res = false ;
			}

			runner++ ;

		}

	}
	else 
		// we have check all the item of the current itemset
		// of this set
		res = true ;

	return res ;

}

bool Trie::includedIn( set<Trie> * items, list< short unsigned > * itemset, list< short unsigned >::iterator spos ) 
{
	bool res = false ; // result of the inclusion test
	
	bool find ;
	
	list< short unsigned >::iterator tmp ;

	set< Trie >::iterator runner ;

	if( items != 0 )
	{
		runner = items->begin() ;

		while( ( res == false ) && ( runner != items->end() ) )
		{

			// test if the id of the current element
			// is included into itemset 
			// without rechecking the passed items 
			// since the itemsets is ordered

			find = false ;

			tmp = spos ; // we save the current pos for all the childrens
						 // in the case that one of this item is not
						 // equal to an item of itemset

			while( ( find == false ) && ( spos != itemset->end() ) )
			{
				if( runner->getId() == *spos  )
					find = true ;

				spos++ ; 
			}

			if( find == true )
				res = includedIn( runner->getChildren(), itemset, spos ) ;

			else
			{
				spos = tmp ;
				res = false ;
			}

			runner++ ;

		}

	}
	else 
		// we have check all the item of the current itemset
		// of this set
		res = true ;

	return res ;

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method searching if itemset is include in one
*  of the itemsets of this set of itemset
*/

bool Trie::include( set<Trie> * items, vector< short unsigned > * itemset, int spos ) 
{

	bool res = false ; // result of the inclusion test
	
	set< Trie >::iterator runner ;

	if( ( items != 0 ) && ( spos != itemset->size() ) )
	{
		runner = items->begin() ;

		while( ( res == false ) && ( runner != items->end() ) 
			 
				&& ( runner->getId() <= (*itemset)[ spos ] ) )
		{

			// test if the id of the current element
			// is equal to the current to the
			// cuurent element of the itemset 
			// and we check the next item of the current
			// itemset of this set 
			
			if( runner->getId() == (*itemset)[ spos ] ) 								
			
				res = include( runner->getChildren(), itemset, spos + 1 ) ;

			else
				res = include( runner->getChildren(), itemset, spos ) ;
			
			runner++ ;

		}

	}
	else 
		// we have check all the item of the current itemset
		// of this set or we have found all the items if itemset
		
		if ( spos == itemset->size() )
			res = true ;

	return res ;

}




bool Trie::include( set<Trie> * items, short unsigned * itemset, int size, int spos ) 
{

	bool res = false ; // result of the inclusion test
	
	set< Trie >::iterator runner ;

	if( ( items != 0 ) && ( spos != size ) )
	{
		runner = items->begin() ;

		while( ( res == false ) && ( runner != items->end() ) 
			 
				&& ( runner->getId() <= itemset[ spos ] ) )
		{

			// test if the id of the current element
			// is equal to the current to the
			// cuurent element of the itemset 
			// and we check the next item of the current
			// itemset of this set 
			
			if( runner->getId() == itemset[ spos ] )								
			
				res = include( runner->getChildren(), itemset, size, spos + 1 ) ;

			else
				res = include( runner->getChildren(), itemset, size, spos ) ;
			
			runner++ ;

		}

	}
	else 
		// we have check all the item of the current itemset
		// of this set or we have found all the items if itemset
		
		if ( spos == size )
			res = true ;

	return res ;

}

// ---------------------------------------------------------------------------------------------- 

/**
*  print to screen all the itemset stored in tree
*  remap is a table for mapping the name of the items
*/

void Trie::printItemsets( short unsigned * remap )  
{
	vector< short unsigned > * vect ;

	vect = new vector<short unsigned>() ;

	if( children != 0 )
		printItemsets( remap, children , vect ) ;
	
	delete vect ;
}

void Trie::printItemsets( short unsigned * remap, set< Trie > * items, vector< short unsigned > * vect, int inSup ) 
{

	if( ( items != 0 ) && ( !items->empty() ) )
	{

		for(set< Trie >::iterator runner = items->begin(); runner != items->end(); runner++ )
		{
			
			// we save the curent item of the itemset being processed

			vect->push_back( runner->getId() ) ;
			
			inSup = runner->getSupport() ;

			// we continue searching with the next items

			printItemsets( remap, runner->getChildren(), vect, inSup ) ;

			vect->pop_back() ;
				
		}

	}
	else	// we have check all the items of the current itemset strored in vect
			// so we print to screen
	{
		
		for( int j = 0; j < vect->size() ; j++ )
		{
			if(remap)  
				cout<< remap[ (*vect)[ j ] ]<<" " ; 
			else 
				cout<< (*vect)[ j ]<<" " ;  
	
		}
		cout<<" size ( "<<vect->size() <<" ) " ;
		cout<<"  support ( "<< inSup<<" )"<<endl ;		

	}

}

// ---------------------------------------------------------------------------------------------- 

/**
*  save into a file all the itemset stored in tree
*  remap is a table for mapping the name of the items
*  if remap is 0 it use the internal id
*/

void Trie::saveItemsets( const char * fileName, short unsigned * remap ) 
{

	ofstream pFile( fileName ) ;

	vector< short unsigned > * vect ;

	vect = new vector<short unsigned>() ;

	if( pFile.is_open() )
	{
		if( children != 0 )
			saveItemsets( pFile, remap, children, vect) ;

		pFile.close()  ;
	}

	delete vect ;

}

void Trie::saveItemsets( ofstream & pFile, short unsigned * remap, set< Trie > * items, vector< short unsigned > * vect
						, int inSup ) 
{

	if( ( items != 0 ) && ( !items->empty() ) )
	{

		for(set< Trie >::iterator runner = items->begin(); runner != items->end(); runner++ )
		{
			
			// we save the curent item of the itemset being processed

			vect->push_back( runner->getId() ) ;

			inSup = runner->getSupport() ;

			// we continue searching with the next items

			saveItemsets( pFile, remap, runner->getChildren(), vect, inSup ) ;

			vect->pop_back() ;
				
		}

	}
	else	// we have check all the items of the current itemset strored in vect
			// so we write into the file
	{
		
		for( int j = 0; j < vect->size() ; j++ )
		{
			if(remap) 
				pFile<< remap[ (*vect)[ j ] ]<<" " ; 
			else 
				pFile<< (*vect)[ j ] <<" " ; 
	
		}
		pFile<<"("<< inSup<<")"<<endl;
	
	
	}

}

// ---------------------------------------------------------------------------------------------- 

/**
*  save into a file all the itemset stored in tree
*  remap is a table for mapping the name of the items
*  according the FIMI format of datasets 
*/

void Trie::saveDataSet( const char * fileName, short unsigned * remap ) 
{

	ofstream pFile( fileName ) ;

	list< unsigned >  * lst ;

	lst = new list<unsigned>() ;

	if( pFile.is_open() )
	{
		if( children != 0 )
			saveDataSet( pFile, remap, children, lst ) ;

		pFile.close()  ;
	}

	delete lst ;

}

void Trie::saveDataSet( ofstream & pFile, short unsigned * remap, set< Trie > * items, list< unsigned >  * lst ) 
{

	if( ( items != 0 ) && ( !items->empty() ) )
	{

		for(set< Trie >::iterator runner = items->begin(); runner != items->end(); runner++ )
		{
			
			// we save the curent item of the itemset being processed

			lst->push_back( remap[ runner->getId() ] ) ;

			// we continue searching with the next items

			saveDataSet( pFile, remap, runner->getChildren(), lst ) ;

			lst->pop_back() ;
				
		}

	}
	else	// we have check all the items of the current itemset strored in vect
			// so we write into the file
	{
		// the itemset are ordered wrt their name
		lst->sort() ;	

		for( list< unsigned >::iterator it = lst->begin(); it != lst->end(); lst++ )
		{
			pFile<< *it <<" " ;
			
		}
		pFile<<endl;

	
	}

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method which insert an itemset in a set of itemset
*/

void Trie::insert( short unsigned * itemset, int level, int inSup ) 
{

	set< Trie > * tmpSet ;

	set< Trie >::iterator runner ;
	
	int i = 0 ;	

	tmpSet = children ;

	// we pass all the items of the itemset to insert
	// that are already stored in itSet

	while( ( tmpSet != 0 ) && ( i < level ) &&
			( ( runner = tmpSet->find( Trie( itemset[ i ] ) ) ) != tmpSet->end() ) )
	{
		tmpSet = runner->getChildren() ;
		i++ ;
	}
	
	// we increment the number of itemsets in this set
	// since we are adding one

	if( i < level )
		support ++ ;

	// insertion of the part of the itemset not already stored in itSet

	for( int j = i; j < level; j++ )
	{
		if( children == 0 )
		{
			children = new set<Trie> ;
			tmpSet = children ;
		}
		else 
			if( tmpSet == 0 )
			{
			    Trie * tmpT = const_cast< Trie * >( &(*runner ) );       			
				tmpSet = tmpT->makeChildren() ;
            }				
		runner = tmpSet->insert( Trie( itemset[ j ], inSup ) ).first ;

		tmpSet =runner->getChildren() ;
	}

}

void Trie::insert( vector< short unsigned > * itemset, int inSup ) 
{

	set< Trie > * tmpSet ;

	set< Trie >::iterator runner ;
	
	int i = 0 ;

	tmpSet = children ;

	// we pass all the items of the itemset to insert
	// that are already stored in itSet

	while( ( tmpSet != 0 ) && ( i < itemset->size() ) &&
			( ( runner = tmpSet->find( Trie( (*itemset)[ i ] ) ) ) != tmpSet->end() ) )
	{
		tmpSet = runner->getChildren() ;
		i++ ;
	}

	// we increment the number of itemsets in this set
	// since we are adding one

	if( i < itemset->size() )
		support ++ ;

	// insertion of the part of the itemset not already stored in itSet

	for( int j = i; j < itemset->size(); j++ )
	{
		if( children == 0 )
		{
			children = new set<Trie> ;
			tmpSet = children ;
		}
		else
			if( tmpSet == 0 )
            {	
                Trie * tmpT = const_cast< Trie * >( &(*runner ) );       			
				tmpSet = tmpT->makeChildren() ;
			}				

		runner = tmpSet->insert( Trie( (*itemset)[ j ] , inSup ) ).first ; 

		tmpSet =runner->getChildren() ;
	}

}

void Trie::insert( list< short unsigned > * itemset, int inSup ) 
{

	set< Trie > * tmpSet ;

	set< Trie >::iterator runner ;
	
	int i = 0 ;

	tmpSet = children ;

	list< short unsigned >::iterator it;

	it = itemset->begin() ;

	// we pass all the items of the itemset to insert
	// that are already stored in itSet

	while( ( tmpSet != 0 ) && ( it != itemset->end() ) &&
			( ( runner = tmpSet->find( Trie( *it ) ) ) != tmpSet->end() ) )
	{
		tmpSet = runner->getChildren() ;
		it++ ;
	}

	// we increment the number of itemsets in this set
	// since we are adding one

	if( i < itemset->size() )
		support ++ ;

	// insertion of the part of the itemset not already stored in itSet

	for( list< short unsigned >::iterator it2 = it; it2 != itemset->end(); it2++ )
	{
		if( children == 0 )
		{
			children = new set<Trie> ;
			tmpSet = children ;
		}
		else
			if( tmpSet == 0 )
            {
                Trie * tmpT = const_cast< Trie * >( &(*runner ) );       						
				tmpSet = tmpT->makeChildren() ;
            }				

		runner = tmpSet->insert( Trie( *it2, inSup ) ).first ;

		tmpSet = runner->getChildren() ;
	}

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method which delete an itemset in a set of itemset
*/

void Trie::deleteIt( vector< short unsigned > * itemset  ) 
{

	deleteIt( itemset, children ) ;	

}

int Trie::deleteIt( vector< short unsigned > * itemset, set< Trie > * items, int pos ) 
{

	set< Trie >::iterator runner ;
	
	
	if( ( items != 0 ) && ( pos < itemset->size() ) )
	{
		// we search for the itemset

		runner = items->find( Trie( (*itemset)[ pos ] ) );
			
		if( runner != items->end() )
		{
			pos = deleteIt( itemset, runner->getChildren(), pos + 1   ) ;

			// we delete the itemset 

			if( ( pos == itemset->size() ) && 
				( ( runner->getChildren() == 0 ) || ( (runner->getChildren())->empty() ) ) )
			{
			    Trie * tmpT = const_cast< Trie * >( &(*runner) ) ;
				tmpT->deleteChildren() ;

				items->erase( runner ) ;
			}

		}

	}
	else
	{
		if( pos == itemset->size() ) // we fave found the itemset

			// we decrement the number of itemsets in this set
			// since we are deleting one

			support -- ;

	}

	return pos ;
	

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method which construct 
*  Tr(i+1) = Si with Tr mininmal transversal
*  				and Si = { X is in Tr(i) | X intersect Ei+1 != 0 }
*  				and H = { E1,..., En } an hypergraph
*  and Bi with Bi = { X is in Tr(i) | X intersect Ei+1 == 0 }
*  Note: here Tr(i) is items, Ei+1 is itemset, level is the size
*        of Ei+1, bi is Bi  
*/  

void Trie::initTrBi( short unsigned * itemset, int level,  Trie * bi ) 
{
	vector< short unsigned > * vect ;

	vect = new vector<short unsigned>() ;

	initTrBi( children, itemset, level, bi, vect ); 

	// number of element of trie now

	support = support - bi->support ;
	
	delete vect ;

}

void Trie::initTrBi( vector< short unsigned > * itemset,  Trie * bi ) 
{
	vector< short unsigned > * vect ;

	vect = new vector<short unsigned>() ;

	initTrBi( children, itemset, bi, vect ); 

	// number of element of trie now

	support = support - bi->support ;
	
	delete vect ;

}

bool Trie::initTrBi( set< Trie > * items, short unsigned * itemset, int level, Trie  * bi, vector< short unsigned > * vect ) 
{

	bool intersect = true ; // variable returning if the intersection is null or not

	bool res ;	// variable used to search an intersection

	int i ;

	set< Trie >::iterator runner ;

	set< Trie >::iterator tmp ;

	if( items != 0 )
	{

		for( runner = items->begin(); runner != items->end(); )
		{
			intersect = true ;
			res = false ;
			i = 0 ;

			// we search if there is an intersection

			while( ( res == false ) && ( i < level ) )
			{
				if( runner->getId() == itemset[ i ] )
					res = true ;

				i++ ;
			}

			if( res == false ) // we continue searching if the next
							   // items of the current motif intersect itemset
			{
				// we save the curent item of the itemset being processed

				vect->push_back( runner->getId() ) ;

				// we continue searching with the next items

				intersect = initTrBi( runner->getChildren(), itemset, level, bi, vect ) ;

				// if there is no intersection 
				// we delete the motif from Tr(i+1)

				if( ( intersect == false ) && 
					( ( runner->getChildren() == 0 ) || ( (runner->getChildren())->empty() ) ) )
				{
					Trie * tmpT = const_cast< Trie * >( &(*runner) ) ;
				tmpT->deleteChildren() ;

					tmp = runner ++ ;

					items->erase( tmp ) ;
				}
				else
					runner++ ;

				vect->pop_back() ;
			}
			else
				runner++ ;

		}

	}
	else	// we have check all the items of the current itemset strored in vect
			// and there is no intersection. so we add this itemset to Bi
	{
		intersect = false ;

		bi->insert( vect ) ;

	}

	return intersect ;
}

bool Trie::initTrBi( set< Trie > * items, vector< short unsigned > * itemset, Trie  * bi, vector< short unsigned > * vect ) 
{

	bool intersect = true ; // variable returning if the intersection is null or not

	bool res ;	// variable used to search an intersection

	int i ;

	set< Trie >::iterator runner ;

	set< Trie >::iterator tmp ;

	if( items != 0 )
	{

		for( runner = items->begin(); runner != items->end(); )
		{
			intersect = true ;
			res = false ;
			i = 0 ;

			// we search if there is an intersection

			while( ( res == false ) && ( i < itemset->size() ) )
			{
				if( runner->getId() == (*itemset)[ i ] ) 
					res = true ;

				i++ ;
			}

			if( res == false ) // we continue searching if the next
							   // items of the current motif intersect itemset
			{
				// we save the curent item of the itemset being processed

				vect->push_back( runner->getId() ) ;

				// we continue searching with the next items

				intersect = initTrBi( runner->getChildren(), itemset, bi, vect ) ;

				// if there is no intersection 
				// we delete the motif from Tr(i+1)

				if( ( intersect == false ) && 
					( ( runner->getChildren() == 0 ) || ( (runner->getChildren())->empty() ) ) )
				{
					Trie * tmpT = const_cast< Trie * >( &(*runner) ) ;
					tmpT->deleteChildren() ;

					tmp = runner ++ ;

					items->erase( tmp ) ;
				}
				else
					runner++ ;

				vect->pop_back() ;
			}
			else
				runner++ ;

		}

	}
	else	// we have check all the items of the current itemset strored in vect
			// and there is no intersection. so we add this itemset to Bi
	{
		intersect = false ;

		bi->insert( vect ) ;

	}

	return intersect ;
}

// ---------------------------------------------------------------------------------------------- 

/**
*  method which construct 
*  Tr(i+1) = Si with Tr mininmal transversal
*  				and Si = { X is in Tr(i) | X intersect Ei+1 != 0 }
*  				and H = { E1,..., En } an hypergraph
*  and Bi with Bi = { X is in Tr(i) | X intersect Ei+1 == 0 }
*  Note: here Tr(i) is items, Ei+1 is itemset, level is the size
*        of Ei+1, bi is Bi  
*  this method is optimised for our problem since it don't add
*  to Bi the transversal of cardinality >= n-k-1 
*                        with n the number of items and
*                        k the actual level of the levelwise approach  
*/

void Trie::initTrBiOpt( vector< short unsigned > * itemset,  Trie * bi, int n, int k  )
{
	vector< short unsigned > * vect ;

	bool intersect = false ; // variable returning if the intersection is null or not

	vect = new vector<short unsigned>() ;

	initTrBiOpt( children, itemset, bi, vect, intersect, n, k ); 
	
	delete vect ;

}


bool Trie::initTrBiOpt( set< Trie > * items, vector< short unsigned > * itemset,Trie * bi,
					   vector< short unsigned > * vect, bool intersect,  int n, int k )
{

	bool oldInter ;

	int i ;

	set< Trie >::iterator runner ;

	set< Trie >::iterator tmp ;

	if( ( items != 0 ) && ( vect->size() < n-k ) )
	{

		oldInter = intersect; 

		for( runner = items->begin(); runner != items->end(); )
		{			

			intersect = oldInter ;

			i = 0 ;

			// we search if there is an intersection

			while( ( i < itemset->size() ) && ( runner->getId() >= (*itemset)[ i ] ) ) 
			{
				if( runner->getId() == (*itemset)[ i ] ) 
					intersect = true ;

				i++ ;
			}
			
			// we save the curent item of the itemset being processed

			vect->push_back( runner->getId() ) ;
					
			// we continue searching with the next items

			intersect = initTrBiOpt( runner->getChildren(), itemset, bi, vect, intersect, n, k ) ;

			// if there is no intersection 
			// we delete the motif from Tr(i+1)

			if( ( intersect == false ) && 
				( ( runner->getChildren() == 0 ) || ( (runner->getChildren())->empty() ) ) )
			{
				Trie * tmpT = const_cast< Trie * >( &(*runner) ) ;
				tmpT->deleteChildren() ;

				tmp = runner ++ ;

				items->erase( tmp ) ;
			}
			else
				runner++ ;

			vect->pop_back() ;

		

		}
		
	

	}
	else	// we have check all the items of the current itemset strored in vect
			// and there is no intersection. so we add this itemset to Bi
	{
		
		if ( vect->size() >= n-k )
		{
			intersect = false ;
			support--;
		}
		else
		{
			if(   intersect == false ) 
			{
			    if( vect->size() < n-k-1 ) 
						bi->insert( vect ) ;

					support--;
				}

		}
		

	}

	return intersect ;
}


// ---------------------------------------------------------------------------------------------- 

/**
*  method generating the new itemsets for Tr(i+1) with 
*  Tr(i), Bi, the new edge itemset
*/

Trie * Trie::newItTr( Trie * bi, short unsigned * itemset, int level ) 
{	

	Trie * res ;

	list< short unsigned > * lst ;
		
	res = new Trie( 0 ) ;

	if( bi->getChildren() != 0 )
	{
		lst = new list< short unsigned >() ;

		newItTr( children, bi->getChildren(), itemset, level, lst, res ) ;

		delete lst ;
	}

	return res ;
	
}

void Trie::newItTr( set< Trie > * trs, set< Trie > * bis, short unsigned * itemset, int level,
						  list< short unsigned > * lst, Trie * res ) 
{

	set< Trie >::iterator runner, tmp ;

	list< short unsigned >::iterator tmpl ;

	
	if( bis != 0 ) // we go throw the tree to an itemset
	{

		for( runner = bis->begin(); runner != bis->end();  )
		{		
			lst->push_back( runner->getId() ) ;

			newItTr( trs, runner->getChildren() , itemset, level , lst, res ) ;

			lst->pop_back() ;

			tmp = runner ++ ;

			bis->erase( tmp ) ;

		}

		delete bis;

	}
	else // we have one itemset of Bi	
	{

		for( int i = 0; i < level ; i++ )
		{
			// we generate a new candidate which might be in the minimal transversal

			lst->push_back( itemset[ i ] ) ;

			// we keep an iterator on the item that
			// we have just inserted

			tmpl = lst->end() ;
			tmpl-- ;

			lst->sort() ;

			// we test that all the itemsets of Si are not inluded into our candidate

			if ( includedIn(trs, lst, lst->begin() ) == false )
			{
				res->insert( lst ) ;
			}

			lst->erase( tmpl ) ;

		}

	}

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method generating the new itemsets for Tr(i+1) with 
*  Tr(i), Bi, the new edge itemset
*  this method is optimised for our problem since it 
*  enable the separation of the minimal transversals
*  that "generate" frequent already tested ("freqTr") from the 
*  others, without affecting the process of the 
*  minimals transversals
*/

Trie * Trie::newItTr( Trie * bi, vector< short unsigned > * itemset, Trie * freqTr ) 
{
	Trie * res ;

	list< short unsigned > * lst ;

	res = new Trie( 0 ) ;

	if( bi->getChildren() != 0 )
	{
		lst = new list< short unsigned >() ;

		if( freqTr != 0 )
			newItTr( children, bi->getChildren(), itemset, lst, res, freqTr->getChildren() ) ;
		else
			newItTr( children, bi->getChildren(), itemset, lst, res ) ;


		delete lst ;
		
	}
	

	return res ;	

}

void  Trie::newItTr( set< Trie > * trs, set< Trie > * bis, vector< short unsigned > * itemset,
						  list< short unsigned > * lst, Trie * res, set< Trie > * freqTrChild ) 
{
	
	set< Trie >::iterator runner,tmp ;

	list< short unsigned >::iterator tmpl ;

	
	if( bis != 0 ) // we go throw the tree to an itemset
	{

		for( runner = bis->begin(); runner != bis->end(); )
		{		
			lst->push_back( runner->getId() ) ;

			newItTr( trs, runner->getChildren() , itemset , lst, res, freqTrChild ) ;

			lst->pop_back() ;

			tmp = runner ++ ;

			bis->erase( tmp ) ;

		}

		delete bis;

	}
	else // we have one itemset of Bi	
	{

		for( int i = 0; i < itemset->size() ; i++ )
		{
			// we generate a new candidate which might be in the minimal transversal

			lst->push_back( (*itemset)[ i ] ) ;

			// we save the place of the item that
			// we have just inserted

			tmpl = lst->end() ;
			tmpl-- ;

			lst->sort() ;

			// we test that all the itemsets of Si are not inluded into our candidate

			if ( ( includedIn( trs, lst, lst->begin() ) == false )
				 && ( ( freqTrChild == 0 ) || ( freqTrChild->empty() ) ||
				      ( includedIn( freqTrChild, lst, lst->begin() ) == false ) ) )
			{

				res->insert( lst ) ;
			}

			lst->erase( tmpl ) ;

		}

	}

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method merging the sets of itemset trie1
*  and trie2 into the set trie 1 ( this )
*  note: the set trie2 is destructed after the copy,
*  the itemsets of trie1 are not included in trie2
*  and the itemsets of trie2 are not included 
*  in trie1
*/

void Trie::merge( Trie * trie2 ) 
{

	support = support + trie2->support ;

	merge( children, trie2->getChildren() ) ;

	trie2->setChildren( 0 ) ;
			
}

void Trie::merge( set< Trie > * trieC1, set< Trie > * trieC2 ) 
{
	
	set< Trie >::iterator runner1 ;

	set< Trie >::iterator runner2 ;

	set< Trie >::iterator tmp ;	

	// we go throw the tree trie2 to an item that is not stored in trie1

	if( ( trieC1 != 0 ) && ( trieC2 != 0  ) )
	{

		// we go throw all the childrens of trie1 

		for( runner2 = trieC2->begin() ; runner2 != trieC2->end();  )
		{		
			
			// we check if the current item is already in trie1

			if( ( runner1 = trieC1->find( Trie( runner2->getId() ) ) ) != trieC1->end() )
			{
								
				merge( runner1->getChildren(), runner2->getChildren() ) ;
			
			}
			else // we have to insert the current item in trie1
			{
				// we insert this item not included in trieC1 and all these chilrens 
				// in trie1, ie we insert the rest of the current itemset that is not 
				// included in trie1, in trie2 				

				trieC1->insert( *runner2 ) ;
				
			}

			tmp = runner2 ++ ;

			trieC2->erase( tmp ) ;

		}

		delete trieC2 ;
		trieC2 = 0 ;
		

	}
	else
		if( trieC2 != 0)
		{
			trieC1 = trieC2 ;
			trieC2 = 0 ;
		}
	

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method that delete all the itemset
*  of size < "level"
*/

int Trie::pruneNodes(int level)
{
  return pruneNodes( level, children, 1 );
}

int Trie::pruneNodes(int level, set<Trie> *items, int depth)
{
  if(items == 0)
  {
	  support--;

	  return 0;
  }

  int nodes = 0;

  if(depth==level)

	  nodes = items->size();
  else
  {
    for(set<Trie>::iterator runner = items->begin(); runner != items->end(); )
	{
      int now = pruneNodes(level, runner->getChildren(), depth+1);

      if(now)
	  {
		  nodes += now;
		  nodes++;
		  runner++;
	  }
      else
	  {
		  // deletion of the itemsets of size != k

		  Trie * tmpT = const_cast< Trie * >( &(*runner) ) ;
				tmpT->deleteChildren();
		  
		  set<Trie>::iterator tmp = runner++;
		  items->erase(tmp);
      }
    }
  }

  return nodes;
}

// ----------------------------------------------------------------------------------------------

/**
*  method which calcul the minimal transversal with i+1 edges
*  from the minimal transversal with i edges (this set) and a new edge
*  ( Demetrovics et Thi approach )
*/

void Trie::trMinI( short unsigned * itemset, int level )
{
	Trie * bi ;

	Trie * res ;
	
	bi = new Trie(0) ;
	
	// construction of Si and Bi
	// Si is stored into this
	// in fact we search all the itemset of
	// this set that don't intersect itemset,
	// we remove them from tr and 
	// we insert them into bi

	initTrBi( itemset, level, bi) ;


	// construction of the new minimal transversal
	// itemsets, construct from all the itemset
	// of this set that don't insersect itemset and the items
	// of itemset,
	// this set of itemset is returned by the method

	res = newItTr( bi, itemset, level ) ;

	delete bi;
	
	// add the news minimal transversal itemsets
	// to tr and delete the set res

	merge( res ) ;

	delete res;
	
}

void Trie::trMinI( vector< short unsigned > * itemset )
{
	Trie * bi ;

	Trie * res ;
	
	bi = new Trie(0) ;

	// construction of Si and Bi
	// Si is stored into this
	// in fact we search all the itemset of
	// this set that don't intersect itemset,
	// we remove them from tr and 
	// we insert them into bi

	initTrBi( itemset, bi) ;

	// construction of the new minimal transversal
	// itemsets, construct from all the itemset
	// of this set that don't insersect itemset and the items
	// of itemset,
	// this set of itemset is returned by the method

	res = newItTr( bi, itemset ) ;

	delete bi;
	
	// add the news minimal transversal itemsets
	// to tr and delete the set res

	merge( res ) ;

	delete res;
	
}

// ----------------------------------------------------------------------------------------------

/**
*  method which calcul the minimal transversal with i+1 edges
*  from the minimal transversal with i edges ( this set ) 
*  and a new edge ( Demetrovics et Thi approach )
*  this method is optimised for our problem since it delete 
*  all the transversal of cardinality >= n-k 
*                        with n the number of items and
*                             k the actual level of the
*								levelwise approach  
*  it also enable the separation of the minimal transversals
*  that "generate" frequent already tested ("freqTr") from the 
*  others, without affecting the process of the 
*  minimals transversals
*/

void Trie::trMinIopt( vector< short unsigned > * itemset, int n, int k, Trie * freqTr )
{

	Trie * bi ;

	Trie * res ;
	
	bi = new Trie(0) ;

	// construction of Si and Bi
	// Si is stored into this set
	// in fact we search all the itemset of
	// this set that don't intersect itemset,
	// we remove them from tr and 
	// we insert them into bi

	initTrBiOpt( itemset, bi, n ,k) ;

	// construction of the new minimal transversal
	// itemsets, construct from all the itemset
	// of this set that don't insersect itemset and the items
	// of itemset,
	// this set of itemset is returned by the method

	res = newItTr( bi, itemset,  freqTr ) ;

	delete bi;
	
	// add the news minimal transversal itemsets
	// to tr and delete the set res

	merge( res ) ;

	delete res;
}

// ----------------------------------------------------------------------------------------------

/**
*  method which calcul the minimal transversal with the edges
*  stored in the set in parameter
*  from the minimal transversal with i edges ( this set ) 
*  and a new edge ( Demetrovics et Thi approach )
*  this method is optimised for our problem since it delete 
*  all the transversal of cardinality >= n-k 
*                        with n the number of items and
*                             k the actual level of the
*								levelwise approach  
*  it also enable the separation of the minimal transversals
*  that "generate" frequent already tested ("freqTr") from the 
*  others, without affecting the process of the 
*  minimals transversals
*/

void Trie::trMinIopt( Trie * tr, int n, int k, Trie * freqTr  ) 
{
	vector< short unsigned > * vect ;

	vect = new vector<short unsigned>() ;

	if( tr->getChildren() != 0 )
		trMinIopt( tr->getChildren() , vect, n, k, freqTr ) ;
	
	delete vect ;
}

void Trie::trMinIopt( set< Trie > * items, vector< short unsigned > * vect, int n, int k, Trie * freqTr  ) 
{

	if( ( items != 0 ) && ( !items->empty() ) )
	{

		for(set< Trie >::iterator runner = items->begin(); runner != items->end(); runner++ )
		{
			
			// we save the curent item of the itemset being processed

			vect->push_back( runner->getId() ) ;					

			// we continue searching with the next items

			trMinIopt( runner->getChildren() , vect, n, k, freqTr ) ;

			vect->pop_back() ;
				
		}

	}
	else	// we have check all the items of the current itemset strored in vect
			// so we insert the new edge in the hypergraph
	{


			trMinIopt( vect, n, k, freqTr ) ;	


	}

}

// ----------------------------------------------------------------------------------------------

/**
*  method which calcul the minimal transversal of this set of
*  itemset
*/

Trie * Trie::trMin()
{
	Trie * res ;

	vector< short unsigned > * vect ;

	res = new Trie( 0 ) ;
	
	vect = new vector< short unsigned >() ;

	if( children != 0 )
		trMin( children, vect, res ) ;

	delete vect ;

	return res ;

}

// ----------------------------------------------------------------------------------------------

/**
*  method which calcul incrementaly the minimal transversal with the edges
*  stored in the set in parameter
*  from the minimal transversal with i edges ( this set ) 
*  and a new edge ( Demetrovics et Thi approach )
*/
void Trie::trMinI( Trie * tr )
{
	vector< short unsigned > * vect ;

	vect = new vector<short unsigned>() ;

	if( tr->getChildren() != 0 )
		trMin( tr->getChildren() , vect, this ) ;
	
	delete vect ;
}


void Trie::trMin( set< Trie > * items, vector< short unsigned > * vect, Trie * res ) 
{
	set< Trie > * tmp ;

	if( ( items != 0 ) && ( !items->empty() ) )
	{

		for(set< Trie >::iterator runner = items->begin(); runner != items->end(); runner++ )
		{
			
			// we save the curent item of the itemset being processed

			vect->push_back( runner->getId() ) ;

			// we continue searching with the next items

			trMin( runner->getChildren(), vect, res ) ;

			vect->pop_back() ;
				
		}

	}
	else	// we have saved all the items of the current itemset in vect
			// so we have a new edge for the hypergraph of trMin( i+1 )
	{

		if ( res->getChildren() == 0 )
		{
			// for the first edge of the iteration
			// we simply add all the items

			tmp = res->makeChildren() ;

			for( int i= 0; i < vect->size(); i++ )
			{
				res->support ++ ;
				
				tmp->insert( Trie( (*vect)[ i ] ) ) ;
			}
		}
		else
		{
			res->trMinI( vect ) ;
		}


	}

}

// ----------------------------------------------------------------------------------------------

/**
*  method which calcul the minimal transversal of this set of
*  itemset
*  this method is optimised for our problem since it delete 
*  all the transversal of cardinality >= n-k 
*                        with n the number of items and
*                             k the actual level of the
*								levelwise approach  
*/

Trie * Trie::trMinOpt( int n, int k )
{
	Trie * res ;

	vector< short unsigned > * vect ;

	res = new Trie( 0 ) ;
	
	vect = new vector< short unsigned >() ;

	if( children != 0 )
		trMinOpt( children, vect, res, n, k ) ;

	delete vect ;

	return res ;

}

void Trie::trMinOpt( set< Trie > * items, vector< short unsigned > * vect, Trie * res, int n, int k ) 
{
	set< Trie > * tmp ;

	if( ( items != 0 ) && ( !items->empty() ) )
	{

		for(set< Trie >::iterator runner = items->begin(); runner != items->end(); runner++ )
		{
			
			// we save the curent item of the itemset being processed

			vect->push_back( runner->getId() ) ;

			// we continue searching with the next items

			trMinOpt( runner->getChildren(), vect, res,n,k ) ;

			vect->pop_back() ;
				
		}

	}
	else	// we have saved all the items of the current itemset in vect
			// so we have a new edge for the hypergraph of trMin( i+1 )
	{

		if ( res->getChildren() == 0 )
		{
			// for the first edge of the iteration
			// we simply add all the items

			tmp = res->makeChildren() ;

			for( int i= 0; i < vect->size(); i++ )
			{
				res->support ++ ;
				
				tmp->insert( Trie( (*vect)[ i ] ) ) ; 
			}
		}
		else
		{
			res->trMinIopt( vect,n,k,0 ) ;
		}


	}

}

// ----------------------------------------------------------------------------------------------

/**
*  method that return the complementary of this set of itemset (res)
*  since we have all the items ordered and with the internal
*  id begining at 1 to the number of items in the database
*  we only have to use the total number of items to find the
*  complementary 
*/

Trie *  Trie::complem( int nb   ) 
{

	Trie * res ;

	res = new Trie( 0 ) ;

	vector< short unsigned > * vect ;
	
	set< Trie > * items;

	set< Trie >::iterator runner ;

	int i ;

	if( res != 0 )
	{	
	
		vect = new vector< short unsigned >() ;

		if( children != 0 )
			complRec( nb, children, vect, res ) ;
		else
		{
			i = 1 ;

			res->children = new set<Trie> ;

			items = res->children ;

			while( i < nb )
			{
				runner = items->insert( Trie( i ) ).first ;
				
				Trie * tmpT = const_cast< Trie * >( &(*runner ) );       			

				items = tmpT->makeChildren() ;

				i++ ;
			}

			items->insert( Trie( nb ) ) ;

			res->support = 1 ;

		}

		delete vect ;
	}

	return res ;

}

void Trie::complRec( int nb, set< Trie > * items, vector< short unsigned > * vect, Trie * res ) 
{

	vector< short unsigned > * comp ;

	if( ( items != 0 ) && ( !items->empty() ) )
	{

		for(set< Trie >::iterator runner = items->begin(); runner != items->end(); runner++ )
		{
			
			// we save the curent item of the itemset being processed

			vect->push_back( runner->getId() ) ;

			// we continue searching with the next items

			complRec( nb, runner->getChildren(), vect, res ) ;

			vect->pop_back() ;
				
		}

	}
	else	// we have saved all the items of the current itemset in vect
			// so we complement it 
	{
		
		comp = complem( vect, nb ) ;

		if( comp->empty() == false )
			res->insert( comp ) ;

		delete comp ;

	

	}


}

// ----------------------------------------------------------------------------------------------

/*
*  method that proces the complement of 
*  an itemset in whom the items are
*  between 1 and nb
*/

vector< short unsigned > * Trie::complem( vector< short unsigned > * vect, int nb ) 
{

	vector< short unsigned > * comp ;

	comp = new vector< short unsigned >() ;

	// we complement the items who are before the first item of
	// the current itemset
		
	for( int j =  0 ; j < (*vect)[0]; j++ ) 
			comp->push_back( j  ) ;	

	// we add the complementary items between the first and the last
	// item of the current itemset

	for( int i = 0; i < vect->size()-1 ; i++ )
	{
		for( int j =  (*vect)[ i ] +1; j < (*vect)[ i + 1 ] ; j++ ) 
			comp->push_back( j ) ;	
	}

	// we complement the items who are after the last item of
	// the current itemset
	// <= since the internal id begin at 1
		
	for( int l =  (*vect)[ vect->size()-1 ] + 1 ; l < nb ; l++ ) 
			comp->push_back(l  ) ;	


	return comp ;

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method that delete all the subsets of items
*  and return the number of subsets deleted
*/

int Trie::delSubsets( short unsigned *iset, int level, set<Trie> *items, int spos, int depth)
{
  
	int del = 0 ;

	if( items !=0 )
	{

		set<Trie>::iterator runner;
  
		int loper = spos;
		spos = depth+1;

		while( --spos >= loper )
		{

			// we search in the set of itemset
			// the current item of a sub itemset of iset

			runner = items->find( Trie( iset[spos] ) );

			if( runner != items->end() )
			{

				if( depth < level-1 )
					del += delSubsets( iset, level, runner->getChildren(), spos+1, depth+1);
	

				// if we have find a sub itemset
				// that is not used for an other itemset
				// we can delete it

				if( ( runner->getChildren() ) == 0 || ( runner->getChildren()->empty() ) )
				{
					Trie * tmpT = const_cast< Trie * >( &(*runner) ) ;
					tmpT->deleteChildren() ;

					items->erase( runner ) ;

					if( depth == level -1 )
					{
						del++;
					}
				 }
			}
				
		}
	}
	return del ;
	 
}

// ---------------------------------------------------------------------------------------------- 

/**
*  method that delete all the subsets of items
*  of size "iset->size - 1"
*  and return the number of subsets deleted
*  without knowing the level of iset
*/
	
int Trie::delSubsets( vector< short unsigned > * iset, set<Trie> *items, int spos , int depth ) 
{
	int del = 0 ;

	if( items !=0 )
	{

		set<Trie>::iterator runner;
  
		int loper = spos;
		spos = depth+1;

		while( --spos >= loper )
		{

			// we search in the set of itemset
			// the current item of a sub itemset of iset

			runner = items->find( Trie( (*iset)[ spos ] ) );

			if( runner != items->end() )
			{
				if( depth < iset->size() -1 )

					del += delSubsets( iset, runner->getChildren(), spos+1, depth+1);
				
				
				// if we have find a sub itemset
				// that is not used for an other itemset
				// we can delete it

				if( ( runner->getChildren() ) == 0 || ( runner->getChildren()->empty() ) )
				{
					Trie * tmpT = const_cast< Trie * >( &(*runner) ) ;
					tmpT->deleteChildren() ;

					items->erase( runner ) ;

					if( depth == iset->size() -1  )
						del++;
				 }
			}				
		}
	}
	 
	return del ;
}

// ---------------------------------------------------------------------------------------------- 

/**
*  method that generate into this set 
*  the subsets of size "size" of an itemsets of size k
*/

void Trie::genSubsets(  vector< short unsigned > * itemset, set<Trie> *items, int spos , int depth ) 
{

	set<Trie>::iterator runner;
  
	int loper = spos;
	spos = depth+1;

	bool insert; // boolean used to count the sub itemset generated

	while( --spos >= loper )
	{

		insert = false;

		// we search in the set of itemset
		// the current item of a sub itemset of iset

		runner = items->find( Trie( (*itemset)[ spos ] ) );

		// we insert the curent item 
		// as a component of a subsets

		if( runner == items->end() )
		{
			insert = true ;

			runner = items->insert( Trie( (*itemset)[ spos ], 0 ) ).first ;
		}
	

		if( depth < itemset->size() -1 )
		{
		    Trie * tmpT = const_cast< Trie * >( &(*runner ) );       			
		    
			genSubsets( itemset, tmpT->makeChildren(), spos+1, depth+1);						
		}
		else if ( insert )
					support++ ;	// to have the total number of elements in the global trie

				
		
	}

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method that return a set of the subsets of size
*  "size" of this set of itemsets 
*/

Trie * Trie::genSubsets( int size ) 
{
	Trie * subset ;
	
	vector< short unsigned > * itemset = new vector<short unsigned>() ;

	if( children != 0 )
	{
		subset = new Trie( 0 ) ;

		genSubsets( size, itemset, children, subset ) ;	
	}		
	
	delete itemset ;

	return subset ;

}

void Trie::genSubsets( int size, vector< short unsigned > * itemset, set< Trie > * items, Trie * subset ) 
{
	
	if( ( items != 0 ) && ( !items->empty() ) )
	{

		for(set< Trie >::iterator runner = items->begin(); runner != items->end(); runner++ )
		{
			
			// we save the curent item of the itemset being processed

			itemset->push_back( runner->getId() ) ;
			
			// we continue searching with the next items

			genSubsets( size, itemset, runner->getChildren(), subset ) ;

			itemset->pop_back() ;
				
		}

	}
	else	// we have check all the items of the current itemset strored in vect
			// so we generate his subset
	{

		subset->genSubsets( itemset, size ) ;		

	}

}

