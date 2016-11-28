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
!   Author(s): Josep Flix 1/2004 <mailto:jflix@ifae.es>
!   Author(s): Javier Rico 1/2004 <mailto:jrico@ifae.es>
!   Author(s): Markus Gaug 4/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MPedPhotCalc
//
//  This is a Task class to compute, for each pixel, the signal mean and
//  rms from a pedestal run file that has undergone the standard signal
//  extration  and calibration procedure. The signal rms can be used as
//  reference to compute the significance of the measured signals in the
//  following data runs (e.g. during the image cleaning).
//
//  Input Containers:
//   MSignalCam
//
//  Output Containers:
//   MPedPhotCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MPedPhotCalc.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MRawRunHeader.h"

#include "MSignalPix.h"
#include "MSignalCam.h"

#include "MPedPhotPix.h"
#include "MPedPhotCam.h"

#include "MBadPixelsPix.h"
#include "MBadPixelsCam.h"

ClassImp(MPedPhotCalc);

using namespace std;

//
// Default constructor
//
MPedPhotCalc::MPedPhotCalc(const char *name, const char *title)
    : fPedestals(NULL), fCerPhot(NULL), fBadPixels(NULL)
{
  fName  = name  ? name  : "MPedPhotCalc";
  fTitle = title ? title : "Task to calculate pedestals in units of photons";
}

// --------------------------------------------------------------------------
//
// Look for the following input containers:
//
//  - MSignalCam
//  - MBadPixelsCam
// 
// The following output containers are also searched and created if
// they were not found:
//
//  - MPedPhotCam
//
Int_t MPedPhotCalc::PreProcess( MParList *pList )
{      
  // Look for input container
  fCerPhot = (MSignalCam*)pList->FindObject("MSignalCam");
  if (!fCerPhot)
    {
      *fLog << err << "MSignalCam not found... aborting." << endl;
      return kFALSE;
    }


  fBadPixels = (MBadPixelsCam*)pList->FindObject("MBadPixelsCam");
  if (!fBadPixels)
      *fLog << warn << "WARNING - MBadPixelsCam not found... ignored." << endl;

  // Create output container
  fPedestals = (MPedPhotCam*)pList->FindCreateObj("MPedPhotCam");
  if (!fPedestals)
    return kFALSE;
  
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// The ReInit searches for the following input containers:
//  - MRawRunHeader
//
// It also initializes the data arrays fSumx and fSumx2 
// (only for the first read file)
// 
Bool_t MPedPhotCalc::ReInit(MParList *pList)
{
  // Initialize arrays
  if(fSumx.GetSize()==0)
  {
      const UShort_t num = fPedestals->GetSize();

      fSumx.Set(num);
      fSumx2.Set(num);

      memset(fSumx.GetArray(),  0, sizeof(Float_t)*num);
      memset(fSumx2.GetArray(), 0, sizeof(Float_t)*num);
  }


  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Store the measured number of photons in arrays fSumx and fSumx2
// so that we can calculate the mean and rms in the PostProcess()
//
Int_t MPedPhotCalc::Process()
{
    const UInt_t n = fCerPhot->GetNumPixels();
    for(UInt_t idx=0; idx<n; idx++)
    {
       const Float_t nphot = (*fCerPhot)[idx].GetNumPhotons();
       
       fSumx[idx]  += nphot;
       fSumx2[idx] += nphot*nphot;
    }

    fPedestals->SetReadyToSave();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Compute mean and rms of the measured charge distribution (in photons)
//
Int_t MPedPhotCalc::PostProcess()
{
    // Compute pedestals and rms from fSumx and fSumx2 arrays
    const Int_t n    = GetNumExecutions();
    const Int_t npix = fSumx.GetSize();

    for(Int_t i=0; i<npix; i++)
    {

      if (fBadPixels)
        {
          const MBadPixelsPix &bad = (*fBadPixels)[i];
          if (bad.IsBad())
            continue;
        }

      const Float_t sum  = fSumx[i];
      const Float_t sum2 = fSumx2[i];
      
      const Float_t photped = sum/n;
      const Float_t photrms = TMath::Sqrt((sum2-sum*sum/n)/(n-1.));
      
      (*fPedestals)[i].Set(photped,photrms);
    }

    return kTRUE;
}
