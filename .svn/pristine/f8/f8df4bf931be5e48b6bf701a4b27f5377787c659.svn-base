#ifndef MARS_MReportRec
#define MARS_MReportRec

#ifndef MARS_MReport
#include "MReport.h"
#endif

class MCameraTH;
class MCameraTD;
class MCameraRecTemp;

class MReportRec : public MReport
{
    friend class MReportCC;
private:
    MCameraTH      *fTH;      //! Discriminator thresholds
    MCameraTD      *fTD;      //! Discriminator delays
    MCameraRecTemp *fRecTemp; //! Receiver Board temperatures

    // Internal
    Bool_t SetupReading(MParList &plist);

    Bool_t InterpreteTH(TString &str, Int_t ver);
    Bool_t InterpreteTD(TString &str, Int_t ver);
    Bool_t InterpreteRecTemp(TString &str);

    // MReport
    Int_t InterpreteRec(TString &str, Int_t ver, MCameraTH &th, MCameraTD &td, MCameraRecTemp &temp);
    Int_t InterpreteBody(TString &str, Int_t ver);

public:
    MReportRec();

    ClassDef(MReportRec, 1) // Class for REC-REPORT information
};

#endif
