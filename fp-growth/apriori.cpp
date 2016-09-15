/*----------------------------------------------------------------------
  File    : apriori.c
  Contents: apriori algorithm for finding frequent item sets
            (specialized version for FIMI 2003 workshop)
  Author  : Christian Borgelt
  History : 15.08.2003 file created from normal apriori.c
            16.08.2003 parameter for transaction filtering added
            18.08.2003 dynamic filtering decision based on times added
            21.08.2003 transaction sort changed to heapsort
            20.09.2003 output file made optional
----------------------------------------------------------------------*/
/*
  Modified by : Frédéric Flouvat
  Modifications : store the positive and negative border into an 
                  an input trie for ABS
                  process stastical informations on dataset to stop
                  the apriori classical iterations                  
  Author  : Frédéric Flouvat
----------------------------------------------------------------------*/

#include "apriori.h"

/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/

static void error (int code, ...)
{                               /* --- print an error message */
  #ifndef QUIET                 /* if not quiet version */
  va_list    args;              /* list of variable arguments */
  const char *msg;              /* error message */

  assert(prgname);              /* check the program name */
  if (code < E_UNKNOWN) code = E_UNKNOWN;
  if (code < 0) {               /* if to report an error, */
    msg = errmsgs[-code];       /* get the error message */
    if (!msg) msg = errmsgs[-E_UNKNOWN];
    fprintf(stderr, "\n%s: ", prgname);
    va_start(args, code);       /* get variable arguments */
    vfprintf(stderr, msg, args);/* print error message */
    va_end(args);               /* end argument evaluation */
  }
  #endif
  #ifndef NDEBUG                /* if debug version */
  if (istree)  ist_delete(istree);
  if (tatree)  tat_delete(tatree);
  if (taset)   tas_delete(taset, 0);
  if (itemset) is_delete(itemset);
  if (in)      fclose(in);      /* clean up memory */
  if (out)     fclose(out);     /* and close files */
  #endif
  exit(code);                   /* abort the program */
}  /* error() */

/*--------------------------------------------------------------------*/

TATREE * apriori( char*fn_in, char*fn_out, int supp, int & level, Trie * bdPapriori, 
					Trie * bdn , set<Element> * relist , double ratioNfC, double & eps,int ismax,
                    vector< unsigned int > * stat, int & maxBdP, bool & generatedFk, bool verbose ) 
{
  int     i, k, n;              /* loop variables, counters */
  int     tacnt   = 0;          /* number of transactions */
  int     max     = 0;          /* maximum transaction size */
  int     empty   = 1;          /* number of empty item sets */
  int     *map, *set;           /* identifier map, item set */
  char    *usage;               /* flag vector for item usage */
  clock_t t, tt, tc, x;         /* timer for measurements */

  double actNfC = 1 ;
  double avgNfC = 0 ;
  int nbgen = 0 ;
  int nbfreq = 0 ;
  level = 1 ;
  bool endApriori = false ;   // boolean to stop the initial classial apriori approach
  int bdnsize = 0 ;    // number of itemsets found infrequent

	/* --- create item set and transaction set --- */  
  itemset = is_create();        /* create an item set and */
  if (!itemset) error(E_NOMEM); /* set the special characters */
  taset = tas_create(itemset);  /* create a transaction set */
  if (!taset) error(E_NOMEM);   /* to store the transactions */
  if( verbose ) MSG(fprintf(stderr, "\n"));   /* terminate the startup message */

  /* --- read transactions --- */
  if( verbose )MSG(fprintf(stderr, "reading %s ... ", fn_in));
  t  = clock();                 /* start the timer and */
  in = fopen(fn_in, "r");       /* open the input file */
  if (!in) error(E_FOPEN, fn_in);
  for (tacnt = 0; 1; tacnt++) { /* transaction read loop */
    k = is_read(itemset, in);   /* read the next transaction */
    if (k < 0) error(k, fn_in, RECCNT(itemset), BUFFER(itemset));
    if (k > 0) break;           /* check for error and end of file */
    k = is_tsize(itemset);      /* update the maximal */
    if (k > max) max = k;       /* transaction size */
    if (taset && (tas_add(taset, NULL, 0) != 0))
      error(E_NOMEM);           /* add the loaded transaction */
  }                             /* to the transaction set */
  fclose(in); in = NULL;        /* close the input file */
  n  = is_cnt(itemset);         /* get the number of items */
  if( verbose ) MSG(fprintf(stderr, "[%d item(s),", n));
  if( verbose ) MSG(fprintf(stderr, " %d transaction(s)] done ", tacnt));
  if( verbose ) MSG(fprintf(stderr, "[%.2fs].\n", SEC_SINCE(t)));

  /* --- sort and recode items --- */
  if( verbose ) MSG(fprintf(stderr, "sorting and recoding items ... "));
  t   = clock();                /* start the timer */
  map = (int*)malloc(is_cnt(itemset) *sizeof(int));
  if (!map) error(E_NOMEM);     /* create an item identifier map */
  n = is_recode(itemset, supp, 2, map);       /* 2: sorting mode */
  tas_recode(taset, map, n);    /* recode the loaded transactions */
  max = tas_max(taset);         /* get the new maximal t.a. size */

  // use in the other part of the implementation to have the corresponding
  // identifiant to an internal id
  stat->reserve( n+2 ) ;
  stat->push_back( 0 ) ;
  for(int j= 0; j< n ; j++ )
  {
    stat->push_back( 0 ) ;
	relist->insert(  Element( atoi( is_name( itemset, j )  ) ,j) );
  } 

  if( verbose ) MSG(fprintf(stderr, "[%d item(s)] ", n));
  if( verbose ) MSG(fprintf(stderr, "done [%.2fs].\n", SEC_SINCE(t)));

  /* --- create a transaction tree --- */
  if( verbose ) MSG(fprintf(stderr, "creating transaction tree ... "));
  t = clock();                  /* start the timer */
  tatree = tat_create(taset,1); /* create a transaction tree */
  if (!tatree) error(E_NOMEM);  /* (compactify transactions) */
  tt = clock() -t;              /* note the construction time */
  if( verbose ) MSG(fprintf(stderr, "done [%.2fs].\n", SEC_SINCE(t)));

  /* --- create an item set tree --- */
  if( verbose ) MSG(fprintf(stderr, "checking subsets of size 1"));
  t = clock(); tc = 0;          /* start the timer and */
  istree = ist_create(n, supp); /* create an item set tree */
  if (!istree) error(E_NOMEM);
  for (k = n; --k >= 0; )       /* set single item frequencies */
    ist_setcnt(istree, k, is_getfrq(itemset, k));
  ist_settac(istree, tacnt);    /* set the number of transactions */
  usage = (char*)malloc(n *sizeof(char));
  if (!usage) error(E_NOMEM);   /* create a item usage vector */

  /* --- check item subsets --- */
  while (ist_height(istree) < max   && ( ( ismax == -1 &&  endApriori == false )
          ||  ist_height(istree) < ismax ) 
          )
  {
	nbgen = 0 ;
	nbfreq = 0 ;

	level ++ ;

	i = ist_check(istree,usage);/* check current item usage */

    if (i < max) max = i;       /* update the maximum set size */
    if (ist_height(istree) >= i) break;

    k = ist_addlvl(istree, nbgen);     /* while max. height is not reached, */

    if (k <  0) error(E_NOMEM); /* add a level to the item set tree */
    if (k != 0) break;          /* if no level was added, abort */
    if( verbose ) MSG(fprintf(stderr, " %d", ist_height(istree)));
    if ((i < n)                 /* check item usage on current level */
    &&  (i *(double)tt < 0.1 *n *tc)) {
      n = i; x = clock();       /* if items were removed and */
      tas_filter(taset, usage); /* the counting time is long enough, */
      tat_delete(tatree);       /* remove unnecessary items */
      tatree = tat_create(taset, 1);
      if (!tatree) error(E_NOMEM);
      tt = clock() -x;          /* rebuild the transaction tree and */
    }                           /* note the new construction time */
    x = clock();                /* start the timer */

    ist_countx(istree, tatree, nbfreq, istree->supp ); /* count the transaction tree */

    tc = clock() -x;            /* in the item set tree */

	actNfC = 1-double(nbfreq)/double(nbgen) ;
	avgNfC =  avgNfC + actNfC ;
	
	if( verbose )
    {    
        cout<<" \t Fk : "<<nbfreq<<" Ck : "<<nbgen<<" NFk/Ck "<<actNfC<<" avg NFk/Ck "<<avgNfC/(level-1)<<endl;        
    }    
    
    bdnsize += nbgen - nbfreq ;
    
    if( level >=4 && ( bdnsize / nbgen < 1.5 ) && ( bdnsize > 100 ) )
    {        
        if( actNfC < ratioNfC )
        {
            eps = 0 ; 
            endApriori = true ;           
        }    
        else if( actNfC > 0.25 )
                endApriori = true ;    
                 
    }            
	
			 
  }                             /* and note the new counting time */
  if( verbose ) MSG(fprintf(stderr, " done [%.2fs].\n", SEC_SINCE(t)));

  /* --- filter item sets --- */
  t = clock();                  /* start the timer */
  #ifdef MAXIMAL                /* filter maximal item sets */
  if( verbose ) MSG(fprintf(stderr, "filtering maximal item sets ... "));

  if( ratioNfC == 0 || nbgen < k+1 || ist_height(istree)>= max )
    ist_filter2(istree, IST_MAXFRQ, 0);
  else
    ist_filter2(istree, IST_MAXFRQ, bdn);
  
  if( verbose ) MSG(fprintf(stderr, " done [%.2fs].\n", SEC_SINCE(t)));
  empty = (n <= 0) ? 1 : 0;     /* check whether the empty item set */
  #endif                        /* is maximal */
  #ifdef CLOSED                 /* filter closed item sets */
  if( verbose ) MSG(fprintf(stderr, "filtering closed item sets ... "));
  ist_filter(istree, IST_CLOSED);
  if( verbose ) MSG(fprintf(stderr, " done [%.2fs].\n", SEC_SINCE(t)));
  for (k = n; --k >= 0; )       /* check for an item in all t.a. */
    if (is_getfrq(itemset, k) == tacnt) break;
  empty = (k <= 0) ? 1 : 0;     /* check whether the empty item set */
  #endif                        /* is closed */

 
	  /* --- print item sets --- */
	  for (i = ist_height(istree); --i >= 0; )
		map[i] = 0;                 /* clear the item set counters */
	  if( verbose ) MSG(fprintf(stderr, "writing %s ... ", (fn_out) ? fn_out : "<none>"));
	  t = clock();                  /* start the timer and */
	  if (fn_out) {                 /* if an output file is given, */
		out = fopen(fn_out, "w");   /* open the output file */
		if (!out) error(E_FOPEN, fn_out);
		if (empty) fprintf(out, " (%d)\n", tacnt);
	  }                             /* report empty item set */
	  ist_init(istree);             /* init. the item set extraction */
	  set = is_tract(itemset);      /* get the transaction buffer */
	  for (n = empty; 1; n++) {     /* extract item sets from the tree */
		
		k = ist_set(istree, set, &supp);

		if (k <= 0) break;          /* get the next frequent item set */
		map[k-1]++;                 /* count the item set */
		if (fn_out) {               /* if an output file is given */
		  for (i = 0; i < k; i++) { /* traverse the items */
			fputs(is_name(itemset, set[i]), out);
			fputc(' ', out);        /* print the name of the next item */
		  }                         /* followed by a separator */
		  fprintf(out, "(%d)\n", supp);
		}                           /* print the item set's support */
		else
		{
		   short unsigned * is = new short unsigned[k] ;
			
		   for (i = 0; i < k; i++)  /* traverse the items */
		   {
				is[i] = set[i] ;
		   }
			if( k < level || nbgen < k+1 || ist_height(istree)>= max )
			{
				bdPapriori->insert(is, k ,supp ) ;
									        			  
                (*stat)[ 0 ] ++;
                (*stat)[ k+1 ]++;
                
                if( maxBdP < k ) 
						maxBdP = k ;

			}	
            else
            {
                generatedFk = true ;
                
               
		    }    
			
		   		   
		   delete[] is;
		               
		}
	  }
	  if (fn_out) {                 /* if an output file is given */
		if (fflush(out) != 0) error(E_FWRITE, fn_out);
		if (out != stdout) fclose(out);
		out = NULL;                 /* close the output file */
	  }
	  if( verbose ) MSG(fprintf(stderr, "[%d set(s)] done ", n));
	  if( verbose ) MSG(fprintf(stderr, "[%.2fs].\n", SEC_SINCE(t)));


  /* --- print item set statistics --- */
  k = ist_height(istree);       /* find last nonzero counter */
  if ((k > 0) && (map[k-1] <= 0)) k--;
  if( verbose ){
      printf("%d\n", empty);        /* print the numbers of item sets */
      for (i = 0; i < k; i++) printf("%d\n", map[i]);
  }
 
  /* --- clean up --- */
  #ifndef NDEBUG                /* if this is a debug version */
  free(usage);                  /* delete the item usage vector */
  free(map);                    /* and the identifier map */
  ist_delete(istree);           /* delete the item set tree, */

  if (taset)  tas_delete(taset, 0);   /* the transaction set, */
  is_delete(itemset);                 /* and the item set */
  #endif

  return tatree ;

}
