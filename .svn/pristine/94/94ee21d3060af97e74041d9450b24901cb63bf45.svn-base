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
!   Author(s): Thomas Bretz, 05/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Harald Kornmayer, 1/2001
!   Author(s): Markus Gaug, 03/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHCamera
//
// Camera Display, based on a TH1D. Pleas be carefull using the
// underlaying TH1D.
//
// To change the scale to a logarithmic scale SetLogy() of the Pad.
//
// You can correct for the abberation. Assuming that the distance
// between the mean position of the light distribution and the position
// of a perfect reflection on a perfect mirror in the distance r on
// the camera plane is dr it is  d = a*dr  while a is the abberation
// constant (for the MAGIC mirror it is roughly 0.0713). You can
// set this constant by calling SetAbberation(a) which results in a
// 'corrected' display (all outer pixels are shifted towards the center
// of the camera to correct for this abberation)
//
// Be carefull: Entries in this context means Entries/bin or Events
//
// FIXME? Maybe MHCamera can take the fLog object from MGeomCam?
//
////////////////////////////////////////////////////////////////////////////
#include "MHCamera.h"

#include <fstream>
#include <iostream>

#include <TBox.h>
#include <TArrow.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TArrayF.h>
#include <TRandom.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TClonesArray.h>
#include <THistPainter.h>
#include <THLimitsFinder.h>
#include <TProfile.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TLegend.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MH.h"
#include "MString.h"
#include "MBinning.h"

#include "MGeom.h"
#include "MGeomCam.h"

#include "MCamEvent.h"

#include "MArrayD.h"
#include "MMath.h"    // MMath::GaussProb

ClassImp(MHCamera);

using namespace std;

void MHCamera::Init()
{
    Sumw2();

    UseCurrentStyle();

    SetDirectory(NULL);

    SetLineColor(kGreen);
    SetMarkerStyle(kFullDotMedium);
    SetXTitle("Pixel Index");

    fNotify  = new TList;
    fNotify->SetBit(kMustCleanup);
    gROOT->GetListOfCleanups()->Add(fNotify);

    /*
    TVirtualPad *save = gPad;
    gPad = 0;
#if ROOT_VERSION_CODE < ROOT_VERSION(3,01,06)
    SetPalette(1, 0);
#endif
    */
/*
#if ROOT_VERSION_CODE < ROOT_VERSION(4,04,00)
    SetPrettyPalette();
#elese
    // WORAROUND - FIXME: Calling it many times becomes slower and slower
    SetInvDeepBlueSeaPalette();
#endif
    gPad = save;
*/
}

// ------------------------------------------------------------------------
//
//  Default Constructor. To be used by the root system ONLY.
//
MHCamera::MHCamera() : TH1D(), fGeomCam(NULL), fAbberation(0)
{
    Init();
}

// ------------------------------------------------------------------------
//
//  Constructor. Makes a clone of MGeomCam. Removed the TH1D from the
// current directory. Calls Sumw2(). Set the histogram line color
// (for error bars) to Green and the marker style to kFullDotMedium.
//
MHCamera::MHCamera(const MGeomCam &geom, const char *name, const char *title)
: fGeomCam(NULL), fAbberation(0)
{
    //fGeomCam = (MGeomCam*)geom.Clone();
    SetGeometry(geom, name, title);
    Init();

    //
    // root 3.02
    //  * base/inc/TObject.h:
    //    register BIT(8) as kNoContextMenu. If an object has this bit set it will
    //    not get an automatic context menu when clicked with the right mouse button.
}

// ------------------------------------------------------------------------
//
// Clone the MHCamera via TH1D::Clone and make sure that the new object is
// not removed from the current directory.
//
TObject *MHCamera::Clone(const char *newname) const
{
    MHCamera *rc = static_cast<MHCamera*>(TH1D::Clone(newname));
    rc->SetDirectory(NULL);

    // fGeomCam need special treatment due to its TObjArray
    if (rc->fGeomCam && fGeomCam)
    {
        delete rc->fGeomCam;
        rc->fGeomCam = static_cast<MGeomCam*>(fGeomCam->Clone());
    }

    return rc;
}

void MHCamera::SetGeometry(const MGeomCam &geom, const char *name, const char *title)
{
    SetNameTitle(name, title);

    TAxis &x = *GetXaxis();

    SetBins(geom.GetNumPixels(), 0, 1);
    x.Set(geom.GetNumPixels(), -0.5, geom.GetNumPixels()-0.5);

    //SetBins(geom.GetNumPixels(), -0.5, geom.GetNumPixels()-0.5);
    //Rebuild();

    if (fGeomCam)
        delete fGeomCam;
    fGeomCam = (MGeomCam*)geom.Clone();

    fUsed.Set(geom.GetNumPixels());
    fUsed.Reset();

    fBinEntries.Set(geom.GetNumPixels()+2);
    fBinEntries.Reset();
}

// ------------------------------------------------------------------------
//
// Destructor. Deletes the cloned fGeomCam and the notification list.
//
MHCamera::~MHCamera()
{
    if (fGeomCam)
      delete fGeomCam;
    if (fNotify)
      delete fNotify;
}

// ------------------------------------------------------------------------
//
// Return kTRUE for sector<0. Otherwise return kTRUE only if the specified
// sector idx matches the sector of the pixel with index idx.
//
Bool_t MHCamera::MatchSector(Int_t idx, const TArrayI &sector, const TArrayI &aidx) const
{
    const MGeom &pix = (*fGeomCam)[idx];
    return FindVal(sector, pix.GetSector()) && FindVal(aidx, pix.GetAidx());
}

// ------------------------------------------------------------------------
//
// Taken from TH1D::Fill(). Uses the argument directly as bin index.
// Doesn't increment the number of entries.
//
//   -*-*-*-*-*-*-*-*Increment bin with abscissa X by 1*-*-*-*-*-*-*-*-*-*-*
//                   ==================================
//
//    if x is less than the low-edge of the first bin, the Underflow bin is incremented
//    if x is greater than the upper edge of last bin, the Overflow bin is incremented
//
//    If the storage of the sum of squares of weights has been triggered,
//    via the function Sumw2, then the sum of the squares of weights is incremented
//    by 1 in the bin corresponding to x.
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Int_t MHCamera::Fill(Axis_t x)
{
#if ROOT_VERSION_CODE > ROOT_VERSION(3,05,00)
   if (fBuffer) return BufferFill(x,1);
#endif
   const Int_t bin = (Int_t)x+1;
   AddBinContent(bin);
   fBinEntries[bin]++;
   if (fSumw2.fN)
       fSumw2.fArray[bin]++;

   if (bin<=0 || bin>fNcells-2)
       return -1;

   fTsumw++;
   fTsumw2++;
   fTsumwx  += x;
   fTsumwx2 += x*x;
   return bin;
}

// ------------------------------------------------------------------------
//
// Taken from TH1D::Fill(). Uses the argument directly as bin index.
// Doesn't increment the number of entries.
//
//   -*-*-*-*-*-*Increment bin with abscissa X with a weight w*-*-*-*-*-*-*-*
//               =============================================
//
//    if x is less than the low-edge of the first bin, the Underflow bin is incremented
//    if x is greater than the upper edge of last bin, the Overflow bin is incremented
//
//    If the storage of the sum of squares of weights has been triggered,
//    via the function Sumw2, then the sum of the squares of weights is incremented
//    by w^2 in the bin corresponding to x.
//
//   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Int_t MHCamera::Fill(Axis_t x, Stat_t w)
{
#if ROOT_VERSION_CODE > ROOT_VERSION(3,05,00)
   if (fBuffer) return BufferFill(x,w);
#endif
   const Int_t bin = (Int_t)x+1;
   AddBinContent(bin, w);
   fBinEntries[bin]++;
   if (fSumw2.fN)
       fSumw2.fArray[bin] += w*w;

   if (bin<=0 || bin>fNcells-2)
       return -1;

   const Stat_t z = (w > 0 ? w : -w);
   fTsumw   += z;
   fTsumw2  += z*z;
   fTsumwx  += z*x;
   fTsumwx2 += z*x*x;
   return bin;
}

// ------------------------------------------------------------------------
//
// Use x and y in millimeters
//
Int_t MHCamera::Fill(Axis_t x, Axis_t y, Stat_t w)
{
    if (fNcells<=1 || IsFreezed())
        return -1;

    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        if (!(*fGeomCam)[idx].IsInside(x, y))
            continue;

        SetUsed(idx);
        return Fill(idx, w);
    }
    return -1;
}

// ------------------------------------------------------------------------
//
// Call this if you want to change the display status (displayed or not)
// for all pixels. val==0 means that the pixel is not displayed.
//
void MHCamera::SetUsed(const TArrayC &arr)
{
    if (fNcells-2 != arr.GetSize())
    {
        gLog << warn << "WARNING - MHCamera::SetUsed: array size mismatch... ignored." << endl;
        return;
    }

    for (Int_t idx=0; idx<fNcells-2; idx++)
        arr[idx] ? SetUsed(idx) : ResetUsed(idx);
}

// ------------------------------------------------------------------------
//
// Return the mean value of all entries which are used if all=kFALSE and
// of all entries if all=kTRUE if sector<0. If sector>=0 only
// entries with match the given sector are taken into account.
//
Stat_t MHCamera::GetMeanSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t ball) const
{
    if (fNcells<=1)
        return 0;

    Int_t n=0;

    Stat_t mean = 0;
    for (int i=0; i<fNcells-2; i++)
    {
        if ((ball || IsUsed(i)) && MatchSector(i, sector, aidx))
        {
            if (TestBit(kProfile) && fBinEntries[i+1]==0)
                continue;

            mean += TestBit(kProfile) ? fArray[i+1]/fBinEntries[i+1] : fArray[i+1];
            n++;
        }
    }

    return n==0 ? 0 : mean/n;
}

UInt_t MHCamera::GetNumUsedSector(const TArrayI &sector, const TArrayI &aidx) const
{
    if (fNcells<=1)
        return 0;

    Int_t n=0;
 
    for (int i=0; i<fNcells-2; i++)
    {
        if (!IsUsed(i) || !MatchSector(i, sector, aidx))
            continue;

        if (TestBit(kProfile) && fBinEntries[i+1]==0)
            continue;
        n++;
    }

    // return Median of the profile data
    return n;
}

// ------------------------------------------------------------------------
//
// Return the median value of all entries which are used if all=kFALSE and
// of all entries if all=kTRUE if sector<0. If sector>=0 only
// entries with match the given sector are taken into account.
//
Stat_t MHCamera::GetMedianSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t ball) const
{
    if (fNcells<=1)
        return 0;

    TArrayD arr(fNcells-2);
    Int_t n=0;
 
    for (int i=0; i<fNcells-2; i++)
    {
        if ((ball || IsUsed(i)) && MatchSector(i, sector, aidx))
        {
            if (TestBit(kProfile) && fBinEntries[i+1]==0)
                continue;

            arr[n++] = TestBit(kProfile) ? fArray[i+1]/fBinEntries[i+1] : fArray[i+1];
        }
    }

    // return Median of the profile data
    return TMath::Median(n, arr.GetArray());
}

// ------------------------------------------------------------------------
//
// Return the sqrt variance of all entries which are used if all=kFALSE and
// of all entries if all=kTRUE if sector<0. If sector>=0 only
// entries with match the given sector are taken into account.
//
Stat_t MHCamera::GetRmsSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t ball) const
{
    if (fNcells<=1)
        return -1;

    Int_t n=0;

    Stat_t sum = 0;
    Stat_t sq  = 0;
    for (int i=0; i<fNcells-2; i++)
    {
        if ((ball || IsUsed(i)) && MatchSector(i, sector, aidx))
        {
            if (TestBit(kProfile) && fBinEntries[i+1]==0)
                continue;

            const Double_t val = TestBit(kProfile) ? fArray[i+1]/fBinEntries[i+1] : fArray[i+1];

            sum += val;
            sq  += val*val;
            n++;
        }
    }

    if (n==0)
        return 0;

    sum /= n;
    sq  /= n;

    return TMath::Sqrt(sq-sum*sum);
}

// ------------------------------------------------------------------------
//
// Return the median value (divided by MMath::GausProb(1.0)=68.3%) of the
// distribution of abs(y[i]-Median). This is my Median equivalent of the RMS.
// Return the deviation of all entries which are used if all=kFALSE and
// of all entries if all=kTRUE if sector<0. If sector>=0 only
// entries with match the given sector are taken into account.
//
Stat_t MHCamera::GetDevSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t ball) const
{
    if (fNcells<=1)
        return 0;

    TArrayD arr(fNcells-2);
    Int_t n=0;
 
    for (int i=0; i<fNcells-2; i++)
    {
        if ((ball || IsUsed(i)) && MatchSector(i, sector, aidx))
        {
            if (TestBit(kProfile) && fBinEntries[i+1]==0)
                continue;

            arr[n++] = TestBit(kProfile) ? fArray[i+1]/fBinEntries[i+1] : fArray[i+1];
        }
    }

    // return Median of the profile data
    return MMath::MedianDev(n, arr.GetArray());
}

// ------------------------------------------------------------------------
//
// Return the minimum contents of all pixels (if all is set, otherwise
// only of all 'used' pixels), fMinimum if fMinimum set. If sector>=0
// only pixels with matching sector number are taken into account.
//
Double_t MHCamera::GetMinimumSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t ball) const
{
    if (fMinimum != -1111)
        return fMinimum;

    if (fNcells<=1)
        return 0;

    Double_t minimum=FLT_MAX;

    for (Int_t i=0; i<fNcells-2; i++)
    {
        if (TestBit(kProfile) && fBinEntries[i+1]==0)
            continue;

        const Double_t val = TestBit(kProfile) ? fArray[i+1]/fBinEntries[i+1] : fArray[i+1];
        if (MatchSector(i, sector, aidx) && (ball || IsUsed(i)) && val<minimum && TMath::Finite(val))
            minimum = val;
    }

    return minimum;
}

// ------------------------------------------------------------------------
//
// Return the maximum contents of all pixels (if all is set, otherwise
// only of all 'used' pixels), fMaximum if fMaximum set. If sector>=0
// only pixels with matching sector number are taken into account.
//
Double_t MHCamera::GetMaximumSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t ball) const
{
    if (fMaximum!=-1111)
        return fMaximum;

    if (fNcells<=1)
        return 1;

    Double_t maximum=-FLT_MAX;
    for (Int_t i=0; i<fNcells-2; i++)
    {
        if (TestBit(kProfile) && fBinEntries[i+1]==0)
            continue;

        const Double_t val = TestBit(kProfile) ? fArray[i+1]/fBinEntries[i+1] : fArray[i+1];
        if (MatchSector(i, sector, aidx) && (ball || IsUsed(i)) && val>maximum && TMath::Finite(val))
            maximum = val;
    }

    return maximum;
}

// ------------------------------------------------------------------------
//
// Return the number of bins (excluding under- and overflow) for which
// GetBinContent returns a value > t
//
Int_t MHCamera::GetNumBinsAboveThreshold(Double_t t) const
{
    Int_t n = 0;
    for (Int_t i=0; i<fNcells-2; i++)
        if (GetBinContent(i+1)>t)
            n++;

    return n;
}

// ------------------------------------------------------------------------
//
// Return the number of bins (excluding under- and overflow) for which
// GetBinContent returns a value < t
//
Int_t MHCamera::GetNumBinsBelowThreshold(Double_t t) const
{
    Int_t n = 0;
    for (Int_t i=0; i<fNcells-2; i++)
        if (GetBinContent(i+1)>t)
            n++;

    return n;
}

// ------------------------------------------------------------------------
//
// Call this function to draw the camera layout into your canvas.
// Setup a drawing canvas. Add this object and all child objects
// (hexagons, etc) to the current pad. If no pad exists a new one is
// created. (To access the 'real' pad containing the camera you have
// to do a cd(1) in the current layer.
//
// To draw a camera into its own pad do something like:
//
//   MGeomCamMagic m;
//   MHCamera *d=new MHCamera(m);
//
//   TCanvas *c = new TCanvas;
//   c->Divide(2,1);
//   c->cd(1);
//
//   d->FillRandom();
//   d->Draw();
//   d->SetBit(kCanDelete);
//
// There are several drawing options:
//   'hist'        Draw as a standard TH1 histogram (value vs. pixel index)
//   'box'         Draw hexagons which size is in respect to its contents
//   'nocol'       Leave the 'boxed' hexagons empty
//   'pixelindex'  Display the pixel index in each pixel
//   'sectorindex' Display the sector index in each pixel
//   'content'     Display the relative content aligned to GetMaximum() and
//                 GeMinimum() ((val-min)/(max-min))
//   'text'        Draw GetBinContent as char
//   'integer'     Draw GetBinContent as integer (TMath::Nint)
//   'float'       Draw GetBinContent as float
//   'proj'        Display the y-projection of the histogram
//   'pal0'        Use Pretty palette
//   'pal1'        Use Deep Blue Sea palette
//   'pal2'        Use Inverse Depp Blue Sea palette
//   'same'        Draw trandparent pixels on top of an existing pad. This
//                 makes it possible to draw the camera image on top of an
//                 existing TH2, but also allows for distorted camera images
//
void MHCamera::Draw(Option_t *option)
{
    const Bool_t hassame = TString(option).Contains("same", TString::kIgnoreCase) && gPad;

    // root 3.02:
    // gPad->SetFixedAspectRatio()
    const Color_t col = gPad ? gPad->GetFillColor() : 16;
    TVirtualPad  *pad = gPad ? gPad : MH::MakeDefCanvas("CamDisplay", "Mars Camera Display", 656, 600);

    if (!hassame)
    {
        pad->SetBorderMode(0);
        pad->SetFillColor(col);

        //
        // Create an own pad for the MHCamera-Object which can be
        // resized in paint to keep the correct aspect ratio
        //
        // The margin != 0 is a workaround for a problem in root 4.02/00
        //pad->Divide(1, 1, 1e-10, 1e-10, col);
        //pad->cd(1);
        //gPad->SetBorderMode(0);
    }

    AppendPad(option);
    //fGeomCam->AppendPad();

    //
    // Do not change gPad. The user should not see, that Draw
    // changes gPad...
    //
//    if (!hassame)
//        pad->cd();
}

// ------------------------------------------------------------------------
//
// Creates a TH1D which contains the projection of the contents of the
// MHCamera onto the y-axis. The maximum and minimum are calculated
// such that a slighly wider range than (GetMinimum(), GetMaximum()) is
// displayed using THLimitsFinder::OptimizeLimits.
//
// If no name is given the newly allocated histogram is removed from
// the current directory calling SetDirectory(0) in any other case
// the newly created histogram is removed from the current directory
// and added to gROOT such the gROOT->FindObject can find the histogram.
//
// If the standard name "_proj" is given "_proj" is appended to the name
// of the MHCamera and the corresponding histogram is searched using
// gROOT->FindObject and updated with the present projection.
//
// It is the responsibility of the user to make sure, that the newly
// created histogram is freed correctly.
//
// Currently the new histogram is restrictred to 50 bins.
// Maybe a optimal number can be calulated from the number of
// bins on the x-axis of the MHCamera?
//
// The code was taken mainly from TH2::ProjectX such the interface
// is more or less the same than to TH2-projections.
//
// If sector>=0 only entries with matching sector index are taken
// into account.
//
TH1D *MHCamera::ProjectionS(const TArrayI &sector, const TArrayI &aidx, const char *name, const Int_t nbins) const
{

    // Create the projection histogram
    TString pname(name);
    if (pname=="_proj")
    {
        pname.Prepend(GetName());
        if (sector.GetSize()>0)
        {
            pname += ";";
            for (int i=0; i<sector.GetSize(); i++)
                pname += sector[i];
        }
        if (aidx.GetSize()>0)
        {
            pname += ";";
            for (int i=0; i<aidx.GetSize(); i++)
                pname += aidx[i];
        }
    }

    TH1D *h1=0;

    //check if histogram with identical name exist
    TObject *h1obj = gROOT->FindObject(pname);
    if (h1obj && h1obj->InheritsFrom(TH1D::Class())) {
        h1 = (TH1D*)h1obj;
        h1->Reset();
    }

    if (!h1)
    {
        h1 = new TH1D;
        h1->UseCurrentStyle();
        h1->SetName(pname);
        h1->SetTitle(GetTitle());
        h1->SetDirectory(0);
        h1->SetXTitle(GetYaxis()->GetTitle());
        h1->SetYTitle("Counts");
        //h1->Sumw2();
    }

    Double_t min = GetMinimumSectors(sector, aidx);
    Double_t max = GetMaximumSectors(sector, aidx);

    if (min==max && max>0)
        min=0;
    if (min==max && min<0)
        max=0;

    Int_t newbins=0;
    THLimitsFinder::OptimizeLimits(nbins, newbins, min, max, kFALSE);

    MBinning bins(nbins, min, max);
    bins.Apply(*h1);

    // Fill the projected histogram
    for (Int_t idx=0; idx<fNcells-2; idx++)
        if (IsUsed(idx) && MatchSector(idx, sector, aidx) && TMath::Finite(GetBinContent(idx+1)))
            h1->Fill(GetBinContent(idx+1));

    return h1;
}

// ------------------------------------------------------------------------
//
// Creates a TH1D which contains the projection of the contents of the
// MHCamera onto the radius from the camera center. 
// The maximum and minimum are calculated
// such that a slighly wider range than (GetMinimum(), GetMaximum()) is
// displayed using THLimitsFinder::OptimizeLimits.
//
// If no name is given the newly allocated histogram is removed from
// the current directory calling SetDirectory(0) in any other case
// the newly created histogram is removed from the current directory
// and added to gROOT such the gROOT->FindObject can find the histogram.
//
// If the standard name "_rad" is given "_rad" is appended to the name
// of the MHCamera and the corresponding histogram is searched using
// gROOT->FindObject and updated with the present projection.
//
// It is the responsibility of the user to make sure, that the newly
// created histogram is freed correctly.
//
// Currently the new histogram is restrictred to 50 bins.
// Maybe a optimal number can be calulated from the number of
// bins on the x-axis of the MHCamera?
//
// The code was taken mainly from TH2::ProjectX such the interface
// is more or less the same than to TH2-projections.
//
// If sector>=0 only entries with matching sector index are taken
// into account.
//
TProfile *MHCamera::RadialProfileS(const TArrayI &sector, const TArrayI &aidx, const char *name, const Int_t nbins) const
{
    // Create the projection histogram
    TString pname(name);
    if (pname=="_rad")
    {
        pname.Prepend(GetName());
        if (sector.GetSize()>0)
        {
            pname += ";";
            for (int i=0; i<sector.GetSize(); i++)
                pname += sector[i];
        }
        if (aidx.GetSize()>0)
        {
            pname += ";";
            for (int i=0; i<aidx.GetSize(); i++)
                pname += aidx[i];
        }
    }

    TProfile *h1=0;

    //check if histogram with identical name exist
    TObject *h1obj = gROOT->FindObject(pname);
    if (h1obj && h1obj->InheritsFrom(TProfile::Class())) {
        h1 = (TProfile*)h1obj;
        h1->Reset();
    }

    if (!h1)
    {
        h1 = new TProfile;
        h1->UseCurrentStyle();
        h1->SetName(pname);
        h1->SetTitle(GetTitle());
        h1->SetDirectory(0);
        h1->SetXTitle("Radius from camera center [mm]");
        h1->SetYTitle(GetYaxis()->GetTitle());
    }

    const Double_t m2d = fGeomCam->GetConvMm2Deg();

    Double_t min = 0.;
    Double_t max = fGeomCam->GetMaxRadius()*m2d;

    Int_t newbins=0;

    THLimitsFinder::OptimizeLimits(nbins, newbins, min, max, kFALSE);

    MBinning bins(nbins, min, max);
    bins.Apply(*h1);
  
    // Fill the projected histogram
    for (Int_t idx=0; idx<fNcells-2; idx++)
        if (IsUsed(idx) && MatchSector(idx, sector, aidx) && TMath::Finite(GetBinContent(idx+1)))
            h1->Fill(TMath::Hypot((*fGeomCam)[idx].GetX(),(*fGeomCam)[idx].GetY())*m2d,
                     GetBinContent(idx+1));
    return h1;
}


// ------------------------------------------------------------------------
//
// Creates a TH1D which contains the projection of the contents of the
// MHCamera onto the azimuth angle in the camera. 
//
// If no name is given the newly allocated histogram is removed from
// the current directory calling SetDirectory(0) in any other case
// the newly created histogram is removed from the current directory
// and added to gROOT such the gROOT->FindObject can find the histogram.
//
// If the standard name "_az" is given "_az" is appended to the name
// of the MHCamera and the corresponding histogram is searched using
// gROOT->FindObject and updated with the present projection.
//
// It is the responsibility of the user to make sure, that the newly
// created histogram is freed correctly.
//
// Currently the new histogram is restrictred to 60 bins.
// Maybe a optimal number can be calulated from the number of
// bins on the x-axis of the MHCamera?
//
// The code was taken mainly from TH2::ProjectX such the interface
// is more or less the same than to TH2-projections.
//
TProfile *MHCamera::AzimuthProfileA(const TArrayI &aidx, const char *name, const Int_t nbins) const
{
    // Create the projection histogram
    TString pname(name);
    if (pname=="_az")
    {
        pname.Prepend(GetName());
        if (aidx.GetSize()>0)
        {
            pname += ";";
            for (int i=0; i<aidx.GetSize(); i++)
                pname += aidx[i];
        }
    }

    TProfile *h1=0;

    //check if histogram with identical name exist
    TObject *h1obj = gROOT->FindObject(pname);
    if (h1obj && h1obj->InheritsFrom(TProfile::Class())) {
        h1 = (TProfile*)h1obj;
        h1->Reset();
    }

    if (!h1)
    {

        h1 = new TProfile;
        h1->UseCurrentStyle();
        h1->SetName(pname);
        h1->SetTitle(GetTitle());
        h1->SetDirectory(0);
        h1->SetXTitle("Azimuth in camera [deg]");
        h1->SetYTitle(GetYaxis()->GetTitle());
    }

    //Double_t min = 0;
    //Double_t max = 360;

    //Int_t newbins=0;
    //THLimitsFinder::OptimizeLimits(nbins, newbins, min, max, kFALSE);

    MBinning bins(nbins, 0, 360);
    bins.Apply(*h1);

    // Fill the projected histogram
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        if (IsUsed(idx) && MatchSector(idx, TArrayI(), aidx) && TMath::Finite(GetPixContent(idx)))
            h1->Fill(TMath::ATan2((*fGeomCam)[idx].GetY(),(*fGeomCam)[idx].GetX())*TMath::RadToDeg()+180,
                     GetPixContent(idx));

    }

    return h1;
}

// ------------------------------------------------------------------------
//
// Updates the pixel colors and paints the pixels
//
void MHCamera::Update(Bool_t islog, Bool_t isbox, Bool_t iscol, Bool_t issame)
{
    Double_t min = GetMinimum(kFALSE);
    Double_t max = GetMaximum(kFALSE);
    if (min==FLT_MAX)
    {
        min = 0;
        max = 1;
    }

    if (min==max)
        max += 1;

    if (!issame)
        UpdateLegend(min, max, islog);

    // Try to estimate the units of the current display. This is only
    // necessary for 'same' option and allows distorted images of the camera!
    const Float_t maxr = (1-fGeomCam->GetConvMm2Deg())*fGeomCam->GetMaxRadius()/2;
    const Float_t conv = !issame ||
        gPad->GetX1()<-maxr || gPad->GetY1()<-maxr ||
        gPad->GetX2()> maxr || gPad->GetY2()>maxr ? 1 : fGeomCam->GetConvMm2Deg();

    TAttLine line(kBlack, kSolid, 1);
    TAttFill fill;
    for (Int_t i=0; i<fNcells-2; i++)
    {
        fill.SetFillStyle(issame || (IsTransparent() && !IsUsed(i)) ? 0 : 1001);

        if (!issame)
        {
            const Bool_t isnan = !TMath::Finite(fArray[i+1]);
            if (!IsUsed(i) || !iscol || isnan)
            {
                fill.SetFillColor(10);

                if (isnan)
                    gLog << warn << "MHCamera::Update: " << GetName() << " <" << GetTitle() << "> - Pixel Index #" << i << " contents is not finite..." << endl;
            }
            else
                fill.SetFillColor(GetColor(GetBinContent(i+1), min, max, islog));
        }

        const MGeom &pix = (*fGeomCam)[i];

        Double_t scale = 1;//conv/(fAbberation+1);

        if (!isbox && !IsUsed(i) && TestBit(kNoUnused))
            continue;

        if (isbox && (!IsUsed(i) || !TMath::Finite(fArray[i+1])))
            continue;

        if (isbox)
        {
            scale = (GetBinContent(i+1)-min)/(max-min);
            if (scale>1)
                scale=1;
        }

        pix.PaintPrimitive(line, fill, conv, scale/(fAbberation+1));
    }
}

// ------------------------------------------------------------------------
//
// Print minimum and maximum
//
void MHCamera::Print(Option_t *) const
{
    gLog << all << "Minimum: " << GetMinimum();
    if (fMinimum==-1111)
        gLog << " <autoscaled>";
    gLog << endl;
    gLog << "Maximum: " << GetMaximum();
    if (fMaximum==-1111)
        gLog << " <autoscaled>";
    gLog << endl;
}

// ------------------------------------------------------------------------
//
// Paint the y-axis title
//
void MHCamera::PaintAxisTitle()
{
    const Float_t range = fGeomCam->GetMaxRadius()*1.05;
    const Float_t w = (1 + 1.5/sqrt((float)(fNcells-2)))*range;

    TLatex *ptitle = new TLatex(w, -.90*range, GetYaxis()->GetTitle());

    ptitle->SetTextSize(0.05);
    ptitle->SetTextAlign(21);

    // box with the histogram title
    ptitle->SetTextColor(gStyle->GetTitleTextColor());
#if ROOT_VERSION_CODE > ROOT_VERSION(3,05,01)
    ptitle->SetTextFont(gStyle->GetTitleFont(""));
#endif
    ptitle->Paint();
}

// ------------------------------------------------------------------------
//
// Paints the camera.
//
void MHCamera::Paint(Option_t *o)
{
    if (fNcells<=1)
        return;

    TString opt(o);
    opt.ToLower();

    if (opt.Contains("hist"))
    {
        opt.ReplaceAll("hist", "");
        opt.ReplaceAll("box", "");
        opt.ReplaceAll("pixelindex", "");
        opt.ReplaceAll("sectorindex", "");
        opt.ReplaceAll("abscontent", "");
        opt.ReplaceAll("content", "");
        opt.ReplaceAll("integer", "");
        opt.ReplaceAll("float", "");
        opt.ReplaceAll("proj", "");
        opt.ReplaceAll("pal0", "");
        opt.ReplaceAll("pal1", "");
        opt.ReplaceAll("pal2", "");
        opt.ReplaceAll("nopal", "");
        TH1D::Paint(opt);
        return;
    }

    if (opt.Contains("proj"))
    {
      opt.ReplaceAll("proj", "");
      Projection(GetName())->Paint(opt);
      return;
    }

    const Bool_t hassame = opt.Contains("same");
    const Bool_t hasbox  = opt.Contains("box");
    const Bool_t hascol  = hasbox ? !opt.Contains("nocol") : kTRUE;

    if (!hassame)
        gPad->Clear();

    if (!fGeomCam)
        return;

    if (!hassame)
    {
        // Maintain aspect ratio
        const Float_t r = fGeomCam->GetMaxRadius();

        MH::SetPadRange(-r*1.02, -r*1.02, TestBit(kNoLegend) ? r : 1.15*r, r*1.2);

        if (GetPainter())
        {
            // Paint statistics
            if (!TestBit(TH1::kNoStats))
                fPainter->PaintStat(gStyle->GetOptStat(), NULL);

            // Paint primitives (pixels, color legend, photons, ...)
            if (fPainter->InheritsFrom(THistPainter::Class()))
            {
                static_cast<THistPainter*>(fPainter)->MakeChopt("");
                static_cast<THistPainter*>(fPainter)->PaintTitle();
            }
        }
    }

    const Bool_t pal1  = opt.Contains("pal1");
    const Bool_t pal2  = opt.Contains("pal2");
    const Bool_t nopal = opt.Contains("nopal");

    if (!pal1 && !pal2 && !nopal)
        SetPrettyPalette();

    if (pal1)
        SetDeepBlueSeaPalette();

    if (pal2)
        SetInvDeepBlueSeaPalette();

    // Update Contents of the pixels and paint legend
    Update(gPad->GetLogy(), hasbox, hascol, hassame);

    if (!hassame)
        PaintAxisTitle();

    if (opt.Contains("pixelindex"))
    {
        PaintIndices(0);
        return;
    }
    if (opt.Contains("sectorindex"))
    {
        PaintIndices(1);
        return;
    }
    if (opt.Contains("abscontent"))
    {
        PaintIndices(3);
        return;
    }
    if (opt.Contains("content"))
    {
        PaintIndices(2);
        return;
    }
    if (opt.Contains("pixelentries"))
    {
        PaintIndices(4);
        return;
    }
    if (opt.Contains("text"))
    {
        PaintIndices(5);
        return;
    }
    if (opt.Contains("integer"))
    {
        PaintIndices(6);
        return;
    }
    if (opt.Contains("float"))
    {
        PaintIndices(7);
        return;
    }
}

void MHCamera::SetDrawOption(Option_t *option)
{
    // This is a workaround. For some reason MHCamera is
    // stored in a TObjLink instead of a TObjOptLink
    if (!option || !gPad)
        return;

    TListIter next(gPad->GetListOfPrimitives());
    delete gPad->FindObject("Tframe");
    TObject *obj;
    while ((obj = next()))
        if (obj == this && (TString)next.GetOption()!=(TString)option)
        {
            gPad->GetListOfPrimitives()->Remove(this);
            gPad->GetListOfPrimitives()->AddFirst(this, option);
            return;
        }
}

// ------------------------------------------------------------------------
//
// With this function you can change the color palette. For more
// information see TStyle::SetPalette. Only palettes with 50 colors
// are allowed.
// In addition you can use SetPalette(52, 0) to create an inverse
// deep blue sea palette
//
void MHCamera::SetPalette(Int_t ncolors, Int_t *colors)
{
    //
    // If not enough colors are specified skip this.
    //
    if (ncolors>1 && ncolors<50)
    {
        gLog << err << "MHCamera::SetPalette: Only default palettes with 50 colors are allowed... ignored." << endl;
        return;
    }

    //
    // If ncolors==52 create a reversed deep blue sea palette
    //
    if (ncolors==52)
    {
        gStyle->SetPalette(51, NULL);

        const Int_t n = GetContour()==0?50:GetContour();

        TArrayI c(n);
        for (int i=0; i<n; i++)
            c[n-i-1] = gStyle->GetColorPalette(i);
        gStyle->SetPalette(n, c.GetArray());
    }
    else
        gStyle->SetPalette(ncolors, colors);
}


// ------------------------------------------------------------------------
//
// Changes the palette of the displayed camera histogram.
//
// Change to the right pad first - otherwise GetDrawOption() might fail.
//
void MHCamera::SetPrettyPalette()
{
    TString opt(GetDrawOption());

    if (!opt.Contains("hist", TString::kIgnoreCase))
        SetPalette(1, 0);

    opt.ReplaceAll("pal1", "");
    opt.ReplaceAll("pal2", "");

    SetDrawOption(opt);
}

// ------------------------------------------------------------------------
//
// Changes the palette of the displayed camera histogram.
//
// Change to the right pad first - otherwise GetDrawOption() might fail.
//
void MHCamera::SetDeepBlueSeaPalette()
{
    TString opt(GetDrawOption());

    if (!opt.Contains("hist", TString::kIgnoreCase))
        SetPalette(51, 0);

    opt.ReplaceAll("pal1", "");
    opt.ReplaceAll("pal2", "");
    opt += "pal1";

    SetDrawOption(opt);
}

// ------------------------------------------------------------------------
//
// Changes the palette of the displayed camera histogram.
//
// Change to the right pad first - otherwise GetDrawOption() might fail.
//
void MHCamera::SetInvDeepBlueSeaPalette()
{
    TString opt(GetDrawOption());

    if (!opt.Contains("hist", TString::kIgnoreCase))
        SetPalette(52, 0);

    opt.ReplaceAll("pal1", "");
    opt.ReplaceAll("pal2", "");
    opt += "pal2";

    SetDrawOption(opt);
}

// ------------------------------------------------------------------------
//
// Paint indices (as text) inside the pixels. Depending of the type-
// argument we paint:
//  0: pixel number
//  1: sector number
//  2: content
//  5: Assume GetBinContent is a char
//
void MHCamera::PaintIndices(Int_t type)
{
    if (fNcells<=1)
        return;

    const Double_t min = GetMinimum();
    const Double_t max = GetMaximum();

    if (type==2 && max==min)
        return;

    TText txt;
    if (type!=5)
        txt.SetTextFont(122);
    txt.SetTextAlign(22);   // centered/centered

    for (Int_t i=0; i<fNcells-2; i++)
    {
        const MGeom &h = (*fGeomCam)[i];

        TString num;
        switch (type)
        {
        case 0: num += i; break;
        case 1: num += h.GetSector(); break;
        case 2: num += TMath::Nint((fArray[i+1]-min)/(max-min)); break;
        case 3: num += TMath::Nint(fArray[i+1]); break;
        case 4: num += fBinEntries[i+1]; break;
        case 5: num  = (char)TMath::Nint(GetBinContent(i+1)); break;
        case 6: num += TMath::Nint(GetBinContent(i+1)); break;
        case 7: num += GetBinContent(i+1); break;
        }

        // FIXME: Should depend on the color of the pixel...
        //(GetColor(GetBinContent(i+1), min, max, 0));
        txt.SetTextColor(kRed);
        txt.SetTextSize(0.3*h.GetT()/fGeomCam->GetMaxRadius()/1.05);
        txt.PaintText(h.GetX(), h.GetY(), num);
    }
}

// ------------------------------------------------------------------------
//
// Call this function to add a MCamEvent on top of the present contents.
//
void MHCamera::AddMeanShift(const MCamEvent &event, Int_t type, Stat_t w)
{
    if (fNcells<=1 || IsFreezed())
        return;

    const Double_t mean = event.GetCameraMean(*fGeomCam, type);

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Double_t val=0;
        if (event.GetPixelContent(val, idx, *fGeomCam, type)/* && !IsUsed(idx)*/)
        {
            SetUsed(idx);
            Fill(idx, (val-mean)*w); // FIXME: Slow!
        }
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Call this function to add a MCamEvent on top of the present contents.
//
void MHCamera::AddMedianShift(const MCamEvent &event, Int_t type, Stat_t w)
{
    if (fNcells<=1 || IsFreezed())
        return;

    const Double_t median = event.GetCameraMedian(*fGeomCam, type);

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Double_t val=0;
        if (event.GetPixelContent(val, idx, *fGeomCam, type)/* && !IsUsed(idx)*/)
        {
            SetUsed(idx);
            Fill(idx, (val-median)*w); // FIXME: Slow!
        }
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Call this function to add a MCamEvent on top of the present contents.
//
void MHCamera::AddCamContent(const MCamEvent &event, Int_t type, Stat_t w)
{
    if (fNcells<=1 || IsFreezed())
        return;

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Double_t val=0;
        if (event.GetPixelContent(val, idx, *fGeomCam, type)/* && !IsUsed(idx)*/)
        {
            SetUsed(idx);
            Fill(idx, val*w); // FIXME: Slow!
        }
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
void MHCamera::AddCamDifference(const MCamEvent &event1, const MCamEvent &event2, Int_t type, Stat_t weight)
{
    if (fNcells<=1 || IsFreezed())
        return;

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Double_t val1=0;
        if (!event1.GetPixelContent(val1, idx, *fGeomCam, type))
            continue;

        Double_t val2=0;
        if (!event2.GetPixelContent(val2, idx, *fGeomCam, type))
            continue;

        SetUsed(idx);
        Fill(idx, (val1-val2)*weight); // FIXME: Slow!
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
void MHCamera::AddCamRatio(const MCamEvent &event1, const MCamEvent &event2, Int_t type, Stat_t weight)
{
    if (fNcells<=1 || IsFreezed())
        return;

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Double_t val1=0;
        if (!event1.GetPixelContent(val1, idx, *fGeomCam, type))
            continue;

        Double_t val2=0;
        if (!event2.GetPixelContent(val2, idx, *fGeomCam, type))
            continue;

        if (val2==0)
            continue;

        SetUsed(idx);
        Fill(idx, weight*val1/val2); // FIXME: Slow!
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Call this function to add a MCamEvent on top of the present contents.
//
void MHCamera::SetCamError(const MCamEvent &evt, Int_t type)
{

    if (fNcells<=1 || IsFreezed())
        return;

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Double_t val=0;
        if (evt.GetPixelContent(val, idx, *fGeomCam, type)/* && !IsUsed(idx)*/)
            SetUsed(idx);

        SetBinError(idx+1, val); // FIXME: Slow!
    }
}

Stat_t MHCamera::GetBinContent(Int_t bin) const
{
    if (fBuffer) ((TH1D*)this)->BufferEmpty();
    if (bin < 0) bin = 0;
    if (bin >= fNcells) bin = fNcells-1;
    if (!fArray) return 0;

    if (!TestBit(kProfile))
        return Stat_t (fArray[bin]);

    if (fBinEntries.fArray[bin] == 0) return 0;
    return fArray[bin]/fBinEntries.fArray[bin];
}

// ------------------------------------------------------------------------
//
// In the case the kProfile flag is set the spread of the bin is returned.
// If you want to have the mean error instead set the kErrorMean bit via
//  SetBit(kErrorMean) first.
//
Stat_t MHCamera::GetBinError(Int_t bin) const
{
    if (!TestBit(kProfile))
        return TH1D::GetBinError(bin);

    const UInt_t n = (UInt_t)fBinEntries[bin];

    if (n==0)
        return 0;

    const Double_t sqr = fSumw2.fArray[bin] / n;
    const Double_t val = fArray[bin]        / n;

    const Double_t spread = sqr>val*val ? TMath::Sqrt(sqr - val*val) : 0;

    return TestBit(kErrorMean) ? spread/TMath::Sqrt(n) : spread;

    /*
    Double_t rc = 0;
    if (TestBit(kSqrtVariance) && GetEntries()>0) // error on the mean
    {
        const Double_t error = fSumw2.fArray[bin]/GetEntries();
        const Double_t val   = fArray[bin]/GetEntries();
        rc = val*val>error ? 0 : TMath::Sqrt(error - val*val);
    }
    else
        rc = TH1D::GetBinError(bin);

    return Profile(rc);*/
}

// ------------------------------------------------------------------------
//
// Call this function to add a MHCamera on top of the present contents.
// Type:
//  0) bin content
//  1) errors
//  2) rel. errors
//
void MHCamera::AddCamContent(const MHCamera &d, Int_t type)
{
    if (fNcells!=d.fNcells || IsFreezed())
        return;

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
        if (d.IsUsed(idx))
            SetUsed(idx);

    switch (type)
    {
    case 1:
        // Under-/Overflow bins not handled!
        for (Int_t idx=0; idx<fNcells-2; idx++)
            if (d.IsUsed(idx))
                Fill(idx, d.GetBinError(idx+1));
        fEntries++;
        break;
    case 2:
        // Under-/Overflow bins not handled!
        for (Int_t idx=0; idx<fNcells-2; idx++)
            if (d.GetBinContent(idx+1)!=0 && d.IsUsed(idx))
                Fill(idx, TMath::Abs(d.GetBinError(idx+1)/d.GetBinContent(idx+1)));
        fEntries++;
        break;
    default:
        if (TestBit(kProfile)!=d.TestBit(kProfile))
            gLog << warn << "WARNING - You have tried to call AddCamContent for two different kind of histograms (kProfile set or not)." << endl;

        // environment
        fEntries += d.fEntries;
        fTsumw   += d.fTsumw;
        fTsumw2  += d.fTsumw2;
        fTsumwx  += d.fTsumwx;
        fTsumwx2 += d.fTsumwx2;
        // Bin contents
        for (Int_t idx=1; idx<fNcells-1; idx++)
        {
            if (!d.IsUsed(idx-1))
                continue;

            fArray[idx]          += d.fArray[idx];
            fBinEntries[idx]     += d.fBinEntries[idx];
            fSumw2.fArray[idx]   += d.fSumw2.fArray[idx];
        }
        // Underflow bin
        fArray[0]                += d.fArray[0];
        fBinEntries[0]           += d.fBinEntries[0];
        fSumw2.fArray[0]         += d.fSumw2.fArray[0];
        // Overflow bin
        fArray[fNcells-1]        += d.fArray[fNcells-1];
        fBinEntries[fNcells-1]   += d.fBinEntries[fNcells-1];
        fSumw2.fArray[fNcells-1] += d.fSumw2.fArray[fNcells-1];
        break;
/*    default:
        if (TestBit(kProfile)!=d.TestBit(kProfile))
            gLog << warn << "WARNING - You have tried to call AddCamContent for two different kind of histograms (kProfile set or not)." << endl;

        for (Int_t idx=0; idx<fNcells-2; idx++)
            Fill(idx, d.GetBinContent(idx+1));
        break;*/
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Call this function to add a TArrayD on top of the present contents.
//
void MHCamera::AddCamContent(const TArrayD &event, const TArrayC *used)
{
    if (event.GetSize()!=fNcells-2 || IsFreezed())
        return;

    if (used && used->GetSize()!=fNcells-2)
        return;

    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Fill(idx, event[idx]); // FIXME: Slow!

        if (!used || (*used)[idx])
            SetUsed(idx);
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Call this function to add a MArrayD on top of the present contents.
//
void MHCamera::AddCamContent(const MArrayD &event, const TArrayC *used)
{
    if (event.GetSize()!=(UInt_t)(fNcells-2) || IsFreezed())
        return;

    if (used && used->GetSize()!=fNcells-2)
        return;

    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Fill(idx, event[idx]); // FIXME: Slow!

        if (!used || (*used)[idx])
            SetUsed(idx);
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Call this function to add a MCamEvent on top of the present contents.
// 1 is added to each pixel if the contents of MCamEvent>threshold
//   (in case isabove is set to kTRUE == default)
// 1 is added to each pixel if the contents of MCamEvent<threshold
//   (in case isabove is set to kFALSE)
//
// in unused pixel is not counted if it didn't fullfill the condition.
//
void MHCamera::CntCamContent(const MCamEvent &event, Double_t threshold, Int_t type, Bool_t isabove)
{
    if (fNcells<=1 || IsFreezed())
        return;

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Double_t val=threshold;
        const Bool_t rc = event.GetPixelContent(val, idx, *fGeomCam, type);
        if (rc)
            SetUsed(idx);

        const Bool_t cond =
            ( isabove && val>threshold) ||
            (!isabove && val<threshold);

        Fill(idx, rc && cond ? 1 : 0);
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Call this function to add a MCamEvent on top of the present contents.
// - the contents of the pixels in event are added to each pixel
//   if the pixel of thresevt<threshold (in case isabove is set
//   to kTRUE == default)
// - the contents of the pixels in event are  added to each pixel
//   if the pixel of thresevt<threshold (in case isabove is set
//   to kFALSE)
//
// in unused pixel is not counted if it didn't fullfill the condition.
//
void MHCamera::CntCamContent(const MCamEvent &event, Int_t type1, const MCamEvent &thresevt, Int_t type2, Double_t threshold, Bool_t isabove)
{
    if (fNcells<=1 || IsFreezed())
        return;

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Double_t th=0;
        if (!thresevt.GetPixelContent(th, idx, *fGeomCam, type2))
            continue;

        if ((isabove && th>threshold) || (!isabove && th<threshold))
            continue;

        Double_t val=th;
        if (event.GetPixelContent(val, idx, *fGeomCam, type1))
        {
            SetUsed(idx);
            Fill(idx, val);
        }
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Call this function to add a MCamEvent on top of the present contents.
// 1 is added to each pixel if the contents of MCamEvent>threshold
//   (in case isabove is set to kTRUE == default)
// 1 is added to each pixel if the contents of MCamEvent<threshold
//   (in case isabove is set to kFALSE)
//
// in unused pixel is not counted if it didn't fullfill the condition.
//
void MHCamera::CntCamContent(const MCamEvent &event, TArrayD threshold, Int_t type, Bool_t isabove)
{
    if (fNcells<=1 || IsFreezed())
        return;

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Double_t val=threshold[idx];
        if (event.GetPixelContent(val, idx, *fGeomCam, type)/* && !IsUsed(idx)*/)
        {
            SetUsed(idx);

            if (val>threshold[idx] && isabove)
                Fill(idx);
            if (val<threshold[idx] && !isabove)
                Fill(idx);
        }
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Call this function to add a TArrayD on top of the present contents.
// 1 is added to each pixel if the contents of MCamEvent>threshold
//
void MHCamera::CntCamContent(const TArrayD &event, Double_t threshold, Bool_t ispos)
{
    if (event.GetSize()!=fNcells-2 || IsFreezed())
        return;

    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        if (event[idx]>threshold)
            Fill(idx);

        if (!ispos || fArray[idx+1]>0)
            SetUsed(idx);
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Call this function to add a MCamEvent on top of the present contents.
// 1 is added to each pixel if the contents of MCamEvent>threshold
//   (in case isabove is set to kTRUE == default)
// 1 is added to each pixel if the contents of MCamEvent<threshold
//   (in case isabove is set to kFALSE)
//
// in unused pixel is not counted if it didn't fullfill the condition.
//
void MHCamera::SetMaxCamContent(const MCamEvent &event, Int_t type)
{
    if (fNcells<=1 || IsFreezed())
        return;

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Double_t val=0;
        const Bool_t rc = event.GetPixelContent(val, idx, *fGeomCam, type);
        if (!rc)
            continue;

        if (!IsUsed(idx))
        {
            fArray[idx+1] = val;
            SetUsed(idx);
            fBinEntries.fArray[idx+1]=1;
        }
        else
            if (val>fArray[idx+1])
                fArray[idx+1] = val;
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Call this function to add a MCamEvent on top of the present contents.
// 1 is added to each pixel if the contents of MCamEvent>threshold
//   (in case isabove is set to kTRUE == default)
// 1 is added to each pixel if the contents of MCamEvent<threshold
//   (in case isabove is set to kFALSE)
//
// in unused pixel is not counted if it didn't fullfill the condition.
//
void MHCamera::SetMinCamContent(const MCamEvent &event, Int_t type)
{
    if (fNcells<=1 || IsFreezed())
        return;

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Double_t val=0;
        const Bool_t rc = event.GetPixelContent(val, idx, *fGeomCam, type);
        if (!rc)
            continue;

        if (!IsUsed(idx))
        {
            fArray[idx+1] = val;
            SetUsed(idx);
            fBinEntries.fArray[idx+1]=1;
        }
        else
            if (val<fArray[idx+1])
                fArray[idx+1] = val;
    }
    fEntries++;
}

// ------------------------------------------------------------------------
//
// Fill the pixels with random contents.
//
void MHCamera::FillRandom()
{
    if (fNcells<=1 || IsFreezed())
        return;

    Reset();

    // FIXME: Security check missing!
    for (Int_t idx=0; idx<fNcells-2; idx++)
    {
        Fill(idx, gRandom->Uniform()*fGeomCam->GetPixRatio(idx));
        SetUsed(idx);
    }
    fEntries=1;
}


// ------------------------------------------------------------------------
//
// The array must be in increasing order, eg: 2.5, 3.7, 4.9
// The values in each bin are replaced by the interval in which the value
// fits. In the example we have four intervals
// (<2.5, 2.5-3.7, 3.7-4.9, >4.9). Maximum and minimum are set
// accordingly.
//
void MHCamera::SetLevels(const TArrayF &arr)
{
    if (fNcells<=1)
        return;

    for (Int_t i=0; i<fNcells-2; i++)
    {
        if (!IsUsed(i))
            continue;

        Int_t j = arr.GetSize();
        while (j && fArray[i+1]<arr[j-1])
            j--;

        fArray[i+1] = j;
    }
    SetMaximum(arr.GetSize());
    SetMinimum(0);
}

// ------------------------------------------------------------------------
//
// Reset the all pixel colors to a default value
//
void MHCamera::Reset(Option_t *opt)
{
    if (fNcells<=1 || IsFreezed())
        return;

    TH1::Reset(opt);

    fUsed.Reset();
    fBinEntries.Reset();

    for (Int_t i=0; i<fNcells; i++)
        fArray[i] = 0;
} 

// ------------------------------------------------------------------------
//
//  Here we calculate the color index for the current value.
//  The color index is defined with the class TStyle and the
//  Color palette inside. We use the command gStyle->SetPalette(1,0)
//  for the display. So we have to convert the value "wert" into
//  a color index that fits the color palette.
//  The range of the color palette is defined by the values fMinPhe
//  and fMaxRange. Between this values we have 50 color index, starting
//  with 0 up to 49.
//
Int_t MHCamera::GetColor(Float_t val, Float_t min, Float_t max, Bool_t islog)
{
    //
    //   first treat the over- and under-flows
    //
    const Int_t ncol = GetContour()==0?50:GetContour();

    const Int_t maxcolidx = ncol-1;

    if (!TMath::Finite(val)) // FIXME: gLog!
        return maxcolidx/2;

    if (val >= max)
        return gStyle->GetColorPalette(maxcolidx);

    if (val <= min)
        return gStyle->GetColorPalette(0);

    //
    // calculate the color index
    //
    Float_t ratio;
    if (islog && min>0)
        ratio = log10(val/min) / log10(max/min);
    else
        ratio = (val-min) / (max-min);

    const Int_t colidx = TMath::FloorNint(ratio*ncol);
    return gStyle->GetColorPalette(colidx);
}

TPaveStats *MHCamera::GetStatisticBox()
{
    TObject *obj = 0;

    TIter Next(fFunctions);
    while ((obj = Next()))
        if (obj->InheritsFrom(TPaveStats::Class()))
            return static_cast<TPaveStats*>(obj);

    return NULL;
}

// ------------------------------------------------------------------------
//
//  Change the text on the legend according to the range of the Display
//
void MHCamera::UpdateLegend(Float_t min, Float_t max, Bool_t islog)
{
    const Float_t range = fGeomCam->GetMaxRadius();

    if (!TestBit(kNoScale))
    {
        TArrow arr;
        arr.PaintArrow(-range*.99, -range*.99, -range*.60, -range*.99, 0.010);
        arr.PaintArrow(-range*.99, -range*.99, -range*.99, -range*.60, 0.010);

        TString text;
        text += (int)(range*.3);
        text += "mm";

        TText newtxt2;
        newtxt2.SetTextSize(0.04);
        newtxt2.PaintText(-range*.95, -range*.95, text);

        text = MString::Format("%.2f\\circ", range*(0.99-0.60)*fGeomCam->GetConvMm2Deg());
        text = text.Strip(TString::kLeading);

        TLatex latex;
        latex.PaintLatex(-range*.95, -range*.85, 0, 0.04, text);
    }

    if (!TestBit(kNoLegend))
    {
        TPaveStats *stats = GetStatisticBox();

        const Float_t hndc   = 0.88 - (stats ? stats->GetY1NDC() : 1);
        const Float_t H      = (0.80-hndc)*range;
        const Float_t offset = hndc*range;

        const Int_t ncol = GetContour()==0 ? 50 : GetContour();

        const Float_t h = 2./ncol;
        const Float_t w = range/sqrt((float)(fNcells-2));

        TBox newbox;
        TText newtxt;
        newtxt.SetTextSize(0.03);
        newtxt.SetTextAlign(12);
#if ROOT_VERSION_CODE > ROOT_VERSION(3,01,06)
        newtxt.SetBit(/*kNoContextMenu|*/kCannotPick);
        newbox.SetBit(/*kNoContextMenu|*/kCannotPick);
#endif

        const Float_t step   = (islog && min>0 ? log10(max/min) : max-min);
        const Int_t   firsts = step/48*3 < 1e-8 ? 8 : (Int_t)floor(log10(step/48*3));
        const TString opt    = MString::Format("%%.%if", firsts>0 ? 0 : TMath::Abs(firsts));
/*
        for (Int_t i=0; i<ncol+1; i+=3)
        {
            Float_t val;
            if (islog && min>0)
                val = pow(10, step*i) * min;
            else
                val = min + step*i;

            //const bool dispexp = max-min>1.5 && fabs(val)>0.1 && fabs(val)<1e6;
            newtxt.PaintText(range+1.5*w, H*(i*h-1)-offset, Form(opt, val));
            }
            */
        const MBinning bins(25, min, max, islog&&min>0?"log":"lin");

        for (Int_t i=0; i<=25; i++)
            newtxt.PaintText(range+1.5*w, H*(i*ncol/25*h-1)-offset, MString::Format(opt, bins[i]));

        for (Int_t i=0; i<ncol; i++)
        {
            newbox.SetFillColor(gStyle->GetColorPalette(i));
            newbox.PaintBox(range, H*(i*h-1)-offset, range+w, H*((i+1)*h-1)-offset);
        }
    }
}

// ------------------------------------------------------------------------
//
// Save primitive as a C++ statement(s) on output stream out
//
void MHCamera::SavePrimitive(ostream &out, Option_t *opt)
{
    gLog << err << "MHCamera::SavePrimitive: Must be rewritten!" << endl;
    /*
    if (!gROOT->ClassSaved(TCanvas::Class()))
        fDrawingPad->SavePrimitive(out, opt);

    out << "   " << fDrawingPad->GetName() << "->SetWindowSize(";
    out << fDrawingPad->GetWw() << "," << fDrawingPad->GetWh() << ");" << endl;
    */
}

void MHCamera::SavePrimitive(ofstream &out, Option_t *)
{
    MHCamera::SavePrimitive(static_cast<ostream&>(out), "");
}

// ------------------------------------------------------------------------
//
// compute the distance of a point (px,py) to the Camera
// this functions needed for graphical primitives, that
// means without this function you are not able to interact
// with the graphical primitive with the mouse!!!
//
// All calcutations are done in pixel coordinates
//
Int_t MHCamera::DistancetoPrimitive(Int_t px, Int_t py)
{
    if (fNcells<=1)
        return 999999;

    TPaveStats *box = (TPaveStats*)gPad->GetPrimitive("stats");
    if (box)
    {
        const Double_t w = box->GetY2NDC()-box->GetY1NDC();
        box->SetX1NDC(gStyle->GetStatX()-gStyle->GetStatW());
        box->SetY1NDC(gStyle->GetStatY()-w);
        box->SetX2NDC(gStyle->GetStatX());
        box->SetY2NDC(gStyle->GetStatY());
    }

    if (TString(GetDrawOption()).Contains("hist", TString::kIgnoreCase))
        return TH1D::DistancetoPrimitive(px, py);

    const Bool_t issame = TString(GetDrawOption()).Contains("same", TString::kIgnoreCase);

    const Float_t maxr = (1-fGeomCam->GetConvMm2Deg())*fGeomCam->GetMaxRadius()/2;
    const Float_t conv = !issame ||
        gPad->GetX1()<-maxr || gPad->GetY1()<-maxr ||
        gPad->GetX2()> maxr || gPad->GetY2()>maxr ? 1 : fGeomCam->GetConvMm2Deg();

    if (GetPixelIndex(px, py, conv)>=0)
        return 0;

    if (!box)
        return 999999;

    const Int_t dist = box->DistancetoPrimitive(px, py);
    if (dist > TPad::GetMaxPickDistance())
        return 999999;

    gPad->SetSelected(box);
    return dist;
}

// ------------------------------------------------------------------------
//
//
Int_t MHCamera::GetPixelIndex(Int_t px, Int_t py, Float_t conv) const
{
    if (fNcells<=1)
        return -1;

    for (Int_t i=0; i<fNcells-2; i++)
        if ((*fGeomCam)[i].DistancetoPrimitive(TMath::Nint(px*conv), TMath::Nint(py*conv))<=0)
            return i;

    return -1;
}

// ------------------------------------------------------------------------
//
// Returns string containing info about the object at position (px,py).
// Returned string will be re-used (lock in MT environment).
//
char *MHCamera::GetObjectInfo(Int_t px, Int_t py) const
{
    if (TString(GetDrawOption()).Contains("hist", TString::kIgnoreCase))
#if ROOT_VERSION_CODE < ROOT_VERSION(5,22,00)
        return MH::GetObjectInfoH(px, py, *this);
#else
        return TH1D::GetObjectInfo(px, py);
#endif

    static char info[128];

    const Int_t idx=GetPixelIndex(px, py);

    if (idx<0)
        return TObject::GetObjectInfo(px, py);

    sprintf(info, "Software Pixel Idx: %d (Hardware Id=%d) c=%.1f <%s>",
            idx, idx+1, GetBinContent(idx+1), IsUsed(idx)?"on":"off");
    return info;
}

// ------------------------------------------------------------------------
//
// Add a MCamEvent which should be displayed when the user clicks on a
// pixel.
// Warning: The object MUST inherit from TObject AND MCamEvent
//
void MHCamera::AddNotify(TObject *obj)
{
    // Make sure, that the object derives from MCamEvent!
    MCamEvent *evt = dynamic_cast<MCamEvent*>(obj);
    if (!evt)
    {
        gLog << err << "ERROR: MHCamera::AddNotify - TObject doesn't inherit from MCamEvent... ignored." << endl;
        return;
    }

    // Make sure, that it is deleted from the list too, if the obj is deleted
    obj->SetBit(kMustCleanup);

    // Add object to list
    fNotify->Add(obj);
}

// ------------------------------------------------------------------------
//
// Execute a mouse event on the camera
//
void MHCamera::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
    if (TString(GetDrawOption()).Contains("hist", TString::kIgnoreCase))
    {
        TH1D::ExecuteEvent(event, px, py);
        return;
    }
    //if (event==kMouseMotion && fStatusBar)
    //    fStatusBar->SetText(GetObjectInfo(px, py), 0);
    if (event!=kButton1Down)
        return;

    const Int_t idx = GetPixelIndex(px, py);
    if (idx<0)
        return;

    gLog << all << GetTitle() << " <" << GetName() << ">" << dec << endl;
    gLog << "Software Pixel Idx: " << idx << endl;
    gLog << "Hardware Pixel Id:  " << idx+1 << endl;
    gLog << "Contents:           " << GetBinContent(idx+1);
    if (GetBinError(idx+1)>0)
        gLog << " +/- " << GetBinError(idx+1);
    gLog << "  <" << (IsUsed(idx)?"on":"off") << ">  n=" << fBinEntries[idx+1] << endl;

    if (fNotify && fNotify->GetSize()>0)
    {
        // FIXME: Is there a simpler and more convinient way?

        // The name which is created here depends on the instance of
        // MHCamera and on the pad on which it is drawn --> The name
        // is unique. For ExecuteEvent gPad is always correctly set.
        const TString name = MString::Format("%p;%p;PixelContent", this, gPad);

        TCanvas *old = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(name);
        if (old)
            old->cd();
        else
            new TCanvas(name);

        /*
         TIter Next(gPad->GetListOfPrimitives());
         TObject *o;
         while (o=Next()) cout << o << ": " << o->GetName() << " " << o->IsA()->GetName() << endl;
         */

        // FIXME: Make sure, that the old histograms are really deleted.
        //        Are they already deleted?

        // The dynamic_cast is necessary here: We cannot use ForEach
        TIter Next(fNotify);
        MCamEvent *evt;
        while ((evt=dynamic_cast<MCamEvent*>(Next())))
            evt->DrawPixelContent(idx);

        gPad->Modified();
        gPad->Update();
    }
}

UInt_t MHCamera::GetNumPixels() const
{
    return fGeomCam ? fGeomCam->GetNumPixels() : 0;
}

// --------------------------------------------------------------------------
//
// Draw a projection of MHCamera onto the y-axis values. Depending on the 
// variable fit, the following fits are performed:
//
// 0: No fit, simply draw the projection
// 1: Single Gauss (for distributions flat-fielded over the whole camera)
// 2: Double Gauss (for distributions different for inner and outer pixels)
// 3: Triple Gauss (for distributions with inner, outer pixels and outliers)
// 4: flat         (for the probability distributions)
// (1-4:) Moreover, sectors 6,1 and 2 of the camera and sectors 3,4 and 5 are 
//        drawn separately, for inner and outer pixels.
// 5: Fit Inner and Outer pixels separately by a single Gaussian 
//                 (only for MAGIC cameras)
// 6: Fit Inner and Outer pixels separately by a single Gaussian and display 
//                 additionally the two camera halfs separately (for MAGIC camera)
// 7: Single Gauss with TLegend to show the meaning of the colours
//
void MHCamera::DrawProjection(Int_t fit) const
{
    if (fit==5 || fit==6)
    {
        const UInt_t n = fGeomCam->GetNumAreas();

        TVirtualPad *pad = gPad;
        pad->Divide(n, 1, 1e-5, 1e-5);;

        for (UInt_t i=0; i<n; i++)
        {
            pad->cd(i+1);
            gPad->SetBorderMode(0);
            gPad->SetRightMargin(0.025);
            gPad->SetTicks();

            TH1D &h = *ProjectionS(TArrayI(), TArrayI(1, (Int_t*)&i),
                                   MString::Format("%s_%d", GetName(), i));
            h.SetTitle(MString::Format("%s %d", GetTitle(), i));
            h.SetDirectory(NULL);
            h.SetBit(kCanDelete);
            h.Draw();

            TAxis *xaxe = h.GetXaxis();
            TAxis *yaxe = h.GetYaxis();

            xaxe->CenterTitle();
            yaxe->CenterTitle();
            xaxe->SetTitleSize(0.06);
            yaxe->SetTitleSize(0.06);
            xaxe->SetTitleOffset(0.8);
            yaxe->SetTitleOffset(0.85);
            xaxe->SetLabelSize(0.05);
            yaxe->SetLabelSize(0.05);
            if (i>0)
                yaxe->SetTitle("");

            if (fit==5)
                continue;

            h.Fit("gaus", "Q");

            TF1 *f = h.GetFunction("gaus");
            if (f)
            {
                f->SetLineWidth(2);
                f->SetLineColor(kBlue);
            }
        }
        return;
    }

    TH1D *obj2 = (TH1D*)Projection(GetName());
    obj2->SetDirectory(0);
    obj2->Draw();
    obj2->SetBit(kCanDelete);

    if (fit==0)
        return;

    const Double_t xmin  = obj2->GetBinCenter(obj2->GetXaxis()->GetFirst());
    const Double_t xmax  = obj2->GetBinCenter(obj2->GetXaxis()->GetLast());
    const Double_t integ = obj2->Integral("width")/2.5;
    const Double_t max   = obj2->GetMaximum();
    const Double_t mean  = obj2->GetMean();
    const Double_t rms   = obj2->GetRMS();
    const Double_t width = xmax-xmin;

    const char *dgausformula = "([0]-[3])/[2]*exp(-0.5*(x-[1])*(x-[1])/[2]/[2])"
        "+[3]/[5]*exp(-0.5*(x-[4])*(x-[4])/[5]/[5])";

    const char *tgausformula = "([0]-[3]-[6])/[2]*exp(-0.5*(x-[1])*(x-[1])/[2]/[2])"
        "+[3]/[5]*exp(-0.5*(x-[4])*(x-[4])/[5]/[5])"
        "+[6]/[8]*exp(-0.5*(x-[7])*(x-[7])/[8]/[8])";

    TF1 *f=0;
    switch (fit)
    {
    case 1: // Single gauss
        f = new TF1("sgaus", "gaus", xmin, xmax);
        f->SetLineColor(kBlue);
        f->SetBit(kCanDelete);
        f->SetParNames("Max", "#mu", "#sigma");
        f->SetParameters(max, mean, rms);
        f->SetParLimits(0, 0,    max*2);
        f->SetParLimits(1, xmin, xmax);
        f->SetParLimits(2, 0,    width/1.5);

        obj2->Fit(f, "QLR");
        break;

    case 2: // Double gauss
        f = new TF1("dgaus", dgausformula, xmin, xmax);
        f->SetLineColor(kBlue);
        f->SetBit(kCanDelete);
        f->SetParNames("A_{tot}", "#mu1", "#sigma1", "A2", "#mu2", "#sigma2");
        f->SetParameters(integ, (xmin+mean)/2., width/4.,
                         integ/width/2., (xmax+mean)/2., width/4.);
        // The left-sided Gauss
        f->SetParLimits(0, integ-1.5,        integ+1.5);
        f->SetParLimits(1, xmin+(width/10.), mean);
        f->SetParLimits(2, 0,                width/2.);
        // The right-sided Gauss
        f->SetParLimits(3, 0,    integ);
        f->SetParLimits(4, mean, xmax-(width/10.));
        f->SetParLimits(5, 0,    width/2.);
        obj2->Fit(f,"QLRM");
        break;

    case 3: // Triple gauss
        f = new TF1("tgaus", tgausformula, xmin,xmax);
        f->SetLineColor(kBlue);
        f->SetBit(kCanDelete);
        f->SetParNames("A_{tot}","#mu_{1}","#sigma_{1}",
                       "A_{2}","#mu_{2}","#sigma_{2}",
                       "A_{3}","#mu_{3}","#sigma_{3}");
        f->SetParameters(integ, (xmin+mean)/2, width/4.,
                         integ/width/3., (xmax+mean)/2., width/4.,
                         integ/width/3., mean,width/2.);
        // The left-sided Gauss
        f->SetParLimits(0, integ-1.5,        integ+1.5);
        f->SetParLimits(1, xmin+(width/10.), mean);
        f->SetParLimits(2, width/15.,        width/2.);
        // The right-sided Gauss
        f->SetParLimits(3, 0.,               integ);
        f->SetParLimits(4, mean,             xmax-(width/10.));
        f->SetParLimits(5, width/15.,        width/2.);
        // The Gauss describing the outliers
        f->SetParLimits(6, 0.,               integ);
        f->SetParLimits(7, xmin,             xmax);
        f->SetParLimits(8, width/4.,         width/1.5);
        obj2->Fit(f,"QLRM");
        break;

    case 4:
        obj2->Fit("pol0", "Q");
        if (obj2->GetFunction("pol0"))
            obj2->GetFunction("pol0")->SetLineColor(kBlue);
        break;

    case 9:
        break;

    default:
        obj2->Fit("gaus", "Q");
        if (obj2->GetFunction("gaus"))
            obj2->GetFunction("gaus")->SetLineColor(kBlue);
        break;
    }
}

// --------------------------------------------------------------------------
//
// Draw a projection of MHCamera vs. the radius from the central pixel. 
//
// The inner and outer pixels are drawn separately, both fitted by a polynomial
// of grade 1.
//
void MHCamera::DrawRadialProfile() const
{
    TProfile *obj2 = (TProfile*)RadialProfile(GetName());
    obj2->SetDirectory(0);
    obj2->Draw();
    obj2->SetBit(kCanDelete);
/*
    if (GetGeomCam().InheritsFrom("MGeomCamMagic"))
    {
        TArrayI s0(6);
        s0[0] = 1;
        s0[1] = 2;
        s0[2] = 3;
        s0[3] = 4;
        s0[4] = 5;
        s0[5] = 6;

        TArrayI inner(1);
        inner[0] = 0;

        TArrayI outer(1);
        outer[0] = 1;

        // Just to get the right (maximum) binning
        TProfile *half[2];
        half[0] = RadialProfileS(s0, inner, MString::Format("%sInner",GetName()));
        half[1] = RadialProfileS(s0, outer, MString::Format("%sOuter",GetName()));

        for (Int_t i=0; i<2; i++)
        {
            Double_t min = GetGeomCam().GetMinRadius(i);
            Double_t max = GetGeomCam().GetMaxRadius(i);

            half[i]->SetLineColor(kRed+i);
            half[i]->SetDirectory(0);
            half[i]->SetBit(kCanDelete);
            half[i]->Draw("same");
            half[i]->Fit("pol1","Q","",min,max);
            half[i]->GetFunction("pol1")->SetLineColor(kRed+i);
            half[i]->GetFunction("pol1")->SetLineWidth(1);
        }
    }
*/
}

// --------------------------------------------------------------------------
//
// Draw a projection of MHCamera vs. the azimuth angle inside the camera.
//
// The inner and outer pixels are drawn separately. 
// The general azimuth profile is fitted by a straight line
//
void MHCamera::DrawAzimuthProfile() const
{
    TProfile *obj2 = (TProfile*)AzimuthProfile(GetName());
    obj2->SetDirectory(0);
    obj2->Draw();
    obj2->SetBit(kCanDelete);
    obj2->Fit("pol0","Q","");
    if (obj2->GetFunction("pol0"))
        obj2->GetFunction("pol0")->SetLineWidth(1);
/*
    if (GetGeomCam().InheritsFrom("MGeomCamMagic"))
    {
        TArrayI inner(1);
        inner[0] = 0;

        TArrayI outer(1);
        outer[0] = 1;

        // Just to get the right (maximum) binning
        TProfile *half[2];
        half[0] = AzimuthProfileA(inner, MString::Format("%sInner",GetName()));
        half[1] = AzimuthProfileA(outer, MString::Format("%sOuter",GetName()));

        for (Int_t i=0; i<2; i++)
        {
            half[i]->SetLineColor(kRed+i);
            half[i]->SetDirectory(0);
            half[i]->SetBit(kCanDelete);
            half[i]->SetMarkerSize(0.5);
            half[i]->Draw("same");
        }
    }
*/
}

// --------------------------------------------------------------------------
//
// Draw the MHCamera into the MStatusDisplay: 
// 
// 1) Draw it as histogram (MHCamera::DrawCopy("hist")
// 2) Draw it as a camera, with MHCamera::SetPrettyPalette() set. 
// 3) If "rad" is not zero, draw its values vs. the radius from the camera center. 
//    (DrawRadialProfile())
// 4) Depending on the variable "fit", draw the values projection on the y-axis
//    (DrawProjection()):
//    0: don't draw
//    1: Draw fit to Single Gauss (for distributions flat-fielded over the whole camera)
//    2: Draw and fit to Double Gauss (for distributions different for inner and outer pixels)
//    3: Draw and fit to Triple Gauss (for distributions with inner, outer pixels and outliers)
//    4: Draw and fit to Polynomial grade 0: (for the probability distributions)
//    >4: Draw and don;t fit.
//
void MHCamera::CamDraw(TCanvas &c, const Int_t x, const Int_t y, 
                       const Int_t fit, const Int_t rad, const Int_t azi,
                       TObject *notify)
{
    c.cd(x);
    gPad->SetBorderMode(0);
    gPad->SetRightMargin(0.02);
    gPad->SetTicks();
    MHCamera *obj1=(MHCamera*)DrawCopy("hist");
    obj1->SetDirectory(NULL);
    obj1->SetStats(kFALSE);

    if (notify)
        obj1->AddNotify(notify);

    c.cd(x+y);
    gPad->SetBorderMode(0);
    obj1->SetPrettyPalette();
    obj1->Draw();

    Int_t cnt = 2;

    if (rad)
    {
        c.cd(x+2*y);
        gPad->SetBorderMode(0);
        gPad->SetTicks();
        DrawRadialProfile();
        cnt++;
    }

    if (azi)
    {
        c.cd(x+cnt*y);
        gPad->SetBorderMode(0);
        gPad->SetTicks();
        DrawAzimuthProfile();
        cnt++;
    }

    if (fit<0)
        return;

    c.cd(x + cnt*y);
    gPad->SetBorderMode(0);
    gPad->SetRightMargin(0.025);
    gPad->SetTicks();
    DrawProjection(fit);
}
