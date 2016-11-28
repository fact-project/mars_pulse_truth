#ifndef MARS_MCalibrateDC
#define MARS_MCalibrateDC

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

#ifndef ROOT_TString
#include <TString.h>
#endif

#ifndef MARS_MHCamera
#include "MHCamera.h"
#endif

#ifndef MARS_MTime
#include "MTime.h"
#endif

#ifndef MARS_MTask
#include "MTask.h"
#endif

class TH1F;
class MGeomCam;
class MCameraDC;

class MCalibrateDC : public MTask
{

private:

    MGeomCam      *fGeomCam;
    MCameraDC     *fCurr;
    MTime         *fTimeCurr;

    MTime fStartingMissCalibration;
    MTime fEndingMissCalibration;
    
    TString fFileName;
    
    TH1F *fCalHist;
    MHCamera fDisplay;

    Double_t fMinDCAllowed; //[uA]
    
    UInt_t fNumPixels ;
    TArrayD  fDCCalibrationFactor;
    Double_t fDCMissCalibrationFactor;
    
    Double_t fDCCalibration; //[uA]
    Double_t fDCCalibrationRMS; //[uA]
    
    Bool_t ProcessFile();
    Bool_t DCCalibrationCalc();
    
  public:
    
    MCalibrateDC(TString filename="", const char *name=NULL, const char *title=NULL);
    ~MCalibrateDC();

    Int_t PreProcess(MParList *pList);
    Int_t Process();

    //Setters
    void SetFileName(TString filename="") {fFileName=filename;}
    void SetMinDCAllowed(Double_t mindc=0.5) {fMinDCAllowed=mindc;}
    
    //Getters
    MHCamera& GetDisplay() { return fDisplay; }
    TArrayD&  GetDCCalibrationFactor() { return fDCCalibrationFactor; }

    ClassDef(MCalibrateDC, 0) // Taks to intercalibrate the DC of all pmts from a continuos light run
};

#endif
