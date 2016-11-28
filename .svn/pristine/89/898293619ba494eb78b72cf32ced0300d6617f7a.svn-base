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
!   Author(s): Markus Gaug   02/2004 <mailto:markus@ifae.es>
!              
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//                                                                        
// MHCalibrationTestTimeCam                                                
//                                                                        
// Fills the calibrated signal from an MArrivalTime into 
// MHCalibrationTestTimePix for every:
//
// - Pixel, stored in the TObjArray's MHCalibrationCam::fHiGainArray  
//   or MHCalibrationCam::fHiGainArray, respectively.
//
// - Average pixel per AREA index (e.g. inner and outer for the MAGIC camera), 
//   stored in the TObjArray's MHCalibrationCam::fAverageHiGainAreas and 
//   MHCalibrationCam::fAverageHiGainAreas
//
// - Average pixel per camera SECTOR (e.g. sectors 1-6 for the MAGIC camera), 
//   stored in the TObjArray's MHCalibrationCam::fAverageHiGainSectors 
//   and MHCalibrationCam::fAverageHiGainSectors 
//
// The signals are filled into a histogram and an array, in order to perform 
// a Fourier analysis (see MHGausEvents). The signals are moreover averaged on an 
// event-by-event basis and written into the corresponding average pixels.
//
// The histograms are fitted to a Gaussian, mean and sigma with its errors 
// and the fit probability are extracted. If none of these values are NaN's and 
// if the probability is bigger than MHGausEvents::fProbLimit (default: 0.5%), 
// the fit is declared valid.
// Otherwise, the fit is repeated within ranges of the previous mean 
// +- MHCalibrationPix::fPickupLimit (default: 5) sigma (see MHCalibrationPix::RepeatFit())
// In case this does not make the fit valid, the histogram means and RMS's are 
// taken directly (see MHCalibrationPix::BypassFit()) and the following flags are set:
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kHiGainNotFitted ) and
// - MBadPixelsPix::SetUnsuitable(   MBadPixelsPix::kUnreliableRun    ) 
// 
// Outliers of more than MHCalibrationPix::fPickupLimit (default: 5) sigmas 
// from the mean are counted as Pickup events (stored in MHCalibrationPix::fPickup) 
//
// The class also fills arrays with the signal vs. event number, creates a fourier 
// spectrum (see MHGausEvents::CreateFourierSpectrum()) and investigates if the 
// projected fourier components follow an exponential distribution. 
// In case that the probability of the exponential fit is less than 
// MHGausEvents::fProbLimit (default: 0.5%), the following flags are set:
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kHiGainOscillating ) and
// - MBadPixelsPix::SetUnsuitable(   MBadPixelsPix::kUnreliableRun      )
// 
// This same procedure is performed for the average pixels.
//
// The following results are written into an MCalibrationCam:
//
// - MCalibrationPix::SetMean()
// - MCalibrationPix::SetMeanErr()
// - MCalibrationPix::SetSigma()
// - MCalibrationPix::SetSigmaErr()
// - MCalibrationPix::SetProb()
// - MCalibrationPix::SetNumPickup()
//
// For all averaged areas, the fitted sigma is multiplied with the square root of 
// the number involved pixels in order to be able to compare it to the average of 
// sigmas in the camera.
//                                                                         
/////////////////////////////////////////////////////////////////////////////
#include "MHCalibrationTestTimeCam.h"

#include "MHCalibrationPix.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MCalibrationCam.h"
#include "MCalibrationPix.h"

#include "MSignalCam.h"
#include "MSignalPix.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include <TOrdCollection.h>

ClassImp(MHCalibrationTestTimeCam);

using namespace std;

const Int_t   MHCalibrationTestTimeCam::fgNbins      = 300;
const Axis_t  MHCalibrationTestTimeCam::fgFirst      = -0.025;
const Axis_t  MHCalibrationTestTimeCam::fgLast       = 14.975;
const Float_t MHCalibrationTestTimeCam::fgProbLimit  = 0.00000001;
const TString MHCalibrationTestTimeCam::gsHistName   = "TestTime";
const TString MHCalibrationTestTimeCam::gsHistTitle  = "Calibrated Calibration Arrival Times";  
const TString MHCalibrationTestTimeCam::gsHistXTitle = "Arrival Time [FADC slices]";
const TString MHCalibrationTestTimeCam::gsHistYTitle = "Nr. events";

// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Sets: 
// - fNbins to fgNbins
// - fFirst to fgFirst
// - fLast  to fgLast 
//
// - fHistName   to gsHistName  
// - fHistTitle  to gsHistTitle 
// - fHistXTitle to gsHistXTitle
// - fHistYTitle to gsHistYTitle
//
MHCalibrationTestTimeCam::MHCalibrationTestTimeCam(const char *name, const char *title) 
{

  fName  = name  ? name  : "MHCalibrationTestTimeCam";
  fTitle = title ? title : "Histogram class for testing the calibration of arrival times";

  SetBinning(fgNbins, fgFirst, fgLast);

  SetProbLimit(fgProbLimit);

  SetHistName  (gsHistName  .Data());
  SetHistTitle (gsHistTitle .Data());
  SetHistXTitle(gsHistXTitle.Data());
  SetHistYTitle(gsHistYTitle.Data());

  SetLoGain(kFALSE);

}

// --------------------------------------------------------------------------
//
// Searches pointer to:
// - MArrivalTime
//
// Calls:
// - MHCalibrationCam::InitHiGainArrays()
// 
// Sets: 
// - SetLoGain(kFALSE);
//
Bool_t MHCalibrationTestTimeCam::ReInitHists(MParList *pList)
{

  if (!InitCams(pList,""))
    return kFALSE;

  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nsectors = fGeom->GetNumSectors();
  const Int_t nareas   = fGeom->GetNumAreas();

  InitHiGainArrays(npixels,nareas,nsectors);
  InitLoGainArrays(npixels,nareas,nsectors);

  return kTRUE;
}


// -------------------------------------------------------------------------------
//
// Retrieves pointer to MArrivalTime:
//
// Retrieves from MGeomCam:
// - number of pixels
// - number of pixel areas
// - number of sectors
//
// Fills HiGain histograms (MHGausEvents::FillHistAndArray())
// with:
// - MArrivalTime::GetArrivalTime(pixid) - MArrivalTime::GetArrivalTime(1);
//   (i.e. the time difference between pixel i and pixel 1 (hardware number: 2) )
//
Bool_t MHCalibrationTestTimeCam::FillHists(const MParContainer *par, const Stat_t w)
{

  MSignalCam *calibration = (MSignalCam*)par;
  if (!calibration)
    {
      gLog << err << "No argument in MHCalibrationRelTimeCam::Fill... abort." << endl;
      return kFALSE;
    }

  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nareas   = fGeom->GetNumAreas();
  const Int_t nsectors = fGeom->GetNumSectors();

  TArrayF sumareahi  (nareas); 
  TArrayF sumsectorhi(nsectors);
  TArrayI numareahi  (nareas); 
  TArrayI numsectorhi(nsectors);

  for (Int_t i=0; i<npixels; i++)
    {

      MHCalibrationPix &histhi = (*this)[i];

      if (histhi.IsExcluded())
	continue;

      const Float_t  time = (*calibration)[i].GetArrivalTime();
      const Int_t  aidx   = (*fGeom)[i].GetAidx();
      const Int_t  sector = (*fGeom)[i].GetSector();

      histhi.FillHistAndArray(time) ;
      sumareahi  [aidx]   += time;
      numareahi  [aidx]   ++;
      sumsectorhi[sector] += time;
      numsectorhi[sector] ++;
    }
  
  for (Int_t j=0; j<nareas; j++)
    {
      MHCalibrationPix &histhi = GetAverageHiGainArea(j);
      histhi.FillHistAndArray(numareahi[j] == 0 ? 0. : sumareahi[j]/numareahi[j]);

    }
  
  for (Int_t j=0; j<nsectors; j++)
    {
      MHCalibrationPix &histhi = GetAverageHiGainSector(j);
      histhi.FillHistAndArray(numsectorhi[j] == 0 ? 0. : sumsectorhi[j]/numsectorhi[j]);

    }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// 
Bool_t MHCalibrationTestTimeCam::FinalizeHists()
{

  for (Int_t i=0; i<fHiGainArray->GetSize(); i++)
    {
      
      MHCalibrationPix &hist = (*this)[i];
      
      if (hist.IsExcluded())
        continue;
      
      if (hist.IsEmpty())
        continue;
      
      if (!hist.FitGaus())
        if (!hist.RepeatFit())
          {
            hist.BypassFit();
          }
      
      hist.CreateFourierSpectrum();
      
    }
  
  for (Int_t j=0; j<fAverageHiGainAreas->GetSize(); j++)
    {
      
      MHCalibrationPix     &hist = GetAverageHiGainArea(j);      
      if (hist.IsEmpty())
        continue;
      
      if (!hist.FitGaus())
        if (!hist.RepeatFit())
          {
            hist.BypassFit();
          }
      
      hist.CreateFourierSpectrum();
      

  }

  for (Int_t j=0; j<fAverageHiGainSectors->GetSize(); j++)
    {
      
      MHCalibrationPix     &hist = GetAverageHiGainSector(j);      
      if (hist.IsEmpty())
        continue;
      
      if (!hist.FitGaus())
        if (!hist.RepeatFit())
          {
            hist.BypassFit();
          }
      hist.CreateFourierSpectrum();
      

    }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
void MHCalibrationTestTimeCam::FinalizeBadPixels()
{

}

// --------------------------------------------------------------------------
//
// The types are as follows:
// 
// Fitted values:
// ============== 
//
// 0: Fitted Mean Time Calibration (MHGausEvents::GetMean())
// 1: Error Mean Time Calibration  (MHGausEvents::GetMeanErr())
// 2: Sigma fitted Time Calibration (MHGausEvents::GetSigma())
// 3: Error Sigma Time Calibration (MHGausEvents::GetSigmaErr())
//
// Useful variables derived from the fit results:
// =============================================
//
// 4: Returned probability of Gauss fit              (calls: MHGausEvents::GetProb())
//
// Localized defects:
// ==================
//
// 5: Gaus fit not OK                               (calls: MHGausEvents::IsGausFitOK())
// 6: Fourier spectrum not OK                       (calls: MHGausEvents::IsFourierSpectrumOK())
//
Bool_t MHCalibrationTestTimeCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{

  if (fHiGainArray->GetSize() <= idx)
    return kFALSE;

  const MHCalibrationPix &pix = (*this)[idx];

  if (pix.IsExcluded())
    return kFALSE;

  switch (type)
    {
    case 0:
      val = pix.GetMean();
      break;
    case 1:
      val = pix.GetMeanErr();
      break;
    case 2:
      val = pix.GetSigma();
      break;
    case 3:
      val = pix.GetSigmaErr();
      break;
    case 4:
      val = pix.GetProb();
      break;
    case 5:
      if (!pix.IsGausFitOK())
        val = 1.;
      break;
    case 6:
      if (!pix.IsFourierSpectrumOK())
        val = 1.;
      break;
    default:
      return kFALSE;
    }
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calls MHCalibrationPix::DrawClone() for pixel idx
//
void MHCalibrationTestTimeCam::DrawPixelContent(Int_t idx) const
{
 (*this)[idx].DrawClone();
}


//------------------------------------------------------------
//
// For all averaged areas, the fitted sigma is multiplied with the square root of 
// the number involved pixels
//
void MHCalibrationTestTimeCam::CalcAverageSigma()
{
  
  for (UInt_t j=0; j<fGeom->GetNumAreas(); j++)
    {
  
      MHCalibrationPix &hist    = GetAverageHiGainArea(j);

      const Float_t numsqr    = TMath::Sqrt((Float_t)fAverageAreaNum[j]);
      fAverageAreaSigma[j]    = hist.GetSigma    () * numsqr;
      fAverageAreaSigmaVar[j] = hist.GetSigmaErr () * hist.GetSigmaErr() * numsqr;

      fAverageAreaRelSigma   [j]  = fAverageAreaSigma[j]    / hist.GetMean();
      fAverageAreaRelSigmaVar[j]  = fAverageAreaSigmaVar[j] / (fAverageAreaSigma[j]*fAverageAreaSigma[j]);
      fAverageAreaRelSigmaVar[j] += hist.GetMeanErr()*hist.GetMeanErr()/hist.GetMean()/hist.GetMean();
      fAverageAreaRelSigmaVar[j] *= fAverageAreaRelSigma[j];
    }
}
