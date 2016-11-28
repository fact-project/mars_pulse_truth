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
!   Author(s): Keiichi Mase 10/2004 <mailto:mase@mppmu.mpg.de>
!   Author(s): Markus Meyer 10/2004 <mailto:meyer@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MMuonCalibPar
//
// Storage Container for muon
//
//  This class holds some information for a calibration using muons. Muons
// are identified by using the class of the MMuonSearchParCalc. You can fill 
// these information by using the MMuonCalibParCalc. See also these class
// manuals.
//
// Class Version 2:
// ----------------
//   + Float_t fRelTimeMean;   // Result of the gaus fit to the arrival time
//   + Float_t fRelTimeSigma;  // Result of the gaus fit to the arrival time
//
/////////////////////////////////////////////////////////////////////////////
#include "MMuonCalibPar.h"

#include "MLog.h"
#include "MLogManip.h"

using namespace std;

ClassImp(MMuonCalibPar);

// --------------------------------------------------------------------------
//
// Default constructor.
//
MMuonCalibPar::MMuonCalibPar(const char *name, const char *title)
{
    fName  = name  ? name  : "MMuonCalibPar";
    fTitle = title ? title : "Parameters to calculate Muon calibration";

    Reset();
}

// --------------------------------------------------------------------------
//
void MMuonCalibPar::Reset()
{
//    fArcLength   = -1.;
    fArcPhi      = -1.;
    fArcWidth    = -1.;
    fChiArcPhi   = -1.;
    fChiArcWidth = -1.;
    fMuonSize    =  0.;
//    fEstImpact   = -1.;
    fPeakPhi     =  0.;

    fRelTimeMean    =  0;
    fRelTimeSigma   = -1;
}

void MMuonCalibPar::Print(Option_t *) const
{
    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Arc Phi        [deg] = " << fArcPhi      << endl;
    *fLog << " - Arc Width      [deg] = " << fArcWidth    << endl;
    *fLog << " - Red ChiSq Arc Phi    = " << fChiArcPhi   << endl;
    *fLog << " - Red ChiSq Arc Width  = " << fChiArcWidth << endl;
    *fLog << " - Size of muon   [phe] = " << fMuonSize    << endl;
    *fLog << " - Peak Phi       [deg] = " << fPeakPhi     << endl;
    *fLog << " - Rel.Time Mean   [ns] = " << fRelTimeMean  << endl;
    *fLog << " - Rel.Time Sigma  [ns] = " << fRelTimeSigma << endl;
}

