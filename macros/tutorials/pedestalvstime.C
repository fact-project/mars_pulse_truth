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

void ProcessFile(TString fname, Int_t idx)
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

    MCerPhotAnal2 ncalc;

    tlist.AddToList(&read);
    tlist.AddToList(&geomapl);
    tlist.AddToList(&ncalc);

    MHPixVsTime hist1(idx, "Pedestal");
    MHPixVsTime hist2(idx, "PedestalRMS");
    hist2.SetType(1);
    plist.AddToList(&hist1);
    plist.AddToList(&hist2);

    MFillH fill1("Pedestal",    "MPedestalCam");
    MFillH fill2("PedestalRMS", "MPedestalCam");
    tlist.AddToList(&fill1);
    tlist.AddToList(&fill2);

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
    c->Divide(2,2);

    c->cd(1);
    TGraph &g1 = hist1.GetGraph();
    TH1 *h1 = g1.GetHistogram();
    h1->SetXTitle("Time [au]");
    h1->SetYTitle("P [au]");
    g1.DrawClone("A*");

    c->cd(2);
    TH1F h1f("Pedestal", "Pedestals", 21, 46, 56);
    h1f.SetXTitle("P");
    h1f.SetYTitle("Counts");
    for (int i=0;i<g1.GetN(); i++)
        h1f.Fill(g1.GetY()[i]);
    ((TH1F*)h1f.DrawCopy())->Fit("gaus");

    c->cd(3);
    TGraph &g2 = hist2.GetGraph();
    TH1 *h2 = g2.GetHistogram();
    h2->SetXTitle("Time [au]");
    h2->SetYTitle("P_{rms} [au]");
    g2.DrawClone("A*");

    c->cd(4);
    TH1F h2f("PedestalRMS", "Pedestals RMS", 26, 0, 3.5);
    h2f.SetXTitle("P_{rms}");
    h2f.SetYTitle("Counts");
    for (int i=0;i<g2.GetN(); i++)
        h2f.Fill(g2.GetY()[i]);
    ((TH1F*)h2f.DrawCopy())->Fit("gaus");

    c->SaveAs(fname(0, fname.Last('.')+1) + "ps");
    //c->SaveAs(fname(0, fname.Last('.')+1) + "root");
}

// -------------------------------------------------------------------------
//
//  plot.C
//
//  This macro shows how to fill and display a histogram using Mars
//
void pedestalvstime(Int_t idx=0, const char *dirname=".")
{
    MDirIter Next;
    Next.AddDirectory(dirname, "raw*.root", -1);

    TString fname;
    while (1)
    {
        fname = Next();
        if (fname.IsNull())
            break;

        ProcessFile(fname, idx);
        return;
    }
}
