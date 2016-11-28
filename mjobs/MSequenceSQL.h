#ifndef MARS_MSequenceSQL
#define MARS_MSequenceSQL

#ifndef MARS_MSequence
#include "MSequence.h"
#endif

class TSQLRow;
class TSQLResult;

class MSQLMagic;

class MSequenceSQL : public MSequence
{
private:
    Bool_t  GetRuns(MSQLMagic &serv, TString query, RunType_t type, UInt_t nevts=(UInt_t)-1);
    TString GetTimeFromDatabase(MSQLMagic &serv, const TString &query);
    Bool_t  GetFromDatabase(MSQLMagic &serv, TSQLResult &res);
    Bool_t  GetFromDatabase(MSQLMagic &serv, TSQLRow &data);

public:
    MSequenceSQL(MSQLMagic &serv, UInt_t seqno, Int_t tel=0) { GetFromDatabase(serv, seqno, tel); }
    MSequenceSQL(const char *rc, UInt_t seqno, Int_t tel=0)  { GetFromDatabase(rc, seqno, tel); }
    MSequenceSQL(UInt_t seqno, Int_t tel=0)                  { GetFromDatabase(seqno, tel); }

    Bool_t GetFromDatabase(MSQLMagic &serv, UInt_t seqno=(UInt_t)-1, Int_t tel=-1);
    Bool_t GetFromDatabase(const char *rc, UInt_t seqno=(UInt_t)-1, Int_t tel=-1);
    Bool_t GetFromDatabase(UInt_t seqno=(UInt_t)-1, Int_t tel=-1) { return GetFromDatabase("sql.rc", seqno, tel); }

    ClassDef(MSequenceSQL, 0) // Extension of MSequence to get a sequence from a database
};

#endif
