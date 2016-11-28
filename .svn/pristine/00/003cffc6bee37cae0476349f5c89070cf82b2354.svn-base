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
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillobjects2.C
// ==============
//
// read file with coordinates for the objects (only the blazars)
// File resources/TeVsources.txt
//
///////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <fstream>

#include <TEnv.h>
#include <TRegexp.h>

#include <TSQLRow.h>
#include <TSQLResult.h>

#include "MAstro.h"
#include "MDirIter.h"
#include "MSQLServer.h"
#include "MSQLMagic.h"

using namespace std;

// --------------------------------------------------------------------------
//
// loop over all files in this path
//
int fillobjects2(TString fname, Bool_t dummy=kTRUE)
{
    TEnv env("sql.rc");

    MSQLMagic serv(env);
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    serv.SetIsDummy(dummy);

    cout << endl;
    cout << "fillobjects" << endl;
    cout << "-----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    ifstream fin(fname);
    if (!fin)
    {
        cout << "Could not open file " << fname << endl;
        return 0;
    }

    Int_t num=0;
    TString object;
    TString query;
    TString where;
    TString RA;
    TString DEC;
    Double_t ra;
    Double_t dec;
    while (1)
    {
        TString line;
        line.ReadLine(fin);
        if (!fin)
            break;

        if (line.IsNull())
            continue;

        TObjArray *arr = line.Tokenize(" ");
        if (arr->GetEntries()!=10)
        {
            cout << "WARNING: line with less or more than 11 arguments found " << endl;
            cout << line << endl;
            continue;
        }
        object=Form("%s/BL",(*arr)[0]->GetName());
        RA=Form("%s:%s:%s", (*arr)[2]->GetName(), (*arr)[3]->GetName(), (*arr)[4]->GetName());
        DEC=Form("%s:%s:%s", (*arr)[5]->GetName(), (*arr)[6]->GetName(), (*arr)[7]->GetName());
        delete arr;
//        cout << "RA: " << RA << " -  DEC " << DEC << endl;
        MAstro::Coordinate2Angle(RA, ra);
        MAstro::Coordinate2Angle(DEC, dec);
//        cout << "ra: " << ra << " -  dec " << dec << endl;

        query=Form("fRightAscension=%.7f, fDeclination=%.7f, fEpoche=2000",
                   ra, dec);

        query+=Form(", fObjectName='%s'", object.Data());
        where=Form(" fObjectName='%s'", object.Data());

        if (serv.InsertUpdate("Object", query, where)==kFALSE)
            return 2;
        num +=1;

    }

    cout << fname << " <" << num << "> " << endl;

    return 1;
}
