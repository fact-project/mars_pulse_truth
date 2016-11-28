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
!   Author(s): Christoph Kolodziejski, 11/2004  <mailto:>
!   Author(s): Thomas Bretz, 11/2004  <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MHexagonalFTCalc
//
// Task to perform forward transformation, cuts in fourier space and
// backward tranformation.
//
// If you don't want to copy the rsult back to any MSignalCam call
// SetNameSignalCamOut()
//
// Currently - this will change in the future! - there are two output
// containers:
//  - MHexagonalFreqSpace1
//      real      part: input values in triangle space
//      imaginary part: output values in triangle space
//
//  - MHexagonalFreqSpace2
//      real      part: real part of frequency space
//      imaginary part: imaginary part of frequency space
//
// To be done:
// Flags to perform forward/backward trafo only to be able to cut outside!
//
//////////////////////////////////////////////////////////////////////////////
#include "MHexagonalFTCalc.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MGeom.h"

#include "MSignalCam.h"
#include "MSignalPix.h"

#include "MHexagonFreqSpace.h"

#include "MArrayD.h"

ClassImp(MHexagonalFTCalc);

using namespace std;

const char *MHexagonalFTCalc::fgNameGeomCam       = "MGeomCam";
const char *MHexagonalFTCalc::fgNameSignalCamIn  = "MSignalCam";
const char *MHexagonalFTCalc::fgNameSignalCamOut = "MSignalCam";

// ---------------------------------------------------------------------------
//
//  Default Constructor - empty. Initialize
//   fMaxAmplitude   = 0.08
//   fMaxRowFraction = 0.75
//
MHexagonalFTCalc::MHexagonalFTCalc(const char *name, const char *title)
    : fNameGeomCam(fgNameGeomCam),
    fNameSignalCamIn(fgNameSignalCamIn), fNameSignalCamOut(fgNameSignalCamOut),
    fEvtIn(0), fEvtOut(0), 
    fMaxAmplitude(0.08), fMaxRowFraction(0.75), fSkipBwdTrafo(kFALSE)
{
    fName  = name  ? name  : "MHexagonalFTCalc";
    fTitle = title ? title : "Task to calculate Hexagonal Fourier Transformation";
}

// ---------------------------------------------------------------------------
//
//  Search for all necessary containers.
//
// Do a forward transformation for a flat camera, eg:
//
//    0 0 1 0 0
//     0 1 1 0
//      1 1 1
//       1 1
//        1
//
// The result (fOffset) is treated as a offset in the fourier space comming
// from the fact that the camera doesn't extend the whole triangle.
//
// Before cutting in the fourier space it is substracted. After cutting it
// is added again. Scaling is done by the constant-frequency-pixel (index 0)
//
// Create the LookUpTable which tells you which fourier space (triangle)
// pixel index corresponds to which camera pixel index.
//
Int_t MHexagonalFTCalc::PreProcess(MParList *plist)
{
    fEvtIn = (MSignalCam*)plist->FindObject(fNameSignalCamIn, "MSignalCam");
    if (!fEvtIn)
    {
        *fLog << err << fNameSignalCamIn << " [MSignalCam] not found... abort." << endl;
        return kFALSE;
    }

    fEvtOut=0;
    if (!fNameSignalCamOut.IsNull())
    {
        fEvtOut = (MSignalCam*)plist->FindCreateObj(fNameSignalCamOut, "MSignalCam");
        if (!fEvtOut)
            return kFALSE;
    }

    fFreq1 = (MHexagonFreqSpace*)plist->FindCreateObj("MHexagonFreqSpace","MHexagonFreqSpace1");
    if (!fFreq1)
        return kFALSE;
    fFreq2 = (MHexagonFreqSpace*)plist->FindCreateObj("MHexagonFreqSpace","MHexagonFreqSpace2");
    if (!fFreq2)
        return kFALSE;

    MGeomCam *geom = (MGeomCam*)plist->FindObject(fNameGeomCam, "MGeomCam");
    if (!geom)
    {
        *fLog << err << fNameGeomCam << " [MGeomCam] not found... abort." << endl;
        return kFALSE;
    }

    // Number of Rings (r) ---> Number of Pixels (p)
    // p = 3*r*(r-1)+1

    // Number of Pixels (p) ---> Number of Rings (r)
    // p = (sqrt(9+12*(p-1))+3)/6

    // Number of pixels at one border == number of rings (r)
    // Row of border                  == number of rings (r)

    // Number of rows to get a triangle: 3r-2

    // One additional line on all three sides, number of rows: 3r-2+3 = 3r+1
    Int_t num = 0;
    for (UInt_t i=0; i<geom->GetNumPixels(); i++)
        if ((*geom)[i].GetAidx()==0)
            num++;

    cout << "Number of pixels with Area Idx==0: " << num << endl;

    const Int_t numrows = (Int_t)((sqrt(9.+12.*(num-1))+3)/2+1); //61;//34;
    fHFT.Prepare(numrows);

    cout << "Number of rows: " << numrows << endl;

    const Int_t lim = fHFT.GetNumKnots();

    fOffset.Set(lim);
    fOffset.Reset();

    //fMap.Set(lim);
    //for (int i=0; i<lim; i++)
    //    fMap[i]=-1;

    const Double_t dx = (*geom)[2].GetX(); // -(*geom)[0].GetX()
    const Double_t dy = (*geom)[2].GetY(); // -(*geom)[0].GetY()

    for (int i=0; i<lim; i++)
    {
        const Float_t x = dx*fHFT.GetCol(i);
        const Float_t y = dy*Int_t(fHFT.GetRow(i)-2*fHFT.GetNumRows()/3);

        const Int_t idx = geom->GetPixelIdxXY(x, y);
        if (idx<0)
            continue;

        if ((*geom)[idx].GetAidx()==0)
        {
            //fMap[idx]  = i;
            fOffset[i] = 1;
        }
    }

    fFreq1->Set(MArrayD(lim), MArrayD(lim));
    fFreq2->Set(MArrayD(lim), MArrayD(lim));

    // Calculate fourier tranformation of a constant field for all
    // 'valid' pixels
    MArrayD freqre;
    MArrayD freqim;
    fHFT.TransformFastFWD(fOffset, freqre, freqim);
    fOffset = freqre;

    return kTRUE;
}

// ---------------------------------------------------------------------------
//
// Do cuts in frequence space (fOffset already sustracted), eg:
// fMaxRowFraction = 0.5
// fMaxAmplitude   = 0.3
//
//    0.1  0.2    0.1   0.3   0.4            0    0      0     0     0
//       0.4   0.3   0.2   0.1                  0     0     0     0
//          0.6   0.4   0.8      ----->           0.6    0    0.8
//             0.4   0.9                              0    0.9
//                 0                                     0
//
// The result (fOffset) is treated as a offset in the fourier space comming
// from the fact that the camera doesn't extend the whole triangle.
//
// Before cutting in the fourier space it is substracted. After cutting it
// is added again. Scaling is done by the constant-frequency-pixel (index 0)
//
void MHexagonalFTCalc::CleanFreqSpace(MArrayD &outre, MArrayD &outim)
{
    // Do cut in frequency space
    for (unsigned int i=0; i<outre.GetSize(); i++)
    {
        if (fHFT.GetRow(i)>(Int_t)fHFT.GetNumRows()*fMaxRowFraction)
            outre[i]=outim[i]=0;
        if (TMath::Hypot(outre[i], outim[i])<fMaxAmplitude)
            outre[i]=outim[i]=0;
    }
}

// ---------------------------------------------------------------------------
//
// - Copy input MSignalCam using the pixel mapping into an array
// - Do forward transformation
// - substract fourier offset
// - perform cuts in fourier space
// - add fourier offset
// - do backward tranformation
// - copy result back into output MSignalCam
//
Int_t MHexagonalFTCalc::Process()
{
    const Int_t lim = fHFT.GetNumKnots();

    MArrayD re(lim);

    //    MSignalPix *pix=0;
    //    MSignalCamIter Next(fEvtIn, kFALSE);
    //    while ((pix = (MSignalPix*)Next()))
    // Copy data from MSignalCam into array
    const UInt_t npix = fEvtIn->GetNumPixels();
    for (UInt_t idx=0; idx<npix; idx++)
    {
        //const Int_t idx = pix->GetPixId();
        //if (fMap[idx]>=0)
        re[idx] = (*fEvtIn)[idx].GetNumPhotons();
    }

    MArrayD freqre, freqim;

    // Do forward transformation
    fHFT.TransformFastFWD(re, freqre, freqim);

    // Substract the 'structure of the hexagon'
    Double_t scale = TMath::Abs(freqre[0]/fOffset[0]);
    for (int i=0; i<lim; i++)
        freqre[i] -= scale*fOffset[i];

    // Put result in output container
    fFreq2->Set(freqre, freqim);

    // ----------------------------------------------------
    // Do we need a backward transformation?
    // ----------------------------------------------------
    if (fSkipBwdTrafo)
        return kTRUE;

    // Clean image in frequency space
    CleanFreqSpace(freqre, freqim);

    // Add the 'structure of the hexagon'
    for (int i=0; i<lim; i++)
        freqre[i] += scale*fOffset[i];


    MArrayD out;

    // Do backward transformation
    fHFT.TransformFastBWD(freqre, freqim, out);

    // Write output into Mars container
    fFreq1->Set(re, out);

    // ----------------------------------------------------
    // Do we have to copy the result back into MSignalCam?
    // ----------------------------------------------------
    if (!fEvtOut)
        return kTRUE;

    fEvtOut->InitSize(lim); // necessary?
    for (int i=0; i<lim; i++)
    {
        //const Int_t map = fMap[i];
        //if (map>=0)
        fEvtOut->AddPixel(i, out[i]);
    }
    //fEvtOut->FixSize();
    fEvtOut->SetReadyToSave();

    return kTRUE;
}

