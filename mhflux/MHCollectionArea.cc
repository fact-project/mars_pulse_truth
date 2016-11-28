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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Harald Kornmayer 1/2001
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MHCollectionArea
//
//  Class Version 2:
//  ----------------
//   + added //! to fMcEvt which was missing before
//   - removed obsolete data member fEnergy
//
//////////////////////////////////////////////////////////////////////////////
#include "MHCollectionArea.h" 

#include <TMath.h>

#include <TLatex.h>
#include <TCanvas.h>
#include <TPaveStats.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"
#include "MBinning.h"

#include "MMcEvt.hxx"
#include "MMcRunHeader.hxx"
#include "MMcCorsikaRunHeader.h"

#include "MParList.h"
#include "MParameters.h"

ClassImp(MHCollectionArea);

using namespace std;

// --------------------------------------------------------------------------
//
//  Creates the three necessary histograms:
//   - selected showers (input)
//   - all showers (input)
//   - collection area (result)
//
MHCollectionArea::MHCollectionArea(const char *name, const char *title)
    : fMcEvt(0), fHeader(0), fMcAreaRadius(-1),
    fTotalNumSimulatedShowers(0), fAllEvtsTriggered(kFALSE),
    fIsExtern(kFALSE)
{ 
    //   initialize the histogram for the distribution r vs E
    //
    //   we set the energy range from 2 Gev to 20000 GeV (in log 4 orders
    //   of magnitude) and for each order we take 25 subdivision --> 100 xbins
    //
    //   we set the radius range from 0 m to 500 m with 10 m bin --> 50 ybins
    //
    fName  = name  ? name  : "MHCollectionArea";
    fTitle = title ? title : "Collection Area vs. Energy/Theta";

    fHistSel.SetName("SelEvts");
    fHistSel.SetTitle("Number of Events after cuts");
    fHistSel.SetXTitle("\\Theta [deg]");
    fHistSel.SetYTitle("E [GeV]");
    fHistSel.SetDirectory(NULL);
    fHistSel.UseCurrentStyle();
    fHistSel.SetLineColor(kBlue);

    fHistAll.SetName("AllEvts");
    fHistAll.SetTitle("Number of events produced");
    fHistAll.SetXTitle("\\Theta [deg]");
    fHistAll.SetYTitle("E_{mc} [GeV]");
    fHistAll.SetDirectory(NULL);
    fHistAll.UseCurrentStyle();

    fHEnergy.SetName("CollEnergy");
    fHEnergy.SetTitle("Collection Area");
    fHEnergy.SetXTitle("E [GeV]");
    fHEnergy.SetYTitle("A_{eff} [m^{2}]");
    fHEnergy.SetDirectory(NULL);
    fHEnergy.UseCurrentStyle();

    MBinning binsa, binse, binst;
    binse.SetEdgesLog(21, 6.3, 100000);
    binst.SetEdgesASin(67, -0.005, 0.665);

    binse.Apply(fHEnergy);

    MH::SetBinning(fHistSel, binst, binse);
    MH::SetBinning(fHistAll, binst, binse);

    // For some unknown reasons this must be called after
    // the binning has been initialized at least once
    fHistSel.Sumw2();
    fHistAll.Sumw2();
    fHEnergy.Sumw2();
}

// --------------------------------------------------------------------------
//
// Return the Area defined by fMcAreaRadius
//
Double_t MHCollectionArea::GetCollectionAreaAbs() const
{
    return TMath::Pi()*fMcAreaRadius*fMcAreaRadius;
}

// --------------------------------------------------------------------------
//
//  Calculate the Efficiency (collection area) and set the 'ReadyToSave'
//  flag
//
void MHCollectionArea::CalcEfficiency()
{
    TH1D *hsel = fHistSel.ProjectionY("Spy", -1, -1, "E");;
    TH1D *hall = fHistAll.ProjectionY("Apy", -1, -1, "E");

    //
    // Impact parameter range.
    //
    const Float_t totalarea = GetCollectionAreaAbs();//TMath::Pi() * (r2*r2 - r1*r1);

    // "b" option: calculate binomial errors
    // Do not use totalarea inside the binomial error calculation:
    //  it is not a weight.
    fHEnergy.Divide(hsel, hall, 1, 1, "b");
#if ROOT_VERSION_CODE < ROOT_VERSION(5,13,04)
    MH::SetBinomialErrors(fHEnergy, *hsel, *hall);
#endif
    if (fMcAreaRadius>0)
        fHEnergy.Scale(totalarea);

    delete hsel;
    delete hall;
}


Bool_t MHCollectionArea::SetupFill(const MParList *pl)
{
    fHistSel.Reset();
    if (!fIsExtern)
        fHistAll.Reset();

    fHeader = (MMcRunHeader*)pl->FindObject("MMcRunHeader");
    if (!fHeader)
    {
        *fLog << err << "MMcRunHeader not found... abort." << endl;
        return kFALSE;
    }

    fMcEvt = (MMcEvt*)pl->FindObject("MMcEvt");
    if (!fMcEvt)
    {
        *fLog << err << "MMcEvt not found... abort." << endl;
        return kFALSE;
    }
    /*
    fEnergy = (MParameterD*)pl->FindObject("MEnergyEst", "MParameterD");
    if (!fEnergy)
    {
        *fLog << err << "MEnergyEst [MParameterD] not found... abort." << endl;
        return kFALSE;
    }
    */
    MBinning binst, binse;
    binst.SetEdges(fHistAll, 'x');
    binse.SetEdges(fHistAll, 'y');

    //if (!fIsExtern)
    {
        MBinning *bins = (MBinning*)pl->FindObject("BinningTheta", "MBinning");
        if (bins)
            binst.SetEdges(*bins);

        bins = (MBinning*)pl->FindObject("BinningEnergyEst", "MBinning");
        if (bins)
            binse.SetEdges(*bins);
    }

    binse.Apply(fHEnergy);

    MH::SetBinning(fHistSel, binst, binse);
    MH::SetBinning(fHistAll, binst, binse);

    fMcAreaRadius   = -1;
    fCorsikaVersion =  0;

    return kTRUE;
}

void MHCollectionArea::GetImpactMax()
{
    if (fHeader->GetImpactMax()<=fMcAreaRadius*100 || fHeader->GetImpactMax()<0)
        return;

    fMcAreaRadius = 0.01*fHeader->GetImpactMax(); // cm->m
    *fLog << inf << "Maximum simulated impact: " << fMcAreaRadius << "m" << endl;
}

Bool_t MHCollectionArea::ReInit(MParList *plist)
{
    GetImpactMax();

    if (fCorsikaVersion!=0 && fCorsikaVersion!=fHeader->GetCorsikaVersion())
    {
        *fLog << warn;
        *fLog << "Warning - Read files have different Corsika versions..." << endl;
        *fLog << "          Last file=" << fCorsikaVersion << "   New file=" << fHeader->GetCorsikaVersion() << endl;
    }
    fCorsikaVersion = fHeader->GetCorsikaVersion();

    if (fIsExtern)
        return kTRUE;

    fTotalNumSimulatedShowers += fHeader->GetNumSimulatedShowers();
    *fLog << inf << "Total Number of Simulated showers: " << fTotalNumSimulatedShowers << endl;

    fAllEvtsTriggered |= fHeader->GetAllEvtsTriggered();
    *fLog << inf << "Only triggered events avail: " << (fAllEvtsTriggered?"yes":"no") << endl;

    MMcCorsikaRunHeader *crh = (MMcCorsikaRunHeader*)plist->FindObject("MMcCorsikaRunHeader");
    if (!crh)
    {
        *fLog << err << "MMcCorsikaRunHeader not found... abort." << endl;
        return kFALSE;
    }

    //
    // Calculate approximately the original number of events in each
    // energy bin:
    //
    const Float_t emin = crh->GetELowLim();
    const Float_t emax = crh->GetEUppLim();
    const Float_t expo = 1 + crh->GetSlopeSpec();
    const Float_t k = fHeader->GetNumSimulatedShowers() /
        (pow(emax,expo) - pow(emin,expo));

    const Int_t nbiny = fHistAll.GetNbinsY();

    TAxis &axe = *fHistAll.GetYaxis();
    for (Int_t i = 1; i <= nbiny; i++)
    {
        const Float_t e1 = axe.GetBinLowEdge(i);
        const Float_t e2 = axe.GetBinLowEdge(i+1);

        if (e1 < emin || e2 > emax)
            continue;

        const Float_t events = k * (pow(e2, expo) - pow(e1, expo));
        //
        // We fill the i-th energy bin, with the total number of events
        // Second argument of Fill would be impact parameter of each
        // event, but we don't really need it for the collection area,
        // so we just put a dummy value (1.)
        //

        const Float_t energy = (e1+e2)/2.;
        for (int j=0; j<TMath::Nint(events); j++)
            fHistAll.Fill(0., energy);
        // you have  MMcRunHeader.fShowerThetaMin and MMcRunHeader.fShowerThetaMax
    }

    return kTRUE;
}

void MHCollectionArea::Paint(Option_t *option)
{
    // This is a workaround to support also older files
    if (TString(option)=="paint3")
        return;

    if (TString(option)=="paint4")
    {
        if (fMcAreaRadius<=0)
            return;

        const TString txt = MString::Format("r_{max}=%.0fm --> A_{max}=%.0fm^{2}",
                                            fMcAreaRadius, GetCollectionAreaAbs());

        TLatex text(0.31, 0.95, txt);
        text.SetBit(TLatex::kTextNDC);
        text.SetTextSize(0.04);
        text.Paint();
        return;
    }

    TVirtualPad *pad;

    TPaveStats *st=0;
    for (int x=0; x<4; x++)
    {
        pad=gPad->GetPad(x+1);
        if (!pad || !(st = (TPaveStats*)pad->GetPrimitive("stats")))
            continue;

        if (st->GetOptStat()==11)
            continue;

        const Double_t y1 = st->GetY1NDC();
        const Double_t y2 = st->GetY2NDC();
        const Double_t x1 = st->GetX1NDC();
        const Double_t x2 = st->GetX2NDC();

        st->SetY1NDC((y2-y1)/3+y1);
        st->SetX1NDC((x2-x1)/3+x1);
        st->SetOptStat(11);
    }

    pad = gPad;

    TH1 *h1=0, *h2=0;

    pad->cd(1);
    if (gPad->FindObject("ProjSelX"))
        fHistSel.ProjectionX("ProjSelX", -1, -1, "E");

    pad->cd(2);
    if (gPad->FindObject("ProjAllY"))
        h1=fHistAll.ProjectionY("ProjAllY", -1, -1, "E");
    if (gPad->FindObject("ProjSelY"))
        h2=fHistSel.ProjectionY("ProjSelY", -1, -1, "E");

    if (h1 && h1->GetMaximum()>0)
    {
        gPad->SetLogx();
        gPad->SetLogy();
    }

    pad->cd(3);
    TH1 *h=dynamic_cast<TH1*>(gPad->FindObject("Efficiency"));
    if (h1 && h2 && h)
    {
            h->Divide(h2, h1, 1, 1, "b");
#if ROOT_VERSION_CODE < ROOT_VERSION(5,13,04)
            MH::SetBinomialErrors(*h, *h2, *h1);
#endif
            h->SetMinimum(0);
    }

    pad->cd(4);
    CalcEfficiency();
    if (fHEnergy.GetMaximum()>0)
    {
        gPad->SetLogx();
        gPad->SetLogy();
    }
}

void MHCollectionArea::Draw(Option_t *option)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);

    // Do the projection before painting the histograms into
    // the individual pads
    AppendPad();

    pad->SetBorderMode(0);
    pad->Divide(2,2);

    TH1 *h=0, *h1=0, *h2=0;

    if (fHistSel.GetNbinsX()>1)
    {
        pad->cd(1);
        gPad->SetBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        /*
        h = fHistAll.ProjectionX("ProjAllX", -1, -1, "E");
        h->SetXTitle("\\Theta [\\circ]");
        h->SetDirectory(NULL);
        h->SetLineColor(kGreen);
        h->SetBit(kCanDelete);
        h->Draw();
        */
        h = fHistSel.ProjectionX("ProjSelX", -1, -1, "E");
        h->SetXTitle("\\Theta [\\circ]");
        h->SetDirectory(NULL);
        h->SetLineColor(kRed);
        h->SetBit(kCanDelete);
        h->Draw("hist"/*"same"*/);
    }
    else
        delete pad->GetPad(1);

    if (fHistSel.GetNbinsY()>1)
    {
        pad->cd(2);
        gPad->SetBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();

        h1 = fHistAll.ProjectionY("ProjAllY", -1, -1, "E");
        h1->SetDirectory(NULL);
        h1->SetLineColor(kGreen);
        h1->SetXTitle("E [GeV]");
        h1->SetBit(kCanDelete);
        h1->Draw();

        h2 = fHistSel.ProjectionY("ProjSelY", -1, -1, "E");
        h2->SetDirectory(NULL);
        h2->SetLineColor(kRed);
        h2->SetBit(kCanDelete);
        h2->Draw("same");
    }
    else
        delete pad->GetPad(2);

    if (h1 && h2)
    {
        pad->cd(3);
        gPad->SetBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        gPad->SetLogx();
        h = h2->DrawCopy();
        h->Divide(h2, h1, 1, 1, "b");
#if ROOT_VERSION_CODE < ROOT_VERSION(5,13,04)
        MH::SetBinomialErrors(*h, *h2, *h1);
#endif
        h->SetNameTitle("Efficiency", "Efficiency");
        h->SetDirectory(NULL);
        //AppendPad("paint3");
    }
    else
        delete pad->GetPad(4);

    if (fHEnergy.GetNbinsX()>1)
    {
        pad->cd(4);
        gPad->SetBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        fHEnergy.Draw();
        AppendPad("paint4");
    }
    else
        delete pad->GetPad(4);
}

Int_t MHCollectionArea::Fill(const MParContainer *par, const Stat_t weight)
{
    // This is not perfect because it selects the maximum impact only
    // from the selected events. Hoever, it will get overwritten
    // in finalize anyway.
    const Double_t impact = fMcEvt->GetImpact()*0.01; // cm->m
    if (impact>fMcAreaRadius)
        fMcAreaRadius = impact;

    const Double_t energy = fMcEvt->GetEnergy();
    const Double_t theta  = fMcEvt->GetTelescopeTheta()*TMath::RadToDeg();

    fHistSel.Fill(theta, energy, weight);

    return kTRUE;
}

Bool_t MHCollectionArea::Finalize()
{
    GetImpactMax();

    *fLog << all << "Maximum simulated impact found: " << fMcAreaRadius << "m" << endl;

    CalcEfficiency();

    return kTRUE;
}
