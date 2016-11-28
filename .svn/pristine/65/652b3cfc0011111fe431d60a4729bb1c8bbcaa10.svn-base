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
!   Author(s): Markus Gaug, 11/2003 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */
#include "MAGIC.h"


const TString pedfile="/.magic/magicserv01/MAGIC/rootdata/2004_01_27/20040126_12149_P_Cab-On_E.root";
const TString calfile="/.magic/magicserv01/MAGIC/rootdata/2004_01_27/20040126_12526_C_Cab-On_E.root";
const TString datafile="/.magic/magicserv01/MAGIC/rootdata/2004_01_27/20040126_12517_D_Cab-On_E.root";

Int_t pedr = 36038;
Int_t calr = 36042;
//Int_t datar = 12517;



void calibration_shape(const TString inpath = "/.magic/magicserv01/scratch/hbartko/rootdata/2004_09_06/", Int_t pedruns = pedr, Int_t calruns = calr, Int_t ipix=316)

{


  MRunIter pruns;
  MRunIter cruns;
  
  pruns.AddRun(pedruns,inpath);
 
  cruns.AddRun(calruns,inpath);
  
 
  gStyle->SetOptStat(1111);
  gStyle->SetOptFit();

  /************************************/
  /* FIRST LOOP: PEDESTAL COMPUTATION */
  /************************************/
  
  MParList plist1;
  MTaskList tlist1;
  plist1.AddToList(&tlist1);
  
  // containers
  MPedestalCam   pedcam;
  MBadPixelsCam  badcam;

 
  plist1.AddToList(&pedcam);
  plist1.AddToList(&badcam);
    
  //tasks
  MReadMarsFile  read("Events");
  read.DisableAutoScheme();
  static_cast<MRead&>(read).AddFiles(pruns);

  MGeomApply     geomapl;
  // MPedCalcPedRun pedcalc;
  MPedCalcFromLoGain pedcalc_ped;
  pedcalc_ped.SetMaxHiGainVar(20);
  pedcalc_ped.SetRange(0, 11, 1, 14);
  pedcalc_ped.SetWindowSize(12,14);
  pedcalc_ped.SetPedestalUpdate(kFALSE);
 


  MGeomCamMagic  geomcam;
    
  tlist1.AddToList(&read);
  tlist1.AddToList(&geomapl);
  tlist1.AddToList(&pedcalc_ped);

  // Create and execute the event looper
  MEvtLoop pedloop;
  pedloop.SetParList(&plist1);
 
  cout << "*************************" << endl;
  cout << "** COMPUTING PEDESTALS **" << endl;
  cout << "*************************" << endl;

  if (!pedloop.Eventloop())
    return;
  
  tlist1.PrintStatistics();

 

  //
  // Create a empty Parameter List and an empty Task List 
  //
  MParList  plist2;
  MTaskList tlist2;
  plist2.AddToList(&tlist2);
  plist2.AddToList(&pedcam);
  plist2.AddToList(&badcam);
  
   
  MReadMarsFile read2("Events");
  read2.DisableAutoScheme();
  static_cast<MRead&>(read2).AddFiles(cruns);
  
  MGeomCamMagic              geomcam;
  MExtractedSignalCam        sigcam;
  MArrivalTimeCam            timecam;
  MRawEvtData                evtdata;
  
  //
  // Get the previously created MPedestalCam into the new Parameter List 
  //
  plist2.AddToList(&geomcam);
  plist2.AddToList(&sigcam);
  plist2.AddToList(&timecam);
  plist2.AddToList(&evtdata);
  
  //
  // We saw that the signal jumps between slices, 
  // thus take the sliding window
  //		
  
  MGeomApply             geomapl;
  
  
  Int_t WindowSize = 6;  // size of the chosen integration window for signal and time extraction
  
  MExtractFixedWindowPeakSearch extractor;
  extractor.SetRange(0, 14, 0, 14);
  extractor.SetWindows( WindowSize, WindowSize, 4);
  
  MExtractTimeHighestIntegral   timeext;
  timeext.SetRange(0, 14, 0, 14);
  //timeext.SetWindowSize(10,10);
  timeext.SetWindowSize(WindowSize,WindowSize);
  
  
  // timecalc->SetRange(8,14,8,14,6);
  
  // filter against cosmics
  MFCosmics            cosmics;
  MContinue            cont(&cosmics);
  
  tlist2.AddToList(&read2);
  tlist2.AddToList(&geomapl);
  tlist2.AddToList(&extractor);
  tlist2.AddToList(&timeext);
  
  //
  // In case, you want to skip the cosmics rejection, 
  // uncomment the next line
  //
  tlist2.AddToList(&cont);
  //
  // In case, you want to skip the somewhat lengthy calculation
  // of the arrival times using a spline, uncomment the next two lines
  //
  
  
  //
  // Create and setup the eventloop
  //
  MEvtLoop evtloop;
  evtloop.SetParList(&plist2);
  //    evtloop.SetDisplay(display);
    
  cout << "***************************" << endl;
  cout << "** COMPUTING CALIBRATION **" << endl;
  cout << "***************************" << endl;
  
  //
  // Execute second analysis
  //
  
  
  if (!evtloop.PreProcess())
    return;
  
  int count = 1; 
  
  TH2F * shape = new TH2F("shape","shape",300,0,30,1000,-20,250);
  TH2F * shape_corr = new TH2F("shape_corr","shape_corr",300,0,30,1000,-20,250);
  TH2F * shape_corr_all = new TH2F("shape_corr_all","shape_corr_all",300,0,30,1000,-20,250);
  TH1F * htime = new TH1F("htime","htime",150,0,15);
  TH1F * hsig = new TH1F("hsig","hsig",300,-50,550);
  
  //   cout << " hello " << endl;
  
  const Float_t ped_mean = pedcam[ipix].GetPedestal();
  const Float_t pedRMS   = pedcam[ipix].GetPedestalRms();
  
  while (tlist2.Process()){ //loop over the events  
    
    count++;
    
    if (count%1000==0)  cout << "Event #" <<  count << endl;  
    
    MRawEvtPixelIter pixel(&evtdata);
    

    double value = 0;
    pedcam.GetPixelContent(value, ipix, geomcam, 0);
    
	
    double time = 0;
    timecam.GetPixelContent(time, ipix, geomcam, 0);  // was 0)
    
    double sig = 0;
    sigcam.GetPixelContent(sig, ipix, geomcam, 0);
    
    Byte_t sat = (sigcam)[ipix].GetNumHiGainSaturated();


    htime->Fill(time);
    hsig->Fill(sig);
    
  
    pixel.Jump(ipix);
	
    const Byte_t *higains = pixel.GetHiGainSamples();
    const Byte_t *logains = pixel.GetLoGainSamples();
    const Int_t nh = evtdata.GetNumHiGainSamples();
    const Int_t nl = evtdata.GetNumLoGainSamples(); 
	
    Bool_t ABFlag = pixel.HasABFlag();
	
    const Byte_t *higains = pixel.GetHiGainSamples();
    const Byte_t *logains = pixel.GetLoGainSamples();
    
    const Float_t ABoffs = pedcam[ipix].GetPedestalABoffset();
    
   
    Float_t PedMean[2];
    PedMean[0] = ped_mean + ABoffs;
    PedMean[1] = ped_mean - ABoffs; 
    
    
      
	  
    for (int sample=0; sample<nh; sample++){
      shape->Fill(sample+0.5,higains[sample]-PedMean[(sample+ABFlag)&0x1]);
      shape->Fill(sample+15.5,logains[sample]-PedMean[(sample+ABFlag)&0x1]);
      shape_corr->Fill(sample+0.5+5.-time,higains[sample]-PedMean[(sample+ABFlag)&0x1]);
      shape_corr->Fill(sample+15.5+5.-time,logains[sample]-PedMean[(sample+nh+ABFlag)&0x1]);
      if (sat==0){
	shape_corr_all->Fill(sample+0.5+5.-time,(higains[sample]-PedMean[(sample+ABFlag)&0x1])/sig*250);
	shape_corr_all->Fill(sample+15.5+5.-time,(logains[sample]-PedMean[(sample+nh+ABFlag)&0x1])/sig*250);
      }
    }
	
	
  } // end loop over the entries
    
  tlist2.PrintStatistics();
  
  
  TCanvas * c_shape = new TCanvas("c_shape","c_shape",600,400);
  c_shape->SetFillColor(0);
  c_shape->SetBorderMode(0);
  c_shape->SetGridx();
  c_shape->SetGridy();
  shape->SetStats(0);
  TString title2 = "Raw Calibration Signal Shape, Pixel ";
  title2+=ipix;
  shape->SetTitle(title2);
  shape->SetXTitle("FADC sample no.");
  shape->SetYTitle("signal [FADC counts]");
  shape->SetMarkerStyle(20);
  shape->SetMarkerColor(4);
  shape->SetMarkerSize(0.3);
  shape->Draw();

  
  TCanvas * c_shape_corr = new TCanvas("c_shape_corr","c_shape_corr",600,400);
  c_shape_corr->SetFillColor(0);
  c_shape_corr->SetBorderMode(0);
  c_shape_corr->SetGridx();
  c_shape_corr->SetGridy();
  shape_corr->SetStats(0);
  TString title3 = "Calibration Signal Shape, Arrival Time Corrected, Pixel ";
  title3+=ipix;
  shape_corr->SetTitle(title3);
  shape_corr->SetXTitle("FADC sample no. + (<t_{arrival}> - t_{arrival}) / T_{ADC}");
  shape_corr->SetYTitle("signal [FADC counts]");
  shape_corr->SetMarkerStyle(20);
  shape_corr->SetMarkerColor(4);
  shape_corr->SetMarkerSize(0.3);
  shape_corr->Draw();


  TCanvas * c_shape_corr_all = new TCanvas("c_shape_corr_all","c_shape_corr_all",600,400);
  c_shape_corr_all->SetFillColor(0);
  c_shape_corr_all->SetBorderMode(0);
  c_shape_corr_all->SetGridx();
  c_shape_corr_all->SetGridy();
  shape_corr_all->SetStats(0);
  TString title3 = "Calibration Signal Shape, Arrival Time + Amplitude Corrected, Pixel ";
  title3+=ipix;
  shape_corr_all->SetTitle(title3);
  shape_corr_all->SetXTitle("FADC sample no. + (<t_{arrival}> - t_{arrival}) / T_{ADC}");
  shape_corr_all->SetYTitle("signal  [a.u.]");
  shape_corr_all->SetMarkerStyle(20);
  shape_corr_all->SetMarkerColor(4);
  shape_corr_all->SetMarkerSize(0.3);
  shape_corr_all->Draw();
 
  


  TCanvas * c_sig = new TCanvas("c_sig","c_sig",600,400);
  c_sig->SetFillColor(0);
  c_sig->SetBorderMode(0);
  c_sig->SetGridx();
  c_sig->SetGridy();

  hsig->SetStats(0);
  TString title4 = "Calibration Extracted Charge, MExtractFixedWindowPeakSearch, Pixel ";
  title4+=ipix;
  hsig->SetTitle(title4);
  hsig->SetXTitle("reco charge [FADC counts]");
  hsig->SetYTitle("events / 2 FADC counts");
  hsig->SetLineColor(4);
  hsig->SetLineWidth(2);
  hsig->Draw();

  TCanvas * c_time = new TCanvas("c_time","c_time",600,400);
  c_time->SetFillColor(0);
  c_time->SetBorderMode(0);
  c_time->SetGridx();
  c_time->SetGridy();

  htime->SetStats(0);
  TString title5 = "Calibration Arrival Time, MExtractTimeHighestIntegral, Pixel ";
  title5+=ipix;
  htime->SetTitle(title5);
  htime->SetXTitle("arrival time [T_{ADC}]");
  htime->SetYTitle("events / 0.1 T_{ADC}");
  htime->SetLineColor(4);
  htime->SetLineWidth(2);
  htime->Draw();


  TCanvas * c7 = new TCanvas("c7","c7",600,400);
  c7->cd();
  c7->SetGridx();
  c7->SetGridy();
  c7->SetFillColor(0);
  //  c7->SetBordermode(0);
    
  TProfile * shape_corr_all_pfx = (TProfile*)shape_corr_all->ProfileX();
  
  shape_corr_all_pfx->SetStats(0);
  TString title6 = "Average Calibration Signal Shape, Amplitude + Arrival Time Corrected, Pixel";
  title6+=ipix;
  shape_corr_all_pfx->SetTitle(title6);
  shape_corr_all_pfx->SetXTitle("FADC sample no. + (<t_{arrival}> - t_{arrival}) / T_{ADC}");
  shape_corr_all_pfx->SetYTitle("signal [a.u.]");
  shape_corr_all_pfx->SetMarkerStyle(20);
  shape_corr_all_pfx->SetMarkerColor(4);
  shape_corr_all_pfx->SetMarkerSize(0.5);
  shape_corr_all_pfx->Draw();
}




