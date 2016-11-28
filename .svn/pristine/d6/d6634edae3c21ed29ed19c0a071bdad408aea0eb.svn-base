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
!   Author(s): Thomas Bretz  07/2011 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2011
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MRawFitsRead
//
//  This tasks reads the fits data file in the form used by FACT.
//
//  Input Containers:
//   -/-
//
//  Output Containers:
//   MRawRunHeader, MRawEvtHeader, MRawEvtData, MRawEvtTime
//
//////////////////////////////////////////////////////////////////////////////
#include "MRawFitsRead.h"

#include <fstream>

#include <TClass.h>

#include "MLog.h"
#include "MLogManip.h"

#include "factfits.h"
#include "MTime.h"

#include "MArrayB.h"
#include "MArrayS.h"

#include "MParList.h"

#include "MRawRunHeader.h"
#include "MRawEvtHeader.h"
#include "MRawEvtData.h"
#include "MRawBoardsFACT.h"

ClassImp(MRawFitsRead);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. It tries to open the given file.
//
MRawFitsRead::MRawFitsRead(const char *fname, const char *name, const char *title)
    : MRawFileRead(fname, name, title), fRawBoards(0)
{
}

const fits *MRawFitsRead::GetFitsFile() const
{
    return static_cast<const fits*>(GetStream());
}

Int_t MRawFitsRead::PreProcess(MParList *pList)
{
    fRawBoards = (MRawBoardsFACT*)pList->FindCreateObj("MRawBoardsFACT");
    if (!fRawBoards)
        return kFALSE;

    return MRawFileRead::PreProcess(pList);
}

Bool_t MRawFitsRead::LoadMap(const char *name)
{
    fPixelMap.resize(1440);

    if (!name)
        return kTRUE;

    ifstream fin(name);

    int l = 0;

    string buf;
    while (getline(fin, buf, '\n'))
    {
        if (l>1439)
            break;

        const TString bf = TString(buf.c_str()).Strip(TString::kBoth);
        if (bf[0]=='#')
            continue;

        stringstream str(bf.Data());

        int index, cbpx;

        str >> index;
        str >> cbpx;

        const int c =  cbpx/1000;
        const int b = (cbpx/100)%10;
        const int p = (cbpx/10)%10;
        const int x =  cbpx%10;

        const int hw = x+p*9+b*36+c*360;
        if (hw>1439)
            break;

        fPixelMap[hw] = index;
        l++;
    }

    if (l!=1440)
    {
        gLog << err << "ERROR - Problems reading " << name << endl;
        fPixelMap.resize(0);
        return kFALSE;
    }

    return kTRUE;
}

Bool_t MRawFitsRead::IsFits(const char *name)
{
    return factfits(name).good();
}

istream *MRawFitsRead::OpenFile(const char *filename)
{
    return new factfits(filename);
}

Bool_t MRawFitsRead::ReadRunHeader(istream &stream)
{
    factfits &fin = static_cast<factfits&>(stream);

    if (fin.GetStr("TELESCOP")!="FACT")
    {
        gLog << err << "Not a valid FACT FITS file (key TELESCOP not 'FACT')." << endl;
        return kFALSE;
    }

    fIsMc = fin.HasKey("ISMC");

    const string type = fin.GetStr("RUNTYPE");

    fRawRunHeader->SetValidMagicNumber();
    fRawRunHeader->SetNumEvents(fin.GetNumRows());//GetUInt("NAXIS2"));
    fRawRunHeader->InitPixels(fin.GetUInt("NPIX"));
    fRawRunHeader->SetObservation(type=="4294967295"?"":fin.GetStr("RUNTYPE"), "FACT");
    fRawRunHeader->SetRunInfo(0, fin.GetUInt("NIGHT"), fin.GetUInt("RUNID"));
    fRawRunHeader->InitFact(fin.GetUInt("NPIX")/9, 9, fin.GetUInt("NROI"), fPixelMap.size()==0?0:fPixelMap.data());
    fRawRunHeader->SetFormat(0xf172, fIsMc ? 0 : fin.GetUInt("BLDVER"));
    fRawRunHeader->SetRunType(fIsMc ? 0x0100/*mc*/ : 0/*data*/);

    if (!fIsMc)
    {
        const string runstart = fin.GetStr("DATE-OBS");
        const string runstop  = fin.GetStr("DATE-END");

        fRawRunHeader->SetRunTime(MTime(runstart.c_str()), MTime(runstop.c_str()));
    }

    for (int i=0; i<1000; i++)
    {
        const string clnamed = Form("CLNAME%d", i);
        if (!fin.HasKey(clnamed))
            break;

        const string cltyped = Form("CLTYPE%d", i);
        const string clname  = fin.GetStr(clnamed);
        const string cltype  = fin.HasKey(cltyped) ? fin.GetStr(cltyped) : clname;

        MParContainer *par = (MParContainer*)fParList->FindCreateObj(cltype.c_str(), clname.c_str());
        if (par && !par->SetupFits(fin))
        {
            *fLog << err << "ERROR - Setting up " << par->GetDescriptor() << " failed." << endl;
            return kFALSE;
        }
    }

    return
        fin.HasKey("NPIX") && fin.HasKey("RUNID")  &&
        fin.HasKey("NROI") && fin.HasKey("NIGHT");
}

Bool_t  MRawFitsRead::InitReadData(istream &stream)
{
    factfits &fin = static_cast<factfits&>(stream);

    MArrayB **data   = reinterpret_cast<MArrayB**>(fRawEvtData1->DataMember("fHiGainFadcSamples"));
    MArrayS **cell   = reinterpret_cast<MArrayS**>(fRawEvtData1->DataMember("fStartCells"));
    UInt_t   *evtnum = reinterpret_cast<UInt_t*>  (fRawEvtHeader->DataMember("fDAQEvtNumber"));

    // The 'wrong' cast is intentional because we read only two bytes from the file
    UShort_t *trg    = reinterpret_cast<UShort_t*>(fRawEvtHeader->DataMember("fTrigPattern"));

    if (!data || !cell || !evtnum || !trg)
        return kFALSE;

    fRawEvtData1->ResetPixels();
    fRawEvtData2->ResetPixels(0, 0);
    fRawEvtData1->InitStartCells();

    if (!fin.SetRefAddress("EventNum", *evtnum))
        return kFALSE;

    if (!fin.SetRefAddress("TriggerType", *trg))
        return kFALSE;

    if (!fIsMc)
    {
        if (!fin.SetRefAddress("NumBoards", fNumBoards))
            return kFALSE;

        fPCTime.resize(2);
        if (!fin.SetVecAddress("UnixTimeUTC", fPCTime))
            if (!fin.SetVecAddress("PCTime", fPCTime))
                return kFALSE;

        if (!fin.SetPtrAddress("BoardTime", fRawBoards->fFadTime, 40))
            return kFALSE;
    }

    if (!fin.SetPtrAddress("Data", (int16_t*)(*data)->GetArray(), (*data)->GetSize()/2))
        return kFALSE;

    if (!fin.SetPtrAddress("StartCellData", (uint16_t*)(*cell)->GetArray(), (*cell)->GetSize()))
        return kFALSE;

    fRawEvtData1->SetIndices(fRawRunHeader->GetPixAssignment());

    return kTRUE;
}

Bool_t MRawFitsRead::ReadEvent(istream &stream)
{
    if (!static_cast<factfits&>(stream).GetNextRow())
        return kFALSE;

    if (!fIsMc)
    {
        // Skip incomplete events
        if (fNumBoards!=40)
            return kCONTINUE;

        fRawEvtTime->SetUnixTime(fPCTime[0], fPCTime[1]);

    }    
    fRawEvtData1->SetReadyToSave();
    fRawEvtData2->SetReadyToSave();

    return kTRUE;
}

void MRawFitsRead::SkipEvent(istream &fin)
{
    static_cast<factfits&>(fin).SkipNextRow();
}
