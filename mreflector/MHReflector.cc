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
!   Author(s): Thomas Bretz, 2/2007 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHReflector
//
/////////////////////////////////////////////////////////////////////////////
#include "MHReflector.h"

#include <TMath.h>
#include <TLegend.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MBinning.h"
#include "MString.h"

#include "MGeomCam.h"

#include "MRflEvtData.h"
#include "MRflEvtHeader.h"


ClassImp(MHReflector);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Creates and initializes the histograms
//
MHReflector::MHReflector(const char *name, const char *title)
{
    fName  = name  ? name  : "MHReflector";
    fTitle = title ? title : "Histogram for the reflected photons";

    fHistXY.SetName("ReflXY");
    fHistXY.SetTitle("Histogram vs X/Y and Energy");
    fHistXY.SetXTitle("X [\\circ]");
    fHistXY.SetYTitle("Y [\\circ]");
    fHistXY.SetZTitle("E [GeV]");
    fHistXY.SetDirectory(NULL);
    fHistXY.Sumw2();

    fHistRad.SetName("ReflRad");
    fHistRad.SetTitle("Histogram vs Radius and Energy");
    fHistRad.SetXTitle("E [GeV]");
    fHistRad.SetYTitle("R [\\circ]");
    fHistRad.SetZTitle("Cnts/deg^{2}");
    fHistRad.SetDirectory(NULL);
    fHistRad.Sumw2();

    fHistSize.SetName("ReflSize");
    fHistSize.SetTitle("Histogram vs Size and Energy");
    fHistSize.SetXTitle("E [GeV]");
    fHistSize.SetYTitle("N\\gamma");
    fHistSize.SetZTitle("Cnts");
    fHistSize.SetDirectory(NULL);
    fHistSize.Sumw2();

    MBinning binse, binsd, binsr;
    binse.SetEdgesLog(2*5, 10, 1000000);
    binsd.SetEdges(50, -2.5, 2.5);
    binsr.SetEdges(15, 0, 2.5);

    SetBinning(&fHistXY,   &binsd, &binsd, &binse);
    SetBinning(&fHistRad,  &binse, &binsr);
    SetBinning(&fHistSize, &binse, &binse);
}

// --------------------------------------------------------------------------
//
// Search for MRflEvtData, MRflEvtHeader and MGeomCam
//
Bool_t MHReflector::SetupFill(const MParList *pList)
{
    fHeader = (MRflEvtHeader*)pList->FindObject("MRflEvtHeader");
    if (!fHeader)
    {
        *fLog << err << "MRflEvtHeader not found... abort." << endl;
        return kFALSE;
    }
    fData = (MRflEvtData*)pList->FindObject("MRflEvtData");
    if (!fData)
    {
        *fLog << err << "MRflEvtData not found... abort." << endl;
        return kFALSE;
    }

    MGeomCam *geom = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!geom)
    {
        *fLog << err << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }

    fMm2Deg = geom->GetConvMm2Deg();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill position and radius versus energy
//
#include "MRflSinglePhoton.h"
Int_t MHReflector::Fill(const MParContainer *par, const Stat_t weight)
{
    const Double_t energy = fHeader->GetEnergy();

    const Int_t n = fData->GetNumPhotons();

    fHistSize.Fill(energy, n);

    for (int i=0; i<n; i++)
    {
        const MRflSinglePhoton &ph = fData->GetPhoton(i);

        const Double_t x = ph.GetX()*fMm2Deg;
        const Double_t y = ph.GetY()*fMm2Deg;
        const Double_t r = TMath::Hypot(x, y);
        const Double_t U = r*TMath::TwoPi();

        if (U==0)
            continue;

        fHistRad.Fill(energy, r, weight/U);
        //fHistXY.Fill(x, y, energy);
    }

    //fData->FillRad(fHistRad, energy/*x*/, fMm2Deg);
    //fData->Fill(fHistXY,     energy/*z*/, fMm2Deg);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Scale radial histogram with area
//
Bool_t MHReflector::Finalize()
{
    /*
    const TAxis &axey = *fHistRad.GetYaxis();
    for (int y=1; y<=fHistRad.GetNbinsY(); y++)
    {
        const Double_t lo = axey.GetBinLowEdge(y);
        const Double_t hi = axey.GetBinLowEdge(y+1);

        const Double_t A = (hi*hi-lo*lo)*TMath::Pi()*TMath::Pi();

        for (int x=1; x<=fHistRad.GetNbinsX(); x++)
            fHistRad.SetBinContent(x, y, fHistRad.GetBinContent(x, y)/A);
    }
    */
    return kTRUE;
}

#include "../mmc/MMcEvt.hxx"
void MHReflector::Draw(Option_t *o)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad();

    pad->Divide(3,2);

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    fHistRad.Draw("colz");

    pad->cd(2);
    gPad->SetBorderMode(0);
    TH1 *h = fHistRad.ProjectionY("ProfRad", -1, -1, "e");
    h->SetTitle("RadialProfile");
    h->SetDirectory(NULL);
    h->SetBit(kCanDelete);
    h->SetLineWidth(2);
    h->SetLineColor(kBlue);
    h->SetStats(kFALSE);
    h->Draw("");

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetLogy();
    h = fHistSize.ProjectionY("ProfSize", -1, -1, "e");
    h->SetTitle("Size distribution");
    h->SetDirectory(NULL);
    h->SetBit(kCanDelete);
    h->SetLineColor(kBlue);
    h->SetLineWidth(2);
    h->SetStats(kFALSE);
    h->Draw("");

    pad->cd(5);
    gPad->SetBorderMode(0);

    TLegend *leg = new TLegend(0.01, 0.01, 0.99, 0.99, "Energy Range");
    for (int i=1; i<=fHistRad.GetNbinsX(); i++)
    {
        h = fHistRad.ProjectionY(MString::Format("ProjRad%d", i), i, i, "e");
        h->SetDirectory(NULL);
        h->SetBit(kCanDelete);
        h->SetLineWidth(2);
        h->SetStats(kFALSE);
        h->Draw(i==1?"":"same");
        if (i==1)
            h->SetTitle("Radial Profile");

        const Float_t xlo = fHistRad.GetXaxis()->GetBinLowEdge(i);
        const Float_t xhi = fHistRad.GetXaxis()->GetBinUpEdge(i);

        const TString lo = MMcEvt::GetEnergyStr(xlo);
        const TString hi = MMcEvt::GetEnergyStr(xhi);

        leg->AddEntry(h, MString::Format("%s - %s", lo.Data(), hi.Data()));
    }

    pad->cd(4);
    gPad->SetBorderMode(0);
    leg->SetBit(kCanDelete);
    leg->Draw();

    pad->cd(6);
    gPad->SetBorderMode(0);

    gPad->SetLogx();
    gPad->SetLogy();

    for (int i=1; i<=fHistSize.GetNbinsX(); i++)
    {
        h = fHistSize.ProjectionY(MString::Format("ProjSize%d", i), i, i, "e");
        h->SetDirectory(NULL);
        h->SetBit(kCanDelete);
        h->SetLineWidth(2);
        h->SetStats(kFALSE);
        h->Draw(i==1?"":"same");
        if (i==1)
            h->SetTitle("Size distribution");
    }
}

void MHReflector::Paint(Option_t *opt)
{
    TVirtualPad *pad = gPad;

    pad->cd(1);
    SetPalette("pretty");

    TH1 *h=0;

    pad->cd(2);
    gPad->SetBorderMode(0);
    if (gPad->FindObject("ProfRad"))
    {
        h = fHistRad.ProjectionY("ProfRad", -1, -1, "e");
        h->Scale(1./h->Integral());
    }

    pad->cd(3);
    gPad->SetBorderMode(0);
    if (gPad->FindObject("ProfSize"))
    {
        h = fHistSize.ProjectionY("ProfSize", -1, -1, "e");
        h->Scale(1./h->Integral());
    }

    pad->cd(5);

    for (int i=1; i<=fHistRad.GetNbinsX(); i++)
    {
        const TString name = MString::Format("ProjRad%d", i);
        if (!gPad->FindObject(name))
            continue;

        h = fHistRad.ProjectionY(name, i, i, "e");
        h->SetLineColor(i);
        h->Scale(1./fHistRad.Integral());
        if (i==1)
            h->SetMaximum(fHistRad.GetMaximum()/fHistRad.Integral()*1.05);
    }

    pad->cd(6);

    for (int i=1; i<=fHistRad.GetNbinsX(); i++)
    {
        const TString name = MString::Format("ProjSize%d", i);
        if (!gPad->FindObject(name))
            continue;

        h = fHistSize.ProjectionY(name, i, i, "e");
        h->SetLineColor(i);
        h->Scale(1./fHistSize.Integral());
        if (i==1)
        {
            h->SetMaximum(TMath::Power(fHistSize.GetMaximum()/fHistSize.Integral(), 1.05));
            h->SetMinimum(TMath::Power(fHistSize.GetMinimum(0)/fHistSize.Integral(), 0.95));
        }
    }

}
