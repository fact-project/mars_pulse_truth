#ifndef MARS_MRawRead
#define MARS_MRawRead

#ifndef MARS_MRead
#include "MRead.h"
#endif

class MTime;
class MParList;
class MRawRunHeader;
class MRawEvtHeader;
class MRawEvtData;
class MRawCrateArray;

class MRawRead : public MRead
{
protected:
    MRawRunHeader  *fRawRunHeader;  // run header information container to fill from file
    MRawEvtHeader  *fRawEvtHeader;  // event header information container to fill from file
    MRawEvtData    *fRawEvtData1;   // raw evt data infomation container to fill from file
    MRawEvtData    *fRawEvtData2;   // raw evt data for pixels with negative indices
    MRawCrateArray *fRawCrateArray; // crate information array container to fill from file
    MTime          *fRawEvtTime;    // raw evt time information container to fill from file

    Bool_t          fForceMode;     // Force mode skipping defect events

    void CreateFakeTime() const;

    virtual Bool_t ReadEvent(std::istream &fin);
    virtual void   SkipEvent(std::istream &fin);
    Int_t  PreProcess(MParList *pList);

private:
    virtual Bool_t OpenStream() { return kTRUE; }

public:
    MRawRead(const char *name=NULL, const char *title=NULL);

    UInt_t GetEntries() { return 0; }
    Bool_t Rewind() {return kTRUE; }

    void SetForceMode(Bool_t b=kTRUE) { fForceMode = b; }

    ClassDef(MRawRead, 0)	// Task to read the raw data binary file
};

#endif
