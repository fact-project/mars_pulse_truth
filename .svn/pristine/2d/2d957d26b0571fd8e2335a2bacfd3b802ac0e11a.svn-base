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
!   Author(s): Markus Gaug 02/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
//  MHCalibrationTestPix
//
//  Histogram class for the charge calibration. 
//  Stores and fits the charges and stores the location of the maximum FADC 
//  slice. Tests are taken from MExtractedSignalPix.
//
//////////////////////////////////////////////////////////////////////////////
#include "MHCalibrationTestPix.h"

#include <TH1.h>
#include <TF1.h>

#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TGraph.h>

#include "MH.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHCalibrationTestPix);

using namespace std;

const Int_t   MHCalibrationTestPix::fgChargeNbins     = 4000;
const Axis_t  MHCalibrationTestPix::fgChargeFirst     = -0.5;
const Axis_t  MHCalibrationTestPix::fgChargeLast      = 39999.5;
// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Sets: 
// - the default number for fNbins        (fgChargeNbins)
// - the default number for fFirst        (fgChargeFirst)
// - the default number for fLast         (fgChargeLast)
//
// - the default name of the  fHGausHist ("HCalibrationTest")
// - the default title of the fHGausHist ("Distribution of calibrated FADC slices Pixel ")
// - the default x-axis title for fHGausHist ("Sum FADC Slices")
// - the default y-axis title for fHGausHist ("Nr. of events")
//
// Calls:
// - Clear();
//
MHCalibrationTestPix::MHCalibrationTestPix(const char *name, const char *title)
{ 
  
  fName  = name  ? name  : "MHCalibrationTestPix";
  fTitle = title ? title : "Statistics of the calibrated FADC sums of calibration events";

  SetNbins ( fgChargeNbins );
  SetFirst ( fgChargeFirst );
  SetLast  ( fgChargeLast  );

  fHGausHist.SetName("HCalibrationTest");
  fHGausHist.SetTitle("Distribution of calibrated Photons Pixel ");  
  fHGausHist.SetXTitle("Nr. Photons");
  fHGausHist.SetYTitle("Nr. of events");

}


// --------------------------------------------------------------------------
//
// returns fHGausHist.Integral("width")
//
const Float_t MHCalibrationTestPix::GetIntegral() const 
{ 
   return fHGausHist.Integral("width");  
}

