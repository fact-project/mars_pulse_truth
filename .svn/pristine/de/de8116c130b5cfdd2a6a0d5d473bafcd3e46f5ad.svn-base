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
!   Author(s): Daniel Hoehne-Moench, 01/2009 <mailto:hoehne@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillmcsignal.C
// ============
//
// This macro is used to read the calibration-/callisto-output files
// signal00000.root.
//
// From this file the mean pedrms, the mean signal and the pulse position
// for the inner and outer camera is extracted and inserted into the database
// in the table MCCalibration, where the results of callisto are stored.
// The sequence number is extracted from the filename.
//
// Usage:
//   .x fillmcsignal.C("/magic/montecarlo/callisto/0002/00028270/signal00028270.root", kTRUE)
//
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests.
//
// The macro can also be run without ACLiC but this is a lot slower...
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b fillmcsignal.C+\(\"filename\"\,kFALSE\) 2>&1 | tee fillsignal.log
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

#include <TFile.h>
#include <TGraph.h>
#include <TSQLResult.h>
#include <TSQLRow.h>

#include "MSQLServer.h"
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

    TString query(Form("SELECT MCSequences.fAmplFadcKEY FROM MCSequences LEFT JOIN "
                       "AmplFadc on MCSequences.fAmplFadcKEY=AmplFadc.fAmplFadcKEY "
                       "where fSequenceFirst=%d;",seq.GetSequence()));

    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query << endl;
        return 2;
    }

    TSQLRow *row = res->Next();
    if (!row)
    {
        cout << "ERROR - Query failed: " << query << endl;
        return 2;
    }
    TString epochkey  = (*row)[0];
    Int_t epoch = atoi(epochkey.Data());

    delete res;

    if (epoch!=1)
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
        cout << "ERROR - MeanInterp'd inner or outer < 0 " << endl;
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

    cam = (MHCamera*)arr.FindObjectInCanvas("Unsuitable;avg", "MHCamera", "Unsuitable");
    if (!cam)
    {
        cout << "ERROR - Reading of Unsuitable;avg failed." << endl;
        return 2;
    }

    Int_t unsuitable50 = cam->GetNumBinsAboveThreshold(0.50);
    Int_t unsuitable01 = cam->GetNumBinsAboveThreshold(0.01);

    // *****************************************************

    cout << "Sequence #" << seq.GetSequence() << endl;
    cout << "  Mean Ped RMS inner [phe] " << meanrmsinner  << endl;
    cout << "  Mean Ped RMS outer [phe] " << meanrmsouter  << endl;
    cout << "  Mean Signal  inner [phe] " << meansiginner  << endl;
    cout << "  Mean Signal  outer [phe] " << meansigouter  << endl;
    cout << "  Mean extracted  PulsePos " << meanextpulpos << " +- " << rmsextpulpos << endl;
    cout << "  Mean calibrated PulsePos " << meanpulpos    << " +- " << rmspulpos    << endl;
    cout << "  Mean calib pulse pos     " << calpos << endl;
    cout << "  Lo-Hi gain offset:       " << medpuloff    << " +- " << devpuloff    << endl;
    cout << "  Hi/Lo gain ratio:        " << medhilocal   << " +- " << devhilocal   << endl;
    cout << "  Unsuitable > 50%:       " << setw(6) << unsuitable50 << endl;
    cout << "  Unsuitable >  1%:       " << setw(6) << unsuitable01 << endl;
    cout << endl;

    //build query and insert information into the database
    // here only a update query is built, as this macro is exexuted after
    // the macro fillmccalib.C in the script fillmccallisto
    // and so the table MCCalibration is always updated
    TString vars = Form(" fMeanPedRmsInner=%s,    fMeanPedRmsOuter=%s,  "
                         " fMeanSignalInner=%s,   fMeanSignalOuter=%s,  "
                         " fPulsePosMean=%s,      fPulsePosRms=%s,      "
                         " fPulsePosCheckMean=%s, fPulsePosCheckRms=%s, "
                         " fPulsePosOffMed=%s,    fPulsePosOffDev=%s,   "
                         " fHiLoGainRatioMed=%s,  fHiLoGainRatioDev=%s, "
                         " fUnsuitable50=%d,      fUnsuitable01=%d,     "
                         " fPulsePosCalib=%s ",
                         meanrmsinner.Data(),  meanrmsouter.Data(),
                         meansiginner.Data(),  meansigouter.Data(),
                         meanpulpos.Data(),    rmspulpos.Data(),
                         meanextpulpos.Data(), rmsextpulpos.Data(),
                         medpuloff.Data(),     devpuloff.Data(),
                         medhilocal.Data(),    devhilocal.Data(),
                         unsuitable50,         unsuitable01,
                         calpos.Data());

    TString where = Form("fSequenceFirst=%d", seq.GetSequence());
    return serv.Update("MCCalibration", vars, where) ? 1 : 2;

}

int fillmcsignal(TString fname, Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "fillmcsignal" << endl;
    cout << "----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "File: " << fname << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    //process file
    return Process(serv, fname);
}
