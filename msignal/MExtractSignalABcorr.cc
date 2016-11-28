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
!   Author(s): Markus Gaug, 09/2003 <mailto:markus@ifae.es>
!              Thomas Bretz, 01/2004 
!              Florian Goebel, 06/2004
!
!   Copyright: MAGIC Software Development, 2000-2004
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractSignalABcorr
//
//   this class is basically a copy of MExtractSignal and is mainly ment as 
//   an example of how to use the "AB pedestal offset" to correct for the 
//   150 MHz clock noise. It allows to extract a signal using an odd number 
//   of slices 
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractSignalABcorr.h"

#include <fstream>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MRawEvtData.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

ClassImp(MExtractSignalABcorr);

using namespace std;

const Byte_t MExtractSignalABcorr::fgSaturationLimit = 254;
const Byte_t MExtractSignalABcorr::fgFirst =  3;
const Byte_t MExtractSignalABcorr::fgLast  = 14;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
MExtractSignalABcorr::MExtractSignalABcorr(const char *name, const char *title)
    : fSaturationLimit(fgSaturationLimit)
{

    fName  = name  ? name  : "MExtractSignalABcorr";
    fTitle = title ? title : "Task to extract the signal from the FADC slices";

    AddToBranchList("MRawEvtData.*");

    SetRange();
}

void MExtractSignalABcorr::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast)
{

    fNumHiGainSamples = hilast-hifirst+1;
    fNumLoGainSamples = lolast-lofirst+1;

    fHiGainFirst = hifirst;
    fLoGainFirst = lofirst;

    fSqrtHiGainSamples = TMath::Sqrt((Float_t)fNumHiGainSamples);
    fSqrtLoGainSamples = TMath::Sqrt((Float_t)fNumLoGainSamples);
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MRawEvtData
//  - MPedestalCam
//
// The following output containers are also searched and created if
// they were not found:
//
//  - MExtractedSignalCam
//
Int_t MExtractSignalABcorr::PreProcess(MParList *pList)
{
    fRawEvt = (MRawEvtData*)pList->FindObject(AddSerialNumber("MRawEvtData"));
    if (!fRawEvt)
    {
        *fLog << err << AddSerialNumber("MRawEvtData") << " not found... aborting." << endl;
        return kFALSE;
    }


    fSignals = (MExtractedSignalCam*)pList->FindCreateObj(AddSerialNumber("MExtractedSignalCam"));
    if (!fSignals)
        return kFALSE;

    fSignals->SetUsedFADCSlices(fHiGainFirst, fHiGainFirst+fNumHiGainSamples-1, (Float_t)fNumHiGainSamples,
                                fLoGainFirst, fLoGainFirst+fNumLoGainSamples-1, (Float_t)fNumLoGainSamples);

    fPedestals = (MPedestalCam*)pList->FindObject(AddSerialNumber("MPedestalCam"));

    if (!fPedestals)
    {
        *fLog << err << AddSerialNumber("MPedestalCam") << " not found... aborting" << endl;
        return kFALSE;
    }

    return kTRUE;
}

void MExtractSignalABcorr::FindSignal(Byte_t *ptr, Int_t size, Int_t &sum, Byte_t &sat) const
{

  Byte_t *end = ptr + size;
  
  sum = 0;
  sat = 0;
  
  while (ptr<end)
    {
      sum += *ptr;
      
      if (*ptr++ >= fSaturationLimit)
        sat++;
    }
}

// --------------------------------------------------------------------------
//
// Calculate the integral of the FADC time slices and store them as a new
// pixel in the MExtractedSignalCam container.
//
Int_t MExtractSignalABcorr::Process()
{
    MRawEvtPixelIter pixel(fRawEvt);
    fSignals->Clear();

    UInt_t  sat=0;

    const Bool_t ApplyABcorrHi = fNumHiGainSamples&0x1;
    const Bool_t ApplyABcorrLo = fNumLoGainSamples&0x1;

    while (pixel.Next())
    {
        Int_t sumhi;
        Byte_t sathi;

        FindSignal(pixel.GetHiGainSamples()+fHiGainFirst, fNumHiGainSamples, sumhi, sathi);

        Int_t  sumlo = 0;
        Byte_t satlo = 0;
        if (pixel.HasLoGain())
        {
            FindSignal(pixel.GetLoGainSamples()+fLoGainFirst, fNumLoGainSamples, sumlo, satlo);

            if (satlo)
              sat++;
        }

        const Int_t pixid   = pixel.GetPixelId();
	const Bool_t ABFlag = pixel.HasABFlag();

        const MPedestalPix  &ped = (*fPedestals)[pixid]; 
	MExtractedSignalPix &pix = (*fSignals)[pixid];

        const Float_t pedes  = ped.GetPedestal();
        const Float_t pedrms = ped.GetPedestalRms();
	const Float_t ABoffs = ped.GetPedestalABoffset();

	Float_t pedtothi = pedes*fNumHiGainSamples;
	Float_t pedtotlo = pedes*fNumLoGainSamples;

	if (ApplyABcorrHi) {
	  pedtothi += ABoffs*(1-2*((fHiGainFirst+ABFlag)&0x1));
	}
	if (ApplyABcorrLo) {
	  pedtotlo += ABoffs*(1-2*((fLoGainFirst+ABFlag)&0x1));
	}

        pix.SetExtractedSignal(sumhi - pedtothi, pedrms*fSqrtHiGainSamples,
                               sumlo - pedtotlo, pedrms*fSqrtLoGainSamples);

	pix.SetGainSaturation(sathi, satlo);

    } /* while (pixel.Next()) */

    fSignals->SetReadyToSave();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MExtractSignalABcorr::StreamPrimitive(ostream &out) const
{
    out << "   " << ClassName() << " " << GetUniqueName() << "(\"";
    out << "\"" << fName << "\", \"" << fTitle << "\");" << endl;

    if (fSaturationLimit!=fgSaturationLimit)
    {
        out << "   " << GetUniqueName() << ".SetSaturationLimit(";
        out << (int)fSaturationLimit << ");" << endl;
    }

    const Bool_t arg4 = fNumLoGainSamples+fLoGainFirst-1 != fgLast;
    const Bool_t arg3 = arg4 || fLoGainFirst != fgFirst;
    const Bool_t arg2 = arg3 || fNumHiGainSamples+fHiGainFirst-1 != fgLast;
    const Bool_t arg1 = arg2 || fHiGainFirst != fgFirst;

    if (!arg1)
        return;

    out << "   " << GetUniqueName() << ".SetRange(";
    out << (int)fLoGainFirst;
    if (arg2)
    {
        out << ", " << (int)(fNumHiGainSamples+fHiGainFirst-1);
        if (arg3)
        {
            out << ", " << (int)fLoGainFirst;
            if (arg4)
                out << ", " << (int)(fNumLoGainSamples+fLoGainFirst-1);
        }
    }
    out << ");" << endl;
}
