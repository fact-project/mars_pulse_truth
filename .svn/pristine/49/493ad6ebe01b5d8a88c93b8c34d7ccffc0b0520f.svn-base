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
// MBadPixelsIntensityCam
//
// Base class for intensity calibration results
//
// Contains TClonesArrays for the following objects:
// - fCams:  Array of classes derived from MBadPixelsCam, one entry
//           per calibration camera result. Has to be created
//
// See also: MCalibrationIntensityChargeCam, MCalibrationCam, MCalibrationPix,
//           MHCalibrationChargePix, MHCalibrationChargeCam
//           MCalibrationChargeBlindPix, MCalibrationChargePINDiode
//
//
/////////////////////////////////////////////////////////////////////////////
#include "MBadPixelsIntensityCam.h"

#include <TOrdCollection.h>
#include <TGraph.h>

#include "MGeomPix.h"
#include "MHCamera.h"
#include "MLogManip.h"

ClassImp(MBadPixelsIntensityCam);

using namespace std;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Initializes and sets owner of:
// - fCams
// - Initializes fCams to entry 
//
MBadPixelsIntensityCam::MBadPixelsIntensityCam(const char *name, const char *title)
{

  fName  = name  ? name  : "MBadPixelsIntensityCam";
  fTitle = title ? title : "Base container for the Intensity Calibration";
  
  fCams = new TOrdCollection;
  fCams->SetOwner();

  InitSize(1);
}

// --------------------------------------------------------------------------
//
// Deletes the cameras if they exist
//
MBadPixelsIntensityCam::~MBadPixelsIntensityCam()
{
  if (fCams)
    delete fCams;
}

// --------------------------------------------------------------------------
//
// Add a new MBadPixelsCam to fCams, give it the name "name" and initialize
// it with geom.
//
void MBadPixelsIntensityCam::AddToList( const char* name, const MGeomCam &geom) 
{
  InitSize(GetSize()+1);
  GetCam()->SetName(name);
  GetCam()->Init(geom);
}



// --------------------------------------------------------------------------
//
// Copy 'constructor'
//
void MBadPixelsIntensityCam::Copy(TObject& object) const
{
  
  MBadPixelsIntensityCam &calib = (MBadPixelsIntensityCam&)object;
  
  MParContainer::Copy(calib);
  
  const UInt_t n = GetSize();
  if (n != 0)
    {
      calib.InitSize(n);
      for (UInt_t i=0; i<n; i++)
        GetCam(i)->Copy(*(calib.GetCam(i)));
    }
  
}

// -----------------------------------------------------
//
// Calls Clear() for all entries fCams
//
void MBadPixelsIntensityCam::Clear(Option_t *o)
{

  fCams->R__FOR_EACH(MBadPixelsCam, Clear)();

  return;
}

// -----------------------------------------------------
//
// Calls Print(o) for all entries fCams
//
void MBadPixelsIntensityCam::Print(Option_t *o) const
{
  fCams->R__FOR_EACH(MBadPixelsCam, Print)(o);
}


// -------------------------------------------------------------------
//
// Initialize the objects inside the TOrdCollection using the 
// function Add().
//
// InitSize can only increase the size, but not shrink. 
// 
// It can be called more than one time. New Containers are 
// added only from the current size to the argument i.
//
void MBadPixelsIntensityCam::InitSize(const UInt_t i)
{

  const UInt_t save = GetSize();

  if (i==save)
    return;
  
  if (i>save)
    Add(save,i);
}

// -------------------------------------------------------------------
//
// Add MBadPixelsCams in the ranges from - to. In order to initialize
// from MBadPixelsCam derived containers, overwrite this function
//
void MBadPixelsIntensityCam::Add(const UInt_t from, const UInt_t to)
{
  for (UInt_t i=from; i<to; i++)
    fCams->AddAt(new MBadPixelsCam,i);
}


// -------------------------------------------------------------------
//
// If size is still 0, Intialize a first Cam.
// Calls Init(geom) for all fCams
//
void MBadPixelsIntensityCam::Init(const MGeomCam &geom)
{
  if (GetSize() == 0)
    InitSize(1);

  fCams->R__FOR_EACH(MBadPixelsCam,Init)(geom);
}


// --------------------------------------------------------------------------
//
// Returns the current size of the TOrdCollection fCams 
// independently if the MBadPixelsCam is filled with values or not.
//
Int_t MBadPixelsIntensityCam::GetSize() const 
{
  return fCams->GetSize();
}

// --------------------------------------------------------------------------
//
// Get i-th pixel from current camera
//
MBadPixelsPix &MBadPixelsIntensityCam::operator[](Int_t i)
{
  return (*GetCam())[i];
}

// --------------------------------------------------------------------------
//
// Get i-th pixel from current camera
//
const MBadPixelsPix &MBadPixelsIntensityCam::operator[](Int_t i) const 
{
  return (*GetCam())[i];
}


// --------------------------------------------------------------------------
//
// Get i-th camera 
//
MBadPixelsCam *MBadPixelsIntensityCam::GetCam(Int_t i)
{
  return static_cast<MBadPixelsCam*>(i==-1 ? fCams->Last() : fCams->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th camera 
//
const MBadPixelsCam *MBadPixelsIntensityCam::GetCam(Int_t i) const 
{
  return static_cast<MBadPixelsCam*>(i==-1 ? fCams->Last() : fCams->At(i));
}
/*
// --------------------------------------------------------------------------
//
// Get camera with name 'name' 
//
MBadPixelsCam *MBadPixelsIntensityCam::GetCam(const char *name )
{
  return static_cast<MBadPixelsCam*>(fCams->FindObject(name));
}

// --------------------------------------------------------------------------
//
// Get camera with name 'name' 
//
const MBadPixelsCam *MBadPixelsIntensityCam::GetCam(const char *name ) const 
{
  return static_cast<MBadPixelsCam*>(fCams->FindObject(name));
}
*/
// --------------------------------------------------------------------------
//
// Calls GetPixelContent for the current entry in fCams
//
Bool_t MBadPixelsIntensityCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
  return GetCam()->GetPixelContent(val,idx,cam,type);
}

// --------------------------------------------------------------------------
//
// Calls DrawPixelContent for the current entry in fCams
//
void MBadPixelsIntensityCam::DrawPixelContent( Int_t num ) const
{
  return GetCam()->DrawPixelContent(num);
}


// -------------------------------------------------------------------
//
// Returns a TGraph with the number of uncalibrated type pixels per area index
// vs. the calibration camera.
//
TGraph *MBadPixelsIntensityCam::GetUncalibratedPerAreaVsTime(const MBadPixelsPix::UncalibratedType_t typ,
                                                             const Int_t aidx, const MGeomCam &geom)
{
  
  const Int_t size = GetSize();
  
  if (size == 0)
    return NULL;
  
  TArrayF uncal(size);
  TArrayF time(size);
  
  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MBadPixelsCam *cam = GetCam(i);

      //
      // Get the calibration pix from the calibration cam
      //
      for (Int_t j=0; j<cam->GetSize(); j++)
        {

          if (geom[j].GetAidx() != aidx && aidx > -1)
            continue;

          const MBadPixelsPix &pix = (*cam)[j];
          //
          // Don't use bad pixels
          //
          if (pix.IsUncalibrated(typ))
            uncal[i]++;
        }
      time[i] = i;
    }
  
  TGraph *gr = new TGraph(size,time.GetArray(),uncal.GetArray());
  
  gr->SetTitle(Form("Uncalibrated Pixels Area %d",aidx));
  gr->GetXaxis()->SetTitle("Camera Nr.");
  gr->GetYaxis()->SetTitle("<N_{uncal}> [1]");      
  return gr;
}

TGraph *MBadPixelsIntensityCam::GetUnsuitablePerAreaVsTime(const MBadPixelsPix::UnsuitableType_t typ, const Int_t aidx, const MGeomCam &geom)
{
  const Int_t size = GetSize();
  
  if (size == 0)
    return NULL;
  
  TArrayF unsuit(size);
  TArrayF time(size);
  
  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MBadPixelsCam *cam = GetCam(i);

      //
      // Get the calibration pix from the calibration cam
      //
      for (Int_t j=0; j<cam->GetSize(); j++)
        {
          if (geom[j].GetAidx() != aidx && aidx > -1)
            continue;

          const MBadPixelsPix &pix = (*cam)[j];
          //
          // Don't use bad pixels
          //
          if (pix.IsUnsuitable(typ))
            unsuit[i]++;
        }
      time[i] = i;
    }
  
  TGraph *gr = new TGraph(size,time.GetArray(),unsuit.GetArray());
  
  gr->SetTitle(Form("Unsuitable Pixels Area %d",aidx));
  gr->GetXaxis()->SetTitle("Camera Nr.");
  gr->GetYaxis()->SetTitle("<N_{unsuit}> [1]");      
  return gr;
}

MHCamera *MBadPixelsIntensityCam::GetUnsuitableSpectrum(const MBadPixelsPix::UnsuitableType_t typ, const MGeomCam &geom)
{
  const Int_t size = GetSize();
  
  if (size == 0)
    return NULL;
  
  TString title;
  TString axist;
  
  switch (typ)
    {
    case MBadPixelsPix::kUnsuitableRun:
      title = "Unsuitable Pixels";
      break;
    case MBadPixelsPix::kUnreliableRun:
      title = "Unreliable Pixels";
      break;
    default:
      *fLog << warn << "Could not determine unsuitable type ... abort " << endl;
      return NULL;
    }
  
  MHCamera *camunsuit = new MHCamera(geom,"Unsuitables",title.Data());

  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MBadPixelsCam *cam = GetCam(i);

      //
      // Get the calibration pix from the calibration cam
      //
      for (Int_t j=0; j<cam->GetSize(); j++)
        {
          const MBadPixelsPix &pix = (*cam)[j];
          //
          // Don't use bad pixels
          //
          if (pix.IsUnsuitable(typ))
            {
              camunsuit->Fill(j,1);
              camunsuit->SetUsed(j);
            }
        }
    }
  
  return camunsuit;
}

MHCamera *MBadPixelsIntensityCam::GetUncalibratedSpectrum(const MBadPixelsPix::UncalibratedType_t typ, const MGeomCam &geom)
{

  const Int_t size = GetSize();
  
  if (size == 0)
    return NULL;
  
  TString title;
  TString axist;
  
  switch (typ)
    {
      case MBadPixelsPix::kPreviouslyExcluded:
        title = "PreviouslyExcluded";
        break;
      case MBadPixelsPix::kHiGainNotFitted:
        title = "HiGainNotFitted";
        break;
      case MBadPixelsPix::kLoGainNotFitted:
        title = "LoGainNotFitted";
        break;
      case MBadPixelsPix::kRelTimeNotFitted:
        title = "RelTimeNotFitted";
        break;
      case MBadPixelsPix::kHiGainOscillating:
        title = "HiGainOscillating";
        break;
      case MBadPixelsPix::kLoGainOscillating:
        title = "LoGainOscillating";
        break;
      case MBadPixelsPix::kRelTimeOscillating:
        title = "RelTimeOscillating";
        break;
      case MBadPixelsPix::kLoGainSaturation:
        title = "LoGainSaturation";
        break;
      case MBadPixelsPix::kChargeIsPedestal:
        title = "ChargeIsPedestal";
        break;
      case MBadPixelsPix::kChargeErrNotValid:
        title = "ChargeErrNotValid";
        break;
      case MBadPixelsPix::kChargeRelErrNotValid:
        title = "ChargeRelErrNotValid";
        break;
      case MBadPixelsPix::kChargeSigmaNotValid:
        title = "ChargeSigmaNotValid";
        break;
      case MBadPixelsPix::kMeanTimeInFirstBin:
        title = "MeanTimeInFirstBin";
        break;
      case MBadPixelsPix::kMeanTimeInLast2Bins:
        title = "MeanTimeInLast2Bins";
        break;
      case MBadPixelsPix::kDeviatingNumPhes:
        title = "DeviatingNumPhes";
        break;
      case MBadPixelsPix::kDeviatingNumPhots:
        title = "DeviatingNumPhots";
        break;
      case MBadPixelsPix::kDeviatingFFactor:
        title = "DeviatingFFactor";
        break;
      case MBadPixelsPix::kDeviatingTimeResolution:
        title = "DeviatingTimeResolution";
        break;
      case MBadPixelsPix::kConversionHiLoNotValid:
        title = "ConversionHiLoNotValid";
        break;
      case MBadPixelsPix::kHiGainOverFlow:
        title = "HiGainOverFlow";
        break;
      case MBadPixelsPix::kLoGainOverFlow:
        title = "LoGainOverFlow";
        break;
      case MBadPixelsPix::kHiLoNotFitted:
        title = "HiLoNotFitted";
        break;
      case MBadPixelsPix::kHiLoOscillating:
        title = "HiLoOscillating";
        break;
      case MBadPixelsPix::kDeadPedestalRms:
        title = "DeadPedestalRms";
        break;
      case MBadPixelsPix::kFluctuatingArrivalTimes:
        title = "FluctuatingArrivalTimes";
        break;
    default:
      *fLog << warn << "Could not determine uncalibrated type ... abort " << endl;
      return NULL;
    }
  
  MHCamera *camuncal = new MHCamera(geom,"Uncalibrated",title.Data());

  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MBadPixelsCam *cam = GetCam(i);

      //
      // Get the calibration pix from the calibration cam
      //
      for (Int_t j=0; j<cam->GetSize(); j++)
        {
          const MBadPixelsPix &pix = (*cam)[j];
          //
          // Don't use bad pixels
          //
          if (pix.IsUncalibrated(typ))
            {
              camuncal->Fill(j,1);
              camuncal->SetUsed(j);
            }
        }
    }
  
  return camuncal;
}



void MBadPixelsIntensityCam::DrawUnsuitablePerAreaVsTime(const MBadPixelsPix::UnsuitableType_t typ, const Int_t aidx, const MGeomCam &geom)
{
  TGraph *gr = GetUnsuitablePerAreaVsTime(typ,aidx,geom);
  gr->SetBit(kCanDelete);
  gr->Draw("A*");
}

void MBadPixelsIntensityCam::DrawUncalibratedPerAreaVsTime(const MBadPixelsPix::UncalibratedType_t typ, const Int_t aidx, const MGeomCam &geom)
{
  TGraph *gr = GetUncalibratedPerAreaVsTime(typ,aidx,geom);
  gr->SetBit(kCanDelete);
  gr->Draw("A*");
}
