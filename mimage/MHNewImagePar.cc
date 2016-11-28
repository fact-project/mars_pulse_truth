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
!   Author(s): Wolfgang Wittek, 03/2003 <mailto:wittek@mppmu.mpg.de>
!   Author(s): Thomas Bretz, 04/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHNewImagePar
//
//
// ClassVersion 2:
// ----------
//  + TH1F fHistConcCOG;   // [ratio] concentration around the center of gravity (all pixels)
//  + TH1F fHistConcCore;  // [ratio] concentration of signals inside ellipse (used pixels)
//
// ClassVersion 3:
// ---------------
//  - fMm2Deg
//  - fUseMmScale
//
////////////////////////////////////////////////////////////////////////////
#include "MHNewImagePar.h"

#include <math.h>

#include <TH1.h>
#include <TPad.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MBinning.h"
#include "MParList.h"

#include "MHillas.h"
#include "MNewImagePar.h"

ClassImp(MHNewImagePar);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup histograms 
//
MHNewImagePar::MHNewImagePar(const char *name, const char *title)
    : fGeom(0)
{
    fName  = name  ? name  : "MHNewImagePar";
    fTitle = title ? title : "Histograms of new image parameters";

    fHistLeakage1.SetName("Leakage1");
    fHistLeakage1.SetTitle("Leakage_{1}");
    fHistLeakage1.SetXTitle("Leakage");
    fHistLeakage1.SetYTitle("Counts");
    fHistLeakage1.SetDirectory(NULL);
    fHistLeakage1.UseCurrentStyle();
    fHistLeakage1.SetFillStyle(4000);

    fHistLeakage2.SetName("Leakage2");
    fHistLeakage2.SetTitle("Leakage_{2}");
    fHistLeakage2.SetXTitle("Leakage");
    fHistLeakage2.SetYTitle("Counts");
    fHistLeakage2.SetDirectory(NULL);
    fHistLeakage2.UseCurrentStyle();
    fHistLeakage2.SetLineColor(kBlue);
    fHistLeakage2.SetFillStyle(4000);
 
    fHistUsedPix.SetName("UsedPix");
    fHistUsedPix.SetTitle("Number of used pixels");
    fHistUsedPix.SetXTitle("Number of Pixels");
    fHistUsedPix.SetYTitle("Counts");
    fHistUsedPix.SetDirectory(NULL);
    fHistUsedPix.UseCurrentStyle();
    fHistUsedPix.SetLineColor(kBlue);
    fHistUsedPix.SetFillStyle(4000);

    fHistCorePix.SetName("CorePix");
    fHistCorePix.SetTitle("Number of core pixels");
    fHistCorePix.SetXTitle("Number of Pixels");
    fHistCorePix.SetYTitle("Counts");
    fHistCorePix.SetDirectory(NULL);
    fHistCorePix.UseCurrentStyle();
    fHistCorePix.SetLineColor(kBlack);
    fHistCorePix.SetFillStyle(4000);

    fHistUsedArea.SetName("UsedArea");
    fHistUsedArea.SetTitle("Area of used pixels");
    fHistUsedArea.SetXTitle("Area [\\circ^{2}]");
    fHistUsedArea.SetYTitle("Counts");
    fHistUsedArea.SetDirectory(NULL);
    fHistUsedArea.UseCurrentStyle();
    fHistUsedArea.SetLineColor(kBlue);
    fHistUsedArea.SetFillStyle(4000);

    fHistCoreArea.SetName("CoreArea");
    fHistCoreArea.SetTitle("Area of core pixels");
    fHistCoreArea.SetXTitle("Area [\\circ^{2}]");
    fHistCoreArea.SetYTitle("Counts");
    fHistCoreArea.SetDirectory(NULL);
    fHistCoreArea.UseCurrentStyle();
    fHistCoreArea.SetLineColor(kBlack);
    fHistCoreArea.SetFillStyle(4000);

    fHistConc.SetDirectory(NULL);
    fHistConc1.SetDirectory(NULL);
    fHistConcCOG.SetDirectory(NULL);
    fHistConcCore.SetDirectory(NULL);
    fHistConc.SetName("Conc2");
    fHistConc1.SetName("Conc1");
    fHistConcCOG.SetName("ConcCOG");
    fHistConcCore.SetName("ConcCore");
    fHistConc.SetTitle("Ratio: Conc");
    fHistConc1.SetTitle("Ratio: Conc1");
    fHistConcCOG.SetTitle("Ratio: ConcCOG");
    fHistConcCore.SetTitle("Ratio: ConcCore");
    fHistConc.SetXTitle("Ratio");
    fHistConc1.SetXTitle("Ratio");
    fHistConcCOG.SetXTitle("Ratio");
    fHistConcCore.SetXTitle("Ratio");
    fHistConc.SetYTitle("Counts");
    fHistConc1.SetYTitle("Counts");
    fHistConcCOG.SetYTitle("Counts");
    fHistConcCore.SetYTitle("Counts");
    fHistConc.UseCurrentStyle();
    fHistConc1.UseCurrentStyle();
    fHistConcCOG.UseCurrentStyle();
    fHistConcCore.UseCurrentStyle();
    fHistConc.SetFillStyle(4000);
    fHistConc1.SetFillStyle(4000);
    fHistConcCOG.SetFillStyle(4000);
    fHistConcCore.SetFillStyle(4000);
    fHistConc1.SetLineColor(kBlue);
    fHistConcCOG.SetLineColor(kBlue);

    MBinning bins;

    bins.SetEdges(100, 0, 1);
    bins.Apply(fHistLeakage1);
    bins.Apply(fHistLeakage2);
    bins.Apply(fHistConc);
    bins.Apply(fHistConc1);
    bins.Apply(fHistConcCOG);
    bins.Apply(fHistConcCore);

    bins.SetEdges(75, 0.5, 150.5);
    bins.Apply(fHistUsedPix);
    bins.Apply(fHistCorePix);

    MBinning b;
    b.SetEdges(50, 0, 1.5);
    b.Apply(fHistUsedArea);
    b.Apply(fHistCoreArea);
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning
//
Bool_t MHNewImagePar::SetupFill(const MParList *plist)
{
    fGeom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }

    const MBinning *bins = (MBinning*)plist->FindObject("BinningArea");
    if (bins)
    {
        bins->Apply(fHistUsedArea);
        bins->Apply(fHistCoreArea);
    }

    ApplyBinning(*plist, "Leakage", fHistLeakage1);
    ApplyBinning(*plist, "Leakage", fHistLeakage2);

    ApplyBinning(*plist, "Pixels",  fHistUsedPix);
    ApplyBinning(*plist, "Pixels",  fHistCorePix);

    //ApplyBinning(*plist, "Area",    fHistUsedArea);
    //ApplyBinning(*plist, "Area",    fHistCoreArea);

    ApplyBinning(*plist, "Conc",     fHistConc);
    ApplyBinning(*plist, "Conc1",    fHistConc1);
    ApplyBinning(*plist, "ConcCOG",  fHistConcCOG);
    ApplyBinning(*plist, "ConcCore", fHistConcCore);

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MNewImagePar container.
//
Int_t MHNewImagePar::Fill(const MParContainer *par, const Stat_t w)
{
    if (!par)
    {
        *fLog << err << "MHNewImagePar::Fill: Pointer (!=NULL) expected." << endl;
        return kERROR;
    }

    const Double_t scale = fGeom->GetConvMm2Deg()*fGeom->GetConvMm2Deg();

    const MNewImagePar &h = *(MNewImagePar*)par;

    fHistLeakage1.Fill(h.GetLeakage1(), w);
    fHistLeakage2.Fill(h.GetLeakage2(), w);

    fHistUsedPix.Fill(h.GetNumUsedPixels(), w);
    fHistCorePix.Fill(h.GetNumCorePixels(), w);

    fHistUsedArea.Fill(h.GetUsedArea()*scale, w);
    fHistCoreArea.Fill(h.GetCoreArea()*scale, w);

    fHistConc.Fill(h.GetConc(), w);
    fHistConc1.Fill(h.GetConc1(), w);
    fHistConcCOG.Fill(h.GetConcCOG(), w);
    fHistConcCore.Fill(h.GetConcCore(), w);

    return kTRUE;
}

void MHNewImagePar::Paint(Option_t *o)
{
    if (fHistLeakage1.GetMaximum()>0 && gPad->GetPad(1) && gPad->GetPad(1)->GetPad(1))
        gPad->GetPad(1)->GetPad(1)->SetLogy();
}

// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the two histograms into it.
// Be careful: The histograms belongs to this object and won't get deleted
// together with the canvas.
//
void MHNewImagePar::Draw(Option_t *o)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    // FIXME: If same-option given make two independant y-axis!
    const TString opt(o);
    const Bool_t same = opt.Contains("same");

    if (!same)
        pad->Divide(2, 1);
    else
    {
        fHistLeakage1.SetName("Leakage1Same");
        fHistLeakage2.SetName("Leakage2Same");
        fHistUsedPix.SetName("UsedPixSame");
        fHistCorePix.SetName("CorePixSame");
        fHistUsedArea.SetName("UsedAreaSame");
        fHistCoreArea.SetName("CoreAreaSame");
        fHistConcCOG.SetName("ConcCOGSame");
        fHistConcCore.SetName("ConcCoreSame");
        fHistConc1.SetName("Conc1Same");
        fHistConc.SetName("Conc2Same");

        fHistLeakage1.SetDirectory(0);
        fHistLeakage2.SetDirectory(0);
        fHistUsedPix.SetDirectory(0);
        fHistCorePix.SetDirectory(0);
        fHistUsedArea.SetDirectory(0);
        fHistCoreArea.SetDirectory(0);
        fHistConcCOG.SetDirectory(0);
        fHistConcCore.SetDirectory(0);
        fHistConc1.SetDirectory(0);
        fHistConc.SetDirectory(0);

        fHistLeakage1.SetLineColor(kMagenta);
        fHistLeakage1.SetLineColor(kCyan);
        fHistCorePix.SetLineColor(kMagenta);
        fHistUsedPix.SetLineColor(kCyan);
        fHistConc1.SetLineColor(kMagenta);
        fHistConc.SetLineColor(kCyan);
        fHistConcCOG.SetLineColor(kMagenta);
        fHistConcCore.SetLineColor(kCyan);
        fHistCoreArea.SetLineColor(kMagenta);
        fHistUsedArea.SetLineColor(kCyan);
    }

    pad->cd(1);
    TVirtualPad *pad1=gPad;
    pad1->SetBorderMode(0);
    if (!same)
        pad1->Divide(1,3, 0.001, 0.001);

    pad1->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    TAxis &x = *fHistLeakage1.GetXaxis();
    x.SetRangeUser(0.0, x.GetXmax());
    RemoveFromPad("Leakage1Same");
    RemoveFromPad("Leakage2Same");
    MH::DrawSame(fHistLeakage1, fHistLeakage2, "Leakage1 and Leakage2", same);
    fHistLeakage1.SetMinimum();
    fHistLeakage2.SetMinimum();
    fHistLeakage2.SetMaximum(0.1);   // dummy value to allow log-scale

    pad1->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("UsedPixSame");
    RemoveFromPad("CorePixSame");
    MH::DrawSame(fHistCorePix, fHistUsedPix, "Number of core/used Pixels", same);

    pad1->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("CoreAreaSame");
    RemoveFromPad("UsedAreaSame");
    MH::DrawSame(fHistCoreArea, fHistUsedArea, "Area of core/used Pixels", same);

    pad->cd(2);
    TVirtualPad *pad2=gPad;
    pad2->SetBorderMode(0);
    if (!same)
        pad2->Divide(1, 2, 0.001, 0.001);

    pad2->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("Conc1Same");
    RemoveFromPad("Conc2Same");
    MH::DrawSame(fHistConc1, fHistConc, "Concentrations", same);

    pad2->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("ConcCOGSame");
    RemoveFromPad("ConcCoreSame");
    MH::DrawSame(fHistConcCore, fHistConcCOG, "Concentrations", same);
}

TH1 *MHNewImagePar::GetHistByName(const TString name) const
{
    if (name.Contains("Leakage1", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistLeakage1);
    if (name.Contains("Leakage2", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistLeakage2);
    if (name.Contains("ConcCOG", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistConcCOG);
    if (name.Contains("ConcCore", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistConcCore);
    if (name.Contains("Conc1", TString::kIgnoreCase)) // must be first!
        return const_cast<TH1F*>(&fHistConc1);
    if (name.Contains("Conc", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistConc);
    if (name.Contains("UsedPix", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistUsedPix);
    if (name.Contains("CorePix", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistCorePix);
    if (name.Contains("UsedArea", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistUsedArea);
    if (name.Contains("CoreArea", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistCoreArea);

    return NULL;
}
