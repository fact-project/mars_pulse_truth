#ifndef MARS_MReadRflFile
#define MARS_MReadRflFile

#ifndef MARS_MReadFiles
#include "MReadFiles.h"
#endif

class MRflEvtData;
class MRflEvtHeader;
class MRflRunHeader;

class MReadRflFile : public MReadFiles
{
private:
    enum {
        kError,
        kEndOfFile,
        kStartOfRun,
        kEndOfRun,
        kStartOfEvtData,
        kEndOfEvtData,
        kUndefined
    };

    MRflEvtData   *fEvtData;    //!
    MRflEvtHeader *fEvtHeader;  //!
    MRflRunHeader *fRunHeader;  //!

    Float_t fCurrentVersion;    //! Version of currently open rfl file

    // MReadRflFile
    float  ReadVersion();
    Bool_t ReadEvtHeader();
    Bool_t ReadEvtData();
    int    ReadFlag();
    Bool_t FlagIsA(const  char *s1, const char *flag);
    Int_t  EvalFlag();

    // MReadFiles
    Int_t  ReadEvent();
    Bool_t ReadHeader();

    // MTask
    Int_t PreProcess(MParList *pList);

public:
    MReadRflFile(const char *filename=NULL, const char *name=NULL,
                 const char *title=NULL);

    Bool_t SearchFor(Int_t runno, Int_t eventno);

    ClassDef(MReadRflFile, 0) // Reads reflector files
};

#endif
