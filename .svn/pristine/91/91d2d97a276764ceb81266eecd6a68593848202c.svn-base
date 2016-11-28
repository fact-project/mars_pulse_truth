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
!   Author(s): Thomas Bretz, 07/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHPhi
//
// Plot delta phi the angle between the reconstructed shower origin and
// the source position.
//
// More detail can be found at:
// http://www.astro.uni-wuerzburg.de/results/ringmethod/
//
// Class Version 2:
//   + TH1D fHPhi;             // Phi plot of the signal w.r.t. source
//   + TH1D fHPhiOff;          // Phi plot of the signal w.r.t. source+180deg
//   + TH1D fHTemplate;        // Template how the background should look in the ideal case
// 
//   + TH1D fHInhom;           // Phi plot off the signal w.r.t. source (out of the ring with the signal)
//   + TH1D fHInhomOff;        // Phi plot off the signal w.r.t. source+180deg (out of the ring with the signal)
// 
//   + Int_t   fNumBinsSignal; // Number of bins for signal region
//   + Float_t fThetaCut;      // Theta cut
//   + Float_t fDistSrc;       // Nominal distance of source from center in wobble
// 
//   + Bool_t  fUseAntiPhiCut; // Whether to use a anti-phi cut or not
//
////////////////////////////////////////////////////////////////////////////
#include "MHPhi.h"

#include <TCanvas.h>
#include <TVector2.h>
#include <TLatex.h>
#include <TLine.h>
#include <TMarker.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MHillas.h"
#include "MSrcPosCam.h"
#include "MParameters.h"
#include "MGeomCam.h"
#include "MBinning.h"
#include "MMath.h"

ClassImp(MHPhi);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup histograms 
//
MHPhi::MHPhi(const char *name, const char *title)
    : fHillas(0), fSrcPos(0), fDisp(0),
    fNumBinsSignal(2), fThetaCut(0.23), fDistSrc(0.4),
    fUseAntiPhiCut(kTRUE)
{
    fName  = name  ? name  : "MHPhi";
    fTitle = title ? title : "Graphs for rate data";

    // Init Graphs
    fHPhi.SetNameTitle("Phi", "\\Delta\\Phi-Distribution");
    fHPhi.SetXTitle("\\Delta\\Phi' [\\circ]");
    fHPhi.SetYTitle("Counts");

    fHPhi.SetMinimum(0);
    fHPhi.SetDirectory(0);
    fHPhi.Sumw2();
    fHPhi.SetBit(TH1::kNoStats);
    fHPhi.SetMarkerStyle(kFullDotMedium);
    fHPhi.SetLineColor(kBlue);
    fHPhi.SetMarkerColor(kBlue);
    fHPhi.GetYaxis()->SetTitleOffset(1.3);

    fHPhiOff.SetMinimum(0);
    fHPhiOff.SetDirectory(0);
    fHPhiOff.Sumw2();
    fHPhiOff.SetBit(TH1::kNoStats);
    fHPhiOff.SetLineColor(kRed);
    fHPhiOff.SetMarkerColor(kRed);

    fHTemplate.SetMinimum(0);
    fHTemplate.SetDirectory(0);
    fHTemplate.SetBit(TH1::kNoStats);
    fHTemplate.SetLineColor(kGreen);

    fHInhom.SetNameTitle("Inhomogeneity", "\\Delta\\Phi-Distribution");
    fHInhom.SetXTitle("\\Delta\\Phi' [\\circ]");
    fHInhom.SetYTitle("Counts");
    fHInhom.Sumw2();
    fHInhom.SetMinimum(0);
    fHInhom.SetDirectory(0);
    fHInhom.SetBit(TH1::kNoStats);
    fHInhom.GetYaxis()->SetTitleOffset(1.3);

    fHInhomOff.Sumw2();
    fHInhomOff.SetMinimum(0);
    fHInhomOff.SetDirectory(0);
    fHInhomOff.SetBit(TH1::kNoStats);
    fHInhomOff.SetLineColor(kRed);
    fHInhomOff.SetMarkerColor(kRed);
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning
//
Bool_t MHPhi::SetupFill(const MParList *plist)
{
    fHillas = (MHillas*)plist->FindObject("MHillas");
    if (!fHillas)
    {
        *fLog << err << "MHillas not found... abort." << endl;
        return kFALSE;
    }
    fSrcPos = (MSrcPosCam*)plist->FindObject("MSrcPosCam");
    if (!fSrcPos)
    {
        *fLog << err << "MSrcPosCam not found... abort." << endl;
        return kFALSE;
    }
    fDisp = (MParameterD*)plist->FindObject("Disp", "MParameterD");
    if (!fDisp)
    {
        *fLog << err << "Disp [MParameterD] not found... abort." << endl;
        return kFALSE;
    }

    MGeomCam *geom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!geom)
    {
        *fLog << err << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }

    fConvMm2Deg = geom->GetConvMm2Deg();

    MParameterD *cut = (MParameterD*)plist->FindObject("ThetaSquaredCut", "MParameterD");
    if (!cut)
        *fLog << warn << "ThetaSquareCut [MParameterD] not found... using default theta<" << fThetaCut << "." << endl;
    else
        fThetaCut = TMath::Sqrt(cut->GetVal());

    const Double_t w  = TMath::ATan(fThetaCut/fDistSrc);
    const Float_t  sz = TMath::RadToDeg()*w/fNumBinsSignal;
    const Int_t    n  = TMath::Nint(TMath::Ceil(180/sz));

    MBinning(n+3, 0, (n+3)*sz).Apply(fHPhi);
    MBinning(n+3, 0, (n+3)*sz).Apply(fHPhiOff);
    MBinning(n+3, 0, (n+3)*sz).Apply(fHTemplate);
    MBinning(n+3, 0, (n+3)*sz).Apply(fHInhom);
    MBinning(n+3, 0, (n+3)*sz).Apply(fHInhomOff);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MMuonCalibPar and
// MMuonSearchPar container.
//
Int_t MHPhi::Fill(const MParContainer *par, const Stat_t weight)
{
    // Here we calculate an upper phi cut to take a
    // possible anti-theta cut into account
    const Double_t ulim = fUseAntiPhiCut ? 180-fHPhi.GetBinLowEdge(fNumBinsSignal+1)*1.1 : 180;

    // Calculate the shower origin and the source position in units of deg
    const TVector2 pos = fHillas->GetMean()*fConvMm2Deg + fHillas->GetNormAxis()*fDisp->GetVal();
    const TVector2 src = fSrcPos->GetXY()*fConvMm2Deg;

    // Calculate radial distance between shower origin and source
    const Double_t d = pos.Mod() - src.Mod();

    // define an upper and lower cut for the radial distance between both
    const Double_t dR = fThetaCut;
    const Double_t dr = fThetaCut*0.913;

    // calculate the phi-angle of the shower origin w.r.t. the source position
    const Double_t delta = src.DeltaPhi(pos)*TMath::RadToDeg();

    // Define the radii of the upper and lower ring border
    const Double_t R = src.Mod()+dR;
    const Double_t r = src.Mod()-dr;

    // Calculate a scale to scale all source positions to the
    // nominal distance to center
    const Double_t scale = src.Mod()/fDistSrc;

    // Fill a phi-histograms with all events outside the ring
    // Take the upper phi cut into account
    if ((d<-dr || d>dR)/*TMath::Abs(d)>fThetaCut*1.2*/ && TMath::Abs(delta)<ulim)
    {
        fHInhom.Fill(TMath::Abs(delta)*scale,  weight);
        fHInhomOff.Fill((ulim-TMath::Abs(delta))*scale,  weight);
    }

    // Now forget about all events which are not inside the ring
    if (d<-dr || d>dR)
        return kTRUE;

    // Fill the histograms for on and off with the scaled phi
    // only if we are below the upper phi cut
    if (TMath::Abs(delta)<ulim)
    {
        fHPhi.Fill(         TMath::Abs(delta)*scale,  weight);
        fHPhiOff.Fill((ulim-TMath::Abs(delta))*scale, weight);
    }

    // Calculate the maximum scaled phi taking the upper phi cut into account
    const Double_t max = scale*ulim;

    // Fill a template, this is how the phi-plot would look like
    // without a signal and for an ideal camera.
    const Int_t n = fHTemplate.GetNbinsX();
    TArrayD arr(n);
    for (int i=1; i<=n; i++)
    {
        const Double_t hi = fHTemplate.GetBinLowEdge(i+1);
        const Double_t lo = fHTemplate.GetBinLowEdge(i);

        // Decide whether the bin is fully contained in the upper phi-cut or
        // the maximum possible phi is inside the bin
        if (hi<max)
            arr[i-1] = 1;
        else
            if (lo<max) // if its inside calculate the ratio
                arr[i-1] = (max-lo)/fHTemplate.GetBinWidth(i+1);
            else
                break;
    }

    // The template is scaled with the current ring width. The upper phi-
    // cut must not be taken into account because it is just a constant
    // for all events.
    const Double_t sum = arr.GetSum();
    for (int i=1; i<=n; i++)
        fHTemplate.AddBinContent(i, (R*R-r*r)*arr[i-1]/sum);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// This displays the TGraph like you expect it to be (eg. time on the axis)
// It should also make sure, that the displayed time always is UTC and
// not local time...
//
void MHPhi::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("update");

    pad->Divide(2,2);

    // --------------------------

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);

    fHPhi.Draw();
    fHPhiOff.Draw("same");

    TH1D *h1 = new TH1D(fHTemplate);
    h1->SetName("Template");
    h1->SetBit(kCanDelete);
    h1->SetDirectory(0);
    h1->Draw("same");

    AppendPad("result1");

    // --------------------------

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);

    fHInhom.Draw();
    fHInhomOff.Draw("same");

    TH1D *h2 = new TH1D(fHTemplate);
    h2->SetName("Template");
    h2->SetBit(kCanDelete);
    h2->SetDirectory(0);
    h2->Draw("same");

    // --------------------------

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);

    fHPhi.Draw();
    TH1D *h4 = new TH1D(fHInhom);
    h4->SetName("Inhomogeneity");
    h4->SetBit(kCanDelete);
    h4->SetDirectory(0);
    h4->Draw("same");

    h1->Draw("same");

    // --------------------------

    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);

    TH1D *h3 = new TH1D(fHPhi);
    h3->SetName("Result");
    h3->SetBit(kCanDelete);
    h3->SetDirectory(0);
    h3->Draw();

    h1->Draw("same");

    AppendPad("result4");

    // --------------------------
}

void MHPhi::PaintUpdate() const
{
    TVirtualPad *pad1 = gPad->GetPad(1);
    TVirtualPad *pad2 = gPad->GetPad(2);
    TVirtualPad *pad3 = gPad->GetPad(3);
    TVirtualPad *pad4 = gPad->GetPad(4);

    const Double_t i2 = fHInhom.Integral();
/*
    Double_t sig2 = 0;
    Double_t bg2  = 0;
    Double_t f2   = 1;
*/
    TH1D *htemp = pad1 ? dynamic_cast<TH1D*>(pad1->FindObject("Template")) : NULL;
    if (htemp)
    {
        fHTemplate.Copy(*htemp);
        htemp->SetName("Template");
        htemp->SetDirectory(0);

        Double_t sc1 = 1;
        Double_t sc2 = 1;

        TH1D *res = pad4 ? dynamic_cast<TH1D*>(pad4->FindObject("Result")) : NULL;
        if (res)
        {
            fHPhi.Copy(*res);

            const Double_t i0 = res->Integral(fNumBinsSignal+1, 9999);
            const Double_t i1 = fHInhom.Integral(fNumBinsSignal+1, 9999);
            const Double_t i3 = htemp->Integral();
            const Double_t i4 = htemp->Integral(fNumBinsSignal+1, 9999);

            // Scale inhomogeneity to match the phi-plot in the off-region
            sc1 = i1==0 ? 0 : i0/i1;
            // Scale inhomogeneity to match the phi-plot in the off-region
            sc2 = i3==0 ? 0 : i2/i3;

            res->Add(&fHInhom, -sc1);
            res->Add(htemp,     sc1*sc2);
            res->SetName("Result");
            res->SetDirectory(0);

            htemp->Scale(i4==0 ? 0 : i0/i4);

            //sig2 = res->Integral(1, fNumBinsSignal);
            //bg2  = fHPhi.Integral(fNumBinsSignal+1, 9999);
            //f2   = htemp->Integral(1, fNumBinsSignal)/i4;
        }

        TH1D *hinhom = pad3 ? dynamic_cast<TH1D*>(pad3->FindObject("Inhomogeneity")) : NULL;
        if (hinhom)
        {
            fHInhom.Copy(*hinhom);
            hinhom->SetName("Inhomogeneity");
            hinhom->SetDirectory(0);
            hinhom->Scale(sc1);
        }
    }

    htemp = pad2 ? dynamic_cast<TH1D*>(pad2->FindObject("Template")) : NULL;
    if (htemp)
    {
        const Double_t integ = htemp->Integral();

        fHTemplate.Copy(*htemp);
        htemp->Scale(integ==0 ? 0 : i2/integ);
        htemp->SetName("Template");
        htemp->SetDirectory(0);
    }
}

void MHPhi::PaintText(const TH1D &res) const
{
    const Double_t cut  = res.GetBinLowEdge(fNumBinsSignal+1);

    const Double_t sig  = res.Integral(1, fNumBinsSignal);
    const Double_t bg   = res.Integral(fNumBinsSignal+1, 9999);

    const Double_t f    = fHTemplate.Integral(1, fNumBinsSignal)/fHTemplate.Integral(fNumBinsSignal+1, 9999);

    const Double_t S0   = MMath::SignificanceLiMaSigned(sig,  bg*f);
    const Double_t S    = MMath::SignificanceLiMaSigned(sig,  bg, f);

    const TString fmt = Form("\\sigma_{L/M}=%.1f (\\sigma_{0}=%.1f)  \\Delta\\Phi_{on}<%.1f\\circ  E=%.0f  B=%.0f  f=%.2f",
                             S, S0, cut, sig-bg*f, bg*f, f);

    const Double_t b = bg             *f/fNumBinsSignal;
    const Double_t e = TMath::Sqrt(bg)*f/fNumBinsSignal;

    TLatex text(0.275, 0.95, fmt);
    text.SetBit(TLatex::kTextNDC);
    text.SetTextSize(0.042);
    text.Paint();

    TLine line;
    line.SetLineColor(14);
    line.PaintLine(cut, gPad->GetUymin(), cut, gPad->GetUymax());

    // Here we calculate an upper phi cut to take a
    // possible anti-theta cut into account
    const Double_t ulim = fUseAntiPhiCut ? 180-fHPhi.GetBinLowEdge(fNumBinsSignal+1)*1.1 : 180;
    line.SetLineStyle(kDotted);
    line.PaintLine(ulim, gPad->GetUymin(), ulim, gPad->GetUymax());

    line.SetLineStyle(kSolid);
    line.SetLineColor(kBlack);
    line.PaintLine(0, b, cut, b);
    line.PaintLine(cut/2, b-e, cut/2, b+e);
    line.SetLineStyle(kDashed);
    line.PaintLine(cut, b, fHPhi.GetXaxis()->GetXmax(), b);

    TMarker m;
    m.SetMarkerStyle(kFullDotMedium);
    m.PaintMarker(cut/2, b);
}

void MHPhi::Paint(Option_t *o)
{
    TString opt(o);
    if (opt=="update")
        PaintUpdate();
    if (opt=="result1")
        PaintText(fHPhi);
    if (opt=="result4")
    {
        TH1D *res = gPad ? dynamic_cast<TH1D*>(gPad->FindObject("Result")) : NULL;
        if (res)
            PaintText(*res);
    }
}

Int_t MHPhi::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "NumBinsSignal", print))
    {
        SetNumBinsSignal(GetEnvValue(env, prefix, "NumBinsSignal", (Int_t)fNumBinsSignal));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "UseAntiPhiCut", print))
    {
        SetUseAntiPhiCut(GetEnvValue(env, prefix, "UseAntiPhiCut", (Int_t)fUseAntiPhiCut));
        rc = kTRUE;
    }

    return rc;
}
