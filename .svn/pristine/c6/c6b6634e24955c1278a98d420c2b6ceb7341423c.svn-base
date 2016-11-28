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
!   Author(s): Thomas Bretz, 03/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHNewImagePar2
//
// ClassVersion 2:
// ---------------
//  - fMm2Deg
//  - fUseMmScale
//
////////////////////////////////////////////////////////////////////////////
#include "MHNewImagePar2.h"

#include <math.h>

#include <TH1.h>
#include <TPad.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MBinning.h"
#include "MParList.h"

#include "MHillas.h"
#include "MNewImagePar2.h"

ClassImp(MHNewImagePar2);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup histograms 
//
MHNewImagePar2::MHNewImagePar2(const char *name, const char *title)
    : fGeom(0)
{
    fName  = name  ? name  : "MHNewImagePar2";
    fTitle = title ? title : "Histograms of new image parameters 2";

    fHistBorder1.SetName("Border1");
    fHistBorder1.SetTitle("Border Line of border pixels (pixel border)");
    fHistBorder1.SetXTitle("Border [\\circ]]");
    fHistBorder1.SetYTitle("Counts");
    fHistBorder1.SetDirectory(NULL);
    fHistBorder1.UseCurrentStyle();
    fHistBorder1.SetFillStyle(4000);

    fHistBorder2.SetName("Border2");
    fHistBorder2.SetTitle("Border Line of border pixels (pixel center)");
    fHistBorder2.SetXTitle("Border [\\circ]]");
    fHistBorder2.SetYTitle("Counts");
    fHistBorder2.SetDirectory(NULL);
    fHistBorder2.UseCurrentStyle();
    fHistBorder2.SetLineColor(kBlue);
    fHistBorder2.SetFillStyle(4000);
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning
//
Bool_t MHNewImagePar2::SetupFill(const MParList *plist)
{
    fGeom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }

    const MBinning *bins = (MBinning*)plist->FindObject("BinningBorder");
    if (!bins)
    {
        MBinning b;
        b.SetEdges(87, 0, 10);
        b.Apply(fHistBorder1);
        b.Apply(fHistBorder2);
    }
    else
    {
        bins->Apply(fHistBorder1);
        bins->Apply(fHistBorder2);
    }

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MNewImagePar2 container.
//
Int_t MHNewImagePar2::Fill(const MParContainer *par, const Stat_t w)
{
    const MNewImagePar2 *h = dynamic_cast<const MNewImagePar2*>(par);
    if (!h)
    {
        *fLog << err << "MHNewImagePar2::Fill: Pointer (!=NULL) expected." << endl;
        return kERROR;
    }

    const Double_t scale = fGeom->GetConvMm2Deg();

    fHistBorder1.Fill(h->GetBorderLinePixel() *scale, w);
    fHistBorder2.Fill(h->GetBorderLineCenter()*scale, w);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the two histograms into it.
// Be careful: The histograms belongs to this object and won't get deleted
// together with the canvas.
//
void MHNewImagePar2::Draw(Option_t *)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    gPad->SetBorderMode(0);
    MH::DrawSame(fHistBorder1, fHistBorder2, "Border Line");
}
