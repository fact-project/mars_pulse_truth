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
!   Author(s): Thomas Bretz, 3/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MHAlpha
//
// Create a single Alpha-Plot. The alpha-plot is fitted online. You can
// check the result when it is filles in the MStatusDisplay
// For more information see MHFalseSource::FitSignificance
//
// For convinience (fit) the output significance is stored in a
// container in the parlisrt
//
// PRELIMINARY!
//
//
// ToDo:
// =====
//
//   - Replace time-binning by histogram (which is enhanced bin-wise)
//
//
//////////////////////////////////////////////////////////////////////////////
#include "MHAlpha.h"

#include <TStyle.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TPaveStats.h>

#include "MSrcPosCam.h"
#include "MHillas.h"
#include "MHillasSrc.h"
#include "MTime.h"
#include "MObservatory.h"
#include "MPointingPos.h"
#include "MAstroSky2Local.h"
#include "MStatusDisplay.h"
#include "MParameters.h"
#include "MHMatrix.h"

#include "MString.h"
#include "MBinning.h"
#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHAlpha);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor
//
MHAlpha::MHAlpha(const char *name, const char *title)
    : fNameParameter("MHillasSrc"), fParameter(0),
    fOffData(0), fResult(0), fSigma(0), fEnergy(0), fBin(0),
    fPointPos(0), fTimeEffOn(0), fTime(0), fNumTimeBins(10),
    fHillas(0), fMatrix(0), fSkipHistTime(kFALSE), fSkipHistTheta(kFALSE),
    fSkipHistEnergy(kFALSE), fForceUsingSize(kFALSE)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHAlpha";
    fTitle = title ? title : "Alpha plot";

    fHist.SetName("Alpha");
    fHist.SetTitle("Alpha");
    fHist.SetXTitle("\\Theta [deg]");
    //fHist.SetYTitle("E_{est} [GeV]");
    fHist.SetZTitle("|\\alpha| [\\circ]");
    fHist.SetDirectory(NULL);
    fHist.UseCurrentStyle();

    // Main histogram
    fHistTime.SetName("Alpha");
    fHistTime.SetXTitle("|\\alpha| [\\circ]");
    fHistTime.SetYTitle("Counts");
    fHistTime.UseCurrentStyle();
    fHistTime.SetDirectory(NULL);

    fHEnergy.SetName("Excess");
    //fHEnergy.SetTitle(" N_{exc} vs. E_{est} ");
    //fHEnergy.SetXTitle("E_{est} [GeV]");
    fHEnergy.SetYTitle("N_{exc}");
    fHEnergy.SetDirectory(NULL);
    fHEnergy.UseCurrentStyle();

    fHTheta.SetName("ExcessTheta");
    fHTheta.SetTitle(" N_{exc} vs. \\Theta ");
    fHTheta.SetXTitle("\\Theta [\\circ]");
    fHTheta.SetYTitle("N_{exc}");
    fHTheta.SetDirectory(NULL);
    fHTheta.UseCurrentStyle();
    fHTheta.SetMinimum(0);

    // effective on time versus time
    fHTime.SetName("ExcessTime");
    fHTime.SetTitle(" N_{exc} vs. Time ");
    fHTime.SetXTitle("Time");
    fHTime.SetYTitle("N_{exc} [s]");
    fHTime.UseCurrentStyle();
    fHTime.SetDirectory(NULL);
    fHTime.GetYaxis()->SetTitleOffset(1.2);
    fHTime.GetXaxis()->SetLabelSize(0.033);
    fHTime.GetXaxis()->SetTimeFormat("%H:%M:%S %F1995-01-01 00:00:00 GMT");
    fHTime.GetXaxis()->SetTimeDisplay(1);
    fHTime.SetMinimum(0);
    fHTime.Sumw2();

    MBinning binsa, binse, binst;
    binsa.SetEdges(18, 0, 90);
    binse.SetEdgesLog(15, 10, 100000);
    binst.SetEdgesASin(67, -0.005, 0.665);
    binse.Apply(fHEnergy);
    binst.Apply(fHTheta);
    binsa.Apply(fHistTime);

    MH::SetBinning(fHist, binst, binse, binsa);
}

Float_t MHAlpha::FitEnergyBins(Bool_t paint)
{
    // Do not store the 'final' result in fFit
    MAlphaFitter fit(fFit);

    const Int_t n = fHist.GetNbinsY();

    fHEnergy.SetEntries(0);

    Float_t mean = 0;
    Int_t   num  = 0;
    for (int i=1; i<=n; i++)
    {
        if (!fit.FitEnergy(fHist, fOffData, i))
            continue;

        // FIXME: Calculate UL!
        if (fit.GetEventsExcess()<=0)
            continue;

        fHEnergy.SetBinContent(i, fit.GetEventsExcess());
        fHEnergy.SetBinError(i, fit.GetErrorExcess());

        mean += fit.GetEventsExcess()*fit.GetEventsExcess()/fit.GetErrorExcess()/fit.GetErrorExcess();
        num++;
    }
    return TMath::Sqrt(mean)/num;
}

void MHAlpha::FitThetaBins(Bool_t paint)
{
    // Do not store the 'final' result in fFit
    MAlphaFitter fit(fFit);

    const Int_t n = fHist.GetNbinsX();

    fHTheta.SetEntries(0);

    for (int i=1; i<=n; i++)
    {
        if (!fit.FitTheta(fHist, fOffData, i))
            continue;

        // FIXME: Calculate UL!
        if (fit.GetEventsExcess()<=0)
            continue;

        fHTheta.SetBinContent(i, fit.GetEventsExcess());
        fHTheta.SetBinError(i, fit.GetErrorExcess());
    }
}

// --------------------------------------------------------------------------
//
// Return the value from fParemeter which should be filled into the plots
//
Double_t MHAlpha::GetVal() const
{
    return static_cast<const MHillasSrc*>(fParameter)->GetAlpha();
}


// --------------------------------------------------------------------------
//
// Store the pointer to the parameter container storing the plotted value
// (here MHillasSrc) in fParameter.
//
// return whether it was found or not.
//
Bool_t MHAlpha::GetParameter(const MParList &pl)
{
    fParameter = (MParContainer*)pl.FindObject(fNameParameter, "MHillasSrc");
    if (fParameter)
        return kTRUE;

    *fLog << err << fNameParameter << " [MHillasSrc] not found... abort." << endl;
    return kFALSE;
}

Bool_t MHAlpha::SetupFill(const MParList *pl)
{
    fHist.Reset();
    fHEnergy.Reset();
    fHTheta.Reset();
    fHTime.Reset();

    const TString off(MString::Format("%sOff", fName.Data()));
    if (fName!=off && fOffData==NULL)
    {
        const TString desc(MString::Format("%s [%s] found... using ", off.Data(), ClassName()));
        MHAlpha *hoff = (MHAlpha*)pl->FindObject(off, ClassName());
        if (!hoff)
            *fLog << inf << "No " << desc << "current data only!" << endl;
        else
        {
            *fLog << inf << desc << "on-off mode!" << endl;
            SetOffData(*hoff);
        }
    }

    if (!GetParameter(*pl))
        return kFALSE;

    fHillas = 0;
    fEnergy = fForceUsingSize ? 0 : (MParameterD*)pl->FindObject("MEnergyEst", "MParameterD");
    if (!fEnergy)
    {
        *fLog << warn << "MEnergyEst [MParameterD] not found... " << flush;

        if (!fHillas)
            fHillas = (MHillas*)pl->FindObject("MHillas");
        if (fHillas)
            *fLog << "using SIZE instead!" << endl;
        else
            *fLog << "ignored." << endl;

        fHEnergy.SetTitle(" N_{exc} vs. Size ");
        fHEnergy.SetXTitle("Size [phe]");
        fHist.SetYTitle("Size [phe]");
    }
    else
    {
        fHEnergy.SetTitle(" N_{exc} vs. E_{est} ");
        fHEnergy.SetXTitle("E_{est} [GeV]");
        fHist.SetYTitle("E_{est} [GeV]");
    }

    fPointPos = (MPointingPos*)pl->FindObject("MPointingPos");
    if (!fPointPos)
        *fLog << warn << "MPointingPos not found... ignored." << endl;

    fTimeEffOn = (MTime*)pl->FindObject("MTimeEffectiveOnTime", "MTime");
    if (!fTimeEffOn)
        *fLog << warn << "MTimeEffectiveOnTime [MTime] not found... ignored." << endl;
    else
        *fTimeEffOn = MTime(); // FIXME: How to do it different?

    fTime = (MTime*)pl->FindObject("MTime");
    if (!fTime)
        *fLog << warn << "MTime not found... ignored." << endl;

    fResult = (MParameterD*)const_cast<MParList*>(pl)->FindCreateObj("MParameterD", "MinimizationValue");
    if (!fResult)
        return kFALSE;
    fSigma = (MParameterD*)const_cast<MParList*>(pl)->FindCreateObj("MParameterD", "GaussSigma");
    if (!fSigma)
        return kFALSE;
    fBin = (MParameterI*)const_cast<MParList*>(pl)->FindCreateObj("MParameterI", "Bin");
    if (!fBin)
        return kFALSE;

    //fExcess = (MParameterD*)const_cast<MParList*>(pl)->FindCreateObj("MParameterD", "MExcess");
    //if (!fExcess)
    //    return kFALSE;

    fLastTime = MTime();
    fNumRebin = fNumTimeBins;

    MBinning binst, binse, binsa;
    binst.SetEdges(fOffData ? *fOffData : fHist, 'x');
    binse.SetEdges(fOffData ? *fOffData : fHist, 'y');
    binsa.SetEdges(fOffData ? *fOffData : fHist, 'z');
    if (!fOffData)
    {
        if (!fPointPos)
            binst.SetEdges(1, 0, 60);
        else
            binst.SetEdges(*pl, "BinningTheta");

        if (!fEnergy && !fHillas)
            binse.SetEdges(1, 10, 100000);
        else
            if (fEnergy)
                binse.SetEdges(*pl, "BinningEnergyEst");
            else
                binse.SetEdges(*pl, "BinningSize");

        binsa.SetEdges(*pl, MString::Format("Binning%s", ClassName()+2));
    }
    else
    {
        fHEnergy.SetTitle(fOffData->GetTitle());
        fHEnergy.SetXTitle(fOffData->GetYaxis()->GetTitle());
        fHist.SetYTitle(fOffData->GetYaxis()->GetTitle());
    }

    binse.Apply(fHEnergy);
    binst.Apply(fHTheta);
    binsa.Apply(fHistTime);
    MH::SetBinning(fHist, binst, binse, binsa);

    MAlphaFitter *fit = (MAlphaFitter*)pl->FindObject("MAlphaFitter");
    if (!fit)
        *fLog << warn << "MAlphaFitter not found... ignored." << endl;
    else
        fit->Copy(fFit);

    *fLog << inf;
    fFit.Print();

    return kTRUE;
}

void MHAlpha::InitAlphaTime(const MTime &t)
{
    //
    // If this is the first call we have to initialize the time-histogram
    //
    const MBinning bins(1, t.GetAxisTime()-60, t.GetAxisTime());
    bins.Apply(fHTime);

    fLastTime=t;
}

void MHAlpha::UpdateAlphaTime(Bool_t final)
{
    if (!fTimeEffOn)
        return;

    if (!final)
    {
        if (fLastTime==MTime() && *fTimeEffOn!=MTime())
        {
            InitAlphaTime(*fTimeEffOn);
            return;
        }

        if (fLastTime!=*fTimeEffOn)
        {
            fLastTime=*fTimeEffOn;
            fNumRebin--;
        }

        if (fNumRebin>0)
            return;
    }

    // Check new 'last time'
    MTime *time = final ? fTime : fTimeEffOn;

    if (time->GetAxisTime()<=fHTime.GetXaxis()->GetXmax())
    {
        *fLog << warn << "WARNING - New time-stamp " << *time << " lower" << endl;
        *fLog << "than upper edge of histogram... skipped." << endl;
        *fLog << "This should not happen. Maybe you started you eventloop with" << endl;
        *fLog << "an already initialized time stamp MTimeEffectiveOnTime?" << endl;
        fNumRebin++;
        return;
    }

    // Fit time histogram
    MAlphaFitter fit(fFit);

    TH1D *h = fOffData ? fOffData->ProjectionZ("ProjTimeTemp", 0, fOffData->GetNbinsX()+1, 0, fOffData->GetNbinsY()+1, "E") : 0;
    const Bool_t rc = fit.ScaleAndFit(fHistTime, h);

    if (h)
        delete h;

    if (!rc)
        return;

    // Reset Histogram
    fHistTime.Reset();
    fHistTime.SetEntries(0);

    //
    // Prepare Histogram
    //
    if (final)
        time->Plus1ns();

    // Enhance binning
    MBinning bins;
    bins.SetEdges(fHTime, 'x');
    bins.AddEdge(time->GetAxisTime());
    bins.Apply(fHTime);

    //
    // Fill histogram
    //
    // Get number of bins
    const Int_t n = fHTime.GetNbinsX();

    fHTime.SetBinContent(n, fit.GetEventsExcess());
    fHTime.SetBinError(n,   fit.GetErrorExcess());

    //*fLog << inf3 << *fTimeEffOn << " (" << n << "): " << fit.GetEventsExcess() << endl;

    fNumRebin = fNumTimeBins;
}

void MHAlpha::SetBin(Int_t ibin)
{
    // Is this necessary?
    // Could be speed up up searching for it only once.
    const Float_t max     = fFit.GetSignalIntegralMax();
    const Int_t bin0      = fHist.GetZaxis()->FindFixBin(max);

    const Int_t nbinsx    = fHist.GetNbinsX();
    const Int_t nbinsy    = fHist.GetNbinsY();
    const Int_t nxy       = (nbinsx+2)*(nbinsy+2);

    const Int_t binz      = ibin/nxy;

    const Bool_t issignal = binz>0 && binz<bin0;

    fBin->SetVal(issignal ? binz : -binz);
}

// --------------------------------------------------------------------------
//
// Fill the histogram. For details see the code or the class description
// 
Int_t MHAlpha::Fill(const MParContainer *par, const Stat_t w)
{
    Double_t alpha, energy, theta;
    Double_t size=-1;

    if (fMatrix)
    {
        alpha  = fMap[0]<0 ? GetVal() : (*fMatrix)[fMap[0]];
        energy = fMap[1]<0 ? -1 : (*fMatrix)[fMap[1]];
        size   = fMap[2]<0 ? -1 : (*fMatrix)[fMap[2]];
        //<0 ? 1000 : (*fMatrix)[fMap[1]];
        theta  = 0;

        if (energy<0)
            energy=size;
        if (size<0)
            size=energy;

        if (energy<0 && size<0)
            energy = size = 1000;
    }
    else
    {
        alpha  = GetVal();

        if (fHillas)
            size = fHillas->GetSize();
        energy = fEnergy   ? fEnergy->GetVal() : (fHillas?fHillas->GetSize():1000);
        theta  = fPointPos ? fPointPos->GetZd() : 0;
    }

    // enhance histogram if necessary
    UpdateAlphaTime();

    // Fill histograms
    const Int_t ibin = fHist.Fill(theta, energy, TMath::Abs(alpha), w);
    SetBin(ibin);

    if (!fSkipHistTime)
        fHistTime.Fill(TMath::Abs(alpha), w);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Paint the integral and the error on top of the histogram
//
void MHAlpha::PaintText(Double_t val, Double_t error) const
{
    TLatex text(0.45, 0.94, MString::Format("N_{exc} = %.1f \\pm %.1f", val, error));
    text.SetBit(TLatex::kTextNDC);
    text.SetTextSize(0.04);
    text.Paint();
}

// --------------------------------------------------------------------------
//
//  Paint the integral and the error on top of the histogram
//
void MHAlpha::PaintText(const TH1D &h) const
{
    Double_t sumv = 0;
    Double_t sume = 0;

    for (int i=0; i<h.GetNbinsX(); i++)
    {
        sumv += h.GetBinContent(i+1);
        sume += h.GetBinError(i+1);
    }
    PaintText(sumv, sume);
}
// --------------------------------------------------------------------------
//
// Update the projections
//
void MHAlpha::Paint(Option_t *opt)
{
    // Note: Any object cannot be added to a pad twice!
    //       The object is searched by gROOT->FindObject only in
    //       gPad itself!
    TVirtualPad *padsave = gPad;

    TH1D *h0=0;

    TString o(opt);

    if (o==(TString)"proj")
    {
        TPaveStats *st=0;
        for (int x=0; x<4; x++)
        {
            TVirtualPad *p=padsave->GetPad(x+1);
            if (!p || !(st = (TPaveStats*)p->GetPrimitive("stats")))
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

        padsave->cd(1);

        TH1D *hon = (TH1D*)gPad->FindObject("Proj");
        if (hon)
        {
            TH1D *dum = fHist.ProjectionZ("dumab", 0, fHist.GetNbinsX()+1, 0, fHist.GetNbinsY()+1);
            dum->SetDirectory(0);
            hon->Reset();
            hon->Add(dum);
            delete dum;

            if (fOffData)
            {
                TH1D *hoff = (TH1D*)gPad->FindObject("ProjOff");
                if (hoff)
                {
                    TH1D *dum = fOffData->ProjectionZ("dumxy", 0, fOffData->GetNbinsX()+1, 0, fOffData->GetNbinsY()+1);
                    dum->SetDirectory(0);
                    hoff->Reset();
                    hoff->Add(dum);
                    delete dum;

                    const Double_t alpha = fFit.Scale(*hoff, *hon);

                    hon->SetMaximum();
                    hon->SetMaximum(TMath::Max(hon->GetMaximum(), hoff->GetMaximum())*1.05);

                    // BE CARFEULL: This is a really weird workaround!
                    hoff->SetMaximum(alpha);

                    // For some reason the line-color is resetted
                    hoff->SetLineColor(kRed);

                    if ((h0=(TH1D*)gPad->FindObject("ProjOnOff")))
                    {
                        h0->Reset();
                        h0->Add(hoff, hon, -1);
                        const Float_t min = h0->GetMinimum()*1.05;
                        hon->SetMinimum(min<0 ? min : 0);
                    }

                }
            }
            else
                hon->SetMinimum(0);
        }
        FitEnergyBins();
        FitThetaBins();
    }

    if (o==(TString)"variable")
        if ((h0 = (TH1D*)gPad->FindObject("Proj")))
        {
            // Check whether we are dealing with on-off analysis
            TH1D *hoff = (TH1D*)gPad->FindObject("ProjOff");

            // BE CARFEULL: This is a really weird workaround!
            const Double_t scale = !hoff || hoff->GetMaximum()<0 ? 1 : hoff->GetMaximum();

            // Do not store the 'final' result in fFit
            MAlphaFitter fit(fFit);
            fit.Fit(*h0, hoff, scale, kTRUE);
            fit.PaintResult();
        }

    if (o==(TString)"time")
        PaintText(fHTime);

    if (o==(TString)"theta")
    {
        TH1 *h = (TH1*)gPad->FindObject(MString::Format("%s_x", fHist.GetName()));
        if (h)
        {
            TH1D *h2 = (TH1D*)fHist.Project3D("dum_x");
            h2->SetDirectory(0);
            h2->Scale(fHTheta.Integral()/h2->Integral());
            h->Reset();
            h->Add(h2);
            delete h2;
        }
        PaintText(fHTheta);
    }

    if (o==(TString)"energy")
    {
        TH1 *h = (TH1*)gPad->FindObject(MString::Format("%s_y", fHist.GetName()));
        if (h)
        {
            TH1D *h2 = (TH1D*)fHist.Project3D("dum_y");
            h2->SetDirectory(0);
            h2->Scale(fHEnergy.Integral()/h2->Integral());
            h->Reset();
            h->Add(h2);
            delete h2;
        }
        PaintText(fHEnergy);

        if (fHEnergy.GetMaximum()>1)
        {
            gPad->SetLogx();
            gPad->SetLogy();
        }
    }

    gPad = padsave;
}

// --------------------------------------------------------------------------
//
// Draw the histogram
//
void MHAlpha::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);

    /*
    if (TString(opt).Contains("sizebins", TString::kIgnoreCase))
    {
        AppendPad("sizebins");
        return;
    }
    */

    // Do the projection before painting the histograms into
    // the individual pads
    AppendPad("proj");

    pad->SetBorderMode(0);
    pad->Divide(2,2);

    TH1D *h=0;

    pad->cd(1);
    gPad->SetBorderMode(0);

    h = fHist.ProjectionZ("Proj", 0, fHist.GetNbinsX()+1, 0, fHist.GetNbinsY()+1, "E");
    h->SetBit(TH1::kNoTitle);
    h->SetStats(kTRUE);
    h->SetXTitle(fHist.GetZaxis()->GetTitle());
    h->SetYTitle("Counts");
    h->SetDirectory(NULL);
    h->SetMarkerStyle(0);
    h->SetBit(kCanDelete);
    h->Draw("");

    if (fOffData)
    {
        // To get green on-data
        //h->SetMarkerColor(kGreen);
        //h->SetLineColor(kGreen);

        h = fOffData->ProjectionZ("ProjOff", 0, fOffData->GetNbinsX()+1, 0, fOffData->GetNbinsY()+1, "E");
        h->SetBit(TH1::kNoTitle);
        h->SetXTitle(fHist.GetZaxis()->GetTitle());
        h->SetYTitle("Counts");
        h->SetDirectory(NULL);
        h->SetMarkerStyle(0);
        h->SetBit(kCanDelete);
        h->SetMarkerColor(kRed);
        h->SetLineColor(kRed);
        //h->SetFillColor(18);
        h->Draw("same"/*"bar same"*/);

        // This is the only way to make it work...
        // Clone and copy constructor give strange crashes :(
        h = fOffData->ProjectionZ("ProjOnOff", 0, fOffData->GetNbinsX()+1, 0, fOffData->GetNbinsY()+1, "E");
        h->SetBit(TH1::kNoTitle);
        h->SetXTitle(fHist.GetZaxis()->GetTitle());
        h->SetYTitle("Counts");
        h->SetDirectory(NULL);
        h->SetMarkerStyle(0);
        h->SetBit(kCanDelete);
        h->SetMarkerColor(kBlue);
        h->SetLineColor(kBlue);
        h->Draw("same");

        TLine lin;
        lin.SetLineStyle(kDashed);
        lin.DrawLine(h->GetXaxis()->GetXmin(), 0, h->GetXaxis()->GetXmax(), 0);
    }

    // After the Alpha-projection has been drawn. Fit the histogram
    // and paint the result into this pad
    AppendPad("variable");

    if (fHEnergy.GetNbinsX()>1 || fHEnergy.GetBinContent(1)>0)
    {
        pad->cd(2);
        gPad->SetBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        fHEnergy.Draw();

        AppendPad("energy");

        h = (TH1D*)fHist.Project3D("y");
        h->SetBit(TH1::kNoTitle|TH1::kNoStats);
        h->SetXTitle("E [GeV]");
        h->SetYTitle("Counts");
        h->SetDirectory(NULL);
        h->SetMarkerStyle(kFullDotSmall);
        h->SetBit(kCanDelete);
        h->SetMarkerColor(kCyan);
        h->SetLineColor(kCyan);
        h->Draw("Psame");
    }
    else
        delete pad->GetPad(2);

    if ((fTimeEffOn && fTime) || fHTime.GetNbinsX()>1 || fHTime.GetBinError(1)>0)
    {
        pad->cd(3);
        gPad->SetBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        fHTime.Draw();
        AppendPad("time");
    }
    else
        delete pad->GetPad(3);

    if (fHTheta.GetNbinsX()>1 || fHTheta.GetBinContent(1)>0)
    {
        pad->cd(4);
        gPad->SetGridx();
        gPad->SetGridy();
        gPad->SetBorderMode(0);
        fHTheta.Draw();

        AppendPad("theta");

        h = (TH1D*)fHist.Project3D("x");
        h->SetBit(TH1::kNoTitle|TH1::kNoStats);
        h->SetXTitle("\\theta [\\circ]");
        h->SetYTitle("Counts");
        h->SetDirectory(NULL);
        h->SetMarkerStyle(kFullDotSmall);
        h->SetBit(kCanDelete);
        h->SetMarkerColor(kCyan);
        h->SetLineColor(kCyan);
        h->Draw("Psame");
    }
    else
        delete pad->GetPad(4);
}

void MHAlpha::DrawAll(Bool_t newc)
{
    if (!newc && !fDisplay)
        return;

    // FIXME: Do in Paint if special option given!
    TCanvas &c = newc ? *new TCanvas : fDisplay->AddTab("SizeBins");
    Int_t n = fHist.GetNbinsY();
    Int_t nc = (Int_t)(TMath::Sqrt((Float_t)n-1)+1);
    c.Divide(nc, nc, 1e-10, 1e-10);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);

    // Do not store the 'final' result in fFit
    MAlphaFitter fit(fFit);

    for (int i=1; i<=fHist.GetNbinsY(); i++)
    {
        c.cd(i);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);

        TH1D *hon = fHist.ProjectionZ("Proj", 0, fHist.GetNbinsX()+1, i, i, "E");
        hon->SetBit(TH1::kNoTitle);
        hon->SetStats(kTRUE);
        hon->SetXTitle(fHist.GetZaxis()->GetTitle());
        hon->SetYTitle("Counts");
        hon->SetDirectory(NULL);
        hon->SetMarkerStyle(0);
        hon->SetBit(kCanDelete);
        hon->Draw("");

        TH1D *hof = 0;
        Double_t alpha = 1;

        if (fOffData)
        {
            hon->SetMarkerColor(kGreen);

            hof = fOffData->ProjectionZ("ProjOff", 0, fOffData->GetNbinsX()+1, i, i, "E");
            hof->SetBit(TH1::kNoTitle|TH1::kNoStats);
            hof->SetXTitle(fHist.GetZaxis()->GetTitle());
            hof->SetYTitle("Counts");
            hof->SetDirectory(NULL);
            hof->SetMarkerStyle(0);
            hof->SetBit(kCanDelete);
            hof->SetMarkerColor(kRed);
            hof->SetLineColor(kRed);
            hof->Draw("same");

            alpha = fit.Scale(*hof, *hon);

            hon->SetMaximum();
            hon->SetMaximum(TMath::Max(hon->GetMaximum(), hof->GetMaximum())*1.05);

            TH1D *diff = new TH1D(*hon);
            diff->Add(hof, -1);
            diff->SetBit(TH1::kNoTitle);
            diff->SetXTitle(fHist.GetZaxis()->GetTitle());
            diff->SetYTitle("Counts");
            diff->SetDirectory(NULL);
            diff->SetMarkerStyle(0);
            diff->SetBit(kCanDelete);
            diff->SetMarkerColor(kBlue);
            diff->SetLineColor(kBlue);
            diff->Draw("same");

            TLine lin;
            lin.SetLineStyle(kDashed);
            lin.DrawLine(diff->GetXaxis()->GetXmin(), 0, diff->GetXaxis()->GetXmax(), 0);

            const Float_t min = diff->GetMinimum()*1.05;
            hon->SetMinimum(min<0 ? min : 0);
        }

        if (hof ? fit.Fit(*hon, *hof, alpha) : fit.Fit(*hon))
        {
            *fLog << dbg << "Bin " << i << ": sigmaexc=" << fit.GetEventsExcess()/fit.GetErrorExcess() << " omega=" << fit.GetGausSigma() << " events=" << fit.GetEventsExcess() << " scale=" << fit.GetScaleFactor() << endl;
            fit.DrawResult();
        }
        /*
        if (fit.FitEnergy(fHist, fOffData, i, kTRUE))
        {
            fHEnergy.SetBinContent(i, fit.GetEventsExcess());
            fHEnergy.SetBinError(i, fit.GetEventsExcess()*0.2);
        }*/
    }

}

void MHAlpha::DrawNicePlot(Bool_t newc, const char *title, const char *watermark, Int_t binlo, Int_t binhi)
{
    if (!newc && !fDisplay)
        return;

    if (!fOffData)
        return;

    // Open and setup canvas/pad
    TCanvas &c = newc ? *new TCanvas : fDisplay->AddTab("ThetsSq");

    c.SetBorderMode(0);
    c.SetFrameBorderMode(0);
    c.SetFillColor(kWhite);

    c.SetLeftMargin(0.12);
    c.SetRightMargin(0.01);
    c.SetBottomMargin(0.16);
    c.SetTopMargin(0.18);

    c.SetGridy();

    gStyle->SetOptStat(0);

    // Get on-data
    TH1D *hon = (TH1D*)fHist.ProjectionZ("Proj", 0, fHist.GetNbinsX()+1, binlo, binhi, "E");
    hon->SetDirectory(NULL);
    hon->SetBit(kCanDelete);
    hon->SetMarkerSize(0);
    hon->SetLineWidth(2);
    hon->SetLineColor(kBlack);
    hon->SetMarkerColor(kBlack);

    // Get off-data
    TH1D *hoff = 0;
    if (fOffData)
    {
        hoff = (TH1D*)fOffData->ProjectionZ("ProjOff", 0, fHist.GetNbinsX()+1, binlo, binhi, "E");
        hoff->SetDirectory(NULL);
        hoff->SetBit(kCanDelete);
        hoff->SetFillColor(17);
        hoff->SetMarkerSize(0);
        hoff->SetLineColor(kBlack);
        hoff->SetMarkerColor(kBlack);
    }

    // Setup plot which is drawn first
    TH1D *h = hoff ? hoff : hon;
    h->GetXaxis()->SetLabelSize(0.06);
    h->GetXaxis()->SetTitleSize(0.06);
    h->GetXaxis()->SetTitleOffset(0.95);
    h->GetYaxis()->SetLabelSize(0.06);
    h->GetYaxis()->SetTitleSize(0.06);
    h->GetYaxis()->CenterTitle();
    h->SetYTitle("Counts");
    h->SetTitleSize(0.07);
    h->SetTitle("");

    const Double_t imax = fFit.GetSignalIntegralMax();
//    cout << "-----> IMAX " << imax << endl;
//    cout << " bins: " << binlo << " " << binhi << endl;
//    cout << " max: " << hon->GetMaximum() << " " << hoff->GetMaximum() << endl;
//    if (imax<1)
//        h->GetXaxis()->SetRangeUser(0, 0.6*0.6);

    //to avoid that '0.3' is not displayed properly
    h->GetXaxis()->SetRangeUser(0, 0.29);

    // scale off-data

    MAlphaFitter fit(fFit);
    fit.ScaleAndFit(*hon, hoff);

//    cout << " max: " << hon->GetMaximum() << " " << hoff->GetMaximum() << endl;
    hon->SetMinimum(0);
    hoff->SetMinimum(0);

    // draw data
    if (hoff)
    {
        hoff->SetMaximum(TMath::Max(hon->GetMaximum(),hoff->GetMaximum())*1.1);
//        cout << "setting max to " << TMath::Max(hon->GetMaximum(),hoff->GetMaximum())*1.1 << endl;
        hoff->Draw("bar");
        hon->Draw("same");
    }
    else
    {
        hon->SetMaximum();
        hon->Draw();
    }

    // draw a preliminary tag
    TLatex text;
    text.SetTextColor(kWhite);
    text.SetBit(TLatex::kTextNDC);
    text.SetTextSize(0.07);
    text.SetTextAngle(2.5);

    TString wm(watermark);
    if (binlo>=1 || binhi<hon->GetNbinsX())
    {
        wm += wm.IsNull() ? "(" : " (";
        if (binlo>=1)
            wm += MString::Format("%.1fGeV", fHist.GetYaxis()->GetBinLowEdge(binlo));
        wm += "-";
        if (binhi<hon->GetNbinsX())
            wm += MString::Format("%.1fGeV", fHist.GetYaxis()->GetBinLowEdge(binhi+1));
        wm += ")";
    }
    if (!wm.IsNull())
        text.DrawLatex(0.45, 0.2, wm);
    //enum { kTextNDC = BIT(14) };

    // draw line showing cut
    TLine line;
    line.SetLineColor(14);
    line.SetLineStyle(7);
    line.DrawLine(imax, 0, imax, h->GetMaximum());

    // Add a title above the plot
    TPaveText *pave=new TPaveText(0.12, 0.83, 0.99, 0.975, "blNDC");
    pave->SetBorderSize(1);
    pave->SetLabel(title);

    TText *ptxt = pave->AddText(" ");
    ptxt->SetTextAlign(20);

    ptxt = pave->AddText(MString::Format("Significance  %.1f#sigma,  off-scale  %.2f",
                                         fit.GetSignificance(), fit.GetScaleFactor()));
//    ptxt = pave->AddText(MString::Format("Significance  %.1f,  off-scale  %.2f",
//                                         fit.GetSignificance(), fit.GetScaleFactor()));
    ptxt->SetTextAlign(21);
    //ptxt->SetTextFont(4);
    //ptxt->SetTextSize(0.2);

    ptxt = pave->AddText(MString::Format("%.1f excess events,  %.1f background events",
                                         fit.GetEventsExcess(), fit.GetEventsBackground()));

    ptxt->SetTextAlign(21);
    //ptxt->SetTextFont(4);
    //ptxt->SetTextSize(20);

    pave->SetBit(kCanDelete);
    pave->Draw();
}

Bool_t MHAlpha::Finalize()
{
    if (!FitAlpha())
    {
        *fLog << warn << "MAlphaFitter - Fit failed..." << endl;
        return kTRUE;
    }

    // Store the final result in fFit
    *fLog << all;
    fFit.Print("result");

    fResult->SetVal(fFit.GetMinimizationValue());
    fSigma->SetVal(fFit.GetGausSigma());

    if (!fSkipHistEnergy)
    {
        *fLog << inf3 << "Processing energy bins..." << endl;
        FitEnergyBins();
    }
    if (!fSkipHistTheta)
    {
        *fLog << inf3 << "Processing theta bins..." << endl;
        FitThetaBins();
    }
    if (!fSkipHistTime)
    {
        *fLog << inf3 << "Processing time bins..." << endl;
        UpdateAlphaTime(kTRUE);
        MH::RemoveFirstBin(fHTime);
    }

    if (fOffData)
        DrawAll(kFALSE);

    return kTRUE;
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
// It takes fSkipHist* into account!
//
void MHAlpha::InitMapping(MHMatrix *mat, Int_t type)
{
    if (fMatrix)
        return;

    fMatrix = mat;

    fMap[0] = fMatrix->AddColumn(GetParameterRule());
    fMap[1] = -1;
    fMap[2] = -1;
    fMap[3] = -1;
    fMap[4] = -1;

    if (!fSkipHistEnergy)
    {
        fMap[1] = type==0 ? fMatrix->AddColumn("MEnergyEst.fVal") : -1;
        fMap[2] = type==0 ? -1 : fMatrix->AddColumn("MHillas.fSize");
    }

    if (!fSkipHistTheta)
        fMap[3] = fMatrix->AddColumn("MPointingPos.fZd");

   // if (!fSkipHistTime)
   //     fMap[4] = fMatrix->AddColumn("MTime.GetAxisTime");
}

void MHAlpha::StopMapping()
{
    fMatrix = NULL; 
}

void MHAlpha::ApplyScaling()
{
    if (!fOffData)
        return;

    fFit.ApplyScaling(fHist, *const_cast<TH3D*>(fOffData));
}

Int_t MHAlpha::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "NumTimeBins", print))
    {
        SetNumTimeBins(GetEnvValue(env, prefix, "NumTimeBins", fNumTimeBins));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "ForceUsingSize", print))
    {
        fForceUsingSize = GetEnvValue(env, prefix, "ForceUsingSize", fForceUsingSize);
        rc = kTRUE;
    }
    return rc;
}

Int_t MHAlpha::DistancetoPrimitive(Int_t px, Int_t py)
{
    // If pad has no subpad return (we are in one of the subpads)
    if (gPad->GetPad(1)==NULL)
        return 9999;

    // If pad has a subpad we are in the main pad. Check its value.
    return gPad->GetPad(1)->DistancetoPrimitive(px,py)==0 ? 0 : 9999;
}

void MHAlpha::RecursiveRemove(TObject *obj)
{
    if (obj==fOffData)
        fOffData = 0;

    MH::RecursiveRemove(obj);
}
