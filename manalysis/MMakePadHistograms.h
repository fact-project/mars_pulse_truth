#ifndef MARS_MMakePadHistograms
#define MARS_MMakePadHistograms

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MH
#include "MH.h"
#endif

class TH2D;
class TH3D;

class MMakePadHistograms : public MH
{
private:

    TString  fType;              // type of data to be padded (ON, OFF, MC)
    Int_t    fMaxEvents;         // maximum no.of events to be read
    TString  fNamePedPhotCam;    // name of the 'MPedPhotCam' container
    TString  fNameInputFile;     // name of input file (calibrated data)
    TString  fNameOutputFile;    // name of output file (Pad histograms)
    Float_t  fPedestalLevel;     // pedestal level for MBadPixelsCalc
    Bool_t   fUseInterpolation;  // option for MBadPixelsTreat
    Bool_t   fProcessPedestal;   // option for MBadPixelsTreat
    Bool_t   fProcessTime;       // option for MBadPixelsTreat


    //----------------------------------
    // plots needed for the padding

    TH2D  *fHSigmaTheta;      // 2D-histogram (Theta, sigmabar(inner))
    TH2D  *fHSigmaThetaOuter; // 2D-histogram (Theta, sigmabar(outer))
    TH3D  *fHDiffPixTheta;  // 3D-histogram (Theta, pixel, (sigma^2-sigmabar^2)/Area)
    TH3D  *fHSigmaPixTheta; // 3D-histogram (Theta, pixel, sigma)

    TH2D  *fHBadIdTheta;    // 2D-histogram (Theta, Id of bad pixel)
    TH2D  *fHBadNTheta;     // 2D-histogram (Theta, no.of bad pixels)


public:
    MMakePadHistograms(const char *name=NULL, const char *title=NULL);
 
    void SetDataType(const char *);       // type of data (ON, OFF, MC) 
    void SetMaxEvents(Int_t);             // mx. no.of events to be read 
    void SetNameInputFile(const char *);  // Calibrated data   
    void SetNameOutputFile(const char *); // Pad histograms 
    void SetNamePedPhotCam(const char *); // "MPedPhotCamFromData"

    // parameters for MBadPixelsCalc
    void SetPedestalLevel(Float_t);

    // parameters for MBadPixelsTreat
    void SetUseInterpolation(Bool_t);
    void SetProcessPedestal(Bool_t);
    void SetProcessTime(Bool_t);

    Bool_t MakeHistograms();

    ClassDef(MMakePadHistograms, 0)    // class for making the padding histograms 
}; 

#endif









