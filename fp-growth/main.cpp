/*----------------------------------------------------------------------
  File    : main.cpp
  Contents: process the input parameters and launch ABS
  Author  : Frédéric Flouvat
----------------------------------------------------------------------*/

#include "Zigzag.h"
#include <iostream>
#include <time.h>


int main(int argc, char *argv[])
{

  if(argc < 3) {
    cerr << "usage: " << argv[0] << " datafile minsup output" << endl;
   }
  else {

    Zigzag a;
    int sets;
    
    double ratio = 0.01;
    double err = 0.001 ;
    bool verbose = false;
    
    if( argc > 4 && strcmp( argv[ 4 ], "-o") == 0 )
    {
        
        ratio = atof( argv[5] ) ;
        
        if( ratio > 1)
                a.setLevel( (int)( ratio) ) ;  
        else              
                a.setLevel( -1 );

       	err = atof( argv[6] ) ;
       	
       	if( argc == 8 && strcmp( argv[ 7 ], "v" ) == 0 )
       	{

       	     verbose = true ;  	        
       	     a.setVerbose(); // print information
      	}          
    }
    else
        a.setLevel( -1 );


   	a.setRatioNfC( ratio ) ;
       	
   	a.setEps( err ) ; 
    
    a.setData(argv[1]);

    a.setMinSup(atoi(argv[2]));	
    
    if( argc >= 4 )	
    	a.setSaveFile( argv[3] ) ;
		    
    clock_t start = clock();

    sets = a.run() ;

    if( verbose ) cout << sets << "\t[" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

  }

  return 0;
}

