#ifndef MARS_MReportFitsRead
#define MARS_MReportFitsRead

#ifndef MARS_MTask
#include "MTask.h"
#endif
#ifndef MARS_MTime
#include "MTime.h"
#endif

#ifdef __CINT__
#define int32_t  Int_t
#else
#include <stdint.h>
#endif

#include <map>

class fits;
class THashTable;

class MTime;
class MReport;
class MReportHelp;

class MReportFitsRead : public MTask
{
private:
    TString  fFileName;   // Name of the input file
    TString  fReportName; // Name of the report class

    MReport *fReport;     // List of possible reports to be interpreted

    MTime   fStart;       // Time range which should be read from file
    MTime   fStop;        // Time range which should be read from file

    int32_t  fBufQos;  //! Buffer for Quality of Service
    Double_t fBufTime; //! Buffer for Time

    UInt_t fMjdRef;

    enum { kHasNoHeader = BIT(14) };

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    virtual Int_t CheckFileHeader() { return 0; }

protected:
    fits *fIn;         //! buffered input stream (file to read from)

public:
    MReportFitsRead(const char *filename, const char *name=NULL, const char *title=NULL);
    ~MReportFitsRead();

    void SetHasNoHeader() { SetBit(kHasNoHeader); }
    void SetTimeStart(const MTime &tm) { fStart = tm; }
    void SetTimeStop(const MTime &tm)  { fStop = tm; }

    void SetReportName(const char *name) { fReportName = name; }

    ClassDef(MReportFitsRead, 0)// Task to read general report file
};

#endif
