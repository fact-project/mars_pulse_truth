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
!   Author(s): Thomas Bretz, 10/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Hendrik Bartko, 08/2004 <mailto:hbartko@mppmu.mpg.de>
!   Author(s): Stefan Ruegamer, 03/2006 <mailto:snruegam@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCameraData
//
// This is a generalized class storing camera data. For example the cleaning
// level for the image cleaning is one possibility.
//
/////////////////////////////////////////////////////////////////////////////
#include "MCameraData.h"

#include <TArrayI.h>

#include "MMath.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MPedPhotCam.h"
#include "MPedPhotPix.h"

#include "MSignalCam.h"
#include "MSignalPix.h"

ClassImp(MCameraData);

using namespace std;

// --------------------------------------------------------------------------
//
// Creates a MSignalPix object for each pixel in the event
//
MCameraData::MCameraData(const char *name, const char *title)
{
    fName  = name  ? name  : "MCameraData";
    fTitle = title ? title : "Generalized storage container for camera contents";
}

/*
// --------------------------------------------------------------------------
//
// This is not yet implemented like it should.
//

void MCameraData::Draw(Option_t* option) 
{
    //
    //   FIXME!!! Here the Draw function of the CamDisplay
    //   should be called to add the CamDisplay to the Pad.
    //   The drawing should be done in MCamDisplay::Paint
    //

    //    MGeomCam *geom = fType ? new MGeomCamMagic : new MGeomCamCT1;
    //    MCamDisplay *disp = new MCamDisplay(geom);
    //    delete geom;
    //    disp->DrawPhotNum(this);
}
*/


// --------------------------------------------------------------------------
//
// Function to calculate the cleaning level for all pixels in a given event
// as the ratio between the measured photons and the pedestal rms.
// In order to do the image cleaning on average in the same photon flux
// (reconstructed photons per pixel area) for the inner and outer pixels,
// a correction factor is applied to the outer pixels (see TDAS 02-14).
// The correction factor assumes the ideal case that the pedestal rms 
// scales with the inverse square root of the pixel area.
//
// FIXME: Should the check noise<=0 be replaced by MBadPixels?
//
void MCameraData::CalcCleaningLevel(const MSignalCam &evt, const MPedPhotCam &cam,
                                    const MGeomCam &geom)
{
    const Int_t n = geom.GetNumPixels();

    // Reset arrays
    fData.Set(n);
    fData.Reset();

    fValidity.Set(n);
    fValidity.Reset();

    const Int_t entries = evt.GetNumPixels();

    // calculate cleaning levels
    for (Int_t idx=0; idx<entries; idx++)
    {
        const MSignalPix &pix = evt[idx];

        const Float_t noise = cam[idx].GetRms();

        if (noise<=0) // fData[idx]=0, fValidity[idx]=0
            continue;

        //
	// We calculate a correction factor which accounts for the 
	// fact that pixels have different size (see TDAS 02-14).
	//
        fData[idx] = pix.GetNumPhotons() * geom.GetPixRatioSqrt(idx) / noise;
        fValidity[idx] = 1;
    }
}

// --------------------------------------------------------------------------
//
// Function to calculate the cleaning level for all pixels in a given event
// as the ratio between the measured photons and the pedestal rms.
// In order to do the image cleaning on average in the same photon flux
// (reconstructed photons per pixel area) for the inner and outer pixels,
// a correction factor is applied to the outer pixels (see TDAS 02-14).
// The correction factor takes the actual average pedestal RMS of the
// inner and outer pixels into account.
//
// FIXME: Should the check noise<=0 be replaced by MBadPixels?
//
void MCameraData::CalcCleaningLevel2(const MSignalCam &evt, const MPedPhotCam &cam,
                                     const MGeomCam &geom)
{
    const Int_t n = geom.GetNumPixels();

    // reset arrays
    fData.Set(n);
    fData.Reset();

    fValidity.Set(n);
    fValidity.Reset();

    // check validity of rms with area index 0
    const Float_t anoise0 = cam.GetArea(0).GetRms();
    if (anoise0<=0)
        return;

    // calculate cleaning levels
    const Int_t entries = evt.GetNumPixels();
    for (Int_t idx=0; idx<entries; idx++)
    {
        const MSignalPix &pix = evt[idx];

        const Float_t noise = cam[idx].GetRms();

        if (noise<=0) // fData[idx]=0, fValidity[idx]=0
            continue;

        //
	// We calculate a correction factor which accounts for the 
	// fact that pixels have different size (see TDAS 02-14).
	// We also take into account that the RMS does not scale 
	// with the square root of the pixel area.
	// 
        const UInt_t  aidx  = geom[idx].GetAidx();
        const Float_t ratio = cam.GetArea(aidx).GetRms()/anoise0;

        fData[idx] = pix.GetNumPhotons() * geom.GetPixRatio(idx) * ratio / noise;
        fValidity[idx] = 1;
    }
}

void MCameraData::CalcCleaningLevel(const MSignalCam &evt, Double_t noise,
                                    const MGeomCam &geom)
{
    const Int_t n = geom.GetNumPixels();

    // reset arrays
    fData.Set(n);
    fData.Reset();

    fValidity.Set(n);
    fValidity.Reset();

    // check validity of noise
    if (noise<=0)
        return;

    // calculate cleaning levels
    const Int_t entries = evt.GetNumPixels();
    for (Int_t idx=0; idx<entries; idx++)
    {
        const MSignalPix &pix = evt[idx];

        //
	// We calculate a correction factor which accounts for the 
	// fact that pixels have different size (see TDAS 02-14).
	//
        fData[idx] = pix.GetNumPhotons() * geom.GetPixRatio(idx) / noise;
        fValidity[idx] = 1;
    }
}

// --------------------------------------------------------------------------
//
// Function to calculate the cleaning level for all pixels in a given event
// as the ratio between the reconstructed number of photons per area of an
// inner pixel and the average pedestal RMS of the inner pixels (democratic
// image cleaning, see TDAS 02-14).
//
// FIXME: Should the check noise<=0 be replaced by MBadPixels?
//
void MCameraData::CalcCleaningLevelDemocratic(const MSignalCam &evt, const MPedPhotCam &cam,
                                              const MGeomCam &geom)
{
    const Int_t n = geom.GetNumPixels();

    // reset arrays
    fData.Set(n);
    fData.Reset();

    fValidity.Set(n);
    fValidity.Reset();

    // check validity of rms with area index 0
    const Float_t noise0 = cam.GetArea(0).GetRms();
    if (noise0<=0)
        return;

    // calculate cleaning levels
    const Int_t entries = evt.GetNumPixels();
    for (Int_t idx=0; idx<entries; idx++)
    {
        const MSignalPix &pix = evt[idx];

        const Float_t noise = cam[idx].GetRms();

        if (noise<=0)
            continue;

        //
        // We calculate a correction factor which accounts for the
        // fact that pixels have different size (see TDAS 02-14).
        //
        fData[idx] = pix.GetNumPhotons() * geom.GetPixRatio(idx) / noise0;
        fValidity[idx] = 1;
    }
}

// --------------------------------------------------------------------------
//
// Function to calculate the cleaning level for all pixels in a given event.
// The level is the probability that the signal is a real signal (taking
// the signal height and the fluctuation of the background into account)
// times one minus the probability that the signal is a background
// fluctuation (calculated from the time spread of arrival times
// around the pixel with the highest signal)
//
// FIXME: Should the check noise<=0 be replaced by MBadPixels?
//
void MCameraData::CalcCleaningProbability(const MSignalCam &evt, const MPedPhotCam &pcam,
                                          const MGeomCam &geom)
{
    const Int_t n = geom.GetNumPixels();

    // Reset arrays
    fData.Set(n);
    fData.Reset();

    fValidity.Set(n);
    fValidity.Reset();

    // check validity of noise
    const Float_t anoise0 = pcam.GetArea(0).GetRms();
    if (anoise0<=0)
        return;

    const Int_t entries = evt.GetNumPixels();

    // Find pixel with max signal
    Int_t maxidx = 0;

    // Find pixel entry with maximum signal
    for (Int_t i=0; i<entries; i++)
    {
        const Double_t s0 = evt[i].GetNumPhotons()      * geom.GetPixRatio(i);
        const Double_t s1 = evt[maxidx].GetNumPhotons() * geom.GetPixRatio(maxidx);
        if (s0>s1)
            maxidx = i;
    }

    const Double_t timemean = evt[maxidx].GetArrivalTime();
    const Double_t timerms  = 0.75; //[slices] rms time spread around highest pixel

    // calculate cleaning levels
    for (Int_t idx=0; idx<entries; idx++)
    {
        const MSignalPix &spix = evt[idx];

        const Float_t rms = pcam[idx].GetRms();
        if (rms<=0) // fData[idx]=0, fValidity[idx]=0
            continue;

        fValidity[idx]=1;

        // get signal and arrival time
        const UInt_t  aidx     = geom[idx].GetAidx();
        const Float_t ratio    = pcam.GetArea(aidx).GetRms()/anoise0;

        const Double_t signal  = spix.GetNumPhotons() * geom.GetPixRatio(idx) * ratio / rms;
        const Double_t time    = evt[idx].GetArrivalTime();

        // if signal<0 the probability is equal 0
        if (signal<0)
            continue;

        // Just for convinience for easy readybility
        const Double_t means   = 0;
        const Double_t meant   = timemean;

        const Double_t sigmas  = rms;
        const Double_t sigmat  = timerms;

        // Get probabilities
        const Double_t psignal = MMath::GaussProb(signal, sigmas, means);
        const Double_t pbckgnd = MMath::GaussProb(time,   sigmat, meant);

        // Set probability
        fData[idx]     = psignal*(1-pbckgnd);
        fValidity[idx] = 1;

        // Make sure, that we don't run into trouble because of
        // a little numerical uncertanty
        if (fData[idx]>1)
            fData[idx]=1;
        if (fData[idx]<0)
            fData[idx]=0;
    }
}

// --------------------------------------------------------------------------
//
// Function to calculate the cleaning level for all pixels in a given event.
// The level is the absolute number of photons times the area-ratio.
//
void MCameraData::CalcCleaningAbsolute(const MSignalCam &evt, const MGeomCam &geom)
{
    const Int_t n = geom.GetNumPixels();

    // Reset arrays
    fData.Set(n);
    fData.Reset();

    fValidity.Set(n);
    fValidity.Reset();

    const Int_t entries = evt.GetNumPixels();

    // calculate cleaning levels
    for (Int_t idx=0; idx<entries; idx++)
    {
        const MSignalPix &spix = evt[idx];

        // Set probability
        fData[idx]     = spix.GetNumPhotons() * geom.GetPixRatio(idx);
        fValidity[idx] = 1;
    }
}

void MCameraData::CalcCleaningArrivalTime(const MSignalCam &evt, const MGeomCam &geom)
{
    const Int_t n = geom.GetNumPixels();

    // Reset arrays
    fData.Set(n);
    fData.Reset();

    fValidity.Set(n);
    fValidity.Reset();

    // check validity of noise
    const Int_t entries = evt.GetNumPixels();
    TArrayD u(6), w(6);
    TArrayI ii(6);

    for (int i=0; i<entries; i++)
    {
        if (evt[i].IsPixelUnmapped())
            continue;

        const Double_t t0 = evt[i].GetArrivalTime();
        const Double_t s0 = evt[i].GetNumPhotons();
        const Double_t r0 = geom.GetPixRatio(i);
        const Double_t x0 = geom[i].GetX();
        const Double_t y0 = geom[i].GetY();

        const Double_t e0 = s0<0.01 ? 100 : 1./s0;

        const Int_t n2 = geom[i].GetNumNeighbors();

        Int_t cnt2=0;
        for (int j=0; j<n2; j++)
        {
            Int_t idx = geom[i].GetNeighbor(j);

            if (evt[idx].IsPixelUnmapped())
                continue;

            const Double_t tj = evt[idx].GetArrivalTime()-t0;
            const Double_t sj = evt[idx].GetNumPhotons();
            const Double_t rj = geom.GetPixRatio(idx)+r0;
            const Double_t xj = geom[idx].GetX()-x0;
            const Double_t yj = geom[idx].GetY()-y0;

            const Double_t d2 = xj*xj+yj*yj;

            const Double_t ej = sj<0.01 ? 100 : 1./sj;

            u[cnt2] =  tj*tj/rj;
            w[cnt2] =  1./(e0+ej)/d2;         // TYPE I+II
            cnt2++;
        }

        TMath::Sort(cnt2, u.GetArray(), ii.GetArray(), kFALSE);

        Double_t sumt = 0;
        Double_t sumw = 0;
        for (int j=0; j<TMath::Min(cnt2, 3); j++)
        {
            sumt += u[ii[j]]*w[ii[j]];
            sumw += w[ii[j]];
        }

        const Double_t wuz = TMath::Sqrt(sumt/sumw);

        if (TMath::IsNaN(wuz))
            *fLog << warn << "WARNING - MCameraData " << sumt << " " << sumw << endl;

        Double_t val = s0<0 || TMath::IsNaN(wuz) || wuz<1e-200 ? 0 : s0*r0/wuz;

        if ((s0>0 && wuz<1e-200) || val>100)
            val=100;

        fData[i] = TMath::Log10(val+1)*5;

        if (TMath::IsNaN(fData[i]))
            //     0                1e-6          0              NaN
            *fLog << warn << "WARNING - MCameraData " << sumt << " " << sumw << " " << wuz << " " << val << endl;

        fValidity[i] = 1;
    }
}

// --------------------------------------------------------------------------
//
// Returns the contents of the pixel.
//
Bool_t MCameraData::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if (idx<0 || idx>=fData.GetSize())
        return kFALSE;

    val = fData[idx];
    return fValidity[idx];
}

void MCameraData::DrawPixelContent(Int_t num) const
{
    *fLog << warn << "MCameraData::DrawPixelContent - not available." << endl;
}

void MCameraData::Print(Option_t *o) const
{
    MParContainer::Print(o);
    *fLog << "Size = " << fData.GetSize() << endl;
    for (int i=0; i<fData.GetSize(); i++)
        cout << i << ": " << fData[i] << endl;
}
