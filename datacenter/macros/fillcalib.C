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
!   Author(s): Thomas Bretz, 08/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniela Dorner, 08/2004 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillcalib.C
// ===========
//
// This macro is used to read the calibartion-/callisto-output files.
// These files are automatically called calib00000.root.
//
// From this file the MBadPixelsCam and the MGeomCam is extracted. If
// the geometry isn't found MGeomCamMagic is used as a default.
// The bad pixel information and other information, extracted from the status
// display, is inserted into the database in the table Calibration, which
// stores the results from the calibration.
// The corresponding sequence number is extracted from the filename...
// FIXME: MSeqeuence should be stored in the calib-file?
//
// Usage:
//  .x fillcalib.C("/magic/data/callisto/0004/00047421/calib00047421.root", kTRUE)
//
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests.
//
// The macro can also be run without ACLiC but this is a lot slower...
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b fillcalib.C+\(\"filename\"\,kFALSE\) 2>&1 | tee fillcalib.log
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Returns 2 in case of failure, 1 in case of success and 0 if the connection
// to the database is not working.
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include <TH1.h>

#include <TFile.h>
#include <TSQLResult.h>
#include <TSQLRow.h>

#include "MSQLMagic.h"

#include "MStatusArray.h"
#include "MHCamera.h"
#include "MSequence.h"
#include "MGeomCamMagic.h"
#include "MBadPixelsCam.h"

using namespace std;

int Process(MSQLMagic &serv, TString fname)
{
    //getting number of unsuitable, unreliable and isolated pixel
    MBadPixelsCam badpix;

    TFile file(fname, "READ");
    if (!file.IsOpen())
    {
        cout << "ERROR - Could not find file " << fname << endl;
        return 2;
    }

    if (badpix.Read("MBadPixelsCam")<=0)
    {
        cout << "ERROR - Reading of MBadPixelsCam failed." << endl;
        return 2;
    }

    MGeomCamMagic def;

    MGeomCam *geom = (MGeomCam*)file.Get("MGeomCam");
    if (!geom)
    {
        cout << "WARNING - Reading of MGeomCam failed... using default <MGeomCamMagic>" << endl;
        geom = &def;
    }

    cout << "Camera Geometry: " << geom->ClassName() << endl;

    const Short_t unsin  = badpix.GetNumUnsuitable(MBadPixelsPix::kUnsuitableRun, geom, 0);
    const Short_t unsout = badpix.GetNumUnsuitable(MBadPixelsPix::kUnsuitableRun, geom, 1);

    const Short_t unrin  = badpix.GetNumUnsuitable(MBadPixelsPix::kUnreliableRun, geom, 0);
    const Short_t unrout = badpix.GetNumUnsuitable(MBadPixelsPix::kUnreliableRun, geom, 1);

    const Short_t isoin  = badpix.GetNumIsolated(*geom, 0);
    const Short_t isoout = badpix.GetNumIsolated(*geom, 1);

    const Short_t clumax = badpix.GetNumMaxCluster(*geom);

    if (unsin<0 || unsout<0 || unrin<0 || unrout<0 || isoin<0 || isoout<0 || clumax<0)
    {
        cout << "ERROR - one of the pixel values < 0." << endl;
        return 2;
    }

    //     MHCamera hist(geom);
    //     hist.SetCamContent(badpix, 1);
    //     hist.DrawCopy();
    //     hist.SetCamContent(badpix, 3);
    //     hist.DrawCopy();

    //Getting values from the status display
    MStatusArray arr;
    if (arr.Read()<=0)
    {
        cout << "ERROR - could not read MStatusArray." << endl;
        return 2;
    }

    TH1 *h;

    //getting the mean and rms from the arrival time (inner cam)
    h = (TH1*)arr.FindObjectInCanvas("HRelTimeHiGainArea0", "TH1F", "Time");
    if (!h)
    {
        cout << "ERROR - Could not find histogram HRelTimeHiGainArea0 in Time." << endl;
        return 2;
    }

    TString meaninner = Form("%5.1f", h->GetMean());
    TString rmsinner  = Form("%6.2f", h->GetRMS());

    //getting the mean and rms from the arrival time (outer cam)
    h = (TH1*)arr.FindObjectInCanvas("HRelTimeHiGainArea1", "TH1F", "Time");
    if (!h)
    {
        cout << "ERROR - Could not find histogram HRelTimeHiGainArea1 in Time." << endl;
        return 2;
    }

    TString meanouter = Form("%5.1f", h->GetMean());
    TString rmsouter  = Form("%6.2f", h->GetRMS());

    //Getting conversion factors
    MHCamera *c = (MHCamera*)arr.FindObjectInCanvas("TotalConvPhe", "MHCamera", "Conversion");
    if (!c)
    {
        cout << "ERROR - Could not find MHCamera TotalConv in Conversion." << endl;
        return 2;
    }

    TArrayI inner(1), outer(1);
    inner[0] = 0;
    outer[0] = 1;

    Int_t s0[] = { 1, 2, 3, 4, 5, 6 };

    Stat_t meanconvi = c->GetMeanSectors(TArrayI(6, s0), inner);
    Stat_t meanconvo = c->GetMeanSectors(TArrayI(6, s0), outer);
    TString meanconvinner=Form("%6.3f", meanconvi);
    TString meanconvouter=Form("%6.3f", meanconvo);

    //Getting relative charge rms
    c = (MHCamera*)arr.FindObjectInCanvas("RMSperMean", "MHCamera", "FitCharge");
    if (!c)
    {
        cout << "ERROR - Could not find MHCamera RMSperMean in FitCharge." << endl;
        return 2;
    }

    Stat_t relrmsi = c->GetMedianSectors(TArrayI(6, s0), inner);
    Stat_t relrmso = c->GetMedianSectors(TArrayI(6, s0), outer);
    TString relrmsinner=Form("%6.3f", relrmsi);
    TString relrmsouter=Form("%6.3f", relrmso);

    //Getting relative charge rms
    c = (MHCamera*)arr.FindObjectInCanvas("NumPhes", "MHCamera", "FitCharge");
    if (!c)
    {
        cout << "ERROR - Could not find MHCamera NumPhes in FitCharge." << endl;
        return 2;
    }

    Stat_t numphei = c->GetMedianSectors(TArrayI(6, s0), inner);
    Stat_t numpheo = c->GetMedianSectors(TArrayI(6, s0), outer);
    TString numpheinner=Form("%5.1f", numphei);
    TString numpheouter=Form("%5.1f", numpheo);

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

    //getting the ratio of calibration events used
    h = (TH1*)arr.FindObjectInCanvas("ArrTm;avg", "MHCamera", "ArrTm");
    if (!h)
    {
        cout << "ERROR - Could not find histogram ArrTime;avg." << endl;
        return 2;
    }

    UInt_t nevts = TMath::Nint(h->GetEntries());

    TString query;
    query = Form("SELECT SUM(fNumEvents) FROM RunData "
                 "LEFT JOIN RunType USING (fRunTypeKEY) "
                 "WHERE fSequenceFirst=%d AND fTelescopeNumber=%d AND "
                 "RunType.fRunTypeName='Calibration'",
                 seq.GetSequence(), seq.GetTelescope());

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

    Float_t ratiocalib = 100.*nevts/atof((*row)[0]);

    TString ratiocal = Form("%.1f", ratiocalib);

    delete res;

    cout << "Sequence M" << seq.GetTelescope() << ":" << seq.GetSequence() << endl;
    cout << "  Unsuitable: (i/o)  " << Form("%3d %3d", (int)unsin, (int)unsout) << endl; // Unbrauchbar
    cout << "  Unreliable: (i/o)  " << Form("%3d %3d", (int)unrin, (int)unrout) << endl; // Unzuverlaessig
    cout << "  Isolated:   (i/o)  " << Form("%3d %3d", (int)isoin, (int)isoout) << endl; // Isolated (unbrauchbar)
    cout << "  Max.Cluster:       " << Form("%3d", (int)clumax)                 << endl; // Max Cluster
    cout << "  Arr Time inner:     " << meaninner << " +- " << rmsinner  << endl;
    cout << "  Arr Time outer:     " << meanouter << " +- " << rmsouter  << endl;
    cout << "  Mean Conv inner:     " << meanconvinner << endl;
    cout << "  Mean Conv outer:     " << meanconvouter << endl;
    cout << "  Rel.charge rms in:   " << relrmsinner << endl;
    cout << "  Rel.charge rms out:  " << relrmsouter << endl;
    cout << "  Med. num phe inner: " << numpheinner << endl;
    cout << "  Med. num phe outer: " << numpheouter << endl;
    cout << "  Ratio Calib Evts:   " << ratiocal << endl;

    // FIXME: Fill calibration charge

    //inserting or updating the information in the database
    TString vars =
        Form(" fSequenceFirst=%d, "
             " fTelescopeNumber=%d, "
             " fUnsuitableInner=%d, "
             " fUnsuitableOuter=%d, "
             " fUnreliableInner=%d, "
             " fUnreliableOuter=%d, "
             " fIsolatedInner=%d, "
             " fIsolatedOuter=%d, "
             " fIsolatedMaxCluster=%d, "
             " fArrTimeMeanInner=%s, "
             " fArrTimeRmsInner=%s, "
             " fArrTimeMeanOuter=%s, "
             " fArrTimeRmsOuter=%s, "
             " fConvFactorInner=%s, "
             " fConvFactorOuter=%s, "
             " fRatioCalEvents=%s, "
             " fRelChargeRmsInner=%s, "
             " fRelChargeRmsOuter=%s, "
             " fMedNumPheInner=%s, "
             " fMedNumPheOuter=%s ",
             seq.GetSequence(), seq.GetTelescope(),
             (int)unsin, (int)unsout, (int)unrin,
             (int)unrout, (int)isoin, (int)isoout, (int)clumax,
             meaninner.Data(), rmsinner.Data(),
             meanouter.Data(), rmsouter.Data(),
             meanconvinner.Data(), meanconvouter.Data(),
             ratiocal.Data(),
             relrmsinner.Data(), relrmsouter.Data(),
             numpheinner.Data(), numpheouter.Data()
            );

    TString where = Form("fSequenceFirst=%d AND fTelescopeNumber=%d",
                         seq.GetSequence(), seq.GetTelescope());

    return serv.InsertUpdate("Calibration", vars, where) ? 1 : 2;
}

int fillcalib(TString fname, Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "fillcalib" << endl;
    cout << "---------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "File: " << fname << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    return Process(serv, fname);
}
