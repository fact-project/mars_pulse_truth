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
// fillobjects.C
// ==============
//
// read file with magnitudes and colour for the objects
// File: resources/good_compstars_R.txt
//
// In resources also the file compstars_R.txt can be found, which contains
// more stars. As the value for the magnitude for these stars is not good
// enough, they are not inserted into the database, as the values there are
// used for the calculation of the extinction.
// E.g. resources/good_compstars_R.txt does not contain the blazars itself.
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
int fillobjects(TString fname, Bool_t dummy=kTRUE)
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
    TString comment;
    while (1)
    {
        TString line;
        line.ReadLine(fin);
        if (!fin)
            break;

        if (line.IsNull())
            continue;

        line.ReplaceAll("\t", " ");
        TObjArray *arr = line.Tokenize(" ");
        if (arr->GetEntries()!=5)
        {
            cout << "WARNING: line with less or more than 5 arguments found " << endl;
            cout << line << endl;
            return 2;
        }
        query=Form("fMagnitude=%s, fVRColour=%s, fObject=",
                   (*arr)[2]->GetName(), (*arr)[3]->GetName());
        comment=(*arr)[4]->GetName();
        if (comment=="-")
            query+="NULL";
        else
            query+=Form("'%s'", comment.Data());

        query+=Form(", fObjectName='%s/%s'", (*arr)[0]->GetName(), (*arr)[1]->GetName());
        where=Form(" fObjectName='%s/%s'", (*arr)[0]->GetName(), (*arr)[1]->GetName());
        delete arr;

        if (serv.InsertUpdate("Object", query, where)==kFALSE)
            return 2;
        num +=1;

    }

    cout << fname << " <" << num << "> " << endl;

    return 1;
}
