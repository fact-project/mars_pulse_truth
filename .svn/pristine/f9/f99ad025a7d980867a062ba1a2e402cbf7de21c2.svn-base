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
!   Author(s): Javier López , 4/2004 <mailto:jlopez@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
#include "MPSFFitCalc.h"

#include <iostream>


#include <TH1D.h>
#include <TString.h>
#include <TArrayS.h>
#include <TArrayI.h>
#include <TArrayD.h>
#include <TMinuit.h>
#include <TStopwatch.h>

#include "MGeomPix.h"
#include "MGeomCam.h"
#include "MMinuitInterface.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

ClassImp(MPSFFitCalc);

using namespace std;


const Int_t numVar = 5;
const Float_t pixelSize = 31.5; //[mm]
const Float_t sqrt2 = sqrt(2.);
const Float_t sqrt3 = sqrt(3.);
const Bool_t usePrintOut = kTRUE;
const Int_t minuitPrintOut = 0;

UInt_t  numPixels;
Bool_t  isPixelUsed[577];
UInt_t  numPixelsUsed;
Float_t pixelPosX[577];
Float_t pixelPosY[577];
Float_t pixelValue[577];
Float_t ChiSquare;

MPSFFitCalc::MPSFFitCalc(ImgCleanMode_t imgmode, const char *name, const char *title)
{
    fName  = name  ? name  : "MPSFFitCalc";
    fTitle = title ? title : "Task that fits the PSF using the dc readout of the camera.";

    fImgCleanMode = imgmode;
    fNumRings     = 6;
    fPedLevel     = 3.0; 


// Initialization of the camera dc mask 'fIsPixelused[]'
    numPixels = 577;
    for (UInt_t pixid=0; pixid<numPixels; pixid++)
	isPixelUsed[pixid] = kTRUE;

    fTotalMeanFit.Reset();
    fNumTotalMeanFits = 0;

    fMaxDC = 30.;

    fPedestalDCHist = new TH1D("ped","",(Int_t)fMaxDC*10,0.,fMaxDC);

    fVname = new TString[numVar];
    fVinit.Set(numVar); 
    fStep.Set(numVar); 
    fLimlo.Set(numVar); 
    fLimup.Set(numVar); 
    fFix.Set(numVar);


    fVname[0] = "max";
    fVinit[0] = fMaxDC;
    fStep[0]  = fVinit[0]/sqrt2;
    fLimlo[0] = 1.;
    fLimup[0] = 40.;
    fFix[0]   = 0;

    fVname[1] = "meanminor";
    fVinit[1] = 0.;
    fStep[1]  = fVinit[0]/sqrt2;
    fLimlo[1] = -600.;
    fLimup[1] = 600.;
    fFix[1]   = 0;

    fVname[2] = "sigmaminor";
    fVinit[2] = pixelSize;
    fStep[2]  = fVinit[0]/sqrt2;
    fLimlo[2] = pixelSize/(2*sqrt3);
    fLimup[2] = 500.;
    fFix[2]   = 0;

    fVname[3] = "meanmajor";
    fVinit[3] = 0.;
    fStep[3]  = fVinit[0]/sqrt2;
    fLimlo[3] = -600.;
    fLimup[3] = 600.;
    fFix[3]   = 0;

    fVname[4] = "sigmamajor";
    fVinit[4] = pixelSize;
    fStep[4]  = fVinit[0]/sqrt2;
    fLimlo[4] = pixelSize/(2*sqrt3);
    fLimup[4] = 500.;
    fFix[4]   = 0;

    fObjectFit  = NULL;
    //    fMethod     = "SIMPLEX";
    fMethod     = "MIGRAD";
    fNulloutput = kFALSE;
}

MPSFFitCalc::~MPSFFitCalc()
{
  delete fPedestalDCHist;
}

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
static void fcnPSF(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{

//calculate chisquare
    Double_t chisq = 0;
    Double_t delta;
    Double_t x,y,z;
    Double_t errorz = 0.2; //[uA]


    for (UInt_t pixid=1; pixid<numPixels; pixid++) 
    {

	if (isPixelUsed[pixid] && pixelValue[pixid]>0.)
	{
	    x = pixelPosX[pixid];
	    y = pixelPosY[pixid];
	    z = pixelValue[pixid];

	    if (errorz > 0.0)
	    {
		delta  = (z-func(x,y,par))/errorz;
		chisq += delta*delta;
	    }
	    else
		cerr << "ERROR fcnPSF: This should never happen errorz[" << pixid << "] " << errorz << endl;
	}
    }
    f = chisq;
    ChiSquare = chisq;

}


void MPSFFitCalc::InitFitVariables()
{

  for (UInt_t pixid=1; pixid<577; pixid++) 
    pixelValue[pixid]=(Float_t)((*fCamera)[pixid]);

  numPixelsUsed = 0;
  Float_t totalDC = 0.0;
  
    fVinit[0] = fMaxDC;

    if(usePrintOut)
      *fLog << dbg << "Pixels used in the fit \t";

    for (UInt_t idx=0; idx<numPixels; idx++)
    {
	if (isPixelUsed[idx])
	{
	    fVinit[1] += pixelPosX[idx]*pixelValue[idx];
	    fVinit[3] += pixelPosY[idx]*pixelValue[idx];
	    totalDC += pixelValue[idx];
	    numPixelsUsed++;

	    if(usePrintOut)
	      *fLog << dbg << ' ' << idx; 
	}
    }
    if(usePrintOut)
      *fLog << dbg << endl;

     
    fVinit[1] /= totalDC;
    fVinit[3] /= totalDC;
    

    fVinit[2] = pixelSize*sqrt((Float_t)numPixelsUsed)/2;
    fVinit[4] = pixelSize*sqrt((Float_t)numPixelsUsed)/3;

    //Init steps

    for(Int_t i=0; i<numVar; i++)
	if (fVinit[i] != 0)
	  fStep[i] = TMath::Abs(fVinit[i]/sqrt2);

    
    for (UInt_t pixidx=0; pixidx<numPixels; pixidx++)
      if ( (*fGeomCam)[pixidx].GetSector() == 6)
	fPedestalDCHist->Fill(pixelValue[pixidx]);
    
    fPedestalDC = fPedestalDCHist->GetBinCenter(fPedestalDCHist->GetMaximumBin());

    for (UInt_t pixid=1; pixid<577; pixid++) 
      pixelValue[pixid]-=fPedestalDC;

    if(usePrintOut)
      {
	*fLog << dbg << "numPixelsUsed \t" << numPixelsUsed << endl;
	*fLog << dbg << "fPedestalDC \t" << fPedestalDC << endl;
	*fLog << dbg << "Maximun DC Init. value \t" << fVinit[0] << endl;
	*fLog << dbg << "Mean Minor Axis Init. value \t" << fVinit[1] << " mm\t";
	*fLog << dbg << "Sigma Minor Axis Init. value \t" << fVinit[2] << endl;
	*fLog << dbg << "Mean Major Axis Init. value \t" << fVinit[3] << " mm\t";
	*fLog << dbg << "Sigma Major Axis Init. value \t" << fVinit[4] << endl;
      }
}

void MPSFFitCalc::RingImgClean()
{

  Bool_t  isPixelUsedTmp[577];

  fMaxDC=0;
  UInt_t maxDCpix[2];


// Look for the two neighbor pixels with the maximun signal and set all pixels as unused
    Float_t dc[2];
    Float_t dcsum;

// Find the two close pixels with the maximun dc
    for(UInt_t pixidx=0; pixidx<numPixels; pixidx++)
    {

      if(isPixelUsed[pixidx])
	{
	  dc[0] = (Float_t)(*fCamera)[pixidx];
	  isPixelUsedTmp[pixidx] = kFALSE;
	  
	  MGeomPix g = (*fGeomCam)[pixidx];
	  Int_t numNextNeighbors = g.GetNumNeighbors();
	  
	  for(Int_t nextNeighbor=0; nextNeighbor<numNextNeighbors; nextNeighbor++)
	    {
	      if(isPixelUsed[pixidx])
		{
		  UInt_t swneighbor = g.GetNeighbor(nextNeighbor);
		  dc[1] = (Float_t)(*fCamera)[swneighbor];
		  
		  dcsum = dc[0] + dc[1];
		  
		  if(dcsum > fMaxDC*2)
		    {
		      maxDCpix[0] = pixidx;
		      maxDCpix[1] = swneighbor;
		      fMaxDC = dcsum/2;
		    }	
		}
	    }
	}
    }

// Those variables are:
// 1. an array of 2 dimensions
//    1.1 the first dimension store the ring around the 'maxiun signal pixel'
//    1.2 the sw numbers of the pixels in this ring
// 2. an array with the number of pixels in each ring
    UInt_t isPixelUesdInRing[fNumRings][577];
    UInt_t numPixelsUsedInRing[fNumRings];

// Store the 'maximun signal pixel in the [0] ring and set it as used 

    for (Int_t core=0; core<2; core++)
    {
	isPixelUesdInRing[0][0] = maxDCpix[core];
	numPixelsUsedInRing[0] = 1;
	isPixelUsedTmp[isPixelUesdInRing[0][0]] = kTRUE;
	
	UInt_t count;
	
// Loop over the neighbors of the previus ring and store the sw numbers in the 2D array to be 
// use in the next loop/ring 
	for (UShort_t ring=0; ring<fNumRings-1; ring++)
	{
	    count = 0;  // In this variable we count the pixels we are in each ring
	    for(UInt_t nextPixelUsed=0; nextPixelUsed<numPixelsUsedInRing[ring]; nextPixelUsed++)
	    {
		MGeomPix g = (*fGeomCam)[isPixelUesdInRing[ring][nextPixelUsed]];
		Int_t numNextNeighbors = g.GetNumNeighbors();
		for(Int_t nextNeighbor=0; nextNeighbor<numNextNeighbors; nextNeighbor++)
		{
		    UInt_t swneighbor = g.GetNeighbor(nextNeighbor);
		    if (!isPixelUsedTmp[swneighbor])
		    {
			isPixelUsedTmp[swneighbor] = kTRUE;
			isPixelUesdInRing[ring+1][count] = swneighbor;
			count++;
		    }
		}
		numPixelsUsedInRing[ring+1] = count;
	    }
	}


    if(usePrintOut)
      {
	for (UInt_t row=0; row<fNumRings; row++)
	  {
	    
	    *fLog << dbg  << "fIsPixeUsed[" << row << "][" << numPixelsUsedInRing[row] << "] ";
	    for (UInt_t column=0; column<numPixelsUsedInRing[row]; column++)
	      *fLog << dbg << isPixelUesdInRing[row][column] << ' ';
	    *fLog << dbg << endl;
	  }	
      }
    }


    for(UInt_t pixidx=0; pixidx<numPixels; pixidx++)
      {
      if(isPixelUsed[pixidx] && isPixelUsedTmp[pixidx])
	  isPixelUsed[pixidx] = kTRUE;
      else
	  isPixelUsed[pixidx] = kFALSE;
      }

}

void MPSFFitCalc::RmsImgClean(Float_t pedestal)
{}

void MPSFFitCalc::MaskImgClean(TArrayS blindpixels)
{
    
    Int_t npix = blindpixels.GetSize();

    for (Int_t idx=0; idx<npix; idx++)
	isPixelUsed[blindpixels[idx]] = kFALSE;

}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MCameraDC
//
// The following output containers are also searched and created if
// they were not found:
//
//  - MPSFFit
//
Int_t MPSFFitCalc::PreProcess(MParList *pList)
{

    fGeomCam = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));

    if (!fGeomCam)
    {
      *fLog << err << AddSerialNumber("MGeomCam") << " not found ... aborting" << endl;
      return kFALSE;
    }

// Initialize positions of the pixels to be used in the minuit minimizations

    for (UInt_t pixid=1; pixid<577; pixid++) 
    {
	MGeomPix &pix=(*fGeomCam)[pixid];
	pixelPosX[pixid] = pix.GetX();
	pixelPosY[pixid] = pix.GetY();
    }
   
    fCamera = (MCameraDC*)pList->FindObject(AddSerialNumber("MCameraDC"));

    if (!fCamera)
    {
      *fLog << err << AddSerialNumber("MCameraDC") << " not found ... aborting" << endl;
      return kFALSE;
    }

    fFit = (MPSFFit*)pList->FindCreateObj(AddSerialNumber("MPSFFit"));
    if (!fFit)
    {
      *fLog << err << AddSerialNumber("MPSFFit") << " cannot be created ... aborting" << endl;
      return kFALSE;
    }
    
    // Minuit initialization

    TMinuit *gMinuit = new TMinuit(6);  //initialize TMinuit with a maximum of 5 params
    gMinuit->SetFCN(fcnPSF);

    Double_t arglist[10];
    Int_t ierflg = 0;

    arglist[0] = 1;
    gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
    arglist[0] = minuitPrintOut;
    gMinuit->mnexcm("SET PRI", arglist ,1,ierflg);

    return kTRUE;

}

Int_t MPSFFitCalc::Process()
{

    // ------------------------------------------
    // image cleaning

    switch(fImgCleanMode)
    {
	case kRing:
	{
	    RingImgClean();
	    break;
	}
	case kRms:
	{
	    RmsImgClean(fPedLevel);
	    break;
	}
	case kMask:
	{
	    MaskImgClean(fBlindPixels);
	    break;
	}
	case kCombined:
	{
	    MaskImgClean(fBlindPixels);
	    RingImgClean();
	    MaskImgClean(fBlindPixels);
	    break;
	}
	default:
	{
	    *fLog << err << "Image Cleaning mode " << fImgCleanMode << " not defined" << endl;
	    return kFALSE;
	}
    }

    InitFitVariables();

    // -------------------------------------------
    // call MINUIT

    Double_t arglist[10];
    Int_t ierflg = 0;

    for (Int_t i=0; i<numVar; i++)
      gMinuit->mnparm(i, fVname[i], fVinit[i], fStep[i], fLimlo[i], fLimup[i], ierflg);

    TStopwatch clock;
    clock.Start();

// Now ready for minimization step
    arglist[0] = 500;
    arglist[1] = 1.;
    gMinuit->mnexcm(fMethod, arglist ,2,ierflg);

    clock.Stop();

    if(usePrintOut)
      {
	*fLog << dbg << "Time spent for the minimization in MINUIT :   " << endl;;
	clock.Print();
      }

    if (ierflg)
    {
	*fLog << err << "MMinuitInterface::CallMinuit error " << ierflg << endl;
	return kCONTINUE;
    }
    
    fNumTotalMeanFits++;

    Double_t parm,parmerr;
    
    gMinuit->GetParameter(0,parm,parmerr);
    fFit->SetMaximun(parm);
    fTotalMeanFit.SetMaximun(fTotalMeanFit.GetMaximun()+parm);

    gMinuit->GetParameter(1,parm,parmerr);
    fFit->SetMeanMinorAxis(parm);
    fTotalMeanFit.SetMeanMinorAxis(fTotalMeanFit.GetMeanMinorAxis()+parm);

    gMinuit->GetParameter(2,parm,parmerr);
    fFit->SetSigmaMinorAxis(parm);
    fTotalMeanFit.SetSigmaMinorAxis(fTotalMeanFit.GetSigmaMinorAxis()+parm);

    gMinuit->GetParameter(3,parm,parmerr);
    fFit->SetMeanMajorAxis(parm);
    fTotalMeanFit.SetMeanMajorAxis(fTotalMeanFit.GetMeanMajorAxis()+parm);

    gMinuit->GetParameter(4,parm,parmerr);
    fFit->SetSigmaMajorAxis(parm);
    fTotalMeanFit.SetSigmaMajorAxis(fTotalMeanFit.GetSigmaMajorAxis()+parm);

    fFit->SetChisquare(ChiSquare/(numPixelsUsed-numVar));
    fTotalMeanFit.SetChisquare(fTotalMeanFit.GetChisquare()+ChiSquare/(numPixelsUsed-numVar));

    if(usePrintOut)
      {
	fFit->Print();
	fTotalMeanFit.Print();
      }

    return kTRUE;
}

Int_t MPSFFitCalc::PostProcess()
{
  
  fTotalMeanFit.SetMaximun(fTotalMeanFit.GetMaximun()/fNumTotalMeanFits);
  fTotalMeanFit.SetMeanMinorAxis(fTotalMeanFit.GetMeanMinorAxis()/fNumTotalMeanFits);
  fTotalMeanFit.SetSigmaMinorAxis(fTotalMeanFit.GetSigmaMinorAxis()/fNumTotalMeanFits);
  fTotalMeanFit.SetMeanMajorAxis(fTotalMeanFit.GetMeanMajorAxis()/fNumTotalMeanFits);
  fTotalMeanFit.SetSigmaMajorAxis(fTotalMeanFit.GetSigmaMajorAxis()/fNumTotalMeanFits);
  fTotalMeanFit.SetChisquare(fTotalMeanFit.GetChisquare()/fNumTotalMeanFits);

  fFit->SetMaximun(fTotalMeanFit.GetMaximun());
  fFit->SetMeanMinorAxis(fTotalMeanFit.GetMeanMinorAxis());
  fFit->SetSigmaMinorAxis(fTotalMeanFit.GetSigmaMinorAxis());
  fFit->SetMeanMajorAxis(fTotalMeanFit.GetMeanMajorAxis());
  fFit->SetSigmaMajorAxis(fTotalMeanFit.GetSigmaMajorAxis());
  fFit->SetChisquare(fTotalMeanFit.GetChisquare());

    if(usePrintOut)
      fTotalMeanFit.Print();
    
  return kTRUE;
}
