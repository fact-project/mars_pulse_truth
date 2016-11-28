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
// MCalibrationIntensityChargeCam                                               
//                                                               
// Storage container for intensity charge calibration results. 
// 
// Individual MCalibrationChargeCam's can be retrieved with: 
// - GetCam() yielding the current cam.
// - GetCam("name") yielding the current camera with name "name".
// - GetCam(i) yielding the i-th camera.
//
// See also: MCalibrationIntensityCam, MCalibrationChargeCam,
//           MCalibrationChargePix, MCalibrationChargeCalc, MCalibrationQECam
//           MCalibrationBlindCam, MCalibrationChargePINDiode
//           MHCalibrationChargePix, MHCalibrationChargeCam              
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationIntensityChargeCam.h"

#include <TF1.h>
#include <TH2.h>
#include <TGraphErrors.h>
#include <TOrdCollection.h>
#include <TH1.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MHCamera.h"

#include "MGeomCamMagic.h"
#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MCalibrationChargeCam.h"
#include "MCalibrationChargePix.h"

ClassImp(MCalibrationIntensityChargeCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCalibrationIntensityChargeCam::MCalibrationIntensityChargeCam(const char *name, const char *title)
{

  fName  = name  ? name  : "MCalibrationIntensityChargeCam";
  fTitle = title ? title : "Results of the Intensity Calibration";
  
  InitSize(1);
}

// -------------------------------------------------------------------
//
// Add MCalibrationChargeCam's in the ranges from - to. 
//
void MCalibrationIntensityChargeCam::Add(const UInt_t from, const UInt_t to)
{
  for (UInt_t i=from; i<to; i++)
    fCams->AddAt(new MCalibrationChargeCam,i);
}

// -------------------------------------------------------------------
//
// Returns a TGraphErrors with the number of photo-electrons vs. 
// the extracted signal of pixel "pixid". 
//
TGraphErrors *MCalibrationIntensityChargeCam::GetPheVsCharge( const UInt_t pixid, const MCalibrationCam::PulserColor_t col)
{
  
  Int_t size = CountNumEntries(col);
  
  if (size == 0)
    return NULL;
  
  TArrayF phe(size);
  TArrayF pheerr(size);
  TArrayF sig(size);
  TArrayF sigerr(size);
  
  Int_t cnt = 0;

  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam(i);

      if (col != MCalibrationCam::kNONE)
        if (cam->GetPulserColor() != col)
          continue;
      //
      // Get the calibration pix from the calibration cam
      //
      MCalibrationChargePix &pix = (MCalibrationChargePix&)(*cam)[pixid];
      //
      // Don't use bad pixels
      //
      if (!pix.IsFFactorMethodValid())
        continue;
      //
      phe[cnt]    = pix.GetPheFFactorMethod();
      pheerr[cnt] = pix.GetPheFFactorMethodErr();
      //
      // For the calculation of Q, we have to use the 
      // converted value!
      //
      sig   [cnt] = pix.GetConvertedMean();
      sigerr[cnt] = pix.GetConvertedMeanErr();
      cnt++;
    }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                     sig.GetArray(),phe.GetArray(),
                                     sigerr.GetArray(),pheerr.GetArray());
  gr->SetTitle(Form("%s%3i","Pixel ",pixid));
  gr->GetXaxis()->SetTitle("Q [FADC counts]");
  gr->GetYaxis()->SetTitle("photo-electrons [1]");      
  return gr;
}

// -------------------------------------------------------------------
//
// Returns a TGraphErrors with the mean effective number of photo-electrons divided by 
// the mean charge of that pixel vs. the mean number of photo-electrons.
//
TGraphErrors *MCalibrationIntensityChargeCam::GetPhePerCharge( const UInt_t pixid, const MGeomCam &geom, const MCalibrationCam::PulserColor_t col)
{
  
  Int_t size = CountNumValidEntries(pixid,col);
  
  if (size == 0)
    return NULL;
  
  TArrayF phepersig(size);
  TArrayF phepersigerr(size);
  TArrayF sig(size);
  TArrayF sigerr(size);
  
  Int_t cnt = 0;

  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam(i);

      if (col != MCalibrationCam::kNONE)
        if (cam->GetPulserColor() != col)
          continue;
      //
      // Get the calibration pix from the calibration cam
      //
      const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*cam)[pixid];
      //
      // Don't use bad pixels
      //
      if (!pix.IsFFactorMethodValid())
        continue;
      //
      // For the calculation of Q, we have to use the 
      // converted value!
      //
      const Int_t aidx = geom[pixid].GetAidx();
      const MCalibrationChargePix &apix = (MCalibrationChargePix&)cam->GetAverageArea(aidx);

      const Float_t q    = pix.GetConvertedMean();
      const Float_t qerr = pix.GetConvertedMeanErr();
      //
      const Float_t phe    = apix.GetPheFFactorMethod();
      const Float_t pheerr = apix.GetPheFFactorMethodErr();

      sig[cnt]       = phe;
      sigerr[cnt]    = pheerr;


      phepersig[cnt] = q > 0.00001 ? phe/q : -1.;

      Float_t var = 0.;

      if (q > 0.00001 && phe > 0.00001)
        {
          var = pheerr * pheerr / phe / phe + qerr*qerr/q/q;
          if (var > 0.00001)
            var = TMath::Sqrt(var)*phepersig[cnt];
        }
      phepersigerr[cnt] = var;
      cnt++;
    }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                     sig.GetArray(),phepersig.GetArray(),
                                     sigerr.GetArray(),phepersigerr.GetArray());
  gr->SetTitle(Form("%s%3i","Pixel ",pixid));
  gr->GetXaxis()->SetTitle("<photo-electrons> [1]");
  gr->GetYaxis()->SetTitle("<phes> / <Q> [FADC cts^{-1}]");      
  return gr;
}

// -------------------------------------------------------------------
//
// Returns a TGraphErrors with the mean effective number of photo-electrons divided by 
// the mean charge of that pixel vs. the mean number of photo-electrons.
//
TGraphErrors *MCalibrationIntensityChargeCam::GetPhePerChargePerArea( const Int_t aidx, const MGeomCam &geom, const MCalibrationCam::PulserColor_t col)
{
  
  Int_t size = CountNumEntries(col);
  
  if (size == 0)
    return NULL;
  
  TArrayF phepersig(size);
  TArrayF phepersigerr(size);
  TArrayF sig(size);
  TArrayF sigerr(size);
  
  Int_t cnt = 0;

  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam(i);

      if (col != MCalibrationCam::kNONE)
        if (cam->GetPulserColor() != col)
          continue;
      //
      // Get the calibration pix from the calibration cam
      //
      const MCalibrationChargePix &apix = (MCalibrationChargePix&)cam->GetAverageArea(aidx);
      const Float_t phe          = apix.GetPheFFactorMethod();
      const Float_t pherelvar    = apix.GetPheFFactorMethodRelVar();
      const Float_t pheerr       = apix.GetPheFFactorMethodErr();

      sig[cnt]       = phe;
      sigerr[cnt]    = pheerr;

      Double_t sig1 = 0.;
      Double_t sig2 = 0.;
      Int_t    num  = 0;

      for (Int_t j=0; j<cam->GetSize(); j++)
        {
          const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*cam)[j];
          //
          // Don't use bad pixels
          //
          if (!pix.IsFFactorMethodValid())
            continue;
          //
          //
          if (aidx != geom[j].GetAidx())
            continue;
          
          sig1 += pix.GetConvertedMean();
          sig2 += pix.GetConvertedMean() * pix.GetConvertedMean();
          num++;
        }
      
      if (num > 1)
        {
          sig1 /= num;

          Double_t var = (sig2 - sig1*sig1*num) / (num-1);
          var /= sig1*sig1;
          var += pherelvar;

          phepersig[cnt] = phe/sig1;
          if (var > 0.)
            phepersigerr[cnt] = TMath::Sqrt(var) * phepersig[cnt];
          else
            phepersigerr[cnt] = 0.;
        }
      else
        {
          phepersig[cnt]    = -1.;
          phepersigerr[cnt] = 0.;
        }
      cnt++;
    }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                     sig.GetArray(),phepersig.GetArray(),
                                     sigerr.GetArray(),phepersigerr.GetArray());
  gr->SetTitle(Form("Conv. Factors Area %d Average",aidx));
  gr->GetXaxis()->SetTitle("<photo-electrons> [1]");
  gr->GetYaxis()->SetTitle("<phes> / <Q> [FADC cts^{-1}]");      
  return gr;
}

// -------------------------------------------------------------------
//
// Returns a TGraphErrors with the number of photo-electrons vs. 
// the extracted signal over all pixels with area index "aidx". 
//
// The points represent the means of the pixels values, while the error bars
// the sigma of the pixels values.  
//
TGraphErrors *MCalibrationIntensityChargeCam::GetPheVsChargePerArea( const Int_t aidx, const MCalibrationCam::PulserColor_t col)
{
  
  Int_t size = CountNumEntries(col);
  
  TArrayF phe(size);
  TArrayF pheerr(size);
  TArrayF sig(size);
  TArrayF sigerr(size);
  
  Int_t cnt = 0;

  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam(i);

      if (col != MCalibrationCam::kNONE)
        if (cam->GetPulserColor() != col)
          continue;

      //
      // Get the area calibration pix from the calibration cam
      //
      MCalibrationChargePix &pix = (MCalibrationChargePix&)(cam->GetAverageArea(aidx));

      phe[cnt]    = pix.GetPheFFactorMethod();
      pheerr[cnt] = pix.GetPheFFactorMethodErr();
      //
      // For the calculation of Q, we have to use the 
      // converted value!
      //
      sig   [cnt] = pix.GetConvertedMean();
      sigerr[cnt] = pix.GetConvertedMeanErr();

      cnt++;
    }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                      sig.GetArray(),phe.GetArray(),
                                      sigerr.GetArray(),pheerr.GetArray());
  gr->SetTitle(Form("%s%3i","Area Index ",aidx));
  gr->GetXaxis()->SetTitle("Q [FADC counts]");
  gr->GetYaxis()->SetTitle("photo-electrons [1]");      
  return gr;
}

// -------------------------------------------------------------------
//
// Returns a TGraphErrors with the 'Razmik plot' of pixel "pixid". 
// The Razmik plot shows the value of 'R' vs. 1/Q where:
//
//        sigma^2         F^2
//  R =   -------    =  ------
//         <Q>^2        <m_pe>
//
// and 1/Q is the inverse (mean) extracted signal
//
TGraphErrors *MCalibrationIntensityChargeCam::GetRazmikPlot( const UInt_t pixid )
{
  
  const Int_t size = GetSize();
  
  TArrayF r(size);
  TArrayF rerr(size);
  TArrayF oneoverq(size);
  TArrayF oneoverqerr(size);
  
  for (Int_t i=0;i<size;i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam(i);
      //
      // Get the calibration pix from the calibration cam
      //
      MCalibrationChargePix &pix = (MCalibrationChargePix&)(*cam)[pixid];
      //
      // Don't use bad pixels
      //
      if (!pix.IsFFactorMethodValid())
        continue;
      //
      // For the calculation of R, use the un-converted values, like 
      // in the calibration, since: 
      //                 C^2*sigma^2     sigma^2
      //  R(lowgain) =   -----------  =  ------  = R
      //                 C^2*<Q>^2       <Q>^2
      //
      const Float_t mean = pix.GetMean();
      const Float_t meanerr = pix.GetMeanErr();
      const Float_t rsigma = pix.GetRSigma();
      const Float_t rsigmaerr = pix.GetRSigmaErr();
      r[i]    = rsigma*rsigma/mean/mean;
      const Float_t rrelvar = 4.*rsigmaerr*rsigmaerr/rsigma/rsigma + 4.*meanerr*meanerr/mean/mean;
      rerr[i] = rrelvar * r[i] * r[i];
      rerr[i] = rerr[i] <= 0 ? 0. : TMath::Sqrt(rerr[i]);
      //
      // For the calculation of 1/Q, we have to use the 
      // converted value!
      //
      const Float_t q  = pix.GetConvertedMean();
      const Float_t qe = pix.GetConvertedMeanErr();
      oneoverq   [i] = 1./q;
      oneoverqerr[i] = qe / (q * q);
    }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                     oneoverq.GetArray(),r.GetArray(),
                                     oneoverqerr.GetArray(),rerr.GetArray());
  gr->SetTitle(Form("%s%3i","Pixel ",pixid));
  gr->GetXaxis()->SetTitle("1/Q [FADC counts^{-1}]");
  gr->GetYaxis()->SetTitle("\\sigma_{red}^{2}/Q^{2} [1]");
  return gr;
}

// -------------------------------------------------------------------
//
// Returns a 2-dimensional histogram with the fit results of the 
// 'Razmik plot' for each pixel of area index "aidx" (see GetRazmikPlot())
// 
// The results of the polynomial fit of grade 1 are: 
//
// x-axis: Offset (Parameter 0 of the polynomial)
// y-axis: Slope  (Parameter 1 of the polynomial)
//
// The offset is a measure of how well-known the supposed additional contributions 
// to the value "reduced sigma" are. Because a photo-multiplier is a linear instrument, 
// the excess fluctuations are linear w.r.t. the signal amplitude and can be expressed by 
// the proportionality constant F (the "F-Factor"). 
// Adding noise from outside (e.g. night sky background) modifies the recorded noise, but 
// not the mean extracted signal, due to the AC-coupling. Thus, noise contributions from outside
// (e.g. calculating the pedestal RMS)have to be subtracted from the recorded signal fluctuations 
// in order to retrieve the linearity relation: 
//
// sigma(signal)^2 / mean(signal)^2  = sigma^2 / <Q>^2 = F^2 / <n_phe>               (1) 
//
// Any systematic offset in the sigma(signal) will produce an offset in the "Razmik plot"), 
// characterized by the Offset of the polynomial fit. Thus, in an ideal case, all pixels have their
// "offset" centered very closely around zero.
//
// The "slope" is the proportionality constant F^2, multiplied with the conversion factor 
// phe's to mean signal (because the "Razmik plot" plots the left side of eq. (1) w.r.t. 
// 1/<Q> instead of 1/<n_phe>. However, the mean number of photo-electrons <n_phe> can be 
// expressed by <Q> with the relation:
//
//  <n_phe> = c_phe * <Q>                                                            (2)
//
// Thus: 
//
// 1/<n_phe> = 1/c_phe * 1/<Q>                                                       (3) 
// 
// and:
//
// Slope = F^2 / c_phe
//
// In the ideal case of having equal photo-multipliers and a perfectly flat-fielded camera,
// the "slope" -values should thus all be closely centered around F^2/c_phe. 
// 
TH2F *MCalibrationIntensityChargeCam::GetRazmikPlotResults( const Int_t aidx, const MGeomCam &geom)
{
  
  TH2F *hist = new TH2F("hist","R vs. Inverse Charges - Fit results",45,-0.02,0.02,45,0.,30.);
  hist->SetXTitle("Offset [FADC counts^{-1}]");
  hist->SetYTitle("F^{2} / <n_phe>/<Q> [FADC count / phe]");  
  hist->SetFillColor(kRed+aidx);

  MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam();

  for (Int_t npix=0;npix<cam->GetSize();npix++)
    {

      if (geom[npix].GetAidx() == aidx)
        {
          TGraph *gr = GetRazmikPlot(npix);
          gr->Fit("pol1","Q");
          hist->Fill(gr->GetFunction("pol1")->GetParameter(0),gr->GetFunction("pol1")->GetParameter(1));
        }
    }
  return hist;
}


// --------------------------------------------------------------------
//
// Returns the number of camera entries matching the required colour 
// and the requirement that pixel "pixid" has been correctly calibrated
//
Int_t MCalibrationIntensityChargeCam::CountNumValidEntries(const UInt_t pixid, const MCalibrationCam::PulserColor_t col) const
{

  Int_t nvalid = 0;

  for (Int_t i=0;i<GetSize();i++)
    {
      const MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam(i);
      const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*cam)[pixid];

      if (col == MCalibrationCam::kNONE)
        {
          if (pix.IsFFactorMethodValid())
            nvalid++;
        }
      else
        {
          if (cam->GetPulserColor() == col)
            {
              if (pix.IsFFactorMethodValid())
                nvalid++;
            }
        }
    }
  
  return nvalid;
}


// -------------------------------------------------------------------
//
// Returns a TGraphErrors with the development of the number of 
// photo-electrons vs. camera number for pixel 'pixid' 
//
TGraphErrors *MCalibrationIntensityChargeCam::GetVarVsTime( const Int_t pixid , const Option_t *varname )
{
  
  const Int_t size = GetSize();

  if (size == 0)
    return NULL;
  
  TString option(varname);
  option.ToLower();

  TArrayF nr(size);
  TArrayF nrerr(size);
  TArrayF var   (size);
  TArrayF varerr(size);
  
  for (Int_t i=0;i<size;i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam(i);
      //
      // Get the calibration pix from the calibration cam
      //
      MCalibrationChargePix &pix = (MCalibrationChargePix&)(*cam)[pixid];
      //
      nr[i]    = i;
      nrerr[i] = 0.;
      var[i]    = -1.;
      varerr[i] = -1.;
      //
      // Don't use bad pixels
      //
      if (!pix.IsFFactorMethodValid())
        continue;
      //
      if (option.Contains("rsigma"))
        {
          var   [i] = pix.GetRSigma();
          varerr[i] = pix.GetRSigmaErr();
        }
      if (option.Contains("abstimemean"))
        {
          var   [i] = pix.GetAbsTimeMean();
          varerr[i] = pix.GetAbsTimeRms();
        }
      if (option.Contains("abstimerms"))
        {
          var   [i] = pix.GetAbsTimeRms();
          varerr[i] = pix.GetAbsTimeRms()/2.;
        }
      if (option.Contains("blackout"))
        {
          var   [i] = pix.GetNumBlackout();
          varerr[i] = 0.;
        }
      if (option.Contains("pickup"))
        {
          var   [i] = pix.GetNumPickup();
          varerr[i] = 0.;
        }
      if (option.Contains("outlier"))
        {
          var   [i] = pix.GetNumPickup() + pix.GetNumBlackout();
          varerr[i] = 0.;
        }
      if (option.Contains("conversionhilo"))
        {
          var   [i] = pix.GetConversionHiLo();
          varerr[i] = pix.GetConversionHiLoErr();
        }
      if (option.Contains("convertedmean"))
        {
          var   [i] = pix.GetConvertedMean();
          varerr[i] = pix.GetConvertedMeanErr();
        }
      if (option.Contains("convertedsigma"))
        {
          var   [i] = pix.GetConvertedSigma();
          varerr[i] = pix.GetConvertedSigmaErr();
        }
      if (option.Contains("convertedrsigma"))
        {
          var   [i] = pix.GetConvertedRSigma();
          varerr[i] = pix.GetConvertedRSigmaErr();
        }
      if (option.Contains("meanconvfadc2phe"))
        {
          var   [i] = pix.GetMeanConvFADC2Phe();
          varerr[i] = pix.GetMeanConvFADC2PheErr();
        }
      if (option.Contains("meanffactorfadc2phot"))
        {
          var   [i] = pix.GetMeanFFactorFADC2Phot();
          varerr[i] = pix.GetMeanFFactorFADC2PhotErr();
        }
      if (option.Contains("ped"))
        {
          var   [i] = pix.GetPed();
          varerr[i] = pix.GetPedErr();
        }
      if (option.Contains("pedrms"))
        {
          var   [i] = pix.GetPedRms();
          varerr[i] = pix.GetPedRmsErr();
        }
      if (option.Contains("pheffactormethod"))
        {
          var   [i] = pix.GetPheFFactorMethod();
          varerr[i] = pix.GetPheFFactorMethodErr();
        }
      if (option.Contains("rsigmapercharge"))
        {
          var   [i] = pix.GetRSigmaPerCharge();
          varerr[i] = pix.GetRSigmaPerChargeErr();
        }
      if (option.Contains("conversionfactor"))
        {
          const MCalibrationChargePix &apix = (MCalibrationChargePix&)cam->GetAverageArea(0);
          const Float_t mean = pix.GetConvertedMean();
          const Float_t phe  = apix.GetPheFFactorMethod();

          var[i]    = phe/mean;
          varerr[i] = TMath::Sqrt(apix.GetPheFFactorMethodErr()*apix.GetPheFFactorMethodErr()/mean/mean
                                  + phe*phe/mean/mean/mean/mean*pix.GetConvertedMeanErr()*pix.GetConvertedMeanErr());
        }
  }
  
  
  TGraphErrors *gr = new TGraphErrors(size,
                                     nr.GetArray(),var.GetArray(),
                                     nrerr.GetArray(),varerr.GetArray());
  gr->SetTitle(Form("%s%3i","Pixel ",pixid));
  gr->GetXaxis()->SetTitle("Camera Nr.");
  //  gr->GetYaxis()->SetTitle("<Q> [FADC cnts]");
  return gr;
}

// --------------------------------------------------------------------------------
//
// Returns a TGraphErrors with a pre-defined variable with name (handed over in 'opt')
// per area index 'aidx' vs. the calibration camera number 
//
TGraphErrors *MCalibrationIntensityChargeCam::GetVarPerAreaVsTime( const Int_t aidx, const MGeomCam &geom, const Option_t *varname)
{
  
  const Int_t size = GetSize();
  
  if (size == 0)
    return NULL;

  TString option(varname);
  option.ToLower();

  TArrayF vararea(size);
  TArrayF varareaerr(size);
  TArrayF nr(size);
  TArrayF nrerr(size);
  
  TH1D *h = 0;

  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam(i);

      //
      // Get the calibration pix from the calibration cam
      //
      Double_t variab   = 0.;
      Double_t variab2  = 0.;
      Double_t variance = 0.;
      Int_t    num      = 0;
      Float_t  pvar     = 0.;

      MHCamera camcharge(geom,"CamCharge","Variable;;channels");
      //
      // Get the area calibration pix from the calibration cam
      //
      for (Int_t j=0; j<cam->GetSize(); j++)
        {
          const MCalibrationChargePix &pix = (MCalibrationChargePix&)(*cam)[j];
          //
          // Don't use bad pixels
          //
          if (!pix.IsFFactorMethodValid())
            continue;
          //
          //
          if (aidx != geom[j].GetAidx())
            continue;

          pvar = 0.;
          
          if (option.Contains("rsigma"))
              pvar = pix.GetRSigma();
          if (option.Contains("abstimemean"))
              pvar = pix.GetAbsTimeMean();
          if (option.Contains("abstimerms"))
              pvar = pix.GetAbsTimeRms();
          if (option.Contains("conversionhilo"))
              pvar = pix.GetConversionHiLo();
          if (option.Contains("convertedmean"))
              pvar = pix.GetConvertedMean();
          if (option.Contains("convertedsigma"))
              pvar = pix.GetConvertedSigma();
          if (option.Contains("convertedrsigma"))
              pvar = pix.GetConvertedRSigma();
          if (option.Contains("meanconvfadc2phe"))
              pvar = pix.GetMeanConvFADC2Phe();
          if (option.Contains("meanffactorfadc2phot"))
              pvar = pix.GetMeanFFactorFADC2Phot();
          if (option.Contains("ped"))
              pvar = pix.GetPed();
          if (option.Contains("pedrms"))
              pvar = pix.GetPedRms();
          if (option.Contains("pheffactormethod"))
              pvar = pix.GetPheFFactorMethod();
          if (option.Contains("rsigmapercharge"))
              pvar = pix.GetRSigmaPerCharge();
          if (option.Contains("conversionfactor"))
          {
              const MCalibrationChargePix &apix = (MCalibrationChargePix&)cam->GetAverageArea(aidx);
              pvar = apix.GetPheFFactorMethod()/pix.GetConvertedMean();
          }


          variab  += pvar;
          variab2 += pvar*pvar;
          num++;
          
          camcharge.Fill(j,pvar);
          camcharge.SetUsed(j);
        }
      
      if (num > 1)
        {
          variab  /= num;
          variance = (variab2 - variab*variab*num) / (num-1);

          vararea[i]    = variab;
          varareaerr[i] = variance>0 ? TMath::Sqrt(variance/num) : 999999999.;

          //
          // Make also a Gauss-fit to the distributions. The RMS can be determined by 
          // outlier, thus we look at the sigma and the RMS and take the smaller one, afterwards.
          // 
          h = camcharge.ProjectionS(TArrayI(),TArrayI(1,&aidx),"_py",750);
          h->SetDirectory(NULL);
          h->Fit("gaus","QL");
          TF1 *fit = h->GetFunction("gaus");

          Float_t ci2   = fit->GetChisquare();
          Float_t sigma = fit->GetParameter(2);

          if (ci2 > 500. || sigma > varareaerr[i])
            {
              h->Fit("gaus","QLM");
              fit = h->GetFunction("gaus");

              ci2   = fit->GetChisquare();
              sigma = fit->GetParameter(2);
            }
          
          const Float_t mean  = fit->GetParameter(1);
          const Float_t ndf   = fit->GetNDF();
          
          *fLog << inf << "Camera Nr: " << i << endl;
          *fLog << inf << option.Data() << " area idx: " << aidx << " Results: " << endl;
          *fLog << inf << "Mean: " << Form("%4.3f",mean) 
                << "+-" << Form("%4.3f",fit->GetParError(1))
                << "  Sigma: " << Form("%4.3f",sigma) << "+-" << Form("%4.3f",fit->GetParError(2)) 
                << "  Chisquare: " << Form("%4.3f",ci2) << "  NDF  : " << ndf << endl;
          delete h;
          gROOT->GetListOfFunctions()->Remove(fit);

          if (sigma<varareaerr[i] && ndf>2 && ci2<500.)
            {
              vararea   [i] = mean;
              varareaerr[i] = sigma/TMath::Sqrt((Float_t)num);
            }
        }
      else
        {
          vararea[i]    = -1.;
          varareaerr[i] = 0.;
        }

      nr[i] = i;
      nrerr[i] = 0.;
    }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                     nr.GetArray(),vararea.GetArray(),
                                     nrerr.GetArray(),varareaerr.GetArray());
  gr->SetTitle(Form("%s Area %3i Average",option.Data(),aidx));
  gr->GetXaxis()->SetTitle("Camera Nr.");
  //  gr->GetYaxis()->SetTitle("<Q> [1]");      
  return gr;
}


// -------------------------------------------------------------------
//
// Returns a TGraphErrors with the mean effective number of photon
// vs. the calibration camera number. With the string 'method', different
// calibration methods can be called.
//
TGraphErrors *MCalibrationIntensityChargeCam::GetPhotVsTime( const Option_t *method )
{
  
  const Int_t size = GetSize();
  
  if (size == 0)
    return NULL;

  TString option(method);

  TArrayF photarr(size);
  TArrayF photarrerr(size);
  TArrayF nr(size);
  TArrayF nrerr(size);
  
  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam(i);

      //
      // Get the calibration pix from the calibration cam
      //
      Float_t phot    = 0.;
      Float_t photerr = 0.;

      if (option.Contains("BlindPixel"))
        {
          phot    = cam->GetNumPhotonsBlindPixelMethod();
          photerr = cam->GetNumPhotonsBlindPixelMethodErr();
        }
      if (option.Contains("FFactor"))
        {
          phot    = cam->GetNumPhotonsFFactorMethod();
          photerr = cam->GetNumPhotonsFFactorMethodErr();
        }
      if (option.Contains("PINDiode"))
        {
          phot    = cam->GetNumPhotonsPINDiodeMethod();
          photerr = cam->GetNumPhotonsPINDiodeMethodErr();
        }

      photarr[i]       = phot;
      photarrerr[i]    = photerr;

      nr[i] = i;
      nrerr[i] = 0.;
    }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                     nr.GetArray(),photarr.GetArray(),
                                     nrerr.GetArray(),photarrerr.GetArray());
  gr->SetTitle("Photons Average");
  gr->GetXaxis()->SetTitle("Camera Nr.");
  gr->GetYaxis()->SetTitle("<N_{phot}> [1]");
  return gr;
}

// -------------------------------------------------------------------
//
// Returns a TGraphErrors with the mean effective number of photo-electrons per 
// area index 'aidx' vs. the calibration camera number 
//
TGraphErrors *MCalibrationIntensityChargeCam::GetPhePerAreaVsTime( const Int_t aidx, const MGeomCam &geom)
{
  
  const Int_t size = GetSize();
  
  if (size == 0)
    return NULL;
  
  TArrayF phearea(size);
  TArrayF pheareaerr(size);
  TArrayF time(size);
  TArrayF timeerr(size);
  
  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam(i);

      //
      // Get the calibration pix from the calibration cam
      //
      const MCalibrationChargePix &apix = (MCalibrationChargePix&)cam->GetAverageArea(aidx);
      const Float_t phe          = apix.GetPheFFactorMethod();
      const Float_t pheerr       = apix.GetPheFFactorMethodErr();

      phearea[i]       = phe;
      pheareaerr[i]    = pheerr;

      time[i] = i;
      timeerr[i] = 0.;
    }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                     time.GetArray(),phearea.GetArray(),
                                     timeerr.GetArray(),pheareaerr.GetArray());
  gr->SetTitle(Form("Phes Area %d Average",aidx));
  gr->GetXaxis()->SetTitle("Camera Nr.");
  gr->GetYaxis()->SetTitle("<N_{phe}> [1]");
  return gr;
}

// -------------------------------------------------------------------
//
// Returns a TGraphErrors with the event-by-event averaged charge per 
// area index 'aidx' vs. the calibration camera number 
//
TGraphErrors *MCalibrationIntensityChargeCam::GetChargePerAreaVsTime( const Int_t aidx, const MGeomCam &geom)
{
  
  const Int_t size = GetSize();
  
  if (size == 0)
    return NULL;
  
  TArrayF chargearea(size);
  TArrayF chargeareaerr(size);
  TArrayF nr(size);
  TArrayF nrerr(size);
  
  for (Int_t i=0;i<GetSize();i++)
    {
      //
      // Get the calibration cam from the intensity cam
      //
      MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam(i);

      //
      // Get the calibration pix from the calibration cam
      //
      const MCalibrationChargePix &apix = (MCalibrationChargePix&)cam->GetAverageArea(aidx);
      const Float_t charge          = apix.GetConvertedMean();
      const Float_t chargeerr       = apix.GetConvertedSigma();

      chargearea[i]       = charge;
      chargeareaerr[i]    = chargeerr;

      nr[i]    = i;
      nrerr[i] = 0.;
    }
  
  TGraphErrors *gr = new TGraphErrors(size,
                                     nr.GetArray(),chargearea.GetArray(),
                                     nrerr.GetArray(),chargeareaerr.GetArray());
  gr->SetTitle(Form("Averaged Charges Area Idx %d",aidx));
  gr->GetXaxis()->SetTitle("Camera Nr.");
  gr->GetYaxis()->SetTitle("<Q> [FADC cnts]");      
  return gr;
}

TH1F *MCalibrationIntensityChargeCam::GetVarFluctuations( const Int_t aidx, const MGeomCam &geom, const Option_t *varname )
{
  
  const Int_t size = GetSize();
  
  if (size == 0)
    return NULL;
  
  TString option(varname);
  option.ToLower();
  
  TH1F *hist = new TH1F("hist",Form("%s - Rel. Fluctuations %s Pixel",option.Data(),aidx ? "Outer" : "Inner"),
                        200,0.,100.);
  hist->SetXTitle("Relative Fluctuation [%]");
  hist->SetYTitle("Nr. channels [1]");  
  hist->SetFillColor(kRed+aidx);

  MCalibrationChargeCam *cam = (MCalibrationChargeCam*)GetCam();

  //
  // Loop over pixels
  //
  for (Int_t npix=0;npix<cam->GetSize();npix++)
    {
      if (geom[npix].GetAidx() != aidx)
        continue;

      Double_t variab   = 0.;
      Double_t variab2  = 0.;
      Double_t variance = 0.;
      Int_t    num      = 0;
      Float_t  pvar     = 0.;
      Float_t  relrms   = 99.9;
      //
      // Loop over the Cams for each pixel
      //
      for (Int_t i=0; i<GetSize(); i++)
        {
          MCalibrationChargeCam *ccam = (MCalibrationChargeCam*)GetCam(i);
          //
          // Get the calibration pix from the calibration cam
          //
          MCalibrationChargePix &pix = (MCalibrationChargePix&)(*ccam)[npix];
          //
          // Don't use bad pixels
          //
          if (!pix.IsFFactorMethodValid())
            continue;

          if (option.Contains("rsigma"))
            pvar = pix.GetRSigma();
          if (option.Contains("abstimemean"))
            pvar = pix.GetAbsTimeMean();
          if (option.Contains("abstimerms"))
            pvar = pix.GetAbsTimeRms();
          if (option.Contains("conversionhilo"))
            pvar = pix.GetConversionHiLo();
          if (option.Contains("convertedmean"))
            pvar = pix.GetConvertedMean();
          if (option.Contains("convertedsigma"))
            pvar = pix.GetConvertedSigma();
          if (option.Contains("convertedrsigma"))
            pvar = pix.GetConvertedRSigma();
          if (option.Contains("meanconvfadc2phe"))
            pvar = pix.GetMeanConvFADC2Phe();
          if (option.Contains("meanffactorfadc2phot"))
            pvar = pix.GetMeanFFactorFADC2Phot();
          if (option.Contains("ped"))
            pvar = pix.GetPed();
          if (option.Contains("pedrms"))
            pvar = pix.GetPedRms();
          if (option.Contains("pheffactormethod"))
            pvar = pix.GetPheFFactorMethod();
          if (option.Contains("rsigmapercharge"))
            pvar = pix.GetRSigmaPerCharge();
          if (option.Contains("conversionfactor"))
          {
              const MCalibrationChargePix &apix = (MCalibrationChargePix&)ccam->GetAverageArea(0);
              pvar = apix.GetPheFFactorMethod()/pix.GetConvertedMean();
          }


          variab  += pvar;
          variab2 += pvar*pvar;
          num++;
        }

      if (num > 1)
        {
          variab  /= num;
          variance = (variab2 - variab*variab*num) / (num-1);

          if (variance > 0.)
            relrms = TMath::Sqrt(variance)/variab * 100.;
        }
      hist->Fill(relrms);
    }
  return hist;
}

void MCalibrationIntensityChargeCam::DrawRazmikPlot( const UInt_t pixid )
{
  TGraphErrors *gr = GetRazmikPlot(pixid );
  gr->SetBit(kCanDelete);
  gr->Draw("A*");

}
void MCalibrationIntensityChargeCam::DrawPheVsCharge( const UInt_t pixid, const MCalibrationCam::PulserColor_t col)
{
  TGraphErrors *gr = GetPheVsCharge(pixid,col);
  gr->SetBit(kCanDelete);
  gr->Draw("A*");
}
void MCalibrationIntensityChargeCam::DrawPhePerCharge( const UInt_t pixid, const MCalibrationCam::PulserColor_t col)
{
  TGraphErrors *gr = GetPhePerCharge(pixid,MGeomCamMagic(),col); 
  gr->SetBit(kCanDelete);
  gr->Draw("A*");
}
void MCalibrationIntensityChargeCam::DrawPhePerChargePerArea( const Int_t aidx, const MCalibrationCam::PulserColor_t col)
{
  TGraphErrors *gr = GetPhePerChargePerArea(aidx,MGeomCamMagic(),col); 
  gr->SetBit(kCanDelete);
  gr->Draw("A*");
}
void MCalibrationIntensityChargeCam::DrawPheVsChargePerArea( const Int_t aidx, const MCalibrationCam::PulserColor_t col)
{
  TGraphErrors *gr = GetPheVsChargePerArea(aidx,col); 
  gr->SetBit(kCanDelete);
  gr->Draw("A*");
}
void MCalibrationIntensityChargeCam::DrawRazmikPlotResults( const Int_t aidx)
{
  TH2F *h = GetRazmikPlotResults(aidx,MGeomCamMagic()); 
  h->SetBit(kCanDelete);
  h->Draw();
}

void MCalibrationIntensityChargeCam::DrawChargePerAreaVsTime( const Int_t aidx)
{
  TGraphErrors *gr = GetChargePerAreaVsTime(aidx,MGeomCamMagic()); 
  gr->SetBit(kCanDelete);
  gr->Draw("A*");
}
void MCalibrationIntensityChargeCam::DrawPhePerAreaVsTime( const Int_t aidx)
{
  TGraphErrors *gr = GetPhePerAreaVsTime(aidx,MGeomCamMagic()); 
  gr->SetBit(kCanDelete);
  gr->Draw("A*");
}
void MCalibrationIntensityChargeCam::DrawPhotVsTime( const Option_t *method)
{
  TGraphErrors *gr = GetPhotVsTime(method); 
  gr->SetBit(kCanDelete);
  gr->Draw("A*");
}

void MCalibrationIntensityChargeCam::DrawVarPerAreaVsTime( const Int_t aidx, const Option_t *varname )
{
  TGraphErrors *gr = GetVarPerAreaVsTime(aidx,MGeomCamMagic(),varname ); 
  gr->SetBit(kCanDelete);
  gr->Draw("A*");
}
void MCalibrationIntensityChargeCam::DrawVarVsTime( const Int_t pixid , const Option_t *varname )
{
  TGraphErrors *gr = GetVarVsTime(pixid,varname ); 
  gr->SetBit(kCanDelete);
  gr->Draw("A*");
}
void MCalibrationIntensityChargeCam::DrawVarFluctuations( const Int_t aidx, const Option_t *varname)
{
  TH1F *h = GetVarFluctuations( aidx,MGeomCamMagic(),varname); 
  h->SetBit(kCanDelete);
  h->Draw();
}
