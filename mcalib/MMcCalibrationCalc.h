#ifndef MARS_MMcCalibrationCalc
#define MARS_MMcCalibrationCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MCalibrationChargeCam;
class MCalibrationQECam;
class MGeomCam;
class MHillas;
class MImagePar;
class MNewImagePar;
class MMcEvt;
class MMcFadcHeader;

class TH1F;

class MMcCalibrationCalc : public MTask
{
private:
    MCalibrationChargeCam     *fCalCam;
    MCalibrationQECam         *fQECam;    
    MGeomCam                  *fGeom;
    MHillas                   *fHillas;
    MNewImagePar              *fNew;
    MImagePar                 *fPar;
    MMcEvt                    *fMcEvt;
    MMcFadcHeader             *fHeaderFadc;

    Float_t fADC2PhotEl;   // Conversion factor (photel / ADC count). FOR INER PIXELS
    Float_t fPhot2PhotEl;  // Conversion factor (photons / photoelectron) = average QE. FOR INER PIXELS
    Long_t  fEvents;
    Float_t fOuterPixelsLightCollection; // Light collection efficiency (plexiglas, light guide) of 
                                         // outer pixels w.r.t inner ones

    Float_t fMinSize; 
    // Minimum SIZE (before calibration, ADC counts) an event must have to be considered in the
    // calculation of the calibration constants.

    TH1F*   fHistADC2PhotEl;
    TH1F*   fHistPhot2PhotEl; // Histograms for monitoring the calibration.

    Bool_t CheckRunType(MParList *pList) const;
    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();
    Int_t  PostProcess();

public:
    MMcCalibrationCalc(const char *name=NULL, const char *title=NULL);

    TH1F*   GetHistADC2PhotEl() { return fHistADC2PhotEl; }
    TH1F*   GetHistPhot2PhotEl() { return fHistPhot2PhotEl; }

    void SetMinSize(Float_t x) { fMinSize = x; }

    ClassDef(MMcCalibrationCalc, 0)   // Task which obtains, for MC files, the calibration factor from ADC counts to photons. 
};

#endif
