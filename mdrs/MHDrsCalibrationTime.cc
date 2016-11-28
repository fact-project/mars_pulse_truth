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
!   Author(s): Thomas Bretz 2013 <mailto:tbretz@physik.rwth-aachen.de>
!
!   Copyright: MAGIC Software Development, 2000-2015
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////
//
// MHDrsCalibrationTime
//
///////////////////////////////////////////////////////////////////////
#include "MHDrsCalibrationTime.h"

#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MStatusDisplay.h"

#include "MDrsCalibrationTime.h"
#include "MPedestalSubtractedEvt.h"

#include "MParList.h"
#include "MRawEvtData.h"

#include "MBinning.h"

ClassImp(MHDrsCalibrationTime);

using namespace std;

MHDrsCalibrationTime::MHDrsCalibrationTime(const char *name, const char *title) :
fRaw(0), fEvt(0), fCal(0)

{
    //
    // set the name and title of this object
    //
    fName  = name  ? name  : "MHDrsCalibrationTime";
    fTitle = title ? title : "Container for ADC spectra histograms";

    MBinning bx( 160, -0.5,  159.5);
    MBinning by(1024, -0.5, 1023.5);

    MH::SetBinning(&fHist, &by, &bx);

    fHist.SetName("Offset");
    fHist.SetTitle("DRS cell time offset");

    fHist.SetXTitle("Absolute position in pipeline");
    fHist.SetYTitle("DRS time marker channel");
    fHist.SetZTitle("\\Delta t / slices");

    fHist.SetStats(kFALSE);
    fHist.SetContour(99);
    fHist.SetDirectory(0);
}

Bool_t MHDrsCalibrationTime::SetupFill(const MParList *pList)
{
    /*
    SetTitle("DRS bla bla;;ADC signal [mV];");

    if (!MHCamEvent::SetupFill(pList))
        return kFALSE;

    fSum->ResetBit(MHCamera::kProfile);
    */


    fData.Reset();

    return kTRUE;
}

Bool_t MHDrsCalibrationTime::ReInit(MParList *pList)
{
    fRaw = (MRawEvtData*)pList->FindObject("MRawEvtData");
    if (!fRaw)
    {
        *fLog << err << "MRawEvtData not found... aborting." << endl;
        return kFALSE;
    }

    if (!fRaw->HasStartCells())
    {
        *fLog << err << "MRawEvtData has no start cells stored... aborting." << endl;
        return kFALSE;
    }

    if (fRaw->GetNumPixels()==0 || fRaw->GetNumSamples()==0)
    {
        *fLog << err << "MRawEvtData contains either no pixels or no samples... aborting." << endl;
        return kFALSE;
    }

    fEvt = (MPedestalSubtractedEvt*)pList->FindObject("MPedestalSubtractedEvt");
    if (!fEvt)
    {
        *fLog << err << "MPedestalSubtractedEvt not found... aborting." << endl;
        return kFALSE;
    }

    if (fRaw->GetNumSamples()!=1024)
    {
        *fLog << err << "Only 1024 samples are supported for the time calibration" << endl;
        return kFALSE;
    }

    fCal = (MDrsCalibrationTime*)pList->FindCreateObj("MDrsCalibrationTime");
    if (!fCal)
        return kFALSE;

    fData.InitSize(160, 1024);

    return kTRUE;
}

Int_t MHDrsCalibrationTime::Fill(const MParContainer *par, const Stat_t w)
{
    fData.AddT(fEvt->GetSamples(0),
               reinterpret_cast<int16_t*>(fRaw->GetStartCells()), +1);

    return kTRUE;
}

void MHDrsCalibrationTime::InitHistogram()
{
    DrsCalibrateTime t = fData.GetResult();
    if (t.fNumEntries==0)
        return;

    for (int p=0; p<160; p++)
        for (int s=0; s<1024; s++)
            fHist.SetBinContent(s+1, p+1, t.fStat[p*t.fNumSamples+s].first);
}

void MHDrsCalibrationTime::PlotAll()
{
    if (!fDisplay)
        return;

    InitHistogram();

    for (int b=0; b<10; b++)
    {
        TCanvas &c = fDisplay->AddTab(Form("B%d", b));

        c.Divide(2,2);

        for (int i=0; i<4; i++)
        {
            c.cd(i+1);

            gPad->SetGrid();
            gPad->SetBorderMode(0);
            gPad->SetFrameBorderMode(0);
            gPad->SetRightMargin(0.01);

            const int col[] = { kBlack, kRed, kBlue, kGreen };

            for (int crate=0; crate<4; crate++)
            {
                TH1 *h = fHist.ProjectionX(Form("%d-%d-%d", crate, b, i), crate*40+b*4+i+1, crate*40+b*4+i+1);
                h->SetName(Form("Crate%d", crate));
                h->SetTitle(Form("Board %d, Chip %d (All four crates)", b, i));
                h->SetXTitle("Slice");
                h->SetYTitle("\\Delta t / slices");
                h->SetMinimum(-10);
                h->SetMaximum(10);
                h->SetStats(kFALSE);
                h->SetDirectory(0);
                h->SetMarkerStyle(kFullDotMedium);
                h->SetMarkerColor(col[crate]);
                h->SetLineColor(col[crate]);
                h->DrawCopy(crate==0 ? "hist p" : "hist P same");
            }
        }
    }

    // 4 crates
    // 10 boards
    // 4 chips
}

void MHDrsCalibrationTime::Draw(Option_t *o)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);

    AppendPad("");

    pad->Divide(1,1);

    pad->cd(1);

    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);

    fHist.Draw("colz");
}


void MHDrsCalibrationTime::Paint(Option_t *o)
{
    MH::SetPalette();
    InitHistogram();
}

Bool_t MHDrsCalibrationTime::Finalize()
{
    //    InitHistogram();
    fCal->SetCalibration(fData.GetResult());
    return kTRUE;
}
