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
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//                                                               
// MHCalibrationChargeCam                                               
//
// Fills the extracted signals of MExtractedSignalCam into the MHCalibrationPix-classes 
// MHCalibrationChargeHiGainPix and MHCalibrationChargeLoGainPix for every:
//
// - Pixel, stored in the TOrdCollection's MHCalibrationCam::fHiGainArray and 
//   MHCalibrationCam::fLoGainArray
//
// - Average pixel per AREA index (e.g. inner and outer for the MAGIC camera), 
//   stored in the TOrdCollection's MHCalibrationCam::fAverageHiGainAreas and 
//   MHCalibrationCam::fAverageLoGainAreas
//
// - Average pixel per camera SECTOR (e.g. sectors 1-6 for the MAGIC camera), 
//   stored in the TOrdCollection's MHCalibrationCam::fAverageHiGainSectors and 
//   MHCalibrationCam::fAverageLoGainSectors
// 
// Every signal is taken from MExtractedSignalCam and filled into a histogram and 
// an array, in order to perform a Fourier analysis (see MHGausEvents). 
// The signals are moreover averaged on an event-by-event basis and written into 
// the corresponding average pixels.
//
// Additionally, the (FADC slice) position of the maximum is stored in an Absolute 
// Arrival Time histogram. This histogram serves for a rough cross-check if the 
// signal does not lie at or outside the edges of the extraction window. 
//
// The Charge histograms are fitted to a Gaussian, mean and sigma with its errors 
// and the fit probability are extracted. If none of these values are NaN's and 
// if the probability is bigger than MHGausEvents::fProbLimit (default: 0.5%), 
// the fit is declared valid.
// Otherwise, the fit is repeated within ranges of the previous mean 
// +- MHCalibrationPix::fPickupLimit (default: 5) sigma (see MHCalibrationPix::RepeatFit())
// In case this does not make the fit valid, the histogram means and RMS's are 
// taken directly (see MHCalibrationPix::BypassFit()) and the following flags are set:
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kHiGainNotFitted ) or  
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kLoGainNotFitted ) and 
// - MBadPixelsPix::SetUnsuitable(   MBadPixelsPix::kUnreliableRun   ) 
// 
// Outliers of more than MHCalibrationPix::fPickupLimit (default: 5) sigmas 
// from the mean are counted as Pickup events (stored in MHCalibrationPix::fPickup) 
//
// If more than fNumHiGainSaturationLimit (default: 15%) of the overall FADC
// slices show saturation, the following flag is set:
// - MCalibrationChargePix::SetHiGainSaturation();
// In that case, the calibration constants are derived from the low-gain results.
//
// If more than fNumLoGainSaturationLimit (default: 1%) of the overall 
// low-gain FADC slices saturate, the following flags are set:
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kLoGainSaturation ) and
// - MBadPixelsPix::SetUnsuitable(   MBadPixelsPix::kUnsuitableRun    )
// 
// The class also fills arrays with the signal vs. event number, creates a fourier 
// spectrum and investigates if the projected fourier components follow an exponential 
// distribution. In case that the probability of the exponential fit is less than 
// MHGausEvents::fProbLimit (default: 0.5%), the following flags are set:
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kHiGainOscillating ) or
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kLoGainOscillating ) and
// - MBadPixelsPix::SetUnsuitable(   MBadPixelsPix::kUnreliableRun     )
// 
// This same procedure is performed for the average pixels.
//
// The following results are written into MCalibrationChargeCam:
//
// - MCalibrationPix::SetHiGainSaturation() 
// - MCalibrationPix::SetHiGainMean()
// - MCalibrationPix::SetHiGainMeanErr()
// - MCalibrationPix::SetHiGainSigma()
// - MCalibrationPix::SetHiGainSigmaErr()
// - MCalibrationPix::SetHiGainProb()
// - MCalibrationPix::SetHiGainNumPickup()
//
// - MCalibrationPix::SetLoGainMean()
// - MCalibrationPix::SetLoGainMeanErr()
// - MCalibrationPix::SetLoGainSigma()
// - MCalibrationPix::SetLoGainSigmaErr()
// - MCalibrationPix::SetLoGainProb()
// - MCalibrationPix::SetLoGainNumPickup()
//
// - MCalibrationChargePix::SetAbsTimeMean()
// - MCalibrationChargePix::SetAbsTimeRms()
//
// For all averaged areas, the fitted sigma is multiplied with the square root of 
// the number involved pixels in order to be able to compare it to the average of 
// sigmas in the camera.
//
// Class Version 2:
//  + Float_t fNumLoGainBlackoutLimit; // Rel. amount blackout logain events until pixel is declared unsuitable
//
// Class Version 3:
//  - Byte_t  fFirstHiGain;                              // First used slice High Gain
//  - Byte_t  fLastHiGain;                               // Last used slice High Gain
//  - Byte_t  fFirstLoGain;                              // First used slice Low Gain
//  - Byte_t  fLastLoGain;                               // Last used slice Low Gain
//
/////////////////////////////////////////////////////////////////////////////
#include "MHCalibrationChargeCam.h"

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
#include <TEnv.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MHCalibrationChargePix.h"
#include "MHCalibrationPix.h"

#include "MCalibrationChargeCam.h"
#include "MCalibrationChargePix.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MRawRunHeader.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalSubtractedEvt.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

ClassImp(MHCalibrationChargeCam);

using namespace std;

const Int_t   MHCalibrationChargeCam::fgChargeHiGainNbins =  500;
const Axis_t  MHCalibrationChargeCam::fgChargeHiGainFirst = -98.;
const Axis_t  MHCalibrationChargeCam::fgChargeHiGainLast  =  1902.;
const Int_t   MHCalibrationChargeCam::fgChargeLoGainNbins =  500;
const Axis_t  MHCalibrationChargeCam::fgChargeLoGainFirst = -99.;
const Axis_t  MHCalibrationChargeCam::fgChargeLoGainLast  =  901.;
const Float_t MHCalibrationChargeCam::fgProbLimit         = 0.00000001;
const TString MHCalibrationChargeCam::gsHistName          = "Charge";
const TString MHCalibrationChargeCam::gsHistTitle         = "Signals";
const TString MHCalibrationChargeCam::gsHistXTitle        = "Signal [FADC counts]";
const TString MHCalibrationChargeCam::gsHistYTitle        = "Nr. events";
const TString MHCalibrationChargeCam::gsAbsHistName       = "AbsTime";
const TString MHCalibrationChargeCam::gsAbsHistTitle      = "Abs. Arr. Times";
const TString MHCalibrationChargeCam::gsAbsHistXTitle     = "Time [FADC slices]";
const TString MHCalibrationChargeCam::gsAbsHistYTitle     = "Nr. events";
const Float_t MHCalibrationChargeCam::fgNumHiGainSaturationLimit = 0.15;
const Float_t MHCalibrationChargeCam::fgNumLoGainSaturationLimit = 0.005;
const Float_t MHCalibrationChargeCam::fgNumLoGainBlackoutLimit   = 0.05;
const Float_t MHCalibrationChargeCam::fgLoGainBlackoutLimit      = 3.5;
const Float_t MHCalibrationChargeCam::fgLoGainPickupLimit        = 3.5;
const Float_t MHCalibrationChargeCam::fgTimeLowerLimit           = 1.;
const Float_t MHCalibrationChargeCam::fgTimeUpperLimit           = 3.;
const TString MHCalibrationChargeCam::fgReferenceFile = "mjobs/calibrationref.rc";

// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Sets:
// - all pointers to NULL
//
// Initializes:
// - fNumHiGainSaturationLimit to fgNumHiGainSaturationLimit
// - fNumLoGainSaturationLimit to fgNumLoGainSaturationLimit
// - fTimeLowerLimit           to fgTimeLowerLimit 
// - fTimeUpperLimit           to fgTimeUpperLimit 
// - fNumLoGainBlackoutLimit   to fgNumLoGainBlackoutLimit
//
// - fNbins to fgChargeHiGainNbins
// - fFirst to fgChargeHiGainFirst
// - fLast  to fgChargeHiGainLast 
//
// - fLoGainNbins to fgChargeLoGainNbins
// - fLoGainFirst to fgChargeLoGainFirst
// - fLoGainLast  to fgChargeLoGainLast 
//
// - fHistName   to gsHistName  
// - fHistTitle  to gsHistTitle 
// - fHistXTitle to gsHistXTitle
// - fHistYTitle to gsHistYTitle
//
// - fAbsHistName   to gsAbsHistName  
// - fAbsHistTitle  to gsAbsHistTitle 
// - fAbsHistXTitle to gsAbsHistXTitle
// - fAbsHistYTitle to gsAbsHistYTitle
//
MHCalibrationChargeCam::MHCalibrationChargeCam(const char *name, const char *title)
    : fRawEvt(NULL), fSignal(NULL)
{

  fName  = name  ? name  : "MHCalibrationChargeCam";
  fTitle = title ? title : "Class to fill the calibration histograms ";
  
  SetNumHiGainSaturationLimit(fgNumHiGainSaturationLimit);
  SetNumLoGainSaturationLimit(fgNumLoGainSaturationLimit);

  SetNumLoGainBlackoutLimit  (fgNumLoGainBlackoutLimit);

  SetTimeLowerLimit();
  SetTimeUpperLimit();

  SetBinning(fgChargeHiGainNbins, fgChargeHiGainFirst, fgChargeHiGainLast );
  SetBinningLoGain(fgChargeLoGainNbins, fgChargeLoGainFirst, fgChargeLoGainLast );

  SetProbLimit(fgProbLimit);

  SetHistName  (gsHistName  .Data());
  SetHistTitle (gsHistTitle .Data());
  SetHistXTitle(gsHistXTitle.Data());
  SetHistYTitle(gsHistYTitle.Data());

  SetAbsHistName  (gsAbsHistName  .Data());
  SetAbsHistTitle (gsAbsHistTitle .Data());
  SetAbsHistXTitle(gsAbsHistXTitle.Data());
  SetAbsHistYTitle(gsAbsHistYTitle.Data());

  SetReferenceFile();

  fInnerRefCharge = 278.;
  fOuterRefCharge = 282.;
}

// --------------------------------------------------------------------------
//
// Creates new MHCalibrationChargeCam only with the averaged areas:
// the rest has to be retrieved directly, e.g. via: 
//     MHCalibrationChargeCam *cam = MParList::FindObject("MHCalibrationChargeCam");
//  -  cam->GetAverageSector(5).DrawClone();
//  -  (*cam)[100].DrawClone()
//
TObject *MHCalibrationChargeCam::Clone(const char *) const
{

  MHCalibrationChargeCam *cam = new MHCalibrationChargeCam();

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
  cam->fLoGainNbins            = fLoGainNbins;
  cam->fLoGainFirst            = fLoGainFirst;
  cam->fLoGainLast             = fLoGainLast;
  cam->fReferenceFile          = fReferenceFile;
  cam->fInnerRefCharge         = fInnerRefCharge;
  cam->fOuterRefCharge         = fOuterRefCharge;

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
// Gets the pointers to:
// - MRawEvtData
//
Bool_t MHCalibrationChargeCam::SetupHists(const MParList *pList)
{
    fRawEvt = (MPedestalSubtractedEvt*)pList->FindObject("MPedestalSubtractedEvt");
    if (!fRawEvt)
    {
        *fLog << err << dbginf << "MPedestalSubtractedEvt not found... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Gets or creates the pointers to:
// - MExtractedSignalCam
// - MCalibrationChargeCam
// - MBadPixelsCam
//
// Initializes the number of used FADC slices from MExtractedSignalCam 
// into MCalibrationChargeCam and test for changes in that variable
//
// Calls:
// - InitHiGainArrays()
// - InitLoGainArrays()
// 
// Sets:
// - fSumhiarea   to nareas 
// - fSumloarea   to nareas 
// - fTimehiarea  to nareas 
// - fTimeloarea  to nareas
// - fSumhisector to nsectors 
// - fSumlosector to nsectors 
// - fTimehisector to nsectors 
// - fTimelosector to nsectors
// - fSathiarea   to nareas 
// - fSatloarea   to nareas
// - fSathisector to nsectors 
// - fSatlosector to nsectors
//
Bool_t MHCalibrationChargeCam::ReInitHists(MParList *pList)
{

  fSignal = (MExtractedSignalCam*)pList->FindObject(AddSerialNumber("MExtractedSignalCam"));
  if (!fSignal)
  {
      *fLog << err << "MExtractedSignalCam not found... abort." << endl;
      return kFALSE;
  }

  if (!InitCams(pList,"Charge"))
    return kFALSE;

  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nsectors = fGeom->GetNumSectors();
  const Int_t nareas   = fGeom->GetNumAreas();

  //
  // In case of the intense blue, double the range
  //
  if (fGeom->InheritsFrom("MGeomCamMagic"))
    if ( fColor == MCalibrationCam::kBLUE)
      SetBinning(fLoGainNbins, fLoGainFirst, 2.*fLoGainLast - fLoGainFirst);

  InitHiGainArrays(npixels,nareas,nsectors);
  InitLoGainArrays(npixels,nareas,nsectors);

  fSumhiarea  .Set(nareas); 
  fSumloarea  .Set(nareas); 
  fTimehiarea .Set(nareas); 
  fTimeloarea .Set(nareas);
  fSumhisector.Set(nsectors); 
  fSumlosector.Set(nsectors); 
  fTimehisector.Set(nsectors); 
  fTimelosector.Set(nsectors);

  fSathiarea  .Set(nareas); 
  fSatloarea  .Set(nareas);
  fSathisector.Set(nsectors); 
  fSatlosector.Set(nsectors);

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
//   * Initialize an MHCalibrationChargePix
//   * Set Binning from  fNbins, fFirst and fLast
//   * Set Binning of Abs Times histogram from  fAbsNbins, fAbsFirst and fAbsLast
//   * Set Histgram names and titles from fHistName and fHistTitle
//   * Set Abs Times Histgram names and titles from fAbsHistName and fAbsHistTitle
//   * Set X-axis and Y-axis titles from fHistXTitle and fHistYTitle
//   * Set X-axis and Y-axis titles of Abs Times Histogram from fAbsHistXTitle and fAbsHistYTitle
//   * Call InitHists
//
//
void MHCalibrationChargeCam::InitHiGainArrays(const Int_t npixels, const Int_t nareas, const Int_t nsectors)
{
  
  TH1F *h;

  const Int_t higainsamples = fRunHeader->GetNumSamplesHiGain();

  if (fHiGainArray->GetSize()==0)
  {
      for (Int_t i=0; i<npixels; i++)
      {
        fHiGainArray->AddAt(new MHCalibrationChargePix(Form("%sHiGainPix%04d",fHistName.Data(),i),
                                                       Form("%s High Gain Pixel%04d",fHistTitle.Data(),i)),i);

        MHCalibrationChargePix &pix = (MHCalibrationChargePix&)(*this)[i];

        pix.SetBinning(fNbins, fFirst, fLast);
        pix.SetProbLimit(fProbLimit);

        pix.SetBinningAbsTime(higainsamples, -0.5, higainsamples-0.5);

        InitHists(pix,(*fBadPixels)[i], i);

        h = pix.GetHAbsTime();

        h->SetName (Form("H%sHiGainPix%04d",fAbsHistName.Data(),i));
        h->SetTitle(Form("%s High Gain Pixel %04d",fAbsHistTitle.Data(),i));
        h->SetXTitle(fAbsHistXTitle.Data());
        h->SetYTitle(fAbsHistYTitle.Data());
      }
  }


  if (fAverageHiGainAreas->GetSize()==0)
  {
    for (Int_t j=0; j<nareas; j++)
      {
        fAverageHiGainAreas->AddAt(new MHCalibrationChargePix(Form("%sHiGainArea%d",fHistName.Data(),j),
                                                  Form("%s High Gain Area Idx %d",fHistTitle.Data(),j)),j);

        MHCalibrationChargePix &pix = (MHCalibrationChargePix&)GetAverageHiGainArea(j);

        pix.SetBinning(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas),
                       fFirst, fLast);

        pix.SetBinningAbsTime(higainsamples, -0.5, higainsamples-0.5);

        InitHists(pix, fCam->GetAverageBadArea(j),j);

        h =  pix.GetHAbsTime();

        h->SetName (Form("H%sHiGainArea%d",fAbsHistName.Data(),j));
        h->SetTitle(Form("%s%s%d",fAbsHistTitle.Data(),
                         " averaged on event-by-event basis High Gain Area Idx ",j));
        h->SetXTitle(fAbsHistXTitle.Data());
        h->SetYTitle(fAbsHistYTitle.Data());
      }
  }
  
  if (fAverageHiGainSectors->GetSize()==0)
  {
      for (Int_t j=0; j<nsectors; j++)
        {
	  fAverageHiGainSectors->AddAt(new MHCalibrationChargePix(Form("%sHiGainSector%02d",fHistName.Data(),j),
                                                      Form("%s High Gain Sector %02d",fHistTitle.Data(),j)),j);

          MHCalibrationChargePix &pix = (MHCalibrationChargePix&)GetAverageHiGainSector(j);

          pix.SetBinning(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas),
                         fFirst, fLast);

          pix.SetBinningAbsTime(higainsamples, -0.5, higainsamples-0.5);

          InitHists(pix, fCam->GetAverageBadSector(j),j);

          h =  pix.GetHAbsTime();

          h->SetName (Form("H%sHiGainSector%02d",fAbsHistName.Data(),j));
          h->SetTitle(Form("%s%s%02d",fAbsHistTitle.Data(),
                           " averaged on event-by-event basis High Gain Area Sector ",j));
          h->SetXTitle(fAbsHistXTitle.Data());
          h->SetYTitle(fAbsHistYTitle.Data());
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
//   * Initialize an MHCalibrationChargePix
//   * Set Binning from  fNbins, fFirst and fLast
//   * Set Binning of Abs Times histogram from  fAbsNbins, fAbsFirst and fAbsLast
//   * Set Histgram names and titles from fHistName and fHistTitle
//   * Set Abs Times Histgram names and titles from fAbsHistName and fAbsHistTitle
//   * Set X-axis and Y-axis titles from fHistXTitle and fHistYTitle
//   * Set X-axis and Y-axis titles of Abs Times Histogram from fAbsHistXTitle and fAbsHistYTitle
//   * Call InitHists
//
void MHCalibrationChargeCam::InitLoGainArrays(const Int_t npixels, const Int_t nareas, const Int_t nsectors)
{

  if (!IsLoGain())
    return;


  const Int_t logainsamples = fRunHeader->GetNumSamplesLoGain();

  TH1F *h;

  if (fLoGainArray->GetSize()==0 )
    {
      for (Int_t i=0; i<npixels; i++)
        {
          fLoGainArray->AddAt(new MHCalibrationChargePix(Form("%sLoGainPix%04d",fHistName.Data(),i),
                                       Form("%s Low Gain Pixel %04d",fHistTitle.Data(),i)),i);

          MHCalibrationChargePix &pix = (MHCalibrationChargePix&)(*this)(i);

          pix.SetBinning(fLoGainNbins, fLoGainFirst, fLoGainLast);
          pix.SetProbLimit(fProbLimit);

          pix.SetBinningAbsTime(logainsamples, -0.5, logainsamples-0.5);
          pix.SetPickupLimit(fgLoGainPickupLimit);
          pix.SetBlackoutLimit(fgLoGainBlackoutLimit);

          InitHists(pix,(*fBadPixels)[i], i);

          h = pix.GetHAbsTime();
          
          h->SetName (Form("H%sLoGainPix%04d",fAbsHistName.Data(),i));
          h->SetTitle(Form("%s Low Gain Pixel %04d",fAbsHistTitle.Data(),i));
          h->SetXTitle(fAbsHistXTitle.Data());
          h->SetYTitle(fAbsHistYTitle.Data());
      }
  }

  if (fAverageLoGainAreas->GetSize()==0)
    {
      for (Int_t j=0; j<nareas; j++)
        {
	  fAverageLoGainAreas->AddAt(new MHCalibrationChargePix(Form("%sLoGainArea%d",fHistName.Data(),j),
                                       Form("%s Low Gain Area Idx %d",fHistTitle.Data(),j)),j);

          MHCalibrationChargePix &pix = (MHCalibrationChargePix&)GetAverageLoGainArea(j);

          pix.SetBinning(fLoGainNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas),
                         fLoGainFirst, fLoGainLast);

          pix.SetBinningAbsTime(logainsamples, -0.5, logainsamples-0.5);
          
          InitHists(pix, fCam->GetAverageBadArea(j),j);

          h =  pix.GetHAbsTime();
          
          h->SetName (Form("H%sLoGainArea%02d",fAbsHistName.Data(),j));
          h->SetTitle(Form("%s%s%02d",fAbsHistTitle.Data(),
                           " averaged on event-by-event basis Low Gain Area Idx ",j));
          h->SetXTitle(fAbsHistXTitle.Data());
          h->SetYTitle(fAbsHistYTitle.Data());
        }
    }
  

  if (fAverageLoGainSectors->GetSize()==0 && IsLoGain())
  {
    for (Int_t j=0; j<nsectors; j++)
      {
        fAverageLoGainSectors->AddAt(new MHCalibrationChargePix(Form("%sLoGainSector%02d",fHistName.Data(),j),
                                                        Form("%s Low Gain Sector %02d",fHistTitle.Data(),j)),j);
        
        MHCalibrationChargePix &pix = (MHCalibrationChargePix&)GetAverageLoGainSector(j);
        
        pix.SetBinning(fLoGainNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas),
                       fLoGainFirst, fLoGainLast);
        
        pix.SetBinningAbsTime(logainsamples, -0.5, logainsamples-0.5);
        
        InitHists(pix, fCam->GetAverageBadSector(j),j);

        h =  pix.GetHAbsTime();
        
        h->SetName (Form("H%sLoGainSector%02d",fAbsHistName.Data(),j));
        h->SetTitle(Form("%s%s%02d",fAbsHistTitle.Data(),
                           " averaged on event-by-event basis Low Gain Area Sector ",j));
        h->SetXTitle(fAbsHistXTitle.Data());
        h->SetYTitle(fAbsHistYTitle.Data());
      }
  }
}


// --------------------------------------------------------------------------
//
// Retrieves from MExtractedSignalCam:
// - first used LoGain FADC slice
//
// Retrieves from MGeomCam:
// - number of pixels
// - number of pixel areas
// - number of sectors
//
// For all TOrdCollection's (including the averaged ones), the following steps are performed: 
//
// 1) Fill Charges histograms (MHGausEvents::FillHistAndArray()) with:
// - MExtractedSignalPix::GetExtractedSignalHiGain();
// - MExtractedSignalPix::GetExtractedSignalLoGain();
//
// 2) Set number of saturated slices (MHCalibrationChargePix::AddSaturated()) with:
// - MExtractedSignalPix::IsHiGainSaturated();
// - MExtractedSignalPix::GetNumLoGainSaturated();
//
// 3) Fill AbsTime histograms (MHCalibrationChargePix::FillAbsTime()) with:
// - MRawEvtPixelIter::GetIdxMaxHiGainSample();       
// - MRawEvtPixelIter::GetIdxMaxLoGainSample(first slice);
//
Bool_t MHCalibrationChargeCam::FillHists(const MParContainer *par, const Stat_t w)
{

  MExtractedSignalCam *signal = (MExtractedSignalCam*)par;
  if (!signal)
    {
      *fLog << err << "No argument in MExtractedSignalCam::Fill... abort." << endl;
      return kFALSE;
    }
  
  const UInt_t npixels  = fGeom->GetNumPixels();
  const UInt_t nareas   = fGeom->GetNumAreas();
  const UInt_t nsectors = fGeom->GetNumSectors();

  const Int_t hifirst = fSignal->GetFirstUsedSliceHiGain();
  const Int_t hilast  = fSignal->GetLastUsedSliceHiGain();
  const Int_t lofirst = fSignal->GetFirstUsedSliceLoGain();
  const Int_t lolast  = fSignal->GetLastUsedSliceLoGain();

  fSumhiarea  .Reset(); 
  fSumloarea  .Reset(); 
  fTimehiarea .Reset(); 
  fTimeloarea .Reset();
  fSumhisector.Reset(); 
  fSumlosector.Reset(); 
  fTimehisector.Reset(); 
  fTimelosector.Reset();

  fSathiarea  .Reset(); 
  fSatloarea  .Reset();
  fSathisector.Reset(); 
  fSatlosector.Reset();

  for (UInt_t i=0; i<npixels; i++)
  {
      MHCalibrationChargePix &histhi = (MHCalibrationChargePix&)(*this)[i];
      if (histhi.IsExcluded())
	continue;

      const Int_t aidx   = (*fGeom)[i].GetAidx();
      const Int_t sector = (*fGeom)[i].GetSector();

      const MExtractedSignalPix &pix = (*signal)[i];

      const Int_t sathi = pix.IsHiGainSaturated() ? 1 : 0;
      histhi.AddSaturated(sathi);
      if (sathi)
      {
          fSathiarea[aidx]++;
          fSathisector[sector]++;
      }

      if (pix.IsHiGainValid())
      {
          const Float_t sumhi = pix.GetExtractedSignalHiGain();
          if (IsOscillations())
              histhi.FillHistAndArray(sumhi);
          else
              histhi.FillHist(sumhi);

          fSumhiarea[aidx]     += sumhi;
          fSumhisector[sector] += sumhi;
      }

      if (!IsLoGain())
          continue;

      MHCalibrationChargePix &histlo = (MHCalibrationChargePix&)(*this)(i);

      const Int_t satlo = pix.IsLoGainSaturated() ? 1 : 0;
      histlo.AddSaturated(satlo);
      if (satlo)
      {
          fSatloarea[aidx]++;
          fSatlosector[sector]++;
      }

      /*
       // Previously:
       fSatloarea[aidx]     += pix.GetNumHiGainSaturated();
       fSatlosector[sector] += pix.GetNumHiGainSaturated();
       */

      if (pix.IsLoGainValid())
      {
          const Float_t sumlo = pix.GetExtractedSignalLoGain();

          if (IsOscillations())
              histlo.FillHistAndArray(sumlo);
          else
              histlo.FillHist(sumlo);

          fSumloarea[aidx]     += sumlo;
          fSumlosector[sector] += sumlo;
      }
  }

  const Int_t num = fRawEvt->GetNumPixels();
  const Int_t nhi = fRunHeader->GetNumSamplesHiGain();
  for (int pixid=0; pixid<num; pixid++)
  {
      MHCalibrationChargePix &histhi = (MHCalibrationChargePix&)(*this)[pixid];
      if (histhi.IsExcluded())
         continue;

      const Float_t timehi = fRawEvt->GetMaxPos(pixid, hifirst, hilast)+hifirst;
      histhi.FillAbsTime(timehi);

      const Int_t aidx   = (*fGeom)[pixid].GetAidx();
      const Int_t sector = (*fGeom)[pixid].GetSector();

      fTimehiarea  [aidx]   += timehi;
      fTimehisector[sector] += timehi;

      if (IsLoGain())
        {
          MHCalibrationChargePix &histlo = (MHCalibrationChargePix&)(*this)(pixid);

          const Float_t timelo = fRawEvt->GetMaxPos(pixid, nhi+lofirst, nhi+lolast)+lofirst;
          histlo.FillAbsTime(timelo);

          fTimeloarea[aidx] += timelo;
          fTimelosector[sector] += timelo;
        }
  }

  for (UInt_t j=0; j<nareas; j++)
    {

      const Int_t npix = fAverageAreaNum[j];

      if (npix == 0)
        continue;

      MHCalibrationChargePix &hipix = (MHCalibrationChargePix&)GetAverageHiGainArea(j);

      if (IsOscillations())
        hipix.FillHistAndArray(fSumhiarea [j]/npix);
      else
        hipix.FillHist(fSumhiarea[j]/npix);
      
      hipix.AddSaturated    ((Float_t)fSathiarea [j]/npix > 0.5 ? 1 : 0); 
      hipix.FillAbsTime     (fTimehiarea[j]/npix);

      if (IsLoGain())
        {
          MHCalibrationChargePix &lopix = (MHCalibrationChargePix&)GetAverageLoGainArea(j);
          if (IsOscillations())
            lopix.FillHistAndArray(fSumloarea [j]/npix);
          else
            lopix.FillHist(fSumloarea [j]/npix);
          lopix.AddSaturated    ((Float_t)fSatloarea [j]/npix > 0.5 ? 1 : 0); 
          lopix.FillAbsTime     (fTimeloarea[j]/npix);
        }
    }

  for (UInt_t j=0; j<nsectors; j++)
    {

      const Int_t npix = fAverageSectorNum[j];

      if (npix == 0)
        continue;

      MHCalibrationChargePix &hipix = (MHCalibrationChargePix&)GetAverageHiGainSector(j);

      if (IsOscillations())      
        hipix.FillHistAndArray(fSumhisector [j]/npix);
      else
        hipix.FillHist(fSumhisector [j]/npix);

      hipix.AddSaturated    ((Float_t)fSathisector[j]/npix > 0.5 ? 1 : 0); 
      hipix.FillAbsTime     (fTimehisector[j]/npix);

      if (IsLoGain())
        {
          MHCalibrationChargePix &lopix = (MHCalibrationChargePix&)GetAverageLoGainSector(j);

          if (IsOscillations())      
            lopix.FillHistAndArray(fSumlosector [j]/npix);
          else
            lopix.FillHist(fSumlosector [j]/npix);
            
          lopix.AddSaturated    ((Float_t)fSatlosector[j]/npix > 0.5 ? 1 : 0); 
          lopix.FillAbsTime     (fTimelosector[j]/npix);
        }
    }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// For all TOrdCollection's (including the averaged ones), the following steps are performed: 
//
// 1) Returns if the pixel is excluded.
// 2) Tests saturation. In case yes, set the flag: MCalibrationPix::SetHiGainSaturation()
//    or the flag: MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kLoGainSaturated )
// 3) Store the absolute arrival times in the MCalibrationChargePix's. If flag 
//    MCalibrationPix::IsHiGainSaturation() is set, the Low-Gain arrival times are stored, 
//    otherwise the Hi-Gain ones.
// 4) Calls to MHCalibrationCam::FitHiGainArrays() and MCalibrationCam::FitLoGainArrays() 
//    with the flags:
//    - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kHiGainNotFitted )
//    - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kLoGainNotFitted )
//    - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kHiGainOscillating )
//    - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kLoGainOscillating )
//
Bool_t MHCalibrationChargeCam::FinalizeHists()
{

  *fLog << endl;

  TH1F *h = NULL;

  const Int_t hifirst = fSignal->GetFirstUsedSliceHiGain();
  const Int_t hilast  = fSignal->GetLastUsedSliceHiGain();
  const Int_t lofirst = fSignal->GetFirstUsedSliceLoGain();
  const Int_t lolast  = fSignal->GetLastUsedSliceLoGain();

  for (Int_t i=0; i<fHiGainArray->GetSize(); i++)
    {

      MHCalibrationChargePix &histhi = (MHCalibrationChargePix&)(*this)[i];
      if (histhi.IsExcluded())
	continue;

      MCalibrationChargePix  &pix = (MCalibrationChargePix&)(*fCam)[i];

      const Int_t numsat = histhi.GetSaturated();

      pix.SetNumSaturated(numsat);

      if (numsat > fNumHiGainSaturationLimit*histhi.GetHGausHist()->GetEntries())
        {
          pix.SetHiGainSaturation();
          if (IsOscillations())
            histhi.CreateFourierSpectrum();
          continue;
        }

      MBadPixelsPix &bad = (*fBadPixels)[i];

      h = histhi.GetHGausHist();

      Stat_t overflow = h->GetBinContent(h->GetNbinsX()+1);
      if (overflow > fOverflowLimit*histhi.GetHGausHist()->GetEntries())
        {
          *fLog << warn
                << "HiGain Hist-overflow " << overflow
                << " times in: " << histhi.GetName() << " (w/o saturation!) " << endl;
          bad.SetUncalibrated( MBadPixelsPix::kHiGainOverFlow ); 
        }

      overflow = h->GetBinContent(0);
      if (overflow >  fOverflowLimit*histhi.GetHGausHist()->GetEntries())
        {
          *fLog << warn
                << "HiGain Hist-underflow " << overflow
                << " times in pix: " << histhi.GetName() << " (w/o saturation!) " << endl;
          bad.SetUncalibrated( MBadPixelsPix::kHiGainOverFlow ); 
        }

      FinalizeAbsTimes(histhi, pix, bad, hifirst, hilast);
    }

  if (IsLoGain())
    for (Int_t i=0; i<fLoGainArray->GetSize(); i++)
      {
        
        MHCalibrationChargePix &histlo = (MHCalibrationChargePix&)(*this)(i);
        
        if (histlo.IsExcluded())
	  continue;

        MBadPixelsPix &bad = (*fBadPixels)[i];
        if (histlo.GetSaturated() > fNumLoGainSaturationLimit*histlo.GetHGausHist()->GetEntries())
          {
              *fLog << warn << "Pixel  " << setw(4) << i << ": More than " << Form("%.1f%%", fNumLoGainSaturationLimit*100) << " lo-gains saturated." << endl;
            bad.SetUncalibrated( MBadPixelsPix::kLoGainSaturation ); 
            if (IsOscillations())
              histlo.CreateFourierSpectrum();
            continue;
          }

        MCalibrationChargePix &pix = (MCalibrationChargePix&)(*fCam)[i] ;
        if (!pix.IsHiGainSaturation())
            continue;
        
        h = histlo.GetHGausHist();

        Stat_t overflow = h->GetBinContent(h->GetNbinsX()+1);
        if (overflow >  fOverflowLimit*histlo.GetHGausHist()->GetEntries())
          {
              *fLog << warn
                  << "LoGain Hist-overflow " << overflow
                  << " times in: " << histlo.GetName() << " (w/o saturation!) " << endl;
            bad.SetUncalibrated( MBadPixelsPix::kLoGainOverFlow ); 
          }

        overflow = h->GetBinContent(0);
        if (overflow > fOverflowLimit*histlo.GetHGausHist()->GetEntries())
          {
            *fLog << warn
                  << "LoGain Hist-underflow " << overflow
                  << " times in: " << histlo.GetName() << " (w/o saturation!) " << endl;
            bad.SetUncalibrated( MBadPixelsPix::kLoGainOverFlow ); 
          }
        
        FinalizeAbsTimes(histlo, pix, bad, lofirst, lolast);
      }

  for (Int_t j=0; j<fAverageHiGainAreas->GetSize(); j++)
    {
      
      MHCalibrationChargePix &histhi = (MHCalibrationChargePix&)GetAverageHiGainArea(j);      
      MCalibrationChargePix  &pix    = (MCalibrationChargePix&)fCam->GetAverageArea(j);

      if (histhi.GetSaturated() > fNumHiGainSaturationLimit*histhi.GetHGausHist()->GetEntries())
        {
          pix.SetHiGainSaturation();
          if (IsOscillations())
            histhi.CreateFourierSpectrum();
          continue;
        }

      MBadPixelsPix &bad = fCam->GetAverageBadArea(j);
      FinalizeAbsTimes(histhi, pix, bad, hifirst, hilast);
   }

  if (IsLoGain())
    for (Int_t j=0; j<fAverageLoGainAreas->GetSize(); j++)
      {
        
        MHCalibrationChargePix &histlo = (MHCalibrationChargePix&)GetAverageLoGainArea(j);      

        if (histlo.GetSaturated() > fNumLoGainSaturationLimit*histlo.GetHGausHist()->GetEntries())
          {
              *fLog << warn << "Area   " << setw(4) << j << ": More than " << Form("%.1f%%", fNumLoGainSaturationLimit*100) << " lo-gains saturated." << endl;
            if (IsOscillations())
                histlo.CreateFourierSpectrum();
            continue;
          }
        
        MCalibrationChargePix &pix = (MCalibrationChargePix&)fCam->GetAverageArea(j);
        if (pix.IsHiGainSaturation())
          {
            MBadPixelsPix &bad = fCam->GetAverageBadArea(j);
            FinalizeAbsTimes(histlo, pix, bad, lofirst, lolast);
          }

      }
  
  for (Int_t j=0; j<fAverageHiGainSectors->GetSize(); j++)
    {
      
      MHCalibrationChargePix &histhi = (MHCalibrationChargePix&)GetAverageHiGainSector(j);      
      MCalibrationChargePix  &pix    = (MCalibrationChargePix&)fCam->GetAverageSector(j);

      if (histhi.GetSaturated() > fNumHiGainSaturationLimit*histhi.GetHGausHist()->GetEntries())
        {
          pix.SetHiGainSaturation();
          if (IsOscillations())
            histhi.CreateFourierSpectrum();
          continue;
        }

      MBadPixelsPix &bad = fCam->GetAverageBadSector(j);
      FinalizeAbsTimes(histhi, pix, bad, hifirst, hilast);
    }
  
  if (IsLoGain())
    for (Int_t j=0; j<fAverageLoGainSectors->GetSize(); j++)
      {

        MHCalibrationChargePix &histlo = (MHCalibrationChargePix&)GetAverageLoGainSector(j);      
        MBadPixelsPix          &bad    = fCam->GetAverageBadSector(j);

        if (histlo.GetSaturated() > fNumLoGainSaturationLimit*histlo.GetHGausHist()->GetEntries())
          {
              *fLog << warn << "Sector " << setw(4) << j << ": More than " << Form("%.1f%%", fNumLoGainSaturationLimit*100) << " lo-gains saturated." << endl;
            bad.SetUncalibrated( MBadPixelsPix::kLoGainSaturation ); 
            if (IsOscillations())
              histlo.CreateFourierSpectrum();
            continue;
          }

        MCalibrationChargePix &pix = (MCalibrationChargePix&)fCam->GetAverageSector(j);
        if (pix.IsHiGainSaturation())
          FinalizeAbsTimes(histlo, pix, bad, lofirst, lolast);
      }
  
  //
  // Perform the fitting for the High Gain (done in MHCalibrationCam)
  //
  FitHiGainArrays(*fCam, *fBadPixels,
                  MBadPixelsPix::kHiGainNotFitted,
                  MBadPixelsPix::kHiGainOscillating);
  
  //
  // Perform the fitting for the Low Gain (done in MHCalibrationCam)
  //
  if (IsLoGain())
    FitLoGainArrays(*fCam, *fBadPixels,
                    MBadPixelsPix::kLoGainNotFitted,
                    MBadPixelsPix::kLoGainOscillating);

  //
  // Check for pixels which have the high-gain saturated, but the low-gain
  // switch not applied in sufficient cases. Have to exclude these pixels,
  // although they not abnormal. They simply cannot be calibrated...
  //
  for (Int_t i=0; i<fLoGainArray->GetSize(); i++)
  {

      MHCalibrationChargePix &histlo = (MHCalibrationChargePix&)(*this)(i);
      if (histlo.IsExcluded())
          continue;

      MCalibrationChargePix  &pix = (MCalibrationChargePix&)(*fCam)[i];
      if (!pix.IsHiGainSaturation())
          continue;

      //
      // Now,treat only low-gain calibrated pixels:
      //
      const Double_t lim = fNumLoGainBlackoutLimit*histlo.GetHGausHist()->GetEntries();
      if (histlo.GetBlackout() <= lim)
          continue;

      MBadPixelsPix &bad = (*fBadPixels)[i];
      bad.SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
      bad.SetUncalibrated(MBadPixelsPix::kLoGainBlackout);
  }

  return kTRUE;
}

// --------------------------------------------------------------------------------
//
// Fill the absolute time results into MCalibrationChargePix
//
// Check absolute time validity:
// - Mean arrival time is at least fTimeLowerLimit slices from the lower edge 
// - Mean arrival time is at least fUpperLimit     slices from the upper edge
//
void MHCalibrationChargeCam::FinalizeAbsTimes(MHCalibrationChargePix &hist, MCalibrationChargePix &pix, MBadPixelsPix &bad, 
                                              Int_t first, Int_t last)
{
    const Float_t mean = hist.GetAbsTimeMean();
    const Float_t rms  = hist.GetAbsTimeRms();

    pix.SetAbsTimeMean(mean);
    pix.SetAbsTimeRms(rms);

    const Float_t lowerlimit = (Float_t)first+1;// + fTimeLowerLimit;
    const Float_t upperlimit = (Float_t)last -1;// - fTimeUpperLimit;

    // FIXME: instead of checking whether the maximum is in the first or
    //        last extracted slice we should check whether the extractor
    //        was able to properly extract the signal!!!

    if (mean<lowerlimit)
    {
        *fLog << warn << hist.GetName() << ": Mean Arr.Time: "
            << Form("%4.1f < %4.1f (first used fadc sl+1)", mean, lowerlimit) << endl;
        bad.SetUncalibrated(MBadPixelsPix::kMeanTimeInFirstBin);
    }

    if (mean>upperlimit)
    {
        *fLog << warn << hist.GetName() << ": Mean Arr.Time: "
            << Form("%4.1f > %4.1f (last used fadc sl-1)", mean, upperlimit) << endl;
        bad.SetUncalibrated(MBadPixelsPix::kMeanTimeInLast2Bins);
    }
}

// --------------------------------------------------------------------------
//
// Sets all pixels to MBadPixelsPix::kUnsuitableRun, if following flags are set:
// - MBadPixelsPix::kLoGainSaturation
//
// Sets all pixels to MBadPixelsPix::kUnreliableRun, if following flags are set:
// - if MBadPixelsPix::kHiGainNotFitted   and !MCalibrationPix::IsHiGainSaturation()
// - if MBadPixelsPix::kHiGainOscillating and !MCalibrationPix::IsHiGainSaturation()
// - if MBadPixelsPix::kLoGainNotFitted   and  MCalibrationPix::IsLoGainSaturation()
// - if MBadPixelsPix::kLoGainOscillating and  MCalibrationPix::IsLoGainSaturation()
//
void MHCalibrationChargeCam::FinalizeBadPixels()
{

  for (Int_t i=0; i<fBadPixels->GetSize(); i++)
    {

      MBadPixelsPix &bad = (*fBadPixels)[i];
      MCalibrationPix &pix = (*fCam)[i];

      if (bad.IsUncalibrated(MBadPixelsPix::kHiGainNotFitted))
          if (!pix.IsHiGainSaturation())
              bad.SetUnsuitable(MBadPixelsPix::kUnreliableRun);
 
      if (bad.IsUncalibrated(MBadPixelsPix::kLoGainNotFitted))
          if (pix.IsHiGainSaturation())
              bad.SetUnsuitable(MBadPixelsPix::kUnreliableRun);

      if (bad.IsUncalibrated(MBadPixelsPix::kLoGainSaturation))
          bad.SetUnsuitable(MBadPixelsPix::kUnsuitableRun);

      if (IsOscillations())
        {
            if (bad.IsUncalibrated(MBadPixelsPix::kHiGainOscillating))
                if (!pix.IsHiGainSaturation())
                    bad.SetUnsuitable(MBadPixelsPix::kUnreliableRun);

            if (bad.IsUncalibrated(MBadPixelsPix::kLoGainOscillating))
                if (pix.IsHiGainSaturation())
                    bad.SetUnsuitable(MBadPixelsPix::kUnreliableRun);
        }
    }
}

// --------------------------------------------------------------------------
//
// Calls MHCalibrationPix::DrawClone() for pixel idx
//
void MHCalibrationChargeCam::DrawPixelContent(Int_t idx) const
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
void MHCalibrationChargeCam::Draw(const Option_t *opt)
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

       MHCalibrationChargePix &hipix = (MHCalibrationChargePix&)GetAverageHiGainArea(i);

       //
       // Ask for Hi-Gain saturation
       //
       if (hipix.GetSaturated() > fNumHiGainSaturationLimit*hipix.GetHGausHist()->GetEntries() && IsLoGain())
       {
           if (i>=fAverageLoGainAreas->GetSize())
               continue;

           MHCalibrationChargePix &lopix = (MHCalibrationChargePix&)GetAverageLoGainArea(i);
           DrawDataCheckPixel(lopix,i ? fOuterRefCharge : fInnerRefCharge);
       }
       else
           DrawDataCheckPixel(hipix,i ? fOuterRefCharge : fInnerRefCharge);
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
void MHCalibrationChargeCam::DrawDataCheckPixel(MHCalibrationChargePix &pix, const Float_t refline)
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

    TH1F  *hist = pix.GetHGausHist();
    TAxis *xaxe = hist->GetXaxis();
    TAxis *yaxe = hist->GetYaxis();
    xaxe->CenterTitle();
    yaxe->CenterTitle();
    xaxe->SetTitleSize(0.07);
    yaxe->SetTitleSize(0.07);
    xaxe->SetTitleOffset(0.7);
    yaxe->SetTitleOffset(0.55);
    xaxe->SetLabelSize(0.06);
    yaxe->SetLabelSize(0.06);
    xaxe->SetTitle(hist->GetXaxis()->GetTitle());
    yaxe->SetTitle(hist->GetYaxis()->GetTitle());
    xaxe->SetRange(hist->GetMaximumBin()-50, hist->GetMaximumBin()+50);

    gStyle->SetOptFit();

    hist->Draw();

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
            fit->SetLineColor(51);
            break;
        case MCalibrationCam::kCT1:
            fit->SetLineColor(6);
            break;
        default:
            fit->SetLineColor(kRed);
        }
        fit->Draw("same");
    }

    DisplayRefLines(hist,refline);

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
        null2->SetStats(kFALSE);

        //
        // set the labels bigger
        //
        TAxis *xaxe2 = null2->GetXaxis();
        TAxis *yaxe2 = null2->GetYaxis();
        xaxe2->CenterTitle();
        yaxe2->CenterTitle();
        xaxe2->SetTitleSize(0.07);
        yaxe2->SetTitleSize(0.07);
        xaxe2->SetTitleOffset(0.7);
        yaxe2->SetTitleOffset(0.55);
        xaxe2->SetLabelSize(0.06);
        yaxe2->SetLabelSize(0.06);
        xaxe2->SetRangeUser(0, pix.GetEvents()->GetSize()/pix.GetEventFrequency());
    }
    pix.DrawEvents();

    //  newpad->cd(3);
    //  pix.DrawPowerSpectrum(*newpad,4);
}

void  MHCalibrationChargeCam::DisplayRefLines(const TH1F *hist, const Float_t refline) const
{
    TLine *line = new TLine(refline, 0, refline, hist->GetMaximum());
    line->SetLineColor(51);
    line->SetLineStyle(2);
    line->SetLineWidth(3);
    line->SetBit(kCanDelete);
    line->Draw();

    TLegend *leg = new TLegend(0.8,0.01,0.99,0.15);
    leg->AddEntry(line, "10 Leds UV", "l");
    leg->SetBit(kCanDelete);
    leg->Draw();
}

Int_t MHCalibrationChargeCam::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{

  Int_t rc = MHCalibrationCam::ReadEnv(env,prefix,print);
  if (rc==kERROR)
    return kERROR;

  if (IsEnvDefined(env, prefix, "HiGainNbins", print))
    {
      fNbins = GetEnvValue(env, prefix, "HiGainNbins", fNbins);
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "HiGainFirst", print))
    {
      fFirst = GetEnvValue(env, prefix, "HiGainFirst", fFirst);
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "HiGainLast", print))
    {
      fLast = GetEnvValue(env, prefix, "HiGainLast", fLast);
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "LoGainNbins", print))
    {
      fLoGainNbins = GetEnvValue(env, prefix, "LoGainNbins", fLoGainNbins);
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "LoGainFirst", print))
    {
      fLoGainFirst = GetEnvValue(env, prefix, "LoGainFirst", fLoGainFirst);
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "LoGainLast", print))
    {
      fLoGainLast = GetEnvValue(env, prefix, "LoGainLast", fLoGainLast);
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "TimeLowerLimit", print))
    {
      SetTimeLowerLimit(GetEnvValue(env, prefix, "TimeLowerLimit", fTimeLowerLimit));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "TimeUpperLimit", print))
    {
      SetTimeUpperLimit(GetEnvValue(env, prefix, "TimeUpperLimit", fTimeUpperLimit));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "ReferenceFile", print))
    {
      SetReferenceFile(GetEnvValue(env,prefix,"ReferenceFile",fReferenceFile.Data()));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "NumHiGainSaturationLimit", print))
  {
      SetNumHiGainSaturationLimit(GetEnvValue(env, prefix, "NumHiGainSaturationLimit", fNumHiGainSaturationLimit));
      rc = kTRUE;
  }

  if (IsEnvDefined(env, prefix, "NumLoGainSaturationLimit", print))
  {
      SetNumLoGainSaturationLimit(GetEnvValue(env, prefix, "NumLoGainSaturationLimit", fNumLoGainSaturationLimit));
      rc = kTRUE;
  }

  if (IsEnvDefined(env, prefix, "NumLoGainBlackoutLimit", print))
  {
      SetNumLoGainBlackoutLimit(GetEnvValue(env, prefix, "NumLoGainBlackoutLimit", fNumLoGainBlackoutLimit));
      rc = kTRUE;
  }

  TEnv refenv(fReferenceFile);

  fInnerRefCharge = refenv.GetValue("InnerRefCharge",fInnerRefCharge);
  fOuterRefCharge = refenv.GetValue("OuterRefCharge",fOuterRefCharge);

  return rc;
}
