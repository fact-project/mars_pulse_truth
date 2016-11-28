/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Tue Dec  9 12:05:18 CET 2003
    copyright            : (C) 2003 by Martin Merck
    email                : merck@astro.uni-wuerzburg.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is part of the MAGIC Software  * ***************************************************************************/

#ifndef GLOBALS_INCLUDED
#include "Globals.h"
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <stdlib.h>
#include "DCCurrentEvent.h"

using namespace std;

int gYear, gMonth, gDay;

void usage();

int main(int argc, char *argv[])
{
  if (argc != 3)
     usage();

  string dcFileName( argv[1] );
  string date = dcFileName.substr( dcFileName.rfind( "dc_" ) + 3, 10 );
  sscanf( date.c_str(), "%d_%d_%d", &gYear, &gMonth, &gDay);
  cout << gYear << ":" << gMonth << ":" << gDay << endl;
  
  
  std::ifstream input( argv[1] );
  ofstream output( const_cast<const char *> (argv[2]) );

  copy( istream_iterator<DCCurrentEvent>( input ),
        istream_iterator<DCCurrentEvent>(),
        ostream_iterator<DCCurrentEvent>( output, "\n") );

  return EXIT_SUCCESS;
}

void usage()
{
  cerr << "USAGE: dcconvertor <input file> <output file>" << endl;
  exit( EXIT_FAILURE );
}
  
