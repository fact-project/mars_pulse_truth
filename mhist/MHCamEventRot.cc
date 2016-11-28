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
!   Author(s): Thomas Bretz, 4/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MHCamEventRot
//
// Derotate MCamEvent before filling a 2D histogram.
//
// Axis titles
// ===========
//
// 1) If no other title is given 'a.u.' or 'counts'is used.
// 2) If the corresponding MBinning has no default title. This title
//    is used for the z-axis
// 3) If the title of MHCamEvent is different from the default,
//    it is used as histogram title. You can use this to set the
//    axis title, too. For more information see TH1::SetTitle, eg.
//       SetTitle("MyHist;;;z[photons]");
//    The x- and y-axis title is ignored.
//
// For example:
//   MHCamEventRot myhist("Title;;;z[photons]");
//
//
// To be done: * Set the sky position of the center of the display and
//               correct if the pointing position of the telescope is
//               different.
//             * Make it quadratic like MHCamera
//
//////////////////////////////////////////////////////////////////////////////
#include "MHCamEventRot.h"

#include <TF1.h>
#include <TH2.h>
#include <TMath.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TStopwatch.h>

#include "MGeomCam.h"
#include "MSrcPosCam.h"
#include "MHillasSrc.h"
#include "MTime.h"
#include "MObservatory.h"
#include "MPointingPos.h"
#include "MAstroCatalog.h"
#include "MAstroSky2Local.h"
#include "MStatusDisplay.h"
#include "MMath.h"

#include "MBinning.h"
#include "MParList.h"
#include "MCamEvent.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHCamEventRot);

using namespace std;

const TString MHCamEventRot::gsDefName  = "MHCamEventRot";
const TString MHCamEventRot::gsDefTitle = "Plot showing derotated MCamEvent data";

// --------------------------------------------------------------------------
//
// Default Constructor
//
MHCamEventRot::MHCamEventRot(const char *name, const char *title)
    : fTime(0), fPointPos(0), fObservatory(0), fType(0), fNameTime("MTime"),
    fThreshold(0), fUseThreshold(kNoBound)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    fHist.SetDirectory(NULL);

    fHist.SetName("Derot");
    fHist.SetTitle("2D-plot of MCamEvents (derotated)");
}

// --------------------------------------------------------------------------
//
// Set binnings (takes BinningCamEvent) and prepare filling of the
// histogram.
//
// Also search for MTime, MObservatory and MPointingPos
// 
Bool_t MHCamEventRot::SetupFill(const MParList *plist)
{
    fGeom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    const MBinning *bins = (MBinning*)plist->FindObject("BinningCamEvent");
    if (!bins)
    {
        const Float_t r = fGeom->GetMaxRadius()*fGeom->GetConvMm2Deg();

        const MBinning b(41, -r, r);
        SetBinning(fHist, b, b);
    }
    else
        SetBinning(fHist, *bins, *bins);

    fPointPos = (MPointingPos*)plist->FindObject(AddSerialNumber("MPointingPos"));
    if (!fPointPos)
        *fLog << warn << "MPointingPos not found... no derotation." << endl;

    fTime = (MTime*)plist->FindObject(AddSerialNumber(fNameTime));
    if (!fTime)
        *fLog << warn << "MTime not found... no derotation." << endl;

    fObservatory = (MObservatory*)plist->FindObject(AddSerialNumber("MObservatory"));
    if (!fObservatory)
        *fLog << warn << "MObservatory not found... no derotation." << endl;

    // FIXME: Because the pointing position could change we must check
    // for the current pointing position and add a offset in the
    // Fill function!
    if (fPointPos)
    {
        fRa  = fPointPos->GetRa();
        fDec = fPointPos->GetDec();
    }

    if (bins && bins->HasTitle())
        fHist.SetZTitle(bins->GetTitle());
    else
        if (fTitle!=gsDefTitle)
        {
            fHist.SetTitle(fTitle);
            fHist.SetXTitle("x [\\circ]");
            fHist.SetYTitle("y [\\circ]");
        }
        else
            if (!fTitle.Contains(";"))
                fHist.SetZTitle(fUseThreshold==kNoBound?"a.u.":"Counts");

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histogram. For details see the code or the class description
// 
Int_t MHCamEventRot::Fill(const MParContainer *par, const Stat_t w)
{
    const MCamEvent *evt = dynamic_cast<const MCamEvent*>(par);
    if (!evt)
    {
        *fLog << err << "MHCamEventRot::Fill: No container specified!" << endl;
        return kERROR;
    }

    // Get max radius...
    // const Double_t maxr = fGeom->GetMaxRadius()*fGeom->GetConvMm2Deg();

    // Get camera rotation angle
    Double_t rho = 0;
    if (fTime && fObservatory && fPointPos)
        rho = fPointPos->RotationAngle(*fObservatory, *fTime);
    //if (fPointPos)
    //    rho = fPointPos->RotationAngle(*fObservatory);

    // Get number of bins and bin-centers
    const Int_t nx = fHist.GetNbinsX();
    const Int_t ny = fHist.GetNbinsY();
    Axis_t cx[nx];
    Axis_t cy[ny];
    fHist.GetXaxis()->GetCenter(cx);
    fHist.GetYaxis()->GetCenter(cy);

    for (int ix=0; ix<nx; ix++)
    {
        for (int iy=0; iy<ny; iy++)
        {
            // check distance... to get a circle plot
            //if (TMath::Hypot(cx[ix], cy[iy])>maxr)
            //    continue;

            // rotate center of bin
            TVector2 v(cx[ix], cy[iy]);
            if (rho!=0)
                v=v.Rotate(rho);

            // FIXME: slow! Create Lookup table instead!
            const Int_t idx = fGeom->GetPixelIdxDeg(v);

            Double_t val;
            if (idx<0 || !evt->GetPixelContent(val, idx, *fGeom, fType))
                continue;

            // Fill histogram
            if (fUseThreshold!=kNoBound)
            {
                if ((val>fThreshold && fUseThreshold==kIsLowerBound) ||
                    (val<fThreshold && fUseThreshold==kIsUpperBound))
                    fHist.Fill(cx[ix], cy[iy]);
            }
            else
                fHist.Fill(cx[ix], cy[iy], val);
        }
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Get the MAstroCatalog corresponding to fRa, fDec. The limiting magnitude
// is set to 9, while the fov is equal to the current fov of the false
// source plot.
//
TObject *MHCamEventRot::GetCatalog()
{
    const Double_t maxr = 0.98*TMath::Abs(fHist.GetBinCenter(1));

    // Create catalog...
    MAstroCatalog stars;
    stars.SetLimMag(9);
    stars.SetGuiActive(kFALSE);
    stars.SetRadiusFOV(maxr);
    stars.SetRaDec(fRa*TMath::DegToRad()*15, fDec*TMath::DegToRad());
    stars.ReadBSC("bsc5.dat");

    TObject *o = (MAstroCatalog*)stars.Clone();
    o->SetBit(kCanDelete);

    return o;
}

// --------------------------------------------------------------------------
//
// Draw the histogram
//
void MHCamEventRot::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    // draw the 2D histogram Sigmabar versus Theta
    fHist.Draw(opt);
    TObject *catalog = GetCatalog();
    catalog->Draw("mirror same");
}
