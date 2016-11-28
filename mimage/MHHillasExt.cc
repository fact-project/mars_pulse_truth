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
!   Author(s): Thomas Bretz, 2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2010
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHHillasExt
//
// This class contains histograms for every Hillas parameter
//
// Class Version 2:
// ----------------
//  - fHMaxDist
//  + fHSlopeL
//
// ClassVersion 3:
// ---------------
//  - fMm2Deg
//  - fUseMmScale
//
// ClassVersion 4:
// ---------------
//  + TH1F fHTimeSpread;      // [ns] Spread (rms) of arrival time around mean
//  + TH1F fHTimeSpreadW;     // [ns] Weighted spread (rms) of arrival time around weighted mean
//  + TH1F fHSlopeSpread;     // [ns] Spread (rms) of arrival time around slope
//  + TH1F fHSlopeSpreadW;    // [ns] Weighted spread (rms) of arrival time around slope
//
/////////////////////////////////////////////////////////////////////////////
#include "MHHillasExt.h"

#include <math.h>

#include <TPad.h>
#include <TLegend.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"

#include "MParList.h"

#include "MBinning.h"

#include "MHillas.h"
#include "MHillasExt.h"
#include "MHillasSrc.h"

ClassImp(MHHillasExt);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup four histograms for Width, Length
//
MHHillasExt::MHHillasExt(const char *name, const char *title)
    : fGeom(0), fHillas(0), fHillasExt(0), fHilName("MHillasExt")
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHHillasExt";
    fTitle = title ? title : "Container for extended Hillas histograms";

    //
    // loop over all Pixels and create two histograms
    // one for the Low and one for the High gain
    // connect all the histogram with the container fHist
    //
    fHAsym.UseCurrentStyle();
    fHM3Long.UseCurrentStyle();
    fHM3Trans.UseCurrentStyle();
    fHSlopeL.UseCurrentStyle();
    fHTimeSpread.UseCurrentStyle();
    fHTimeSpreadW.UseCurrentStyle();
    fHSlopeSpread.UseCurrentStyle();
    fHSlopeSpreadW.UseCurrentStyle();

    fHAsym.SetName("Asymmetry");
    fHM3Long.SetName("M3l");
    fHM3Trans.SetName("M3t");
    fHSlopeL.SetName("SlopeL");
    fHTimeSpread.SetName("TimeSpread");
    fHTimeSpreadW.SetName("TimeSpreadW");
    fHSlopeSpread.SetName("SlopeSpread");
    fHSlopeSpreadW.SetName("SlopeSpreadW");

    fHAsym.SetTitle("Asymmetry");
    fHM3Long.SetTitle("3^{rd} Moment Longitudinal");
    fHM3Trans.SetTitle("3^{rd} Moment Transverse");
    fHSlopeL.SetTitle("Longitudinal time-slope vs. Dist");
    fHTimeSpread.SetTitle("Time spread around mean");
    fHTimeSpreadW.SetTitle("Weighted time spread around weighted mean");
    fHSlopeSpread.SetTitle("Time spread around slope");
    fHSlopeSpreadW.SetTitle("Weighted time spread around slope");

    fHAsym.SetXTitle("Asym [\\circ]");
    fHM3Long.SetXTitle("3^{rd} M_{l} [\\circ]");
    fHM3Trans.SetXTitle("3^{rd} M_{t} [\\circ]");
    fHSlopeL.SetXTitle("D [\\circ]");
    fHTimeSpread.SetXTitle("T_{rms} [ns]");
    fHTimeSpreadW.SetXTitle("T_{rms} [ns]");
    fHSlopeSpread.SetXTitle("T_{rms} [ns]");
    fHSlopeSpreadW.SetXTitle("T_{rms} [ns]");

    fHAsym.SetYTitle("Counts");
    fHM3Long.SetYTitle("Counts");
    fHM3Trans.SetYTitle("Counts");
    fHSlopeL.SetYTitle("S_{l} [ns/\\circ]");
    fHTimeSpread.SetYTitle("Counts");
    fHTimeSpreadW.SetYTitle("Counts");
    fHSlopeSpread.SetYTitle("Counts");
    fHSlopeSpreadW.SetYTitle("Counts");

    fHAsym.SetFillStyle(4000);
    fHM3Long.SetFillStyle(4000);
    fHM3Trans.SetFillStyle(4000);
    //fHSlopeL.SetFillStyle(4000);

    fHAsym.SetDirectory(NULL);
    fHM3Long.SetDirectory(NULL);
    fHM3Trans.SetDirectory(NULL);
    fHSlopeL.SetDirectory(NULL);

    fHTimeSpread.SetDirectory(NULL);
    fHTimeSpreadW.SetDirectory(NULL);
    fHSlopeSpread.SetDirectory(NULL);
    fHSlopeSpreadW.SetDirectory(NULL);

    fHM3Trans.SetLineColor(kBlue);

    fHSlopeSpread.SetLineColor(kBlue);
    fHSlopeSpreadW.SetLineColor(kBlue);

    fHTimeSpreadW.SetLineStyle(kDashed);
    fHSlopeSpreadW.SetLineStyle(kDashed);

    MBinning binsx, binsy;

    binsx.SetEdges(51, -1.1, 1.1);
    binsx.Apply(fHM3Long);
    binsx.Apply(fHM3Trans);

    binsx.SetEdges(51, -2.0, 2.0);
    binsx.Apply(fHAsym);

    binsx.SetEdges(100,   0, 1.5);
    binsy.SetEdges(100,  -9,   9);
    MH::SetBinning(fHSlopeL, binsx, binsy);

    binsx.SetEdges( 50,   0, 1.5);
    MH::SetBinning(fHTimeSpread,   binsx);
    MH::SetBinning(fHSlopeSpread,  binsx);
    MH::SetBinning(fHTimeSpreadW,  binsx);
    MH::SetBinning(fHSlopeSpreadW, binsx);
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning (with the names 'BinningWidth' and 'BinningLength')
// are found in the parameter list
// Use this function if you want to set the conversion factor which
// is used to convert the mm-scale in the camera plain into the deg-scale
// used for histogram presentations. The conversion factor is part of
// the camera geometry. Please create a corresponding MGeomCam container.
//
Bool_t MHHillasExt::SetupFill(const MParList *plist)
{
    fHillasExt = (MHillasExt*)plist->FindObject(fHilName, "MHillasExt");
    if (!fHillasExt)
    {
        *fLog << err << fHilName << "[MHillasExt] not found in parameter list... aborting." << endl;
        return kFALSE;
    }

    fHillas = (MHillas*)plist->FindObject("MHillas");
    if (!fHillas)
    {
        *fLog << err << "MHillas not found in parameter list... aborting." << endl;
        return kFALSE;
    }

    fGeom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }

    ApplyBinning(*plist, "Asym",             fHAsym);
    ApplyBinning(*plist, "M3Long",           fHM3Long);
    ApplyBinning(*plist, "M3Trans",          fHM3Trans);
    ApplyBinning(*plist, "Dist",    "Slope", fHSlopeL);

    ApplyBinning(*plist, "TimeSpread",       fHTimeSpread);
    ApplyBinning(*plist, "TimeSpread",       fHTimeSpreadW);
    ApplyBinning(*plist, "TimeSpread",       fHSlopeSpread);
    ApplyBinning(*plist, "TimeSpread",       fHSlopeSpreadW);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the four histograms with data from a MHillas-Container.
// Be careful: Only call this with an object of type MHillas
//
Int_t MHHillasExt::Fill(const MParContainer *par, const Stat_t w)
{
    const MHillasSrc *src = (MHillasSrc*)par;

    const Double_t scale = TMath::Sign(fGeom->GetConvMm2Deg(), (src ? src->GetCosDeltaAlpha() : 1));
    const Double_t dist  = src ? src->GetDist() : fHillas->GetDist0();

    fHAsym.Fill(scale*fHillasExt->GetAsym(), w);
    fHM3Long.Fill(scale*fHillasExt->GetM3Long(), w);
    fHM3Trans.Fill(scale*fHillasExt->GetM3Trans(), w);
    fHSlopeL.Fill(scale*dist, fHillasExt->GetSlopeLong()/scale, w);

    fHTimeSpread.Fill(fHillasExt->GetTimeSpread(), w);
    fHTimeSpreadW.Fill(fHillasExt->GetTimeSpreadWeighted(), w);
    fHSlopeSpread.Fill(fHillasExt->GetSlopeSpread(), w);
    fHSlopeSpreadW.Fill(fHillasExt->GetSlopeSpreadWeighted(), w);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the four histograms into it.
// Be careful: The histograms belongs to this object and won't get deleted
// together with the canvas.
//
void MHHillasExt::Draw(Option_t *o)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    // FIXME: If same-option given make two independant y-axis!
    const TString opt(o);
    const Bool_t same = opt.Contains("same");

    if (!same)
        pad->Divide(2,2);
    else
    {
        fHAsym.SetName("AsymmetrySame");
        fHM3Long.SetName("M3lSame");
        fHM3Trans.SetName("M3tSame");
        fHSlopeL.SetName("SlopeLSame");

        fHSlopeSpreadW.SetName("SlopeSpreadWSame");
        fHSlopeSpread.SetName("SlopeSpreadSame");
        fHTimeSpread.SetName("TimeSpreadSame");
        fHTimeSpreadW.SetName("TimeSpreadWSame");

        fHAsym.SetDirectory(0);
        fHM3Long.SetDirectory(0);
        fHM3Trans.SetDirectory(0);
        fHSlopeL.SetDirectory(0);

        fHSlopeSpreadW.SetDirectory(0);
        fHSlopeSpread.SetDirectory(0);
        fHTimeSpread.SetDirectory(0);
        fHTimeSpreadW.SetDirectory(0);

        fHM3Long.SetLineColor(kMagenta);
        fHM3Trans.SetLineColor(kCyan);
        fHAsym.SetLineColor(kBlue);
        fHSlopeL.SetMarkerColor(kBlue);

        fHSlopeSpreadW.SetLineColor(kMagenta);
        fHSlopeSpread.SetLineColor(kMagenta);
        fHTimeSpread.SetLineColor(kCyan);
        fHTimeSpreadW.SetLineColor(kCyan);
    }

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("M3lSame");
    RemoveFromPad("M3tSame");
    MH::DrawSame(fHM3Long, fHM3Trans, "3^{rd} Moments", same);

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("AsymmetrySame");
    fHAsym.Draw(same?"same":"");

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    //RemoveFromPad("SlopeLSame");
    //fHSlopeL.Draw(same?"same":"");
    if (same)
    {
        TH2 *h=dynamic_cast<TH2*>(gPad->FindObject("SlopeL"));
        if (h)
        {
            // This causes crashes in THistPainter::PaintTable
            // if the z-axis is not kept. No idea why...
            h->SetDrawOption("z");
            h->SetMarkerColor(kBlack);
        }

        RemoveFromPad("SlopeLSame");
        fHSlopeL.SetMarkerColor(kBlue);
        fHSlopeL.Draw("same");
    }
    else
        fHSlopeL.Draw("colz");

    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();

    RemoveFromPad("SlopeSpreadSame");
    RemoveFromPad("SlopeSpreadWSame");
    RemoveFromPad("TimeSpreadSame");
    RemoveFromPad("TimeSpreadWSame");

    fHSlopeSpreadW.Draw(same?"same":"");
    fHSlopeSpread.Draw("same");
    fHTimeSpread.Draw("same");
    fHTimeSpreadW.Draw("same");
}

TH1 *MHHillasExt::GetHistByName(const TString name) const
{
    if (name.Contains("Asym", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHAsym);
    if (name.Contains("M3L", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHM3Long);
    if (name.Contains("M3T", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHM3Trans);
    if (name.Contains("SlopeL", TString::kIgnoreCase))
        return const_cast<TH2F*>(&fHSlopeL);

    return NULL;
}
