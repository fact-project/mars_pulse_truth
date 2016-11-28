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
!   Author(s): Oscar Blanch 12/2001 <mailto:blanch@ifae.es>
!   Author(s): Thomas Bretz 08/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Stefan Ruegamer <mailto:snruegam@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MBadPixelsTreat
//
//  You can use MBadPixelsTreat::SetUseInterpolation to replaced the
//  bad pixels by the average of its neighbors instead of unmapping
//  them. If you want to include the central pixel use
//  MBadPixelsTreat::SetUseCentralPixel. The bad pixels are taken from
//  an existing MBadPixelsCam.
//
//  It check if there are enough neighbors to calculate the mean
//  If not, unmap the pixel. The minimum number of good neighbors
//  should be set using SetNumMinNeighbors
//
//  If you want to interpolate unreliable pixels and  unsuitable
//  (broken) pixels use SetHardTreatment().
//
//
// Options:
// --------
//   SetHardTreatment:      Also interpolate unreliable pixels not only unsuitable
//   SetUseInterpolation:   Interpolate pixels instead of unmapping them
//   SetUseCentralPixel:    also use the pixel itself for interpolation
//   SetProcessPedestalRun: process the pedestals once per run/file
//   SetProcessPedestalEvt: process the pedestal once per event
//   SetProcessTimes:       do interpolation of the arrival time
//
// If the arrival time treatment is switched on and "MPedPhotFromExtractor"
// and "MPedPhotFromExtractorRndm" are found the pixel is filled with
// a random gaus calculated from these two MPedPhotCams in the case
// the pixels is detected as background.
//
//
//  Input Containers:
//   MSignalCam
//   MPedPhotCam
//   MBadPixelsCam
//   [MGeomCam]
//
//  Output Containers:
//   MSignalCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MBadPixelsTreat.h"

#include <fstream>

#include <TEnv.h>
#include <TRandom.h>
#include <TObjString.h>

//#include "MArrayD.h" // Used instead of TArrayD because operator[] has no range check

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeom.h"
#include "MGeomCam.h"

#include "MSignalPix.h"
#include "MSignalCam.h"

#include "MPedPhotPix.h"
#include "MPedPhotCam.h"

#include "MBadPixelsPix.h"
#include "MBadPixelsCam.h"

ClassImp(MBadPixelsTreat);

using namespace std;

static const TString gsDefName  = "MBadPixelsTreat";
static const TString gsDefTitle = "Task to treat bad pixels (interpolation, unmapping)";

// --------------------------------------------------------------------------
//
// Default constructor.
//
MBadPixelsTreat::MBadPixelsTreat(const char *name, const char *title)
    : fGeomCam(0), fEvt(0), fBadPixels(0), fPedPhot1(0), fPedPhot2(0),
    fNameSignalCam("MSignalCam"),
    fFlags(0), fNumMinNeighbors(3), fMaxArrivalTimeDiff(3.)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    SetUseInterpolation();
    SetProcessPedestal();
    SetProcessTimes();
}

// --------------------------------------------------------------------------
//
// Returns the status of a pixel. If kHardTreatment is set a pixel must
// be unsuitable or uriliable to be treated. If not it is treated only if 
// it is unsuitable
// (IsBad() checks for any flag)
//
Bool_t MBadPixelsTreat::IsPixelBad(Int_t idx) const
{
    return TESTBIT(fFlags, kHardTreatment) ? (*fBadPixels)[idx].IsBad():(*fBadPixels)[idx].IsUnsuitable();
}

void MBadPixelsTreat::AddNamePedPhotCam(const char *name)
{
    fNamePedPhotCams.Add(new TObjString(name));
}

// --------------------------------------------------------------------------
//
//  - Try to find or create MBlindPixels in parameter list.
//  - get the MSignalCam from the parlist (abort if missing)
//  - if no pixels are given by the user try to determin the starfield
//    from the monte carlo run header.
//
Int_t MBadPixelsTreat::PreProcess (MParList *pList)
{
    fBadPixels = (MBadPixelsCam*)pList->FindObject(AddSerialNumber("MBadPixelsCam"));
    if (!fBadPixels)
    {
        *fLog << err << AddSerialNumber("MBadPixelsCam") << " not found... aborting." << endl;
        return kFALSE;
    }

    fEvt = (MSignalCam*)pList->FindObject(AddSerialNumber(fNameSignalCam), "MSignalCam");
    if (!fEvt)
    {
        *fLog << err << AddSerialNumber(fNameSignalCam) << " [MSignalCam] not found... aborting." << endl;
        return kFALSE;
    }

    fGeomCam = 0;
    if (!IsUseInterpolation())
        return kTRUE;

    fGeomCam = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
    if (!fGeomCam)
    {
        *fLog << err << AddSerialNumber("MGeomCam") << " not found - can't use interpolation... abort." << endl;
        *fLog << " Use MBadPixelsTreat::SetUseInterpolation(kFALSE) to switch interpolation" << endl;
        *fLog << " off and use unmapping instead." << endl;
        return kFALSE;
    }

    const Bool_t proc = IsProcessPedestalEvt() || IsProcessPedestalRun();

    if (fNamePedPhotCams.GetSize()>0 && !proc)
    {
        *fLog << err << "Pedestal list contains entries, but pedestal treatment is switched off... abort." << endl;
        return kFALSE;
    }

    if (proc)
    {
        if (fNamePedPhotCams.GetSize()==0)
        {
            *fLog << inf << "No container names specified... using default: MPedPhotCam." << endl;
            AddNamePedPhotCam();
        }

        fPedPhotCams.Clear();

        TIter Next(&fNamePedPhotCams);
        TObject *o=0;
        while ((o=Next()))
        {
            TObject *p = pList->FindObject(AddSerialNumber(o->GetName()), "MPedPhotCam");
            if (!p)
            {
                *fLog << err << AddSerialNumber(o->GetName()) << " [MPedPhotCam] not found... aborting." << endl;
                return kFALSE;
            }

            fPedPhotCams.Add(p);
        }
    }

    if (IsProcessTimes())
    {
        fPedPhot1 = (MPedPhotCam*)pList->FindObject("MPedPhotFromExtractor",     "MPedPhotCam");
        fPedPhot2 = (MPedPhotCam*)pList->FindObject("MPedPhotFromExtractorRndm", "MPedPhotCam");

        *fLog << inf << "Additional no-signal-interpolation switched ";
        *fLog << (fPedPhot1 && fPedPhot2 ? "on" : "off");
        *fLog << "." << endl;

        if (fPedPhot1 && fPedPhot2)
            *fLog << "Maximum arrival time difference: " << fMaxArrivalTimeDiff << "ns" << endl;

    }

    *fLog << inf << "Minimum number of neighbor pixels: " << (int)fNumMinNeighbors << endl;

    if (IsProcessPedestalEvt())
        *fLog << "Processing Pedestals once per event..." << endl;
    if (IsProcessPedestalRun())
        *fLog << "Processing Pedestals once per run..." << endl;
    if (IsProcessTimes())
        *fLog << "Processing Arrival Times once per event..." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Replaces each pixel (signal, signal error, pedestal, pedestal rms)
//  by the average of its surrounding pixels.
//  If TESTBIT(fFlags, kUseCentralPixel) is set the central pixel is also
//  included.
//
void MBadPixelsTreat::InterpolateSignal() const
{
    const UShort_t entries = fGeomCam->GetNumPixels();

    //
    // Loop over all pixels
    //
    for (UShort_t i=0; i<entries; i++)
    {
        //
        // Check whether pixel with idx i is blind
        //
        if (!IsPixelBad(i))
            continue;

        //
        // Get the corresponding geometry and pedestal
        //
        MSignalPix  &pix  = (*fEvt)[i];
        const MGeom &gpix = (*fGeomCam)[i];

        // Do Not-Use-Central-Pixel
        const Bool_t nucp = !TESTBIT(fFlags, kUseCentralPixel);

        Int_t num = nucp ? 0 : 1;

        Double_t nphot  = nucp ? 0 : pix.GetNumPhotons();
        Double_t perr   = nucp ? 0 : Pow2(pix.GetErrorPhot());

        //
	// The values are rescaled to the small pixels area for the right comparison
        //
        const Double_t ratio = fGeomCam->GetPixRatio(i);

        nphot *= ratio;
        perr  *= ratio;

        //
        // Loop over all its neighbors
        //
        const Int_t n = gpix.GetNumNeighbors();
        for (int j=0; j<n; j++)
        {
            const UShort_t nidx = gpix.GetNeighbor(j);

            //
            // Do not use blind neighbors
            //
            if (IsPixelBad(nidx))
                continue;

            //
            // Get the geometry for the neighbor
            //
            const Double_t nratio = fGeomCam->GetPixRatio(nidx);

            //
	    //The error is calculated as the quadratic sum of the errors
	    //
            const MSignalPix &evtpix = (*fEvt)[nidx];

            nphot += nratio*evtpix.GetNumPhotons();
            perr  += nratio*Pow2(evtpix.GetErrorPhot());

            num++;
        }

	// Check if there are enough neighbors to calculate the mean
        // If not, unmap the pixel. The maximum number of blind neighbors
        // should be 2
        if (num<fNumMinNeighbors)
        {
            pix.SetPixelUnmapped();
            continue;
        }

        //
        // Now the mean is calculated and the values rescaled back
        // to the pixel area
        //
	nphot /= num*ratio;
        perr   = TMath::Sqrt(perr/(num*ratio));
 
        pix.Set(nphot, perr);
    }
}

// --------------------------------------------------------------------------
//
void MBadPixelsTreat::InterpolatePedestals(MPedPhotCam &pedphot) const
{
    const Int_t entries = pedphot.GetSize();

    //
    // Loop over all pixels
    //
    for (UShort_t i=0; i<entries; i++)
    {
        //
        // Check whether pixel with idx i is blind
        //
        if (!IsPixelBad(i))
            continue;

        //
        // Get the corresponding geometry and pedestal
        //
        const MGeom       &gpix = (*fGeomCam)[i];
        const MPedPhotPix &ppix = pedphot[i];

        // Do Not-Use-Central-Pixel
        const Bool_t nucp = !TESTBIT(fFlags, kUseCentralPixel);

        Int_t num = nucp ? 0 : 1;

        Double_t ped = nucp ? 0 : ppix.GetMean();
        Double_t rms = nucp ? 0 : Pow2(ppix.GetRms());

        //
        // The values are rescaled to the small pixels area for the right comparison
        //
        const Double_t ratio = fGeomCam->GetPixRatio(i);

        ped *= ratio;
        rms *= ratio;

        //
        // Loop over all its neighbors
        //
        const Int_t n = gpix.GetNumNeighbors();
        for (int j=0; j<n; j++)
        {
            const UShort_t nidx = gpix.GetNeighbor(j);

            //
            // Do not use blind neighbors
            //
            if (IsPixelBad(nidx))
                continue;

            //
            // Get the geometry for the neighbor
            //
            const Double_t    nratio = fGeomCam->GetPixRatio(nidx);
            const MPedPhotPix &nppix = pedphot[nidx];

            //
            //The error is calculated as the quadratic sum of the errors
            //
            ped += nratio*nppix.GetMean();
            rms += nratio*Pow2(nppix.GetRms());

            num++;
        }

        // Check if there are enough neighbors to calculate the mean
        // If not, unmap the pixel. The minimum number of good neighbors
        // should be fNumMinNeighbors
        if (num<fNumMinNeighbors)
        {
            (*fEvt)[i].SetPixelUnmapped();
            continue;
        }

        //
        // Now the mean is calculated and the values rescaled back
        // to the pixel area
        //
        ped /= num*ratio;
        rms  = TMath::Sqrt(rms/(num*ratio));

        pedphot[i].Set(ped, rms);
    }
    pedphot.SetReadyToSave();
}

// --------------------------------------------------------------------------
//
// loop over all MPedPhotCam and interpolate them
//
void MBadPixelsTreat::InterpolatePedestals() const
{
    TIter Next(&fPedPhotCams);
    MPedPhotCam *cam=0;
    while ((cam=(MPedPhotCam*)Next()))
    {
        InterpolatePedestals(*cam);
        cam->ReCalc(*fGeomCam, fBadPixels);
    }
}

// --------------------------------------------------------------------------
//
void MBadPixelsTreat::InterpolateTimes() const
{
    const Int_t n = fEvt->GetNumPixels();
    for (int i=0; i<n; i++)
    {
        // Check whether pixel with idx i is bad
        if (!IsPixelBad(i))
            continue;

        // Geometry of bad pixel
        const MGeom &gpix = (*fGeomCam)[i];

        // Number of neighbor pixels
        const Int_t n2 = gpix.GetNumNeighbors();

        // Copy the arrival time of all neighboring bad pixels
        // to a new array for simplicity
        Double_t time[6];
        Int_t cnt = 0;
        for (Int_t j=0; j<n2; j++)
        {
            const Int_t idx = gpix.GetNeighbor(j);
            if (!IsPixelBad(idx))
                time[cnt++] = (*fEvt)[idx].GetArrivalTime();
        }

        // if there are too few neighbours, don't interpolate the pixel
        //if ((cnt < 3 && n2 > 3) || (cnt < 2 && n2 == 3))
        if (cnt<fNumMinNeighbors)
        {
            (*fEvt)[i].SetPixelUnmapped();
            continue;
        }

        Double_t min =  FLT_MAX; // Find minimum arrival time
        Double_t max = -FLT_MAX; // Find maximum arrival time

        Double_t sum2 = 0; // Sum of arrival times of the pixels
        Int_t    cnt2 = 0; // Number of pixels summed in sum2

        for (Int_t j=0; j<cnt; j++)
        {
            const Double_t tm1 = time[j];         // time of one neighbor pixel
            const Double_t tm2 = time[(j+1)%cnt]; // time of its neighbor pixel

            // Calculate mean arrival time of pixel probably inside the shower
            if (TMath::Abs(tm1 - tm2)<fMaxArrivalTimeDiff)
            {
                sum2 += tm1+tm2;
                cnt2++;
            }

            // Find minimum arrival time
            if (tm1<min)
                min = tm1;

            // Find maximum arrival time
            if (tm1>max)
                max = tm1;
        }

        // If less than two nbeighbors belong to a shower the pixel doesn't
        // belong to the shower, too. Set the arrival time to a uniform
        // random value, otherwise use the mean value of the pixels belonging
        // to the shower.
        if (cnt2<=2)
        {
            sum2 = gRandom->Uniform(max-min)+min; // FIXME? Set Seed value?

            // Proceed with a treatment of the signal of empty pixels
            // better than the interpolation. (FIXME: Maybe a function
            // different from a gaussian could be a better choice...)
            if (fPedPhot1 && fPedPhot2)
            {
                const Int_t    aidx = gpix.GetAidx();
                // This is to which bias level the signal fluctuates
                const Double_t mean = fPedPhot1->GetArea(aidx).GetMean();
                // This is how the signal fluctuates
                const Double_t rms  = fPedPhot2->GetArea(aidx).GetRms();
                const Double_t phe  = gRandom->Gaus(mean, rms);

                (*fEvt)[i].SetNumPhotons(phe);
            }
        }
        else
            sum2 /= cnt2*2;

        (*fEvt)[i].SetArrivalTime(sum2);
    }
}

// --------------------------------------------------------------------------
//
//  Removes all blind pixels from the analysis by setting their state
//  to unused.
//
void MBadPixelsTreat::Unmap() const
{
    const UShort_t entries = fEvt->GetNumPixels();

    //
    // remove the pixels in fPixelsIdx if they are set to be used,
    // (set them to 'unused' state)
    //
    for (UShort_t i=0; i<entries; i++)
    {
        if (IsPixelBad(i))
            (*fEvt)[i].SetPixelUnmapped();
    }
}

// --------------------------------------------------------------------------
//
// Interpolate Pedestals if kProcessPedestal not set
//
Bool_t MBadPixelsTreat::ReInit(MParList *pList)
{
    if (IsUseInterpolation() && IsProcessPedestalRun())
        InterpolatePedestals();
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Treat the blind pixels
//
Int_t MBadPixelsTreat::Process()
{
    if (IsUseInterpolation())
    {
        InterpolateSignal();
        if (IsProcessPedestalEvt())
            InterpolatePedestals();
        if (IsProcessTimes())
            InterpolateTimes();
    }
    else
        Unmap();

    return kTRUE;
}

void MBadPixelsTreat::StreamPrimitive(ostream &out) const
{
    out << "   MBadPixelsTreat " << GetUniqueName();
    if (fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << "(\"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\"";
        out <<")";
    }
    out << ";" << endl;

    if (IsUseInterpolation())
        out << "   " << GetUniqueName() << ".SetUseInterpolation();" << endl;
    if (IsUseCentralPixel())
        out << "   " << GetUniqueName() << ".SetUseCentralPixel();" << endl;
    if (IsProcessPedestalRun())
        out << "   " << GetUniqueName() << ".SetProcessPedestalRun();" << endl;
    if (IsProcessPedestalEvt())
        out << "   " << GetUniqueName() << ".SetProcessPedestalEvt();" << endl;
    if (IsProcessTimes())
        out << "   " << GetUniqueName() << ".SetProcessTimes();" << endl;
    if (IsHardTreatment())
        out << "   " << GetUniqueName() << ".SetHardTreatment();" << endl;
    if (fNumMinNeighbors!=3)
        out << "   " << GetUniqueName() << ".SetNumMinNeighbors(" << (int)fNumMinNeighbors << ");" << endl;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MBadPixelsTreat.UseInterpolation:    no
//   MBadPixelsTreat.UseCentralPixel:     no
//   MBadPixelsTreat.HardTreatment:       no
//   MBadPixelsTreat.ProcessPedestalRun:  no
//   MBadPixelsTreat.ProcessPedestalEvt:  no
//   MBadPixelsTreat.NumMinNeighbors:     3
//   MBadPixelsTreat.MaxArrivalTimeDiff:  0.9
//
Int_t MBadPixelsTreat::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "UseInterpolation", print))
    {
        rc = kTRUE;
        SetUseInterpolation(GetEnvValue(env, prefix, "UseInterpolation", IsUseInterpolation()));
    }
    if (IsEnvDefined(env, prefix, "UseCentralPixel", print))
    {
        rc = kTRUE;
        SetUseCentralPixel(GetEnvValue(env, prefix, "UseCentralPixel", IsUseCentralPixel()));
    }
    if (IsEnvDefined(env, prefix, "HardTreatment", print))
    {
        rc = kTRUE;
        SetHardTreatment(GetEnvValue(env, prefix, "HardTreatment", IsHardTreatment()));
    }
    if (IsEnvDefined(env, prefix, "ProcessPedestalRun", print))
    {
        rc = kTRUE;
        SetProcessPedestalRun(GetEnvValue(env, prefix, "ProcessPedestalRun", IsProcessPedestalRun()));
    }
    if (IsEnvDefined(env, prefix, "ProcessPedestalEvt", print))
    {
        rc = kTRUE;
        SetProcessPedestalEvt(GetEnvValue(env, prefix, "ProcessPedestalEvt", IsProcessPedestalEvt()));
    }
    if (IsEnvDefined(env, prefix, "ProcessTimes", print))
    {
        rc = kTRUE;
        SetProcessTimes(GetEnvValue(env, prefix, "ProcessTimes", IsProcessTimes()));
    }
    if (IsEnvDefined(env, prefix, "NumMinNeighbors", print))
    {
        rc = kTRUE;
        SetNumMinNeighbors(GetEnvValue(env, prefix, "NumMinNeighbors", fNumMinNeighbors));
    }
    if (IsEnvDefined(env, prefix, "MaxArrivalTimeDiff", print))
    {
        rc = kTRUE;
        SetMaxArrivalTimeDiff(GetEnvValue(env, prefix, "MaxArrivalTimeDiff", fMaxArrivalTimeDiff));
    }
    return rc;
}
