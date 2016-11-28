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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MSignalCalc
//
//   This is a task which calculates the number of photons from the FADC
//   time slices. At the moment it integrates simply the FADC values.
//
//  Input Containers:
//   MRawEvtData, MPedestalCam
//
//  Output Containers:
//   MSignalCam
//
//////////////////////////////////////////////////////////////////////////////

#include "MSignalCalc.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MRawRunHeader.h"
#include "MSignalCam.h"
#include "MPedestalPix.h"
#include "MPedestalCam.h"
#include "MRawEvtPixelIter.h"
#include "MPedestalSubtractedEvt.h"

ClassImp(MSignalCalc);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. b is the number of slices before the maximum slice,
// a the number of slices behind the maximum slice which is taken as signal.
//
MSignalCalc::MSignalCalc(Byte_t b, Byte_t a, const char *name, const char *title)
    : fBefore(b), fAfter(a)
{
    fName  = name  ? name  : "MSignalCalc";
    fTitle = title ? title : "Task to calculate Cerenkov photons from raw data";
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MRawRunHeader
//  - MRawEvtData
//  - MPestalCam
//
// The following output containers are also searched and created if
// they were not found:
//  - MSignalCam
//
Int_t MSignalCalc::PreProcess(MParList *pList)
{
    fSkip = 0;

    fRunHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRunHeader)
    {
        *fLog << dbginf << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fRawEvt = (MPedestalSubtractedEvt*)pList->FindObject("MPedestalSubtractedEvt");
    if (!fRawEvt)
    {
        *fLog << dbginf << "MPedestalSubtractedEvt not found... aborting." << endl;
        return kFALSE;
    }

    fCerPhotEvt = (MSignalCam*)pList->FindCreateObj("MSignalCam");
    if (!fCerPhotEvt)
        return kFALSE;

    return kTRUE;
}

Bool_t MSignalCalc::ReInit(MParList *pList)
{
    fPedestals=NULL;

    // This must be done in ReInit because in PreProcess the
    // headers are not available
    if (fRunHeader->IsMonteCarloRun())
        return kTRUE;

    fPedestals = (MPedestalCam*)pList->FindCreateObj("MPedestalCam");
    if (!fPedestals)
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calculate the integral of the FADC time slices and store them as a new
// pixel in the MSignalCam container.
//
Int_t MSignalCalc::Process()
{
    if (!fPedestals)
        return kTRUE;

    const Int_t npix = fRawEvt->GetNumPixels();
    const Int_t nhi  = fRunHeader->GetNumSamplesHiGain();
    const Int_t nlo  = fRunHeader->GetNumSamplesLoGain();

    for (int i=0; i<npix; i++)
    {
        USample_t *raw = fRawEvt->GetSamplesRaw(i);

        USample_t *ptr   = raw;
        USample_t *max   = ptr+fRawEvt->GetMaxPos(i, 0, nhi-1);
	USample_t *end   = ptr+nhi;
        USample_t *first = max-fBefore;
        USample_t *last  = max+fAfter;

        ULong_t sumb  = 0;   // sum background
        ULong_t sqb   = 0;   // sum sqares background
        //ULong_t sumsb = 0;   // sum signal+background
        ULong_t sqsb  = 0;   // sum sqares signal+background

        Int_t sat  = 0; // saturates?
        Int_t ishi = 0; // has a high content?
        Int_t nb   = 0;
        Int_t nsb  = 0;

        if (*max==255)  // FIXME!!!!
            sat++;

        if (*max>80)
            ishi++;

        while (ptr<end)
        {
            if (ptr<first || ptr>last)
            {
                sumb += *ptr;
                sqb  += *ptr* *ptr;
                nb++;
            }
            else
            {
                //sumsb += *ptr;
                sqsb  += *ptr* *ptr;
                nsb++;
            }
            ptr++;
        }

        if (nlo>0 && sat==0 && ishi)
        {
            // Area: x9
            ptr = raw+nhi;
            end = ptr+nlo;

            sumb = 0;   // sum background
            sqb  = 0;   // sum sqares background
            nb   = 0;

            while (ptr<end)
            {
                // Background already caced from hi-gains!
                sumb += *ptr;
                sqb  += *ptr* *ptr;
                nb++;
                ptr++;
            }
        }

        if (nlo>0 && sat>1 && !ishi)
        {
            // Area: x9
            ptr = raw+nhi;
            max = ptr+fRawEvt->GetMaxPos(i, nhi, nhi+nlo-1);

            if (*max>250) // FIXME!!!!
            {
                fSkip++;
                return kCONTINUE;
            }

            end   = ptr+nlo;
            first = max-fBefore;
            last  = max+fAfter;

            //sumsb = 0;   // sum signal+background
            //sqsb  = 0;   // sum sqares signal+background
            sumb  = 0;   // sum background
            //sqb   = 0;   // sum sqares background

            //nb = 0;
            nsb = 0;
            while (ptr<end)
            {
                if (ptr<first || ptr>last)
                {
                    /*
                     // Background already calced from hi-gains!
                    sumb += ptr[i];
                    sqb  += ptr[i]*ptr[i];
                    nb++;*/
                }
                else
                {
                    //sumsb += *ptr;
                    sqsb  += *ptr* *ptr;
                    nsb++;
                }
                ptr++;
            }
        }

        Float_t b  = nb==0 ? 0 : (float)sumb/nb;       // background
        //Float_t sb = (float)sumsb/nsb;     // signal+background

        Float_t msb  = nb==0 ? 0 : (float)sqb/nb;      // mean square background
        //Float_t mssb = (float)sqsb/nsb;    // mean square signal+background

        Float_t sigb  = sqrt(msb-b*b);     // sigma background
        //Float_t sigsb = sqrt(mssb-sb*sb);  // sigma signal+background

        //Float_t s   = sb-b;                // signal
        //Float_t sqs = sqsb-nsb*b;          // sum squares signal

        //Float_t mss  = (float)sqs/nsb;     // mean quare signal
        //Float_t sigs = sqrt(mss-s*s);      // sigma signal

        //if (sat>1)
        //    s *= 11.3;

        //fCerPhotEvt->AddPixel(idx, s, sigs);

        // Preliminary: Do not overwrite pedestals calculated by
        // MMcPedestalCopy and MMcPedestalNSBAdd
        (*fPedestals)[i].Set(b/fRunHeader->GetScale(), sigb/fRunHeader->GetScale());
    }

    //fCerPhotEvt->FixSize();
    //fCerPhotEvt->SetReadyToSave();

    fPedestals->SetReadyToSave();

    return kTRUE;
}

Int_t MSignalCalc::PostProcess()
{
    if (GetNumExecutions()==0 || fSkip==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << dec << setfill(' ');
    *fLog << " " << setw(7) << fSkip << " (" << setw(3) << (int)(fSkip*100/GetNumExecutions()) << "%) Evts skipped due to: lo gain saturated." << endl;

    return kTRUE;
}
