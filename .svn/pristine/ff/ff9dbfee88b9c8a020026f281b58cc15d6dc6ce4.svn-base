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
!   Author(s): Thomas Bretz <mailto:tbretz@astro.uni-wuerzburg.de>
!              
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHCalibrationRelTimeCam
//
// Fills the extracted relative arrival times of MArrivalTimeCam into
// the MHCalibrationPix-classes MHCalibrationPix for every:
//
// - Pixel, stored in the TObjArray's MHCalibrationCam::fHiGainArray
//   or MHCalibrationCam::fHiGainArray, respectively, depending if
//   MArrivalTimePix::IsLoGainUsed() is set.
//
// - Average pixel per AREA index (e.g. inner and outer for the MAGIC camera),
//   stored in the TObjArray's MHCalibrationCam::fAverageHiGainAreas and
//   MHCalibrationCam::fAverageHiGainAreas
//
// - Average pixel per camera SECTOR (e.g. sectors 1-6 for the MAGIC camera),
//   stored in the TObjArray's MHCalibrationCam::fAverageHiGainSectors
//   and MHCalibrationCam::fAverageHiGainSectors
//
// Every relative time is calculated as the difference between the individual
// pixel arrival time and the one of pixel 1 (hardware number: 2).
// The relative times are filled into a histogram and an array, in order to perform
// a Fourier analysis (see MHGausEvents). The signals are moreover averaged on an
// event-by-event basis and written into the corresponding average pixels.
//
// The histograms are fitted to a Gaussian, mean and sigma with its errors
// and the fit probability are extracted. If none of these values are NaN's and
// if the probability is bigger than MHGausEvents::fProbLimit (default: 0.5%),
// the fit is declared valid.
// Otherwise, the fit is repeated within ranges of the previous mean
// - MHCalibrationPix::fPickupLimit (default: 5) sigma (see MHCalibrationPix::RepeatFit())
//   In case this does not make the fit valid, the histogram means and RMS's are
//   taken directly (see MHCalibrationPix::BypassFit()) and the following flags are set:
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kRelTimeNotFitted ) and
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
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kRelTimeOscillating ) and
// - MBadPixelsPix::SetUnsuitable(   MBadPixelsPix::kUnreliableRun      )
// 
// This same procedure is performed for the average pixels.
//
// The following results are written into MCalibrationRelTimeCam:
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
#include "MHCalibrationRelTimeCam.h"
#include "MHCalibrationPix.h"

#include <TEnv.h>
#include <TMath.h>

#include <TOrdCollection.h>

#include <TStyle.h>
#include <TLine.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TCanvas.h>

#include <TF1.h>
#include <TGraph.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MCalibrationRelTimeCam.h"
#include "MCalibrationRelTimePix.h"
#include "MCalibrationPix.h"

#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

ClassImp(MHCalibrationRelTimeCam);

using namespace std;

const Float_t MHCalibrationRelTimeCam::fgNumHiGainSaturationLimit = 0.25;
//const UInt_t  MHCalibrationRelTimeCam::fgReferencePixel = 1;
const Int_t   MHCalibrationRelTimeCam::fgNbins      = 400;
const Axis_t  MHCalibrationRelTimeCam::fgFirst      = -9.975;
const Axis_t  MHCalibrationRelTimeCam::fgLast       = 10.025;
const Float_t MHCalibrationRelTimeCam::fgProbLimit  = 0.0;
const TString MHCalibrationRelTimeCam::gsHistName   = "RelTime";
const TString MHCalibrationRelTimeCam::gsHistTitle  = "Arr. Times";
const TString MHCalibrationRelTimeCam::gsHistXTitle = "Arr. Time [FADC slices]";
const TString MHCalibrationRelTimeCam::gsHistYTitle = "Nr. events";
const TString MHCalibrationRelTimeCam::fgReferenceFile = "mjobs/calibrationref.rc";

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
MHCalibrationRelTimeCam::MHCalibrationRelTimeCam(const char *name, const char *title) 
{

  fName  = name  ? name  : "MHCalibrationRelTimeCam";
  fTitle = title ? title : "Histogram class for the relative time calibration of the camera";

  SetNumHiGainSaturationLimit(fgNumHiGainSaturationLimit);

//  SetReferencePixel();

  SetBinning(fgNbins, fgFirst, fgLast);

  SetProbLimit(fgProbLimit);

  SetHistName  (gsHistName  .Data());
  SetHistTitle (gsHistTitle .Data());
  SetHistXTitle(gsHistXTitle.Data());
  SetHistYTitle(gsHistYTitle.Data());

  SetReferenceFile();

  fInnerRefTime  = 2.95;   
  fOuterRefTime  = 3.6;
}

// --------------------------------------------------------------------------
//
// Creates new MHCalibrationRelTimeCam only with the averaged areas:
// the rest has to be retrieved directly, e.g. via: 
//     MHCalibrationRelTimeCam *cam = MParList::FindObject("MHCalibrationRelTimeCam");
//  -  cam->GetAverageSector(5).DrawClone();
//  -  (*cam)[100].DrawClone()
//
TObject *MHCalibrationRelTimeCam::Clone(const char *) const
{

  MHCalibrationRelTimeCam *cam = new MHCalibrationRelTimeCam();

  //
  // Copy the data members
  //
  cam->fColor                  = fColor;
  cam->fRunNumbers             = fRunNumbers;
  cam->fPulserFrequency        = fPulserFrequency;
  cam->fFlags                  = fFlags;
  cam->fNbins                  = fNbins;
  cam->fFirst                  = fFirst;
  cam->fLast                   = fLast;

  cam->fReferenceFile          = fReferenceFile;
  cam->fInnerRefTime           = fInnerRefTime;
  cam->fOuterRefTime           = fOuterRefTime;

  //
  // Copy the MArrays
  //
  cam->fAverageAreaRelSigma    = fAverageAreaRelSigma;
  cam->fAverageAreaRelSigmaVar = fAverageAreaRelSigmaVar;
  cam->fAverageAreaSat         = fAverageAreaSat;
  cam->fAverageAreaSigma       = fAverageAreaSigma;
  cam->fAverageAreaSigmaVar    = fAverageAreaSigmaVar;
  cam->fAverageAreaNum         = fAverageAreaNum;
  cam->fAverageSectorNum       = fAverageSectorNum;

  if (!IsAverageing())
    return cam;

  const Int_t navhi   =  fAverageHiGainAreas->GetSize();

  for (int i=0; i<navhi; i++)
    cam->fAverageHiGainAreas->AddAt(GetAverageHiGainArea(i).Clone(),i);

  if (IsLoGain())
    {
      
      const Int_t navlo = fAverageLoGainAreas->GetSize();
      for (int i=0; i<navlo; i++)
        cam->fAverageLoGainAreas->AddAt(GetAverageLoGainArea(i).Clone(),i);

    }

  return cam;
}

// --------------------------------------------------------------------------
//
// Gets or creates the pointers to:
// - MCalibrationRelTimeCam
//
// Searches pointer to:
// - MArrivalTimeCam
//
// Calls:
// - MHCalibrationCam::InitHiGainArrays()
// - MHCalibrationCam::InitLoGainArrays()
// 
// Sets:
// - fSumareahi   to nareas
// - fSumarealo   to nareas
// - fSumsectorhi to nareas
// - fSumsectorlo to nareas
// - fNumareahi   to nareas
// - fNumarealo   to nareas
// - fNumsectorhi to nareas
// - fNumsectorlo to nareas
//
Bool_t MHCalibrationRelTimeCam::ReInitHists(MParList *pList)
{

  if (!InitCams(pList,"RelTime"))
    return kFALSE;

  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nsectors = fGeom->GetNumSectors();
  const Int_t nareas   = fGeom->GetNumAreas();

  InitHiGainArrays(npixels,nareas,nsectors);
  InitLoGainArrays(npixels,nareas,nsectors);

  fSumareahi  .Set(nareas); 
  fSumarealo  .Set(nareas);
  fSumsectorhi.Set(nsectors); 
  fSumsectorlo.Set(nsectors);
  fNumareahi  .Set(nareas); 
  fNumarealo  .Set(nareas);
  fNumsectorhi.Set(nsectors); 
  fNumsectorlo.Set(nsectors);

  return kTRUE;
}


// -------------------------------------------------------------------------------
//
// Retrieves pointer to MArrivalTimeCam:
//
// Retrieves from MGeomCam:
// - number of pixels
// - number of pixel areas
// - number of sectors
//
// Fills HiGain or LoGain histograms (MHGausEvents::FillHistAndArray()), respectively
// depending on MArrivalTimePix::IsLoGainUsed(), with:
// - MArrivalTimePix::GetArrivalTime(pixid) - MArrivalTimePix::GetArrivalTime(1);
//   (i.e. the time difference between pixel i and pixel 1 (hardware number: 2) )
//
Bool_t MHCalibrationRelTimeCam::FillHists(const MParContainer *par, const Stat_t w)
{

  MArrivalTimeCam *arrtime = (MArrivalTimeCam*)par;
  if (!arrtime)
    {
      gLog << err << "No argument in MArrivalTime::Fill... abort." << endl;
      return kFALSE;
    }

  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nareas   = fGeom->GetNumAreas();
  const Int_t nsectors = fGeom->GetNumSectors();

  fSumareahi  .Reset();
  fSumarealo  .Reset();
  fSumsectorhi.Reset();
  fSumsectorlo.Reset();
  fNumareahi  .Reset();
  fNumarealo  .Reset();
  fNumsectorhi.Reset();
  fNumsectorlo.Reset();

  TArrayF arr(npixels);
  Int_t n = 0;
  for (Int_t i=0; i<npixels; i++)
  {
      if ((*fGeom)[i].GetAidx()>0)
          continue;

      const MArrivalTimePix &pix = (*arrtime)[i];
      if (pix.IsHiGainValid() && !pix.IsHiGainSaturated())
          arr[n++] = pix.GetArrivalTimeHiGain();
  }

  const Float_t reftime = TMath::Median(n, arr.GetArray());

  for (Int_t i=0; i<npixels; i++)
  {
      MHCalibrationPix &histhi = (*this)[i];
      if (histhi.IsExcluded())
          continue;

      const MArrivalTimePix &pix = (*arrtime)[i];

      // If hi-gain arrival time has been extracted successfully
      // fill hi-gain histograms and arrays
      if (pix.IsHiGainValid() && !pix.IsHiGainSaturated())
      {
          const Float_t time = pix.GetArrivalTimeHiGain();

          if (IsOscillations())
              histhi.FillHistAndArray(time-reftime);
          else
              histhi.FillHist(time-reftime);

          const Int_t aidx   = (*fGeom)[i].GetAidx();
          const Int_t sector = (*fGeom)[i].GetSector();

          fSumareahi  [aidx]   += time;
          fNumareahi  [aidx]   ++;
          fSumsectorhi[sector] += time;
          fNumsectorhi[sector] ++;
      }

      if (!pix.IsHiGainSaturated())
          continue;

      histhi.AddSaturated(1);
/*
      // If lo-gain arrival time has been extracted successfully,
      // the hi-gain has saturated and the lo-gain is switched on
      // fill hi-gain histograms and arrays
      if (pix.IsLoGainValid() && IsLoGain())
      {
          const Float_t time = pix.GetArrivalTimeLoGain();

          // FIXME: We need the reference time of the lo-gains!

          MHCalibrationPix &histlo = (*this)(i);
          if (IsOscillations())
              histlo.FillHistAndArray(time-reftime);
          else
              histlo.FillHist(time-reftime);

          fSumarealo  [aidx]   += time;
          fNumarealo  [aidx]   ++;
          fSumsectorlo[sector] += time;
          fNumsectorlo[sector] ++;
      }*/
  }

  for (Int_t j=0; j<nareas; j++)
  {
      if (fNumareahi[j]>0)
      {
          MHCalibrationPix &histhi = GetAverageHiGainArea(j);
          if (IsOscillations())
              histhi.FillHistAndArray(fSumareahi[j]/fNumareahi[j]);
          else
              histhi.FillHist(fSumareahi[j]/fNumareahi[j]);
      }

      if (IsLoGain() && fNumarealo[j]>0)
      {
          MHCalibrationPix &histlo = GetAverageLoGainArea(j);
          if (IsOscillations())
              histlo.FillHistAndArray(fSumarealo[j]/fNumarealo[j]);
          else
              histlo.FillHist(fSumarealo[j]/fNumarealo[j]);
      }
  }

  for (Int_t j=0; j<nsectors; j++)
  {
      if (fNumsectorhi[j]>0)
      {
          MHCalibrationPix &histhi = GetAverageHiGainSector(j);
          if (IsOscillations())
              histhi.FillHistAndArray(fSumsectorhi[j]/fNumsectorhi[j]);
          else
              histhi.FillHist(fSumsectorhi[j]/fNumsectorhi[j]);
      }

      if (IsLoGain() && fNumsectorlo[j]>0)
      {
          MHCalibrationPix &histlo = GetAverageLoGainSector(j);
          if (IsOscillations())
              histlo.FillHistAndArray(fSumsectorlo[j]/fNumsectorlo[j]);
          else
              histlo.FillHist(fSumsectorlo[j]/fNumsectorlo[j]);
      }
  }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calls:
// - MHCalibrationCam::FitHiGainArrays() with flags:
//   MBadPixelsPix::kRelTimeNotFitted and MBadPixelsPix::kRelTimeOscillating
// - MHCalibrationCam::FitLoGainArrays() with flags:
//   MBadPixelsPix::kRelTimeNotFitted and MBadPixelsPix::kRelTimeOscillating
// 
Bool_t MHCalibrationRelTimeCam::FinalizeHists()
{

  *fLog << endl;

  const Int_t nareas = fAverageHiGainAreas->GetSize();
  const Int_t nsectors = fAverageHiGainSectors->GetSize();

  TArrayI satarea(nareas);
  TArrayI satsect(nsectors);
  fNumareahi  .Reset(); 
  fNumsectorhi.Reset(); 

  for (Int_t i=0; i<fHiGainArray->GetSize(); i++)
    {
      
      MHCalibrationPix &histhi = (*this)[i];

      if (histhi.IsExcluded())
        continue;

      const Int_t aidx   = (*fGeom)[i].GetAidx();
      const Int_t sector = (*fGeom)[i].GetSector();

      fNumareahi[aidx]++;
      fNumsectorhi[sector]++;

      //
      // Check saturation
      // 
      MCalibrationRelTimePix &pix = (MCalibrationRelTimePix&)(*fCam)[i] ;
      if (histhi.GetSaturated() > fNumHiGainSaturationLimit*histhi.GetHGausHist()->GetEntries())
        {
          pix.SetHiGainSaturation();
          histhi.SetExcluded();
          satarea[aidx]++;
          satsect[sector]++;
        }
      else
        if (IsLoGain())
          (*this)(i).SetExcluded();

      //
      // Check histogram overflow
      // 
      CheckOverflow(histhi);
      if (IsLoGain())
        CheckOverflow((*this)(i));

    }

  for (Int_t j=0; j<nareas; j++)
    {

      MHCalibrationPix     &histhi = GetAverageHiGainArea(j);
      MCalibrationRelTimePix &pix = (MCalibrationRelTimePix&)fCam->GetAverageArea(j);

      if (satarea[j] > 0.5*fNumareahi[j])
        {
          pix.SetHiGainSaturation();
          histhi.SetExcluded();
        }
      else
        if (IsLoGain())
          GetAverageLoGainArea(j).SetExcluded();

      //
      // Check histogram overflow
      // 
      CheckOverflow(histhi);
      if (IsLoGain())
        CheckOverflow(GetAverageLoGainArea(j));
   }
  
  for (Int_t j=0; j<fAverageHiGainSectors->GetSize(); j++)
    {

      MHCalibrationPix &histhi = GetAverageHiGainSector(j);      

      if (satsect[j] > 0.5*fNumsectorhi[j])
        {
          MCalibrationRelTimePix  &pix = (MCalibrationRelTimePix&)fCam->GetAverageSector(j) ;
          pix.SetHiGainSaturation();
          histhi.SetExcluded();
        }
      else
        if (IsLoGain())        
          GetAverageLoGainSector(j).SetExcluded();

      //
      // Check histogram overflow
      // 
      CheckOverflow(histhi);
      if (IsLoGain())
        CheckOverflow(GetAverageLoGainSector(j));
    }

  FitHiGainArrays(*fCam, *fBadPixels,
                  MBadPixelsPix::kRelTimeNotFitted,
                  MBadPixelsPix::kRelTimeOscillating);

  if (IsLoGain())
    FitLoGainArrays(*fCam, *fBadPixels,
                    MBadPixelsPix::kRelTimeNotFitted,
                    MBadPixelsPix::kRelTimeOscillating);

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Sets all pixels to MBadPixelsPix::kUnreliableRun, if following flags are set:
// - MBadPixelsPix::kRelTimeNotFitted
// - MBadPixelsPix::kRelTimeOscillating
//
void MHCalibrationRelTimeCam::FinalizeBadPixels()
{

  for (Int_t i=0; i<fBadPixels->GetSize(); i++)
    {
      MBadPixelsPix &bad = (*fBadPixels)[i];

      if (bad.IsUncalibrated( MBadPixelsPix::kRelTimeNotFitted ))
          bad.SetUnsuitable(   MBadPixelsPix::kUnreliableRun    );

      if (bad.IsUncalibrated( MBadPixelsPix::kRelTimeOscillating))
          bad.SetUnsuitable(   MBadPixelsPix::kUnreliableRun    );

    }
}

// --------------------------------------------------------------------------
//
// The types are as follows:
// 
// Fitted values:
// ============== 
//
// 0: Fitted Mean  Relative Arrival Time in FADC slices (MHGausEvents::GetMean()
// 1: Error  Mean  Relative Arrival Time in FADC slices (MHGausEvents::GetMeanErr()
// 2: Sigma fitted Relative Arrival Time in FADC slices (MHGausEvents::GetSigma()
// 3: Error Sigma  Relative Arrival Time in FADC slices (MHGausEvents::GetSigmaErr()
//
// Useful variables derived from the fit results:
// =============================================
//
// 4: Returned probability of Gauss fit  (MHGausEvents::GetProb())
//
// Localized defects:
// ==================
//
// 5: Gaus fit not OK                    (MHGausEvents::IsGausFitOK())
// 6: Fourier spectrum not OK            (MHGausEvents::IsFourierSpectrumOK())
//
Bool_t MHCalibrationRelTimeCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{

  if (fHiGainArray->GetSize() <= idx)
    return kFALSE;

  const MHCalibrationPix &pix = (*this)[idx];

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
void MHCalibrationRelTimeCam::DrawPixelContent(Int_t idx) const
{
 (*this)[idx].DrawClone();
}

// -----------------------------------------------------------------------------
// 
// Default draw:
//
// Displays the averaged areas, both High Gain and Low Gain 
//
// Calls the Draw of the fAverageHiGainAreas and fAverageLoGainAreas objects with options
//
void MHCalibrationRelTimeCam::Draw(const Option_t *opt)
{

  const Int_t nareas = fAverageHiGainAreas->GetSize();
  if (nareas == 0)
    return;

  TString option(opt);
  option.ToLower();

  if (!option.Contains("datacheck"))
    {
      MHCalibrationCam::Draw(opt);
      return;
    }

  // 
  // From here on , the datacheck - Draw
  //
  TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this);  
  pad->SetBorderMode(0);
  pad->Divide(1,nareas);

  //
  // Loop over inner and outer pixels
  //  
  for (Int_t i=0; i<nareas;i++) 
     {
       
       pad->cd(i+1);
       
       MHCalibrationPix &hipix = GetAverageHiGainArea(i);
       //
       // Ask for Hi-Gain saturation
       //
       if (hipix.IsExcluded() && IsLoGain())
        {
           MHCalibrationPix &lopix = GetAverageLoGainArea(i);
           DrawDataCheckPixel(lopix,i ? fOuterRefTime+1.5 : fInnerRefTime+1.5);
         }
       else
         DrawDataCheckPixel(hipix,i ? fOuterRefTime : fInnerRefTime);
    }      
}

void MHCalibrationRelTimeCam::CheckOverflow( MHCalibrationPix &pix ) const
{
    if (pix.IsExcluded())
        return;

    const TH1F &hist = *pix.GetHGausHist();

    const Int_t   n   = hist.GetNbinsX();
    const Float_t max = fOverflowLimit*hist.GetEntries();

    const Stat_t overflow = hist.GetBinContent(n+1);
    if (overflow > max)
    {
        *fLog << warn << overflow << " overflows above " << hist.GetBinLowEdge(n);
        *fLog << " in " << pix.GetName() << " (w/o saturation!) " << endl;
    }
  
    const Stat_t underflow = hist.GetBinContent(0);
    if (underflow > max)
    {
        *fLog << warn << underflow << " underflows below " << hist.GetBinLowEdge(1);
        *fLog << " in " << pix.GetName() << " (w/o saturation!) " << endl;
    }
}


// -----------------------------------------------------------------------------
// 
// Draw the average pixel for the datacheck:
//
// Displays the averaged areas, both High Gain and Low Gain 
//
// Calls the Draw of the fAverageHiGainAreas and fAverageLoGainAreas objects with options
//
void MHCalibrationRelTimeCam::DrawDataCheckPixel(MHCalibrationPix &pix, const Float_t refline)
{
    if (pix.IsEmpty())
        return;

    TVirtualPad *pad = gPad;
    pad->Divide(1,2, 1e-10, 1e-10);
    pad->cd(1);

    gPad->SetBorderMode(0);

    gPad->SetTicks();
    if (!pix.IsEmpty() && !pix.IsOnlyOverflow() && !pix.IsOnlyUnderflow())
        gPad->SetLogy();

    TH1F *hist = pix.GetHGausHist();
  
    //
    // set the labels bigger
    //
    TAxis *xaxe = hist->GetXaxis();
    TAxis *yaxe = hist->GetYaxis();
    xaxe->CenterTitle();
    yaxe->CenterTitle();
    xaxe->SetTitleSize(0.07);
    yaxe->SetTitleSize(0.07);
    xaxe->SetTitleOffset(0.65);
    yaxe->SetTitleOffset(0.55);
    xaxe->SetLabelSize(0.06);
    yaxe->SetLabelSize(0.06);
    xaxe->SetTitle(hist->GetXaxis()->GetTitle());
    yaxe->SetTitle(hist->GetYaxis()->GetTitle());
    xaxe->SetRange(hist->GetMaximumBin()-1500, hist->GetMaximumBin()+1500);

    hist->Draw();

    gStyle->SetOptFit();

    TF1 *fit = pix.GetFGausFit();

    if (fit)
    {
        switch (fColor)
        {
        case MCalibrationCam::kGREEN:
            fit->SetLineColor(kGreen);
            break;
        case MCalibrationCam::kBLUE:
            fit->SetLineColor(kBlue);
            break;
        case MCalibrationCam::kUV:
            fit->SetLineColor(106);
            break;
        case MCalibrationCam::kCT1:
            fit->SetLineColor(006);
            break;
        default:
            fit->SetLineColor(kRed);
        }
        fit->Draw("same");
    }

    DisplayRefLines(hist, refline);

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetTicks();

    pix.CreateGraphEvents();
    TGraph *gr = pix.GetGraphEvents();
    if (gr)
    {
        TH1F *null2 = gr->GetHistogram();

        null2->SetMinimum(pix.GetMean()-10.*pix.GetSigma());
        null2->SetMaximum(pix.GetMean()+10.*pix.GetSigma());

        //
        // set the labels bigger
        //
        TAxis *xaxe2 = null2->GetXaxis();
        TAxis *yaxe2 = null2->GetYaxis();
        xaxe2->CenterTitle();
        yaxe2->CenterTitle();
        xaxe2->SetTitleSize(0.07);
        yaxe2->SetTitleSize(0.07);
        xaxe2->SetTitleOffset(0.65);
        yaxe2->SetTitleOffset(0.55);
        xaxe2->SetLabelSize(0.06);
        yaxe2->SetLabelSize(0.06);
        xaxe2->SetRangeUser(0.,pix.GetEvents()->GetSize()/pix.GetEventFrequency());
    }

    pix.DrawEvents();
}

void  MHCalibrationRelTimeCam::DisplayRefLines(const TH1F *hist, const Float_t refline) const
{
    TLine *line = new TLine(refline, 0, refline, hist->GetMaximum());
    line->SetLineColor(kGreen);
    line->SetLineStyle(2);
    line->SetLineWidth(3);
    line->SetBit(kCanDelete);
    line->Draw();

    TLegend *leg = new TLegend(0.75,0.01,0.99,0.3);
    leg->SetBit(kCanDelete);
    leg->AddEntry(line, "Trigger Calibration", "l");
    leg->Draw();
}

Int_t MHCalibrationRelTimeCam::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;

    if (IsEnvDefined(env, prefix, "ReferenceFile", print))
    {
        SetReferenceFile(GetEnvValue(env, prefix, "ReferenceFile", fReferenceFile.Data()));
        rc = kTRUE;
    }

    TEnv refenv(fReferenceFile);

    fInnerRefTime = refenv.GetValue("InnerRefTime", fInnerRefTime);
    fOuterRefTime = refenv.GetValue("OuterRefTime", fOuterRefTime);

    return MHCalibrationCam::ReadEnv(env,prefix,print) ? kTRUE : rc;
}
