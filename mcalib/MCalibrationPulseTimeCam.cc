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
// MCalibrationPulseTimeCam                                               
//                                                               
// Storage container for relative arrival time calibration results         
// of the whole camera.
//
// Individual pixels have to be cast when retrieved e.g.:
// MCalibrationPix &avpix = (MCalibrationPix&)(*fPulseCam)[i]
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
// MCalibrationPix &avpix = (MCalibrationPix&)fPulseCam->GetAverageArea(i)
//
// Averaged values over one whole camera sector can be retrieved via: 
// MCalibrationPix &avpix = (MCalibrationPix&)fPulseCam->GetAverageSector(i)
//
// Note the averageing has been done on an event-by-event basis. Resulting 
// Sigma's of the Gauss fit have been multiplied with the square root of the number 
// of involved pixels in order to make a direct comparison possible with the mean of 
// sigmas. 
//
// See also: MHCalibrationPix, MHCalibrationPulseTimeCam              
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
// 4: Returned probability of Gauss fit to Pulse. Arrival Time distribution
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationPulseTimeCam.h"
#include "MCalibrationCam.h"

#include <TOrdCollection.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MCalibrationPix.h"

ClassImp(MCalibrationPulseTimeCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCalibrationPulseTimeCam::MCalibrationPulseTimeCam(const char *name, const char *title)
{

  fName  = name  ? name  : "MCalibrationPulseTimeCam";
  fTitle = title ? title : "Container for Pulse Time Information";
  
}

// --------------------------------------------------------------------------
//
// Print first the well fitted pixels 
// and then the ones which are not Valid
//
void MCalibrationPulseTimeCam::Print(Option_t *o) const
{

  *fLog << all << GetDescriptor() << ":" << endl;
  int id = 0;

  *fLog << all << "Calibrated pixels:" << endl;
  *fLog << all << endl;

  TIter Next(fPixels);
  MCalibrationPix *pix;
  while ((pix=(MCalibrationPix*)Next()))
    {

      if (!pix->IsExcluded()) 
	{                            

          *fLog << all 
                << Form("Pix  %4i:  Mean Time: %4.2f+-%4.2f Time Jitter: %4.2f+-%4.2f",pix->GetPixId(),
                        pix->GetMean(),pix->GetMeanErr(),pix->GetSigma(),pix->GetSigmaErr())
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
  while ((pix=(MCalibrationPix*)Next4()))
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
  while ((pix=(MCalibrationPix*)Next5()))
  {
    *fLog << all 
	  << Form("Pix  %4i:  Mean Time: %4.2f+-%4.2f Time Jitter: %4.2f+-%4.2f",pix->GetPixId(),
		  pix->GetMean(),pix->GetMeanErr(),pix->GetSigma(),pix->GetSigmaErr())
          << endl;
  }

  TIter Next6(fAverageSectors);
  while ((pix=(MCalibrationPix*)Next5()))
  {
    *fLog << all 
	  << Form("Pix  %4i:  Mean Time: %4.2f+-%4.2f Time Jitter: %4.2f+-%4.2f",pix->GetPixId(),
		  pix->GetMean(),pix->GetMeanErr(),pix->GetSigma(),pix->GetSigmaErr())
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
// 0: Fitted PulseTime
// 1: Error of fitted PulseTime
// 2: Sigma of fitted PulseTime
// 3: Error of Sigma of fitted PulseTime
//
// Useful variables derived from the fit results:
// =============================================
//
// 4: Returned probability of Gauss fit to PulseTime distribution
//
Bool_t MCalibrationPulseTimeCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{

  if (idx > GetSize())
    return kFALSE;

  Float_t area = cam[idx].GetA();

  if (area == 0)
    return kFALSE;

  const MCalibrationPix &pix = static_cast<const MCalibrationPix&>((*this)[idx]);

  switch (type)
    {
    case 0:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiGainMean();
      break;
    case 1:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiGainMeanErr();
      break;
    case 2:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiGainSigma();
      break;
    case 3:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetHiGainSigmaErr();
      break;
    case 4:
      if (pix.IsExcluded())
        return kFALSE;
      val = pix.GetProb();
      break;
    default:
      return kFALSE;
    }

  return val!=-1.;

}
