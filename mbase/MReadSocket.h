#ifndef MARS_MReadSocket
#define MARS_MReadSocket

#ifndef ROOT_TROOT
#include <TROOT.h>
#endif
#ifndef ROOT_TTime
#include <TTime.h>
#endif

#include <iostream>  // base classes for MReadSocket

class TSocket;
class TServerSocket;

class MReadSocket : public std::streambuf, public std::istream
{
private:
    char *fBuffer; //!

    int   fPort;
    int   fMtu;
    TTime fTimeout;

    TServerSocket  *fServSock;
    TSocket        *fRxSocket;

    void OpenServerSocket(int port);
    void OpenConnection(Bool_t block);

    int underflow();
    int sync();

public:
    MReadSocket(int port=-1, int mtu=1500);
    MReadSocket(MReadSocket const& log) : std::ios(), std::streambuf(), std::istream((std::streambuf*)&log)
    {
    }
    ~MReadSocket();

    Bool_t Open(int port=-1, Bool_t block=kFALSE);
    void   Close();

    void SetTimeout(UInt_t millisec) { fTimeout = millisec; }

    ClassDef(MReadSocket, 0) // This is what we call 'The logging system'
};

#endif
