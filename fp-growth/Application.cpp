/*----------------------------------------------------------------------
  File    : Application.cpp
  Contents: Class that count the support in a database stored in memory
  Author  : Frédéric Flouvat
----------------------------------------------------------------------*/

#include "Application.h"

// ----------------------------------------------------------------------------------------------

/**
*  default constructor
*/
	
Application::Application()
{
	minsup=0;
	remap=0;
	relist=0;
	k = 0 ;
	verbose = false;
	trie = 0;
	bdN = 0;
	bdP = 0;
}

// ----------------------------------------------------------------------------------------------

/**
*  default destructor
*/

Application::~Application()
{

	if(trie) 
    {
	    trie->deleteChildren();
		delete trie;
		trie = 0 ;
	}
	
	if(remap) delete [] remap;
  
	if(relist) delete relist;

	if(bdN) {
		bdN->deleteChildren();
		delete bdN;
		bdN = 0 ;
	}

	if(bdP) {
		bdP->deleteChildren();
		delete bdP;
		bdP = 0 ;
	}

	if (tatree )
		tat_delete( tatree );
}

// ---------------------------------------------------------------------------------------------- 

/**
*   this method update the support for the set of itemsets tr
*   acording to the database stored in tat 
*   min : minimal size of itemsets
*   set of itemset and not only trie
*/

int Application::countCandidates(Trie * tr,TATREE *tat, int min )
{
	if( tr == 0 || tat == 0 ) return 0 ;

	int used = 0 ;

	set<Trie> * child = tr->getChildren() ;

	for( set<Trie>::iterator itChild = child->begin(); itChild != child->end();itChild++ )
	{
	    Trie * tmpT = const_cast< Trie * >( &(*itChild) ) ;
		used += count( tmpT, tat, min ) ;
	}
	return used ;

}

int Application::count( Trie * tr, TATREE *tat, int min )
{

  if(tr == 0 || tat == 0 ) return 0;

  int used=0 ;
  int size ;

  if (tat_max(tat) < min)       // if the transactions are too short, //
    return used;                     // abort the recursion //

  size = tat_size(tat);            // get the number of children //

  if (size <= 0) {                 // if there are no children //
    if (size < 0)
	{
		used += countLeaf( tat->items, -size , tat_cnt( tat ), tr, 0 ) ;
	}
    return used;                     // count the normal transaction //
  } 

  for (int l = 0 ; l < tat_size(tat) ; l++ ) 
  {		  
	if( tat_item( tat,l ) == tr->getId() )
	{
		if( tr->getChildren() == 0 || tr->getChildren()->empty() ) 
		{
			tr->Increment( tat_cnt( tat_child( tat,l ) )  ) ;

			used+= tat_cnt( tat_child( tat,l ) )  ;

			return used;
		}
		else
		{
			set<Trie> * child = tr->getChildren() ;

			for( set<Trie>::iterator itChild = child->begin(); itChild != child->end();itChild++ )
			{
			    Trie * tmpT = const_cast< Trie * >( &(*itChild) ) ;
				used += count(  tmpT, tat_child( tat, l ), min -1 );

			}

			return used ;			
		}
	}
	else
	{
		if( tat_item( tat,l ) < tr->getId() )
		{
			used += count( tr, tat_child( tat, l ) , min );

		}
		else
			return used;

	}
  }
			
  return used;
}

int Application::countLeaf( int *t, int size, int cnt, Trie * tr, int spos)
{
  if(tr == 0) return 0;
  int used=0 ;

  while(spos<size && t[spos] < tr->getId())
	spos++;

  if(spos<size && (t[spos]== tr->getId()) )
  {
      if( ( tr->getChildren() == 0 ) || ( tr->getChildren()->empty() ) )
      { 
		tr->Increment( cnt );

		used += cnt ;
      }
      else 
	  {
		  set< Trie > * child = tr->getChildren() ;

		  for(set<Trie>::iterator it = child->begin(); spos<size  && it!=child->end() && t[spos]<=it->getId() ; it++) 
		  {			
			
            Trie * tmpT = const_cast< Trie * >( &(*it) ) ;
			used += countLeaf(t, size,cnt,  tmpT, spos+1);
		  }
	  }
  }
  
  return used;
}
