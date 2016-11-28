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
!   Author(s): Thomas Bretz, 5/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MReadRflFile
//
// Reads a output file of the reflector program
//
/////////////////////////////////////////////////////////////////////////////
#include "MReadRflFile.h"

#include "structures_rfl.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MZlib.h"

#include "MParList.h"

#include "MRflEvtData.h"
#include "MRflEvtHeader.h"
#include "MRflRunHeader.h"
#include "MRflSinglePhoton.h"


ClassImp(MReadRflFile);

using namespace std;

// ------------------------------------------------
const char PROGNAME[] = "reflector";
#define SIZE_OF_FLAGS           13
#define SIZE_OF_SIGNATURE       13
#define FLAG_START_OF_RUN       "\nSTART---RUN\n"
#define FLAG_START_OF_EVENT     "\nSTART-EVENT\n"
#define FLAG_END_OF_EVENT       "\nEND---EVENT\n"
#define FLAG_END_OF_RUN         "\nEND-----RUN\n"
#define FLAG_END_OF_FILE        "\nEND----FILE\n"
#define FLAG_END_OF_STDIN       "\nEND---STDIN\n"
// ------------------------------------------------

Bool_t MReadRflFile::FlagIsA(const  char *s1, const char *flag)
{
    return strncmp(s1, flag, SIZE_OF_FLAGS)==0;
}

Bool_t MReadRflFile::ReadEvtHeader()
{
    if (fCurrentVersion <= 0.5)
    {
        RflEventHeader_old revth;
        fIn->read((char*)&revth, sizeof(RflEventHeader_old));
        fEvtHeader->SetEvtNumber((int)revth.EvtNumber);
    }
    else
    {
        RflEventHeader revth;
        fIn->read((char*)&revth, sizeof(RflEventHeader));
        fEvtHeader->SetEvtNumber((int)revth.EvtNumber);
        fEvtHeader->SetEnergy((int)revth.Etotal);
        fEvtHeader->SetMomentum(revth.p[0], revth.p[1], revth.p[2]);
        fEvtHeader->SetCorePosition(revth.CorePos[0][0], revth.CorePos[1][0]);
        
        fEvtHeader->SetHeightFirstInt((Float_t)revth.zFirstInt);

        fEvtHeader->SetPhi((Float_t)revth.telescopePhi);
        fEvtHeader->SetTheta((Float_t)revth.telescopeTheta);

        fEvtHeader->SetNmax((Float_t)revth.longi_Nmax);
        fEvtHeader->SetT0((Float_t)revth.longi_t0);
        fEvtHeader->SetTmax((Float_t)revth.longi_tmax);
        fEvtHeader->SetChi2((Float_t)revth.longi_chi2);

        fEvtHeader->SetEFraction((Float_t)revth.elec_cph_fraction);
        fEvtHeader->SetMFraction((Float_t)revth.muon_cph_fraction);
        fEvtHeader->SetOFraction((Float_t)revth.other_cph_fraction);
    }

    return (bool)*fIn;
}

int MReadRflFile::ReadFlag()
{
    char flag[SIZE_OF_FLAGS];
    fIn->read(flag, SIZE_OF_FLAGS);

    if (!fIn)
        return kError;

    if (FlagIsA(flag, FLAG_END_OF_FILE))
        return kEndOfFile;
    if (FlagIsA(flag, FLAG_END_OF_RUN))
        return kEndOfRun;
    if (FlagIsA(flag, FLAG_START_OF_RUN))
        return kStartOfRun;
    if (FlagIsA(flag, FLAG_END_OF_EVENT))
        return kEndOfEvtData;
    if (FlagIsA(flag, FLAG_START_OF_EVENT))
        return kStartOfEvtData;

    return kUndefined;
}

Bool_t MReadRflFile::ReadEvtData()
{
    Bool_t rc = kFALSE;
    while (1)
    {
        cphoton data; // FIRST READ "START OF EVENT"
        fIn->read((char*)&data, SIZE_OF_FLAGS);
        if (!*fIn)
            break;

        if (FlagIsA((char*)&data, FLAG_END_OF_EVENT))
        {
            rc = kTRUE;
            break;
        }

        fIn->read((char*)&data+SIZE_OF_FLAGS, sizeof(cphoton)-SIZE_OF_FLAGS);
        if (!*fIn)
            break;

        MRflSinglePhoton &ph = fEvtData->GetNewPhoton();
        ph.SetXY(data.x*10, data.y*10);
        ph.SetCosUV(data.u, data.v);
        ph.SetTime(data.t);
        ph.SetHeight(data.h);
        ph.SetInclinationAngle(data.phi);
    }

    fEvtData->FixSize();
    return rc;
}

Int_t MReadRflFile::EvalFlag()
{
    const Int_t flag = ReadFlag();

    switch (flag)
    {
    case kEndOfFile:
        fCurrentVersion = ReadVersion();
        if (fCurrentVersion<0)
        {
            *fLog << inf << "Found end of file...Everything OK." << endl;
            break;
        }

        *fLog << warn << "Found flag of end of file, but file goes on..." << endl;
        if (ReadFlag()<0)
            return kError;
        /* FALLTHROU */
    case kStartOfRun:
        if (fCurrentVersion>0.5)
        {
            RflRunHeader rrunh;
            fIn->read((char*)&rrunh, sizeof(RflRunHeader));
            if (*fIn)
            {
                *fLog << inf << "FIXME: Call ReInit" << endl;

                // FIXME: fRunHeader->Read(fIn);
                //        fRunHeader->Print();

                fRunHeader->SetRunNumber((int)rrunh.RunNumber);
                *fLog << underline << "RunHeader:" << endl;
                *fLog << " Run Number:    " << rrunh.RunNumber << endl;
                *fLog << " Date:          " << rrunh.date << endl;
                *fLog << " Corsika Ver.:  " << rrunh.Corsika_version << endl;
                *fLog << " Energy Range:  " << rrunh.ELowLim << "GeV to " << rrunh.EUppLim << "GeV" << endl;
                *fLog << " Energy Slope:  " << rrunh.SlopeSpec << endl;
                *fLog << " Wobble Mode:   " << rrunh.wobble_mode << endl;
                *fLog << " Atm. Model:    " << rrunh.atmospheric_model << endl;
                // *fLog << " Theta Range:   " << rrunh.ThetaMin << "deg to " << rrunh.ThetaMax << "deg" << endl;
                // *fLog << " Phi Range:     " << rrunh.PhiMin << "deg to " << rrunh.PhiMax << "deg" << endl;
                // *fLog << " View Cone Rad: " << rrunh.ViewconeRadius << "deg" << endl;
                break;
            }

            *fLog << err << "Error! found end of file... But no EOF flag. Exiting." << endl;
            return kError;
        }/*
        else
        {
            *fLog << err << "ERROR - Reflector versions <= 0.5 are not supported!" << endl;
            return kError;
        }*/
        return kUndefined;

    case kStartOfEvtData:
    case kEndOfRun:
        break;

    case kError:
        *fLog << err << "ERROR - Flag 'error'" << endl;
        return kError;

    case kUndefined:
        *fLog << err << "ERROR - Flag 'undefined'" << endl;
        return kError;

    default:
        *fLog << err << "ERROR - Unhandled flag" << endl;
        return kError;

    }
    return flag;
}

Int_t MReadRflFile::ReadEvent()
{
    for (;;)
    {
        switch (EvalFlag())
        {
        case kError:
            return kFALSE;

        case kEndOfFile:
            Close();
            return kCONTINUE; // FIXME: CHECK
//            if (!OpenNextFile())
//                return kFALSE;
            /* FALLTHROU */
        case kStartOfRun:
        case kEndOfRun:
            continue;

        case kStartOfEvtData:
            break;
        }
        break;
    }

    if (!ReadEvtHeader())
        return kFALSE;

    return ReadEvtData();
}

/*
Int_t MReadRflFile::Process()
{
    for (;;)
    {
        switch (EvalFlag())
        {
        case kError:
            return kFALSE;

        case kEndOfFile:
            if (!OpenNextFile())
                return kFALSE;
            // FALLTHROU
        case kStartOfRun:
        case kEndOfRun:
            continue;

        case kStartOfEvtData:
            break;
        }
        break;
    }

    if (!ReadEvtHeader())
        return kFALSE;

    return ReadEvtData();
}
*/

Int_t MReadRflFile::PreProcess(MParList *plist)
{
    fEvtData=(MRflEvtData*)plist->FindCreateObj("MRflEvtData");
    if (!fEvtData)
        return kFALSE;

    fEvtHeader=(MRflEvtHeader*)plist->FindCreateObj("MRflEvtHeader");
    if (!fEvtHeader)
        return kFALSE;

    fRunHeader=(MRflRunHeader*)plist->FindCreateObj("MRflRunHeader");
    if (!fRunHeader)
        return kFALSE;

    return MReadFiles::PreProcess(plist);
}

Bool_t MReadRflFile::ReadHeader()
{
    fCurrentVersion = ReadVersion();
    if (fCurrentVersion<0)
    {
        *fLog << err << "ERROR reading signature." << endl;
        return kFALSE;
    }
    *fLog << inf << "Version " << fCurrentVersion << endl << endl;

    return kTRUE;
}


/****************************************************/

float MReadRflFile::ReadVersion()
{
    char sign[20];
    fIn->read(sign, SIZE_OF_SIGNATURE);
    if (!*fIn)
        return -1;

    if (strncmp(sign, PROGNAME, strlen(PROGNAME)) != 0)
    {
        /* For the ascii tail of the file! : */
        if (strncmp(sign, "\n############", SIZE_OF_SIGNATURE))
            cout << "ERROR: Signature of .rfl file is not correct: " << sign << endl;

        return -1;
    }

    float version;
    sscanf(sign, "%*s %f", &version);

    //If the version is < 0.6 the signature had one more byte
    if (version < 0.6)
        *fIn >> sign[0];

    return version;
}

// --------------------------------------------------------------------------
//
// Default constructor. Creates an array which stores the file names of
// the files which should be read. If a filename is given it is added
// to the list.
//
MReadRflFile::MReadRflFile(const char *fname, const char *name,
                           const char *title) : MReadFiles(fname, name, title)
{
    fName  = name  ? name  : "MRead";
    fTitle = title ? title : "Reads a Reflector output file";
}

Bool_t MReadRflFile::SearchFor(Int_t runno, Int_t eventno)
{
    if (!fEvtHeader)
        return kFALSE;

    Rewind();

    while (1)
    {
        fEvtData->Reset();
        if (!Process())
            return kFALSE;

        if (fEvtHeader->GetEvtNumber()==eventno &&
            fRunHeader->GetRunNumber()==runno)
            return kTRUE;
    }

    Close();
}
