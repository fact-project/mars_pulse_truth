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
!   Author(s): Robert Wagner, 2/2004 <mailto:rwagner@mppmu.mpg.de>
!   Author(s): Javier López , 4/2004 <mailto:jlopez@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MFindStars
//
/////////////////////////////////////////////////////////////////////////////
#include "MFindStars.h"

#include <TMinuit.h>
#include <TStopwatch.h>
#include <TTimer.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TF1.h>
#include <TEllipse.h>


#include "MObservatory.h"
#include "MAstroCamera.h"
#include "MMcConfigRunHeader.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MHCamera.h"

#include "MGeomCam.h"
#include "MGeomPix.h"
#include "MCameraDC.h"
#include "MTime.h"
#include "MReportDrive.h"
#include "MStarLocalCam.h"
#include "MStarLocalPos.h"

#include "MParList.h"
#include "MTaskList.h"

ClassImp(MFindStars);
using namespace std;

const Float_t sqrt2 = sqrt(2.);
const Float_t sqrt3 = sqrt(3.);
const UInt_t  lastInnerPixel = 396;
    

//______________________________________________________________________________
//
// The 2D gaussian fucntion used to fit the spot of the star
//
static Double_t func(float x,float y,Double_t *par)
{
    Double_t value=par[0]*exp(-(x-par[1])*(x-par[1])/(2*par[2]*par[2]))*exp(-(y-par[3])*(y-par[3])/(2*par[4]*par[4]));
    return value;
}

//______________________________________________________________________________
//
// Function used by Minuit to do the fit
//
static void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{

  MParList*      plist = (MParList*)gMinuit->GetObjectFit();
  MTaskList*     tlist = (MTaskList*)plist->FindObject("MTaskList");
  MFindStars*    find = (MFindStars*)tlist->FindObject("MFindStars");
  MStarLocalCam* stars = (MStarLocalCam*)plist->FindObject("MStarLocalCam");
  MGeomCam*      geom = (MGeomCam*)plist->FindObject("MGeomCam");

  MHCamera& display = (MHCamera&)find->GetDisplay();
  
  Float_t innerped = stars->GetInnerPedestalDC();
  Float_t innerrms = stars->GetInnerPedestalRMSDC();
  Float_t outerped = stars->GetOuterPedestalDC();
  Float_t outerrms = stars->GetOuterPedestalRMSDC();

  UInt_t numPixels = geom->GetNumPixels();
  
//calculate chisquare
    Double_t chisq = 0;
    Double_t delta;
    Double_t x,y,z;
    Double_t errorz=0;

    UInt_t usedPx=0;
    for (UInt_t pixid=1; pixid<numPixels; pixid++) 
    {
	if (display.IsUsed(pixid))
	{
	    x = (*geom)[pixid].GetX();
	    y = (*geom)[pixid].GetY();
            z = display.GetBinContent(pixid+1)-(pixid>lastInnerPixel?outerped:innerped);
            errorz=(pixid>lastInnerPixel?outerrms:innerrms);

	    if (errorz > 0.0)
	    {
              usedPx++;
              delta  = (z-func(x,y,par))/errorz;
              chisq += delta*delta;
	    }
	    else
		cerr << " TMinuit::fcn errorz[" << pixid << "] " << errorz << endl;
	}
    }
    f = chisq;

    find->SetChisquare(chisq);
    find->SetDegreesofFreedom(usedPx);
}

MFindStars::MFindStars(const char *name, const char *title): 
  fGeomCam(NULL), fCurr(NULL), fTimeCurr(NULL), fDrive(NULL), fStars(NULL), fNumVar(5)
{
  fName  = name  ? name  : "MFindStars";
  fTitle = title ? title : "Tool to find stars from DC Currents";

  fNumIntegratedEvents=0;
  fMaxNumIntegratedEvents = 10;
  fRingInterest = 125.; //[mm] ~ 0.4 deg
  fDCTailCut = 4;
  
  fPixelsUsed.Set(577);
  fPixelsUsed.Reset((Char_t)kTRUE);
  
  //Fitting(Minuit) initialitation
  const Float_t pixelSize = 31.5; //[mm]
  fMinuitPrintOutLevel = -1;
  
  fVname = new TString[fNumVar];
  fVinit.Set(fNumVar); 
  fStep.Set(fNumVar); 
  fLimlo.Set(fNumVar); 
  fLimup.Set(fNumVar); 
  fFix.Set(fNumVar);

  fVname[0] = "max";
  fVinit[0] = 10.*fMaxNumIntegratedEvents;
  fStep[0]  = fVinit[0]/sqrt2;
  fLimlo[0] = fMinDCForStars;
  fLimup[0] = 30.*fMaxNumIntegratedEvents;
  fFix[0]   = 0;

  fVname[1] = "meanx";
  fVinit[1] = 0.;
  fStep[1]  = fVinit[1]/sqrt2;
  fLimlo[1] = -600.;
  fLimup[1] = 600.;
  fFix[1]   = 0;

  fVname[2] = "sigmaminor";
  fVinit[2] = pixelSize;
  fStep[2]  = fVinit[2]/sqrt2;
  fLimlo[2] = pixelSize/(2*sqrt3);
  fLimup[2] = 500.;
  fFix[2]   = 0;

  fVname[3] = "meany";
  fVinit[3] = 0.;
  fStep[3]  = fVinit[3]/sqrt2;
  fLimlo[3] = -600.;
  fLimup[3] = 600.;
  fFix[3]   = 0;

  fVname[4] = "sigmamajor";
  fVinit[4] = pixelSize;
  fStep[4]  = fVinit[4]/sqrt2;
  fLimlo[4] = pixelSize/(2*sqrt3);
  fLimup[4] = 500.;
  fFix[4]   = 0;

  fObjectFit  = NULL;
  //  fMethod     = "SIMPLEX";
  fMethod     = "MIGRAD";
  //  fMethod     = "MINIMIZE";
  fNulloutput = kFALSE;

  // Set output level
  //  fLog->SetOutputLevel(3); // No dbg messages

  fGeometryFile="";
  fBSCFile="";
}

Int_t MFindStars::PreProcess(MParList *pList)
{

    fGeomCam = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));

    if (!fGeomCam)
    {
      *fLog << err << AddSerialNumber("MGeomCam") << " not found ... aborting" << endl;
      return kFALSE;
    }

    // Initialize camera display with the MGeomCam information
    fDisplay.SetGeometry(*fGeomCam,"FindStarsDisplay","FindStarsDisplay");
    fDisplay.SetUsed(fPixelsUsed);

    fCurr = (MCameraDC*)pList->FindObject(AddSerialNumber("MCameraDC"));

    if (!fCurr)
    {
      *fLog << err << AddSerialNumber("MCameraDC") << " not found ... aborting" << endl;
      return kFALSE;
    }

    fTimeCurr = (MTime*)pList->FindObject(AddSerialNumber("MTimeCurrents"));

    if (!fTimeCurr)
    {
      *fLog << err << AddSerialNumber("MTimeCurrents") << " not found ... aborting" << endl;
      return kFALSE;
    }

    fDrive = (MReportDrive*)pList->FindObject(AddSerialNumber("MReportDrive"));

    if (!fDrive)
      {

        *fLog << warn << AddSerialNumber("MReportDrive") << " not found ... ignored." << endl;

      }
    else
      {
	
	MObservatory magic1;

	MMcConfigRunHeader *config=0;
	MGeomCam           *geom=0;

	TFile file(fGeometryFile);
	TTree *tree = (TTree*)file.Get("RunHeaders");
	tree->SetBranchAddress("MMcConfigRunHeader", &config);
	if (tree->GetBranch("MGeomCam")) tree->SetBranchAddress("MGeomCam", &geom);
	tree->GetEntry(0);
	
	fAstro.SetMirrors(*config->GetMirrors());
	fAstro.SetGeom(*geom);	
	fAstro.ReadBSC(fBSCFile);
	
	fAstro.SetObservatory(magic1);
	
      }
    

    fStars = (MStarLocalCam*)pList->FindCreateObj(AddSerialNumber("MStarLocalCam"));
    if (!fStars)
    {
      *fLog << err << AddSerialNumber("MStarLocalCam") << " cannot be created ... aborting" << endl;
      return kFALSE;
    }

    fMinDCForStars = 1.*fMaxNumIntegratedEvents; //[uA]

    // Initialize the TMinuit object

    TMinuit *gMinuit = new TMinuit(fNumVar);  //initialize TMinuit with a maximum of params
    gMinuit->SetFCN(fcn);

    Double_t arglist[10];
    Int_t ierflg = 0;

    arglist[0] = 1;
    gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
    arglist[0] = fMinuitPrintOutLevel;
    gMinuit->mnexcm("SET PRI", arglist ,1,ierflg);

    // Set MParList object pointer to allow mimuit to access internally
    gMinuit->SetObjectFit(pList);

    return kTRUE;
}

Int_t MFindStars::Process()
{

  UInt_t numPixels = fGeomCam->GetNumPixels();
  TArrayC origPixelsUsed;
  origPixelsUsed.Set(numPixels);

  if (fNumIntegratedEvents >= fMaxNumIntegratedEvents) {

    //Fist delete the previous stars in the list
    fStars->GetList()->Delete();

    if (fDrive) {

      //If drive information is provided we take RaDec info
      //from the drive and let the star list fill by the astrocamera.

      //FIXME: rwagner: Doesn't work as expected
      //FIXME: rwagner: For the time being set manually.
      //fAstro.SetRaDec(fDrive->GetRa(), fDrive->GetDec());              

      fAstro.SetTime(*fTimeCurr);
      fAstro.SetGuiActive();
      fAstro.FillStarList(fStars->GetList());      

      cout << "Number of Stars added by astrocamera is " <<fStars->GetList()->GetSize() << endl;
      
      MStarLocalPos* starpos;
      TIter Next(fStars->GetList());
      while ((starpos=(MStarLocalPos*)Next())) {
	starpos->SetCalcValues(40,40,starpos->GetXExp(),starpos->GetYExp(),fRingInterest/2,fRingInterest/2);
	starpos->SetFitValues (40,40,starpos->GetXExp(),starpos->GetYExp(),fRingInterest/2,fRingInterest/2,0.,1);
      }

      for (UInt_t pix=1; pix<numPixels; pix++) {
	if (fDisplay.IsUsed(pix))
	  origPixelsUsed[pix]=(Char_t)kTRUE;
	else
	  origPixelsUsed[pix]=(Char_t)kFALSE;
      }
          
      DCPedestalCalc();

    } 
    else 
    {
      
      cout << "No drive information available: Will not use a star catalog to identify stars."<< endl;
      
      for (UInt_t pix=1; pix<numPixels; pix++) {
	if (fDisplay.IsUsed(pix))
	  origPixelsUsed[pix]=(Char_t)kTRUE;
	else
	  origPixelsUsed[pix]=(Char_t)kFALSE;
      }
          
      if (DCPedestalCalc()) {

	Float_t innermin = fStars->GetInnerPedestalDC()+fDCTailCut*fStars->GetInnerPedestalRMSDC();
	Float_t outermin = fStars->GetOuterPedestalDC()+fDCTailCut*fStars->GetOuterPedestalRMSDC();
	fMinDCForStars = innermin>outermin?innermin:outermin;
	if (fMinuitPrintOutLevel>=0) *fLog << dbg << "fMinDCForStars = " << fMinDCForStars << endl;
              
	// Find the star candidates searching the most brights pairs of pixels
	Float_t maxPixelDC;
	MGeomPix maxPixel;
	
	while(FindPixelWithMaxDC(maxPixelDC, maxPixel)) {
	  
	  MStarLocalPos *starpos = new MStarLocalPos;
	  starpos->SetExpValues(maxPixelDC,maxPixel.GetX(),maxPixel.GetY());
	  starpos->SetCalcValues(maxPixelDC,maxPixelDC,maxPixel.GetX(),maxPixel.GetY(),fRingInterest/2,fRingInterest/2);
	  starpos->SetFitValues(maxPixelDC,maxPixelDC,maxPixel.GetX(),maxPixel.GetY(),fRingInterest/2,fRingInterest/2,0.,1);
	  fStars->GetList()->Add(starpos);
	  
	  ShadowStar(starpos);
	}	
	fDisplay.SetUsed(origPixelsUsed);
      }      
    }

    //Show the stars found
    //fStars->Print("namepos");
   
    //loop to extract position of stars on the camera
    if (fStars->GetList()->GetSize() == 0) {
      *fLog << err << GetName() << "No stars candidates in the camera." << endl;
      return kCONTINUE;
    } else
      *fLog << inf << GetName() << " found " << fStars->GetList()->GetSize() 
	    << " stars candidates in the camera." << endl;
    
    for (UInt_t pix=1; pix<numPixels; pix++) {
      if (fDisplay.IsUsed(pix))
	origPixelsUsed[pix]=(Char_t)kTRUE;
      else
	origPixelsUsed[pix]=(Char_t)kFALSE;
    }

    TIter Next(fStars->GetList());
    MStarLocalPos* star;
    while ((star=(MStarLocalPos*)Next())) {
       FindStar(star);
       ShadowStar(star);
    }

    //Show the stars found: Here it is interesting to see what FindStars
    //made out of the positions we gave to it.
    fStars->Print("namepos");

    
    //After finding stars reset all variables
    fDisplay.Reset();
    fStars->GetDisplay().Reset(); //FIXME: Put this display just in the container
    fDisplay.SetUsed(origPixelsUsed);
    fNumIntegratedEvents=0;
  }

  for (UInt_t pix=1; pix<numPixels; pix++) {
    if (fDisplay.IsUsed(pix))
      origPixelsUsed[pix]=(Char_t)kTRUE;
    else
      origPixelsUsed[pix]=(Char_t)kFALSE;
    
  }
  
  fDisplay.AddCamContent(*fCurr);
  fNumIntegratedEvents++;
  fDisplay.SetUsed(origPixelsUsed);
  
  return kTRUE;
}

Int_t MFindStars::PostProcess()
{
  return kTRUE;
}

void MFindStars::SetBlindPixels(TArrayS blindpixels)
{
    Int_t npix = blindpixels.GetSize();

    for (Int_t idx=0; idx<npix; idx++)
      {
	fPixelsUsed[blindpixels[idx]]=(Char_t)kFALSE;
        if (fMinuitPrintOutLevel>=0) *fLog << dbg << "MFindStars::SetBlindPixels fDisplay.IsUsed(" <<blindpixels[idx]  << ") kFALSE" << endl;
      }
    
    fDisplay.SetUsed(fPixelsUsed);
}

Bool_t MFindStars::DCPedestalCalc()
{
    //-------------------------------------------------------------
    // save pointer to the MINUIT object for optimizing the supercuts
    // because it will be overwritten 
    // when fitting the alpha distribution in MHFindSignificance
    TMinuit *savePointer = gMinuit;
    //-------------------------------------------------------------

   UInt_t numPixels = fGeomCam->GetNumPixels();
   Float_t ped;
   Float_t rms;

   TH1F **dchist = new TH1F*[2];
   for (UInt_t i=0; i<2; i++)
      dchist[i] = new TH1F("","",26,0.4*fMaxNumIntegratedEvents,3.*fMaxNumIntegratedEvents);
   
   for (UInt_t pix=1; pix<=lastInnerPixel; pix++)
       dchist[0]->Fill(fDisplay.GetBinContent(pix+1));
   for (UInt_t pix=lastInnerPixel+1; pix<numPixels; pix++)
       dchist[1]->Fill(fDisplay.GetBinContent(pix+1));

   // inner/outer pixels
   for (UInt_t i=0; i<2; i++)
    {
      Float_t nummaxprobdc = dchist[i]->GetBinContent(dchist[i]->GetMaximumBin());
      Float_t maxprobdc = dchist[i]->GetBinCenter(dchist[i]->GetMaximumBin());
      UInt_t bin = dchist[i]->GetMaximumBin();
      do
        {
          bin++;
        }
      while(dchist[i]->GetBinContent(bin)/nummaxprobdc > 0.5);
      Float_t halfmaxprobdc = dchist[i]->GetBinCenter(bin);
      
      if (fMinuitPrintOutLevel>=0) *fLog << dbg << " maxprobdc[high] " << maxprobdc << "[" << nummaxprobdc << "] ";
      if (fMinuitPrintOutLevel>=0) *fLog << dbg << " halfmaxprobdc[high] " << halfmaxprobdc << "[" << dchist[i]->GetBinContent(bin) << "]" << endl;
      
      Float_t rmsguess = TMath::Abs(maxprobdc-halfmaxprobdc);
      Float_t min = maxprobdc-3*rmsguess;
      min = (min<0.?0.:min);
      Float_t max = maxprobdc+3*rmsguess;
      
      if (fMinuitPrintOutLevel>=0) *fLog << dbg << " maxprobdc " << maxprobdc << " rmsguess " << rmsguess << endl;
      
      TF1 func("func","gaus",min,max);
      func.SetParameters(nummaxprobdc, maxprobdc, rmsguess);
      
      dchist[i]->Fit("func","QR0");
      
      UInt_t aproxnumdegrees = 6*(bin-dchist[i]->GetMaximumBin());
      Float_t chiq = func.GetChisquare();
      ped = func.GetParameter(1);
      rms = func.GetParameter(2);
      
      if (fMinuitPrintOutLevel>=0) *fLog << dbg << " ped " << ped << " rms " << rms << " chiq/ndof " << chiq << "/" << aproxnumdegrees << endl;
      
      Int_t numsigmas = 5;
      Axis_t minbin = ped-numsigmas*rms/dchist[i]->GetBinWidth(1);
      minbin=minbin<1?1:minbin;
      Axis_t maxbin = ped+numsigmas*rms/dchist[i]->GetBinWidth(1);
      if (fMinuitPrintOutLevel>=0) *fLog << dbg << " Number of pixels with dc under " << numsigmas << " sigmas = " << dchist[i]->Integral((int)minbin,(int)maxbin) << endl;
      
      //Check results from the fit are consistent
      if (ped < 0. || rms < 0.)
        {
          *fLog << dbg << "dchist[i]->GetEntries()" << dchist[i]->GetEntries();
//            TCanvas *c1 = new TCanvas("c2","c2",500,800);
//            dchist[i]->Draw();
//            c1->Print("dchist.ps");
//            delete c1;
//            exit(1);
        }
      else if (TMath::Abs(ped-maxprobdc) > rmsguess || rms > rmsguess)
        {
          *fLog << warn << GetName() << " Pedestal DC fit give non consistent results for " << (i==0?"Inner":"Outer") << "pixels." << endl;
          *fLog << warn << " maxprobdc " << maxprobdc << " rmsguess " << rmsguess << endl;
          *fLog << warn << " ped " << ped << " rms " << rms << " chiq/ndof " << chiq << "/" << aproxnumdegrees << endl;
          ped = maxprobdc;
          rms = rmsguess/1.175; // FWHM=2.35*rms
        }
   
      if (i == 0)
        {
          fStars->SetInnerPedestalDC(ped);
          fStars->SetInnerPedestalRMSDC(rms);
        }
      else
        {
          fStars->SetOuterPedestalDC(ped);
          fStars->SetOuterPedestalRMSDC(rms);
        }

      

    }
   

   for (UInt_t i=0; i<2; i++)
      delete dchist[i];
   delete [] dchist;

   //=================================================================

   // reset gMinuit to the MINUIT object for optimizing the supercuts 
   gMinuit = savePointer;
   //-------------------------------------------
   
   if (fStars->GetInnerPedestalDC() < 0. ||  fStars->GetInnerPedestalRMSDC() < 0. || fStars->GetOuterPedestalDC() < 0. ||  fStars->GetOuterPedestalRMSDC() < 0.)
     return kFALSE;
  
   return kTRUE;
}
    
Bool_t MFindStars::FindPixelWithMaxDC(Float_t &maxDC, MGeomPix &maxPix)
{
    UInt_t numPixels = fGeomCam->GetNumPixels();

// Find the two close pixels with the maximun dc
    UInt_t maxPixIdx[2];

    maxDC = 0;

    for (UInt_t pix=1; pix<numPixels; pix++)
    {
	if(fDisplay.IsUsed(pix))
	{
	    Float_t dc[2];
	    dc[0] = fDisplay.GetBinContent(pix+1);
	    if (dc[0] < fMinDCForStars)
		continue;

	    MGeomPix &g = (*fGeomCam)[pix];
	    Int_t numNextNeighbors = g.GetNumNeighbors();
	    
	    Float_t dcsum;
	    for(Int_t nextNeighbor=0; nextNeighbor<numNextNeighbors; nextNeighbor++)
	    {
              UInt_t swneighbor = g.GetNeighbor(nextNeighbor);
              if(fDisplay.IsUsed(swneighbor))
                {
                  dc[1] = fDisplay.GetBinContent(swneighbor+1);
                  if (dc[1] < fMinDCForStars)
                    continue;
                  
                  dcsum = dc[0] + dc[1];
                  
                  if(dcsum > maxDC*2)
                    {
                      if(dc[0]>=dc[1])
                        {
                          maxPixIdx[0] = pix;
                          maxPixIdx[1] = swneighbor;
                          maxDC = dc[0];
                        }
                      else
                        {
                          maxPixIdx[1] = pix;
                          maxPixIdx[0] = swneighbor;
                          maxDC = dc[1];
                        }
                    }	
                }
            }
        }
    }

    if (maxDC == 0)
      {
        *fLog << warn << " No found pixels with maximum dc" << endl;
	return kFALSE;
      }
    
    maxPix = (*fGeomCam)[maxPixIdx[0]];

    if (fMinuitPrintOutLevel>=0) *fLog << dbg << "Star candidate maxDC(" << setw(3) << maxDC << " uA) x position(" << setw(3) << maxPix.GetX() <<  " mm) x position(" << setw(3) << maxPix.GetY() << " mm) swnumber(" << maxPixIdx[0] << ")" << endl;

    return kTRUE;
}

Bool_t MFindStars::FindStar(MStarLocalPos* star)
{    
 
  UInt_t numPixels = fGeomCam->GetNumPixels();
  Float_t innerped = fStars->GetInnerPedestalDC();
  Float_t outerped = fStars->GetOuterPedestalDC();

  TArrayC origPixelsUsed;
  origPixelsUsed.Set(numPixels);
  
  for (UInt_t pix=1; pix<numPixels; pix++)
    {
      if (fDisplay.IsUsed(pix))
        origPixelsUsed[pix]=(Char_t)kTRUE;
      else
        origPixelsUsed[pix]=(Char_t)kFALSE;
    }
  
  Float_t expX = star->GetXExp();
  Float_t expY = star->GetYExp();
  
  Float_t max=0;
  UInt_t  pixmax=0;
  Float_t meanX=0;
  Float_t meanY=0;
  Float_t meanSqX=0;
  Float_t meanSqY=0;
  Float_t sumCharge=0;
  UInt_t usedInnerPx=0;	
  UInt_t usedOuterPx=0;	

  const Float_t meanPresition = 3.; //[mm]
  const UInt_t maxNumIterations = 10;
  UInt_t numIterations = 0;

  do
    {

      //rwagner: Need to find px with highest dc and need to assume it is
      // somewhere near the center of the star
      //after that we need to REDEFINE our roi! because expected pos could be
      // quite off that px!
      
      // 1.) Initial roi around expected position

      for (UInt_t pix=1; pix<numPixels; pix++)
	{
	  
	  Float_t pixXpos=(*fGeomCam)[pix].GetX();
	  Float_t pixYpos=(*fGeomCam)[pix].GetY();
	  Float_t dist = sqrt((pixXpos-expX)*(pixXpos-expX)+
			      (pixYpos-expY)*(pixYpos-expY));
	  
	  if ((dist < fRingInterest) && fDisplay.IsUsed(pix))
	    fPixelsUsed[pix]=(Char_t)kTRUE;
	  else
	    fPixelsUsed[pix]=(Char_t)kFALSE;
	}
      fDisplay.SetUsed(fPixelsUsed);

      // 2.) Find px with highest dc in that region

      for(UInt_t pix=0; pix<numPixels; pix++)	
	if(fDisplay.IsUsed(pix))
	  {
	    Float_t charge  = fDisplay.GetBinContent(pix+1);	      
	    if (charge>max)
	      {
		max=charge;
		pixmax=pix;
	      }
	    
	  }         

      // 3.) make it new center

      expX = (*fGeomCam)[pixmax].GetX();
      expY = (*fGeomCam)[pixmax].GetY();
      for (UInt_t pix=1; pix<numPixels; pix++)
	fPixelsUsed[pix]=(Char_t)kTRUE;       
      fDisplay.SetUsed(fPixelsUsed);

      // First define a area of interest around the expected position of the star
      for (UInt_t pix=1; pix<numPixels; pix++)
	{
	  
	  Float_t pixXpos=(*fGeomCam)[pix].GetX();
	  Float_t pixYpos=(*fGeomCam)[pix].GetY();
	  Float_t dist = sqrt((pixXpos-expX)*(pixXpos-expX)+
			      (pixYpos-expY)*(pixYpos-expY));
	  
	  if ((dist < fRingInterest) && fDisplay.IsUsed(pix))
	    fPixelsUsed[pix]=(Char_t)kTRUE;
	  else
	    fPixelsUsed[pix]=(Char_t)kFALSE;
	}
  
      fDisplay.SetUsed(fPixelsUsed);
    
      // determine mean x and mean y
      usedInnerPx=0;	
      usedOuterPx=0;	
      for(UInt_t pix=0; pix<numPixels; pix++)
	{
	  if(fDisplay.IsUsed(pix))
	    {
	      pix>lastInnerPixel?usedOuterPx++:usedInnerPx++;
	      
	      Float_t charge  = fDisplay.GetBinContent(pix+1);
	      Float_t pixXpos = (*fGeomCam)[pix].GetX();
	      Float_t pixYpos = (*fGeomCam)[pix].GetY();
	      
	      if (charge>max)
		{
		  max=charge;
		  pixmax=pix;
		}
	      
	      meanX     += charge*pixXpos;
	      meanY     += charge*pixYpos;
	      meanSqX   += charge*pixXpos*pixXpos;
	      meanSqY   += charge*pixYpos*pixYpos;
	      sumCharge += charge;
	    }
	} 
      
      if (fMinuitPrintOutLevel>=0) *fLog << dbg << " usedInnerPx " << usedInnerPx << " usedOuterPx " << usedOuterPx << endl;
      
      meanX   /= sumCharge;
      meanY   /= sumCharge;
      meanSqX /= sumCharge;
      meanSqY /= sumCharge;
      
      expX = meanX;
      expY = meanY;
      
      if (++numIterations >  maxNumIterations)
	{
	  *fLog << warn << GetName() << "Mean calculation not converge after " << maxNumIterations << " iterations" << endl;
	  break;
	}
        
    } while(TMath::Abs(meanX-expX) > meanPresition || TMath::Abs(meanY-expY) > meanPresition);
  
  Float_t rmsX = (meanSqX - meanX*meanX) - fRingInterest*fRingInterest/12;
  Float_t rmsY = (meanSqY - meanY*meanY) - fRingInterest*fRingInterest/12;
  
  if ( rmsX > 0)
    rmsX =  TMath::Sqrt(rmsX);
  else
    {
      *fLog << warn << " MFindStars::FindStar negative rmsX² " << rmsX << endl;
      *fLog << warn << " meanSqX " << meanSqX << " meanX " << meanX << " fRingInterest " << fRingInterest << " sumCharge " << sumCharge << endl;
      rmsX = 0.;
    }
  
  if ( rmsY > 0)
    rmsY =  TMath::Sqrt(rmsY);
  else
    {
      *fLog << warn << " MFindStars::FindStar negative rmsY² " << rmsY << endl;
      *fLog << warn << " meanSqY " << meanSqY << " meanY " << meanY << " fRingInterest " << fRingInterest << " sumCharge " << sumCharge<< endl;
      rmsY = 0.;
    }
  
  // Substrack pedestal DC
  sumCharge-= (usedInnerPx*innerped+usedOuterPx*outerped)/(usedInnerPx+usedOuterPx);
  max-=pixmax>lastInnerPixel?outerped:innerped;
  
  
  star->SetCalcValues(sumCharge,max,meanX,meanY,rmsX,rmsY);
  
  if (rmsX <= 0. || rmsY <= 0.)
    return kFALSE;
    
    
// fit the star spot using TMinuit

    
    for (UInt_t pix=1; pix<numPixels; pix++)
      if (fDisplay.IsUsed(pix))
        if (fMinuitPrintOutLevel>=0) *fLog << dbg << "[fit the star spot] fDisplay.IsUsed(" << pix << ") kTRUE" << endl;
  
    if (fMinuitPrintOutLevel>=0) *fLog << dbg << "fMinDCForStars " << fMinDCForStars << " pixmax>lastInnerPixel?outerped:innerped " << (pixmax>lastInnerPixel?outerped:innerped) << " fMaxNumIntegratedEvents " << fMaxNumIntegratedEvents << endl;

  //Initialate variables for fit
    fVinit[0] = max;
    fLimlo[0] = fMinDCForStars-(pixmax>lastInnerPixel?outerped:innerped);
    fLimup[0] = 30*fMaxNumIntegratedEvents-(pixmax>lastInnerPixel?outerped:innerped);
    fVinit[1] = meanX;
    fVinit[2] = rmsX;
    fVinit[3] = meanY;
    fVinit[4] = rmsY;
    //Init steps
    for(Int_t i=0; i<fNumVar; i++)
      {
	if (fVinit[i] != 0)
	  fStep[i] = TMath::Abs(fVinit[i]/sqrt2);
        if (fMinuitPrintOutLevel>=0) *fLog << dbg << " fVinit[" << i << "] " << fVinit[i];
        if (fMinuitPrintOutLevel>=0) *fLog << dbg << " fStep[" << i << "] " << fStep[i];
        if (fMinuitPrintOutLevel>=0) *fLog << dbg << " fLimlo[" << i << "] " << fLimlo[i];
        if (fMinuitPrintOutLevel>=0) *fLog << dbg << " fLimup[" << i << "] " << fLimup[i] << endl;
      }
    //

    // -------------------------------------------
    // call MINUIT

    Double_t arglist[10];
    Int_t ierflg = 0;

    for (Int_t i=0; i<fNumVar; i++)
      gMinuit->mnparm(i, fVname[i], fVinit[i], fStep[i], fLimlo[i], fLimup[i], ierflg);

    TStopwatch clock;
    clock.Start();

// Now ready for minimization step
    arglist[0] = 500;
    arglist[1] = 1.;
    gMinuit->mnexcm(fMethod, arglist ,2,ierflg);

    clock.Stop();

    if(fMinuitPrintOutLevel>=0)
      {
	if (fMinuitPrintOutLevel>=0) *fLog << dbg << "Time spent for the minimization in MINUIT :   " << endl;;
	clock.Print();
      }

    Double_t integratedCharge;
    Double_t maxFit, maxFitError;
    Double_t meanXFit, meanXFitError;
    Double_t sigmaMinorAxis, sigmaMinorAxisError;
    Double_t meanYFit, meanYFitError;
    Double_t sigmaMajorAxis, sigmaMajorAxisError;
    Float_t chisquare = GetChisquare();
    Int_t   dregrees  = GetDegreesofFreedom()-fNumVar;

    if (!ierflg)
      {
        gMinuit->GetParameter(0,maxFit, maxFitError);
        gMinuit->GetParameter(1,meanXFit,meanXFitError);
        gMinuit->GetParameter(2,sigmaMinorAxis,sigmaMinorAxisError);
        gMinuit->GetParameter(3,meanYFit,meanYFitError);
        gMinuit->GetParameter(4,sigmaMajorAxis,sigmaMajorAxisError);
        
        //FIXME: Do the integral properlly
        integratedCharge = 0.;

        
      }
    else
      {
        maxFit = 0.;
        meanXFit = 0.;
        sigmaMinorAxis = 0.;
        meanYFit = 0.;
        sigmaMajorAxis = 0.;
        integratedCharge = 0.;

	*fLog << err << "TMinuit::Call error " << ierflg << endl;
      }

    //rwagner: get error matrix
    Double_t matrix[2][2];
    gMinuit->mnemat(&matrix[0][0],2);

    star->SetFitValues(integratedCharge,maxFit,meanXFit,meanYFit,sigmaMinorAxis,sigmaMajorAxis,chisquare,dregrees,
  		       matrix[0][0],matrix[1][0],matrix[1][1]);
    
    // reset the display to the starting values
    fDisplay.SetUsed(origPixelsUsed);

    if (ierflg)
      return kCONTINUE;
    return kTRUE;
}

Bool_t MFindStars::ShadowStar(MStarLocalPos* star)
{
    UInt_t numPixels = fGeomCam->GetNumPixels();

// Define an area around the star which will be set unused.
    UInt_t shadowPx=0;	
    for (UInt_t pix=1; pix<numPixels; pix++)
    {
	Float_t pixXpos  = (*fGeomCam)[pix].GetX();
	Float_t pixYpos  = (*fGeomCam)[pix].GetY();
        Float_t starXpos = star->GetMeanX();
        Float_t starYpos = star->GetMeanY();
        
	Float_t starSize = 3*star->GetSigmaMajorAxis();
        
	Float_t dist = sqrt((pixXpos-starXpos)*(pixXpos-starXpos)+
			    (pixYpos-starYpos)*(pixYpos-starYpos));

        if (dist > starSize && fDisplay.IsUsed(pix))
	  fPixelsUsed[pix]=(Char_t)kTRUE;
        else
          {
            fPixelsUsed[pix]=(Char_t)kFALSE;
            shadowPx++;
          }
    }

    if (fMinuitPrintOutLevel>=0) *fLog << dbg << " shadowPx " << shadowPx << endl;

    fDisplay.SetUsed(fPixelsUsed);

    return kTRUE;
}





