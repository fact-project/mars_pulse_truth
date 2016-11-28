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
!   Author(s): Wolfgang Wittek  04/2003 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MFCT1SelFinal
//
//  WHAT ARE THE FINAL CUTS?
//
//  This is a class to evaluate the Final Cuts
//  (these cuts define the final sample of gammas;
//   relevant for the calculation of the effective collection areas)
//
//  to be called after the calculation of the hadroness
//
/////////////////////////////////////////////////////////////////////////////
#include "MFCT1SelFinal.h"

#include <math.h>

#include "MParList.h"

#include "MMcEvt.hxx"

#include "MCerPhotEvt.h"

#include "MGeomPix.h"
#include "MGeomCam.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MHillasExt.h"
#include "MHillasSrc.h"
#include "MHadronness.h"

ClassImp(MFCT1SelFinal);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MFCT1SelFinal::MFCT1SelFinal(const char *hilsrcname,
                             const char *name, const char *title)
    : fHilSrcName(hilsrcname), fHadronnessName("MHadronness")
{
    fName  = name  ? name  : "MFCT1SelFinal";
    fTitle = title ? title : "Class to evaluate the Final Cuts";

    // default values of cuts
    SetCuts(1.0, 100.0, 10.);
}

// --------------------------------------------------------------------------
//
// Set cut values
// 
//
void MFCT1SelFinal::SetCuts(Float_t hadmax, Float_t alphamax, Float_t distmax) 
{ 
    fHadronnessMax =   hadmax;
    fAlphaMax      = alphamax;
    fDistMax       =  distmax;

    *fLog << inf << "MFCT1SelFinal cut values : fHadronnessMax, fAlphaMax, fDistMax = ";
    *fLog << fHadronnessMax << ",  " << fAlphaMax << ",  " << fDistMax <<  endl;
}

// --------------------------------------------------------------------------
//
// Set the pointers
//
Int_t MFCT1SelFinal::PreProcess(MParList *pList)
{
    fHilSrc = (MHillasSrc*)pList->FindObject(fHilSrcName, "MHillasSrc");
    if (!fHilSrc)
    {
      *fLog << dbginf << "MHillasSrc object " << fHilSrcName << " not found... aborting." << endl;
      return kFALSE;
    }

    fHadronness = (MHadronness*)pList->FindObject(fHadronnessName, "MHadronness");
    if (!fHadronness)
    {
      *fLog << dbginf << "MHadronness not found... aborting." << endl;
      return kFALSE;
    }

    MGeomCam *cam = (MGeomCam*)pList->FindCreateObj("MGeomCamCT1","MGeomCam");
    if (!cam)
    {
        *fLog << err << "MGeomCam (Camera Geometry) not found... aborting." << endl;
        return kFALSE;
    }

    fMm2Deg = cam->GetConvMm2Deg();

    memset(fCut, 0, sizeof(fCut));

    return kTRUE;
}

Int_t MFCT1SelFinal::Set(Int_t rc)
{
    fCut[rc]++;
    fResult=kTRUE;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Evaluate final cuts
// 
// if cuts are fulfilled set fResult = kTRUE
//
Int_t MFCT1SelFinal::Process()
{
    const Double_t modalpha = fabs( fHilSrc->GetAlpha() );
    const Double_t h = fHadronness->GetHadronness();

    fResult = kFALSE;

    if (h>fHadronnessMax)
        return Set(1);

    if (modalpha>fAlphaMax)
        return Set(2);

    if (fMm2Deg*fHilSrc->GetDist()>fDistMax)
        return Set(3);

    fCut[0]++;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Prints some statistics about the Final selections.
//
Int_t MFCT1SelFinal::PostProcess()
{
    if (GetNumExecutions()==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << dec << setfill(' ');
    *fLog << " " << setw(7) << fCut[1] << " (" << setw(3) ;
    *fLog << (int)(fCut[1]*100/GetNumExecutions());
    *fLog << "%) Evts skipped due to: hadronness > "<< fHadronnessMax;
    *fLog << " (hadronness from '" << fHadronnessName << "')" << endl;

    *fLog << " " << setw(7) << fCut[2] << " (" << setw(3) ;
    *fLog << (int)(fCut[2]*100/GetNumExecutions());
    *fLog << "%) Evts skipped due to: |ALPHA| > " << fAlphaMax;
    *fLog << " [degrees]" << endl;

    *fLog << " " << setw(7) << fCut[3] << " (" << setw(3) ;
    *fLog << (int)(fCut[3]*100/GetNumExecutions());
    *fLog << "%) Evts skipped due to: DIST > " << fDistMax;
    *fLog << " [degrees]" << endl;

    *fLog << " " << fCut[0] << " (" ;
    *fLog << (int)(fCut[0]*100/GetNumExecutions());
    *fLog << "%) Evts survived Final selections!" << endl;
    *fLog << endl;

    return kTRUE;
}
