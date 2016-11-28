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
!   Author(s): Thomas Bretz, 02/2012 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2012
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportTemperatures
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportTemperatures.h"

#include "fits.h"

#include "MLogManip.h"

ClassImp(MReportTemperatures);

using namespace std;

// --------------------------------------------------------------------------
//
// Default construtor. Initialize identifier to "DRIVE-REPORT"
//
MReportTemperatures::MReportTemperatures() : MReport("TEMPERATURES-REPORT"),
    fTimeStamp(0)
{
    fName  = "MReportTemperatures";
    fTitle = "Class for DRIVE-REPORT information (raw telescope position)";

    memset(fTempSensors,  0, sizeof(Float_t)*31);
    memset(fTempCrate,    0, sizeof(Float_t)*8);
    memset(fTempPS,       0, sizeof(Float_t)*8);
    memset(fTempAux,      0, sizeof(Float_t)*4);
    memset(fTempBack,     0, sizeof(Float_t)*4);
    memset(fTempEthernet, 0, sizeof(Float_t)*4);
}

Bool_t MReportTemperatures::SetupReadingFits(fits &file)
{
    return
        file.SetRefAddress("t",       fTimeStamp)       &&
        file.SetPtrAddress("T_sens",  fTempSensors, 31) &&
        file.SetPtrAddress("T_crate", fTempCrate,    8) &&
        file.SetPtrAddress("T_ps",    fTempPS,       8) &&
        file.SetPtrAddress("T_aux",   fTempAux,      4) &&
        file.SetPtrAddress("T_back",  fTempBack,     4) &&
        file.SetPtrAddress("T_eth",   fTempEthernet, 4);
}

Int_t MReportTemperatures::InterpreteFits(const fits &fits)
{
    return kTRUE;
}

Float_t MReportTemperatures::GetTempMean() const
{
    Double_t avg = 0;
    UInt_t   num = 0;

    for (int i=0; i<31; i++)
    {
        if (fTempSensors[i]==0)
            continue;

        avg += fTempSensors[i];
        num++;
    }

    return avg/num;
}

Float_t MReportTemperatures::GetTempMedian() const
{
    Float_t dat[31];
    UInt_t  num = 0;

    for (int i=0; i<31; i++)
    {
        if (fTempSensors[i]!=0)
            dat[num++] = fTempSensors[i];
    }

    return TMath::Median(num, dat);
}

void MReportTemperatures::Print(Option_t *o) const
{
    *fLog << GetDescriptor() << ": Time=" << fTimeStamp << endl;
}
