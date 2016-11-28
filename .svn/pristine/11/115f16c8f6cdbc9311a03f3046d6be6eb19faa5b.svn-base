#ifndef MARS_MMovieWrite
#define MARS_MMovieWrite

#ifndef MARS_MTask
#include "MTask.h"
#endif

class TH1;
class TASImage;

class MMcEvt;
class MGeomCam;
class MMovieData;
class MSignalCam;
class MPedestalCam;
class MRawEvtHeader;
class MRawRunHeader;
class MBadPixelsCam;

class MMovieWrite : public MTask
{
private:
    MRawRunHeader *fRun;   //! Header with run number and sampling frequency
    MRawEvtHeader *fHead;  //! Header with event number
    MGeomCam      *fCam;   //! Camera geometry necessary for the histograms
    MSignalCam    *fSig;   //! Input signal to display cleaned and uncleaned event
    MBadPixelsCam *fBad;   //! Information about bad pixels
    MPedestalCam  *fPed;   //! Fundamental pedestal for palette and cleaning
    MMcEvt        *fMC;    //! Informatio about MC events

    MMovieData    *fIn;    //! Input data with splines for all pixels

    FILE          *fPipe;  //! Ouput pipe to player or encoder

    Float_t fTargetLength; // [s] Target length for stream of one event (+1 frame)
    Float_t fThreshold;    // Threshold for cleaning
    UInt_t  fNumEvents;    // Maximum number of events to encode

    TString fFilename;     // name of output file

    // MTask
    Int_t PreProcess(MParList *plist);
    Int_t Process();
    Int_t PostProcess();

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // MMovieWrite
    Bool_t OpenPipe();
    Bool_t CheckPipe();

    Double_t GetMedianPedestalRms() const;

    void Clean(TH1 &h, Double_t rms) const;
    void TreatBadPixels(TH1 &h) const;

    Int_t SetPalette(Double_t rms, const TH1 &h) const;
    void DeletePalette(Int_t colidx) const;

    void UpdateImage(TASImage &img, TVirtualPad &pad);

    Bool_t WriteImage(TASImage &img);
    Bool_t WriteImage(TASImage &img, TVirtualPad &pad);
    //Bool_t WriteImage(TVirtualPad &pad);

    Bool_t Process(TH1 &h, TVirtualPad &c);

public:
    MMovieWrite(const char *name=NULL, const char *title=NULL);
    ~MMovieWrite();

    void SetFilename(const char *f) { fFilename = f; }
    void SetNumEvents(Int_t n) { fNumEvents = n; }
    void SetThreshold(Float_t f) { fThreshold = f; }
    void SetTargetLength(Float_t l) { fTargetLength = l; }

    ClassDef(MMovieWrite, 0) // Task to encode a movie
};

#endif
