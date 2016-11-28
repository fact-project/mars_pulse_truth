#ifndef MARS_MReport
#define MARS_MReport

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MTime
#include "MTime.h"
#endif

class fits;

class MTime;
class MParList;

class MReport : public MParContainer
{
private:
    const TString fIdentifier;    //! Identifier of the subsystem
    const Bool_t  fHasReportTime; //! Intermediate solution for DC currents

    Byte_t  fState;               // Status of the subsystem
    MTime  *fTime;                //! pointer to the corresponding time stamp

    ULong_t Hash() const { return fIdentifier.Hash(); }
    Bool_t InterpreteHeader(TString &str, Int_t ver);

protected:
    Bool_t CheckTag(TString &str, const char *tag) const;

    void Copy(TObject &obj) const;

public:
    MReport(const char *id, Bool_t time=kTRUE) : fIdentifier(id), fHasReportTime(time), fState(0xff), fTime(0) { }

    virtual Bool_t SetupReading(MParList &plist);
    virtual Bool_t SetupReadingFits(fits &) { return kTRUE; }
    virtual Int_t  InterpreteBody(TString &str, Int_t ver);

    Int_t  Interprete(TString &str, const MTime &start, const MTime &stop, const Int_t ver);
    virtual Int_t  InterpreteFits(const fits &) { return kTRUE; }
    Bool_t CheckIdentifier(TString &str) const
    {
        if (!str.BeginsWith(fIdentifier))
            return kFALSE;

        str.Remove(0, fIdentifier.Length());
        str = str.Strip(TString::kBoth);

        return kTRUE;
    }

    const TString &GetIdentifier() const { return fIdentifier; }

    Byte_t GetState() const { return fState; }
    Double_t GetMjd() const { return fTime ? fTime->GetMjd() : -1; }

    MTime *GetTime() { return fTime; }

    ClassDef(MReport, 1) // Base class for control reports
};

#endif
