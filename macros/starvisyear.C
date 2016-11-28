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
!   Author(s): Thomas Bretz 10/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004-2006
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// starvisyear.C
//
// displays the star visibility along one year
//
// See the code for ducumentation and setup
//
///////////////////////////////////////////////////////////////////////////

#include <TString.h>
#include <TCanvas.h>
#include <TH2D.h>
#include <TGraph.h>

#include "MObservatory.h"
#include "MTime.h"
#include "MAstro.h"
#include "MAstroSky2Local.h"
#include "MVector3.h"
#include "MH.h"


void starvisyear()
{
    // Setup the observatory location (see MObservatory for details)
    MObservatory obs(MObservatory::kMagic1);;

    // Setup the start time of the year (365 days) to be displayed
    MTime time(-1);  // -1 mean NOW

    // Setup a different time if you like
    time.Set(2006, 1, 1, 0);

    // Setup right ascension and declination of the source to display
    //Crab
    const Double_t ra  = MAstro::Hms2Rad(5, 34, 31.9);
    const Double_t dec = MAstro::Dms2Rad(22, 0, 52.0);

    cout << obs.Print() << endl;

    return;

    // Setup the name of the source
    TString source("CrabNebula");

    // Setup palette you would like to see (99 is fixed)
    // for more details see MH::SetPalette
    MH::SetPalette("glow2", 99);

    // --------------------------------------------------------------------------
    //
    //  Start producing the data for the plot
    //

    // Setup the source in the appropriate format
    MVector3 v;
    v.SetRaDec(ra, dec);

    // For simplicity get mjd of time
    Double_t mjd = time.GetMjd();

    // Setup axis ranges
    Double_t first = time.GetAxisTime();
    Double_t last  = MTime(mjd+365).GetAxisTime();

    Int_t h0 = 13-obs.GetLongitudeDeg()/15;

    // Define histograms
    TH2D hist( "1", "", 365, first, last, 24*12, first+h0*60*60, first+(h0+24)*60*60);//h0/24.+1, (h0+24)/24.+1);
    TH2D hmoon("2", "", 365, first, last, 24*12, first+h0*60*60, first+(h0+24)*60*60);//h0/24.+1, (h0+24)/24.+1);

    // Fill histograms
    TGraph sunset[4], sunrise[4];
    for (int x=0; x<365; x++)
    {
        // Get moon phase and axis time for mjd+x
        Double_t moon = MAstro::GetMoonPhase(mjd+x);
        Double_t tmx  = MTime(mjd+x).GetAxisTime();

        // get sunrise and sunset for the current day
        Double_t sunst[4], sunri[4];
        for (int i=0; i<4; i++)
        {
            // get sunset and sunrise of next day
            sunri[i] = obs.GetSunRiseSet(mjd+x+1,-18+6*i)[0];
            sunst[i] = obs.GetSunRiseSet(mjd+x,  -18+6*i)[1];

            // fill both into the TGraphs
            sunset[i].SetPoint( sunset[i].GetN(),  tmx, MTime(sunst[i]).GetAxisTime()-x*24*60*60);
            sunrise[i].SetPoint(sunrise[i].GetN(), tmx, MTime(sunri[i]).GetAxisTime()-x*24*60*60);
        }

        // If nautical twilight doesn't take place skip the rest
        if (sunri[1]<0 || sunst[1]<0)
            continue;

        // Now calculate in steps of five minutes
        for (int hor=0; hor<24*12; hor++)
        {
            Double_t hr = hor/12. + h0;
            Double_t offset = x + hr/24.;

            // mjd of the current time
            MTime tm(mjd+offset);

            // Check if current time is within darkness
            if (tm.GetMjd()>sunri[1] || tm.GetMjd()<sunst[1])
                continue;

            // Initialize conversion from sky coordinates to local coordinates
            MAstroSky2Local conv(tm.GetGmst(), obs);

            // get moon position on the sky
            Double_t moonra, moondec;
            MAstro::GetMoonRaDec(tm.GetMjd(), moonra, moondec);

            // calculate moon position as seen by the telescope
            v.SetRaDec(moonra, moondec);
            v *= conv;

            // check if moon is above or below horizont
            if (v.Theta()*TMath::RadToDeg()<90)
                moon = 0;

            // fill moon visibility into histpogram
            hmoon.SetBinContent(x+1, hor+1, moon);

            // calculate source position as seen by the telescope
            v.SetRaDec(ra, dec);
            v *= conv;

            // fill altitude of source into histogram
            hist.SetBinContent(x+1, hor+1, v.Theta()*TMath::RadToDeg());
        }
    }

    // --------------------------------------------------------------------------
    //
    //  Start producing the nice plot
    //

    // Setup canvas
    TCanvas *c = new TCanvas;
    c->SetBorderMode(0);
    c->SetGridx();
    c->SetGridy();

    // Setup histogram
    TString name = obs.GetObservatoryName();
    hist.SetTitle(Form("Zenith distance of %s at %s", source.Data(), name.Data()));

    hist.SetXTitle("Day");
    hist.SetYTitle("UTC");

    hist.GetXaxis()->CenterTitle();
    hist.GetYaxis()->CenterTitle();

    hist.GetXaxis()->SetTimeFormat("%d/%m/%y %F1995-01-01 00:00:00 GMT");
    hist.GetXaxis()->SetTimeDisplay(1);
    hist.GetXaxis()->SetLabelSize(0.033);

    hist.GetYaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
    hist.GetYaxis()->SetTimeDisplay(1);
    hist.GetYaxis()->SetLabelSize(0.033);
    hist.GetYaxis()->SetTitleOffset(1.3);

    hist.SetContour(99);
    hist.SetMinimum(0);
    hist.SetMaximum(90);

    hist.SetStats(kFALSE);

    // draw histograms
    hist.DrawCopy("colz");
    hmoon.DrawCopy("scat=7 same");

    // Draw rise and set time of sun
    Int_t style[] = { kSolid, 5, 4, kDotted };
    for (int i=0; i<4; i++)
    {
        sunset[i].SetLineColor(kBlue);
        sunrise[i].SetLineColor(kBlue);
        sunset[i].SetLineWidth(2);
        sunrise[i].SetLineWidth(2);
        sunset[i].SetLineStyle(style[i]);
        sunrise[i].SetLineStyle(style[i]);
        sunset[i].DrawClone("L");
        sunrise[i].DrawClone("L");
    }
}
