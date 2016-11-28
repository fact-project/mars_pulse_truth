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
!   Author(s): Unknown
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MMcFadcHeader
//
// This class contains the MonteCarlo information
// of the FADC simulation for the current run.
// The information is saved only once, whatever the
// number of events is
//
// NOTE : meaning of fAmplFadc, fAmplFadcOuter changed in camera 0.7,
// 30/03/2004: before it was amplitude of (gaussian) pulse, now is
// integral of pulse (which may be gaussian or not).
//
// In camera 0.7, the meaning of fPedesSigmaHigh, fPedesSigmaLow changed:
// before it was the rms of the single FADC slice. Now we calculate the
// RMS of the distribution of the sum of 14 FADC slices. The value we set
// as fPedesSigmaHigh/Low is that RMS divided by sqrt(14). It can be seen
// that the fluctuations of the integrated pedestal, when adding n slices
// to obtain the pixel signal, with n>~6, is more or less well
// approximated by sqrt(n)*RMS(sum_14)slices)/sqrt(14).
//
// Version 5:
//   Added member fGainFluctuations
//
// Version 6:
//   Added member fNoiseGainFluctuations
//
// Version 7:
//   Derived class from MCamEvent
//
// Version 9:
//   Added member fElecNoiseFileName
//
//////////////////////////////////////////////////////////////////////////////
#include "MMcFadcHeader.hxx"

#include <iostream>

ClassImp(MMcFadcHeader);

using namespace std;

MMcFadcHeader::MMcFadcHeader(const char *name, const char *title) {
  //
  //  default constructor

  fName  = name  ? name  : "MMcFadcHeader";
  fTitle = title ? title : "Fadc Header Information from Monte Carlo";

  //  set all values to zero

  Int_t i;

  fFadcShape=0.0; 
  fAmplFadc=MFADC_RESPONSE_INTEGRAL;
  fFwhmFadc=MFADC_RESPONSE_FWHM;
  fAmplFadcOuter=MFADC_RESPONSE_INTEGRAL;
  fFwhmFadcOuter=MFADC_RESPONSE_FWHM;

  for(i=0;i<MFADC_CHANNELS;i++){
    fPedesMean[i]= 0.0    ;
    fPedesSigmaHigh[i]=-1.0    ;
    fPedesSigmaLow[i]=-1.0    ;
    fElecNoise[i]=-1.0   ;
    fDigitalNoise[i]=-1.0   ;
  }
}

void MMcFadcHeader::Print(Option_t *Option) const {
  //
  //  print out the data member on screen
  //
  cout << endl;
  cout << "Monte Carlo Fadc output:" << endl;
  cout << " Shape type of the signal: "       << fFadcShape << endl;
  cout << " FADC integral for sphe [counts*ns]: " << fAmplFadc << endl;
  cout << " Width of the signal in nsec: "    << fFwhmFadc << endl;
  cout << " Outer FADC integral for sphe [counts*ns]: " << fAmplFadcOuter
       << endl;
  cout << " Width of the signal in nsec for outer: "    << fFwhmFadcOuter 
       << endl;
  cout << " Pedestals and ElecNoise in fadc counts: " << endl;
  for (int i=0;i<MFADC_CHANNELS;i++){
    cout << " Pixel "<<i<<": "<<fPedesMean[i]<<"  "<<fElecNoise[i]<<endl;
  }
  cout << endl ; 
}

Bool_t MMcFadcHeader::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if ((UInt_t)idx>=GetNumPixel())
        return kFALSE;

    switch (type)
    {
    case 0:
        val = fPedesMean[idx];
        break;
    case 1:
        val = fPedesSigmaHigh[idx];
        break;
    case 2:
        val = fPedesSigmaLow[idx];
        break;
    case 3:
        val = fElecNoise[idx];
        break;
    case 4:
        val = fDigitalNoise[idx];
        break;
    default:
        return kFALSE;
    }
    return kTRUE;
}
