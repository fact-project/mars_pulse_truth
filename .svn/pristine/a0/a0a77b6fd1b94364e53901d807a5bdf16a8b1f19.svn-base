#ifndef MARS_MExtractedSignalBlindPixel
#define MARS_MExtractedSignalBlindPixel

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

#ifndef MARS_MExtractBlindPixel
#include <MExtractBlindPixel.h>
#endif

class MExtractedSignalBlindPixel : public MParContainer
{
private:

  static const Int_t gkSignalInitializer; //! Initializer for variables

  TArrayI fBlindPixelIdx;                 // Array Blind Pixel IDs
  TArrayF fExtractedSignal;               // Array Extracted signals per Blind Pixel ID
  TArrayI fNumSaturated;                  // Array Number of saturated slices per Blind Pixel ID 
  					  
  TArrayF fPed;                           // Array Pedestal per Blind Pixel IDs                               
  TArrayF fPedErr;                        // Array Pedestal Error per Blind Pixel ID          
  TArrayF fPedRms;                        // Array Pedestal RMS per Blind Pixel ID 
  TArrayF fPedRmsErr;                     // Array Pedestal RMS Error per Blind Pixel ID 
					  
  Byte_t  fFirst;                         // First FADC extraction slice
  Byte_t  fNumFADCSamples;                // Number of summed FADC slices
  Byte_t  fExtractionType;                // What extraction type has been chosen?
  
public:

  MExtractedSignalBlindPixel(const char* name=NULL, const char* title=NULL);

  void Clear(Option_t *o="");
  void Print(Option_t *o="") const;
  
  // Getters
  Int_t   GetBlindPixelIdx   ( const Int_t i=0 )  const { return fBlindPixelIdx.At(i)    ; }  
  Float_t GetExtractedSignal ( const Int_t i=0 )  const { return fExtractedSignal.At(i)  ; }
  Int_t   GetNumBlindPixels  ()                   const { return fBlindPixelIdx.GetSize(); }
  Int_t   GetNumSaturated    ( const Int_t i=0 )  const { return fNumSaturated.At(i)     ; }
  Byte_t  GetNumFADCSamples  ()                   const { return fNumFADCSamples         ; }

  Float_t GetPed             ( const Int_t i=0 )  const { return fPed.At(i)              ; }
  Float_t GetPedErr          ( const Int_t i=0 )  const { return fPedErr.At(i)           ; }
  Float_t GetPedRms          ( const Int_t i=0 )  const { return fPedRms.At(i)           ; }
  Float_t GetPedRmsErr       ( const Int_t i=0 )  const { return fPedRmsErr.At(i)        ; }

  Bool_t  IsExtractionType   ( const MExtractBlindPixel::ExtractionType_t typ );
  Bool_t  IsValid            ( const Int_t i=0 )  const;   

  // Setter
  void SetExtractionType( const Byte_t b=0 )            { fExtractionType  = b           ; }
  void SetNumFADCSamples( const Byte_t num )            { fNumFADCSamples  = num         ; }    
  void SetUsedFADCSlices( const Byte_t first, const Byte_t num );

  void SetBlindPixelIdx  ( const Int_t i,   const Int_t nr=0)      {
    if (nr>fBlindPixelIdx.GetSize()-1)
      {
	fBlindPixelIdx.Set(nr+1);
	fExtractedSignal.Set(nr+1);
	fNumSaturated.Set(nr+1);
	fPed.Set(nr+1);
	fPedErr.Set(nr+1);
	fPedRms.Set(nr+1);
	fPedRmsErr.Set(nr+1);
      }
    fBlindPixelIdx.AddAt(i,nr); }
  void SetExtractedSignal( const Float_t f, const Int_t nr=0 )     {
    fExtractedSignal.AddAt(f,nr); }
  void SetNumSaturated   ( const Int_t i,   const Int_t nr=0 )     {
    fNumSaturated.AddAt(i,nr); }
  void SetPed            ( const Float_t f, const Int_t nr=0 )     {
    fPed.AddAt(f,nr); }
  void SetPedErr         ( const Float_t f, const Int_t nr=0 )     {
    fPedErr.AddAt(f,nr); }
  void SetPedRms         ( const Float_t f, const Int_t nr=0 )     {
    fPedRms.AddAt(f,nr); }
  void SetPedRmsErr      ( const Float_t f, const Int_t nr=0 )     {
    fPedRmsErr.AddAt(f,nr); }

  ClassDef(MExtractedSignalBlindPixel, 3)	// Storage Container for extracted signal of Blind Pixel
};

#endif








