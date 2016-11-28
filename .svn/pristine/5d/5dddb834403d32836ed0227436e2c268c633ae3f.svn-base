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
!   Author(s): Markus Gaug, 4/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MJPedestal
//
// Resource file entries are case sensitive!
//
// We require at least fMinEvents (def=50) to be processed by the
// ExtractPedestal-task. If not an error is returned.
//
/////////////////////////////////////////////////////////////////////////////
#include "MJPedestal.h"

// C/C++ includes
#include <fstream>

// root classes
#include <TF1.h>
#include <TLine.h>
#include <TLatex.h>
#include <TLegend.h>

#include <TEnv.h>
#include <TFile.h>

// mars core
#include "MLog.h"
#include "MLogManip.h"

#include "MDirIter.h"
#include "MTaskEnv.h"
#include "MSequence.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MStatusDisplay.h"

// Other basic classes
#include "MExtractTimeAndCharge.h"

// parameter containers
#include "MGeomCam.h"
#include "MHCamera.h"
#include "MPedestalPix.h"

//#include "MHPedestalPix.h"
#include "MCalibrationPix.h"
#include "MHCalibrationPulseTimeCam.h"
#include "MCalibrationPulseTimeCam.h"

// tasks
#include "MReadMarsFile.h"
#include "MRawFileRead.h"
#include "MGeomApply.h"
#include "MContinue.h"
#include "MPedestalSubtract.h"
#include "MTriggerPatternDecode.h"
#include "MBadPixelsMerge.h"
#include "MFillH.h"
#include "MPedCalcPedRun.h"
#include "MPedCalcFromLoGain.h"
#include "MBadPixelsCalc.h"
#include "MPedestalSubtract.h"

// filter
#include "MFilterList.h"
#include "MFTriggerPattern.h"
#include "MFDataMember.h"

ClassImp(MJPedestal);

using namespace std;

const TString  MJPedestal::fgReferenceFile   = "mjobs/pedestalref.rc";
const TString  MJPedestal::fgBadPixelsFile   = "mjobs/badpixels_0_559.rc";
const Float_t  MJPedestal::fgExtractWinLeft  = 0;
const Float_t  MJPedestal::fgExtractWinRight = 0;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets:
// - fExtractor to NULL, 
// - fExtractType to kUsePedRun
// - fStorage to Normal Storage
// - fExtractorResolution to kFALSE
//
MJPedestal::MJPedestal(const char *name, const char *title) 
    : fExtractor(NULL), fDisplayType(kDisplayDataCheck),
    fExtractType(kUsePedRun), fExtractionType(kFundamental),
    /*fIsUseHists(kFALSE),*/ fDeadPixelCheck(kFALSE), fMinEvents(50),
    fMinPedestals(100), fMaxPedestals(0), fMinCosmics(25), fMaxCosmics(100)
{
    fName  = name  ? name  : "MJPedestal";
    fTitle = title ? title : "Tool to create a pedestal file (MPedestalCam)";

    SetUsePedRun();
    SetPathIn("");
    SetReferenceFile();
    SetBadPixelsFile();

    SetExtractWinLeft();
    SetExtractWinRight();
    //
    // Default references for case that no value references file is there
    // (should not occur)
    //

    fPedestalMin                 = 4.;
    fPedestalMax                 = 16.;
    fPedRmsMin                   = 0.;
    fPedRmsMax                   = 20.;
    fRefPedClosedLids            = 9.635;
    fRefPedExtraGalactic         = 9.93;
    fRefPedGalactic              = 10.03;
    fRefPedRmsClosedLidsInner    = 1.7;
    fRefPedRmsExtraGalacticInner = 5.6;
    fRefPedRmsGalacticInner      = 6.92;
    fRefPedRmsClosedLidsOuter    = 1.7;
    fRefPedRmsExtraGalacticOuter = 3.35;
    fRefPedRmsGalacticOuter      = 4.2;
}

MJPedestal::~MJPedestal()
{
    if (fExtractor)
        delete fExtractor;
}

const char* MJPedestal::GetOutputFileName() const
{
    return Form("pedest%08d.root", fSequence.GetSequence());
}

MExtractor *MJPedestal::ReadCalibration()
{
    const TString fname = Form("%s/calib%08d.root", fPathIn.Data(), fSequence.GetSequence());

    *fLog << inf << "Reading extractor from file: " << fname << endl;

    TFile file(fname, "READ");
    if (!file.IsOpen())
    {
        *fLog << err << dbginf << "ERROR - Could not open file " << fname << endl;
        return NULL;
    }

    if (file.FindKey("MBadPixelsCam"))
    {
        MBadPixelsCam bad;
        if (bad.Read()<=0)
            *fLog << warn << "Unable to read MBadPixelsCam from " << fname << endl;
        else
            fBadPixels.Merge(bad);
    }

    if (fExtractor)
        return fExtractor;

    TObject *o=0;
    o = file.Get("ExtractSignal");
    if (o && !o->InheritsFrom(MExtractor::Class()))
    {
        *fLog << err << dbginf << "ERROR - ExtractSignal read from " << fname << " doesn't inherit from MExtractor!" << endl;
        return NULL;
    }
    return o ? (MExtractor*)o->Clone("ExtractSignal") : NULL;
}

//---------------------------------------------------------------------------------
//
// Display the results. 
// If Display type "kDataCheck" was chosen, also the reference lines are displayed.
//
void MJPedestal::DisplayResult(const MParList &plist)
{
    if (!fDisplay)
        return;

    //
    // Update display
    //
    TString title = "--  Pedestal: ";
    title += fSequence.GetSequence();
    title += "  --";
    fDisplay->SetTitle(title, kFALSE);

    //
    // Get container from list
    //
    const MGeomCam &geomcam = *(MGeomCam*)plist.FindObject("MGeomCam");
    //    MCalibrationPedCam &calpedcam = *(MCalibrationPedCam*)plist.FindObject("MCalibrationPedCam");

    //
    // Create container to display
    //
    MHCamera disp0 (geomcam, "MPedestalCam;ped", "Mean Pedestal");
    MHCamera disp1 (geomcam, "MPedestalCam;RMS", "Pedestal RMS");
    MHCamera disp2 (geomcam, "MCalibPedCam;histmean", "Mean Pedestal (Hist.)");
    MHCamera disp3 (geomcam, "MCalibPedCam;histsigma", "Pedestal RMS (Hist.)");
    MHCamera disp4 (geomcam, "MCalibPedCam;ped", "Mean Pedestal");
    MHCamera disp5 (geomcam, "MCalibPedCam;RMS", "Pedestal RMS");
    MHCamera disp6 (geomcam, "MCalibDiffCam;ped", "Diff. Mean Pedestal (Hist.)");
    MHCamera disp7 (geomcam, "MCalibDiffCam;RMS", "Diff. Pedestal RMS (Hist.)");
    MHCamera disp8 (geomcam, "MCalibDiffCam;ped", "Diff. Mean Pedestal");
    MHCamera disp9 (geomcam, "MCalibDiffCam;AbsRMS", "Diff. Abs. Pedestal RMS");
    MHCamera disp10(geomcam, "MCalibDiffCam;RelRMS", "Diff. Rel. Pedestal RMS");

    disp0.SetCamContent(fPedestalCamOut, 0);
    disp0.SetCamError  (fPedestalCamOut, 1);

    disp1.SetCamContent(fPedestalCamOut, 2);
    disp1.SetCamError  (fPedestalCamOut, 3);

    /*
    if (fIsUseHists)
      {
        disp2.SetCamContent(calpedcam, 0);
        disp2.SetCamError  (calpedcam, 1);

        disp3.SetCamContent(calpedcam, 2);
        disp3.SetCamError  (calpedcam, 3);

        disp4.SetCamContent(calpedcam, 5);
        disp4.SetCamError  (calpedcam, 6);

        disp5.SetCamContent(calpedcam, 7);
        disp5.SetCamError  (calpedcam, 8);

        for (UInt_t i=0;i<geomcam.GetNumPixels();i++)
          {

            MPedestalPix    &ped  = fPedestalCamOut[i];
            MCalibrationPix &hist = calpedcam [i];
            MBadPixelsPix &bad  = fBadPixels[i];

            if (bad.IsUnsuitable())
              continue;

            disp6.Fill(i,ped.GetPedestal()-hist.GetHiGainMean());
            disp6.SetUsed(i);

            disp7.Fill(i,hist.GetHiGainSigma()-ped.GetPedestalRms());
            if (TMath::Abs(ped.GetPedestalRms()-hist.GetHiGainSigma()) < 4.0)
              disp7.SetUsed(i);

            disp8.Fill(i,ped.GetPedestal()-hist.GetLoGainMean());
            disp8.SetUsed(i);

            disp9.Fill(i,hist.GetLoGainSigma()-ped.GetPedestalRms());
            if (TMath::Abs(hist.GetLoGainSigma() - ped.GetPedestalRms()) < 4.0)
              disp9.SetUsed(i);
          }
      }
    */

    if (fExtractionType!=kFundamental/*fExtractorResolution*/)
      {
        for (UInt_t i=0;i<geomcam.GetNumPixels();i++)
          {

            MPedestalPix &pedo  = fPedestalCamOut[i];
            MPedestalPix &pedi  = fPedestalCamIn[i];
            MBadPixelsPix &bad  = fBadPixels[i];

            if (bad.IsUnsuitable())
              continue;

            const Float_t diff = pedo.GetPedestalRms()-pedi.GetPedestalRms();
            const Float_t sum  = 0.5*(pedo.GetPedestalRms()+pedi.GetPedestalRms());

            disp9.Fill(i,pedo.GetPedestalRms()-pedi.GetPedestalRms());
            if (pedo.IsValid() && pedi.IsValid())
              disp9.SetUsed(i);

            disp10.Fill(i,sum == 0. ? 0. : diff/sum);
            if (pedo.IsValid() && pedi.IsValid() && sum != 0.)
              disp10.SetUsed(i);
          }
      }

    disp0.SetYTitle("P [cts/slice]");
    disp1.SetYTitle("P_{rms} [cts/slice]");
    disp2.SetYTitle("Hist. Mean  [cts/slice]");
    disp3.SetYTitle("Hist. Sigma [cts/slice]");
    disp4.SetYTitle("Calc. Mean  [cts/slice]");
    disp5.SetYTitle("Calc. RMS   [cts/slice]");
    disp6.SetYTitle("Diff. Mean  [cts/slice]");
    disp7.SetYTitle("Diff. RMS   [cts/slice]");
    disp8.SetYTitle("Diff. Mean  [cts/slice]");
    disp9.SetYTitle("Abs.Diff.RMS [cts/slice]");
    disp10.SetYTitle("Rel.Diff.RMS [1]");

    //
    // Display data
    //
    if (fDisplayType != kDisplayDataCheck &&  fExtractionType==kFundamental/*fExtractorResolution*/)
      {
        TCanvas &c3 = fDisplay->AddTab("Pedestals");
        c3.Divide(2,3);
        
        disp0.CamDraw(c3, 1, 2, 1);
        disp1.CamDraw(c3, 2, 2, 6);
        return;
      }

/*
    if (fIsUseHists)
      {
        
        TCanvas &c3 = fDisplay->AddTab("Extractor Hist.");
        c3.Divide(2,3);

        disp2.CamDraw(c3, 1, 2, 1);
        disp3.CamDraw(c3, 2, 2, 5);

        TCanvas &c4 = fDisplay->AddTab("Extractor Calc.");
        c4.Divide(2,3);
        
        disp4.CamDraw(c4, 1, 2, 1);
        disp5.CamDraw(c4, 2, 2, 5);

        //TCanvas &c5 = fDisplay->AddTab("Difference Hist.");
        //c5.Divide(2,3);
        //
        //disp6.CamDraw(c5, 1, 2, 1);
        //disp7.CamDraw(c5, 2, 2, 5);

        TCanvas &c6 = fDisplay->AddTab("Difference Calc.");
        c6.Divide(2,3);
        
        disp8.CamDraw(c6, 1, 2, 1);
        disp9.CamDraw(c6, 2, 2, 5);
        return;
      }
*/
    if (fDisplayType == kDisplayDataCheck)
    {

        TCanvas &c3 = fDisplay->AddTab(fExtractionType!=kFundamental/*fExtractorResolution*/ ? "PedExtrd" : "Ped");
        c3.Divide(2,3);

        if (fExtractionType==kFundamental)
            disp0.SetMinMax(fPedestalMin, fPedestalMax);
        disp1.SetMinMax(fPedRmsMin, fPedRmsMax);

        disp0.CamDraw(c3, 1, 2, 0);  // Don't devide, don't fit
        disp1.CamDraw(c3, 2, 2, 6);  // Divide, fit

        c3.cd(1);
        if (fExtractionType==kFundamental)
            DisplayReferenceLines(disp0, 0);

        c3.cd(2);
        DisplayReferenceLines(disp1, 1);

        return;
    }

    if (fExtractionType!=kFundamental/*fExtractorResolution*/)
    {

          TCanvas &c3 = fDisplay->AddTab(fExtractionType==kWithExtractor?"PedExtrd":"PedRndm");
          c3.Divide(2,3);

          disp0.CamDraw(c3, 1, 2, 1);  // Don't divide, fit
          disp1.CamDraw(c3, 2, 2, 6);  // Divide, fit

          TCanvas &c13 = fDisplay->AddTab(fExtractionType==kWithExtractor?"DiffExtrd":"DiffRndm");
          c13.Divide(2,3);

          disp9.CamDraw(c13,  1, 2, 1);
          disp10.CamDraw(c13, 2, 2, 1);
    }
}

void  MJPedestal::DisplayReferenceLines(const MHCamera &hist, const Int_t what) const
{
    MHCamera *cam = dynamic_cast<MHCamera*>(gPad->FindObject(hist.GetName()));
    if (!cam)
        return;

  const MGeomCam *geom = cam->GetGeometry();

  const Double_t x = geom->InheritsFrom("MGeomCamMagic") && what ? 397 : cam->GetNbinsX() ;

  TLine line;
  line.SetLineStyle(kDashed);
  line.SetLineWidth(3);
  line.SetLineColor(kBlue);
  
  TLegend *leg = new TLegend(0.75,0.75,0.999,0.99);
  leg->SetBit(kCanDelete);

  if (fExtractionType==kWithExtractorRndm && !(what))
    {
      TLine *l0 = line.DrawLine(0,0.,cam->GetNbinsX(),0.);
      l0->SetBit(kCanDelete);
      leg->AddEntry(l0, "Reference","l");
      leg->Draw();
      return;
    }

  line.SetLineColor(kBlue);
  TLine *l1 = line.DrawLine(0, what ? fRefPedRmsGalacticInner : fRefPedGalactic,
			    x, what ? fRefPedRmsGalacticInner : fRefPedGalactic);
  l1->SetBit(kCanDelete);  
  line.SetLineColor(kYellow);
  TLine *l2 = line.DrawLine(0, what ? fRefPedRmsExtraGalacticInner : fRefPedExtraGalactic,
			    x, what ? fRefPedRmsExtraGalacticInner : fRefPedExtraGalactic);
  l2->SetBit(kCanDelete);  
  line.SetLineColor(kMagenta);
  TLine *l3 = line.DrawLine(0, what ? fRefPedRmsClosedLidsInner : fRefPedClosedLids,
			    x, what ? fRefPedRmsClosedLidsInner : fRefPedClosedLids);
  l3->SetBit(kCanDelete);

  if (geom->InheritsFrom("MGeomCamMagic"))
    if (what)
      {
        const Double_t x2 = cam->GetNbinsX();

        line.SetLineColor(kBlue);
        line.DrawLine(398, fRefPedRmsGalacticOuter,
                      x2,  fRefPedRmsGalacticOuter);

        line.SetLineColor(kYellow);
        line.DrawLine(398, fRefPedRmsExtraGalacticOuter,
                      x2,  fRefPedRmsExtraGalacticOuter);

        line.SetLineColor(kMagenta);
        line.DrawLine(398, fRefPedRmsClosedLidsOuter,
                      x2,  fRefPedRmsClosedLidsOuter);
      }
  
    
  leg->AddEntry(l1, "Galactic","l");
  leg->AddEntry(l2, "Extra-Galactic","l");
  leg->AddEntry(l3, "Closed Lids","l");
  leg->Draw();
}

/*
void  MJPedestal::DisplayOutliers(TH1D *hist) const
{
    const Float_t mean  = hist->GetFunction("gaus")->GetParameter(1);
    const Float_t lolim = mean - 3.5*hist->GetFunction("gaus")->GetParameter(2);
    const Float_t uplim = mean + 3.5*hist->GetFunction("gaus")->GetParameter(2);
    const Stat_t  dead  = hist->Integral(0,hist->FindBin(lolim)-1);
    const Stat_t  noisy = hist->Integral(hist->FindBin(uplim)+1,hist->GetNbinsX()+1);

    TLatex deadtex;
    deadtex.SetTextSize(0.06);
    deadtex.DrawLatex(0.1,hist->GetBinContent(hist->GetMaximumBin())/1.1,Form("%3i dead pixels",(Int_t)dead));

    TLatex noisytex;
    noisytex.SetTextSize(0.06);
    noisytex.DrawLatex(0.1,hist->GetBinContent(hist->GetMaximumBin())/1.2,Form("%3i noisy pixels",(Int_t)noisy));
}
*/

void MJPedestal::FixDataCheckHist(TH1D *hist) const 
{
    hist->SetDirectory(NULL);
    hist->SetStats(0);

    //
    // set the labels bigger
    //
    TAxis *xaxe = hist->GetXaxis();
    TAxis *yaxe = hist->GetYaxis();
    
    xaxe->CenterTitle();
    yaxe->CenterTitle();    
    xaxe->SetTitleSize(0.06);
    yaxe->SetTitleSize(0.06);    
    xaxe->SetTitleOffset(0.8);
    yaxe->SetTitleOffset(0.5);    
    xaxe->SetLabelSize(0.05);
    yaxe->SetLabelSize(0.05);    
}

/*
Bool_t MJPedestal::WriteEventloop(MEvtLoop &evtloop) const
{
    if (fOutputPath.IsNull())
        return kTRUE;

    const TString oname(GetOutputFile());

    *fLog << inf << "Writing to file: " << oname << endl;

    TFile file(oname, fOverwrite?"RECREATE":"NEW", "File created by MJPedestal", 9);
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

void MJPedestal::SetExtractor(MExtractor* ext)
{
    if (ext)
    {
        if (fExtractor)
            delete fExtractor;
        fExtractor = ext ? (MExtractor*)ext->Clone(ext->GetName()) : NULL;
    }
    else
        fExtractor = 0;
}

// --------------------------------------------------------------------------
//
// Read the following values from resource file:
//
//   PedestalMin
//   PedestalMax
//
//   PedRmsMin
//   PedRmsMax
//
//   RefPedClosedLids
//   RefPedExtraGalactic
//   RefPedGalactic
//
//   RefPedRmsClosedLidsInner
//   RefPedRmsExtraGalacticInner
//   RefPedRmsGalacticInner
//   RefPedRmsClosedLidsOuter
//   RefPedRmsExtraGalacticOuter
//   RefPedRmsGalacticOuter
//
void MJPedestal::ReadReferenceFile()
{
    TEnv refenv(fReferenceFile);
    
    fPedestalMin                 = refenv.GetValue("PedestalMin",fPedestalMin);
    fPedestalMax                 = refenv.GetValue("PedestalMax",fPedestalMax);
    fPedRmsMin                   = refenv.GetValue("PedRmsMin",fPedRmsMin);                   
    fPedRmsMax                   = refenv.GetValue("PedRmsMax",fPedRmsMax);                   
    fRefPedClosedLids            = refenv.GetValue("RefPedClosedLids",fRefPedClosedLids);            
    fRefPedExtraGalactic         = refenv.GetValue("RefPedExtraGalactic",fRefPedExtraGalactic);         
    fRefPedGalactic              = refenv.GetValue("RefPedGalactic",fRefPedGalactic);              
    fRefPedRmsClosedLidsInner    = refenv.GetValue("RefPedRmsClosedLidsInner",fRefPedRmsClosedLidsInner);    
    fRefPedRmsExtraGalacticInner = refenv.GetValue("RefPedRmsExtraGalacticInner",fRefPedRmsExtraGalacticInner); 
    fRefPedRmsGalacticInner      = refenv.GetValue("RefPedRmsGalacticInner",fRefPedRmsGalacticInner);      
    fRefPedRmsClosedLidsOuter    = refenv.GetValue("RefPedRmsClosedLidsOuter",fRefPedRmsClosedLidsOuter);    
    fRefPedRmsExtraGalacticOuter = refenv.GetValue("RefPedRmsExtraGalacticOuter",fRefPedRmsExtraGalacticOuter); 
    fRefPedRmsGalacticOuter      = refenv.GetValue("RefPedRmsGalacticOuter",fRefPedRmsGalacticOuter);      
}

// --------------------------------------------------------------------------
//
// The following resource options are available:
//
// Do a datacheck run (read raw-data and enable display)
//   Prefix.DataCheck:        Yes, No <default>
//
// Setup display type
//   Prefix.Display:          normal <default>, datacheck, none
//
// Use cosmic data instead of pedestal data (DatRuns)
//   Prefix.UseData:          Yes, No <default>
//
// Write an output file with pedestals and status-display
//   Prefix.DisableOutput:    Yes, No <default>
//
// Name of a file containing reference values (see ReadReferenceFile)
//   Prefix.ReferenceFile:    filename
// (see ReadReferenceFile)
//
Bool_t MJPedestal::CheckEnvLocal()
{
    if (HasEnv("Display"))
    {
        TString type = GetEnv("Display", "normal");
        type.ToLower();
        if (type==(TString)"normal")
            fDisplayType = kDisplayNormal;
        if (type==(TString)"datacheck")
            fDisplayType = kDisplayDataCheck;
        if (type==(TString)"none")
            fDisplayType = kDisplayNone;
    }


    SetExtractWinLeft (GetEnv("ExtractWinLeft",  fExtractWinLeft ));
    SetExtractWinRight(GetEnv("ExtractWinRight", fExtractWinRight));

    fMinEvents = (UInt_t)GetEnv("MinEvents", (Int_t)fMinEvents);

    fMinPedestals = (UInt_t)GetEnv("MinPedestals", (Int_t)fMinPedestals);
    fMaxPedestals = (UInt_t)GetEnv("MaxPedestals", (Int_t)fMaxPedestals);

    fMinCosmics   = (UInt_t)GetEnv("MinCosmics", (Int_t)fMinCosmics);
    fMaxCosmics   = (UInt_t)GetEnv("MaxCosmics", (Int_t)fMaxCosmics);

    if (!MJCalib::CheckEnvLocal())
        return kFALSE;
    
    if (HasEnv("UseData"))
        fExtractType = GetEnv("UseData",kFALSE) ? kUseData : kUsePedRun;

    if (fSequence.IsMonteCarlo() && fExtractType==kUseData)
    {
        // The reason is, that the standard data files contains empty
        // (untriggered) events. If we would loop over the default 500
        // first events of the data file you would calculate the
        // pedestal from only some single events...
        *fLog << inf;
        *fLog << "Sorry, you cannot extract the starting pedestal from the first" << endl;
        *fLog << "events in your data files... using pedestal file instead.  The" << endl;
        *fLog << "result should not differ..." << endl;
        fExtractType = kUsePedRun;
    }

//    fIsUseHists = GetEnv("UseHists", fIsUseHists);

    SetNoStorage(GetEnv("DisableOutput", IsNoStorage()));

    fDeadPixelCheck = GetEnv("DeadPixelsCheck", fDeadPixelCheck);

    fBadPixelsFile = GetEnv("BadPixelsFile",fBadPixelsFile.Data());
    fReferenceFile = GetEnv("ReferenceFile",fReferenceFile.Data());
    ReadReferenceFile();

    // ------------- Do not put simple resource below --------------

    // Setup an environment task
    MTaskEnv tenv("ExtractSignal");
    tenv.SetDefault(fExtractor);

    // check the resource file for it
    if (!CheckEnv(tenv))
        return kFALSE;

//    if (tenv.ReadEnv(*GetEnv(), GetEnvPrefix()+".ExtractSignal", GetEnvDebug()>2)==kERROR)
//        return kFALSE;

    // If the resource file didn't change the default we are done
    if (fExtractor==tenv.GetTask())
        return kTRUE;

    // If it changed the default check its inheritance...
    if (!tenv.GetTask()->InheritsFrom(MExtractor::Class()))
    {
        *fLog << err << "ERROR: ExtractSignal from resource file doesn't inherit from MExtractor.... abort." << endl;
        return kFALSE;
    }

    // ..and store it
    SetExtractor((MExtractor*)tenv.GetTask());

    return kTRUE;
}

//---------------------------------------------------------------------------------
//
Bool_t MJPedestal::WritePulsePos(TObject *obj) const
{
    if (IsNoStorage())
        return kTRUE;

    const TString name(Form("signal%08d.root", fSequence.GetSequence()));

    TObjArray arr;
    arr.Add(obj);
    return WriteContainer(arr, name, fOverwrite?"RECREATE":"NEW");
}

Int_t MJPedestal::PulsePosCheck(const MParList &plist) const
{
    /*
    if (fIsPixelCheck)
    {
        MHPedestalCam *hcam = (MHPedestalCam*)plist.FindObject("MHPedestalCam");
        if (hcam)
        {
            MHPedestalPix &pix1 = (MHPedestalPix&)(*hcam)[fCheckedPixId];
            pix1.DrawClone("");
        }
    }
    */
    if (!fIsPulsePosCheck)
        return kTRUE;

    // FIXME:
    // The MC cannot run over the first 2000 pedestal events since almost all
    // events are empty, therefore a pulse pos. check is not possible, either.
    // For the moment, have to fix the problem hardcoded...
    //
    //            MMcEvt *evt = (MMcEvt*)plist.FindObject("MMcEvt");
    //            const Float_t meanpulsetime = evt->GetFadcTimeJitter();
    Float_t meanpulsetime  = 4.5;
    Float_t rmspulsetime   = 1.0;

    MCalibrationPulseTimeCam *cam = NULL;
    if (!fSequence.IsMonteCarlo())
    {  /*
        if (fIsPixelCheck)
        {
            MHCalibrationPulseTimeCam *hcam = (MHCalibrationPulseTimeCam*)plist.FindObject("MHCalibrationPulseTimeCam");
            if (!hcam)
            {
                *fLog << err << "MHCalibrationPulseTimeCam not found... abort." << endl;
                return kFALSE;
            }
            hcam->DrawClone();
            gPad->SaveAs(Form("%s/PulsePosTest_all.root",fPathOut.Data()));

            MHCalibrationPix &pix = (*hcam)[fCheckedPixId];
            pix.DrawClone();
            gPad->SaveAs(Form("%s/PulsePosTest_Pixel%04d.root",fPathOut.Data(),fCheckedPixId));
        }
        */
        cam = (MCalibrationPulseTimeCam*)plist.FindObject("MCalibrationPulseTimeCam");
        if (!cam)
        {
            *fLog << err << "MCalibrationPulseTimeCam not found... abort." << endl;
            return kFALSE;
        }

        meanpulsetime = cam->GetAverageArea(0).GetHiGainMean();
        rmspulsetime  = cam->GetAverageArea(0).GetHiGainRms();
    }

    if (!WritePulsePos(cam))
        return kFALSE;

    *fLog << all << "Mean pulse time/Avg pos.of maximum (" << (fSequence.IsMonteCarlo()?"MC":"cosmics") << "): ";
    *fLog << meanpulsetime << "+-" << rmspulsetime << endl;

    MExtractTimeAndCharge *ext = dynamic_cast<MExtractTimeAndCharge*>(fExtractor);
    if (!ext)
    {
        *fLog << warn << "WARNING - no extractor found inheriting from MExtractTimeAndCharge... no pulse position check." << endl;
        return kTRUE;
    }

    const Int_t hi0 = ext->GetHiGainFirst();
    const Int_t lo1 = ext->GetLoGainLast();
    Int_t hi1 = ext->GetHiGainLast();
    Int_t lo0 = ext->GetLoGainFirst();

    //
    // This is for data without lo-gains
    //
    const Bool_t haslo = ext->HasLoGain();

    //
    // Get the ranges for the new extractor setting. The window
    // size is always rounded to the next higher integer.
    //
    const Int_t wshigain = ext->GetWindowSizeHiGain();
    const Int_t wslogain = ext->GetWindowSizeLoGain();

    //
    // Here we calculate the end of the lo-gain range
    // as it is done in MExtractTimeAndCharge
    //
    const Double_t poshi  = meanpulsetime;
    const Double_t poslo  = poshi + ext->GetOffsetLoGain();
    const Double_t poslo2 = poslo + ext->GetLoGainStartShift();

    //
    // Do the right side checks range checks
    //
    if (poshi+wshigain+fExtractWinRight > hi1-0.5)
    {
        *fLog << err;
        *fLog << "ERROR - Pulse is too much to the right, out of hi-gain range [";
        *fLog << hi0 << "," << hi1 << "]" << endl;
        *fLog << endl;
        return -2;
    }

    if (haslo && poslo+wslogain+fExtractWinRight > lo1-0.5)
    {
        *fLog << err;
        *fLog << "ERROR - Pulse is too much to the right, out of lo-gain range [";
        *fLog << lo0 << "," << lo1 << "]" << endl;
        return -2;
    }

    //
    // Do the left side checks range checks
    //
    if (poshi-fExtractWinLeft < hi0+0.5)
    {
        *fLog << err;
        *fLog << "ERROR - Pulse is too much to the left, out of hi-gain range [";
        *fLog << hi0 << "," << hi1 << "]" << endl;
        return -3;
    }

    if (haslo && poslo2-fExtractWinLeft < lo0+0.5)
    {
        *fLog << warn;
        *fLog << "WARNING - Pulse is too much to the left, out of lo-gain range [";
        *fLog << lo0 << "," << lo1 << "]" << endl;
        *fLog << "Trying to match extraction window and pulse position..." << endl;

        //
        // Set and store the new ranges
        //
        while (poslo2-fExtractWinLeft < lo0+0.5)
        {
            hi1--;
            lo0--;

            if (poshi+wshigain+fExtractWinRight > hi1-0.5)
            {
                *fLog << err << "ERROR - No proper extraction window found.... abort." << endl;
                return -3;
            }
        }

        if (lo0<0)
            lo0=0;

        *fLog << "Changed extraction to hi-gain [" << hi0 << "," << hi1;
        *fLog << "] and lo-gain [" << lo0 << "," << lo1 << "]" << endl;

        ext->SetRange(hi0, hi1, lo0, lo1);
    }

    return kTRUE;
}

Int_t MJPedestal::Process()
{
    if (!fSequence.IsValid())
    {
	  *fLog << err << "ERROR - Sequence invalid..." << endl;
	  return kFALSE;
    }

    // --------------------------------------------------------------------------------

    const TString type = IsUseData() ? "data" : "pedestal";

    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "Calculate MPedestalCam from " << type << "-runs ";
    *fLog << fSequence.GetFileName() << endl;
    *fLog << endl;

    // --------------------------------------------------------------------------------

    if (!CheckEnv())
        return kFALSE;

    MParList  plist;
    MTaskList tlist;
    plist.AddToList(&tlist);
    plist.AddToList(this); // take care of fDisplay!

    MReadMarsFile read("Events");
    MRawFileRead rawread(NULL);
    rawread.SetForceMode(); // Ignore broken time-stamps

    MDirIter iter;
    if (fSequence.IsValid())
    {
        const Int_t n0 = IsUseData()
            ? fSequence.GetRuns(iter, MSequence::kRawDat)
            : fSequence.GetRuns(iter, MSequence::kRawPed);

        if (n0<=0)
            return kFALSE;
    }

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

    // Setup Tasklist
    plist.AddToList(&fPedestalCamOut);
    plist.AddToList(&fBadPixels);

    MGeomApply geomapl;
    MBadPixelsMerge merge(&fBadPixels);

    MPedCalcPedRun pedcalc;
    //pedcalc.SetPedestalUpdate(kFALSE);

    MPedCalcFromLoGain pedlogain;
    pedlogain.SetPedestalUpdate(kFALSE);
    
//    MHPedestalCam hpedcam;
//    hpedcam.SetPedestalsOut(&fPedestalCamOut);
//    if (fExtractionType != kFundamental)
//        hpedcam.SetRenorm(kTRUE);

    // To have it in the parlist for MEnv!
    MHCalibrationPulseTimeCam pulcam;
    plist.AddToList(&pulcam);
    MFillH fillpul(&pulcam, "MPedestalSubtractedEvt", "FillPulseTime");
    fillpul.SetBit(MFillH::kDoNotDisplay);

    tlist.AddToList(&geomapl);
    tlist.AddToList(&merge);

    if (!fPathIn.IsNull())
    {
        fExtractor = ReadCalibration();
        if (!fExtractor)
            return kFALSE;

        // The requested setup might have been overwritten
        if (!CheckEnv(*fExtractor))
            return kFALSE;

        *fLog << all;
        *fLog << underline << "Signal Extractor found in calibration file and setup:" << endl;
        fExtractor->Print();
        *fLog << endl;
    }

    //
    // Read bad pixels from outside
    // 
    if (!fBadPixelsFile.IsNull())
    {
        *fLog << inf << "Excluding: " << fBadPixelsFile << endl;
        ifstream fin(fBadPixelsFile);
        fBadPixels.AsciiRead(fin);
    }

    MTriggerPatternDecode decode;
    tlist.AddToList(&decode);

    // produce pedestal subtracted raw-data
    MPedestalSubtract pedsub;
    if (fExtractor && fExtractionType!=kFundamental)
        pedsub.SetPedestalCam(&fPedestalCamIn);
    else
        pedsub.SetNamePedestalCam(""); // Only copy hi- and lo-gain together!
    tlist.AddToList(&pedsub);

    // ----------------------------------------------------------------
    //  Setup filter for pulse position extraction and its extraction

    // This will make that for data with version less than 5, where
    // trigger patterns were not yet correct, all the events in the real
    // data file will be processed. In any case there are no interleaved
    // calibration events in such data, so this is fine.
    // The selection is done with the trigger bits before prescaling
    // Extract pulse position from Lvl1 events.
    MFTriggerPattern fcos("SelectCosmics");
    fcos.SetDefault(kTRUE);
    fcos.DenyAll();
    fcos.RequireTriggerLvl1();
    fcos.AllowTriggerLvl2();
    fcos.AllowSumTrigger();

    // Number of events filled into the histogram presenting the
    // trigger area
    MFDataMember filterc("MHCalibrationPulseTimeCam.GetNumEvents", '<', fMaxCosmics, "LimitNumCosmics");

    // Combine both filters
    MFilterList flistc("&&", "FilterCosmics");
    flistc.AddToList(&fcos);

    // For the case the pulse positon check is switched on
    // compile the tasklist accordingly
    // FIXME: MUX Monte Carlos?!??
    if (fIsPulsePosCheck)
    {
        flistc.AddToList(&filterc);
        fillpul.SetFilter(&flistc);

	tlist.AddToList(&flistc);
        tlist.AddToList(&fillpul);
    }

    //MFillH fillC("MHPedestalCor", "MPedestalSubtractedEvt", "FillAcor");
    //fillC.SetNameTab("Acor");
    //if (fExtractor && fExtractionType==kWithExtractorRndm)
    //    tlist.AddToList(&fillC);

    // ------------------------------------------------------------
    //  Setup filter for pedestal extraction
    MFTriggerPattern ftp2("SelectPedestals");
    ftp2.SetDefault(kTRUE);
    ftp2.DenyAll();
    ftp2.RequirePedestal();

    // Limit number of events from which a pedestal is extracted
    MFDataMember filterp(Form("%s.fNumEvents", fPedestalCamOut.GetName()), '<', fMaxPedestals, "LimitNumPedestal");

    // Combine both filters together
    MFilterList flistp("&&", "FilterPedestal");
    // If data is not MC and has no lo-gains select pedestals from trigger pattern
    if (!fSequence.IsMonteCarlo() && !(fExtractor && fExtractor->HasLoGain()))
        flistp.AddToList(&ftp2);
    if (fMaxPedestals>0)
        flistp.AddToList(&filterp);

    // ------------------------------------------------------------
    //  Setup pedestal extraction
    MTaskEnv taskenv("ExtractPedestal");

    taskenv.SetDefault(fExtractType==kUsePedRun ?
                       static_cast<MTask*>(&pedcalc) :
                       static_cast<MTask*>(&pedlogain));

    taskenv.SetFilter(&flistp);
    tlist.AddToList(&flistp);
    tlist.AddToList(&taskenv);

    // ------------------------------------------------------------
    //  Setup a filter which defines when the loop is stopped
    MFilterList flist("||");
    flist.SetInverted();
    if (fMaxPedestals>0)
        flist.AddToList(&filterp);
    if (fIsPulsePosCheck && fMaxCosmics>0)
        flist.AddToList(&filterc);

    MContinue stop(&flist, "Stop");
    stop.SetRc(kFALSE);
    if (flist.GetNumEntries()>0)
        tlist.AddToList(&stop);

/*
    if (fIsUseHists && fExtractor)
      {
        if (fExtractor->InheritsFrom("MExtractTimeAndCharge"))
          {
              if (fExtractionType!=kFundamental)
              {
                  const MExtractTimeAndCharge &e = *static_cast<MExtractTimeAndCharge*>(fExtractor);
                  hpedcam.SetFitStart(-5*e.GetWindowSizeHiGain());
              }
              else
                  hpedcam.SetFitStart(10.);
          }
        
        plist.AddToList(&hpedcam);
    }
 */
    pedcalc.SetPedestalsOut(&fPedestalCamOut);
    pedlogain.SetPedestalsOut(&fPedestalCamOut);

    // kFundamental
    if (fExtractor)
    {
        if (fExtractionType!=kFundamental)
        {
            pedcalc.SetRandomCalculation(fExtractionType==kWithExtractorRndm);
            pedlogain.SetRandomCalculation(fExtractionType==kWithExtractorRndm);

            pedcalc.SetExtractor((MExtractTimeAndCharge*)fExtractor);
            pedlogain.SetExtractor((MExtractTimeAndCharge*)fExtractor);
        }
        else
        {
            pedcalc.SetRangeFromExtractor(*fExtractor);
            pedlogain.SetRangeFromExtractor(*fExtractor);
        }

        if (!fExtractor->InheritsFrom("MExtractTimeAndCharge") && fExtractionType!=kFundamental)
        {
            *fLog << inf;
            *fLog << "Signal extractor doesn't inherit from MExtractTimeAndCharge..." << endl;
            *fLog << " --> falling back to fundamental pedestal extraction." << endl;
            fExtractionType=kFundamental;
        }
    }
    else
    {
        *fLog << warn << GetDescriptor() << ": WARNING - No extractor has been handed over! " << endl;
        *fLog << "Taking default window for pedestal extraction. The calculated pedestal  RMS" << endl;
        *fLog << "will probably not match with future pedestal RMS' from different extraction" << endl;
        *fLog << "windows." << endl;
    }


    /*
    MHCamEvent evt0(0, "Ped",    "Pedestal;;P [cnts/sl]");
    MHCamEvent evt1(2, "PedRms", "Pedestal RMS;;\\sigma_{p} [cnts/sl]");

    MFillH fill0(&evt0, &fPedestalCamOut, "FillPedestal");
    MFillH fill1(&evt1, &fPedestalCamOut, "FillPedRms");

    tlist.AddToList(&fill0);
    tlist.AddToList(&fill1);
    */

    //
    // Execute the eventloop
    //
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(fLog);
    if (!SetupEnv(evtloop))
        return kFALSE;

    //    if (!WriteEventloop(evtloop))
    //        return kFALSE;

    // Execute first analysis
    if (!evtloop.Eventloop(fMaxEvents))
    {
        *fLog << err << GetDescriptor() << ": Failed." << endl;
        return kFALSE;
    }

    if (taskenv.GetNumExecutions()<fMinEvents)
    {
        *fLog << err << GetDescriptor() << ": Failed. Less (" << taskenv.GetNumExecutions() << ") than the required " << fMinEvents << " pedestal events extracted." << endl;
        return kFALSE;
    }

    if (fIsPulsePosCheck && pulcam.GetNumEvents()<fMinCosmics)
    {
        *fLog << err << GetDescriptor() << ": Failed. Less (" << pulcam.GetNumEvents() << ") than the required " << fMinCosmics << " cosmics evts processed." << endl;
        return kFALSE;
    }

    if (fPedestalCamOut.GetNumEvents()<fMinPedestals)
    {
        *fLog << err << GetDescriptor() << ": Failed. Less (" << fPedestalCamOut.GetNumEvents() << ") than the required " << fMinPedestals << " pedestal evts processed." << endl;
        return kFALSE;
    }

    if (fDeadPixelCheck)
    {
        MBadPixelsCalc calc;
        calc.SetPedestalLevelVarianceLo(4.5);
        calc.SetPedestalLevelVarianceHi();
        calc.SetPedestalLevel();
        if (!CheckEnv(calc))
            return kFALSE;
        calc.SetGeomCam(dynamic_cast<MGeomCam*>(plist.FindObject("MGeomCam")));
        if (!calc.CheckPedestalRms(fBadPixels, fPedestalCamOut))
        {
            *fLog << err << "ERROR - MBadPixelsCalc::CheckPedestalRms failed...." << endl;
            return kFALSE;
        }
    }

    if (fDisplayType!=kDisplayNone)
        DisplayResult(plist);

    //    if (!WriteResult())
    //        return kFALSE;

    const Int_t rc = PulsePosCheck(plist);
    if (rc<1)
        return rc;

    *fLog << all << GetDescriptor() << ": Done." << endl << endl << endl;

    return kTRUE;
}
