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
!   Author(s): Thomas Bretz 11/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2005-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MJTrainRanForest
//
// Base class for classes training a random forest
//
// The order when reading a file is:
//   1) Execution of PreTasks  (set by user)
//   2) Execution of PreCuts   (set by user)
//   3) Selector               (calculated from number of requested events)
//   4) Splitter               (if sample is split automatically in test/train)
//   5) PostTasks              (set by user)
//
// The split into Pre- and PostTasks is done for speed reason. So, if
// you calculate a vlue which is not needed for your PreCuts, you can
// calculate it afterwards, which will speed up execution.
//
/////////////////////////////////////////////////////////////////////////////
#include "MJTrainRanForest.h"

ClassImp(MJTrainRanForest);

using namespace std;
