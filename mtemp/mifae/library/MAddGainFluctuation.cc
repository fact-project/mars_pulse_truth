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
!   Author(s): Oscar Blanch Bigas 01/2005  <mailto:blanch@ifae.es>
!
!   Copyright: MAGIC Software Development, 2002-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MAddGainFluctuation
//
//   This is a task which add gain fluctuatins
//
//  Input Containers:
//   MCerPhotEvt
//   MGainFluctuationCam
//   MGeomCam
//
//  Output Containers:
//   MCerPhotEvt
//
//////////////////////////////////////////////////////////////////////////////
#include "MAddGainFluctuation.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"

#include "MCerPhotEvt.h"
#include "MCerPhotPix.h"
#include "MGainFluctuationCam.h"

#include "TRandom.h"

ClassImp(MAddGainFluctuation);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MAddGainFluctuation::MAddGainFluctuation(const char *name, const char *title)
{
    fName  = name  ? name  : "MAddGainFluctuation";
    fTitle = title ? title : "Add gain fluctuation";

    FillHist(1,0.1);
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MCerPhotEvt
//  - MGeomCam
//
// The following containers are also searched and created if
// they were not found:
// - MGainFluctuationCam
//
//
Int_t MAddGainFluctuation::PreProcess(MParList *pList)
{

    fGeomCam = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
    if (!fGeomCam)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    fCerPhotEvt = (MCerPhotEvt*)pList->FindObject(AddSerialNumber("MCerPhotEvt"));
    if (!fCerPhotEvt)
        return kFALSE;

    fGainFlucCam = (MGainFluctuationCam*)pList->FindCreateObj(AddSerialNumber("MGainFluctuationCam"));
    if (!fGainFlucCam)
        return kFALSE;


    Fill();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
Bool_t MAddGainFluctuation::ReInit(MParList *pList)
{
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Scale the content of MCerPhotPix by Gain fluctuaion
//
Int_t MAddGainFluctuation::Process()
{
    //fCerPhotEvt->InitSize(fRawEvt->GetNumPixels());

//    if (fIsMcFile)
//        ScalePedestals();

    if(!fAddFluctuations)
      return kTRUE;

    Double_t gain;

    MCerPhotPix *pix = 0;

    TIter Next(*fCerPhotEvt);

    while ((pix=(MCerPhotPix*)Next()))
    {
        const UInt_t idx = pix->GetPixId();

	fGainFlucCam->GetPixelContent(gain,idx,*fGeomCam,1);
	pix->Scale(1.0/gain);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Fill Histogram with Gain Fluctuations distribution in the Camera
//
//  flag:
//
//     0 : All to one
//     1 : Gaussian centered at one with sigma "sigma"
//     2 : Uniform distribution from 1/sigma to 1*sigma
void MAddGainFluctuation::FillHist(int flag, float sigma){

  TRandom rnd;
  Float_t val;

  fGainsDistribution = new TH1F("Gain","Gain Fluctuatins",1000,0.0,2.0);

  switch(flag){
  case 0:
    fAddFluctuations=0;
    break;
  case 1:
    fAddFluctuations=1;
    for(int i=0;i<10000;i++){
      val=rnd.Gaus(1.0,sigma);
      fGainsDistribution->Fill(val);
    }
    break;
  case 2:
    fAddFluctuations=1;
    for(int i=0;i<10000;i++){
      val=rnd.Uniform(1.0/sigma,1.0*sigma);
      fGainsDistribution->Fill(val);
    }
    break;
  default:
    fAddFluctuations=0;
    for(int i=0;i<10000;i++){
      val=rnd.Gaus(1.0,sigma);
      fGainsDistribution->Fill(val);
    }
    break;
  }
}

// --------------------------------------------------------------------------
//
//  Set and Fill Histogram with Gain Fluctuations distribution in the Camera
//
void MAddGainFluctuation::SetHist(int bins, float fb, float lb, float *values){

  fGainsDistribution = new TH1F("Gain","Gain Fluctuatins",bins,fb,lb);
  
  for(int i=0;i<bins;i++){
    fGainsDistribution->SetBinContent(i,values[i]);
  }

}

// --------------------------------------------------------------------------
//
//  Fill MGainFluctuatinCam 
//
void MAddGainFluctuation::Fill(){

  const Int_t pixels=fGeomCam->GetNumPixels();
  
  if(fAddFluctuations)
    for (int idx=0;idx<pixels;idx++)
      fGainFlucCam->AddPixel(idx,fGainsDistribution->GetRandom());
  else
    for (int idx=0;idx<pixels;idx++)
      fGainFlucCam->AddPixel(idx,1.0);

}

