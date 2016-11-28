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
!   Author(s): Thomas Bretz  2001 <mailto:tbretz@uni-sw.gwdg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////
//
// MHHillasSrc
//
// This class contains histograms for every Hillas parameter
//
// ClassVersion 2:
// ---------------
//  - fMm2Deg
//  - fUseMmScale
//
///////////////////////////////////////////////////////////////////////
#include "MHHillasSrc.h"

#include <math.h>

#include <TH1.h>
#include <TPad.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"

#include "MParList.h"

#include "MHillasSrc.h"

ClassImp(MHHillasSrc);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup four histograms for Alpha, and Dist
//
MHHillasSrc::MHHillasSrc(const char *name, const char *title)
    : fGeom(0)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHHillasSrc";
    fTitle = title ? title : "Container for Hillas histograms";

    //
    // loop over all Pixels and create two histograms
    // one for the Low and one for the High gain
    // connect all the histogram with the container fHist
    //
    fAlpha    = new TH1F("Alpha",    "Alpha of Ellipse",                 90,   -90,  90);
    fDist     = new TH1F("Dist",     "Dist of Ellipse",                  70,     0, 2.1);
    fCosDA    = new TH1F("CosDA",    "cos(Delta,Alpha) of Ellipse",     101,    -1,   1);
    fDCA      = new TH1F("DCA",      "Distance of closest aproach",     101,  -1.7, 1.7);
    fDCADelta = new TH1F("DCADelta", "Angle between shower and x-axis",  80,     0, 360);

    fAlpha->SetDirectory(NULL);
    fDist->SetDirectory(NULL);
    fCosDA->SetDirectory(NULL);
    fDCA->SetDirectory(NULL);
    fDCADelta->SetDirectory(NULL);

    fAlpha->SetXTitle("\\alpha [\\circ]");
    fDist->SetXTitle("Dist [\\circ]");
    fCosDA->SetXTitle("cos(\\delta,\\alpha)");
    fDCA->SetXTitle("DCA [\\circ]");
    fDCADelta->SetXTitle("DCADelta [0, 2\\pi]");

    fAlpha->SetYTitle("Counts");
    fDist->SetYTitle("Counts");
    fCosDA->SetYTitle("Counts");
    fDCA->SetYTitle("Counts");
    fDCADelta->SetYTitle("Counts");

    fAlpha->SetMinimum(0);
    fCosDA->SetMinimum(0);
    fDCADelta->SetMinimum(0);
}

// --------------------------------------------------------------------------
//
// Delete the four histograms
//
MHHillasSrc::~MHHillasSrc()
{
    delete fAlpha;
    delete fDist;
    delete fCosDA;
    delete fDCA;
    delete fDCADelta;
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning (with the names 'BinningAlpha' and 'BinningDist')
// are found in the parameter list
// Use this function if you want to set the conversion factor which
// is used to convert the mm-scale in the camera plain into the deg-scale
// used for histogram presentations. The conversion factor is part of
// the camera geometry. Please create a corresponding MGeomCam container.
//
Bool_t MHHillasSrc::SetupFill(const MParList *plist)
{
    fGeom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }

    ApplyBinning(*plist, "Alpha",    *fAlpha);
    ApplyBinning(*plist, "Dist",     *fDist);
    ApplyBinning(*plist, "DCA",      *fDCA);
    ApplyBinning(*plist, "DCADelta", *fDCADelta);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the four histograms with data from a MHillas-Container.
// Be careful: Only call this with an object of type MHillas
//
Int_t MHHillasSrc::Fill(const MParContainer *par, const Stat_t w)
{
    if (!par)
    {
        *fLog << err << "MHHillasSrc::Fill: Pointer (!=NULL) expected." << endl;
        return kERROR;
    }

    const MHillasSrc &h = *(MHillasSrc*)par;

    const Double_t scale = fGeom->GetConvMm2Deg();

    fAlpha->Fill(h.GetAlpha(), w);
    fDist ->Fill(h.GetDist()*scale, w);
    fCosDA->Fill(h.GetCosDeltaAlpha(), w);
    fDCA  ->Fill(h.GetDCA()*scale, w);
    fDCADelta->Fill(h.GetDCADelta(), w);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the two histograms into it.
// Be careful: The histograms belongs to this object and won't get deleted
// together with the canvas.
//
void MHHillasSrc::Draw(Option_t *o)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    // FIXME: Display Source position

    // FIXME: If same-option given make two independant y-axis!
    const TString opt(o);
    const Bool_t same = opt.Contains("same");

    if (!same)
        pad->Divide(2,2);
    else
    {
        fAlpha->SetName("AlphaSame");
        fDist ->SetName("DistSame");
        fCosDA->SetName("CosDASame");
        fDCA  ->SetName("DCASame");
        fDCADelta->SetName("DCADeltaSame");

        fAlpha->SetDirectory(0);
        fDist ->SetDirectory(0);
        fCosDA->SetDirectory(0);
        fDCA  ->SetDirectory(0);
        fDCADelta->SetDirectory(0);

        fAlpha->SetLineColor(kBlue);
        fDist->SetLineColor(kBlue);
        fDCA->SetLineColor(kBlue);
        fCosDA->SetLineColor(kBlue);
        fDCADelta->SetLineColor(kBlue);
    }

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("AlphaSame");
    fAlpha->Draw(same?"same":"");

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("DistSame");
    fDist->Draw(same?"same":"");

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("DCASame");
    fDCA->Draw(same?"same":"");

    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();

    TVirtualPad *p = gPad;
    if (!same)
        p->Divide(1,2);
    p->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("CosDASame");
    fCosDA->Draw(same?"same":"");

    p->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    RemoveFromPad("DCADeltaSame");
    fDCADelta->Draw(same?"same":"");
}

void MHHillasSrc::Paint(Option_t *opt)
{
    if (fCosDA->GetEntries()==0)
        return;

    TVirtualPad *savepad = gPad;
    savepad->cd(4);
    gPad->SetLogy();
    gPad = savepad;
}

TH1 *MHHillasSrc::GetHistByName(const TString name) const
{
    if (name.Contains("Alpha", TString::kIgnoreCase))
        return fAlpha;
    if (name.Contains("Dist", TString::kIgnoreCase))
        return fDist;
    if (name.Contains("CosDA", TString::kIgnoreCase))
        return fCosDA;

    return NULL;
}
