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
!   Author(s): Thomas Bretz  07/2011 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2013
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////
//
// MHDrsCalibration
//
// This class contains a list of MHFadcPix.
//
///////////////////////////////////////////////////////////////////////
#include "MDrsCalibration.h"

#include <fstream>
#include <sstream>
#include <limits.h>

#include "MH.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MGeomCam.h"

#include "MRawEvtData.h"
#include "MRawRunHeader.h"
#include "MHCamera.h"

#include <TH2.h>

ClassImp(MDrsCalibration);

using namespace std;

void MDrsCalibration::Plot()
{
    TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this);
    pad->SetBorderMode(0);

    //AppendPad();

    pad->Divide(2,2);

    MH::SetPalette();

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetRightMargin(0.15);

    TH2F h1("Physcial", "Primary baseline (physical pipeline)", 1440, -0.5, 1439.5, 1024, -0.5, 1023.5);
    h1.SetStats(kFALSE);
    h1.SetXTitle("Pixel");
    h1.SetYTitle("Cell");
    h1.SetZTitle("mV");
    h1.GetYaxis()->SetTitleOffset(1.2);
    h1.GetZaxis()->SetTitleOffset(1.2);
    for (int p=0; p<1440; p++)
        for (int c=0; c<1024; c++)
            h1.SetBinContent(p+1, c+1, fOffset[p*1024+c]);
    h1.Scale(125./fNumOffset/256);
    h1.DrawCopy("colz");

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetRightMargin(0.15);

    TH2F h2("Gain", "Gain measurement (physcial pipeline)", 1440, -0.5, 1439.5, 1024, -0.5, 1023.5);
    h2.SetStats(kFALSE);
    h2.SetXTitle("Pixel");
    h2.SetYTitle("Cell");
    h2.SetZTitle("mV");
    h2.GetYaxis()->SetTitleOffset(1.2);
    h2.GetZaxis()->SetTitleOffset(1.2);
    for (int p=0; p<1440; p++)
        for (int c=0; c<1024; c++)
            h2.SetBinContent(p+1, c+1, fGain[p*1024+c]);
    h2.Scale(125.*1953125/(fNumOffset*fNumGain)/256/1024);
    h2.DrawCopy("colz");

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetRightMargin(0.15);

    TH2F h3("Logical", "Secondary baseline (logical pipeline)", 1440, -0.5, 1439.5, fRoi, -0.5, fRoi-0.5);
    h3.SetStats(kFALSE);
    h3.SetXTitle("Pixel");
    h3.SetYTitle("Channel");
    h3.SetZTitle("mV");
    h3.GetYaxis()->SetTitleOffset(1.2);
    h3.GetZaxis()->SetTitleOffset(1.2);
    for (int p=0; p<1440; p++)
        for (int c=0; c<fRoi; c++)
            h3.SetBinContent(p+1, c+1, fTrgOff[p*fRoi+c]);
    h3.Scale(125./(fNumOffset*fNumTrgOff)/256);
    h3.GetYaxis()->SetRangeUser(4, fRoi-4);
    h3.DrawCopy("colz");

    pad->cd(4);
    gPad->SetFrameBorderMode(0);
}
