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
// MHCalibrationChargeBlindCam                                               
//
// Histogram class for blind pixels in the camera. Incorporates the TObjArray's:
// - fBlindPixelsArray (for calibrated High Gains per pixel)
//
/////////////////////////////////////////////////////////////////////////////
#include "MHCalibrationChargeBlindCam.h"

#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TOrdCollection.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"

#include "MExtractedSignalBlindPixel.h"

#include "MCalibrationBlindPix.h"

#include "MParList.h"

#include "MRawRunHeader.h"

ClassImp(MHCalibrationChargeBlindCam);

using namespace std;
const Int_t     MHCalibrationChargeBlindCam::fgNbins    =  128;
const Axis_t    MHCalibrationChargeBlindCam::fgFirst    = -0.5;
const Axis_t    MHCalibrationChargeBlindCam::fgLast     =  511.5;
const Axis_t    MHCalibrationChargeBlindCam::fgSPheCut  =  20.;
const TString   MHCalibrationChargeBlindCam::gsHistName   = "ChargeBlind";
const TString   MHCalibrationChargeBlindCam::gsHistTitle  = "Signals Blind";
const TString   MHCalibrationChargeBlindCam::gsHistXTitle = "Signal [FADC counts]";
const TString   MHCalibrationChargeBlindCam::gsHistYTitle = "Nr. events";
// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Sets:
// - all pointers to NULL
//
// - fFitFunc to kEPoisson4
// - fNbins   to fgNbins
// - fFirst   to fgFirst
// - fLast    to fgLast 
// - fSPheCut to fgSPheCut
// 
// - fHistName   to gsHistName  
// - fHistTitle  to gsHistTitle 
// - fHistXTitle to gsHistXTitle
// - fHistYTitle to gsHistYTitle
//
// - SetAverageing  (kFALSE);
// - SetLoGain      (kFALSE);
// - SetOscillations(kFALSE);
// - SetSizeCheck   (kFALSE);
//
MHCalibrationChargeBlindCam::MHCalibrationChargeBlindCam(const char *name, const char *title)
    : fRawEvt(NULL)
{

  fName  = name  ? name  : "MHCalibrationChargeBlindCam";
  fTitle = title ? title : "Class to fille the blind pixel histograms";

  SetSPheCut();

  SetBinning(fgNbins, fgFirst, fgLast);

  SetHistName  (gsHistName  .Data());
  SetHistTitle (gsHistTitle .Data());
  SetHistXTitle(gsHistXTitle.Data());
  SetHistYTitle(gsHistYTitle.Data());

  SetAverageing  (kFALSE);
  SetLoGain      (kFALSE);
  SetOscillations(kFALSE);
  SetSizeCheck   (kFALSE);

  SetFitFunc(MHCalibrationChargeBlindPix::kEPoisson4);
}

// --------------------------------------------------------------------------
//
// Gets the pointers to:
// - MRawEvtData
//
Bool_t MHCalibrationChargeBlindCam::SetupHists(const MParList *pList)
{

  fRawEvt = (MRawEvtData*)pList->FindObject("MRawEvtData");
  if (!fRawEvt)
  {
      *fLog << err << dbginf << "MRawEvtData not found... aborting." << endl;
      return kFALSE;
  }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Gets or creates the pointers to:
// - MExtractedSignalBlindPixel
// - MCalibrationChargeCam or MCalibrationBlindCam
//
// Initializes the number of used FADC slices from MExtractedSignalCam 
// into MCalibrationChargeCam and test for changes in that variable
//
// Retrieve:
// - fRunHeader->GetNumSamplesHiGain();
//
// Initializes the High Gain Arrays:
//
// - Expand fHiGainArrays to nblindpixels
// 
// - For every entry in the expanded arrays: 
//   * Initialize an MHCalibrationPix
//   * Set Binning from  fNbins, fFirst and fLast
//   * Set Histgram names and titles from fHistName and fHistTitle
//   * Set X-axis and Y-axis titles from fHistXTitle and fHistYTitle
//   * Call InitHists
//
Bool_t MHCalibrationChargeBlindCam::ReInitHists(MParList *pList)
{

  if (!InitCams(pList,"Blind"))
    return kFALSE;

  MExtractedSignalBlindPixel *signal = 
    (MExtractedSignalBlindPixel*)pList->FindObject(AddSerialNumber("MExtractedSignalBlindPixel"));
  if (!signal)
  {
      *fLog << err << "MExtractedSignalBlindPixel not found... abort." << endl;
      return kFALSE;
  }

  const Int_t nblindpixels = signal->GetNumBlindPixels();
  const Int_t samples      = signal->GetNumFADCSamples();
  const Int_t integ        = signal->IsExtractionType( MExtractBlindPixel::kIntegral );
  
  if (fHiGainArray->GetSize()==0)
  {
      for (Int_t i=0; i<nblindpixels; i++)
      {
        fHiGainArray->AddAt(new MHCalibrationChargeBlindPix(Form("%s%s%d",fHistName.Data(),"Pix",i),
                                                  Form("%s%s%d",fHistTitle.Data()," Pixel ",i)),i);

        MHCalibrationChargeBlindPix &pix = (MHCalibrationChargeBlindPix&)(*this)[i];

        const Axis_t last = integ ? ((fLast+0.5)*samples)-0.5 : fLast;

        pix.SetBinning(fNbins, fFirst, last);
        pix.SetSinglePheCut ( integ ? fSPheCut * samples        : fSPheCut );
        pix.SetFitFunc      ( integ ? MHCalibrationChargeBlindPix::kEPoisson5 : fFitFunc );

        pix.InitBins();

      }
  }
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Resets the histogram titles for each entry in:
// - fHiGainArray
//
void MHCalibrationChargeBlindCam::ResetHistTitles()
{
  
  TH1F *h;

  if (fHiGainArray)
    for (Int_t i=0;i<fHiGainArray->GetSize(); i++)
      {
        MHCalibrationPix &pix = (*this)[i];        
        h = pix.GetHGausHist();
        h->SetName (Form("%s%s%s%d","H",fHistName.Data(),"Pix",i));
        h->SetTitle(Form("%s%s%d%s",fHistTitle.Data()," Pixel ",i," Runs: "));  
        h->SetXTitle(fHistXTitle.Data());
        h->SetYTitle(fHistYTitle.Data());
      }
}

// --------------------------------------------------------------------------
//
// Retrieves from MExtractedSignalBlindPixel:
// - number of blind pixels
//
// Retrieves from MExtractedSignalBlindPixel:
// - number of FADC samples
// - extracted signal 
// - blind Pixel ID
//
// Resizes (if necessary):
// - fASinglePheFADCSlices to sum of HiGain and LoGain samples
// - fAPedestalFADCSlices to sum of HiGain and LoGain samples
//
// Fills the following histograms:
// - MHGausEvents::FillHistAndArray(signal)
//
// Creates MRawEvtPixelIter, jumps to blind pixel ID, 
// fills the vectors fASinglePheFADCSlices and fAPedestalFADCSlices 
// with the full FADC slices, depending on the size of the signal w.r.t. fSinglePheCut
//
Bool_t MHCalibrationChargeBlindCam::FillHists(const MParContainer *par, const Stat_t w)
{

  MExtractedSignalBlindPixel *signal = (MExtractedSignalBlindPixel*)par;
  if (!signal)
    {
      *fLog << err << "No argument in MExtractedSignalBlindCam::Fill... abort." << endl;
      return kFALSE;
    }
  
  const Int_t nblindpixels  = signal->GetNumBlindPixels();
  
  if (GetSize() != nblindpixels)
    {
      gLog << err << "ERROR - Size mismatch... abort." << endl;
      return kFALSE;
    }

  Float_t slices = (Float_t)signal->GetNumFADCSamples();
  
  if (slices == 0.)
    {
      *fLog << err << dbginf
	    << "Number of used signal slices in MExtractedSignalBlindPix "
	    << "is zero  ... abort." 
            << endl;
      return kFALSE;
    }
  
  for (Int_t i=0; i<nblindpixels; i++)
    {

      //
      // Signal extraction and histogram filling
      // If filter has been applied, sig has been set to -1.
      //
      const Float_t sig = signal->GetExtractedSignal(i);
      
      if (sig < -0.5)
        continue;

      MHCalibrationChargeBlindPix &hist = (MHCalibrationChargeBlindPix&)(*this)[i];

      hist.FillHist(sig);
      //
      // In order to study the single-phe posistion, we extract the slices
      //
      const Int_t blindpixIdx = signal->GetBlindPixelIdx(i);
      
      MRawEvtPixelIter pixel(fRawEvt);
      pixel.Jump(blindpixIdx);
/*
      if (sig > fSPheCut)
        hist.FillSinglePheFADCSlices(pixel);
      else
        hist.FillPedestalFADCSlices(pixel);
  */
    }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// For all TObjArray's (including the averaged ones), the following steps are performed: 
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
Bool_t MHCalibrationChargeBlindCam::FinalizeHists()
{

  *fLog << endl;

  for (Int_t i=0; i<fHiGainArray->GetSize(); i++)
    {
      MHCalibrationChargeBlindPix &hist = (MHCalibrationChargeBlindPix&)(*this)[i];
      
      TH1F *h = hist.GetHGausHist();

      switch (fColor)
        {
        case MCalibrationCam::kNONE: 
          break;
        case MCalibrationCam::kBLUE: 
          h->SetTitle( Form("%s%s", h->GetTitle(),"BLUE "));
          break;
        case MCalibrationCam::kGREEN: 
          h->SetTitle( Form("%s%s", h->GetTitle(),"GREEN "));
          break;
        case MCalibrationCam::kUV: 
          h->SetTitle( Form("%s%s", h->GetTitle(),"UV "));
          break;
        case MCalibrationCam::kCT1: 
          h->SetTitle( Form("%s%s", h->GetTitle(),"CT1-Pulser "));
          break;
        }
      
      Stat_t overflow = h->GetBinContent(h->GetNbinsX()+1);
      if (overflow > 0.1)
        {
          *fLog << warn << GetDescriptor()
                << ": Histogram Overflow occurred " << overflow 
                << " times in blind pixel: " << i << endl;
        }

      overflow = h->GetBinContent(0);
      if (overflow > 0.1)
        {
          *fLog << warn << GetDescriptor()
                << ": Histogram Underflow occurred " << overflow
                << " times in blind pixel: " << i << endl;
        }

      FitBlindPixel(hist, (MCalibrationBlindPix&)(*fCam)[i]);
    }

  return kTRUE;
}


// --------------------------------------------------------------------------
//
// Returns kFALSE, if empty
//
// - Creates the fourier spectrum and sets bit MHGausEvents::IsFourierSpectrumOK()
// - Retrieves the pedestals from MExtractedSignalBlindPixel
// - Normalizes fASinglePheFADCSlices and fAPedestalFADCSlices
// - Executes FitPedestal()
// - Executes FitSinglePhe()
// - Retrieves fit results and stores them in MCalibrationBlindPix
// 
void MHCalibrationChargeBlindCam::FitBlindPixel(MHCalibrationChargeBlindPix &hist, MCalibrationBlindPix &pix)
{

  if (hist.IsEmpty())
  {
    *fLog << err << GetDescriptor() << " ID: " << hist.GetName()
	  << " My histogram has not been filled !! " << endl;
      return;
  }

  //hist.FinalizeSinglePheSpectrum();
  
  hist.FitPedestal();

  pix.SetValid(kTRUE);

  if (hist.FitSinglePhe())
    pix.SetSinglePheFitOK();
  else
    pix.SetValid(hist.IsPedestalFitOK());

  pix.SetLambda      ( hist.GetLambda   () );
  pix.SetLambdaVar   ( hist.GetLambdaErr()*hist.GetLambdaErr() );
  pix.SetMu0         ( hist.GetMu0      () );
  pix.SetMu0Err      ( hist.GetMu0Err   () );
  pix.SetMu1         ( hist.GetMu1      () );
  pix.SetMu1Err      ( hist.GetMu1Err   () );
  pix.SetSigma0      ( hist.GetSigma0   () );
  pix.SetSigma0Err   ( hist.GetSigma0Err() );
  pix.SetSigma1      ( hist.GetSigma1   () );
  pix.SetSigma1Err   ( hist.GetSigma1Err() );
  pix.SetProb        ( hist.GetProb     () );

  pix.SetLambdaCheck    ( hist.GetLambdaCheck()    );
  pix.SetLambdaCheckErr ( hist.GetLambdaCheckErr() );
}

// --------------------------------------------------------------------------
//
// This Clone-function has to be different from the MHCalibrationCam 
// Clone function since it does not store and display the averaged values 
// (in fAverageAreas), but the blind pixels stored in fHiGainArray. 
//
// Creates new MHCalibrationChargeBlindCam and
// Clones MHCalibrationChargeBlindPix's individually
//
#if 0
TObject *MHCalibrationChargeBlindCam::Clone(const char *name) const
{

  MHCalibrationChargeBlindCam *cam = new MHCalibrationChargeBlindCam();

  //
  // Copy the data members
  //
  cam->fRunNumbers             = fRunNumbers;
  cam->fPulserFrequency        = fPulserFrequency;
  cam->fFlags                  = fFlags;
  cam->fNbins                  = fNbins;
  cam->fFirst                  = fFirst;
  cam->fLast                   = fLast;
  cam->fFitFunc                = fFitFunc;

  const Int_t nhi = fHiGainArray->GetSize();
  
  for (int i=0; i<nhi; i++)
    cam->fHiGainArray->AddAt((*this)[i].Clone(),i);

  return cam;
}
#endif
// -----------------------------------------------------------------------------
// 
// Default draw:
//
// Displays the averaged areas, both High Gain and Low Gain 
//
// Calls the Draw of the fAverageHiGainAreas and fAverageLoGainAreas objects with options
//
void MHCalibrationChargeBlindCam::Draw(Option_t *opt)
{

  const Int_t size = fHiGainArray->GetSize();

  if (size == 0)
    return;
  
  TString option(opt);
  option.ToLower();

  TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this);  
  pad->SetBorderMode(0);

  switch (size)
    {
    case 1: 
      break;
    case 2:
      pad->Divide(2,1);
      break;
    case 3:
    case 4:
      pad->Divide(2,2);
      break;
    default:
      pad->Divide(size/2+1,size/2+1);
      break;
    }

  for (Int_t i=0; i<size;i++) 
    {
      pad->cd(i+1);
      (*this)[i].Draw(option);
    }

  pad->Modified();
  pad->Update();

}

Int_t MHCalibrationChargeBlindCam::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{

  Bool_t rc = kFALSE;

  if (MHCalibrationCam::ReadEnv(env,prefix,print))
    rc = kTRUE;
  
  if (IsEnvDefined(env, prefix, "SPheCut", print))
    {
      SetSPheCut(GetEnvValue(env, prefix, "SPheCut", fSPheCut));
      rc = kTRUE;
    }
  
  // FIXME: GetEnvValue does not work with enums yet
  /*
  if (IsEnvDefined(env, prefix, "FitFunc", print))
    {
      SetFitFunc((Int_t)GetEnvValue(env, prefix, "FitFunc", fFitFunc));
      rc = kTRUE;
  }
  */
  return rc;
}
