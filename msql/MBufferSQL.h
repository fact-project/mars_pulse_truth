// @(#)root/tree:$Id: MBufferSQL.h 25450 2008-09-18 21:13:42Z pcanal $
// Author: Philippe Canal 2005

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_MBufferSQL
#define ROOT_MBufferSQL

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MBufferSQL                                                           //
//                                                                      //
// Implement TBuffer for a SQL backend                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TBufferFile
#include "TBufferFile.h"
#endif
#ifndef ROOT_TString
#include "TString.h"
#endif


class TSQLResult;
class TSQLRow;

class MBufferSQL : public TBufferFile
{
private:
    TSQLRow **fRowPtr;      //!
    Int_t     fIndex;
    Bool_t    fIsDate;

   // TBuffer objects cannot be copied or assigned
    MBufferSQL(const MBufferSQL &);        // not implemented
    void operator=(const MBufferSQL &);    // not implemented

public:
    MBufferSQL();
    MBufferSQL(TSQLRow **row, Int_t index, TString type);

    void ReadDouble(Double_t &d);

    ClassDef(MBufferSQL, 1); // Implementation of TBuffer to load and write to a SQL database
};

#endif


