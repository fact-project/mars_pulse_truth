/* ======================================================================== *\
!
!
!   Author(s): Ester Aliu, 3/2004
!  
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MIslandClean
//
// The Island Cleaning task selects the islands you use for the Hillas
// parameters calculation, after the normal image cleaning.
//
// There are two methods to make the selection: 
//
//    - No time method, as used is Whipple. It calculates an island parameter 
//   called "signal to noise" and adds a new threshold that eliminates some   
//   of the islands. The way the island is eliminated is seeting the pixels 
//   of the islands as UNUSED 
//
//    - Time method, taking profit of the time information in MAGIC.
//   Calculates the maximum time difference (in time slices) for each island 
//   and corrects for the island size. With an other new threshold "noise" 
//   islands are supposed to be eliminated.     
//
// Other cleanings that are allowed in this code are:
// 
//    - Resting only with the continent, i.e. the larger island
//    
//  Example:
//
//  MIslands   isl;
//  isl.SetName("MIslands1");

//  MImgCleanStd clean;
//  MIslandClean islclean(0.2);
//  islclean.SetInputName("MIslands1");  
//  islclean.SetMethod(0); // for timing method 
//
//  tlist.AddToList(&clean);
//  tlist.AddToList(&islclean);
//
//
//  Input Containers:
//    MGeomCam
//    MCerPhotEvt
//    MPedestalCam
//    MArrivalTime
//    MIslands
//
//  Output Containers:
//    MCerPhotEvt
//
/////////////////////////////////////////////////////////////////////////////
#include "MIslandsClean.h"

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

ClassImp(MIslandsClean);


using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MIslandsClean::MIslandsClean(const Float_t newThres, const char *name, const char *title)    
  : fIsl(NULL), fIslandCleaningMethod(kNoTiming), fIslCleanThres(newThres)
{
    fName  = name  ? name  : "MIslandsClean";
    fTitle = title ? title : "Clean islands";
}


Int_t MIslandsClean::PreProcess (MParList *pList)
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
      fIsl = (MIslands*)pList->FindObject(AddSerialNumber(fIslName));
    else
      fIsl = (MIslands*)pList->FindObject(AddSerialNumber("MIslands"));
    if (!fIsl)
      {
        *fLog << dbginf << "MIslands not found... aborting." << endl;
        return kFALSE;
      }
    
    return kTRUE;
}



Int_t MIslandsClean::Process()
{
  
  MImgIsland *imgIsl = new MImgIsland;
  TIter Next(fIsl->GetList());

  Int_t pixNum = 0;  
  Int_t idPix = -1;

  ////////////////////////////////////////////////////
  //
  //       TIME SPREAD ISLAND CLEANING
  //  eliminates the island with a time spread 
  //         higher than a given limit 
  /////////////////////////////////////////////////// 
  if( fIslandCleaningMethod == 0 ){
    while ((imgIsl=(MImgIsland*)Next())) {
      
      //fIslCleanThreshold has different values, FIXME, put two variables
      if(imgIsl->GetTimeSpread() > fIslCleanThres)
	{
	  pixNum = imgIsl->GetPixNum();
	  
	  for(Int_t k = 0; k<pixNum; k++)
	    {
	      idPix = imgIsl->GetPixList(k);
	      MCerPhotPix &pix  = (*fEvt)[idPix];
	      pix.SetPixelUnused();
	    }
	}
    }
    
  }
  

  ////////////////////////////////////////////////////
  //
  //      SIGNAL TO NOISE ISLAND CLEANING
  //  eliminates the island with a signal-to-noise 
  //         lower than a given limit 
  /////////////////////////////////////////////////// 
  else if ( fIslandCleaningMethod == 1 ) {
    while ((imgIsl=(MImgIsland*)Next())) {
      
      if(imgIsl->GetSigToNoise() < fIslCleanThres)
	{
	  pixNum = imgIsl->GetPixNum();
	  
	  for(Int_t k = 0; k<pixNum; k++)
	    {
	      idPix = imgIsl->GetPixList(k);
	      MCerPhotPix &pix  = (*fEvt)[idPix];
	      pix.SetPixelUnused();
	    }
	}
    }	
  }  
  

  ////////////////////////////////////////////////////
  //
  //      ISLAND SIZE OVER EVENT SIZE ISLAND CLEANING
  //  eliminates the island with an island size over event size  
  //         lower than a given limit 
  /////////////////////////////////////////////////// 
  else if ( fIslandCleaningMethod == 2 ) {
    Float_t size = 0;
    while ((imgIsl=(MImgIsland*)Next())) {
      size += imgIsl->GetSizeIsl();
    }
    
    Next.Reset();
    while ((imgIsl=(MImgIsland*)Next())) {
      
      if(imgIsl->GetSizeIsl()/size < fIslCleanThres)
	{
	  pixNum = imgIsl->GetPixNum();
	  
	  for(Int_t k = 0; k<pixNum; k++)
	    {
	      idPix = imgIsl->GetPixList(k);
	      MCerPhotPix &pix  = (*fEvt)[idPix];
	      pix.SetPixelUnused();
	    }
	}
    }	
  }  
  
  
  ////////////////////////////////////////////////////
  //
  //       CONTINENT ISLAND CLEANING
  //  eliminates all the islands except the continent 
  //      i.e. the larger island in the event
  //     according the number of pixels
  /////////////////////////////////////////////////// 
  else if( fIslandCleaningMethod == 3 ){
    Int_t i = 0;
    while ((imgIsl=(MImgIsland*)Next())) {
      if (i != 0){
	
	pixNum = imgIsl->GetPixNum();
	
	for(Int_t k = 0; k<pixNum; k++)
	  {
	    idPix = imgIsl->GetPixList(k);
	    MCerPhotPix &pix  = (*fEvt)[idPix];
	    pix.SetPixelUnused();
	  }
      }
      i++;   
    }
  }
  

  ////////////////////////////////////////////////////
  //
  //       LARGER and SECOND LARGER ISLAND CLEANING
  // eliminates all the islands except the two biggest 
  //               ones according size
  //                
  /////////////////////////////////////////////////// 
  else if( fIslandCleaningMethod == 4 ){

    Int_t i = 0;
    Int_t islnum = fIsl->GetIslNum();
    Float_t islSize[islnum]; 
    Int_t islIdx[islnum]; 

    for (Int_t j = 0; j<islnum ; j++){
      islSize[j] = -1;
      islIdx[j] = -1;
    }
    
    while ((imgIsl=(MImgIsland*)Next())) {
      
      islSize[i] = imgIsl->GetSizeIsl();
      i++;
    }
 
    
    TMath::Sort(islnum, islSize, islIdx, kTRUE);
    
    i = 0;
    Next.Reset();
    while ((imgIsl=(MImgIsland*)Next())) {
      if (islnum > 1 && islIdx[0]!=i && islIdx[1]!=i){
	
	//cout <<  "removed " << i << " isl 0" << islIdx[0] << " isl 1" << islIdx[1] << endl;
	  
	pixNum = imgIsl->GetPixNum();
	
	for(Int_t k = 0; k<pixNum; k++)
	  {
	    idPix = imgIsl->GetPixList(k);
	    MCerPhotPix &pix  = (*fEvt)[idPix];
	    pix.SetPixelUnused();
	  }
      }
      i++;   
    }  
  }



  ///////////////////////////////////////////////////////
  //
  //       LARGER and SECOND LARGER ISLAND CLEANING II
  // eliminates all the islands except the two biggest 
  // ones according size, if the second one almost has 
  // the 80% of the size of the biggest one
  //
  //                
  ////////////////////////////////////////////////////// 
  else if( fIslandCleaningMethod == 5 ){

    Int_t i = 0;
    Int_t islnum = fIsl->GetIslNum();
    Float_t islSize[islnum]; 
    Int_t islIdx[islnum]; 

    for (Int_t j = 0; j<islnum ; j++){
      islSize[j] = -1;
      islIdx[j] = -1;
    }
    
    while ((imgIsl=(MImgIsland*)Next())) {
      
      islSize[i] = imgIsl->GetSizeIsl();
      i++;
    }
    
    TMath::Sort(islnum, islSize, islIdx, kTRUE);
    
    i = 0;
    Next.Reset();

    while ((imgIsl=(MImgIsland*)Next())) {

      if (i!=0 && islIdx[0]!=i && islIdx[1]!=i){
	  
	pixNum = imgIsl->GetPixNum();
	
	for(Int_t k = 0; k<pixNum; k++)
	  {
	    idPix = imgIsl->GetPixList(k);
	    MCerPhotPix &pix  = (*fEvt)[idPix];
	    pix.SetPixelUnused();
	  }
      }
      else if(i!=0 && islSize[islIdx[i]]<0.6*islSize[islIdx[0]]){
	
	pixNum = imgIsl->GetPixNum();
      
	for(Int_t k = 0; k<pixNum; k++)
	  {
	    idPix = imgIsl->GetPixList(k);
	    MCerPhotPix &pix  = (*fEvt)[idPix];
	    pix.SetPixelUnused();
	  }
      }
      i++;   
    }  
  }
  
  fEvt->SetReadyToSave();
  
  return kTRUE;
  
}
