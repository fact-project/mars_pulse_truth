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
!   Author(s): Hendrik Bartko  09/2004 <mailto:hbartko@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */


const TString defname = "/.magic/magicserv01/scratch/hbartko/rootdata/2004_09_03/20040903_35698_P_multiplexFADC_E.root";
const TString defpedname = "/.magic/magicserv01/scratch/hbartko/rootdata/2004_09_03/20040903_35698_P_multiplexFADC_E.root";

// works with one pedestal file to calculate the noise autocorrelation

void noise_autocorrelation_AB(const TString fname = defpedname, const TString pedname = defpedname, Int_t ipix = 67) {
  
  MParList plist_ped;
  
  MTaskList tlist_ped;
  plist_ped.AddToList(&tlist_ped);
  
  MPedestalCam   pedcam;
  plist_ped.AddToList(&pedcam);
  
  MReadMarsFile read("Events", pedname);
  read.DisableAutoScheme();
  tlist_ped.AddToList(&read);
  
  MGeomApply     geomapl_ped;
  MGeomCamMagic  geomcam;
  tlist_ped.AddToList(&geomapl_ped);

  
  MPedCalcFromLoGain pedcalc_ped;
  //pedcalc_ped.SetMaxHiGainVar(20);
  //pedcalc_ped.SetRange(0, 11, 1, 14);
  //pedcalc_ped.SetWindowSize(12,14);
  pedcalc_ped.SetPedestalUpdate(kFALSE);
  tlist_ped.AddToList(&pedcalc_ped);
  
  MEvtLoop evtloop_ped;
  evtloop_ped.SetParList(&plist_ped);
  
  if (!evtloop_ped.Eventloop())
    return;
  
  tlist_ped.PrintStatistics();
  
  
  // now the event loop for the signal reconstruction with pedestals subtracted
  
  
  MParList plist;
  MTaskList     tlist;
  plist.AddToList(&tlist);

  plist.AddToList(&pedcam);
  
  MRawRunHeader runheader;
  plist.AddToList(&runheader);

  MRawEvtData evtdata; 
  plist.AddToList(&evtdata);
 
  MExtractedSignalCam sigcam;
  plist.AddToList(&sigcam);
  
  MArrivalTimeCam timecam;
  plist.AddToList(&timecam);

  
  MReadMarsFile read("Events", fname);
  read.DisableAutoScheme();
  
  MGeomApply geomapl;

  Int_t WindowSize = 6;  // size of the chosen integration window for signal and time extraction
  
  MExtractFixedWindowPeakSearch extractor;
  extractor.SetRange(0, 14, 0, 14);
  extractor.SetWindows( WindowSize, WindowSize, 4);
  
  MExtractTimeHighestIntegral   timeext;
  timeext.SetRange(0, 14, 0, 14);
  timeext.SetWindowSize(WindowSize,WindowSize);

    
  tlist.AddToList(&read);
  tlist.AddToList(&geomapl);
  tlist.AddToList(&extractor);
  tlist.AddToList(&timeext);

  
  MEvtLoop evtloop;
  evtloop.SetParList(&plist);
  
  if (!evtloop.PreProcess())
    return;

  TString title = "Noise Autocorrelation B_{ij} = <b_{i}*b_{j}> - <b_{i}>*<b_{j}>, Pixel ";
  title += ipix;
 

  Int_t First = 1;

  TH2F * hcorr = new TH2F("hcorr","hcorr",15,0,15,15,0,15);
 
  TH2F * shape = new TH2F("shape","shape",311,-1.05,30.05,1000,-20,250);
  TH2F * shape_corr = new TH2F("shape_corr","shape_corr",311,-1.05,30.05,1000,-20,250);
  TH1F * htime = new TH1F("htime","htime",150,0,15);
  TH1F * hsig = new TH1F("hsig","hsig",300,-50,550); 


  Float_t B[15][15];
  Float_t bi[15];
  Float_t bj[15];
  
  for (int i=0; i<15; i++){
    bi[i]=0;
    bj[i]=0;
    for (int j=0; j<15; j++){
      B[i][j]=0;
    }
  }

  const Float_t ped_mean = pedcam[ipix].GetPedestal();
  const Float_t pedRMS   = pedcam[ipix].GetPedestalRms();

  cout << " ped_mean " << ped_mean << " pedRMS " << pedRMS << endl;
  int count = 0;

  while (tlist.Process()) {

    if (First) {
      First = 0;

      const Int_t nh = runheader.GetNumSamplesHiGain();
      const Int_t nl = runheader.GetNumSamplesLoGain();
      const Int_t nt = nh+nl;
      
    }

    
    htime->Fill(timecam[ipix].GetArrivalTimeHiGain());
    hsig->Fill(sigcam[ipix].GetExtractedSignalHiGain());


    MRawEvtPixelIter pixel(&evtdata);
    pixel.Jump(ipix);

    Bool_t ABFlag = pixel.HasABFlag();

   
    count++; // couter for the number of events

    const Byte_t *higains = pixel.GetHiGainSamples();
    const Byte_t *logains = pixel.GetLoGainSamples();
  
    const Float_t ABoffs = pedcam[ipix].GetPedestalABoffset();

    Float_t PedMean[2];
    PedMean[0] = ped_mean + ABoffs;
    PedMean[1] = ped_mean - ABoffs;


    // calculate the noise autocorrelation matrix
    for (int slice=0; slice<nh; slice++) {     
      bi[slice]+= higains[slice]-PedMean[(slice+ABFlag)&0x1];
      bj[slice]+= higains[slice]-PedMean[(slice+ABFlag)&0x1];  
      for (int ii = 0; ii< nh; ii++){
	B[slice][ii]+=(higains[slice]-PedMean[(slice+ABFlag)&0x1])*(higains[ii]-PedMean[(ii+ABFlag)&0x1]);
      }
    }


    // compute the noise signal shape
    
    Int_t trigger = 0;
    Double_t charge = 0;
    Double_t charge_time = 0;


    for (int slice=0; slice<nh; slice++) {
      shape->Fill(slice,higains[slice]-PedMean[(slice+ABFlag)&0x1]);
      shape->Fill(slice+nh,logains[slice]-PedMean[(slice+ABFlag)&0x1]);
      if (trigger==0 && (higains[slice]-PedMean[(slice+ABFlag)&0x1]) >= 4.0) 
	trigger = slice;
    }

    for (int slice=0; slice<nh; slice++) 
      if (trigger>=1 && slice<=trigger+2){
	 charge += higains[slice]-PedMean[(slice+ABFlag)&0x1];
	 charge_time += slice*(higains[slice]-PedMean[(slice+ABFlag)&0x1]);
      }
  
    if (charge>0) charge_time /= charge;

    for (int slice=0; slice<nh; slice++)
      if (trigger>=1)
	shape_corr->Fill(slice+3-charge_time,(higains[slice]-PedMean[(slice+ABFlag)&0x1]));



    
  } // end eventloop.Process()
  
  evtloop.PostProcess();


  for (int i=0; i<15; i++){
    for (int j=0; j<15; j++){
      hcorr->Fill(i,j,B[i][j]/count-bi[i]/count*bj[j]/count);
      //      cout << "i " << i << " j " << j << " B[i][j] " << B[i][j] << " bi[i] " << bi[i] << " bj[j] " << bj[j] << endl;
    }
  }

  cout << " pedmean " << ped_mean << " pedRMS " << pedRMS << endl;


  TCanvas * c_corr = new TCanvas("c_corr","c_corr",600,400);
  c_corr->SetFillColor(0);
  c_corr->SetBorderMode(0);
  c_corr->SetGridx();
  c_corr->SetGridy();
  gStyle->SetPalette(1);

  hcorr->SetStats(0);
  hcorr->SetTitle(title);
  hcorr->SetXTitle("slice i");
  hcorr->SetYTitle("slice j");
  hcorr->Draw("colz");

 
  TCanvas * c_corr_px = new TCanvas("c_corr_px","c_corr_px",600,400);
  c_corr_px->SetFillColor(0);
  c_corr_px->SetBorderMode(0);
  c_corr_px->SetGridx();
  c_corr_px->SetGridy();
  
  TH1F * hcorr_px = (TH1F*)hcorr->ProjectionX("hcorr_px",6,6);
  hcorr_px->SetLineColor(4);
  hcorr_px->SetLineWidth(2);
  hcorr_px->SetStats(0);
  TString title1 = "Noise Autocorrelation, FADC sample 6, Pixel";
  title1+=ipix;
  hcorr_px->SetTitle(title);
  hcorr_px->SetXTitle("slice i");
  hcorr_px->SetYTitle("B_{ij} = <b_{i}*b_{j}> - <b_{i}>*<b_{j}> [FADC counts]");
  
  hcorr_px->Draw();



  TCanvas * c_shape = new TCanvas("c_shape","c_shape",600,400);
  c_shape->SetFillColor(0);
  c_shape->SetBorderMode(0);
  c_shape->SetGridx();
  c_shape->SetGridy();
  shape->SetStats(0);
  TString title2 = "Noise Signal Shape, Signal #geq 3 FADC counts, Pixel ";
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
  TString title3 = "Noise Signal Shape, Signal #geq 3 FADC counts, Pixel ";
  title3+=ipix;
  shape_corr->SetTitle(title3);
  shape_corr->SetXTitle("FADC sample no. + (<t_{arrival}> - t_{arrival}) / T_{ADC}");
  shape_corr->SetYTitle("signal [FADC counts]");
  shape_corr->SetMarkerStyle(20);
  shape_corr->SetMarkerColor(4);
  shape_corr->SetMarkerSize(0.3);
  shape_corr->Draw();


  TCanvas * c_sig = new TCanvas("c_sig","c_sig",600,400);
  c_sig->SetFillColor(0);
  c_sig->SetBorderMode(0);
  c_sig->SetGridx();
  c_sig->SetGridy();

  hsig->SetStats(0);
  TString title4 = "Noise Extracted Charge, MExtractFixedWindowPeakSearch, Pixel ";
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
  TString title5 = "Noise Arrival Time, MExtractTimeHighestIntegral, Pixel ";
  title5+=ipix;
  htime->SetTitle(title5);
  htime->SetXTitle("arrival time [T_{ADC}]");
  htime->SetYTitle("events / 0.1 T_{ADC}");
  htime->SetLineColor(4);
  htime->SetLineWidth(2);
  htime->Draw();

}







