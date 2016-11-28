
Double_t ChiSquareNDof(TH1D *h1, TH1D *h2);
Int_t GetBin(Double_t size, Int_t numberSizeBins, Double_t sizeBins[]);

void lightcurve(TString f_on_name = "../HillasFiles/Mrk421/*_H.root", 
                TString f_off_name = "../HillasFiles/OffMrk421/*_H.root")
{

  gROOT->Reset();
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetTimeOffset(-3600);

  // Configuration
  const Bool_t debug = kFALSE;
  const Double_t timebin = 1380.; //[sec]
  TString psname = "./20040421_Mrk421.1380s.ps";

  //Constanst
  const Double_t kConvDegToRad = TMath::Pi()/180.;
  const Double_t kSec = 1e3;  //[sec/milisec]
  const Double_t kDay = 24.*60.*60.;  //[Day/sec]

  UInt_t numberTimeBins = 1;
  TArrayI numberOnEvents(1);
  TArrayD numberOnEventsAfterCleaning(1);
  TArrayD numberBkgEventsToNormOn(1);
  TArrayD timeOn(1);
  
  TArrayD meanTimeBinOn(1);
  TArrayD widthTimeBinOn(1);

  TArrayD zenithMinimumOn(1);
  TArrayD zenithMaximumOn(1);

  TArrayD deadFractionOn(1);
  
  TObjArray coszenithHistoOn;
  TObjArray alphaHistoOn;
  TObjArray srcposHistoOn;
  TObjArray timediffHistoOn;
  
  const Int_t numberSizeBins = 3;
  Double_t sizeBins[numberSizeBins] = {1897., 2785., 4087.}; //[Photons]

  //cuts

  Double_t widthmin[numberSizeBins]  = { 0.06, 0.06, 0.06 }; 
  Double_t widthmax[numberSizeBins]  = { 0.10, 0.12, 0.12 };
  Double_t lengthmin[numberSizeBins] = { 0.10, 0.10, 0.10 }; 
  Double_t lengthmax[numberSizeBins] = { 0.25, 0.26, 0.36 };
  Double_t distmin[numberSizeBins]   = { 0.30, 0.30, 0.30 }; 
  Double_t distmax[numberSizeBins]   = { 1.20, 1.20, 1.20 };

//   const Int_t numberSizeBins = 4;
//   Double_t sizeBins[numberSizeBins] = {1292., 1897., 2785., 4087.}; //[Photons]

//   //cuts

//   Double_t widthmin[numberSizeBins]  = { 0.06, 0.06, 0.06, 0.06 }; 
//   Double_t widthmax[numberSizeBins]  = { 0.10, 0.10, 0.12, 0.12 };
//   Double_t lengthmin[numberSizeBins] = { 0.10, 0.10, 0.10, 0.10 }; 
//   Double_t lengthmax[numberSizeBins] = { 0.20, 0.25, 0.26, 0.36 };
//   Double_t distmin[numberSizeBins]   = { 0.30, 0.30, 0.30, 0.30 }; 
//   Double_t distmax[numberSizeBins]   = { 1.20, 1.20, 1.20, 1.20 };

  //alpha plot integration for gammas
  Double_t sigexccmin = 0.;
  Double_t sigexccmax = 15.;
  Double_t bkgnormmin = 40.;
  Double_t bkgnormmax = 80.;
  
  gStyle->SetOptStat(111111);
  gStyle->SetOptFit();
  
  //
  // Make a loop only for the ON data:
  //
  
  MParList plist_on;
  MTaskList tlist_on;
  plist_on.AddToList(&tlist_on);
  
  // ON containers
  MGeomCamMagic geomcam;
  MRawRunHeader runheader_on;
  MRawEvtHeader evtheader_on;
  MTime              time_on;
  MLiveTime      livetime_on;
  MHillas          hillas_on;
  MHillasSrc    hillassrc_on;
  MSrcPosCam       srcpos_on;
  MReportDrive      drive_on;

  plist_on.AddToList(&geomcam);
  plist_on.AddToList(&runheader_on);
  plist_on.AddToList(&evtheader_on);
  plist_on.AddToList(&time_on);
  plist_on.AddToList(&livetime_on);
  plist_on.AddToList(&hillas_on);
  plist_on.AddToList(&hillassrc_on);
  plist_on.AddToList(&srcpos_on);
  plist_on.AddToList(&drive_on);

  Int_t nbins_Size = 60;
  Double_t min_Size = log10(sizeBins[0])*0.8;
  Double_t max_Size = log10(1000000)*1.2;
  TH1F *hSize_on = new TH1F ("hSizeOn","",nbins_Size,min_Size,max_Size);

  Int_t nbins_Width = 20;
  Double_t min_Width = 0.;
  Double_t max_Width = widthmax[numberSizeBins-1]*1.2;
  TH1F *hWidth_on = new TH1F ("hWidthOn","",nbins_Width,min_Width,max_Width);

  Int_t nbins_Length = 20;
  Double_t min_Length = 0.;
  Double_t max_Length =  lengthmax[numberSizeBins-1]*1.2;
  TH1F *hLength_on = new TH1F ("hLengthOn","",nbins_Length,min_Length,max_Length);

  Int_t nbins_Dist = 20;
  Double_t min_Dist = 0.;
  Double_t max_Dist = distmax[numberSizeBins-1]*1.2;
  TH1F *hDist_on = new TH1F ("hDistOn","",nbins_Dist,min_Dist,max_Dist);

  Int_t nbins_abs = 18;
  Double_t minalpha_abs = 0.;
  Double_t maxalpha_abs =90.;
  TH1F *hAlpha_on_abs = new TH1F ("hAbsAlphaOn","",nbins_abs,minalpha_abs,maxalpha_abs);

  Int_t nbins = nbins_abs*2;
  Double_t minalpha = -90.;
  Double_t maxalpha =  90.;
  TH1F *hAlpha_on = new TH1F ("hAlphaOn","",nbins,minalpha,maxalpha);

  Int_t nbins_srcpos = 200;
  Double_t minsrcpos = -200.;
  Double_t maxsrcpos =  200.;  //[mm]  //!!! position precition 3mm ~ 0.01 deg
  TH2F *hSrcPos_on = new TH2F ("hSrcPosOn","",nbins_srcpos,minsrcpos,maxsrcpos,nbins_srcpos,minsrcpos,maxsrcpos);

  //
  //tasks
  //
  
  MReadTree read_on("Parameters", f_on_name);
  read_on.DisableAutoScheme();
 
  MSrcPlace srcplace;
  MHillasSrcCalc csrc_on;

  MLiveTimeCalc livetimecalc_on;
  livetimecalc_on.SetRealTimeBinSize(timebin);

//   // prints
//   MPrint pevent("MRawEvtHeader");
//   MPrint phillas("MHillas");
//   MPrint phillassrc("MHillasSrc");
//   MPrint psrcpos("MSrcPosCam");
  
  //tasklist
  tlist_on.AddToList(&read_on);
  tlist_on.AddToList(&livetimecalc_on);
  tlist_on.AddToList(&srcplace);
  tlist_on.AddToList(&csrc_on);
  
  // Create and setup the eventloop
  MEvtLoop loop_on;
  loop_on.SetParList(&plist_on);
  //loop_on.SetDisplay(display);
  
//   MProgressBar bar;
//   loop_on.SetProgressBar(&bar);
  
//   if (!loop_on.Eventloop())
//     return;

  if (!loop_on.PreProcess())
    return;
  
  numberOnEventsAfterCleaning[numberTimeBins-1] = 0;
  zenithMinimumOn[numberTimeBins-1] = 100.;
  zenithMaximumOn[numberTimeBins-1] = 0.;
  timeOn[numberTimeBins-1] = 0;

  //create histos needed in the time bins

  TString alphaTitle = Form("%s%02i","hAlphaOn",numberTimeBins-1);
  TH1F *hAlpha_on_abs_timebin = new TH1F (alphaTitle,"",nbins_abs,minalpha_abs,maxalpha_abs);

  TString srcposTitle =  Form("%s%02i","hSrcPosOn",numberTimeBins-1);
  TH2F *hSrcPos_on_timebin = new TH2F (srcposTitle,"",nbins_srcpos,minsrcpos,maxsrcpos,nbins_srcpos,minsrcpos,maxsrcpos);

  Int_t nbins_coszenith = 200;
  Double_t mincoszenith = 0.;  
  Double_t maxcoszenith = 1.;  
  TString coszenithTitle =  Form("%s%02i","hCosZenithOn",numberTimeBins-1);
  TH1F *hCosZenith_on_timebin = new TH1F (coszenithTitle,"",nbins_coszenith,mincoszenith,maxcoszenith);

  Int_t nbins_timediff = 2000;
  Double_t mintimediff = 0.;  
  Double_t maxtimediff = 40.;  
  TString timediffTitle =  Form("%s%02i","hTimeDiffOn",numberTimeBins-1);
  TH1F *hTimeDiff_on_timebin = new TH1F (timediffTitle,"",nbins_timediff,mintimediff,maxtimediff);
  TF1 *f1 = new TF1("exp","expo",mintimediff,maxtimediff);	      

  Double_t lastSrcPosX = 0;
  Double_t lastSrcPosY = 0;

  while(tlist_on.Process())
    {
      numberOnEventsAfterCleaning[numberTimeBins-1]++;
      
      if (srcpos_on.GetX() == 0. && srcpos_on.GetY() == 0.)
	srcpos_on.SetXY(lastSrcPosX,lastSrcPosY);
      else
	{
	  lastSrcPosX = srcpos_on.GetX();
	  lastSrcPosY = srcpos_on.GetY();
	}
      srcplace.CallProcess();
      csrc_on.CallProcess();

      
      Double_t size = hillas_on.GetSize();
      Double_t width = hillas_on.GetWidth()*geomcam.GetConvMm2Deg();
      Double_t length = hillas_on.GetLength()*geomcam.GetConvMm2Deg();
      Double_t meanx = hillas_on.GetMeanX()*geomcam.GetConvMm2Deg();
      Double_t meany = hillas_on.GetMeanY()*geomcam.GetConvMm2Deg();
      Double_t centerdist = TMath::Sqrt(meanx*meanx + meany*meany);
      Double_t dist = hillassrc_on.GetDist()*geomcam.GetConvMm2Deg();
      Double_t alpha = hillassrc_on.GetAlpha();
      Double_t srcposx = srcpos_on.GetX();
      Double_t srcposy = srcpos_on.GetY();
      Double_t zenith = drive_on.GetNominalZd();
	  

      Int_t sizebin = GetBin(size,numberSizeBins,sizeBins);
	  
      if (sizebin >= 0)
	{
	  if (width > widthmin[sizebin] && width < widthmax[sizebin])
	    {
	      if (length > lengthmin[sizebin] && length < lengthmax[sizebin])
		{
		  if (dist > distmin[sizebin] && centerdist < distmax[sizebin])
		    {      
		      
		      //General histos
		      hSize_on->Fill(log10(size));
		      hWidth_on->Fill(width);
		      hLength_on->Fill(length);
		      hDist_on->Fill(dist);
		      hAlpha_on_abs->Fill(TMath::Abs(alpha));
		      hAlpha_on->Fill(alpha);
		      hSrcPos_on->Fill(srcposx,srcposy);
		      
		      // Time bin histos
		      hAlpha_on_abs_timebin->Fill(TMath::Abs(alpha));
		      hSrcPos_on_timebin->Fill(srcposx,srcposy);
		      hCosZenith_on_timebin->Fill(TMath::Cos(zenith*kConvDegToRad));
		      
		      if (zenith != 0 && zenith < zenithMinimumOn[numberTimeBins-1])
			zenithMinimumOn[numberTimeBins-1] = zenith;
		      if (zenith>zenithMaximumOn[numberTimeBins-1])
			zenithMaximumOn[numberTimeBins-1] = zenith;
		      
		    }
		}
	    }
	}
      
      
      // Check if you are overload the maxim time bin
      if (numberTimeBins != livetime_on.GetNumberTimeBins())
	{
	  timeOn[numberTimeBins-1] = livetime_on.GetLiveTimeTArray().At(numberTimeBins-1);
	  meanTimeBinOn[numberTimeBins-1] = livetime_on.GetMeanRealTimeTArray().At(numberTimeBins-1);
	  widthTimeBinOn[numberTimeBins-1] = livetime_on.GetWidthRealTimeTArray().At(numberTimeBins-1);

	  //Compute the number of on events
	  numberOnEvents[numberTimeBins-1] = (Double_t) hAlpha_on_abs_timebin->Integral((Int_t)sigexccmin*nbins_abs/90+1,(Int_t)sigexccmax*nbins_abs/90);
	  numberBkgEventsToNormOn[numberTimeBins-1] =  (Double_t)  hAlpha_on_abs_timebin->Integral((Int_t)bkgnormmin*nbins_abs/90+1,(Int_t)bkgnormmax*nbins_abs/90);	      
	  
// 	  hTimeDiff_on_timebin->Fit("exp","RQ0");
// 	  deadFractionOn[numberTimeBins-1] = (nbins_timediff/(maxtimediff-mintimediff))*TMath::Exp(f1->GetParameter(0))/(TMath::Abs(f1->GetParameter(1))*hTimeDiff_on_timebin->GetEntries());
// 	  cout << "1-> Exp(" << f1->GetParameter(0) << " + " << f1->GetParameter(1) << "*x) +- [" <<  f1->GetParError(0) << ' ' << f1->GetParError(1) << "]" << endl;
// 	  cout << "Calc entries " << (nbins_timediff/(maxtimediff-mintimediff))*TMath::Exp(f1->GetParameter(0))/(TMath::Abs(f1->GetParameter(1))) << " +- Nt*(" << TMath::Sqrt( f1->GetParameter(0)*f1->GetParError(0)*f1->GetParameter(0)*f1->GetParError(0) + (f1->GetParError(1)*f1->GetParError(1))/(f1->GetParameter(1)*f1->GetParameter(1)))  << ") meas entries " << hTimeDiff_on_timebin->GetEntries() << " dead fraction " << deadFractionOn[numberTimeBins-1] << endl;
	  deadFractionOn[numberTimeBins-1] = 1.;
	  
	  alphaHistoOn.AddLast(hAlpha_on_abs_timebin);
	  srcposHistoOn.AddLast(hSrcPos_on_timebin);
	  coszenithHistoOn.AddLast(hCosZenith_on_timebin);
	  timediffHistoOn.AddLast(hTimeDiff_on_timebin);
	  
	  numberTimeBins = livetime_on.GetNumberTimeBins();
	  
	  timeOn.Set(numberTimeBins);
	  numberOnEvents.Set(numberTimeBins);
	  numberBkgEventsToNormOn.Set(numberTimeBins);
	  widthTimeBinOn.Set(numberTimeBins);
	  meanTimeBinOn.Set(numberTimeBins);
	  zenithMinimumOn.Set(numberTimeBins);
	  zenithMaximumOn.Set(numberTimeBins);
	  deadFractionOn.Set(numberTimeBins);
	  numberOnEventsAfterCleaning.Set(numberTimeBins);
	  
	  timeOn[numberTimeBins-1] = 0.;
	  zenithMinimumOn[numberTimeBins-1] = 100.;
	  zenithMaximumOn[numberTimeBins-1] = 0.;
	  numberOnEventsAfterCleaning[numberTimeBins-1] = 0;
	  
	  alphaTitle =  Form("%s%02i","hAlphaOn",numberTimeBins-1);         
	  hAlpha_on_abs_timebin = new TH1F (alphaTitle,"",nbins_abs,minalpha_abs,maxalpha_abs);
	  
	  srcposTitle =  Form("%s%02i","hSrcPosOn",numberTimeBins-1);
	  hSrcPos_on_timebin = new TH2F (srcposTitle,"",nbins_srcpos,minsrcpos,maxsrcpos,nbins_srcpos,minsrcpos,maxsrcpos);
	  
	  coszenithTitle =  Form("%s%02i","hCosZenithOn",numberTimeBins-1);
	  hCosZenith_on_timebin = new TH1F (coszenithTitle,"",nbins_coszenith,mincoszenith,maxcoszenith);
	  
	  timediffTitle =  Form("%s%02i","hTimeDiffOn",numberTimeBins-1);
	  hTimeDiff_on_timebin = new TH1F (timediffTitle,"",nbins_timediff,mintimediff,maxtimediff);
	  
	}
      
    }
	
  loop_on.PostProcess();
  
  tlist_on.PrintStatistics();
  
  timeOn[numberTimeBins-1] = livetime_on.GetLiveTimeTArray().At(numberTimeBins-1);
  meanTimeBinOn[numberTimeBins-1] = livetime_on.GetMeanRealTimeTArray().At(numberTimeBins-1);
  widthTimeBinOn[numberTimeBins-1] = livetime_on.GetWidthRealTimeTArray().At(numberTimeBins-1);
  
  //Compute the number of on events for the last time bin
  numberOnEvents[numberTimeBins-1] = (Double_t) hAlpha_on_abs_timebin->Integral((Int_t)sigexccmin*nbins_abs/90+1,(Int_t)sigexccmax*nbins_abs/90);
  numberBkgEventsToNormOn[numberTimeBins-1] =  (Double_t)  hAlpha_on_abs_timebin->Integral((Int_t)bkgnormmin*nbins_abs/90+1,(Int_t)bkgnormmax*nbins_abs/90);	      

//   hTimeDiff_on_timebin->Fit("exp","RQ0");
//   deadFractionOn[numberTimeBins-1] = (nbins_timediff/(maxtimediff-mintimediff))*TMath::Exp(f1->GetParameter(0))/(TMath::Abs(f1->GetParameter(1))*hTimeDiff_on_timebin->GetEntries());

//   cout.precision(5);
//   cout << "2-> Exp(" << f1->GetParameter(0) << " + 
//   cout << "Calc entries " << (nbins_timediff/(maxtimediff-mintimediff))*TMath::Exp(f1->GetParameter(0))/TMath::Abs(f1->GetParameter(1)) << " +- Nt*(" << TMath::Sqrt( f1->GetParameter(0)*f1->GetParError(0)*f1->GetParameter(0)*f1->GetParError(0) + (f1->GetParError(1)*f1->GetParError(1))/(f1->GetParameter(1)*f1->GetParameter(1)))  << ") meas entries " << hTimeDiff_on_timebin->GetEntries() << " dead fraction " << deadFractionOn[numberTimeBins-1] << endl;
  deadFractionOn[numberTimeBins-1] = 1.;

  alphaHistoOn.AddLast(hAlpha_on_abs_timebin);
  srcposHistoOn.AddLast(hSrcPos_on_timebin);
  coszenithHistoOn.AddLast(hCosZenith_on_timebin);
  timediffHistoOn.AddLast(hTimeDiff_on_timebin);

  //-----------------------OFF------------------------

  TObjArray alphaHistoOff;
  TObjArray srcposHistoOff;

  TArrayD timeOff(numberTimeBins);

  TArrayI numberOffEvents(numberTimeBins);

  TArrayD numberBkgEventsToNormOff(numberTimeBins);
  TArrayD onOffNormFactor(numberTimeBins);
  TArrayD onOffNormFactorWithEvents(numberTimeBins);
  TArrayD onOffNormFactorWithLiveTime(numberTimeBins);

  TArrayD numberExcessEvents(numberTimeBins);
  TArrayD errorNumberExcessEvents(numberTimeBins);

  TArrayD numberExcessEventsPerSec(numberTimeBins);
  TArrayD errorNumberExcessEventsPerSec(numberTimeBins);

  TArrayD numberExcessEventsPerMin(numberTimeBins);
  TArrayD errorNumberExcessEventsPerMin(numberTimeBins);

  timeOff.Set(numberTimeBins);
  
  TH1F* hAlpha_off_abs_timebin;
  TH2F* hSrcPos_off_timebin;
  for (UInt_t bin=0; bin<numberTimeBins; bin++)
    {
      alphaTitle =  Form("%s%02i","hAlphaOff",bin);
      hAlpha_off_abs_timebin = new TH1F (alphaTitle,"",nbins_abs,minalpha_abs,maxalpha_abs);;
      alphaHistoOff.AddLast(hAlpha_off_abs_timebin);
      //      cout << "hAlpha_off_abs_timebin " << hAlpha_off_abs_timebin <<  " alphaHistoOff [" << bin << "] " << alphaHistoOff[bin] << endl;

      srcposTitle =  Form("%s%02i","hSrcPosOff",bin);
      hSrcPos_off_timebin = new TH2F (srcposTitle,"",nbins_srcpos,minsrcpos,maxsrcpos,nbins_srcpos,minsrcpos,maxsrcpos);
      srcposHistoOff.AddLast(hSrcPos_off_timebin);
      //      cout << "hSrcPos_off_timebin " << hSrcPos_off_timebin <<  " srcposHistoOff [" << bin << "] " << srcposHistoOff[bin] << endl;
    }

  // 
  // Make a loop only for the OFF data:
  //
  
  MParList plist_off;
  MTaskList tlist_off;
  plist_off.AddToList(&tlist_off);
  
  MRawRunHeader runheader_off;
  MRawEvtHeader evtheader_off;
  MTime              time_off;
  MLiveTime      livetime_off;
  MHillas          hillas_off;
  MHillasSrc    hillassrc_off;
  MSrcPosCam       srcpos_off;
  MReportDrive      drive_off;
  
  plist_off.AddToList(&geomcam);
  plist_off.AddToList(&runheader_off);
  plist_off.AddToList(&evtheader_off);
  plist_off.AddToList(&time_off);
  plist_off.AddToList(&livetime_off);
  plist_off.AddToList(&hillas_off);
  plist_off.AddToList(&hillassrc_off);
  plist_off.AddToList(&srcpos_off);
  plist_off.AddToList(&drive_off);

  TH1F *hSize_off      = new TH1F ("hSizeOff","",nbins_Size,min_Size,max_Size);
  TH1F *hWidth_off     = new TH1F ("hWidthOff","",nbins_Width,min_Width,max_Width);
  TH1F *hLength_off    = new TH1F ("hLengthOff","",nbins_Length,min_Length,max_Length);
  TH1F *hDist_off      = new TH1F ("hDistOff","",nbins_Dist,min_Dist,max_Dist);
  TH1F *hAlpha_off_abs = new TH1F ("hAbsAlphaOff","",nbins_abs,minalpha_abs,maxalpha_abs);
  TH1F *hAlpha_off     = new TH1F ("hAlphaOff","",nbins,minalpha,maxalpha);
  TH2F *hSrcPos_off    = new TH2F ("hSrcPosOff","",nbins_srcpos,minsrcpos,maxsrcpos,nbins_srcpos,minsrcpos,maxsrcpos);

  //tasks
  MReadTree read_off("Parameters", f_off_name);
  read_off.DisableAutoScheme();

  // taks to set the src position in the off data for the time bins
  // --------------------------------------------------------------
  MSrcPlace srcplace_timebin;
  srcplace_timebin.SetCreateHisto(kFALSE);
  srcplace_timebin.SetMode(MSrcPlace::kOff);
  // --------------------------------------------------------------

  //  srcplace.SetMode(MSrcPlace::kOff);
  
  MHillasSrcCalc csrc_off;
  
  MLiveTimeCalc livetimecalc_off;

 //tasklist
  tlist_off.AddToList(&read_off);
  tlist_off.AddToList(&livetimecalc_off);
  tlist_off.AddToList(&srcplace_timebin); // This is just to run the preprocess correctely
  tlist_off.AddToList(&csrc_off); // This is just to run the preprocess correctely
  
  // Create and setup the eventloop
  MEvtLoop loop_off;
  loop_off.SetParList(&plist_off);
  //loop_off.SetDisplay(display);
  
//   MProgressBar bar_off;
//   loop_off.SetProgressBar(&bar_off);
  
//   if (!loop_off.Eventloop(numEntries))
//     return;
  
  if (!loop_off.PreProcess())
    return;

  while(tlist_off.Process())
    {

      //First read the variables source(i.e. time bin) independent
      Double_t zenith = drive_off.GetNominalZd();

      Double_t size = hillas_off.GetSize();
      Double_t width = hillas_off.GetWidth()*geomcam.GetConvMm2Deg();
      Double_t length = hillas_off.GetLength()*geomcam.GetConvMm2Deg();
      Double_t meanx = hillas_off.GetMeanX()*geomcam.GetConvMm2Deg();
      Double_t meany = hillas_off.GetMeanY()*geomcam.GetConvMm2Deg();
      Double_t centerdist = TMath::Sqrt(meanx*meanx + meany*meany);
      
      Int_t sizebin = GetBin(size,numberSizeBins,sizeBins);
      
      if (sizebin >= 0)
	{
	  if (width > widthmin[sizebin] && width < widthmax[sizebin])
	    {
	      if (length > lengthmin[sizebin] && length < lengthmax[sizebin])
		{
		  if (centerdist < distmax[sizebin])
		    {
		      //General histos

		      srcplace_timebin.SetPositionHisto(hSrcPos_on);
		      srcplace_timebin.CallProcess();
		      csrc_off.CallProcess();
		      
		      Double_t dist = hillassrc_off.GetDist()*geomcam.GetConvMm2Deg();
		      Double_t alpha = hillassrc_off.GetAlpha();
		      Double_t srcposx = srcpos_off.GetX();
		      Double_t srcposy = srcpos_off.GetY();
		      
		      if (dist > distmin[sizebin] )
			{
			  hSize_off->Fill(log10(size));
			  hWidth_off->Fill(width);
			  hLength_off->Fill(length);
			  
			  hDist_off->Fill(dist);
			  hAlpha_off_abs->Fill(TMath::Abs(alpha));
			  hAlpha_off->Fill(alpha);
			  hSrcPos_off->Fill(srcposx,srcposy);
			}
		      
		      // Time bin histos
		      for (UInt_t bin=0; bin<numberTimeBins; bin++)
			{
			  srcplace_timebin.SetPositionHisto((TH2F*)srcposHistoOn[bin]);
			  srcplace_timebin.CallProcess();
			  csrc_off.CallProcess();
			  
			  dist = hillassrc_off.GetDist()*geomcam.GetConvMm2Deg();
			  alpha = hillassrc_off.GetAlpha();
			  srcposx = srcpos_off.GetX();
			  srcposy = srcpos_off.GetY();
			  
			  if (dist > distmin[sizebin])
			    {
			      ((TH1F*)alphaHistoOff[bin])->Fill(TMath::Abs(alpha));
			      ((TH2F*)srcposHistoOff[bin])->Fill(srcposx,srcposy);
			    }
			}
		    }
		}
	    }
	}
    }
  
  loop_off.PostProcess();

  tlist_off.PrintStatistics();

  TArrayD meanTimeBinOnInSec(numberTimeBins);
  TArrayD widthTimeBinOnInSec(numberTimeBins);
  
  TArrayD meanTriggerRateOn(numberTimeBins);
  TArrayD errorMeanTriggerRateOn(numberTimeBins);

  for (UInt_t bin=0; bin<numberTimeBins; bin++)
    {
      cout.precision(5);
      cout << bin << " timeOn " << timeOn[bin] << " mean-width time " << meanTimeBinOn[bin] << "-" << widthTimeBinOn[bin] << endl;
    }
  livetime_off.Print("last");
  livetime_off.Print("all");
  cout << "livetime_off.GetLiveTime() " << livetime_off.GetLiveTime() << endl;

  for (UInt_t bin=0; bin<numberTimeBins; bin++)
    {
      timeOff[bin] = livetime_off.GetLiveTime();

      //
      meanTriggerRateOn[bin] = numberOnEventsAfterCleaning[bin]/timeOn[bin];
      errorMeanTriggerRateOn[bin] = TMath::Sqrt(numberOnEventsAfterCleaning[bin])/timeOn[bin];

      meanTimeBinOnInSec[bin]  = (meanTimeBinOn[bin]-(Int_t)meanTimeBinOn[bin]);
      if (meanTimeBinOnInSec[bin] > 0.5)
	meanTimeBinOnInSec[bin] = meanTimeBinOnInSec[bin] - 1 ;
      meanTimeBinOnInSec[bin] *= kDay;

      widthTimeBinOnInSec[bin] = widthTimeBinOn[bin]*kDay;

      numberOffEvents[bin] = (Double_t) ((TH1F*)alphaHistoOff[bin])->Integral((Int_t)sigexccmin*nbins_abs/90+1,(Int_t)sigexccmax*nbins_abs/90);
      numberBkgEventsToNormOff[bin] =  (Double_t) ((TH1F*)alphaHistoOff[bin])->Integral((Int_t)bkgnormmin*nbins_abs/90+1,(Int_t)bkgnormmax*nbins_abs/90);
      if (numberOffEvents[bin] != 0 && numberBkgEventsToNormOff[bin] != 0)
	{
	  onOffNormFactorWithEvents[bin] = numberBkgEventsToNormOn[bin]/numberBkgEventsToNormOff[bin];
	  onOffNormFactorWithLiveTime[bin] = timeOn[bin]/timeOff[bin];
	  onOffNormFactor[bin] = onOffNormFactorWithEvents[bin];
	  numberExcessEvents[bin] = numberOnEvents[bin] - onOffNormFactor[bin]*numberOffEvents[bin];
	  errorNumberExcessEvents[bin] = TMath::Sqrt(numberOnEvents[bin] + onOffNormFactor[bin]*onOffNormFactor[bin]*numberOffEvents[bin]);
	  numberExcessEventsPerSec[bin] = numberExcessEvents[bin]/timeOn[bin]*(deadFractionOn[bin]>1.?deadFractionOn[bin]:1.);
	  errorNumberExcessEventsPerSec[bin] = errorNumberExcessEvents[bin]/timeOn[bin];
	  numberExcessEventsPerMin[bin] = 60.*numberExcessEvents[bin]/timeOn[bin]*(deadFractionOn[bin]>1.?deadFractionOn[bin]:1.);
	  errorNumberExcessEventsPerMin[bin] = 60.*errorNumberExcessEvents[bin]/timeOn[bin];

	}
    }
  
  for (UInt_t bin=0; bin<numberTimeBins; bin++)
    {
      cout.precision(5);
      cout << bin << " timeOn " << timeOn[bin] << " mean-width time " << meanTimeBinOnInSec[bin] << "-" << widthTimeBinOnInSec[bin] << endl;
      cout << " numberOnEvents\t " << numberOnEvents[bin] << endl;
      cout << " numberOffEvents\t " << numberOffEvents[bin] << endl;
      cout << " numberBkgEventsToNormOn\t " << numberBkgEventsToNormOn[bin] << endl;
      cout << " numberBkgEventsToNormOff\t " << numberBkgEventsToNormOff[bin] << endl;
      cout << " onOffNormFactorWithEvents\t " << onOffNormFactorWithEvents[bin] << endl;
      cout << " onOffNormFactorWithLiveTime\t " << onOffNormFactorWithLiveTime[bin] << endl;
      cout << " numberExcessEvents\t " <<  numberExcessEvents[bin] <<  " +- " << errorNumberExcessEvents[bin] << endl;
      cout << " deadFraction\t" << deadFractionOn[bin] << endl;
      cout << " Excess/Sec\t " << numberExcessEventsPerSec[bin] << " +- " << errorNumberExcessEventsPerSec[bin] << endl;
      cout << " Trigger Rate\t " << meanTriggerRateOn[bin] << " +- " << errorMeanTriggerRateOn[bin] << endl;
    }

  

  TString openpsname = psname + "(";
  TString closepsname = psname + ")";

  TCanvas *c0 = new TCanvas;
  c0->cd(1);
  TGraphErrors* lightcurvegraph = new TGraphErrors(numberTimeBins,meanTimeBinOnInSec.GetArray(),numberExcessEventsPerMin.GetArray(),widthTimeBinOnInSec.GetArray(),errorNumberExcessEventsPerMin.GetArray());
  lightcurvegraph->SetTitle("LightCurve");
  lightcurvegraph->SetMinimum(0.);
  lightcurvegraph->SetMarkerStyle(21);
  lightcurvegraph->SetMarkerSize(0.03);
  lightcurvegraph->Draw("AP");
  lightcurvegraph->GetYaxis()->SetTitle("Excess/min");
  lightcurvegraph->GetXaxis()->SetTitle("UTC Time");
  lightcurvegraph->GetXaxis()->SetTimeDisplay(1);
  c0->Print(openpsname);
      
  TCanvas *c00 = new TCanvas;
  c00->cd(1);
  TGraphErrors* cosmicrategraph = new TGraphErrors(numberTimeBins,meanTimeBinOnInSec.GetArray(),meanTriggerRateOn.GetArray(),widthTimeBinOnInSec.GetArray(),errorMeanTriggerRateOn.GetArray());
  cosmicrategraph->SetTitle("Cosmic Rate");
  cosmicrategraph->SetMarkerStyle(21);
  cosmicrategraph->SetMarkerSize(0.03);
  cosmicrategraph->Draw("AP");
  cosmicrategraph->GetYaxis()->SetTitle("[Hz]");
  cosmicrategraph->GetXaxis()->SetTitle("UTC Time");
  cosmicrategraph->GetXaxis()->SetTimeDisplay(1);
  c00->Print(psname);

  TCanvas** c = new TCanvas*[numberTimeBins];
  
  //Compute the maximum of all hAlphaOn histograms
  Float_t maxAlphaHistoHeight = 0;
  
  for (UInt_t bin=0; bin<numberTimeBins; bin++)
    {
      for (UInt_t i=1; i<=nbins_abs; i++)
	if (((TH1F*)alphaHistoOn[bin])->GetBinContent(i) > maxAlphaHistoHeight)
	  maxAlphaHistoHeight = ((TH1F*)alphaHistoOn[bin])->GetBinContent(i);
    }      
  
  cout << "maxAlphaHistoHeight " << maxAlphaHistoHeight << endl;

  for (UInt_t bin=0; bin<numberTimeBins-1; bin++)
    {
      c[bin] = new TCanvas;
      c[bin]->cd(1);
      
      ((TH1F*)alphaHistoOn[bin])->Sumw2();
      ((TH1F*)alphaHistoOff[bin])->SetStats(0);
      ((TH1F*)alphaHistoOff[bin])->Sumw2();
      ((TH1F*)alphaHistoOff[bin])->Scale(onOffNormFactor[bin]); 
      ((TH1F*)alphaHistoOn[bin])->SetStats(0); //-> Do NOT show the legend with statistics
      ((TH1F*)alphaHistoOn[bin])->SetLineColor(kBlack);
      ((TH1F*)alphaHistoOn[bin])->SetMarkerStyle(21);
      ((TH1F*)alphaHistoOn[bin])->SetMarkerColor(kBlack);
      ((TH1F*)alphaHistoOn[bin])->SetMarkerSize(0.7);
      ((TH1F*)alphaHistoOff[bin])->SetFillColor(46);
      ((TH1F*)alphaHistoOff[bin])->SetLineColor(46);
      ((TH1F*)alphaHistoOff[bin])->SetFillStyle(3004); //(1001)-> To set the pad NOT transparent and solid; (3004)-> pattern lines
      ((TH1F*)alphaHistoOn[bin])->SetMaximum(maxAlphaHistoHeight*1.2);
      ((TH1F*)alphaHistoOff[bin])->SetMinimum(0.);
      alphaTitle =  Form("%s%02i","hAlphaOnOff",bin);
      ((TH1F*)alphaHistoOn[bin])->SetTitle(alphaTitle);
      
      
      ((TH1F*)alphaHistoOn[bin])->DrawCopy("E1P");
      ((TH1F*)alphaHistoOff[bin])->DrawCopy("HISTSAME");
      ((TH1F*)alphaHistoOff[bin])->DrawCopy("ESAME");
      ((TH1F*)alphaHistoOn[bin])->DrawCopy("E1PSAME");

      c[bin]->Print(psname);
    }

  c[numberTimeBins-1] = new TCanvas;
  c[numberTimeBins-1]->cd(1);
  
  ((TH1F*)alphaHistoOn[numberTimeBins-1])->Sumw2();
  ((TH1F*)alphaHistoOff[numberTimeBins-1])->SetStats(0);
  ((TH1F*)alphaHistoOff[numberTimeBins-1])->Sumw2();
  ((TH1F*)alphaHistoOff[numberTimeBins-1])->Scale(onOffNormFactor[numberTimeBins-1]); 
  ((TH1F*)alphaHistoOn[numberTimeBins-1])->SetStats(0); //-> Do NOT show the legend with statistics
  ((TH1F*)alphaHistoOn[numberTimeBins-1])->SetLineColor(kBlack);
  ((TH1F*)alphaHistoOn[numberTimeBins-1])->SetMarkerStyle(21);
  ((TH1F*)alphaHistoOn[numberTimeBins-1])->SetMarkerColor(kBlack);
  ((TH1F*)alphaHistoOn[numberTimeBins-1])->SetMarkerSize(0.7);
  ((TH1F*)alphaHistoOff[numberTimeBins-1])->SetFillColor(46);
  ((TH1F*)alphaHistoOff[numberTimeBins-1])->SetLineColor(46);
  ((TH1F*)alphaHistoOff[numberTimeBins-1])->SetFillStyle(3004); //(1001)-> To set the pad NOT transparent and solid; (3004)-> pattern lines
  ((TH1F*)alphaHistoOn[numberTimeBins-1])->SetMaximum(maxAlphaHistoHeight*1.2);
  ((TH1F*)alphaHistoOff[numberTimeBins-1])->SetMinimum(0.);
  alphaTitle =  Form("%s%02i","hAlphaOnOff",numberTimeBins-1);
  ((TH1F*)alphaHistoOn[numberTimeBins-1])->SetTitle(alphaTitle);
  
  
  ((TH1F*)alphaHistoOn[numberTimeBins-1])->DrawCopy("E1P");
  ((TH1F*)alphaHistoOff[numberTimeBins-1])->DrawCopy("HISTSAME");
  ((TH1F*)alphaHistoOff[numberTimeBins-1])->DrawCopy("ESAME");
  ((TH1F*)alphaHistoOn[numberTimeBins-1])->DrawCopy("E1PSAME");
  
  c[numberTimeBins-1]->Print(psname);

  //  TString rootname = psname.ReplaceAll(".ps",".root");
  TString rootname = "./prueba.root";
  TFile input(rootname, "RECREATE");

   for (UInt_t bin=0; bin<numberTimeBins; bin++)
     {
       ((TH1F*)alphaHistoOn[bin])->Write();
       ((TH2F*)srcposHistoOn[bin])->Write();
       ((TH1F*)coszenithHistoOn[bin])->Write();
       ((TH1F*)timediffHistoOn[bin])->Write();
       ((TH1F*)alphaHistoOff[bin])->Write();
       ((TH2F*)srcposHistoOff[bin])->Write();
    }
  
  input.Close();

  // ############################################################################
  // Calculate significance and excess: 
  // ############################################################################

  Double_t norm_on_abs  = (Double_t) hAlpha_on_abs->Integral((Int_t)bkgnormmin*nbins_abs/90+1,(Int_t)bkgnormmax*nbins_abs/90);
  Double_t exces_on_abs = (Double_t) hAlpha_on_abs->Integral((Int_t)sigexccmin*nbins_abs/90+1,(Int_t)sigexccmax*nbins_abs/90);
  Double_t norm_off_abs  = (Double_t) hAlpha_off_abs->Integral((Int_t)bkgnormmin*nbins_abs/90+1,(Int_t)bkgnormmax*nbins_abs/90);
  Double_t exces_off_abs = (Double_t) hAlpha_off_abs->Integral((Int_t)sigexccmin*nbins_abs/90+1,(Int_t)sigexccmax*nbins_abs/90);
  Double_t norm = norm_on_abs/norm_off_abs;

  char text_tit_alpha[256];
  sprintf(text_tit_alpha, " Alpha Plot On and Off ");
  hAlpha_off_abs->SetTitle(text_tit_alpha);
  hAlpha_on_abs->SetTitle(text_tit_alpha);

  Double_t excess  = exces_on_abs - exces_off_abs*norm;
  Double_t sign    = excess / sqrt( exces_on_abs + norm*norm*exces_off_abs );
  Double_t int_off = (Double_t) hAlpha_off_abs->Integral(1, 18);
  int hAlpha_on_entries  = (int) hAlpha_on_abs->GetEntries();
  int hAlpha_off_entries = (int) hAlpha_off_abs->GetEntries();
    
  cout << "---> Normalization F factor =\t" << norm <<endl;
  cout << "---> Excess =\t\t\t" << excess <<endl;
  cout << "---> Significancia =\t\t" << sign <<endl;    
  cout << "---> entries on   =\t\t" << hAlpha_on_entries  <<endl;
  cout << "---> entries off  =\t\t" << hAlpha_off_entries <<endl;
  cout << "---> integral off =\t\t" << int_off <<endl;

  Double_t shiftx;

  // ############################################################################
  // Draw SIZE
  // ############################################################################
  TCanvas *c1 = new TCanvas;

  gPad->cd();

  gPad->SetLogy();
  hSize_on->Sumw2();
  hSize_off->Sumw2();
  hSize_off->Scale(norm); 
  hSize_on->SetLineColor(kBlack);
  hSize_on->SetMarkerStyle(21);
  hSize_on->SetMarkerSize(0.7);
  hSize_on->SetMarkerColor(kBlack);
  hSize_off->SetFillColor(46);
  hSize_off->SetLineColor(46);
  hSize_off->SetFillStyle(3004); //(1001)-> To set the pad NOT transparent and solid; (3004)-> pattern lines
  hSize_off->SetMinimum(0.1);
  hSize_on->SetMinimum(0.1);
  hSize_on->SetTitle("SIZE distribution");
  hSize_off->SetTitle("SIZE distribution");

  hSize_on->DrawCopy("E1P");

  // move stat box to make them all visible
  gPad->Update();
  TPaveStats* pavs_on_size = (TPaveStats*) hSize_on->GetListOfFunctions()->FindObject("stats");
  if(pavs_on_size){
    shiftx = pavs_on_size->GetX2NDC() - pavs_on_size->GetX1NDC();
    pavs_on_size->SetX1NDC(pavs_on_size->GetX1NDC() - shiftx);
    pavs_on_size->SetX2NDC(pavs_on_size->GetX2NDC() - shiftx);  
  }
  gPad->Modified();
  gPad->Update();

  hSize_off->DrawCopy("HISTSAME");
  hSize_off->DrawCopy("ESAME");

  gPad->Modified();
  gPad->Update();

  Double_t chisize = ChiSquareNDof((TH1D*)hSize_on,(TH1D*)hSize_off);

  Double_t x_label_pos  = log10(1000000)*0.7;
  Double_t y_label_pos  = log10((hSize_on->GetBinContent(hSize_on->GetMaximumBin()))/2.);
  Double_t textsize = 0.03;

  char text_size[256];
  sprintf(text_size,"ChiSquare/NDof = %4.2f",chisize);

  TLatex *tsize = new TLatex(x_label_pos, y_label_pos, text_size);
  tsize->SetTextSize(textsize);
//  tsize->Draw();

  gPad->Modified();
  gPad->Update();

  c1->Print(psname);

  // ############################################################################
  // DrawCopy DIST
  // ############################################################################
  TCanvas *c2 = new TCanvas;

  gPad->cd();

  hDist_on->Sumw2();
  hDist_off->Sumw2();
  hDist_off->Scale(norm); 
  hDist_on->SetLineColor(kBlack);
  hDist_on->SetMarkerStyle(21);
  hDist_on->SetMarkerSize(0.7);
  hDist_on->SetMarkerColor(kBlack);
  hDist_off->SetFillColor(46);
  hDist_off->SetLineColor(46);
  hDist_off->SetFillStyle(3004); //(1001)-> To set the pad NOT transparent and solid; (3004)-> pattern lines
  hDist_off->SetMinimum(0.);
  hDist_on->SetTitle("DIST distribution");
  hDist_off->SetTitle("DIST distribution");

  hDist_on->DrawCopy("E1P");

  // move stat box to make them all visible
  gPad->Update();
  TPaveStats* pavs_on_dist = (TPaveStats*) hDist_on->GetListOfFunctions()->FindObject("stats");
  if(pavs_on_dist){
    shiftx = pavs_on_dist->GetX2NDC() - pavs_on_dist->GetX1NDC();
    pavs_on_dist->SetX1NDC(pavs_on_dist->GetX1NDC() - shiftx);
    pavs_on_dist->SetX2NDC(pavs_on_dist->GetX2NDC() - shiftx);  
  }
  gPad->Modified();
  gPad->Update();

  hDist_off->DrawCopy("HISTSAME");
  hDist_off->DrawCopy("ESAME");
  hDist_on->DrawCopy("E1PSAME");

  Double_t chidist = ChiSquareNDof((TH1D*)hDist_on,(TH1D*)hDist_off);

  x_label_pos  = distmax[numberSizeBins-1]*0.7;
  y_label_pos  = hDist_on->GetBinContent(hDist_on->GetMaximumBin())/2.;

  char text_dist[256];
  sprintf(text_size,"ChiSquare/NDof = %4.2f",chidist);

  TLatex *tdist = new TLatex(x_label_pos, y_label_pos, text_dist);
  tdist->SetTextSize(textsize);
//  tdist->Draw();

  gPad->Modified();
  gPad->Update();

  c2->Print(psname);

   // ############################################################################
  // DrawCopy WIDTH
  // ############################################################################
  TCanvas *c3 = new TCanvas;

  gPad->cd();

  hWidth_off->Sumw2();
  hWidth_off->Scale(norm); 
  hWidth_on->SetLineColor(kBlack);
  hWidth_on->SetMarkerStyle(21);
  hWidth_on->SetMarkerSize(0.7);
  hWidth_on->SetMarkerColor(kBlack);
  hWidth_off->SetFillColor(46);
  hWidth_off->SetLineColor(46);
  hWidth_off->SetFillStyle(3004); //(1001)-> To set the pad NOT transparent and solid; (3004)-> pattern lines
  hWidth_off->SetMinimum(0.);
  hWidth_on->SetTitle("WIDTH distribution");
  hWidth_off->SetTitle("WIDTH distribution");

  hWidth_on->DrawCopy("E1P");

  // move stat box to make them all visible
  gPad->Update();
  TPaveStats* pavs_on_width = (TPaveStats*) hWidth_on->GetListOfFunctions()->FindObject("stats");
  if(pavs_on_width){
    shiftx = pavs_on_width->GetX2NDC() - pavs_on_width->GetX1NDC();
    pavs_on_width->SetX1NDC(pavs_on_width->GetX1NDC() - shiftx);
    pavs_on_width->SetX2NDC(pavs_on_width->GetX2NDC() - shiftx);  
  }
  gPad->Modified();
  gPad->Update();

  hWidth_off->DrawCopy("HISTSAME");
  hWidth_off->DrawCopy("ESAME");
  hWidth_on->DrawCopy("E1PSAME");

  Double_t chiwidth = ChiSquareNDof((TH1D*)hWidth_on,(TH1D*)hWidth_off);

  x_label_pos  = widthmax[numberSizeBins-1]*0.7;
  y_label_pos  = hWidth_on->GetBinContent(hWidth_on->GetMaximumBin())/2.;

  char text_width[256];
  sprintf(text_size,"ChiSquare/NDof = %4.2f",chiwidth);

  TLatex *twidth = new TLatex(x_label_pos, y_label_pos, text_width);
  twidth->SetTextSize(textsize);
//  twidth->Draw();

  gPad->Modified();
  gPad->Update();
 
  c3->Print(psname);

  // ############################################################################
  // DrawCopy LENGTH
  // ############################################################################
  TCanvas *c4 = new TCanvas;
 
  gPad->cd();

  hLength_on->Sumw2();
  hLength_off->Sumw2();
  hLength_off->Scale(norm); 
  hLength_on->SetLineColor(kBlack);
  hLength_on->SetMarkerStyle(21);
  hLength_on->SetMarkerSize(0.7);
  hLength_on->SetMarkerColor(kBlack);
  hLength_off->SetFillColor(46);
  hLength_off->SetLineColor(46);
  hLength_off->SetFillStyle(3004); //(1001)-> To set the pad NOT transparent and solid; (3004)-> pattern lines
  hLength_off->SetMinimum(0.);
  hLength_on->SetTitle("LENGTH distribution");
  hLength_off->SetTitle("LENGTH distribution");

  hLength_on->DrawCopy("E1P");

  // move stat box to make them all visible
  gPad->Update();
  TPaveStats* pavs_on_length = (TPaveStats*) hLength_on->GetListOfFunctions()->FindObject("stats");
  if(pavs_on_length){
    shiftx = pavs_on_length->GetX2NDC() - pavs_on_length->GetX1NDC();
    pavs_on_length->SetX1NDC(pavs_on_length->GetX1NDC() - shiftx);
    pavs_on_length->SetX2NDC(pavs_on_length->GetX2NDC() - shiftx);  
  }
  gPad->Modified();
  gPad->Update();

  hLength_off->DrawCopy("HISTSAME");
  hLength_off->DrawCopy("ESAME");
  hLength_on->DrawCopy("E1PSAME");

  Double_t chilength = ChiSquareNDof((TH1D*)hLength_on,(TH1D*)hLength_off);

  x_label_pos  = lengthmax[numberSizeBins-1]*0.7;
  y_label_pos  = hLength_on->GetBinContent(hLength_on->GetMaximumBin())/2.;

  char text_length[256];
  sprintf(text_size,"ChiSquare/NDof = %4.2f",chilength);

  TLatex *tlength = new TLatex(x_label_pos, y_label_pos, text_length);
  tlength->SetTextSize(textsize);
//  tlength->Draw();

  gPad->Modified();
  gPad->Update();

  c4->Print(psname);

 // ############################################################################
  // DrawCopy normalized ALPHA plot
  // ############################################################################
  TCanvas *c5 = new TCanvas;
  
  gPad->cd();

  hAlpha_on_abs->Sumw2();
  hAlpha_off_abs->SetStats(0);
  hAlpha_off_abs->Sumw2();
  hAlpha_off_abs->Scale(norm); 
  hAlpha_on_abs->SetStats(0); //-> Do NOT show the legend with statistics
  hAlpha_on_abs->SetLineColor(kBlack);
  hAlpha_on_abs->SetMarkerStyle(21);
  //hAlpha_on_abs->SetMarkerSize();
  hAlpha_on_abs->SetMarkerColor(kBlack);
  hAlpha_on_abs->SetMarkerSize(0.7);
  hAlpha_off_abs->SetFillColor(46);
  hAlpha_off_abs->SetLineColor(46);
  hAlpha_off_abs->SetFillStyle(3004); //(1001)-> To set the pad NOT transparent and solid; (3004)-> pattern lines
  hAlpha_off_abs->SetMinimum(0.);
  hAlpha_on_abs->SetTitle("Alpha plot");
  hAlpha_off_abs->SetTitle("Alpha plot");

  
  hAlpha_on_abs->DrawCopy("E1P");
  hAlpha_off_abs->DrawCopy("HISTSAME");
  hAlpha_off_abs->DrawCopy("ESAME");
  hAlpha_on_abs->DrawCopy("E1PSAME");


   //draw the LEGEND with excess and significance values in the alpha plot:
  char text_Fnorm[256], text_excess[256], text_sign[256];
  char text_entries_on[256], text_entries_off[256], text_integral_off[256];
  int hAlpha_on_entries  = (int) hAlpha_on_abs->GetEntries();
  int hAlpha_off_entries = (int) hAlpha_off_abs->GetEntries();
  sprintf(text_Fnorm,       " F norm =       %.3f", norm);
  sprintf(text_excess,      " Excess =       %.3f", excess);
  sprintf(text_sign,        " Significance = %.3f", sign);
  sprintf(text_entries_on,  " Entries ON   = %d",  hAlpha_on_entries);
  sprintf(text_entries_off, " Entries OFF  = %d",  hAlpha_off_entries);
  sprintf(text_integral_off," Integral OFF = %d",  int_off);
  
  x_label_pos  = 90.*0.7;
  y_label_pos  = (hAlpha_on_abs->GetBinContent(hAlpha_on_abs->GetMaximumBin())); //2.;
  Double_t y_label_step = y_label_pos / 8.;

  TLatex *t0 = new TLatex(x_label_pos, y_label_pos - y_label_step*0, text_Fnorm);
  t0->SetTextSize(textsize);
  t0->Draw();
  TLatex *t1 = new TLatex(x_label_pos, y_label_pos - y_label_step*1, text_excess);
  t1->SetTextSize(textsize);
  t1->Draw();
  TLatex *t2 = new TLatex(x_label_pos, y_label_pos - y_label_step*2, text_sign);
  t2->SetTextSize(textsize);
  t2->Draw();
  TLatex *t3 = new TLatex(x_label_pos, y_label_pos - y_label_step*3, text_entries_on);
  t3->SetTextSize(textsize);
  t3->Draw();
  TLatex *t4 = new TLatex(x_label_pos, y_label_pos - y_label_step*4, text_entries_off);
  t4->SetTextSize(textsize);
  t4->Draw();
  TLatex *t5 = new TLatex(x_label_pos, y_label_pos - y_label_step*5, text_integral_off);
  t5->SetTextSize(textsize);
  t5->Draw();
  

  Double_t chialpha = ChiSquareNDof((TH1D*)hAlpha_on_abs,(TH1D*)hAlpha_off_abs);

  y_label_pos  = (hAlpha_on_abs->GetBinContent(hAlpha_on_abs->GetMaximumBin()))/2.;

  char text_alpha[256];
  sprintf(text_size,"ChiSquare/NDof = %4.2f",chialpha);

  TLatex *talpha = new TLatex(x_label_pos, y_label_pos, text_alpha);
  talpha->SetTextSize(textsize);
//  talpha->Draw();

  gPad->Modified();
  gPad->Update();

  c5->Print(psname);

  // ############################################################################
  // DrawCopy normalized alpha histos for alpha form -90 to 90 deg.
  // ############################################################################
  TCanvas *c6 = new TCanvas;

  gPad->cd();

  hAlpha_on->Sumw2();
  hAlpha_off->SetStats(0);
  hAlpha_off->Sumw2();
  hAlpha_off->Scale(norm); 
  hAlpha_off->SetFillColor(46);
  hAlpha_off->SetLineColor(46);
  hAlpha_off->SetFillStyle(3004); //(1001)-> To set the pad NOT transparent and solid; (3004)-> pattern lines
  hAlpha_off->SetMinimum(0.); 
  hAlpha_on->SetStats(0); //-> Do NOT show the legend with statistics
  hAlpha_on->SetLineColor(kBlack);
  hAlpha_on->SetMarkerStyle(21);
  hAlpha_on->SetMarkerSize(0.7);
  hAlpha_on->SetMarkerColor(kBlack);
  hAlpha_on->SetTitle("Alpha plot form -90 to 90 deg");
  hAlpha_off->SetTitle("Alpha plot form -90 to 90 deg");

  hAlpha_on->DrawCopy("E1P");
  hAlpha_off->DrawCopy("HISTSAME");
  hAlpha_off->DrawCopy("ESAME");
  hAlpha_on->DrawCopy("E1PSAME");

  Double_t chialpha90 = ChiSquareNDof((TH1D*)hAlpha_on,(TH1D*)hAlpha_off);

  x_label_pos  = 90.*0.5;
  y_label_pos  = hAlpha_on->GetBinContent(hAlpha_on->GetMaximumBin())/2.;

  char text_alpha90[256];
  sprintf(text_alpha90,"ChiSquare/NDof = %4.2f",chialpha90);

  TLatex *talpha90 = new TLatex(x_label_pos, y_label_pos, text_alpha90);
  talpha90->SetTextSize(textsize);
//  talpha90->Draw();

  gPad->Update();
  gPad->Modified();

  c6->Print(psname);

  cout << "---> ChiSquare/NDof [Size] =\t\t" << chisize << endl;
  cout << "---> ChiSquare/NDof [Dist] =\t\t" << chidist << endl;
  cout << "---> ChiSquare/NDof [Width] =\t\t" << chiwidth << endl;
  cout << "---> ChiSquare/NDof [Length] =\t\t" << chilength << endl;
  cout << "---> ChiSquare/NDof [Abs(Alpha)] =\t" << chialpha << endl;
  cout << "---> ChiSquare/NDof [Alpha] =\t\t" << chialpha90 << endl;


  TCanvas *c7 = new TCanvas;
  hSrcPos_on->DrawCopy("BOX");
  c7->Print(psname);

  TCanvas *c8 = new TCanvas;
  hSrcPos_off->DrawCopy("BOX");
  c8->Print(closepsname);

  cout << "Done!!" <<endl;
  
}


Double_t ChiSquareNDof(TH1D *h1, TH1D *h2)
{
    Double_t chiq = 0.;
    Double_t chi;
    Double_t error;
    Int_t nbinsnozero = 0;

    Int_t nbins = h1->GetNbinsX();
    if (nbins != h2->GetNbinsX() || nbins == 0)
	return -1;

    for (UInt_t bin=1; bin<=nbins; bin++)
    {
	error = sqrt(h1->GetBinError(bin)*h1->GetBinError(bin) +
			   h2->GetBinError(bin)*h2->GetBinError(bin));
	if (error != 0)
	{
	    chi = (h1->GetBinContent(bin)-h2->GetBinContent(bin))/error;
	    chiq += chi*chi;
	    nbinsnozero++;
	}
    }

    return (nbinsnozero>0?chiq/nbinsnozero:0);
}

Int_t GetBin(Double_t size, Int_t numberSizeBins, Double_t sizeBins[])
{

  Int_t result = -1;

  Int_t lowerbin = 0;
  Int_t upperbin = numberSizeBins;
  Int_t bin;

  Int_t count = 0;

  if (size >= sizeBins[0])
    {
      while (upperbin - lowerbin > 1 && count++<=numberSizeBins)
	{
	  bin = (upperbin+lowerbin)/2;
	  if (size >= sizeBins[bin])
	    lowerbin = bin;
	  else
	    upperbin = bin;
	}
      result = count<=numberSizeBins?lowerbin:-1;
    }

  return result;

}

