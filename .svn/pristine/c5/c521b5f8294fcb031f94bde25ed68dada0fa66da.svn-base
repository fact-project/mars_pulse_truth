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


#include <iomanip>

void readpix()
{
    //
    // open the file
    //
    TFile input("delme.root", "READ");

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
    MRawEvtData *evtdata = new MRawEvtData();

    //
    // enable the corresponding branches
    //
    evttree->GetBranch("MRawEvtData")->SetAddress(&evtdata);

    evttree->GetEvent(0);

    MRawEvtPixelIter pixel(evtdata);

    while (pixel.Next())
    {
       
       cout << "Pixel Nr: " << dec << pixel.GetPixelId() << " ";
       cout << setfill('0') << endl;
      
       cout << "Hi Gains: ";
       for (int i=0; i<evtdata->GetNumHiGainSamples(); i++)
	  cout << setw(3) << (UInt_t)pixel.GetHiGainFadcSamples()[i];
       cout << endl;
      
       cout << "Lo Gains: ";
       if (pixel.IsLoGain())
	  for (int i=0; i<evtdata->GetNumLoGainSamples(); i++)
	     cout << setw(3) << (UInt_t)pixel.GetLoGainFadcSamples()[i];
       cout << endl << endl;

       if (pixel.GetPixelId() == 11)
	 pixel.Draw();
    };
}




