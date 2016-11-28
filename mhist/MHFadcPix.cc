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
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////
//
// MHFadcPix
//
// This container stores a histogram to display an Fadc Spektrum.
// The spektrum of all the values measured by the Fadcs.
//
///////////////////////////////////////////////////////////////////////
#include "MHFadcPix.h"

#include <TPad.h>

#include "MH.h"
#include "MBinning.h"

#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"

ClassImp(MHFadcPix);

// --------------------------------------------------------------------------
//
// Creates the histograms for lo and hi gain of one pixel
//
MHFadcPix::MHFadcPix(Int_t pixid, Type_t t)
    : fPixId(pixid), fType(t)
{
    fHistHi.SetName(pixid>=0 ? Form("HiGain%03d", pixid) : "HiGain");
    fHistHi.SetTitle(pixid>=0 ? Form("Hi Gain Pixel #%d", pixid) : "Hi Gain Samples");
    fHistHi.SetDirectory(NULL);
    fHistHi.UseCurrentStyle();

    fHistLo.SetName(pixid>=0 ? Form("LoGain%03d", pixid) : "LoGain");
    fHistLo.SetTitle(pixid>=0 ? Form("Lo Gain Pixel #%d", pixid) : "Lo Gain Samples");
    fHistLo.SetDirectory(NULL);
    fHistLo.UseCurrentStyle();

    if (fType==kValue)
    {
        fHistHi.SetXTitle("Signal/FADC Units");
        fHistLo.SetXTitle("Signal/FADC Units");
        fHistHi.SetYTitle("Count");
        fHistLo.SetYTitle("Count");

        MBinning bins;
        bins.SetEdges(255, -.5, 255.5);

        bins.Apply(fHistHi);
        bins.Apply(fHistLo);
    }
    else
    {
        fHistHi.SetXTitle("Time/FADC Slices");
        fHistLo.SetXTitle("Time/FADC Slices");
        fHistLo.SetYTitle("Sum Signal/FADC Units");
        fHistHi.SetYTitle("Sum Signal/FADC Units");
    }
}

void MHFadcPix::Init(Byte_t nhi, Byte_t nlo)
{
    MBinning bins;

    bins.SetEdges(nhi, -.5, -.5+nhi);
    bins.Apply(fHistHi);

    bins.SetEdges(nlo, -.5, -.5+nlo);
    bins.Apply(fHistLo);
}

Bool_t MHFadcPix::Fill(const MRawEvtData &evt)
{
    MRawEvtPixelIter pixel((MRawEvtData*)&evt);

    if (!pixel.Jump(fPixId))
        return kTRUE;

    const Int_t nhisamples = evt.GetNumHiGainSamples();

    if (fType==kValue)
        for (Int_t i=0; i<nhisamples; i++)
            fHistHi.Fill(pixel.GetHiGainSamples()[i]);
    else
        for (Int_t i=0; i<nhisamples; i++)
            fHistHi.Fill(i, pixel.GetHiGainSamples()[i]);

    if (!pixel.HasLoGain())
        return kTRUE;

    const Int_t nlosamples = evt.GetNumLoGainSamples();

    if (fType==kValue)
        for (Int_t i=0; i<nlosamples; i++)
            fHistLo.Fill(pixel.GetLoGainSamples()[i]);
    else
        for (Int_t i=0; i<nlosamples; i++)
            fHistLo.Fill(i, pixel.GetLoGainSamples()[i]);

    return kTRUE;
}

// --------------------------------------------------------------------------
void MHFadcPix::DrawHi()
{
    fHistHi.Draw();
}

// --------------------------------------------------------------------------
void MHFadcPix::DrawLo()
{
    fHistLo.Draw();
}

// --------------------------------------------------------------------------
//
// We need our own clone function to get rid of the histogram in any
// directory
//
TObject *MHFadcPix::Clone(const char *) const
{
    MHFadcPix &pix = *(MHFadcPix*)TObject::Clone();

    pix.fHistHi.SetDirectory(NULL);
    pix.fHistLo.SetDirectory(NULL);

    return &pix;
}

// --------------------------------------------------------------------------
void MHFadcPix::Draw(Option_t *)
{
    if (!gPad)
    {
        const char *name = StrDup(fPixId ? Form("Pixel #%d", fPixId) : "Pixel");
        MH::MakeDefCanvas(name, fPixId ? Form("%s FADC Samples", name) : "FADC Samples");
        delete [] name;
    }

    gPad->Divide(1, 2);

    gPad->cd(1);
    fHistHi.Draw();

    gPad->cd(2);
    fHistLo.Draw();
}
