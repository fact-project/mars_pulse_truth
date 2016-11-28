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
!   Author(s): Harald Kornmayer, 1/2001
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCT1ReadAscii
//
// Reads a ascii file with CT1 data. The file description and some example
// files can be found on the Magic homepage.
//
//  Input Containers:
//   -/-
//
//  Output Containers:
//   MCerPhotEvt
//
/////////////////////////////////////////////////////////////////////////////
#include "MCT1ReadAscii.h"

#include <errno.h>
#include <fstream>

#include <TList.h>
#include <TSystem.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MCerPhotEvt.h"

#include "MPedPhotPix.h"
#include "MPedPhotCam.h"

ClassImp(MCT1ReadAscii);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Creates an array which stores the file names of
// the files which should be read. If a filename is given it is added
// to the list.
//
MCT1ReadAscii::MCT1ReadAscii(const char *fname,
			     const char *name, 
                             const char *title)
    : fIn(NULL)
{
    fName  = name  ? name  : "MCT1ReadAscii";
    fTitle = title ? title : "Task to loop over events in CT1 ascii file";

    //
    // remember file name for opening the file in the preprocessor
    //
    fFileNames = new TList;
    fFileNames->SetOwner();

    if (fname)
        AddFile(fname);
}

// --------------------------------------------------------------------------
//
// Delete the filename list and the input stream if one exists.
//
MCT1ReadAscii::~MCT1ReadAscii()
{
    delete fFileNames;
    if (fIn)
        delete fIn;
}

// --------------------------------------------------------------------------
//
// Add this file as the last entry in the chain
//
Int_t MCT1ReadAscii::AddFile(const char *txt, Int_t)
{
    TNamed *name = new TNamed(txt, "");
    fFileNames->AddLast(name);
    return 1;
}

// --------------------------------------------------------------------------
//
// This opens the next file in the list and deletes its name from the list.
//
Bool_t MCT1ReadAscii::OpenNextFile()
{
    //
    // open the input stream and check if it is really open (file exists?)
    //
    if (fIn)
        delete fIn;
    fIn = NULL;

    //
    // Check for the existance of a next file to read
    //
    TNamed *file = (TNamed*)fFileNames->First();
    if (!file)
        return kFALSE;

    //
    // open the file which is the first one in the chain
    //
    const char *name = file->GetName();

    const char *expname = gSystem->ExpandPathName(name);
    fIn = new ifstream(expname);

    const Bool_t noexist = !(*fIn);
    if (noexist)
    {
        *fLog << err << "Cannot open file " << expname << ": ";
        *fLog << strerror(errno) << endl;
    }
    else
        *fLog << inf << "Open file: '" << name << "'" << endl;

    delete [] expname;

    //
    // Remove this file from the list of pending files
    //
    fFileNames->Remove(file);

    return !noexist;
}

// --------------------------------------------------------------------------
//
// Open the first file in the list. Check for the output containers or create
// them if they don't exist.
//
// Initialize the size of the MPedPhotCam container to 127 pixels (CT1 camera)
//
Int_t MCT1ReadAscii::PreProcess(MParList *pList)
{
    //
    // Preprocessing
    //

    //
    // Try to open at least one (the first) file
    //
    if (!OpenNextFile())
        return kFALSE;

    //
    //  look for the MCerPhotEvt class in the plist
    //
    fNphot = (MCerPhotEvt*)pList->FindCreateObj("MCerPhotEvt");
    if (!fNphot)
        return kFALSE;

    //
    //  look for the pedestal class in the plist
    //
    fPedest = (MPedPhotCam*)pList->FindCreateObj("MPedPhotCam");
    if (!fPedest)
        return kFALSE;

    MGeomCam *geom = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!geom)
    {
        *fLog << err << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }

    fPedest->Init(*geom);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read apedestal entry (line) from the file
//
void MCT1ReadAscii::ReadPedestals()
{
    //
    // skip the next 4 values
    //
    Float_t val;

    *fIn >> val;
    *fIn >> val;
    *fIn >> val;
    *fIn >> val;

    //
    //    read in the next 127 numbers as the pedestals
    //
    for (Int_t i = 0; i<127; i++)
    {
        *fIn >> val;

        if (val>0)
            (*fPedest)[i].Set(0, val);
    }
}

// --------------------------------------------------------------------------
//
// Read a data entry (line) from the file
//
void MCT1ReadAscii::ReadData()
{
    //
    // five unsused numbers
    //
    Int_t val;

    *fIn >> val;   // ener
    *fIn >> val;   // zenang
    *fIn >> val;   // sec1
    *fIn >> val;   // sec2

    //
    // read in the number of cerenkov photons and add the 'new' pixel
    // too the list with it's id, number of photons and error
    //
    for (Int_t i = 0; i<127; i++ )
    {
        Float_t nphot;

        *fIn >> nphot;

        if (nphot > 0.0)
            fNphot->AddPixel(i, nphot, (*fPedest)[i].GetRms());
    }
    fNphot->FixSize();
}

// --------------------------------------------------------------------------
//
// Check for the event number and depending on this number decide if
// pedestals or event data has to be read.
//
// If the end of the file is reached try to open the next in the list. If
// there is now next file stop the eventloop.
//
Int_t MCT1ReadAscii::Process()
{
    //
    // FIXME. This function should switch between reading pedestals and
    // reading event data by the 'switch entry'.
    // After reading it should set the InputStreamID correctly.
    // (should use MPedPhotCam )
    //
 
    //
    // read in the event nr
    //
    Int_t evtnr;
    *fIn >> evtnr;

    //
    // check if we are done. Try to open the next file in chain.
    // If it was possible start reading. If not break the event loop
    //
    if (fIn->eof())
        return OpenNextFile() ? kCONTINUE : kFALSE;

    //
    // if the first number is negativ this is a pedestal line:
    // read in pedestals
    //
    // FIXME! Set InputStreamID

    if (evtnr < 0)
    {
        ReadPedestals();
        return kCONTINUE;
    }

    ReadData();

    return kTRUE;
}
