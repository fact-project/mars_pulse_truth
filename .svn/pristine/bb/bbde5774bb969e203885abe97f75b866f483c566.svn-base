/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz  04/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MData
//
//   This base class defines an interface to a generalized value.
//   This value can be a simple number, it can also be a data member
//   of a class or some kind of concatenation of MData objects.
//
//   A class inheriting from MData must implement:
//
//    - Double_t GetValue() const
//      which return the value corresponding to the object
//
//    - Bool_t IsValid() const
//      should tell whether the object is valid (eg if the object parses
//      a string the result might be invalid)
//
//    - Bool_t PreProcess(const MParList *plist)
//      which can be used to get some necessary data (befor processing)
//      from the parlist.
//
//    - TString GetRule()
//      returns the rule as a text which would recreate the same structure
//      when used in a MDataChain
//
//    - TString GetDataMember()
//      returns the names (seperated by a comma) used by this class. This
//      is mainly used for the AutoScheme when reading data from a file.
//      (s.MReadTree)
//
//    - void SetVariables(const TArrayD &arr)
//      is used to distribute variable numbers through lists holding
//      MDatas to its counterpart (mainly to MDataValue)
//
//   The 'must' ist represented by the =0 in the class header. In the C++
//   language this is called an abstract member function. Because the
//   class contains abstract member function which makes it impossible
//   to create an instance of this class one calls it also:
//   abstract base class
//
/////////////////////////////////////////////////////////////////////////////

#include "MData.h"

#include <fstream>

#include "MLog.h"

ClassImp(MData);

using namespace std;

Bool_t MData::AsciiWrite(ostream &out) const
{
    out << GetValue() << " ";
    return kTRUE;
}

void MData::Print(Option_t *opt) const
{
    *fLog << GetRule() << flush;
}
