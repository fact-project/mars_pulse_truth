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
!   Author(s): Javier Lopez, 12/2003 <mailto:jlopez@ifae.es>
!   Author(s): Alex Armada,   1/2004 <mailto:armada@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */


//------------------------------------------------------------------------- //
//                                                                          //
//  This macro fits the dc signal of a star using a two dimension gaussian  //
//  for each dc measurement. Then the values of parameters of the fit are   //
//  stored in histograms and shown at the end of the macro.                 //
//                                                                          //
//  USAGE:                                                                  //
//  It has two arguments,                                                   //
//  1- The first one is the dc file with the tracked star                   //
//  2- The second one is a continuos light file used to intercalibrate      //
//     the gain of the photomultipliers.                                    //
//     (It's possible not to use the calibration file and then the gain     //
//      of the pmts are supouse to be the same for all of them.             //
//  3- The third argument is just the number of dc measurements you want    //
//     analize. If you put 0 it just stops after each fit and show you      //
//     results.                                                             //
//                                                                          //
//--------------------------------------------------------------------------// 

const Int_t numPixels = 577;
Int_t nPixelsFitted; 
Bool_t isPixelsFitted[numPixels];
Float_t z[numPixels],x[numPixels],y[numPixels],errorz[numPixels];
Float_t chisquare;

//______________________________________________________________________________
//
// Function used by Minuit to do the fit
//
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
    Int_t i;

//calculate chisquare
    Double_t chisq = 0;
    Double_t delta;
    nPixelsFitted=0;
    for (i=1;i<numPixels; i++) {
	if (isPixelsFitted[i])
	{
	    if (errorz[i] != 0.0)
	    {
		delta  = (z[i]-func(x[i],y[i],par))/errorz[i];
		chisq += delta*delta;
		nPixelsFitted++;
	    }
	    else
		cout << "This should never happen errorz[" << i << "] " << errorz[i] << endl;
	}
    }
    f = chisq;
    chisquare = chisq;
}

//______________________________________________________________________________
//
// The 2D gaussian fucntion used to fit the spot of the star
//
Double_t func(float x,float y,Double_t *par)
{
    Double_t value=par[0]*TMath::exp(-(x-par[1])*(x-par[1])/(2*par[2]*par[2]))*TMath::exp(-(y-par[3])*(y-par[3])/(2*par[4]*par[4]));
    return value;
}

Bool_t HandleInput()
{
    TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
    
    while (1)
    {
        //
        // While reading the input process gui events asynchronously
        //
        timer.TurnOn();
        TString input = Getline("Type 'q' to exit, <return> to go on: ");
        timer.TurnOff();
	
        if (input=="q\n")
            return kFALSE;
	
        if (input=="\n")
            return kTRUE;
    };
    
    return kFALSE;
}


void pointspreadfunction(TString fname, TString clname = "NULL", Int_t userNumLines = 1000)
{


    Float_t currmean[numPixels];
    Float_t currsquaremean[numPixels];
    Float_t currrms[numPixels];
    Float_t meanofcurrmean = 0;

    for (Int_t swpix=0; swpix<numPixels; swpix++)
    {
 	currmean[swpix] = 0.;
 	currsquaremean[swpix] = 0.;
  	currrms[swpix] = 0.;
    }
    
    Int_t numLines=0;

    //containers
    MGeomCamMagic geomcam;
    MCameraDC     curr;
    MCameraDC     currbis;
    
    const Float_t conv4mm2deg = geomcam.GetConvMm2Deg();

	 
    if (clname != "NULL")
    {

	//
	// First run over continuos light files to have a DC calibration
	//

	MParList plist0;
	
	MTaskList     tlist0;
	plist0.AddToList(&tlist0);
	
	plist0.AddToList(&geomcam);
	plist0.AddToList(&curr);
	
	MReportFileRead read0(clname);
	read0.SetHasNoHeader();
	read0.AddToList("MReportCurrents");
	
	tlist0.AddToList(&read0);
	
	MEvtLoop evtloop0;
	evtloop0.SetParList(&plist0);
	
	
	if (!evtloop0.PreProcess())
	    return;
	
	while (tlist0.Process())
	{
	    for (Int_t swpix=0; swpix<numPixels; swpix++)
	    {
		meanofcurrmean += curr[swpix];
		currmean[swpix] += curr[swpix];
		currsquaremean[swpix] += curr[swpix]*curr[swpix];
	    }
	    numLines++;
	}
	
	evtloop0.PostProcess();
	
	meanofcurrmean /= (numLines*numPixels);
	for (Int_t swpix=0; swpix<numPixels; swpix++)
	{
	    
	    currmean[swpix] /= numLines;
	    currsquaremean[swpix] /= numLines;
	    currrms[swpix] = sqrt(fabs(currsquaremean[swpix] - currmean[swpix]*currmean[swpix]));
	    
	    curr[swpix] = currmean[swpix];
	    currbis[swpix] = currrms[swpix];
	    
	    currmean[swpix] /= meanofcurrmean;	
	    currrms[swpix] /= meanofcurrmean;	
	    
	}
	
	
/*     MHCamera display0(geomcam);
       display0.SetPrettyPalette();
       display0.Draw();
       
	//    curr.Print();
       display0.SetCamContent(currbis);
       cout << "PSF>> DC mean values drawn" << endl;
       // Remove the comments if you want to go through the file
       // event-by-event:
       if (!HandleInput())
       break;
*/
    }
    else
    {
	// If you don't use the continuous light this is the currrms[] array 
	// is the error associated to the currents TMinuit will use.
	for (Int_t swpix=0; swpix<numPixels; swpix++)
	{
	    currmean[swpix] = 1.;
	    currrms[swpix] = 0.2; 
	}
	
    }

//
// Now we can run over the dc data to extract the psf. 
//	
    const Int_t maxNumLines = 10000;
	
    Double_t ux[maxNumLines];
    Double_t uy[maxNumLines];
    Double_t sx[maxNumLines];
    Double_t sy[maxNumLines];
    Double_t chisqu[maxNumLines];
    Double_t time[maxNumLines];

    MParList plist;

    MGeomCamMagic geomcam;
    MCameraDC     curr;
    MTaskList     tlist;

    plist.AddToList(&geomcam);
    plist.AddToList(&curr);
    plist.AddToList(&tlist);

    MReportFileRead read(fname);
    read.SetHasNoHeader();
    read.AddToList("MReportCurrents");

    tlist.AddToList(&read);
    
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);
    
    if (!evtloop.PreProcess())
        return;
    
    MHCamera display(geomcam);
    display.SetPrettyPalette();
    display.Draw();
    gPad->SetLogy();
    gPad->cd(1);

    Double_t amin,edm,errdef;
    Int_t nvpar,nparx,icstat;

    Double_t max,maxerror;
    Double_t xmean,xsigma,ymean,ysigma;
    Double_t xmeanerror,xsigmaerror,ymeanerror,ysigmaerror;

    TEllipse ellipse;
    ellipse.SetFillStyle(4000);
    ellipse.SetLineWidth(2);
    ellipse.SetLineColor(2);

    ellipse.Draw();

    Int_t nbinsxy = 80;
    Float_t minxy = -600*conv4mm2deg;
    Float_t maxxy = 600*conv4mm2deg;
    Float_t fromdegtobin = (maxxy-minxy)/nbinsxy;

    TH2D psfhist("psfhist","",nbinsxy,minxy,maxxy,nbinsxy,minxy,maxxy);
    psfhist->GetXaxis()->SetTitle("[deg]");
    psfhist->GetYaxis()->SetTitle("[deg]");
    psfhist->GetZaxis()->SetTitle("DC [uA]");

    TCanvas *psfcanvas = new TCanvas("psfcanvas","Point Spread Funtion 2D",200,20,900,700);


    //
    // Using the first dc measurement we search the pixels which contains the star and define
    // an area to be fitted by Minuit which is 3 rings of neightbords around the peak of the star.
    // 

    tlist.Process();

    Int_t numLines=0;
    Float_t minDCStar = 6.0;

    Int_t numPixelsInStar = 0;
    Float_t maxDC = 0;
    Int_t swpixelmaxDC;
    
    Bool_t isPixelsFittedTmp[numPixels];
    
    for(Int_t swpixel=1; swpixel<numPixels; swpixel++)
	isPixelsFittedTmp[swpixel] = kFALSE;
    
    for(Int_t swpixel=1; swpixel<numPixels; swpixel++)
    {
	if(curr[swpixel] > maxDC)
	{
	    swpixelmaxDC = swpixel;
	    maxDC = curr[swpixelmaxDC];
	}
	
	if(curr[swpixel]>minDCStar)
	{
	    numPixelsInStar++;
	    isPixelsFitted[swpixel] = kTRUE;
	}
	else
	    isPixelsFitted[swpixel] = kFALSE;
    }
    
    if (numPixelsInStar == 0)
    {
	cout << "PSF>> Warning: none pixel over minDCStar(" << minDCStar  << ')' << endl; 
	return;
    }
    
//1st neighboor ring
    for(Int_t swpixel=1; swpixel<numPixels; swpixel++)
	if (isPixelsFitted[swpixel])
	    for(Int_t next=0; next<geomcam[swpixel].GetNumNeighbors(); next++)
		isPixelsFittedTmp[geomcam[swpixel].GetNeighbor(next)] = kTRUE;
    
    for(Int_t swpixel=1; swpixel<numPixels; swpixel++)
	if (isPixelsFittedTmp[swpixel])
	    isPixelsFitted[swpixel] = kTRUE;
    
//2on neighboor ring
    for(Int_t swpixel=1; swpixel<numPixels; swpixel++)
	if (isPixelsFitted[swpixel])
	    for(Int_t next=0; next<geomcam[swpixel].GetNumNeighbors(); next++)
		isPixelsFittedTmp[geomcam[swpixel].GetNeighbor(next)] = kTRUE;
    
    for(Int_t swpixel=1; swpixel<numPixels; swpixel++)
	if (isPixelsFittedTmp[swpixel])
	    isPixelsFitted[swpixel] = kTRUE;
    

//3rt neighboor ring
    for(Int_t swpixel=1; swpixel<numPixels; swpixel++)
	if (isPixelsFitted[swpixel])
	    for(Int_t next=0; next<geomcam[swpixel].GetNumNeighbors(); next++)
		isPixelsFittedTmp[geomcam[swpixel].GetNeighbor(next)] = kTRUE;
    
    for(Int_t swpixel=1; swpixel<numPixels; swpixel++)
	if (isPixelsFittedTmp[swpixel])
	    isPixelsFitted[swpixel] = kTRUE;
	      
   
    for(Int_t swpixel=1; swpixel<numPixels; swpixel++)
	curr[swpixel] = (Float_t)isPixelsFitted[swpixel]; 
    
/*    MHCamera display0(geomcam);
    display0.SetPrettyPalette();
    display0.Draw();
    
    display0.SetCamContent(curr);
    cout << "PSF>> Fitted pixels drawn" << endl;
    // Remove the comments if you want to go through the file
    // event-by-event:
    if (!HandleInput())
	break;
*/

    // Minuit initialization

    TMinuit *gMinuit = new TMinuit(7);  //initialize TMinuit with a maximum of 5 params
    gMinuit->SetFCN(fcn);

    Double_t arglist[10];
    Int_t ierflg = 0;

    arglist[0] = 1;
    gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
//    arglist[0] = -1;
    arglist[0] = 0;
    gMinuit->mnexcm("SET PRI", arglist ,1,ierflg);

// Set starting values and step sizes for parameters
    Double_t vstart[5];
    Double_t step[5];
    Double_t lowlimit[5] = {minDCStar, -2., 0.05, -2, 0.05};
    Double_t uplimit[5] = {30., 2., 1., 2., 1.};

    vstart[0] = maxDC;
    vstart[1] = geomcam[swpixelmaxDC].GetX()*conv4mm2deg;
    vstart[2] = 30*TMath::Sqrt(numPixelsInStar/2)*conv4mm2deg;
    vstart[3] = geomcam[swpixelmaxDC].GetY()*conv4mm2deg;
    vstart[4] = 30*TMath::Sqrt(numPixelsInStar/2)*conv4mm2deg;
    
    for(Int_t i=0; i<5; i++)
    {
	if (vstart[i] != 0)
	    step[i] = TMath::Abs(vstart[i]/sqrt(2));
	else
	    step[i] = uplimit[i]/2;
    }
  
    gMinuit->mnparm(0, "max", vstart[0], step[0], lowlimit[0], uplimit[0],ierflg);
    gMinuit->mnparm(1, "xmean", vstart[1], step[1], lowlimit[1], uplimit[1],ierflg);
    gMinuit->mnparm(2, "xsigma", vstart[2], step[2], lowlimit[2], uplimit[2],ierflg);
    gMinuit->mnparm(3, "ymean", vstart[3], step[3], lowlimit[3], uplimit[3],ierflg);
    gMinuit->mnparm(4, "ysigma", vstart[4], step[4], lowlimit[4], uplimit[4],ierflg);

    while (tlist.Process() && numLines < maxNumLines)
    {

	for (Int_t swpixel=1; swpixel<577; swpixel++)
	{
	    
	    x[swpixel] = geomcam[swpixel].GetX()*conv4mm2deg;
	    y[swpixel] = geomcam[swpixel].GetY()*conv4mm2deg;
	    z[swpixel] = curr[swpixel]/currmean[swpixel];
	    errorz[swpixel] = TMath::Sqrt((curr[swpixel]*currrms[swpixel]/(currmean[swpixel]*currmean[swpixel]))*(curr[swpixel]*currrms[swpixel]/(currmean[swpixel]*currmean[swpixel]))+(0.1)/currmean[swpixel]*(0.1)/currmean[swpixel]);


	    psfhist->SetBinContent((Int_t)((x[swpixel]+600*conv4mm2deg)/fromdegtobin),(Int_t)((y[swpixel]+600*conv4mm2deg)/fromdegtobin),z[swpixel]);
	}
	
	psfcanvas->cd(1);
	psfhist->Draw("lego2");
	
// Now ready for minimization step
	arglist[0] = 500;
	arglist[1] = 1.;
	gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);

// Print results
/*	gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
	gMinuit->mnprin(3,amin);
*/
	gMinuit->GetParameter(0,max,maxerror);
	gMinuit->GetParameter(1,xmean,xmeanerror);
	gMinuit->GetParameter(2,xsigma,xsigmaerror);
	gMinuit->GetParameter(3,ymean,ymeanerror);
	gMinuit->GetParameter(4,ysigma,ysigmaerror);

/*	cout << endl;
	cout << "numLine " << numLines << endl; 
	cout << "max \t" << max <<  " +- " << maxerror << endl;
	cout << "xmean \t" << xmean << " +- " << xmeanerror << endl;
	cout << "xsigma \t" << TMath::Abs(xsigma) << " +- " << xsigmaerror << endl;
	cout << "ymean \t" << ymean << " +- " << ymeanerror << endl;
	cout << "ysigma \t" << TMath::Abs(ysigma) << " +- " << ysigmaerror << endl;
	cout << "chisquare/ndof \t" << chisquare/(nPixelsFitted-5) << endl;
*/
	
	chisqu[numLines]=chisquare/(nPixelsFitted-5);

        if(chisqu[numLines]<100.)
	{
	    ux[numLines]=xmean;
	    uy[numLines]=ymean;
	    sx[numLines]=TMath::Abs(xsigma);
	    sy[numLines]=TMath::Abs(ysigma);
	    time[numLines]=numLines;

	    display.SetCamContent(curr);
	    gPad->cd(1);
	    ellipse.SetX1(xmean/conv4mm2deg);
	    ellipse.SetY1(ymean/conv4mm2deg);
	    ellipse.SetR1(TMath::Abs(xsigma)/conv4mm2deg);
	    ellipse.SetR2(TMath::Abs(ysigma)/conv4mm2deg);

	    gPad->Modified();
	    gPad->Update();

	    // Remove the comments if you want to go through the file
	    //event-by-event:
	    if (userNumLines>0)
	    {
		if (numLines>userNumLines)
		    break;
	    }
	    else
	    {
		if (!HandleInput())
		    break;
	    }
	    numLines++;
	}
    }

    
    evtloop.PostProcess();

    //
    // Draw the ditributions of the sigmas the point spread function
    //

    cout<<"PSF>> Number of lines "<<numLines<<endl;

    gROOT->Reset();
    gStyle->SetCanvasColor(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetFrameBorderMode(0);
    gStyle->SetOptStat(00000000);

//
// Find in the file name the date, run and project name to put it in the title
//

    Size_t pos = fname.Last('/');
    TString iRun = TString(fname(pos+24,5));
    TString iYear = TString(fname(pos+4,4));
    TString iMonth = TString(fname(pos+9,2));
    TString iDay = TString(fname(pos+12,2));

    TString iHour = TString(fname(pos+15,2));
    TString iMin = TString(fname(pos+18,2));
    TString iSec = TString(fname(pos+21,2));

    Size_t poslast = fname.Last('.');
    Size_t posfirst = poslast-1;
    while (fname[posfirst] != '_')
	posfirst--;

    TString iSource = TString(fname(posfirst+1,poslast-posfirst-1));

    
    char str[100];
    
//    sprintf(str,"Date %s/%s/%s Run %s Source %s",iYear.Data(),iMonth.Data(),iDay.Data(),iRun.Data(),iSource.Data());
    sprintf(str,"Date %s/%s/%s   StartTime %s:%s:%s   Run %s   Source %s",iYear.Data(),iMonth.Data(),iDay.Data(),iHour.Data(),iMin.Data(),iSec.Data(),iRun.Data(),iSource.Data());

    c1 = new TCanvas("c1",str,0,0,1200,850);
//    c1 = new TCanvas("c1","Time evolution & distributions",0,0,1200,850);
    c1->Divide(3,2);

    c1->cd(1);

    TMath math;
   
    Double_t minmeanx, maxmeanx;
    minmeanx = ux[math.LocMin(numLines,ux)];
    maxmeanx = ux[math.LocMax(numLines,ux)];

    Double_t minmeany, maxmeany;
    minmeany = uy[math.LocMin(numLines,uy)];
    maxmeany = uy[math.LocMax(numLines,uy)];

    Double_t minmean, maxmean;
    minmean = math.Min(minmeanx,minmeany);
    maxmean = math.Max(maxmeanx,maxmeany);

    Double_t diff;
    diff = maxmean - minmean;
    diff = 0.1*diff;
    minmean = minmean - diff;
    maxmean = maxmean + diff;

    Double_t mintime, maxtime;
    mintime = time[math.LocMin(numLines,time)];
    maxtime = time[math.LocMax(numLines,time)];

    TH2D *h1 = new TH2D("h1","",1,mintime-1,maxtime+1,1,minmean,maxmean);
    h1->GetXaxis()->SetTitle("Event number");
    h1->GetYaxis()->SetTitle("mean position (deg)");
    h1->Draw();

    TGraph *grtimeevolmeanx = new TGraph(numLines,time,ux);
    grtimeevolmeanx->SetMarkerColor(3);
    grtimeevolmeanx->SetMarkerStyle(20);
    grtimeevolmeanx->SetMarkerSize (0.4);
    grtimeevolmeanx->Draw("P");

    TGraph *grtimeevolmeany = new TGraph(numLines,time,uy);
    grtimeevolmeany->SetMarkerColor(6);
    grtimeevolmeany->SetMarkerStyle(24);
    grtimeevolmeany->SetMarkerSize (0.4);
    grtimeevolmeany->Draw("P");

    legmeanxy = new TLegend(0.8,0.85,0.95,0.95);
    legmeanxy.SetTextSize(0.03);
    legmeanxy.AddEntry(grtimeevolmeanx,"mean x","P");
    legmeanxy.AddEntry(grtimeevolmeany,"mean y","P");
    legmeanxy.Draw();

    c1->cd(2);

    TMath math;
   
    Double_t minsigmax, maxsigmax;
    minsigmax = sx[math.LocMin(numLines,sx)];
    maxsigmax = sx[math.LocMax(numLines,sx)];

    Double_t minsigmay, maxsigmay;
    minsigmay = sy[math.LocMin(numLines,sy)];
    maxsigmay = sy[math.LocMax(numLines,sy)];

    Double_t minsigma, maxsigma;
    minsigma = math.Min(minsigmax,minsigmay);
    maxsigma = math.Max(maxsigmax,maxsigmay);

    diff = maxsigma - minsigma;
    diff = 0.1*diff;
    minsigma = minsigma - diff;
    maxsigma = maxsigma + diff;

    TH2D *h2 = new TH2D("h2","",1,mintime-1,maxtime+1,1,minsigma,maxsigma);
    h2->GetXaxis()->SetTitle("Event number");
    h2->GetYaxis()->SetTitle("PSF Rms (deg)");
    h2->Draw();

    TGraph* grtimeevolsigmax= new TGraph(numLines,time,sx);
    grtimeevolsigmax->SetMarkerColor(3);
    grtimeevolsigmax->SetMarkerStyle(20);
    grtimeevolsigmax->SetMarkerSize (0.4);
    grtimeevolsigmax->Draw("P");

    TGraph* grtimeevolsigmay= new TGraph(numLines,time,sy);
    grtimeevolsigmay->SetMarkerColor(6);
    grtimeevolsigmay->SetMarkerStyle(24);
    grtimeevolsigmay->SetMarkerSize (0.4);
    grtimeevolsigmay->Draw("P");

    legsigmaxy = new TLegend(0.8,0.85,0.95,0.95);
    legsigmaxy.SetTextSize(0.03);
    legsigmaxy.AddEntry(grtimeevolsigmax,"sigma x","P");
    legsigmaxy.AddEntry(grtimeevolsigmay,"sigma y","P");
    legsigmaxy.Draw();
   
    c1->cd(3);

    Double_t minchisqu, maxchisqu;

    minchisqu = chisqu[math.LocMin(numLines,chisqu)];
    maxchisqu = chisqu[math.LocMax(numLines,chisqu)];

    diff = maxchisqu - minchisqu;
    diff = 0.1*diff;
    minchisqu = minchisqu - diff;
    maxchisqu = maxchisqu + diff;

    TH2D *h3 = new TH2D("h3","",1,mintime-1,maxtime+1,1,minchisqu,maxchisqu);
    h3->GetXaxis()->SetTitle("Event number");
    h3->Draw();

    TGraph * grtimeevolchisqu = new TGraph(numLines,time,chisqu);
    grtimeevolchisqu->SetMarkerColor(215);
    grtimeevolchisqu->SetMarkerStyle(20);
    grtimeevolchisqu->SetMarkerSize(0.4);
    grtimeevolchisqu->Draw("P");

    legchisqu = new TLegend(0.55,0.90,0.95,0.95);
    legchisqu.SetTextSize(0.03);
    legchisqu.AddEntry(grtimeevolchisqu,"chi square / ndof","P");
    legchisqu.Draw();

//***************************************

    const Int_t nbins = 100; 
  
    TH1D *xsigmahist = new TH1D("xsigmahist","",nbins,minsigma,maxsigma);
    TH1D *ysigmahist = new TH1D("ysigmahist","",nbins,minsigma,maxsigma);
    TH1D *xmeanhist = new TH1D("xmeanhist","",nbins,minmean,maxmean);
    TH1D *ymeanhist = new TH1D("ymeanhist","",nbins,minmean,maxmean);
    TH1D *chisquhist = new TH1D("chisquhist","",nbins,minchisqu,maxchisqu);

    for (Int_t i=0; i<numLines; i++)
    {
	xsigmahist->Fill(TMath::Abs(sx[i]));
	ysigmahist->Fill(TMath::Abs(sy[i]));
	xmeanhist->Fill(ux[i]);
	ymeanhist->Fill(uy[i]);
	chisquhist->Fill(chisqu[i]);
	
    }

    c1->cd(5);

    TMath math;
    Double_t maxsigma;
    Int_t binmaxx, binmaxy;
    xsigmahist->SetLineColor(3);
    xsigmahist->SetLineWidth(2);
    xsigmahist->SetXTitle("RMS [deg]");
    binmaxx = xsigmahist->GetMaximumBin();
    binmaxx = xsigmahist->GetBinContent(binmaxx);
       
    ysigmahist->SetLineColor(6);
    ysigmahist->SetLineWidth(2);
    binmaxy = ysigmahist->GetMaximumBin();
    binmaxy = ysigmahist->GetBinContent(binmaxy);
   
    maxsigma = math.Max(binmaxx,binmaxy);
    maxsigma += 1;

    xsigmahist->SetMaximum(maxsigma);
    ysigmahist->SetMaximum(maxsigma);
    xsigmahist->DrawCopy();
    ysigmahist->DrawCopy("Same");

    TLegend *legendsigma = new TLegend(.3,.8,.95,.95);
    legendsigma->SetTextSize(0.03);
    char xsigmatitle[100];
    char ysigmatitle[100];
    sprintf(xsigmatitle,"PSF Rms on X axis -- %5.2f +/- %5.2f deg",xsigmahist->GetMean(),xsigmahist->GetRMS());
    sprintf(ysigmatitle,"PSF Rms on Y axis -- %5.2f +/- %5.2f deg",ysigmahist->GetMean(),ysigmahist->GetRMS());
    legendsigma->AddEntry(xsigmahist,xsigmatitle,"F");
    legendsigma->AddEntry(ysigmahist,ysigmatitle,"F");
    legendsigma->Draw();

    c1->cd(4);
   
    Double_t maxmean;

    xmeanhist->SetLineColor(3);
    xmeanhist->SetLineWidth(2);
    xmeanhist->SetXTitle("mean [deg]");
    binmaxx = xmeanhist->GetMaximumBin();
    binmaxx = xmeanhist->GetBinContent(binmaxx);

    ymeanhist->SetLineColor(6);
    ymeanhist->SetLineWidth(2);
    binmaxy = ymeanhist->GetMaximumBin();
    binmaxy = ymeanhist->GetBinContent(binmaxy);

    maxmean = math.Max(binmaxx,binmaxy);
    maxmean += 1;

    xmeanhist->SetMaximum(maxmean);
    ymeanhist->SetMaximum(maxmean);
    xmeanhist->DrawCopy();
    ymeanhist->DrawCopy("Same");

    TLegend *legendmean = new TLegend(.35,.8,.95,.95);
    legendmean->SetTextSize(0.03);
    char xmeantitle[100];
    char ymeantitle[100];
    sprintf(xmeantitle,"mean on X axis -- %5.2f +/- %5.2f deg",xmeanhist->GetMean(),xmeanhist->GetRMS());
    sprintf(ymeantitle,"mean on Y axis -- %5.2f +/- %5.2f deg",ymeanhist->GetMean(),ymeanhist->GetRMS());
    legendmean->AddEntry(xmeanhist,xmeantitle,"F");
    legendmean->AddEntry(ymeanhist,ymeantitle,"F");
    legendmean->Draw();

    //meancanvas->Modified();
    //meancanvas->Update();

    c1->cd(6);

    chisquhist->SetLineColor(215);
    chisquhist->SetLineWidth(2);
    chisquhist->SetXTitle("chi square / ndof");
    TAxis * axis = chisquhist->GetXaxis();
    axis->SetLabelSize(0.025);
    chisquhist->DrawCopy();
    
    TLegend *legendchisqu = new TLegend(.4,.85,.95,.95);
    legendchisqu->SetTextSize(0.03);
    char chisqutitle[100];
    sprintf(chisqutitle,"chi square / ndof -- %5.2f +/- %5.2f ",chisquhist->GetMean(),chisquhist->GetRMS());
    legendchisqu->AddEntry(chisquhist,chisqutitle,"F");
    legendchisqu->Draw();
    
    
    return;
      
}















































































