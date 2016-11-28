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
!   Author(s): Markus Gaug   11/2003 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                               
// MCalibrationHiLoCam                                               
//                                                               
// Storage container for ratio between higain and logain charge extraction
// calibration results of the whole camera.
//
// Individual pixels have to be cast when retrieved e.g.:
// MCalibrationHiLoPix &avpix = (MCalibrationHiLoPix&)(*fHiLoCam)[i]
// 
// The following "calibration" constants can be retrieved from each pixel:
// - GetHiLoRatio(): The mean signal ratio between high-gain and low-gain
// - GetHiLoRatioErr(): The Gauss sigma of histogrammed signal ratios
//
// Averaged values over one whole area index (e.g. inner or outer pixels for 
// the MAGIC camera), can be retrieved via: 
// MCalibrationHiLoPix &avpix = (MCalibrationHiLoPix&)fRelCam->GetAverageArea(i)
//
// Averaged values over one whole camera sector can be retrieved via: 
// MCalibrationHiLoPix &avpix = (MCalibrationHiLoPix&)fRelCam->GetAverageSector(i)
//
// Note the averageing has been done on an event-by-event basis. Resulting 
// Sigma's of the Gauss fit have been multiplied with the square root of the number 
// of involved pixels in order to make a direct comparison possible with the mean of 
// sigmas. 
//
// See also: MHCalibrationHiLoPix, MHCalibrationHiLoCam              
//                                                                         
// The calculated values (types of GetPixelContent) are:
// 
// Fitted values:
// ============== 
//
// 0: HiLo Ratio
// 1: Error of HiLo Ratio
// 2: Sigma of HiLo Ratio == Resolution 
// 3: Error of Sigma of HiLo Ratio
//
// Useful variables derived from the fit results:
// =============================================
//
// 4: Returned probability of Gauss fit to distribution
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationHiLoCam.h"
#include "MCalibrationCam.h"

#include <TOrdCollection.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MCalibrationHiLoPix.h"

ClassImp(MCalibrationHiLoCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCalibrationHiLoCam::MCalibrationHiLoCam(const char *name, const char *title)
{

  fName  = name  ? name  : "MCalibrationHiLoCam";
  fTitle = title ? title : "Container for High Gain vs. Low Gain amplification ratio";
  
}

void MCalibrationHiLoCam::Add(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fPixels->AddAt(new MCalibrationHiLoPix,i);
}


void MCalibrationHiLoCam::AddArea(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fAverageAreas->AddAt(new MCalibrationHiLoPix,i);
}

void MCalibrationHiLoCam::AddSector(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fAverageSectors->AddAt(new MCalibrationHiLoPix,i);
}

void MCalibrationHiLoCam::PrintPix(const MCalibrationHiLoPix &pix, const char *type) const
{
    *fLog << all << setw(7) << type << Form("%4i", pix.GetPixId()) << ":   ";
    *fLog << "   Ratio: " << Form("%4.2f", pix.GetHiLoChargeRatio());
    *fLog << " +- " << Form("%4.2f", pix.GetHiLoChargeRatioErr());
    *fLog << "   Sigma: " << Form("%4.2f", pix.GetHiLoChargeRatioSigma());
    *fLog << " +- " << Form("%4.2f", pix.GetHiLoChargeRatioSigmaErr());
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Print first the well fitted pixels 
// and then the ones which are not Valid
//
void MCalibrationHiLoCam::Print(Option_t *o) const
{

  *fLog << all << GetDescriptor() << ":" << endl;
  int id = 0;
  
  *fLog << all << "Calibrated pixels:" << endl;
  *fLog << all << endl;

  TIter Next(fPixels);
  MCalibrationHiLoPix *pix;
  while ((pix=(MCalibrationHiLoPix*)Next()))
      if (!pix->IsExcluded())
      {
          PrintPix(*pix, "Pixel");
          id++;
      }
  
  *fLog << all << id << " pixels" << endl;
  id = 0;
  
   
  *fLog << all << endl;
  *fLog << all << "Excluded pixels:" << endl;
  *fLog << all << endl;
  
  id = 0;

  TIter Next4(fPixels);
  while ((pix=(MCalibrationHiLoPix*)Next4()))
  {
      if (pix->IsExcluded())
      {
	  *fLog << all << pix->GetPixId() << endl;
	  id++;
      }
  }
  *fLog << all << id << " Excluded pixels " << endl;
  *fLog << endl;

  TIter Next5(fAverageAreas);
  while ((pix=(MCalibrationHiLoPix*)Next5()))
      PrintPix(*pix, "Area");

  TIter Next6(fAverageSectors);
  while ((pix=(MCalibrationHiLoPix*)Next5()))
      PrintPix(*pix, "Sector");
}


// --------------------------------------------------------------------------
//
// The types are as follows:
// 
// Fitted values:
// ============== 
//
// 0: Fitted HiLo
// 1: Error of fitted HiLo
// 2: Sigma of fitted HiLo
// 3: Error of Sigma of fitted HiLo
//
// Useful variables derived from the fit results:
// =============================================
//
// 4: Returned probability of Gauss fit to HiLo distribution
//
Bool_t MCalibrationHiLoCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{

  if (idx > GetSize())
    return kFALSE;

  Float_t area = cam[idx].GetA();

 if (area == 0)
    return kFALSE;

 MCalibrationHiLoPix &pix = (MCalibrationHiLoPix&)(*this)[idx];

  switch (type)
    {
    case 0:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiLoChargeRatio();
      break;
    case 1:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiLoChargeRatioErr();
      break;
    case 2:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiLoChargeRatioSigma();
      break;
    case 3:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiLoChargeRatioSigmaErr();
      break;
    case 4:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiLoChargeRatioProb();
      break;
    case 5:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiLoTimeDiff();
      break;
    case 6:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiLoTimeDiffErr();
      break;
    case 7:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiLoTimeDiffSigma();
      break;
    case 8:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiLoTimeDiffSigmaErr();
      break;
    case 9:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiLoTimeDiffProb();
      break;
    default:
      return kFALSE;
    }

  return val!=-1.;

}

