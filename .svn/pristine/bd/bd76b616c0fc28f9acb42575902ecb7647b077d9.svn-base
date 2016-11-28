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
!   Author(s): Javier López , 4/2004 <mailto:jlopez@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
#include "MPSFFit.h"

#include <TEllipse.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MPSFFit);

using namespace std;

MPSFFit::MPSFFit(const char *name, const char *title)
{

    fName  = name  ? name  : "MPSFFit";
    fTitle = title ? title : "Container that holds the values of the PSF fit.";

    fMaximun = -666.; 
    fMeanMinorAxis = -666.; 
    fMeanMajorAxis = -666.; 
    fSigmaMinorAxis = -666.; 
    fSigmaMajorAxis = -666.; 
    fChisquare = -666.; 
}

void MPSFFit::Print(Option_t *opt) const
{

    *fLog << all << GetDescriptor() << " " << GetTitle() << endl;

    *fLog << all << " Maximun \t " << fMaximun << " uA" << endl;
    *fLog << all << " Mean Minor Axis \t " << fMeanMinorAxis << " mm \t";
    *fLog << all << " Sigma Minor Axis \t " << fSigmaMinorAxis << " mm" << endl;
    *fLog << all << " Mean Major Axis \t " << fMeanMajorAxis << " mm \t";
    *fLog << all << " Sigma Major Axis \t " << fSigmaMajorAxis << " mm" << endl;
    *fLog << all << " Chi Square \t " << fChisquare;
    *fLog << all << endl;

}

// --------------------------------------------------------------------------
//
// Paint the ellipse corresponding to the parameters
//
void MPSFFit::Paint(Option_t *opt)
{
    if (fSigmaMinorAxis<0 || fSigmaMajorAxis<0)
        return;

    TEllipse e(fMeanMinorAxis, fMeanMajorAxis, fSigmaMinorAxis, fSigmaMajorAxis, 0, 360, 0);
    e.SetLineWidth(2);
    e.Paint();
}

void MPSFFit::Reset()
{
     SetMaximun(0.0);
     SetMeanMinorAxis(0.0);
     SetMeanMajorAxis(0.0);
     SetSigmaMinorAxis(0.0);
     SetSigmaMajorAxis(0.0);
     SetChisquare(0.0);
}
