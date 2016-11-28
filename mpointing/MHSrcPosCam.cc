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
!   Author(s): Thomas Bretz, 2/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2007
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MHSrcPosCam
//
//
// FIXME: Use ReInit...
//
//////////////////////////////////////////////////////////////////////////////
#include "MHSrcPosCam.h"

#include <TVector2.h>
#include <TCanvas.h>
#include <TEllipse.h>

#include "MGeomCam.h"
#include "MSrcPosCam.h"
#include "MParameters.h"
#include "MPointingPos.h"

#include "MString.h"
#include "MBinning.h"
#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHSrcPosCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor
//
MHSrcPosCam::MHSrcPosCam(Float_t offset, const char *name, const char *title)
    : fTimeEffOn(NULL), fEffOnTime(NULL), fSourcePos(NULL), fGeom(NULL),
    fPositions("TVector2", 50000), fWobbleOffset(offset)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHSrcPosCam";
    fTitle = title ? title : "Histogram for source position distribution";

    fHist.SetName("SourcePos");
    fHist.SetTitle("Source position distribution in camera coordinates");
    fHist.SetXTitle("dx [\\circ]");
    fHist.SetYTitle("dy [\\circ]");
    fHist.SetZTitle("T_{eff} [s]");
    fHist.SetDirectory(NULL);
    fHist.UseCurrentStyle();
    fHist.GetXaxis()->CenterTitle();
    fHist.GetYaxis()->CenterTitle();
    fHist.SetContour(99);

    const Float_t x = offset>0 ? offset+0.0499 : offset/2+0.0499;
    const Int_t   n = offset>0 ? 101           : 51;

    const MBinning bins(n, -x, x); // bin=0.01ø  ~0.5SE

    MH::SetBinning(fHist, bins, bins);
}

Bool_t MHSrcPosCam::SetupFill(const MParList *pl)
{
    fGeom = (MGeomCam*)pl->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "ERROR - MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    fTimeEffOn = (MTime*)      pl->FindObject("MTimeEffectiveOnTime");
    fEffOnTime = (MParameterD*)pl->FindObject("MEffectiveOnTime");

    if (!fTimeEffOn && fEffOnTime)
    {
        *fLog << err << "ERROR - MTimeEffOnTime not found... aborting." << endl;
        return kFALSE;
    }
    if (!fEffOnTime && fTimeEffOn)
    {
        *fLog << err << "ERROR - MEffectiveOnTime not found... aborting." << endl;
        return kFALSE;
    }

    if (!fEffOnTime && !fTimeEffOn)
        *fLog << inf << "Neither MTimeEffOnTime nor MEffectiveOnTime found... assuming MC." << endl;
    else
        fTimeLastEffOn = MTime();

    const MPointingPos *pos = (MPointingPos*)pl->FindObject("MSourcePos", "MPointingPos");

    const TString src = pos ? pos->GetString("radec") : "MonteCarlo";
    fHist.SetTitle(MString::Format("SrcPos distribution in camera: %s", src.Data()));

    fHist.Reset();
    fNum = 0;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// All source positions are buffered until the time of the effective on
// time time stamp changes. Then the observation time is split into
// identical parts and the histogram is filled by these events. The
// effective on time time stamp is reset and the buffered source positions
// deleted.
//
Int_t MHSrcPosCam::Fill(const MParContainer *par, const Stat_t w)
{
    const MSrcPosCam *cam = dynamic_cast<const MSrcPosCam*>(par);
    if (!cam)
    {
        *fLog << err << dbginf << "Got no MSrcPosCam as argument of Fill()..." << endl;
        return kERROR;
    }

    if (!fEffOnTime)
    {
        const TVector2 v(cam->GetXY()*fGeom->GetConvMm2Deg());
        fHist.Fill(v.X(), v.Y(), w);
        return kTRUE;
    }

    // Increase array size if necessary
    if (fNum==fPositions.GetSize())
        fPositions.Expand(fNum*2);

    // buffer position into array (could be speed up a little bit more
    // by using ExpandCreate and memcpy)
    new (fPositions[fNum++]) TVector2(cam->GetXY()*fGeom->GetConvMm2Deg());

    // Check if there is a new effective on time
    if (fTimeLastEffOn==MTime())
        fTimeLastEffOn=*fTimeEffOn;

    if (fTimeLastEffOn == *fTimeEffOn)
        return kTRUE;

    // Split the observation time to all buffered events
    const Double_t scale = fEffOnTime->GetVal()/fNum;

    // Fill histogram from array
    for (int i=0; i<fNum; i++)
    {
        const TVector2 &v = (TVector2&)*fPositions[i];
        fHist.Fill(v.X(), v.Y(), scale*w);
    }

    // reset time stamp and remove all buffered positions
    fTimeLastEffOn = *fTimeEffOn;
    fNum = 0;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
void MHSrcPosCam::Paint(Option_t *)
{
    MH::SetPalette("pretty", 99);
}

// --------------------------------------------------------------------------
//
void MHSrcPosCam::Draw(Option_t *)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    //pad->Divide(2,2);

    gPad->SetLeftMargin(0.25);
    gPad->SetRightMargin(0.25);

    gPad->SetGridx();
    gPad->SetGridy();

    AppendPad();

    fHist.Draw("colz");

    if (fHist.GetXaxis()->GetXmax()>0.5)
    {
        // Typical wobble distance +/- 1 shaftencoder step
        TEllipse el;
        el.SetFillStyle(0);
        el.SetLineColor(kBlack);
        el.SetLineStyle(kDashed);
        el.DrawEllipse(0, 0, fWobbleOffset, 0, 0, 360, 0);
        el.SetLineColor(17);
        el.DrawEllipse(0, 0, fWobbleOffset-0.022, 0, 0, 360, 0);
        el.DrawEllipse(0, 0, fWobbleOffset+0.022, 0, 0, 360, 0);
    }
}

