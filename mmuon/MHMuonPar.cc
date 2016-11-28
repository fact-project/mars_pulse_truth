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
!   Author(s): Markus Meyer, 02/2005 <mailto:meyer@astro.uni-wuerzburg.de>
!   Author(s): Thomas Bretz, 04/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHMuonPar
//
// This class is a histogram class for displaying muonparameters.
// The folowing histgrams will be plotted:
// - Radius (TH1F)
// - ArcWidth (TH1F)
// - ArcWidth/Radius vs Radius (TProfile) (it is the energy dependent
//   relative broadening of the muon ring)
// - Size Vs Radius
//
//
// Inputcontainer:
//   - MGeomCam
//   - MMuonSearchPar
//   - MMuonCalibPar
//
//
// Class Version 2:
// ----------------
//  - fMm2Deg
//
////////////////////////////////////////////////////////////////////////////
#include "MHMuonPar.h"

#include <TH1.h>
#include <TF1.h>
#include <TPad.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TProfile.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"

#include "MGeomCam.h"
#include "MBinning.h"
#include "MParList.h"

#include "MMuonSearchPar.h"
#include "MMuonCalibPar.h"

ClassImp(MHMuonPar);

using namespace std;

const Float_t MHMuonPar::fgIntegralLoLim = 0.751;
const Float_t MHMuonPar::fgIntegralUpLim = 1.199;

// --------------------------------------------------------------------------
//
// Setup histograms 
//
MHMuonPar::MHMuonPar(const char *name, const char *title) :
    fMuonSearchPar(NULL), fMuonCalibPar(NULL), fGeom(NULL)
{
    fName  = name  ? name  : "MHMuonPar";
    fTitle = title ? title : "Histograms of muon parameters";

    fHistRadius.SetName("Radius");
    fHistRadius.SetTitle("Distribution of Radius'");
    fHistRadius.SetXTitle("R [\\circ]");
    fHistRadius.SetYTitle("Counts");
    fHistRadius.GetXaxis()->SetTitleOffset(1.2);
    fHistRadius.SetDirectory(NULL);
    fHistRadius.UseCurrentStyle();
    fHistRadius.SetFillStyle(4000);

    fHistArcWidth.SetName("ArcWidth");
    fHistArcWidth.SetTitle("Distribution of ArcWidth");
    fHistArcWidth.SetXTitle("W [\\circ]");
    fHistArcWidth.GetXaxis()->SetTitleOffset(1.2);
    fHistArcWidth.SetYTitle("Counts");
    fHistArcWidth.SetDirectory(NULL);
    fHistArcWidth.UseCurrentStyle();
    fHistArcWidth.SetFillStyle(4000);

    fHistBroad.SetName("RingBroadening");
    fHistBroad.SetTitle("Profile ArcWidth vs Radius");
    fHistBroad.SetXTitle("R [\\circ]");
    fHistBroad.SetYTitle("W/R [1]");
    fHistBroad.GetXaxis()->SetTitleOffset(1.2);
    fHistBroad.SetDirectory(NULL);
    fHistBroad.UseCurrentStyle();
    fHistBroad.SetFillStyle(4000);

    fHistSize.SetName("SizeVsRadius");
    fHistSize.SetTitle("Profile MuonSize vs Radius");
    fHistSize.SetXTitle("R [\\circ]");
    fHistSize.SetYTitle("S [phe]");
    fHistSize.GetXaxis()->SetTitleOffset(1.2);
    fHistSize.SetDirectory(NULL);
    fHistSize.UseCurrentStyle();
    fHistSize.SetFillStyle(4000);

    MBinning bins;

    bins.SetEdges(20, 0.5, 1.5);
    bins.Apply(fHistRadius);

    bins.SetEdges(60, 0., 0.3);
    bins.Apply(fHistArcWidth);

    bins.SetEdges(20, 0.5, 1.5);
    bins.Apply(fHistBroad);

    bins.SetEdges(20, 0.5, 1.5);
    bins.Apply(fHistSize);
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning
//
Bool_t MHMuonPar::SetupFill(const MParList *plist)
{
    fGeom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << warn << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }

    fMuonSearchPar = (MMuonSearchPar*)plist->FindObject("MMuonSearchPar");
    if (!fMuonSearchPar)
    {
        *fLog << warn << "MMuonSearchPar not found... abort." << endl;
        return kFALSE;
    }
    fMuonCalibPar = (MMuonCalibPar*)plist->FindObject("MMuonCalibPar");
    if (!fMuonCalibPar)
    {
        *fLog << warn << "MMuonCalibPar not found... abort." << endl;
        return kFALSE;
    }

    ApplyBinning(*plist, "Radius",          fHistRadius);
    ApplyBinning(*plist, "ArcWidth",        fHistArcWidth);
    ApplyBinning(*plist, "RingBroadening",  fHistBroad);
    ApplyBinning(*plist, "SizeVsRadius",    fHistSize);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MMuonCalibPar and
// MMuonSearchPar container.
//
Int_t MHMuonPar::Fill(const MParContainer *par, const Stat_t w)
{
    const Double_t fMm2Deg = fGeom->GetConvMm2Deg();

    fHistRadius.Fill(fMm2Deg*fMuonSearchPar->GetRadius(), w);

    fHistArcWidth.Fill(fMuonCalibPar->GetArcWidth(), w);

    fHistBroad.Fill(fMm2Deg*fMuonSearchPar->GetRadius(),
                    fMuonCalibPar->GetArcWidth(), w);

    fHistSize.Fill(fMm2Deg*fMuonSearchPar->GetRadius(),
                   fMuonCalibPar->GetMuonSize(), w);

    return kTRUE;
}
// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the two histograms into it.
// Be careful: The histograms belongs to this object and won't get deleted
// together with the canvas.
//
void MHMuonPar::Draw(Option_t *)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    pad->Divide(2,2);

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fHistRadius.Draw();

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fHistArcWidth.Draw();

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fHistSize.Draw();

    TText txt;
    txt.SetTextColor(kBlue);

    TF1 ref("RefShape100%", "-77.07+1249.3*x-351.2*x*x", fgIntegralLoLim, fgIntegralUpLim);
    // old: 573*x + 430
    ref.SetLineColor(kBlue);
    ref.SetLineWidth(1);
    ref.SetLineStyle(kDashed);
    gROOT->GetListOfFunctions()->Remove(ref.DrawCopy("same"));

    txt.SetTextAlign(31);
    txt.DrawText(fgIntegralLoLim, ref.Eval(fgIntegralLoLim+0.05), "100%");

    AppendPad("pad3");

    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fHistBroad.Draw();

    ref.SetName("RefShape12mm");
    ref.Compile("- 0.02711*x + 0.09359");
    gROOT->GetListOfFunctions()->Remove(ref.DrawCopy("same"));

    txt.SetTextAlign(11);
    txt.DrawText(fgIntegralLoLim, ref.Eval(fgIntegralLoLim-0.05), "12mm");

    AppendPad("pad4");
}
/*
Double_t MHMuonPar::Integral(const TProfile &p, Int_t a, Int_t b) const
{
    Float_t numerator   = 0;
    Float_t denominator = 0;

    for (Int_t i=a; i<b; i++)
    {
        numerator   += p.GetBinContent(i)*p.GetBinEntries(i);
        denominator += p.GetBinEntries(i);
    }

    return denominator==0 ? 0 : numerator/denominator;
}
*/
Double_t MHMuonPar::Integral(const TProfile &p, Float_t a, Float_t b) const
{
    const Int_t bin1 = p.GetXaxis()->FindFixBin(a);
    const Int_t bin2 = p.GetXaxis()->FindFixBin(b);

    return p.Integral(bin1, bin2);
}

void MHMuonPar::Paint(Option_t *opt)
{
    if (TString(opt)==TString("pad4"))
    {
        const TString txt = MString::Format("\\Sigma_{%.2f\\circ}^{%.2f\\circ} = %.3f",
                                            fgIntegralLoLim, fgIntegralUpLim, Integral(fHistBroad));

        TLatex text(0.55, 0.93, txt);
        text.SetBit(TLatex::kTextNDC);
        text.SetTextSize(0.055);
        text.Paint();
    }

    if (TString(opt)==TString("pad3"))
    {
        const TString txt = MString::Format("\\Sigma_{%.2f\\circ}^{%.2f\\circ} = %.f",
                                            fgIntegralLoLim, fgIntegralUpLim, Integral(fHistSize));

        TLatex text(0.47, 0.93, txt);
        text.SetBit(TLatex::kTextNDC);
        text.SetTextSize(0.055);
        text.Paint();
    }
}
