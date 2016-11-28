#ifndef MARS_MBzlib2
#define MARS_MBzlib2

#ifndef ROOT_TObject
#include <TObject.h>
#endif

#ifdef __CINT__
typedef void *BZFILE;
#else
#include <bzlib.h>
#endif

#include <iostream>  // base classes for MLog

class MBzlib2 : public std::streambuf, virtual public std::istream, public TObject
{
private:
    static const int fgBufferSize = 47+256; // size of data buff totals 512 bytes under g++ for igzstream at the end.

    BZFILE *fFile;                 // file handle for compressed file
    char    fBuffer[fgBufferSize]; // data buffer

    int flush_buffer();
    int underflow();
    int sync();

public:
    MBzlib2() : istream(this), fFile(0)
    {
        setp(fBuffer,   fBuffer+fgBufferSize-1);
        setg(fBuffer+4, fBuffer+4, fBuffer+4);
    }
    MBzlib2(const char *name) : istream(this), fFile(0)
    {
        setp(fBuffer,   fBuffer+fgBufferSize-1);
        setg(fBuffer+4, fBuffer+4, fBuffer+4);

        open(name);
    }
    ~MBzlib2() { MBzlib2::close(); }

    int is_open() { return fFile!=0; }

    void open(const char* name);
    void close();

    std::streambuf::pos_type seekoff(std::streambuf::off_type, std::ios_base::seekdir,
                                     std::ios_base::openmode = std::ios_base::in | std::ios_base::out);
    //std::streambuf::pos_type seekpos(std::streambuf::pos_type,
    //                                 std::ios_base::openmode = std::ios_base::in | std::ios_base::out);

    ClassDef(MBzlib2, 0) // A C++ wrapper to istream zlib files
};

#endif
