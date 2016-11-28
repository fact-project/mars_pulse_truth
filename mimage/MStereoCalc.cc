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
!   Author(s): Abelardo Moralejo, 11/2003 <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MStereoCalc
//
//  This is a task to calculate some shower parameters from the images of
//  two telescopes in stereo mode. 
//
//  Input Containers:
//   MGeomCam
//   MHillas
//   MPointingPos
//
//  Output Containers:
//   MStereoPar
//
/////////////////////////////////////////////////////////////////////////////
#include "MStereoCalc.h"

#include "MParList.h"

#include "MHillas.h"
#include "MStereoPar.h"
#include "MPointingPos.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MStereoCalc);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MStereoCalc::MStereoCalc(const char *name, const char *title)
    : fStereoParName("MStereoPar")
{
    fName  = name  ? name  : "MStereoCalc";
    fTitle = title ? title : "Calculate shower parameters in stereo mode";
}

// --------------------------------------------------------------------------
//
// Check for MMcEvt and MHillas containers.
// Try to find the Geometry conatiner.
// Try to find (and maybe create) the container MStereoPar.
//
Int_t MStereoCalc::PreProcess(MParList *pList)
{
    fPointingPos1 = (MPointingPos*)pList->FindObject(AddSerialNumber("MPointingPos",fCT1id));
    if (!fPointingPos1)
    {
        *fLog << err << AddSerialNumber("MPointingPos",fCT1id) << " not found... aborting." << endl;
        return kFALSE;
    }

    fPointingPos2 = (MPointingPos*)pList->FindObject(AddSerialNumber("MPointingPos",fCT2id));
    if (!fPointingPos2)
    {
        *fLog << err << AddSerialNumber("MPointingPos",fCT2id) << " not found... aborting." << endl;
        return kFALSE;
    }

    fGeomCam1 = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam", fCT1id));
    if (!fGeomCam1)
    {
        *fLog << err << AddSerialNumber("MGeomCam", fCT1id) << " not found... aborting." << endl;
        return kFALSE;
    }

    fGeomCam2 = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam", fCT2id));
    if (!fGeomCam2)
    {
        *fLog << err << AddSerialNumber("MGeomCam", fCT2id) << " not found... aborting." << endl;
        return kFALSE;
    }

    fHillas1 = (MHillas*)pList->FindObject(AddSerialNumber("MHillas", fCT1id));
    if (!fHillas1)
    {
        *fLog << err << AddSerialNumber("MHillas", fCT1id) << " not found... aborting." << endl;
        return kFALSE;
    }

    fHillas2 = (MHillas*)pList->FindObject(AddSerialNumber("MHillas", fCT2id));
    if (!fHillas2)
    {
        *fLog << err << AddSerialNumber("MHillas", fCT2id) << " not found... aborting." << endl;
        return kFALSE;
    }

    fStereoPar = (MStereoPar*)pList->FindCreateObj("MStereoPar", fStereoParName);
    if (!fStereoPar)
	return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Call the Calc procedure of the MStereoPar object, where the 
// calculations combining the data from the two telescopes are performed.
//
Int_t MStereoCalc::Process()
{
    fStereoPar->Calc(*fHillas1, *fPointingPos1, *fGeomCam1, fCT1x, fCT1y,
                     *fHillas2, *fPointingPos2, *fGeomCam2, fCT2x, fCT2y);

    return kTRUE;
}
