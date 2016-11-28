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
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

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

    // First Task: Read file with image parameters
    // (created with the star.C macro)

    MReadMarsFile read("Events", fname);
    read.DisableAutoScheme();

    MGeomApply geomapl;
    MPedestalSum  ncalc;
    //MCerPhotCalc  ncalc;
    //MCerPhotAnal  ncalc;
    //MCerPhotAnal2 ncalc;

    tlist.AddToList(&read);
    tlist.AddToList(&geomapl);
    tlist.AddToList(&ncalc);

    MHCamEvent hist;
    hist.SetType(3);
    plist.AddToList(&hist);

    MFillH fill("MHCamEvent", "MCerPhotEvt");
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

    TCanvas *c = MH::MakeDefCanvas();
    c->Divide(3, 2);

    MHCamEvent &h = *(MHCamEvent*)plist->FindObject("MHCamEvent");
    MHCamera *disp0 = h.GetHistByName();
    MHCamera *disp1 = new MHCamera(geom, "MCerPhotEvt;avg", "Cerenkov Photons RMS Avarage");
    MHCamera *disp2 = new MHCamera(geom, "MCerPhotEvt;err", "Cerenkov Photons RMS Error");
    MHCamera *disp3 = new MHCamera(geom, "MCerPhotEvt;rel", "Cerenkov Photons RMS ERR/VAL");
    disp1->SetBit(kCanDelete);
    disp2->SetBit(kCanDelete);
    disp3->SetBit(kCanDelete);

    disp1->SetCamContent(*disp0, 0);
    disp2->SetCamContent(*disp0, 1);
    disp3->SetCamContent(*disp0, 2);

    disp2->SetStats(kFALSE);
    disp3->SetStats(kFALSE);

    disp1->SetYTitle("a.u.");
    disp2->SetYTitle("err");
    disp3->SetYTitle("rel.err [%]");

    TText text(0.1, 0.5, &fname[fname.Last('/')+1]);
    text.SetTextSize(0.015);
    text.DrawClone();

    c->cd(1);
    disp1->Draw("hist");

    c->cd(2);
    gPad->SetLogy();
    disp2->Draw("hist");

    c->cd(3);
    gPad->SetLogy();
    disp3->Draw("hist");

    c->cd(4);
    gPad->SetBorderMode(0);
    gPad->Divide(1,1);
    gPad->cd(1);
    disp1->Draw();

    c->cd(5);
    gPad->SetBorderMode(0);
    gPad->Divide(1,1);
    gPad->cd(1);
    disp2->Draw();

    c->cd(6);
    gPad->SetBorderMode(0);
    gPad->Divide(1,1);
    gPad->cd(1);
    disp3->Draw();

    c->SaveAs(fname(0, fname.Last('.')+1) + "ps");
    //c->SaveAs(fname(0, fname.Last('.')+1) + "root");
}

// -------------------------------------------------------------------------
//
//  plot.C
//
//  This macro shows how to fill and display a histogram using Mars
//
void sumeventsrms(const char *dirname=".")
{
    MDirIter Next;
    Next.AddDirectory(dirname, "*ped*.root", -1);

    TString fname;
    while (1)
    {
        fname = Next();
        if (fname.IsNull())
            break;

        ProcessFile(fname);
        return;
    }
}
