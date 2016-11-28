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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

////////////////////////////////////////////////////////////////////////
//
//  MRawFileWrite
//
//  Here we write the root containers which contains the data from a
//  root binary file to a root file. See also MRawFileRead
//
//  Input Containers:
//   MRawRunHeader
//   MRawEvtHeader
//   MRawEvtData
//   MRawEvtData2 [MRawEvtData]
//   MRawCrateArray
//   MTime
//
//  Output Containers:
//   -/-
//
////////////////////////////////////////////////////////////////////////

#include "MRawFileWrite.h"

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MRawRunHeader.h"
#include "MRawEvtHeader.h"
#include "MRawEvtData.h"
#include "MRawCrateArray.h"

ClassImp(MRawFileWrite);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. It opens the output file (root-file)
//
MRawFileWrite::MRawFileWrite(const char *fname,  const Option_t *opt,
                             const char *ftitle, const Int_t comp,
                             const char *name, const char *title)
    : fTData(0), fSplit(kFALSE)
{
    fName  = name  ? name  : "MRawFileWrite";
    fTitle = title ? title : "Write task to write DAQ root files";

    //
    // Open a rootfile
    //
    TString str(fname);
    if (!str.EndsWith(".root", TString::kIgnoreCase))
        str += ".root";

    fOut = new TFile(str, opt, ftitle, comp);
}

MRawFileWrite::~MRawFileWrite()
{
    if (fTData)
    {
        if (fOut != fTData->GetCurrentFile())
        {
            *fLog << warn << endl;
            *fLog << "WARNING - MWriteRootFile:   Root's  TTree/TFile   has  opened   a  new  file" << endl;
            *fLog << "  automatically.  You can change this behaviour using TTree::SetMaxTreeSize." << endl;
            *fLog << "  You won't be able to read splitted  files  correctly with MReadMarsFile if" << endl;
            *fLog << "  they have more than one entry in 'RunHeaders' or you try to read more than" << endl;
            *fLog << "  one of such sequences at once." << endl;
            *fLog << endl;
        }

        //
        // For more information see TTree:ChangeFile()
        //
        fOut = fTData->GetCurrentFile();
    }

    //
    // delete instance, this also does a fOut->Close()
    //
    if (fOut->IsOpen())
        fOut->Write();

    delete fOut;

    //
    // Remark:
    // - Trees are automatically deleted by the the file
    //   (unless file.SetDirectory(0) was called)
    // - Branches are automatically deleted by the tree destructor
    //
}


// --------------------------------------------------------------------------
//
// The PreProcess function checks for the following input containers:
//  - MRawEvtHeader
//  - MRawEvtData
//  - MRawCrateArray
//  - MTime
//  - MRawRunHeader
// if a container isn't found the eventloop is stopped.
//
// The tree which should containe the run header is created. <RunHeaders>
// The trees which contains the Events <Events>, <PedEvents>, <CalEvents>
// are created.
//
Int_t MRawFileWrite::PreProcess (MParList *pList)
{
    //
    // test whether file is now open or not
    //
    if (!fOut->IsOpen())
    {
        *fLog << dbginf << "Error: Cannot open file '" << fOut->GetName() << "'" << endl;
        return kFALSE;
    }

    //
    //  check if MEvtHeader exists in the Parameter list already.
    //  if not create one and add them to the list
    //
    fRawEvtHeader = (MRawEvtHeader*)pList->FindObject("MRawEvtHeader");
    if (!fRawEvtHeader)
    {
        *fLog << err << "MRawEvtHeader not found... aborting." << endl;
        return kFALSE;
    }

    fRawEvtData1 = (MRawEvtData*)pList->FindObject("MRawEvtData");
    if (!fRawEvtData1)
    {
        *fLog << err << "MRawEvtData not found... aborting." << endl;
        return kFALSE;
    }
    fRawEvtData2 = (MRawEvtData*)pList->FindObject("MRawEvtData2", "MRawEvtData");
    if (!fRawEvtData2)
    {
        *fLog << err << "MRawEvtData2 [MRawEvtData] not found... aborting." << endl;
        return kFALSE;
    }

    fRawCrateArray = (MRawCrateArray*)pList->FindObject("MRawCrateArray");
    if (!fRawCrateArray)
    {
        *fLog << err << "MRawCrateArray not found... aborting." << endl;
        return kFALSE;
    }

    fTime = (MTime*)pList->FindObject("MTime");
    if (!fTime)
    {
        *fLog << err << "MTime not found... aborting." << endl;
        return kFALSE;
    }

    fRawRunHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRawRunHeader)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    //
    // Remark:
    // - Trees are automatically deleted by the the file
    //   (unless file.SetDirectory(0) was called)
    // - Branches are automatically deleted by the tree destructor
    //
    fTRunHeader = new TTree("RunHeaders", "Run headers of all runs in this file");
    fTRunHeader->Branch("MRawRunHeader.", "MRawRunHeader", &fRawRunHeader, 32000);

    //
    // create data trees for the three types of data
    //
    fTData        =          new TTree("Events",      "Normal Triggered Events");
    fTPedestal    = fSplit ? new TTree("Pedestals",   "Pedestal Triggered Events")    : fTData;
    fTCalibration = fSplit ? new TTree("Calibration", "Calibration Triggered Events") : fTData;

    //
    // From the root dicumentation:
    //
    // Note that calling TTree::AutoSave too frequently (or similarly calling
    // TTree::SetAutoSave with a small value) is an expensive operation.
    // You should make tests for your own application to find a compromize
    // between speed and the quantity of information you may loose in case of
    // a job crash.
    //
    // In case your program crashes before closing the file holding this tree,
    // the file will be automatically recovered when you will connect the file
    // in UPDATE mode.
    // The Tree will be recovered at the status corresponding to the last AutoSave.
    //
    fTData       ->SetAutoSave(2000000000); // 2GB
    fTPedestal   ->SetAutoSave(2000000000); // 2GB
    fTCalibration->SetAutoSave(2000000000); // 2GB

    //
    // create all branches which are necessary
    //
    // FIXME: Can we calculate a good buffer size out of the event size?
    //        using splitlevel=0 sppeds up writing by a factor of 5-10%
    fTData->Branch("MTime.",          "MTime",          &fTime,          32000);
    fTData->Branch("MRawEvtHeader.",  "MRawEvtHeader",  &fRawEvtHeader,  32000);
    fTData->Branch("MRawEvtData.",    "MRawEvtData",    &fRawEvtData1,   320000);
    fTData->Branch("MRawEvtData2.",   "MRawEvtData",    &fRawEvtData2,   320000);
    fTData->Branch("MRawCrateArray.", "MRawCrateArray", &fRawCrateArray, 32000);
    if (!fSplit)
        return kTRUE;

    fTPedestal   ->Branch("MTime.",          "MTime",          &fTime,          32000);
    fTCalibration->Branch("MTime.",          "MTime",          &fTime,          32000);
    fTPedestal   ->Branch("MRawEvtHeader.",  "MRawEvtHeader",  &fRawEvtHeader,  32000);
    fTCalibration->Branch("MRawEvtHeader.",  "MRawEvtHeader",  &fRawEvtHeader,  32000);
    fTPedestal   ->Branch("MRawEvtData.",    "MRawEvtData",    &fRawEvtData1,   320000);
    fTCalibration->Branch("MRawEvtData.",    "MRawEvtData",    &fRawEvtData1,   320000);
    fTPedestal   ->Branch("MRawEvtData2.",   "MRawEvtData",    &fRawEvtData2,   320000);
    fTCalibration->Branch("MRawEvtData2.",   "MRawEvtData",    &fRawEvtData2,   320000);
    fTPedestal   ->Branch("MRawCrateArray.", "MRawCrateArray", &fRawCrateArray, 32000);
    fTCalibration->Branch("MRawCrateArray.", "MRawCrateArray", &fRawCrateArray, 32000);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Write the run header information to the file
//
Bool_t MRawFileWrite::ReInit(MParList *pList)
{
    fTRunHeader->Fill();
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Gets the trigger type from the run header to decide into which tree the
// event should be filled in and fills  it into this tree.
//
Int_t MRawFileWrite::Process()
{
    //
    // get the trigger type of the actual event
    //
    const UShort_t type = fRawEvtHeader->GetTrigType();

    //
    // writa data to the tree. the tree is choosen by the type of the event
    //
    switch (type)
    {
    case MRawEvtHeader::kTTEvent:
        fTData->Fill();
        return kTRUE;

    case MRawEvtHeader::kTTPedestal:
        fTPedestal->Fill();
        return kTRUE;

    case MRawEvtHeader::kTTCalibration:
        fTCalibration->Fill();
        return kTRUE;
    }

    *fLog << warn << dbginf << "Got wrong number for the trigger type: " << type;
    *fLog << " - skipped" << endl;

    return kCONTINUE;
}

