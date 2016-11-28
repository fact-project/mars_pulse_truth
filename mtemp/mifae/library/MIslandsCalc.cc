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
!   Author(s): Ester Aliu, 2/2004 <aliu@ifae.es>
|
!   Last Update: 7/2004
!
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MIslandsCalc
//
// The Island Calc task calculates some islands parameters for each 
// of the events such as:
// 
//   - fPixNum                 //  number of pixels in the island
//   - fSigToNoise             //  signal to noise of the island
//   - fTimeSpread             //  "time" of the island
//   - fMeanX                  //  mean X position of the island
//   - fMeanY                  //  mean Y position of the island
//   - fDist                   //  dist between an island and the continent
//   - fLength                 //  major axis of the island ellipse
//   - fWidth                  //  minor axis of the island ellipse
//   - fDistL                  //  dist divided by lenght of the larger island
//   - fDistW                  //  dist divided by width of the larger island
//   - fDistS                  //  dist divided by size of the larger island
//
//   - fPixList                //  list of pixels in the island (TArrayI)
//   - fPeakPulse              //  mean arrival time of the pixels in the island (TArrayF)
//
// Input Containers:
//   MGeomCam
//   MCerPhotEvt
//   MPedestalCam
//   MArrivalTimeCam
//
// Output Containers:
//   MIslands
//   MImgIsland
//
/////////////////////////////////////////////////////////////////////////////
#include "MIslandsCalc.h"

#include <stdlib.h>       // atof					  
#include <fstream>        // ofstream, SavePrimitive

#include "MLog.h"
#include "MLogManip.h"

#include "MIslands.h"
#include "MImgIsland.h"

#include "MParList.h"

#include "MGeomPix.h"
#include "MGeomCam.h"

#include "MCerPhotPix.h"
#include "MCerPhotEvt.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MArrivalTimeCam.h"
#include "MArrivalTimePix.h"

ClassImp(MIslandsCalc);


using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MIslandsCalc::MIslandsCalc(const char* name, const char* title)    
  : fIsl(NULL)
{
    fName  = name  ? name  : "MIslandsCalc";
    fTitle = title ? title : "Calculate island parameters";
}


// --------------------------------------------------------------------------
Int_t MIslandsCalc::PreProcess (MParList *pList)
{
    fCam = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
    if (!fCam)
    {
        *fLog << dbginf << "MGeomCam not found (no geometry information available)... aborting." << endl;
        return kFALSE;
    }

    fEvt = (MCerPhotEvt*)pList->FindObject(AddSerialNumber("MCerPhotEvt"));
    if (!fEvt)
    {
        *fLog << dbginf << "MCerPhotEvt not found... aborting." << endl;
        return kFALSE;
    }

    fPed = (MPedestalCam*)pList->FindObject(AddSerialNumber("MPedestalCam"));
    if (!fPed)
      {
        *fLog << dbginf << "MPedestalCam not found... aborting." << endl;
        return kFALSE;
      }

    fTime = (MArrivalTimeCam*)pList->FindObject(AddSerialNumber("MArrivalTimeCam"));
    if (!fTime)
      {
        *fLog << dbginf << "MArrivalTimeCam not found... aborting." << endl;
        return kFALSE;
      }
   
    if (strlen(fIslName) > 0)
      {
	fIsl = (MIslands*)pList->FindCreateObj("MIslands", AddSerialNumber(fIslName));
      }
    else
      {
      fIsl = (MIslands*)pList->FindCreateObj(AddSerialNumber("MIslands"));
      }
    if (!fIsl)
      return kFALSE;
    
    return kTRUE;
}


Int_t MIslandsCalc::Process(){
 
  fIsl->GetList()->Delete();
  IslandPar();
  
  return kTRUE;     
}


Int_t MIslandsCalc::IslandPar(){

  //calculates all the island parameters 

  const Int_t nPix=fCam->GetNumPixels();
  const Int_t nVect=50;
  Int_t numisl;

  Int_t** vect;
  vect = new Int_t*[nVect];
  for(Int_t i=0;i<nVect;i++)
    vect[i]= new Int_t[nPix];
  
  Int_t* num;
  num = new Int_t[nVect];
  
  if (fIslandAlgorithm == 1)
    Calc1(numisl,nVect,nPix,vect,num);
  if (fIslandAlgorithm == 2)
    Calc2(numisl,nVect,nPix,vect,num);
  
  //set the number of islands in one event
  fIsl->SetIslNum(numisl);

  //cout << "code numisl " << numisl << endl;
  //examine each island...

  Float_t  noise;
  Float_t  signal;
 
  Int_t PixelNumIsl[numisl];
  Float_t SigToNoise[numisl];
  Float_t time[nPix];
  Float_t timeVariance[numisl];
  Float_t meanX[numisl];
  Float_t meanY[numisl];
  Float_t length[numisl];
  Float_t width[numisl];
  Float_t dist[numisl];
  Float_t distL[numisl];
  Float_t distW[numisl];
  Float_t distS[numisl];


  Float_t size[numisl], sizeLargeIsl, distance, alpha, alphaW, sizetot;

  sizeLargeIsl = 0;
  alphaW = 0;
  sizetot = 0;

  for(Int_t i = 1; i<=numisl ; i++)
    {
     
      MImgIsland *imgIsl = new MImgIsland;
      
      imgIsl->InitSize(num[i]);
      
      Int_t n = 0;
      
      Float_t minTime = 10000.;
      Float_t maxTime = 0.;

      Float_t minX = 10000.;
      Float_t maxX = 0.;

      Float_t minY = 10000.;
      Float_t maxY = 0.;

      signal = 0;
      noise = 0;
      
      size[i-1] = 0;
      meanX[i-1] = 0;
      meanY[i-1] = 0;
      dist[i-1] = 0;
      
      PixelNumIsl[i-1] = 0;
      timeVariance[i-1] = 0;
           
      for(Int_t idx=0 ; idx<nPix ; idx++)
	{
	  MCerPhotPix *pix = fEvt->GetPixById(idx);
	  if(!pix) continue;
	  const MGeomPix &gpix2 = (*fCam)[pix->GetPixId()];
	  const MPedestalPix &ped  = (*fPed)[idx];
	  const MArrivalTimePix &timepix = (*fTime)[idx];
	  const Float_t nphot = pix->GetNumPhotons();

	  if (vect[i][idx]==1){
	    
	    PixelNumIsl[i-1]++;
	    signal += nphot * (fCam->GetPixRatio(idx));
	    noise += pow(ped.GetPedestalRms(),2);
	    
	    size[i-1] += nphot;
	    if (i == 1)
	      sizeLargeIsl += nphot;
	    
	    meanX[i-1] += nphot * gpix2.GetX();
	    meanY[i-1] += nphot * gpix2.GetY();
	    
	    time[i-1] = timepix.IsLoGainUsed() ? timepix.GetArrivalTimeLoGain() : timepix.GetArrivalTimeHiGain();
	    
	    imgIsl->SetPixList(PixelNumIsl[i-1]-1,pix->GetPixId());
	    imgIsl->SetPeakPulse(PixelNumIsl[i-1]-1,time[i-1]);     	  	
	    
	    //calculates the time spread only for core pixels  
	    if (fEvt->IsPixelCore(idx)){ 
	      
	      if (time[i-1] > maxTime){
		maxTime = time[i-1];
		maxX = gpix2.GetX();
		maxY = gpix2.GetY();
	      }
	      if (time[i-1] < minTime){
		minTime = time[i-1];
		minX = gpix2.GetX();
		minY = gpix2.GetY();
	      }
	      
	    }
	    n++;
	  }	  
	}  
      
      meanX[i-1] /= size[i-1];
      meanY[i-1] /= size[i-1];

      dist[i-1] = TMath::Power(meanX[i-1]-meanX[0],2) + TMath::Power(meanY[i-1]-meanY[i-1],2);
      dist[i-1] = TMath::Sqrt(dist[i-1]);
      
      //timeVariance[i-1] = (maxTime-minTime);
      
      if (maxX!=minX && maxY!=minY)
	timeVariance[i-1] = (maxTime-minTime)/sqrt(TMath::Power(maxX-minX,2) + TMath::Power(maxY-minY,2)); 
      else
	timeVariance[i-1] = -1;
      
      //noise = 0, in the case of MC w/o noise
      if (noise == 0) noise = 1;
      
      SigToNoise[i-1]= (Float_t)signal/(Float_t)sqrt(noise);
      
      imgIsl->SetPixNum(PixelNumIsl[i-1]);
      imgIsl->SetSigToNoise(SigToNoise[i-1]);
      imgIsl->SetTimeSpread(timeVariance[i-1]);
      imgIsl->SetMeanX(meanX[i-1]);
      imgIsl->SetMeanY(meanY[i-1]);
      imgIsl->SetDist(dist[i-1]);
      imgIsl->SetSizeIsl(size[i-1]);
      

      // sanity check: if one island has 2 or less pixels
      if (num[i]>2){
      

      // calculate width and lenght of each island

      Double_t corrxx=0;              // [m^2]
      Double_t corrxy=0;              // [m^2]
      Double_t corryy=0;              // [m^2]
      
      for(Int_t idx=0 ; idx<nPix ; idx++){
	
	MCerPhotPix *pix = fEvt->GetPixById(idx);
	if(!pix) continue;
	const MGeomPix &gpix3 = (*fCam)[pix->GetPixId()];
	const Float_t nphot = pix->GetNumPhotons();
	
	if (vect[i][idx]==1){
	  
	  const Float_t dx = gpix3.GetX() - meanX[i-1];     // [mm]
	  const Float_t dy = gpix3.GetY() - meanY[i-1];     // [mm]
	  
	  corrxx += nphot * dx*dx;                     // [mm^2]
	  corrxy += nphot * dx*dy;                     // [mm^2]
	  corryy += nphot * dy*dy;                     // [mm^2]	       
	  
	}
      }
   
      const Double_t d0    = corryy - corrxx;
      const Double_t d1    = corrxy*2;
      const Double_t d2    = d0 + TMath::Sqrt(d0*d0 + d1*d1);
      const Double_t tand  = d2 / d1;
      const Double_t tand2 = tand*tand;
      
      const Double_t s2 = tand2+1;
      const Double_t s  = TMath::Sqrt(s2);
      
      const Double_t CosDelta =  1.0/s;   // need these in derived classes
      const Double_t SinDelta = tand/s;   // like MHillasExt
      
      const Double_t axis1 = (tand2*corryy + d2 + corrxx)/s2/size[i-1];
      const Double_t axis2 = (tand2*corrxx - d2 + corryy)/s2/size[i-1];
      
      //
      // fLength^2 is the second moment along the major axis of the ellipse
      // fWidth^2  is the second moment along the minor axis of the ellipse
      //
      // From the algorithm we get: fWidth <= fLength is always true
      //
      // very small numbers can get negative by rounding
      //
      length[i-1] = axis1<0 ? 0 : TMath::Sqrt(axis1);  // [mm]
      width[i-1]  = axis2<0 ? 0 : TMath::Sqrt(axis2);  // [mm]
      
      
      // alpha calculation
      
      const Double_t mx = meanX[i-1];     // [mm]
      const Double_t my = meanY[i-1];     // [mm]
      
      //FIXME: xpos, ypos from MSrcPos
      const Double_t xpos = 0.;
      const Double_t ypos = 0.;
      
      const Double_t sx = mx - xpos;   // [mm]
      const Double_t sy = my - ypos;   // [mm]
      
      const Double_t sd = SinDelta;  // [1]
      const Double_t cd = CosDelta;  // [1]
      
      //
      // Distance from source position to center of ellipse.
      // If the distance is 0 distance, Alpha is not specified.
      // The calculation has failed and returnes kFALSE.
      //
      distance = TMath::Sqrt(sx*sx + sy*sy);  // [mm]
      if (distance==0){
	
	for (Int_t l = 0; l< nVect; l++)
	  delete [] vect[l]; 
	
	delete [] vect;
	delete [] num;
	
	return 1;
      }
      
      //
      // Calculate Alpha and Cosda = cos(d,a)
      // The sign of Cosda will be used for quantities containing
      // a head-tail information
      //
      // *OLD* const Double_t arg = (sy-tand*sx) / (dist*sqrt(tand*tand+1));
      // *OLD* fAlpha = asin(arg)*kRad2Deg;
      //
      const Double_t arg2 = cd*sx + sd*sy;          // [mm]
      if (arg2==0){
	
	for (Int_t l = 0; l< nVect; l++)
	  delete [] vect[l]; 
	
	delete [] vect;
	delete [] num;
	
	return 2;
      }

      const Double_t arg1 = cd*sy - sd*sx;          // [mm]
      
      //
      // Due to numerical uncertanties in the calculation of the
      // square root (dist) and arg1 it can happen (in less than 1e-5 cases)
      // that the absolute value of arg exceeds 1. Because this uncertainty
      // results in an Delta Alpha which is still less than 1e-3 we don't care
      // about this uncertainty in general and simply set values which exceed
      // to 1 saving its sign.
      //
      const Double_t arg = arg1/distance;
      alpha = TMath::Abs(arg)>1 ? TMath::Sign(90., arg) : TMath::ASin(arg)*TMath::RadToDeg(); // [deg]
      
      alphaW += alpha*size[i-1];
      sizetot += size[i-1];
      
      ////////////////////////////////////////
      
      distL[i-1]=dist[i-1]/length[0];
      distW[i-1]=dist[i-1]/width[0];
      distS[i-1]= dist[i-1]/size[0];
      
      imgIsl->SetLength(length[i-1]);
      imgIsl->SetWidth(width[i-1]);
      
      imgIsl->SetDistL(distL[i-1]);
      imgIsl->SetDistW(distW[i-1]);
      imgIsl->SetDistS(distS[i-1]);
      
      imgIsl->SetAlpha(alpha);

      }

      fIsl->GetList()->Add(imgIsl);
      
    }
  
  fIsl->SetAlphaW(alphaW/sizetot);   
  //fIsl->SetReadyToSave();
  
  for (Int_t l = 0; l< nVect; l++)
    delete [] vect[l]; 

 delete [] vect;
 delete [] num;

  return kTRUE;  
}

//------------------------------------------------------------------------------------------
void MIslandsCalc::Calc1(Int_t& numisl, const Int_t nv, const Int_t npix, Int_t** vect, Int_t* num){
  
  
  /////////////////////////////
  //
  //        ALGORITHM # 1
  // counts the number of islands as you can see in 
  // the event display after doing the std image cleaning
  //
  /////////////////////////////
  
  Int_t    sflag;
  Int_t    control = 0;
  
  Int_t    nvect = 0;
  
  numisl = 0;

  Int_t    zeros[nv];
  
  for(Int_t m = 0; m < nv ; m++)
    for(Int_t n = 0; n < npix ; n++)
	vect[m][n] = 0;
    
  for(Int_t n = 0; n < nv ; n++)
    zeros[n] = 0;
  
  //cout << "new event" <<endl;
  MCerPhotPix *pix;

  // Loop over used pixels only
  TIter Next(*fEvt);
  
  //after the interpolation of the pixels, these can be disordered by index. This is important for this algorithm of calculating islands
  fEvt->Sort();

  while ((pix=static_cast<MCerPhotPix*>(Next())))
    {
      const Int_t idx = pix->GetPixId();

      const MGeomPix &gpix  = (*fCam)[idx];
      const Int_t    nnmax  = gpix.GetNumNeighbors();

      if( fEvt->IsPixelUsed(idx)) 
     	{
	  //cout <<idx <<endl;
	  sflag = 0;
	  
	  for(Int_t j=0; j < nnmax ; j++)
	    {
	      const Int_t idx2 = gpix.GetNeighbor(j);
	      
	      if (idx2 < idx)
		{
		  for(Int_t k = 1; k <= nvect; k++)
		    {
		      if (vect[k][idx2] == 1)
			{
			  sflag = 1;
			  vect[k][idx] = 1;
			}
		    }
		}
	    }
	  
	  if (sflag == 0)
	    {
	      nvect++;
	      vect[nvect][idx] = 1;	     
	    }
	  
	}
    }
  
  numisl = nvect;
  
    
  // Repeated Chain Corrections

  Int_t jmin = 0;
  
  for(Int_t i = 1; i <= nvect; i++){
    control=0;
    for(Int_t j = i+1; j <= nvect; j++){
      control = 0;
      for(Int_t k = 0; k < npix; k++){
	if (vect[i][k] == 1 && vect[j][k] == 1){
	  control = 1;
	  k=npix;
	}
      }
      if (control == 1){
	for(Int_t k = 0; k < npix; k++){
	  if(vect[j][k] == 1) vect[i][k] = 1;
	  vect[j][k] = 0;
	  zeros[j] = 1;
	}	
	numisl = numisl-1;	    
      }
    }
    
    for(Int_t j = 1; j <= i-1; j++){
      for(Int_t k = 0; k < npix; k++){
	if (vect[i][k] == 1 && vect[j][k] == 1){
	  control = 2; 
	  jmin=j;
	  k=npix;
	  j=i;
	}
      }
      
      if (control == 2){
	for (Int_t k = 0; k < npix; k++){
	  if(vect[i][k]==1) vect[jmin][k]=1;
	  vect[i][k] = 0;
	  zeros[i] = 1;
	}
	numisl = numisl-1;	    
      }	   
    } 
  }
  
  Int_t pixMAX = 0;
  Int_t idMAX = 1;
  Int_t l = 1;
  Int_t numpixels;
  
  for(Int_t i = 1;  i<= nvect ; i++)
    {
      numpixels = 0;

      if (zeros[i] == 0)
	{
	  for(Int_t k=0; k<npix; k++)
	    {
	      vect[l][k] = vect[i][k];
	      if (vect[l][k] == 1)
		numpixels++;      		
	    }

	  num[l] = numpixels;      	

	  if (numpixels>pixMAX)
	    {
	      pixMAX = numpixels;
	      idMAX = l;
	    }
	  l++;
	}
    }


  //the larger island will correspond to the 1st component of the vector
  
  num[nvect+1] = num[1];
  num[1] = num[idMAX];
  num[idMAX] = num[nvect+1];

  
  for(Int_t k = 0; k<npix; k++) 
    {
      vect[nvect+1][k] = vect[1][k];
      vect[1][k] = vect[idMAX][k];
      vect[idMAX][k] = vect[nvect+1][k];
    }
}

//------------------------------------------------------------------------------------------

void MIslandsCalc::Calc2(Int_t& numisl, const Int_t nv, const Int_t npix, Int_t** vect, Int_t* num){  

  
  /////////////////////////////
  //
  //        ALGORITHM # 2
  // counts the number of islands considering as the same 
  // islands the ones separated for 2 or less pixels
  //
  /////////////////////////////
  
  Int_t    sflag;
  Int_t    control;
  
  Int_t    nvect = 0;
  numisl = 0;
 
  Int_t    zeros[nv];
  
  Int_t kk[npix];

  for(Int_t m = 0; m < nv ; m++)
    for(Int_t n = 0; n < npix ; n++)
	vect[m][n] = 0;
    
  for(Int_t n = 0; n < nv ; n++)
    zeros[n] = 0;
  
  for(Int_t n = 0; n < npix ; n++)
    kk[n] = 0;
  
  MCerPhotPix *pix;

  //after the interpolation of the pixels, these can be disordered by index. This is important for this algorithm of calculating islands
  fEvt->Sort();

  // 1st loop over used pixels only
  TIter Next0(*fEvt);
 
  while ((pix=static_cast<MCerPhotPix*>(Next0())))
    {
      const Int_t idx = pix->GetPixId();
      
      const MGeomPix &gpix  = (*fCam)[idx]; 
      const Int_t    nnmax  = gpix.GetNumNeighbors();

      if( fEvt->IsPixelUsed(idx))
	{
	  kk[idx] = 1 ;
	  for(Int_t j=0; j< nnmax; j++)
	    {
	      kk[gpix.GetNeighbor(j)] = 1;
	    }
	} 
      
    }
      
 
  //2nd loop over all pixels
  TIter Next(*fEvt);
   
  while ((pix=static_cast<MCerPhotPix*>(Next())))
    {
      const Int_t idx = pix->GetPixId();
      
      const MGeomPix &gpix  = (*fCam)[idx];
      const Int_t    nnmax  = gpix.GetNumNeighbors();
      
      if ( kk[idx] > 0)
     	{
	  sflag = 0;
	  
	  for(Int_t j=0; j < nnmax ; j++)
	    {
	      const Int_t idx2 = gpix.GetNeighbor(j);
	      
	      if (idx2 < idx)
		{
		  for(Int_t k = 1; k <= nvect; k++)
		    {
		      if (vect[k][idx2] == 1)
			{
			  sflag = 1;
			  vect[k][idx] = 1;
			}
		    }
		}
	    }
	  
	  if (sflag == 0)
	    {
	      nvect++;
	      vect[nvect][idx] = 1;	     
	    }
	  
	}
    }
  
  numisl = nvect;
  
 // Repeated Chain Corrections

  Int_t jmin = 0;
  
  for(Int_t i = 1; i <= nvect; i++){
    control=0;
    for(Int_t j = i+1; j <= nvect; j++){
      control = 0;
      for(Int_t k = 0; k < npix; k++){
	if (vect[i][k] == 1 && vect[j][k] == 1){
	  control = 1;
	  k=npix;
	}
      }
      if (control == 1){
	for(Int_t k = 0; k < npix; k++){
	  if(vect[j][k] == 1) vect[i][k] = 1;
	  vect[j][k] = 0;
	  zeros[j] = 1;
	}	
	numisl = numisl-1;	    
      }
    }
    
    for(Int_t j = 1; j <= i-1; j++){
      for(Int_t k = 0; k < npix; k++){
	if (vect[i][k] == 1 && vect[j][k] == 1){
	  control = 2; 
	  jmin=j;
	  k=npix;
	  j=i;
	}
      }
      
      if (control == 2){
	for (Int_t k = 0; k < npix; k++){
	  if(vect[i][k]==1) vect[jmin][k]=1;
	  vect[i][k] = 0;
	  zeros[i] = 1;
	}
	numisl = numisl-1;	    
      }	   
    } 
  } 
  
  Int_t l = 1;
  Int_t numpixels;
  Int_t pixMAX = 0;
  Int_t idMAX = 1;

  for(Int_t i = 1;  i<= nvect ; i++)
    {
      numpixels = 0;

      if (zeros[i] == 0)
	{
	  for(Int_t k = 0; k<npix; k++)
	    {
	      vect[l][k] = vect[i][k];
	      if (vect[l][k] == 1)
		numpixels++;
	    }

	  num[l] = numpixels;
	  
	  if (numpixels>pixMAX)
	    {
	      pixMAX = numpixels;
	      idMAX = l;
	    }
	  l++;
	}
    }
  
  
  //the larger island will correspond to the 1st component of the vector

  num[nvect +1] = num[1];
  num[1] = num[idMAX];
  num[idMAX]=num[nvect+1];

  for(Int_t k = 0; k<npix; k++) 
    {
      vect[nvect+1][k] = vect[1][k];
      vect[1][k] = vect[idMAX][k];
      vect[idMAX][k] = vect[nvect+1][k];
    }

}

