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
!   Author(s): Josep  Flix,  01/2004 <mailto:jflix@ifae.es>
!              Javier Rico,  01/2004 <mailto:jrico@ifae.es>
!              Markus Gaug,  03/2004 <mailto:markus@ifae.es>
!
!   (based on bootcampstandardanalysis.C by Javier López)
!
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//
//  pedphotcalc.C
//
//  This macro is an example of the use of MPedPhotCalc. It computes
//  the pedestal mean and rms from pedestal files undergoing the
//  signal extraction + calibration chain, in units of photons. It
//  produces plots of the relevant computed quantities.
//
//  Needs as arguments the run number of a pedestal file ("*_P_*.root"), 
//  one of a calibration file ("*_C_*.root").
//  performs the pedestal calculation, the calibration 
/// constants calculation and the calibration of the pedestals. 
//
//  The TString inpath has to be set correctly.
//
//  The macro searches for the pulser colour which corresponds to the calibration
//  run number. If the run number is smaller than 20000, pulser colour "CT1" 
//  is assumed, otherwise, it searches for the strings "green", "blue", "uv" or 
//  "ct1" in the filenames. If no colour or multiple colours are found, the 
//  execution is aborted.  
//
////////////////////////////////////////////////////////////////////////////////////
#include "MParList.h"
#include "MTaskList.h"
#include "MJPedestal.h"
#include "MJCalibration.h"
#include "MPedestalCam.h"
#include "MPedestalPix.h"
#include "MReadMarsFile.h"
#include "MGeomApply.h"
#include "MGeomCamMagic.h"
#include "MEvtLoop.h"
#include "MCalibrationCam.h"
#include "MCalibrationChargeCam.h"
#include "MCalibrationChargePix.h"
#include "MCalibrationQECam.h"
#include "MCalibrationQEPix.h"
#include "MExtractedSignalCam.h"
#include "MExtractSlidingWindow.h" 
#include "MExtractFixedWindow.h" 
#include "MCerPhotEvt.h"
#include "MCalibrateData.h"
#include "MPedPhotCalc.h"
#include "MPedPhotCam.h"
#include "MPedPhotPix.h"
#include "MHCamera.h"
#include "MRunIter.h"
#include "MDirIter.h"
#include "MStatusDisplay.h"
#include "MHCamera.h"

#include "TTimer.h"
#include "TString.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TF1.h"
#include "TLegend.h"

#include <iostream.h>
#include "Getline.h"

const TString inpath = "/mnt/Data/rootdata/CrabNebula/2004_02_10/";
const Int_t dpedrun  = 14607;
const Int_t dcalrun1 = 14608;
const Int_t dcalrun2 = 0;
const Bool_t usedisplay = kTRUE;



void DrawProjection(MHCamera *obj1, Int_t fit) 
{
    TH1D *obj2 = (TH1D*)obj1->Projection(obj1->GetName());
    obj2->SetDirectory(0);
    obj2->Draw();
    obj2->SetBit(kCanDelete);
    gPad->SetLogy();

    if (obj1->GetGeomCam().InheritsFrom("MGeomCamMagic"))
    {
        TArrayI s0(3);
        s0[0] = 6;
        s0[1] = 1;
        s0[2] = 2;

        TArrayI s1(3);
        s1[0] = 3;
        s1[1] = 4;
        s1[2] = 5;

        TArrayI inner(1);
        inner[0] = 0;

        TArrayI outer(1);
        outer[0] = 1;

        // Just to get the right (maximum) binning
        TH1D *half[4];
        half[0] = obj1->ProjectionS(s0, inner, "Sector 6-1-2 Inner");
        half[1] = obj1->ProjectionS(s1, inner, "Sector 3-4-5 Inner");
        half[2] = obj1->ProjectionS(s0, outer, "Sector 6-1-2 Outer");
        half[3] = obj1->ProjectionS(s1, outer, "Sector 3-4-5 Outer");

        for (int i=0; i<4; i++)
        {
            half[i]->SetLineColor(kRed+i);
            half[i]->SetDirectory(0);
            half[i]->SetBit(kCanDelete);
            half[i]->Draw("same");
        }
    }

    const Double_t min   = obj2->GetBinCenter(obj2->GetXaxis()->GetFirst());
    const Double_t max   = obj2->GetBinCenter(obj2->GetXaxis()->GetLast());
    const Double_t integ = obj2->Integral("width")/2.5066283;
    const Double_t mean  = obj2->GetMean();
    const Double_t rms   = obj2->GetRMS();
    const Double_t width = max-min;

    if (rms==0 || width==0)
        return;

    const TString dgausformula("([0]-[3])/[2]*exp(-0.5*(x-[1])*(x-[1])/[2]/[2])"
                               "+[3]/[5]*exp(-0.5*(x-[4])*(x-[4])/[5]/[5])");

    TF1 *f=0;
    switch (fit)
    {
        // FIXME: MAYBE add function to TH1?
    case 0:
        f = new TF1("sgaus", "gaus(0)", min, max);
        f->SetLineColor(kYellow);
        f->SetBit(kCanDelete);
        f->SetParNames("Area", "#mu", "#sigma");
        f->SetParameters(integ/rms, mean, rms);
        f->SetParLimits(0, 0,   integ);
        f->SetParLimits(1, min, max);
        f->SetParLimits(2, 0,   width/1.5);
        obj2->Fit(f, "QLRM");
        break;

    case 1:
        f = new TF1("dgaus", dgausformula, min, max);
        f->SetLineColor(kYellow);
        f->SetBit(kCanDelete);
        f->SetParNames("A_{tot}", "#mu_{1}", "#sigma_{1}", "A_{2}", "#mu_{2}", "#sigma_{2}");
        f->SetParameters(integ,         (min+mean)/2, width/4,
                         integ/width/2, (max+mean)/2, width/4);
        // The left-sided Gauss
        f->SetParLimits(0, integ-1.5,      integ+1.5);
        f->SetParLimits(1, min+(width/10), mean);
        f->SetParLimits(2, 0,              width/2);
        // The right-sided Gauss
        f->SetParLimits(3, 0,    integ);
        f->SetParLimits(4, mean, max-(width/10));
        f->SetParLimits(5, 0,    width/2);
        obj2->Fit(f, "QLRM");
        break;

    default:
        obj2->Fit("gaus", "Q");
        obj2->GetFunction("gaus")->SetLineColor(kYellow);
        break;
    }
}

void CamDraw(TCanvas &c, Int_t x, Int_t y, MHCamera &cam1, Int_t fit)
{
    c.cd(x);
    gPad->SetBorderMode(0);
    MHCamera *obj1=(MHCamera*)cam1.DrawCopy("hist");

    c.cd(x+y);
    gPad->SetBorderMode(0);
    obj1->Draw();

    c.cd(x+2*y);
    gPad->SetBorderMode(0);
    DrawProjection(obj1, fit);
}

void pedphotcalc(const Int_t prun=dpedrun, // pedestal file
                 const Int_t crun1=dcalrun1, const Int_t crun2=dcalrun2 // calibration file(s)
                 )
{

  MExtractFixedWindow extractor;
  
  MRunIter pruns;
  MRunIter cruns;

  pruns.AddRun(prun,inpath);

  if (crun2==0)
    cruns.AddRun(crun1,inpath);
  else
    cruns.AddRuns(crun1,crun2,inpath);

  //
  // Now setup the tasks and tasklist for the pedestals:
  // ---------------------------------------------------
  //
  MGeomCamMagic     geomcam;
  MGeomApply        geomapl;
  MStatusDisplay   *display = NULL;

  /************************************/
  /* FIRST LOOP: PEDESTAL COMPUTATION */
  /************************************/
  
  MJPedestal pedloop;
  pedloop.SetInput(&pruns);
  if (usedisplay)
    {
      display = new MStatusDisplay;
      display->SetUpdateTime(3000);
      display->Resize(850,700);
      display->SetBit(kCanDelete);
      pedloop.SetDisplay(display);
    }
  
  cout << "*************************" << endl;
  cout << "** COMPUTING PEDESTALS **" << endl;
  cout << "*************************" << endl;

  if (!pedloop.Process())
    return;

  MPedestalCam pedcam = pedloop.GetPedestalCam();

  /****************************/
  /* SECOND LOOP: CALIBRATION */
  /****************************/
  
  //
  // Now setup the new tasks for the calibration:
  // ---------------------------------------------------
  //
  MJCalibration     calloop;
  calloop.SetInput(&cruns);
  //
  // Use as signal extractor MExtractSignal:
  //
  calloop.SetExtractor(&extractor);
  //
  // The next two commands are for the display:
  //
  if (usedisplay)
    {
      calloop.SetDisplay(display);
      calloop.SetDataCheck();
    }
  
  cout << "***********************************" << endl;
  cout << "** COMPUTING CALIBRATION FACTORS **" << endl;
  cout << "***********************************" << endl;

  if (!calloop.Process(pedcam))
    return;
  
  MCalibrationChargeCam &calcam = calloop.GetCalibrationCam();
  MCalibrationQECam     &qecam  = calloop.GetQECam();

  /************************************************************************/
  /* THIRD LOOP: PEDESTAL COMPUTATION USING EXTRACTED SIGNAL (IN PHOTONS) */
  /************************************************************************/
  
  // Create an empty Parameter List and an empty Task List
  MParList  plist3;  
  MTaskList tlist3;
  plist3.AddToList(&tlist3);

  // containers
  MCerPhotEvt    photcam;
  MPedPhotCam    pedphotcam;
  MExtractedSignalCam extedsig;
  
  plist3.AddToList(&geomcam);
  plist3.AddToList(&pedcam );
  plist3.AddToList(&calcam );
  plist3.AddToList(&qecam  );
  plist3.AddToList(&photcam);
  plist3.AddToList(&extedsig);
  plist3.AddToList(&pedphotcam);
  
  //tasks
  MReadMarsFile read3("Events");
  read3.DisableAutoScheme();
  static_cast<MRead&>(read3).AddFiles(pruns);  

  MCalibrateData  photcalc;
  photcalc.SetCalibrationMode(MCalibrateData::kFfactor);
  MPedPhotCalc    pedphotcalc;  

  tlist3.AddToList(&read3);
  tlist3.AddToList(&geomapl);
  tlist3.AddToList(&extractor);
  tlist3.AddToList(&photcalc);
  tlist3.AddToList(&pedphotcalc);
  
  // Create and execute eventloop
  MEvtLoop evtloop3;
  evtloop3.SetParList(&plist3);
    
  cout << "*************************************************************" << endl;
  cout << "** COMPUTING PEDESTALS USING EXTRACTED SIGNAL (IN PHOTONS) **" << endl;
  cout << "*************************************************************" << endl;
  
  if (!evtloop3.Eventloop())  
    return;  
  tlist3.PrintStatistics();

  /**********************/
  /* PRODUCE NICE PLOTS */
  /**********************/

  if (usedisplay)
    {
      
      MHCamera disp0(geomcam, "MPedPhotCam;ped", "Pedestals");
      MHCamera disp1(geomcam, "MPedPhotCam;rms", "Sigma Pedestal");
      
      disp0.SetCamContent(pedphotcam, 0);
      disp0.SetCamError  (pedphotcam, 1);
      
      disp1.SetCamContent(pedphotcam, 1);
      
      disp0.SetYTitle("Pedestal signal [photons]");
      disp1.SetYTitle("Pedestal signal RMS [photons]");
      
      //
      // Display data
      //
      TCanvas &c1 = display->AddTab("PedPhotCam");
      c1.Divide(2,3);
      
      CamDraw(c1, 1, 2, disp0, 0);
      CamDraw(c1, 2, 2, disp1, 1);
      
    }
  

#if 0
  const UShort_t npix = 577;

  // declare histograms
  // pedestals
  TH1F* pedhist    = new TH1F("pedhist","Pedestal",npix,0,npix);
  TH1F* pedrmshist = new TH1F("pedrmshist","Pedestal RMS",npix,0,npix);
  TH1F* peddist    = new TH1F("peddist","Pedestal",100,0,20);
  TH1F* pedrmsdist = new TH1F("pedrmsdist","Pedestal RMS",100,0,15);
  
  // calibration factors
  TH1F* calhist    = new TH1F("calhist","Calibration factors",npix,0,npix);
  TH1F* caldist    = new TH1F("caldist","Calibration factors",100,0,1);
  TH1F* qehist     = new TH1F("qehist", "Quantrum efficiencies",npix,0,npix);
  TH1F* qedist     = new TH1F("qedist", "Quantrum efficiencies",100,0,1);

  // pedestal signals
  TH1F* pedphothist    = new TH1F("pedphothist","Pedestal Signal",npix,0,npix);
  TH1F* pedphotrmshist = new TH1F("pedphotrmshist","Pedestal Signal RMS",npix,0,npix);
  TH1F* pedphotdist    = new TH1F("pedphotdist","Pedestal Signal",100,-0.4,0.4);
  TH1F* pedphotrmsdist = new TH1F("pedphotrmsdist","Pedestal Signal RMS",100,0,25);

  // fill histograms
  for(int i=0;i<npix;i++)
    {
      MCalibrationChargePix &calpix = (MCalibrationChargePix&)calcam[i];
      MCalibrationQEPix     &qepix  = (MCalibrationQEPix&)    qecam[i];

      const Float_t ped        = pedcam[i].GetPedestal();
      const Float_t pedrms     = pedcam[i].GetPedestalRms();
      const Float_t cal        = calpix.GetMeanConvFADC2Phe();
      const Float_t qe         = qepix .GetQECascadesFFactor();
      const Float_t pedphot    = pedphotcam[i].GetMean();
      const Float_t pedphotrms = pedphotcam[i].GetRms();

      pedhist->Fill(i,ped);
      peddist->Fill(ped);
      pedrmshist->Fill(i,pedrms);
      pedrmsdist->Fill(pedrms);

      calhist->Fill(i,cal);
      caldist->Fill(cal);
      qehist->Fill(i,qe);
      qedist->Fill(qe);

      pedphothist->Fill(i,pedphot);
      pedphotdist->Fill(pedphot);
      pedphotrmshist->Fill(i,pedphotrms);
      pedphotrmsdist->Fill(pedphotrms);
    }

  // Draw
  gROOT->Reset();
  gStyle->SetCanvasColor(0);
  TCanvas* canvas = new TCanvas("canvas","pedphotcalc.C", 0, 100, 650, 800);
  canvas->SetBorderMode(0);    // Delete the Canvas' border line     
  canvas->cd();

  canvas->Divide(2,5);

  // draw pedestal histo
  canvas->cd(1);
  gPad->cd();
  gPad->SetBorderMode(0);

  pedhist->SetStats(kFALSE);
  pedhist->GetXaxis()->SetTitle("Pixel SW Id");
  pedhist->GetYaxis()->SetTitle("Pedestal (ADC counts)");
  pedrmshist->SetStats(kFALSE);
  pedrmshist->SetLineColor(2);
  pedhist->Draw();
  pedrmshist->Draw("same");

  TLegend* leg1 = new TLegend(.14,.68,.39,.88);
  leg1->SetHeader("");
  leg1->AddEntry(pedhist,"Pedestal","L");
  leg1->AddEntry(pedrmshist,"Pedestal RMS","L");
  leg1->SetFillColor(0);
  leg1->SetLineColor(0);
  leg1->SetBorderSize(0);
  leg1->Draw();
     
  // draw pedestal distribution
  canvas->cd(2);
  gPad->cd();
  gPad->SetBorderMode(0);
  peddist->GetXaxis()->SetTitle("Pedestal (ADC counts)");
  pedrmsdist->SetLineColor(2);
  peddist->Draw();
  pedrmsdist->Draw("same");

  TLegend* leg2 = new TLegend(.14,.68,.39,.88);
  leg2->SetHeader("");
  leg2->AddEntry(pedhist,"Pedestal","L");
  leg2->AddEntry(pedrmshist,"Pedestal RMS","L");
  leg2->SetFillColor(0);
  leg2->SetLineColor(0);
  leg2->SetBorderSize(0);
  leg2->Draw();

  // draw calibration histo
  canvas->cd(3);
  gPad->cd();
  gPad->SetBorderMode(0);
  calhist->GetXaxis()->SetTitle("Pixel SW Id");
  calhist->SetMaximum(1);
  calhist->SetMinimum(0);
  calhist->GetYaxis()->SetTitle("Calibration factor (phe/ADC count)");
  calhist->SetStats(kFALSE);
  calhist->Draw();

  // draw calibration distribution
  canvas->cd(4);
  gPad->cd();
  gPad->SetBorderMode(0);
  caldist->GetXaxis()->SetTitle("Calibration factor (phe/ADC count)");
  caldist->Draw();

  // draw qe histo
  canvas->cd(5);
  gPad->cd();
  gPad->SetBorderMode(0);
  qehist->GetXaxis()->SetTitle("Pixel SW Id");
  qehist->SetMaximum(1);
  qehist->SetMinimum(0);
  qehist->GetYaxis()->SetTitle("Quantum efficiency for cascades");
  qehist->SetStats(kFALSE);
  qehist->Draw();

  // draw qe distribution
  canvas->cd(6);
  gPad->cd();
  gPad->SetBorderMode(0);
  qedist->GetXaxis()->SetTitle("Quantum efficiency for cascades");
  qedist->Draw();

  // draw pedestal signal histo
  canvas->cd(7);
  gPad->cd();
  gPad->SetBorderMode(0);
  pedphothist->GetXaxis()->SetTitle("Pixel SW Id");
  pedphothist->GetYaxis()->SetTitle("Pedestal signal (photons)");
  pedphothist->SetStats(kFALSE);
  pedphothist->Draw();

  // draw pedestal signal distribution
  canvas->cd(8);
  gPad->cd();
  gPad->SetBorderMode(0);
  pedphotdist->GetXaxis()->SetTitle("Pedestal signal (photons)");
  pedphotdist->Draw();

  // draw pedestal signal rms histo
  canvas->cd(9);
  gPad->cd();
  gPad->SetBorderMode(0);
  pedphotrmshist->GetXaxis()->SetTitle("Pixel SW Id");
  pedphotrmshist->GetYaxis()->SetTitle("Pedestal signal rms (photons)");
  pedphotrmshist->SetStats(kFALSE);
  pedphotrmshist->Draw();

  // draw pedestal signal rms distribution
  canvas->cd(10);
  gPad->cd();
  gPad->SetBorderMode(0);
  pedphotrmsdist->GetXaxis()->SetTitle("Pedestal signal rms (photons)");
  pedphotrmsdist->Draw();

  canvas->SaveAs("pedphotcalc.root");

#endif
}
