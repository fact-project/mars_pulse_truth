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
!   Author(s): Thomas Bretz, 2013<mailto:thomas.bretz@phys.ethz.ch>
!
!   Copyright: MAGIC Software Development, 2013
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MFilterData
//
//////////////////////////////////////////////////////////////////////////////
#include "MFilterData.h"

#include <algorithm>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MPedestalSubtractedEvt.h"

ClassImp(MFilterData);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MFilterData::MFilterData(const char *name, const char *title)
    : fSignalIn(NULL), fSignalOut(NULL),
    fNameSignalIn("MPedestalSubtractedEvt"),
    fNameSignalOut("FilteredEvt")
{
    fName  = name  ? name  : "MFilterData";
    fTitle = title ? title : "Class to filter the data";

    // Default weights
    fWeights.resize(14);
    fWeights[ 0] = -0.217305;
    fWeights[ 1] = -0.213277;
    fWeights[ 2] = -0.193537;
    fWeights[ 3] = -0.181686;
    fWeights[ 4] = -0.15356;
    fWeights[ 5] = -0.129926;
    fWeights[ 6] = -0.0792033;
    fWeights[ 7] = -0.0219311;
    fWeights[ 8] =  0.0550301;
    fWeights[ 9] =  0.127364;
    fWeights[10] =  0.206711;
    fWeights[11] =  0.246864;
    fWeights[12] =  0.271012;
    fWeights[13] =  0.283444;
}

// --------------------------------------------------------------------------
//
Int_t MFilterData::PreProcess(MParList *pList)
{
    fSignalIn = (MPedestalSubtractedEvt*)pList->FindObject(fNameSignalIn);
    if (!fSignalIn)
    {
        *fLog << err << fNameSignalIn << " [MPedestalSubtractedEvt] not found... aborting." << endl;
        return kFALSE;
    }

    fSignalOut = (MPedestalSubtractedEvt*)pList->FindCreateObj("MPedestalSubtractedEvt", fNameSignalOut);
    if (!fSignalOut)
        return kFALSE;

    return kTRUE;
}

void MFilterData::Filter(UShort_t npix, UShort_t nroi, const Float_t *beg, Float_t *out) const
{
    const uint16_t nw = fWeights.size();

    const float *begw = fWeights.data();
    const float *endw = fWeights.data()+nw;

    for (const Float_t *in=beg; in<beg+nroi*npix; )
    {
        // Loop from 0 to nroi-nw-1
        const Float_t *end = in+nroi-nw;
        for (; in<end; in++, out++)
        {
            // This works even if out==in
            const float *w = begw;
            const float *p = in;

            // Loop over weights
            *out = *p++ * *w++;
            while (w<endw)
                *out += *p++ * *w++;
        }

        // skip last nw samples
        in += nw;
        out += nw;

        // Loop from nroi-nw to nroi
        // end = in+nw;
        // for (; in<end; in++)
        // {
        //     Double_t sum = 0;
        //     for (uint16_t j=0; j<nw; j++)
        //         sum += in[min(j, last)]*fWeights[j];
        //     *out++ = sum;
        // }
    }
}

// --------------------------------------------------------------------------
//
//
Int_t MFilterData::Process()
{
    // FIXME: Implement an alternative: the simple filter
    // (p[10]+p[11]+p[12]) - (p[0]+p[1]+p[2])
    // Store first sum in ring-buffer

    const uint16_t nroi = fSignalIn->GetNumSamples();
    const uint16_t npix = fSignalIn->GetNumPixels();

    if (fSignalIn!=fSignalOut)
        fSignalOut->InitSamples(nroi); // contains setting to 0

    Filter(npix, nroi, fSignalIn->GetSamples(), fSignalOut->GetSamples());

    return kTRUE;
}
