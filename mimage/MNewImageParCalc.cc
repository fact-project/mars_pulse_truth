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
!   Author(s): Wolfgang Wittek     03/2003 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MNewImageParCalc
//
// Task to calculate the source dependant part of the hillas parameters
//
//////////////////////////////////////////////////////////////////////////////
#include "MNewImageParCalc.h"

#include <fstream.h>

#include "MParList.h"

#include "MGeomCam.h"
#include "MSrcPosCam.h"
#include "MCerPhotEvt.h"
#include "MNewImagePar.h"
#include "MNewImagePar.h"
#include "MLog.h"
#include "MLogManip.h"

ClassImp(MNewImageParCalc);

static const TString gsDefName  = "MNewImageParCalc";
static const TString gsDefTitle = "Calculate new image parameters";

// -------------------------------------------------------------------------
//
// Default constructor. The first argument is the name of a container
// containing the source position in the camera plain (MScrPosCam).
// The default is "MSrcPosCam". newpar is the name of a container
// of type MNewImagePar, in which the parameters are stored.
// The default is "MNewImagePar"
//
//
MNewImageParCalc::MNewImageParCalc(const char *src, const char *newpar,
                                   const char *name, const char *title)
    : fHillas(NULL), fSrcPos(NULL), fNewImagePar(NULL)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    fSrcName     = src;
    fNewParName  = newpar;
    fHillasInput = "MHillas";
}

// -------------------------------------------------------------------------
//
Int_t MNewImageParCalc::PreProcess(MParList *pList)
{
    fHillas = (MHillas*)pList->FindObject(fHillasInput, "MHillas");
    if (!fHillas)
    {
        *fLog << err << dbginf << "MHillas not found... aborting." << endl;
        return kFALSE;
    }

    fSrcPos = (MSrcPosCam*)pList->FindObject(fSrcName, "MSrcPosCam");
    if (!fSrcPos)
    {
        *fLog << err << dbginf << fSrcName << " [MSrcPosCam] not found... aborting." << endl;
        return kFALSE;
    }

    fCerPhotEvt = (MCerPhotEvt*)pList->FindObject("MCerPhotEvt");
    if (!fCerPhotEvt)
    {
        *fLog << dbginf << "MCerPhotEvt not found... aborting." << endl;
        return kFALSE;
    }

    fGeomCam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fGeomCam)
    {
        *fLog << dbginf << "MGeomCam (Camera Geometry) missing in Parameter List... aborting." << endl;
        return kFALSE;
    }

    fNewImagePar = (MNewImagePar*)pList->FindCreateObj("MNewImagePar", fNewParName);
    if (!fNewImagePar)
        return kFALSE;

    //fErrors = 0;

    return kTRUE;
}

// -------------------------------------------------------------------------
//
Int_t MNewImageParCalc::Process()
{

    /*if (!*/fNewImagePar->Calc(*fGeomCam, *fCerPhotEvt, *fHillas);/*)
    {
        fErrors++;
        return kCONTINUE;

    }*/
    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Prints some statistics about the hillas calculation. The percentage
//  is calculated with respect to the number of executions of this task.
//
/*
Bool_t MNewImageParCalc::PostProcess()
{
    if (GetNumExecutions()==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << dec << setfill(' ');
    *fLog << " " << fErrors << " (" << (int)(fErrors*100/GetNumExecutions()) << "%) Evts skipped due to: calculation failed" << endl;
    *fLog << endl;

    return kTRUE;
}
*/
