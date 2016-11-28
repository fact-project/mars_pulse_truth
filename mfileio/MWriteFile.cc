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
!   Author(s): Thomas Bretz, 6/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MWriteFile
//
// This is a base class for writing tasks. If you want to implement
// writing out parameter containers in a new file format, this is a good
// starting point.
// The class defines a generalized interface between writing out data in
// an eventloop and your file format.
//
/////////////////////////////////////////////////////////////////////////////
#include "MWriteFile.h"

#include <fstream>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

ClassImp(MWriteFile);

using namespace std;

// --------------------------------------------------------------------------
//
// Tries to open the given output file.
// The derived class should retrieve needed containers from the parameter
// list.
// instance of the container which should be written.
// If the container has already the HasChanged flag it is immediatly written
// to the output file.
//
Int_t MWriteFile::PreProcess(MParList *pList)
{
    //
    // test whether file is now open or not
    //
    if (!IsFileOpen())
    {
        *fLog << err << dbginf << "Cannot open file '" << GetFileName() << "'" << endl;
        return kFALSE;
    }

    *fLog << inf << "File '" << GetFileName() << "' open for writing." << endl;

    //
    // Get the containers (pointers) from the parameter list you want to write
    //
    return GetContainer(pList);
    /*
    if (!GetContainer(pList))
        return kFALSE;

    //
    // write the container if it is already in changed state
    //
    return CheckAndWrite();
    */
}

// --------------------------------------------------------------------------
//
// Checks if the SetReadyToSave flag of the output container is set. If it
// is set the container should be written to the output.
//
Bool_t MWriteFile::ReInit(MParList *pList)
{
    return CheckAndWrite();
}

// --------------------------------------------------------------------------
//
// Checks if the SetReadyToSave flag of the output container is set. If it is
// set the container should be written to the output.
//
Int_t MWriteFile::Process()
{
    return CheckAndWrite();
}

// --------------------------------------------------------------------------
//
// Checks if the SetReadyToSave flag of the output container is set. If it is
// set the container should be written to the output.
//
Int_t MWriteFile::PostProcess()
{
    //
    // check if the container changed state is set
    //
    return CheckAndWrite();
}
