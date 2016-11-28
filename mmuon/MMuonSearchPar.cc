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
!   Author(s): Keiichi Mase 10/2004 <mailto:mase@mppmu.mpg.de>
!   Author(s): Markus Meyer 10/2004 <mailto:meyer@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MMuonSearchPar
//
// Storage Container for muon
//
// This class is the container for muon parameters. The calculation
// of Radius and center of the ring is done here.
// Muons are searched by fitting the image with a circle.
//
//  In order to use further information of muons such as the width of arcs,
// the size of the fraction of the ring and the muons size, use the
// infomation stored in
//
//   MMuonCalibPar.
//
// The information will be available by using the task of
//
//   MMuonCalibParCalc.
//
// Version 2:
// ----------
//   + Float_t fTime;      // Mean arrival time of core pixels
//   + Float_t fTimeRms;   // Rms of arrival time of core pixels
//
//  Input Containers:
//   MGeomCam
//   MHillas
//   MSignalCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MMuonSearchPar.h"

#include <TMinuit.h>
#include <TEllipse.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MHillas.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MSignalPix.h"
#include "MSignalCam.h"

using namespace std;

ClassImp(MMuonSearchPar);

// --------------------------------------------------------------------------
//
// Default constructor.
//
MMuonSearchPar::MMuonSearchPar(const char *name, const char *title)
{
    fName  = name  ? name  : "MMuonSearchPar";
    fTitle = title ? title : "Parameters to find Muons";
}

// --------------------------------------------------------------------------
//
void MMuonSearchPar::Reset()
{
    fRadius    = -1;
    fDeviation = -1;
    fCenterX   =  0;
    fCenterY   =  0;
    fTime      =  0;
    fTimeRms   = -1;
}

// --------------------------------------------------------------------------
//
//  return TMath::Hypot(fCenterX, fCenterY);
//
Float_t MMuonSearchPar::GetDist() const
{
    return TMath::Hypot(fCenterX, fCenterY);
}

// --------------------------------------------------------------------------
//
// This is a wrapper function to have direct access to the data members
// in the function calculating the minimization value.
//
void MMuonSearchPar::fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
    const MMuonSearchPar *optim = (MMuonSearchPar*)gMinuit->GetObjectFit();
    f = optim->Fcn(par);
}

// --------------------------------------------------------------------------
//
//  This function gives you the ring radius fitted best to the camera image
//  and its RMS for the input position.
//
Double_t MMuonSearchPar::Fcn(Double_t *par) const
{
    const Int_t entries = fSignal.GetSize();

    Double_t meanr=0;
    Double_t devr =0;
    Double_t sums =0;

    // It seems that the loop is easy enough for a compiler optimization.
    // Using pointer arithmetics doesn't improve the speed of the fit.
    for (Int_t i=0; i<entries; i++ )
    {
        const Double_t dx = fX[i]-par[0];
        const Double_t dy = fY[i]-par[1];

        const Double_t sq = dx*dx + dy*dy;

        sums  += fSignal[i];
        meanr += fSignal[i] * TMath::Sqrt(sq);
        devr  += fSignal[i] * sq;
    }

    par[2] = meanr/sums;
    par[3] = devr/sums - par[2]*par[2];

    return par[3];
}

// --------------------------------------------------------------------------
//
//  This function finds the center position of the circle which gives minimum 
// RMS of the fit, changing the center position of the circle.
//
void MMuonSearchPar::CalcMinimumDeviation(const MGeomCam &geom,
                                          const MSignalCam &evt,
                                          Double_t &x, Double_t &y,
                                          Double_t &sigma, Double_t &radius)
{
    // ------- Make a temporaray copy of the signal ---------
    // ------- and calculate arrival time parameters --------
    const Int_t n = geom.GetNumPixels();

    fSignal.Set(n);
    fX.Set(n);
    fY.Set(n);

    Int_t p=0;
    Int_t q=0;

    Double_t mean=0;
    Double_t sq  =0;

    for (int i=0; i<n; i++)
    {
        const MSignalPix &pix = evt[i];
        if (!pix.IsPixelUsed())
            continue;

        fSignal[p] = pix.GetNumPhotons();

        fX[p] = geom[i].GetX();
        fY[p] = geom[i].GetY();
        p++;

        //timing
        if (!pix.IsPixelCore())
            continue;

        mean += pix.GetArrivalTime();
        sq   += pix.GetArrivalTime()*pix.GetArrivalTime();
        q++;
    }

    mean /= q;
    sq   /= q;

    fTime    = mean;
    fTimeRms = TMath::Sqrt(sq-mean*mean);

    fSignal.Set(p);


    // ----------------- Setup and call minuit -------------------
    const Float_t  delta = 30.;  // 3 mm (1/10 of an inner pixel size) Step to move.
    //const Double_t r     = geom.GetMaxRadius()*2;

    // Save gMinuit
    TMinuit *minsave = gMinuit;

    // Initialize TMinuit with 4 parameters
    TMinuit minuit;
    minuit.SetPrintLevel(-1);     // Switch off printing
    minuit.Command("set nowarn"); // Switch off warning
    // Define Parameters
    minuit.DefineParameter(0, "x",     x, delta,  0, 0);//-r, r);
    minuit.DefineParameter(1, "y",     y, delta,  0, 0);//-r, r);
    minuit.DefineParameter(2, "r",     0, 1,      0, 0);
    minuit.DefineParameter(3, "sigma", 0, 1,      0, 0);
    // Fix return parameters
    minuit.FixParameter(2);
    minuit.FixParameter(3);
    // Setup Minuit for 'this' and use fit function fcn
    minuit.SetObjectFit(this);
    minuit.SetFCN(fcn);

    // Perform Simplex minimization
    Int_t er;
    Double_t tmp[2] = { 0, 0 };
    minuit.mnexcm("simplex", tmp, 2, er);

    // Get resulting parameters
    Double_t error;
    minuit.GetParameter(0, x,      error);
    minuit.GetParameter(1, y,      error);
    minuit.GetParameter(2, radius, error);
    minuit.GetParameter(3, sigma,  error);

    sigma = sigma>0 ? TMath::Sqrt(sigma) : 0;

    gMinuit = minsave;
}

// --------------------------------------------------------------------------
//
//  Calculation of muon parameters
//
void MMuonSearchPar::Calc(const MGeomCam &geom, const MSignalCam &evt,
                          const MHillas &hillas)
{
    Double_t x = hillas.GetMeanX();
    Double_t y = hillas.GetMeanY();

    // -------------------------------------------------
    // Keiichi suggested trying to precalculate the Muon
    // center a bit better, but it neither improves the
    // fit result nor the speed
    //
    // const Float_t tmpr = 300.;  // assume that the temporal cherenkov angle is 1 deg. (300 mm).
    //
    // const Double_t a = TMath::Tan(hillas.GetDelta());
    //
    // const Double_t dx =     a/TMath::Sqrt(tmpr+a*a)/3.;
    // const Double_t dy = -tmpr/TMath::Sqrt(1+a*a)/3.;
    //
    // Double_t par1[] = { x+dx, y+dy, 0, 0 };
    // Double_t par2[] = { x-dx, y-dy, 0, 0 };
    //
    // const Double_t dev1 = MMuonSearchPar::Fcn(par1);
    // const Double_t dev2 = MMuonSearchPar::Fcn(par2);
    //
    // if (dev1<dev2)
    // {
    //     x += dx;
    //     y += dy;
    // }
    // else
    // {
    //     x -= dx;
    //     y -= dy;
    // }
    // -------------------------------------------------

    Double_t sigma, rad;

    // find the best fit.
    CalcMinimumDeviation(geom, evt, x, y, sigma, rad);

    fCenterX   = x;
    fCenterY   = y;
    fRadius    = rad;
    fDeviation = sigma;

    //SetReadyToSave();
} 

void MMuonSearchPar::Print(Option_t *) const
{
    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Est. Radius     [mm] = " << fRadius  << endl;
    *fLog << " - Deviation       [mm] = " << fDeviation  << endl;
    *fLog << " - Center Pos. X   [mm] = " << fCenterX << endl;
    *fLog << " - Center Pos. Y   [mm] = " << fCenterY << endl;
}

void MMuonSearchPar::Print(const MGeomCam &geom, Option_t *) const
{
    *fLog << all;
    *fLog << GetDescriptor() << endl;
    *fLog << " - Est. Radius    [deg] = " << fRadius*geom.GetConvMm2Deg()   << endl;
    *fLog << " - Deviation      [deg] = " << fDeviation*geom.GetConvMm2Deg()   << endl;
    *fLog << " - Center Pos. X  [deg] = " << fCenterX*geom.GetConvMm2Deg()  << endl;
    *fLog << " - Center Pos. Y  [deg] = " << fCenterY*geom.GetConvMm2Deg()  << endl;
}

// --------------------------------------------------------------------------
//
// Paint the ellipse corresponding to the parameters
//
void MMuonSearchPar::Paint(Option_t *opt)
{
    //if (fRadius<180 || fRadius>400 || fDeviation>45)
    //    return;

    TEllipse e1(fCenterX, fCenterY, fRadius-fDeviation, fRadius-fDeviation);
    TEllipse e2(fCenterX, fCenterY, fRadius+fDeviation, fRadius+fDeviation);
    e1.SetLineWidth(1);
    e2.SetLineWidth(1);
    e1.SetLineColor(kYellow);
    e2.SetLineColor(kYellow);
    e1.Paint();
    e2.Paint();
}
