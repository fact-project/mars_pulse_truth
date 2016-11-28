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
!   Author(s): Thomas Bretz  2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Wolfgang Wittek  2002 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHVsSize
//
// This class contains histograms for the source independent image parameters
//
// ClassVersion 2:
// ---------------
//  - fMm2Deg
//  - fUseMmScale
//
/////////////////////////////////////////////////////////////////////////////
#include "MHVsSize.h"

#include <TH2.h>
#include <TPad.h>
#include <TStyle.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MHillas.h"
#include "MHillasSrc.h"
#include "MHillasExt.h"
#include "MNewImagePar.h"
#include "MGeomCam.h"
#include "MBinning.h"

ClassImp(MHVsSize);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup four histograms for Width, Length
//
MHVsSize::MHVsSize(const char *name, const char *title)
    : fGeom(0), fHillas(0), fHillasExt(0), fNewImagePar(0)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHVsSize";
    fTitle = title ? title : "Source independent image parameters";

    fLength.SetNameTitle("Length", "Length vs. Size");
    fWidth.SetNameTitle( "Width",  "Width vs. Size");
    fDist.SetNameTitle(  "Dist",   "Dist vs. Size");
    fConc1.SetNameTitle( "Conc1",  "Conc1 vs. Size");
    fArea.SetNameTitle(  "Area",   "Area vs. Size");
    fM3Long.SetNameTitle("M3Long", "M3Long vs. Size");

    fLength.SetDirectory(NULL);
    fWidth.SetDirectory(NULL);
    fDist.SetDirectory(NULL);
    fConc1.SetDirectory(NULL);
    fArea.SetDirectory(NULL);
    fM3Long.SetDirectory(NULL);

    fLength.SetXTitle("Size [phe]");
    fWidth.SetXTitle("Size [phe]");
    fDist.SetXTitle("Size [phe]");
    fConc1.SetXTitle("Size [phe]");
    fArea.SetXTitle("Size [phe]");
    fM3Long.SetXTitle("Size [phe]");

    fLength.SetYTitle("Length [\\circ]");
    fWidth.SetYTitle("Width [\\circ]");
    fDist.SetYTitle("Distance [\\circ]");
    fConc1.SetYTitle("Conc1 [ratio]");
    fArea.SetYTitle("Area [\\circ^{2}]");
    fM3Long.SetYTitle("M3Long [\\circ]");

    const MBinning binse( 50,   10, 1e5, "", "log");
    const MBinning binsc(100, 3e-3, 1,   "", "log");
    const MBinning binsl(100,    0, 0.5);
    const MBinning binsd(100,    0, 2.0);
    const MBinning binsa(100,    0, 0.25);
    const MBinning binsm(100, -1.5, 1.5);

    MH::SetBinning(fLength, binse, binsl);
    MH::SetBinning(fWidth,  binse, binsl);
    MH::SetBinning(fDist,   binse, binsd);
    MH::SetBinning(fConc1,  binse, binsc);
    MH::SetBinning(fArea,   binse, binsa);
    MH::SetBinning(fM3Long, binse, binsm);

    fLength.UseCurrentStyle();
    fWidth.UseCurrentStyle();
    fDist.UseCurrentStyle();
    fConc1.UseCurrentStyle();
    fArea.UseCurrentStyle();
    fM3Long.UseCurrentStyle();
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning (with the names 'BinningWidth' and 'BinningLength')
// are found in the parameter list
// Use this function if you want to set the conversion factor which
// is used to convert the mm-scale in the camera plain into the deg-scale
// used for histogram presentations. The conversion factor is part of
// the camera geometry. Please create a corresponding MGeomCam container.
//
Bool_t MHVsSize::SetupFill(const MParList *plist)
{
    fGeom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }

    fHillas = (MHillas*)plist->FindObject("MHillas");
    if (!fHillas)
    {
        *fLog << err << "MHillas not found... abort." << endl;
        return kFALSE;
    }

    fHillasExt = (MHillasExt*)plist->FindObject("MHillasExt");
    if (!fHillasExt)
    {
        *fLog << err << "MHillasExt not found... abort." << endl;
        return kFALSE;
    }

    fNewImagePar = (MNewImagePar*)plist->FindObject("MNewImagePar");
    if (!fNewImagePar)
    {
        *fLog << err << "MNewImagePar not found... abort." << endl;
        return kFALSE;
    }

    /*
     ApplyBinning(*plist, "Width",  "Size", fWidth);
     ApplyBinning(*plist, "Length", "Size", fLength);
     ApplyBinning(*plist, "Area",   "Size", fArea);
     ApplyBinning(*plist, "M3Long", "Size", fM3Long);
     ApplyBinning(*plist, "Conc1",  "Size", fConc1);
     */

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MHillas-Container.
// Be careful: Only call this with an object of type MHillas
//
Int_t MHVsSize::Fill(const MParContainer *par, const Stat_t w)
{
    const MHillasSrc *src = dynamic_cast<const MHillasSrc*>(par);
    if (!src)
    {
        *fLog << err << "MHVsSize::Fill: Wrong argument... abort." << endl;
        return kERROR;
    }

    const Double_t scale = fGeom->GetConvMm2Deg();

    fLength.Fill(fHillas->GetSize(), scale*fHillas->GetLength(),     w);
    fWidth.Fill( fHillas->GetSize(), scale*fHillas->GetWidth(),      w);
    fDist.Fill(  fHillas->GetSize(), scale*src->GetDist(),           w);
    fConc1.Fill( fHillas->GetSize(), fNewImagePar->GetConc1(),       w);
    fArea.Fill(  fHillas->GetSize(), scale*scale*fHillas->GetArea(), w);
    fM3Long.Fill(fHillas->GetSize(), scale*fHillasExt->GetM3Long()*TMath::Sign(1.0f, src->GetCosDeltaAlpha()), w);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the four histograms into it.
// Be careful: The histograms belongs to this object and won't get deleted
// together with the canvas.
//
void MHVsSize::Draw(Option_t *o)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    TString opt(o);
    opt.ToLower();

    // FIXME: If same-option given make two independant y-axis!
    const Bool_t same = opt.Contains("same");

    if (!same)
        pad->Divide(3,2);
    else
    {
        fLength.SetName("LengthSame");
        fWidth.SetName("WidthSame");
        fDist.SetName("DistSame");
        fConc1.SetName("Conc1Same");
        fArea.SetName("AreaSame");
        fM3Long.SetName("M3LongSame");

        fLength.SetDirectory(0);
        fWidth.SetDirectory(0);
        fDist.SetDirectory(0);
        fConc1.SetDirectory(0);
        fArea.SetDirectory(0);
        fM3Long.SetDirectory(0);

        fDist.SetMarkerColor(kBlue);
        fConc1.SetMarkerColor(kBlue);
        fWidth.SetMarkerColor(kBlue);
        fLength.SetMarkerColor(kBlue);
        fArea.SetMarkerColor(kBlue);
        fM3Long.SetMarkerColor(kBlue);
    }

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    RemoveFromPad("LengthSame");
    fLength.Draw(same?"same":"");

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    RemoveFromPad("WidthSame");
    fWidth.Draw(same?"same":"");

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    RemoveFromPad("DistSame");
    fDist.Draw(same?"same":"");

    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    RemoveFromPad("AreaSame");
    fArea.Draw(same?"same":"");

    pad->cd(5);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    RemoveFromPad("M3LongSame");
    fM3Long.Draw(same?"same":"");

    pad->cd(6);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetLogy();
    RemoveFromPad("Conc1Same");
    fConc1.Draw(same?"same":"");
}
