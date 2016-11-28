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
!              Markus Meyer 10/2004 <mailto:meyer@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MMuonCalibParCalc
//
// Task to calculate the muon parameters
//
// This class allows you to get more muon information especially useful for
// the calibration of our telescope. This class store the information into the
// container of MMuonCalibPar. 
//
// In order to make this class work, we need the information of the arc
// center and the radius. Therefore, we need to use the task of 
// MMuonSearchParCalc.
// In the second step two histograms has to be filled for every muon.
// This is made by MHSingleMuon (look there for more information).
//
// The calculation of Muon parameters works as the followings;
//
//   MTaskList tlist;
//   MMuonSearchParCalc musearchcalc;
//   MFillH fillmuon("MHSingleMuon", "", "FillMuon");
//   MMuonCalibParCalc mucalibcalc;
//   tlist.AddToList(&musearchcalc);
//   tlist.AddToList(&fillmuon);
//   tlist.AddToList(&mucalibcalc);.
//
//
// For a faster coputation, pre cuts to select the candidates
// of muons for the calibration should be done. It is already implemented
// in star. You can set the values in star.rc.
//
// ### TODO ###
//  Up to now, in the histogram the error of the signal is estimated from
// the signal using a rough conversion factor and a F-factor and this values
// are global for all pixels. This is not the case for the real data. This
// value should be taken from some containers. In addition, the error of 
// the pedestal is not taken into accout. The error treatment should be
// done correctly.
//
//
//  Input Containers:
//   MGeomCam
//   MSignalCam
//   MMuonSearchPar
//
//  Output Containers:
//   MMuonCalibPar
//
//////////////////////////////////////////////////////////////////////////////

#include "MMuonCalibParCalc.h"

#include <TH1.h>
#include <TF1.h>
#include <TMinuit.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MSignalCam.h"

#include "MMuonCalibPar.h"
#include "MMuonSetup.h"
#include "MMuonSearchPar.h"
#include "MHSingleMuon.h"

using namespace std;

ClassImp(MMuonCalibParCalc);

static const TString gsDefName  = "MMuonCalibParCalc";
static const TString gsDefTitle = "Calculate new image parameters";

// -------------------------------------------------------------------------
//
// Default constructor. 
//
MMuonCalibParCalc::MMuonCalibParCalc(const char *name, const char *title)
//    : fEnableImpactCalc(kFALSE)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// -------------------------------------------------------------------------
//
Int_t MMuonCalibParCalc::PreProcess(MParList *pList)
{
    fGeomCam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fGeomCam)
    {
        *fLog << err << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }

    fHist = (MHSingleMuon*)pList->FindObject("MHSingleMuon");
    if (!fHist)
    {
        *fLog << err << "MHSingleMuon not found... abort." << endl;
        return kFALSE;
    }

    fMuonSetup = (MMuonSetup*)pList->FindObject("MMuonSetup");
    if (!fMuonSetup)
    {
        *fLog << err << "MMuonSetup not found... abort." << endl;
        return kFALSE;
    }

    fMuonCalibPar = (MMuonCalibPar*)pList->FindCreateObj("MMuonCalibPar");
    if (!fMuonCalibPar)
        return kFALSE;

    fMuonSearchPar = (MMuonSearchPar*)pList->FindCreateObj("MMuonSearchPar");
    if (!fMuonSearchPar)
        return kFALSE;

    return kTRUE;
}

// -------------------------------------------------------------------------
//
Int_t MMuonCalibParCalc::Process()
{
    // Calculation of ArcPhi, ArcWidth and MuonSize.
    const Float_t thresphi   = fMuonSetup->GetThresholdArcPhi();
    const Float_t threswidth = fMuonSetup->GetThresholdArcWidth();

    Double_t peakphi, arcphi;
    Double_t width, chi;

    if (!fHist->CalcPhi(thresphi, peakphi, arcphi))
        return kTRUE;

    if (!fHist->CalcWidth(threswidth, width, chi))
        return kTRUE;

    // Get Muon Size
    //fMuonCalibPar->SetMuonSize(fHist->GetHistPhi().Integral());

    fMuonCalibPar->SetMuonSize(fHist->CalcSize());

    // Calculate ArcPhi
    fMuonCalibPar->SetPeakPhi(peakphi);
    fMuonCalibPar->SetArcPhi(arcphi);

    fMuonCalibPar->SetTime(fHist->GetRelTimeMean(), fHist->GetRelTimeSigma());

//    const Float_t conv = TMath::RadToDeg()*fGeomCam->GetConvMm2Deg();
//    fMuonCalibPar->SetArcLength(fMuonCalibPar->GetArcPhi()
//                                *fMuonSearchPar->GetRadius()*conv);

    // Calculation of ArcWidth etc...
    fMuonCalibPar->SetChiArcWidth(chi);
    fMuonCalibPar->SetArcWidth(width);

    const Double_t rad = fMuonSearchPar->GetRadius()*fGeomCam->GetConvMm2Deg();
    const Double_t dev = fMuonSearchPar->GetDeviation()*fGeomCam->GetConvMm2Deg();

    // Check if this is a 'Write-Out' candidate
    if (arcphi>160 && rad>0.573  && rad<1.35 && dev<0.169)
        fMuonCalibPar->SetReadyToSave();

    return kTRUE;
}
