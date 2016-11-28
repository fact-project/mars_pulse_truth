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
// MHCalibrationCam                                               
//
// Base class for camera calibration classes. Incorporates the TOrdCollection's:
// - fHiGainArray (for calibrated High Gains per pixel)
// - fLoGainArray (for calibrated Low Gains per pixel) 
// - fAverageHiGainAreas (for averaged High Gains events per camera area index)
// - fAverageLoGainAreas (for averaged High Gains events per camera area index)
// - fAverageHiGainSectors (for averaged High Gains events per camera sector )
// - fAverageLoGainSectors (for averaged High Gains events per camera sector )
// These TOrdCollection's are called by their default constructors, thus no objects 
// are created, until the derived class does so. 
//
// The corresponding operators: [],() and the operators GetAverageHiGainArea(), 
// GetAverageLoGainArea(), GetAverageHiGainSector() and GetAverageLoGainSector() 
// have to be cast to the corresponding class. It is assumed that all classes 
// dealing with calibration pixels derive from MHCalibrationPix.
//
// The following flag can be set:
// - SetAverageing() for calculating the event-by-event averages.
// - SetDebug()      for debug output.
// - SetLoGain()     for the case that low-gain slices are available, but 
//                       MRawRunHeader::GetNumLoGainSlices() gives still 0.
// - SetCheckSize()  for testing the sizes of the f*Arrays in ReInit() if they 
//                       are coinciding with the equiv. sizes in MGeomCam
//
// The flag kLoGain steers if the low-gain signal is treated at all or not.
// The flag kAverageing steers if the event-by-event averages are treated at all.
//
// Class Version 5:
//  + Double_t fLowerFitLimitHiGain;          // Lower limit for the fit range for the hi-gain hist
//  + Double_t fUpperFitLimitHiGain;          // Upper limit for the fit range for the hi-gain hist
//  + Double_t fLowerFitLimitLoGain;          // Lower limit for the fit range for the lo-gain hist
//  + Double_t fUpperFitLimitLoGain;          // Upper limit for the fit range for the lo-gain hist
//  + Bool_t   fIsHiGainFitRanges;            // Are high-gain fit ranges defined?
//  + Bool_t   fIsLoGainFitRanges;            // Are low-gain fit ranges defined?
//
// Class Version 5:
//  + Int_t   fMaxNumEvts;                    // Max Number of events
//
/////////////////////////////////////////////////////////////////////////////
#include "MHCalibrationCam.h"
#include "MHCalibrationPix.h"

#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TText.h>
#include <TPaveText.h>
#include <TOrdCollection.h>
#include <TROOT.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MCalibrationCam.h"
#include "MCalibrationPix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MParList.h"

#include "MRawRunHeader.h"

ClassImp(MHCalibrationCam);

using namespace std;

const Double_t MHCalibrationCam::fgLowerFitLimitHiGain = 0;
const Double_t MHCalibrationCam::fgUpperFitLimitHiGain = 0;
const Double_t MHCalibrationCam::fgLowerFitLimitLoGain = 0;
const Double_t MHCalibrationCam::fgUpperFitLimitLoGain = 0;

const Int_t   MHCalibrationCam::fgPulserFrequency  = 500;
const Float_t MHCalibrationCam::fgProbLimit        = 0.0001;
const Float_t MHCalibrationCam::fgOverflowLimit    = 0.005;
const Int_t   MHCalibrationCam::fgMaxNumEvts       = 4096;

const TString MHCalibrationCam::gsHistName   = "Hist";
const TString MHCalibrationCam::gsHistTitle  = "";
const TString MHCalibrationCam::gsHistXTitle = "";
const TString MHCalibrationCam::gsHistYTitle = "Nr. events";

// --------------------------------------------------------------------------
//
// Default Constructor. 
//
// Sets:
// - all pointers to NULL
//
// Initializes and sets owner of:
// - fHiGainArray, fLoGainArray
// - fAverageHiGainAreas, fAverageLoGainAreas
// - fAverageHiGainSectors, fAverageLoGainSectors
//
// Initializes:
// - fPulserFrequency to fgPulserFrequency
// - fProbLimit       to fgProbLimit
// - fOverflowLimit   to fgOverflowLimit
//
// - SetAveregeing  (kTRUE);
// - SetDebug       (kFALSE);
// - SetLoGain      (kTRUE);
//-  SetOscillations(kTRUE);
//-  SetSizeCheck   (kTRUE);
//-  SetInterlaced  (kFALSE);
//-  SetLowerFitLimitHiGain();
//-  SetUpperFitLimitHiGain();
//-  SetLowerFitLimitLoGain();
//-  SetUpperFitLimitLoGain();
//
MHCalibrationCam::MHCalibrationCam(const char *name, const char *title)
    :  fIsHiGainFitRanges(kFALSE), fIsLoGainFitRanges(kFALSE),
    fHistName(gsHistName),fHistTitle(gsHistTitle),
    fHistXTitle(gsHistXTitle),fHistYTitle(gsHistYTitle),
    fCurrentNumEvts(0), fColor(MCalibrationCam::kNONE),
    fBadPixels(NULL), fCam(NULL), fGeom(NULL),
    fRunHeader(NULL)
{

    fHiGainArray = new TOrdCollection;
    fHiGainArray->SetOwner();

    fLoGainArray = new TOrdCollection;
    fLoGainArray->SetOwner();

    fAverageHiGainAreas = new TOrdCollection;
    fAverageHiGainAreas->SetOwner();

    fAverageLoGainAreas = new TOrdCollection;
    fAverageLoGainAreas->SetOwner();

    fAverageHiGainSectors = new TOrdCollection;
    fAverageHiGainSectors->SetOwner();

    fAverageLoGainSectors = new TOrdCollection;
    fAverageLoGainSectors->SetOwner();

    SetPulserFrequency();
    SetProbLimit();
    SetOverflowLimit();
    SetMaxNumEvts();

    SetAverageing  (kTRUE);
    SetDebug       (kFALSE);
    SetLoGain      (kTRUE);
    SetOscillations(kTRUE);
    SetSizeCheck   (kTRUE);
    SetIsReset     (kTRUE);

    SetLowerFitLimitHiGain();
    SetUpperFitLimitHiGain();
    SetLowerFitLimitLoGain();
    SetUpperFitLimitLoGain();
}

// --------------------------------------------------------------------------
//
// Deletes the TOrdCollection of:
// - fHiGainArray, fLoGainArray
// - fAverageHiGainAreas, fAverageLoGainAreas
// - fAverageHiGainSectors, fAverageLoGainSectors
//
MHCalibrationCam::~MHCalibrationCam()
{

  delete fHiGainArray;
  delete fLoGainArray;

  delete fAverageHiGainAreas;
  delete fAverageLoGainAreas;

  delete fAverageHiGainSectors;
  delete fAverageLoGainSectors;
  /*

  Remove ( fHiGainArray );
  Remove ( fLoGainArray );

  Remove ( fAverageHiGainAreas );
  Remove ( fAverageLoGainAreas );

  Remove ( fAverageHiGainSectors );
  Remove ( fAverageLoGainSectors );
  */

}

Bool_t MHCalibrationCam::IsLoGain() const
{
    if (!fRunHeader)
        return TESTBIT(fFlags,kLoGain);
    return TESTBIT(fFlags,kLoGain) && fRunHeader->GetNumSamplesLoGain()>0;
}

void MHCalibrationCam::Remove(TOrdCollection *col)
{

  if (!col)
    return;

  TOrdCollectionIter Next(col);
  
  Int_t count = 0;

  while (MHCalibrationPix *obj = (MHCalibrationPix*)Next())
    {
      *fLog << ++count << " " << obj << flush;
      if (obj && obj->IsOnHeap())
        {
          obj->Draw();
          delete obj;
        }
    }
  
  delete col;
}

// --------------------------------------------------------------------------
//
// Returns size of fHiGainArray
//
const Int_t MHCalibrationCam::GetSize() const
{
  return fHiGainArray->GetSize();
}

// --------------------------------------------------------------------------
//
// Get i-th High Gain pixel (pixel number)
//
MHCalibrationPix &MHCalibrationCam::operator[](UInt_t i)
{
  return *static_cast<MHCalibrationPix*>(fHiGainArray->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th High Gain pixel (pixel number)
//
const MHCalibrationPix &MHCalibrationCam::operator[](UInt_t i) const
{
  return *static_cast<MHCalibrationPix*>(fHiGainArray->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th Low Gain pixel (pixel number)
//
MHCalibrationPix  &MHCalibrationCam::operator()(UInt_t i)
{
  return *static_cast<MHCalibrationPix*>(fLoGainArray->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th Low Gain pixel (pixel number)
//
const MHCalibrationPix  &MHCalibrationCam::operator()(UInt_t i) const
{
  return *static_cast<MHCalibrationPix*>(fLoGainArray->At(i));
}

// --------------------------------------------------------------------------
//
// Returns the current size of the TOrdCollection fAverageHiGainAreas
// independently if the MHCalibrationPix is filled with values or not.
//
const Int_t MHCalibrationCam::GetAverageAreas() const
{
  return fAverageHiGainAreas->GetSize();
}

// --------------------------------------------------------------------------
//
// Get i-th High Gain pixel Area (area number)
//
MHCalibrationPix  &MHCalibrationCam::GetAverageHiGainArea(UInt_t i)
{
  return *static_cast<MHCalibrationPix*>(fAverageHiGainAreas->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th High Gain pixel Area (area number)
//
const MHCalibrationPix  &MHCalibrationCam::GetAverageHiGainArea(UInt_t i) const
{
  return *static_cast<MHCalibrationPix *>(fAverageHiGainAreas->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th Low Gain pixel Area (area number)
//
MHCalibrationPix  &MHCalibrationCam::GetAverageLoGainArea(UInt_t i)
{
  return *static_cast<MHCalibrationPix*>(fAverageLoGainAreas->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th Low Gain pixel Area (area number)
//
const MHCalibrationPix  &MHCalibrationCam::GetAverageLoGainArea(UInt_t i) const
{
  return *static_cast<MHCalibrationPix*>(fAverageLoGainAreas->At(i));
}

// --------------------------------------------------------------------------
//
// Returns the current size of the TOrdCollection fAverageHiGainSectors
// independently if the MHCalibrationPix is filled with values or not.
//
const Int_t MHCalibrationCam::GetAverageSectors() const
{
  return fAverageHiGainSectors->GetSize();
}

// --------------------------------------------------------------------------
//
// Get i-th High Gain Sector (sector number)
//
MHCalibrationPix  &MHCalibrationCam::GetAverageHiGainSector(UInt_t i)
{
  return *static_cast<MHCalibrationPix*>(fAverageHiGainSectors->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th High Gain Sector (sector number)
//
const MHCalibrationPix  &MHCalibrationCam::GetAverageHiGainSector(UInt_t i) const
{
  return *static_cast<MHCalibrationPix*>(fAverageHiGainSectors->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th Low Gain Sector (sector number)
//
MHCalibrationPix  &MHCalibrationCam::GetAverageLoGainSector(UInt_t i)
{
  return *static_cast<MHCalibrationPix*>(fAverageLoGainSectors->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th Low Gain Sector (sector number)
//
const MHCalibrationPix  &MHCalibrationCam::GetAverageLoGainSector(UInt_t i) const
{
  return *static_cast<MHCalibrationPix*>(fAverageLoGainSectors->At(i));
}

// --------------------------------------------------------------------------
//
// Calls ResetHistTitles()
//
// Calls Reset() for each entry in:
// - fHiGainArray, fLoGainArray
// - fAverageHiGainAreas, fAverageLoGainAreas
// - fAverageHiGainSectors, fAverageLoGainSectors
//
void MHCalibrationCam::ResetHists()
{
  SetIsReset();

  fCurrentNumEvts = 0;
  
  ResetHistTitles();

  if (fHiGainArray)
    { fHiGainArray->R__FOR_EACH(MHCalibrationPix,Reset)();  }

  if (IsAverageing())
    {
      if (fAverageHiGainAreas)
        { fAverageHiGainAreas->R__FOR_EACH(MHCalibrationPix,Reset)();  }
      if (fAverageHiGainSectors)
        { fAverageHiGainSectors->R__FOR_EACH(MHCalibrationPix,Reset)();  }
    }

  if (!IsLoGain())
    return;

  if (fLoGainArray)
    { fLoGainArray->R__FOR_EACH(MHCalibrationPix,Reset)();  }
  if (IsAverageing())
    {
      if (fAverageLoGainAreas)
        { fAverageLoGainAreas->R__FOR_EACH(MHCalibrationPix,Reset)();  }
      if (fAverageLoGainSectors)
        { fAverageLoGainSectors->R__FOR_EACH(MHCalibrationPix,Reset)();  }
    }
}

// --------------------------------------------------------------------------
//
// Resets the histogram titles for each entry in:
// - fHiGainArray, fLoGainArray
// - fAverageHiGainAreas, fAverageLoGainAreas
// - fAverageHiGainSectors, fAverageLoGainSectors
//
void MHCalibrationCam::ResetHistTitles()
{
  
  TH1F *h;

  if (fHiGainArray)
    for (Int_t i=0;i<fHiGainArray->GetSize(); i++)
      {
        MHCalibrationPix &pix = (*this)[i];        
        h = pix.GetHGausHist();
        h->SetName (Form("H%sHiGainPix%04d",fHistName.Data(),i));
        h->SetTitle(Form("%s High Gain Pixel %04d Runs: ",fHistTitle.Data(),i));
        h->SetXTitle(fHistXTitle.Data());
        h->SetYTitle(fHistYTitle.Data());
      }
  
  if (IsAverageing())
    {
      if (fAverageHiGainAreas)
        for (Int_t j=0; j<fAverageHiGainAreas->GetSize(); j++)
          {
            MHCalibrationPix &pix = GetAverageHiGainArea(j);        
            h = pix.GetHGausHist();
            h->SetName (Form("H%sHiGainArea%d",fHistName.Data(),j));
            h->SetXTitle(fHistXTitle.Data());
            h->SetYTitle(fHistYTitle.Data());
            if (fGeom->InheritsFrom("MGeomCamMagic"))
                h->SetTitle(Form("%s averaged on event-by-event basis %s High Gain Runs: ",
                                 fHistTitle.Data(), j==0 ? "Inner Pixels" : "Outer Pixels"));
            else
                h->SetTitle(Form("%s averaged on event-by-event basis High Gain Area Idx %d Runs: ",
                                 fHistTitle.Data(), j));
          }
      
      if (fAverageHiGainSectors)
        for (Int_t j=0; j<fAverageHiGainSectors->GetSize(); j++)
          {
            MHCalibrationPix &pix = GetAverageHiGainSector(j);        
            h = pix.GetHGausHist();
            h->SetName (Form("H%sHiGainSector%02d",fHistName.Data(),j));
            h->SetTitle(Form("%s averaged on event-by-event basis High Gain Sector %02d Runs: ",
                             fHistTitle.Data(), j));
            h->SetXTitle(fHistXTitle.Data());
            h->SetYTitle(fHistYTitle.Data());
          }
    }
  
  if (!IsLoGain())
    return;
  
  if (fLoGainArray)
    for (Int_t i=0;i<fLoGainArray->GetSize(); i++)
      {
        MHCalibrationPix &pix = (*this)(i);        
        h = pix.GetHGausHist();
        h->SetName (Form("H%sLoGainPix%04d",fHistName.Data(),i));
        h->SetTitle(Form("%s Low Gain Pixel %04d Runs: ", fHistTitle.Data(),i));
        h->SetXTitle(fHistXTitle.Data());
        h->SetYTitle(fHistYTitle.Data());
      }
  
  if (IsAverageing())
    {
      if (fAverageLoGainAreas)
        for (Int_t j=0; j<fAverageLoGainAreas->GetSize(); j++)
          {
            MHCalibrationPix &pix = GetAverageLoGainArea(j);        
            h = pix.GetHGausHist();
            h->SetName (Form("H%sLoGainArea%d", fHistName.Data(), j));
            h->SetXTitle(fHistXTitle.Data());
            h->SetYTitle(fHistYTitle.Data());
            if (fGeom->InheritsFrom("MGeomCamMagic"))
                h->SetTitle(Form("%s averaged on event-by-event basis %s Low Gain Runs: ",
                                 fHistTitle.Data(), j==0 ? "Inner Pixels" : "Outer Pixels"));
            else
                h->SetTitle(Form("%s averaged on event-by-event basis Low Gain Area Idx %d Runs: ",
                                 fHistTitle.Data(), j));
          }
      
      if (fAverageLoGainSectors)
        for (Int_t j=0; j<fAverageLoGainSectors->GetSize(); j++)
          {
            MHCalibrationPix &pix = GetAverageLoGainSector(j);        
            h = pix.GetHGausHist();
            h->SetName (Form("H%sLoGainSector%02d",fHistName.Data(),j));
            h->SetTitle(Form("%s averaged on event-by-event basis Low Gain Sector %02d Runs: ",
                             fHistTitle.Data(), j));
            h->SetXTitle(fHistXTitle.Data());
            h->SetYTitle(fHistYTitle.Data());
          }
    }
}

// --------------------------------------------------------------------------
//
// Gets the pointers to:
// - MGeomCam
//
// Calls SetupHists(const MParList *pList)
//
// Calls Delete-Function of:
// - MHCalibrationCam::fHiGainArray, MHCalibrationCam::fLoGainArray
// - MHCalibrationCam::fAverageHiGainAreas, MHCalibrationCam::fAverageLoGainAreas
// - MHCalibrationCam::fAverageHiGainSectors, MHCalibrationCam::fAverageLoGainSectors
//
Bool_t MHCalibrationCam::SetupFill(const MParList *pList)
{
  
  fGeom = (MGeomCam*)pList->FindObject("MGeomCam");
  if (!fGeom)
  {
      *fLog << err << GetDescriptor() 
            << ": MGeomCam not found... aborting." << endl;
      return kFALSE;
  }

  fRunHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
  if (!fRunHeader)
  {
    *fLog << warn << GetDescriptor() 
          << ": MRawRunHeader not found... will not store run numbers." << endl;
  }

  fCurrentNumEvts = 0;

  return SetupHists(pList);
}


// --------------------------------------------------------------------------
//
// Searches MRawEvtHeader to find the correct pulser colour
//
// Gets or creates the pointers to:
// - MBadPixelsCam
//
// Searches pointer to:
// - MArrivalTimeCam
//
// Initializes, if empty to MArrivalTimeCam::GetSize() for:
// - MHCalibrationCam::fHiGainArray, MHCalibrationCam::fLoGainArray
//
// Initializes, if empty to MGeomCam::GetNumAreas() for:
// - MHCalibrationCam::fAverageHiGainAreas, MHCalibrationCam::fAverageLoGainAreas
//
// Initializes, if empty to MGeomCam::GetNumSectors() for:
// - MHCalibrationCam::fAverageHiGainSectors, MHCalibrationCam::fAverageLoGainSectors
// 
// Initializes TArray's to MGeomCam::GetNumAreas and MGeomCam::GetNumSectors, respectively
// Fills with number of valid pixels (if !MBadPixelsPix::IsBad()):
// - MHCalibrationCam::fAverageAreaNum[area index]
// - MHCalibrationCam::fAverageSectorNum[area index]
//
// Calls InitializeHists() for every entry in:
// - MHCalibrationCam::fHiGainArray
// - MHCalibrationCam::fAverageHiGainAreas
// - MHCalibrationCam::fAverageHiGainSectors
//
// Sets Titles and Names for the Histograms 
// - MHCalibrationCam::fAverageHiGainAreas
// - MHCalibrationCam::fAverageHiGainSectors
// 
// Retrieves the run numbers from MRawRunHeader and stores them in fRunNumbers
//
Bool_t MHCalibrationCam::ReInit(MParList *pList)
{

  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nsectors = fGeom->GetNumSectors();
  const Int_t nareas   = fGeom->GetNumAreas();

  fBadPixels = (MBadPixelsCam*)pList->FindObject("MBadPixelsCam");
  if (!fBadPixels)
  {
      /*
      fBadPixels = (MBadPixelsCam*)pList->FindCreateObj(AddSerialNumber("MBadPixelsCam"));
      if (!fBadPixels)
          return kFALSE;

          fBadPixels->InitSize(npixels);
          */
      *fLog << err << "MBadPixelsCam not found... abort." << endl;
      return kFALSE;
  }

  if (IsAverageing())
    {
      //
      // The function TArrayF::Set() already sets all entries to 0.
      //
      fAverageAreaNum.        Set(nareas);
      fAverageAreaSat.        Set(nareas);           
      fAverageAreaSigma.      Set(nareas);      
      fAverageAreaSigmaVar.   Set(nareas);   
      fAverageAreaRelSigma.   Set(nareas);   
      fAverageAreaRelSigmaVar.Set(nareas);
      fAverageSectorNum.      Set(nsectors);
      
      for (Int_t aidx=0; aidx<nareas; aidx++)
        fAverageAreaNum[aidx] = 0;
      
      for (Int_t sector=0; sector<nsectors; sector++)
        fAverageSectorNum[sector] = 0;
  
      for (Int_t i=0; i<npixels; i++)
        {

          if ((*fBadPixels)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
            continue;
          
          fAverageAreaNum  [(*fGeom)[i].GetAidx()  ]++;
          fAverageSectorNum[(*fGeom)[i].GetSector()]++;
        }
    }

  //
  // Because ReInit has been called, a new run number is added
  //
  fRunNumbers.Set(fRunNumbers.GetSize()+1);

  if (fRunHeader)
    {
      fRunNumbers[fRunNumbers.GetSize()-1] = fRunHeader->GetRunNumber();
      if (IsLoGain())
        SetLoGain(fRunHeader->GetNumSamplesLoGain());
    }

  fCurrentNumEvts = 0;

  if (!ReInitHists(pList))
    return kFALSE;

  ResetHistTitles();

  if (!fRunHeader)
    return kTRUE;
  
  for (Int_t i=0; i<fHiGainArray->GetSize(); i++)
    {
      TH1F *h = (*this)[i].GetHGausHist();
      h->SetTitle( Form("%s%08d ", h->GetTitle(),fRunNumbers[fRunNumbers.GetSize()-1]));
    }

  if (IsLoGain())
    for (Int_t i=0; i<fLoGainArray->GetSize(); i++)
      {
        TH1F *h = (*this)(i).GetHGausHist();
        h->SetTitle( Form("%s%08d ", h->GetTitle(),fRunNumbers[fRunNumbers.GetSize()-1]));
      }
  
  if (!IsAverageing())
    return kTRUE;
  
  for (Int_t j=0; j<nareas; j++)
    {
      TH1F *h = GetAverageHiGainArea(j).GetHGausHist();
      h->SetTitle( Form("%s%08d ", h->GetTitle(),fRunNumbers[fRunNumbers.GetSize()-1]));
    }
  
  if (IsLoGain())
    for (Int_t j=0; j<nareas; j++)
      {
        TH1F *h = GetAverageLoGainArea(j).GetHGausHist();
        h->SetTitle( Form("%s%08d ", h->GetTitle(),fRunNumbers[fRunNumbers.GetSize()-1]));
      }
  
  for (Int_t j=0; j<nsectors; j++)
    {
      TH1F *h = GetAverageHiGainSector(j).GetHGausHist();
      h->SetTitle( Form("%s%08d ", h->GetTitle(),fRunNumbers[fRunNumbers.GetSize()-1]));
    }
  
  if (IsLoGain())
    for (Int_t j=0; j<nsectors; j++)
      {
        TH1F *h = GetAverageLoGainSector(j).GetHGausHist();
        h->SetTitle( Form("%s%08d ", h->GetTitle(),fRunNumbers[fRunNumbers.GetSize()-1]));
      }
  
  return kTRUE;
}

//--------------------------------------------------------------------------------------
//
// Initializes the High Gain Arrays:
// 
// - For every entry in the expanded arrays: 
//   * Initialize an MHCalibrationPix
//   * Set Binning from  fNbins, fFirst and fLast
//   * Set Histgram names and titles from fHistName and fHistTitle
//   * Set X-axis and Y-axis titles with fHistXTitle and fHistYTitle
//   * Call InitHists
//
void MHCalibrationCam::InitHiGainArrays(const Int_t npixels, const Int_t nareas, const Int_t nsectors)
{

  if (fHiGainArray->GetSize()==0)
  {
      for (Int_t i=0; i<npixels; i++)
      {
        fHiGainArray->AddAt(new MHCalibrationPix(Form("%sHiGainPix%04d",fHistName.Data(),i),
                                                 Form("%s High Gain Pixel %4d",fHistTitle.Data(),i)),i);
        
        MHCalibrationPix &pix = (*this)[i];          
        pix.SetBinning(fNbins, fFirst, fLast);
        pix.SetProbLimit(fProbLimit);
        
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

        pix.SetBinning(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas), fFirst, fLast);

        if (fGeom && fGeom->InheritsFrom("MGeomCamMagic"))
          {
            pix.InitBins();
            pix.SetEventFrequency(fPulserFrequency);
          }
        else
          InitHists(pix, fCam->GetAverageBadArea(j),j);
      }
  }

  if (fAverageHiGainSectors->GetSize()==0)
    {
      for (Int_t j=0; j<nsectors; j++)
        {
          fAverageHiGainSectors->AddAt(new MHCalibrationPix(Form("%sHiGainSector%02d",fHistName.Data(),j),
                                                            Form("%s High Gain Sector %02d",fHistTitle.Data(),j)),j);
          MHCalibrationPix &pix = GetAverageHiGainSector(j);

          pix.SetBinning(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nsectors), fFirst, fLast);

          InitHists(pix, fCam->GetAverageBadSector(j),j);
      }
  }
}

//--------------------------------------------------------------------------------------
//
// Return, if IsLoGain() is kFALSE 
//
// Initializes the Low Gain Arrays:
//
// - For every entry in the expanded arrays: 
//   * Initialize an MHCalibrationPix
//   * Set Binning from  fNbins, fFirst and fLast
//   * Set Histgram names and titles from fHistName and fHistTitle
//   * Set X-axis and Y-axis titles with fHistXTitle and fHistYTitle
//   * Call InitHists
//
void MHCalibrationCam::InitLoGainArrays(const Int_t npixels, const Int_t nareas, const Int_t nsectors)
{

  if (!IsLoGain())
    return;

  if (fLoGainArray->GetSize()==0)
  {
      for (Int_t i=0; i<npixels; i++)
      {
        fLoGainArray->AddAt(new MHCalibrationPix(Form("%sLoGainPix%04d",fHistName.Data(),i),
                                                 Form("%s Low Gain Pixel%04d",fHistTitle.Data(),i)),i);
        
        MHCalibrationPix &pix = (*this)(i);          
        pix.SetBinning(fNbins, fFirst, fLast);
        pix.SetProbLimit(fProbLimit);
        
        InitHists(pix, (*fBadPixels)[i], i);
      }
  }

  if (!IsAverageing())
    return;

  if (fAverageLoGainAreas->GetSize()==0)
  {
    for (Int_t j=0; j<nareas; j++)
      {
        fAverageLoGainAreas->AddAt(new MHCalibrationPix(Form("%sLoGainArea%d",fHistName.Data(),j),
                                                        Form("%s Low Gain Area Idx %d",fHistTitle.Data(),j)),j);
        
        MHCalibrationPix &pix = GetAverageLoGainArea(j);
        
        pix.SetBinning(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas), fFirst, fLast);
        
        if (fGeom && fGeom->InheritsFrom("MGeomCamMagic"))
          {
            pix.InitBins();
            pix.SetEventFrequency(fPulserFrequency);
          }
        else
          InitHists(pix, fCam->GetAverageBadArea(j),j);
      }
  }

  if (fAverageLoGainSectors->GetSize()==0)
    {
      for (Int_t j=0; j<nsectors; j++)
        {
          fAverageLoGainSectors->AddAt(new MHCalibrationPix(Form("%sLoGainSector%02d",fHistName.Data(),j),
                                                            Form("%s Low Gain Sector %02d",fHistTitle.Data(),j)),j);
          MHCalibrationPix &pix = GetAverageLoGainSector(j);

          pix.SetBinning(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nsectors), fFirst, fLast);
          
          InitHists(pix, fCam->GetAverageBadSector(j),j);
      }
  }
}

//--------------------------------------------------------------------------------
//
// Retrieves from MGeomCam:
// - number of pixels
// - number of pixel areas
// - number of sectors
//
// Return kFALSE, if sizes of the TOrdCollections do not match npixels, nareas or nsectors
// 
// Call FillHists()
//
Int_t MHCalibrationCam::Fill(const MParContainer *par, const Stat_t w)
{
  if (fCurrentNumEvts >= fMaxNumEvts)
    return kTRUE;

  fCurrentNumEvts++;

  SetIsReset(kFALSE);

  if (!IsSizeCheck())
  {
      const Bool_t rc = FillHists(par,w);
      return rc ? kTRUE : kERROR;
  }

  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nareas   = fGeom->GetNumAreas();
  const Int_t nsectors = fGeom->GetNumSectors();
  
  //
  // Hi-Gain OrdCollections
  //
  if (fHiGainArray->GetSize() != npixels)
    {
      *fLog << err << "ERROR - Size mismatch in number of pixels... abort." << endl;
      return kERROR;
    }
  
  if (IsLoGain())
    {
      if (fLoGainArray->GetSize() != npixels)
        {
          *fLog << err << "ERROR - Size mismatch in number of pixels... abort." << endl;
          return kERROR;
        }
    }
  
  if (!IsAverageing())
    return FillHists(par,w);

  if (fAverageHiGainAreas->GetSize() != nareas)
    {
      *fLog << err << "ERROR - Size mismatch in number of areas ... abort." << endl;
      return kERROR;
    }
  
  if (fAverageHiGainSectors->GetSize() != nsectors)
    {
      *fLog << err << "ERROR - Size mismatch in number of sectors ... abort." << endl;
      return kERROR;
    }
  
  if (IsLoGain())
    {
      
      if (fAverageLoGainAreas->GetSize() != nareas)
        {
          *fLog << err << "ERROR - Size mismatch in number of areas ... abort." << endl;
          return kERROR;
        }
      
      if (fAverageLoGainSectors->GetSize() != nsectors)
        {
          *fLog << err << "ERROR - Size mismatch in number of sectors ... abort." << endl;
          return kERROR;
        }
    }

  const Bool_t rc = FillHists(par, w);

  return rc ? kTRUE : kERROR;
}

// --------------------------------------------------------------------------
//
// 0) Ask if fHiGainArray and fLoGainArray have been initialized, 
//    otherwise return kFALSE.
// 1) FinalizeHists()
// 2) FinalizeBadPixels()
// 3) CalcAverageSigma()
//
Bool_t MHCalibrationCam::Finalize()
{
  if (IsReset())
    return kTRUE;

  if (GetNumExecutions() < 2)
    return kTRUE;

  *fLog << inf << GetDescriptor() << ": Number of events used to fill histograms == " << fCurrentNumEvts << endl;

  if (fHiGainArray->GetSize() == 0 && fLoGainArray->GetSize() == 0)
    {
      *fLog << err << GetDescriptor() 
            << ": ERROR - Both (HiGain and LoGain) histogram arrays have not been initialized... abort." << endl;
      return kFALSE;
    }
  
  for (Int_t i=0; i<fAverageHiGainAreas->GetSize(); i++)
    {
      TH1F *h = GetAverageHiGainArea(i).GetHGausHist();
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
    }

  for (Int_t i=0; i<fAverageLoGainAreas->GetSize(); i++)
    {
      TH1F *h = GetAverageLoGainArea(i).GetHGausHist();
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
    }
  
  if (!FinalizeHists())
    return kFALSE;


  FinalizeBadPixels();
  CalcAverageSigma();

  return kTRUE;
}

// -------------------------------------------------------------
//
// If MBadPixelsPix::IsUnsuitable(MBadPixelsPix::kUnsuitableRun):
// - calls MHCalibrationPix::SetExcluded()
//
// Calls:
// - MHGausEvents::InitBins()
// - MHCalibrationPix::ChangeHistId(i)
// - MHCalibrationPix::SetEventFrequency(fPulserFrequency)
// 
void MHCalibrationCam::InitHists(MHCalibrationPix &hist, MBadPixelsPix &bad, const Int_t i)
{

  if (bad.IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
    hist.SetExcluded();

  hist.InitBins();
  hist.SetEventFrequency(fPulserFrequency);
}

// -------------------------------------------------------------
//
// - Searches for the Calibration*Cam corresponding to 'name'.
// - Initializes the MCalibration*Cam, if not yet done.
// 
Bool_t MHCalibrationCam::InitCams( MParList *plist, const TString name )
{

  TString ordname = "MCalibration";
  ordname += name;
  ordname += "Cam";

  fCam = (MCalibrationCam*)plist->FindObject(AddSerialNumber(ordname));
  if (!fCam)
  {
      fCam = (MCalibrationCam*)plist->FindCreateObj(AddSerialNumber(ordname));
      if (!fCam)
          return kFALSE;

      fCam->Init(*fGeom);
  }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calls FitHiGainHists for every entry in:
// - fHiGainArray
// - fAverageHiGainAreas
// - fAverageHiGainSectors
// 
void MHCalibrationCam::FitHiGainArrays(MCalibrationCam &calcam, MBadPixelsCam &badcam,
                                       MBadPixelsPix::UncalibratedType_t fittyp, 
                                       MBadPixelsPix::UncalibratedType_t osctyp)
{
  fIsHiGainFitRanges = TMath::Abs(fUpperFitLimitHiGain - fLowerFitLimitHiGain) > 1E-5;

  for (Int_t i=0; i<fHiGainArray->GetSize(); i++)
    {
      
      MHCalibrationPix &hist = (*this)[i];
      
      if (hist.IsExcluded())
        continue;
      
      MCalibrationPix &pix    = calcam[i];
      MBadPixelsPix   &bad    = badcam[i];
      
      FitHiGainHists(hist,pix,bad,fittyp,osctyp);
    }

  if (!IsAverageing())
    return;
  
  for (Int_t j=0; j<fAverageHiGainAreas->GetSize(); j++)
    {
      
      MHCalibrationPix &hist = GetAverageHiGainArea(j);      
      MCalibrationPix  &pix  = calcam.GetAverageArea(j);
      MBadPixelsPix    &bad  = calcam.GetAverageBadArea(j);        

      FitHiGainHists(hist,pix,bad,fittyp,osctyp);
  }
  
  for (Int_t j=0; j<fAverageHiGainSectors->GetSize(); j++)
    {
      MHCalibrationPix &hist = GetAverageHiGainSector(j);      
      MCalibrationPix  &pix  = calcam.GetAverageSector(j);
      MBadPixelsPix    &bad  = calcam.GetAverageBadSector(j);        

      FitHiGainHists(hist,pix,bad,fittyp,osctyp);
    }
}

// --------------------------------------------------------------------------
//
// Calls FitLoGainHists for every entry in:
// - fLoGainArray
// - fAverageLoGainAreas
// - fAverageLoGainSectors
// 
void MHCalibrationCam::FitLoGainArrays(MCalibrationCam &calcam, MBadPixelsCam &badcam,
                                            MBadPixelsPix::UncalibratedType_t fittyp, 
                                            MBadPixelsPix::UncalibratedType_t osctyp)
{
  fIsLoGainFitRanges = TMath::Abs(fUpperFitLimitLoGain - fLowerFitLimitLoGain) > 1E-5;

  if (!IsLoGain())
    return;

  for (Int_t i=0; i<fLoGainArray->GetSize(); i++)
    {
      
      MHCalibrationPix &hist = (*this)(i);
      
      if (hist.IsExcluded())
        continue;
      
      MCalibrationPix &pix    = calcam[i];
      MBadPixelsPix   &bad    = badcam[i];
      
      FitLoGainHists(hist,pix,bad,fittyp,osctyp);
      
    }

  if (!IsAverageing())
    return;
  
  for (Int_t j=0; j<fAverageLoGainAreas->GetSize(); j++)
    {
      
      MHCalibrationPix &hist = GetAverageLoGainArea(j);      
      MCalibrationPix  &pix  = calcam.GetAverageArea(j);
      MBadPixelsPix    &bad  = calcam.GetAverageBadArea(j);        
      
      FitLoGainHists(hist,pix,bad,fittyp,osctyp);
  }
  
  for (Int_t j=0; j<fAverageLoGainSectors->GetSize(); j++)
    {
      
      MHCalibrationPix &hist = GetAverageLoGainSector(j);      
      MCalibrationPix  &pix  = calcam.GetAverageSector(j);
      MBadPixelsPix    &bad  = calcam.GetAverageBadSector(j);        
      
      FitLoGainHists(hist,pix,bad,fittyp,osctyp);
    }
}

//------------------------------------------------------------
//
// For all averaged areas, the fitted sigma is multiplied with the square root of 
// the number involved pixels
//
void MHCalibrationCam::CalcAverageSigma()
{
  if (!IsAverageing())
    return;

  MCalibrationCam *cam = fCam;
  if (!cam)
    return;

  for (UInt_t j=0; j<fGeom->GetNumAreas(); j++)
    {

      MCalibrationPix &pix    = cam->GetAverageArea(j);

      const Float_t numsqr    = TMath::Sqrt((Float_t)fAverageAreaNum[j]);
      fAverageAreaSigma[j]    = pix.GetSigma    () * numsqr;
      fAverageAreaSigmaVar[j] = pix.GetSigmaErr () * pix.GetSigmaErr() * numsqr;

      pix.SetSigma   (fAverageAreaSigma[j]);
      pix.SetSigmaVar(fAverageAreaSigmaVar[j]);

      fAverageAreaRelSigma   [j]  = fAverageAreaSigma[j]    / pix.GetMean();
      fAverageAreaRelSigmaVar[j]  = fAverageAreaSigmaVar[j] / (fAverageAreaSigma[j]*fAverageAreaSigma[j]);
      fAverageAreaRelSigmaVar[j] += pix.GetMeanRelVar();
      fAverageAreaRelSigmaVar[j] *= fAverageAreaRelSigma[j];
    }

  for (UInt_t j=0; j<fGeom->GetNumSectors(); j++)
    {
      MCalibrationPix &pix    = cam->GetAverageSector(j);

      const Float_t numsqr    = TMath::Sqrt((Float_t)fAverageSectorNum[j]);
      pix.SetSigma   (pix.GetSigma() * numsqr);
      pix.SetSigmaVar(pix.GetSigmaErr() * pix.GetSigmaErr() * numsqr);
    }
}

// ---------------------------------------------------------------------------
//
// Returns if the histogram is empty and sets the following flag:
// - MBadPixelsPix::SetUnsuitable(MBadPixelsPix::kUnsuitableRun)
//
// Fits the histograms with a Gaussian, in case of failure 
// calls MHCalibrationPix::RepeatFit(), in case of repeated failure 
// calls MHCalibrationPix::BypassFit() and sets the following flags:
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::UncalibratedType_t fittyp )
// - MBadPixelsPix::SetUnsuitable(   MBadPixelsPix::kUnreliableRun   )
// 
// Creates the fourier spectrum and tests MHGausEvents::IsFourierSpectrumOK(). 
// In case no, sets the following flags:
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::UncalibratedType_t osctyp )
// - MBadPixelsPix::SetUnsuitable(   MBadPixelsPix::kUnreliableRun     )
//
// Retrieves the results and store them in MCalibrationPix
//
void MHCalibrationCam::FitHiGainHists(MHCalibrationPix &hist, 
                                      MCalibrationPix &pix, 
                                      MBadPixelsPix &bad, 
                                      MBadPixelsPix::UncalibratedType_t fittyp,
                                      MBadPixelsPix::UncalibratedType_t osctyp)
{
  if (hist.IsEmpty())
  {
      //*fLog << warn << "Pixel  " << setw(4) << pix.GetPixId() << ": Hi-Gain histogram empty." << endl;
      //bad.SetUncalibrated( fittyp );
      return;
  }
  if (hist.IsOnlyOverflow())
  {
      *fLog << warn << "Pixel  " << setw(4) << pix.GetPixId() << ": Hi-Gain histogram contains only overflows (Check Binning!)." << endl;
      bad.SetUncalibrated( fittyp );
      return;
  }
  if (hist.IsOnlyUnderflow())
  {
      *fLog << warn << "Pixel  " << setw(4) << pix.GetPixId() << ": Hi-Gain histogram contains only underflows (Check Binning!)." << endl;
      bad.SetUncalibrated( fittyp );
      return;
  }

  //
  // 2) Fit the Hi Gain histograms with a Gaussian
  //
  if (fIsHiGainFitRanges)
  {
      if (!hist.FitGaus("R",fLowerFitLimitHiGain,fUpperFitLimitHiGain))
          bad.SetUncalibrated( fittyp );
  }
  else
      if (!hist.FitGaus())
          //
          // 3) In case of failure set the bit Fitted to false and take histogram means and RMS
          //
          if (!hist.RepeatFit())
          {
              hist.BypassFit();
              bad.SetUncalibrated( fittyp );
          }

 
  //
  // 4) Check for oscillations
  // 
  if (IsOscillations())
    {
      hist.CreateFourierSpectrum();
  
      if (!hist.IsFourierSpectrumOK())
        bad.SetUncalibrated( osctyp );
    }
  
  //
  // 5) Retrieve the results and store them in this class
  //
  pix.SetHiGainMean       ( hist.GetMean()      );
  pix.SetHiGainMeanVar    ( hist.GetMeanErr() * hist.GetMeanErr()   );
  pix.SetHiGainRms        ( hist.GetHistRms()   );
  pix.SetHiGainSigma      ( hist.GetSigma()     );
  pix.SetHiGainSigmaVar   ( hist.GetSigmaErr()* hist.GetSigmaErr()  );
  pix.SetHiGainProb       ( hist.GetProb()      );
  pix.SetHiGainNumBlackout( hist.GetBlackout()  );
  pix.SetHiGainNumPickup  ( hist.GetPickup()    );
  
  if (IsDebug())
    {
      *fLog << dbginf << GetDescriptor() << ": ID " << GetName() 
            << " "<<pix.GetPixId()
            << " HiGainSaturation: "   << pix.IsHiGainSaturation() 
            << " HiGainMean: "         << hist.GetMean    ()
            << " HiGainMeanErr: "      << hist.GetMeanErr ()
            << " HiGainMeanSigma: "    << hist.GetSigma   ()
            << " HiGainMeanSigmaErr: " << hist.GetSigmaErr()
            << " HiGainMeanProb: "     << hist.GetProb    ()
            << " HiGainNumBlackout: "  << hist.GetBlackout()
            << " HiGainNumPickup  : "  << hist.GetPickup  ()
            << endl;
    }

}


// ---------------------------------------------------------------------------
//
// Returns if the histogram is empty and sets the following flag:
// - MBadPixelsPix::SetUnsuitable(MBadPixelsPix::kUnsuitableRun)
//
// Fits the histograms with a Gaussian, in case of failure 
// calls MHCalibrationPix::RepeatFit(), in case of repeated failure 
// calls MHCalibrationPix::BypassFit() and sets the following flags:
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::UncalibratedType_t fittyp )
// - MBadPixelsPix::SetUnsuitable(   MBadPixelsPix::kUnreliableRun   )
// 
// Creates the fourier spectrum and tests MHGausEvents::IsFourierSpectrumOK(). 
// In case no, sets the following flags:
// - MBadPixelsPix::SetUncalibrated( MBadPixelsPix::UncalibratedType_t osctyp )
// - MBadPixelsPix::SetUnsuitable(   MBadPixelsPix::kUnreliableRun     )
//
// Retrieves the results and store them in MCalibrationPix
//
void MHCalibrationCam::FitLoGainHists(MHCalibrationPix &hist, 
                                      MCalibrationPix &pix, 
                                      MBadPixelsPix &bad, 
                                      MBadPixelsPix::UncalibratedType_t fittyp,
                                      MBadPixelsPix::UncalibratedType_t osctyp)
{

  if (hist.IsEmpty())
  {
      //*fLog << warn << "Pixel   " << setw(4) << pix.GetPixId() << ": Lo-Gain histogram empty." << endl;
      //bad.SetUncalibrated( fittyp ); // FIXME: Combine with HiGain!
      return;
  }
  if (hist.IsOnlyOverflow())
  {
      *fLog << warn << "Pixel  " << setw(4) << pix.GetPixId() << ": Lo-Gain histogram contains only overflows (Check Binning!)." << endl;
      bad.SetUncalibrated( fittyp );
      return;
  }
  if (hist.IsOnlyUnderflow())
  {
      *fLog << warn << "Pixel  " << setw(4) << pix.GetPixId() << ": Lo-Gain histogram contains only underflows (Check Binning!)." << endl;
      bad.SetUncalibrated( fittyp );
      return;
  }

  //
  // 2) Fit the Hi Gain histograms with a Gaussian
  //
  if (fIsLoGainFitRanges)
  {
      if (!hist.FitGaus("R",fLowerFitLimitLoGain,fUpperFitLimitLoGain))
          bad.SetUncalibrated( fittyp );
  }
  else
      if (!hist.FitGaus())
          //
          // 3) In case of failure set the bit Fitted to false and take histogram means and RMS
          //
          if (!hist.RepeatFit())
          {
              hist.BypassFit();
              if (pix.IsHiGainSaturation())
                  bad.SetUncalibrated( fittyp );
          }

  //
  // 4) Check for oscillations
  // 
  if (IsOscillations())
    {
      hist.CreateFourierSpectrum();
      
      if (!hist.IsFourierSpectrumOK())
        bad.SetUncalibrated( osctyp );
    }
  
  //
  // 5) Retrieve the results and store them in this class
  //
  pix.SetLoGainMean       ( hist.GetMean()      );
  pix.SetLoGainMeanVar    ( hist.GetMeanErr()  * hist.GetMeanErr()   );
  pix.SetLoGainRms        ( hist.GetHistRms()   );
  pix.SetLoGainSigma      ( hist.GetSigma()     );
  pix.SetLoGainSigmaVar   ( hist.GetSigmaErr() * hist.GetSigmaErr()  );
  pix.SetLoGainProb       ( hist.GetProb()      );
  pix.SetLoGainNumBlackout( hist.GetBlackout()  );
  pix.SetLoGainNumPickup  ( hist.GetPickup()    );
  
  if (IsDebug())
    {
      *fLog << dbginf << GetDescriptor() << "ID: " << hist.GetName() 
            << " "<<pix.GetPixId()
            << " HiGainSaturation: "   << pix.IsHiGainSaturation()
            << " LoGainMean: "         << hist.GetMean    ()
            << " LoGainMeanErr: "      << hist.GetMeanErr ()
            << " LoGainMeanSigma: "    << hist.GetSigma   ()
            << " LoGainMeanSigmaErr: " << hist.GetSigmaErr()
            << " LoGainMeanProb: "     << hist.GetProb    ()
            << " LoGainNumBlackout: "  << hist.GetBlackout()
            << " LoGainNumPickup  : "  << hist.GetPickup  ()
            << endl;
    }

}



// -----------------------------------------------------------------------------
// 
// Default draw:
//
// Displays the averaged areas, both High Gain and Low Gain 
//
// Calls the Draw of the fAverageHiGainAreas and fAverageLoGainAreas objects with options
//
void MHCalibrationCam::Draw(const Option_t *opt)
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

      if (!fAverageAreaSat[i])
        DrawAverageSigma(fAverageAreaSat[i], i,
                         fAverageAreaSigma[i],    fAverageAreaSigmaVar[i],
                         fAverageAreaRelSigma[i], fAverageAreaRelSigmaVar[i]);

      if (IsLoGain())
        {
          pad->cd(2*(i+1));
          GetAverageLoGainArea(i).Draw(opt);
        }
      
      if (fAverageAreaSat[i])
        DrawAverageSigma(fAverageAreaSat[i], i,
                         fAverageAreaSigma[i], fAverageAreaSigmaVar[i],
                         fAverageAreaRelSigma[i], fAverageAreaRelSigmaVar[i]);
    }
      
}

// -----------------------------------------------------------------------------
// 
// Default draw:
//
// Displays a TPaveText with the re-normalized sigmas of the average area
//
void MHCalibrationCam::DrawAverageSigma(Bool_t sat, Bool_t inner,
                                        Float_t sigma, Float_t sigmavar,
                                        Float_t relsigma, Float_t relsigmavar) const
{
  
    if (sigma==0 || sigmavar<0 || relsigmavar<0)
        return;

      TPaveText *text = new TPaveText(sat? 0.1 : 0.35,0.7,sat ? 0.4 : 0.7,1.0);
      text->SetTextSize(0.05);
      text->SetBit(kCanDelete);
      text->Draw("");

      const TString line1 = Form("%s Pixels %s Gain",
                                 inner?"Outer" : "Inner", sat?"Low":"High");
      const TString line2 = Form("#sigma per pix: %2.2f #pm %2.2f", sigma, TMath::Sqrt(sigmavar));
      const TString line3 = Form("Rel. #sigma per pix: %2.2f #pm %2.2f", relsigma, TMath::Sqrt(relsigmavar));

      text->AddText(line1.Data());
      text->AddText(line2.Data());
      text->AddText(line3.Data());
}

// -----------------------------------------------------------------------------
// 
// Available options
//  Debug
//  LoGain
//  Oscillations
//  SizeCheck
//  Averageing
//  Nbins
//  First
//  Last
//  ProbLimit
//  OverflowLimit
//  PulserFrequency
//  LowerFitLimitHiGain
//  UpperFitLimitHiGain
//  LowerFitLimitLoGain
//  UpperFitLimitLoGain
//
Int_t MHCalibrationCam::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{

  Bool_t rc = kFALSE;
  if (IsEnvDefined(env, prefix, "Debug", print))
    {
      SetDebug(GetEnvValue(env, prefix, "Debug", IsDebug()));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "LoGain", print))
    {
      SetDebug(GetEnvValue(env, prefix, "LoGain", IsLoGain()));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "Oscillations", print))
    {
      SetOscillations(GetEnvValue(env, prefix, "Oscillations", IsOscillations()));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "SizeCheck", print))
    {
      SetSizeCheck(GetEnvValue(env, prefix, "SizeCheck", IsSizeCheck()));
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "Averageing", print))
    {
      SetAverageing(GetEnvValue(env, prefix, "Averageing", IsAverageing()));
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "Nbins", print))
    {
      fNbins = GetEnvValue(env, prefix, "Nbins", fNbins);
      rc = kTRUE;
    }
  if (IsEnvDefined(env, prefix, "First", print))
    {
      fFirst = GetEnvValue(env, prefix, "First", fFirst);
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "Last", print))
    {
      fLast = GetEnvValue(env, prefix, "Last", fLast);
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "ProbLimit", print))
    {
      SetProbLimit(GetEnvValue(env, prefix, "ProbLimit", fProbLimit));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "MaxNumEvts", print))
    {
      SetMaxNumEvts(GetEnvValue(env, prefix, "MaxNumEvts", fMaxNumEvts));
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "OverflowLimit", print))
    {
      SetOverflowLimit(GetEnvValue(env, prefix, "OverflowLimit", fOverflowLimit));
      rc = kTRUE;
    }
  
  if (IsEnvDefined(env, prefix, "PulserFrequency", print))
    {
      SetPulserFrequency(GetEnvValue(env, prefix, "PulserFrequency", fPulserFrequency));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "LowerFitLimitHiGain", print))
    {
      SetLowerFitLimitHiGain(GetEnvValue(env, prefix, "LowerFitLimitHiGain", fLowerFitLimitHiGain));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "UpperFitLimitHiGain", print))
    {
      SetUpperFitLimitHiGain(GetEnvValue(env, prefix, "UpperFitLimitHiGain", fUpperFitLimitHiGain));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "LowerFitLimitLoGain", print))
    {
      SetLowerFitLimitLoGain(GetEnvValue(env, prefix, "LowerFitLimitLoGain", fLowerFitLimitLoGain));
      rc = kTRUE;
    }

  if (IsEnvDefined(env, prefix, "UpperFitLimitLoGain", print))
    {
      SetUpperFitLimitLoGain(GetEnvValue(env, prefix, "UpperFitLimitLoGain", fUpperFitLimitLoGain));
      rc = kTRUE;
    }

  
  return rc;
}

