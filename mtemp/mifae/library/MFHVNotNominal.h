#ifndef MARS_MFHVNotNominal
#define MARS_MFHVNotNominal

#ifndef ROOT_TString
#include <TString.h>
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MCameraHV;
class MBadPixelsCam;

class MFHVNotNominal : public MFilter
{
private:
    MCameraHV      *fHV;          // HV monitored values of all pixels in the camera
    MBadPixelsCam  *fBadPixels;   // Bad Pixels storage container

    TString fHVConfFile;
    TArrayD fHVNominal;
    Bool_t  fResult;

    Float_t fMaxHVDeviation;         // Maximum HV deviation from nominal values in %.
    Float_t fMaxNumPixelsDeviated;   // Maximum number of pixels with HV deviated from nominal values.

    UInt_t fCut[2];

    Int_t  PreProcess(MParList *pList);
    Int_t  Process();
    Int_t  PostProcess();

    Bool_t HVNotNominalRejection();

    Bool_t IsExpressionTrue() const { return fResult; }
  
public:
    MFHVNotNominal(const char *name=NULL, const char *title=NULL);

    void    SetHVNominalValues(const TString hvconf)    { fHVConfFile = hvconf; }
    void    SetHVNominalValues(const TArrayD hvnominal) { fHVNominal  = hvnominal; }

    void    SetMaxHVDeviation(const Float_t d)       { fMaxHVDeviation = d; }
    void    SetMaxNumPixelsDeviated(const Float_t n) { fMaxNumPixelsDeviated = n; }

    Float_t GetfMaxHVDeviation() const          { return fMaxHVDeviation; }
    Float_t GetfMaxNumPixelsDeviated() const    { return fMaxNumPixelsDeviated; }
    TArrayD GetHVNominal() const { return fHVNominal; }
    
    ClassDef(MFHVNotNominal, 0)   // Filter to filter the events with HV out of nominal values 
};

#endif



