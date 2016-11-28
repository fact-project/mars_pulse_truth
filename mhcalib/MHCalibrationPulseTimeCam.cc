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
// MHCalibrationPulseTimeCam                                               
//
// Fills the extracted signals of MExtractedSignalCam into the MHCalibrationPix-classes 
// MHCalibrationPulseTimeHiGainPix and MHCalibrationPulseTimeLoGainPix for every:
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
// The PulseTime histograms are fitted to a Gaussian, mean and sigma with its errors 
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
// Unless more than fNumHiGainSaturationLimit (default: 1%) of the overall FADC 
// slices show saturation, the following flag is set:
// - MCalibrationPulseTimePix::SetHiGainSaturation();
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
// The following results are written into MCalibrationPulseTimeCam:
//
// - MCalibrationPix::SetHiGainSaturation() 
// - MCalibrationPix::SetHiGainMean()
// - MCalibrationPix::SetHiGainMeanErr()
// - MCalibrationPix::SetHiGainSigma()
// - MCalibrationPix::SetHiGainSigmaErr()
// - MCalibrationPix::SetHiGainProb()
// - MCalibrationPix::SetHiGainNumPickup()
//
// For all averaged areas, the fitted sigma is multiplied with the square root of 
// the number involved pixels in order to be able to compare it to the average of 
// sigmas in the camera.
//
//
// ClassVersion 2:
// ---------------
//   - Byte_t fSaturationLimit
//   - Byte_t fLowerSignalLimit
//   + UInt_t fSaturationLimit
//   + UInt_t fLowerSignalLimit
//
/////////////////////////////////////////////////////////////////////////////
#include "MHCalibrationPulseTimeCam.h"

#include <TEnv.h>
#include <TLine.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TOrdCollection.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MArrayD.h"
#include "MRawRunHeader.h"
#include "MExtractedSignalCam.h"
#include "MPedestalSubtractedEvt.h"

#include "MCalibrationPix.h"
#include "MHCalibrationPix.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MBadPixelsCam.h"

ClassImp(MHCalibrationPulseTimeCam);

using namespace std;

const UInt_t  MHCalibrationPulseTimeCam::fgSaturationLimit   = 245;
const UInt_t  MHCalibrationPulseTimeCam::fgLowerSignalLimit  =  85;
const Int_t   MHCalibrationPulseTimeCam::fgNumPixelsRequired =   3;
const Int_t   MHCalibrationPulseTimeCam::fgHiGainNbins =  20;
const Axis_t  MHCalibrationPulseTimeCam::fgHiGainFirst =  -0.5;
const Axis_t  MHCalibrationPulseTimeCam::fgHiGainLast  =  19.5;
const Float_t MHCalibrationPulseTimeCam::fgProbLimit   = 0.001;
const TString MHCalibrationPulseTimeCam::gsHistName    = "PulseTime";
const TString MHCalibrationPulseTimeCam::gsHistTitle   = "Extracted Times";
const TString MHCalibrationPulseTimeCam::gsHistXTitle  = "Time [FADC slices]";
const TString MHCalibrationPulseTimeCam::gsHistYTitle  = "Nr. events";
const TString MHCalibrationPulseTimeCam::fgReferenceFile = "mjobs/signalref.rc";

// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Sets:
// - all pointers to NULL
//
// - fNbins to fgHiGainNbins
// - fFirst to fgHiGainFirst
// - fLast  to fgHiGainLast 
//
// - fHistName   to gsHistName  
// - fHistTitle  to gsHistTitle 
// - fHistXTitle to gsHistXTitle
// - fHistYTitle to gsHistYTitle
//
// - fSaturationLimit to fgSaturationLimit
// - fLowerSignalLimit to fgLowerSignalLimit
// - fNumPixelsRequired to fgNumPixelsRequired
//
MHCalibrationPulseTimeCam::MHCalibrationPulseTimeCam(const char *name, const char *title)
    : fSignalCam(NULL), fBadPixels(NULL)
{

  fName  = name  ? name  : "MHCalibrationPulseTimeCam";
  fTitle = title ? title : "Class to fill the extracted pulse times for cosmics ";

  SetBinning(fgHiGainNbins, fgHiGainFirst, fgHiGainLast);

  SetProbLimit(fgProbLimit);

  SetHistName  (gsHistName  .Data());
  SetHistTitle (gsHistTitle .Data());
  SetHistXTitle(gsHistXTitle.Data());
  SetHistYTitle(gsHistYTitle.Data());

  SetReferenceFile();
  SetLoGain(kFALSE);
  SetOscillations(kFALSE);

  SetSaturationLimit();
  SetLowerSignalLimit();
  SetNumPixelsRequired();

  fInnerRefTime = 5.;
  fOuterRefTime = 5.;
}

// --------------------------------------------------------------------------
//
// Creates new MHCalibrationPulseTimeCam only with the averaged areas:
// the rest has to be retrieved directly, e.g. via: 
//     MHCalibrationPulseTimeCam *cam = MParList::FindObject("MHCalibrationPulseTimeCam");
//  -  cam->GetAverageSector(5).DrawClone();
//  -  (*cam)[100].DrawClone()
//
TObject *MHCalibrationPulseTimeCam::Clone(const char *) const
{

  MHCalibrationPulseTimeCam *cam = new MHCalibrationPulseTimeCam();

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

  if (!IsAverageing())
    return cam;

  const Int_t navhi   =  fAverageHiGainAreas->GetSize();

  for (int i=0; i<navhi; i++)
    cam->fAverageHiGainAreas->AddAt(GetAverageHiGainArea(i).Clone(),i);

  return cam;
}

// --------------------------------------------------------------------------
//
// Gets the pointers to:
// - MRawEvtData
//
Bool_t MHCalibrationPulseTimeCam::SetupHists(const MParList *pList)
{

  fBadPixels = (MBadPixelsCam*)pList->FindObject("MBadPixelsCam");
  if (!fBadPixels)
  {
      *fLog << warn << GetDescriptor() << "MBadPixelsCam not found... " << endl;
  }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Gets or creates the pointers to:
// - MExtractedSignalCam
// - MCalibrationPulseTimeCam
// - MBadPixelsCam
//
// Initializes the number of used FADC slices from MExtractedSignalCam 
// into MCalibrationPulseTimeCam and test for changes in that variable
//
// Calls:
// - InitHiGainArrays()
// 
// Sets:
// - fSumhiarea   to nareas 
// - fSumloarea   to nareas 
// - fSumhisector to nsectors 
// - fSumlosector to nsectors 
//
Bool_t MHCalibrationPulseTimeCam::ReInitHists(MParList *pList)
{
  fSignalCam = (MExtractedSignalCam*)pList->FindObject(AddSerialNumber("MExtractedSignalCam"));
  if (!fSignalCam)
  {
      *fLog << err << "MExtractedSignalCam not found... abort." << endl;
      return kFALSE;
  }

  if (!InitCams(pList,"PulseTime"))
    return kFALSE;

  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nsectors = fGeom->GetNumSectors();
  const Int_t nareas   = fGeom->GetNumAreas();

  InitHiGainArrays(npixels,nareas,nsectors);

  return kTRUE;
}

void MHCalibrationPulseTimeCam::InitHiGainArrays(const Int_t npixels, const Int_t nareas, const Int_t nsectors)
{

  const Int_t samples = fRunHeader->GetNumSamples();
  SetBinning(samples, -0.5, samples-0.5);

  if (fHiGainArray->GetSize()==0)
  {
      for (Int_t i=0; i<npixels; i++)
      {
        fHiGainArray->AddAt(new MHCalibrationPix(Form("%sHiGainPix%04d",fHistName.Data(),i),
                                                 Form("%s High Gain Pixel %4d",fHistTitle.Data(),i)),i);

        MHCalibrationPix &pix = (*this)[i];
        pix.SetBinning(fNbins, fFirst, fLast);

        InitHists(pix, (*fBadPixels)[i], i);

	if (fCam)
	  (*fCam)[i].SetPixId(i);
      }
  }

  if (!IsAverageing())
    return;

  if (fAverageHiGainAreas->GetSize()==0)
  {
    for (Int_t j=0; j<nareas; j++)
      {
        fAverageHiGainAreas->AddAt(new MHCalibrationPix(Form("%sHiGainArea%d",fHistName.Data(),j),
                                                        Form("%s High Gain Area Idx %d",fHistTitle.Data(),j)),j);

        MHCalibrationPix &pix = GetAverageHiGainArea(j);
        pix.SetBinning(fNbins, fFirst, fLast);

        InitHists(pix, fCam->GetAverageBadArea(j), j);
      }
  }

  if (fAverageHiGainSectors->GetSize()==0)
    {
      for (Int_t j=0; j<nsectors; j++)
        {
          fAverageHiGainSectors->AddAt(new MHCalibrationPix(Form("%sHiGainSector%02d",fHistName.Data(),j),
                                                            Form("%s High Gain Sector %02d",fHistTitle.Data(),j)),j);
          MHCalibrationPix &pix = GetAverageHiGainSector(j);
          pix.SetBinning(fNbins, fFirst, fLast);

          InitHists(pix, fCam->GetAverageBadSector(j), j);
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
// 1) Fill PulseTimes histograms (MHGausEvents::FillHistAndArray()) with:
// - MExtractedSignalPix::GetExtractedSignalHiGain();
// - MExtractedSignalPix::GetExtractedSignalLoGain();
//
Bool_t MHCalibrationPulseTimeCam::FillHists(const MParContainer *par, const Stat_t w)
{
    MPedestalSubtractedEvt *evt = (MPedestalSubtractedEvt*)par;
    if (!evt)
    {
        *fLog << err << "No argument in MHCalibrationPulseTimeCam::Fill... abort." << endl;
        return kFALSE;
    }

    const UInt_t nareas   = fGeom->GetNumAreas();
    const UInt_t nsectors = fGeom->GetNumSectors();

    MArrayD sumarea(nareas);
    MArrayD sumsector(nsectors);

    fAverageAreaNum.Reset();
    fAverageSectorNum.Reset();

    const Int_t npix = evt->GetNumPixels();
    for (int idx=0; idx<npix; idx++)
    {
        if (fBadPixels)
            if ((*fBadPixels)[idx].IsUnsuitable())
                continue;

        // Check for saturation
        if (evt->GetSaturation(idx, fSaturationLimit*fRunHeader->GetScale())>0)
            continue;

        // Get position of maximum
        Float_t max;
        const Int_t pos = evt->GetMax(idx, fSignalCam->GetFirstUsedSliceHiGain(),
                                      fSignalCam->GetLastUsedSliceHiGain(), max);

        if (max<fLowerSignalLimit)
            continue;

        const Int_t maxpos = pos-1;

        (*this)[idx].FillHist(maxpos);

        const Int_t aidx   = (*fGeom)[idx].GetAidx();
        const Int_t sector = (*fGeom)[idx].GetSector();

        sumarea[aidx]     += maxpos;
        sumsector[sector] += maxpos;

        fAverageAreaNum[aidx]++;
        fAverageSectorNum[sector]++;
    }

    for (UInt_t j=0; j<nareas; j++)
    {
        if (fAverageAreaNum[j]<fNumPixelsRequired)
            continue;

        sumarea[j] /= fAverageAreaNum[j];

        if (IsOscillations())
            GetAverageHiGainArea(j).FillHistAndArray(sumarea[j]);
        else
            GetAverageHiGainArea(j).FillHist(sumarea[j]);
    }

    for (UInt_t j=0; j<nsectors; j++)
    {
        if (fAverageSectorNum[j]<fNumPixelsRequired)
            continue;

        sumsector[j] /= fAverageSectorNum[j];

        if (IsOscillations())
            GetAverageHiGainSector(j).FillHistAndArray(sumsector[j]);
        else
            GetAverageHiGainSector(j).FillHist(sumsector[j]);
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
// 3) Store the absolute arrival times in the MCalibrationPulseTimePix's. If flag 
//    MCalibrationPix::IsHiGainSaturation() is set, the Low-Gain arrival times are stored, 
//    otherwise the Hi-Gain ones.
// 4) Calls to MHCalibrationCam::FitHiGainArrays() and MCalibrationCam::FitLoGainArrays() 
//    with the flags:
//    - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kHiGainNotFitted )
//    - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kLoGainNotFitted )
//    - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kHiGainOscillating )
//    - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::kLoGainOscillating )
//
Bool_t MHCalibrationPulseTimeCam::FinalizeHists()
{

  *fLog << endl;

  //
  // Perform the fitting for the High Gain (done in MHCalibrationCam)
  // 
  for (Int_t i=0; i<fHiGainArray->GetSize(); i++)
    {

      MHCalibrationPix &hist = (*this)[i];
      if (hist.IsExcluded())
        continue;

      CalcHists(hist, (*fCam)[i]);
    }

  if (!IsAverageing())
    return kTRUE;

  for (Int_t j=0; j<fAverageHiGainAreas->GetSize(); j++)
      CalcHists(GetAverageHiGainArea(j), fCam->GetAverageArea(j));

  for (Int_t j=0; j<fAverageHiGainSectors->GetSize(); j++)
      CalcHists(GetAverageHiGainSector(j), fCam->GetAverageSector(j));

  return kTRUE;
}

void MHCalibrationPulseTimeCam::CalcHists(MHCalibrationPix &hist, MCalibrationPix &pix) const
{

  if (hist.IsEmpty())
  {
      *fLog << warn << hist.GetName() << ": Histogram empty." << endl;
      return;
  }
  if (hist.IsOnlyOverflow())
  {
      *fLog << warn << hist.GetName() << ": Histogram contains only overflows." << endl;
      return;
  }
  if (hist.IsOnlyUnderflow())
  {
      *fLog << warn << hist.GetName() << ": Histogram contains only underflows." << endl;
      return;
  }

  hist.BypassFit();

  pix.SetHiGainMean       ( hist.GetMean()      );
  pix.SetHiGainMeanVar    ( hist.GetMeanErr() * hist.GetMeanErr()   );
  pix.SetHiGainRms        ( hist.GetHistRms()   );
  pix.SetHiGainSigma      ( hist.GetSigma()     );
  pix.SetHiGainSigmaVar   ( hist.GetSigmaErr()* hist.GetSigmaErr()  );

  if (IsDebug())
    {
      *fLog << dbginf << GetDescriptor() << ": ID " << GetName() 
            << " "<<pix.GetPixId()
            << " Mean: "         << hist.GetMean    ()
            << " MeanErr: "      << hist.GetMeanErr ()
            << " MeanSigma: "    << hist.GetSigma   ()
            << " MeanSigmaErr: " << hist.GetSigmaErr()
            << " Prob: "         << hist.GetProb    ()
            << endl;
    }
}

Double_t MHCalibrationPulseTimeCam::GetNumEvents() const
{
    return GetAverageAreas()>0 ? GetAverageHiGainArea(0).GetEntries() : 0;
}

// --------------------------------------------------------------------------
//
// Calls MHCalibrationPix::DrawClone() for pixel idx
//
void MHCalibrationPulseTimeCam::DrawPixelContent(Int_t idx) const
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
void MHCalibrationPulseTimeCam::Draw(const Option_t *opt)
{

  const Int_t nareas = fAverageHiGainAreas->GetSize();
  if (nareas == 0)
    return;

  TString option(opt);
  option.ToLower();

  TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this);
  pad->SetBorderMode(0);
  pad->Divide(1,nareas, 1e-10, 1e-10);

  //
  // Loop over inner and outer pixels
  //
  for (Int_t i=0; i<nareas;i++)
     {
       pad->cd(i+1);

       MHCalibrationPix &hipix = GetAverageHiGainArea(i);
       DrawDataCheckPixel(hipix,i ? fOuterRefTime : fInnerRefTime);
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
void MHCalibrationPulseTimeCam::DrawDataCheckPixel(MHCalibrationPix &pix, const Float_t refline)
{
    gPad->SetBorderMode(0);
    gPad->SetTicks();

    TH1F *hist = pix.GetHGausHist();

    //
    // set the labels bigger
    //
    TAxis *xaxe = hist->GetXaxis();
    TAxis *yaxe = hist->GetYaxis();
    xaxe->CenterTitle();
    yaxe->CenterTitle();
    xaxe->SetTitleSize(0.06);
    yaxe->SetTitleSize(0.076);
    xaxe->SetTitleOffset(0.6);
    yaxe->SetTitleOffset(0.65);
    xaxe->SetLabelSize(0.06);
    yaxe->SetLabelSize(0.06);
    xaxe->SetTitle(hist->GetXaxis()->GetTitle());
    yaxe->SetTitle(hist->GetYaxis()->GetTitle());
    xaxe->SetRange(hist->GetMaximumBin()-30, hist->GetMaximumBin()+30);

    hist->Draw();

    DisplayRefLines(hist, refline);
}

void  MHCalibrationPulseTimeCam::DisplayRefLines(const TH1F *hist, const Float_t refline) const
{
    TLine *line = new TLine(refline, 0, refline, hist->GetMaximum());
    line->SetLineColor(106);
    line->SetLineStyle(2);
    line->SetLineWidth(3);
    line->SetBit(kCanDelete);
    line->Draw();

    TLegend *leg = new TLegend(0.8,0.35,0.99,0.65);
    leg->AddEntry(line, "Reference", "l");
    leg->SetBit(kCanDelete);
    leg->Draw();
}

Int_t MHCalibrationPulseTimeCam::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{

  Int_t rc = MHCalibrationCam::ReadEnv(env,prefix,print);
  if (rc==kERROR)
      return kERROR;

  if (IsEnvDefined(env, prefix, "SaturationLimit", print))
    {
      SetSaturationLimit(GetEnvValue(env, prefix, "SaturationLimit", (Int_t)fSaturationLimit));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "LowerSignalLimit", print))
    {
      SetLowerSignalLimit(GetEnvValue(env, prefix, "LowerSignalLimit", (Int_t)fLowerSignalLimit));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "NumPixelsRequired", print))
    {
      SetNumPixelsRequired(GetEnvValue(env,prefix,"NumPixelsRequired",fNumPixelsRequired));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "ReferenceFile", print))
    {
      SetReferenceFile(GetEnvValue(env,prefix,"ReferenceFile",fReferenceFile.Data()));
      rc = kTRUE;
    }

  TEnv refenv(fReferenceFile);

  fInnerRefTime = refenv.GetValue("InnerRefTime",fInnerRefTime);
  fOuterRefTime = refenv.GetValue("OuterRefTime",fOuterRefTime);

  return rc;
}
