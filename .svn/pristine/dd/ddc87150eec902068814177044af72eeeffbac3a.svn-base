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
!   Author(s): Thomas Bretz, 05/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniela Dorner, 05/2005 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillstar.C
// ==========
//
// This macro is used to read the star-output files.
// These files are automatically called star00000000.root.
// From these files the muon parameters (psf, muon number, ratio, muon rate),
// the  rate, the number of islands, the effective ontime, the maximum
// humidity and a parameter describing the inhomogeneity are extracted from
// the status display.
// The sequence number is extracted from the filename.
//
// Usage:
//   .x fillstar.C("/magic/data/star/0004/00047421/star00047421.root", kTRUE)
//
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests.
//
// The macro can also be run without ACLiC but this is a lot slower...
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b fillstar.C+\(\"filename\"\,kFALSE\) 2>&1 | tee fillstar.log
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Returns 2 in case of failure, 1 in case of success and 0 if the connection
// to the database is not working.
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>

#include "MSQLMagic.h"

#include "MHCamera.h"
#include "MHMuonPar.h"
#include "MSequence.h"
#include "MStatusArray.h"
#include "MBadPixelsCam.h"
#include "MHEffectiveOnTime.h"

using namespace std;

bool CheckGraph(const TGraph *g)
{
    return g && g->GetN()>0 && !(g->GetN()==1 && g->GetX()[0]==0);
}

int Process(MSQLMagic &serv, TString fname)
{
    TFile file(fname, "READ");
    if (!file.IsOpen())
    {
        cout << "ERROR - Could not find file " << fname << endl;
        return 2;
    }


    MStatusArray arr;
    if (arr.Read()<=0)
    {
        cout << "ERROR - Reading of MStatusDisplay failed." << endl;
        return 2;
    }

    MHCamera *hsparks = (MHCamera*)arr.FindObjectInCanvas("Sparks;avg", "MHCamera", "Sparks");
    if (!hsparks)
    {
        cout << "ERROR - Reading of Sparks failed." << endl;
        return 2;
    }

    TH2F *hcog = (TH2F*)arr.FindObjectInCanvas("Center", "TH2F", "MHHillas");
    if (!hcog)
    {
        cout << "ERROR - Reading of MHHillas failed." << endl;
        return 2;
    }

    MHMuonPar *hmuon = (MHMuonPar*)arr.FindObjectInCanvas("MHMuonPar", "MHMuonPar", "MHMuonPar");
    if (!hmuon)
    {
        cout << "ERROR - Reading of MHMuon failed." << endl;
        return 2;
    }

    Double_t val[6];
    for (int x=1; x<hcog->GetNbinsX(); x++)
        for (int y=1; y<hcog->GetNbinsY(); y++)
        {
            Stat_t px = hcog->GetXaxis()->GetBinCenter(x);
            Stat_t py = hcog->GetYaxis()->GetBinCenter(y);
            Int_t  i  = (TMath::Nint(3*TMath::ATan2(px,py)/TMath::Pi())+6)%6;
            val[i] += hcog->GetBinContent(x, y);
        }

    Float_t inhom = TMath::RMS(6, val)*6/hcog->GetEntries()*100;
    TString inhomogen = Form("%5.1f", inhom);

    Float_t mw  = hmuon->GetMeanWidth();
    Float_t psf = 70.205*mw - 28.055;
    TString PSF = Form("%5.1f", (psf<0?0:psf));
    Int_t   num = (int)hmuon->GetEntries();

    Float_t ratiodatamc = (hmuon->GetMeanSize()/7216)*100;
    TString ratio = Form("%5.1f", ratiodatamc);

    TH1 *h = (TH1*)arr.FindObjectInCanvas("Islands", "TH1F", "MHImagePar");
    if (!h)
    {
        cout << "ERROR - Reading of Islands failed." << endl;
        return 2;
    }

    TString islands = Form("%6.2f", h->GetMean());

    h = (TH1*)arr.FindObjectInCanvas("EffOnTheta", "TH1D", "EffOnTime");

    Float_t effon = h ? h->Integral() : -1;

    h = (TH1*)arr.FindObjectInCanvas("Cleaned;avg", "TH1D", "Cleaned");
    if (!h)
    {
        cout << "ERROR - Reading of Cleaned;avg failed." << endl;
        return 2;
    }

    if (effon<0)
        effon = h->GetEntries()/100;

    Float_t mrate = num/effon;

    TString muonrate  = mrate<0 || effon==0 ? "NULL" : Form("%6.2f", mrate);
    TString effontime = Form("%.1f", effon);

    Int_t numsparks = (int)hsparks->GetEntries();
    Int_t numevents = (int)h->GetEntries() - numsparks;

    TString datarate  = effon==0 ? "NULL" : Form("%.0f", numevents/effon);
    TString sparkrate = Form("%5.2f", numsparks/effon);
    if (sparkrate.Contains("inf") || sparkrate.Contains("nan"))
        sparkrate="NULL";

    MHEffectiveOnTime *ontm = (MHEffectiveOnTime*)arr.FindObjectInCanvas("MHEffectiveOnTime", "MHEffectiveOnTime", "EffOnTime");
    TString totontime = ontm ? Form("%d", TMath::Nint(ontm->GetTotalTime())) : "NULL";
    TString relontime = ontm ? Form("%.2f", effon/ontm->GetTotalTime()*100) : "NULL";

    TGraph *g = (TGraph*)arr.FindObjectInCanvas("MCamEvent", "TGraph", "Currents");

    TString maxdc = CheckGraph(g) ? Form("%5.2f", TMath::MaxElement(g->GetN(), g->GetY())) : "NULL";
    TString mindc = CheckGraph(g) ? Form("%5.2f", TMath::MinElement(g->GetN(), g->GetY())) : "NULL";
    TString meddc = CheckGraph(g) ? Form("%5.2f", TMath::Median(g->GetN(), g->GetY())) : "NULL";

    g = (TGraph*)arr.FindObjectInCanvas("Humidity", "TGraph", "MHWeather");

    TString maxhum = CheckGraph(g) ? Form("%5.1f", TMath::MaxElement(g->GetN(), g->GetY())) : "NULL";
    TString avghum = CheckGraph(g) ? Form("%5.1f", g->GetMean(2)) : "NULL";

    g = (TGraph*)arr.FindObjectInCanvas("Temperature", "TGraph", "MHWeather");

    TString avgtemp = CheckGraph(g) ? Form("%5.1f", g->GetMean(2)) : "NULL";

    g = (TGraph*)arr.FindObjectInCanvas("WindSpeed", "TGraph", "MHWeather");

    TString avgwind = CheckGraph(g) ? Form("%5.1f", g->GetMean(2)) : "NULL";

    g = (TGraph*)arr.FindObjectInCanvas("Cloudiness", "TGraph", "MHWeather");
    //if (!g)
    //    cout << "WARNING - Reading of Cloudiness failed." << endl;
    TString avgclouds = CheckGraph(g) ? Form("%5.1f", g->GetMean(2)) : "NULL";
    TString rmsclouds = CheckGraph(g) ? Form("%5.1f", g->GetRMS(2))  : "NULL";

    g = (TGraph*)arr.FindObjectInCanvas("TempSky", "TGraph", "MHWeather");
    //if (!g)
    //    cout << "WARNING - Reading of TempSky failed." << endl;
    TString avgsky = CheckGraph(g) ? Form("%5.1f", g->GetMean(2)+200) : "NULL";


    g = (TGraph*)arr.FindObjectInCanvas("NumStars", "TGraph", "MHPointing");
    //if (!g)
    //    cout << "WARNING - Reading of NumStars failed." << endl;
    TString numstarsmed = CheckGraph(g) ? Form("%5.1f", TMath::Median(g->GetN(), g->GetY())) : "NULL";
    TString numstarsrms = CheckGraph(g) ? Form("%5.1f", g->GetRMS(2)) : "NULL";

    g = (TGraph*)arr.FindObjectInCanvas("NumStarsCor", "TGraph", "MHPointing");
    //if (!g)
    //    cout << "WARNING - Reading of NumStarsCor failed." << endl;
    TString numcorsmed = CheckGraph(g) ? Form("%5.1f", TMath::Median(g->GetN(), g->GetY())) : "NULL";
    TString numcorsrms = CheckGraph(g) ? Form("%5.1f", g->GetRMS(2)) : "NULL";

    g = (TGraph*)arr.FindObjectInCanvas("Brightness", "TGraph", "MHPointing");
    //if (!g)
    //    cout << "WARNING - Reading of SkyBrightness failed." << endl;
    TString skybrightnessmed = CheckGraph(g) ? Form("%5.1f", TMath::Median(g->GetN(), g->GetY())) : "NULL";
    TString skybrightnessrms = CheckGraph(g) ? Form("%5.1f", g->GetRMS(2)) : "NULL";

    MSequence seq;
    if (seq.Read("sequence[0-9]{8}[.]txt|MSequence")<=0)
    {
        cout << "ERROR - Could not find sequence in file: " << fname << endl;
        return 2;
    }
    if (!seq.IsValid())
    {
        cout << "ERROR - Sequence read from file inavlid: " << fname << endl;
        return 2;
    }

    cout << "Sequence M" << seq.GetTelescope() << ":" << seq.GetSequence() << endl;
    cout << "  Inhomogeneity            " << inhomogen << endl;
    cout << "  PSF [mm]                 " << PSF       << endl;
    cout << "  Island Quality           " << islands   << endl;
    cout << "  Ratio [%]                " << ratio     << endl;
    cout << "  Muon Number              " << num       << endl;
    cout << "  Eff. OnTime [s]          " << effontime << endl;
    cout << "  Tot. OnTime [s]          " << totontime << endl;
    cout << "  Rel. OnTime [%]          " << relontime << endl;
    cout << "  Muon Rate [Hz]           " << muonrate  << endl;
    cout << "  # of Events (w/o sparks) " << numevents << endl;
    cout << "  # of Sparks              " << numsparks << endl;
    cout << "  Rate after ImgCl [Hz]    " << datarate  << endl;
    cout << "  Rate of sparks [Hz]      " << sparkrate << endl;
    cout << "  Minimum DC current [nA]  " << mindc     << endl;
    cout << "  Median  DC current [nA]  " << meddc     << endl;
    cout << "  Maximum DC current [nA]  " << maxdc     << endl;
    cout << "  Maximum Humidity [%]     " << maxhum    << endl;
    cout << "  Average Humidity [%]     " << avghum    << endl;
    cout << "  Average WindSpeed [km/h] " << avgwind   << endl;
    cout << "  Average Temp [" << UTF8::kDeg << "C]        " << avgtemp   << endl;
    cout << "  Average Sky Temp [K]     " << avgsky    << endl;
    cout << "  Cloundiness [%]          " << avgclouds        << " +/- " << rmsclouds        << endl;
    cout << "  Number of Stars          " << numstarsmed      << " +/- " << numstarsrms      << endl;
    cout << "  Number of cor. Stars     " << numcorsmed       << " +/- " << numcorsrms       << endl;
    cout << "  Skybrightness            " << skybrightnessmed << " +/- " << skybrightnessrms << endl;

    TString vars = Form(" fSequenceFirst=%d, "
                        " fTelescopeNumber=%d, "
                        " fMeanNumberIslands=%s,"
                        " fRatio=%s,"
                        " fMuonNumber=%d,"
                        " fEffOnTime=%s,"
                        " fTotOnTime=%s,"
                        " fMuonRate=%s,"
                        " fPSF=%s,"
                        " fDataRate=%s,"
                        " fSparkRate=%s,"
                        " fMinCurrents=%s, "
                        " fMedCurrents=%s, "
                        " fMaxCurrents=%s, "
                        " fMaxHumidity=%s,"
                        " fAvgHumidity=%s, "
                        " fAvgTemperature=%s,"
                        " fAvgWindSpeed=%s,"
                        " fAvgTempSky=%s,"
                        " fAvgCloudiness=%s, "
                        " fRmsCloudiness=%s, "
                        " fNumStarsMed=%s,"
                        " fNumStarsRMS=%s,"
                        " fNumStarsCorMed=%s,"
                        " fNumStarsCorRMS=%s,"
                        " fBrightnessMed=%s,"
                        " fBrightnessRMS=%s,"
                        " fInhomogeneity=%s ",
                        seq.GetSequence(), seq.GetTelescope(),
                        islands.Data(), ratio.Data(),
                        num, effontime.Data(), totontime.Data(),
                        muonrate.Data(), PSF.Data(),
                        datarate.Data(), sparkrate.Data(),
                        mindc.Data(), meddc.Data(), maxdc.Data(),
                        maxhum.Data(), avghum.Data(),
                        avgtemp.Data(), avgwind.Data(),
                        avgsky.Data(), avgclouds.Data(), rmsclouds.Data(),
                        numstarsmed.Data(), numstarsrms.Data(),
                        numcorsmed.Data(), numcorsrms.Data(),
                        skybrightnessmed.Data(), skybrightnessrms.Data(),
                        inhomogen.Data());

    TString where = Form("fSequenceFirst=%d AND fTelescopeNumber=%d",
                         seq.GetSequence(), seq.GetTelescope());

    if (!serv.InsertUpdate("Star", vars, where))
        return 2;

    cout << endl;

    h = (TH1*)arr.FindObjectInCanvas("Rate", "TH2D", "Rate");
    if (!h)
        return 1;

    h->ResetBit(TH1::kCanRebin);

    Int_t itrig = h->GetYaxis()->FindBin("Trig");
    Int_t isum  = h->GetYaxis()->FindBin("Sum");
    Int_t inull = h->GetYaxis()->FindBin("0");

    for (int i=0; i<h->GetNbinsX(); i++)
    {
        Int_t id = atoi(h->GetXaxis()->GetBinLabel(i+1));

        Int_t run  = seq.GetSequence()<1000000 ? id : id/1000 + 1000000;
        Int_t file = seq.GetSequence()<1000000 ? 0  : id%1000;

        const char *rtrig = itrig<0 ? "NULL" : Form("%8.1f", h->GetBinContent(i+1, itrig));
        const char *rsum  = isum <0 ? "NULL" : Form("%8.1f", h->GetBinContent(i+1, isum));
        const char *rnull = inull<0 ? "NULL" : Form("%8.1f", h->GetBinContent(i+1, inull));

        cout << "  M" << seq.GetTelescope() << ":" << run << "/" << file << " " <<rtrig << " " << rsum << " " << rnull << endl;

        TString vars = Form(" fTelescopeNumber=%d, "
                            " fRunNumber=%d, "
                            " fFileNumber=%d, "
                            " fRateCleanedTrig=%s,"
                            " fRateCleanedSum=%s,"
                            " fRateCleanedNull=%s",
                            seq.GetTelescope(), run, file,
                            rtrig, rsum, rnull);

        TString where = Form("fTelescopeNumber=%d AND fRunNumber=%d AND fFileNumber=%d",
                             seq.GetTelescope(), run, file);

        if (!serv.InsertUpdate("RunDataCheck", vars, where))
            return 2;
    }

    return 1;
}

int fillstar(TString fname, Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "fillstar" << endl;
    cout << "--------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "File: " << fname << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    return Process(serv, fname);
}
