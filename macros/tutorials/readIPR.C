///////////////////////////////////////////////////////////////////////////
// 
//    readIPR.C
//
//   This macro shows how to read the Individual Pixel Rates from 
//   a CC report file.
//
//   Input: 
//     - root file obtained merpping a .rep CC file
//       container: MTriggerIPR
//
//   Output:
//     - a camera display showing the IPRs
//     - Some histos for checking purposes
//   
//   Note: 
//     a similar macro can be used to read the following trigger containers:
//     - MTriggerIPR        (Individual Pixel Rates)
//     - MTriggerCell       (Rate of trigger cells)
//     - MTriggerBit        (Output Bits from prescaler (before/after presc.)
//     - MTriggerPrescFact  (Prescaling factors for each bit)
//     - MTriggerLiveTime   (Values of counters for dead/livetime)
//
//    Author(s): Antonio Stamerra. 09/04 <antonio.stamerra@pi.infn.it>
//
////////////////////////////////////////////////////////////////////////////
void readIPR(TString fname)
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

    // First Task: Read ROOT file with Trigger-REPORT data
    MReadTree read("Trigger", fname);
    read.DisableAutoScheme();

    tlist.AddToList(&read);

    // Create the container for the IPRs
    MTriggerIPR ipr;
    plist.AddToList(&ipr);

    // Create the histo to display the IPRs
    MHCamEvent IPRhist("IPRhist","IPRs");
    plist.AddToList(&IPRhist);
        
    // create a task to fill a histogram from the container
    MFillH fillIPR(&IPRhist, "MTriggerIPR");
    tlist.AddToList(&fillIPR);

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    if (!evtloop.Eventloop())
        return;

    //-----------------------------------
    // Now display the result of the loop
    //

    // create a MHCamera histo where the sum off all events is filled
    MHCamera &h = *(MHCamera*)IPRhist.GetHistByName("sum");

    TCanvas *c = MH::MakeDefCanvas();
    c->Divide(2, 2);

    MHCamera *disp1=h.Clone();
    MHCamera *disp2=h.Clone();
    //MHCamera *disp3=h.Clone();
    disp2->SetCamContent(h, 1);
    //disp3->SetCamContent(h, 2);

    disp1->SetYTitle("Rate [Hz]");
    disp2->SetYTitle("\\sigma_{Rate} [Hz]");
    //disp3->SetYTitle("\\sigma_{Rate} [%]");
    disp1->SetName("IPRs;avg");
    disp2->SetName("IPRs;err");
    //disp3->SetName("IPRs;rel");
    disp1->SetTitle("IPRs Average");
    disp2->SetTitle("IPRs error");
    //disp3->SetTitle("IPRs relative error");

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
    //c->cd(3);
    //gPad->SetBorderMode(0);
    //gPad->Divide(1,1);
    //gPad->cd(1);
    //gPad->SetLogy();
    //disp3->Draw();
    //disp3->SetBit(kCanDelete);
    c->cd(3);
    gPad->SetBorderMode(0);
    disp1->Draw("EPhist");
    c->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetLogy();
    disp2->Draw("Phist");
    //c->cd(6);
    //gPad->SetBorderMode(0);
    //gPad->SetLogy();
    //disp3->Draw("Phist");

    c->SaveAs(fname(0, fname.Last('.')+1) + "ps");
    //c->SaveAs(fname(0, fname.Last('.')+1) + "root");
}

