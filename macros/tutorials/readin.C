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
!   Author(s): Thomas Bretz  12/2000 (tbretz@uni-sw.gwdg.de)
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */


void readin()
{
    //
    // open the file
    //
    TFile input("test.root", "READ");

    //
    // open the Run Header tree
    //
    TTree *runtree = (TTree*) input.Get("RunHeaders") ;

    if (!runtree)
        return;

    cout << " Entries in Tree RunHeaders: " << runtree->GetEntries() << endl ;

    //
    // create an instance of MRawRunHeader, enable the branch and
    // read the header, print it
    //
    MRawRunHeader *runheader = new MRawRunHeader();
    runtree->GetBranch("MRawRunHeader")->SetAddress(&runheader);
    runtree->GetEvent(0);
    runheader->Print();

    //
    // open the Data Tree
    //
    TTree *evttree = (TTree*) input.Get("Data") ;

    //
    // create the instances of the data to read in
    //
    MRawEvtHeader  *evtheader = new MRawEvtHeader();
    MTime          *evttime   = new MTime();
    MRawEvtData    *evtdata   = new MRawEvtData();
    MRawCrateArray *evtcrate  = new MRawCrateArray();

    //
    // enable the corresponding branches
    //
    evttree->GetBranch("MRawEvtHeader")->SetAddress(&evtheader);
    evttree->GetBranch("MTime")->SetAddress(&evttime);
    evttree->GetBranch("MRawEvtData")->SetAddress(&evtdata);
    evttree->GetBranch("MRawCrateArray")->SetAddress(&evtcrate);

    //
    // loop over all events and print the information
    //
    Int_t iEnt = evttree->GetEntries();
    cout << " Entries in Tree Data: " << iEnt << endl;

    for (Int_t i = 0; i<iEnt; i++)
    {
      // readin event with the selected branches
      evttree->GetEvent(i);

      evtheader->Print();
      evttime->Print();
      evtcrate->Print();
      evtdata->Print();
    } 
}


