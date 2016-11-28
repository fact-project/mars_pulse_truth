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

    MHCamEvent evt0(0, "IPR", "Individual Pixel Rate;;IPR [Hz]");
    //    evt0b.SetThreshold(0);

    MFillH fill0(&evt0, "MTriggerIPR", "FillIPR");

    tlist.AddToList(&fill0);

    MStatusDisplay *d = new MStatusDisplay;

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(d);

    if (!evtloop.Eventloop())
      return;

    tlist.PrintStatistics();
}

