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
!   Author(s): Thomas Bretz, 5/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// This macro demonstrates how to fill MHPoinitingPos directly from a
// CC file. This may be helpfull when debugging reading starguider
// information from a CC file.
//
//////////////////////////////////////////////////////////////////////////////

void readstarguider(const char *fname="CC_2003_11_04_23_53_18.rep")
{
    //
    // Read a report file and write containers into a root file
    //
    MParList  plist;
    MTaskList tlist;
    plist.AddToList(&tlist);

    MReportFileReadCC read(fname);
    tlist.AddToList(&read);

    read.AddToList("MReportStarguider");

    MFillH fillh("MHPointing", "MTimeStarguider");
    tlist.AddToList(&fillh);

    MStatusDisplay *d = new MStatusDisplay;

    MEvtLoop evtloop;
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(d);

    if (!evtloop.Eventloop())
        return;
}
