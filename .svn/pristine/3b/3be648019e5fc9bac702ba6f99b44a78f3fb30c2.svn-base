#ifndef MARS_MCT1ReadPreProc
#define MARS_MCT1ReadPreProc

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

#ifndef MARS_MRead
#include "MRead.h"
#endif

class TList;
class MTime;
class MMcEvt;
class MMcTrig;
class MGeomCam;
class MSrcPosCam;
class MCerPhotEvt;
class MPedPhotCam;
class MBadPixelsCam;
class MRawRunHeader;
class MTaskList;
class MParList;
class MParameterD;
class MBinning;

struct outputpars;
struct eventrecord;

class MCT1ReadPreProc : public MRead
{
private:
    ifstream *fIn;          // the inputfile
    TList    *fFileNames;   // Array which stores the \0-terminated filenames

    MGeomCam      *fGeom;    // camera geometry
    MCerPhotEvt   *fNphot;   // the data container for all data.
    MPedPhotCam   *fPedest;  // ct1 pedestals
    MTime         *fTime;    // event time
    MMcEvt        *fMcEvt;   // monte carlo data container for MC files
    MMcTrig       *fMcTrig;  // mc data container for trigger information
    MSrcPosCam    *fSrcPos;  // source position in the camera
    MBadPixelsCam *fBlinds;  // Array holding blind pixels
    MRawRunHeader *fRawRunHeader; // raw run header
    MParList      *fParList;      // parameter list
    MParameterD   *fHourAngle;    // hour angle [deg]
    MParameterD   *fThetaOrig;    // original zenith angle [rad]
    MBinning      *fBinningT;     // Theta binning for the smearing

    Bool_t fIsMcFile;       // Flag whether current run is a MC run

    UInt_t fNumFile;
    UInt_t fNumEvents;      // number of events counted in all runs in all files
    UInt_t fNumEventsInRun; // number of events in the counted in th ecurrent run
    UInt_t fNumRuns;        // number of processed runs of all files
    UInt_t fEntries;        // entries of all files succesfully added
    UInt_t fNumFilterEvts;  // number of events mentioned in the runs footers

    TArrayF fPedRMS;
    TArrayI fBlnd;

    Bool_t OpenNextFile();

    Int_t  GetNumEvents(const TString name) const;
    Bool_t CheckHeader(const TString fname) const;

    void   ReadPedestals();
    Int_t  ReadRunHeader();
    Int_t  ReadRunFooter();
    Bool_t CheckFilePosition();
    void   ProcessRunHeader(const struct outputpars &outpars);
    Bool_t ProcessEvent(const struct eventrecord &event);

    Double_t SmearTheta(Double_t theta);
    Double_t DiscreteTheta(Double_t theta);

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    Bool_t Rewind();

public:
    MCT1ReadPreProc(const char *filename=NULL,
                    const char *name=NULL,
                    const char *title=NULL);

    ~MCT1ReadPreProc();

    Int_t AddFile(const char *fname, int i=0);

    UInt_t GetEntries() { return fEntries; }

    ClassDef(MCT1ReadPreProc, 0) // Reads the CT1 preproc data file
};

#endif



