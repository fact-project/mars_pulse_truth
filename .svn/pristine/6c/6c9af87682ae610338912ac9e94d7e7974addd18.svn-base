#ifndef MARS_MFEvtNumber
#define MARS_MFEvtNumber

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

#ifndef ROOT_TExMap
#include <TExMap.h>
#endif

class MRawRunHeader;
class MRawEvtHeader;

class MFEvtNumber : public MFilter
{
private:
    MRawRunHeader *fRun;    //! MRawRunHeader with run-number of event
    MRawEvtHeader *fEvt;    //! MRawEvtHeader with evt-number of event

    Bool_t  fResult;        //! Result returned by IsExpressionTrue

    TString fFileName;      // File name of the file with the even-/run-number
    TString fTreeName;      // Tree name of the file with the even-/run-number

    TString fSelector;      // Selector to choose run-/evt-number from file

    TExMap  fList;          // List with all event-/run-numbers

    ULong_t Compile(ULong64_t run, ULong64_t evt) const { return (run<<32)|evt; }
    ULong_t GetEvtId() const;

    // MTask
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

    // MFilter
    Bool_t IsExpressionTrue() const { return fResult; }

public:
    MFEvtNumber(const char *name=NULL, const char *title=NULL);

    // Setter
    void SetFileName(const char *name) { fFileName=name; }
    void SetTreeName(const char *name) { fTreeName=name; }
    void SetSelector(const char *sel)  { fSelector=sel; }

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    ClassDef(MFEvtNumber, 0) // Filter to select events by run- and evt-number
};

#endif






