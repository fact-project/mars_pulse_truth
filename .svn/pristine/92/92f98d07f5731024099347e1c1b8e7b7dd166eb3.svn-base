// @(#)root/tree:$Id: MBufferSQL.cxx 43518 2012-03-28 01:04:07Z pcanal $
// Author: Philippe Canal and al. 08/2004

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MBufferSQL                                                           //
//                                                                      //
// Implement TBuffer for a SQL backend                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "MBufferSQL.h"

#include <stdio.h>
#include <stdlib.h>

#include "TError.h"

#include "TSQLResult.h"
#include "TSQLRow.h"

#include "MTime.h"

using namespace std;

ClassImp(MBufferSQL);

//________________________________________________________________________
MBufferSQL::MBufferSQL() : TBufferFile(), fRowPtr(0)
{
   // Constructor.
}

//________________________________________________________________________
MBufferSQL::MBufferSQL(TSQLRow **r, Int_t index, TString type) :
TBufferFile(TBufferFile::kRead), fRowPtr(r), fIndex(index), fIsDate(kFALSE)
{
    // Constructor.
    if (type.BeginsWith("date", TString::kIgnoreCase) ||
        type.BeginsWith("time", TString::kIgnoreCase))
        fIsDate = kTRUE;
}

//________________________________________________________________________
void MBufferSQL::ReadDouble(Double_t &d)
{
   // Operator>>
   const char *ptr = (*fRowPtr)->GetField(fIndex);

   if (ptr==0)
   {
       d = 0;
       Error("operator>>(Double_t&)","NULL value found in cell");
       return;
   }

   if (!fIsDate)
   {
       d = atof(ptr);
       return;
   }

   d = MTime(ptr).GetAxisTime();
}
