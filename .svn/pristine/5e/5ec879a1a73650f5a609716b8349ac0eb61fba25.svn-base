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
!   Author(s): Thomas Bretz, 08/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillganymed.C
// =============
//
// This macro is used to read the ganymed-output files.
// These files are automatically called ganymed00000.root.
// From MAlphaFitter and the Status Display the results from ganymed are
// extracted an inserted into the database, where they are stored in the table
// Ganymed.
// The dataset number is extracted from the filename.
//
// Usage:
//  .x fillganymed.C("/magic/data/ganymed/0000/0001/ganmymed0001.root", kTRUE)
//
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests.
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b fillganymed.C+\(\"filename\"\,kFALSE\) 2>&1 | tee file.log
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

#include <TH1.h>
#include <TGraph.h>
#include <TProfile.h>
#include <TFile.h>
#include <TSQLResult.h>
#include <TSQLRow.h>

#include "MSQLMagic.h"

#include "MStatusArray.h"
#include "MGeomCamMagic.h"
#include "MAlphaFitter.h"

using namespace std;

int Process(MSQLMagic &serv, TString fname, Bool_t dummy)
{
    TFile file(fname, "READ");
    if (!file.IsOpen())
    {
        cout << "ERROR - Could not find file " << fname << endl;
        return 2;
    }

    //get excess, signal, background events and the scale factor from MAlphaFitter
    MAlphaFitter *fit;
    file.GetObject("MAlphaFitter", fit);

    if (!fit)
    {
        cout << "WARNING - Reading of MAlphaFitter failed." << endl;
        return 2;
    }

    Int_t exc = (Int_t)fit->GetEventsExcess();
    Int_t sig = (Int_t)fit->GetEventsSignal();
    Int_t bgd = (Int_t)fit->GetEventsBackground();
    Float_t S = fit->GetSignificance();
    TString signif = Form("%5.1f", S);
    Float_t f = fit->GetScaleFactor();
    TString scale = Form("%5.2f", f);

    //get effective ontime from the status display
    MStatusArray arr;
    if (arr.Read()<=0)
    {
        cout << "ERROR - Reading of MStatusDisplay failed." << endl;
        return 2;
    }

    TH1D *vstime = (TH1D*)arr.FindObjectInCanvas("Theta",  "TH1D", "OnTime");
    if (!vstime)
    {
        cout << "WARNING - Reading of Theta failed." << endl;
        return 2;
    }


    Int_t tm = (Int_t)vstime->Integral();

    //get dataset number from filename
    TString dataset = fname(TRegexp("ganymed[0-9]+[.]root$"));
    if (dataset.IsNull())
    {
        cout << "WARNING - Could get dataset# from filename: " << fname << endl;
        return 2;
    }

    Int_t ds = atoi(dataset.Data()+8);

    cout << "Dataset #" << ds << endl;
    cout << "  Excess     Events:  " << exc    << endl;
    cout << "  Background Events:  " << bgd    << endl;
    cout << "  Signal     Events:  " << sig    << endl;
    cout << "  Significance:       " << signif << endl;
    cout << "  Scale Factor:       " << scale  << endl;
    cout << "  Total eff. on-time: " << tm     << "s = " << tm/3600. << "h" << endl;

//    cout << "  Excess       Rate:  " << exc*60/tm << " evts/min"  << endl;
//    cout << "  Background   Rate:  " << bgd*60/tm << " evts/min"  << endl;
//    cout << "  Significance Rate:  " << S/TMath::Sqrt(tm/3600.) << " sigma/sqrt(h)" << endl;

    TString vars =  Form(" fDataSetNumber=%d, "
                         " fExcessEvents=%d, "
                         " fBackgroundEvents=%d, "
                         " fSignalEvents=%d, "
                         " fSignificance=%s, "
                         " fScaleFactor=%s, "
                         " fEffOnTime=%d ",
                         ds, exc, bgd, sig,
                         signif.Data(),
                         scale.Data(), tm);


    TString where = Form("fDataSetNumber=%d", ds);

    return serv.InsertUpdate("Ganymed", vars, where) ? 1 : 2;
}

int fillganymed(TString fname, Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "fillganymed" << endl;
    cout << "-----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "File: " << fname << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    return Process(serv, fname, dummy);
}
