#ifndef MARS_MRawSocketRead
#define MARS_MRawSocketRead

#ifndef MARS_MRawRead
#include "MRawRead.h"
#endif

class MReadSocket;

class MRawSocketRead : public MRawRead
{
private:
    MReadSocket *fIn;         //! buffered input stream (file to read from)
    MParList    *fParList;    //!

    UInt_t       fRunNumber;  //!
    UInt_t       fEvtNumber;  //!

    Int_t        fPort;       // Port on which we wait for the connection

    Bool_t OpenSocket();

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

public:
    MRawSocketRead(const char *name=NULL, const char *title=NULL);
    ~MRawSocketRead();

    void SetPort(int port) { fPort = port; }

    TString GetFullFileName() const { return "<socket>"; }

    ClassDef(MRawSocketRead, 0)	//Task to read DAQ binary data from tcp/ip socket
};

#endif
