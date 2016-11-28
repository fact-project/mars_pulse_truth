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
!   Author(s): Daniel Hoehne-Moench, 01/2009 <mailto:hoehne@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillmcstar.C
// ==========
//
// This macro is used to read the star-output files.
// These files are automatically called star00000000.root.
// From these files the number of islands and a parameter describing the
// inhomogeneity are extracted from the status display.
// The sequence number is extracted from the filename.
//
// Usage:
//   .x fillmcstar.C("/magic/montecarlo/star/0002/00028270/star00028270.root", kTRUE)
//
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. This is usefull
// for tests.
//
// The macro can also be run without ACLiC but this is a lot slower...
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b fillmcstar.C+\(\"filename\"\,kFALSE\)
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

    MHCamera *hevts = (MHCamera*)arr.FindObjectInCanvas("Sparkless;avg", "MHCamera", "Sparkless");
    if (!hevts)
    {
        cout << "ERROR - Reading of Sparkless failed." << endl;
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

    Int_t numsparks = (int)hsparks->GetEntries();
    Int_t numevents = (int)hevts->GetEntries();

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

    cout << "Sequence " << seq.GetSequence() << endl;
    cout << "  Inhomogeneity            " << inhomogen << endl;
    cout << "  Island Quality           " << islands   << endl;
    cout << "  Muon Number              " << num       << endl;
    cout << "  # of Events (w/o sparks) " << numevents << endl;
    cout << "  # of Sparks              " << numsparks << endl;

    TString vars = Form(" fSequenceFirst=%d, "
                        " fMeanNumberIslands=%s,"
                        " fMuonNumber=%d,"
                        " fNumEvts=%d, "
                        " fNumSparks=%d, "
                        " fInhomogeneity=%s ",
                        seq.GetSequence(),
                        islands.Data(),
                        num,
                        numevents, numsparks,
                        inhomogen.Data());

    TString where = Form("fSequenceFirst=%d", seq.GetSequence());

    return serv.InsertUpdate("MCStar", vars, where) ? 1 : 2;
}

int fillmcstar(TString fname, Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "fillmcstar" << endl;
    cout << "--------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "File: " << fname << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    return Process(serv, fname);
}
