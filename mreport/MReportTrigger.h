#ifndef MARS_MReportTrigger
#define MARS_MReportTrigger

#ifndef MARS_MReport
#include "MReport.h"
#endif

#ifndef ROOT_TArrayL
#include <TArrayL.h>
#endif

class MTriggerIPR;
class MTriggerCell;
class MTriggerBit;
class MTriggerPrescFact;
class MTriggerLiveTime;

class MReportTrigger : public MReport
{
private:
  
  Float_t fL2BeforePrescaler;       // L2 trigger rate before prescaler
  Float_t fL2AfterPrescaler;        // L2 trigger rate after prescaler
  TString fTablename;               // Name of the trigger table
  TString fL1Tablename;             // Name of the L1 trigger table
  TString fL2Tablename;             // Name of the L2 trigger table
  
  MTriggerBit *fBit;                //! container of the L2 prescaler rates
  MTriggerIPR *fIPR;                //! container of the IPR
  MTriggerCell *fCell;              //! container of the L1 cell trigger rates
  MTriggerPrescFact *fPrescFactor;  //! container of the L2 prescaling factors
  MTriggerLiveTime *fLiveTime;      //! container of the scaler live-deadtime
  
  Bool_t SetupReading(MParList &plist);

  Bool_t InterpreteIPR(TString &str);
  Bool_t InterpreteCell(TString &str);
  Bool_t InterpreteBit(TString &str);
  Bool_t InterpretePrescFact(TString &str);
  Bool_t InterpreteLiveTime(TString &str);
  Bool_t InterpreteDummy(TString &str);
  Bool_t InterpreteL1L2Table(TString &str);

  Int_t InterpreteBody(TString &str, Int_t ver);
  
  Bool_t InterpreteOldBody(TString &str);

public:
    MReportTrigger();

    Float_t GetL2BeforePrescaler() const { return fL2BeforePrescaler; }
    Float_t GetL2AfterPrescaler() const { return fL2AfterPrescaler; }
    TString GetTablename() const { return fTablename; } 
    TString GetL1Tablename() const { return fL1Tablename; } 
    TString GetL2Tablename() const { return fL2Tablename; } 

    ClassDef(MReportTrigger, 2) // Class for TRIGGER-REPORT information
 };

#endif
