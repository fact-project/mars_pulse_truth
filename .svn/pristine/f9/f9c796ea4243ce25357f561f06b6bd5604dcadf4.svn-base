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
!   Author(s): Thomas Bretz, 1/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Markus Gaug, 02/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MJCalibration
//
//  Do one calibration loop over serious of runs with the same pulser 
//  colour and the same intensity. The following containers (rectangular) and 
//  tasks (ellipses) are called to produce an MCalibrationChargeCam and to 
//  update the MCalibrationQECam: (MCalibrate is not called from this class)
//
//Begin_Html
/*
<img src="images/CalibClasses.gif">
*/
//End_Html
//
// Different signal extractors can be set with the command SetExtractor()
// Only extractors deriving from MExtractor can be set, default is MExtractSlidingWindow
//
// Different arrival time extractors can be set with the command SetTimeExtractor()
// Only extractors deriving from MExtractTime can be set, default is MExtractTimeHighestIntegral
//
// At the end of the eventloop, plots and results are displayed, depending on 
// the flags set (see DisplayResult()) 
// 
// If the flag SetFullDisplay() is set, all MHCameras will be displayed. 
// if the flag SetDataCheckDisplay() is set, only the most important ones are displayed 
// Otherwise, (default: SetNormalDisplay()), a good selection of plots is given
//
// The absolute light calibration devices Blind Pixel and PIN Diode can be switched on
// and off with the commands:
//
// - SetUseBlindPixel(Bool_t )
// - SetUsePINDiode(Bool_t )
//
// See also: MHCalibrationChargePix, MHCalibrationChargeCam, MHGausEvents
//           MHCalibrationChargeBlindPix, MHCalibrationChargePINDiode
//           MCalibrationChargePix, MCalibrationChargeCam, MCalibrationChargeCalc
//           MCalibrationBlindPix, MCalibrationChargePINDiode,
//           MCalibrationQECam, MBadPixelsPix, MBadPixelsCam
//
// If the flag RelTimeCalibration() is set, a calibration of the relative arrival 
// times is also performed. The following containers (rectangular) and 
// tasks (ellipses) are called to produce an MCalibrationRelTimeCam used by  
// MCalibrateTime to correct timing offset between pixels: (MCalibrateTime is not 
// called from this class)
//
//Begin_Html
/*
<img src="images/RelTimeClasses.gif">
*/
//End_Html
//
// Different arrival time extractors can be set directly with the command
// SetTimeExtractor(MExtractor *)
//
// Resource file entries are case sensitive!
//
// See also: MHCalibrationRelTimePix, MHCalibrationRelTimeCam, MHGausEvents
//           MCalibrationRelTimePix, MCalibrationRelTimeCam
//           MBadPixelsPix, MBadPixelsCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MJCalibration.h"

#include <TFile.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TLine.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TRegexp.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TEnv.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MEnv.h"
#include "MString.h"
#include "MDirIter.h"
#include "MSequence.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MHCamera.h"
#include "MGeomCam.h"

#include "MCalibrationPatternDecode.h"
#include "MCalibrationCam.h"
#include "MCalibrationQECam.h"
#include "MCalibrationQEPix.h"
#include "MCalibrationChargeCam.h"
#include "MCalibrationChargePix.h"
#include "MCalibrationChargePINDiode.h"
#include "MCalibrationBlindPix.h"
#include "MCalibrationBlindCam.h"
#include "MCalibrationBlindCamOneOldStyle.h"
#include "MCalibrationBlindCamTwoNewStyle.h"
#include "MCalibrationBlindCamThreeNewStyle.h"
#include "MCalibrationChargeCalc.h"
#include "MCalibColorSet.h"
#include "MCalibrationRelTimeCam.h"
#include "MCalibrationRelTimeCalc.h"

#include "MHGausEvents.h"
#include "MHCalibrationCam.h"
#include "MHCalibrationChargeCam.h"
#include "MHCalibrationChargeBlindCam.h"
#include "MHCalibrationChargePINDiode.h"
#include "MHCalibrationRelTimeCam.h"
#include "MHCalibrationPix.h"

#include "MHCamEvent.h"

#include "MReadMarsFile.h"
#include "MPedCalcPedRun.h"
#include "MRawFileRead.h"
#include "MGeomApply.h"
#include "MPedestalSubtract.h"
#include "MTaskEnv.h"
#include "MBadPixelsMerge.h"
#include "MBadPixelsCam.h"
#include "MExtractTime.h"
#include "MExtractor.h"
#include "MExtractPINDiode.h"
#include "MExtractBlindPixel.h"
#include "MExtractTimeAndChargeSpline.h"
#include "MFCosmics.h"
#include "MFTriggerPattern.h"
#include "MContinue.h"
#include "MFillH.h"

#include "MTriggerPatternDecode.h"   

#include "MArrivalTimeCam.h"

#include "MStatusDisplay.h"

ClassImp(MJCalibration);

using namespace std;

const Int_t MJCalibration::gkIFAEBoxInaugurationRun        = 20113;
const Int_t MJCalibration::gkSecondBlindPixelInstallation  = 31693;
const Int_t MJCalibration::gkSpecialPixelsContInstallation = 34057;
const Int_t MJCalibration::gkThirdBlindPixelInstallation   = 43308;
const TString MJCalibration::fgReferenceFile = "mjobs/calibrationref.rc";
const TString MJCalibration::fgHiLoCalibFile = "resources/hilocalib.rc";

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// - fExtractor to NULL, fTimeExtractor to NULL, fColor to kNONE,
//   fDisplay to kNormalDisplay, kRelTimes to kFALSE, kataCheck to kFALSE, kDebug to kFALSE
// - SetUseBlindPixel()
// - SetUsePINDiode()
//
MJCalibration::MJCalibration(const char *name, const char *title) 
    : fExtractor(NULL), fTimeExtractor(NULL), 
      fColor(MCalibrationCam::kNONE), fDisplayType(kDataCheckDisplay),
      fMinEvents(1000), fGeometry("MGeomCamMagic")
{

  fName  = name  ? name  : "MJCalibration";
  fTitle = title ? title : "Tool to create the calibration constants for one calibration run";

  //SetHiLoCalibration();
  SetRelTimeCalibration();
  SetDebug(kFALSE);

  SetReferenceFile();
  SetHiLoCalibFile();

  fConvFADC2PheMin           = 0.;
  fConvFADC2PheMax           = 1.5;
  fConvFADC2PhotMin          = 0.; 
  fConvFADC2PhotMax          = 10.; 
  fQEMin                     = 0.;
  fQEMax                     = 0.3;
  fArrivalTimeMin            = 1.;
  fArrivalTimeMax            = 10.;
  fTimeOffsetMin             = -3.;
  fTimeOffsetMax             =  3.;
  fTimeResolutionMin         =  0.;
  fTimeResolutionMax         =  1.;

  fRefFADC2PheInner          = 0.14;
  fRefFADC2PheOuter          = 0.4;
  fRefConvFADC2PheInner      = 0.14;
  fRefConvFADC2PheOuter      = 0.52;
  fRefQEInner                = 0.18;
  fRefQEOuter                = 0.12;
  fRefArrivalTimeInner       = 4.5;
  fRefArrivalTimeOuter       = 5.0;
  fRefArrivalTimeRmsInner    = 0.5;
  fRefArrivalTimeRmsOuter    = 0.5;
  fRefTimeOffsetOuter        = 0.62;
  fRefTimeResolutionInner    = 0.12;
  fRefTimeResolutionOuter    = 0.09;
}

void MJCalibration::DrawTab(MParList &plist, const char *cont, const char *name, Option_t *opt)
{
    TObject *obj = plist.FindObject(cont);
    if (!obj)
        return;

    fDisplay->AddTab(name);
    obj->DrawClone(opt);
}

MHCamera *MJCalibration::DrawBadPixelPad(const MHCamera &h, Bool_t unsuit) const
{
    MHCamera *obj=(MHCamera*)h.DrawCopy("hist");

    gStyle->SetPalette(1);

    const Int_t numcol = gStyle->GetNumberOfColors();

    const Double_t min = 1;
    const Double_t max = unsuit ? MBadPixelsPix::GetNumUnsuitable() : MBadPixelsPix::GetNumUnreliable();
    const Double_t f   = (numcol-1)/(max-min);

    FixDataCheckHist(*obj, min, max);

    TPaveText *pave = new TPaveText(0.05, 0.012, 0.975, 0.999);

    const Double_t height = (pave->GetY2()-pave->GetY1())/(max+1);

    pave->SetBit(kCanDelete);
    pave->ConvertNDCtoPad();
    pave->SetFillColor(14);
    pave->Draw();

    Int_t n=0;
    while (1)
    {
        const TString name = unsuit ? MBadPixelsPix::GetUnsuitableName(++n) : MBadPixelsPix::GetUnreliableName(++n);
        if (name.IsNull())
            break;

        Int_t cnt = 0;
        for (UInt_t pix=0; pix<h.GetNumPixels(); pix++)
            if (TMath::Nint(h.GetPixContent(pix)) == n)
                cnt++;

        const TString loc = unsuit?MBadPixelsPix::GetUnsuitableName(n):MBadPixelsPix::GetUnreliableName(n);

        const TString left  = Form("%d) %s", n, loc.Data());
        const TString right = Form("%3i pixels", cnt);

        const Int_t col = gStyle->GetColorPalette(TMath::FloorNint((n-1)*f));

        TText *p = pave->AddText(0.05, pave->GetY2()-height*(n+0.3), left);
        p->SetTextColor(col);
        p->SetTextAlign(12);

        p = pave->AddText(0.95, p->GetY(), right);
        p->SetTextColor(col);
        p->SetTextAlign(32);
    }

    return obj;
}

// --------------------------------------------------------------------------
//
// Display the results in MStatusDisplay: 
//
// - Add "Calibration" to the MStatusDisplay title
// - Retrieve the MGeomCam from MParList
// - Initialize the following MHCamera's:
//   1)  MCalibrationPix::GetMean()
//   2)  MCalibrationPix::Sigma()
//   3)  MCalibrationChargePix::GetRSigma()
//   4)  MCalibrationChargePix::GetRSigmaPerCharge()
//   5)  MCalibrationChargePix::GetPheFFactorMethod()
//   6)  MCalibrationChargePix::GetMeanConvFADC2Phe()
//   7)  MCalibrationChargePix::GetMeanFFactorFADC2Phot()
//   8)  MCalibrationQEPix::GetQECascadesFFactor()
//   9)  MCalibrationQEPix::GetQECascadesBlindPixel()
//   10) MCalibrationQEPix::GetQECascadesPINDiode()
//   11) MCalibrationQEPix::GetQECascadesCombined()
//   12) MCalibrationQEPix::IsAverageQEFFactorAvailable()
//   13) MCalibrationQEPix::IsAverageQEBlindPixelAvailable()
//   14) MCalibrationQEPix::IsAverageQEPINDiodeAvailable()
//   15) MCalibrationQEPix::IsAverageQECombinedAvailable()
//   16) MCalibrationChargePix::IsHiGainSaturation()
//   17) MCalibrationPix::GetHiLoMeansDivided()
//   18) MCalibrationPix::GetHiLoSigmasDivided()
//   19) MCalibrationChargePix::GetHiGainPickup()
//   20) MCalibrationChargePix::GetLoGainPickup()
//   21) MCalibrationChargePix::GetHiGainBlackout()
//   22) MCalibrationChargePix::GetLoGainBlackout()
//   23) MCalibrationPix::IsExcluded()
//   24) MBadPixelsPix::IsUnsuitable(MBadPixelsPix::kUnsuitableRun)
//   25) MBadPixelsPix::IsUnsuitable(MBadPixelsPix::kUnreliableRun)
//   26) MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kHiGainOscillating)
//   27) MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kLoGainOscillating)
//   28) MCalibrationChargePix::GetAbsTimeMean()
//   29) MCalibrationChargePix::GetAbsTimeRms()
//
// If the flag SetFullDisplay() is set, all MHCameras will be displayed. 
// if the flag SetDataCheckDisplay() is set, only the most important ones are displayed 
// and otherwise, (default: SetNormalDisplay()), a good selection of plots is given
//
void MJCalibration::DisplayResult(MParList &plist)
{
    if (!fDisplay)
        return;

    TString drawoption = "nonew ";
    if (fDisplayType == kDataCheckDisplay)
        drawoption += "datacheck";
    if (fDisplayType == kFullDisplay)
        drawoption += "all";

    if (IsUsePINDiode())
        DrawTab(plist, "MHCalibrationChargePINDiode", "PINDiode",  drawoption);
    if (IsUseBlindPixel())
        DrawTab(plist, "MHCalibrationChargeBlindCam", "BlindPix",  drawoption);
    if (IsRelTimes())
        DrawTab(plist, "MHCalibrationRelTimeCam",     "Time",      drawoption);
    DrawTab(plist, "MHCalibrationChargeCam",          "Charge",    drawoption);

    //
    // Update display
    //
    TString title = "--  Calibration: ";
    title += fSequence.GetSequence();
    title += "  --";
    fDisplay->SetTitle(title, kFALSE);

    //
    // Get container from list
    //
    MGeomCam &geomcam = *(MGeomCam*)plist.FindObject("MGeomCam");

    // Create histograms to display
    MHCamera disp1 (geomcam, "Charge",            "Fitted Mean Signal (Charges)");
    MHCamera disp2 (geomcam, "SigmaCharge",       "Sigma of Fitted Signal");
    MHCamera disp3 (geomcam, "RSigma",            "Reduced Sigmas");
    MHCamera disp4 (geomcam, "RSigmaPerCharge",   "Reduced Sigma per Charge");
    MHCamera disp5 (geomcam, "NumPhes",           "Number Photo-electrons");
    MHCamera disp6 (geomcam, "ConvFADC2Phes",     "Phes per Charge (Before Flat-Field)");
    MHCamera disp7 (geomcam, "TotalFFactor",      "Total F-Factor(F-Factor Method)");
    MHCamera disp8 (geomcam, "CascadesQEFFactor", "Cascades QE (F-Factor Method)");
    MHCamera disp9 (geomcam, "CascadesQEBlindPix","Cascades QE (Blind Pixel Method)");
    MHCamera disp10(geomcam, "CascadesQEPINDiode","Cascades QE (PIN Diode Method)");
    MHCamera disp11(geomcam, "CascadesQECombined","Cascades QE (Combined Method)");
    MHCamera disp12(geomcam, "FFactorValid",      "Pixels with Valid F-Factor Calibration");
    MHCamera disp13(geomcam, "BlindPixelValid",   "Pixels with valid BlindPixel Calibration");
    MHCamera disp14(geomcam, "PINdiodeValid",     "Pixels with Valid PINDiode Calibration");
    MHCamera disp15(geomcam, "CombinedValid",     "Pixels with Valid Combined Calibration");
    MHCamera disp16(geomcam, "Saturation",        "Pixels with Saturated Hi Gain");
    MHCamera disp17(geomcam, "ConversionMeans",   "Conversion HiGain.vs.LoGain Means");
    MHCamera disp18(geomcam, "ConversionSigmas",  "Conversion HiGain.vs.LoGain Sigmas");
    MHCamera disp19(geomcam, "HiGainPickup",      "Number Pickup Events Hi Gain");
    MHCamera disp20(geomcam, "LoGainPickup",      "Number Pickup Events Lo Gain");
    MHCamera disp21(geomcam, "HiGainBlackout",    "Number Blackout Events Hi Gain");
    MHCamera disp22(geomcam, "LoGainBlackout",    "Number Blackout Events Lo Gain");
    MHCamera disp23(geomcam, "Excluded",          "Pixels Previously Excluded");
    MHCamera disp24(geomcam, "UnSuitable",        "Pixels NOT Suited for Further Analysis");
    MHCamera disp25(geomcam, "UnReliable",        "Pixels Suitable, but NOT Reliable for Further Analysis");
    MHCamera disp26(geomcam, "HiGainOscillating", "Oscillating Pixels High Gain");
    MHCamera disp27(geomcam, "LoGainOscillating", "Oscillating Pixels Low Gain");
    MHCamera disp28(geomcam, "AbsTimeMean",       "Abs. Arrival Times");
    MHCamera disp29(geomcam, "AbsTimeRms",        "RMS of Arrival Times");
    MHCamera disp30(geomcam, "MeanTime",          "Mean Rel. Arrival Times");
    MHCamera disp31(geomcam, "SigmaTime",         "Sigma Rel. Arrival Times");
    MHCamera disp32(geomcam, "TimeProb",          "Probability of Time Fit");
    MHCamera disp33(geomcam, "TimeNotFitValid",   "Pixels with not valid Fit Results");
    MHCamera disp34(geomcam, "TimeOscillating",   "Oscillating Pixels");
    MHCamera disp35(geomcam, "TotalConv",         "Conversion Factor to Photons");
    MHCamera disp36(geomcam, "RMSperMean",        "Charge histogram RMS per Mean");
    MHCamera disp37(geomcam, "TotalConvPhe",      "Conversion Factor to equiv. Phe's");

    // Fitted charge means and sigmas
    disp1.SetCamContent(fCalibrationCam,  0);
    disp1.SetCamError(  fCalibrationCam,  1);
    disp2.SetCamContent(fCalibrationCam,  2);
    disp2.SetCamError(  fCalibrationCam,  3);

    // Reduced Sigmas and reduced sigmas per charge
    disp3.SetCamContent(fCalibrationCam,  5);
    disp3.SetCamError(  fCalibrationCam,  6);
    disp4.SetCamContent(fCalibrationCam,  7);
    disp4.SetCamError(  fCalibrationCam,  8);

    // F-Factor Method
    disp5.SetCamContent(fCalibrationCam,  9);
    disp5.SetCamError(  fCalibrationCam, 10);
    disp6.SetCamContent(fCalibrationCam, 11);
    disp6.SetCamError(  fCalibrationCam, 12);
    disp7.SetCamContent(fCalibrationCam, 13);
    disp7.SetCamError(  fCalibrationCam, 14);

    // Quantum Efficiencies
    disp8.SetCamContent (fQECam, 0 );
    disp8.SetCamError   (fQECam, 1 );
    disp9.SetCamContent (fQECam, 2 );
    disp9.SetCamError   (fQECam, 3 );
    disp10.SetCamContent(fQECam, 4 );
    disp10.SetCamError  (fQECam, 5 );
    disp11.SetCamContent(fQECam, 6 );
    disp11.SetCamError  (fQECam, 7 );

    // Valid flags
    disp12.SetCamContent(fQECam, 8 );
    disp13.SetCamContent(fQECam, 9 );
    disp14.SetCamContent(fQECam, 10);
    disp15.SetCamContent(fQECam, 11);

    // Conversion Hi-Lo
    disp16.SetCamContent(fCalibrationCam, 25);
    disp17.SetCamContent(fCalibrationCam, 16);
    disp17.SetCamError  (fCalibrationCam, 17);
    disp18.SetCamContent(fCalibrationCam, 18);
    disp18.SetCamError  (fCalibrationCam, 19);

    // Pickup and Blackout
    disp19.SetCamContent(fCalibrationCam, 21);
    disp20.SetCamContent(fCalibrationCam, 22);
    disp21.SetCamContent(fCalibrationCam, 23);
    disp22.SetCamContent(fCalibrationCam, 24);

    // Pixels with defects
    disp23.SetCamContent(fCalibrationCam, 20);
    disp24.SetCamContent(fBadPixels, 6);
    disp25.SetCamContent(fBadPixels, 7);

    // Oscillations
    disp26.SetCamContent(fBadPixels, 10);
    disp27.SetCamContent(fBadPixels, 11);

    // Arrival Times
    disp28.SetCamContent(fCalibrationCam, 26);
    disp28.SetCamError(  fCalibrationCam, 27);
    disp29.SetCamContent(fCalibrationCam, 27);

    // RMS and Mean
    disp36.SetCamContent(fCalibrationCam,32);
    disp36.SetCamError(fCalibrationCam,33);

    disp1.SetYTitle("Q [FADC cnts]");
    disp2.SetYTitle("\\sigma_{Q} [FADC cnts]");

    disp3.SetYTitle("\\sqrt{\\sigma^{2}_{Q} - RMS^{2}_{Ped}} [FADC cnts]");
    disp4.SetYTitle("Red.Sigma/<Q> [1]");

    disp5.SetYTitle("Photo-electons [1]");
    disp6.SetYTitle("Phes/<Q> [FADC cnts^{-1}]");
    disp7.SetYTitle("Total F-Factor [1]");

    disp8.SetYTitle("QE [1]");
    disp9.SetYTitle("QE [1]");
    disp10.SetYTitle("QE [1]");
    disp11.SetYTitle("QE [1]");

    disp12.SetYTitle("[1]");
    disp13.SetYTitle("[1]");
    disp14.SetYTitle("[1]");
    disp15.SetYTitle("[1]");
    disp16.SetYTitle("[1]");

    disp17.SetYTitle("<Q>(High)/<Q>(Low) [1]");
    disp18.SetYTitle("\\sigma_{Q}(High)/\\sigma_{Q}(Low) [1]");

    disp19.SetYTitle("[1]");
    disp20.SetYTitle("[1]");
    disp21.SetYTitle("[1]");
    disp22.SetYTitle("[1]");
    //    disp23.SetYTitle("[1]");
    //    disp24.SetYTitle("[1]");
    //    disp25.SetYTitle("[1]");
    disp26.SetYTitle("[1]");
    disp27.SetYTitle("[1]");

    disp28.SetYTitle("Mean Abs. Time [FADC sl.]");
    disp29.SetYTitle("RMS Abs. Time [FADC sl.]");
    disp35.SetYTitle("Conv.Factor [Ph/FADC cnts]");
    disp36.SetYTitle("Charge RMS/<Q> [1]");
    disp37.SetYTitle("Conv.Factor [Phe/FADC cnts]");

    for (UInt_t i=0;i<geomcam.GetNumPixels();i++)
      {

        MCalibrationChargePix &pix = (MCalibrationChargePix&)fCalibrationCam[i];
        MCalibrationQEPix     &qe  = (MCalibrationQEPix&)fQECam[i];

        if (!pix.IsFFactorMethodValid())
          continue;

        const Float_t convphe  = pix.GetMeanConvFADC2Phe();
        const Float_t quaeff   = qe.GetQECascadesFFactor();

        disp35.Fill(i,convphe/quaeff);
        disp35.SetUsed(i);

        disp37.Fill(i,convphe/quaeff*MCalibrationQEPix::gkDefaultAverageQE);
        disp37.SetUsed(i);
      }
    

    if (IsRelTimes())
    {
        disp30.SetCamContent(fRelTimeCam, 0);
        disp30.SetCamError(  fRelTimeCam, 1);
        disp31.SetCamContent(fRelTimeCam, 2);
        disp31.SetCamError(  fRelTimeCam, 3);
        disp32.SetCamContent(fRelTimeCam, 4);
        disp33.SetCamContent(fBadPixels, 20);
        disp34.SetCamContent(fBadPixels, 21);

        disp30.SetYTitle("Time Offset [FADC units]");
        disp31.SetYTitle("Timing resolution [FADC units]");
        disp32.SetYTitle("P_{Time} [1]");
        disp33.SetYTitle("[1]");
        disp34.SetYTitle("[1]");
    }

    if (fDisplayType == kDataCheckDisplay)
    {
      // -------------------- FitCharge -------------------

      TCanvas &c1 = fDisplay->AddTab("FitCharge");
      c1.Divide(3, 3);

      disp1.CamDraw( c1, 1, 3, 6);   // MEAN CHARGES
      disp36.CamDraw(c1, 2, 3, 6);   // RMS per Charge
      disp5.CamDraw( c1, 3, 3, 6);   // PHOTO ELECTRONS

      // -------------------- Conversion -------------------

      TCanvas &c2 = fDisplay->AddTab("Conversion");
      c2.Divide(3,3);

      disp6.SetMinMax(fConvFADC2PheMin, fConvFADC2PheMax);
      disp8.SetMinMax(fQEMin, fQEMax);
      disp37.SetMinMax(fConvFADC2PheMin, fConvFADC2PheMax);

      disp6.CamDraw( c2, 1, 3, 6);   // CONVERSION FACTORS
      disp8.CamDraw( c2, 2, 3, 6);   // QUANTUM EFFICIENCY
      disp37.CamDraw(c2, 3, 3, 6);   // CONVERSION FADC TO PHOTONS

      c2.cd(1);
      DisplayReferenceLines(disp6, 2);

      c2.cd(2);
      DisplayReferenceLines(disp8, 0);

      c2.cd(3);
      DisplayReferenceLines(disp37, 1);

      // -------------------- AbsTimes -------------------

      TCanvas &c3 = fDisplay->AddTab("AbsTimes");
      c3.Divide(2,3);

      disp28.SetMinMax(fArrivalTimeMin, fArrivalTimeMax);

      disp28.CamDraw(c3, 1, 2, 6);  // Arrival times
      disp29.CamDraw(c3, 2, 2, 6);  // Arrival times rms

      c3.cd(1);
      DisplayReferenceLines(disp28, 3);

      c3.cd(2);
      DisplayReferenceLines(disp29, 4);

      if (IsRelTimes())
      {
          // -------------------- RelTimes -------------------

          TCanvas &c5 = fDisplay->AddTab("RelTimes");
          c5.Divide(2,3);

          disp30.SetMinMax(fTimeOffsetMin, fTimeOffsetMax);
          disp31.SetMinMax(fTimeResolutionMin, fTimeResolutionMax);

          disp30.CamDraw(c5, 1, 2, 6); // MEAN REL. ARR. TIMES
          disp31.CamDraw(c5, 2, 2, 6); // JITTER Rel. Arr. Times

          c5.cd(1);
          DisplayReferenceLines(disp30, 5);

          c5.cd(2);
          DisplayReferenceLines(disp31, 6);
      }

      // -------------------- Unsuitable -------------------

      //
      // UNSUITABLE PIXELS
      //
      TCanvas &c4 = fDisplay->AddTab("Defect");
      c4.Divide(2,2, 0.005, 0.005);

      c4.cd(1);
      gPad->SetBorderMode(0);
      gPad->SetTicks();

      MHCamera *obj8 = DrawBadPixelPad(disp24, kTRUE);

      c4.cd(3);
      gPad->SetBorderMode(0);
      obj8->SetPrettyPalette();
      obj8->Draw();

      //
      // UNRELIABLE PIXELS
      // 
      c4.cd(2);
      gPad->SetBorderMode(0);
      gPad->SetTicks();

      MHCamera *obj9 = DrawBadPixelPad(disp25, kFALSE);

      c4.cd(4);
      gPad->SetBorderMode(0);
      obj9->SetPrettyPalette();
      obj9->Draw();
      return;
    }

    if (fDisplayType == kNormalDisplay)
      {

        // Charges
        TCanvas &c11 = fDisplay->AddTab("FitCharge");
        c11.Divide(2, 4);

        disp1.CamDraw(c11, 1, 2, 5, 1);
        disp2.CamDraw(c11, 2, 2, 5, 1);

        // Reduced Sigmas
        TCanvas &c12 = fDisplay->AddTab("RedSigma");
        c12.Divide(2,4);

        disp3.CamDraw(c12, 1, 2, 5, 1);
        disp4.CamDraw(c12, 2, 2, 5, 1);

        //  F-Factor
        TCanvas &c13 = fDisplay->AddTab("Phe's");
        c13.Divide(3,4);

        disp5.CamDraw(c13, 1, 3, 5, 1);
        disp6.CamDraw(c13, 2, 3, 5, 1);
        disp7.CamDraw(c13, 3, 3, 5, 1);

        // QE's
        TCanvas &c14 = fDisplay->AddTab("QE's");
        c14.Divide(4,4);

        disp8.CamDraw(c14, 1, 4, 5, 1);
        disp9.CamDraw(c14, 2, 4, 5, 1);
        disp10.CamDraw(c14, 3, 4, 5, 1);
        disp11.CamDraw(c14, 4, 4, 5, 1);

        // Defects
        TCanvas &c15 = fDisplay->AddTab("Defect");
        //      c15.Divide(5,2);
        c15.Divide(4,2);

        /*
        disp23.CamDraw(c15, 1, 5, 0);
        disp24.CamDraw(c15, 2, 5, 0);
        disp25.CamDraw(c15, 3, 5, 0);
        disp26.CamDraw(c15, 4, 5, 0);
        disp27.CamDraw(c15, 5, 5, 0);
        */
        disp24.CamDraw(c15, 1, 4, 0);
        disp25.CamDraw(c15, 2, 4, 0);
        disp26.CamDraw(c15, 3, 4, 0);
        disp27.CamDraw(c15, 4, 4, 0);

        // Abs. Times
        TCanvas &c16 = fDisplay->AddTab("AbsTimes");
        c16.Divide(2,3);

        disp28.CamDraw(c16, 1, 2, 5);
        disp29.CamDraw(c16, 2, 2, 5);

        if (IsRelTimes())
        {
            // Rel. Times
            TCanvas &c17 = fDisplay->AddTab("RelTimes");
            c17.Divide(2,4);

            disp30.CamDraw(c17, 1, 2, 5, 1);
            disp31.CamDraw(c17, 2, 2, 5, 1);
        }

        return;
    }

    if (fDisplayType == kFullDisplay)
    {
        MHCalibrationCam *cam = (MHCalibrationCam*)plist.FindObject("MHCalibrationChargeCam");

        for (Int_t sector=1;sector<cam->GetAverageSectors();sector++)
        {
            cam->GetAverageHiGainSector(sector).DrawClone("all");
            cam->GetAverageLoGainSector(sector).DrawClone("all");
        }

        // Charges
        TCanvas &c21 = fDisplay->AddTab("FitCharge");
        c21.Divide(2, 4);

        disp1.CamDraw(c21, 1, 2, 2, 1);
        disp2.CamDraw(c21, 2, 2, 2, 1);

        // Reduced Sigmas
        TCanvas &c23 = fDisplay->AddTab("RedSigma");
        c23.Divide(2,4);

        disp3.CamDraw(c23, 1, 2, 2, 1);
        disp4.CamDraw(c23, 2, 2, 2, 1);

        //  F-Factor
        TCanvas &c24 = fDisplay->AddTab("Phe's");
        c24.Divide(3,5);

        disp5.CamDraw(c24, 1, 3, 2, 1, 1);
        disp6.CamDraw(c24, 2, 3, 2, 1, 1);
        disp7.CamDraw(c24, 3, 3, 2, 1, 1);

        // QE's
        TCanvas &c25 = fDisplay->AddTab("QE's");
        c25.Divide(4,5);

        disp8.CamDraw(c25, 1, 4, 2, 1, 1);
        disp9.CamDraw(c25, 2, 4, 2, 1, 1);
        disp10.CamDraw(c25, 3, 4, 2, 1, 1);
        disp11.CamDraw(c25, 4, 4, 2, 1, 1);

        // Validity
        TCanvas &c26 = fDisplay->AddTab("Valid");
        c26.Divide(4,2);

        disp12.CamDraw(c26, 1, 4, 0);
        disp13.CamDraw(c26, 2, 4, 0);
        disp14.CamDraw(c26, 3, 4, 0);
        disp15.CamDraw(c26, 4, 4, 0);

        // Other info
        TCanvas &c27 = fDisplay->AddTab("HiLoGain");
        c27.Divide(3,3);
        
        disp16.CamDraw(c27, 1, 3, 0);
        disp17.CamDraw(c27, 2, 3, 1);
        disp18.CamDraw(c27, 3, 3, 1);

        // Pickup
        TCanvas &c28 = fDisplay->AddTab("Pickup");
        c28.Divide(4,2);

        disp19.CamDraw(c28, 1, 4, 0);
        disp20.CamDraw(c28, 2, 4, 0);
        disp21.CamDraw(c28, 3, 4, 0);
        disp22.CamDraw(c28, 4, 4, 0);

        // Defects
        TCanvas &c29 = fDisplay->AddTab("Defect");
        //      c29.Divide(5,2);
        c29.Divide(4,2);

        disp24.CamDraw(c29, 1, 4, 0);
        disp25.CamDraw(c29, 2, 4, 0);
        disp26.CamDraw(c29, 3, 4, 0);
        disp27.CamDraw(c29, 4, 4, 0);

        // Abs. Times
        TCanvas &c30 = fDisplay->AddTab("AbsTimes");
        c30.Divide(2,3);

        disp28.CamDraw(c30, 1, 2, 2);
        disp29.CamDraw(c30, 2, 2, 1);

        if (IsRelTimes())
        {
            // Rel. Times
            TCanvas &c31 = fDisplay->AddTab("RelTimes");
            c31.Divide(3,5);

            disp30.CamDraw(c31, 1, 3, 2, 1, 1);
            disp31.CamDraw(c31, 2, 3, 2, 1, 1);
            disp32.CamDraw(c31, 3, 3, 4, 1, 1);

            // Time Defects
            TCanvas &c32 = fDisplay->AddTab("DefTime");
            c32.Divide(2,2);

            disp33.CamDraw(c32, 1, 2, 0);
            disp34.CamDraw(c32, 2, 2, 0);

            MHCalibrationCam *ccam = (MHCalibrationCam*)plist.FindObject("MHCalibrationRelTimeCam");

            for (Int_t sector=1;sector<ccam->GetAverageSectors();sector++)
            {
                ccam->GetAverageHiGainSector(sector).DrawClone("fourierevents");
                ccam->GetAverageLoGainSector(sector).DrawClone("fourierevents");
            }

        }

        return;
    }
}

void  MJCalibration::DisplayReferenceLines(const MHCamera &hist, const Int_t what) const
{
    MHCamera *cam = dynamic_cast<MHCamera*>(gPad->FindObject(hist.GetName()));
    if (!cam)
        return;

  const MGeomCam *geom = cam->GetGeometry();

  const Double_t x = geom->InheritsFrom("MGeomCamMagic") ? 397 : cam->GetNbinsX() ;

  TLine line;
  line.SetLineStyle(kDashed);
  line.SetLineWidth(3);
  line.SetLineColor(kBlue);

  TLine *l1 = NULL;

  switch (what)
    {
    case 0:
      l1 = line.DrawLine(0, fRefQEInner, x, fRefQEInner);      
      break;
    case  1:
      l1 = line.DrawLine(0, fRefConvFADC2PheInner, x, fRefConvFADC2PheInner);
      break;
    case  2:
      l1 = line.DrawLine(0, fRefFADC2PheInner, x, fRefFADC2PheInner );
      break;
    case  3:
      l1 = line.DrawLine(0, fRefArrivalTimeInner, x, fRefArrivalTimeInner );
      break;
    case  4:
      l1 = line.DrawLine(0, fRefArrivalTimeRmsInner, x, fRefArrivalTimeRmsInner );
      break;
    case 5:
      l1 = line.DrawLine(0, 0, x, 0);
      break;
    case 6:
      l1 = line.DrawLine(0, fRefTimeResolutionInner, x, fRefTimeResolutionInner );
      break;
    default:
      break;
    }

  if (geom->InheritsFrom("MGeomCamMagic"))
    {
      const Double_t x2 = cam->GetNbinsX();

      switch (what)
        {
        case 0:
          line.DrawLine(x2, fRefQEOuter, 398, fRefQEOuter);      
          break;
        case 1:
          line.DrawLine(x2, fRefConvFADC2PheOuter, 398, fRefConvFADC2PheOuter );
          break;
        case 2:
          line.DrawLine(x2, fRefFADC2PheOuter, 398, fRefFADC2PheOuter);
          break;
        case 3:
          line.DrawLine(x2, fRefArrivalTimeOuter, 398, fRefArrivalTimeOuter);
          break;
        case 4:
          line.DrawLine(x2, fRefArrivalTimeRmsOuter, 398, fRefArrivalTimeRmsOuter);
          break;
        case 5:
          line.DrawLine(x2, fRefTimeOffsetOuter, 398, fRefTimeOffsetOuter);
          break;
        case 6:
          line.DrawLine(x2, fRefTimeResolutionOuter, 398, fRefTimeResolutionOuter);
          break;
	default:
	  break;
        }
    }

  TLegend *leg = new TLegend(0.6,0.85,0.9 ,0.95);
  leg->SetBit(kCanDelete);
  leg->AddEntry(l1, "Reference","l");
  leg->Draw();
}

/*
void  MJCalibration::DisplayOutliers(TH1D *hist, const char* whatsmall, const char* whatbig) const
{

  const Int_t kNotDraw = 1<<9;
  TF1 *f = hist->GetFunction("gaus");
  f->ResetBit(kNotDraw);

  const Float_t mean  = f->GetParameter(1);
  const Float_t lolim = mean - 4.0*f->GetParameter(2);
  const Float_t uplim = mean + 4.0*f->GetParameter(2);
  const Stat_t  dead  = hist->Integral(0,hist->FindBin(lolim)-1);
  const Stat_t  noisy = hist->Integral(hist->FindBin(uplim)+1,hist->GetNbinsX()+1);

  const Double_t max = hist->GetBinContent(hist->GetMaximumBin());                                           
  
  const Double_t minl = hist->GetBinCenter(hist->GetXaxis()->GetFirst());
  const Double_t maxl = hist->GetBinCenter(hist->GetXaxis()->GetLast());

  TLatex deadtex;
  deadtex.SetTextSize(0.07);
  deadtex.DrawLatex(minl+0.015*(maxl-minl),max/1.1,
                    Form("%3i %s pixels",(Int_t)dead,whatsmall));

  TLatex noisytex;
  noisytex.SetTextSize(0.07);
  noisytex.DrawLatex(minl+0.015*(maxl-minl),max/1.2,
                     Form("%3i %s pixels",(Int_t)noisy,whatbig));

}
*/

void MJCalibration::FixDataCheckHist(TH1D &h, Double_t min, Double_t max)
{
    h.SetDirectory(NULL);
    h.SetStats(kFALSE);
    h.SetMinimum(min);
    h.SetMaximum(max);

    //
    // set the labels bigger
    //
    TAxis *xaxe = h.GetXaxis();
    TAxis *yaxe = h.GetYaxis();

    xaxe->CenterTitle();
    yaxe->CenterTitle();    
    xaxe->SetTitleSize(0.06);
    yaxe->SetTitleSize(0.06);    
    xaxe->SetTitleOffset(0.8);
    yaxe->SetTitleOffset(0.85);    
    xaxe->SetLabelSize(0.05);
    yaxe->SetLabelSize(0.05);    
}

// --------------------------------------------------------------------------
//
// Retrieve the output file written by WriteResult()
// 
const char* MJCalibration::GetOutputFileName() const
{
    return Form("calib%08d.root", fSequence.GetSequence());
}

// --------------------------------------------------------------------------
//
// Read the following values from resource file:
//
//   ConvFADC2PheMin
//   ConvFADC2PheMax
//   ConvFADC2PhotMin
//   ConvFADC2PhotMax
//
//   QEMin
//   QEMax
//
//   ArrivalTimeMin
//   ArrivalTimeMax
//
//   TimeOffsetMin
//   TimeOffsetMax
//   TimeResolutionMin
//   TimeResolutionMax
//
//   RefConvFADC2PheInner
//   RefConvFADC2PheOuter
//   RefConvFADC2PhotInner
//   RefConvFADC2PhotOuter
//
//   RefQEInner
//   RefQEOuter
//
//   RefArrivalTimeInner
//   RefArrivalTimeOuter
//   RefArrivalTimeRmsInner
//   RefArrivalTimeRmsOuter
//
//   RefTimeOffsetOuter
//   RefTimeResolutionInner
//   RefTimeResolutionOuter
//
void MJCalibration::ReadReferenceFile()
{
    TEnv refenv(fReferenceFile);

    fConvFADC2PheMin        = refenv.GetValue("ConvFADC2PheMin",fConvFADC2PheMin);
    fConvFADC2PheMax        = refenv.GetValue("ConvFADC2PheMax",fConvFADC2PheMax);
    fConvFADC2PhotMin       = refenv.GetValue("ConvFADC2PhotMin",fConvFADC2PhotMin);
    fConvFADC2PhotMax       = refenv.GetValue("ConvFADC2PhotMax",fConvFADC2PhotMax);
    fQEMin                  = refenv.GetValue("QEMin",fQEMin);
    fQEMax                  = refenv.GetValue("QEMax",fQEMax);
    fArrivalTimeMin         = refenv.GetValue("ArrivalTimeMin",fArrivalTimeMin);
    fArrivalTimeMax         = refenv.GetValue("ArrivalTimeMax",fArrivalTimeMax);
    fTimeOffsetMin          = refenv.GetValue("TimeOffsetMin",fTimeOffsetMin);
    fTimeOffsetMax          = refenv.GetValue("TimeOffsetMax",fTimeOffsetMax);
    fTimeResolutionMin      = refenv.GetValue("TimeResolutionMin",fTimeResolutionMin);
    fTimeResolutionMax      = refenv.GetValue("TimeResolutionMax",fTimeResolutionMax);
			     		                        
    fRefFADC2PheInner       = refenv.GetValue("RefFADC2PheInner",fRefFADC2PheInner);
    fRefFADC2PheOuter       = refenv.GetValue("RefFADC2PheOuter",fRefFADC2PheOuter);
    fRefConvFADC2PhotInner  = refenv.GetValue("RefConvFADC2PhotInner",fRefConvFADC2PhotInner);
    fRefConvFADC2PhotOuter  = refenv.GetValue("RefConvFADC2PhotOuter",fRefConvFADC2PhotOuter);
    fRefConvFADC2PheInner   = refenv.GetValue("RefConvFADC2PheInner",fRefConvFADC2PheInner);
    fRefConvFADC2PheOuter   = refenv.GetValue("RefConvFADC2PheOuter",fRefConvFADC2PheOuter);
    fRefQEInner             = refenv.GetValue("RefQEInner",fRefQEInner);
    fRefQEOuter             = refenv.GetValue("RefQEOuter",fRefQEOuter);
    fRefArrivalTimeInner    = refenv.GetValue("RefArrivalTimeInner",fRefArrivalTimeInner);
    fRefArrivalTimeOuter    = refenv.GetValue("RefArrivalTimeOuter",fRefArrivalTimeOuter);
    fRefArrivalTimeRmsInner = refenv.GetValue("RefArrivalTimeRmsInner",fRefArrivalTimeRmsInner);
    fRefArrivalTimeRmsOuter = refenv.GetValue("RefArrivalTimeRmsOuter",fRefArrivalTimeRmsOuter);
    fRefTimeOffsetOuter     = refenv.GetValue("RefTimeOffsetOuter",fRefTimeOffsetOuter);
    fRefTimeResolutionInner = refenv.GetValue("RefTimeResolutionInner",fRefTimeResolutionInner);
    fRefTimeResolutionOuter = refenv.GetValue("RefTimeResolutionOuter",fRefTimeResolutionOuter);
}

// --------------------------------------------------------------------------
//
// Read the following values from resource file.
//
Bool_t MJCalibration::ReadHiLoCalibFile()
{
    if (fExtractor && !fExtractor->HasLoGain())
        return kTRUE;

//    if (!fIsHiLoCalibration)
//        return kTRUE;

    // We use the night time stamp to determine the period
    // because the night must be in the sequence file
    const MTime &night = fSequence.GetNight();
    const Int_t period = night.GetMagicPeriod();

    // Open resource file
    MEnv env(fHiLoCalibFile);
    if (!env.IsValid())
    {
        *fLog << err << "ERROR - Resource file " << fHiLoCalibFile;
        *fLog << " could not be opened... abort." << endl;
        return kFALSE;
    }

    // Defined resource id
    const TString id = fSequence.IsMonteCarlo() ? "MC" : Form("%02d", period);

    // Check for a valid entry for the correct period
    TString fname = env.GetValue(id, "");
    if (fname.IsNull())
    {
        *fLog << err << "ERROR - No entry for resource id '" << id;
        *fLog << "' found in " << fHiLoCalibFile << "... looking for default." << endl;
        return kFALSE;
/*
        *fLog << warn << "WARNING - No entry for period " << period;
        *fLog << " found in " << fHiLoCalibFile << "... looking for default." << endl;

        fname = env.GetValue("00", "");
        if (fname.IsNull())
        {
            *fLog << err << "ERROR - No default entry (00) found in ";
            *fLog << fHiLoCalibFile << "... abort." << endl;
            return kFALSE;
        }*/
    }

    *fLog << inf << "Reading Hi-/Lo-Gain calibration constants from " << fname << endl;

    // Open file with calibration constants
    TFile file(fname, "READ");
    if (!file.IsOpen())
    {
        *fLog << err << "ERROR - Couldn't open file " << fname << " for reading... abort." << endl;
        return kFALSE;
    }

    // read calibration constants
    MHCamEvent hilocam;
    if (hilocam.Read()<=0)
    {
        *fLog << err << "ERROR - Unable to read MHCamEvent from " << fname << "... abort." << endl;
        return kFALSE;
    }

    // Get histogram with constants
    MHCamera *hist = hilocam.GetHist();
    if (!hist)
    {
        *fLog << err << "ERROR - MHCamEvent from " << fname << " empty... abort." << endl;
        return kFALSE;
    }

    // Do some sanity stuff
    if (fCalibrationCam.GetSize() < 1)
        fCalibrationCam.InitSize(hist->GetNumPixels());
  
    if (fBadPixels.GetSize() < 1)
        fBadPixels.InitSize(hist->GetNumPixels());
  
    if ((UInt_t)fCalibrationCam.GetSize() != hist->GetNumPixels())
    {
        *fLog << err << "ERROR - Size mismatch MHCamEvent and MCalibrationChargeCam.. abort." << endl;
        return kFALSE;
    }

    // Copy the constants to their final location
    // FIXME: For what the hell do we need to have the constants in
    //        in MCalibrationChargeCam?
    for (UInt_t i=0; i<hist->GetNumPixels(); i++)
    {
        hist->SetBit(MHCamera::kProfile);
        Double_t v = hist->GetBinContent(i);
        hist->SetBit(MHCamera::kErrorMean);
        Double_t e = hist->GetBinError(i);
        hist->ResetBit(MHCamera::kErrorMean);
        Double_t s = hist->GetBinError(i);

        if (!hist->IsUsed(i))
        {
            fBadPixels[i].SetUncalibrated(MBadPixelsPix::kConversionHiLoNotValid);
            v = e = s = -1;
        }

        MCalibrationChargePix &cpix = (MCalibrationChargePix&)fCalibrationCam[i];
        cpix.SetConversionHiLo(v);
        cpix.SetConversionHiLoErr(e);
        cpix.SetConversionHiLoSigma(s);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// MJCalibration allows to setup several option by a resource file:
//   MJCalibration.Display: full, datacheck, normal
//   MJCalibration.RelTimeCalibration: yes,no
//   MJCalibration.DataCheck: yes,no
//   MJCalibration.Debug: yes,no
//   MJCalibration.UseBlindPixel: yes,no
//   MJCalibration.UsePINDiode: yes,no
//   MJCalibration.Geometry: MGeomCamMagic, MGeomCamECO1000
//
// Name of a file containing reference values (see ReadReferenceFile)
//   Prefix.ReferenceFile:    filename
// (see ReadReferenceFile)
//
// For more details see the class description and the corresponding Getters
// 
Bool_t MJCalibration::CheckEnvLocal()
{
    TString dis = GetEnv("Display", "");
    if (dis.BeginsWith("Full", TString::kIgnoreCase))
        SetFullDisplay();
    if (dis.BeginsWith("DataCheck", TString::kIgnoreCase))
        SetDataCheckDisplay();
    if (dis.BeginsWith("Normal", TString::kIgnoreCase))
        SetNormalDisplay();

    if (!MJCalib::CheckEnvLocal())
        return kFALSE;

    SetRelTimeCalibration(GetEnv("RelTimeCalibration", IsRelTimes()));
    SetDebug(GetEnv("Debug", IsDebug()));

    SetUseBlindPixel(GetEnv("UseBlindPixel", IsUseBlindPixel()));
    SetUsePINDiode(GetEnv("UsePINDiode", IsUsePINDiode()));
    SetGeometry(GetEnv("Geometry", fGeometry));

    fMinEvents = (UInt_t)GetEnv("MinEvents", (Int_t)fMinEvents);

    fReferenceFile = GetEnv("ReferenceFile",fReferenceFile.Data());
    ReadReferenceFile();

    fHiLoCalibFile = GetEnv("HiLoCalibFile",fHiLoCalibFile.Data());

    /*
    if (IsUseMC() && !fHiLoCalibFile.EndsWith("_mc.root"))
    {
        if (!fHiLoCalibFile.EndsWith(".root"))
        {
            *fLog << warn << "WARNING - Hi-/Lo-Gain intercalibration file ";
            *fLog << fHiLoCalibFile << " has not .root as extension..." << endl;
        }
        else
            fHiLoCalibFile.Insert(fHiLoCalibFile.Length()-5, "_mc");
    }
    */

    return ReadHiLoCalibFile();
}

void MJCalibration::InitBlindPixel(MExtractBlindPixel &blindext,
                                   MHCalibrationChargeBlindCam &blindcam)
{
    const Int_t run = fSequence.GetLastRun();

    //
    // Initialize the blind pixel. Unfortunately, there is a hardware
    // difference in the first blind pixel until run
    // gkSecondBlindPixelInstallation and the later setup. The first
    // needs to use a filter because of the length of spurious NSB photon
    // signals. The latter get better along extracting the amplitude
    // from a small window.
    //
    const MCalibrationBlindCamOneOldStyle   one;
    const MCalibrationBlindCamTwoNewStyle   two;
    const MCalibrationBlindCamThreeNewStyle three;

    const MCalibrationBlindCam &blindresults =
        run<gkSecondBlindPixelInstallation ? static_cast<MCalibrationBlindCam>(one) :
        (run<gkThirdBlindPixelInstallation ? static_cast<MCalibrationBlindCam>(two) : static_cast<MCalibrationBlindCam>(three));

    blindresults.Copy(fCalibrationBlindCam);

    blindext.SetExtractionType(MExtractBlindPixel::kIntegral);
    blindext.SetExtractionType(MExtractBlindPixel::kFilter);

    if (run<gkSecondBlindPixelInstallation)
    {
        blindext.SetRange(10,19,0,6);
        blindext.SetNSBFilterLimit(70);
    }
    else
    {
        blindext.SetRange(5,8,0,2);
        blindext.SetNSBFilterLimit(38);
    }

    if (run>=gkThirdBlindPixelInstallation)
        blindext.SetDataType(MExtractBlindPixel::kRawEvt2);
}

// --------------------------------------------------------------------------
//
// Execute the task list and the eventloop:
//
// - Check the colour of the files in fRuns (FindColor()), otherwise return
// - Check for consistency between run numbers and number of files
// - Add fRuns to MReadMarsFile
// - Put into MParList:
//   1) MPedestalCam          (pedcam)
//   2) MCalibrationQECam     (fQECam)
//   3) MCalibrationChargeCam (fCalibrationCam)
//   4) MCalibrationRelTimeCam (fRelTimeCam)   (only if flag IsRelTimes() is chosen)
//   5) MBadPixelsCam         (fBadPixels)
//   6) MCalibrationChargePINDiode
//   7) MCalibrationBlindPix
// - Put into the MTaskList:
//   1)  MReadMarsFile
//   2)  MBadPixelsMerge
//   3)  MGeomApply
//   4)  MExtractor
//   5)  MExtractPINDiode
//   6)  MExtractBlindPixel
//   7)  MExtractTime (only if flag IsRelTimes() is chosen)
//   8)  MContinue(MFCosmics)
//   9)  MFillH("MHCalibrationChargePINDiode", "MExtractedSignalPINDiode",   "FillPINDiode")
//   10) MFillH("MHCalibrationChargeBlindCam", "MExtractedSignalBlindPixel", "FillBlindCam")
//   11) MFillH("MHCalibrationChargeCam",      "MExtractedSignalCam",        "FillChargeCam")
//   12) MFillH("MHCalibrationChargeCam",      "MExtractedSignalCam",        "FillRelTime")
//   13) MCalibrationChargeCalc
//   14) MFillH("MHCalibrationRelTimeCam",     "MArrivalTimeCam") (only if flag IsRelTimes() is chosen)
//   15) MCalibrationRelTimeCalc
// - Execute MEvtLoop
// - DisplayResult()
// - WriteResult()
//
Bool_t MJCalibration::Process(MPedestalCam &pedcam)
{
    if (!fSequence.IsValid())
    {
	  *fLog << err << "ERROR - Sequence invalid..." << endl;
	  return kFALSE;
    }

    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "Calculate calibration constants from Sequence #";
    *fLog << fSequence.GetSequence() << endl << endl;

    // --------------------------------------------------------------------------------
    
    if (!CheckEnv())
        return kFALSE;

    // --------------------------------------------------------------------------------

    // Setup Tasklist
    MParList plist;
    MTaskList tlist;
    plist.AddToList(&tlist);
    plist.AddToList(this); // take care of fDisplay!

    MDirIter iter;
    if (fSequence.GetRuns(iter, MSequence::kRawCal)<=0)
        return kFALSE;

    //
    // Input containers
    //
    pedcam.SetName("MPedestalCam"); // MPedestalFundamental
    plist.AddToList(&pedcam);
    plist.AddToList(&fBadPixels);

    //
    // Calibration Results containers
    //
    plist.AddToList(&fQECam);
    plist.AddToList(&fCalibrationCam);
    plist.AddToList(&fRelTimeCam);
    if (IsUseBlindPixel())
        plist.AddToList(&fCalibrationBlindCam);
    if (IsUsePINDiode())
        plist.AddToList(&fCalibrationPINDiode);

    //
    // Initialize two histogram containers which could be modified in this class
    //
    MHCalibrationRelTimeCam     reltimecam;
    MHCalibrationChargeCam      chargecam;
    MHCalibrationChargeBlindCam blindcam;
    plist.AddToList(&chargecam);

    if (IsUseBlindPixel())
      plist.AddToList(&blindcam);
    if (IsRelTimes())
      plist.AddToList(&reltimecam);
    //
    // Data Reading tasks
    //
    MReadMarsFile read("Events");
    MRawFileRead rawread(NULL);
    rawread.SetForceMode(); // Ignore broken time-stamps

    if (!fSequence.IsMonteCarlo())
    {
        rawread.AddFiles(iter);
        tlist.AddToList(&rawread);
    }
    else
    {
        read.DisableAutoScheme();
        read.AddFiles(iter);
        tlist.AddToList(&read);
    }

    //
    // Other Tasks
    //

    // Set the default for data version earlier than 5, where no valid
    // trigger pattern exists. There should not be pin diode or other
    // types of events inside the calibration files which should be skipped,
    // anyway. So we set the default such that the MContinue ccalib
    // will never be executed.
    // We allow to have only calibration events before Prescaling.
    // We require that the calibration events have not been prescaled (why?)
    MTriggerPatternDecode     trgpat;
    MFTriggerPattern          fcalib("CalibFilter");
    fcalib.SetDefault(kFALSE);
    fcalib.DenyAll(MFTriggerPattern::kPrescaled);
    fcalib.DenyAll(MFTriggerPattern::kUnPrescaled);
    fcalib.AllowTriggerLvl1(MFTriggerPattern::kUnPrescaled);
    fcalib.AllowTriggerLvl1(MFTriggerPattern::kPrescaled);
    fcalib.AllowTriggerLvl2(MFTriggerPattern::kUnPrescaled);
    fcalib.AllowTriggerLvl2(MFTriggerPattern::kPrescaled);
    fcalib.AllowSumTrigger(MFTriggerPattern::kUnPrescaled);
    fcalib.AllowSumTrigger(MFTriggerPattern::kPrescaled);
    fcalib.RequireCalibration(MFTriggerPattern::kPrescaled);
    fcalib.AllowCalibration(MFTriggerPattern::kUnPrescaled);

    MContinue                 ccalib(&fcalib, "ContTrigPattern");
    ccalib.SetInverted();

    MCalibrationPatternDecode decode;
    MGeomApply                apply;
    apply.SetGeometry(fGeometry);

    MBadPixelsMerge          merge(&fBadPixels);
    MExtractPINDiode         pinext;
    MExtractBlindPixel       blindext;

    if (IsUseBlindPixel())
        InitBlindPixel(blindext, blindcam);

//    MExtractSlidingWindow    extract2;
//    MExtractTimeHighestIntegral timehigh;
    MCalibrationChargeCalc   calcalc;
    MCalibrationRelTimeCalc  timecalc;
    calcalc.SetExtractor(fExtractor);

    if (IsDebug())
    {
        chargecam.SetDebug();
        calcalc.SetDebug();
    }

    //
    // Calibration histogramming
    //
    MFillH fillpin("MHCalibrationChargePINDiode", "MExtractedSignalPINDiode",   "FillPINDiode");
    MFillH fillbnd("MHCalibrationChargeBlindCam", "MExtractedSignalBlindPixel", "FillBlindCam");
    MFillH fillcam("MHCalibrationChargeCam",      "MExtractedSignalCam",        "FillChargeCam");
    MFillH filltme("MHCalibrationRelTimeCam",     "MArrivalTimeCam",            "FillRelTime");
    fillpin.SetBit(MFillH::kDoNotDisplay);
    fillbnd.SetBit(MFillH::kDoNotDisplay);
    fillcam.SetBit(MFillH::kDoNotDisplay);
    filltme.SetBit(MFillH::kDoNotDisplay);

    //
    // Set default extractors in case, none has been set...
    //
    /*
    if (!fExtractor)
      fExtractor = &extract2;
    if (!fTimeExtractor)
      fTimeExtractor = &timehigh;
     */
    MExtractTimeAndChargeSpline spline;
    if (!fExtractor)
      fExtractor = &spline;
//    if (!fTimeExtractor)
//      fTimeExtractor = &timehigh;

    const Bool_t istimecharge = fExtractor->InheritsFrom("MExtractTimeAndCharge");

    //
    // Look if the extractor is a pure charge or also a time extractor
    //
    if (istimecharge)
    {
        if (fExtractorCam.GetSize() == pedcam.GetSize())
            calcalc.SetPedestals(&fExtractorCam);
        else
        {
            *fLog << err << GetDescriptor() << "ERROR - ";
            *fLog << "Used Extractor derives from MExtractTimeAndCharge, " << endl;
            *fLog << "but MExtractorCam size " << fExtractorCam.GetSize() << " ";
            *fLog << "mismatch pedcam size " << pedcam.GetSize() << "! " << endl;
            return kFALSE;
        }
    }

    //
    // Setup more tasks and tasklist
    //
    MTaskEnv taskenv("ExtractSignal");
    taskenv.SetDefault(fExtractor);

    tlist.AddToList(&trgpat);
    if (fColor != MCalibrationCam::kCT1)
        tlist.AddToList(&ccalib);
    tlist.AddToList(&decode);
    tlist.AddToList(&merge);
    tlist.AddToList(&apply);

    // Produce pedestal subtracted raw-data
    MPedestalSubtract pedsub;
    pedsub.SetPedestalCam(&pedcam);
    tlist.AddToList(&pedsub);

    MCalibColorSet colorset;
    if (fColor != MCalibrationCam::kNONE)
        colorset.SetExplicitColor(fColor);
    tlist.AddToList(&colorset);

    tlist.AddToList(&taskenv);

    if (IsUsePINDiode())
      tlist.AddToList(&pinext);
    if (IsUseBlindPixel())
      tlist.AddToList(&blindext);

    MTaskEnv taskenv2("ExtractTime");
    if (!istimecharge)
    {
      taskenv2.SetDefault(fTimeExtractor);

      if (IsRelTimes())
        tlist.AddToList(&taskenv2);
    }

    //
    // Apply a filter against cosmics (this is for the old times in which
    // the calibration events where triggered by level 1 and for
    // sanity against empty trigger events)
    //
    MFCosmics cosmics;
    cosmics.SetMaxEmptyPixels(0.05);
    cosmics.SetMaxAcceptedFraction(0.5); // max=0.5 cosmics

    MContinue cont(&cosmics, "ContCosmics");
    tlist.AddToList(&cont);

    tlist.AddToList(&fillcam);

    if (IsUseBlindPixel())
      tlist.AddToList(&fillbnd);
    if (IsUsePINDiode())
      tlist.AddToList(&fillpin);

    tlist.AddToList(&calcalc);

    if (IsRelTimes())
    {
        tlist.AddToList(&filltme);
        tlist.AddToList(&timecalc);
    }

    MHCamEvent evt2(0, "Extra'd", "Extracted Calibration Signal;;S [cnts/sl]");
    MHCamEvent evt9(4, "ArrTm",   "Extracted ArrivalTime;;T");

    MFillH fill2(&evt2, "MExtractedSignalCam", "FillExtractedSignal");
    MFillH fill9(&evt9, "MArrivalTimeCam",     "FillArrivalTime");

    tlist.AddToList(&fill2);
    tlist.AddToList(&fill9);

    /*
     MFillH fillP("MHPulseShape", "", "FillPulseShape");
     fillP.SetNameTab("Pulse");
     tlist.AddToList(&fillP);
     */

    // Create and setup the eventloop
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(fLog);
    if (!SetupEnv(evtloop))
        return kFALSE;

    if (!taskenv.GetTask() && !taskenv2.GetTask())
    {
        *fLog << err << "ERROR - Neither ExtractSignal nor ExtractTime initialized or both '<dummy>'." << endl;
        return kFALSE;
    }

    if (!WriteTasks(taskenv.GetTask(), istimecharge ? 0 : taskenv2.GetTask()))
        return kFALSE;

    // Execute first analysis
    const Bool_t rc = evtloop.Eventloop();

    if (!fCalibrationPINDiode.IsValid())
        SetUsePINDiode(kFALSE);

    // Only display result if PreProcessing was successfull
    const Int_t numexec = !fSequence.IsMonteCarlo() ? rawread.GetNumExecutions() : read.GetNumExecutions();
    if (numexec>0)
    {
        DisplayResult(plist);
        if (!WriteResult(plist))
            return kFALSE;
    }

    if (!rc)
    {
        *fLog << err << GetDescriptor() << ": Failed." << endl;
        return kFALSE;
    }

    if (calcalc.GetNumExecutions()<fMinEvents)
    {
        *fLog << err << GetDescriptor() << ": Failed. Less than the required " << fMinEvents << " evts processed." << endl;
        return kFALSE;
    }

    // --------------------------------------------------------------------------------

    if (fIsPixelCheck)
    {
        chargecam[fCheckedPixId].DrawClone("datacheck");
        chargecam(fCheckedPixId).DrawClone("datacheck");

        if (IsRelTimes())
        {
            reltimecam[fCheckedPixId].DrawClone("");
            reltimecam(fCheckedPixId).DrawClone("");
        }
    }

    *fLog << all << GetDescriptor() << ": Done." << endl << endl << endl;

    return kTRUE;
}

/*
Bool_t MJCalibration::WriteEventloop(MEvtLoop &evtloop) const
{
    if (IsNoStorage())
        return kTRUE;

  if (fPathOut.IsNull())
    return kTRUE;
  
  const TString oname(GetOutputFile());
  
  *fLog << inf << "Writing to file: " << oname << endl;
  
  TFile file(oname, fOverwrite?"RECREATE":"NEW", "File created by MJCalibration", 9);
  if (!file.IsOpen())
  {
      *fLog << err << "ERROR - Couldn't open file " << oname << " for writing..." << endl;
      return kFALSE;
  }

  if (evtloop.Write(fName)<=0)
  {
      *fLog << err << "Unable to write MEvtloop to " << oname << endl;
      return kFALSE;
  }
  
  return kTRUE;
}
*/

Bool_t MJCalibration::WriteTasks(MTask *t1, MTask *t2) const
{
    if (IsNoStorage())
        return kTRUE;

    TObjArray cont;
    if (t1)
        cont.Add(t1);
    if (t2)
        cont.Add(t2);

    return WriteContainer(cont, GetOutputFileName(), fOverwrite?"RECREATE":"NEW");
}

// --------------------------------------------------------------------------
//
// Write the result into the output file GetOutputFile(), if fOutputPath exists.
// 
// The following containers are written:
// - MStatusDisplay
// - MCalibrationChargeCam
// - MCalibrationBlindCam
// - MCalibrationQECam
// - MCalibrationChargePINDiode
// - MBadPixelsCam
//
// If the flag kRelTimes is set, then also:
// - MCalibrationRelTimeCam
//
Bool_t MJCalibration::WriteResult(MParList &plist)
{
    if (IsNoStorage())
        return kTRUE;

    TObjArray cont;
    cont.Add(&fBadPixels);
    cont.Add(&fCalibrationCam);
    cont.Add(&fQECam);
    cont.Add(&fCalibrationBlindCam);
    cont.Add(&fCalibrationPINDiode);

    if (IsRelTimes())
        cont.Add(&fRelTimeCam);

    if (fExtractorCam.GetSize() != 0)
    {
        fExtractorCam.SetName("MPedestalExtracted");
        cont.Add(&fExtractorCam);
    }

    TObject *pedcam = plist.FindObject("MPedestalCam");
    if (!pedcam)
        *fLog << warn << " - WARNING - MPedestalCam (fundamental)... not found for writing!" << endl;
    else
        cont.Add(pedcam);

    TObject *geom = plist.FindObject("MGeomCam");
    if (!geom)
        *fLog << warn << " - WARNING - MGeomCam... not found for writing!" << endl;
    else
        cont.Add(geom);

    if (IsHistsStorage())
    {
        cont.Add(plist.FindObject("MHCalibrationChargeCam"));
        cont.Add(plist.FindObject("MHCalibrationChargeBlindCam"));
        cont.Add(plist.FindObject("MHCalibrationChargePINDiode"));
        if (IsRelTimes())
            cont.Add(plist.FindObject("MHCalibrationRelTimeCam"));
    }

    if (fDisplay)
        cont.Add(fDisplay);

    cont.Add(const_cast<TEnv*>(GetEnv()));
    cont.Add(&fSequence);

    TNamed cmdline("CommandLine", fCommandLine.Data());
    cont.Add(&cmdline);

    return WriteContainer(cont, GetOutputFileName(), "UPDATE");
}


void MJCalibration::DisplayDoubleProject(const MHCamera &cam)
{
    const UInt_t n = cam.GetGeometry()->GetNumAreas();

    TVirtualPad *pad = gPad;
    pad->Divide(n, 1, 1e-5, 1e-5);;

    for (UInt_t i=0; i<n; i++)
    {
        pad->cd(i+1);
        gPad->SetBorderMode(0);
        gPad->SetTicks();

        TH1D &h = *cam.ProjectionS(TArrayI(), TArrayI(1, (Int_t*)&i), MString::Format("%s_%d", cam.GetName(), i));
        FixDataCheckHist(h);
        h.SetTitle(MString::Format("%s %d",cam.GetTitle(), i));
        h.SetDirectory(NULL);
        h.SetBit(kCanDelete);
        h.Draw();

        h.Fit("gaus", "Q");

        TF1 *f = h.GetFunction("gaus");
        if (f)
        {
            f->SetLineWidth(2);
            f->SetLineColor(kBlue);
        }
    }
}
