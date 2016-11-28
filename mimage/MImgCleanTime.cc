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
!   Author(s): Thomas Bretz, 4/2014 <mailto:tbretz@phys.ethz.ch>
!
!   Copyright: MAGIC Software Development, 2014
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MImgCleanTime
//
//  Input Containers:
//   MGeomCam
//   MSignalCam
//
//  Output Containers:
//   MSignalCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MImgCleanTime.h"

#include <TEnv.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomPix.h"
#include "MGeomCam.h"

#include "MSignalPix.h"
#include "MSignalCam.h"

ClassImp(MImgCleanTime);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Here you can specify the cleaning method and levels. 
// If you don't specify them the 'common standard' values 3.0 and 2.5 (sigma
// above mean) are used.
// Here you can also specify how many rings around the core pixels you want 
// to analyze (with the fixed lvl2). The default value for "rings" is 1.
//
MImgCleanTime::MImgCleanTime(const char *name, const char *title)
    : fCam(0), fEvt(0), fMinCount(0), fMinSize(25), fDeltaT(2.5*17.5*0.1111),
    fNameSignalCam("MSignalCam")
{
    fName  = name  ? name  : "MImgCleanTime";
    fTitle = title ? title : "Task to perform image cleaning";
}

// --------------------------------------------------------------------------
//
//  Check if MEvtHeader exists in the Parameter list already.
//  if not create one and add them to the list
//
Int_t MImgCleanTime::PreProcess (MParList *pList)
{
    fCam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fCam)
    {
        *fLog << err << "MGeomCam not found (no geometry information available)... aborting." << endl;
        return kFALSE;
    }
    fEvt = (MSignalCam*)pList->FindObject(fNameSignalCam, "MSignalCam");
    if (!fEvt)
    {
        *fLog << err << fNameSignalCam << " [MSignalCam] not found... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

Island MImgCleanTime::CalcIsland(MSignalPix &pix1, const MGeom &gpix1, const uint16_t &island1)
{
    pix1.SetIdxIsland(island1);

    const Float_t time1 = pix1.GetArrivalTime();

    if (pix1.IsPixelUnmapped())
        return Island(0,time1);

    Island island(pix1.GetNumPhotons(), time1);

    for (UInt_t i=0; i<gpix1.GetNumNeighbors(); i++)
    {
        const Int_t idx2 = gpix1.GetNeighbor(i);

        MSignalPix &pix2 = (*fEvt)[idx2];
        if (pix2.IsPixelUnmapped())
            continue;

        const Int_t island2 = pix2.GetIdxIsland();
        if (island2>=0)
        {
            if (island1==island2)
                continue;

            // The entries are sorted naturally, therefore
            // there is no need to check the whole array
            const auto it = fContacts.rbegin();
            if (it==fContacts.rend() ||
                (it->first!=island1 && it->second!=island2))
                fContacts.emplace_back(island1, island2);

            continue;
        }

        const Float_t time2 = pix2.GetArrivalTime();
        if (fabs(time2-time1)<fDeltaT) // FIXME: Scale with distance?
            island += CalcIsland(pix2, (*fCam)[idx2], island1);
    }

    return island;
}

// --------------------------------------------------------------------------
//
// Cleans the image.
//
Int_t MImgCleanTime::Process()
{
    // The assumption is that all mapped pixels contain valid data
    const UInt_t npix = fEvt->GetNumPixels();
    for (UInt_t i=0; i<npix; i++)
    {
        MSignalPix &pix = (*fEvt)[i];
        if (!pix.IsPixelUnmapped())
            pix.SetPixelUnused();
        pix.SetPixelCore(kFALSE);
        pix.SetIdxIsland(-1);
    }

    // Contains the id of the island for each pixel
    fIslands.clear();
    fContacts.clear();

    // Start with island idx==0
    UShort_t idx = 0;
    for (UInt_t i=0; i<npix; i++)
    {
        MSignalPix &spix = (*fEvt)[i];

        // The following might be much more efficient and faster
        // if we omit small counted and sized islands already,
        // but what is the disadvantage?
        if (spix.GetIdxIsland()<0)
            fIslands.emplace_back(CalcIsland(spix, (*fCam)[i], idx++));
    }

    fLut.resize(fIslands.size());
    for (UInt_t i=0; i<fLut.size(); i++)
        fLut[i] = i;

    // Unify touching islands if their arrival time ranges overlap
    for (auto it=fContacts.begin(); it!=fContacts.end(); it++)
    {
        const uint16_t &idx1 = fLut[it->first];
        const uint16_t &idx2 = fLut[it->second];
        if (idx1==idx2)
            continue;

              Island &I1 = fIslands[idx1];
        const Island &I2 = fIslands[idx2];

        // FIXME: Put a limit on count? Put a limit on size?
        // The ideal cut would be to allow that a single
        // pixel still can connect two clusters
        if (I1.count==1 || I2.count==1)
            continue;

        if (I1.max<I2.min || I2.max<I1.min)
            continue;

        // Combine both islands
        I1 += I2;

        // Remove idx2 from the list
        for (auto is=fLut.begin(); is!=fLut.end(); is++)
            if (*is==idx2)
                *is=idx1;

        // Erase entry form contact list
        fContacts.erase(it);

        // Start over again
        it = fContacts.begin();
    }

    uint16_t num_islands = 0;
    double   size_main   = 0;
    double   size_tot    = 0;

    vector<bool> used(fIslands.size());
    for (UInt_t i=0; i<npix; i++)
    {
        MSignalPix &pix = (*fEvt)[i];

        // At the end every pixel has an island assigned
        const Short_t ii = fLut[pix.GetIdxIsland()];

        const Island &island = fIslands[ii];

        if (island.count<=fMinCount || island.size<=fMinSize)
            continue;

        if (!used[ii])
        {
            used[ii] = true;
            num_islands++;

            size_tot += island.size;
            if (island.size>size_main)
                size_main = island.size;
        }

        pix.SetPixelUsed();
        pix.SetPixelCore();
    }

    fEvt->SetIslandInfo(num_islands, size_main, size_tot-size_main);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
Int_t MImgCleanTime::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "MinSize", print))
    {
        rc = kTRUE;
        SetMinSize(GetEnvValue(env, prefix, "MinSize", fMinSize));
    }
    if (IsEnvDefined(env, prefix, "MinCount", print))
    {
        rc = kTRUE;
        SetMinSize(GetEnvValue(env, prefix, "MinCount", (Int_t)fMinCount));
    }
    if (IsEnvDefined(env, prefix, "DeltaT", print))
    {
        rc = kTRUE;
        SetMinSize(GetEnvValue(env, prefix, "DeltaT", fDeltaT));
    }

    return rc;
}
