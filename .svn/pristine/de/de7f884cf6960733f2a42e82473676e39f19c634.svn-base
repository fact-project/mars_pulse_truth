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
!   Author(s): Thomas Bretz  11/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
               Reiner Rohlfs 11/2010 <mailto:Reiner.Rohlfs@unige.ch>
!
!   Copyright: Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MCorsikaRead
//
//  Input Containers:
//   -/-
//
//  Output Containers:
//   MCorsikaRunHeader
//   MCorsikaEvtHeader
//   MPhotonEvent
//
//////////////////////////////////////////////////////////////////////////////
#include "MCorsikaRead.h"

#include <errno.h>
#include <fstream>

#include <TSystem.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MStatusDisplay.h"

#include "MCorsikaFormat.h"
#include "MCorsikaRunHeader.h"
#include "MCorsikaEvtHeader.h"

#include "MPhotonEvent.h"

ClassImp(MCorsikaRead);

using namespace std;


/*  ----------- please don't delete and don't care about (Thomas) ------------
#define kBUFSZ 64 //1024*1024*64
#include <iomanip.h>
class bifstream : public istream, public streambuf
{
private:
    char fBuffer[kBUFSZ]; //!
    FILE *fd;

    int sync()
    {
        memset(fBuffer, 0, kBUFSZ);
        return 0; 
    }
    int underflow()
    {
        int sz=fread(fBuffer, kBUFSZ, 1, fd);
        //int sz=fread(fBuffer, 1, kBUFSZ, fd);
        setg(fBuffer, fBuffer, fBuffer+kBUFSZ);

        return sz==1 ? *(unsigned char*)fBuffer : EOF;//EOF;
        //return sz==kBUFSZ ? *(unsigned char*)fBuffer : EOF;//EOF;
    }
public:
    bifstream(const char *name) : istream(this)
    {
        fd = fopen(name, "rb");
        setbuf(fBuffer, kBUFSZ);
    }
};
*/

// --------------------------------------------------------------------------
//
// Default constructor. It tries to open the given file.
//
MCorsikaRead::MCorsikaRead(const char *fname, const char *name, const char *title)
    : fRunHeader(0), fEvtHeader(0), fEvent(0), fTelescopeIdx(-1), fArrayIdx(-1), 
    fForceMode(kFALSE), fFileNames(0), fNumFile(0), fNumEvents(0), 
    fNumTotalEvents(0), fInFormat(0), fParList(0), fNumTelescopes(1), fReadState(0)
{
    fName  = name  ? name  : "MRead";
    fTitle = title ? title : "Read task to read DAQ binary files";

    fFileNames = new TList;
    fFileNames->SetOwner();

    if (fname!=NULL)
        AddFile(fname);
}

// --------------------------------------------------------------------------
//
// Destructor. Delete input stream.
//
MCorsikaRead::~MCorsikaRead()
{
    delete fFileNames;
    if (fInFormat)
        delete fInFormat;
}

/*
Byte_t MCorsikaRead::IsFileValid(const char *name)
{
    MZlib fin(name);
    if (!fin)
        return 0;

    Byte_t c[4];
    fin.read((char*)c, 4);
    if (!fin)
        return 0;

    if (c[0]!=0xc0)
        return 0;

    if (c[1]==0xc0)
        return 1;

    if (c[1]==0xc1)
        return 2;

    return 0;
}
*/

// --------------------------------------------------------------------------
//
// Add a new file to a list of files to be processed, Returns the number
// of files added. (We can enhance this with a existance chack and
// wildcard support)
//
Int_t MCorsikaRead::AddFile(const char *fname, Int_t entries)
{
    TNamed *name = new TNamed(fname, "");
    fFileNames->AddLast(name);
    return 1;

}
// --------------------------------------------------------------------------
//
// This opens the next file in the list and deletes its name from the list.
//
Int_t MCorsikaRead::OpenNextFile(Bool_t print)
{

    //
    // open the input stream and check if it is really open (file exists?)
    //
    if (fInFormat)
       delete fInFormat;
    fInFormat = NULL;

    //
    // Check for the existance of a next file to read
    //
    TObject *file = fFileNames->At(fNumFile);
    if (!file)
        return kFALSE;

    //
    // open the file which is the first one in the chain
    //
    const char *name = file->GetName();

    const char *expname = gSystem->ExpandPathName(name);
    fInFormat = MCorsikaFormat::CorsikaFormatFactory(expname);
    delete [] expname;

    if (fInFormat == NULL)
        return kERROR;

    *fLog << inf << "Open file: '" << name << "'" << endl;

    if (fDisplay)
    {
       // Show the number of the last event after
       // which we now open a new file
       TString txt = GetFileName();
       txt += " @ ";
       txt += GetNumExecutions()-1;
       fDisplay->SetStatusLine2(txt);
    }

    fNumFile++;

    if (!fParList)
        return kTRUE;

    //
    // Search for MTaskList
    //
    MTask *tlist = (MTask*)fParList->FindObject("MTaskList");
    if (!tlist)
    {
        *fLog << err << dbginf << "MTaskList not found... abort." << endl;
        return kERROR;
    }

    //
    // A new file has been opened and new headers have been read.
    //  --> ReInit tasklist
    //
    return tlist->ReInit(fParList) ? kTRUE : kERROR;
}

// --------------------------------------------------------------------------
//
// Return file name of current file.
//
TString MCorsikaRead::GetFullFileName() const
{
    const TObject *file = fFileNames->At(fNumFile-1);
    return file ? file->GetName() : "";
}

// --------------------------------------------------------------------------
//
// Restart with the first file
//
Bool_t MCorsikaRead::Rewind()
{
    fNumFile=0;
    fNumEvents=0;
    return OpenNextFile()==kTRUE;
}

// --------------------------------------------------------------------------
//
// Calculates the total number of events from all input files and stores
// the number in fNumTotalEvents.                                       
//
Bool_t MCorsikaRead::CalcNumTotalEvents()
{
    fNumTotalEvents = 0;

    Bool_t rc = kTRUE;

    while (1)
    {
        switch (OpenNextFile(kFALSE))
        {
        case kFALSE:
            break;
        case kERROR:
            rc = kFALSE;
            break;
        case kTRUE:

            // read run header(1200), telescope position(1201) and 
            // first event header(1202)
            Bool_t status = kTRUE;
            Int_t blockType, blockVersion, blockIdentifier, blockLength;
            while (status && 
                   fInFormat->NextBlock(fReadState/* == 4*/, blockType, blockVersion, 
                              blockIdentifier, blockLength))
               {
               if (blockType == 1200)
                  status = fRunHeader->ReadEvt(fInFormat);

               else if(blockType == 1201)
                   status = ReadTelescopePosition();

               else if (blockType == 1202)
                  {
                  Float_t buffer[272];
                  status = fInFormat->Read(buffer, 272 * sizeof(Float_t));
                  status = fRunHeader->ReadEventHeader(buffer);
                  break;
                  }
               else
                  fInFormat->Seek(blockLength);
               }
                  
            if (status != kTRUE)
               return status;

            if (!fInFormat->SeekEvtEnd())
            {
               *fLog << (fForceMode?warn:err) << "Error: RUNE section not found in file." << endl;
               if (!fForceMode)
                  return fForceMode ? kTRUE : kFALSE;
            }

            if (!fRunHeader->ReadEvtEnd(fInFormat, kTRUE))
            {
               *fLog << (fForceMode?warn:err) << "Error: Reading RUNE section failed." << endl;
               if (!fForceMode)
                  return kFALSE;
            }

            fNumTotalEvents += fRunHeader->GetNumEvents()*fRunHeader->GetNumReuse()*
                (fTelescopeIdx<0 ? fNumTelescopes : 1);
            continue;
        }
        break;
    }

    return rc;
}

// --------------------------------------------------------------------------
//
// Reads the the position of all telescopes in one array
//
Int_t MCorsikaRead::ReadTelescopePosition()
{
   if (!fInFormat->Read(&fNumTelescopes, 4)) return kERROR;

   if (fTelescopeIdx>=fNumTelescopes)
   {
      *fLog << err << "ERROR - Requested telescope index " << fTelescopeIdx << 
            " exceeds number of telescopes " << fNumTelescopes << " in file." << endl;
      return kERROR;
   }

   fTelescopeX.Set(fNumTelescopes);
   fTelescopeY.Set(fNumTelescopes);
   fTelescopeZ.Set(fNumTelescopes);
   fTelescopeR.Set(fNumTelescopes);

   if (!fInFormat->Read(fTelescopeX.GetArray(), 4*fNumTelescopes)) return kERROR;
   if (!fInFormat->Read(fTelescopeY.GetArray(), 4*fNumTelescopes)) return kERROR;
   if (!fInFormat->Read(fTelescopeZ.GetArray(), 4*fNumTelescopes)) return kERROR;
   if (!fInFormat->Read(fTelescopeR.GetArray(), 4*fNumTelescopes)) return kERROR;

   *fLog << all;
   *fLog << "Number of telescopes: " << fNumTelescopes << " (using ";
   if (fTelescopeIdx>=0)
      *fLog << "telescope " << fTelescopeIdx;
   else
      *fLog << "all telescopes";
   *fLog << ")" << endl;

   const Int_t lo = fTelescopeIdx<0 ? 0              : fTelescopeIdx;
   const Int_t up = fTelescopeIdx<0 ? fNumTelescopes : fTelescopeIdx+1;

   for (int i=lo; i<up; i++)
   {
      *fLog << all << "Telescope #" << setw(4) << i << " [X/Y/Z (R)]: ";
      *fLog << setw(7) << fTelescopeX[i] << "/";
      *fLog << setw(7) << fTelescopeY[i] << "/";
      *fLog << setw(7) << fTelescopeZ[i] << "  (R=" << setw(4) << fTelescopeR[i] << ")" << endl;
   }

   fNumTelescope = 0;

   return kTRUE;
}

// --------------------------------------------------------------------------
//
// Reads the header of the next block. Depending on the current fReadState
// and the block-type of the new header the methods decides wether 
// to continue the reading (kCONTINEU), to exit the Process() method (kTRUE)
// or to try to read a new file (kFALSE).
// Return codes:
//   - kFALSE :   end of file was detected and no new header was read
//   - kTRU:      A EventEnd was already found (fReadStatus == 3) and
//                the current header is not the RunEnd
//   - kCONTINUE: all other cases.
Int_t MCorsikaRead::ReadNextBlockHeader()
{
   if (fInFormat->NextBlock(fReadState/* == 4*/, fBlockType, fBlockVersion, 
                            fBlockIdentifier, fBlockLength)               == kFALSE)
      // end of file
      return kFALSE;
/*
    gLog << "Next fBlock:  type=" << fBlockType << " version=" << fBlockVersion;
    gLog << " identifier=" << fBlockIdentifier << " length=" << fBlockLength; 
    gLog << " readState= " << fReadState << endl;
*/
   if (fReadState == 3 && fBlockType != 1210)
      // fReadState == 3    means we have read the event end 
      //                    most of the time we can now save our data. BUT:
      // bBlockType != 1210 means the next block is not RUNE, 
      //                    we want to read the RUNE block, before we 
      //                    save everything (last events and RUNE) 
      return kTRUE;

   if (fBlockType == 1204 && fReadState != 2)
      // next is a new set of telescope arrays, we store the previous ones
      // but not if this is the first one (fReadState != 2)
      return kTRUE;

   return kCONTINUE;
  
}

// --------------------------------------------------------------------------
//
// The PreProcess of this task checks for the following containers in the
// list:
//   MCorsikaRunHeader <output>   if not found it is created
//   MCorsikaEvtHeader <output>   if not found it is created
//   MCorsikaEvtData <output>     if not found it is created
//   MCorsikaCrateArray <output>  if not found it is created
//   MCorsikaEvtTime <output>     if not found it is created (MTime)
//
// If all containers are found or created the run header is read from the
// binary file and printed.  If the Magic-Number (file identification)
// doesn't match we stop the eventloop.
//
// Now the EvtHeader and EvtData containers are initialized.
//
Int_t MCorsikaRead::PreProcess(MParList *pList)
{
    //
    // open the input stream
    // first of all check if opening the file in the constructor was
    // successfull
    //
    fParList = 0;

    //
    //  check if all necessary containers exist in the Parameter list.
    //  if not create one and add them to the list
    //
    fRunHeader = (MCorsikaRunHeader*)pList->FindCreateObj("MCorsikaRunHeader");
    if (!fRunHeader)
        return kFALSE;

    fEvtHeader = (MCorsikaEvtHeader*)pList->FindCreateObj("MCorsikaEvtHeader");
    if (!fEvtHeader)
        return kFALSE;

    fEvent = (MPhotonEvent*)pList->FindCreateObj("MPhotonEvent");
    if (!fEvent)
        return kFALSE;

    *fLog << inf << "Calculating number of total events..." << flush;
    if (!CalcNumTotalEvents())
        return kFALSE;
    *fLog << inf << " " << fNumTotalEvents << " found." << endl;

    fNumFile=0;
    fNumEvents=0;

    fParList = pList;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The Process reads one event from the binary file:
//  - The event header is read
//  - the run header is read (only once per file)
//  - the raw data information of one event is read
//
Int_t MCorsikaRead::Process()
{
   while (1)  // loop for multiple input files
   {
      if (fInFormat)
      {

      while (1)  // loop per input block
         {

         if (fReadState == 4)
            {
            fTopBlockLength -= fBlockLength + 12;
            if (fTopBlockLength <= 0)
               // all data of a top block are read, go back to normal state
               fReadState = 15; // not used
            }


         Int_t status = kTRUE;
         switch (fBlockType)
            {
            case 1200:       // the run header
               status = fRunHeader->ReadEvt(fInFormat);
               fReadState = 1;  // RUNH is read
               break;

            case 1201:       // telescope positions
               status = ReadTelescopePosition();
               break;

            case 1202:  // the event header
               Float_t buffer[272];
               if (!fInFormat->Read(buffer, 272 * sizeof(Float_t))) 
                  return kFALSE;

               if (fReadState == 1)  // first event after RUN header
                  {
                  fRunHeader->ReadEventHeader(buffer);
                  fRunHeader->Print();
                  }

               status = fEvtHeader->ReadEvt(buffer);
               if (fArrayIdx >= (Int_t)fEvtHeader->GetTotReuse())
                  {
                  *fLog << err << "ERROR - Requested array index " << fArrayIdx << 
                        " exceeds number of arrays " << fEvtHeader->GetTotReuse() << 
                        " in file." << endl;
                  return kERROR;
                  }

               fReadState = 2;
               break;

            case 1203: // 16 bytes
                fInFormat->Seek(fBlockLength);
                break;

            case 1212:
                {
                    char *buf = new char[fBlockLength];
                    fInFormat->Read(buf, fBlockLength);
                    status = kTRUE;

                    char *ptr = buf;

                    unsigned int n = ((int*)ptr)[0];
                    ptr += 4;

                    *fLog << all << endl;

                    for (unsigned int i=0; i<n && ptr<buf+fBlockLength; i++)
                    {
                        unsigned short s = ((unsigned short*)ptr)[0];
                        ptr += 2;

                        *fLog << string(ptr, ptr+s) << '\n';
                        ptr += s;
                    }
                    *fLog << '\n' << endl;

                    delete [] buf;
                }
                break;


            case 1204: // top level block for one array (only for eventio data)
               if (fArrayIdx < 0 || fArrayIdx == fBlockIdentifier)
                  { 
                  fReadState = 4;
                  fTopBlockLength = fBlockLength;
                  }
               else
                  // skip this array of telescopes
                  fInFormat->Seek(fBlockLength);

               break;


            case 1205: // eventio data
               {
               Int_t telIdx   = fBlockIdentifier % 1000;
               if ((fBlockVersion == 0 || fBlockVersion == 1000)                              &&
                     (fTelescopeIdx < 0 || fTelescopeIdx ==  telIdx)     )
                  {
                  status = fBlockVersion==0 ? fEvent->ReadEventIoEvt(fInFormat) : fEvent->ReadEventIoEvtCompact(fInFormat);

                  Int_t arrayIdx = fBlockIdentifier / 1000;
                  Float_t xArrOff, yArrOff;
                  fEvtHeader->GetArrayOffset(arrayIdx, xArrOff, yArrOff);
                  fEvtHeader->SetTelescopeOffset(arrayIdx, 
                                                   xArrOff + fTelescopeY[telIdx], 
                                                   yArrOff - fTelescopeX[telIdx] );
                  fEvent->AddXY(xArrOff + fTelescopeY[telIdx], 
                                 yArrOff - fTelescopeX[telIdx]);
                  fEvent->SimWavelength(fRunHeader->GetWavelengthMin(), 
                                          fRunHeader->GetWavelengthMax());
                  }
               else
                  // skip this telescope
                  fInFormat->Seek(fBlockLength);
               }
               break;

            case 1209:  // the event end
               status = fEvtHeader->ReadEvtEnd(fInFormat);

               if (fReadState == 10 || fReadState == 2)
                  {
                  // corsika events
                  fEvtHeader->ResetNumReuse();
                  fEvtHeader->InitXY();
                  fBlockType = 1109;  // save corsika events
                  continue;
                  }

               fReadState = 3;  // event closed, run still open
               break;


            case 1210:  // the run end
               status = fRunHeader->ReadEvtEnd(fInFormat, kTRUE);
               fNumEvents += fRunHeader->GetNumEvents();
               //fRunHeader->SetReadyToSave();
               fReadState = 5;           // back to  starting point
               // this may be the last block in the file. 
               // We exit to write the header into the file
               fBlockLength = 0;
               fBlockType   = 0; // not available type 
               return kTRUE;
               break;


            case 1105:  // event block of raw format
               if (fReadState == 2 || fReadState == 10)
                  {
                  Int_t oldSize = fRawEvemtBuffer.size();
                  fRawEvemtBuffer.resize(oldSize + fBlockLength / sizeof(Float_t));
                  status = fInFormat->Read(&fRawEvemtBuffer[oldSize], fBlockLength); 
                  fReadState = 10;
                  }
               else
                  fInFormat->Seek(fBlockLength);
               break;


            case 1109:  // save corsika events
               fEvtHeader->InitXY();
               status = fEvent->ReadCorsikaEvt(&fRawEvemtBuffer[0], 
                                               fRawEvemtBuffer.size() / 7, 
                                               fEvtHeader->GetNumReuse()+1);
               fEvtHeader->IncNumReuse();

               if (fEvtHeader->GetNumReuse() == fRunHeader->GetNumReuse())
                  {
                  // this was the last reuse. Set fBlockType to EVTE to save
                  // it the next time.
                  fRawEvemtBuffer.resize(0);

                  fReadState = 3;
                  fBlockType = 1209;
                  }
               else
                  // save this reuse
                  return status;

               break;               

            default:
               // unknown block, we skip it
               fReadState = 15;
               fInFormat->Seek(fBlockLength);

            }

         if (status != kTRUE)
            // there was an error while data were read
            return status;

         Int_t headerStatus =  ReadNextBlockHeader();
         if (headerStatus == kFALSE) 
            // end of file
            break;
         if (headerStatus == kTRUE)
            // we shall quit this loop
            return kTRUE;

            // else continue
         }

      }

      //
      // If an event could not be read from file try to open new file
      //
      const Int_t rc = OpenNextFile();
      if (rc!=kTRUE)
          return rc;

      if (ReadNextBlockHeader() == kFALSE)
         return kFALSE;
   }
   return kTRUE;
}


// --------------------------------------------------------------------------
//
//  Close the file. Check whether the number of read events differs from
//  the number the file should containe (MCorsikaRunHeader). Prints a warning
//  if it doesn't match.
//
Int_t MCorsikaRead::PostProcess()
{

    const UInt_t n = fNumTotalEvents;//fNumEvents*fRunHeader->GetNumReuse()*(fTelescopeIdx<0?fNumTelescopes:1);

    //
    // Sanity check for the number of events
    //
    if (n==GetNumExecutions()-1 || GetNumExecutions()==0)
        return kTRUE;

    *fLog << warn << dec;
    *fLog << "Warning - Number of read events (" << GetNumExecutions()-1;
    *fLog << ") doesn't match number in RUNE section(s) (" << n << ")";
    //*fLog << fRunHeader->GetNumReuse() << "*" << fNumEvents << "=" << n << ")." << endl;


    return kTRUE;
}

