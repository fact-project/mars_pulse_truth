#ifndef MARS_MCT1ReadAscii
#define MARS_MCT1ReadAscii

#ifndef MARS_MTask
#include "MTask.h"
#endif

class TList;
class MCerPhotEvt;
class MPedPhotCam;

class MCT1ReadAscii : public MTask
{
private:
    ifstream    *fIn;        // the inputfile
    MCerPhotEvt *fNphot;     // the data container for all data.
    MPedPhotCam *fPedest;    // CT1 pedestals
    TList       *fFileNames; // Array which stores the \0-terminated filenames

    Bool_t OpenNextFile();

    void ReadPedestals();
    void ReadData();

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MCT1ReadAscii(const char *filename=NULL,
                  const char *name=NULL,
                  const char *title=NULL);

    ~MCT1ReadAscii();

    Int_t AddFile(const char *fname, Int_t dummy=-1);

    ClassDef(MCT1ReadAscii, 0)	// Reads the CT1 data file
};

#endif

