/*----------------------------------------------------------------------
  File    : apriori.h
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
#ifndef APRIRORI_H
#define APRIRORI_H

#include <iostream>
using namespace std;
#define MAXIMAL

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "tract.h"
#include "istree.h"
#include "Application.h"

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define PRGNAME     "fim/apriori"
#define DESCRIPTION "frequent item sets miner for FIMI 2003"
#define VERSION     "version 1.7 (2003.12.02)         " \
                    "(c) 2003   Christian Borgelt"

/* --- error codes --- */
#define E_OPTION    (-5)        /* unknown option */
#define E_OPTARG    (-6)        /* missing option argument */
#define E_ARGCNT    (-7)        /* too few/many arguments */
#define E_SUPP      (-8)        /* invalid minimum support */
#define E_NOTAS     (-9)        /* no items or transactions */
#define E_UNKNOWN  (-18)        /* unknown error */

#ifndef QUIET                   /* if not quiet version */
#define MSG(x)        x         /* print messages */
#else                           /* if quiet version */
#define MSG(x)                  /* suppress messages */
#endif

#define SEC_SINCE(t)  ((clock()-(t)) /(double)CLOCKS_PER_SEC)
#define RECCNT(s)     (tfs_reccnt(is_tfscan(s)) \
                      + ((tfs_delim(is_tfscan(s)) == TFS_REC) ? 0 : 1))
#define BUFFER(s)     tfs_buf(is_tfscan(s))

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/
#ifndef QUIET                   /* if not quiet version */

/* --- error messages --- */
static const char *errmsgs[] = {
  /* E_NONE      0 */  "no error\n",
  /* E_NOMEM    -1 */  "not enough memory\n",
  /* E_FOPEN    -2 */  "cannot open file %s\n",
  /* E_FREAD    -3 */  "read error on file %s\n",
  /* E_FWRITE   -4 */  "write error on file %s\n",
  /* E_OPTION   -5 */  "unknown option -%c\n",
  /* E_OPTARG   -6 */  "missing option argument\n",
  /* E_ARGCNT   -7 */  "wrong number of arguments\n",
  /* E_SUPP     -8 */  "invalid minimal support %d\n",
  /* E_NOTAS    -9 */  "no items or transactions to work on\n",
  /*    -10 to -15 */  NULL, NULL, NULL, NULL, NULL, NULL,
  /* E_ITEMEXP -16 */  "file %s, record %d: item expected\n",
  /* E_DUPITEM -17 */  "file %s, record %d: duplicate item %s\n",
  /* E_UNKNOWN -18 */  "unknown error\n"
};
#endif

/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/
#ifndef QUIET
static char    *prgname;        /* program name for error messages */
#endif
static ITEMSET *itemset = NULL; /* item set */
static TASET   *taset   = NULL; /* transaction set */
static TATREE  *tatree  = NULL; /* transaction tree */
static ISTREE  *istree  = NULL; /* item set tree */
static FILE    *in      = NULL; /* input  file */
static FILE    *out     = NULL; /* output file */


extern "C" TATREE * apriori( char*fn_in, char*fn_out, int supp, int & level,
							 Trie * bdPapriori, Trie * bdn, set<Element> * relist, double ratioNfC, double & eps, int ismax,
                             vector< unsigned int > * stat, int & maxBdP, bool & generatedFk, bool verbose  ) ;

#endif
