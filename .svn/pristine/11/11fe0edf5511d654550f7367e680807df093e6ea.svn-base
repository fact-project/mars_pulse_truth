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
!   Author(s): Wolfgang Wittek 4/2002 <mailto:wittek@mppmu.mpg.de>
!   Author(s): Abelardo Moralejo 5/2003 <mailto:moralejo@pd.infn.it>
!   Author(s): Thomas Bretz 1/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MHEnergyEst
//
//  calculates the migration matrix E-est vs. E-true
//  for different bins in Theta
//
//  Class Version 2:
//  - fHResolution
//  + fHResolutionEst
//  + fHResolutionMC
//
//////////////////////////////////////////////////////////////////////////////
#include "MHEnergyEst.h"

#include <TF1.h>
#include <TLine.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TProfile.h>
#include <TPaveStats.h>

#include "MString.h"

#include "MMcEvt.hxx"

#include "MBinning.h"
#include "MParList.h"
#include "MParameters.h"
#include "MHMatrix.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHEnergyEst);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor. It sets name and title of the histogram.
//
MHEnergyEst::MHEnergyEst(const char *name, const char *title)
    : fMcEvt(0), fEnergy(0), fResult(0), fMatrix(0)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHEnergyEst";
    fTitle = title ? title : "Histogram for the result of the energy reconstruction";

    //fNameEnergy = "MEnergyEst";
    //fNameResult = "MinimizationValue";

    fHEnergy.SetDirectory(NULL);
    fHEnergy.SetName("EnergyEst");
    fHEnergy.SetTitle("Histogram in E_{est}, E_{mc} and \\Theta");
    fHEnergy.SetXTitle("E_{est} [GeV]");
    fHEnergy.SetYTitle("E_{mc} [GeV]");
    fHEnergy.SetZTitle("\\Theta [\\circ]");

    fHResolutionEst.SetDirectory(NULL);
    fHResolutionEst.SetName("ResEnergyEst");
    fHResolutionEst.SetTitle("Histogram in \\Delta E/E_{est} vs E_{est}");
    fHResolutionEst.SetXTitle("E_{est} [GeV]");
    fHResolutionEst.SetYTitle("1-E_{mc}/E_{est}");

    fHResolutionMC.SetDirectory(NULL);
    fHResolutionMC.SetName("ResEnergyMC");
    fHResolutionMC.SetTitle("Histogram in \\Delta E/E_{mc} vs E_{mc}");
    fHResolutionMC.SetXTitle("E_{mc} [GeV]");
    fHResolutionMC.SetYTitle("E_{est}/E_{mc}-1");

    fHImpact.SetDirectory(NULL);
    fHImpact.SetName("Impact");
    fHImpact.SetTitle("\\Delta E/E vs Impact parameter");
    fHImpact.SetXTitle("Impact parameter [m]");
    fHImpact.SetYTitle("E_{est}/E_{mc}-1");

    MBinning binsi, binse, binst, binsr;
    binse.SetEdgesLog(21, 6.3, 100000);
    binst.SetEdgesASin(51, -0.005, 0.505);
    binsr.SetEdges(75, -1.75, 1.75);

    // Use the binning in impact to do efficiency studies
    binsi.SetEdges(1, 0, 1000);

    SetBinning(fHEnergy, binse, binse, binst);
    SetBinning(fHImpact, binsi, binsr);

    SetBinning(fHResolutionEst, binse, binsr);
    SetBinning(fHResolutionMC,  binse, binsr);

    // For some unknown reasons this must be called after
    // the binning has been initialized at least once
    fHEnergy.Sumw2();
    fHImpact.Sumw2();
    fHResolutionEst.Sumw2();
    fHResolutionMC.Sumw2();
}

// --------------------------------------------------------------------------
//
// Set the binnings and prepare the filling of the histograms
//
Bool_t MHEnergyEst::SetupFill(const MParList *plist)
{
    if (!fMatrix)
    {
        fMcEvt = (MMcEvt*)plist->FindObject("MMcEvt");
        if (!fMcEvt)
        {
            *fLog << err << "MMcEvt not found... aborting." << endl;
            return kFALSE;
        }
    }

    fEnergy = (MParameterD*)plist->FindObject("MEnergyEst", "MParameterD");
    if (!fEnergy)
    {
        *fLog << err << "MEnergyEst [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }

    fResult = (MParameterD*)const_cast<MParList*>(plist)->FindCreateObj("MParameterD", "MinimizationValue");
    if (!fResult)
        return kFALSE;

    MBinning binst, binse, binsi, binsr;
    binse.SetEdges(fHEnergy, 'x');
    binst.SetEdges(fHEnergy, 'z');
    binsi.SetEdges(fHImpact, 'x');
    binsr.SetEdges(fHImpact, 'y');

    binst.SetEdges(*plist, "BinningTheta");
    binse.SetEdges(*plist, "BinningEnergyEst");
    binsi.SetEdges(*plist, "BinningImpact");
    binsr.SetEdges(*plist, "BinningEnergyRes");

    SetBinning(fHEnergy, binse, binse, binst);
    SetBinning(fHImpact, binsi, binsr);

    SetBinning(fHResolutionEst, binse, binsr);
    SetBinning(fHResolutionMC,  binse, binsr);

    fChisq = 0;
    fBias  = 0;

    fHEnergy.Reset();
    fHImpact.Reset();

    fHResolutionEst.Reset();
    fHResolutionMC.Reset();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histogram
//
Int_t MHEnergyEst::Fill(const MParContainer *par, const Stat_t w)
{
    const Double_t eest  = fEnergy->GetVal();
    const Double_t etru  = fMatrix ? GetVal(0) : fMcEvt->GetEnergy();
    const Double_t imp   = fMatrix ? GetVal(1) : fMcEvt->GetImpact()/100;
    const Double_t theta = fMatrix ? GetVal(2) : fMcEvt->GetTelescopeTheta()*TMath::RadToDeg();

    const Double_t resEst  = (eest-etru)/eest;
    const Double_t resMC   = (eest-etru)/etru;

    fHEnergy.Fill(eest, etru, theta, w);
    fHImpact.Fill(imp, resEst, w);

    fHResolutionEst.Fill(eest, resEst, w);
    fHResolutionMC.Fill(etru, resMC, w);

    // For the fit we use a different quantity
    //const Double_t res = TMath::Log10(eest/etru);
    const Double_t res = eest-etru;

    fChisq += res*res;
    fBias  += res;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Divide chisq and bias by number of executions
// Print result
//
Bool_t MHEnergyEst::Finalize()
{
    fChisq /= GetNumExecutions();
    fBias  /= GetNumExecutions();

    fResult->SetVal(fChisq);

    *fLog << all << endl;
    Print();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print result
//
void MHEnergyEst::Print(Option_t *o) const
{
    const Double_t res = TMath::Sqrt(fChisq-fBias*fBias);
    if (!TMath::Finite(res))
    {
        *fLog << all << "MHEnergyEst::Print: ERROR - Resolution is not finite (eg. NaN)." << endl;
        return;
    }

    TH1D *h = (TH1D*)fHResolutionEst.ProjectionY("Dummy", -1, -1, "s");
    h->Fit("gaus", "Q0", "", -1.0, 0.25);

    TF1 *f = h->GetFunction("gaus");

    *fLog << all << "F=" << fChisq << endl;
    *fLog << "Results from Histogram:" << endl;
    *fLog << " Mean  of Delta E/E: " << Form("%+4.2f%%", 100*h->GetMean()) << endl;
    *fLog << " RMS   of Delta E/E: " << Form("%4.2f%%",  100*h->GetRMS()) << endl;
    *fLog << "Results from Histogram-Fit:" << endl;
    if (!f)
        *fLog << " <fit did not succeed>" << endl;
    else
    {
        *fLog << " Bias  of Delta E/E: " << Form("%+4.2f%%", 100*f->GetParameter(1)) << endl;
        *fLog << " Sigma of Delta E/E: " << Form("%4.2f%%",  100*f->GetParameter(2)) << endl;
        *fLog << " Res   of Delta E/E: " << Form("%4.2f%%",  100*TMath::Hypot(f->GetParameter(1), f->GetParameter(2))) << endl;
    }

    delete h;
}

// --------------------------------------------------------------------------
//
// Return Correction Coefficients (weights)
// hist = E_mc/E_est
//
void MHEnergyEst::GetWeights(TH1D &hist) const
{
    // Project into EnergyEst_ey
    // the "e" ensures that errors are calculated
    TH1D *h1 = (TH1D*)fHEnergy.Project3D("rtlprmft_ex"); // E_Est
    TH1D *h2 = (TH1D*)fHEnergy.Project3D("rtlprmft_ey"); // E_mc

    h2->Copy(hist);
    hist.Divide(h1);

    delete h1;
    delete h2;

    hist.SetNameTitle("EnergyRatio", "Ratio between estimated and monte carlo energy");
    hist.SetXTitle("E [GeV]");
    hist.SetYTitle("N_{mc}/N_{est} [1]");
    hist.SetDirectory(0);
}

void MHEnergyEst::Paint(Option_t *opt)
{
    TVirtualPad *pad = gPad;

    pad->cd(1);

    TH1 *hx=0;
    TH1 *hy=0;

    if (pad->GetPad(1))
    {
        pad->GetPad(1)->cd(1);

        if ((hx = dynamic_cast<TH1*>(gPad->FindObject("EnergyEst_ex"))))
        {
            hx->GetSumw2()->Set(0); // Workaround. Otherwise we get a warning because it is recreated
            hx = fHEnergy.Project3D("ex");
        }

        if ((hy = dynamic_cast<TH1*>(gPad->FindObject("EnergyEst_ey"))))
        {
            hy->GetSumw2()->Set(0); // Workaround. Otherwise we get a warning because it is recreated
            hy = fHEnergy.Project3D("ey");
        }

        if (hx && hy)
        {
            hx->SetLineColor(kBlue);
            hx->SetMarkerColor(kBlue);
            hy->SetMaximum();
            hy->SetMaximum(TMath::Max(hx->GetMaximum(), hy->GetMaximum())*1.2);
            if (hy->GetMaximum()>0)
                gPad->SetLogy();
        }

        if (pad->GetPad(1)->GetPad(2))
        {
            pad->GetPad(1)->GetPad(2)->cd(1);
            if ((hx = dynamic_cast<TH1*>(gPad->FindObject("EnergyEst_ez"))))
            {
                hx->GetSumw2()->Set(0); // Workaround. Otherwise we get a warning because it is recreated
                fHEnergy.Project3D("ez");
            }

            pad->GetPad(1)->GetPad(2)->cd(2);
            if (gPad->FindObject("ResEnergyEst_py"))
            {
                hx = (TH1D*)fHResolutionEst.ProjectionY("_py", -1, -1, "e");
                TPaveStats *stats = dynamic_cast<TPaveStats*>(hx->FindObject("stats"));
                if (stats)
                {
                    stats->SetBit(BIT(17)); // TStyle.cxx: kTakeStyle=BIT(17)
                    stats->SetX1NDC(0.63);
                    stats->SetY1NDC(0.68);
                }

                if (hx->GetEntries()>0)
                {
                    hx->Fit("gaus", "Q", "", -0.25, 1.0);
                    if (hx->GetFunction("gaus"))
                    {
                        hx->GetFunction("gaus")->SetLineColor(kBlue);
                        hx->GetFunction("gaus")->SetLineWidth(2);
                        gPad=NULL;
                        gStyle->SetOptFit(101);
                    }
                }
            }
        }
    }

    if (pad->GetPad(2))
    {
        pad->GetPad(2)->cd(1);
        if (gPad->FindObject("EnergyEst_yx"))
        {
            TH2D *hyx = static_cast<TH2D*>(fHEnergy.Project3D("yx"));
            UpdateProf(*hyx, kTRUE);
        }

        TLine *l = (TLine*)gPad->FindObject("TLine");
        if (l)
        {
            const Float_t min = TMath::Max(fHEnergy.GetXaxis()->GetXmin(), fHEnergy.GetYaxis()->GetXmin());
            const Float_t max = TMath::Min(fHEnergy.GetXaxis()->GetXmax(), fHEnergy.GetYaxis()->GetXmax());

            l->SetX1(min);
            l->SetX2(max);
            l->SetY1(min);
            l->SetY2(max);
        }

        pad->GetPad(2)->cd(2);
        UpdateProf(fHResolutionMC, kFALSE);

        pad->GetPad(2)->cd(3);
        UpdateProf(fHResolutionEst, kFALSE);
    }
}

void MHEnergyEst::UpdateProf(TH2 &h, Bool_t logy)
{
    const TString pname = MString::Format("Prof%s", h.GetName());

    if (!gPad->FindObject(pname))
        return;

    TH1D *hx = h.ProfileX(pname, -1, -1, "s");
    hx->SetLineColor(kBlue);
    hx->SetMarkerColor(kBlue);

    if (logy && hx->GetMaximum()>0)
        gPad->SetLogy();
}

TH1 *MHEnergyEst::MakeProj(const char *how)
{
    TH1 *p = fHEnergy.Project3D(how);
    p->SetDirectory(NULL);
    p->SetBit(kCanDelete);
    p->SetBit(TH1::kNoStats);
    p->SetMarkerStyle(kFullDotMedium);
    p->SetLineColor(kBlue);

    return p;
}

TH1 *MHEnergyEst::MakeProf(TH2 &h)
{
    TH1 *p = h.ProfileX(MString::Format("Prof%s", h.GetName()), -1, -1, "s");
    p->SetDirectory(NULL);
    p->SetBit(kCanDelete);
    p->SetLineWidth(2);
    p->SetLineColor(kBlue);
    p->SetFillStyle(4000);
    p->SetStats(kFALSE);

    return p;
}

// --------------------------------------------------------------------------
//
// Draw the histogram
//
void MHEnergyEst::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);

    // Do the projection before painting the histograms into
    // the individual pads
    AppendPad("");

    pad->SetBorderMode(0);
    pad->Divide(2, 1, 1e-10, 1e-10);

    TH1 *h;

    // ----------------------------------------

    pad->cd(1);
    gPad->SetBorderMode(0);

    gPad->Divide(1, 2, 1e-10, 1e-10);

    TVirtualPad *pad2 = gPad;

    // ----------------------------------------

    pad2->cd(1);
    gPad->SetBorderMode(0);

    gPad->SetGridx();
    gPad->SetGridy();
    gPad->SetLogx();

    h = MakeProj("ey");
    h->SetTitle("Energy disribution: Monte Carlo E_{mc} (black), Estimated E_{est} (blue)");
    h->SetXTitle("E [GeV]"); // E_mc
    h->SetYTitle("Counts");
    h->Draw();

    h->GetXaxis()->SetMoreLogLabels();
    h->GetXaxis()->SetNoExponent();

    h = MakeProj("ex");
    h->SetLineColor(kBlue);
    h->SetMarkerColor(kBlue);
    h->Draw("same");

    // ----------------------------------------

    pad2->cd(2);
    gPad->SetBorderMode(0);

    TVirtualPad *pad3 = gPad;
    pad3->Divide(2, 1, 1e-10, 1e-10);
    pad3->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();

    h = MakeProj("ez");
    h->SetTitle("Zenith Angle Distribution");
    h->GetXaxis()->SetMoreLogLabels();
    h->GetXaxis()->SetNoExponent();
    h->Draw();

    // ----------------------------------------

    pad3->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();

    h = fHResolutionEst.ProjectionY("_py");
    h->SetTitle("Distribution of \\Delta E/E_{est}");
    h->SetDirectory(NULL);
    h->SetBit(kCanDelete);
    h->GetXaxis()->SetRangeUser(-1.3, 1.3);
    h->Draw();
    // ----------------------------------------

    pad->cd(2);
    gPad->SetBorderMode(0);

    gPad->Divide(1, 3, 1e-10, 1e-10);
    pad2 = gPad;

    // ----------------------------------------

    pad2->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetLogy();
    gPad->SetLogx();
    gPad->SetGridx();
    gPad->SetGridy();

    // Results in crashes....
    //gROOT->GetListOfCleanups()->Add(gPad); // WHY?

    TH2D *h2 = (TH2D*)fHEnergy.Project3D("yx");
    h2->SetDirectory(NULL);
    h2->SetBit(kCanDelete);
    h2->SetFillColor(kBlue);
    h2->SetTitle("Estimated energy E_{mc} vs Monte Carlo energy E_{est}");

    h2->Draw("");
    MakeProf(*h2)->Draw("E0 same");

    h2->GetXaxis()->SetMoreLogLabels();
    h2->GetXaxis()->SetNoExponent();

    TLine line;
    line.DrawLine(0,0,1,1);

    line.SetLineColor(kBlue);
    line.SetLineWidth(2);
    line.SetLineStyle(kDashed);

    // ----------------------------------------

    pad2->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetGridx();
    gPad->SetGridy();
    fHResolutionMC.Draw();
    MakeProf(fHResolutionMC)->Draw("E0 same");

    fHResolutionMC.GetXaxis()->SetMoreLogLabels();
    fHResolutionMC.GetXaxis()->SetNoExponent();

    line.DrawLine(fHResolutionMC.GetXaxis()->GetXmin(), 0, fHResolutionMC.GetXaxis()->GetXmax(), 0);

    // ----------------------------------------

    pad2->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetGridx();
    gPad->SetGridy();
    fHResolutionEst.Draw();
    MakeProf(fHResolutionEst)->Draw("E0 same");

    fHResolutionEst.GetXaxis()->SetMoreLogLabels();
    fHResolutionEst.GetXaxis()->SetNoExponent();

    line.DrawLine(fHResolutionEst.GetXaxis()->GetXmin(), 0, fHResolutionEst.GetXaxis()->GetXmax(), 0);
}

// --------------------------------------------------------------------------
//
// Returns the mapped value from the Matrix
//
Double_t MHEnergyEst::GetVal(Int_t i) const
{
    return (*fMatrix)[fMap[i]];
}

// --------------------------------------------------------------------------
//
// You can use this function if you want to use a MHMatrix instead of the
// given containers. This function adds all necessary columns to the
// given matrix. Afterward you should fill the matrix with the corresponding
// data (eg from a file by using MHMatrix::Fill). If you now loop
// through the matrix (eg using MMatrixLoop) MEnergyEstParam2::Process
// will take the values from the matrix instead of the containers.
//
void MHEnergyEst::InitMapping(MHMatrix *mat)
{
    if (fMatrix)
        return;

    fMatrix = mat;

    fMap[0] = fMatrix->AddColumn("MMcEvt.fEnergy");
    fMap[1] = fMatrix->AddColumn("MMcEvt.fImpact/100");
    fMap[2] = fMatrix->AddColumn("MMcEvt.fTelescopeTheta*kRad2Deg");
}

void MHEnergyEst::StopMapping()
{
    fMatrix = NULL; 
}
