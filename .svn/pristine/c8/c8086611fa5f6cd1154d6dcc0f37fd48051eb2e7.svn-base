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
!   Author(s): Christoph Kolodziejski, 12/2004  <mailto:>
!   Author(s): Thomas Bretz, 12/2004  <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MGeomCamMagicEnhance
//
// This task takes MSignalCam assuming that it is related to the Magic
// camera geometry. By deviding and interpolation it resamples MSignalCam
// such, that afterwards it is related to a geometry MGeomCamMagicXT
//
//////////////////////////////////////////////////////////////////////////////
#include "MGeomCamMagicEnhance.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MSignalCam.h"
#include "MSignalPix.h"

#include "MArrayD.h"

ClassImp(MGeomCamMagicEnhance);

using namespace std;

const char *MGeomCamMagicEnhance::fgNameSignalCamIn  = "MSignalCam";
const char *MGeomCamMagicEnhance::fgNameSignalCamOut = "MSignalCam";

// ---------------------------------------------------------------------------
//
//  Default Constructor - empty
//
MGeomCamMagicEnhance::MGeomCamMagicEnhance(const char *name, const char *title)
    : fNameSignalCamIn(fgNameSignalCamIn), fNameSignalCamOut(fgNameSignalCamOut),
    fEvtIn(0), fEvtOut(0)
{
    fName  = name  ? name  : "MGeomCamMagicEnhance";
    fTitle = title ? title : "Task to convert MSignalCam from MGeomCamMagic to MGeomCamMagicXT";
}

// ---------------------------------------------------------------------------
//
//  Search for MSignalCam input and output container.
//
Int_t MGeomCamMagicEnhance::PreProcess(MParList *plist)
{
    fEvtIn = (MSignalCam*)plist->FindObject(fNameSignalCamIn, "MSignalCam");
    if (!fEvtIn)
    {
        *fLog << err << fNameSignalCamIn << " [MSignalCam] not found... abort." << endl;
        return kFALSE;
    }

    fEvtOut = (MSignalCam*)plist->FindCreateObj("MSignalCam", fNameSignalCamOut);
    if (!fEvtOut)
        return kFALSE;

    return kTRUE;
}

// ---------------------------------------------------------------------------
//
//  Convert MCerPhotEvent into a new MArrayD depending on the new geometry
//
MArrayD MGeomCamMagicEnhance::Convert() const
{
    Double_t dEvt[577];
    for (int i = 0; i<577; ++i)
        dEvt[i] = (*fEvtIn)[i].GetNumPhotons();

    MArrayD dEvent(1141);
    memcpy(dEvent.GetArray(), dEvt, 577*sizeof(Double_t));

    int k = 397; // alter Pixelindex
    int m = 397; // neuer Index nach einem Durchlauf

    double* pEvent = dEvent.GetArray() + 397;

    for( int i = 0 ; i < 4 ; ++i )
    {
        for( int j = 0; j < 6; ++j )
            for( int h = 0; h < 6 + i; ++h )
            {
                if( !j && !h )
                    *(pEvent++) = 0.25*0.5*( dEvt[ m ] + dEvt[ m + (6 + i)*6 - 1 ] );

                *(pEvent++) = 0.25*dEvt[ k ];

                if(!(j == 5 && h == 5 + i)){
                    if( i && h != 5 + i){
                        *(pEvent++) = 0.25*1/3.*( dEvt[ k ] + dEvt[ k + 1 ] + dEvt[ k - (5 + i)*6 - j ] );
                    }else
                        *(pEvent++) = 0.25*0.5*( dEvt[ k ] + dEvt[ k + 1 ] );
                }
                k++;
            }

        k = m;

        for( int j = 0; j < 6; ++j )
            for( int h = 0; h < 6 + i; ++h )
            {
                if( !h && !j ){
                    if( i != 3)
                        *(pEvent++) = 0.25*0.25*( dEvt[ m ] + dEvt[ m + (6 + i)*6 - 1 ] + dEvt[ m + (6 + i)*6 ] + dEvt[ m + 12*i + 77 /*m + (6 + i)*6 + (6 + i + 1)*6 - 1*/ ] );
                    else
                        *(pEvent++) = 0.25*0.5*( dEvt[ m ] + dEvt[ m + (6 + i)*6 - 1 ] );
                }

                if( !h && j ){
                    if( i != 3 ){
                        *(pEvent++) = 0.25*0.25*( dEvt[ k - 1 ] + dEvt[ k ] + dEvt[ k + (6 + i)*6 + j - 1 ] + dEvt[ k + (6 + i)*6 + j ] );
                    }else
                        *(pEvent++) = 0.25*0.5*( dEvt[ k - 1 ] + dEvt[ k ] );
                }

                *(pEvent++) = 0.25*dEvt[ k ];
                *(pEvent++) = 0.25*dEvt[ k ];

                k++;

            }

        m += (6 + i)*6;
    }

    return dEvent;
}

// ---------------------------------------------------------------------------
//
//  Call Convert and copy result into output MSignalCam
//
Int_t MGeomCamMagicEnhance::Process()
{
    // Convert old stylish magic camera into new enhanced version
    const MArrayD res(Convert());

    // Copy result into output MSignalCam
    fEvtOut->InitSize(res.GetSize());

    for (UInt_t i=0; i<res.GetSize(); i++)
        fEvtOut->AddPixel(i, res[i], 0);

    //fEvtOut->FixSize();
    fEvtOut->SetReadyToSave();

    return kTRUE;
}

