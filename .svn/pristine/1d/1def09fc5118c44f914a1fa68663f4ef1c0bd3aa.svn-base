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
!   Author(s): Thomas Bretz, 3/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniel Mazin, 8/2004 <mailto:mazin@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MSkyPlot
//
// Create a false source plot. For the Binning in x,y the object MBinning
// "BinningFalseSource" is taken from the paremeter list.
//
// The binning in alpha is currently fixed as 18bins from 0 to 90deg.
//
// If MTime, MObservatory and MPointingPos is available in the paremeter
// list each image is derotated.
//
// MSkyPlot fills a 3D histogram with alpha distribution for
// each (derotated) x and y position.
//
// Only a radius of 1.2deg around the camera center is taken into account.
//
// The displayed histogram is the projection of alpha<fAlphaCut into
// the x,y plain and the projection of alpha>90-fAlphaCut
//
// By using the context menu (find it in a small region between the single
// pads) you can change the AlphaCut 'online'
//
// Each Z-Projection (Alpha-histogram) is scaled such, that the number
// of entries fits the maximum number of entries in all Z-Projections.
// This should correct for the different acceptance in the center
// and at the border of the camera. This, however, produces more noise
// at the border.
//
// Here is a slightly simplified version of the algorithm:
// ------------------------------------------------------
//    MHillas hil; // Taken as second argument in MFillH
//
//    MSrcPosCam src;
//    MHillasSrc hsrc;
//    hsrc.SetSrcPos(&src);
//
//    for (int ix=0; ix<nx; ix++)
//        for (int iy=0; iy<ny; iy++)
//        {
//            TVector2 v(cx[ix], cy[iy]); //cx center of x-bin ix
//            if (rho!=0)                 //cy center of y-bin iy
//                v=v.Rotate(rho);         //image rotation angle
//
//            src.SetXY(v);               //source position in the camera
//
//            if (!hsrc.Calc(hil))        //calc source dependant hillas
//                return;
//
//            //fill absolute alpha into histogram
//            const Double_t alpha = hsrc.GetAlpha();
//            fHist.Fill(cx[ix], cy[iy], TMath::Abs(alpha), w);
//        }
//    }
//
// Use MHFalse Source like this:
// -----------------------------
//    MFillH fill("MSkyPlot", "MHillas");
// or
//    MSkyPlot hist;
//    hist.SetAlphaCut(12.5);  // The default value
//    hist.SetBgmean(55);      // The default value
//    MFillH fill(&hist, "MHillas");
//
// To be implemented:
// ------------------
//  - a switch to use alpha or |alpha|
//  - taking the binning for alpha from the parlist (binning is
//    currently fixed)
//  - a possibility to change the fit-function (eg using a different TF1)
//  - a possibility to change the fit algorithm (eg which paremeters
//    are fixed at which time)
//  - use a different varaible than alpha
//  - a possiblity to change the algorithm which is used to calculate
//    alpha (or another parameter) is missing (this could be done using
//    a tasklist somewhere...)
//  - a more clever (and faster) algorithm to fill the histogram, eg by
//    calculating alpha once and fill the two coils around the mean
//  - more drawing options...
//  - Move Significance() to a more 'general' place and implement
//    also different algorithms like (Li/Ma)
//  - implement fit for best alpha distribution -- online (Paint)
//  - currently a constant pointing position is assumed in Fill()
//  - the center of rotation need not to be the camera center
//  - ERRORS on each alpha bin to estimate the chi^2 correctly!
//    (sqrt(N)/binwidth) needed for WOlfgangs proposed caluclation
//    of alpha(Li/Ma)
//
//////////////////////////////////////////////////////////////////////////////
#include "MSkyPlot.h"

#include <TF1.h>
#include <TH2.h>
#include <TH1.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TStopwatch.h>
#include <TFile.h>

#include "MGeomCam.h"
#include "MSrcPosCam.h"
#include "MReportDrive.h"
#include "MHillasSrc.h"
#include "MHillas.h"
#include "MHillasExt.h"
#include "MNewImagePar.h"
#include "MHadronness.h"
#include "MTime.h"
#include "MObservatory.h"
#include "MPointingPos.h"
#include "MAstroCatalog.h"
#include "MAstroSky2Local.h"
#include "MStarCamTrans.h"
#include "MStatusDisplay.h"
#include "MMath.h"
#include "MSupercuts.h"

#include "MBinning.h"
#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include <TOrdCollection.h>

ClassImp(MSkyPlot);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor
//
MSkyPlot::MSkyPlot(const char *name, const char *title)
  : fGeomCam(NULL),   
    fTime(NULL),      
    fPointPos(NULL),  
    fRepDrive(NULL),  
    fSrcPosCam(NULL), 
    fPntPosCam(NULL), 
    fObservatory(NULL),
    fHillas(NULL),    
    fHillasExt(NULL), 
    fHillasSrc(NULL), 
    fNewImagePar(NULL),
    fMm2Deg(-1)
{

  *fLog << warn << "entering default constructor in MSkyPlot" << endl; 
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MSkyPlot";
    fTitle = title ? title : "sky plot vs x, y";

    fSetCenter=kTRUE;

    fHistSignif.SetDirectory(NULL);
    fHistNexcess.SetDirectory(NULL);
    fHistOn.SetDirectory(NULL);
    fHistSignifGaus.SetDirectory(NULL);

    fHistSignif.UseCurrentStyle();
    fHistNexcess.UseCurrentStyle();
    fHistOn.UseCurrentStyle();
    fHistSignifGaus.UseCurrentStyle();

    fHistSignif.SetName("SkyPlotSignif");
    fHistSignif.SetTitle("Sky Plot of significance vs x, y");
    fHistSignif.SetXTitle("x [\\circ]");
    fHistSignif.SetYTitle("y [\\circ]");

    fHistNexcess.SetName("SkyPlotNexcess");
    fHistNexcess.SetTitle("Sky Plot of number of excess vs x, y");
    fHistNexcess.SetXTitle("x [\\circ]");
    fHistNexcess.SetYTitle("y [\\circ]");

    fHistOn.SetName("SkyPlotOn");
    fHistOn.SetTitle("Sky Plot of number of On events vs x, y");
    fHistOn.SetXTitle("x [\\circ]");
    fHistOn.SetYTitle("y [\\circ]");

    fHistSignifGaus.SetName("SignifDistrib");
    fHistSignifGaus.SetTitle("Distribution of the significances from the sky plot");
    fHistSignifGaus.SetXTitle("significance");
    fHistSignifGaus.SetYTitle("counts");

    // set some values for the sky plot geometry:
    fMinXGrid    =-1.;  		// [deg] 
    fMaxXGrid    = 1.;             //  [deg] , right edge of the skyplot
    fMinYGrid    =-1.;            //  [deg] , upper edge of the skyplot
    fMaxYGrid    = 1.;             //  [deg] , lower edge of the skyplot
    fBinStepGrid = 0.05;         //  [deg], 
    fAlphaONMax  = 5.;           //  [deg] , upper cut for alpha ON region in the alpha plot
                      // [deg], ON region in the alpha plot, maybe 5 deg is better
                      // NOTE: up to now only values of 5, 10, 15, 20 degrees are possible
// ra,dec lines from wolfgang:
    fGridBinning = 0.50;   // degrees
    fGridFineBin = 0.01;   // degrees

// elapsed time:
    fElaTime = 0.;

    // some filter cuts:
    fSizeMin    = 2000.;             // min size in photons
    fSizeMax    = 100000.;             // max size in photons
    fLeakMax    = 0.25;             // leakmax  in per cent
    fMaxDist = 1.4;          // dist max cut (ever possible)
    fMinDist = 0.1;          // dist max cut (ever possible)
    fHadrCut = 0.2;          // hadronness cut

    fNumBinsAlpha   = 36; 	 // number of bins for alpha histograms
    fAlphaLeftEdge  = 0.;        // [deg] left edge
    fAlphaRightEdge = 90.;       // [deg] left edge

    fAlphaBgLow     = 30.;
    fAlphaBgUp      = 90.;

    {
// SET DEFAULT VALUES HERE	
    fLengthUp[0] =  0.2;
    fLengthUp[1] =  0.0;
    fLengthUp[2] =  0.0;
    fLengthUp[3] =  0.0;
    fLengthUp[4] =  0.0;
    fLengthUp[5] =  0.0;
    fLengthUp[6] =  0.0;
    fLengthUp[7] =  0.0;

    fLengthLo[0] =  0.;
    fLengthLo[1] =  0.;
    fLengthLo[2] =  0.;
    fLengthLo[3] =  0.;
    fLengthLo[4] =  0.;
    fLengthLo[5] =  0.;
    fLengthLo[6] =  0.;
    fLengthLo[7] =  0.;

    fWidthUp[0] =  0.1;
    fWidthUp[1] =  0.0;
    fWidthUp[2] =  0.0;
    fWidthUp[3] =  0.0;
    fWidthUp[4] =  0.0;
    fWidthUp[5] =  0.0;
    fWidthUp[6] =  0.0;
    fWidthUp[7] =  0.0;

    fWidthLo[0] =  0.;
    fWidthLo[1] =  0.;
    fWidthLo[2] =  0.;
    fWidthLo[3] =  0.;
    fWidthLo[4] =  0.;
    fWidthLo[5] =  0.;
    fWidthLo[6] =  0.;
    fWidthLo[7] =  0.;

    fDistUp[0] =  1.e10;
    fDistUp[1] =  0.0;
    fDistUp[2] =  0.0;
    fDistUp[3] =  0.0;
    fDistUp[4] =  0.0;
    fDistUp[5] =  0.0;
    fDistUp[6] =  0.0;
    fDistUp[7] =  0.0;

    fDistLo[0] =  0.0;
    fDistLo[1] =  0.0;
    fDistLo[2] =  0.0;
    fDistLo[3] =  0.0;
    fDistLo[4] =  0.0;
    fDistLo[5] =  0.0;
    fDistLo[6] =  0.0;
    fDistLo[7] =  0.0;
    }

    fSaveAlphaPlots=kTRUE;
    fSaveSkyPlots=kTRUE;
    fSaveNexPlot=kTRUE;
    fUseRF=kFALSE;
    fAlphaHName = "alphaplot.root";
    fSkyHName   = "skyplot.root";
    fNexHName   = "Nexcess.gif";

    fHistAlpha = new TOrdCollection();
    fHistAlpha->SetOwner();    

}

MSkyPlot::~MSkyPlot()
{

  if (fHistAlpha)
    delete fHistAlpha;
}

// --------------------------------------------------------------------------
//
// Get i-th hist 
//
TH1D *MSkyPlot::GetAlphaPlot(Int_t i)
{
  if (GetSize() == 0)
    return NULL;

  return static_cast<TH1D*>(i==-1 ? fHistAlpha->At(GetSize()/2+1) : fHistAlpha->At(i));
}

// --------------------------------------------------------------------------
//
// Get i-th hist 
//
const TH1D *MSkyPlot::GetAlphaPlot(Int_t i) const 
{
  if (GetSize() == 0)
    return NULL;

  return static_cast<TH1D*>(i==-1 ? fHistAlpha->At(GetSize()/2+1) : fHistAlpha->At(i));
}


void MSkyPlot::ReadCuts(const TString parSCinit="OptSCParametersONOFFThetaRange0_1570mRad.root")
{

cout << " parameters read from file: " << parSCinit << endl;
    //--------------------------------
    // create container for the supercut parameters
    // and set them to their initial values
    MSupercuts super;

  // read the cuts coefficients
    TFile inparam(parSCinit);
    super.Read("MSupercuts");
    inparam.Close();
    *fLog << "MFindSupercutsONOFF::FindParams; initial values of parameters are taken from file "
          << parSCinit << endl;

    TArrayD params = super.GetParameters();
    TArrayD steps  = super.GetStepsizes();
 // TMP2
    if (params.GetSize() == steps.GetSize())
    {
            *fLog << "SC parameters and Setps are: " << endl;
            for (Int_t z = 0; z < params.GetSize(); z++)
            {
                cout << params[z] << setw(20) << steps[z] << endl;
            }
    }
 // ENDTMP2
    for (Int_t i=0; i<8; i++)
    {
	fLengthUp[i]  = params[i];
        fLengthLo[i] = params[i+8];
	fWidthUp[i]   = params[i+16];
	fWidthLo[i]  = params[i+24];
        fDistUp[i]    = params[i+32];
        fDistLo[i]   = params[i+40];
    }
}

void MSkyPlot::SetSkyPlot(Float_t xmin, Float_t xmax, Float_t ymin, Float_t ymax, Float_t step)
{
    Float_t temp;

    if (xmax<xmin)
    {
        *fLog << warn << "SetSkyPlot: xmax is smaller xmin ... exchanging them." << endl;
	temp = xmax;
	xmax = xmin;
	xmin = temp;
    }	

    if (ymax<ymin)
    {
        *fLog << warn << "SetSkyPlot: ymax is smaller ymin ... exchanging them." << endl;
	temp = ymax;
	ymax = ymin;
	ymin = temp;
    }	
    
    if (step<0)
        *fLog << warn << "SetSkyPlot: step<0... taking absolute value." << endl;

    fBinStepGrid = TMath::Abs(step);
    fMinXGrid    = xmin;
    fMaxXGrid    = xmax;
    fMinYGrid    = ymin;
    fMaxYGrid    = ymax;
    
    *fLog << endl <<  inf << " SetSkyPlot: fMinXGrid, fMaxXGrid, fMinYGrid, fMaxYGrid, fBinStepGrid: " << endl;
    *fLog << inf << "           " << fMinXGrid << ", " << fMaxXGrid << ", " << fMinYGrid << ", " 
                 << fMaxYGrid<< ", " << fBinStepGrid << endl;
}

// --------------------------------------------------------------------------
//
// Set the alpha cut (|alpha|<fAlphaCut) which is use to estimate the
// number of excess events
//
void MSkyPlot::SetAlphaCut(Float_t alpha)
{
    if (alpha<0)
        *fLog << warn << "Alpha<0... taking absolute value." << endl;

    fAlphaONMax = TMath::Abs(alpha);
}

// --------------------------------------------------------------------------
//
// Set the upper and lower limit for the background region in the alpha plot
// to estimate the number of background events
//
void MSkyPlot::SetAlphaBGLimits(Float_t alphalow, Float_t alphaup)
{
    Float_t alphatemp;
    if (alphalow<0)
        *fLog << warn << "Alpha<0... taking absolute value." << endl;

    if (alphaup<0)
        *fLog << warn << "Alpha<0... taking absolute value." << endl;

    if (TMath::Abs(alphaup)<TMath::Abs(alphalow)) {
        *fLog << warn << "AlphaLow > AlphaUp... exchanging limits." << endl;
	alphatemp = alphaup;
	alphaup = alphalow;
	alphalow = alphatemp;
    }

    fAlphaBgLow = TMath::Abs(alphalow);
    fAlphaBgUp  = TMath::Abs(alphaup);
}

// calculate the values for the cuts:
Double_t MSkyPlot::CalcLimit(Double_t *a, Double_t ls, Double_t ls2, Double_t dd2)
{

    Double_t  limit = a[0] + a[1] * dd2 +
                      ls  * (a[3] + a[4] * dd2) +
                      ls2 * (a[6] + a[7] * dd2);

     return limit;
}

// --------------------------------------------------------------------------
//
// Set binnings (takes BinningFalseSource) and prepare filling of the
// histogram.
//
// Also search for MTime, MObservatory and MPointingPos
// 
Int_t MSkyPlot::PreProcess(MParList *plist)
{

  *fLog << warn << "entering PreProcess in MSkyPlot::PreProcess" << endl; 
  
    fGeomCam = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fGeomCam)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    fMm2Deg = fGeomCam->GetConvMm2Deg();

    fRepDrive = (MReportDrive*)plist->FindObject(AddSerialNumber("MReportDrive"));
    if (!fRepDrive)
        *fLog << warn << "MReportDrive not found... could be problem for sky map." << endl;


    fSrcPosCam = (MSrcPosCam*)plist->FindCreateObj(AddSerialNumber("MSrcPosCam"));
    if (!fSrcPosCam)
        *fLog << warn << "MSrcPosCam not found... no sky map." << endl;

/*
    fPntPosCam = (MSrcPosCam*)plist->FindObject(AddSerialNumber("MPntPosCam"));
    if (!fPntPosCam)
        *fLog << warn << "MPntPosCam not found... no sky map." << endl;
*/

    fPointPos = (MPointingPos*)plist->FindObject(AddSerialNumber("MPointingPos"));
    if (!fPointPos)
        *fLog << warn << "MPointingPos not found... no sky map." << endl;

    fTime = (MTime*)plist->FindObject(AddSerialNumber("MTime"));
    if (!fTime)
        *fLog << warn << "MTime not found... could be problem for sky map." << endl;

    fObservatory = (MObservatory*)plist->FindObject(AddSerialNumber("MObservatory"));
    if (!fObservatory)
        *fLog << warn << "MObservatory not found... no sky map." << endl;


    fHillas = (MHillas*)plist->FindObject(AddSerialNumber("MHillas"));
    if (!fHillas)
        *fLog << err << "MHillas not found... no sky map." << endl;

    fHillasExt = (MHillasExt*)plist->FindObject(AddSerialNumber("MHillasExt"));
    if (!fHillasExt)
        *fLog << err << "MHillasExt not found... no sky map." << endl;

    fHillasSrc = (MHillasSrc*)plist->FindObject(AddSerialNumber("MHillasSrc"));
    if (!fHillasSrc)
        *fLog << err << "MHillasSrc not found... no sky map." << endl;

    fNewImagePar = (MNewImagePar*)plist->FindObject(AddSerialNumber("MNewImagePar"));
    if (!fNewImagePar)
        *fLog << err << "MNewImagePar not found... no sky map." << endl;

    if(fUseRF)
    {
       fHadron = (MHadronness*)plist->FindObject(AddSerialNumber("MHadronness"));
       if (!fHadron)
           *fLog << err << "MHadronness not found although specified... no sky map." << endl;

       *fLog << inf << "Hadronness cut set to : " << fHadrCut << endl;
    }

    // FIXME: Because the pointing position could change we must check
    // for the current pointing position and add a offset in the
    // Fill function!

   // prepare skyplot
    fNumStepsX     =  (int) ((fMaxXGrid - fMinXGrid) / fBinStepGrid + 1.5);
    fNumStepsY     =  (int) ((fMaxYGrid - fMinYGrid) / fBinStepGrid + 1.5);
    fNumalphahist  =  (int) (fNumStepsX * fNumStepsY  + 0.5);

    *fLog << inf << "SetSkyPlot: fNumStepsX, fNumStepsY, fNumalphahist: "
                 << fNumStepsX << ", " << fNumStepsY << ", " << fNumalphahist << endl;

   // fHistSignif.SetName("SPSignif2ndOrder");
   // fHistSignif.SetTitle("Sky Plot of significance (2nd order fit)");
    fHistSignif.SetBins(fNumStepsX, fMinXGrid-0.5*fBinStepGrid, fMaxXGrid+0.5*fBinStepGrid, 
			fNumStepsY, fMinYGrid-0.5*fBinStepGrid, fMaxYGrid+0.5*fBinStepGrid);
    fHistSignif.SetFillStyle(4000);

   // fHistNexcess.SetName("SPNex2ndOrder");
   // fHistNexcess.SetTitle("Sky Plot of Number of excess events (2nd order fit)");
    fHistNexcess.SetBins(fNumStepsX, fMinXGrid-0.5*fBinStepGrid, fMaxXGrid+0.5*fBinStepGrid, 
			fNumStepsY, fMinYGrid-0.5*fBinStepGrid, fMaxYGrid+0.5*fBinStepGrid);
    fHistNexcess.SetFillStyle(4000);

   // fHistOn.SetName("SPOnCounted");
   // fHistOn.SetTitle("Sky Plot of ON events (counted)");
    fHistOn.SetBins(fNumStepsX, fMinXGrid-0.5*fBinStepGrid, fMaxXGrid+0.5*fBinStepGrid, 
			fNumStepsY, fMinYGrid-0.5*fBinStepGrid, fMaxYGrid+0.5*fBinStepGrid);
    fHistOn.SetFillStyle(4000);

    //fHistSignifGaus.SetName("SignifDistrib");
    fHistSignifGaus.SetTitle("Distribution of the significances from the sky plot");
    fHistSignifGaus.SetBins(100, -10., 10.); 

    // create alpha histograms
    for (Int_t i=0; i< fNumalphahist; i++)
      {
    	// temp histogram for an alpha plot
	TH1D *temp = new TH1D("alphaplot","alphaplot",fNumBinsAlpha,fAlphaLeftEdge,fAlphaRightEdge);
	temp->SetDirectory(NULL);
	fHistAlpha->AddAt(temp,i);
      }

    fHistAlphaBinWidth = GetAlphaPlot()->GetBinWidth(1);
//cout  << " (*fHistAlpha)[10].GetBinContent(5) " << (*fHistAlpha)[10].GetBinContent(5) << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histogram. For details see the code or the class description
// 
Int_t MSkyPlot::Process()
{

//      *fLog << err << "MPointingPos ENTERING the process" << endl;
//      *fLog << err << "MPointingPos:: fUseRF " << (int)fUseRF << endl;
  // check whether MPointingPos comtains something:
  if (TMath::Abs(fPointPos->GetRa())<1e-3 && TMath::Abs(fPointPos->GetDec())<1e-3) 
    {
      *fLog << warn << "MPointingPos is not filled ... event skipped" << endl;
      return kTRUE;
    }
  
  // Get RA_0, DEC_0 for the camera center (Tracking MDrive?, can be set from outside)
  if (fSetCenter==kTRUE)
    {
      if (fRepDrive)
	{
	  fRa0  = fRepDrive->GetRa();  // [h]
	  fDec0 = fRepDrive->GetDec(); // [deg]
	  if (fRa0 < 0. || fRa0 > 24. || fDec0 < -90. || fDec0 > 90. || (fRa0==0 && fDec0==0)) return kTRUE;  // temp!, should be changed
	}
      else
	{
	  fRa0 = fPointPos->GetRa(); 
	  fDec0 = fPointPos->GetDec();
	} 
      *fLog << inf << "Ra (center) = " << fRa0 << ", Dec = " << fDec0 << endl;
      fSetCenter=kFALSE;
    }

  // some filter cuts:
  if ( fHillas->GetSize() > fSizeMin && fHillas->GetSize() < fSizeMax && fNewImagePar->GetLeakage1() < fLeakMax)
    {
      
      Double_t xsource, ysource, cosgam, singam, x_0, y_0, xsourcam, ysourcam;
      Double_t dx, dy, beta, tanbeta, alphapar, distpar;
      Double_t logsize, lgsize, lgsize2, dist2, hadr; 
      const Double_t log3000n = log(3000.*0.18);  // now in phe, thus must shift the offset
      const Float_t fDistOffset = 0.9;
      
      //Get Hadronness if available:
      if(fUseRF) 
	{
		hadr=fHadron->GetHadronness();
//		cout << " will use RF " << hadr << endl;
	}
      // Get camera rotation angle
      Double_t rho = 0;
      if (fTime && fObservatory && fPointPos)
	{
	  rho = fPointPos->RotationAngle(*fObservatory, *fTime);
	  //*fLog << inf << " rho = " << rho*180./TMath::Pi() << ", Zd = " << fPointPos->GetZd() << 
	  //                ", Az = " << fPointPos->GetAz() << ", Ra = " << fPointPos->GetRa() << ", Dec = " << fPointPos->GetDec() << endl;
	  
	  // => coord. system: xtilde, ytilde with center in RA_0, DEC_0 
	  
	  // Get Rot. Angle:
	  cosgam = TMath::Cos(rho);
	  singam = TMath::Sin(rho);
	  // Get x_0, y_0 for RA_0,DEC_0 of the current event
	}
      else 
	{
	  //	rho = fPointPos->RotationAngle(*fObservatory);
	  Double_t theta, phi, sing, cosg;
	  theta = fPointPos->GetZd()*TMath::Pi()/180.;
	  phi = fPointPos->GetAz()*TMath::Pi()/180.;
	  printf("theta: %5.3f, phi: %5.3f\n", theta*180./4.1415, phi*180./4.1415);
	  fObservatory->RotationAngle(theta, phi, sing, cosg);        
	  cosgam = cosg;
	  singam = sing;
	}
      // if (fPointPos)
      //    rho = fPointPos->RotationAngle(*fObservatory);
      
      /*
	//TEMP
	//        theta = mcevt->GetTelescopeTheta();
	Double_t theta, phi, sing, cosg;
	theta = fPointPos->GetZd()*TMath::Pi()/180.;
	phi = fPointPos->GetAz()*TMath::Pi()/180.;
	//   printf("theta: %5.3f, phi: %5.3f\n", theta*180./4.1415, phi*180./4.1415);
	fObservatory->RotationAngle(theta, phi, sing, cosg);
	
	// conclusion: diffference in rho = 7 deg
	//  *fLog << "new thetaTel, phiTel, cosal, sinal, rho = " << theta << ",  "
	//        << phi << ",  " << cosg << ",  " << sing << ", " << TMath::ATan2(sing,cosg)*180./TMath::Pi() << endl;
	
	Double_t a1 =  0.876627;
	Double_t a3 = -0.481171;
	Double_t thetaTel=theta, phiTel=phi;
	
	Double_t denom =  1./ sqrt( sin(thetaTel)*sin(phiTel) * sin(thetaTel)*sin(phiTel) +
	( a1*cos(thetaTel)+a3*sin(thetaTel)*cos(phiTel) ) *
	( a1*cos(thetaTel)+a3*sin(thetaTel)*cos(phiTel) )   );
	Double_t cosal = - (a3 * sin(thetaTel) + a1 * cos(thetaTel) * cos(phiTel)) * denom;
	Double_t sinal =    a1 * sin(phiTel) * denom;
	
	//  *fLog << "old thetaTel, phiTel, cosal, sinal, rho = " << thetaTel << ",  "
	//        << phiTel << ",  " << cosal << ",  " << sinal << ", " << TMath::ATan2(sinal,cosal)*180./TMath::Pi() << endl;
	
	// END TEMP
      */
	    
      // make the center of the plot different from the center of the camera
      /*
	x_0 = fPntPosCam->GetX()*fMm2Deg; 
	y_0 = fPntPosCam->GetY()*fMm2Deg; 
      */      
      x_0 = 0.;  
      y_0 = 0.;
      
      Int_t index = 0;  // index for alpha histograms
      // loop over xtilde
      for (Int_t gridy = 0; gridy < fNumStepsY; gridy++)   
	{
	  ysource = fMinYGrid + gridy * fBinStepGrid;
	  // loop over ytilde
	  for (Int_t gridx = 0; gridx < fNumStepsX; gridx++)
	    {
	      
	      xsource = fMinXGrid + gridx * fBinStepGrid;
	      
	      /*     derotation    : rotate  into camera coordinates */
	      /*     formel: (x_cam)      (cos(gam)  -sin(gam))   (xtilde)   (x_0)
		     (     )  = - (                   ) * (      ) + (   ) 
	        (y_cam)      (sin(gam)   cos(gam))   (ytilde)   (y_0)
	      */
	      xsourcam = - (cosgam * xsource - singam * ysource) + x_0;
	      ysourcam = - (singam * xsource + cosgam * ysource) + y_0;
	      
	      
	      /*    end derotatiom    */
	      //           xsourcam = xsource;
	      //           ysourcam = ysource;

	      /* calculate ALPHA und DIST according to the source position */
	      dx = fHillas->GetMeanX()*fMm2Deg - xsourcam;
	      dy = fHillas->GetMeanY()*fMm2Deg - ysourcam;
	      tanbeta = dy / dx ;
	      beta = TMath::ATan(tanbeta);
	      alphapar = (fHillas->GetDelta() - beta) * 180./ TMath::Pi();
	      distpar = sqrt( dy*dy + dx*dx );
	      if(alphapar >  90.) alphapar -= 180.;
	      if(alphapar < -90.) alphapar += 180.;
	      alphapar = TMath::Abs(alphapar);
	      
	      if(fUseRF)
	      {
		
//		cout << " will use RF " << hadr << endl;
		if(hadr<fHadrCut  &&  distpar < fMaxDist && distpar > fMinDist)
		{
		    TH1D *hist = GetAlphaPlot(index);
                    hist->Fill(alphapar);
		}
	      }
	      else
	      {
	      	// apply cuts
	      	logsize = log(fHillas->GetSize());
	      	lgsize = logsize-log3000n;
	      	lgsize2 = lgsize*lgsize;
	      	dist2 = distpar*distpar - fDistOffset*fDistOffset;
	      
	      	if ( (fHillas->GetLength()*fMm2Deg) < CalcLimit(fLengthUp, lgsize, lgsize2, dist2) &&
		   (fHillas->GetLength()*fMm2Deg) > CalcLimit(fLengthLo, lgsize, lgsize2, dist2))
			if ( (fHillas->GetWidth()*fMm2Deg) < CalcLimit(fWidthUp, lgsize, lgsize2, dist2) &&
		     	(fHillas->GetWidth()*fMm2Deg) > CalcLimit(fWidthLo, lgsize, lgsize2, dist2))
			  if ( distpar < CalcLimit(fDistUp, lgsize, lgsize2, dist2) &&
		      	  distpar > CalcLimit(fDistLo, lgsize, lgsize2, dist2) &&
		      	  distpar < fMaxDist && distpar > fMinDist)
		    	{
		      // gamma candidates!
		      //*fLog <<  "Length : " << fHillas->GetLength()*fMm2Deg << ", Width : " << fHillas->GetWidth()*fMm2Deg << endl;
		      //*fLog <<  "distpar: " << distpar << ", Size : " << fHillas->GetSize() << endl;
		      	TH1D *hist = GetAlphaPlot(index);
		      	hist->Fill(alphapar);
		    	}
		}
	      	index++;
	    }
	}
    }
    return kTRUE;
}

// calculate number of events below alphacut, number of excess events, significance

Int_t MSkyPlot::PostProcess()
{

    Int_t nrow, ncolumn;
    Double_t Non, chisquarefit, numdegfreed, Noff_fit, Nex, Sign; 
    const Int_t onbinsalpha = TMath::Nint(fAlphaONMax/fHistAlphaBinWidth);


// fit function for the background
    TF1 * fitbgpar = new TF1("fbgpar", "[0]*x*x + [1]", fAlphaBgLow, fAlphaBgUp);
    fitbgpar->SetLineColor(2);

// number degrees of freedom:
    numdegfreed = (fAlphaBgUp - fAlphaBgLow) / fHistAlphaBinWidth - 2.; 

    int index2 = 0;  // index of the TH2F histograms

    TOrdCollectionIter Next(fHistAlpha);
    TH1D *alpha_iterator = NULL;

    fHistNexcess.Reset();
    fHistOn.Reset();
    fHistSignif.Reset();   
    fHistSignifGaus.Reset();
    
    while ( (alpha_iterator = (TH1D*)Next())) {
	 // find the bin in the 2dim histogram
         nrow    = index2/fHistOn.GetNbinsX() + 1;              // row of the histogram (y)
         ncolumn = index2%fHistOn.GetNbinsX()+1;   		// column of the histogram (x)
         //ncolumn = TMath::Nint(fmod(index2,fHistOn.GetNbinsX()))+1;   // column of the histogram (x)

         // number of ON events below fAlphaONMax
         Non = 0.;
         for(Int_t i=1; i<=onbinsalpha;i++) Non += (*alpha_iterator).GetBinContent(i);

         fHistOn.SetBinContent(ncolumn, nrow, Non);		// fill in the fHistOn

         // fit parabola to a background region
         alpha_iterator->Fit(fitbgpar,"EQRN");  // NWR OK?????????????????????????
         // get Chi2
         chisquarefit = fitbgpar->GetChisquare();
         if (chisquarefit/numdegfreed<2. && chisquarefit/numdegfreed>0.01);
         else  *fLog << warn << "Fit is bad, chi2/ndf = " << chisquarefit/numdegfreed << endl;
	 
         // estimate Noff from the fit:
       	 Noff_fit = (1./3. * (fitbgpar->GetParameter(0)) * TMath::Power(fAlphaONMax,3.) +
		     (fitbgpar->GetParameter(1)) * fAlphaONMax) /  fHistAlphaBinWidth;
	 
	 Nex = Non - Noff_fit;

         fHistNexcess.SetBinContent(ncolumn, nrow, Nex);	// fill in the fHistNexcess
	 
         if (Noff_fit<0.) Sign = 0.;
	 //         else Sign = LiMa17(Non,Noff_fit,1.);
         else Sign = MMath::SignificanceLiMaSigned(Non, Noff_fit, 1.);
	 
         fHistSignif.SetBinContent(ncolumn, nrow, Sign);	// fill in the fHistSignif
         fHistSignifGaus.Fill(Sign);
	 
         index2++;
    }
    
    // fit with gaus 
    fHistSignifGaus.Fit("gaus","N");
    
    
    //temp
    /*
      Double_t decl, hang;
      MStarCamTrans mstarc(*fGeomCam, *fObservatory);
      mstarc.LocToCel(fRepDrive->GetNominalZd(),fRepDrive->GetNominalAz(),decl, hang);
      
      *fLog << warn << "MDrive, RA, DEC = " << fRepDrive->GetRa() << ", " << fRepDrive->GetDec() << endl;
      *fLog << warn << "MStarCamTrans, H angle , DEC = " << hang << ", " << decl << endl;
      */
    //endtemp
    
    
    // save alpha plots:
    //  TString stri1 = "alphaplots.root";
    if(fSaveAlphaPlots==kTRUE) SaveAlphaPlots(fAlphaHName);
    
    // save sky plots:
    //  TString stri2 = "skyplots.root";
    if(fSaveSkyPlots==kTRUE) SaveSkyPlots(fSkyHName);
    
    // save nex plot:
    if(fSaveNexPlot==kTRUE) SaveNexPlot(fNexHName);
    
    fHistAlpha->Clear();

    delete fitbgpar;
    
    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Get the MAstroCatalog corresponding to fRa, fDec. The limiting magnitude
// is set to 9, while the fov is equal to the current fov of the false
// source plot.
//
TObject *MSkyPlot::GetCatalog()
{
    const Double_t maxr = 0.98*TMath::Abs(fHistSignif.GetBinCenter(1));

    // Create catalog...
    MAstroCatalog stars;
    stars.SetLimMag(9);
    stars.SetGuiActive(kFALSE);
    stars.SetRadiusFOV(maxr);
    stars.SetRaDec(fRa*TMath::DegToRad()*15, fDec*TMath::DegToRad());
    stars.ReadBSC("bsc5.dat");

    TObject *o = (MAstroCatalog*)stars.Clone();
    o->SetBit(kCanDelete);

    return o;
}

void MSkyPlot::SaveNexPlot(TString nexplotname)
{
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetCanvasBorderSize(0);
    gStyle->SetCanvasColor(10);
//    gStyle->SetPadBorderMode(0);
//    gStyle->SetPadBorderSize(0);
    gStyle->SetPadColor(10);
    gStyle->SetOptFit(0);
    gStyle->SetOptStat(0);
    gStyle->SetStatColor(10);
    gStyle->SetPalette(1);
    gStyle->SetPadRightMargin(0.16);
    gStyle->SetPadLeftMargin(0.13);

    Char_t timet[100];

    TH2D tmp = fHistNexcess;
    tmp.SetMaximum(470); 
    tmp.SetMinimum(-90); 
    TCanvas can("SkyPlot","SkyPlot", 0, 0, 800, 400);
    can.Divide(2,1);
    can.cd(1);
    tmp.GetYaxis()->SetTitleOffset(1.3);
    tmp.Draw("colz"); 
    TPaveLabel myname(fMinXGrid-0.5,fMinYGrid-0.4,fMinXGrid+0.3,fMinYGrid-0.2,"by D. Mazin");
    myname.Draw();

    can.cd(2);
    fHistNexcess.SetMaximum(470);    
    fHistNexcess.SetMinimum(-40);    
    fHistNexcess.GetXaxis()->SetTitleOffset(1.3);
    fHistNexcess.GetYaxis()->SetTitleOffset(1.6);
    gPad->SetTheta(20);
    fHistNexcess.Draw("lego2");
    TLatex tu(0.5,0.8,"elapsed time:");
    tu.Draw();
    sprintf(timet,"%.1f min",fElaTime);
    TLatex tut(0.5,0.7,timet);
    tut.Draw();

    can.Print(nexplotname);  
//    can.Print("test.root");
}

void MSkyPlot::SaveSkyPlots(TString skyplotfilename)
{

  TFile rootfile(skyplotfilename, "RECREATE",
		 "sky plots in some variations");
  fHistSignif.Write();
  fHistNexcess.Write();
  fHistOn.Write();
  fHistSignif.Write();
     
  // from Wolfgang: 
  //--------------------------------------------------------------------
  // Dec-Hour lines
  Double_t rho, sinrho, cosrho;
  Double_t theta, phi, sing, cosg;
  // do I need it?
  if (fTime && fObservatory && fPointPos)
    {
      rho = fPointPos->RotationAngle(*fObservatory, *fTime);
      sinrho=TMath::Sin(rho);
      cosrho=TMath::Cos(rho);
    }
  
  theta = fPointPos->GetZd()*TMath::Pi()/180.;
  phi = fPointPos->GetAz()*TMath::Pi()/180.;
  //   printf("theta: %5.3f, phi: %5.3f\n", theta*180./4.1415, phi*180./4.1415);
  fObservatory->RotationAngle(theta, phi, sing, cosg);
  
  *fLog << "1: sinrho, cosrho = " << sinrho << ", " << cosrho << endl;
  *fLog << "2: sinrho, cosrho = " << sing << ", " << cosg << endl;
  sinrho=sing;
  cosrho=cosg;

  Double_t fDistCam = fGeomCam->GetCameraDist() * 1000.0;     //  [mm]
  Double_t gridbinning = fGridBinning;
  Double_t gridfinebin = fGridFineBin;
  Int_t    numgridlines = (Int_t)(4.0/gridbinning);
  Double_t aberr = 1.07;
  Double_t mmtodeg = 180.0 / TMath::Pi() / fDistCam ;
  
  Double_t declin, hangle;  // [deg], [h]
  MStarCamTrans mstarc(*fGeomCam, *fObservatory);
  if (fRepDrive) mstarc.LocToCel(fRepDrive->GetNominalZd(),fRepDrive->GetNominalAz(),declin, hangle);
  else mstarc.LocToCel(theta*180./TMath::Pi(),phi*180./TMath::Pi(),declin, hangle); // NOT GOOD!
  
  TLatex *pix;
  
  Char_t tit[100];
  Double_t xtxt;
  Double_t ytxt;
  
  Double_t xlo = -534.0 * mmtodeg;
  Double_t xup =  534.0 * mmtodeg;
  
  Double_t ylo = -534.0 * mmtodeg;
  Double_t yup =  534.0 * mmtodeg;
  
  Double_t xx, yy;
  
  
  // direction for the center of the camera
  Double_t dec0 = declin;
  Double_t h0   = hangle*360./24.; //deg
  //    Double_t RaHOffset = fRepDrive->GetRa() - h0;
    //trans.LocToCel(theta0, phi0, dec0, h0);
  
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  TCanvas *c1 = new TCanvas("SPlotsRaDecLines","SPlotsRaDecLines", 400, 0, 700, 600);
  c1->Divide(2,2);
  c1->cd(1);
  fHistSignif.Draw("colz");
  c1->cd(2);
  fHistNexcess.Draw("colz");
  c1->cd(3);
  fHistOn.Draw("colz");
  c1->cd(4);
  gPad->SetLogy();
  fHistSignifGaus.Draw(); 
  
  //-----   lines for fixed dec   ------------------------------------
  
  const Int_t Ndec = numgridlines;
  Double_t dec[Ndec];
  Double_t ddec = gridbinning;
  
  Int_t nh = (Int_t)(4.0/gridfinebin);
  const Int_t Nh   = nh+1;
  Double_t h[Nh];
  Double_t dh = gridfinebin/cos(dec0/180.0*3.1415926);
  if ( dh > 360.0/((Double_t)(Nh-1)) )
    dh = 360.0/((Double_t)(Nh-1));
  
  // start to copy
  for (Int_t j=0; j<Ndec; j++)
    {
      dec[j] = dec0 + ((Double_t)j)*ddec
	- ((Double_t)(Ndec/2)-1.0)*ddec;
    }
  
  for (Int_t k=0; k<Nh; k++)
    {
      h[k] = h0 + ((Double_t)k)*dh - ((Double_t)(Nh/2)-1.0)*dh;
    }
  
  Double_t xh[Nh];
  Double_t yh[Nh];
  
  for (Int_t j=0; j<Ndec; j++)
    {
      if (fabs(dec[j]) > 90.0) continue;
      
      for (Int_t k=0; k<Nh; k++)
	{
	  Double_t hh0 = h0   *24.0/360.0;
	  Double_t hx = h[k]*24.0/360.0;
	  mstarc.Cel0CelToCam(dec0, hh0, dec[j], hx,
			      xx, yy);
	  xx = xx * mmtodeg * aberr;
	  yy = yy * mmtodeg * aberr;
	  //        xh[k] = xx * mmtodeg * aberr;
	  //        yh[k] = yy * mmtodeg * aberr;
	  xh[k] = cosg * xx + sing * yy;
	  yh[k] =-sing * xx + cosg * yy;
	  //        xh[k] = cosrho * xx + sinrho * yy;
	  //        yh[k] =-sinrho * xx + cosrho * yy;
	  
	  
	  //gLog << "dec0, h0 = " << dec0 << ",  " << h0
	  //     << " : dec, h, xh, yh = " << dec[j] << ",  "
	  //     << h[k] << ";   " << xh[k] << ",  " << yh[k] << endl;
	}
      
      //      c1->cd(2);
      TGraph * graph1 = new TGraph(Nh,xh,yh);
      //graph1->SetLineColor(j+1);
      graph1->SetLineColor(36);
      graph1->SetLineStyle(1);
      graph1->SetLineWidth(1);
      //graph1->SetMarkerColor(j+1);
      graph1->SetMarkerColor(1);
      graph1->SetMarkerSize(.2);
      graph1->SetMarkerStyle(20);
      
      c1->cd(1);
      graph1->Draw("L");
      c1->cd(2);
      graph1->Draw("L");
      c1->cd(3);
      graph1->Draw("L");
      //delete graph1;
      //      graphvec.push_back(*graph1);
      //      graphvec[j].Draw("L");
      
      sprintf(tit,"Dec = %6.2f", dec[j]);
      xtxt = xlo + (xup-xlo)*0.1;
      ytxt = ylo + (yup-ylo)*0.80 - ((Double_t)j) *(yup-ylo)/20.0;
      pix = new TLatex(xtxt, ytxt, tit);
      pix->SetTextColor(36);
      pix->SetTextSize(.03);
      //pix->Draw("");
      //delete pix;
      
    }
  //stop copy
  //-----   lines for fixed hour angle   ------------------------------------
  
  Int_t ndec1 = (Int_t)(4.0/gridfinebin);
  const Int_t Ndec1 = ndec1;
  Double_t dec1[Ndec1];
  Double_t ddec1 = gridfinebin;
  
  const Int_t Nh1   = numgridlines;
  Double_t h1[Nh1];
  Double_t dh1 = gridbinning/cos(dec0/180.0*3.1415926);
  if ( dh1 > 360.0/((Double_t)Nh1) )
    dh1 = 360.0/((Double_t)Nh1);
  
  // start copy
  for (Int_t j=0; j<Ndec1; j++)
    {
      dec1[j] = dec0 + ((Double_t)j)*ddec1
	- ((Double_t)(Ndec1/2)-1.0)*ddec1;
    }
  
  for (Int_t k=0; k<Nh1; k++)
    {
      h1[k] = h0 + ((Double_t)k)*dh1 - ((Double_t)(Nh1/2)-1.0)*dh1;
    }
  
  Double_t xd[Ndec1];
  Double_t yd[Ndec1];
  
  for (Int_t k=0; k<Nh1; k++)
    {
      Int_t count = 0;
      for (Int_t j=0; j<Ndec1; j++)
	{
	  if (fabs(dec1[j]) > 90.0) continue;
	  
	  Double_t hh0 = h0   *24.0/360.0;
	  Double_t hhx = h1[k]*24.0/360.0;
	  mstarc.Cel0CelToCam(dec0, hh0, dec1[j], hhx,
                           xx, yy);
	  //        xd[count] = xx * mmtodeg * aberr;
	  //        yd[count] = yy * mmtodeg * aberr;
	  
	  xx = xx * mmtodeg * aberr;
	  yy = yy * mmtodeg * aberr;
	  xd[count] = cosg * xx + sing * yy;
	  yd[count] =-sing * xx + cosg * yy;

	  //gLog << "dec0, h0 = " << dec0 << ",  " << h0
	  //     << " : dec1, h1, xd, yd = " << dec1[j] << ",  "
	  //     << h1[k] << ";   " << xd[count] << ",  " << yd[count] << endl;
	  
	  count++;
	}
      
      //      c1->cd(2);
      TGraph * graph1 = new TGraph(count,xd,yd);
      //graph1->SetLineColor(k+1);
      graph1->SetLineColor(36);
      graph1->SetLineWidth(2);
      graph1->SetLineStyle(1);
      //graph1->SetMarkerColor(k+1);
      graph1->SetMarkerColor(1);
      graph1->SetMarkerSize(.2);
      graph1->SetMarkerStyle(20);
      c1->cd(1);
      graph1->Draw("L");
      c1->cd(2);
      graph1->Draw("L");
      c1->cd(3);
      graph1->Draw("L");

      sprintf(tit,"RA = %6.2f", fRa0 + (h0 - h1[k]));
      Double_t xtxt = xlo + (xup-xlo)*0.75;
      Double_t ytxt = ylo + (yup-ylo)*0.80 - ((Double_t)k) *(yup-ylo)/20.0;
      pix = new TLatex(xtxt, ytxt, tit);
      pix->SetTextColor(36);
      pix->SetTextSize(.03);
      //pix->Draw("");
      //delete pix;
      
    }
  
  //    c1->cd(2);
  sprintf(tit,"Dec0 = %6.2f [deg]   Ra0 = %6.2f [h]", dec0, fRa0);
  xtxt = xlo + (xup-xlo)*0.05 + 0.80;
  ytxt = ylo + (yup-ylo)*0.75;
  pix = new TLatex(xtxt, ytxt, tit);
  pix->SetTextColor(1);
  pix->SetTextSize(.05);
  c1->cd(1);
  pix->Draw("");
  c1->cd(2);
  pix->Draw("");
  c1->cd(3);
  pix->Draw("");
  //delete pix;
  
  c1->Write();
  // we suppose that the {skyplotfilename} ends with .root
  Int_t sizeofout = skyplotfilename.Sizeof();
  TString outps = skyplotfilename.Remove(sizeofout-5,5) + "ps";
  c1->Print(outps);  // temporary!!!!!

  TCanvas *c2 = new TCanvas("SkyPlotsWithRaDecLines","SkyPlotsWithRaDecLines", 0, 0, 300, 600);
  c2->Divide(1,2);
  c2->SetBorderMode(0);
  c2->cd(1);
  fHistSignif.Draw("colz");
  c2->cd(2);
  fHistNexcess.Draw("colz");
  c2->Write();
  
  rootfile.Close();
  delete c1;
  delete c2;
  delete pix;

}

void MSkyPlot::SaveAlphaPlots(const TString alphaplotfilename)
{
   TFile rootfile(alphaplotfilename, "RECREATE",
                   "all the alpha plots");

    int index = 0;  // index of the TH2F histograms
    Char_t strtitle[100];
    Char_t strname[100];
    Float_t xpos, ypos, signif, nex;
    Int_t nrow, ncolumn, non;

    TH1D *alpha_iterator = NULL;
    TOrdCollectionIter Next(fHistAlpha);

    while( (alpha_iterator = (TH1D*)Next())) {

          nrow    = index/fHistOn.GetNbinsX() + 1;                    // row of the histogram (y)
          //ncolumn = TMath::Nint(fmod(index,fHistOn.GetNbinsX()))+1;   // column of the histogram (x)
          ncolumn = index%fHistOn.GetNbinsX()+1;   // column of the histogram (x)
          xpos    = fMinXGrid + fBinStepGrid*(ncolumn-1);
          ypos    = fMinYGrid + fBinStepGrid*(nrow-1);
          non     = TMath::Nint(fHistOn.GetBinContent(ncolumn,nrow));
          nex     = fHistNexcess.GetBinContent(ncolumn,nrow);
          signif  = fHistSignif.GetBinContent(ncolumn,nrow);

          sprintf(strname,"AlphaPlotX%.2fY%.2f", xpos, ypos); 
          sprintf(strtitle,"for x= %.2f deg, y= %.2f deg: S= %.2f sigma, Nex= %.2f, Non= %d", 
			xpos, ypos, signif, nex, non);
           
	  alpha_iterator->SetName(strname);
	  alpha_iterator->SetTitle(strtitle);
          alpha_iterator->Write();
          index++;
    }

   rootfile.Close();
}


// --------------------------------------------------------------------------
//
// This is a preliminary implementation of a alpha-fit procedure for
// all possible source positions. It will be moved into its own
// more powerfull class soon.
//
// The fit function is "gaus(0)+pol2(3)" which is equivalent to:
//   [0]*exp(-0.5*((x-[1])/[2])^2) + [3] + [4]*x + [5]*x^2
// or
//   A*exp(-0.5*((x-mu)/sigma)^2) + a + b*x + c*x^2
//
// Parameter [1] is fixed to 0 while the alpha peak should be
// symmetric around alpha=0.
//
// Parameter [4] is fixed to 0 because the first derivative at
// alpha=0 should be 0, too.
//
// In a first step the background is fitted between bgmin and bgmax,
// while the parameters [0]=0 and [2]=1 are fixed.
//
// In a second step the signal region (alpha<sigmax) is fittet using
// the whole function with parameters [1], [3], [4] and [5] fixed.
//
// The number of excess and background events are calculated as
//   s = int(0, sigint, gaus(0)+pol2(3))
//   b = int(0, sigint,         pol2(3))
//
// The Significance is calculated using the Significance() member
// function.
//

// I might need this
/*
    TCanvas *c=new TCanvas;

    gStyle->SetPalette(1, 0);

    c->Divide(3,2, 0, 0);
    c->cd(1);
    gPad->SetBorderMode(0);
    hists->Draw("colz");
    hists->SetBit(kCanDelete);
    catalog->Draw("mirror same");
    c->cd(2);
    gPad->SetBorderMode(0);
    hist->Draw("colz");
    hist->SetBit(kCanDelete);
    catalog->Draw("mirror same");
    c->cd(3);
    gPad->SetBorderMode(0);
    histb->Draw("colz");
    histb->SetBit(kCanDelete);
    catalog->Draw("mirror same");
    c->cd(4);
    gPad->Divide(1,3, 0, 0);
    TVirtualPad *p=gPad;
    p->SetBorderMode(0);
    p->cd(1);
    gPad->SetBorderMode(0);
    h0b.DrawCopy();
    h0a.DrawCopy("same");
    p->cd(2);
    gPad->SetBorderMode(0);
    h3.DrawCopy();
    p->cd(3);
    gPad->SetBorderMode(0);
    h2.DrawCopy();

*/
