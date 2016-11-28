/* ======================================================================== *\
!
! *
! * This file is part of CheObs, the Modular Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appears in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz, 1/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHPhotonEvent
//
// This is a histogram class to visualize the contents of a MPhotonEvent.
//
// Histograms for the distribution in x/y (z is just ignored), the
// arrival direction, a profile of the arrival time vs position and
// a spectrum is filles and displayed.
//
// There are several types of histograms (in the sense of binnings)
// for several purposes:
//
// Type 1:
//   The maximum in x and y is determined from MCorsikaRunHeader
//   (not yet implemented. Fixed to 25000, 100 bins)
//
// Type 2:
//   The maximum in x and y is determined from MReflector->GetMaxR();
//   (100 bins)
//
// Type 3:
//   The maximum in x and y is determined from MGeomCam->GetMaxR();
//   (roughly 10 bins per pixel)
//
// Type 4:
//   Two times the traversal size of pixel #0 ( 4*MGeomCam[0].GetT() )
//   (101 bins, units cm)
//
// Type 5:
//   As type 3 but in cm instead of mm
//
// The binning is optimized using MH::FindGoodLimits.
//
// Fill expects a MPhotonEvent (the second argumnet in MFillH).
//
//
// Class Version 2:
// ----------------
//  + TH1D fHistH;
//
// Class Version 3:
// ----------------
//  + replaced TH1D by TH1F
//  + replaced TH2D by TH2F
//
/////////////////////////////////////////////////////////////////////////////
#include "MHPhotonEvent.h"

#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MBinning.h"
#include "MParList.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MPhotonEvent.h"
#include "MPhotonData.h"

#include "MCorsikaRunHeader.h"

#include "MReflector.h"

ClassImp(MHPhotonEvent);

using namespace std;

void MHPhotonEvent::Init(const char *name, const char *title)
{
    fName  = name  ? name  : "MHPhotonEvent";
    fTitle = title ? title : "Histogram to display the information of MPhotonEvents";

    fHistXY.SetName("Position");
    fHistXY.SetTitle("Histogram of position x/y");
    fHistXY.SetXTitle("X [cm]");
    fHistXY.SetYTitle("Y [cm]");
    fHistXY.SetZTitle("Counts");
    fHistXY.SetDirectory(NULL);
    fHistXY.Sumw2();

    fHistUV.SetName("Direction");
    fHistUV.SetTitle("Histogram of arrival direction CosU/CosV");
    fHistUV.SetXTitle("CosU");
    fHistUV.SetYTitle("CosV");
    fHistUV.SetZTitle("Counts");
    fHistUV.SetDirectory(NULL);
    fHistUV.Sumw2();

    fHistT.SetName("Time");
    fHistT.SetTitle("Time profile in x/y");
    fHistT.SetXTitle("X [cm]");
    fHistT.SetYTitle("Y [cm]");
    fHistT.SetZTitle("T [ns]");
    fHistT.SetDirectory(NULL);

    fHistWL.SetName("Spectrum");
    fHistWL.SetTitle("Wavelength distribution");
    fHistWL.SetXTitle("\\lambda [nm]");
    fHistWL.SetYTitle("Counts");
    fHistWL.SetDirectory(NULL);

    fHistH.SetName("Height");
    fHistH.SetTitle("Production Height");
    fHistH.SetXTitle("h [km]");
    fHistH.SetYTitle("Counts");
    fHistH.SetDirectory(NULL);
}

// --------------------------------------------------------------------------
//
// Default constructor. Creates and initializes the histograms.
//
MHPhotonEvent::MHPhotonEvent(Double_t max, const char *name, const char *title)
    : fHistT("", "", 1, 0, 1, 1, 0, 1), fType(-1), fPermanentReset(kFALSE)
{
    // pre-initialization of the profile is necessary to get fZmin and fZmax set

    Init(name, title);

    const MBinning binsd(51, -max, max);
    const MBinning binsa(51, -1,   1);

    SetBinning(fHistXY, binsd, binsd);
    SetBinning(fHistUV, binsa, binsa);
    SetBinning(fHistT,  binsd, binsd);
}

// --------------------------------------------------------------------------
//
// Creates and initializes the histograms.
//
MHPhotonEvent::MHPhotonEvent(Int_t type, const char *name, const char *title)
    : fHistT("", "", 1, 0, 1, 1, 0, 1), fType(type), fPermanentReset(kFALSE)
{
    // pre-initialization of the profile is necessary to get fZmin and fZmax set

    Init(name, title);

    const MBinning bins(51, -1, 1);

    SetBinning(fHistUV, bins, bins);
}

// --------------------------------------------------------------------------
//
// Find good limits for a binning num x [-max;max]
// and apply it to fHistXY and fHistT.
//
void MHPhotonEvent::SetBinningXY(Int_t num, Double_t max)
{
    Double_t min = -max;

    MH::FindGoodLimits(num, num, min, max, kFALSE);
    MH::FindGoodLimits(num, num, min, max, kFALSE);

    const MBinning binsd(num+1, min, max);

    SetBinning(fHistXY, binsd, binsd);
    SetBinning(fHistT,  binsd, binsd);
}

// --------------------------------------------------------------------------
//
// Search for MRflEvtData, MRflEvtHeader and MGeomCam
//
Bool_t MHPhotonEvent::SetupFill(const MParList *pList)
{
    Double_t xmax =  -1;
    Int_t    num  = 100;

    const Int_t f = fPermanentReset ? 2 : 1;
    MBinning(100/f, 0, 25).Apply(fHistH);
    MBinning(70, 275, 625).Apply(fHistWL);

    switch (fType)
    {
    case -1:
        return kTRUE;
        // case0: Take a value defined by the user
    case 1:
        xmax = 25000;
        break;
    case 2:
        {
            MReflector *r = (MReflector*)pList->FindObject("Reflector", "MReflector");
            if (!r)
            {
                *fLog << err << "Reflector [MReflector] not found... aborting." << endl;
                return kFALSE;
            }
            xmax = r->GetMaxR();
            break;
        }
    case 3: // The maximum radius (mm)
        fHistXY.SetXTitle("X [mm]");
        fHistXY.SetYTitle("Y [mm]");
        fHistT.SetXTitle("X [mm]");
        fHistT.SetYTitle("Y [mm]");
        // *Fallthrough*

    case 4: // Two times the pixel-0 traversal size
    case 5: // The maximum radius (cm) -- ~ 10 bins / pix
    case 6: // The maximum radius (cm) -- ~  5 bins / pix
    case 7: // The maximum radius (cm) -- ~  3 bins / pix
    case 8: // The maximum radius (cm) -- ~  2 bins / pix
        {
            MGeomCam *c = (MGeomCam*)pList->FindObject("MGeomCam");
            if (!c)
            {
                *fLog << err << "MGeomCam not found... aborting." << endl;
                return kFALSE;
            }
            // Type 3: Define ~10 bins per pixel
            xmax = fType!=4 ? c->GetMaxRadius() : 4*(*c)[0].GetT()/10;

            switch (fType)
            {
            case 3:
            case 5:
                // ~10 bins per pixel
                num = TMath::Nint(10*c->GetMaxRadius()/(*c)[0].GetT());
                break;
            case 6:
                // ~5 bins per pixel
                num = TMath::Nint(5*c->GetMaxRadius()/(*c)[0].GetT());
                break;
            case 7:
                // ~3 bins per pixel
                num = TMath::Nint(3*c->GetMaxRadius()/(*c)[0].GetT());
                break;
            case 8:
                // ~2 bins per pixel
                num = TMath::Nint(2*c->GetMaxRadius()/(*c)[0].GetT());
                break;
            case 4:
                num = 101;
                break;
            }

            if (fType==5 || fType==6 || fType==7)
                xmax /= 10;

            break;
        }
    default:
        *fLog << err << "No valid binning (Type=" << fType << ") given... aborting." << endl;
        return kFALSE;
    }

    SetBinningXY(num, xmax);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// ReInit overwrites the binning of the Wavelength histogram
// by the wavlenegth band from MCorsikaRunHeader.
// The bin-width is 5nm (fPermanentReset==kTRUE) or 10nm (kFALSE).
//
Bool_t MHPhotonEvent::ReInit(MParList *pList)
{
    MCorsikaRunHeader *h = (MCorsikaRunHeader*)pList->FindObject("MCorsikaRunHeader");
    if (!h)
    {
        *fLog << err << "MCorsikaRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    /*
    // What is the size of the light pool onm the ground?
    if (fType==1)
        SetBinningXY(100, h->GetImpactMax());
    */

    const Int_t f = fPermanentReset ? 10 : 2;

    Double_t xmin = h->GetWavelengthMin()-20;
    Double_t xmax = h->GetWavelengthMax()+20;
    Int_t    num  = TMath::CeilNint((xmax-xmin)/f);

    MH::FindGoodLimits(num, num, xmin, xmax, kTRUE);

    MBinning(TMath::Abs(num), xmin-.5, xmax-.5).Apply(fHistWL);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill contents of MPhotonEvent into histograms
//
Int_t MHPhotonEvent::Fill(const MParContainer *par, const Stat_t weight)
{
    const MPhotonEvent *evt = dynamic_cast<const MPhotonEvent*>(par);
    if (!evt)
    {
        *fLog << err << dbginf << "No MPhotonEvent found..." << endl;
        return kERROR;
    }

    // Check if we want to use this class as a single event display
    if (fPermanentReset && evt->GetNumPhotons()>0)
        Clear();

    // Get number of photons
    const Int_t num = evt->GetNumPhotons();

    // Set minimum to maximum possible value
    Double_t min = FLT_MAX;

    // Loop over all photons and determine the time of the first photon
    // FIXME: Should we get it from some statistics container?
    for (Int_t idx=0; idx<num; idx++)
        min = TMath::Min(min, (*evt)[idx].GetTime());

    // Now fill all histograms
    for (Int_t idx=0; idx<num; idx++)
    {
        const MPhotonData &ph = (*evt)[idx];

        if (ph.GetPrimary()==MMcEvtBasic::kNightSky || ph.GetPrimary()==MMcEvtBasic::kArtificial)
            continue;

        const Double_t x = ph.GetPosX();
        const Double_t y = ph.GetPosY();
        const Double_t u = ph.GetCosU();
        const Double_t v = ph.GetCosV();
        const Double_t t = ph.GetTime()-min;
        const Double_t w = ph.GetWavelength();
        const Double_t h = ph.GetProductionHeight()/100000;

        //TVector3 dir = dat->GetDir3();
        //dir *= -1./dir.Z();

        fHistXY.Fill(x, y);
        fHistUV.Fill(u, v);
        fHistT.Fill(x, y, t);
        fHistWL.Fill(w);
        fHistH.Fill(h);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
/*
Bool_t MHPhotonEvent::Finalize()
{
    const TAxis &axey = *fHistRad.GetYaxis();
    for (int y=1; y<=fHistRad.GetNbinsY(); y++)
    {
        const Double_t lo = axey.GetBinLowEdge(y);
        const Double_t hi = axey.GetBinLowEdge(y+1);

        const Double_t A = (hi*hi-lo*lo)*TMath::Pi()*TMath::Pi();

        for (int x=1; x<=fHistRad.GetNbinsX(); x++)
            fHistRad.SetBinContent(x, y, fHistRad.GetBinContent(x, y)/A);
    }
    return kTRUE;
}
*/

// --------------------------------------------------------------------------
//
// Make sure that the TProfile2D doesn't fix it's displayed minimum at 0.
// Set the pretty-palette.
//
void MHPhotonEvent::Paint(Option_t *opt)
{
    SetPalette("pretty");

    fHistT.SetMinimum();
    fHistT.SetMinimum(fHistT.GetMinimum(0));
}

// --------------------------------------------------------------------------
//
void MHPhotonEvent::Draw(Option_t *o)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad();

    pad->Divide(3,2);

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGrid();
    fHistXY.Draw("colz");

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGrid();
    fHistT.Draw("colz");

    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetGrid();
    fHistUV.Draw("colz");

    pad->cd(5);
    gPad->SetBorderMode(0);
    gPad->SetGrid();
    fHistWL.Draw();

    pad->cd(6);
    gPad->SetBorderMode(0);
    gPad->SetGrid();
    fHistH.Draw();
}

// --------------------------------------------------------------------------
//
// PermanentReset: Off
//
Int_t MHPhotonEvent::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "PermanentReset", print))
    {
        rc = kTRUE;
        fPermanentReset = GetEnvValue(env, prefix, "PermanentReset", fPermanentReset);
    }

    return rc;
}
