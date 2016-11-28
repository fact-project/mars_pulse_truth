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
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MExtractSignal
//
//////////////////////////////////////////////////////////////////////////////
#include "MExtractSignal.h"

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

ClassImp(MExtractSignal);

using namespace std;

const Byte_t MExtractSignal::fgSaturationLimit = 254;
const Byte_t MExtractSignal::fgFirst =  3;
const Byte_t MExtractSignal::fgLast  = 14;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
MExtractSignal::MExtractSignal(const char *name, const char *title)
    : fSaturationLimit(fgSaturationLimit)
{

    fName  = name  ? name  : "MExtractSignal";
    fTitle = title ? title : "Task to extract the signal from the FADC slices";

    AddToBranchList("MRawEvtData.*");

    SetRange();
}

void MExtractSignal::SetRange(Byte_t hifirst, Byte_t hilast, Byte_t lofirst, Byte_t lolast)
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
Int_t MExtractSignal::PreProcess(MParList *pList)
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

void MExtractSignal::FindSignal(Byte_t *ptr, Int_t size, Int_t &sum, Byte_t &sat) const
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
Int_t MExtractSignal::Process()
{
    MRawEvtPixelIter pixel(fRawEvt);
    fSignals->Clear();

    UInt_t  sat=0;

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

        const Int_t pixid = pixel.GetPixelId();

        const MPedestalPix  &ped = (*fPedestals)[pixid]; 
	MExtractedSignalPix &pix = (*fSignals)[pixid];

        const Float_t pedes  = ped.GetPedestal();
        const Float_t pedrms = ped.GetPedestalRms();

        pix.SetExtractedSignal(sumhi - pedes*fNumHiGainSamples, pedrms*fSqrtHiGainSamples,
                               sumlo - pedes*fNumLoGainSamples, pedrms*fSqrtLoGainSamples);

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
void MExtractSignal::StreamPrimitive(ostream &out) const
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
