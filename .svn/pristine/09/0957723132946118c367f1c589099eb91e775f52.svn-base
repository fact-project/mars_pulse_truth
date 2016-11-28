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
!   Author(s): Thomas Bretz    12/2000 <mailto:tbretz@atsro.uni-wuerzburg.de>
!   Author(s): Rudolf Bock     10/2001 <mailto:Rudolf.Bock@cern.ch>
!   Author(s): Wolfgang Wittek  6/2002 <mailto:wittek@mppmu.mpg.de>
!   Author(s): Oscar Blanch     3/2004 <mailto:blanch@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MConcentration
//
// Storage Container for Concentration parameters
//
//
// Version 1:
// ----------
// fConc[i]    [ratio] Number of photons in the i+1 more populated pixels
//                     over the event size (till i=7).
//
/////////////////////////////////////////////////////////////////////////////
#include "MConcentration.h"


#include <TArrayF.h>
#include <TArrayI.h>

#include "MHillas.h"

#include "MGeomPix.h"
#include "MGeomCam.h"

#include "MSignalPix.h"
#include "MSignalCam.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MConcentration);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MConcentration::MConcentration(const char *name, const char *title)
{
    fName  = name  ? name  : "MConcentration";
    fTitle = title ? title : "Storage container for concentrations";

    Reset();
}

// --------------------------------------------------------------------------
//
// Initializes the values with defaults. For the default values see the
// source code.
//
void MConcentration::Reset()
{
    for (int i=0; i<9; i++)
        fConc[i] = -1;
}

// --------------------------------------------------------------------------
//
// Print the Concetration Parameters to *fLog
//
void MConcentration::Print(Option_t *) const
{
    *fLog << all;
    *fLog << "Concentrations (" << GetName() << ")" << endl;
    for(int i=0;i<9;i++)
      *fLog << "Conc" << i+1 <<" = "<< fConc[i] << endl;
}

// --------------------------------------------------------------------------
//
// Calculate the Concentrations from a Cherenkov photon event
// assuming Cher.photons/pixel, their standard hillas parameters 
// and  pixel coordinates are given.
// In case you don't call Calc from within an eventloop make sure, that
// you call the Reset member function before.
//
// Returns:
//    Nothing.
//
Int_t MConcentration::Calc(const MGeomCam &geom, const MSignalCam &evt, const MHillas &hillas)
{
    Float_t maxpix[9] = {0,0,0,0,0,0,0,0,0};             // [#phot]

   const UInt_t npix = evt.GetNumPixels();
   for (UInt_t j=0; j<npix; j++)
   {
       const MSignalPix &pix = evt[j];
       if (!pix.IsPixelUsed())
           continue;

        const Double_t nphot = pix.GetNumPhotons()* geom.GetPixRatio(j);

	// Get number of photons in the 8 most populated pixels	
        if (maxpix[0]<=nphot)
        {
            for(int i=0;i<8;i++)
                maxpix[8-i]=maxpix[7-i];
            maxpix[0]=nphot;
            continue;
        }

	// Check if the latest value is 'somewhere in between'
        for (int i=0; i<8; i++)
        {
            if (nphot>=maxpix[7-i])
                continue;

            for(int k=0;k<i-1;k++)
                maxpix[7-k]=maxpix[6-k];                 // [#phot]

            maxpix[8-i]=nphot;
            break;
        }
    }

    // Compute concentrations from the 8 pixels with higher signal
    fConc[0]=maxpix[0];

    // No calculate the integral of the n highest pixels
    for(int i=1; i<8; i++)
        fConc[i] = fConc[i-1]+maxpix[i];

    for(int i=0; i<8; i++)
        fConc[i] /= hillas.GetSize();                    // [ratio]

    SetReadyToSave();

    return 0;
}
