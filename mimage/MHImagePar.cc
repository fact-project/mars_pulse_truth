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
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHImagePar
//
////////////////////////////////////////////////////////////////////////////
#include "MHImagePar.h"

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
#include "MImagePar.h"

ClassImp(MHImagePar);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup histograms 
//
MHImagePar::MHImagePar(const char *name, const char *title)
{
    fName  = name  ? name  : "MHImagePar";
    fTitle = title ? title : "Histograms of image parameters";

    fHistSatHi.SetName("SatHi");
    fHistSatHi.SetTitle("Number of pixels with saturating hi-gains");
    fHistSatHi.SetXTitle("Pixels");
    fHistSatHi.SetYTitle("Counts");
    fHistSatHi.SetDirectory(NULL);
    fHistSatHi.UseCurrentStyle();
    fHistSatHi.SetLineColor(kBlue);
    fHistSatHi.SetFillStyle(4000);

    fHistSatLo.SetName("SatLo");
    fHistSatLo.SetTitle("Number of pixels with saturating lo-gains");
    fHistSatLo.SetXTitle("Pixels");
    fHistSatLo.SetYTitle("Counts");
    fHistSatLo.SetDirectory(NULL);
    fHistSatLo.UseCurrentStyle();
    fHistSatLo.SetFillStyle(4000);

    fHistIslands.SetName("Islands");
    fHistIslands.SetTitle("Number of Islands");
    fHistIslands.SetXTitle("N");
    fHistIslands.SetYTitle("Counts");
    fHistIslands.SetDirectory(NULL);
    fHistIslands.UseCurrentStyle();
    //fHistIslands.SetLineColor(kBlue);
    fHistIslands.SetFillStyle(4000);

    fHistSizeSubIslands.SetName("SizeSub");
    fHistSizeSubIslands.SetTitle("Size of Sub Islands");
    fHistSizeSubIslands.SetXTitle("S [phe]");
    fHistSizeSubIslands.SetYTitle("Counts");
    fHistSizeSubIslands.SetDirectory(NULL);
    fHistSizeSubIslands.UseCurrentStyle();
    fHistSizeSubIslands.SetLineColor(kBlue);
    fHistSizeSubIslands.SetFillStyle(4000);

    fHistSizeMainIsland.SetName("SizeMain");
    fHistSizeMainIsland.SetTitle("Size of Main Island");
    fHistSizeMainIsland.SetXTitle("S [phe]");
    fHistSizeMainIsland.SetYTitle("Counts");
    fHistSizeMainIsland.SetDirectory(NULL);
    fHistSizeMainIsland.UseCurrentStyle();
    fHistSizeMainIsland.SetFillStyle(4000);

    fHistNumSP.SetName("NumSP");
    fHistNumSP.SetTitle("Number of single core pixels");
    fHistNumSP.SetXTitle("N");
    fHistNumSP.SetYTitle("Counts");
    fHistNumSP.SetDirectory(NULL);
    fHistNumSP.UseCurrentStyle();
    //fHistNumSP.SetLineColor(kBlue);
    fHistNumSP.SetFillStyle(4000);

    fHistSizeSP.SetName("SizeSP");
    fHistSizeSP.SetTitle("Size of single core pixels");
    fHistSizeSP.SetXTitle("S [phe]");
    fHistSizeSP.SetYTitle("Counts");
    fHistSizeSP.SetDirectory(NULL);
    fHistSizeSP.UseCurrentStyle();
    //fHistSizeSP.SetLineColor(kBlue);
    fHistSizeSP.SetFillStyle(4000);


    MBinning bins;

    bins.SetEdges(60, -0.5, 59.5);
    bins.Apply(fHistSatLo);
    bins.Apply(fHistSatHi);
    bins.Apply(fHistNumSP);

    bins.SetEdgesLog(50, 1, 1e7);
    bins.Apply(fHistSizeSubIslands);
    bins.Apply(fHistSizeMainIsland);
    bins.Apply(fHistSizeSP);

    bins.SetEdges(15, 0.5, 15.5);
    bins.Apply(fHistIslands);
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning
//
Bool_t MHImagePar::SetupFill(const MParList *plist)
{
    ApplyBinning(*plist, "Pixels",  fHistSatLo);
    ApplyBinning(*plist, "Pixels",  fHistSatHi);
    ApplyBinning(*plist, "Pixels",  fHistNumSP);

    ApplyBinning(*plist, "Islands", fHistIslands);

    ApplyBinning(*plist, "Size",    fHistSizeSubIslands);
    ApplyBinning(*plist, "Size",    fHistSizeMainIsland);
    ApplyBinning(*plist, "Size",    fHistSizeSP);

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MNewImagePar container.
//
Int_t MHImagePar::Fill(const MParContainer *par, const Stat_t w)
{
    if (!par)
    {
        *fLog << err << "MImagePar::Fill: Pointer (!=NULL) expected." << endl;
        return kERROR;
    }

    const MImagePar &h = *(MImagePar*)par;

    fHistSatHi.Fill(h.GetNumSatPixelsHG(),    w);
    fHistSatLo.Fill(h.GetNumSatPixelsLG(),    w);
    fHistNumSP.Fill(h.GetNumSinglePixels(),   w);
    fHistSizeSP.Fill(h.GetSizeSinglePixels(), w);
    fHistSizeSubIslands.Fill(h.GetSizeSubIslands(), w);
    fHistSizeMainIsland.Fill(h.GetSizeMainIsland(), w);
    fHistIslands.Fill(h.GetNumIslands(),      w);

    return kTRUE;
}

void MHImagePar::Paint(Option_t *o)
{
     if (fHistSatHi.GetMaximum()>0 && gPad->GetPad(1))
         gPad->GetPad(1)->SetLogy();
     if (fHistIslands.GetMaximum()>0 && gPad->GetPad(3))
         gPad->GetPad(3)->SetLogy();

     TVirtualPad *pad = gPad->GetPad(2);
     if (pad)
     {
         if (fHistNumSP.GetMaximum()>0 && pad->GetPad(1))
             pad->GetPad(1)->SetLogy();
         if (fHistSizeSP.GetMaximum()>0 && pad->GetPad(2))
         {
             pad->GetPad(2)->SetLogx();
             pad->GetPad(2)->SetLogy();
         }
     }
     if (fHistSizeMainIsland.GetMaximum()>0 && gPad->GetPad(4))
     {
         gPad->GetPad(4)->SetLogx();
         gPad->GetPad(4)->SetLogy();
     }
}

// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the two histograms into it.
// Be careful: The histograms belongs to this object and won't get deleted
// together with the canvas.
//
void MHImagePar::Draw(Option_t *o)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    TString opt(o);
    opt.ToLower();

    // FIXME: If same-option given make two independant y-axis!
    const Bool_t same = opt.Contains("same");

    if (!same)
        pad->Divide(2,2);
    else
    {
        fHistSatHi.SetName("SatHiSame");
        fHistSatLo.SetName("SatLoSame");
        fHistIslands.SetName("IslandsSame");
        fHistSizeSubIslands.SetName("SizeSubSame");
        fHistSizeMainIsland.SetName("SizeMainSame");
        fHistNumSP.SetName("NumSPSame");
        fHistSizeSP.SetName("SizeSPSame");

        fHistSatHi.SetDirectory(0);
        fHistSatLo.SetDirectory(0);
        fHistIslands.SetDirectory(0);
        fHistSizeSubIslands.SetDirectory(0);
        fHistSizeMainIsland.SetDirectory(0);
        fHistNumSP.SetDirectory(0);
        fHistSizeSP.SetDirectory(0);

        fHistSatHi.SetLineColor(kMagenta);
        fHistSatLo.SetLineColor(kCyan);
        fHistSizeMainIsland.SetLineColor(kMagenta);
        fHistSizeSubIslands.SetLineColor(kCyan);
        fHistIslands.SetLineColor(kBlue);
        fHistNumSP.SetLineColor(kBlue);
        fHistSizeSP.SetLineColor(kBlue);
    }

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("SatHiSame");
    RemoveFromPad("SatLoSame");
    MH::DrawSame(fHistSatHi, fHistSatLo, "Saturating Pixels", same);
    fHistSatHi.SetMinimum();    // switch off to allow log-scale
    fHistSatLo.SetMinimum();    // switch off to allow log-scale
    fHistSatLo.SetMaximum(0.1); // dummy value to allow log-scale

    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("SizeSubSame");
    RemoveFromPad("SizeMainSame");
    MH::DrawSame(fHistSizeMainIsland, fHistSizeSubIslands, "Sizes...", same);
    fHistSizeMainIsland.SetMinimum();    // switch off to allow log-scale
    fHistSizeSubIslands.SetMinimum();    // switch off to allow log-scale
    fHistSizeSubIslands.SetMaximum(0.1); // dummy value to allow log-scale

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    if (!same)
        pad->GetPad(2)->Divide(1,2,1e-10,1e-10);
    if (pad->GetPad(2))
    {
        pad->GetPad(2)->cd(1);
        gPad->SetBorderMode(0);
        RemoveFromPad("NumSPSame");
        fHistNumSP.Draw(same?"same":"");
        pad->GetPad(2)->cd(2);
        gPad->SetBorderMode(0);
        RemoveFromPad("SizeSPSame");
        fHistSizeSP.Draw(same?"same":"");
    }

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("IslandsSame");
    fHistIslands.Draw(same?"same":"");
}

TH1 *MHImagePar::GetHistByName(const TString name) const
{
    if (name.Contains("SatHi", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistSatHi);
    if (name.Contains("SatLo", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistSatLo);
    if (name.Contains("Islands", TString::kIgnoreCase))
        return const_cast<TH1F*>(&fHistIslands);

    return NULL;
}
