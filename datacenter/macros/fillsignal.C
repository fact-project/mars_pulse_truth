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
!   Author(s): Thomas Bretz, 04/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniela Dorner, 04/2005 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillsignal.C
// ============
//
// This macro is used to read the calibration-/callisto-output files
// signal00000.root.
//
// From this file the mean pedrms, the mean signal and the pulse position
// for the inner and outer camera is extracted and inserted into the database
// in the table Calibration, where the results of callisto are stored.
// The sequence number is extracted from the filename.
//
// Usage:
//   .x fillsignal.C("/magic/data/callisto/0004/00047421/signal00047421.root", kTRUE)
//
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests.
//
// The macro can also be run without ACLiC but this is a lot slower...
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b fillsignal.C+\(\"filename\"\,kFALSE\) 2>&1 | tee fillsignal.log
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

#include <TRegexp.h>

#include <TH2.h>
#include <TFile.h>
#include <TGraph.h>
#include <TSQLResult.h>

#include "MSQLMagic.h"

#include "MStatusArray.h"
#include "MSequence.h"
#include "MHCamera.h"
#include "MHVsTime.h"

#include "MCalibrationPulseTimeCam.h"
#include "MCalibrationPix.h"

using namespace std;

int Process(MSQLMagic &serv, TString fname)
{
    TFile file(fname, "READ");
    if (!file.IsOpen())
    {
        cout << "ERROR - Could not find file " << fname << endl;
        return 2;
    }

    TString meanextpulpos("NULL");
    TString rmsextpulpos("NULL");

    MCalibrationPulseTimeCam *pt;
    file.GetObject("MCalibrationPulseTimeCam", pt);
    if (pt)
    {
        Double_t meanextpul = pt->GetAverageArea(0).GetHiGainMean();
        Double_t rmsextpul  = pt->GetAverageArea(0).GetHiGainRms();

        if (meanextpul>=0 || rmsextpulpos>=0)
        {
            meanextpulpos.Form("%6.2f", meanextpul);
            rmsextpulpos.Form("%6.2f", rmsextpul);
        }
    }


    MStatusArray arr;
    if (arr.Read()<=0)
    {
        cout << "ERROR - Reading of MStatusDisplay failed." << endl;
        return 2;
    }

    MHCamera *cam = (MHCamera*)arr.FindObjectInCanvas("PedRMS;avg", "MHCamera", "PedRMS");
    if (!cam)
    {
        cout << "ERROR - Reading of PedRMS;avg failed." << endl;
        return 2;
    }

    MHCamera *cal = (MHCamera*)arr.FindObjectInCanvas("CalPos;avg", "MHCamera", "CalPos");
    MHCamera *pul = (MHCamera*)arr.FindObjectInCanvas("PulsePos;avg", "MHCamera", "PulsePos");
    if (!pul)
    {
        cout << "ERROR - Reading of PulsePos;avg failed." << endl;
        return 2;
    }
/*
    MHCamera *difflo = (MHCamera*)arr.FindObjectInCanvas("DiffLo;avg", "MHCamera", "DiffLo");
    if (!difflo)
    {
        cout << "ERROR - Reading of DiffLo;avg failed." << endl;
        return 2;
    }
    MHCamera *diffhi = (MHCamera*)arr.FindObjectInCanvas("DiffHi;avg", "MHCamera", "DiffHi");
    if (!diffhi)
    {
        cout << "ERROR - Reading of DiffHi;avg failed." << endl;
        return 2;
    }
*/

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

    TString medpuloff("NULL");
    TString devpuloff("NULL");
    TString medhilocal("NULL");
    TString devhilocal("NULL");

    if (seq.GetSequence()<200000 && seq.GetTelescope()==1)
    {
        MHCamera *hilooff = (MHCamera*)arr.FindObjectInCanvas("HiLoOff;avg", "MHCamera", "HiLoOff");
        if (!hilooff)
        {
            cout << "ERROR - Reading of HiLoOff failed." << endl;
            return 2;
        }

        MHCamera *hilocal = (MHCamera*)arr.FindObjectInCanvas("HiLoCal;avg", "MHCamera", "HiLoCal");
        if (!hilocal)
        {
            cout << "ERROR - Reading of HiLoCal failed." << endl;
            return 2;
        }

        medpuloff.Form("%7.4f",  hilooff->GetMedian());
        devpuloff.Form("%7.4f",  hilooff->GetDev());
        medhilocal.Form("%6.2f", hilocal->GetMedian());
        devhilocal.Form("%6.2f", hilocal->GetDev());
    }

    TArrayI inner(1);
    inner[0] = 0;

    TArrayI outer(1);
    outer[0] = 1;

    Int_t s0[] = { 1, 2, 3, 4, 5, 6 };

    Stat_t meanrmsi = cam->GetMeanSectors(TArrayI(6, s0), inner);
    Stat_t meanrmso = cam->GetMeanSectors(TArrayI(6, s0), outer);

    if (meanrmsi<0 || meanrmso<0)
    {
        cout << "ERROR - MeanPedRMS inner or outer < 0 " << endl;
        cout << "MeanPedRMS inner " << meanrmsi << endl;
        cout << "MeanPedRMS outer " << meanrmso << endl;
        return 2;
    }

    TString meanrmsinner=Form("%6.2f", meanrmsi);
    TString meanrmsouter=Form("%6.2f", meanrmso);

    cam = (MHCamera*)arr.FindObjectInCanvas("Interp'd;avg", "MHCamera", "Interp'd");
    if (!cam)
    {
        cout << "ERROR - Reading of Interp'd;avg failed." << endl;
        return 2;
    }

    Stat_t meansigi = cam->GetMeanSectors(TArrayI(6, s0), inner);
    Stat_t meansigo = cam->GetMeanSectors(TArrayI(6, s0), outer);

    if (meansigi<0 || meansigo<0)
    {
        cout << "ERRROR - MeanInterp'd inner or outer < 0 " << endl;
        cout << "MeanInterp'd inner " << meansigi << endl;
        cout << "MeanInterp'd outer " << meansigo << endl;
        return 2;
    }

    TString meansiginner =Form("%6.2f", meansigi);
    TString meansigouter =Form("%6.2f", meansigo);

    TString calpos = cal ? Form("%5.1f", cal->GetMean()) : "NULL";

    if (pul->GetMean()<0 || pul->GetRMS()<0)
    {
        cout << "ERROR - PulsePos'd mean or rms < 0 " << endl;
        cout << "PulsePos'd mean " << pul->GetMean() << endl;
        cout << "PulsePos'd rms  " << pul->GetRMS() << endl;
        return 2;
    }

    TString meanpulpos = Form("%6.2f", pul->GetMean());
    TString rmspulpos  = Form("%6.2f", pul->GetRMS());

/*
    Double_t meanhi  = TMath::Nint(pulhi->GetMean()*100.)/100.;
    Double_t rmshi   = TMath::Nint(pulhi->GetRMS() *100.)/100.;

    Double_t meanlo  = TMath::Nint(pullo->GetMean()*100.)/100.;
    Double_t rmslo   = TMath::Nint(pullo->GetRMS() *100.)/100.;
    pullo->Add(pullo, pulhi, 1, -1);
    pullo->ResetBit(MHCamera::kProfile);

    Double_t meanoff = TMath::Nint(pullo->GetMean()*100.)/100.;
    Double_t rmsoff  = TMath::Nint(pullo->GetRMS() *100.)/100.;

    // USE MEDIAN INSTEAD? GetQuantiles(Int_t nprobSum, Double_t *q, const Double_t *probSum=0);

    TString meanpulhi    =Form("%6.2f", meanhi);
    TString rmspulhi     =Form("%6.2f", rmshi);

    TString meanpullo    =Form("%6.2f", meanlo);
    TString rmspullo     =Form("%6.2f", rmslo);
    */

    cam = (MHCamera*)arr.FindObjectInCanvas("Unsuitable;avg", "MHCamera", "Unsuitable");
    if (!cam)
    {
        cout << "ERROR - Reading of Unsuitable;avg failed." << endl;
        return 2;
    }

    Int_t unsuitable50 = cam->GetNumBinsAboveThreshold(0.50);
    Int_t unsuitable01 = cam->GetNumBinsAboveThreshold(0.01);

    TString unsuitablemax = "NULL";
    TString deadmax       = "NULL";

    TGraph *gr = (TGraph*)arr.FindObjectInCanvas("BadPixTm", "TGraph", "BadPixTm");
    if (gr)
    {
        const Int_t p = TMath::FloorNint(gr->GetN()*0.999);
        unsuitablemax = Form("%d", TMath::Nint(TMath::KOrdStat(gr->GetN(), gr->GetY(), p)));
    }

    gr = (TGraph*)arr.FindObjectInCanvas("DeadPixTm", "TGraph", "DeadPixTm");
    if (gr)
        deadmax = Form("%d", TMath::Nint(TMath::MaxElement(gr->GetN(), gr->GetY())));

    TString rateped  = "NULL";
    TString rateped2 = "NULL";
    TString ratecal  = "NULL";
    TString ratetrig = "NULL";
    TString ratesum  = "NULL";
    TString ratena   = "NULL";
    TString ratenull = "NULL";

    TH2D *htp = (TH2D*)arr.FindObjectInCanvas("TrigPat", "TH2D", "TrigPat");
    if (htp)
    {
        htp->ResetBit(TH1::kCanRebin);

        Int_t iped  = htp->GetYaxis()->FindBin("Ped");
        Int_t iped2 = htp->GetYaxis()->FindBin("Ped+Trig");
        Int_t ical  = htp->GetYaxis()->FindBin("Cal");
        Int_t itrig = htp->GetYaxis()->FindBin("Trig");
        Int_t isum  = htp->GetYaxis()->FindBin("Sum");
        Int_t inull = htp->GetYaxis()->FindBin("0");
        Int_t ina   = htp->GetYaxis()->FindBin("UNKNOWN");

        Int_t nx = htp->GetNbinsX();

        rateped  = iped <0 ? "NULL" : Form("%8.1f", htp->Integral(1, nx, iped,  iped)  / nx);
        rateped2 = iped2<0 ? "NULL" : Form("%7.2f", htp->Integral(1, nx, iped2, iped2) / nx);
        ratecal  = ical <0 ? "NULL" : Form("%8.1f", htp->Integral(1, nx, ical,  ical)  / nx);
        ratetrig = itrig<0 ? "NULL" : Form("%8.1f", htp->Integral(1, nx, itrig, itrig) / nx);
        ratesum  = isum <0 ? "NULL" : Form("%8.1f", htp->Integral(1, nx, isum,  isum)  / nx);
        ratenull = inull<0 ? "NULL" : Form("%8.1f", htp->Integral(1, nx, inull, inull) / nx);
        ratena   = ina  <0 ? "NULL" : Form("%7.2f", htp->Integral(1, nx, ina,   ina)   / nx);
    }

    // *****************************************************

    cout << "Sequence M" << seq.GetTelescope() << "/" << seq.GetSequence() << endl;
    cout << "  Mean Ped RMS inner [phe] " << meanrmsinner  << endl;
    cout << "  Mean Ped RMS outer [phe] " << meanrmsouter  << endl;
    cout << "  Mean Signal  inner [phe] " << meansiginner  << endl;
    cout << "  Mean Signal  outer [phe] " << meansigouter  << endl;
    cout << "  Mean extracted  PulsePos " << meanextpulpos << " +- " << rmsextpulpos << endl;
    cout << "  Mean calibrated PulsePos " << meanpulpos    << " +- " << rmspulpos    << endl;
    cout << "  Mean calib pulse pos     " << calpos << endl;
//    cout << "  Mean ext.HiGain PulsePos " << meanpulhi     << " +- " << rmspulhi     << endl;
//    cout << "  Mean ext.LoGain PulsePos " << meanpullo     << " +- " << rmspullo     << endl;
    cout << "  Lo-Hi gain offset:      " << medpuloff    << " +-  " << devpuloff    << endl;
    cout << "  Hi/Lo gain ratio:        " << medhilocal   << " +- " << devhilocal   << endl;
    cout << "  Unsuitable > 50%:       " << setw(6) << unsuitable50 << endl;
    cout << "  Unsuitable >  1%:       " << setw(6) << unsuitable01 << endl;
    cout << "  UnsuitableMax (99.9%)   " << setw(6) << unsuitablemax << endl;
    cout << "  DeadMax                 " << setw(6) << deadmax << endl;
    cout << "  Rate Trigger       [Hz] "  << ratetrig << endl;
    cout << "  Rate SUM           [Hz] "  << ratesum  << endl;
    cout << "  Rate Ped+Trigger   [Hz]  " << rateped2 << endl;
    cout << "  Rate Pedestal      [Hz] "  << rateped  << endl;
    cout << "  Rate Calibration   [Hz] "  << ratecal  << endl;
    cout << "  Rate 0             [Hz] "  << ratenull << endl;
    cout << "  Rate UNKNOWN       [Hz]  " << ratena   << endl;
    cout << endl;

    //build query and insert information into the database
    // here only a update query is built, as this macro is exexuted after
    // the macro fillcalib.C in the script fillcallisto
    // and so the table Calibration is always updated
    TString vars = Form(" fMeanPedRmsInner=%s,   fMeanPedRmsOuter=%s,  "
                        " fMeanSignalInner=%s,   fMeanSignalOuter=%s,  "
                        " fPulsePosMean=%s,      fPulsePosRms=%s,      "
                        " fPulsePosCheckMean=%s, fPulsePosCheckRms=%s, "
                        " fPulsePosCalib=%s, "
                        //" fPulsePosHiMean=%s,    fPulsePosHiRms=%s,    "
                        //" fPulsePosLoMean=%s,    fPulsePosLoRms=%s,    "
                        " fPulsePosOffMed=%s,    fPulsePosOffDev=%s,   "
                        " fHiLoGainRatioMed=%s,  fHiLoGainRatioDev=%s, "
                        " fUnsuitable50=%d,      fUnsuitable01=%d, "
                        " fUnsuitableMax=%s,     fDeadMax=%s, "
                        " fRateTrigEvts=%s,      fRateSumEvts=%s, "
                        " fRatePedTrigEvts=%s,   fRatePedEvts=%s, "
                        " fRateCalEvts=%s,       fRateNullEvts=%s, "
                        " fRateUnknownEvts=%s ",
                         meanrmsinner.Data(),  meanrmsouter.Data(),
                         meansiginner.Data(),  meansigouter.Data(),
                         meanpulpos.Data(),    rmspulpos.Data(),
                         meanextpulpos.Data(), rmsextpulpos.Data(),
                         calpos.Data(),
                         //meanpulhi.Data(),     rmspulhi.Data(),
                         //meanpullo.Data(),     rmspullo.Data(),
                         medpuloff.Data(),     devpuloff.Data(),
                         medhilocal.Data(),    devhilocal.Data(),
                         unsuitable50,         unsuitable01,
                         unsuitablemax.Data(), deadmax.Data(),
                         ratetrig.Data(),      ratesum.Data(),
                         rateped2.Data(),      rateped.Data(),
                         ratecal.Data(),       ratenull.Data(),
                         ratena.Data()
                       );

    TString where = Form("fSequenceFirst=%d AND fTelescopeNumber=%d",
                         seq.GetSequence(), seq.GetTelescope());
    return serv.Update("Calibration", vars, where) ? 1 : 2;
}

int fillsignal(TString fname, Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "fillsignal" << endl;
    cout << "----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "File: " << fname << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    //process file
    return Process(serv, fname);
}
