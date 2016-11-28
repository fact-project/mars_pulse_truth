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
!   Author(s): Thomas Bretz, 10/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHPedestalCor
//
// A histogram to get the pedestal autocorrelation matrix. Tests have shown
// the the autocorrelation does not depend on the actual slice so a profile
// with the autocorrelation depeding on the distance between two slices
// is filled.
//
// Functions to provide the autocorrelation as a TH2D or a TMatrix are
// provided.
//
// Input:
//  - MPedestalSubtractedEvt (from MFillH)
//
/////////////////////////////////////////////////////////////////////////////
#include "MHPedestalCor.h"

#include <TH2.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MCamEvent.h"

#include "MGeomCam.h"

#include "MBinning.h"
#include "MPedestalSubtractedEvt.h"

ClassImp(MHPedestalCor);

using namespace std;

const TString MHPedestalCor::gsDefName  = "MHPedestalCor";
const TString MHPedestalCor::gsDefTitle = "Histogram for autocorrelation";

// --------------------------------------------------------------------------
//
// Initialize the name and title of the task. Set fType to 0
//
MHPedestalCor::MHPedestalCor(const char *name, const char *title)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
    /*
    fHist.SetNameTitle("AutoCor", "Autocorrelation of slices");
    fHist.SetDirectory(0);

    fHist2.SetNameTitle("AutoCorProf", "Autocorrelation from Profile");
    fHist2.SetDirectory(0);

    fHist3.SetNameTitle("Dev", "Deviation of Profile from plain correlation");
    fHist3.SetDirectory(0);

    MBinning binsx(61, -30.5, 30.5);
    MBinning binsy(30, -0.5, 29.5);

    MH::SetBinning(&fHist,  &binsy, &binsy);
    MH::SetBinning(&fHist2, &binsy, &binsy);
    MH::SetBinning(&fHist3, &binsy, &binsy);
    */
    fProf.SetNameTitle("AutoCorP", "Profile of auto correlation");
    fProf.SetDirectory(0);

    const MBinning binsx(15, -0.5, 14.5);
    MH::SetBinning(fProf, binsx);
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MCamEvent-Container.
//
Int_t MHPedestalCor::Fill(const MParContainer *par, const Stat_t w)
{
    const MPedestalSubtractedEvt *evt = dynamic_cast<const MPedestalSubtractedEvt*>(par);
    if (!evt)
    {
        *fLog << err << dbginf << "Got no MCamEvent as argument of Fill()..." << endl;
        return kERROR;
    }

    // Implement an extraction range
    // Implement a check as in PedCalcFromLoGain

    const Int_t np = evt->GetNumPixels();
    const Int_t ns = evt->GetNumSamples();

    Int_t fCheckWinFirst = 0;
    Int_t fCheckWinLast  = ns;

    Int_t fExtractWinFirst = 17;
    Int_t fExtractWinLast  = ns;

    Float_t fMaxSignalVar =  40;
    Float_t fMaxSignalAbs = 250;

    for (int k=0; k<np; k++)
    {
        // This is the fast workaround to put hi- and lo-gains together
        USample_t *slices = evt->GetSamplesRaw(k);//pixel.GetSamples();

        USample_t max = 0;
        USample_t min = (USample_t)-1;

        // Find the maximum and minimum signal per slice in the high gain window
        for (USample_t *slice=slices+fCheckWinFirst; slice<slices+fCheckWinLast; slice++)
        {
            if (*slice > max)
                max = *slice;
            if (*slice < min)
                min = *slice;
        }

        // If the maximum in the high gain window is smaller than
        if (max-min>=fMaxSignalVar || max>=fMaxSignalAbs)
            continue;

        const Float_t *sig = evt->GetSamples(k);

        for (int i=fExtractWinFirst; i<fExtractWinLast; i++)
        {
            const Double_t s2 = sig[i]*sig[i];
            //            fHist.Fill(i, i, s2);
            fProf.Fill(0., s2);

            for (int j=fExtractWinFirst; j<fExtractWinLast; j++)
            {
                const Double_t sij = sig[i]*sig[j];

//                fHist.Fill(i, j, sij);
//                fHist.Fill(j, i, sij);

                fProf.Fill(i-j,  sij);
//                fProf.Fill(j-i,  sij);
            }
        }
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Return the autocorrelation matrix as a TH2D
//
void MHPedestalCor::GetAutoCorrelation(TH2 &h) const
{
    const Int_t n = fProf.GetNbinsX()*2+1;

    const Axis_t xmax = fProf.GetXaxis()->GetXmax();

    const MBinning bins(n, -xmax, xmax);
    MH::SetBinning(h, bins, bins);

    for (int x=0; x<n; x++)
        for (int y=0; y<n; y++)
            h.SetBinContent(x+1, y+1, fProf.GetBinContent(TMath::Abs(x-y)));

}

// --------------------------------------------------------------------------
//
// Return the autocorrelation into the TMatrix
//
void MHPedestalCor::GetAutoCorrelation(TMatrix &m) const
{
    const Int_t n = fProf.GetNbinsX()*2+1;
    m.ResizeTo(n, n);

    for (int x=0; x<n; x++)
        for (int y=0; y<n; y++)
            m[x][y] = fProf.GetBinContent(TMath::Abs(x-y));
}

/*
void MHPedestalCor::Paint(Option_t *)
{
    MH::SetPalette("pretty");

    for (int x=0; x<fHist.GetNbinsX(); x++)
        for (int y=0; y<fHist.GetNbinsX(); y++)
            fHist2.SetBinContent(x+1, y+1, fProf.GetBinContent(fProf.GetXaxis()->FindFixBin(x-y)));

    fHist.Copy(fHist3);
    fHist3.Add(&fHist2, -(fHist.GetEntries()/30/30));
    fHist3.Divide(&fHist);
    fHist3.Scale(100);
    fHist3.SetMaximum();
}
*/

void MHPedestalCor::Draw(Option_t *)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);
    pad->SetFrameBorderMode(0);

    //pad->Divide(2, 2, 0.001, 0.001);

    AppendPad();
/*
    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fHist.Draw("colz");

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fHist2.Draw("colz");

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fHist3.Draw("colz");

    pad->cd(4);*/

    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fProf.Draw();
}
