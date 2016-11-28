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
!   Author(s): Thomas Bretz, 12/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */


//////////////////////////////////////////////////////////////////////////////
//
// MBzlib2
//
// This is a C++ wrapper for zlib (taken from root)
//
// WARNING: - There might not be support for all features.
//          - seek calls might be rather slow
//
//////////////////////////////////////////////////////////////////////////////
#include "MBzlib2.h"

#include <string.h>

ClassImp(MBzlib2);

using namespace std;

// --------------------------------------------------------------------------
//
// Open a file by name. Test if it is open like for an ifstream
// It doesn't matter whether the file is gzip compressed or not.
//
void MBzlib2::open(const char* name)
{
    if (is_open())
    {
        clear(rdstate()|ios::badbit);
        return;
    }

    fFile = BZ2_bzopen(name, "rb0");
    if (fFile == 0)
    {
        clear(rdstate()|ios::badbit);
        return;
    }
}

// --------------------------------------------------------------------------
//
// Close an open file.
//
void MBzlib2::close()
{
    if (!is_open())
        return;

    sync();

    BZ2_bzclose(fFile);
//    if (BZ2_bzclose(fFile) != Z_OK)
//        clear(rdstate()|ios::badbit);

    fFile = 0;
}

// --------------------------------------------------------------------------
//
int MBzlib2::underflow()
{
    if (gptr() && gptr()<egptr())
        return * reinterpret_cast<unsigned char *>(gptr());

    if (!is_open())
        return EOF;

    // implementation of inbuf
    const int putback = gptr()-eback()>4 ? 4 : gptr()-eback();

    memcpy(fBuffer+(4-putback), gptr()-putback, putback);

    const int num = BZ2_bzread(fFile, fBuffer+4, fgBufferSize-4);
    if (num <= 0) // ERROR or EOF
        return EOF;

    // reset buffer pointers
    setg(fBuffer+(4-putback), fBuffer+4, fBuffer+4+num);

    // return next character
    return *reinterpret_cast<unsigned char *>(gptr());
}

// --------------------------------------------------------------------------
//
int MBzlib2::flush_buffer()
{
    // Separate the writing of the buffer from overflow() and sync() operation.
    const int w = pptr() - pbase();

    if (BZ2_bzwrite(fFile, pbase(), w) != w)
        return EOF;

    pbump(-w);

    return w;
}

/*
int MBzlib2::overflow( int c)
{
    if ( ! ( mode & ios::out) || ! opened)
        return EOF;

    if (c != EOF)
    {
        *pptr() = c;
        pbump(1);

    }
    if ( flush_buffer() == EOF)
        return EOF;

    return c;
}
*/

// --------------------------------------------------------------------------
//
int MBzlib2::sync()
{
    // Use flush_buffer() instead of overflow(EOF) to
    // cause proper behavior with std::endl and flush()
    if (pptr() && pptr()>pbase())
    {
        if (flush_buffer() == EOF)
            return -1;
    }
    return 0;
}

// --------------------------------------------------------------------------
//
streambuf::pos_type MBzlib2::seekoff(streambuf::off_type offset, ios_base::seekdir dir, ios_base::openmode)
{
    char *c = new char[offset];
    int rc = BZ2_bzread(fFile, c, offset);
    delete c;
    return rc;
//    return gzseek(fFile, offset, dir);
}
