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
!   Author(s): Thomas Bretz, 3/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MHFalseSource
//
// Create a false source plot. For the Binning in x,y the object MBinning
// "BinningFalseSource" is taken from the paremeter list.
//
// The binning in alpha is currently fixed as 18bins from 0 to 90deg.
//
// If MTime, MObservatory and MPointingPos is available in the paremeter
// list each image is derotated.
//
// MHFalseSource fills a 3D histogram with alpha distribution for
// each (derotated) x and y position.
//
// Only a radius of 1.2deg around the camera center is taken into account.
//
// The displayed histogram is the projection of alpha<fAlphaCut into
// the x,y plain and the projection of alpha>90-fAlphaCut
//
// By using the context menu (find it in a small region between the single
// pads) you can change the AlphaCut 'online'
//
// Each Z-Projection (Alpha-histogram) is scaled such, that the number
// of entries fits the maximum number of entries in all Z-Projections.
// This should correct for the different acceptance in the center
// and at the vorder of the camera. This, however, produces more noise
// at the border.
//
// Here is a slightly simplified version of the algorithm:
// ------------------------------------------------------
//    MHillas hil; // Taken as second argument in MFillH
//
//    MSrcPosCam src;
//    MHillasSrc hsrc;
//    hsrc.SetSrcPos(&src);
//
//    for (int ix=0; ix<nx; ix++)
//        for (int iy=0; iy<ny; iy++)
//        {
//            TVector2 v(cx[ix], cy[iy]); //cx center of x-bin ix
//            if (rho!=0)                 //cy center of y-bin iy
//                v=v.Rotate(rho);         //image rotation angle
//
//            src.SetXY(v);               //source position in the camera
//
//            if (!hsrc.Calc(hil))        //calc source dependant hillas
//                return;
//
//            //fill absolute alpha into histogram
//            const Double_t alpha = hsrc.GetAlpha();
//            fHist.Fill(cx[ix], cy[iy], TMath::Abs(alpha), w);
//        }
//    }
//
// Use MHFalse Source like this:
// -----------------------------
//    MFillH fill("MHFalseSource", "MHillas");
// or
//    MHFalseSource hist;
//    hist.SetAlphaCut(12.5);  // The default value
//    hist.SetBgmean(55);      // The default value
//    MFillH fill(&hist, "MHillas");
//
// To be implemented:
// ------------------
//  - a switch to use alpha or |alpha|
//  - taking the binning for alpha from the parlist (binning is
//    currently fixed)
//  - a possibility to change the fit-function (eg using a different TF1)
//  - a possibility to change the fit algorithm (eg which paremeters
//    are fixed at which time)
//  - use a different varaible than alpha
//  - a possiblity to change the algorithm which is used to calculate
//    alpha (or another parameter) is missing (this could be done using
//    a tasklist somewhere...)
//  - a more clever (and faster) algorithm to fill the histogram, eg by
//    calculating alpha once and fill the two coils around the mean
//  - more drawing options...
//  - Move Significance() to a more 'general' place and implement
//    also different algorithms like (Li/Ma)
//  - implement fit for best alpha distribution -- online (Paint)
//  - currently a constant pointing position is assumed in Fill()
//  - the center of rotation need not to be the camera center
//  - ERRORS on each alpha bin to estimate the chi^2 correctly!
//    (sqrt(N)/binwidth) needed for WOlfgangs proposed caluclation
//    of alpha(Li/Ma)
//  - use the g/h-separation filters from the tasklists ("Cut1") as filters
//    two
//
//////////////////////////////////////////////////////////////////////////////
#include "MHFalseSource.h"

#include <TF1.h>
#include <TF2.h>
#include <TH2.h>
#include <TLatex.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TRandom.h>
#include <TEllipse.h>
#include <TPaveText.h>
#include <TStopwatch.h>

#include "MGeomCam.h"
#include "MSrcPosCam.h"
#include "MHillas.h"
#include "MHillasSrc.h"
#include "MTime.h"
#include "MObservatory.h"
#include "MPointingPos.h"
#include "MAstroCatalog.h"
#include "MAstroSky2Local.h"
#include "MStatusDisplay.h"

#include "MMath.h"
#include "MAlphaFitter.h"

#include "MString.h"
#include "MBinning.h"
#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHFalseSource);

using namespace std;

//class MHillasExt;

// --------------------------------------------------------------------------
//
// Default Constructor
//
MHFalseSource::MHFalseSource(const char *name, const char *title)
    : fTime(0), fPointPos(0), fObservatory(0), fMm2Deg(-1), fAlphaCut(12.5),
    fBgMean(55), fMinDist(-1), fMaxDist(-1), fMinDW(-1), fMaxDW(-1),
    fHistOff(0)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHFalseSource";
    fTitle = title ? title : "3D-plot of Alpha vs x, y";

    fHist.SetDirectory(NULL);

    fHist.SetName("Alpha");
    fHist.SetTitle("3D-plot of Alpha vs x, y");
    fHist.SetXTitle("x [\\circ]");
    fHist.SetYTitle("y [\\circ]");
    fHist.SetZTitle("\\alpha [\\circ]");
}

void MHFalseSource::MakeSymmetric(TH1 *h)
{
    h->SetMinimum();
    h->SetMaximum();

    const Float_t min = TMath::Abs(h->GetMinimum());
    const Float_t max = TMath::Abs(h->GetMaximum());

    const Float_t absmax = TMath::Max(min, max)*1.002;

    h->SetMaximum( absmax);
    h->SetMinimum(-absmax);
}

// --------------------------------------------------------------------------
//
// Set the alpha cut (|alpha|<fAlphaCut) which is use to estimate the
// number of excess events
//
void MHFalseSource::SetAlphaCut(Float_t alpha)
{
    if (alpha<0)
        *fLog << warn << "Alpha<0... taking absolute value." << endl;

    fAlphaCut = TMath::Abs(alpha);

    Modified();
}

// --------------------------------------------------------------------------
//
// Set mean alpha around which the off sample is determined
// (fBgMean-fAlphaCut/2<|fAlpha|<fBgMean+fAlphaCut/2) which is use
// to estimate the number of off events
//
void MHFalseSource::SetBgMean(Float_t alpha)
{
    if (alpha<0)
        *fLog << warn << "Alpha<0... taking absolute value." << endl;

    fBgMean = TMath::Abs(alpha);

    Modified();
}

// --------------------------------------------------------------------------
//
// Set binnings (takes BinningFalseSource) and prepare filling of the
// histogram.
//
// Also search for MTime, MObservatory and MPointingPos
//
Bool_t MHFalseSource::SetupFill(const MParList *plist)
{
    const MGeomCam *geom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!geom)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }
    fMm2Deg = geom->GetConvMm2Deg();

    const TString off(MString::Format("%sOff", fName.Data()));
    if (fName!=off && fHistOff==NULL)
    {
        const TString desc(MString::Format("%s [%s] found... using ", off.Data(), ClassName()));
        MHFalseSource *hoff = (MHFalseSource*)plist->FindObject(off, ClassName());
        if (!hoff)
            *fLog << inf << "No " << desc << "current data only!" << endl;
        else
        {
            *fLog << inf << desc << "on-off mode!" << endl;
            SetOffData(*hoff);
        }
    }

    if (fHistOff)
        MH::CopyBinning(*fHistOff, fHist);
    else
    {
        MBinning binsa(18, 0, 90);
        binsa.SetEdges(*plist, "BinningAlpha");

        const MBinning *bins = (MBinning*)plist->FindObject("BinningFalseSource");
        if (!bins || bins->IsDefault())
        {
            const Float_t r = (geom ? geom->GetMaxRadius()/3 : 200)*fMm2Deg;

            const MBinning b(20, -r, r);
            SetBinning(fHist, b, b, binsa);
        }
        else
            SetBinning(fHist, *bins, *bins, binsa);
    }

    fPointPos = (MPointingPos*)plist->FindObject(AddSerialNumber("MPointingPos"));
    if (!fPointPos)
        *fLog << warn << "MPointingPos not found... no derotation." << endl;

    fTime = (MTime*)plist->FindObject(AddSerialNumber("MTime"));
    if (!fTime)
        *fLog << warn << "MTime not found... no derotation." << endl;

    fSrcPos = (MSrcPosCam*)plist->FindObject(AddSerialNumber("MSrcPosCam"));
    if (!fSrcPos)
        *fLog << warn << "MSrcPosCam not found... no translation." << endl;

    fObservatory = (MObservatory*)plist->FindObject(AddSerialNumber("MObservatory"));
    if (!fObservatory)
        *fLog << warn << "MObservatory not found... no derotation." << endl;

    MPointingPos *point = (MPointingPos*)plist->FindObject("MSourcePos", "MPointingPos");
    if (!point)
        point = fPointPos;

    // FIXME: Because the pointing position could change we must check
    // for the current pointing position and add a offset in the
    // Fill function!
    fRa  = point ? point->GetRa()  :  0;
    fDec = point ? point->GetDec() : 90;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histogram. For details see the code or the class description
// 
Int_t MHFalseSource::Fill(const MParContainer *par, const Stat_t w)
{
    const MHillas *hil = dynamic_cast<const MHillas*>(par);
    if (!hil)
    {
        *fLog << err << "MHFalseSource::Fill: No container specified!" << endl;
        return kERROR;
    }

    // Get max radius...
    const Double_t maxr = 0.98*TMath::Abs(fHist.GetBinCenter(1));

    // Get camera rotation angle
    Double_t rho = 0;
    if (fTime && fObservatory && fPointPos)
        rho = fPointPos->RotationAngle(*fObservatory, *fTime);
    //if (fPointPos)
    //    rho = fPointPos->RotationAngle(*fObservatory);

    // Create necessary containers for calculation
    MSrcPosCam src;
    MHillasSrc hsrc;
    hsrc.SetSrcPos(&src);

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
            if (TMath::Hypot(cx[ix], cy[iy])>maxr)
                continue;

            // rotate center of bin
            // precalculation of sin/cos doesn't accelerate
            TVector2 v(cx[ix], cy[iy]);
            if (rho!=0)
                v=v.Rotate(rho);

            // convert degrees to millimeters
            v *= 1./fMm2Deg;

            if (fSrcPos)
                v += fSrcPos->GetXY();

            src.SetXY(v);

            // Source dependant hillas parameters
            if (hsrc.Calc(*hil/*, ext*/)>0)
            {
                *fLog << warn << "Calculation of MHillasSrc failed for x=" << cx[ix] << " y=" << cy[iy] << endl;
                return kFALSE;
            }

            // FIXME: This should be replaced by an external MFilter
            //        and/or MTaskList
            // Source dependant distance cut
            if (fMinDist>0 && hsrc.GetDist()*fMm2Deg<fMinDist)
                continue;
            if (fMaxDist>0 && hsrc.GetDist()*fMm2Deg>fMaxDist)
                continue;

            if (fMaxDW>0 && hsrc.GetDist()>fMaxDW*hil->GetWidth())
                continue;
            if (fMinDW<0 && hsrc.GetDist()<fMinDW*hil->GetWidth())
                continue;

            // Fill histogram
            const Double_t alpha = hsrc.GetAlpha();
            fHist.Fill(cx[ix], cy[iy], TMath::Abs(alpha), w);
        }
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Create projection for off data, taking sum of bin contents of
// range (fBgMean-fAlphaCut/2, fBgMean+fAlphaCut) Making sure to take
// the same number of bins than for on-data
//
void MHFalseSource::ProjectOff(const TH3D &src, TH2D *h2, TH2D *hall)
{
    TAxis &axe = *src.GetZaxis();

    // Find range to cut (left edge and width)
    const Int_t f = axe.FindBin(fBgMean-fAlphaCut/2);
    const Int_t l = axe.FindBin(fAlphaCut)+f-1;

    axe.SetRange(f, l);
    const Float_t cut1 = axe.GetBinLowEdge(f);
    const Float_t cut2 = axe.GetBinUpEdge(l);
    h2->SetTitle(MString::Format("Distribution of %.1f\\circ<|\\alpha|<%.1f\\circ in x,y", cut1, cut2));

    // Get projection for range
    TH2D *p = (TH2D*)src.Project3D("yx_off_NULL");

    // Reset range
    axe.SetRange(0,9999);

//#if ROOT_VERSION_CODE < ROOT_VERSION(4,02,00)
    // Move contents from projection to h2
    h2->Reset();
    h2->Add(p, hall->GetMaximum());
    h2->Divide(hall);

    // Delete p
    delete p;
/*#else
    p->Scale(all->GetMaximum());
    p->Divide(all);
#endif*/

    // Set Minimum as minimum value Greater Than 0
    h2->SetMinimum(GetMinimumGT(*h2));
}

// --------------------------------------------------------------------------
//
// Create projection for on data, taking sum of bin contents of
// range (0, fAlphaCut)
//
void MHFalseSource::ProjectOn(const TH3D &src, TH2D *h3, TH2D *hall)
{
    TAxis &axe = *src.GetZaxis();

    // Find range to cut
    axe.SetRangeUser(0, fAlphaCut);
    const Float_t cut = axe.GetBinUpEdge(axe.GetLast());
    h3->SetTitle(MString::Format("Distribution of |\\alpha|<%.1f\\circ in x,y", cut));

    // Get projection for range
    TH2D *p = (TH2D*)src.Project3D("yx_on_NULL");

    // Reset range
    axe.SetRange(0,9999);

//#if ROOT_VERSION_CODE < ROOT_VERSION(4,02,00)
    // Move contents from projection to h3
    h3->Reset();
    h3->Add(p, hall->GetMaximum());
    h3->Divide(hall);

    // Delete p
    delete p;
/*#else
    p->Scale(all->GetMaximum());
    p->Divide(all);
#endif*/

    // Set Minimum as minimum value Greater Than 0
    h3->SetMinimum(GetMinimumGT(*h3));
}

// --------------------------------------------------------------------------
//
// Create projection for all data, taking sum of bin contents of
// range (0, 90) - corresponding to the number of entries in this slice.
//
void MHFalseSource::ProjectAll(TH2D *h3)
{
    h3->SetTitle("Number of entries");

    // Get projection for range
#if ROOT_VERSION_CODE < ROOT_VERSION(4,02,00)
    TH2D *p = (TH2D*)fHist.Project3D("yx_all");

    // Move contents from projection to h3
    h3->Reset();
    h3->Add(p);
    delete p;
#else
    fHist.Project3D("yx_all");
#endif

    // Set Minimum as minimum value Greater Than 0
    h3->SetMinimum(GetMinimumGT(*h3));
}

void MHFalseSource::ProjectOnOff(TH2D *h2, TH2D *h0)
{
    ProjectOn(*fHistOff, h2, h0);

    TH2D h;
    MH::SetBinning(h, *h2);

    // Divide by number of entries in off region (of off-data)
    ProjectOff(*fHistOff, &h, h0);
    h2->Divide(&h);

    // Multiply by number of entries in off region (of on-data)
    ProjectOff(fHist, &h, h0);
    h2->Multiply(&h);

    // Recalculate Minimum
    h2->SetMinimum(GetMinimumGT(*h2));
}

// --------------------------------------------------------------------------
//
// Update the projections and paint them
//
void MHFalseSource::Paint(Option_t *opt)
{
    // Set pretty color palette
    gStyle->SetPalette(1, 0);

    TVirtualPad *padsave = gPad;

    TH1D* h1;
    TH2D* h0;
    TH2D* h2;
    TH2D* h3;
    TH2D* h4;
    TH2D* h5;

    // Update projection of all-events
    padsave->GetPad(2)->cd(3);
    if ((h0 = (TH2D*)gPad->FindObject("Alpha_yx_all")))
        ProjectAll(h0);

    // Update projection of on-events
    padsave->GetPad(1)->cd(1);
    if ((h3 = (TH2D*)gPad->FindObject("Alpha_yx_on")))
        ProjectOn(fHist, h3, h0);

    // Update projection of off-events
    padsave->GetPad(1)->cd(3);
    if ((h2 = (TH2D*)gPad->FindObject("Alpha_yx_off")))
    {
        if (!fHistOff)
            ProjectOff(fHist, h2, h0);
        else
            ProjectOnOff(h2, h0);
    }

    padsave->GetPad(2)->cd(2);
    if ((h5 = (TH2D*)gPad->FindObject("Alpha_yx_diff")))
    {
        h5->Add(h2, h3, -1);
        MakeSymmetric(h5);
    }

    // Update projection of significance
    padsave->GetPad(1)->cd(2);
    if (h2 && h3 && (h4 = (TH2D*)gPad->FindObject("Alpha_yx_sig")))
    {
        const Int_t nx = h4->GetXaxis()->GetNbins();
        const Int_t ny = h4->GetYaxis()->GetNbins();

        Int_t maxx=nx/2;
        Int_t maxy=ny/2;

        Int_t max = h4->GetBin(nx, ny);

        h4->SetEntries(0);
        for (int ix=1; ix<=nx; ix++)
            for (int iy=1; iy<=ny; iy++)
            {
                const Int_t n = h4->GetBin(ix, iy);

                const Double_t s = h3->GetBinContent(n);
                const Double_t b = h2->GetBinContent(n);

                const Double_t sig = MMath::SignificanceLiMaSigned(s, b);

                h4->SetBinContent(n, sig);

                if (sig>h4->GetBinContent(max) && sig>0/* && (ix-nx/2)*(ix-nx/2)+(iy-ny/2)*(iy-ny/2)<nr*nr/9*/)
                {
                    max = n;
                    maxx=ix;
                    maxy=iy;
                }
            }

        MakeSymmetric(h4);

        // Update projection of 'the best alpha-plot'
        padsave->GetPad(2)->cd(1);
        if ((h1 = (TH1D*)gPad->FindObject("Alpha_z")) && max>0)
        {
            const Double_t x = h4->GetXaxis()->GetBinCenter(maxx);
            const Double_t y = h4->GetYaxis()->GetBinCenter(maxy);
            const Double_t s = h4->GetBinContent(max);

            // This is because a 3D histogram x and y are vice versa
            // Than for their projections
            TH1 *h = fHist.ProjectionZ("Alpha_z", maxx, maxx, maxy, maxy);
            h->SetTitle(MString::Format("Distribution of \\alpha for x=%.2f y=%.2f (S_{max}=%.1f\\sigma)", x, y, s));

            TH1D *ho=0;
            if ((ho = (TH1D*)gPad->FindObject("AlphaOff_z")))
            {
                fHistOff->ProjectionZ("AlphaOff_z", maxx, maxx, maxy, maxy);

                /* ============= local scaling ================ */
                const Int_t f = ho->GetXaxis()->FindFixBin(fBgMean-1.5*fAlphaCut);
                const Int_t l = ho->GetXaxis()->FindFixBin(fAlphaCut*3)+f-1;
                ho->Scale(h1->Integral(f, l)/ho->Integral(f, l));


                //h0->Scale(h1->GetEntries()/h0->GetEntries());

                /* ============= global scaling ================
                const Int_t f = fHistOff->GetZaxis()->FindFixBin(fBgMean-1.5*fAlphaCut);
                const Int_t l = fHistOff->GetZaxis()->FindFixBin(fAlphaCut*3)+f-1;

                Double_t c0 = fHist.Integral(0, 9999, 0, 9999, f, l);
                Double_t c1 = fHistOff->Integral(0, 9999, 0, 9999, f, l);

                h0->Scale(c0/c1);
                */
            }
        }
    }

    gPad = padsave;
}

// --------------------------------------------------------------------------
//
// Get the MAstroCatalog corresponding to fRa, fDec. The limiting magnitude
// is set to 9, while the fov is equal to the current fov of the false
// source plot.
//
TObject *MHFalseSource::GetCatalog() const
{
    const Double_t maxr = TMath::Abs(fHist.GetBinLowEdge(1))*TMath::Sqrt(2);

    // Create catalog...
    MAstroCatalog *stars = new MAstroCatalog;
    stars->SetMarkerColor(kWhite);
    stars->SetLimMag(9);
    stars->SetGuiActive(kFALSE);
    stars->SetRadiusFOV(maxr);
    stars->SetRaDec(fRa*TMath::DegToRad()*15, fDec*TMath::DegToRad());
    stars->ReadBSC("bsc5.dat");

    stars->SetBit(kCanDelete);
    return stars;
}

// --------------------------------------------------------------------------
//
// Draw the histogram
//
void MHFalseSource::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    pad->Divide(1, 2, 1e-10, 0.03);

//    TObject *catalog = GetCatalog();

    // Initialize upper part
    pad->cd(1);
    // Make sure that the catalog is deleted only once
    // Normally this is not done by root, because it is not necessary...
    // but in this case it is necessary, because the catalog is
    // deleted by the first pad and the second one tries to do the same!
//    gROOT->GetListOfCleanups()->Add(gPad);
    gPad->SetBorderMode(0);
    gPad->Divide(3, 1);

    // PAD #1
    pad->GetPad(1)->cd(1);
    gPad->SetBorderMode(0);
    fHist.GetZaxis()->SetRangeUser(0,fAlphaCut);
    TH1 *h3 = fHist.Project3D("yx_on");
    fHist.GetZaxis()->SetRange(0,9999);
    h3->SetDirectory(NULL);
    h3->SetXTitle(fHist.GetXaxis()->GetTitle());
    h3->SetYTitle(fHist.GetYaxis()->GetTitle());
    h3->Draw("colz");
    h3->SetBit(kCanDelete);
//    catalog->Draw("mirror same *");

    // PAD #2
    pad->GetPad(1)->cd(2);
    gPad->SetBorderMode(0);
    fHist.GetZaxis()->SetRange(0,0);
    TH1 *h4 = fHist.Project3D("yx_sig"); // Do this to get the correct binning....
    fHist.GetZaxis()->SetRange(0,9999);
    h4->SetTitle("Significance");
    h4->SetDirectory(NULL);
    h4->SetXTitle(fHist.GetXaxis()->GetTitle());
    h4->SetYTitle(fHist.GetYaxis()->GetTitle());
    h4->Draw("colz");
    h4->SetBit(kCanDelete);
//    catalog->Draw("mirror same *");

    // PAD #3
    pad->GetPad(1)->cd(3);
    gPad->SetBorderMode(0);
    TH1 *h2 = 0;
    if (fHistOff)
    {
        fHistOff->GetZaxis()->SetRangeUser(0,fAlphaCut);
        h2 = fHistOff->Project3D("yx_off");
        fHistOff->GetZaxis()->SetRange(0,9999);
    }
    else
    {
        fHist.GetZaxis()->SetRangeUser(fBgMean-fAlphaCut/2, fBgMean+fAlphaCut/2);
        h2 = fHist.Project3D("yx_off");
        fHist.GetZaxis()->SetRange(0,9999);
    }
    h2->SetDirectory(NULL);
    h2->SetXTitle(fHist.GetXaxis()->GetTitle());
    h2->SetYTitle(fHist.GetYaxis()->GetTitle());
    h2->Draw("colz");
    h2->SetBit(kCanDelete);
//    catalog->Draw("mirror same *");

    // Initialize lower part
    pad->cd(2);
    // Make sure that the catalog is deleted only once
//    gROOT->GetListOfCleanups()->Add(gPad);
    gPad->SetBorderMode(0);
    gPad->Divide(3, 1);

    // PAD #4
    pad->GetPad(2)->cd(1);
    gPad->SetBorderMode(0);
    TH1 *h1 = fHist.ProjectionZ("Alpha_z");
    h1->SetDirectory(NULL);
    h1->SetTitle("Distribution of \\alpha");
    h1->SetXTitle(fHist.GetZaxis()->GetTitle());
    h1->SetYTitle("Counts");
    h1->Draw();
    h1->SetBit(kCanDelete);
    if (fHistOff)
    {
        h1->SetLineColor(kGreen);

        h1 = fHistOff->ProjectionZ("AlphaOff_z");
        h1->SetDirectory(NULL);
        h1->SetTitle("Distribution of \\alpha");
        h1->SetXTitle(fHistOff->GetZaxis()->GetTitle());
        h1->SetYTitle("Counts");
        h1->Draw("same");
        h1->SetBit(kCanDelete);
        h1->SetLineColor(kRed);
    }

    // PAD #5
    pad->GetPad(2)->cd(2);
    gPad->SetBorderMode(0);
    TH1 *h5 = (TH1*)h3->Clone("Alpha_yx_diff");
    h5->Add(h2, -1);
    h5->SetTitle("Difference of on- and off-distribution");
    h5->SetDirectory(NULL);
    h5->Draw("colz");
    h5->SetBit(kCanDelete);
//    catalog->Draw("mirror same *");

    // PAD #6
    pad->GetPad(2)->cd(3);
    gPad->SetBorderMode(0);
    TH1 *h0 = fHist.Project3D("yx_all");
    h0->SetDirectory(NULL);
    h0->SetXTitle(fHist.GetXaxis()->GetTitle());
    h0->SetYTitle(fHist.GetYaxis()->GetTitle());
    h0->Draw("colz");
    h0->SetBit(kCanDelete);
//    catalog->Draw("mirror same *");
}

// --------------------------------------------------------------------------
//
// Everything which is in the main pad belongs to this class!
//
Int_t MHFalseSource::DistancetoPrimitive(Int_t px, Int_t py)
{
    return 0;
}

// --------------------------------------------------------------------------
//
// Set all sub-pads to Modified()
//
void MHFalseSource::Modified()
{
    if (!gPad)
        return;

    TVirtualPad *padsave = gPad;
    padsave->Modified();
    padsave->GetPad(1)->cd(1);
    gPad->Modified();
    padsave->GetPad(1)->cd(3);
    gPad->Modified();
    padsave->GetPad(2)->cd(1);
    gPad->Modified();
    padsave->GetPad(2)->cd(2);
    gPad->Modified();
    padsave->GetPad(2)->cd(3);
    gPad->Modified();
    gPad->cd();
}

// --------------------------------------------------------------------------
//
// The following resources are available:
//
//    MHFalseSource.DistMin: 0.5
//    MHFalseSource.DistMax: 1.4
//    MHFalseSource.DWMin:   0.1
//    MHFalseSource.DWMax:   0.3
//
Int_t MHFalseSource::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "DistMin", print))
    {
        rc = kTRUE;
        SetMinDist(GetEnvValue(env, prefix, "DistMin", fMinDist));
    }
    if (IsEnvDefined(env, prefix, "DistMax", print))
    {
        rc = kTRUE;
        SetMaxDist(GetEnvValue(env, prefix, "DistMax", fMaxDist));
    }

    if (IsEnvDefined(env, prefix, "DWMin", print))
    {
        rc = kTRUE;
        SetMinDW(GetEnvValue(env, prefix, "DWMin", fMinDist));
    }
    if (IsEnvDefined(env, prefix, "DWMax", print))
    {
        rc = kTRUE;
        SetMaxDW(GetEnvValue(env, prefix, "DWMax", fMaxDist));
    }

    return rc;
}

static Double_t FcnGauss2d(Double_t *x, Double_t *par)
{
    TVector2 v = TVector2(x[0], x[1]).Rotate(par[5]*TMath::DegToRad());

    const Double_t g0 = TMath::Gaus(v.X(), par[1], par[2]);
    const Double_t g1 = TMath::Gaus(v.Y(), par[3], par[4]);

    //Gaus(Double_t x, Double_t mean=0, Double_t sigma=1, Bool_t norm=kFALSE);
    return par[0]*g0*g1;
}

// --------------------------------------------------------------------------
//
// This is a preliminary implementation of a alpha-fit procedure for
// all possible source positions. It will be moved into its own
// more powerfull class soon.
//
// The fit function is "gaus(0)+pol2(3)" which is equivalent to:
//   [0]*exp(-0.5*((x-[1])/[2])^2) + [3] + [4]*x + [5]*x^2
// or
//   A*exp(-0.5*((x-mu)/sigma)^2) + a + b*x + c*x^2
//
// Parameter [1] is fixed to 0 while the alpha peak should be
// symmetric around alpha=0.
//
// Parameter [4] is fixed to 0 because the first derivative at
// alpha=0 should be 0, too.
//
// In a first step the background is fitted between bgmin and bgmax,
// while the parameters [0]=0 and [2]=1 are fixed.
//
// In a second step the signal region (alpha<sigmax) is fittet using
// the whole function with parameters [1], [3], [4] and [5] fixed.
//
// The number of excess and background events are calculated as
//   s = int(0, sigint, gaus(0)+pol2(3))
//   b = int(0, sigint,         pol2(3))
//
// The Significance is calculated using the Significance() member
// function.
//
void MHFalseSource::FitSignificance(Float_t sigint, Float_t sigmax, Float_t bgmin, Float_t bgmax, Byte_t polynom)
{
//    TObject *catalog = GetCatalog();

    TH1D h0a("A",          "", 50,   0, 4000);
    TH1D h4a("chisq1",     "", 50,   0,   35);
    //TH1D h5a("prob1",      "", 50,   0,  1.1);
    TH1D h6("signifcance", "", 50, -20,   20);

    TH1D h1("mu",    "Parameter \\mu",    50,   -1,    1);
    TH1D h2("sigma", "Parameter \\sigma", 50,    0,   90);
    TH1D h3("b",     "Parameter b",       50, -0.1,  0.1);

    TH1D h0b("a",         "Parameter a (red), A (blue)", 50, 0, 4000);
    TH1D h4b("\\chi^{2}", "\\chi^{2} (red, green) / significance (black)", 50, 0, 35);
    //TH1D h5b("prob",      "Fit probability: Bg(red), F(blue)", 50, 0, 1.1);

    h0a.SetLineColor(kBlue);
    h4a.SetLineColor(kBlue);
    //h5a.SetLineColor(kBlue);
    h0b.SetLineColor(kRed);
    h4b.SetLineColor(kRed);
    //h5b.SetLineColor(kRed);

    TH1 *hist  = fHist.Project3D("yx_fit");
    hist->SetDirectory(0);
    TH1 *hists = fHist.Project3D("yx_fit");
    hists->SetDirectory(0);
    TH1 *histb = fHist.Project3D("yx_fit");
    histb->SetDirectory(0);
    hist->Reset();
    hists->Reset();
    histb->Reset();
    hist->SetNameTitle("Significance",
                       MString::Format("Fit Region: Signal<%.1f\\circ, %.1f\\circ<Bg<%.1f\\circ",
                                       sigmax, bgmin, bgmax));
    hists->SetName("Excess");
    histb->SetName("Background");
    hist->SetXTitle(fHist.GetXaxis()->GetTitle());
    hists->SetXTitle(fHist.GetXaxis()->GetTitle());
    histb->SetXTitle(fHist.GetXaxis()->GetTitle());
    hist->SetYTitle(fHist.GetYaxis()->GetTitle());
    hists->SetYTitle(fHist.GetYaxis()->GetTitle());
    histb->SetYTitle(fHist.GetYaxis()->GetTitle());

    const Double_t w = fHist.GetZaxis()->GetBinWidth(1);

    TArrayD maxpar;

    /*  func.SetParName(0, "A");
     *  func.SetParName(1, "mu");
     *  func.SetParName(2, "sigma");
    */

    const Int_t nx = hist->GetXaxis()->GetNbins();
    const Int_t ny = hist->GetYaxis()->GetNbins();
    //const Int_t nr = nx*nx+ny*ny;

    Double_t maxalpha0=0;
    Double_t maxs=3;

    Int_t maxx=0;
    Int_t maxy=0;

    TStopwatch clk;
    clk.Start();

    *fLog << inf;
    *fLog << "Signal fit:     alpha < " << sigmax << endl;
    *fLog << "Integration:    alpha < " << sigint << endl;
    *fLog << "Background fit: " << bgmin << " < alpha < " << bgmax << endl;
    *fLog << "Polynom order:  " << (int)polynom << endl;
    *fLog << "Fitting False Source Plot..." << flush;

    TH1 *h0 = fHist.Project3D("yx_entries");
    Float_t entries = h0->GetMaximum();
    delete h0;

    MAlphaFitter fit;
    fit.SetSignalIntegralMax(sigint);
    fit.SetSignalFitMax(sigmax);
    fit.SetBackgroundFitMin(bgmin);
    fit.SetBackgroundFitMax(bgmax);
    fit.SetPolynomOrder(polynom);

    TH1D *h=0, *hoff=0;
    Double_t scale = 1;
    for (int ix=1; ix<=nx; ix++)
        for (int iy=1; iy<=ny; iy++)
        {
            // This is because a 3D histogram x and y are vice versa
            // Than for their projections
            h = fHist.ProjectionZ("AlphaFit", ix, ix, iy, iy);

            if (h->GetEntries()==0)
                continue;

            // This is a quick hack to correct for the lower acceptance at
            // the edge of the camera
            h->Scale(entries/h->GetEntries());

            if (fHistOff)
            {
                hoff = fHistOff->ProjectionZ("AlphaFitOff", ix, ix, iy, iy);
                // This is a quick hack to correct for the lower acceptance at
                // the edge of the camera
                hoff->Scale(entries/h->GetEntries());
                scale = fit.Scale(*hoff, *h);
            }

            if (!fit.Fit(*h, hoff, scale))
                continue;

            const Double_t alpha0 = h->GetBinContent(1);
            if (alpha0>maxalpha0)
                maxalpha0=alpha0;
 
            // Fill results into some histograms
            h0a.Fill(fit.GetGausA());        // gaus-A
            h0b.Fill(fit.GetCoefficient(3)); // 3
            h1.Fill(fit.GetGausMu());        // mu
            h2.Fill(fit.GetGausSigma());     // sigma-gaus
            if (polynom>1 && !fHistOff)
                h3.Fill(fit.GetCoefficient(5));
            h4b.Fill(fit.GetChiSqSignal());

            const Double_t sig = fit.GetSignificance();
            const Double_t b   = fit.GetEventsBackground();
            const Double_t s   = fit.GetEventsSignal();

            const Int_t n = hist->GetBin(ix, iy);
            hists->SetBinContent(n, s-b);
            histb->SetBinContent(n, b);

            hist->SetBinContent(n, sig);
            if (sig!=0)
                h6.Fill(sig);

            if (sig>maxs)
            {
                maxs = sig;
                maxx = ix;
                maxy = iy;
                maxpar = fit.GetCoefficients();
            }
        }

    *fLog << "Done." << endl;

    h0a.GetXaxis()->SetRangeUser(0, maxalpha0*1.5);
    h0b.GetXaxis()->SetRangeUser(0, maxalpha0*1.5);

    hists->SetTitle(MString::Format("Excess events for \\alpha<%.0f\\circ (N_{max}=%d)", sigint, (int)hists->GetMaximum()));
    histb->SetTitle(MString::Format("Background events for \\alpha<%.0f\\circ", sigint));

    //hists->SetMinimum(GetMinimumGT(*hists));
    histb->SetMinimum(GetMinimumGT(*histb));

    MakeSymmetric(hists);
    MakeSymmetric(hist);

    clk.Stop();
    clk.Print("m");

    TCanvas *c=new TCanvas;

    gStyle->SetPalette(1, 0);

    c->Divide(3,2, 1e-10, 1e-10);
    c->cd(1);
    gPad->SetBorderMode(0);
    hists->Draw("colz");
    hists->SetBit(kCanDelete);
//    catalog->Draw("mirror same *");
    c->cd(2);
    gPad->SetBorderMode(0);
    hist->Draw("colz");
    hist->SetBit(kCanDelete);


    const Double_t maxr = 0.9*TMath::Abs(fHist.GetBinCenter(1));
    TF2 f2d("Gaus-2D", FcnGauss2d, -maxr, maxr, -maxr, maxr, 6);
    f2d.SetLineWidth(1);
    f2d.SetParName(0, "Max   sigma");
    f2d.SetParName(1, "Mean_1  deg");
    f2d.SetParName(2, "Sigma_1 deg");
    f2d.SetParName(3, "Mean_2  deg");
    f2d.SetParName(4, "Sigma_2 deg");
    f2d.SetParName(5, "Phi     deg");
    f2d.SetParLimits(1, -maxr/2, maxr/2); // mu_1
    f2d.SetParLimits(3, -maxr/2, maxr/2); // mu_2
    f2d.SetParLimits(2, 0, maxr);         // sigma_1
    f2d.SetParLimits(4, 0, maxr);         // sigma_2
    f2d.SetParLimits(5, 0, 45);           // phi
    f2d.SetParameter(0, maxs);            // A
    f2d.SetParameter(1, hist->GetXaxis()->GetBinCenter(maxx)); // mu_1
    f2d.SetParameter(2, 0.1);             // sigma_1
    f2d.SetParameter(3, hist->GetYaxis()->GetBinCenter(maxy)); // mu_2
    f2d.SetParameter(4, 0.1);             // sigma_2
    f2d.FixParameter(5, 0);               // phi
    hist->Fit(&f2d, "NI0R");
    f2d.DrawCopy("cont2same");


//    catalog->Draw("mirror same *");
    c->cd(3);
    gPad->SetBorderMode(0);
    histb->Draw("colz");
    histb->SetBit(kCanDelete);
//    catalog->Draw("mirror same *");
    c->cd(4);
    gPad->Divide(1,3, 0, 0);
    TVirtualPad *p=gPad;
    p->SetBorderMode(0);
    p->cd(1);
    gPad->SetBorderMode(0);
    h0b.DrawCopy();
    h0a.DrawCopy("same");
    p->cd(2);
    gPad->SetBorderMode(0);
    h3.DrawCopy();
    p->cd(3);
    gPad->SetBorderMode(0);
    h2.DrawCopy();
    c->cd(6);
    gPad->Divide(1,2, 0, 0);
    TVirtualPad *q=gPad;
    q->SetBorderMode(0);
    q->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetBorderMode(0);
    h4b.DrawCopy();
    h4a.DrawCopy("same");
    h6.DrawCopy("same");
    q->cd(2);
    gPad->SetBorderMode(0);
    //h5b.DrawCopy();
    //h5a.DrawCopy("same");
    c->cd(5);
    gPad->SetBorderMode(0);
    if (maxx>0 && maxy>0)
    {
        const TString title = MString::Format(" \\alpha for x=%.2f y=%.2f (S_{max}=%.1f\\sigma) ",
                                              hist->GetXaxis()->GetBinCenter(maxx),
                                              hist->GetYaxis()->GetBinCenter(maxy), maxs);

        h = fHist.ProjectionZ("AlphaFit", maxx, maxx, maxy, maxy);
        h->Scale(entries/h->GetEntries());

        h->SetDirectory(NULL);
        h->SetNameTitle("Result \\alpha", title);
        h->SetBit(kCanDelete);
        h->SetXTitle("\\alpha [\\circ]");
        h->SetYTitle("Counts");
        h->Draw();

        if (fHistOff)
        {
            h->SetLineColor(kGreen);

            TH1D *hof=fHistOff->ProjectionZ("AlphaFitOff", maxx, maxx, maxy, maxy);
            hof->Scale(entries/hof->GetEntries());

            fit.Scale(*(TH1D*)hof, *(TH1D*)h);

            hof->SetLineColor(kRed);
            hof->SetDirectory(NULL);
            hof->SetNameTitle("Result \\alpha", title);
            hof->SetBit(kCanDelete);
            hof->SetXTitle("\\alpha [\\circ]");
            hof->SetYTitle("Counts");
            hof->Draw("same");

            TH1D *diff = (TH1D*)h->Clone("AlphaFitOnOff");
            diff->Add(hof, -1);
            diff->SetLineColor(kBlue);
            diff->SetNameTitle("Result On-Off \\alpha", title);
            diff->SetBit(kCanDelete);
            diff->SetXTitle("\\alpha [\\circ]");
            diff->SetYTitle("Counts");
            diff->Draw("same");

            h->SetMinimum(diff->GetMinimum()<0 ? diff->GetMinimum()*1.2 : 0);

            TLine l;
            l.DrawLine(0, 0, 90, 0);
        }

        TF1 f1("f1", MString::Format("gaus(0) + pol%d(3)", fHistOff ? 0 : polynom).Data(), 0, 90);
        TF1 f2("f2", MString::Format("gaus(0) + pol%d(3)", fHistOff ? 0 : polynom).Data(), 0, 90);
        f1.SetParameters(maxpar.GetArray());
        f2.SetParameters(maxpar.GetArray());
        f2.FixParameter(0, 0);
        f2.FixParameter(1, 0);
        f2.FixParameter(2, 1);
        f1.SetLineColor(kGreen);
        f2.SetLineColor(kRed);

        f2.DrawCopy("same");
        f1.DrawCopy("same");

        TPaveText *leg = new TPaveText(0.35, 0.10, 0.90, 0.35, "brNDC");
        leg->SetBorderSize(1);
        leg->SetTextSize(0.04);
        leg->AddText(0.5, 0.82, MString::Format("A * exp(-(\\frac{x-\\mu}{\\sigma})^{2}/2) + pol%d", polynom))->SetTextAlign(22);
        //leg->AddText(0.5, 0.82, "A * exp(-(\\frac{x-\\mu}{\\sigma})^{2}/2) + b*x^{2} + a")->SetTextAlign(22);
        leg->AddLine(0, 0.65, 0, 0.65);
        leg->AddText(0.06, 0.54, MString::Format("A=%.2f", maxpar[0]))->SetTextAlign(11);
        leg->AddText(0.06, 0.34, MString::Format("\\sigma=%.2f", maxpar[2]))->SetTextAlign(11);
        leg->AddText(0.06, 0.14, MString::Format("\\mu=%.2f (fix)", maxpar[1]))->SetTextAlign(11);
        leg->AddText(0.60, 0.54, MString::Format("a=%.2f", maxpar[3]))->SetTextAlign(11);
        leg->AddText(0.60, 0.34, MString::Format("b=%.2f (fix)", maxpar[4]))->SetTextAlign(11);
        if (polynom>1)
            leg->AddText(0.60, 0.14, MString::Format("c=%.2f", !fHistOff?maxpar[5]:0))->SetTextAlign(11);
        leg->SetBit(kCanDelete);
        leg->Draw();

        q->cd(2);

        TGraph *g = new TGraph;
        g->SetPoint(0, 0, 0);

        Int_t max=0;
        Float_t maxsig=0;
        for (int i=1; i<89; i++)
        {
            const Double_t s = f1.Integral(0, (float)i)/w;
            const Double_t b = f2.Integral(0, (float)i)/w;

            const Double_t sig = MMath::SignificanceLiMaSigned(s, b);

            g->SetPoint(g->GetN(), i, sig);

            if (sig>maxsig)
            {
                max = i;
                maxsig = sig;
            }
        }

        g->SetNameTitle("SigVs\\alpha", "Significance vs \\alpha");
        g->GetHistogram()->SetXTitle("\\alpha_{0} [\\circ]");
        g->GetHistogram()->SetYTitle("Significance");
        g->SetBit(kCanDelete);
        g->Draw("AP");

        leg = new TPaveText(0.35, 0.10, 0.90, 0.25, "brNDC");
        leg->SetBorderSize(1);
        leg->SetTextSize(0.1);
        leg->AddText(MString::Format("S_{max}=%.1f\\sigma at \\alpha_{max}=%d\\circ", maxsig, max));
        leg->SetBit(kCanDelete);
        leg->Draw();
    }
}

void MHFalseSource::DrawNicePlot() const
{
    Int_t newc = kTRUE;
    Float_t zoom = 0.95;
    Int_t col = kBlue+180;

    if (!newc && !fDisplay)
        return;

    TH1 *h = dynamic_cast<TH1*>(FindObjectInPad("Alpha_yx_on"));
    if (!h)
        return;

    // Open and setup canvas/pad
    TCanvas &c = newc ? *new TCanvas("Excess", "Excess Plot", TMath::Nint(500.), TMath::Nint(500*0.77/0.89)) : fDisplay->AddTab("ThetsSq");

    //c.SetPad(0.15, 0, 0.90, 1);

    c.SetBorderMode(0);
    c.SetFrameBorderMode(0);
    c.SetFillColor(kWhite);

    c.SetLeftMargin(0.11);
    c.SetRightMargin(0.12);
    c.SetBottomMargin(0.10);
    c.SetTopMargin(0.01);

    TH1 *h1 = (TH1*)h->Clone("");
    h1->SetDirectory(0);
    h1->SetTitle("");
    h1->SetContour(99);
    h1->SetBit(TH1::kNoStats);
    h1->SetBit(TH1::kCanDelete);

    if (h1->FindObject("stats"))
        delete h1->FindObject("stats");

    TAxis &x = *h1->GetXaxis();
    TAxis &y = *h1->GetYaxis();
    TAxis &z = *h1->GetZaxis();

    x.SetRangeUser(-zoom, zoom);
    y.SetRangeUser(-zoom, zoom);

    x.SetTitleOffset(1.1);
    y.SetTitleOffset(1.3);

    x.SetTickLength(0.025);
    y.SetTickLength(0.025);

    x.SetAxisColor(kWhite);
    y.SetAxisColor(kWhite);

    x.CenterTitle();
    y.CenterTitle();

    x.SetTitle("Offset [#circ]");
    y.SetTitle("Offset [#circ]");

    x.SetDecimals();
    y.SetDecimals();
    z.SetDecimals();

    MH::SetPalette("glowsym", 99);

    const Float_t max = TMath::Max(h1->GetMinimum(), h1->GetMaximum());

    h1->SetMinimum(-max);
    h1->SetMaximum(max);

    h1->Draw("colz");

    // ------
    // Convert pave coordinates from NDC to Pad coordinates.

    gPad->Update();

    Float_t x0 = 0.80;
    Float_t y0 = 0.88;

    Double_t dx  = gPad->GetX2() - gPad->GetX1();
    Double_t dy  = gPad->GetY2() - gPad->GetY1();

    // Check if pave initialisation has been done.
    // This operation cannot take place in the Pave constructor because
    // the Pad range may not be known at this time.
    Float_t px = gPad->GetX1() + x0*dx;
    Float_t py = gPad->GetY1() + y0*dy;
    // -------

    TEllipse *el = new TEllipse(px, py, 0.12, 0.12, 0, 360, 0);
    el->SetFillStyle(4100);
    el->SetFillColor(kBlack);
    el->SetLineWidth(2);
    el->SetLineColor(kWhite);
    el->SetBit(kCanDelete);
    el->Draw();

    TString str1("el.SetX1(gPad->GetX1()+0.9*(gPad->GetX2()-gPad->GetX1()));");
    TString str2("el.SetY1(gPad->GetY1()+0.9*(gPad->GetY2()-gPad->GetY1()));");

    str1.ReplaceAll("el.", MString::Format("((TEllipse*)%p)->", el));
    str2.ReplaceAll("el.", MString::Format("((TEllipse*)%p)->", el));

    str1.ReplaceAll("0.9", MString::Format("%f", x0));
    str2.ReplaceAll("0.9", MString::Format("%f", y0));

    TLatex tex;
    tex.SetBit(TText::kTextNDC);
    tex.SetTextColor(kWhite);
    tex.SetTextAlign(22);
    tex.SetTextSize(0.04);
    tex.SetTextAngle(0);
    tex.DrawLatex(x0, y0, "psf");

    TPad *pad = new TPad("pad", "Catalog Pad",
                         c.GetLeftMargin(), c.GetBottomMargin(),
                         1-c.GetRightMargin(), 1-c.GetTopMargin());

    pad->SetFillStyle(4000);
    pad->SetFillColor(kBlack);
    pad->SetBorderMode(0);
    pad->SetFrameBorderMode(0);
    pad->SetBit(kCanDelete);
    pad->Draw();

    pad->Range(x.GetBinLowEdge(x.GetFirst()),
               y.GetBinLowEdge(y.GetFirst()),
               x.GetBinLowEdge(x.GetLast()+1),
               y.GetBinLowEdge(y.GetLast()+1));

    TString str3("pad->Range(x.GetBinLowEdge(x.GetFirst()),"
                 "y.GetBinLowEdge(y.GetFirst()),"
                 "x.GetBinLowEdge(x.GetLast()+1),"
                 "y.GetBinLowEdge(y.GetLast()+1));");

    str3.ReplaceAll("x.", MString::Format("((TAxis*)%p)->", &x));
    str3.ReplaceAll("y.", MString::Format("((TAxis*)%p)->", &y));
    // str3.ReplaceAll("pad", Form("((TPad*)(%p))", pad));

    c.AddExec("SetPosX", str1);
    c.AddExec("SetPosY", str2);
    c.AddExec("Resize",  str3);

    pad->cd();
    gROOT->SetSelectedPad(0);

    MAstroCatalog *cat = (MAstroCatalog*)GetCatalog();

    cat->GetAttLineSky().SetLineColor(col);
    cat->GetAttLineSky().SetLineWidth(2);
    cat->GetAttLineSky().SetLineStyle(7);

    cat->GetList()->Clear();
    cat->SetBit(kCanDelete);
    //    cat->AddObject(MAstro::Hms2Hor(12,17,52)*TMath::Pi()/12, TMath::DegToRad()*MAstro::Dms2Deg(30,7,0),   6, "1ES1215+303");
    //    cat->AddObject(MAstro::Hms2Hor(12,18,27)*TMath::Pi()/12, TMath::DegToRad()*MAstro::Dms2Deg(29,48,46), 6, "Mrk766");

    cat->Draw("mirror same");

    /*
     Int_t col = kBlue+180;

     TLatex tex;
     tex.SetTextColor(col);
     tex.SetTextAlign(21);
     tex.SetTextSize(0.04);
     tex.DrawLatex(-0.79, -0.8, "43.0\\circ");
     tex.DrawLatex(-0.78,  0.2, "42.0\\circ");

     tex.SetTextAngle(-90);
     tex.DrawLatex(-0.45, -0.55, "22.00h");
     tex.DrawLatex( 0.30, -0.55, "22.07h");
     */
}
