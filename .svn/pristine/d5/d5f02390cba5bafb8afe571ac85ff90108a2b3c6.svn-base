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
!   Author(s): Thomas Bretz  08/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// MIter                                                                    //
//                                                                          //
// This is the original MIter, but returns a MParContainer instead of a     //
// TObject.                                                                 //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#include "MIter.h"

ClassImp(MIter);

//______________________________________________________________________________
MIter::MIter(const MIter &iter)
{
   // Copy a MIter. This involves allocating a new MIterator of the right
   // sub class and assigning it with the original.

   if (iter.fIterator) {
      fIterator = iter.GetCollection()->MakeIterator();
      fIterator->operator=(*iter.fIterator);
   } else
      fIterator = 0;
}

//______________________________________________________________________________
MIter &MIter::operator=(const MIter &rhs)
{
   // Assigning an MIter to another. This involves allocatiing a new MIterator
   // of the right sub class and assigning it with the original.

   if (this != &rhs) {
      if (rhs.fIterator) {
         delete fIterator;
         fIterator = rhs.GetCollection()->MakeIterator();
         fIterator->operator=(*rhs.fIterator);
      }
   }
   return *this;
}
