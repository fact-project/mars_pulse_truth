#ifndef MARS_MExtractTime
#define MARS_MExtractTime

#ifndef MARS_MExtractor
#include "MExtractor.h"
#endif

class MPedestalPix;
class MArrivalTimeCam;

class MExtractTime : public MExtractor
{
protected:
  
  static const char *fgNameTimeCam;   //! "MArrivalTimeCam"
  TString  fNameTimeCam;              // Name of the 'MArrivalTimeCam' container
  
  MArrivalTimeCam *fArrTime;          //! Container with the photons arrival times
/*
  virtual void FindTimeHiGain(Byte_t *firstused, Float_t &time, Float_t &dtime,
                              Byte_t &sat, const MPedestalPix &ped) const {}
  virtual void FindTimeLoGain(Byte_t *firstused, Float_t &time, Float_t &dtime,
                              Byte_t &sat, const MPedestalPix &ped) const {}
  */
  Int_t  PreProcess( MParList *pList );
//  Bool_t ReInit    ( MParList *pList );
//  Int_t  Process   ();

public:

  MExtractTime(const char *name=NULL, const char *title=NULL);

  void Print(Option_t *o) const; //*MENU*

  void SetNameTimeCam(const char *name=fgNameTimeCam) { fNameTimeCam = name; }
  
  ClassDef(MExtractTime, 3)   // Arrival Time Extractor Base Class
};

#endif
