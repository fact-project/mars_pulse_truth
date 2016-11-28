#ifndef MARS_MExtractPedestal
#define MARS_MExtractPedestal

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MArrayD
#include <MArrayD.h>
#endif

#ifndef MARS_MArrayI
#include <MArrayI.h>
#endif

class MGeomCam;
class MExtractor;
class MPedestalCam;
class MRawEvtData;
class MRawRunHeader;
class MRawEvtHeader;
class MExtractTimeAndCharge;
class MPedestalSubtractedEvt;
class MRawEvtPixelIter;

typedef UShort_t USample_t;

class MExtractPedestal : public MTask
{
private:
  static const TString  fgNamePedestalCam;  //! "MPedestalCam"
  static const TString  fgNameRawEvtData;   //! "MRawEvtData"

  static const UShort_t fgCheckWinFirst;    //! First FADC slice to check for signal (currently set to: 0)
  static const UShort_t fgCheckWinLast;     //! Last FADC slice to check for signal  (currently set to: 29)
  static const UShort_t fgMaxSignalVar;     //! The maximum difference between the highest and lowest slice
  static const UShort_t fgMaxSignalAbs;     //! The maximum absolte slice

  TString fNamePedestalCamOut;       // Name of the outgoing 'MPedestalCam' container
  TString fNamePedestalCamInter;     // Name of the intermediate 'MPedestalCam' container
  TString fNameRawEvtData;           // Name of MRawEvtData

  Bool_t  fRandomCalculation;        // Is pedestalextraction by extractor random?

  // Helper functions
  void CalcAreaResults(const UInt_t aidx);
  void CalcSectorResults(const UInt_t sector);

protected:

  Bool_t  fIntermediateStorage;      // Is pedestal stored every event?

  MGeomCam      *fGeom;              //! Camera geometry
  MPedestalCam  *fPedestalsInter;    //! Pedestals of all pixels in the camera (intermediate)
  MPedestalCam  *fPedestalsOut;      //! Pedestals of all pixels in the camera (outgoing)
  MRawEvtData   *fRawEvt;            //! Raw event data (time slices)
  MRawRunHeader *fRunHeader;         //! RunHeader information
  MExtractTimeAndCharge *fExtractor; //  Possible Extractor
  MPedestalSubtractedEvt *fSignal;   //!

  UShort_t fExtractWinFirst;         // First FADC slice to extract pedestal from
  UShort_t fExtractWinSize;          // Number of slices to calculate the pedestal from
  UShort_t fExtractWinLast;          // Last FADC slice to extract pedestal from 

  UShort_t fCheckWinFirst;
  UShort_t fCheckWinLast;

  UShort_t fMaxSignalVar;
  UShort_t fMaxSignalAbs;

  Bool_t  fUseSpecialPixels;         // Flag if the special pixels shall be treated

  MArrayD fSumx;                     // sum of values
  MArrayD fSumx2;                    // sum of squared values
  MArrayD fSumAB0;                   // sum of ABFlag=0 slices
  MArrayD fSumAB1;                   // sum of ABFlag=1 slices
  MArrayD fAreaSumx;                 // averaged sum of values per area idx
  MArrayD fAreaSumx2;                // averaged sum of squared values per area idx
  MArrayD fAreaSumAB0;               // averaged sum of ABFlag=0 slices per area idx
  MArrayD fAreaSumAB1;               // averaged sum of ABFlag=1 slices per area idx
  MArrayI fAreaFilled;               // number of valid entries with area idx
  MArrayI fAreaValid;                // number of valid pixels  within area idx
  MArrayD fSectorSumx;               // averaged sum of values per sector
  MArrayD fSectorSumx2;              // averaged sum of squared values per sector
  MArrayD fSectorSumAB0;             // averaged sum of ABFlag=0 slices per sector
  MArrayD fSectorSumAB1;             // averaged sum of ABFlag=1 slices per sector 
  MArrayI fSectorFilled;             // number of valid entries with sector idx
  MArrayI fSectorValid;              // number of valid pixels within sector idx

  MArrayI fNumEventsUsed;            // Number of events used for pedestal calc for each pixel

  UInt_t  fCounter;                  // Counter for events processed

  // MTask virtual functions
  Int_t  PreProcess(MParList *pList);
  Int_t  Process();
  Int_t  PostProcess();
  Bool_t ReInit(MParList *pList);
  Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);

  // Interface to be overwritten by a new class
  virtual void ResetArrays();
  virtual void Calc() = 0;

  // Helper functions
  void CalcPixResults(const UInt_t pixid);

  void CalcPixResult();
  void CalcSectorResult();
  void CalcAreaResult();

  Bool_t  CalcPixel(const MRawEvtPixelIter &pixel, Int_t offset, UInt_t usespecialpixels=kFALSE);
  Float_t CalcExtractor(const MRawEvtPixelIter &pixel, Int_t offset) const;
  UInt_t  CalcSums(const MRawEvtPixelIter &pixel, Int_t offset, UInt_t &ab0, UInt_t &ab1) const;
  Bool_t  CheckVariation(UInt_t idx) const;

  void CheckExtractionWindow(UInt_t offset=0);

  Bool_t SetRangeFromExtractor(const MExtractor &ext, Bool_t logain);

public:
  MExtractPedestal(const char *name=NULL, const char *title=NULL);

  void Clear(const Option_t *o="");
  void Print(const Option_t *o="") const;

  // Setters
  Bool_t SetExtractWindow(UShort_t first, UShort_t size);
  Bool_t SetCheckRange(UShort_t checkfirst=fgCheckWinFirst, UShort_t checklast=fgCheckWinLast);

  virtual Bool_t SetRangeFromExtractor(const MExtractor &ext) = 0;

  void SetMaxSignalVar(UShort_t maxvar=40)   { fMaxSignalVar = maxvar; }
  void SetMaxSignalAbs(UShort_t maxabs=250)  { fMaxSignalAbs = maxabs; }

  // names
  void SetNamePedestalCamInter(const char *name=fgNamePedestalCam) { fNamePedestalCamInter = name; }
  void SetNamePedestalCamOut  (const char *name=fgNamePedestalCam) { fNamePedestalCamOut   = name; }
  void SetNameRawEvtData      (const char *name=fgNameRawEvtData)  { fNameRawEvtData       = name; }

  // pointers
  void SetExtractor     ( MExtractTimeAndCharge *e) { fExtractor      = e; }
  void SetPedestalsInter( MPedestalCam *pedcam    ) { fPedestalsInter = pedcam; }
  void SetPedestalsOut  ( MPedestalCam *pedcam    ) { fPedestalsOut   = pedcam; }

  // flags
  void SetIntermediateStorage (Bool_t b=kTRUE) { fIntermediateStorage = b; }
  void SetUseSpecialPixels    (Bool_t b=kTRUE) { fUseSpecialPixels    = b; }
  void SetRandomCalculation   (Bool_t b=kTRUE) { fRandomCalculation   = b; }

  ClassDef(MExtractPedestal, 0)   // Base class for pedestal extractors
};

#endif
