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
!   Author(s): Antonio Stamerra  1/2003 <mailto:antono.stamerra@pi.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MTriggerLvl2Calc                                                        //
//   This is a task to calculate the 2nd level trigger selection           //
//   parameters                                                            //
//                                                                         //
//  Input containers:                                                      //
//    MMcTriggerLvl2                                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "MMcTriggerLvl2Calc.h"
#include "MMcTriggerLvl2.h"
#include "MHMcTriggerLvl2.h"

#include "MParList.h"
#include "MLog.h"
#include "MLogManip.h"

//#include "MMcEvt.hxx"
#include "MMcTrig.hxx"
#include "MRawRunHeader.h"

#include "MGeomCam.h"

ClassImp(MMcTriggerLvl2Calc);

using namespace std;

// --------------------------------------------------------------------------
//                                                                         
//  Default constructor
// 
//
MMcTriggerLvl2Calc::MMcTriggerLvl2Calc(const char *name, const char *title)
{
  fName  = name  ? name  : "MMcTriggerLvl2Calc";
  fTitle = title ? title : "Task to Fill the MMcTriggerLvl2 object";  
} 

 // --------------------------------------------------------------------------
//
// Check for the run type. Return kTRUE if it is a MC run or if there
// is no MC run header (old camera files) kFALSE in case of a different
// run type
//
Bool_t MMcTriggerLvl2Calc::IsMCRun(MParList *pList) const
{
  const MRawRunHeader *run = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
  if (!run)
    {
      *fLog << warn << dbginf << "Warning - cannot check file type, MRawRunHeader not found." << endl;
      return kTRUE;
    }
  
  return run->IsMonteCarloRun();
}

// --------------------------------------------------------------------------
//
// Check for the runtype.
// Search for .
//
Bool_t MMcTriggerLvl2Calc::ReInit(MParList *pList)
{
    return kTRUE;
}



// --------------------------------------------------------------------------
//                                                                         
//  PreProcess
//  Check the exxistence of the MMcTriggerLvl2 containers and the correct 
//    setting of the fCompactNN that defines a compact pixel
//
Int_t MMcTriggerLvl2Calc::PreProcess (MParList *pList)
{

    //    fMMcTriggerLvl2 = (MMcTriggerLvl2*)pList->FindObject("MMcTriggerLvl2");
    fMMcTriggerLvl2 = (MMcTriggerLvl2*)pList->FindCreateObj("MMcTriggerLvl2");
    if (!fMMcTriggerLvl2)
    {
        *fLog << err << dbginf << "MMcTriggerLvl2 not found... exit." << endl;
        return kFALSE;
    }

    fMHMcTriggerLvl2 = (MHMcTriggerLvl2*)pList->FindCreateObj("MHMcTriggerLvl2");
    if (!fMHMcTriggerLvl2)
    {
        *fLog << err << dbginf << "MHMcTriggerLvl2 not found... exit." << endl;
        return kFALSE;
    }

    // Check if the variable fCompactNN has been correctly set;
    //  accepted values for fCompactNN are (up to now) 2 and 3.
    if (fMMcTriggerLvl2->GetCompactNN()<2 || fMMcTriggerLvl2->GetCompactNN()>3)
      {
	*fLog << err << dbginf << "fCompactNN is not correctly set ("<<fMMcTriggerLvl2->GetCompactNN() <<") ... exit" <<endl;
        return kFALSE;	
      }
    else
      *fLog << "Compact pixel is set with at least "<<fMMcTriggerLvl2->GetCompactNN() << " NN" <<endl;

    //------------------------------------------------------------
    //
    // If it is no MC file skip this function...
    //
    if (!IsMCRun(pList))
    {
        *fLog << inf << "Reading a data file...skipping the rest of PreProcess()" << endl;
        return kTRUE;
    }
        
    //
    // Check all necessary containers in case of a MC run
    //
    fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
     if (!fMcEvt)
     {
         *fLog << err << dbginf << "MMcEvt not found... exit." << endl;
         return kFALSE;
     }

    fMcTrig = (MMcTrig*)pList->FindObject("MMcTrig");
    if (!fMcTrig)
    {
        *fLog << err << dbginf << "MMcTrig not found... exit." << endl;
        return kFALSE;
    }

    fCam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fCam)
    {
        *fLog << err << "MGeomCam not found (no geometry information available)... aborting." << endl;
        return kFALSE;
    }
    // Check if fCam is a Magic geometry: only MGeomCamMagic geometry and 
    // geometries with 577 pixels are now accepted by MMcTriggerLvl2
    if (fCam->GetNumPixels()!= 577)
      {
        *fLog << warn << "MGeomCam has a wrong geometry; only MAGIC geometry (577 pixels) is accepted by now... the Task is skipped." <<endl;
	return kSKIP;
      }

    return kTRUE;

}


// --------------------------------------------------------------------------
//                                                                         
//  Process 
// 
//
Int_t MMcTriggerLvl2Calc::Process()
{
  //  fMMcTriggerLvl2->GetEnergy(fMcEvt);

  fMMcTriggerLvl2->SetLv1(fMcTrig);

  fMMcTriggerLvl2->CalcCompactPixels(fCam);

  fMMcTriggerLvl2->Calc();

  fMMcTriggerLvl2->CalcTriggerPattern(fCam);

  return kTRUE;
}


// --------------------------------------------------------------------------
//                                                                         
//  PostProcess : Display the histogram 
//           !to be fixed: create an histogram class!
//
Int_t MMcTriggerLvl2Calc::PostProcess()
{ 
  
    return kTRUE;
}




