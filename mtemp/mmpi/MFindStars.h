#ifndef MARS_MFindStars
#define MARS_MFindStars

#ifndef ROOT_TArrayS
#include <TArrayS.h>
#endif

#ifndef ROOT_TArrayC
#include <TArrayC.h>
#endif

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MHCamera
#include "MHCamera.h"
#endif

#ifndef MARS_MAstroCamera
#include "MAstroCamera.h"
#endif

class MGeomCam;
class MGeomPix;
class MCameraDC;
class MTime;
class MReportDrive;
class MStarLocalCam;
class MStarLocalPos;

class MFindStars : public MTask
{

private:

    MGeomCam      *fGeomCam;
    MCameraDC     *fCurr;
    MTime         *fTimeCurr;
    MReportDrive  *fDrive;
    MStarLocalCam *fStars;

    MAstroCamera fAstro;
    TArrayC      fPixelsUsed;
    MHCamera     fDisplay;

    UInt_t fMaxNumIntegratedEvents;
    UInt_t fNumIntegratedEvents;

    Float_t fRingInterest; //[mm]
    Float_t fMinDCForStars; //[uA]

    Float_t fDCTailCut;

    //Fitting(Minuit) variables
    const Int_t fNumVar;
    Float_t fTempChisquare;
    Int_t fTempDegreesofFreedom;
    Int_t fMinuitPrintOutLevel;
    
    TString *fVname;
    TArrayD fVinit; 
    TArrayD fStep; 
    TArrayD fLimlo; 
    TArrayD fLimup; 
    TArrayI fFix;
    TObject *fObjectFit;
    TString fMethod;
    Bool_t fNulloutput;
    
    Bool_t DCPedestalCalc();
    Bool_t FindPixelWithMaxDC(Float_t &maxDC, MGeomPix &maxPix);
    Bool_t FindStar(MStarLocalPos* star);
    Bool_t ShadowStar(MStarLocalPos* star);

    TString fGeometryFile;
    TString fBSCFile;

  public:
    
    MFindStars(const char *name=NULL, const char *title=NULL);
    
    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    // setters
    void SetNumIntegratedEvents(UInt_t max) {fMaxNumIntegratedEvents=max;}
    void SetRingInterest(Float_t ring) {fRingInterest=ring;}
    void SetBlindPixels(TArrayS blindpixels);
    void SetMinuitPrintOutLevel(Int_t level) {fMinuitPrintOutLevel=level;}
    void SetDCTailCut(Float_t cut) {fDCTailCut=cut;}

    void SetChisquare(Float_t chi) {fTempChisquare=chi;}
    void SetDegreesofFreedom(Int_t free) {fTempDegreesofFreedom=free;}

    void SetGeometryFile(TString f) {fGeometryFile=f;}
    void SetBSCFile(TString f) {fBSCFile=f;}
    void SetRaDec(Double_t ra, Double_t dec) {fAstro.SetRaDec(ra,dec);}
    void SetRaDec(TVector3 &v) { fAstro.SetRaDec(v); }
    void SetLimMag(Double_t mag) { fAstro.SetLimMag(mag); } 
    void SetRadiusFOV(Double_t deg) { fAstro.SetRadiusFOV(deg); }


    //Getters
    MHCamera& GetDisplay() { return fDisplay; }
    
    Float_t GetChisquare() {return fTempChisquare;}
    Int_t GetDegreesofFreedom() {return fTempDegreesofFreedom;}
    UInt_t GetNumIntegratedEvents() {return fMaxNumIntegratedEvents;}
    Float_t GetRingInterest() {return fRingInterest;}
    
  ClassDef(MFindStars, 0) // Tool to find stars from DC Currents
};

#endif
