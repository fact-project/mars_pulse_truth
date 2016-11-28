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
!   Author(s): Reiner Rohlfs 2010
!   Author(s): Thomas Bretz  2010 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: Software Development, 2000-2010
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MCorsikaFormat
//
//////////////////////////////////////////////////////////////////////////////
#include "MCorsikaFormat.h"

#include <errno.h>
#include <fstream>

#include "MLogManip.h"


using namespace std;


const unsigned int MCorsikaFormat::kMagicNumber = 0x5994;
const unsigned int MCorsikaFormat::kSyncMarker  = 0xd41f8a37;

// --------------------------------------------------------------------------
//
MCorsikaFormat *MCorsikaFormat::CorsikaFormatFactory(const char * fileName)
{
    ifstream * fileIn = new ifstream(fileName);

    const Bool_t noexist = !(*fileIn);
    if (noexist)
    {
        gLog << err << "Cannot open file " << fileName << ": ";
        gLog << (errno!=0?strerror(errno):"Insufficient memory for decompression") << endl;
        delete fileIn;
        return NULL;
    }

    char *buffer = new char[5];
    memset(buffer, 0, 5);
    fileIn->read(buffer, 4);

    // This seems to be a new corsika binary identifier
    const bool hasMagicNumber = *reinterpret_cast<unsigned int*>(buffer) == kMagicNumber;
    if (hasMagicNumber)
        fileIn->read(buffer, 4);

    fileIn->seekg(-4, ios::cur);

    if (strcmp(buffer, "RUNH") == 0)
    {
        delete [] buffer;
        gLog << inf2 << "Corsika RAW format detected." << endl;
        return new MCorsikaFormatRaw(fileIn, hasMagicNumber);
    }

    if (*reinterpret_cast<unsigned int*>(buffer) == kSyncMarker)
    {
        delete [] buffer;
        gLog << inf2 << "Corsika EventIO format detected." << endl;
        return new MCorsikaFormatEventIO(fileIn);
    }

    gLog << err << "File " << fileName <<
            " is neither a CORSIKA raw nor EventIO file" << endl;
    delete fileIn;
    delete [] buffer;

    return NULL;
}

Bool_t MCorsikaFormat::Read(void *ptr, Int_t i) const
{
   fIn->read((char*)ptr, i);
   return !fIn->fail();

}
// --------------------------------------------------------------------------
//
Bool_t MCorsikaFormat::Eof() const
{
    return fIn->eof();
}

// --------------------------------------------------------------------------
//
MCorsikaFormat::~MCorsikaFormat() 
{
    delete fIn;
}


// --------------------------------------------------------------------------
//
// After a call to this function, the file pointer is located after the   
// header of the block. As the event block has no header it is located    
// at the beginning of this block, which is as the beginning of the data  
Bool_t MCorsikaFormatRaw::NextBlock(Int_t   readState,
                                    Int_t & blockType, 
                                    Int_t & blockVersion,
                                    Int_t & blockIdentifier, 
                                    Int_t & blockLength) const
{

    int blockHeader = 22932;
    while (blockHeader==22932)
    {
        fIn->read((char*)&blockHeader, 4);
        if (fIn->eof())
            return kFALSE;
    }

    blockVersion = 0;
    blockIdentifier = 0;
    blockLength     = 272 * 4;

    switch(blockHeader)
      {
      case 1213093202 : // RUNH
         blockType = 1200;
         break;
      case 1162761554 : // RUNE
         blockType = 1210;
         break;
      case 1213486661 : // EVTH
         if (readState != 10)
            blockType = 1202;
         else
            {
            blockType = 1105;
            fIn->seekg(-4, ios::cur);
            blockLength += 4;
            }
         break;
      case 1163155013 : // EVTE
         blockType = 1209;
         break;
      default:          // the events, they don't have a specific header
         blockType = 1105;
         fIn->seekg(-4, ios::cur);
         blockLength += 4;
      }            
    return kTRUE;
}
// --------------------------------------------------------------------------
//
Bool_t MCorsikaFormatRaw::SeekEvtEnd()
{
    // Search subblockwise backward (Block: 5733*4 = 21*273*4)
    for (int i=1; i<22; i++)
    {
        fIn->seekg(-i*273*4-(fHasMagicNumber?4:0), ios::end);

        char runh[5]="\0\0\0\0";
        fIn->read(runh, 4);

        if (!strcmp(runh, "RUNE"))
        {
//            fIn->seekg(-4, ios::cur);
            return kTRUE;
        }
    }

    return kFALSE;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Bool_t MCorsikaFormatEventIO::NextBlock(Int_t   readState,
                                        Int_t & blockType, 
                                        Int_t & blockVersion,
                                        Int_t & blockIdentifier, 
                                        Int_t & blockLength) const
{
// we read - synchronisation markerif not subBlock
//         - type / version field
//         - identification field
//         - length
//         - unknown field
//         - id (first 4 bytes of data field)

   if (readState == 4)
//   if (subBlock)
      {
      // this is a sub-block
      int blockHeader[3];
      fIn->read((char*)blockHeader, 3 * sizeof(int));

      if (fIn->eof())
         return kFALSE;


      blockType       = blockHeader[0] & 0xFFFF;
      blockVersion    = (blockHeader[0] & 0xFFF00000) >> 20;
      blockIdentifier = blockHeader[1];
      blockLength     = blockHeader[2] & 0x3FFFFFFF;
      }
   else
      {
       int blockHeader[4];
       fIn->read((char*)blockHeader, 4 * sizeof(int));

       if (fIn->eof())
           return kFALSE;


       blockType       = blockHeader[1] & 0xFFFF;
       blockVersion    = (blockHeader[1] & 0xFFF00000) >> 20;
       blockIdentifier = blockHeader[2];
       blockLength     = blockHeader[3] & 0x3FFFFFFF;

       if (blockType == 1200  || blockType == 1210 ||
           blockType == 1202  || blockType == 1209    )
           {
           // read the "old" corsika header plus additional 4 unknown byte
           char tmp[8];
           fIn->read(tmp, 8);
           blockLength -= 8;
           }
    
      }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
Bool_t MCorsikaFormatEventIO::SeekEvtEnd()
{

    // the RUNE block it at the very end of the file.
    fIn->seekg(-32, ios::end);

    unsigned int blockHeader[4];
    fIn->read((char*)blockHeader, 4 * sizeof(int));

    if ( blockHeader[0]               == kSyncMarker &&
        (blockHeader[1] & 0xffff)     == 1210        &&
        (blockHeader[3] & 0x3fffffff) == 16)
    {
        // this seams to be a RUNE (1210)  block
        fIn->seekg( 8, ios::cur);
        return kTRUE;
    }

    return kFALSE;
}

