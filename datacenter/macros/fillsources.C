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
!   Author(s): Daniela Dorner, 01/2005 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillsources.C
// ============
//
// Returns 0 in case of failure and 1 in case of success.
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include <fstream>

#include <TSQLRow.h>
#include <TSQLResult.h>

#include "MSQLMagic.h"

using namespace std;

int fillsources(TString catalog, Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }
    cout << "insertsources" << endl;
    cout << "-------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "Catalog file: " << catalog << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    ifstream fin(catalog);
    if (!fin)
    {
        cout << "Could not open file " << catalog << endl;
        return 1;
    }

    while (1)
    {
        TString src, ra, de, dum, epoch;

        src.ReadToDelim(fin, ',');
        dum.ReadToDelim(fin, ',');
        ra.ReadToDelim(fin, ',');
        de.ReadToDelim(fin, ',');
        dum.ReadToDelim(fin, ',');
        epoch.ReadToDelim(fin, '\n');

        if (!fin)
            break;
/*
        Double_t r,d;
        if (!MAstro::Coordinate2Angle(ra, r))
        {
            cout << "ERROR - Interpreting right ascension: " << ra << endl;
            return 2;
        }
        if (!MAstro::Coordinate2Angle(de, d))
        {
            cout << "ERROR - Interpreting declination: " << de << endl;
            return 2;
        }
 */
        TString vars(Form("fRightAscension='%s', fDeclination='%s', fEpoch='%s'",
                           ra.Data(), de.Data(), epoch.Data()));

        TString where(Form("fSourceName='%s'", src.Data()));

        if (!serv.InsertUpdate("Source", vars, where))
            return 2;
    }

    return 1;
}
