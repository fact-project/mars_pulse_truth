#ifndef MARS_MReportCamera
#define MARS_MReportCamera

#ifndef MARS_MReport
#include "MReport.h"
#endif

class MCameraCooling;
class MCameraLids;
class MCameraHV;
class MCameraDC;
class MCameraLV;
class MCameraAUX;
class MCameraCalibration;
class MCameraActiveLoad;
class MCameraCentralPix;

class MReportCamera : public MReport
{
private:
    Byte_t fStatus;   // CaCo monitored status of the sentinel (0-9), Sentinel_state

    MCameraCooling     *fCooling;     //!
    MCameraLids        *fLids;        //!
    MCameraAUX         *fAUX;         //!
    MCameraHV          *fHV;          //!
    MCameraDC          *fDC;          //!
    MCameraLV          *fLV;          //!
    MCameraActiveLoad  *fActiveLoad;  //!
    MCameraCalibration *fCalibration; //!
    MCameraCentralPix  *fCentralPix;  //!

    Bool_t SetupReading(MParList &plist);

    Bool_t InterpreteCamera(TString &str, Int_t ver);
    Bool_t InterpreteDC(TString &str);
    Bool_t InterpreteHV(TString &str);
    Bool_t InterpreteCOOL(TString &str);
    Bool_t InterpreteLID(TString &str);
    Bool_t InterpreteHVPS(TString &str);
    Bool_t InterpreteLV(TString &str);
    Bool_t InterpreteAUX(TString &str);
    Bool_t InterpreteCAL(TString &str);
    Bool_t InterpreteHOT(TString &str);
    Bool_t InterpreteActiveLoad(TString &str);
    Bool_t InterpreteCentralPix(TString &str, Int_t ver);
    Bool_t InterpreteCHTEMP(TString &str);
    Bool_t InterpreteHVFIL(TString &str);
    Bool_t InterpretePSSEN(TString &str);
    Bool_t InterpreteLIQ(TString &str);

    Int_t  InterpreteBody(TString &str, Int_t ver);

public:
    MReportCamera();

    Byte_t GetStatus() const { return  fStatus; }

    ClassDef(MReportCamera, 2) // Class for CAMERA-REPORT information
};

#endif
