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
!   Author(s): Wolfgang Wittek  03/2003 <mailto:wittek@mppmu.mpg.de>
!              Nadia Tonello    05/2003 <mailto:tonello@mppmu.mpg.de>
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  MCT1PointingCorrCalc                                                   //
//                                                                         //
//  This is a task to do the CT1 pointing correction.                      // 
//                                                                         //
//  NT: You can correct the Mkn421 data (default setting), or the 1ES1959  //
//      data.                                                              // 
//      To change to the correction needed for 1ES1959,  you have to call  //
//      the member funcion: SetPointedSource                               //
//                                                                         //
//  Example:                                                               //
//      MCT1PointingCorrCalc correct;                                      //
//      correct.SetPointedSource(MCT1PointingCorrectionCalc::K1959)        //
//                                                                         // 
/////////////////////////////////////////////////////////////////////////////

#include "MCT1PointingCorrCalc.h"

#include "MParList.h"

#include "MSrcPosCam.h"
#include "MGeomCam.h"
#include "MParameters.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCT1PointingCorrCalc);

using namespace std;
// --------------------------------------------------------------------------
//
// Default constructor.
//
MCT1PointingCorrCalc::MCT1PointingCorrCalc(const char *srcname,
                                           const char *name, const char *title)
  : fSrcName(srcname), fPoiSource(k421)
{
    fName  = name  ? name  : "MCT1PointingCorrCalc";
    fTitle = title ? title : "Task to do the CT1 pointing correction";
}

// --------------------------------------------------------------------------
//
Int_t MCT1PointingCorrCalc::PreProcess(MParList *pList)
{
    MGeomCam *geom = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!geom)
        *fLog << warn << GetDescriptor() << ": No Camera Geometry available. Using mm-scale for histograms." << endl;
    else
    {
        fMm2Deg = geom->GetConvMm2Deg();
    }

   fHourAngle = (MParameterD*)pList->FindObject("HourAngle", "MParameterD");
    if (!fHourAngle)
    {
        *fLog << err << "HourAngle [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }


    fSrcPos = (MSrcPosCam*)pList->FindObject(fSrcName, "MSrcPosCam");
    if (!fSrcPos)
    {
        *fLog << err << fSrcName << " [MSrcPosCam] not found... aborting." << endl;
        return kFALSE;
    }


    return kTRUE;
}


// --------------------------------------------------------------------------
//
//Implemented Daniel Kranich's pointing correction for Mkn421 (2001 data)
//

void MCT1PointingCorrCalc::PointCorr421()
{
   //*fLog << "MCT1PointingCorrCalc::Process; fhourangle = " 
   //      << fhourangle << endl;

   Float_t fhourangle = fHourAngle->GetVal();

   Float_t cx = -0.05132 - 0.001064 * fhourangle 
                         - 3.530e-6 * fhourangle * fhourangle;
   cx /= fMm2Deg;

   Float_t cy = -0.04883 - 0.0003175* fhourangle
                         - 2.165e-5 * fhourangle * fhourangle;
   cy /= fMm2Deg;

   fSrcPos->SetXY(cx, cy);

   //*fLog << "MCT1PointingCorrCal::Process; fhourangle, cx, cy, fMm2Deg = "
   //      << fhourangle << ",  " << cx << ",  " << cy << ",  " 
   //      << fMm2Deg << endl;

   fSrcPos->SetReadyToSave();
   return;
}


// --------------------------------------------------------------------------
//
// NT :Implemente Daniel Kranich's pointing correction for 1ES1959 (2002 data)

void MCT1PointingCorrCalc::PointCorr1959()
{
   //*fLog << "MCT1PointingCorrCalc::Process; fhourangle = " 
   //      << fhourangle << endl;

   Float_t fhourangle = fHourAngle->GetVal();

   Float_t cx = -0.086 - 0.00091 * fhourangle ;
   cx /= fMm2Deg;

   Float_t cy = -0.083 - 0.001 * fhourangle ;
   cy /= fMm2Deg;

   fSrcPos->SetXY(cx, cy);

   //*fLog << "MCT1PointingCorrCal::Process; fhourangle, cx, cy, fMm2Deg = "
   //      << fhourangle << ",  " << cx << ",  " << cy << ",  " 
   //      << fMm2Deg << endl;

   fSrcPos->SetReadyToSave();
   return;

}
// --------------------------------------------------------------------------
//
// Do the pointing correction
//
Int_t MCT1PointingCorrCalc::Process()
{
   // fhourangle is the hour angle [degrees]
   // (cx, cy) is the source position in the camera [mm]
   //
   switch (fPoiSource)
    {
    case k421:
        PointCorr421();
    case k1959:
        PointCorr1959();
    }  
   return kTRUE;
}











































