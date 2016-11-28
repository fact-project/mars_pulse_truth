#ifndef MARS_MExtractBlindPixel
#define MARS_MExtractBlindPixel

#ifndef MARS_MExtractor
#include "MExtractor.h"
#endif

#ifndef MARS_MArrayI
#include "MArrayI.h"
#endif
#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif

class MCalibrationBlindCam;
class MExtractedSignalBlindPixel;

class MExtractBlindPixel : public MExtractor
{
private:

  static const UInt_t  fgBlindPixelIdx;    //! Default blind pixels index before modification run
  static const Byte_t  fgHiGainFirst;      //! Default First FADC slice Hi-Gain Signal (currently set to: 10   ) 
  static const Byte_t  fgHiGainLast;       //! Default Last  FADC slice Hi-Gain Signal (currently set to: 29   ) 
  static const Byte_t  fgLoGainFirst;      //! Default First FADC slice Filter         (currently set to: 0    ) 
  static const Byte_t  fgLoGainLast;       //! Default Last  FADC slice Filter         (currently set to: 6    ) 
  static const Int_t   fgNSBFilterLimit;   //! Default for fNSBFilterLimit
  static const Float_t fgResolution;       //! Default for fResolution         (currently set to: 0.003)
  static const Float_t gkOverflow;         //! Default sum to assign overflow in case of saturation

  MExtractedSignalBlindPixel *fBlindPixel; // Extracted signal of the Blind Pixel

//  Byte_t   fHiLoFirst;                     // If not zero, start extraction from fHiLoFirst slice of Low-Gain
//  Byte_t   fHiLoLast;

//  MArrayF  fHiGainSignal;                  //! Need fast access to the signals in a float way
  MArrayF  fHiGainFirstDeriv;              //! First derivative at intersection
  MArrayF  fHiGainSecondDeriv;             //! Second derivative at intersection

  Float_t fResolution;                     // The time resolution in FADC units
  MArrayI fBlindPixelIdx;                  // Array holding the IDs of the blind pixel(s)
  Int_t   fNSBFilterLimit;                 // Limit of sum of FADC slices for filter part

  Byte_t  fExtractionType;                 // What extraction type has been chosen?
  Byte_t  fDataType;                       // What data container type is needed?  
//  Int_t   fNumBlindPixels;                 // Current number of blind pixels

public:
  enum ExtractionType_t { kAmplitude, kIntegral, kFilter };
  enum DataType_t       { kRawEvt, kRawEvt2 };

private:
  Float_t FindAmplitude(Int_t idx, Int_t numsat) const;
//  void FindIntegral    (Byte_t *firstused, Byte_t *lowgain, Float_t &sum, Byte_t &sat);
//  void FindSignalFilter(Byte_t *ptr, Int_t range,             Int_t &sum, Byte_t &sat) const;
  
  Int_t  PreProcess(MParList *pList);
  Bool_t ReInit(MParList *pList);
  Int_t  Process();

public:

  MExtractBlindPixel(const char *name=NULL, const char *title=NULL);

  void Clear( const Option_t *o ="");
  
  // Getters
  Bool_t IsExtractionType ( const ExtractionType_t typ );
  Bool_t IsDataType       ( const DataType_t       typ );

  // Setters
  /*
  void SetBlindPixelIdx(  const Int_t  idx=fgBlindPixelIdx, const UInt_t nr=0 )
  {
      if (nr>=fBlindPixelIdx.GetSize())
          fBlindPixelIdx.Set(nr+1);
      fBlindPixelIdx[nr] = idx;
      }
      */

  void SetBlindPixels(const MCalibrationBlindCam &cam);

  void SetExtractionType( const ExtractionType_t typ=kAmplitude );
  void SetDataType    ( const DataType_t       typ=kRawEvt    );  
  void SetNSBFilterLimit( const Int_t   lim=fgNSBFilterLimit )  { fNSBFilterLimit = lim;   }     
  
//  void SetNumBlindPixels( const Int_t   num=1 )  { fNumBlindPixels = num;   }
  
  void SetRange         ( const UShort_t hifirst=0, const UShort_t hilast=0,
   		          const Int_t    lofirst=0, const Byte_t   lolast=0 );
  void SetResolution    ( const Float_t f=fgResolution       )  { fResolution     = f;     }
  
  ClassDef(MExtractBlindPixel, 0) // Signal Extractor for the Blind Pixel
};

#endif

