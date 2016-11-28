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
!   Author(s): Daniela Dorner, 06/2005 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// createdataset.C
// ===============
//
// this script is not run automatically
// the purpose of this script is to make it easier for people to write
// datasetfiles
//
// be careful with the path in this macro, if you use it for test-reasons!
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <fstream>

#include <TEnv.h>

#include <MSQLServer.h>
#include <TSQLRow.h>
#include <TSQLResult.h>
#include <TSystem.h>

using namespace std;



int createdataset(Int_t dataset, TString source, TString wobble,
                  Bool_t cal=kFALSE,
                  TString startdate="0000-00-00 00:00:00",
                  TString stopdate="3000-00-00 00:00:00",
                  TString off="none",
                  TString offstartdate="0000-00-00 00:00:00",
                  TString offstopdate="3000-00-00 00:00:00")
{
    TEnv env("sql.rc");

    MSQLServer serv(env);
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }
    cout << "createdataset" << endl;
    cout << "-------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    TString fname(Form("/magic/datasets/%05d/dataset%08d.txt", dataset/1000, dataset));
    TString fhtml="/home/operator/html";
    Bool_t exist=gSystem->AccessPathName(fname,kFileExists);
    if (exist==0)
    {
        cout << fname << " already exists, please choose different dataset#" << endl;
        return 0;
    }

    ofstream fouthtml(fhtml, ios::app);
    fouthtml << "<tr><td>" << endl;
    fouthtml << dataset << endl;
    fouthtml << "</td><td>" << endl;
    fouthtml << source << endl;
    fouthtml << "</td><td>" << endl;
    if (wobble!="N")
        fouthtml << "wobble," << endl;
    if (startdate=="0000-00-00 00:00:00")
        fouthtml << "all" << endl;
    else
        fouthtml << "from " << startdate << " to " << stopdate << endl;
    if (cal)
        fouthtml << ", only data processed to the imgpar is taken into account" << endl;
    fouthtml << "</td></tr>" << endl;

    ofstream fout(fname, ios::app);
    cout << "writing to file " << fname << endl;
    fout << "AnalysisNumber: " << dataset << endl << endl;

    TString query="Select Sequences.fSequenceFirst from Sequences left join Source on ";
    query +="Sequences.fSourceKEY=Source.fSourceKEY ";
    if (cal)
        query +=" left join SequenceProcessStatus on Sequences.fSequenceFirst=SequenceProcessStatus.fSequenceFirst ";
    if (wobble=="N")
        query +=Form(" where fSourceName like '%s%%' and fRunStart between '%s' and '%s'",
                     source.Data(), startdate.Data(), stopdate.Data());
    else
        query +=Form(" where fSourceName like '%s%%' and fRunStart between '%s' and '%s'",
                     wobble.Data(), startdate.Data(), stopdate.Data());
    if (cal)
        query +=" and not IsNull(fStar) ";
    query +=" order by Sequences.fSequenceFirst ";

    cout << "Q: " << query << endl;

    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "Error - no sequence found" << endl;
        return 0;
    }

    fout << "SequencesOn: " << flush;
    TSQLRow *row=0;
    while ((row = res->Next()))
        fout << " " << (*row)[0] <<  " " << flush;
    fout << endl << endl;

    delete res;

    if (wobble=="N")
    {
        if (off=="none")
        {
            off  ="Off";
            off +=source;
            off.Remove(10,15);
        }

        cout << "off: " << off << endl;
        query  ="Select Sequences.fSequenceFirst from Sequences left join Source on ";
        query +="Sequences.fSourceKEY=Source.fSourceKEY ";
        if (cal)
            query +=" left join SequenceProcessStatus on Sequences.fSequenceFirst=SequenceProcessStatus.fSequenceFirst ";
        query +=Form("where fSourceName like '%s%%' and fRunStart between '%s' and '%s'",
                     off.Data(), offstartdate.Data(), offstopdate.Data());
        if (cal)
            query +=" and not IsNull(fStar) ";
        query +=" order by Sequences.fSequenceFirst ";

        cout << "Q: " << query << endl;

        res = serv.Query(query);
        if (!res)
        {
            cout << "Error - no sequence found" << endl;
            return 0;
        }

        fout << "SequencesOff: " << flush;
        row=0;
        while ((row = res->Next()))
            fout << " " << (*row)[0] <<  " " << flush;
        fout << endl << endl;

        delete res;
    }

    if (wobble=="N")
        fout << "#" << flush;
    fout << "SourceName: " << source << endl;
    fout << "Catalog: /magic/datacenter/setup/magic_favorites.edb" << endl;
    if (wobble=="N")
        fout << "#" << flush;
    fout << "WobbleMode: On" << endl << endl;


    return 1;
}

int createdataset(TString filename)
{
    ifstream fin(filename);
    if (!fin)
    {
        cout << "Could not open file " << filename << endl;
        return 0;
    }
    cout << "reading file " << filename << endl;

    while (1)
    {
        TString strng;
        strng.ReadToDelim(fin, ',');
        if (!fin)
            break;

        if (strng.BeginsWith("#"))
        {
            cout << "comment line: " << strng << endl;
            strng.ReadToDelim(fin, '\n');
            continue;
        }

        Int_t dataset=atoi(strng.Data());
        if (dataset==0)
            continue;
        cout << "creating dataset # " << dataset << endl;

        TString source;
        source.ReadToDelim(fin, ',');
        TString wobble;
        wobble.ReadToDelim(fin, ',');
        strng.ReadToDelim(fin, ',');
        Bool_t cal=kFALSE;
        if (strng=="kTRUE")
            cal=kTRUE;
        TString startdate;
        startdate.ReadToDelim(fin, ',');
        TString stopdate;
        stopdate.ReadToDelim(fin, ',');
        TString off;
        off.ReadToDelim(fin, ',');
        TString offstartdate;
        offstartdate.ReadToDelim(fin, ',');
        TString offstopdate;
        offstopdate.ReadToDelim(fin, '\n');

        cout << createdataset(dataset, source, wobble, cal, startdate, stopdate, off, offstartdate, offstopdate) << endl;
    }

    return 1;
}
