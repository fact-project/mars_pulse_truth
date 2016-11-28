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
!   Author(s): Markus Gaug, 04/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
// pedestalstudies.C
//
// macro to study the pedestal and pedestalRMS with the number of FADC 
// slices summed up. 
//
/////////////////////////////////////////////////////////////////////////////////
const TString pedfile = "./20040303_20123_P_NewCalBoxTestLidOpen_E.root";

void pedestalstudies(const TString pedname=pedfile)
{

  Int_t loops = 13;
  Int_t stepsize = 2;

  gStyle->SetOptStat(1111);
  gStyle->SetOptFit();
  
  TArrayF *hmeandiffinn = new TArrayF(loops);
  TArrayF *hrmsdiffinn  = new TArrayF(loops);
  TArrayF *hmeandiffout = new TArrayF(loops);
  TArrayF *hrmsdiffout  = new TArrayF(loops);
  TArrayF *hmeaninn  = new TArrayF(loops);
  TArrayF *hmeanout  = new TArrayF(loops);
  TArrayF *hrmsinn   = new TArrayF(loops);
  TArrayF *hrmsout   = new TArrayF(loops);
  TArrayF *hmuinn    = new TArrayF(loops);
  TArrayF *hmuout    = new TArrayF(loops);
  TArrayF *hsigmainn = new TArrayF(loops);
  TArrayF *hsigmaout = new TArrayF(loops);

  TArrayF *hmeandiffinnerr = new TArrayF(loops);
  TArrayF *hrmsdiffinnerr  = new TArrayF(loops);
  TArrayF *hmeandiffouterr = new TArrayF(loops);
  TArrayF *hrmsdiffouterr  = new TArrayF(loops);
  TArrayF *hmeaninnerr  = new TArrayF(loops);
  TArrayF *hmeanouterr  = new TArrayF(loops);
  TArrayF *hrmsinnerr   = new TArrayF(loops);
  TArrayF *hrmsouterr   = new TArrayF(loops);
  TArrayF *hmuinnerr    = new TArrayF(loops);
  TArrayF *hmuouterr    = new TArrayF(loops);
  TArrayF *hsigmainnerr = new TArrayF(loops);
  TArrayF *hsigmaouterr = new TArrayF(loops);


  MStatusDisplay *display = new MStatusDisplay;
  display->SetUpdateTime(500);
  display->Resize(850,700);
      
  //
  // Create a empty Parameter List and an empty Task List
  // The tasklist is identified in the eventloop by its name
  //
  MParList  plist;
  MTaskList tlist;
  plist.AddToList(&tlist);

  for (Int_t samples=2;samples<stepsize*loops+1;samples=samples+stepsize)
    {

      plist.Reset();
      tlist.Reset();
      
      //
      // Now setup the tasks and tasklist for the pedestals:
      // ---------------------------------------------------
      //
      
      MReadMarsFile read("Events", pedname);
      read.DisableAutoScheme();
      
      MGeomApply      geomapl;
      //
      // Set the extraction range higher:
      //		
      MExtractFixedWindow sigcalc;
      sigcalc.SetRange(0,samples-1,0,1);
  
      MPedCalcPedRun pedcalc;
      pedcalc.SetRange(0,samples-1,0,0);
      pedcalc.SetWindowSize((Int_t)sigcalc.GetNumHiGainSamples());

      //
      // Additionally to calculating the pedestals, 
      // you can fill histograms and look at them
      //
      MFillH fill("MHPedestalCam", "MExtractedSignalCam");
      fill.SetNameTab(Form("%s%2d","PedCam",samples));

      tlist.AddToList(&read);
      tlist.AddToList(&geomapl);
      tlist.AddToList(&sigcalc);
      tlist.AddToList(&pedcalc);
      tlist.AddToList(&fill);
      
      MGeomCamMagic      geomcam;
      MPedestalCam       pedcam;
      MBadPixelsCam      badcam;
      badcam.AsciiRead("badpixels.dat");  
      
      MHPedestalCam      hpedcam;
      MCalibrationPedCam cpedcam;
      
      plist.AddToList(&geomcam);
      plist.AddToList(&pedcam);
      plist.AddToList(&hpedcam);
      plist.AddToList(&cpedcam);
      plist.AddToList(&badcam);
      
      //
      // Create and setup the eventloop
      //
      MEvtLoop evtloop;
      
      evtloop.SetParList(&plist);
      evtloop.SetDisplay(display);

      //
      // Execute first analysis
      //
      if (!evtloop.Eventloop())
        return;
      
      // 
      // Look at one specific pixel, after all the histogram manipulations:
      //
      /*
      MHGausEvents &hpix = hpedcam.GetAverageHiGainArea(0);
      hpix.DrawClone("fourierevents");
      
      MHGausEvents &lpix = hpedcam.GetAverageHiGainArea(1);
      lpix.DrawClone("fourierevents");

      hpedcam[170].DrawClone("fourierevents");
  
      */

      MHCamera dispped0  (geomcam, "Ped;Pedestal",       "Mean per Slice");
      MHCamera dispped2  (geomcam, "Ped;PedestalRms",    "RMS per Slice");
      MHCamera dispped4  (geomcam, "Ped;Mean",           "Fitted Mean per Slice");
      MHCamera dispped6  (geomcam, "Ped;Sigma",          "Fitted Sigma per Slice");
      MHCamera dispped9  (geomcam, "Ped;DeltaPedMean",   "Rel. Diff. Mean per Slice (Fit-Calc.)");
      MHCamera dispped11 (geomcam, "Ped;DeltaRmsSigma",  "Rel. Diff. RMS per Slice (Fit-Calc.)");
  
      dispped0.SetCamContent(  pedcam, 0);
      dispped0.SetCamError(    pedcam, 1);
      dispped2.SetCamContent(  pedcam, 2);
      dispped2.SetCamError(    pedcam, 3);
      
      dispped4.SetCamContent( hpedcam, 0);
      dispped4.SetCamError(   hpedcam, 1);
      dispped6.SetCamContent( hpedcam, 2);
      dispped6.SetCamError(   hpedcam, 3);
      dispped9.SetCamContent( hpedcam, 5);
      dispped9.SetCamError(   hpedcam, 6);
      dispped11.SetCamContent(hpedcam, 8);
      dispped11.SetCamError(  hpedcam, 9);
  
      dispped0.SetYTitle("Calc. Pedestal per slice [FADC counts]");
      dispped2.SetYTitle("Calc. Pedestal RMS per slice [FADC counts]");
      dispped4.SetYTitle("Fitted Mean per slice [FADC counts]");
      dispped6.SetYTitle("Fitted Sigma per slice [FADC counts]");
      dispped9.SetYTitle("Rel. Diff. Pedestal per slice Fit-Calc [1]");
      dispped11.SetYTitle("Rel. Diff. Pedestal RMS per slice Fit-Calc [1]");


      // Histogram values
      TCanvas &b1 = display->AddTab(Form("%s%d","MeanRMS",samples));
      b1.Divide(4,3);

      CamDraw(b1,dispped0,1,4,*hmeaninn,*hmeanout,*hmeaninnerr,*hmeanouterr,samples,stepsize);
      CamDraw(b1,dispped2,2,4,*hrmsinn,*hrmsout,*hrmsinnerr,*hrmsouterr,samples,stepsize);  
      CamDraw(b1,dispped4,3,4,*hmuinn,*hmuout,*hmuinnerr,*hmuouterr,samples,stepsize);
      CamDraw(b1,dispped6,4,4,*hsigmainn,*hsigmaout,*hsigmainnerr,*hsigmaouterr,samples,stepsize); 
      
      display->SaveAsGIF(3*((samples-1)/stepsize)+2,Form("%s%d","MeanRmsSamples",samples));

      // Differences
      TCanvas &c4 = display->AddTab(Form("%s%d","RelDiff",samples));
      c4.Divide(2,3);
      
      CamDraw(c4,dispped9,1,2,*hmeandiffinn,*hmeandiffout,*hmeandiffinnerr,*hmeandiffouterr,samples,stepsize);
      CamDraw(c4,dispped11,2,2,*hrmsdiffinn,*hrmsdiffout,*hrmsdiffinnerr,*hrmsdiffouterr,samples,stepsize); 

      display->SaveAsGIF(3*((samples-1)/stepsize)+3,Form("%s%d","RelDiffSamples",samples));

    }

  /*
  TF1 *logg = new TF1("logg","[1]+TMath::Log(x-[0])",1.,30.,2);
  logg->SetParameters(1.,3.5);
  logg->SetParLimits(0,-1.,3.);
  logg->SetParLimits(1,-1.,7.);
  logg->SetLineColor(kRed);
  */

  TCanvas *canvas = new TCanvas("PedstudInner","Pedestal Studies Inner Pixels",600,900);
  canvas->Divide(2,3);
  canvas->cd(1);

  TGraphErrors *gmeaninn = new TGraphErrors(hmeaninn->GetSize(),
                                            CreateXaxis(hmeaninn->GetSize(),stepsize),hmeaninn->GetArray(),
                                            CreateXaxisErr(hmeaninnerr->GetSize(),stepsize),hmeaninnerr->GetArray());
  gmeaninn->Draw("A*");
  gmeaninn->SetTitle("Calculated Mean per Slice Inner Pixels");
  gmeaninn->GetXaxis()->SetTitle("Nr. added FADC slices");
  gmeaninn->GetYaxis()->SetTitle("Calculated Mean per slice");
  //  gmeaninn->Fit("pol0");
  //  gmeaninn->GetFunction("pol0")->SetLineColor(kGreen);
  //  //  gmeaninn->Fit(logg);

  canvas->cd(2);

  TGraphErrors *gmuinn = new TGraphErrors(hmuinn->GetSize(),
                                          CreateXaxis(hmuinn->GetSize(),stepsize),hmuinn->GetArray(),
                                          CreateXaxisErr(hmuinnerr->GetSize(),stepsize),hmuinnerr->GetArray());
  gmuinn->Draw("A*");
  gmuinn->SetTitle("Fitted Mean per Slice Inner Pixels");
  gmuinn->GetXaxis()->SetTitle("Nr. added FADC slices");
  gmuinn->GetYaxis()->SetTitle("Fitted Mean per Slice");
  //  gmuinn->Fit("pol0");
  //  gmuinn->GetFunction("pol0")->SetLineColor(kGreen);
  //gmuinn->Fit(logg);


  canvas->cd(3);

  TGraphErrors *grmsinn = new TGraphErrors(hrmsinn->GetSize(),
                                           CreateXaxis(hrmsinn->GetSize(),stepsize),hrmsinn->GetArray(),
                                           CreateXaxisErr(hrmsinnerr->GetSize(),stepsize),hrmsinnerr->GetArray());
  grmsinn->Draw("A*");
  grmsinn->SetTitle("Calculated Rms per Slice Inner Pixels");
  grmsinn->GetXaxis()->SetTitle("Nr. added FADC slices");
  grmsinn->GetYaxis()->SetTitle("Calculated Rms per Slice");
  //  //grmsinn->Fit("pol2");
  //  //grmsinn->GetFunction("pol2")->SetLineColor(kRed);
  //  grmsinn->Fit(logg);

  canvas->cd(4);

  TGraphErrors *gsigmainn = new TGraphErrors(hsigmainn->GetSize(),
                                             CreateXaxis(hsigmainn->GetSize(),stepsize),hsigmainn->GetArray(),
                                             CreateXaxisErr(hsigmainnerr->GetSize(),stepsize),hsigmainnerr->GetArray());
  gsigmainn->Draw("A*");
  gsigmainn->SetTitle("Fitted Sigma per Slice Inner Pixels");
  gsigmainn->GetXaxis()->SetTitle("Nr. added FADC slices");
  gsigmainn->GetYaxis()->SetTitle("Fitted Sigma per Slice");
  //  //  gsigmainn->Fit("pol2");
  //  //  gsigmainn->GetFunction("pol2")->SetLineColor(kRed);
  //  gsigmainn->Fit(logg);

  canvas->cd(5);

  TGraphErrors *gmeandiffinn = new TGraphErrors(hmeandiffinn->GetSize(),
                                                CreateXaxis(hmeandiffinn->GetSize(),stepsize),hmeandiffinn->GetArray(),
                                                CreateXaxisErr(hmeandiffinnerr->GetSize(),stepsize),hmeandiffinnerr->GetArray());
  gmeandiffinn->Draw("A*"); 
  gmeandiffinn->SetTitle("Rel. Difference  Mean per Slice Inner Pixels");
  gmeandiffinn->GetXaxis()->SetTitle("Nr. added FADC slices");
  gmeandiffinn->GetYaxis()->SetTitle("Rel. Difference Mean per Slice");
  //  //gmeandiffinn->Fit("pol2");
  //  //gmeandiffinn->GetFunction("pol2")->SetLineColor(kBlue);
  //  gmeandiffinn->Fit(logg);


  canvas->cd(6);

  TGraphErrors *grmsdiffinn = new TGraphErrors(hrmsdiffinn->GetSize(),
                                               CreateXaxis(hrmsdiffinn->GetSize(),stepsize),hrmsdiffinn->GetArray(),
                                               CreateXaxisErr(hrmsdiffinnerr->GetSize(),stepsize),hrmsdiffinnerr->GetArray());
  grmsdiffinn->Draw("A*");
  grmsdiffinn->SetTitle("Rel. Difference Sigma per Slice-RMS Inner Pixels");
  grmsdiffinn->GetXaxis()->SetTitle("Nr. added FADC slices");
  grmsdiffinn->GetYaxis()->SetTitle("Rel. Difference Sigma per Slice-RMS");
  //  //grmsdiffinn->Fit("pol2");
  //  //grmsdiffinn->GetFunction("pol2")->SetLineColor(kBlue);
  //  grmsdiffinn->Fit(logg);

  canvas->SaveAs("PedestalStudyInner.root");
  canvas->SaveAs("PedestalStudyInner.ps");

  TCanvas *canvas2 = new TCanvas("PedstudOut","Pedestal Studies Outer Pixels",600,900);
  canvas2->Divide(2,3);
  canvas2->cd(1);

  TGraphErrors *gmeanout = new TGraphErrors(hmeanout->GetSize(),
                                            CreateXaxis(hmeanout->GetSize(),stepsize),hmeanout->GetArray(),
                                            CreateXaxisErr(hmeanouterr->GetSize(),stepsize),hmeanouterr->GetArray());
  gmeanout->Draw("A*");
  gmeanout->SetTitle("Calculated Mean per Slice Outer Pixels");
  gmeanout->GetXaxis()->SetTitle("Nr. added FADC slices");
  gmeanout->GetYaxis()->SetTitle("Calculated Mean per Slice");
  //  gmeanout->Fit("pol0");
  //  gmeanout->GetFunction("pol0")->SetLineColor(kGreen);
  //gmeanout->Fit(logg);

  canvas2->cd(2);

  TGraphErrors *gmuout = new TGraphErrors(hmuout->GetSize(),
                                          CreateXaxis(hmuout->GetSize(),stepsize),hmuout->GetArray(),
                                          CreateXaxisErr(hmuouterr->GetSize(),stepsize),hmuouterr->GetArray());
  gmuout->Draw("A*");
  gmuout->SetTitle("Fitted Mean per Slice Outer Pixels");
  gmuout->GetXaxis()->SetTitle("Nr. added FADC slices");
  gmuout->GetYaxis()->SetTitle("Fitted Mean per Slice");
  //  gmuout->Fit("pol0");
  //  gmuout->GetFunction("pol0")->SetLineColor(kGreen);
  //gmuout->Fit(logg);

  canvas2->cd(3);

  TGraphErrors *grmsout = new TGraphErrors(hrmsout->GetSize(),
                                           CreateXaxis(hrmsout->GetSize(),stepsize),hrmsout->GetArray(),
                                           CreateXaxisErr(hrmsouterr->GetSize(),stepsize),hrmsouterr->GetArray());
  grmsout->Draw("A*");
  grmsout->SetTitle("Calculated Rms per Slice Outer Pixels");
  grmsout->GetXaxis()->SetTitle("Nr. added FADC slices");
  grmsout->GetYaxis()->SetTitle("Calculated Rms per Slice");
  //  //grmsout->Fit("pol2");
  //  //grmsout->GetFunction("pol2")->SetLineColor(kRed);
  //  grmsout->Fit(logg);

  canvas2->cd(4);

  TGraphErrors *gsigmaout = new TGraphErrors(hsigmaout->GetSize(),
                                             CreateXaxis(hsigmaout->GetSize(),stepsize),hsigmaout->GetArray(),
                                             CreateXaxisErr(hsigmaouterr->GetSize(),stepsize),hsigmaouterr->GetArray());
  gsigmaout->Draw("A*");
  gsigmaout->SetTitle("Fitted Sigma per Slice Outer Pixels");
  gsigmaout->GetXaxis()->SetTitle("Nr. added FADC slices");
  gsigmaout->GetYaxis()->SetTitle("Fitted Sigma per Slice");
  //  //gsigmaout->Fit("pol2");
  //  //gsigmaout->GetFunction("pol2")->SetLineColor(kRed);
  //  gsigmaout->Fit(logg);


  canvas2->cd(5);

  TGraphErrors *gmeandiffout = new TGraphErrors(hmeandiffout->GetSize(),
                                                CreateXaxis(hmeandiffout->GetSize(),stepsize),hmeandiffout->GetArray(),
                                                CreateXaxisErr(hmeandiffouterr->GetSize(),stepsize),hmeandiffouterr->GetArray());
  gmeandiffout->Draw("A*");
  gmeandiffout->SetTitle("Rel. Difference  Mean per Slice Outer Pixels");
  gmeandiffout->GetXaxis()->SetTitle("Nr. added FADC slices");
  gmeandiffout->GetYaxis()->SetTitle("Rel. Difference Mean per Slice");
  //  //gmeandiffout->Fit("pol2");
  //w  //gmeandiffout->GetFunction("pol2")->SetLineColor(kBlue);
  //  gmeandiffout->Fit(logg);

  canvas2->cd(6);

  TGraphErrors *grmsdiffout = new TGraphErrors(hrmsdiffout->GetSize(),
                                               CreateXaxis(hrmsdiffout->GetSize(),stepsize),hrmsdiffout->GetArray(),
                                               CreateXaxisErr(hrmsdiffouterr->GetSize(),stepsize),hrmsdiffouterr->GetArray());
  grmsdiffout->Draw("A*");
  grmsdiffout->SetTitle("Rel. Difference Sigma per Slice-RMS Outer Pixels");
  grmsdiffout->GetXaxis()->SetTitle("Nr. added FADC slices");
  grmsdiffout->GetYaxis()->SetTitle("Rel. Difference Sigma per Slice-RMS");
  //  //grmsdiffout->Fit("pol2");
  //  //grmsdiffout->GetFunction("pol2")->SetLineColor(kBlue);
  //  grmsdiffout->Fit(logg);


  canvas2->SaveAs("PedestalStudyOuter.root");
  canvas2->SaveAs("PedestalStudyOuter.ps");


}


void CamDraw(TCanvas &c, MHCamera &cam, Int_t i, Int_t j, TArrayF &a1, TArrayF &a2, 
             TArrayF &a1err, TArrayF &a2err, Int_t samp, Int_t stepsize)
{

  c.cd(i);
  MHCamera *obj1=(MHCamera*)cam.DrawCopy("hist");
  obj1->SetDirectory(NULL);
  
  c.cd(i+j);
  obj1->Draw();
  ((MHCamera*)obj1)->SetPrettyPalette();

  c.cd(i+2*j);
  TH1D *obj2 = (TH1D*)obj1->Projection();
  obj2->SetDirectory(NULL);
  
  //      obj2->Sumw2();
  obj2->Draw();
  obj2->SetBit(kCanDelete);
  
  const Double_t min   = obj2->GetBinCenter(obj2->GetXaxis()->GetFirst());
  const Double_t max   = obj2->GetBinCenter(obj2->GetXaxis()->GetLast());
  const Double_t integ = obj2->Integral("width")/2.5066283;
  const Double_t mean  = obj2->GetMean();
  const Double_t rms   = obj2->GetRMS();
  const Double_t width = max-min;
  
  if (rms == 0. || width == 0. )
    return;
  
  TArrayI s0(6);
  s0[0] = 6;
  s0[1] = 1;
  s0[2] = 2;
  s0[3] = 3;
  s0[4] = 4;
  s0[5] = 5;
  
  TArrayI inner(1);
  inner[0] = 0;
  
  TArrayI outer(1);
  outer[0] = 1;
      
  // Just to get the right (maximum) binning
  TH1D *half[2];
  half[0] = obj1->ProjectionS(s0, inner, "Inner");
  half[1] = obj1->ProjectionS(s0, outer, "Outer");

  half[0]->SetDirectory(NULL);
  half[1]->SetDirectory(NULL);
  
  for (int i=0; i<2; i++)
    {
      half[i]->SetLineColor(kRed+i);
      half[i]->SetDirectory(0);
      half[i]->SetBit(kCanDelete);
      half[i]->Draw("same");
      half[i]->Fit("gaus","Q+");

      if (i==0)
        {
          a1[(samp-1)/stepsize] = half[i]->GetFunction("gaus")->GetParameter(1);
          a1err[(samp-1)/stepsize] = half[i]->GetFunction("gaus")->GetParError(1);
          if (a1err[(samp-1)/stepsize] > 3.)
            a1err[(samp-1)/stepsize] = 1.;
        }
     if (i==1)
       {
         a2[(samp-1)/stepsize] = half[i]->GetFunction("gaus")->GetParameter(1);
         a2err[(samp-1)/stepsize] = half[i]->GetFunction("gaus")->GetParError(1);
          if (a2err[(samp-1)/stepsize] > 3.)
            a2err[(samp-1)/stepsize] = 1.;
       }
    }
  
  
}

// -----------------------------------------------------------------------------
// 
// Create the x-axis for the event graph
//
Float_t *CreateXaxis(Int_t n, Int_t step)
{

  Float_t *xaxis = new Float_t[n];

  for (Int_t i=0;i<n;i++)
    xaxis[i] = 2. + step*i;

  return xaxis;
                 
}

// -----------------------------------------------------------------------------
// 
// Create the x-axis for the event graph
//
Float_t *CreateXaxisErr(Int_t n, Int_t step)
{

  Float_t *xaxis = new Float_t[n];

  for (Int_t i=0;i<n;i++)
    xaxis[i] = step/2.;

  return xaxis;
                 
}
