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
// MCalibrationRelTimeCam                                               
//                                                               
// Storage container for relative arrival time calibration results         
// of the whole camera.
//
// Individual pixels have to be cast when retrieved e.g.:
// MCalibrationRelTimePix &avpix = (MCalibrationRelTimePix&)(*fRelCam)[i]
// 
// The following "calibration" constants can be retrieved from each pixel:
// - GetTimeOffset(): The mean offset in relative times, 
//   has to be added to any calculated relative time in the camera. 
// - GetTimePrecision(): The Gauss sigma of histogrammed relative arrival 
//   times for the calibration run. Gives an estimate about the timing 
//   resolution.
//
// ALL RELATIVE TIMES HAVE TO BE CALCULATED W.R.T. PIXEL IDX 1 
// (HARDWARE NUMBER: 2) !!
//
// Averaged values over one whole area index (e.g. inner or outer pixels for 
// the MAGIC camera), can be retrieved via: 
// MCalibrationRelTimePix &avpix = (MCalibrationRelTimePix&)fRelCam->GetAverageArea(i)
//
// Averaged values over one whole camera sector can be retrieved via: 
// MCalibrationRelTimePix &avpix = (MCalibrationRelTimePix&)fRelCam->GetAverageSector(i)
//
// Note the averageing has been done on an event-by-event basis. Resulting 
// Sigma's of the Gauss fit have been multiplied with the square root of the number 
// of involved pixels in order to make a direct comparison possible with the mean of 
// sigmas. 
//
// See also: MHCalibrationRelTimePix, MHCalibrationRelTimeCam              
//                                                                         
// The calculated values (types of GetPixelContent) are:
// 
// Fitted values:
// ============== 
//
// 0: Mean Time Offset
// 1: Error of Mean Time Offset
// 2: Sigma of Time Offset == Time Resolution 
// 3: Error of Sigma of Time Offset
//
// Useful variables derived from the fit results:
// =============================================
//
// 4: Returned probability of Gauss fit to Rel. Arrival Time distribution
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationRelTimeCam.h"
#include "MCalibrationCam.h"

#include <TOrdCollection.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MCalibrationRelTimePix.h"

ClassImp(MCalibrationRelTimeCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCalibrationRelTimeCam::MCalibrationRelTimeCam(const char *name, const char *title)
{

  fName  = name  ? name  : "MCalibrationRelTimeCam";
  fTitle = title ? title : "Container for Relative Time Calibration Information";
  
}

void MCalibrationRelTimeCam::Add(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fPixels->AddAt(new MCalibrationRelTimePix,i);
}


void MCalibrationRelTimeCam::AddArea(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fAverageAreas->AddAt(new MCalibrationRelTimePix,i);
}

void MCalibrationRelTimeCam::AddSector(const UInt_t a, const UInt_t b)
{
  for (UInt_t i=a; i<b; i++)
    fAverageSectors->AddAt(new MCalibrationRelTimePix,i);
}

// --------------------------------------------------------------------------
//
// Print first the well fitted pixels 
// and then the ones which are not Valid
//
void MCalibrationRelTimeCam::Print(Option_t *o) const
{

  *fLog << all << GetDescriptor() << ":" << endl;
  int id = 0;

  *fLog << all << "Calibrated pixels:" << endl;
  *fLog << all << endl;

  TIter Next(fPixels);
  MCalibrationRelTimePix *pix;
  while ((pix=(MCalibrationRelTimePix*)Next()))
  {
      if (!pix->IsExcluded())
      {
          *fLog << all
              << "Pix  " << Form("%4i", pix->GetPixId()) << ":         "
              << "   Offset: "
              << Form("%4.2f +- %4.2f", pix->GetTimeOffset(), pix->GetTimeOffsetErr())
              << "   Precision: "
              << Form("%4.2f +- %4.2f", pix->GetTimePrecision(), pix->GetTimePrecisionErr())
              << endl;
          id++;
      }
  }

  *fLog << all << id << " pixels" << endl;
  id = 0;


  *fLog << all << endl;
  *fLog << all << "Excluded pixels:" << endl;
  *fLog << all << endl;

  id = 0;

  TIter Next4(fPixels);
  while ((pix=(MCalibrationRelTimePix*)Next4()))
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
  while ((pix=(MCalibrationRelTimePix*)Next5()))
  {
      *fLog << all
          << "Average Area   " << Form("%4i", pix->GetPixId()) << ":"
          << "Offset: "
          << Form("%4.2f +- %4.2f", pix->GetTimeOffset(), pix->GetTimeOffsetErr())
          << "   Precision: "
          << Form("%4.2f +- %4.2f", pix->GetTimePrecision(), pix->GetTimePrecisionErr())
          << endl;
  }

  TIter Next6(fAverageSectors);
  while ((pix=(MCalibrationRelTimePix*)Next5()))
  {
      *fLog << all
          << "Average Sector " << Form("%4i", pix->GetPixId()) << ":"
          << "Offset: "
          << Form("%4.2f +- %4.2f", pix->GetTimeOffset(), pix->GetTimeOffsetErr())
          << "   Precision: "
          << Form("%4.2f +- %4.2f", pix->GetTimePrecision(), pix->GetTimePrecisionErr())
          << endl;
  }
}


// --------------------------------------------------------------------------
//
// The types are as follows:
// 
// Fitted values:
// ============== 
//
// 0: Fitted RelTime
// 1: Error of fitted RelTime
// 2: Sigma of fitted RelTime
// 3: Error of Sigma of fitted RelTime
//
// Useful variables derived from the fit results:
// =============================================
//
// 4: Returned probability of Gauss fit to RelTime distribution
//
Bool_t MCalibrationRelTimeCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    if (idx > GetSize())
        return kFALSE;

    const Float_t area = cam[idx].GetA();
    if (area == 0)
        return kFALSE;

    const MCalibrationRelTimePix &pix = (MCalibrationRelTimePix&)(*this)[idx];
    switch (type)
    {
    case 0:
        if (pix.IsExcluded() || pix.GetMeanErr()<0)
            return kFALSE;
        val = pix.GetMean();
        return kTRUE;
    case 1:
        if (pix.IsExcluded() || pix.GetMeanErr()<0)
            return kFALSE;
        val = pix.GetMeanErr();
        return kTRUE;
    case 2:
        if (pix.IsExcluded() || pix.GetSigmaErr()<0)
            return kFALSE;
        val = pix.GetSigma();
        return kTRUE;
    case 3:
        if (pix.IsExcluded() || pix.GetSigmaErr()<0)
            return kFALSE;
        val = pix.GetSigmaErr();
        return kTRUE;
    case 4:
        if (pix.IsExcluded())
            return kFALSE;
        val = pix.GetProb();
        return val>=0;
    default:
        return kFALSE;
    }

  return kFALSE;

}

