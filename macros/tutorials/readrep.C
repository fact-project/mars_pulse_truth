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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// This macro demonstrates how to read a central control report file.
// (rootification, see merpp, too)
//
// In a second eventloop it gives an example on how to read such a root file.
//
//////////////////////////////////////////////////////////////////////////////

void readrep(const char *fname="CC_2003_11_04_23_53_18.rep")
{
    //
    // Read a report file and write containers into a root file
    //
    MParList  plist;
    MTaskList tlist;
    plist.AddToList(&tlist);

    MReportFileRead read(fname);
    tlist.AddToList(&read);

    read.AddToList("DAQ");
    read.AddToList("Drive");
    read.AddToList("Camera");
    read.AddToList("Trigger");

    MWriteRootFile write("test.root");
    write.AddContainer("MReportCamera",      "Camera");
    write.AddContainer("MTimeCamera",        "Camera");
    write.AddContainer("MCameraAUX",         "Camera");
    write.AddContainer("MCameraCalibration", "Camera");
    write.AddContainer("MCameraCooling",     "Camera");
    write.AddContainer("MCameraHV",          "Camera");
    write.AddContainer("MCameraLV",          "Camera");
    write.AddContainer("MCameraLids",        "Camera");
    write.AddContainer("MReportTrigger",     "Trigger");
    write.AddContainer("MTimeTrigger",       "Trigger");
    write.AddContainer("MReportDrive",       "Drive");
    write.AddContainer("MTimeDrive",         "Drive");
    tlist.AddToList(&write);

    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();

    // ----------------------------------------------------------------
    //
    // Read a report file and write containers into a root file
    //
    MTaskList tlist2;
    plist.Replace(&tlist2);

    // Create a tasklist to process the read events from the Camera tree
    MTaskList list1("ProcessCamera");
    MPrint print1("MTimeCamera");
    list1.AddToList(&print1);

    // Create a tasklist to process the read events from the Drive tree
    MTaskList list2("ProcessDrive");
    MPrint print2("MTimeDrive");
    list2.AddToList(&print2);

    // Tell the reader to read the trees Drive, Trigger and Camera
    MReadReports read;
    read.AddTree("Drive");
    read.AddTree("Trigger");
    read.AddTree("Camera");
    //read.AddTree("Events", "MTime."); // for later use!

    // Now (not earlier!) set the file to read!
    read.AddFile("test.root");

    // First read the events
    tlist.AddToList(&read);
    // Process the events from the Camera tree with the task list list1
    tlist.AddToList(&list1, "Camera");
    // Process the events from the Camera tree with the task list list2
    tlist.AddToList(&list2, "Drive");

    // The task lists list1 and list2 are only executed (according to
    // their stream id - the second argument of AddToList) if a
    // corresponding event was read and MReadReports has set the stream
    // id accoringly. MReadReports always sets the stream id to the name
    // of the tree from which the event was read

    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    //
    // Execute the eventloop which should print the time-stamps of the subsystem
    // events from Drive and Camera in the correct order...
    //
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();
}
