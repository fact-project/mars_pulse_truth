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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// testenv.C
// =========
//
// This example reads an config-file (steering card, input card, ...)
// The contents of the file a forwarded to the apropriate eventloop
// setup in your file.
//
// All tasks and containers in an eventloop should implement the
// ReadEnv/WriteEnv function (for an example see the tasks used below).
//
// The functions gets the corresponding setup data from the file as an
// argument and can change their behaviour and setup on this information.
//
///////////////////////////////////////////////////////////////////////////

void testenv()
{
    // Setup for all MHMatrix objects is done by:
    // MHMatrix.Column0: ...
    //
    // This can be overwritten for a MHMatrix called MatrixGammas by:
    // MatrixGammas.Column0;
    //
    // This can be overwritten for all MHMatrix in one Job by:
    // Job1.MHMatrix.Column0;
    //
    // This can be overwritten for a MHMatrix called MatrixGammas in one Job by:
    // Job1.MatrixGammas.Column0;
    //
    TEnv env(".marsrc");

    //
    // For developers: Set this to kTRUE to see how the TEnv file
    // entries are checked.
    //
    Bool_t print = kFALSE;

    // ------------ Job 1 -------------------
    if (env.GetValue("Job1", kFALSE))
    {
        cout << "++++++++++++++++++ Job 1 +++++++++++++++++++" << endl;
        MParList plist1;
        MTaskList tlist1;

        plist1.AddToList(&tlist1);

        MReadMarsFile read1("Events");

        MHMatrix matrix1("MatrixGammas");

        MFillH fillm1(&matrix1);
        plist1.AddToList(&matrix1);

        tlist1.AddToList(&read1);
        tlist1.AddToList(&fillm1);

        MEvtLoop evtloop1("Job1");
        evtloop1.SetParList(&plist1);
        cout << "--------------------------------------------" << endl;
        evtloop1.ReadEnv(env, "", print);
        cout << "--------------------------------------------" << endl;
        evtloop1.Eventloop();
        cout << endl;
    }

    // ------------ Job 2 -------------------

    if (env.GetValue("Job2", kTRUE))
    {
        cout << "++++++++++++++++++ Job 2 +++++++++++++++++++" << endl;
        MParList plist2;
        MTaskList tlist2;

        plist2.AddToList(&tlist2);

        MReadMarsFile read2("Events");

        MHMatrix matrix2("MatrixGammas");

        MFillH fillm2(&matrix2);
        plist2.AddToList(&matrix2);

        tlist2.AddToList(&read2);
        tlist2.AddToList(&fillm2);

        MEvtLoop evtloop2("Job2");
        evtloop2.SetParList(&plist2);
        cout << "--------------------------------------------" << endl;
        evtloop2.ReadEnv(env, "", print);
        cout << "--------------------------------------------" << endl;
        evtloop2.Eventloop();
        cout << endl;
    }
}
