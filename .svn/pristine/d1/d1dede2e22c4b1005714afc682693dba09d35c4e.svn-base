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
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// writesequencefile.C
// ===================
//
// reads the sequence information from the database and writes it into a
// txt file
//
// Usage:
//  .x writesequencefile.C+(sequno,"sequpath")
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Returns 2 in case of failure, 1 in case of success and 0 if the connection
// to the database is not working.
//
//
// This tool will work from Period017 (2004_05_17) on...
//
// For more details see MSequence and MSequenceSQL
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include "MSQLMagic.h"
#include "MSequenceSQL.h"

using namespace std;

int writesequencefile(Int_t sequno, Int_t tel, TString sequpath="")
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "writesequencefile" << endl;
    cout << "-----------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "Sequence:  " << sequno << endl;
    cout << "Telescope: " << tel << endl;
    cout << endl;

    const MSequenceSQL s(serv, sequno, tel);
    if (!s.IsValid())
        return 2;

    if (sequpath.IsNull())
    {
        s.Print();
        return 1;
    }

    const TString fname(Form("%s/%04d/sequence%08d.txt", sequpath.Data(), sequno/10000, sequno));

    cout << "File:     " << fname << endl;

    return s.WriteFile(fname) ? 1 : 2;
}
