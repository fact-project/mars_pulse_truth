/* ======================================================================== *\
! $Name: not supported by cvs2svn $:$Id: writedatasetfile.C,v 1.3 2008-07-05 19:01:43 tbretz Exp $
! --------------------------------------------------------------------------
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
!   Author(s): Thomas Bretz, 11/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// writedatasetfile.C
// ==================
//
// reads the dataset information from the database and writes it into a
// txt file
//
// Usage:
//  .x writedatasetfile.C+(dsno, "dspath")
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
#include <fstream>

#include <TSQLRow.h>
#include <TSQLResult.h>

#include <errno.h>

#include "MSQLMagic.h"

using namespace std;

Int_t GetDataOnOff(MSQLMagic &serv, Int_t dsno, Int_t type, TString &data)
{
    //get sequence information from database
    TString query1(Form("SELECT fSequenceFirst FROM DataSetSequenceMapping "
                       " WHERE fDataSetNumber=%d AND fOnOff=%d", dsno, type));

    TSQLResult *res = serv.Query(query1);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query1 << endl;
        return 2;
    }
    if (res->GetRowCount()==0 && type==1)
    {
        cout << "ERROR - Dataset #" << dsno << " has no on-sequences." << endl;
        return 2;
    }

    TSQLRow *row = 0;
    while ((row=res->Next()))
    {
        data += " ";
        data += (*row)[0];
    }

    delete res;

    return kTRUE;
}

Int_t Process(MSQLMagic &serv, Int_t dsno, TString seqpath)
{
    //get sequence information from database
    TString query0(Form("SELECT fSourceName, fComment, fDataSetName, fRunTime, fObservationModeKEY"
                       " FROM DataSets LEFT JOIN Source USING (fSourceKEY)"
                       " WHERE fDataSetNumber=%d", dsno));

    TSQLResult *res = serv.Query(query0);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query0 << endl;
        return 2;
    }
    if (res->GetRowCount()==0)
    {
        cout << "ERROR - No dataset #" << dsno << " found." << endl;
        return 2;
    }
    if (res->GetRowCount()!=1)
    {
        cout << "ERROR - Wrong number of datasets #" << dsno << " found." << endl;
        return 2;
    }

    TSQLRow *row = res->Next();

    TString source  = (*row)[0];
    TString comment = (*row)[1];
    TString name    = (*row)[2];
    TString runtime = (*row)[3];
    TString mode    = (*row)[4];
    TString on      = "SequencesOn:";
    TString off     = "SequencesOff:";

    const Bool_t wobble = mode.Atoi()==2;

    delete res;


    Int_t rc1 = GetDataOnOff(serv, dsno, 1, on);
    if (rc1!=kTRUE)
        return rc1;

    Int_t rc2 = GetDataOnOff(serv, dsno, 2, off);
    if (rc2!=kTRUE)
        return rc1;


    if (!seqpath.IsNull() && !seqpath.EndsWith("/"))
        seqpath += "/";
    seqpath += Form("dataset%08d.txt", dsno);

    ofstream fout(seqpath);
    if (!fout)
    {
        cout << "ERROR - Cannot open file " << seqpath << ": " << strerror(errno) << endl;
        return 2;
    }

    fout << "AnalysisNumber: " << dsno << endl << endl;
    fout << on << endl;
    if (off.Length()>13)
        fout << off << endl;
    fout << endl;
    fout << "SourceName: " << source << endl;
    fout << "Catalog: /magic/datacenter/setup/magic_favorites_dc.edb" << endl;
    if (wobble)
        fout << "WobbleMode: On" << endl;
    fout << endl;
    fout << "RunTime: " << runtime << endl;
    fout << "Name: " << name << endl;
    fout << "Comment: " << comment << endl;
    fout << endl;

    return 1;
}

int writedatasetfile(Int_t dsno, TString seqpath)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "writedatasetfile" << endl;
    cout << "----------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    return Process(serv, dsno, seqpath);
}
