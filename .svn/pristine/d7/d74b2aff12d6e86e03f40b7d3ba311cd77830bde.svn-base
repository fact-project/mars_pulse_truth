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
!   Author(s): Marcos Lopez, 10/2003 <mailto:marcos@gae.ucm.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// weights.C
// =========
//
// This macro shows how to use the class MMcWeightEnergySpecCalc
// to convert the energy spectrum of the MC showers generated with Corsika,
// to a different one.
//
//////////////////////////////////////////////////////////////////////////////

Double_t myfunction(Double_t *x, Double_t *par)
{
  Double_t xx = x[0];

  return pow(xx,-2)*exp(-xx/20);  
}

void weights(TString filename="/up1/data/Magic-MC/CameraAll/Gammas/zbin0/Gamma_zbin0_0_7_1000to1009_w0-4:4:2.root")
{

    //
    // PartList
    //
    MParList  parlist;
    MTaskList tasklist;
    
    MHMcEnergyImpact h1("h1");
    MHMcEnergyImpact h2("h2");
    parlist.AddToList(&h1);
    parlist.AddToList(&h2);

    MBinning binsenergy("BinningEnergy");
    binsenergy.SetEdgesLog(100, 1, 1e5);
    parlist.AddToList(&binsenergy);

    MBinning binsimpact("BinningImpact");
    binsimpact.SetEdges(100, 0, 450);
    parlist.AddToList(&binsimpact);

    parlist.AddToList(&tasklist);


    //
    // TaskList
    //
    MReadMarsFile reader("Events", filename);
    reader.EnableBranch("fEnergy");
    reader.EnableBranch("fImpact");


    // -------------------------------------------------------------
    //
    // Option 1. Just change the slope of the MC power law spectrum
    //
    //MMcWeightEnergySpecCalc wcalc(-2.0);                //<-- Uncomment me

    //
    // Option 2. A completely differente specturm pass as a TF1 function
    //           e.g. spectrum with exponential cutoff
    //
    //TF1 spec("spectrum","pow(x,[0])*exp(-x/[1])");      //<-- Uncomment me
    //spec->SetParameter(0,-2.0);                         //<-- Uncomment me
    //spec->SetParameter(1,50);                           //<-- Uncomment me
    //MMcWeightEnergySpecCalc wcalc(spec);                //<-- Uncomment me
 
    //
    // Option 3. A completely differente specturm pass as a cahr*
    //           
    //char* func = "pow(x,-2)";                           //<-- Uncomment me
    //MMcWeightEnergySpecCalc wcalc(func);                //<-- Uncomment me

    //
    // Option 4. A completely differente specturm pass as a c++ function
    //     
    MMcWeightEnergySpecCalc wcalc((void*)myfunction);   //<-- Uncomment me
    //
    //-------------------------------------------------------------

    MFillH hfill(&h1,"MMcEvt");
    MFillH hfill2(&h2,"MMcEvt");
    hfill2.SetWeight("MWeight");

    tasklist.AddToList(&reader);
    tasklist.AddToList(&wcalc);
    tasklist.AddToList(&hfill);
    tasklist.AddToList(&hfill2);

    //
    // EventLoop
    //
    MEvtLoop magic;
    magic.SetParList(&parlist);

    if (!magic.Eventloop())
        return;

    tasklist.PrintStatistics();
    parlist.Print();

    //
    // Draw the Results
    //
    TCanvas *c = new TCanvas();
    c->SetLogy();
    c->SetLogx();

    TH1D* hist1 = (h1->GetHist())->ProjectionX();
    TH1D* hist2 = (h2->GetHist())->ProjectionX();
    hist2->SetLineColor(2);

    hist1->DrawClone();
    hist2->DrawClone("same");    
}
