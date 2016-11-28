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
// MHCalibrationHiLoCam                                                
//                                                                        
// Fills the extracted high-gain low-gain charge ratios of MArrivalTimeCam into 
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
// The histograms are fitted to a Gaussian, mean and sigma with its errors 
// and the fit probability are extracted. If none of these values are NaN's and 
// if the probability is bigger than MHGausEvents::fProbLimit (default: 0.5%), 
// the fit is declared valid.
// Otherwise, the fit is repeated within ranges of the previous mean 
// +- MHCalibrationPix::fPickupLimit (default: 5) sigma (see MHCalibrationPix::RepeatFit())
// In case this does not make the fit valid, the histogram means and RMS's are 
// taken directly (see MHCalibrationPix::BypassFit()) and the following flags are set:
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kHiLoNotFitted ) and
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
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kHiLoOscillating ) and
// - MBadPixelsPix::SetUnsuitable(   MBadPixelsPix::kUnreliableRun      )
// 
// This same procedure is performed for the average pixels.
//
// The following results are written into MCalibrationHiLoCam:
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
#include "MHCalibrationHiLoCam.h"

#include <TOrdCollection.h>
#include <TPad.h>
#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TF1.h>
#include <TLine.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TProfile.h>

#include "MHCalibrationHiLoPix.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MCalibrationHiLoCam.h"
#include "MCalibrationHiLoPix.h"
#include "MCalibrationCam.h"
#include "MCalibrationPix.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"
#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

ClassImp(MHCalibrationHiLoCam);

using namespace std;

const Int_t   MHCalibrationHiLoCam::fgNbins    = 175;
const Axis_t  MHCalibrationHiLoCam::fgFirst    = -5.1;
const Axis_t  MHCalibrationHiLoCam::fgLast     = 29.9;
const Float_t MHCalibrationHiLoCam::fgProbLimit = 0.;
const Int_t   MHCalibrationHiLoCam::fgHivsLoNbins = 90;
const Axis_t  MHCalibrationHiLoCam::fgHivsLoFirst = 95.;
const Axis_t  MHCalibrationHiLoCam::fgHivsLoLast  = 995.;
const Axis_t  MHCalibrationHiLoCam::fgLowerFitLimitProfile = 480.;
const Axis_t  MHCalibrationHiLoCam::fgUpperFitLimitProfile = 680.;
const TString MHCalibrationHiLoCam::gsHistName  = "HiLo";
const TString MHCalibrationHiLoCam::gsHistTitle = "HiGain vs. LoGain";
const TString MHCalibrationHiLoCam::gsHistXTitle = "Amplification Ratio [1]";
const TString MHCalibrationHiLoCam::gsHistYTitle = "Nr. events";
const TString MHCalibrationHiLoCam::gsHivsLoHistName   = "HivsLo";
const TString MHCalibrationHiLoCam::gsHivsLoHistTitle  = "High-gain vs. Low-gain Charge";
const TString MHCalibrationHiLoCam::gsHivsLoHistXTitle = "Q High-Gain [FADC counts]";
const TString MHCalibrationHiLoCam::gsHivsLoHistYTitle = "Q Low-Gain [FADC counts]";

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
// - fLowerLimt  to fgLowerLim
// - fUpperLimt  to fgUpperLim
//
MHCalibrationHiLoCam::MHCalibrationHiLoCam(const char *name, const char *title) 
  : fArrTimes(NULL), fHivsLoResults("Results","Fit Results high-gain vs. low-gain",
				    200,-10.,10.,200,0.,20.),
    fUsedLoGainSlices(0)
{

  fName  = name  ? name  : "MHCalibrationHiLoCam";
  fTitle = title ? title : "Histogram class for the high-gain vs. low-gain amplification ratio calibration";

  SetNbins(fgNbins);
  SetFirst(fgFirst);
  SetLast (fgLast );

  SetProbLimit(fgProbLimit);

  SetHistName  (gsHistName  .Data());
  SetHistTitle (gsHistTitle .Data());
  SetHistXTitle(gsHistXTitle.Data());
  SetHistYTitle(gsHistYTitle.Data());

  SetHivsLoNbins(fgHivsLoNbins);
  SetHivsLoFirst(fgHivsLoFirst);
  SetHivsLoLast (fgHivsLoLast );

  SetLowerFitLimitProfile();
  SetUpperFitLimitProfile();

  SetHivsLoHistName  (gsHivsLoHistName  .Data());
  SetHivsLoHistTitle (gsHivsLoHistTitle .Data());
  SetHivsLoHistXTitle(gsHivsLoHistXTitle.Data());
  SetHivsLoHistYTitle(gsHivsLoHistYTitle.Data());

  SetOscillations(kFALSE);
  
  fHivsLoResults.GetXaxis()->SetTitle("Offset per FADC slices [FADC cnts]");
  fHivsLoResults.GetYaxis()->SetTitle("Gains ratio [1]");
  fHivsLoResults.SetDirectory(0);

}

// --------------------------------------------------------------------------
//
// Creates new MHCalibrationHiLoCam only with the averaged areas:
// the rest has to be retrieved directly, e.g. via: 
//     MHCalibrationHiLoCam *cam = MParList::FindObject("MHCalibrationHiLoCam");
//  -  cam->GetAverageSector(5).DrawClone();
//  -  (*cam)[100].DrawClone()
//
TObject *MHCalibrationHiLoCam::Clone(const char *) const
{

  MHCalibrationHiLoCam *cam = new MHCalibrationHiLoCam();

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
  const Int_t navlo   =  fAverageLoGainAreas->GetSize();

  for (int i=0; i<navhi; i++)
    cam->fAverageHiGainAreas->AddAt(GetAverageHiGainArea(i).Clone(),i);

  for (int i=0; i<navlo; i++)
    cam->fAverageLoGainAreas->AddAt(GetAverageLoGainArea(i).Clone(),i);

  return cam;
}

// --------------------------------------------------------------------------
//
// Gets or creates the pointers to:
// - MCalibrationHiLoCam
//
// Searches pointer to:
// - MExtractedSignalCam
// - MArrivalTimeCam
//
// Calls:
// - MHCalibrationCam::InitHiGainArrays()
// - MHCalibrationCam::InitLoGainArrays()
// 
// Sets:
// - fSumarea   to nareas
// - fSumsector to nareas
// - fNumarea   to nareas
// - fNumsector to nareas
//
Bool_t MHCalibrationHiLoCam::ReInitHists(MParList *pList)
{

  fCam = (MCalibrationCam*)pList->FindObject(AddSerialNumber("MCalibrationHiLoCam"));
  if (!fCam)
    {
      fCam = (MCalibrationCam*)pList->FindCreateObj(AddSerialNumber("MCalibrationHiLoCam"));
      if (!fCam)
	  return kFALSE;
      fCam->Init(*fGeom);
    }

  MExtractedSignalCam *signal = (MExtractedSignalCam*)pList->FindObject("MExtractedSignalCam");
  if (!signal)
  {
      *fLog << err << "MExtractedSignalCam not found... abort." << endl;
      return kFALSE;
  }

  fUsedLoGainSlices = signal->GetNumUsedLoGainFADCSlices();

  fArrTimes = (MArrivalTimeCam*)pList->FindObject("MArrivalTimeCam");
  if (!fArrTimes)
  {
      *fLog << warn << "MArrivalTimeCam not found... cannot calibrated arrival times between "
	    <<"high and low-gain" << endl;
      SetLoGain(kFALSE);
  }

  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nsectors = fGeom->GetNumSectors();
  const Int_t nareas   = fGeom->GetNumAreas();

  InitHiGainArrays(npixels,nareas,nsectors);
  InitLoGainArrays(npixels,nareas,nsectors);

  fSumareahi  .Set(nareas); 
  fSumsectorhi.Set(nsectors); 
  fNumareahi  .Set(nareas); 
  fNumsectorhi.Set(nsectors); 
  if (IsLoGain())
    {
      fSumarealo  .Set(nareas); 
      fSumsectorlo.Set(nsectors); 
      fNumarealo  .Set(nareas); 
      fNumsectorlo.Set(nsectors); 
    }
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Retrieve:
// - fRunHeader->GetNumSamplesHiGain();
//
// Initializes the High Gain Arrays:
//
// - For every entry in the expanded arrays: 
//   * Initialize an MHCalibrationHiLoPix
//   * Set Binning from  fNbins, fFirst and fLast
//   * Set Binning of Abs Times histogram from  fAbsNbins, fAbsFirst and fAbsLast
//   * Set Histgram names and titles from fHistName and fHistTitle
//   * Set Abs Times Histgram names and titles from fAbsHistName and fAbsHistTitle
//   * Set X-axis and Y-axis titles from fHistXTitle and fHistYTitle
//   * Set X-axis and Y-axis titles of Abs Times Histogram from fAbsHistXTitle and fAbsHistYTitle
//   * Call InitHists
//
//
void MHCalibrationHiLoCam::InitHiGainArrays(const Int_t npixels, const Int_t nareas, const Int_t nsectors)
{
  
  TProfile *h;

  if (fHiGainArray->GetSize()==0)
  {
      for (Int_t i=0; i<npixels; i++)
      {
        fHiGainArray->AddAt(new MHCalibrationHiLoPix(Form("%sHiGainPix%04d",fHistName.Data(),i),
                                                       Form("%s High Gain Pixel%04d",fHistTitle.Data(),i)),i);

        MHCalibrationHiLoPix &pix = (MHCalibrationHiLoPix&)(*this)[i];

        pix.SetNbins(fNbins);
        pix.SetFirst(fFirst);
        pix.SetLast (fLast);

        pix.SetProbLimit(fProbLimit);

        pix.SetHivsLoNbins(fHivsLoNbins);
        pix.SetHivsLoFirst(fHivsLoFirst);
        pix.SetHivsLoLast (fHivsLoLast);

        InitHists(pix,(*fBadPixels)[i],i);

	if (fCam)
	  (*fCam)[i].SetPixId(i);

        h = pix.GetHivsLo();

        h->SetName (Form("H%sHiGainPix%04d",fHivsLoHistName.Data(),i));
        h->SetTitle(Form("%s High Gain Pixel %04d",fHivsLoHistTitle.Data(),i));
        h->SetXTitle(fHivsLoHistXTitle.Data());
        h->SetYTitle(fHivsLoHistYTitle.Data());
        h->SetDirectory(0);
      }
  }


  if (fAverageHiGainAreas->GetSize()==0)
  {
    for (Int_t j=0; j<nareas; j++)
      {
        fAverageHiGainAreas->AddAt(new MHCalibrationHiLoPix(Form("%sHiGainArea%d",fHistName.Data(),j),
                                                  Form("%s High Gain Area Idx %d",fHistTitle.Data(),j)),j);
        
        MHCalibrationHiLoPix &pix = (MHCalibrationHiLoPix&)GetAverageHiGainArea(j);
        
        pix.SetNbins(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas));
        pix.SetFirst(fFirst);
        pix.SetLast (fLast);
        
        pix.SetHivsLoNbins(fHivsLoNbins);
        pix.SetHivsLoFirst(fHivsLoFirst);
        pix.SetHivsLoLast (fHivsLoLast);

        InitHists(pix, fCam->GetAverageBadArea(j), j);

	if (fCam)
	  fCam->GetAverageArea(j).SetPixId(j);

        h =  pix.GetHivsLo();
        
        h->SetName (Form("H%sHiGainArea%d",fHivsLoHistName.Data(),j));
        h->SetTitle(Form("%s averaged on event-by-event basis High Gain Area Idx %d",
                         fHivsLoHistTitle.Data(), j));
        h->SetXTitle(fHivsLoHistXTitle.Data());
        h->SetYTitle(fHivsLoHistYTitle.Data());
        h->SetDirectory(0);
      }
  }
  
  if (fAverageHiGainSectors->GetSize()==0)
  {
      for (Int_t j=0; j<nsectors; j++)
        {
	  fAverageHiGainSectors->AddAt(new MHCalibrationHiLoPix(Form("%sHiGainSector%02d",fHistName.Data(),j),
                                                      Form("%s High Gain Sector %02d",fHistTitle.Data(),j)),j);

          MHCalibrationHiLoPix &pix = (MHCalibrationHiLoPix&)GetAverageHiGainSector(j);

          pix.SetNbins(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas));
          pix.SetFirst(fFirst);
          pix.SetLast (fLast);

          pix.SetHivsLoNbins(fHivsLoNbins);
          pix.SetHivsLoFirst(fHivsLoFirst);
          pix.SetHivsLoLast (fHivsLoLast);
          
          InitHists(pix, fCam->GetAverageBadSector(j), j);

	  if (fCam)
	    fCam->GetAverageSector(j).SetPixId(j);

          h =  pix.GetHivsLo();
          
          h->SetName (Form("H%sHiGainSector%02d",fHivsLoHistName.Data(),j));
          h->SetTitle(Form("%s averaged on event-by-event basis High Gain Area Sector %02d",
                           fHivsLoHistTitle.Data(),j));
          h->SetXTitle(fHivsLoHistXTitle.Data());
          h->SetYTitle(fHivsLoHistYTitle.Data());
          h->SetDirectory(0);
      }
  }
}

//--------------------------------------------------------------------------------------
//
// Return, if IsLoGain() is kFALSE 
//
// Retrieve:
// - fRunHeader->GetNumSamplesHiGain();
//
// Initializes the Low Gain Arrays:
//
// - For every entry in the expanded arrays: 
//   * Initialize an MHCalibrationHiLoPix
//   * Set Binning from  fNbins, fFirst and fLast
//   * Set Binning of HivsLo Times histogram from  fHivsLoNbins, fHivsLoFirst and fHivsLoLast
//   * Set Histgram names and titles from fHistName and fHistTitle
//   * Set HivsLo Times Histgram names and titles from fHivsLoHistName and fHivsLoHistTitle
//   * Set X-axis and Y-axis titles from fHistXTitle and fHistYTitle
//   * Set X-axis and Y-axis titles of HivsLo Times Histogram from fHivsLoHistXTitle and fHivsLoHistYTitle
//   * Call InitHists
//
void MHCalibrationHiLoCam::InitLoGainArrays(const Int_t npixels, const Int_t nareas, const Int_t nsectors)
{
  if (!IsLoGain())
    return;

  TProfile *h;

  if (fLoGainArray->GetSize()==0 )
    {
      for (Int_t i=0; i<npixels; i++)
        {
          fLoGainArray->AddAt(new MHCalibrationHiLoPix(Form("%sLoGainPix%04d",fHistName.Data(),i),
                                       Form("%s Low Gain Pixel %04d",fHistTitle.Data(),i)),i);

          MHCalibrationHiLoPix &pix = (MHCalibrationHiLoPix&)(*this)(i);

          pix.SetNbins(fNbins);
          pix.SetFirst(fFirst);
          pix.SetLast (fLast);
          
          pix.SetProbLimit(fProbLimit);

          pix.SetHivsLoNbins(fHivsLoNbins);
          pix.SetHivsLoFirst(fHivsLoFirst);
          pix.SetHivsLoLast (fHivsLoLast );
          
          InitHists(pix,(*fBadPixels)[i],i);

          h = pix.GetHivsLo();
          
          h->SetName (Form("H%sLoGainPix%04d",fHivsLoHistName.Data(),i));
          h->SetTitle(Form("%s Low Gain Pixel %04d",fHivsLoHistTitle.Data(),i));
          h->SetXTitle(fHivsLoHistXTitle.Data());
          h->SetYTitle(fHivsLoHistYTitle.Data());
          h->SetDirectory(0);
      }
  }

  if (fAverageLoGainAreas->GetSize()==0)
    {
      for (Int_t j=0; j<nareas; j++)
        {
	  fAverageLoGainAreas->AddAt(new MHCalibrationHiLoPix(Form("%sLoGainArea%d",fHistName.Data(),j),
                                       Form("%s Low Gain Area Idx %d",fHistTitle.Data(),j)),j);
        
          MHCalibrationHiLoPix &pix = (MHCalibrationHiLoPix&)GetAverageLoGainArea(j);
          
          pix.SetNbins(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas));
          pix.SetFirst(fFirst);
          pix.SetLast (fLast);
          
          pix.SetHivsLoNbins(fHivsLoNbins);
          pix.SetHivsLoFirst(fHivsLoFirst);
          pix.SetHivsLoLast (fHivsLoLast );
          
          InitHists(pix, fCam->GetAverageBadArea(j), j);

          h =  pix.GetHivsLo();
          
          h->SetName (Form("H%sLoGainArea%02d",fHivsLoHistName.Data(),j));
          h->SetTitle(Form("%s%s%02d",fHivsLoHistTitle.Data(),
                           " averaged on event-by-event basis Low Gain Area Idx ",j));
          h->SetXTitle(fHivsLoHistXTitle.Data());
          h->SetYTitle(fHivsLoHistYTitle.Data());
          h->SetDirectory(0);
        }
    }
  

  if (fAverageLoGainSectors->GetSize()==0 && IsLoGain())
  {
    for (Int_t j=0; j<nsectors; j++)
      {
        fAverageLoGainSectors->AddAt(new MHCalibrationHiLoPix(Form("%sLoGainSector%02d",fHistName.Data(),j),
                                                        Form("%s Low Gain Sector %02d",fHistTitle.Data(),j)),j);
        
        MHCalibrationHiLoPix &pix = (MHCalibrationHiLoPix&)GetAverageLoGainSector(j);
        
        pix.SetNbins(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas));
        pix.SetFirst(fFirst);
        pix.SetLast (fLast);
        
        pix.SetHivsLoNbins(fHivsLoNbins);
        pix.SetHivsLoFirst(fHivsLoFirst);
        pix.SetHivsLoLast (fHivsLoLast);
        
        InitHists(pix, fCam->GetAverageBadSector(j), j);

        h =  pix.GetHivsLo();
        
        h->SetName (Form("H%sLoGainSector%02d",fHivsLoHistName.Data(),j));
        h->SetTitle(Form("%s%s%02d",fHivsLoHistTitle.Data(),
                           " averaged on event-by-event basis Low Gain Area Sector ",j));
        h->SetXTitle(fHivsLoHistXTitle.Data());
        h->SetYTitle(fHivsLoHistYTitle.Data());
        h->SetDirectory(0);
      }
  }
}



// -------------------------------------------------------------------------------
//
// Retrieves pointer to MExtractedSignalCam:
//
// Retrieves from MGeomCam:
// - number of pixels
// - number of pixel areas
// - number of sectors
//
// Fills histograms (MHGausEvents::FillHistAndArray()) with:
// - MExtractedSignalPix::GetExtractedSignalHiGain(pixid) / MExtractedSignalPix::GetExtractedSignalLoGain;
//   if the high-gain signal does not show high-gain saturation, but the low-gain
//   has been extracted.
// - MArrivalTimePix::GetArrivalTimeHiGain(pixid) / MArrivalTimePix::GetArrivalTimeLoGain;
//   if the high-gain signal does not show high-gain saturation, but the low-gain
//   has been extracted.
//
Bool_t MHCalibrationHiLoCam::FillHists(const MParContainer *par, const Stat_t w)
{

  MExtractedSignalCam *signal = (MExtractedSignalCam*)par;
  if (!signal)
    {
      gLog << err << "No argument in MExtractedSignal::Fill... abort." << endl;
      return kFALSE;
    }
  
  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nareas   = fGeom->GetNumAreas();
  const Int_t nsectors = fGeom->GetNumSectors();

  fSumareahi  .Reset();
  fSumsectorhi.Reset(); 
  fNumareahi  .Reset(); 
  fNumsectorhi.Reset(); 
  fSumarealo  .Reset();
  fSumsectorlo.Reset(); 
  fNumarealo  .Reset(); 
  fNumsectorlo.Reset(); 

  for (Int_t i=0; i<npixels; i++)
    {
      const MExtractedSignalPix &pix = (*signal)[i];
      const Int_t aidx   = (*fGeom)[i].GetAidx();
      const Int_t sector = (*fGeom)[i].GetSector();

      const Float_t siglo = pix.GetExtractedSignalLoGain();

      //
      // Skip all pixels with:
      // - Saturated high-gain
      // - Not extracted low-gain 
      //   (see MExtractTimeAndCharge::fLoGainSwitch for setting the criteria)
      //
      if (siglo < 0.5 || pix.IsHiGainSaturated())
	continue;

      const Float_t sighi = pix.GetExtractedSignalHiGain();

      //      *fLog << err << sighi << "  " << siglo << endl;
      const Float_t ratio = sighi / siglo;
      
      MHCalibrationHiLoPix &histhi = (MHCalibrationHiLoPix&)(*this)[i];

      histhi.FillHist(ratio);
      histhi.FillHivsLo(sighi,siglo);

      if (IsAverageing())
	{
	  MHCalibrationHiLoPix &histhi2 = (MHCalibrationHiLoPix&)GetAverageHiGainArea(aidx);
	  histhi2.FillHivsLo(sighi,siglo);
	}

      fSumareahi  [aidx]   += ratio;
      fNumareahi  [aidx]   ++;
      fSumsectorhi[sector] += ratio;
      fNumsectorhi[sector] ++;

      if (IsLoGain())
	{
	  const MArrivalTimePix &tix = (*fArrTimes)[i];
	  MHCalibrationPix &histlo = (*this)(i);	  

	  const Float_t diff = tix.GetArrivalTimeLoGain() - tix.GetArrivalTimeHiGain();

	  histlo.FillHist(diff);
	  fSumarealo  [aidx]   += diff;
	  fNumarealo  [aidx]   ++;
	  fSumsectorlo[sector] += diff;
	  fNumsectorlo[sector] ++;
	}
    }

  if (!IsAverageing())
      return kTRUE;
  
  for (Int_t j=0; j<nareas; j++)
    {

      MHCalibrationHiLoPix &histhi = (MHCalibrationHiLoPix&)GetAverageHiGainArea(j);
      
      if (IsOscillations())
	histhi.FillHistAndArray(fNumareahi[j] == 0 ? 0. : fSumareahi[j]/fNumareahi[j]);
      else
	histhi.FillHist(fNumareahi[j] == 0 ? 0. : fSumareahi[j]/fNumareahi[j]);

      if (IsLoGain())
	{
	  MHCalibrationPix &histlo = GetAverageLoGainArea(j);
	  if (IsOscillations())
	    histlo.FillHistAndArray(fNumarealo[j] == 0 ? 0. : fSumarealo[j]/fNumarealo[j]);
	  else
	    histlo.FillHist(fNumarealo[j] == 0 ? 0. : fSumarealo[j]/fNumarealo[j]);
	}
    }
  
  for (Int_t j=0; j<nsectors; j++)
    {
      MHCalibrationPix &hist = GetAverageHiGainSector(j);

      if (IsOscillations())
	hist.FillHistAndArray(fNumsectorhi[j] == 0 ? 0. : fSumsectorhi[j]/fNumsectorhi[j]);
      else
	hist.FillHist(fNumsectorhi[j] == 0 ? 0. : fSumsectorhi[j]/fNumsectorhi[j]);

      if (IsLoGain())
	{

	  MHCalibrationPix &histlo = GetAverageLoGainSector(j);

	  if (IsOscillations())
	    histlo.FillHistAndArray(fNumsectorlo[j] == 0 ? 0. : fSumsectorlo[j]/fNumsectorlo[j]);
	  else
	    histlo.FillHist(fNumsectorlo[j] == 0 ? 0. : fSumsectorlo[j]/fNumsectorlo[j]);
	}
    }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calls:
// - MHCalibrationCam::FitHiGainArrays() with flags:
//   MBadPixelsPix::kHiLoNotFitted and MBadPixelsPix::kHiLoOscillating
// - MHCalibrationCam::FitLoGainArrays() with flags:
//   MBadPixelsPix::kHiLoNotFitted and MBadPixelsPix::kHiLoOscillating
// 
Bool_t MHCalibrationHiLoCam::FinalizeHists()
{

  *fLog << endl;

  MCalibrationCam *hilocam = fCam;

  const Int_t nareas   = fAverageHiGainAreas->GetSize();

  for (Int_t i=0; i<fHiGainArray->GetSize(); i++)
    {
      
      MHCalibrationHiLoPix &hist = (MHCalibrationHiLoPix&)(*this)[i];

      if (hist.IsExcluded())
        continue;
      
      CheckOverflow(hist);

      TProfile *h = hist.GetHivsLo();
      h->Fit("pol1","RQ","",fLowerFitLimitProfile,fUpperFitLimitProfile);

      TF1 *fit = h->GetFunction("pol1");

      const Float_t gainr  = fit->GetParameter(1) > 0.001 
          ? 1./fit->GetParameter(1) : 0.;

      // The number of used slices are just a mean value
      // the real number might change from event to event.
      // (up to 50%!)
      const Float_t offset = fit->GetParameter(0)/fUsedLoGainSlices;

      fHivsLoResults.Fill(offset,gainr);
      
      MCalibrationHiLoPix &pix = (MCalibrationHiLoPix&)(*fCam)[i];
      pix.SetOffsetPerSlice(offset);
      pix.SetGainRatio     (gainr );

    }

  //
  // Check histogram overflow
  //
  if (IsAverageing())
  {
      for (Int_t j=0; j<nareas; j++)
          CheckOverflow(GetAverageHiGainArea(j));

      for (Int_t j=0; j<fAverageHiGainSectors->GetSize(); j++)
          CheckOverflow(GetAverageHiGainSector(j));
  }


  FitHiGainArrays(*hilocam, *fBadPixels,
                  MBadPixelsPix::kHiLoNotFitted,
                  MBadPixelsPix::kHiLoOscillating);
  
  if (!IsLoGain())
    return kTRUE;

  for (Int_t i=0; i<fLoGainArray->GetSize(); i++)
    {
      
      MHCalibrationPix &hist = (*this)(i);

      if (hist.IsExcluded())
        continue;
      
      CheckOverflow(hist);
    }

  if (IsAverageing())
  {
      for (Int_t j=0; j<nareas; j++)
        {
	  
	  MHCalibrationHiLoPix &hist = (MHCalibrationHiLoPix&)GetAverageHiGainArea(j);
	  //
	  // Check histogram overflow
	  // 
	  CheckOverflow(hist);
	  
	  TProfile *h = hist.GetHivsLo();
	  h->Fit("pol1","RQ","",fLowerFitLimitProfile,fUpperFitLimitProfile);

	  TF1 *fit = h->GetFunction("pol1");
	  
	  const Float_t gainr  = fit->GetParameter(1) > 0.001 
	    ? 1./fit->GetParameter(1) : 0.;

          // The number of used slices are just a mean value
          // the real number might change from event to event.
          // (up to 50%!)
	  const Float_t offset = fit->GetParameter(0)/fUsedLoGainSlices;
	  
	  MCalibrationHiLoPix &pix = (MCalibrationHiLoPix&)fCam->GetAverageArea(0);
	  pix.SetOffsetPerSlice(offset);
	  pix.SetGainRatio     (gainr );

        }
      
      for (Int_t j=0; j<fAverageHiGainSectors->GetSize(); j++)
        {
	  
          MHCalibrationHiLoPix &hist = (MHCalibrationHiLoPix&)GetAverageHiGainSector(j);      
          //
          // Check histogram overflow
          // 
          CheckOverflow(hist);

	  TProfile *h = hist.GetHivsLo();
	  h->Fit("pol1","RQ","",fLowerFitLimitProfile,fUpperFitLimitProfile);

	  TF1 *fit = h->GetFunction("pol1");
	  
	  const Float_t gainr  = fit->GetParameter(1) > 0.001 
	    ? 1./fit->GetParameter(1) : 0.;

          // The number of used slices are just a mean value
          // the real number might change from event to event.
          // (up to 50%!)
	  const Float_t offset = fit->GetParameter(0)/fUsedLoGainSlices;
	  
	  MCalibrationHiLoPix &pix = (MCalibrationHiLoPix&)fCam->GetAverageSector(0);
	  pix.SetOffsetPerSlice(offset);
	  pix.SetGainRatio     (gainr );

        }
  }

  FitLoGainArrays(*hilocam, *fBadPixels,
                  MBadPixelsPix::kHiLoNotFitted,
                  MBadPixelsPix::kHiLoOscillating);
  
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Sets all pixels to MBadPixelsPix::kUnreliableRun, if following flags are set:
// - MBadPixelsPix::kHiLoNotFitted
// - MBadPixelsPix::kHiLoOscillating
//
void MHCalibrationHiLoCam::FinalizeBadPixels()
{

  for (Int_t i=0; i<fBadPixels->GetSize(); i++)
    {
      MBadPixelsPix &bad = (*fBadPixels)[i];

      if (bad.IsUncalibrated( MBadPixelsPix::kHiLoNotFitted ))
        bad.SetUnsuitable(   MBadPixelsPix::kUnreliableRun    );

      if (bad.IsUncalibrated( MBadPixelsPix::kHiLoOscillating))
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
// 0: Fitted Mean High-Gain Low-Gain Charge Ratio in FADC slices  (MHGausEvents::GetMean()
// 1: Error Mean High-Gain Low-Gain Charge Ratio in FADC slices   (MHGausEvents::GetMeanErr()
// 2: Sigma fitted High-Gain Low-Gain Charge Ratio in FADC slices (MHGausEvents::GetSigma()
// 3: Error Sigma High-Gain Low-Gain Charge Ratio in FADC slices  (MHGausEvents::GetSigmaErr()
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
Bool_t MHCalibrationHiLoCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{

  if (fHiGainArray->GetSize() <= idx)
    return kFALSE;

  const MHCalibrationPix &pixhi = (*this)[idx];
  const MHCalibrationPix &pixlo = (*this)(idx);

  switch (type)
    {
    case 0:
      val = pixhi.GetMean();
      break;
    case 1:
      val = pixhi.GetMeanErr();
      break;
    case 2:
      val = pixhi.GetSigma();
      break;
    case 3:
      val = pixhi.GetSigmaErr();
      break;
    case 4:
      val = pixhi.GetProb();
      break;
    case 5:
      if (!pixhi.IsGausFitOK())
        val = 1.;
      break;
    case 6:
      if (!pixhi.IsFourierSpectrumOK())
        val = 1.;
      break;
    case 7:
      if (!IsLoGain())
	break;
      val = pixlo.GetMean();
      break;
    case 8:
      if (!IsLoGain())
	break;
      val = pixlo.GetMeanErr();
      break;
    case 9:
      if (!IsLoGain())
	break;
      val = pixlo.GetSigma();
      break;
    case 10:
      if (!IsLoGain())
	break;
      val = pixlo.GetSigmaErr();
      break;
    case 11:
      if (!IsLoGain())
	break;
      val = pixlo.GetProb();
      break;
    case 12:
      if (!IsLoGain())
	break;
      if (!pixlo.IsGausFitOK())
        val = 1.;
      break;
    case 13:
      if (!IsLoGain())
	break;
      if (!pixlo.IsFourierSpectrumOK())
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
void MHCalibrationHiLoCam::DrawPixelContent(Int_t idx) const
{
 (*this)[idx].DrawClone();
}

void MHCalibrationHiLoCam::CheckOverflow( MHCalibrationPix &pix )
{

  if (pix.IsExcluded())
    return;

  TH1F *hist = pix.GetHGausHist();
  
  Stat_t overflow = hist->GetBinContent(hist->GetNbinsX()+1);
  if (overflow > fOverflowLimit*hist->GetEntries())
    {
      *fLog << warn << "Hist-overflow " << overflow
            << " times in " << pix.GetName() << endl;
    }
  
  overflow = hist->GetBinContent(0);
  if (overflow > fOverflowLimit*hist->GetEntries())
    {
      *fLog << warn << "Hist-underflow " << overflow
            << " times in " << pix.GetName()  << endl;
    }
}

// -----------------------------------------------------------------------------
// 
// Default draw:
//
// Displays the averaged areas, both amplification ratio as time difference
//
void MHCalibrationHiLoCam::Draw(const Option_t *opt)
{

  if (!IsAverageing())
    return;

  const Int_t nareas = fAverageHiGainAreas->GetSize();
  if (nareas == 0)
    return;

  TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this);  
  pad->SetBorderMode(0);

  pad->Divide(IsLoGain() ? 2 : 1,nareas);

  for (Int_t i=0; i<nareas;i++) 
    {

      pad->cd(IsLoGain() ? 2*(i+1)-1 : i+1);

      GetAverageHiGainArea(i).Draw(opt);

      if (IsLoGain())
        {
          pad->cd(2*(i+1));

          TH1F *hist = GetAverageLoGainArea(i).GetHGausHist();
          hist->SetXTitle("Extracted Time Difference [FADC sl.]");
          GetAverageLoGainArea(i).Draw(opt);
        }
    }
}

Int_t MHCalibrationHiLoCam::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{

  Bool_t rc = kFALSE;

  if (MHCalibrationCam::ReadEnv(env,prefix,print))
    rc = kTRUE;
   
  if (IsEnvDefined(env, prefix, "LowerFitLimitProfile", print))
    {
      SetLowerFitLimitProfile(GetEnvValue(env, prefix, "LowerFitLimitProfile", fLowerFitLimitProfile));
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "UpperFitLimitProfile", print))
    {
      SetUpperFitLimitProfile(GetEnvValue(env, prefix, "UpperFitLimitProfile", fUpperFitLimitProfile));
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "HivsLoNbins", print))
    {
      SetHivsLoNbins(GetEnvValue(env, prefix, "HivsLoNbins", fHivsLoNbins));
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "HivsLoFirst", print))
    {
      SetHivsLoFirst(GetEnvValue(env, prefix, "HivsLoFirst", fHivsLoFirst));
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "HivsLoLast", print))
    {
      SetHivsLoLast(GetEnvValue(env, prefix, "HivsLoLast", fHivsLoLast));
      rc = kTRUE;
    }
  
  return rc;
}
