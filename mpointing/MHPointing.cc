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
!   Author(s): Thomas Bretz, 05/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHPointing
//
// Display drive information
//
// Class Version 2:
// ----------------
//
//  + TGraph fNumStarsCor; // Number of correlated stars identified by starguider
//
////////////////////////////////////////////////////////////////////////////
#include "MHPointing.h"

#include <TH1.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TGaxis.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MTime.h"
#include "MAstro.h"

#include "MReportDrive.h"
#include "MReportStarguider.h"

ClassImp(MHPointing);

using namespace std;

void MHPointing::ResetGraph(TGraph &g) const
{
    g.Set(1);
    g.SetPoint(0, 0, 0); // Dummy Point
    g.SetEditable();     // Used as flag: First point? yes/no
}

void MHPointing::InitGraph(TGraph &g) const
{
    ResetGraph(g);
    g.SetMarkerStyle(kFullDotMedium);
}

void MHPointing::AddPoint(TGraph &g, Double_t x, Double_t y) const
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
MHPointing::MHPointing(const char *name, const char *title)
: /*fTime(0),*/ fReportCosy(0), fReportSG(0)
{
    fName  = name  ? name  : "MHPointing";
    fTitle = title ? title : "Graphs for rate data";

    // Init Graphs
    fDevTimeSG.SetNameTitle("DevSG",         "Absolute deviation of drive (black) and starguider (blue)");
    fDevTimeCosy.SetNameTitle("DevCosy",     "Cosy deviation");
    fBrightness.SetNameTitle("Brightness",   "Arbitrary Sky Brightness (black), No. of stars identified by starguider (blue)");
    fNumStars.SetNameTitle("NumStars",       "Number of stars identified by starguider");
    fNumStarsCor.SetNameTitle("NumStarsCor", "Number of stars correlated by starguider");
    fDevZd.SetNameTitle("DevZd",             "Starguider deviation Zd (blue), Az (black)");
    fDevAz.SetNameTitle("DevAz",             "Starguider Deviation Az");
    fPosZd.SetNameTitle("PosZd",             "Nominal position Zd");
    //fPosAz.SetNameTitle("PosZd",          "Position Az");

    InitGraph(fDevTimeSG);
    InitGraph(fDevTimeCosy);
    InitGraph(fBrightness);
    InitGraph(fNumStars);
    InitGraph(fNumStarsCor);
    InitGraph(fDevZd);
    InitGraph(fDevAz);
    InitGraph(fPosZd);
    //InitGraph(fPosAz);

    fDevTimeSG.SetMarkerColor(kBlue);
    fDevZd.SetMarkerColor(kBlue);
    fNumStars.SetMarkerColor(kBlue);
    fNumStarsCor.SetMarkerColor(kMagenta);
    //fPosAz.SetMarkerColor(kBlue);
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning
//
Bool_t MHPointing::SetupFill(const MParList *plist)
{
    //fTime = (MTime*)plist->FindObject("MTime");
    //if (!fTime)
    //{
    //    *fLog << warn << "MTime not found... abort." << endl;
    //    return kFALSE;
    //}
    fReportSG = (MReportStarguider*)plist->FindObject("MReportStarguider");
    if (!fReportSG)
        *fLog << warn << "MReportStarguider not found..." << endl;

    fReportCosy = (MReportDrive*)plist->FindObject("MReportDrive");
    if (!fReportCosy)
        *fLog << warn << "MReportDrive not found..." << endl;

    // Reset Graphs
    ResetGraph(fDevTimeSG);
    ResetGraph(fDevTimeCosy);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MMuonCalibPar and
// MMuonSearchPar container.
//
Int_t MHPointing::Fill(const MParContainer *par, const Stat_t w)
{
    const MTime *t = dynamic_cast<const MTime*>(par);
    if (!t)
    {
        *fLog << err <<"MHPointing::Fill - ERROR: MTime not given as argument... abort." << endl;
        return kERROR;
    }

    const Double_t tm = t->GetAxisTime();

    if (t->GetName()==(TString)"MTimeStarguider")
    {
        if (!fReportSG)
        {
            *fLog << err << "ERROR: fReportSG==NULL... abort." << endl;
            return kERROR;
        }

        // Check for old files
        if (fReportSG->GetSkyBrightness()>0)
        {
            AddPoint(fBrightness,  tm, fReportSG->GetSkyBrightness());
            AddPoint(fNumStars,    tm, fReportSG->GetNumIdentifiedStars());
            AddPoint(fNumStarsCor, tm, fReportSG->GetNumCorrelatedStars());
        }

        const Bool_t zdok     = TMath::Abs(fReportSG->GetDevZd())<30;
        const Bool_t azok     = TMath::Abs(fReportSG->GetDevAz())<90;

        const Double_t devzd  = fReportSG->GetDevZd();
        const Double_t devaz  = fReportSG->GetDevAz();

        if (zdok && azok)
            AddPoint(fDevTimeSG, tm, fReportSG->GetDevAbs());
        if (zdok)
            AddPoint(fDevZd, tm, devzd);
        if (azok)
            AddPoint(fDevAz, tm, devaz);
        return kTRUE;
    }

    if (fReportCosy && t->GetName()==(TString)"MTimeDrive")
    {
        if (!fReportCosy)
        {
            *fLog << err << "ERROR: fReportCosy==NULL... abort." << endl;
            return kERROR;
        }

        AddPoint(fDevTimeCosy, tm, fReportCosy->GetAbsError()*60);
        if (fPosZd.GetY()[fPosZd.GetN()-1] != fReportCosy->GetNominalZd())
            AddPoint(fPosZd, tm, fReportCosy->GetNominalZd());
        //if (fPosAz.GetY()[fPosAz.GetN()-1] != fReportCosy->GetNominalAz())
        //    AddPoint(fPosAz, tm, fReportCosy->GetNominalAz());
        return kTRUE;
    }

    return kTRUE;
}

void MHPointing::DrawGraph(TGraph &g, const char *y) const
{
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
        axe->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
        axe->SetTimeDisplay(1);
        axe->SetTitle("Time");
        if (y)
            h->SetYTitle(y);
    }
}

// --------------------------------------------------------------------------
//
// Update position of an axis on the right side of the histogram
//
void MHPointing::UpdateRightAxis(TGraph &g1, TGraph &g2) const
{
    TH1 &h1 = *g1.GetHistogram();
    TH1 &h2 = *g2.GetHistogram();

    const Double_t max = TMath::Max(h1.GetMaximum(), h2.GetMaximum());
    if (max==0)
        return;

    TGaxis *axis = (TGaxis*)gPad->FindObject("RightAxis");
    if (!axis)
        return;

    axis->SetX1(g1.GetXaxis()->GetXmax());
    axis->SetX2(g1.GetXaxis()->GetXmax());
    axis->SetY1(gPad->GetUymin());
    axis->SetY2(gPad->GetUymax());
    axis->SetWmax(max);
}

// --------------------------------------------------------------------------
//
// Draw an axis on the right side of the histogram
//

void MHPointing::DrawRightAxis(const char *title) const
{
    TGaxis *axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(),
                              gPad->GetUxmax(), gPad->GetUymax(),
                              0, 1, 510, "+L");
    axis->SetName("RightAxis");
    axis->SetTitle(title);
    axis->SetTitleOffset(0.9);
    axis->SetTextColor(kBlue);
    axis->SetBit(kCanDelete);
    axis->Draw();
}

// --------------------------------------------------------------------------
//
// This displays the TGraph like you expect it to be (eg. time on the axis)
// It should also make sure, that the displayed time always is UTC and
// not local time...
//
void MHPointing::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad();

    pad->Divide(2,2);

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fDevTimeSG.Draw("AP");
    fDevTimeCosy.Draw("P");

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fBrightness.Draw("AP");
    fNumStars.Draw("P");
    fNumStarsCor.Draw("P");
    DrawRightAxis("N");

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fDevZd.Draw("AP");
    fDevAz.Draw("P");

    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fPosZd.Draw("AP");
    //fPosAz.Draw("P");
    //DrawRightAxis("Az [\\circ]");
}

void MHPointing::Paint(Option_t *o)
{
    // If this is set to early the plot remains empty in root 5.12/00
    if (fDevTimeSG.GetN()>0)
        fDevTimeSG.SetMinimum(0);
    if (fDevTimeCosy.GetN()>0)
        fDevTimeCosy.SetMinimum(0);


    if (fBrightness.GetN()>0)
    {
        fBrightness.SetMinimum(0);
        fBrightness.SetMaximum(95);
    }
    if (fNumStars.GetN()>0)
    {
        fNumStars.SetMinimum(0);
        fNumStars.SetMaximum(95);
    }
    if (fNumStarsCor.GetN()>0)
    {
        fNumStarsCor.SetMinimum(0);
        fNumStarsCor.SetMaximum(95);
    }

    DrawGraph(fDevTimeSG,   "\\Delta [arcmin]");
    DrawGraph(fDevTimeCosy, "\\Delta [arcmin]");
    DrawGraph(fBrightness,  "Brightness [au]");
    DrawGraph(fNumStars,    "N");
    DrawGraph(fDevZd,       "\\Delta [arcmin]");
    DrawGraph(fDevAz,       "\\Delta [arcmin]");
    DrawGraph(fPosZd,       "Zd [\\circ]");
    //DrawGraph(fPosAz,       "Az [\\circ]");

    TVirtualPad *pad = gPad;

    pad->cd(2);
    if (gPad)
    {
        fNumStars.GetHistogram()->GetYaxis()->SetTitleColor(kBlue);
        UpdateRightAxis(fNumStars, fNumStarsCor);
    }
/*
    pad->cd(4);
    if (gPad)
    {
        fPosAz.GetHistogram()->GetYaxis()->SetTitleColor(kBlue);
        UpdateRightAxis(fPosAz);
    }*/
}
