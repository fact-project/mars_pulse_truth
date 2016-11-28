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

/////////////////////////////////////////////////////////////////////////////
//
// merpp.C
// =======
//
// This is an easy implementation of the Merging process (as root Macro)
//
// at the moment it reads a binary file ("rawtest.raw") which was written
// in the DAQ raw format.
//
// The data are stored in root container objects (classes derived from
// TObject like MRawRunHeader)
//
// This containers are written to a root file ("rawtest.root")
//
// It also demonstrates how you can loop over files in a single or more
// than one directory and process them. For details see MRunIter.
//
/////////////////////////////////////////////////////////////////////////////

void ProcessFile(TString fname)
{
    //
    // create a (empty) list of parameters which can be used by the tasks
    // and an (empty) list of tasks which should be executed
    //
    MTaskList tasks;
    MParList plist;

    plist.AddToList(&tasks);

    //
    // create the tasks which should be executed and add them to the list
    // in the case you don't need parameter containers, all of them can
    // be created by MRawFileRead::PreProcess
    //
    // REMARK: Don't change the order of the two instantiations.
    //         I don't have an idea why, but here it crashes the
    //         Interpreter.
    //         (Using root 2.25/03, with Cint 5.14.50 on OSF1)
    //
    MRawFileRead  reader(fname);
    MRawFileWrite writer(fname(0, fname.Last('.')+1) + "root", "RECREATE", fname, 1);
    tasks.AddToList(&reader);
    tasks.AddToList(&writer);

    //
    // create the looping object and thell it about the parameters to use
    // and the tasks to execute
    //
    MEvtLoop magic;

    magic.SetParList(&plist);

    //
    // Start the eventloop which reads the raw file (MRawFileRead) and
    // write all the information into a root file (MRawFileWrite)
    //
    // between reading and writing we can do, transformations, checks, etc.
    // (I'm think of a task like MRawDataCheck)
    //
    magic.Eventloop();
}

void merpp(const char *dirname="/home/MAGIC/online_data/rawdata/")
{
    MDirIter Next;
    Next.AddDirectory(dirname, "*.raw", -1);

    while (1)
    {
        TString fname = Next();
        if (fname.IsNull())
            break;

        ProcessFile(fname);
    }
}

