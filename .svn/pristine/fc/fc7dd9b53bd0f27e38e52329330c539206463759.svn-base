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
!   Copyright: MAGIC Software Development, 2000-2001
!
\* ======================================================================== */

/*
 !   Modified 4/7/2002, Abelardo Moralejo:
 !   Added one optional input parameter: a camera .root file containing
 !   pure NSB events. One such file is generated running the camera over an
 !   "empty" reflector file, with the NSB option on, and asking the camera
 !   program (see camera program manual) to do the simulation even if no
 !   photoelectron from the shower arrives at the camera. One also needs to
 !   write to the output file all the events, both triggered and untriggered
 !   (see again camera manual). These nsb camera files must contain the same
 !   trigger conditions as the proton file.
 !
 !   If no nsb file is supplied, the macro will assume no triggers from
 !   pure NSB fluctuations.
 */

Float_t GetNSBEvents(TString name, Float_t *BgR, int dim)
{
    Int_t numnsbevents;

    TFile bgfile(name);
    TTree *events = (TTree*) bgfile.Get("Events");

    TH1F h("h","",5,.5,5.5);

    UInt_t from = dim>0 ?   1 : -dim;
    UInt_t to   = dim>0 ? dim : -dim;

    cout << endl << "Calculating NSB triggers from file " << name << "..." << endl << endl;

    for (UInt_t i = from; i <= to; i++)
    {
      TString cond("MMcTrig;");
      cond += i;
      cond += ".";
      TBranch *b = events->GetBranch(cond);
      MMcTrig *mctrig;
      b->SetAddress(&mctrig);

      Int_t tottrig = 0.;
      
      UInt_t imax = b->GetEntries();

      for (UInt_t iev = 0; iev < imax; iev++)
	{	
	  b->GetEvent(iev);
	  tottrig += mctrig->GetFirstLevel();
	}
      // Set total number of L1 triggers:
      BgR[dim>0? i-1: 0] = (Float_t) tottrig;

      numnsbevents = (Float_t) imax;
    }

    return numnsbevents;
}


void trigrate(int dim=0, char *filename = "data/camera.root", 
	      char *nsbfile = NULL)
{
    // The dim parameter has three possible values:
    // dim : = 0 -> root file with 1 trigger condition.
    //       > 0 -> number of trigger condition to be analised 
    //              in multi conditon file.
    //       < 0 -> selects the -dim trigger condition.
    //
    // first we have to create our empty lists
    //
    MParList  parlist;
    MTaskList tasklist;

    //
    // Setup the parameter list.
    //  - we do not need to create any other container. All of them
    //    are created automatically without loss - we don't have to
    //    access them-
    //
    //  - we need to create MHMcRate only. The other containers
    //    are created automatically without loss - we don't have to
    //    access them-
    //  - MHMcRate must be created by us because we need the pointer
    //    to it and if it would get created automatically it would also be
    //    deleted automatically
    //  - Actually, depending on using a single trigger option MonteCarlo
    //    file or a multyple trigger option, a MHMcRate or an array of 
    //    MHMcRate are needed.
    //
    parlist.AddToList(&tasklist);

    //
    // You don't have to add the MHMcRate container here by hand.
    // But if you want to print or display these containers later on
    // it is necessary (Rem: No printing or displaying is done in this
    // macro yet)
    //
    UInt_t from = dim>0 ?   1 : -dim;
    UInt_t to   = dim>0 ? dim : -dim;

    Int_t num = to-from+1;

    TObjArray hists(MParList::CreateObjList("MHMcRate", from, to));
    hists.SetOwner();

    //
    // Check if the list really contains the right number of histograms
    //
    if (hists.GetEntriesFast() != num)
        return;

    // Set for each MHMcRate object the trigger condition number in the 
    // camera file (for the case of camera files with several conditions,
    // produced with the trigger_loop option.
    //

    if (dim < 0)
      ((MHMcRate*)(hists[0]))->SetTriggerCondNum((Short_t)(-dim));
    if (dim > 0)
      for (Short_t i = from ; i <= to; i++)
	((MHMcRate*)(hists[i-1]))->SetTriggerCondNum(i);

    //
    // Add the histograms to the paramater list.
    //
    parlist.AddToList(&hists);

    //
    // Setup out tasks:
    //  - First we have to read the events
    //  - Then we can calculate rates, for what the number of
    //    triggered showers from a empty reflector file for the
    //    analised trigger conditions should be set (BgR[])
    //

    MReadMarsFile reader("Events", filename);
    tasklist.AddToList(&reader);

    // Now we have to build the BgR array, containing the number 
    // of triggers (may be more than 1 trigger/event!) from the
    // numnsbevents simulated in the nsbfile (3rd input parameter).
    // If no nsbfile is supplied, we assume no triggers from NSB

    Float_t* BgR = new Float_t[num];
    memset(BgR, 0, num*sizeof(Float_t));

    Float_t numnsbevents = 5.e4; // some default value.
    if (nsbfile)
      numnsbevents = GetNSBEvents(nsbfile, BgR, dim);

    cout << "Number of Trigger conditions: " << num << endl;

    MMcTriggerRateCalc rate(dim, BgR, numnsbevents);

    tasklist.AddToList(&rate);

    //
    // set up the loop for the processing
    //
    MEvtLoop magic;
    magic.SetParList(&parlist);

    //
    // Start to loop over all events
    //
    MProgressBar bar;
    magic.SetProgressBar(&bar);
    if (!magic.Eventloop())
        return;

    delete BgR;

    hists.Print();

    if (num > 1)
      {
	gStyle->SetOptStat(0);
	rate->Draw();

	TFile f("ratehists.root", "recreate");
	
	rate->GetHist(2)->Write();
	rate->GetHist(3)->Write();
	rate->GetHist(4)->Write();
      }


}
