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
// MHRate
//
// Display information about software trigger rate
//
////////////////////////////////////////////////////////////////////////////
#include "MHRate.h"

#include <TCanvas.h>
#include <TGaxis.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MTime.h"
#include "MEventRate.h"
#include "MPointingPos.h"
#include "MBinning.h"

ClassImp(MHRate);

using namespace std;

void MHRate::ResetGraph(TGraph &g) const
{
    g.Set(1);
    g.SetPoint(0, 0, 0); // Dummy Point
    g.SetEditable();     // Used as flag: First point? yes/no
}

void MHRate::InitGraph(TGraph &g) const
{
    ResetGraph(g);
    g.SetMarkerStyle(kFullDotMedium);
}

void MHRate::AddPoint(TGraph &g, Double_t x, Double_t y) const
{
    if (g.IsEditable())
    {
        g.RemovePoint(0);
        g.SetEditable(kFALSE);
    }

    g.SetPoint(g.GetN(), x, y);
}

// --------------------------------------------------------------------------
//
// Setup histograms 
//
MHRate::MHRate(const char *name, const char *title)
: /*fTime(0),*/ fPointPos(0), fEvtRate(0)
{
    fName  = name  ? name  : "MHRate";
    fTitle = title ? title : "Graphs for rate data";

    // Init Graphs
    fRateTime.SetNameTitle("RateTime", "Rate vs Time");
    fRateZd.SetNameTitle("RateZd", "Rate vs Zenith distance");
    fRate.SetNameTitle("Rate", "Distribution of Rate");

    InitGraph(fRateTime);

    fRateZd.SetXTitle("Zd [\\circ]");
    fRateZd.SetYTitle("Rate [Hz]");

    fRate.SetXTitle("Rate [Hz]");
    fRate.SetYTitle("Counts");

    fRate.SetDirectory(0);
    fRateZd.SetDirectory(0);

    fRateZd.SetMarkerStyle(kFullDotMedium);
    fRateZd.SetBit(TH1::kNoStats);

    MBinning b;
    b.SetEdges(120, 0, 1200);
    b.Apply(fRate);

    b.SetEdgesASin(67, -0.005, 0.665);
    b.Apply(fRateZd);
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning
//
Bool_t MHRate::SetupFill(const MParList *plist)
{
    fEvtRate = (MEventRate*)plist->FindObject("MEventRate");
    if (!fEvtRate)
    {
        *fLog << err << "MEventRate not found... abort." << endl;
        return kFALSE;
    }
    fPointPos = (MPointingPos*)plist->FindObject("MPointingPos");
    if (!fPointPos)
    {
        *fLog << err << "MPointingPos not found... abort." << endl;
        return kFALSE;
    }

    // Reset Graphs
    ResetGraph(fRateTime);

    fCounter = 0;
    fMean    = 0;
    fLast    = MTime();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MMuonCalibPar and
// MMuonSearchPar container.
//
Int_t MHRate::Fill(const MParContainer *par, const Stat_t w)
{
    const MTime *t = dynamic_cast<const MTime*>(par);
    if (!t)
    {
        *fLog << err <<"MHRate::Fill - ERROR: MTime not given as argument... abort." << endl;
        return kERROR;
    }

    const Double_t rate = fEvtRate->GetRate();
    if (rate<0)
        return kTRUE;

    const Double_t tm   = t->GetAxisTime();

    if (fLast==MTime())
        fLast = MTime();

    fMean += rate;
    fCounter++;

    if ((double)*t > (double)fLast+5 && fCounter>0)
    {
        AddPoint(fRateTime, tm, fMean/fCounter);
        fMean     = 0;
        fCounter = 0;
        fLast = *t;
    }

    fRateZd.Fill(fPointPos->GetZd(), rate);
    fRate.Fill(rate);

    return kTRUE;
}

void MHRate::DrawGraph(TGraph &g, const char *y) const
{
    // If this is set to early the plot remains empty in root 5.12/00
    if (g.GetN()>0)
        g.SetMinimum(0);

    // This is not done automatically anymore since root 5.12/00
    // and it is necessary to force a proper update of the axis.
    TH1 *h = g.GetHistogram();
    if (h)
    {
        delete h;
        g.SetHistogram(0);
        h = g.GetHistogram();
    }

    if (h)
    {
        TAxis *axe = h->GetXaxis();
        axe->SetLabelSize(0.033);
        axe->SetTimeFormat("%H:%M:%S %F1995-01-01 00:00:00 GMT");
        axe->SetTimeDisplay(1);
        axe->SetTitle("Time");
        if (y)
            h->SetYTitle(y);
    }
}

// --------------------------------------------------------------------------
//
// Update position of an axis on the right side of the histogram
/*
void MHRate::UpdateRightAxis(TGraph &g) const
{
    TH1 &h = *g.GetHistogram();

    const Double_t max = h.GetMaximum();
    if (max==0)
        return;

    TGaxis *axis = (TGaxis*)gPad->FindObject("RightAxis");
    if (!axis)
        return;

    axis->SetX1(g.GetXaxis()->GetXmax());
    axis->SetX2(g.GetXaxis()->GetXmax());
    axis->SetY1(gPad->GetUymin());
    axis->SetY2(gPad->GetUymax());
    axis->SetWmax(max);
}
*/
// --------------------------------------------------------------------------
//
// Draw an axis on the right side of the histogram
//
/*
void MHRate::DrawRightAxis(const char *title) const
{
    TGaxis *axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(),
                              gPad->GetUxmax(), gPad->GetUymax(),
                              0, 1, 510, "+L");
    axis->SetName("RightAxis");
    axis->SetTitle(title);
    axis->SetTitleOffset(0.9);
    axis->SetTextColor(kRed);
    axis->SetBit(kCanDelete);
    axis->Draw();
}
*/
// --------------------------------------------------------------------------
//
// This displays the TGraph like you expect it to be (eg. time on the axis)
// It should also make sure, that the displayed time always is UTC and
// not local time...
//
void MHRate::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad();

    pad->Divide(2,2);

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    gPad->SetLogy();
    fRate.Draw();
    //fHumidity.Draw("AP");
    //fTemperature.Draw("P");
    //DrawRightAxis("T [\\circ C]");

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fRateTime.Draw("AP");
    //fSolarRadiation.Draw("AP");

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fRateZd.Draw();

    if (pad->GetPad(4))
        delete pad->GetPad(4);

    /*
    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fWindSpeed.Draw("AP");*/
}

void MHRate::Paint(Option_t *o)
{
    DrawGraph(fRateTime, "f [Hz]");
    /*
    gPad->cd(1);

    if (gPad)
    {
        fHumidity.GetHistogram()->GetYaxis()->SetTitleColor(kBlue);
        UpdateRightAxis(fTemperature);
    }*/
}
