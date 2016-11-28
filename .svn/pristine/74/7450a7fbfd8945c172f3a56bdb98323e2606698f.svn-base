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
!   Author(s): Markus Gaug   06/2005 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//                                                               
// MCalibrationIntensityConstCam                                               
//                                                               
// Storage container for intensity charge calibration results. 
// 
// Individual MCalibrationConstCam's can be retrieved with: 
// - GetCam() yielding the current cam.
// - GetCam("name") yielding the current camera with name "name".
// - GetCam(i) yielding the i-th camera.
//
// See also: MCalibrationIntensityCam, MCalibConstCam,
//           MCalibrationChargePix, MCalibrationChargeCalc, MCalibrationQECam
//           MHCalibrationPix, MHCalibrationCam              
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationIntensityConstCam.h"
#include "MCalibConstCam.h"
#include "MCalibConstPix.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MLogManip.h"
#include "MHCamera.h"

#include <TH1D.h>
#include <TF1.h>
#include <TOrdCollection.h>
#include <TGraphErrors.h>

ClassImp(MCalibrationIntensityConstCam);

using namespace std;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCalibrationIntensityConstCam::MCalibrationIntensityConstCam(const char *name, const char *title)
{

  fName  = name  ? name  : "MCalibrationIntensityConstCam";
  fTitle = title ? title : "Results of the Intensity Calibration";
  
  fCams = new TOrdCollection;
  fCams->SetOwner();
  
  InitSize(1);
}

// --------------------------------------------------------------------------
//
// Deletes the histograms if they exist
//
MCalibrationIntensityConstCam::~MCalibrationIntensityConstCam()
{
  if (fCams)
    delete fCams;
}

// -------------------------------------------------------------------
//
// Add MCalibrationConstCam's in the ranges from - to. 
//
void MCalibrationIntensityConstCam::Add(const UInt_t from, const UInt_t to)
{
  for (UInt_t i=from; i<to; i++)
    fCams->AddAt(new MCalibConstCam,i);
}

// --------------------------------------------------------------------------
//
// Add a new MCalibConstCam to fCams, give it the name "name" and initialize
// it with geom.
//
void MCalibrationIntensityConstCam::AddToList( const char* name, const MGeomCam &geom) 
{
  InitSize(GetSize()+1);
  GetCam()->SetName(name);
  GetCam()->Init(geom);
}

// -----------------------------------------------------
//
// Calls Clear() for all entries fCams
//
void MCalibrationIntensityConstCam::Clear(Option_t *o)
{
  fCams->R__FOR_EACH(MCalibConstCam, Clear)();
}

// -----------------------------------------------------
//
// Calls Print(o) for all entries fCams
//
void MCalibrationIntensityConstCam::Print(Option_t *o) const
{
  fCams->R__FOR_EACH(MCalibConstCam, Print)(o);
}


// -------------------------------------------------------------------
//
// Initialize the objects inside the TOrdCollection using the 
// virtual function Add().
//
// InitSize can only increase the size, but not shrink. 
// 
// It can be called more than one time. New Containers are 
// added only from the current size to the argument i.
//
void MCalibrationIntensityConstCam::InitSize(const UInt_t i)
{

  const UInt_t save = GetSize();

  if (i==save)
    return;
  
  if (i>save)
    Add(save,i);
}

// -------------------------------------------------------------------
//
// If size is still 0, Intialize a first Cam.
// Calls Init(geom) for all fCams
//
void MCalibrationIntensityConstCam::Init(const MGeomCam &geom)
{
  if (GetSize() == 0)
    InitSize(1);

  fCams->R__FOR_EACH(MCalibConstCam,Init)(geom);
}



// --------------------------------------------------------------------------
//
// Returns the current size of the TOrdCollection fCams 
// independently if the MCalibrationCam is filled with values or not.
//
const Int_t MCalibrationIntensityConstCam::GetSize() const 
{
  return fCams->GetSize();
}

// --------------------------------------------------------------------------
//
// Get i-th pixel from current camera
//
MCalibConstPix &MCalibrationIntensityConstCam::operator[](UInt_t i)
{
  return (*GetCam())[i];
}

// --------------------------------------------------------------------------
//
// Get i-th pixel from current camera
//
const MCalibConstPix &MCalibrationIntensityConstCam::operator[](UInt_t i) const 
{
  return (*GetCam())[i];
}

// --------------------------------------------------------------------------
//
// Get i-th camera 
//
MCalibConstCam *MCalibrationIntensityConstCam::GetCam(Int_t i)
{
  return static_cast<MCalibConstCam*>(i==-1 ? fCams->Last() : fCams->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th camera 
//
const MCalibConstCam *MCalibrationIntensityConstCam::GetCam(Int_t i) const 
{
  return static_cast<MCalibConstCam*>(i==-1 ? fCams->Last() : fCams->At(i));
}

Bool_t MCalibrationIntensityConstCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
  if ((*fBadPixels)[idx].IsUnsuitable())
    return kFALSE;
  
  return GetCam()->GetPixelContent(val,idx,cam,type);
}

TGraphErrors *MCalibrationIntensityConstCam::GetConvFactorPerAreaVsTime( const Int_t aidx, const MGeomCam &geom)
{
  
  const Int_t size = GetSize();
  
  if (size == 0)
    return NULL;
  
  TArrayF convarea(size);
  TArrayF convareaerr(size);
  TArrayF time(size);
  TArrayF timeerr(size);
  
  TH1D *h = 0;
  MCalibConstCam *cam = 0;

  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      cam = GetCam(i);

      //
      // Get the calibration pix from the calibration cam
      //
      Double_t variab   = 0.;
      Double_t variab2  = 0.;
      Double_t variance = 0.;
      Int_t    num      = 0;
      Float_t  pvar     = 0.;

      MHCamera camconv(geom,"CamConv","Variable;;channels");
      //
      // Get the area calibration pix from the calibration cam
      //
      for (Int_t j=0; j<cam->GetSize(); j++)
        {
          const MCalibConstPix &pix = (*cam)[j];
          //
          // Use only pixels of a same area index
          //
          if (aidx != geom[j].GetAidx())
            continue;

          pvar = pix.GetCalibConst();

          if (pvar < 0)
            continue;
          
          variab  += pvar;
          variab2 += pvar*pvar;
          num++;
          
          camconv.Fill(j,pvar);
          camconv.SetUsed(j);
        }
      
      if (num > 1)
        {
          variab  /= num;
          variance = (variab2 - variab*variab*num) / (num-1);

          convarea[i] = variab;
          if (variance > 0.)
            convareaerr[i] = TMath::Sqrt(variance);
          else
            convareaerr[i] = 999999999.;

          //
          // Make also a Gauss-fit to the distributions. The RMS can be determined by 
          // outlier, thus we look at the sigma and the RMS and take the smaller one, afterwards.
          // 
          h = camconv.ProjectionS(TArrayI(),TArrayI(1,&aidx),"_py",500);
          h->SetDirectory(NULL);
          h->Fit("gaus","QL");
          TF1 *fit = h->GetFunction("gaus");

          Float_t ci2   = fit->GetChisquare();
          Float_t sigma = fit->GetParameter(2);

          if (ci2 > 500. || sigma > convareaerr[i])
            {
              h->Fit("gaus","QLM");
              fit = h->GetFunction("gaus");

              ci2   = fit->GetChisquare();
              sigma = fit->GetParameter(2);
            }
          
          const Float_t mean    = fit->GetParameter(1);
          const Float_t meanerr = fit->GetParError(1);
          const Float_t ndf     = fit->GetNDF();
          
          *fLog << inf << "Camera Nr: " << i << endl;
          *fLog << inf << " area idx: " << aidx << " Results: " << endl;
          *fLog << inf << "Mean: " << Form("%4.3f",mean) 
                << "+-" << Form("%4.3f",meanerr)
                << "  Sigma: " << Form("%4.3f",sigma) << "+-" << Form("%4.3f",fit->GetParError(2)) 
                << "  Chisquare: " << Form("%4.3f",ci2) << "  NDF  : " << ndf << endl;          
          delete h;
          gROOT->GetListOfFunctions()->Remove(fit);

          if (meanerr < convareaerr[i] && ndf > 2 && ci2/ndf < 10.)
            {
              convarea   [i] = mean;
              convareaerr[i] = meanerr;
            }
          else
            {
              convareaerr[i] /= TMath::Sqrt((Float_t)num);
            }
        }
      else
        {
          convarea[i]    = -1.;
          convareaerr[i] = 0.;
        }

      time[i] = i;
      timeerr[i] = 0.;
    }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                      time.GetArray(),convarea.GetArray(),
                                      timeerr.GetArray(),convareaerr.GetArray());
  gr->SetTitle(Form("Conversion Factors Area %3i Average",aidx));
  gr->GetXaxis()->SetTitle("Camera Nr.");
  gr->GetYaxis()->SetTitle("<N_{phe}>/<Q> [FADC cnt^{-1}]");      
  return gr;
}

TGraph *MCalibrationIntensityConstCam::GetConvFactorVsTime ( const Int_t pixid )
{
  
  const Int_t size = GetSize();

  if (size == 0)
    return NULL;
  
  TArrayF time(size);
  TArrayF conv(size);
  
  MCalibConstCam *cam = 0;

  for (Int_t i=0;i<size;i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      cam = GetCam(i);

      if (cam->GetSize()-1 < pixid)
        {
          *fLog << err << "Pixel " << pixid << " out of range " << endl;
          return NULL;
        }

      //
      // Get the calibration pix from the calibration cam
      //
      MCalibConstPix &pix = (*cam)[pixid];
      //
      time[i]    = i;
      conv[i]    = pix.GetCalibConst();
      //
  }
  
  
  TGraph *gr = new TGraph(size,time.GetArray(),conv.GetArray());
  gr->SetTitle(Form("%s%3i","Pixel ",pixid));
  gr->GetXaxis()->SetTitle("Camera Nr.");
  gr->GetYaxis()->SetTitle("<N_{phe}>/<Q> [FADC cnts^{-1}]");
  return gr;

  
}

void MCalibrationIntensityConstCam::DrawConvFactorPerAreaVsTime( const Int_t aidx )
{

  TGraphErrors *gr = GetConvFactorPerAreaVsTime(aidx,MGeomCamMagic()); 

  if (gr)
    {
      gr->SetBit(kCanDelete);
      gr->Draw("A*");
    }
}

void MCalibrationIntensityConstCam::DrawConvFactorVsTime( const Int_t idx )
{
  TGraph *gr = GetConvFactorVsTime(idx); 
  if (gr)
    {
      gr->SetBit(kCanDelete);
      gr->Draw("A*");
    }
}

