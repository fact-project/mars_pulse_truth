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
!   Author(s): Thomas Bretz  1/2002 <mailto:tbretz@uni-sw.gwdg.de>
!   Author(s): Wolfgang Wittek 1/2002 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MEnergyEstimate
//
// Task to estimate the energy by a rule, eg:
//
// MEnergyEstimate est;
// est.SetRule("0.5 + (1.1*MHillas.fLength) + (2.2*MHillasSrc.fDist) + (3.3*MHillas.fSize) +"
//             "(4.4*MHillas.fSize*MHillas.fLength) + (5.5*MHillasSrc.fDist*MHillas.fLength)");
//
// For description of rules, see MDataPhrase.
//
// The default rule is "MMcEvt.fEnergy"
//
// Output:
//   MEnergyEst [MParameterD]
//
/////////////////////////////////////////////////////////////////////////////
#include "MEnergyEstimate.h"

ClassImp(MEnergyEstimate);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Initialize fData with default rule "MMcEvt.fEnergy"
//
MEnergyEstimate::MEnergyEstimate(const char *name, const char *title)
    : MParameterCalc("MMcEvt.fEnergy")
{
    fName  = name  ? name  : "MEnergyEstimate";
    fTitle = title ? title : "Task to estimate the energy by a rule";

    SetNameParameter("MEnergyEst");
}
