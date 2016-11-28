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
!   Author(s): Keiichi Mase 09/2004 <mailto:mase@mppmu.mpg.de>
!   Author(s): Markus Meyer 09/2004 <mailto:meyer@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MMuonSearchParCalc
//
// Task to calculate the muon parameters
//
// This class search for muons and store the information into the container
// of MMuonSearchPar. Please see the manual for more information.
//
//
//  Input Containers:
//   MGeomCam
//   MHillas
//   MSignalCam
//
//  Output Containers:
//   MMuonSearchPar
//
//////////////////////////////////////////////////////////////////////////////
#include "MMuonSearchParCalc.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MSignalCam.h"
#include "MMuonSearchPar.h"

using namespace std;

ClassImp(MMuonSearchParCalc);

static const TString gsDefName  = "MMuonSearchParCalc";
static const TString gsDefTitle = "Calculate muon parameters";

// -------------------------------------------------------------------------
//
// Default constructor. 
//
MMuonSearchParCalc::MMuonSearchParCalc(const char *name, const char *title)
  : fHillas(NULL), fMuonPar(NULL)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// -------------------------------------------------------------------------
//
Int_t MMuonSearchParCalc::PreProcess(MParList *pList)
{
    fHillas = (MHillas*)pList->FindObject("MHillas");
    if (!fHillas)
    {
        *fLog << err << "MHillas not found... aborting." << endl;
        return kFALSE;
    }

    fSignalCam = (MSignalCam*)pList->FindObject("MSignalCam");
    if (!fSignalCam)
    {
        *fLog << err << "MSignalCam not found... aborting." << endl;
        return kFALSE;
    }

    fGeomCam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fGeomCam)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    fMuonPar = (MMuonSearchPar*)pList->FindCreateObj("MMuonSearchPar");
    if (!fMuonPar)
        return kFALSE;

    return kTRUE;
}

// -------------------------------------------------------------------------
//
Int_t MMuonSearchParCalc::Process()
{
    fMuonPar->Calc(*fGeomCam, *fSignalCam, *fHillas);
    return kTRUE;
}
