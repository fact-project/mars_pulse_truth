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
// MCalibrationIntensityRelTimeCam                                               
//                                                               
// Storage container for intensity charge calibration results. 
// 
// Individual MCalibrationRelTimeCam's can be retrieved with: 
// - GetCam() yielding the current cam.
// - GetCam("name") yielding the current camera with name "name".
// - GetCam(i) yielding the i-th camera.
//
// See also: MCalibrationIntensityCam, MCalibrationRelTimeCam,
//           MCalibrationRelTimePix, MCalibrationRelTimeCalc, MCalibrationQECam
//           MHCalibrationRelTimePix, MHCalibrationRelTimeCam              
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationIntensityRelTimeCam.h"
#include "MCalibrationRelTimeCam.h"
#include "MCalibrationChargeCam.h"
#include "MCalibrationRelTimePix.h"
#include "MCalibrationChargePix.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MHCamera.h"

#include "MLogManip.h"

#include <TOrdCollection.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TF1.h>

ClassImp(MCalibrationIntensityRelTimeCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCalibrationIntensityRelTimeCam::MCalibrationIntensityRelTimeCam(const char *name, const char *title)
{

  fName  = name  ? name  : "MCalibrationIntensityRelTimeCam";
  fTitle = title ? title : "Results of the Intensity Calibration";
  
  InitSize(1);
}

// -------------------------------------------------------------------
//
// Add MCalibrationRelTimeCam's in the ranges from - to. 
//
void MCalibrationIntensityRelTimeCam::Add(const UInt_t from, const UInt_t to)
{
  for (UInt_t i=from; i<to; i++)
    fCams->AddAt(new MCalibrationRelTimeCam,i);
}

TGraphErrors *MCalibrationIntensityRelTimeCam::GetTimeResVsCharge( const UInt_t pixid, const MCalibrationIntensityChargeCam &chargecam, const MCalibrationCam::PulserColor_t col)
{

  if (chargecam.GetSize() != GetSize())
    {
      *fLog << err << GetDescriptor() << ": Size mismatch between MCalibrationIntensityRelTimeCam "
            << "and MCalibrationIntensityChargeCam. " << endl;
      return NULL;
    }
  
  Int_t size = CountNumEntries(col);
  
  if (size == 0)
    return NULL;

  if (size != chargecam.CountNumEntries(col))
    {
      *fLog << err << GetDescriptor() << ": Size mismatch in colour between MCalibrationIntensityRelTimeCam "
            << "and MCalibrationIntensityChargeCam. " << endl;
      return NULL;
    }
  
  const Int_t nvalid = chargecam.CountNumValidEntries(pixid,col);

  if (nvalid == 0)
    {
      *fLog << err << GetDescriptor() << ": Only un-calibrated events in pixel: " << pixid << endl;
      return NULL;
    }

  TArrayF res(nvalid);
  TArrayF reserr(nvalid);
  TArrayF sig(nvalid);
  TArrayF sigerr(nvalid);

  const Float_t sqrt2   = 1.414;
  const Float_t fadc2ns = 3.333;
  
  Int_t cnt = 0;
  
  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationChargeCam *cam     = (MCalibrationChargeCam*)chargecam.GetCam(i);
      MCalibrationRelTimeCam *relcam = (MCalibrationRelTimeCam*)GetCam(i);

      if (col != MCalibrationCam::kNONE)
        if (relcam->GetPulserColor() != col)
          continue;
      //
      // Get the calibration pix from the calibration cam
      //
      MCalibrationChargePix  &pix    = (MCalibrationChargePix&)(*cam)[pixid];
      MCalibrationRelTimePix &relpix = (MCalibrationRelTimePix&)(*relcam)[pixid];
      //
      // Don't use bad pixels
      //
      if (!pix.IsFFactorMethodValid())
        continue;
      //
      res[cnt]    = relpix.GetTimePrecision() / sqrt2 * fadc2ns;
      reserr[cnt] = relpix.GetTimePrecisionErr() / sqrt2 * fadc2ns;
      //
      sig   [cnt] = pix.GetPheFFactorMethod();
      sigerr[cnt] = pix.GetPheFFactorMethodErr();
      cnt++;
    }

  TGraphErrors *gr = new TGraphErrors(nvalid,
                                     sig.GetArray(),res.GetArray(),
                                     sigerr.GetArray(),reserr.GetArray());
  gr->SetTitle(Form("%s%3i","Pixel ",pixid));
  gr->GetXaxis()->SetTitle("<Photo-electrons> [1]");
  gr->GetYaxis()->SetTitle("Time Resolution [ns]");      
  return gr;
}


TGraphErrors *MCalibrationIntensityRelTimeCam::GetTimeResVsChargePerArea( const Int_t aidx,const MCalibrationIntensityChargeCam &chargecam, const MGeomCam &geom, const MCalibrationCam::PulserColor_t col)
{
  
  Int_t size = CountNumEntries(col);
  
  if (size == 0)
    return NULL;

  if (size != chargecam.CountNumEntries(col))
    {
      *fLog << err << GetDescriptor() << ": Size mismatch in colour between MCalibrationIntensityRelTimeCam "
            << "and MCalibrationIntensityChargeCam. " << endl;
      return NULL;
    }

  if (col == MCalibrationCam::kBLUE)
    size -= 5;
  if (col == MCalibrationCam::kNONE)
    size -= 5;
  
  const Float_t sqrt2   = 1.414;
  const Float_t fadc2ns = 3.333;
  const Float_t norm    = fadc2ns / sqrt2;

  TArrayD res(size);
  TArrayD reserr(size);
  TArrayD sig(size);
  TArrayD sigerr(size);
  
  Int_t cnt = 0;

  TH1D *h = 0;

  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationRelTimeCam *relcam = (MCalibrationRelTimeCam*)GetCam(i);
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)chargecam.GetCam(i);

      if (relcam->GetPulserColor() != col && col != MCalibrationCam::kNONE)
        continue;

      const MCalibrationChargePix &apix = (MCalibrationChargePix&)cam->GetAverageArea(aidx);
      const Double_t phe          = (Double_t)apix.GetPheFFactorMethod();
      const Double_t pheerr       = (Double_t)apix.GetPheFFactorMethodErr();

      if (relcam->GetPulserColor() == MCalibrationCam::kBLUE)
	if (aidx == 0)
	  {
	    if (phe > 100. && phe < 190.)
	      continue;
	  }
	else
	  {
	    if (phe > 200. && phe < 480.)
	      continue;
	  }

      if (relcam->GetPulserColor() == MCalibrationCam::kUV)
	*fLog << inf << "NUMBER: " << i << endl;

      sig[cnt]       = phe;
      sigerr[cnt]    = pheerr;

      Double_t resol  = 0.;
      Double_t resol2 = 0.;
      Double_t var    = 0.;
      Int_t    num    = 0;

      MHCamera camres(geom,"CamRes","Time Resolution;Time Resolution [ns];channels");
      //
      // Get the area calibration pix from the calibration cam
      //
      for (Int_t j=0; j<cam->GetSize(); j++)
        {
          const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*cam)[j];
          const MCalibrationRelTimePix &relpix = (MCalibrationRelTimePix&)(*relcam)[j];
          //
          // Don't use bad pixels
          //
          if (!pix.IsFFactorMethodValid())
            continue;
          //
          //
          if (aidx != geom[j].GetAidx())
            continue;
          
          const Double_t pres = (Double_t)relpix.GetTimePrecision();

          resol  += pres;
          resol2 += pres;
          num++;
          
          camres.Fill(j,pres);
          camres.SetUsed(j);
        }
      
      if (num > 100)
        {
          resol /= num;
          var  = (resol2 - resol*resol*num) / (num-1);

          res[cnt] = resol;
          if (var > 0.)
            reserr[cnt] = TMath::Sqrt(var);
          else
            reserr[cnt] = 0.;

          //
          // Make also a Gauss-fit to the distributions. The RMS can be determined by 
          // outlier, thus we look at the sigma and the RMS and take the smaller one, afterwards.
          // 
          h = camres.ProjectionS(TArrayI(),TArrayI(1,&aidx),"_py",750);
          h->SetDirectory(NULL);
          h->Fit("gaus","QL");
          TF1 *fit = h->GetFunction("gaus");

          Double_t ci2   = fit->GetChisquare();
          Double_t sigma = fit->GetParameter(2);

          if (ci2 > 500. || sigma > reserr[cnt])
            {
              h->Fit("gaus","QLM");
              fit = h->GetFunction("gaus");

              ci2   = fit->GetChisquare();
              sigma = fit->GetParameter(2);
            }
          
          const Double_t mean  = fit->GetParameter(1);
          const Int_t ndf      = fit->GetNDF();
          
          *fLog << inf << "Mean number photo-electrons: " << sig[cnt] << endl;
          *fLog << inf << "Time Resolution area idx: " << aidx << " Results: " << endl;
          *fLog << inf << "Mean: " << Form("%4.3f",mean) 
                << "+-" << Form("%4.3f",fit->GetParError(1))
                << "  Sigma: " << Form("%4.3f",sigma) << "+-" << Form("%4.3f",fit->GetParError(2)) 
                << "  Chisquare: " << Form("%4.3f",fit->GetChisquare()) << "  NDF  : " << ndf << endl;          

          delete h;
          gROOT->GetListOfFunctions()->Remove(fit);

          if ((sigma < reserr[cnt] || reserr[cnt]<0.001) && ndf > 2)
            {
              res   [cnt] = mean;
              reserr[cnt] = sigma;
            }
	  else
	    *fLog << warn << "Do not take fit results, but Mean and RMS: " << Form("%4.3f",res[cnt]) << "+-" << Form("%4.3f",reserr[cnt]) << endl;

          res[cnt]    *= norm;
          reserr[cnt] *= norm;
	  cnt++;
        }
   }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                      sig.GetArray(),res.GetArray(),
                                      sigerr.GetArray(),reserr.GetArray());
  gr->SetTitle(Form("%s%3i","Area Index ",aidx));
  gr->GetXaxis()->SetTitle("<phes> [1]");
  gr->GetYaxis()->SetTitle("Time Resolution [ns]");      
  return gr;
}

TGraphErrors *MCalibrationIntensityRelTimeCam::GetTimeResVsSqrtPhePerArea( const Int_t aidx,const MCalibrationIntensityChargeCam &chargecam, const MGeomCam &geom, const MCalibrationCam::PulserColor_t col)
{
  
  const Int_t size = CountNumEntries(col);
  
  if (size == 0)
    return NULL;

  const Int_t asiz = chargecam.CountNumEntries(col);

  if (size != asiz)
    {
      *fLog << err << GetDescriptor() << ": Size mismatch in colour between MCalibrationIntensityRelTimeCam "
            << "and MCalibrationIntensityChargeCam. " << endl;
      return NULL;
    }
  
  const Float_t sqrt2   = 1.414;
  const Float_t fadc2ns = 3.333;
  const Float_t norm    = fadc2ns / sqrt2;

  TArrayF res(size);
  TArrayF reserr(size);
  TArrayF sig(size);
  TArrayF sigerr(size);
  
  Int_t cnt = 0;

  TH1D *h = 0;

  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationRelTimeCam *relcam = (MCalibrationRelTimeCam*)GetCam(i);
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)chargecam.GetCam(i);

      if (relcam->GetPulserColor() != col && col != MCalibrationCam::kNONE)
        continue;

      const MCalibrationChargePix &apix = (MCalibrationChargePix&)cam->GetAverageArea(aidx);
      const Float_t phe          = apix.GetPheFFactorMethod();
      const Float_t phesq        = phe > 0. ? TMath::Sqrt(phe) : 0.;
      const Float_t phesqerr     = phe > 0. ? 0.5*apix.GetPheFFactorMethodErr()/phesq : 0.;

      sig[cnt]       = phesq;
      sigerr[cnt]    = phesqerr;

      Double_t resol  = 0.;
      Double_t resol2 = 0.;
      Double_t var    = 0.;
      Int_t    num    = 0;

      MHCamera camres(geom,"CamRes","Time Resolution;Time Resolution [ns];channels");
      //
      // Get the area calibration pix from the calibration cam
      //
      for (Int_t j=0; j<cam->GetSize(); j++)
        {
          const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*cam)[j];
          const MCalibrationRelTimePix &relpix = (MCalibrationRelTimePix&)(*relcam)[j];
          //
          // Don't use bad pixels
          //
          if (!pix.IsFFactorMethodValid())
            continue;
          //
          //
          if (aidx != geom[j].GetAidx())
            continue;
          
          const Float_t pres = relpix.GetTimePrecision();

          resol  += pres;
          resol2 += pres;
          num++;
          
          camres.Fill(j,pres);
          camres.SetUsed(j);
        }
      
      if (num > 1)
        {
          resol /= num;
          var  = (resol2 - resol*resol*num) / (num-1);

          res[cnt] = resol;
          if (var > 0.)
            reserr[cnt] = TMath::Sqrt(var);
          else
            reserr[cnt] = 0.;

          //
          // Make also a Gauss-fit to the distributions. The RMS can be determined by 
          // outlier, thus we look at the sigma and the RMS and take the smaller one, afterwards.
          // 
          h = camres.ProjectionS(TArrayI(),TArrayI(1,&aidx),"_py",750);
          h->SetDirectory(NULL);
          h->Fit("gaus","QL");
          TF1 *fit = h->GetFunction("gaus");

          Float_t ci2   = fit->GetChisquare();
          Float_t sigma = fit->GetParameter(2);

          if (ci2 > 500. || sigma > reserr[cnt])
            {
              h->Fit("gaus","QLM");
              fit = h->GetFunction("gaus");

              ci2   = fit->GetChisquare();
              sigma = fit->GetParameter(2);
            }
          
          const Float_t mean  = fit->GetParameter(1);
          const Float_t ndf   = fit->GetNDF();

          
          *fLog << inf << "Sqrt Mean number photo-electrons: " << sig[cnt] << endl;
          *fLog << inf << "Time Resolution area idx: " << aidx << " Results: " << endl;
          *fLog << inf << "Mean: " << Form("%4.3f",mean) 
                << "+-" << Form("%4.3f",fit->GetParError(1))
                << "  Sigma: " << Form("%4.3f",sigma) << "+-" << Form("%4.3f",fit->GetParError(2)) 
                << "  Chisquare: " << Form("%4.3f",fit->GetChisquare()) << "  NDF  : " << ndf << endl;          

          delete h;
          gROOT->GetListOfFunctions()->Remove(fit);

          if (sigma < reserr[cnt] && ndf > 2)
            {
              res   [cnt] = mean;
              reserr[cnt] = sigma;
            }

          res[cnt]    *= norm;
          reserr[cnt] *= norm;
        }
      else
        {
          res[cnt]    = -1.;
          reserr[cnt] = 0.;
        }
      cnt++;
   }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                      sig.GetArray(),res.GetArray(),
                                      sigerr.GetArray(),reserr.GetArray());
  gr->SetTitle(Form("%s%3i","Area Index ",aidx));
  gr->GetXaxis()->SetTitle("Sqrt(<phes>) [1]");
  gr->GetYaxis()->SetTitle("Time Resolution [ns]");      
  return gr;
}
