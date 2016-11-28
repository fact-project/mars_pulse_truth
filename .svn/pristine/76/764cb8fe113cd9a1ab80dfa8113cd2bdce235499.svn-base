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
!   Author(s): Thomas Bretz, 6/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// sumcurrents.C
// =============
//
// This is a demonstration macro to display mean DC currents for all pixels.
// The input is cc report file. The output are histograms and plots.
// Using the MDirIter functionality you can process more than one file
// in one or more directories. For more information see MDirIter.
//
///////////////////////////////////////////////////////////////////////////

void ProcessFile(TString fname)
{
    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    //
    // Now setup the tasks and tasklist:
    // ---------------------------------
    //

    // Create the magic geometry
    MGeomCamMagic geom;
    plist.AddToList(&geom);

    // First Task: Read file with image parameters
    // (created with the star.C macro)
    MReportFileRead read(fname);
    read.SetHasNoHeader();
    read.AddToList("MReportCurrents");
    tlist.AddToList(&read);

    // create a task to fill a histogram
    MFillH fill("MHCamEvent", "MCameraDC");
    tlist.AddToList(&fill);

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    //
    // Execute your analysis
    //
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();

    //
    // Now display the result of the loop
    //
    MHCamEvent &h2 = *(MHCamEvent*)plist->FindObject("MHCamEvent");
    MHCamera &h = *(MHCamera*)h2.GetHistByName("sum");
;

    TCanvas *c = MH::MakeDefCanvas();
    c->Divide(3, 2);

    MHCamera *disp1=h.Clone();
    MHCamera *disp2=h.Clone();
    MHCamera *disp3=h.Clone();
    disp2->SetCamContent(h, 1);
    disp3->SetCamContent(h, 2);

    disp1->SetYTitle("I [nA]");
    disp2->SetYTitle("\\sigma_{I} [\\mu A]");
    disp3->SetYTitle("\\sigma_{I} [%]");
    disp1->SetName("Currents;avg");
    disp2->SetName("Currents;err");
    disp3->SetName("Currents;rel");
    disp1->SetTitle("Currents Average");
    disp2->SetTitle("Currents error");
    disp3->SetTitle("Currents relative error");

    c->cd(1);
    TText text(0.1, 0.95, &fname[fname.Last('/')+1]);
    text.SetTextSize(0.03);
    text.DrawClone();
    gPad->SetBorderMode(0);
    gPad->Divide(1,1);
    gPad->cd(1);
    gPad->SetLogy();
    disp1->Draw();
    disp1->SetBit(kCanDelete);
    c->cd(2);
    gPad->SetBorderMode(0);
    gPad->Divide(1,1);
    gPad->cd(1);
    gPad->SetLogy();
    disp2->Draw();
    disp2->SetBit(kCanDelete);
    c->cd(3);
    gPad->SetBorderMode(0);
    gPad->Divide(1,1);
    gPad->cd(1);
    gPad->SetLogy();
    disp3->Draw();
    disp3->SetBit(kCanDelete);
    c->cd(4);
    gPad->SetBorderMode(0);
    disp1->Draw("EPhist");
    c->cd(5);
    gPad->SetBorderMode(0);
    gPad->SetLogy();
    disp2->Draw("Phist");
    c->cd(6);
    gPad->SetBorderMode(0);
    gPad->SetLogy();
    disp3->Draw("Phist");

    c->SaveAs(fname(0, fname.Last('.')+1) + "ps");
    c->SaveAs(fname(0, fname.Last('.')+1) + "root");
}

void sumcurrents(const char *dirname=".")
{
    MDirIter Next;
    Next.AddDirectory(dirname, "dc_*.txt", -1);

    TString fname;
    while (1)
    {
        fname = Next();
        if (fname.IsNull())
            break;

        ProcessFile(fname);
    }
}
