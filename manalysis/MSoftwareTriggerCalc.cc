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
!   Author(s): Thomas Bretz, 2016 <mailto:tbretz@physik.rwth-aachen.de>
!
!   Copyright: MAGIC Software Development, 2016
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MSoftwareTriggerCalc
//
//////////////////////////////////////////////////////////////////////////////
#include "MSoftwareTriggerCalc.h"

#include <algorithm>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawEvtData.h"
#include "MSoftwareTrigger.h"
#include "MPedestalSubtractedEvt.h"

ClassImp(MSoftwareTriggerCalc);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MSoftwareTriggerCalc::MSoftwareTriggerCalc(const char *name, const char *title)
    : fRawEvt(0), fSignal(0), fTrigger(0)
{
    fName  = name  ? name  : "MSoftwareTriggerCalc";
    fTitle = title ? title : "Calculate the FACT trigger in software";
}

// --------------------------------------------------------------------------
//
Int_t MSoftwareTriggerCalc::PreProcess(MParList *pList)
{
    fRawEvt = (MRawEvtData*)pList->FindObject("MRawEvtData");//, AddSerialNumber(fNamePedestalSubtractedEvt));
    if (!fRawEvt)
    {
        *fLog << err << "MRawEvtData not found... aborting." << endl;
        return kFALSE;
    }

    fSignal = (MPedestalSubtractedEvt*)pList->FindObject("MPedestalSubtractedEvt");//, AddSerialNumber(fNamePedestalSubtractedEvt));
    if (!fSignal)
    {
        *fLog << err << "MPedestalSubtractedEvt not found... aborting." << endl;
        return kFALSE;
    }

    fTrigger = (MSoftwareTrigger*)pList->FindCreateObj("MSoftwareTrigger");
    if (!fTrigger)
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
Bool_t MSoftwareTriggerCalc::ReInit(MParList *pList)
{
    // FIXME: Check number of samples!
    return kTRUE;
}

// --------------------------------------------------------------------------
//
Int_t MSoftwareTriggerCalc::Process()
{
    const UShort_t *idx = fRawEvt->GetPixelIds();

    const int beg =  10;
    const int end = std::min(UInt_t(200), fSignal->GetNumSamples());
    const int num = end-beg;

    MArrayF buf(160*num);

    double avg = 0;

    for (int hw=0; hw<1440; hw++)
    {
        // dead pixels
        if (hw==927 || hw==80 || hw==873)                   // 3
            continue;
        // crazy pixels
        if (hw==863 || hw==297 || hw==868)                  // 3
            continue;
        // broken DRS board
        if (hw>=720 && hw<=728)                             // 9
            continue;
        // broken/suspicious bias channels
        if ((hw>=171 && hw<=174) || (hw>=184 && hw<=188))   // 9
            continue;

        const Float_t *raw = fSignal->GetSamples(idx[hw]);

        Float_t *sum = buf.GetArray()+(hw/9)*num;

        // Sum all nine pixel of one trigger patch
        for (const Float_t *ptr=raw+beg; ptr<raw+end; ptr++, sum++)
            *sum += *ptr;
    }

    // apply correction factor for patches
    // 927/9, 80/9, 873/9, 863/9, 297/9, 868/9
/*
    const int excl[] = { 8, 33, 95, 96, 97, 103, -1};
    for (const int *e=excl; *e>=0; e++)
    {
        Float_t *raw = buf.GetArray() + *e * num;
        for (Float_t *ptr=raw; ptr<raw+num; ptr++)
            *ptr *= 1.125; // 9/8
    }
    {
        Float_t *raw = buf.GetArray() + 19 * num;   // 171-174
        for (Float_t *ptr=raw; ptr<raw+num; ptr++)
            *ptr *= 1.8;   // 9/5 // 5 channels left
    }
    {
        Float_t *raw = buf.GetArray() + 20 * num;   // 184-188
        for (Float_t *ptr=raw; ptr<raw+num; ptr++)
            *ptr *= 2.25;  // 9/4 // 4 channels left
    }
*/

    Float_t  max = -50000;
    UShort_t pos = 0;
    Short_t  patch = -1;

    for (int i=0; i<160; i++)
    {
        int idx = 0;
        Float_t v[4] = { 0, 0, 0, 0 };

        Float_t *sum=buf.GetArray() + num*i;
        for (Float_t *ptr=sum+15; ptr<sum+num; ptr++)
        {
            ptr[0] -= 0.5*ptr[-15];

            avg += ptr[0];
            v[idx++%4] = ptr[0];

            const Float_t min = *std::min_element(v, v+4);
            if (min<=max)
                continue;

            max = min;
            pos = ptr-sum+beg;
            patch = i;
        }
    }

    // Question: Should we also keep position and patch id?

    avg /= (num-15)*(1440-24);

    fTrigger->SetData(patch, avg, pos, max);
    fTrigger->SetReadyToSave();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
Int_t MSoftwareTriggerCalc::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    return kTRUE;
}

