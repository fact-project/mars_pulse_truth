/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz, 10/2003 <mailto:tbretz@astro.uni.wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */


//////////////////////////////////////////////////////////////////////////////
//
// MReadSocket
//
// This class acts like a standard C++ istream, but read from a socket
// (ifstream works similar)
//
// ios::io_state:
// --------------
// eof()  or ios::eofbit:  Connection closed or not established
// fail() or ios::failbit: Error trying to establish connection or
//                         waiting for data in underflow() timed out
// good() tells you that everything is ok and we can read from the stream
//
// Example:
// --------
//
//    Double_t d;
//
//    MReadSocket sin(1024); // open port 1024
//
//    sin >> d;
//    sin.read((char*)&d, sizeof(Double_t));
//
//
//////////////////////////////////////////////////////////////////////////////
#include "MReadSocket.h"

#include <unistd.h>          // usleep

#include <TMath.h>           // TMath::Min
#include <TTime.h>           // TTime
#include <TDatime.h>         // TDatime
#include <TSystem.h>         // gSystem
#include <TSocket.h>         // TSocket
#include <TServerSocket.h>   // TServerSocket

ClassImp(MReadSocket);

using namespace std;

// --------------------------------------------------------------------------
//
// You can use the constructor in two ways:
//
//    MReadSocket read(7000);
// This opens the socket and blocks until the connection has been
// established.
//
//    MReadSocket read;
// Returns immidiatly. The connection will be opend by calling
//    read.Open(7000);
//
MReadSocket::MReadSocket(int port, int mtu) : istream(this), fMtu(mtu), fTimeout(2500), fServSock(NULL), fRxSocket(NULL)
{
    fBuffer = new char[mtu];

    setg(fBuffer, fBuffer, fBuffer+1);

    clear(ios::eofbit);

    if (port>0)
        Open(port);
}

// --------------------------------------------------------------------------
//
//  Destructor. Close an possible open connection and delete the fBuffer
//
MReadSocket::~MReadSocket()
{
    Close();
    delete fBuffer;
}

void MReadSocket::OpenServerSocket(int port)
{
    if (fServSock)
        return;

    cout << "Starting server socket on port #" << port << "..." << endl;

    while (!fServSock)
    {
        fServSock=new TServerSocket(port, kTRUE);
        if (fServSock->IsValid())
            continue;

        cout << "ServerSocket not valid: ";
        switch (fServSock->GetErrorCode())
        {
        case  0: cout << "No error." << endl; break;
        case -1: cout << "low level socket() call failed." << endl; break;
        case -2: cout << "low level bind() call failed." << endl; break;
        case -3: cout << "low level listen() call failed." << endl; break;
        default: cout << "Unknown." << endl; break;
        }

        Close();
        clear(ios::failbit);
        return;
    }

    fServSock->SetOption(kNoBlock, 1);
}

void MReadSocket::OpenConnection(Bool_t block)
{
    do
    {
        const TTime timeout = gSystem->Now() + TTime(5000);

        cout << TDatime().AsString() << ": Waiting for connection..." << endl;

        //
        // fRxSocket<0  means: No connection,non-blocking mode
        // fRxSocket==0 means: Error
        // This is only done until timeout is reached
        //
        while (fRxSocket==0 && gSystem->Now()<timeout)
        {
            fRxSocket = fServSock->Accept();
            if (fRxSocket==0)
            {
                cout << "MReadSocket::OpenConnection: ERROR - TServerSock::Accept()" << endl;
                setstate(ios::failbit);
                return;
            }
            if ((Long_t)fRxSocket<0)
                fRxSocket=NULL;

            usleep(10);
        }

        //
        // No connection has been established. Restart waiting for
        // connection except we are in non-blocking mode.
        //
        if (fRxSocket==0)
            continue;

        //
        // Check if the established connection is valid
        //
        if (fRxSocket->IsValid())
        {
            cout << "Connection established..." << endl;
            fRxSocket->SetOption(kNoBlock, 1);
            clear();
            return;
        }

        cout << "TSocket: Connection not valid..." << endl;
        delete fRxSocket;
        fRxSocket=NULL;
        setstate(ios::failbit);
        return;

    } while (block);
}

// --------------------------------------------------------------------------
//
//  Open the connectionj on port port. Wait until the connection has
//  been established. If an error occures and the connection cannot
//  be established return kFALSE. To check whether an error occured
//  use operator!() or operator void*() or fail()
//
Bool_t MReadSocket::Open(int port, Bool_t block)
{
    //
    // If no port is given use the port given in the constructor
    //
    if (port<=0)
        port = fPort;

    //
    // Remember port for later uses
    //
    if (fPort<=0)
        fPort = port;

    //
    // Check whether a connection has already been established
    //
    if (fServSock)
    {
        //
        // Check whether the connection has the right port
        //
        if (fServSock->GetLocalPort()!=port)
            Close();
    }

    //
    // Check whether port is valid
    //
    if (port<=0)
    {
        cout << "Invalid port #" << port << "!" << endl;
        clear(ios::failbit);
        return kFALSE;
    }

    //
    // Start server socket...
    //
    OpenServerSocket(port);
    if (!fServSock)
        return kFALSE;

    OpenConnection(block);
    if (!fRxSocket)
        return kFALSE;

    underflow();
    return kTRUE;
}

void MReadSocket::Close()
{
    if (fRxSocket)
    {
        delete fRxSocket;
        fRxSocket=NULL;
    }
    if (fServSock)
    {
        const Int_t port = fServSock->GetLocalPort();

        delete fServSock;
        fServSock=NULL;

        cout << "Connection on Port #" << port << " closed." << endl;
    }

    clear(ios::eofbit);
}

// --------------------------------------------------------------------------
//
// This is called to flush the buffer of the streaming devices
//
int MReadSocket::sync()
{
    cout << "sync" << endl;
    return 0;
}

int MReadSocket::underflow()
{
    // FIXME:     vvvvv is this correct?
    if (fail() || eof())
    {
        setg(fBuffer, fBuffer, fBuffer+fMtu);
        return 0;
    }

    //
    // This simple trick should do its job, because the
    // TCP/IP stream is buffered already
    //
    const TTime timeout = fTimeout+gSystem->Now();

    Int_t len=-1;
    while (len<0 && gSystem->Now()<timeout)
    {
        Int_t l;
        fRxSocket->GetOption(kBytesToRead, l);
        if (l==0)
        {
            gSystem->Sleep(1);
            continue;
        }
        len = fRxSocket->RecvRaw(fBuffer, TMath::Min(fMtu, l));
    }

    if (len<0)
    {
        cout << "MReadSocket: TSocket::RecvRaw - Connection timed out." << endl;
        setstate(ios::failbit);
        memset(fBuffer, 0, fMtu);
        len = fMtu;
    }

    setg(fBuffer, fBuffer, fBuffer+len);
    return 0;
}
