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
!   Author(s): Thomas Bretz  2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Wolfgang Wittek  2002 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHHillas
//
// This class contains histograms for the source independent image parameters
//
// ClassVersion 2:
// ---------------
//  - fMm2Deg
//  - fUseMmScale
//
/////////////////////////////////////////////////////////////////////////////
#include "MHHillas.h"

#include <math.h>

#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TStyle.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MHillas.h"
#include "MGeomCam.h"
#include "MBinning.h"

#include "MHCamera.h"

ClassImp(MHHillas);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup four histograms for Width, Length
//
MHHillas::MHHillas(const char *name, const char *title)
    : fGeomCam(0)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHHillas";
    fTitle = title ? title : "Source independent image parameters";

    fLength  = new TH1F("Length", "Length of Ellipse",               100,   0, 1.0);
    fWidth   = new TH1F("Width",  "Width of Ellipse",                100,   0, 1.0);
    fDistC   = new TH1F("DistC",  "Distance from center of camera",  100,   0, 1.5);
    fDelta   = new TH1F("Delta",  "Angle (Main axis - x-axis)",      101, -90,  90);

    fLength->SetLineColor(kBlue);

    fLength->SetDirectory(NULL);
    fWidth->SetDirectory(NULL);
    fDistC->SetDirectory(NULL);
    fDelta->SetDirectory(NULL);

    fLength->SetXTitle("Length [\\circ]");
    fWidth->SetXTitle("Width [\\circ]");
    fDistC->SetXTitle("Distance [\\circ]");
    fDelta->SetXTitle("Delta [\\circ]");

    fLength->SetYTitle("Counts");
    fWidth->SetYTitle("Counts");
    fDistC->SetYTitle("Counts");
    fDelta->SetYTitle("Counts");

    fDelta->SetMinimum(0);

    MBinning bins;
    bins.SetEdgesLog(50, 1, 1e7);

    fSize  = new TH1F;
    fSize->SetName("Size");
    fSize->SetTitle("Number of Photons");
    fSize->SetDirectory(NULL);
    fSize->SetXTitle("Size");
    fSize->SetYTitle("Counts");
    fSize->GetXaxis()->SetTitleOffset(1.2);
    fSize->GetXaxis()->SetLabelOffset(-0.015);
    fSize->SetFillStyle(4000);
    fSize->UseCurrentStyle();

    bins.Apply(*fSize);

    fCenter = new TH2F("Center", "Center of gravity", 51, -1.5, 1.5, 51, -1.5, 1.5);
    fCenter->SetDirectory(NULL);
    fCenter->SetXTitle("x [mm]");
    fCenter->SetYTitle("y [mm]");
    fCenter->SetZTitle("Counts");
}

// --------------------------------------------------------------------------
//
// Delete the histograms
//
MHHillas::~MHHillas()
{
    delete fLength;
    delete fWidth;

    delete fDistC;
    delete fDelta;

    delete fSize;
    delete fCenter;
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
Bool_t MHHillas::SetupFill(const MParList *plist)
{
    fGeomCam = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fGeomCam)
    {
        *fLog << err << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }


    ApplyBinning(*plist, "Width",  *fWidth);
    ApplyBinning(*plist, "Length", *fLength);
    ApplyBinning(*plist, "Dist",   *fDistC);
    ApplyBinning(*plist, "Delta",  *fDelta);
    ApplyBinning(*plist, "Size",   *fSize);

    const MBinning *bins = (MBinning*)plist->FindObject("BinningCamera");
    if (!bins)
    {
        const Float_t r = fGeomCam->GetMaxRadius()*fGeomCam->GetConvMm2Deg();

        const MBinning b(61, -r, r);
        SetBinning(*fCenter, b, b);
    }
    else
        SetBinning(*fCenter, *bins, *bins);


    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MHillas-Container.
// Be careful: Only call this with an object of type MHillas
//
Int_t MHHillas::Fill(const MParContainer *par, const Stat_t w)
{
    if (!par)
    {
        *fLog << err << "MHHillas::Fill: Pointer (!=NULL) expected." << endl;
        return kERROR;
    }

    const MHillas &h = *(MHillas*)par;

    const Double_t scale = fGeomCam->GetConvMm2Deg();

    fLength->Fill(scale*h.GetLength(), w);
    fWidth ->Fill(scale*h.GetWidth(), w);
    fDistC ->Fill(scale*h.GetDist0(), w);
    fCenter->Fill(scale*h.GetMeanX(), scale*h.GetMeanY(), w);
    fDelta ->Fill(kRad2Deg*h.GetDelta(), w);
    fSize  ->Fill(h.GetSize(), w);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the four histograms into it.
// Be careful: The histograms belongs to this object and won't get deleted
// together with the canvas.
//
void MHHillas::Draw(Option_t *o)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    TString opt(o);
    opt.ToLower();

    // FIXME: If same-option given make two independant y-axis!
    const Bool_t same = opt.Contains("same");

    if (!same)
        pad->Divide(2,3);
    else
    {
        fLength->SetName("LengthSame");
        fWidth->SetName("WidthSame");
        fDistC->SetName("DistCSame");
        fDelta->SetName("DeltaSame");
        fSize->SetName("SizeSame");
        fCenter->SetName("CenterSame");

        fLength->SetDirectory(0);
        fWidth->SetDirectory(0);
        fDistC->SetDirectory(0);
        fDelta->SetDirectory(0);
        fSize->SetDirectory(0);
        fCenter->SetDirectory(0);

        fDistC->SetLineColor(kBlue);
        fSize->SetLineColor(kBlue);
        fDelta->SetLineColor(kBlue);
        fWidth->SetLineColor(kMagenta);
        fLength->SetLineColor(kCyan);
    }

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("WidthSame");
    RemoveFromPad("LengthSame");
    MH::DrawSame(*fWidth, *fLength, "Width'n'Length", same);

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("DistCSame");
    fDistC->Draw(same?"same":"");

    pad->cd(3);
    gPad->SetGridx();
    gPad->SetGridy();
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetLogy();
    RemoveFromPad("SizeSame");
    fSize->Draw(same?"same":"");

    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    gPad->SetPad(0.51, 0.01, 0.99, 0.65);
    if (same)
    {
        TH2 *h=dynamic_cast<TH2*>(gPad->FindObject("Center"));
        if (h)
        {
            // This causes crashes in THistPainter::PaintTable
            // if the z-axis is not kept. No idea why...
            h->SetDrawOption("z");
            h->SetMarkerColor(kBlack);
        }

        RemoveFromPad("CenterSame");
        fCenter->SetMarkerColor(kBlue);
        fCenter->Draw("same");
    }
    else
        fCenter->Draw("colz");

    if (fGeomCam)
    {
//        MHCamera *cam = new MHCamera(*fGeomCam);
//        cam->Draw("same");
//        cam->SetBit(kCanDelete);
    }

    pad->cd(5);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("DeltaSame");
    fDelta->Draw(same?"same":"");

    pad->cd(6);
    if (gPad && !same)
        delete gPad;
}

TH1 *MHHillas::GetHistByName(const TString name) const
{
    if (name.Contains("Width", TString::kIgnoreCase))
        return fWidth;
    if (name.Contains("Length", TString::kIgnoreCase))
        return fLength;
    if (name.Contains("Size", TString::kIgnoreCase))
        return fSize;
    if (name.Contains("Delta", TString::kIgnoreCase))
        return fDelta;
    if (name.Contains("DistC", TString::kIgnoreCase))
        return fDistC;
    if (name.Contains("Center", TString::kIgnoreCase))
        return fCenter;

    return NULL;
}

void MHHillas::Paint(Option_t *opt)
{
    MH::SetPalette("pretty");
    MH::Paint();
}
