/*----------------------------------------------------------------------
  File    : Zigzag.cpp
  Contents: Class that implement the algorithm ABS.
  Author  : Frédéric Flouvat
----------------------------------------------------------------------*/

#include "Zigzag.h"

// ----------------------------------------------------------------------------------------------

/**
*  default constructor
*/

Zigzag::Zigzag():Application()
{
	ratioNfC = 0 ;

	maxBdP = 0 ;

	avgBdP = 0 ;
	
	bdN = new Trie(0) ;

	bdP = new Trie(0);

	bdPapriori = new Trie( 0 ) ;

}

// ----------------------------------------------------------------------------------------------

/**
*  default destructor
*/

Zigzag::~Zigzag()
{

	if( bdPapriori )
	{
		bdPapriori->deleteChildren();
		delete bdPapriori;
		bdPapriori = 0 ;
	}
	

} 

// ----------------------------------------------------------------------------------------------

/**
*  method that execute the algrithm zigzag
*/

int Zigzag::run()
{
	Trie * complSet = 0 ;



	Trie * opt=  new Trie( 0 );	// store all the itemsets that are closed to the positive border

	Trie * transv = 0 ;

	Trie * freqTr = 0 ;

	
	int sets ; // number of frequent itemset found by the levelwise approach

	int nbFreq ; // number of itemset of the optimist border
				 // which are frequent or their error is <= eps

	int totGen ; // total number of itemset in the optimist border
	
    bool generatedFk = false ; // true if | Fk | is not null

	vector< unsigned int > * stat = new vector< unsigned int> ;  // array storing the number of itemsets in bd+ by level

	clock_t start ;

	start = clock();

	relist = new set< Element >;
	
	// execution of apriori Borgelt until a certain level k conditionned by ratioNfC

	tatree = apriori( fileName,0, minsup, k, bdPapriori, bdN, relist, ratioNfC, eps, k, stat, maxBdP, generatedFk, verbose );

   if( verbose ) cout<<"Minsup : "<<minsup<<"\t Ratio or Level : "<<ratioNfC<<"\t Erreur :"<<eps<<endl;
	
	remap = new short unsigned[ relist->size() ];

	initK = k ;
	
	for( set<Element>::iterator itRelist = relist->begin(); itRelist != relist->end() ; itRelist++ )
	{
		remap[ itRelist->id ] = itRelist->oldid; 
	}

	
	if(verbose)
	{
		cout <<"Apriori : [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;		
		cout<<"Bd+k-1 size : "<<bdPapriori->getSupport()<<endl;
		cout<<"Bd+ size : "<<bdP->getSupport()<<endl;
		cout<<"Bd- size : "<<bdN->getSupport()<<endl;
	}

	if( k != 0 && generatedFk == true )
	{
		// Calculation of the transversals mins
		// from the negative border of apriori

		freqTr = new Trie( 0 ) ;

		start = clock();

		transv = bdN->trMinOpt( relist->size(), k-1 ) ;

		// we delete bd-
		// since we don't keep the negative border
		// but we use it to store the non frequent
		// to be inserted in the hypergraph

		bdN->deleteChildren() ;
			
		while( ( transv->getChildren() != 0 ) && ( ! transv->getChildren()->empty() ) )
		{

			if(verbose)
			{
                cout<<endl;
				cout<<"Nb attributes : "<<relist->size()<<" Actual level (k) : "<<k<<endl;				
				cout <<"Tr Min : "<< transv->getSupport() <<"   [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;
			}

			// Generation of the complementaries

			start = clock();

			if( transv->getSupport() != 0 )
				complSet = transv->complem( relist->size() ) ;
			
			
			if(verbose)
				cout <<"Complement [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

			
			// Update of the support from the db

			start = clock();
			
			countCandidates( complSet,tatree, k-1 );
				
			if(verbose)
				cout <<"Complement support update [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

			// process of the optimist positive border 
			// ie all the itemsets frequent update Bd+, the
			// non frequent with an error < "eps"
			// are stored in opt and the others gennerates the sub sets of bad itemsets(in trie),
			// it update also the set of transversal that generate
			// max frequent itemset ( freqTr )

			totGen = complSet->getSupport() ;

			start = clock();

			trie = new Trie( 0 ) ;
			nbFreq = processOptGenSub( complSet, opt, transv, freqTr, trie, k+1, stat ) ;
			complSet->deleteChildren() ;
			delete complSet ;
			
			if(verbose)
			{
				cout <<"Bd+opt processing and sub itemsets generation[" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;
				cout <<" : "<< nbFreq <<" itemsets are closed or in Bd+ on "<< totGen <<" itemsets generated"<<endl ;
				cout<<"Bd+ size : "<<bdP->getSupport()<<endl;
				cout<<"Opt size : "<<opt->getSupport()<<endl;
				cout <<"Subsets generated : "<<trie->getSupport()<<endl;
			}
			// we execute the pessimist approach
			// for the sub sets of size k+1 of comlSet
			// for nbLev level 
			
			if(  trie !=0 && trie->getChildren() != 0 && ! trie->getChildren()->empty()   )
			{				
				start = clock();
			
				// we prune the itemsets included into an itemset of Bd+

                genCand( trie, bdP) ;
                genCand( trie , opt );

                if(verbose)
                	cout<<"Generated wrt bd+ and opt: "<<trie->getSupport()<<" [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

                //candidates support update
                	
				countCandidates( trie,tatree, k+1 );

				if(verbose) 
				  	  cout <<"Support Update "<< trie->getSupport() << " [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s] " << flush;

				// processing of the candidates
                // frequents update the pessimist positive border				
                // infrequents are insert in bd- 
                                               
				start = clock();
				
				nbFreq = pruneCandidates( trie, k+1);

				trie->deleteChildren() ;
				trie = 0 ;

				k++;
    
				if(verbose)
                { 
					cout <<"Prune Candidates "<< nbFreq << " [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]\n" << flush;
					cout <<" bd- k : "<<bdN->getSupport() << endl;
				}				

			}

			start = clock();

			if( ! transv->getChildren()->empty() )  // if we can generate sets that are not
													// supersets of ones that generated frequent
			{
				// dualization from the itemset of the negative border found at this iteration
				
				transv->trMinIopt( bdN , relist->size(), k-2, freqTr ) ;

				bdN->deleteChildren() ;
			}

		}

		freqTr->deleteChildren() ;
		delete freqTr ;
		transv->deleteChildren() ;
		delete transv;

		if( verbose )
		{
			cout<<"Opt size : "<< opt->getSupport()<<endl;
		}

		// we execute the optimist approach
		// ie we found all the maximals that specialize an itemset of opt

		start = clock();

		optApproach( opt, tatree, initK, stat ) ;

		if(verbose)
			cout<<"Optimist Approach [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;
		
    	opt->deleteChildren();
    	delete opt;

		if( verbose )
		{
			cout<<"Bd+ size : "<< bdP->getSupport()<<endl;
		}

		// update of the positive border with the
		// itemsets representing candidates for the 
		// positive border found by the optimist and pessimist approach

		start = clock();

		updateBdP() ;

		if(verbose)
		{
			cout<<"Final update [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

			cout<<"Bd+ size : "<<bdP->getSupport()<<endl;

			cout<<"Size of the largest itemset in Bd+ : "<<maxBdP<<endl; 

			cout<<"Average size of itemset in Bd+ : "<< avgBdP/bdP->getSupport() <<endl; 
			
		}

	}
	else
	{
		bdP = bdPapriori ;

		bdPapriori = 0 ;

	}
	
	for( int l = 0; l<= maxBdP+1 ; l++ )
	{
	    cout<<(*stat)[ l ] <<endl;	    
	}    
	
	delete stat;

	if( ! saveFile.empty() )
	{
		start = clock() ;

		bdP->saveItemsets( saveFile.c_str() , remap );

		if( verbose )cout<<"Saving Bd+ in file "<< saveFile.c_str() <<" [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;
	}


	
	return 1 ;
}

// ----------------------------------------------------------------------------------------------

/**
*  process the optimist positive border
*  all the frequent update Bd+, the
*  non frequent with an error < "eps"
*  are stored in optIt and the others are keep in the set
*  Note : complSet is the complement of the transversals mins
*         optIt is a set of trie of itemsets which are closed to
*          the positive border, all the itemset are stored
*           with itemset of the same size in a trie, those 
*           trie ared classified according to the size of 
*           these itemsets
*/

int Zigzag::processOptBorder(Trie * complSet, set< Trie > * optIt, Trie * tr, Trie * freqTr,
                              vector< unsigned int > * stat  )
{

	int pruned;	// number of element deleted 
				// ie number of itemsets frequent or near the positive border

	vector< short unsigned > * tmp ;
  
	tmp = new vector< short unsigned > ;

	pruned = complSet->getSupport() ;

	processOptBorder( complSet->getChildren(),tmp, optIt, complSet, tr, freqTr, stat );
	
	pruned = pruned - complSet->getSupport() ;

	delete tmp;

	return pruned;

}
	
bool Zigzag::processOptBorder( set<Trie> *items, vector< short unsigned > *itemset, set< Trie > * optIt,
							   Trie * complSet, Trie * tr ,Trie * freqTr, vector< unsigned int > * stat  )
{
	
	if(items == 0) return 0;

	set<Trie>::iterator tmp ;

	bool del = true;


	for(set<Trie>::iterator runner = items->begin(); runner != items->end(); )
	{
		itemset->push_back( runner->getId() );

		if( ( runner->getChildren() == 0 ) || ( ( runner->getChildren() )->empty() ) )
		{


			if( runner->getSupport() < minsup )
			{

				if( error(runner->getSupport(), minsup ) <= eps ) // we are closed to the positive border
				{

					tmp = optIt->find( Trie( itemset->size() ) );

					if( tmp == optIt->end() )
						tmp = optIt->insert( Trie( itemset->size() ) ).first ;

					// the itemsets for the optimist approach
					// are inserted into the good trie of optIt

					Trie * tmpT = const_cast< Trie * >( &(*tmp ) );
					
					tmpT->insert( itemset ) ; 

					
					// we move the transversal assoicated with this itemset
					// to the subset freqTr
					// it avoid us to create subset of this itemset

					moveTransFreq( itemset, tr, freqTr )  ;

					Trie * tmpT2 = const_cast< Trie * >( &(*runner ) );
					     
					tmpT2->deleteChildren();
					tmp = runner++;
					items->erase(tmp);
					
					complSet->Decrement() ;	// we decrement the number of itemset in complSet

				
				}
				else // the itemsets for the pessimist approach stay in complSet
				{

					runner++;
					
					del = false ;
				}

								
			}
			else
			{

				// Update of the positive border 

				bdP->insert( itemset,runner->getSupport() ) ;
     
				// update the size of the max and avg itemsets of bdP

				if( maxBdP < itemset->size() ) 
						maxBdP = itemset->size() ;

				avgBdP = avgBdP + itemset->size() ;
										  
                (*stat)[ 0 ] ++;
                (*stat)[ itemset->size()+1 ]++;

				// we move the transversal assoicated with this itemset
				// to the subset freqTr
				// it avoid us to create subset of this itemset

				moveTransFreq( itemset, tr, freqTr )  ;
		
				// we delete the itemset that is include in the positive border

				Trie * tmpT = const_cast< Trie * >( &(*runner ) );
				tmpT->deleteChildren();
				tmp = runner++;
				items->erase(tmp);

				complSet->Decrement() ;	// we decrement the number of itemset in complSet

				
			}
		}
		else
		{	

			del = processOptBorder( runner->getChildren(), itemset, optIt, complSet, tr, freqTr, stat);
      
			if(	( del == true ) && 

				( ( runner->getChildren() == 0 ) || ( (runner->getChildren())->empty() ) ))
				// if all the children have beene deleted we delete the father node
			{
			    Trie * tmpT = const_cast< Trie * >( &(*runner ) );
			    
				tmpT->deleteChildren();
				tmp = runner++;
				items->erase(tmp);				
				
			}
			else 

				runner++;
			
		}
	
		itemset->pop_back() ;
	}

	return del;


}

int Zigzag::processOptBorder(Trie * complSet, Trie * opt, Trie * tr, Trie * freqTr,vector< unsigned int > * stat  )
{

	int pruned;	// number of element deleted 
				// ie number of itemsets frequent or near the positive border

	vector< short unsigned > * tmp ;
  
	tmp = new vector< short unsigned > ;

	pruned = complSet->getSupport() ;

	processOptBorder( complSet->getChildren(),tmp, opt, complSet, tr, freqTr, stat );
	
	pruned = pruned - complSet->getSupport() ;

	delete tmp;

	return pruned;

}
	
bool Zigzag::processOptBorder( set<Trie> *items, vector< short unsigned > *itemset, Trie * opt,
							   Trie * complSet, Trie * tr ,Trie * freqTr, vector< unsigned int > * stat  )
{
	
	if(items == 0) return 0;

	set<Trie>::iterator tmp ;

	bool del = true;


	for(set<Trie>::iterator runner = items->begin(); runner != items->end(); )
	{
		itemset->push_back( runner->getId() );

		if( ( runner->getChildren() == 0 ) || ( ( runner->getChildren() )->empty() ) )
		{


			if( runner->getSupport() < minsup )
			{

				if( error(runner->getSupport(), minsup ) <= eps ) // we are closed to the positive border
				{			

					// the itemsets for the optimist approach
					// are inserted stored in opt
					opt->insert( itemset ) ; 
					
					// we move the transversal assoicated with this itemset
					// to the subset freqTr
					// it avoid us to create subset of this itemset

					moveTransFreq( itemset, tr, freqTr )  ;

					Trie * tmpT = const_cast< Trie * >( &(*runner ) );
					tmpT->deleteChildren();
					tmp = runner++;
					items->erase(tmp);
					
					complSet->Decrement() ;	// we decrement the number of itemset in complSet

				
				}
				else // the itemsets for the pessimist approach stay in complSet
				{
					runner++;
					
					del = false ;
				}
							
			}
			else
			{

				// Update of the positive border 
				
				bdP->insert( itemset,runner->getSupport() ) ;
     
				// update the size of the max and avg itemsets of bdP

				if( maxBdP < itemset->size() ) 
						maxBdP = itemset->size() ;

				avgBdP = avgBdP + itemset->size() ;
								  
                (*stat)[ 0 ] ++;
                (*stat)[ itemset->size()+1 ]++;

				// we move the transversal assoicated with this itemset
				// to the subset freqTr
				// it avoid us to create subset of this itemset

				moveTransFreq( itemset, tr, freqTr )  ;
		
				// we delete the itemset that is include in the positive border

				Trie * tmpT = const_cast< Trie * >( &(*runner ) );
				tmpT->deleteChildren();
				tmp = runner++;
				items->erase(tmp);

				complSet->Decrement() ;	// we decrement the number of itemset in complSet
				
			}
		}
		else
		{	

			del = processOptBorder( runner->getChildren(), itemset, opt, complSet, tr, freqTr, stat );
      
			if(	( del == true ) && 
				( ( runner->getChildren() == 0 ) || ( (runner->getChildren())->empty() ) ))
				// if all the children have beene deleted we delete the father node
			{
			    Trie * tmpT = const_cast< Trie * >( &(*runner ) );
				tmpT->deleteChildren();
				tmp = runner++;
				items->erase(tmp);				
				
			}
			else 

				runner++;
			
		}
	
		itemset->pop_back() ;
	}

	return del;


}


// ----------------------------------------------------------------------------------------------

/**
*  process the optimist positive border: all the frequent update Bd+, the non frequent 
*  with an error < "eps" are stored in optIt and the others generate the subsets (of size "size")
*  of the infrequent itemsets "far" from the positive border for the generating candidate
*/

int Zigzag::processOptGenSub(Trie * complSet, Trie  * opt, Trie * tr, Trie * freqTr,
							 Trie * subSet, int size, vector< unsigned int > * stat  )
{

	int nbFreq;	// number of element deleted 
				// ie number of itemsets frequent or near the positive border

	vector< short unsigned > * tmp ;
  
	tmp = new vector< short unsigned > ;

	nbFreq = bdP->getSupport() + opt->getSupport() ;

	processOptGenSub( complSet->getChildren(),tmp, opt, complSet, tr, freqTr, subSet, size, stat  );
	
	nbFreq = bdP->getSupport() + opt->getSupport()  - nbFreq ;

	delete tmp;

	return nbFreq;

}
	
bool Zigzag::processOptGenSub( set<Trie> *items, vector< short unsigned > *itemset, Trie * opt,
							   Trie * complSet, Trie * tr ,Trie * freqTr, Trie * subSet, int size,
                               vector< unsigned int > * stat  )
{
	
	if(items == 0) return 0;

	set<Trie>::iterator tmp ;

	bool del = true;

	bool inserted ;
	

	for(set<Trie>::iterator runner = items->begin(); runner != items->end(); )
	{
		itemset->push_back( runner->getId() );

		if( ( runner->getChildren() == 0 ) || ( ( runner->getChildren() )->empty() ) )
		{

			if( runner->getSupport() < minsup )
			{

				if( error(runner->getSupport(), minsup ) <= eps ) // we are closed to the positive border
				{			

					// the itemsets for the optimist approach
					// are inserted stored in opt
					opt->insert( itemset ) ; 
					
					// we move the transversal assoicated with this itemset
					// to the subset freqTr
					// it avoid us to create subset of this itemset

					moveTransFreq( itemset, tr, freqTr )  ;

					Trie * tmpT = const_cast< Trie * >( &(*runner ) );
					tmpT->deleteChildren();
					tmp = runner++;
					items->erase(tmp);
					
					complSet->Decrement() ;	// we decrement the number of itemset in complSet

				
				}
				else // this "very" infrequent itemsets are used to generate subsets
					 // for the candidate generation
				{

					subSet->genSubsets( itemset, size );
					
					Trie * tmpT = const_cast< Trie * >( &(*runner ) );

					tmpT->deleteChildren();
					tmp = runner++;
					items->erase(tmp);
					
					complSet->Decrement() ;	// we decrement the number of itemset in complSet
				}

								
			}
			else
			{

				// Update of the positive border 
				
				bdP->insert( itemset,runner->getSupport() ) ;
				
				// update the size of the max and avg itemsets of bdP

				if( maxBdP < itemset->size() ) 
						maxBdP = itemset->size() ;

				avgBdP = avgBdP + itemset->size() ;

                (*stat)[ 0 ] ++;
                (*stat)[ itemset->size()+1 ]++;

				// we move the transversal assoicated with this itemset
				// to the subset freqTr
				// it avoid us to create subset of this itemset

				moveTransFreq( itemset, tr, freqTr )  ;
		
				// we delete the itemset that is include in the positive border

				Trie * tmpT = const_cast< Trie * >( &(*runner ) );
				tmpT->deleteChildren();
				tmp = runner++;
				items->erase(tmp);

				complSet->Decrement() ;	// we decrement the number of itemset in complSet

				
			}
		}
		else
		{	

			del = processOptGenSub( runner->getChildren(), itemset, opt, complSet, tr, freqTr,
									subSet, size, stat  );
      
			if(	( del == true ) && 
				( ( runner->getChildren() == 0 ) || ( (runner->getChildren())->empty() ) ))
				// if all the children have beene deleted we delete the father node
			{
			    Trie * tmpT = const_cast< Trie * >( &(*runner ) );
				tmpT->deleteChildren();
				tmp = runner++;
				items->erase(tmp);				
				
			}
			else 

				runner++;
			
		}
	
		itemset->pop_back() ;
	}

	return del;


}

// ----------------------------------------------------------------------------------------------

/**
*  method that move a transversal that have
*  "generated" a frequent itemset "itemset"
*  into a set of itemset (freqTr) from 
*  the original set of transversal (tr)
*/

void Zigzag::moveTransFreq( vector< short unsigned > *itemset, Trie * tr,  Trie * freqTr ) 
{
	vector< short unsigned > * comp;
		
	// we search the transversal associated with this itemset

	comp = freqTr->complem( itemset, relist->size() );

	// we insert it in the set of transversal that 
	// enable to obtain frequent itemset

	freqTr->insert( comp ) ;

	// we delete this transvresal from the original set

	tr->deleteIt( comp ) ;

	delete comp;

}

// ----------------------------------------------------------------------------------------------

/**
*  method updating the positive border
*  with the itemsets of bdPopt
*  and insert the itemset of bd+ found by apriori
*/

void Zigzag::updateBdP() 
{
	vector< short unsigned > * vect ;

	vect = new vector<short unsigned>() ;

	clock_t start = clock();

	start = clock();

	if( bdPapriori->getChildren() != 0 )
		bdP->merge( bdPapriori ) ;

	if(verbose)
		cout <<"Update bd+ with all the max freq found \t[" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

	delete vect ;

}
	
// ----------------------------------------------------------------------------------------------

/**
*  method processing the optimist approach
*  ie from a set of itemset in parameter
*  it found all the maximals frequents 
*  itemsets of the sub lattice 
*  the maximal itemset are inserted in Bd+
*  and save the min non frequent, 
*  until the level k
*/

void Zigzag::optApproach( set< Trie > * optIt, TATREE * tat, int lvl, vector< unsigned int > * stat  ) 
{
	vector< short unsigned > * itemset ;

	set< Trie >::iterator runner ;

	set< Trie >::iterator runner2 ;

	set<Trie>::iterator tmp ;
	
	Trie tmpSet(0) ;

	int nbfreq ;
	
	int level ;

	if( ! optIt->empty() )
	{

		itemset = new vector<short unsigned>() ;

		// we search the actual the level
		// ie we find the itemsets with the max size

		runner = optIt->end() ;

		runner --;

		runner2 = runner  ;
		
		while( ( ! optIt->empty() ) && !( runner->getChildren()->empty() )&& ( runner->getId() > lvl )    )
		{
			
			level = runner->getId() ;

			if(verbose)
				cout <<"Optimist approach : level "<<level-1<<endl;

			// if the sub set of the current itemsets
			// don't have the same level than the next trie
			// we create a new sub set in this trie
			// according to the level
			
			if( ( runner2 != optIt->begin() ) && ( ( --runner2 )->getId() == level - 1 ) )
			{
		
				tmpSet.setChildren( runner2->getChildren() ) ;	
				tmpSet.setSupport( runner2->getSupport() );
		    }				
			else
			{
				tmpSet.setChildren( new set<Trie>() ) ;

				tmpSet.setId( level - 1 ) ;

				optIt->insert( tmpSet ) ;
			}

			clock_t start = clock();
  
			// generation of the sub set of the itemsets of the current itemsets in optIt

			optApproach( runner->getChildren(), itemset, &tmpSet, stat ) ;

			if(verbose)
				cout <<"\t subests generation : "<<tmpSet.getSupport() <<"\t[" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;
			start = clock();
			
			// we prune the itemsets included into an itemset of Bd+
		
			genCand( &tmpSet, bdP) ;
				
			if(verbose)
				cout<<"\t Generated wrt bd+ an bd+opt : "<<tmpSet.getSupport()<<" [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

			start = clock();

			// support update

			countCandidates( &tmpSet,tat, level - 1 );			

			if(verbose)
				cout <<"\t Supports update \t[" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

			// we delete the old non frequent

			Trie * tmpT = const_cast< Trie * >( &(*runner ) );
			tmpT->deleteChildren() ;
			tmp = runner-- ;
			optIt->erase( tmp ) ;
			
			// To keep in runner the actual number of itemets
			tmpT = const_cast< Trie * >( &(*runner ) );
			tmpT->setSupport( tmpSet.getSupport() );

			start = clock();

			// prune the frequent
			// and update of the positive and negative border 

			nbfreq = pruneCandidatesOpt( &tmpSet, stat ) ;

			// To keep in runner the actual number of itemets
			tmpT = const_cast< Trie * >( &(*runner ) );
			tmpT->setSupport( tmpSet.getSupport() );

			if(verbose)
			{
				cout <<"\t frequent suppression and Bd+opt update \t[" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;
				cout <<"\t number of frequent itemsets : "<<nbfreq <<endl;
				cout <<"\t Bd+ :"<<bdP->getSupport()<<endl; ;
			}				
		
			runner2 =  runner ;

		}

		delete itemset ;
	}
}

void Zigzag::optApproach( set< Trie > * items,  vector< short unsigned > * itemset, Trie * nextLev, vector< unsigned int > * stat ) 
{

	set<Trie>::iterator tmp ;

	if( ( items != 0 ) && ( !items->empty() ) )
	{

		for(set< Trie >::iterator runner = items->begin(); runner != items->end(); )
		{
			
			// we save the curent item of the itemset being processed

			itemset->push_back( runner->getId() ) ;			
			
			// we continue searching with the next items

			optApproach( runner->getChildren(), itemset, nextLev, stat ) ;

			// deletion of the itemset

			Trie * tmpT = const_cast< Trie * >( &(*runner ) );
			tmpT->deleteChildren();
			tmp = runner++;
			items->erase(tmp);
					

			itemset->pop_back() ;
			
		}
	
	}
	else	// we have check all the items of the current itemset strored in vect			
	{
			// we generate the immediate subsets of the itemset
		
			nextLev->genSubsets( itemset, itemset->size() - 1 ) ;
	}

}

void Zigzag::optApproach( Trie * opt, TATREE * tat, int lvl, vector< unsigned int > * stat ) 
{
	vector< short unsigned > * vect ;

	set < Trie > * 	optIt= new set< Trie >() ;
	

	if( opt->getChildren() != 0 )
	{
		vect = new vector<short unsigned>() ;

		initOptApproach( opt->getChildren() , vect, optIt ) ;

		delete vect ;

		optApproach( optIt, tat, lvl, stat ) ;

	}

	for( set< Trie >::iterator runner = optIt->begin(); runner != optIt->end(); runner++ )
	{
	      Trie * tmpT = const_cast< Trie * >( &(*runner ) );
	    
          tmpT->deleteChildren();
	}	
	delete optIt ;
	

		
}

void Zigzag::initOptApproach( set< Trie > * items,  vector< short unsigned > * itemset, set< Trie > * optIt ) 
{

	set<Trie>::iterator tmp ;

	if( ( items != 0 ) && ( !items->empty() ) )
	{

		for(set< Trie >::iterator runner = items->begin(); runner != items->end(); )
		{
			
			// we save the curent item of the itemset being processed

			itemset->push_back( runner->getId() ) ;			
			
			// we continue searching with the next items

			initOptApproach( runner->getChildren(), itemset, optIt ) ;

			// deletion of the itemset

			Trie * tmpT = const_cast< Trie * >( &(*runner ) );
			tmpT->deleteChildren();
			tmp = runner++;
			items->erase(tmp);
					

			itemset->pop_back() ;
			
		}
	
	}
	else	// we have check all the items of the current itemset strored in vect			
	{       // construction of the trees that regroup itemsets wrt their size
	
		tmp = optIt->find( Trie( itemset->size() ) );

		if( tmp == optIt->end() )
				tmp = optIt->insert( Trie( itemset->size() ) ).first ;

		// the itemsets for the optimist approach
		// are inserted into the good trie of optIt

		Trie * tmpT = const_cast< Trie * >( &(*tmp ) );
		tmpT->insert( itemset ) ;
	}

}

// ----------------------------------------------------------------------------------------------

/**
*  prune the non frequent itemset 
*  insert the non frequent in Bd-
*  and update Bd+ of the pess 
*  approach
*  (used in the pess approach)
*/

int Zigzag::pruneCandidates(Trie * tr, int level)
{
  int pruned;
  short unsigned *tmp = new short unsigned[level];

  pruned = pruneCandidates(level,tr->getChildren(),1,tmp ) ;

  tr->setSupport( pruned ) ;

  delete [] tmp;
  return pruned;
}

int Zigzag::pruneCandidates( int level, set<Trie> *items, int depth, short unsigned *itemset )
{

  if(items == 0) return 0;
  int left = 0;

  for(set<Trie>::iterator runner = items->begin(); runner != items->end(); )
  {
    itemset[depth-1] = runner->getId();

    if(depth == level)
    {
      if(runner->getSupport() < minsup)
      {
		// Update of the negative border 

		bdN->insert( itemset, level,runner->getSupport()  ) ;

        Trie * tmpT = const_cast< Trie * >( &(*runner ) );
    	tmpT->deleteChildren();
    	set<Trie>::iterator tmp = runner++;
    	items->erase(tmp);
      }
      else
      {
		left++;

		runner++;
      }
    }
    else
    {	
      int now = pruneCandidates(level, runner->getChildren(), depth+1, itemset);
      
      if(now)
      {
         left += now;
    	 runner++;
      }
      else
      {
         Trie * tmpT = const_cast< Trie * >( &(*runner ) );
         tmpT->deleteChildren();
         set<Trie>::iterator tmp = runner++;
         items->erase(tmp);
      }
    }
  }
  return left;
}

// ----------------------------------------------------------------------------------------------

/**
*  prune the infrequent candidates itemsets, for each infrequent 
*  itemset update/process the transversal min, for each frequent
*  update Bd+ of the pessimist approach
*/

int Zigzag::processCandTrans(Trie * tr, int level, Trie * transv, int n, int lvl, Trie * freqTr)
{
  int pruned;
  vector< unsigned short> *tmp = new vector< short unsigned>;

  pruned = processCandTrans(level,tr->getChildren(),1,tmp, transv, n, lvl, freqTr ) ;

  tr->setSupport( pruned ) ;

  delete tmp;
  return pruned;
}

int Zigzag::processCandTrans( int level, set<Trie> *items, int depth, vector< unsigned short> *itemset,
							 Trie * transv, int n, int lvl, Trie * freqTr )
{

  if(items == 0) return 0;
  int left = 0;

  for(set<Trie>::iterator runner = items->begin(); runner != items->end(); )
  {
    itemset->push_back( runner->getId() );

    if(depth == level)
	{
      if(runner->getSupport() < minsup)
	  {
		
		transv->trMinIopt( itemset, n, lvl, freqTr );

        Trie * tmpT = const_cast< Trie * >( &(*runner ) );
		tmpT->deleteChildren();
		set<Trie>::iterator tmp = runner++;
		items->erase(tmp);
      }
      else {

		left++;

        Trie * tmpT = const_cast< Trie * >( &(*runner ) );
		tmpT->deleteChildren();
		set<Trie>::iterator tmp = runner++;
		items->erase(tmp);
      }
    }
    else 
	{	
      int now = processCandTrans(level, runner->getChildren(), depth+1, itemset,
								 transv, n, lvl, freqTr );
      if(now)
	  {
		left += now;
		runner++;
	  }
      else
	  {
	      Trie * tmpT = const_cast< Trie * >( &(*runner ) );
	      tmpT->deleteChildren();
	      set<Trie>::iterator tmp = runner++;
	      items->erase(tmp);
      }
    }
	itemset->pop_back() ;
  }

  return left;
}

// ----------------------------------------------------------------------------------------------

/**
*  prune the frequent itemset  without knowing the level
*  for the optimist approach since we go through from the 
*  high level itemsets ie it the positive border passed of this approach
*  return the number of frequent found
*/

int Zigzag::pruneCandidatesOpt( Trie * tr, vector< unsigned int > * stat  )
{
	int nbnf =0 ;
	int nbfreq =0 ;

	vector< short unsigned > * tmp ;
  
	tmp = new vector< short unsigned > ;

	nbfreq = tr->getSupport() ;

	nbnf = pruneCandidatesOpt( tr->getChildren() , tmp, stat );

	tr->setSupport(  nbnf ) ;

	nbfreq = nbfreq -  nbnf ;

	delete tmp;

	return nbfreq;

}
	

int Zigzag::pruneCandidatesOpt( set<Trie> *items, vector< short unsigned > *itemset, vector< unsigned int > * stat )
{

	set< Trie >::iterator runDel ;

	if(items == 0) return 0;

	int left = 0;

	for(set<Trie>::iterator runner = items->begin(); runner != items->end(); )
	{
		itemset->push_back( runner->getId() );

		// if we have find an itemset

		if( ( runner->getChildren() == 0 ) || ( ( runner->getChildren() )->empty() ) )
		{			

			// we keep the non frequent itemset in the set
			// we update tmp bd+ with the frequent
			// and delete them from the set
			// since we search the maximals frequents

			if(runner->getSupport() >= minsup)
			{

			   // update the size of the max and avg itemsets of bdP

				if( maxBdP < itemset->size() ) 
						maxBdP = itemset->size() ;

				avgBdP = avgBdP + itemset->size() ;
					  
                (*stat)[ 0 ] ++;
                (*stat)[ itemset->size()+1 ]++;

				// we insert the frequent in  Bd+ of the optimist approach
				
				bdP->insert( itemset, runner->getSupport() ) ;

				// the itemset is  frequent
				// so we delete it

				Trie * tmpT = const_cast< Trie * >( &(*runner ) );
				tmpT->deleteChildren();
				set<Trie>::iterator tmp = runner++;
				items->erase(tmp);
			}
			else
			{

				left++;
				runner++;
			}
			
	    }
		else
		{
			int now = pruneCandidatesOpt( runner->getChildren(), itemset, stat );

			if(now)
			{
				left += now;
				runner++;
			}
			else 
			{	

                Trie * tmpT = const_cast< Trie * >( &(*runner ) );
				tmpT->deleteChildren();
				set<Trie>::iterator tmp = runner++;
				items->erase(tmp);
			}
		}
		itemset->pop_back() ;

	}

	return left;
}

// ---------------------------------------------------------------------------------------------- 

/**
*  method that calcul the distance
*  form the positive border
*/

double Zigzag::error( int supp, int minSup )
{
	double err ;

	if( supp <= minSup )
		err =  1 - (double) supp / (double) minSup ;
	else 
		err = - 1 + (double) supp / (double) minSup ;

	return err;

}

// ----------------------------------------------------------------------------------------------  

/**
*  method that prune the candidates generated from the itemsets found far 
*  from the positive border (stored in tr), ie it delete from all the 
*  subsets of these itemsets at the level k+1, th itemsets included in an 
*  itemset of Bd+ (bd)
*/

void Zigzag::pruneCandBottomUp( Trie * tr, Trie * bd, Trie * opt ) 
{
	vector< short unsigned > * vect ;

	vect = new vector<short unsigned>() ;

	if( tr->getChildren() != 0 && bd->getChildren() != 0)
		pruneCandBottomUp( tr->getChildren(), bd , opt, vect, tr ) ;
	
	delete vect ;
}



bool Zigzag::pruneCandBottomUp( set< Trie > * items, Trie * bd, Trie * opt, vector< short unsigned > * vect, Trie * tr ) 
{
	set< Trie >::iterator tmp ;

	bool res ;		

	if( ( items != 0 ) && ( !items->empty() ) )
	{

		for(set< Trie >::iterator runner = items->begin(); runner != items->end();)
		{
			res = false ;

			// we save the curent item of the itemset being processed

			vect->push_back( runner->getId() ) ;
			
			// we continue searching with the next items

			res = pruneCandBottomUp( runner->getChildren(), bd , opt, vect, tr ) ;
			
			// we delete the itemset inlcuded in an itemset of bd

			if ( ( res == true ) &&  
					( ( runner->getChildren() == 0 ) || ( (runner->getChildren())->empty() ) ) )
				{
				    Trie * tmpT = const_cast< Trie * >( &(*runner ) );
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
	{
	
		res = bd->include( vect );

		if( res == true )
		{

			tr->setSupport( tr->getSupport() -1 );
		}
		else
		{
			res = opt->include( vect );

			if( res == true )
				tr->setSupport( tr->getSupport() -1 );
		}
	}

	return res ;

}

// ----------------------------------------------------------------------------------------------  

/**
*  method that generates the candidates at the level k+1
*  this method delete all the subsets included into one itemset of bdp
*  subSet : trie with all the subset of size k+1 of the infrequent found by the jump
*  bdp : set of itemsets of bd+ or near bd+ found by jumps
*/

void Zigzag::genCand( Trie * subSet, Trie * inbdp )
{
	bool del = false ;

	set< Trie >::iterator tmpIt ;

	if( subSet != 0 && inbdp != 0 )
	{

	
		set<Trie> * child = subSet->getChildren() ;

		for( set<Trie>::iterator itChild = child->begin(); itChild != child->end(); )
		{
		    Trie * tmpT = const_cast< Trie * >( &(*itChild ) );
			del = genCand( tmpT, inbdp->getChildren(), subSet ) ;

			if( del && ( itChild->getChildren() == 0 || itChild->getChildren()->empty()  ))
			{
			    Trie * tmpT = const_cast< Trie * >( &(*itChild ) );
				tmpT->deleteChildren() ;

				tmpIt = itChild ++ ;

				child->erase( tmpIt ) ;

			}
			else
				itChild++ ;
		}
	}
}

bool Zigzag::genCand( Trie * subSet, set<Trie> * childbdp, Trie * tr )
{
	bool del =false ;

	set< Trie >::iterator tmpIt ;


	if(subSet == 0 || childbdp == 0 || childbdp->empty() ) return del;

	int size ;

	size = childbdp->size();            // get the number of children //

	for( set<Trie>::iterator itChildbdp = childbdp->begin(); itChildbdp != childbdp->end() 
		&& !(del  && ( subSet->getChildren() == 0 || subSet->getChildren()->empty() ))
		; itChildbdp++ )
	{
		  
		if( itChildbdp->getId() == subSet->getId() )
		{
			if( subSet->getChildren() == 0 || subSet->getChildren()->empty() ) 
			{
				del = true ;

				tr->Decrement() ;

				return del;
			}
			else
			{
				set<Trie> * child = subSet->getChildren() ;

				for( set<Trie>::iterator itChild = child->begin(); itChild != child->end();)
				{

	                Trie * tmpT = const_cast< Trie * >( &(*itChild ) );
					del = genCand(  tmpT, itChildbdp->getChildren(),tr );

					if( del && ( itChild->getChildren() == 0 || itChild->getChildren()->empty() ) )
					{
					    Trie * tmpT = const_cast< Trie * >( &(*itChild ) );
						tmpT->deleteChildren() ;

						tmpIt = itChild ++ ;

						child->erase( tmpIt ) ;

					}
					else
						itChild++ ;

				}
				return del ;			
			}
		}
		else
		{
			if( itChildbdp->getId() < subSet->getId() )
			{
				del = genCand(  subSet, itChildbdp->getChildren(), tr  );
			}
			else
				return del;

		}
	}
			
  return del;

}
