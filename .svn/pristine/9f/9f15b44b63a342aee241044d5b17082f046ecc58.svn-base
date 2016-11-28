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
!   Author(s): Keiichi Mase, 10/2004
!   Author(s): Markus Meyer, 02/2005 <mailto:meyer@astro.uni-wuerzburg.de>
!   Author(s): Thomas Bretz, 04/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHSingleMuon
//
// This class is a histogram class for displaying the radial (fHistWidth)
// and the azimuthal (fHistPhi) intensity distribution for one muon.
// You can retrieve the histogram (TH1F) using the function GetHistPhi()
// or GetHistWidth().
// From these histograms the fraction of the ring segment (ArcPhi) and the
// Width of the muon ring (ArcWidth) is calculated.
//
// First, the radius and center of the ring has to be calculted by
// MMuonSearchParCalc
// After that the histograms has to be filled in the following way:
//
// MFillH fillmuon("MHSingleMuon", "", "FillMuon");
//
// The allowed region to estimate ArcPhi is a certain margin around the
// radius. The default value is 0.2 deg (60mm). If the estimated radius
// of the arc is 1.0 deg, the pixel contents in the radius range from
// 0.8 deg to 1.2 deg are fill in the histogram.
//
// For ArcPhi only bins over a certain threshold are supposed to be part
// of the ring.
// For ArcWidth, the same algorithm is used to determine the fit region
// for a gaussian fit to the radial intensity distribution. The ArcWidth
// is defined as the sigma value of the gaussian fit.
//
// The binning of the histograms can be changed in the following way:
//
// MBinning bins1("BinningMuonWidth");
// MBinning bins2("BinningArcPhi");
// bins1.SetEdges(28, 0.3, 1.7);
// bins2.SetEdges(20, -180,180);
// plist.AddToList(&bins1);
// plist.AddToList(&bins2);
//
// The values for the thresholds and the margin are saved in MMuonSetup.
// They can be easily changed in star.rc.
//
// Please have in mind, that changes in this basic parameters will change
// your results!!
//
// InputContainer:
//   - MGeomCam
//   - MMuonSearchPar
//
//
// Class Version 2:
// ----------------
//   + Double_t fRelTimeMean;   // Result of the gaus fit to the arrival time
//   + Double_t fRelTimeSigma;  // Result of the gaus fit to the arrival time
//
////////////////////////////////////////////////////////////////////////////
#include "MHSingleMuon.h"

#include <TF1.h>
#include <TPad.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MBinning.h"
#include "MParList.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MSignalCam.h"
#include "MSignalPix.h"

#include "MMuonSetup.h"
#include "MMuonCalibPar.h"
#include "MMuonSearchPar.h"

ClassImp(MHSingleMuon);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup histograms 
//
MHSingleMuon::MHSingleMuon(const char *name, const char *title) :
    fSignalCam(0), fMuonSearchPar(0), fGeomCam(0), fMargin(0)
{
    fName  = name  ? name  : "MHSingleMuon";
    fTitle = title ? title : "Histograms of muon parameters";

    fHistPhi.SetName("HistPhi");
    fHistPhi.SetTitle("HistPhi");
    fHistPhi.SetXTitle("\\phi [\\circ]");
    fHistPhi.SetYTitle("sum of ADC");
    fHistPhi.SetDirectory(NULL);
    fHistPhi.SetFillStyle(4000);
    fHistPhi.UseCurrentStyle();

    fHistWidth.SetName("HistWidth");
    fHistWidth.SetTitle("HistWidth");
    fHistWidth.SetXTitle("distance from the ring center [\\circ]");
    fHistWidth.SetYTitle("sum of ADC");
    fHistWidth.SetDirectory(NULL);
    fHistWidth.SetFillStyle(4000);
    fHistWidth.UseCurrentStyle();

    fHistTime.SetName("HistTime");
    fHistTime.SetTitle("HistTime");
    fHistTime.SetXTitle("timing difference");
    fHistTime.SetYTitle("Counts");
    fHistTime.SetDirectory(NULL);
    fHistTime.SetFillStyle(4000);
    fHistTime.UseCurrentStyle();

    MBinning bins;
    bins.SetEdges(20, -180, 180);
    bins.Apply(fHistPhi);

    bins.SetEdges(28, 0.3, 1.7);
    bins.Apply(fHistWidth);

    bins.SetEdges(101, -33, 33);   // +/- 33ns
    bins.Apply(fHistTime);
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning
//
Bool_t MHSingleMuon::SetupFill(const MParList *plist)
{
    fGeomCam = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fGeomCam)
    {
        *fLog << warn << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }
    fMuonSearchPar = (MMuonSearchPar*)plist->FindObject("MMuonSearchPar");
    if (!fMuonSearchPar)
    {
        *fLog << warn << "MMuonSearchPar not found... abort." << endl;
        return kFALSE;
    }
    fSignalCam = (MSignalCam*)plist->FindObject("MSignalCam");
    if (!fSignalCam)
    {
        *fLog << warn << "MSignalCam not found... abort." << endl;
        return kFALSE;
    }

    MMuonSetup *setup = (MMuonSetup*)const_cast<MParList*>(plist)->FindCreateObj("MMuonSetup");
    if (!setup)
        return kFALSE;

    fMargin = setup->GetMargin()/fGeomCam->GetConvMm2Deg();

    ApplyBinning(*plist, "ArcPhi",    fHistPhi);
    ApplyBinning(*plist, "MuonWidth", fHistWidth);
    ApplyBinning(*plist, "MuonTime",  fHistTime);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MMuonCalibPar and
// MMuonSearchPar container.
//
Int_t MHSingleMuon::Fill(const MParContainer *par, const Stat_t w)
{
    fRelTimeMean  =  0;
    fRelTimeSigma = -1;

    fHistPhi.Reset();
    fHistWidth.Reset();
    fHistTime.Reset();

    const Int_t entries = fSignalCam->GetNumPixels();

    // the position of the center of a muon ring
    const Float_t cenx = fMuonSearchPar->GetCenterX();
    const Float_t ceny = fMuonSearchPar->GetCenterY();

    for (Int_t i=0; i<entries; i++)
    {
        const MSignalPix &pix  = (*fSignalCam)[i];
        const MGeom      &gpix = (*fGeomCam)[i];

        const Float_t dx = gpix.GetX() - cenx;
        const Float_t dy = gpix.GetY() - ceny;

        const Float_t dist = TMath::Hypot(dx, dy);

        // if the signal is not near the estimated circle, it is ignored.
        if (TMath::Abs(dist-fMuonSearchPar->GetRadius())<fMargin)
        {
            // The arrival time is aligned around 0 for smaller
            // and more stable histogram range
            fHistTime.Fill(pix.GetArrivalTime()-fMuonSearchPar->GetTime());
        }

        // use only the inner pixles. FIXME: This is geometry dependent
        if (gpix.GetAidx()>0)
            continue;

        fHistWidth.Fill(dist*fGeomCam->GetConvMm2Deg(), pix.GetNumPhotons());
    }

    // Setup the function and perform the fit
    TF1 g1("g1", "gaus");//, -fHistTime.GetXmin(), fHistTime.GetXmax());

    // Choose starting values as accurate as possible
    g1.SetParameter(0, fHistTime.GetMaximum());
    g1.SetParameter(1, 0);
    g1.SetParameter(2, 0.7); // FIXME! GetRMS instead???

    // According to fMuonSearchPar->GetTimeRMS() identified muons
    // do not have an arrival time rms>3
    g1.SetParLimits(1, -1.7, 1.7);
    g1.SetParLimits(2,  0,   3.4);

    // options : N  do not store the function, do not draw
    //           I  use integral of function in bin rather than value at bin center
    //           R  use the range specified in the function range
    //           Q  quiet mode
    if (fHistTime.Fit(&g1, "QNB"))
        return kTRUE;

    fRelTimeMean  = g1.GetParameter(1);
    fRelTimeSigma = g1.GetParameter(2);

    // The mean arrival time which was subtracted before will
    // be added again, now
    const Double_t tm0 = fMuonSearchPar->GetTime()+fRelTimeMean;

    for (Int_t i=0; i<entries; i++)
    {
        const MSignalPix &pix  = (*fSignalCam)[i];
        const MGeom      &gpix = (*fGeomCam)[i];

        const Float_t dx = gpix.GetX() - cenx;
        const Float_t dy = gpix.GetY() - ceny;

        const Float_t dist = TMath::Hypot(dx, dy);

        // if the signal is not near the estimated circle, it is ignored.
        if (TMath::Abs(dist-fMuonSearchPar->GetRadius())<fMargin &&
            TMath::Abs(pix.GetArrivalTime()-tm0) < 2*fRelTimeSigma)
        {
            fHistPhi.Fill(TMath::ATan2(dx, dy)*TMath::RadToDeg(), pix.GetNumPhotons());
        }
    }

    return kTRUE;

/*
    // Because the errors (sqrt(content)) are only scaled by a fixed
    // factor, and the absolute value of the error is nowhere
    // needed we skip this step

    // error estimation (temporarily)
    //  The error is estimated from the signal. In order to do so, we have to
    // once convert the signal from ADC to photo-electron. Then we can get
    // the fluctuation such as F-factor*sqrt(phe).
    //  Up to now, the error of pedestal is not taken into accout. This is not
    // of course correct. We will include this soon.
    const Double_t Ffactor  = 1.4;
    for (Int_t i=0; i<fHistPhi.GetNbinsX()+1; i++)
        fHistPhi.SetBinError(i, fHistPhi.GetBinError(i)*Ffactor);

    for (Int_t i=0; i<fHistWidth.GetNbinsX()+1; i++)
        fHistWidth.SetBinError(i, fHistWidth.GetBinError(i)*Ffactor);

    return kTRUE;
 */
}

// --------------------------------------------------------------------------
//
// Find the first bins starting at the bin with maximum content in both
// directions which are below threshold.
// If in a range of half the histogram size in both directions no bin
// below the threshold is found, kFALSE is returned.
//
Bool_t MHSingleMuon::FindRangeAboveThreshold(const TProfile &h, Float_t thres, Int_t &first, Int_t &last) const
{
    const Int_t n      = h.GetNbinsX();
    const Int_t maxbin = h.GetMaximumBin();
    const Int_t edge   = maxbin+n/2;

    // Search from the peak to the right
    last = -1;
    for (Int_t i=maxbin; i<edge; i++)
    {
        const Float_t val = h.GetBinContent(i%n + 1);
        if (val<thres)
        {
            last = i%n+1;
            break;
        }
    }

    // Search from the peak to the left
    first = -1;
    for (Int_t i=maxbin+n-1; i>=edge; i--)
    {
        const Float_t val = h.GetBinContent(i%n + 1);
        if (val<thres)
        {
            first = i%n+1;
            break;
        }
    }

    return first>=0 && last>=0;
}

// --------------------------------------------------------------------------
//
//  Photon distribution along the estimated circle is fitted with theoritical
// function in order to get some more information such as Arc Phi and Arc 
// Length.
//
Bool_t MHSingleMuon::CalcPhi(Double_t thres, Double_t &peakphi, Double_t &arcphi) const
{
    if (fHistPhi.GetMaximum()<thres)
        return kFALSE;

    peakphi = 180.-fHistPhi.GetBinCenter(fHistPhi.GetMaximumBin());

    // Now find the position at which the peak edges crosses the threshold
    Int_t first, last;

    FindRangeAboveThreshold(fHistPhi, thres, first, last);

    const Int_t n    = fHistPhi.GetNbinsX();
    const Int_t edge = fHistPhi.GetMaximumBin()+n/2;
    if (first<0)
        first = (edge-1)%n+1;
    if (last<0)
        last  = edge%n+1;;

    const Float_t startfitval = fHistPhi.GetBinLowEdge(first+1);
    const Float_t endfitval   = fHistPhi.GetBinLowEdge(last);

    arcphi = last-1<first ? 360+(endfitval-startfitval) : endfitval-startfitval;

    //if (fEnableImpactCalc)
    //    CalcImpact(effbinnum, startfitval, endfitval);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Photon distribution of distance from the center of estimated ring is
// fitted in order to get some more information such as ARC WIDTH which 
// can represent to the PSF of our reflector.
//
// thres: Threshold above zero to determin the edges of the peak which
//        is used as fit range
// width: ArcWidth returned in deg
// chi:   Chi^2/NDF of the fit
//
Bool_t MHSingleMuon::CalcWidth(Double_t thres, Double_t &width, Double_t &chi)
{
    Int_t first, last;

    if (!FindRangeAboveThreshold(fHistWidth, thres, first, last))
        return kFALSE;

    // This happens in some cases
    const Int_t n = fHistWidth.GetNbinsX()/2;
    const Int_t m = fHistWidth.GetMaximumBin();
    if (first>last)
    {
        if (m>n)       // If maximum is on the right side of histogram
            last = n;
        else
            first = 0; // If maximum is on the left side of histogram
    }

    if (last-first<=3)
        return kFALSE;

    // Now get the fit range
    const Float_t startfitval = fHistWidth.GetBinLowEdge(first+1);
    const Float_t endfitval   = fHistWidth.GetBinLowEdge(last);

    // Setup the function and perform the fit
    TF1 f1("f1", "gaus + [3]", startfitval, endfitval);
    f1.SetLineColor(kBlue);

    // Choose starting values as accurate as possible
    f1.SetParameter(0, fHistWidth.GetMaximum());
    f1.SetParameter(1, fHistWidth.GetBinCenter(m));
//    f1.SetParameter(2, (endfitval-startfitval)/2);
    f1.SetParameter(2, 0.1);
    f1.SetParameter(3, 1.8);

    // options : N  do not store the function, do not draw
    //           I  use integral of function in bin rather than value at bin center
    //           R  use the range specified in the function range
    //           Q  quiet mode
//    fHistWidth.Fit(&f1, "QRO");
    if (fHistWidth.Fit(&f1, "QRN"))
        return kFALSE;

    chi   = f1.GetChisquare()/f1.GetNDF();
    width = f1.GetParameter(2);

    return kTRUE;
}

/*
// --------------------------------------------------------------------------
//
// An impact parameter is calculated by fitting the histogram of photon
// distribution along the circle with a theoritical model. 
// (See G. Vacanti et. al., Astroparticle Physics 2, 1994, 1-11. 
// The function (6) is used here.) 
//
// By default this calculation is suppressed because this calculation is
// very time consuming. If you want to calculate an impact parameter,
// you can call the function of EnableImpactCalc().
//
void MMuonCalibParCalc::CalcImpact(Int_t effbinnum, Float_t startfitval, Float_t endfitval)
{
  // Fit the distribution with Vacanti function. The function is different
  // for the impact parameter of inside or outside of our reflector. 
  // Then two different functions are applied to the photon distribution,
  // and the one which give us smaller chisquare value is taken as a 
  // proper one.

    Double_t val1,err1,val2,err2;
    // impact parameter inside mirror radius (8.5m)
    TString func1;
    Float_t tmpval = (*fMuonSearchPar).GetRadius()*(*fGeomCam).GetConvMm2Deg()*TMath::DegToRad();
    tmpval = sin(2.*tmpval)*8.5;
    func1 += "[0]*";
    func1 += tmpval;
    func1 += "*(sqrt(1.-([1]/8.5)**2*sin((x-[2])*3.1415926/180.)**2)+([1]/8.5)*cos((x-[2])*3.1415926/180.))";

    TF1 f1("f1",func1,startfitval,endfitval);
    f1.SetParameters(2000,3,0);
    f1.SetParLimits(1,0,8.5);
    f1.SetParLimits(2,-180.,180.);

    fMuonCalibPar->fHistPhi->Fit("f1","RQEM");

    Float_t chi1 = -1;
    Float_t chi2 = -1;
    if(effbinnum>3)
        chi1 = f1.GetChisquare()/((Float_t)(effbinnum-3));

    gMinuit->GetParameter(1,val1,err1);  // get the estimated IP
    Float_t estip1 = val1;

    // impact parameter beyond mirror area (8.5m)
    TString func2;
    Float_t tmpval2 = (*fMuonSearchPar).GetRadius()*(*fGeomCam).GetConvMm2Deg()*TMath::DegToRad();
    tmpval2 = sin(2.*tmpval2)*8.5*2.;
    func2 += "[0]*";
    func2 += tmpval2;
    func2 += "*sqrt(1.-(([1]/8.5)*sin((x-[2])*3.1415926/180.))**2)";

    TF1 f2("f2",func2,startfitval,endfitval);
    f2.SetParameters(2000,20,0);
    f2.SetParLimits(1,8.5,300.);
    f2.SetParLimits(2,-180.,180.);

    fMuonCalibPar->fHistPhi->Fit("f2","RQEM+");

    if(effbinnum>3)
        chi2 = f2.GetChisquare()/((Float_t)(effbinnum-3));

    gMinuit->GetParameter(1,val2,err2);  // get the estimated IP
    Float_t estip2 = val2;

    if(effbinnum<=3)
    {
        fMuonCalibPar->SetEstImpact(-1.);
    }
    if(chi2 > chi1)
    {
        fMuonCalibPar->SetEstImpact(estip1);
        fMuonCalibPar->SetChiArcPhi(chi1);
    }
    else
    {
        fMuonCalibPar->SetEstImpact(estip2);
        fMuonCalibPar->SetChiArcPhi(chi2);
    }
}
*/

Float_t MHSingleMuon::CalcSize() const
{
    const Int_t n = fHistPhi.GetNbinsX();

    Double_t sz=0;
    for (Int_t i=1; i<=n; i++)
        sz += fHistPhi.GetBinContent(i)*fHistPhi.GetBinEntries(i);

    return sz;
}

void MHSingleMuon::Paint(Option_t *o)
{
    TF1 *f = fHistWidth.GetFunction("f1");
    if (f)
        f->ResetBit(1<<9);
}

void MHSingleMuon::Draw(Option_t *o)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    pad->Divide(1,2);

    pad->cd(1);
    gPad->SetBorderMode(0);
    fHistPhi.Draw();

    pad->cd(2);
    gPad->SetBorderMode(0);
    fHistWidth.Draw();
}
