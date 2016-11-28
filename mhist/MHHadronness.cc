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
!   Author(s): Thomas Bretz, 5/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Abelardo Moralejo <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHHadronness
//
// This is histogram is a way to evaluate the quality of a gamma/hadron
// seperation method. It is filled from a MHadronness container, which
// stores a hadroness for the current event. The Value must be in the
// range [0,1]. To fill the histograms correctly the information
// whether it is a gamma or hadron (not a gamma) must be available from
// a MMcEvt container.
//
// In the constructor you can change the number of used bns for the
// evaluation.
//
// The meaning of the histograms (Draw, DrawClone) are the following:
//  * Upper Left Corner:
//    - black: histogram of all hadronesses for gammas
//    - red:   histogram of all hadronesses for non gammas
//  * Upper Right Corner:
//    - black: acceptance of gammas (Ag) vs. the hadroness
//    - red:   acceptance of non gammas (Ah) vs. the hadroness
//  * Bottom Left Corner:
//    Naive quality factor: Ag/sqrt(Ah)
//  * Bottom Right Corner:
//    - black: Acceptance Gammas vs. Acceptance Hadrons
//    - blue cross: minimum of distance to (0, 1)
//
// As a default MHHadronness searches for the container "MHadronness".
// This can be overwritten by a different pointer specified in the
// Fill function (No type check is done!) Use a different name in
// MFillH.
//
// If you are using filtercuts which gives you only two discrete values
// of the hadronness (0.25 and 0.75) you may want to use MHHadronness(2).
// If you request Q05() from such a MHHadronness instance you will get
// Acc_g/sqrt(Acc_h)
//
////////////////////////////////////////////////////////////////////////////
#include "MHHadronness.h"

#include <TH1.h>
#include <TPad.h>
#include <TMath.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMarker.h>

#include "MParList.h"
#include "MBinning.h"
#include "MHMatrix.h"
#include "MParameters.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MMcEvt.hxx"

ClassImp(MHHadronness);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup histograms, nbins is the number of bins used for the evaluation.
// The default is 100 bins.
//
MHHadronness::MHHadronness(Int_t nbins, const char *name, const char *title)
    : fMatrix(NULL)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHHadronness";
    fTitle = title ? title : "Gamma/Hadron Separation Quality Histograms";

    fGraph = new TGraph;
    fGraph->SetTitle("Acceptance Gammas vs. Hadrons");
    fGraph->SetMarkerStyle(kFullDotSmall);

    fGhness = new TH1D("Ghness", "Acceptance vs. Hadronness (Gammas)",  nbins, 0, 1.0001);
    fPhness = new TH1D("Phness", "Acceptance vs. Hadronness (Hadrons)", nbins, 0, 1.0001);
    fGhness->SetXTitle("Hadronness");
    fPhness->SetXTitle("Hadronness");
    fGhness->SetYTitle("Acceptance");
    fPhness->SetYTitle("Acceptance");
    fPhness->SetLineColor(kRed);
    fGhness->SetDirectory(NULL);
    fPhness->SetDirectory(NULL);
    
    fIntGhness = new TH1D("AccGammas",  "Integral Acceptance vs. Hadronness (Gammas)", nbins, 0, 1);
    fIntPhness = new TH1D("AccHadrons", "Integral Acceptance vs. Hadronness (Hadrons)", nbins, 0, 1);
    fIntGhness->SetXTitle("Hadronness");
    fIntPhness->SetXTitle("Hadronness");
    fIntGhness->SetYTitle("Acceptance");
    fIntPhness->SetYTitle("Acceptance");
    fIntGhness->SetMaximum(1);
    fIntPhness->SetMaximum(1);
    fIntGhness->SetMinimum(0);
    fIntPhness->SetMinimum(0);
    fIntGhness->SetDirectory(NULL);
    fIntPhness->SetDirectory(NULL);
    fIntPhness->SetLineColor(kRed);
    fIntGhness->SetBit(TH1::kNoStats);
    fIntPhness->SetBit(TH1::kNoStats);

    fQfac = new TGraph;
    fQfac->SetTitle(" Naive Quality factor ");
    fQfac->SetMarkerStyle(kFullDotSmall);
}

// --------------------------------------------------------------------------
//
// Delete the histograms.
//
MHHadronness::~MHHadronness()
{
    delete fGhness;
    delete fIntGhness;
    delete fPhness;
    delete fIntPhness;
    delete fQfac;
    delete fGraph;
}

// --------------------------------------------------------------------------
//
// Setup Filling of the histograms. It needs:
//  MMcEvt and MHadronness
//
Bool_t MHHadronness::SetupFill(const MParList *plist)
{
    if (!fMatrix)
    {
        fMcEvt = (MMcEvt*)plist->FindObject(AddSerialNumber("MMcEvt"));
        if (!fMcEvt)
        {
            *fLog << err << dbginf << AddSerialNumber("MMcEvt");
            *fLog << " not found... aborting." << endl;
            return kFALSE;
        }
    }

    fHadronness = (MParameterD*)plist->FindObject("MHadronness");

//    fGhness->Reset();
//    fPhness->Reset();

    /*
     MBinning* bins = (MBinning*)plist->FindObject("BinningHadronness");
     if (!bins)
     {
     *fLog << err << dbginf << "BinningHadronness [MBinning] not found... aborting." << endl;
     return kFALSE;
     }

     SetBinning(&fHist, binsalpha, binsenergy, binstheta);

     fHist.Sumw2();
     */

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the Hadronness from a MHadronness container into the corresponding
// histogram dependant on the particle id.
//
// Every particle Id different than kGAMMA is considered a hadron.
//
// If you call Fill with a pointer (eg. from MFillH) this container is
// used as a hadronness (Warning: No type check is done!) otherwise
// the default is used ("MHadronness")
//
// Sometimes a distance is calculated as NaN (not a number). Such events
// are skipped at the moment.
//
Int_t MHHadronness::Fill(const MParContainer *par, const Stat_t w)
{
    // Preliminary Workaround: FIXME!
    if (!par && !fHadronness)
    {
        *fLog << err << "MHHadronness::Fill: No MHadronness container specified!" << endl;
        return kERROR;
    }

    const MParameterD &had = par ? *(MParameterD*)par : *fHadronness;

    const Double_t h = TMath::Min(TMath::Max(had.GetVal(), 0.), 1.);

    if (!TMath::Finite(h))
        return kTRUE;  // Use kCONTINUE with extreme care!

    const Int_t particleid = fMatrix ? (Int_t)(*fMatrix)[fMap] : fMcEvt->GetPartId();

    if (particleid==MMcEvt::kGAMMA)
        fGhness->Fill(h, w);
    else
        fPhness->Fill(h, w);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Returns the quality factor at gamma acceptance 0.5.
//
// If the histogram containes only two bins we return the
// naive quality: ag/sqrt(ah)
// with ag the acceptance of gammas and ah the acceptance of hadrons.
//
// You can use this (nbins=2) in case of some kind of filter cuts giving
// only a result: gamma yes/no (means discrete values of hadronness 0.25
// or 0.75)
//
// FIXME: In the later case weights cannot be used!
//
Float_t MHHadronness::GetQ05() const
{
    if (fGhness->GetNbinsX()==2)
    {
        // acceptance of all gamma-like gammas  (h<0.5)
        const Double_t ig = fGhness->GetBinContent(1);

        // acceptance of all gamma-like hadrons (h<0.5)
        const Double_t ip = fPhness->GetBinContent(1);

        if (ip==0)
            return 0; // FIXME!

        // naive quality factor
        const Double_t q = ig / sqrt(ip);

        *fLog << all << ip << "/" << ig << ": " << q << endl;

        return q;
    }

    const Int_t n = fQfac->GetN();

    Double_t val1x=0;
    Double_t val2x=1;

    Double_t val1y=0;
    Double_t val2y=0;

    for (Int_t i=1; i<=n; i++)
    {
        Double_t x, y;

        fQfac->GetPoint(i, x, y);

        if (x<0.5 && x>val1x)
        {
            val1x = x;
            val1y = y;
        }

        if (x>0.5 && x<val2x)
        {
            val2x = x;
            val2y = y;
        }
    }

    //*fLog << dbg << val1x << "/" << val1y << "  " << val2x << "/" << val2y << endl;

    return val2x-val1x == 0 ? 0 : val1y - (val2y-val1y)/(val2x-val1x) * (val1x-0.5);
}

// --------------------------------------------------------------------------
//
// Finalize the histograms:
//  - integrate the hadroness histograms --> acceptance
//  - fill the Minimum Distance histogram (formular see class description)
//  - fill the Quality histogram (formular see class description)
//
void MHHadronness::CalcGraph(Double_t sumg, Double_t sump)
{
    Int_t n = fGhness->GetNbinsX();

    fGraph->Set(n);
    fQfac->Set(n);

    // Calculate acceptances
    Float_t max=0;

    for (Int_t i=1; i<=n; i++)
    {
        const Stat_t ip = fPhness->Integral(1, i)/sump;
        const Stat_t ig = fGhness->Integral(1, i)/sumg;

        fIntPhness->SetBinContent(i, ip);
        fIntGhness->SetBinContent(i, ig);

        fGraph->SetPoint(i, ip, ig);

        if (ip<=0)
            continue;

        const Double_t val = ig/sqrt(ip);
        fQfac->SetPoint(i, ig, val);

        if (val>max)
            max = val;
    }

    fQfac->SetMaximum(max*1.05);
}

Bool_t MHHadronness::Finalize()
{
    const Stat_t sumg = fGhness->Integral();
    const Stat_t sump = fPhness->Integral();

    *fLog << inf << "Sum Hadronness: gammas=" << sumg << " hadrons=" << sump << endl;

    // Normalize photon distribution
    if (sumg>0)
        fGhness->Scale(1./sumg);
    else
        *fLog << warn << "Cannot calculate hadronness for 'gammas'." << endl;

    // Normalize hadron distribution
    if (sump>0)
        fPhness->Scale(1./sump);
    else
        *fLog << warn << "Cannot calculate hadronness for 'hadrons'." << endl;

    CalcGraph(1, 1);

    return kTRUE;
}

void MHHadronness::Paint(Option_t *opt)
{
    Stat_t sumg = fGhness->Integral();
    Stat_t sump = fPhness->Integral();

    // Normalize photon distribution
    if (sumg<=0)
        sumg=1;

    // Normalize hadron distribution
    if (sump<=0)
        sump=1;

    CalcGraph(sumg, sump);
}

// --------------------------------------------------------------------------
//
// Search the corresponding points for the given hadron acceptance (acchad)
// and interpolate the tow points (linear)
//
Double_t MHHadronness::GetGammaAcceptance(Double_t acchad) const
{
    const Int_t n = fGraph->GetN();
    const Double_t *x = fGraph->GetX();
    const Double_t *y = fGraph->GetY();

    Int_t i = 0;
    while (i<n && x[i]<acchad)
        i++;

    if (i==0 || i==n)
        return 0;

    if (i==n-1)
        i--;

    const Double_t x1 = x[i-1];
    const Double_t y1 = y[i-1];

    const Double_t x2 = x[i];
    const Double_t y2 = y[i];

    return (y2-y1)/(x2-x1) * (acchad-x2) + y2;
}

// --------------------------------------------------------------------------
//
// Search the corresponding points for the given gamma acceptance (accgam)
// and interpolate the tow points (linear)
//
Double_t MHHadronness::GetHadronAcceptance(Double_t accgam) const
{
    const Int_t n = fGraph->GetN();
    const Double_t *x = fGraph->GetX();
    const Double_t *y = fGraph->GetY();

    Int_t i = 0;
    while (i<n && y[i]<accgam)
        i++;

    if (i==0 || i==n)
        return 0;

    if (i==n-1)
        i--;

    const Double_t x1 = y[i-1];
    const Double_t y1 = x[i-1];

    const Double_t x2 = y[i];
    const Double_t y2 = x[i];

    return (y2-y1)/(x2-x1) * (accgam-x2) + y2;
}

// --------------------------------------------------------------------------
//
// Search the hadronness corresponding to a given hadron acceptance.
//
Double_t MHHadronness::GetHadronness(Double_t acchad) const
{
    for (int i=1; i<fIntPhness->GetNbinsX()+1; i++)
        if (fIntPhness->GetBinContent(i)>acchad)
            return fIntPhness->GetBinLowEdge(i);

    return -1;
}

// --------------------------------------------------------------------------
//
// Print the corresponding Gammas Acceptance for a hadron acceptance of
// 10%, 20%, 30%, 40% and 50%. Also the minimum distance to the optimum
// acceptance and the corresponding acceptances and hadroness value is
// printed, together with the maximum Q-factor.
//
void MHHadronness::Print(Option_t *) const
{
    *fLog << all;
    *fLog << underline << GetDescriptor() << endl;

    if (fGraph->GetN()==0)
    {
        *fLog << " <No Entries>" << endl;
        return;
    }

    *fLog << "Used " << fGhness->GetEntries() << " Gammas and " << fPhness->GetEntries() << " Hadrons." << endl;
    *fLog << "acc(hadron) acc(gamma) acc(g)/acc(h)  h" << endl <<endl;

    *fLog << "    0.005    " << Form("%6.3f", GetGammaAcceptance(0.005)) << "      " << Form("%6.3f", GetGammaAcceptance(0.005)/0.005) << "      " << GetHadronness(0.005) << endl;
    *fLog << "    0.02     " << Form("%6.3f", GetGammaAcceptance(0.02))  << "      " << Form("%6.3f", GetGammaAcceptance(0.02)/0.02)   << "      " << GetHadronness(0.02) << endl;
    *fLog << "    0.05     " << Form("%6.3f", GetGammaAcceptance(0.05))  << "      " << Form("%6.3f", GetGammaAcceptance(0.05)/0.05)   << "      " << GetHadronness(0.05) << endl;
    *fLog << "    0.1      " << Form("%6.3f", GetGammaAcceptance(0.1 ))  << "      " << Form("%6.3f", GetGammaAcceptance(0.1)/0.1)     << "      " << GetHadronness(0.1) << endl;
    *fLog << "    0.2      " << Form("%6.3f", GetGammaAcceptance(0.2 ))  << "      " << Form("%6.3f", GetGammaAcceptance(0.2)/0.2)     << "      " << GetHadronness(0.2) << endl;
    *fLog << "    0.3      " << Form("%6.3f", GetGammaAcceptance(0.3 ))  << "      " << Form("%6.3f", GetGammaAcceptance(0.3)/0.3)     << "      " << GetHadronness(0.3) << endl;
    *fLog << Form("%6.3f", GetHadronAcceptance(0.1)) << "        0.1  " << endl;
    *fLog << Form("%6.3f", GetHadronAcceptance(0.2)) << "        0.2  " << endl;
    *fLog << Form("%6.3f", GetHadronAcceptance(0.3)) << "        0.3  " << endl;
    *fLog << Form("%6.3f", GetHadronAcceptance(0.4)) << "        0.4  " << endl;
    *fLog << Form("%6.3f", GetHadronAcceptance(0.5)) << "        0.5  " << endl;
    *fLog << Form("%6.3f", GetHadronAcceptance(0.6)) << "        0.6  " << endl;
    *fLog << Form("%6.3f", GetHadronAcceptance(0.7)) << "        0.7  " << endl;
    *fLog << Form("%6.3f", GetHadronAcceptance(0.8)) << "        0.8  " << endl;
    *fLog << endl;

    *fLog << "Q-Factor @ Acc Gammas=0.5: Q(0.5)=" << Form("%.1f", GetQ05()) << endl;
    *fLog << "  Acc Hadrons = " << Form("%5.1f", GetHadronAcceptance(0.5)*100) << "%" << endl;
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Draw all histograms. (For the Meaning see class description)
//
void MHHadronness::Draw(Option_t *)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas("Hadronness", fTitle);
    pad->SetBorderMode(0);

    AppendPad("");

    pad->Divide(2, 2);

    TH1 *h;

    pad->cd(1);
    gPad->SetBorderMode(0);
    //gStyle->SetOptStat(10);
    MH::DrawSame(*fGhness, *fPhness, "Hadronness"); // Displ both stat boxes

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fIntGhness->Draw();
    fIntPhness->Draw("same");

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fQfac->Draw("A*");
    gPad->Modified();
    gPad->Update();
    if ((h=fQfac->GetHistogram()))
    {
        h->GetXaxis()->SetRangeUser(0, 1);
        h->SetXTitle("Acceptance Gammas");
        h->SetYTitle("Quality");
        fQfac->Draw("P");
        gPad->Modified();
        gPad->Update();
    }

    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fGraph->Draw("AC");
    gPad->Modified();
    gPad->Update();
    if ((h=fGraph->GetHistogram()))
    {
        h->GetXaxis()->SetRangeUser(0, 1);
        h->SetXTitle("Acceptance Hadrons");
        h->SetYTitle("Acceptance Gammas");
        fGraph->SetMaximum(1);
        fGraph->Draw("P");
        gPad->Modified();
        gPad->Update();
    }
}

// --------------------------------------------------------------------------
//
// You can use this function if you want to use a MHMatrix instead of
// MMcEvt. This function adds all necessary columns to the
// given matrix. Afterward you should fill the matrix with the corresponding
// data (eg from a file by using MHMatrix::Fill). If you now loop
// through the matrix (eg using MMatrixLoop) MHHadronness::Fill
// will take the values from the matrix instead of the containers.
//
void MHHadronness::InitMapping(MHMatrix *mat)
{
    if (fMatrix)
        return;

    fMatrix = mat;

    TString str = AddSerialNumber("MMcEvt");
    str += ".fPartId";

    fMap = fMatrix->AddColumn(str);
}

void MHHadronness::StopMapping()
{
    fMatrix = NULL; 
}
