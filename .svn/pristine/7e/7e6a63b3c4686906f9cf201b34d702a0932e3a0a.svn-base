#ifndef MARS_MRawFileRead
#define MARS_MRawFileRead

#ifndef MARS_MRawRead
#include "MRawRead.h"
#endif

class TList;
class MTaskList;

class MRawFileRead : public MRawRead
{
private:
    TList    *fFileNames;      // list of file names
    UInt_t    fNumFile;        //! number of next file
    UInt_t    fNumEvents;      //! input stream (file to read from)
    UInt_t    fNumTotalEvents; //! total number of events in all files

    std::istream *fIn;         //! input stream (file to read from)

protected:
    MParList *fParList;        //! tasklist to call ReInit from

private:
    UInt_t    fInterleave;

    Bool_t    fForce;

    virtual std::istream *OpenFile(const char *filename);
    virtual Bool_t        ReadRunHeader(std::istream &fin);
    virtual Bool_t        InitReadData(std::istream &/*fin*/) { return kTRUE; }

    Int_t  OpenNextFile(Bool_t print=kTRUE);
    Bool_t CalcNumTotalEvents();

protected:
    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    Bool_t    fIsMc;

public:
    MRawFileRead(const char *filename=NULL, const char *name=NULL, const char *title=NULL);
    ~MRawFileRead();

    static Byte_t IsFileValid(const char *name);

    void SetInterleave(UInt_t i) { fInterleave = i; }
    void SetForce(Bool_t b=kTRUE) { fForce=b; }

    TString GetFullFileName() const;

    Int_t  AddFile(const char *fname, Int_t entries=-1);
    Bool_t Rewind();
    UInt_t GetEntries() { return fNumTotalEvents/fInterleave; }

    const std::istream *GetStream() const { return fIn; }

    ClassDef(MRawFileRead, 0)	// Task to read the raw data binary file
};

#endif
