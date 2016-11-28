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
!   Author(s): Markus Gaug  02/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MExtractedSignalBlindPixel
//
// The storage container of the extracted signal of the calibration Blind Pixel(s). 
// Additionally, the number of saturated Slices are stored. 
// There is place for various blind pixels set into the camera in  July. Default 
// is one blind pixel.
//
// Default values for the extracted signals are: gkSignalInitializer
//
/////////////////////////////////////////////////////////////////////////////
#include "MExtractedSignalBlindPixel.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MExtractedSignalBlindPixel);

using namespace std;

const Int_t MExtractedSignalBlindPixel::gkSignalInitializer = 99999;
// ------------------------------------------------------------------------
//
//
// Sets:
// - the dimenstion of fBlindPixelIdx to 1
// - the dimenstion of fExtractedSignal to 1
// - the dimenstion of fNumSaturated to 1
// - the dimenstion of fPed. to 1;      
// - the dimenstion of fPedErr. to 1;
// - the dimenstion of fPedRms. to 1;   
// - the dimenstion of fPedRmsErr. to 1;
//
// Calls:
// - Clear()
//
MExtractedSignalBlindPixel::MExtractedSignalBlindPixel(const char* name, const char* title)
{

  fName  = name  ? name  : "MExtractedSignalBlindPixel";
  fTitle = title ? title : "Container of the Extracted Signals";

  fBlindPixelIdx.Set(1);
  fExtractedSignal.Set(1);
  fNumSaturated.Set(1);

  fPed.Set(1);      
  fPedErr.Set(1);   
  fPedRms.Set(1);   
  fPedRmsErr.Set(1);

  Clear();
}

// ------------------------------------------------------------------------
//
// Set values of:
// - fNumSaturated   
// - fExtractedSignal
// to gkSignalInitializer
//
void MExtractedSignalBlindPixel::Clear(Option_t *o)
{

  for (Int_t i=0;i<fBlindPixelIdx.GetSize();i++)
    {
      fNumSaturated   .AddAt(gkSignalInitializer,i);
      fExtractedSignal.AddAt(gkSignalInitializer,i);
    }
}

// ------------------------------------------------------------------------
//
// Set number num of used FADC slices, starting from (including) first 
//
void MExtractedSignalBlindPixel::SetUsedFADCSlices(const Byte_t first, const Byte_t num)   
{
  fFirst          = first; 
  fNumFADCSamples = num;
}

// --------------------------------------------------------------------------------------------
//
// Returns true, if fExtractedSignal is greater or equal 0 and smaller than gkSignalInitializer
//
Bool_t MExtractedSignalBlindPixel::IsValid( const Int_t i ) const
{
    return fExtractedSignal.At(i) >= 0 && fExtractedSignal.At(i) <  gkSignalInitializer;
}

// --------------------------------------------------------------------------------------------
//
// Prints for all stored blind pixels the ID, the signal and the number of saturated slices
//
void MExtractedSignalBlindPixel::Print(Option_t *o) const
{

  for (Int_t i=0;i<fBlindPixelIdx.GetSize();i++)
    {
      
      *fLog << "Blind Pixel ID: " << fBlindPixelIdx.At(i)
            << ": Signal: " << fExtractedSignal.At(i)
            << " Saturated Slices: " <<  fNumSaturated.At(i)
            << endl;
    }
}

// ------------------------------------------------------------------------------------
//
// Returns true if the extraction type. Available are: kAmplitude, kIntegral and kFilter
// The flags kIntegral and kFilter may be set both. 
//
Bool_t MExtractedSignalBlindPixel::IsExtractionType( const MExtractBlindPixel::ExtractionType_t typ )
{
  
  return TESTBIT( fExtractionType, typ );

}
