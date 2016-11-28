/* ======================================================================== *\
!
! *
! * This file is part of CheObs, the Modular Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appears in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Qi Zhe,       06/2007 <mailto:qizhe@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MPhotonEvent
//
// Storage container to store photon collections
//
// The class is designed to be extremely fast which is important taking into
// account the extremely high number of photons. This has some impacts on
// its handling.
//
// The list has to be kept consistent, i.e. without holes.
//
// There are two ways to achieve this:
//
//   a) Use RemoveAt to remove an entry somewhere
//   b) Compress() the TClonesArray afterwards
//
// Compress is not the fastes, so there is an easier way.
//
//   a) When you loop over the list and want to remove an entry copy all
//      following entry backward in the list, so that the hole will
//      be created at its end.
//   b) Call Shrink(n) with n the number of valid entries in your list.
//
// To loop over the TClonesArray you can use a TIter which has some
// unnecessary overhead and therefore is slower than necessary.
//
// Since the list is kept consistent you can use a simple loop saving
// a lot of CPU time taking into account the high number of calls to
// TObjArrayIter::Next which you would create.
//
// Here is an example (how to remove every second entry)
//
//  ---------------------------------------------------------------------
//
//    Int_t cnt = 0;
//
//    const Int_t num = event->GetNumPhotons();
//    for (Int_t idx=0; idx<num; idx++)
//    {
//        if (idx%2==0)
//           continue;
//
//        MPhotonData *dat = (*event)[idx];
//
//        (*event)[cnt++] = *dat;
//     }
//
//     event->Shrink(cnt);
//
//  ---------------------------------- or -------------------------------
//
//    TClonesArray &arr = MPhotonEvent->GetArray();
//
//    Int_t cnt = 0;
//
//    const Int_t num = arr.GetEntriesFast();
//    for (Int_t idx=0; idx<num; idx++)
//    {
//        if (idx%2==0)
//           continue;
//
//        MPhotonData *dat = static_cast<MPhotonData*>(arr.UncheckedAt(idx));
//
//        *static_cast<MPhotonData*>(arr.UncheckedAt(cnt++)) = *dat;
//     }
//
//     MPhotonEvent->Shrink(cnt);
//
//  ---------------------------------------------------------------------
//
// The flag for a sorted array is for speed reasons not in all conditions
// maintained automatically. Especially Add() doesn't reset it.
//
// So be sure that if you want to sort your array it is really sorted.
//
//
//   Version 1:
//   ----------
//    - First implementation
//
/////////////////////////////////////////////////////////////////////////////
#include "MPhotonEvent.h"

#include <fstream>
#include <iostream>

#include <TMarker.h>

#include <MMath.h>

#include "MArrayF.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MPhotonData.h"
#include "MCorsikaFormat.h"

ClassImp(MPhotonEvent);

using namespace std;

// ==========================================================================

class MyClonesArray : public TClonesArray
{
public:
    TObject **FirstRef() { return fCont; }

    // --------------------------------------------------------------------------
    //
    // This is an extremly optimized version of ExpandCreateFast. It only resets
    // the marker for the last element (fLast) to n-1 and doen't change anything
    // else. This implicitly assumes that the stored objects don't allocate
    // memory. It does not necessarily mean that the slots after fLast
    // are empty (set to 0). This is what is assumed in the TClonesArray.
    // We also don't call Changed() because it would reset Sorted. If the
    // array was sorted before it is also sorted now. You MUST make sure
    // that you only set n in a range for which valid entries have been
    // created before (e.g. by ExpandCreateFast).
    //
    void FastShrink(Int_t n)
    {
        fLast = n - 1;
    }

    // --------------------------------------------------------------------------
    //
    // This is a optimized (faster) version of Delete which deletes consequtive
    // entries from index idx1 to idx2 (both included) and calls their
    // destructor. Note that there is no range checking done!
    //
    void FastRemove(Int_t idx1, Int_t idx2)
    {
        // Remove object at index idx.

        //if (!BoundsOk("RemoveAt", idx1)) return 0;
        //if (!BoundsOk("RemoveAt", idx2)) return 0;

        Long_t dtoronly = TObject::GetDtorOnly();

        idx1 -= fLowerBound;
        idx2 -= fLowerBound;

        for (TObject **obj=fCont+idx1; obj<=fCont+idx2; obj++)
        {
            if (!*obj)
                continue;

            if ((*obj)->TestBit(kNotDeleted)) {
                // Tell custom operator delete() not to delete space when
                // object fCont[i] is deleted. Only destructors are called
                // for this object.
                TObject::SetDtorOnly(*obj);
                delete *obj;
            }

            *obj = 0;
            // recalculate array size
        }
        TObject::SetDtorOnly((void*)dtoronly);

        if (idx1<=fLast && fLast<=idx2)
        {
            do {
                fLast--;
            } while (fLast >= 0 && fCont[fLast] == 0);
        }

        Changed();
    }


    //void SetSorted() { fSorted = kTRUE; }

    // --------------------------------------------------------------------------
    //
    // This is an optimized version of Sort which doesn't check the
    // IsSortable flag before. It only sorts the entries from 0
    // to GetEntriesFast().
    //
    void UncheckedSort()
    {
        if (fSorted)
            return;

        const Int_t nentries = GetEntriesFast();
        if (nentries <= 0)
            return;

        QSort(GetObjectRef(First()), fKeep->GetObjectRef(fKeep->First()),
              0, TMath::Min(nentries, kMaxInt-fLowerBound));

        fSorted = kTRUE;
    }
};

// ==========================================================================

// --------------------------------------------------------------------------
//
// Default constructor. It initializes all arrays with zero size.
//
MPhotonEvent::MPhotonEvent(const char *name, const char *title)
    : fData("MPhotonData", 1)
{
    fName  = name  ? name  : "MPhotonEvent";
    fTitle = title ? title : "Corsika Event Data Information";

    fData.SetBit(TClonesArray::kForgetBits);
    fData.BypassStreamer(kFALSE);
}

// --------------------------------------------------------------------------
//
// This is an extremly optimized version of ExpandCreateFast. It only resets
// the marker for the last element (fLast) to n-1 and doen't change anything
// else. This has the advantage that the allocated memory is kept but only
// valid entries are written to a file.
//
// If the array was sorted before it is also sorted now. You MUST make sure
// that you only set n in a range for which valid entries have been
// created before (e.g. by ExpandCreateFast).
//
Int_t MPhotonEvent::Shrink(Int_t n)
{
    /*
    // The number of object written by the streamer is defined by
    // GetEntriesFast(), i.e. this number must be shrinked to
    // the real array size. We use RemoveAt instead of ExpandCreate
    // because RemoveAt doesn't free memory. Thus in the next
    // iteration it can be reused and doesn't need to be reallocated.
    // Do not change this. It is optimized for execution speed
    //        for (int i=fData.GetSize()-1; i>=n; i--)
    //          fData.RemoveAt(i);
    const Bool_t sorted = fData.IsSorted();

    MyClonesArray &loc = static_cast<MyClonesArray&>(fData);

    loc.FastRemove(n, fData.GetSize()-1);

    // If it was sorted before it is also sorted now.
    if (sorted)
        loc.SetSorted();
    */

    // fData.ExpandCreateFast(n);  // Just set fLast = n -1

    // Just set fLast = n -1
    static_cast<MyClonesArray&>(fData).FastShrink(n);
    return fData.GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// The resized the array. If it has to be increased in size it is done
// with ExpandCreateFast. If it should be shrinked it is done with
// ExpandCreateFast if n>fData.GetSize()/100 or n==0. This keeps the allocated
// memory and just sets the marker for the last element in the array (fLast).
//
// If the allocated memory grew to huge we reset the allocated memory
// by calling ExpandCreate(n) (frees the allocated storage for the
// objects) and Expand(n) (frees the allocated memory for the list
// of pointers to the objects)
//
// 100 hundred is an arbitrary number taking into account that todays
// computers have a lot of memory and we don't want to free and allocate
// new memory too often.
//
// In priciple there might be more clever methods to handle the memory.
//
void MPhotonEvent::Resize(Int_t n)
{
    if (n==0 || n*100>fData.GetSize())
        fData.ExpandCreateFast(n);  // Just set fLast = n -1
    else
    {
        fData.ExpandCreate(n);      // Free memory of allocated MPhotonData
        fData.Expand(n);            // Free memory of allocated pointers
    }
}

// Overload the AsciiWrite method to store the informations of the photons onto disc.
// For each corsika/pedestal... event there is a simple header written. After this
// the informations for each photon are written in one line.
Bool_t MPhotonEvent::AsciiWrite(ostream &out) const
{
    // Write a simple header. Be aware there will be written one header for each
    // corsika/pedestal... event. Each photon of the event will be written in one
    // line.
    out << "#";
    out << "photonID" << ",";
    out << "tag" << ",";
    out << "mirrorTag" << ",";
    out << "posX" << ",";
    out << "posY" << ",";
    out << "cosU" << ",";
    out << "cosV" << ",";
    out << "time" << ",";
    out << "weight" << ",";
    out << "wavelength" << ",";
    out << "productionHeight" << ",";
    out << "primaryID" << endl;

    // Get number of photons
    const Int_t num = GetNumPhotons();

    // Loop over all photons
    for (Int_t i=0; i<num; i++)
    {
        // Get i-th photon
        const MPhotonData &ph = operator[](i);

        out << i << "," ;
        out << ph.GetTag() << "," ;
        out << ph.GetMirrorTag() << "," ;
        out << ph.GetPosX() << "," ;
        out << ph.GetPosY() << "," ;
        out << ph.GetCosU() << "," ;
        out << ph.GetCosV() << "," ;
        out << ph.GetTime() << "," ;
        out << ph.GetWeight() << "," ;
        out << ph.GetWavelength() << "," ;
        out << ph.GetProductionHeight() << "," ;
        out << ph.GetPrimary() << endl ;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// If n is smaller than the current allocated array size a reference to
// the n-th entry is returned, otherwise an entry at n is created
// calling the default constructor. Note, that there is no range check
// but it is not recommended to call this function with
// n>fData.GetSize()
//
MPhotonData &MPhotonEvent::Add(Int_t n)
{
    // Do not modify this. It is optimized for execution
    // speed and flexibility!
    TObject *o = 0;
    if (n<fData.GetSize() && fData.UncheckedAt(n))
    {
        o=fData.UncheckedAt(n);
        static_cast<MyClonesArray&>(fData).FastShrink(n+1);
    }
    else
    {
        o=fData.New(n);
    }
    return static_cast<MPhotonData&>(*o);
}

// --------------------------------------------------------------------------
//
// Add a new photon (MPhtonData) at the end of the array.
// In this case the default constructor of MPhotonData is called.
//
// A reference to the new object is returned.
//
MPhotonData &MPhotonEvent::Add()
{
    return Add(GetNumPhotons());
}

void MPhotonEvent::Sort(Bool_t force)
{
    if (force)
        fData.UnSort();

    static_cast<MyClonesArray&>(fData).UncheckedSort(); /*Sort(GetEntriesFast())*/
}

// --------------------------------------------------------------------------
//
// Get the i-th photon from the array. Not, for speed reasons there is no
// range check so you are responsible that you do not excess the number
// of photons (GetNumPhotons)
//
MPhotonData &MPhotonEvent::operator[](UInt_t idx)
{
    return *static_cast<MPhotonData*>(fData.UncheckedAt(idx));
}

// --------------------------------------------------------------------------
//
// Get the i-th photon from the array. Not, for speed reasons there is no
// range check so you are responsible that you do not excess the number
// of photons (GetNumPhotons)
//
const MPhotonData &MPhotonEvent::operator[](UInt_t idx) const
{
    return *static_cast<MPhotonData*>(fData.UncheckedAt(idx));
}

// --------------------------------------------------------------------------
//
// Return a pointer to the first photon if available.
//
MPhotonData *MPhotonEvent::GetFirst() const
{
    return fData.GetEntriesFast()==0 ? 0 : static_cast<MPhotonData*>(fData.First());
}

// --------------------------------------------------------------------------
//
// Return a pointer to the last photon if available.
//
MPhotonData *MPhotonEvent::GetLast() const
{
    return fData.GetEntriesFast()==0 ? 0 : static_cast<MPhotonData*>(fData.Last());
}

// --------------------------------------------------------------------------
//
// Return the number of "external" photons, i.e. which are not NightSky
//
Int_t MPhotonEvent::GetNumExternal() const
{
    const Int_t n=GetNumPhotons();

    Int_t rc = 0;
    for (int i=0; i<n; i++)
        if ((*this)[i].GetPrimary()!=MMcEvtBasic::kNightSky && (*this)[i].GetPrimary()!=MMcEvtBasic::kArtificial)
            rc++;

    return rc;
}

// --------------------------------------------------------------------------
//
// Return time of first photon, 0 if none in array.
// Note: If you want this to be the earliest make sure that the array
// is properly sorted.
//
Float_t MPhotonEvent::GetTimeFirst() const
{
    const MPhotonData *dat=GetFirst();
    return dat ? dat->GetTime() : 0;
}

// --------------------------------------------------------------------------
//
// Return time of first photon, 0 if none in array.
// Note: If you want this to be the latest make sure that the array
// is properly sorted.
//
Float_t MPhotonEvent::GetTimeLast() const
{
    const MPhotonData *dat=GetLast();
    return dat ? dat->GetTime() : 0;
}

// --------------------------------------------------------------------------
//
// Return the median devian from the median of all arrival times.
// The median deviation is calculated using MMath::MedianDev.
// It is the half width in which one sigma (~68%) of all times are
// contained around the median.
//
Double_t MPhotonEvent::GetTimeMedianDev() const
{
    const UInt_t n = GetNumPhotons();

    MArrayF arr(n);
    for (UInt_t i=0; i<n; i++)
        arr[i] = operator[](i).GetTime();

    return MMath::MedianDev(n, arr.GetArray()/*, Double_t &med*/);
}

Double_t MPhotonEvent::GetMeanX() const
{
    const UInt_t n = GetNumPhotons();
    if (n==0)
        return 0;

    Double_t mean = 0;
    for (UInt_t i=0; i<n; i++)
        mean += operator[](i).GetPosX();

    return mean / n;
}

Double_t MPhotonEvent::GetMeanY() const
{
    const UInt_t n = GetNumPhotons();
    if (n==0)
        return 0;

    Double_t mean = 0;
    for (UInt_t i=0; i<n; i++)
        mean += operator[](i).GetPosY();

    return mean / n;
}

Double_t MPhotonEvent::GetMeanT() const
{
    const UInt_t n = GetNumPhotons();
    if (n==0)
        return 0;

    Double_t mean = 0;
    for (UInt_t i=0; i<n; i++)
        mean += operator[](i).GetTime();

    return mean / n;
}

void MPhotonEvent::AddXY(Double_t x, Double_t y)
{
    const UInt_t n = GetNumPhotons();

    for (UInt_t i=0; i<n; i++)
    {
        MPhotonData &p = operator[](i);
        p.SetPosition(p.GetPosX()+x, p.GetPosY()+y);
    }
}

void MPhotonEvent::SimWavelength(Float_t wmin, Float_t wmax)
{
    const UInt_t n = GetNumPhotons();
    if (n==0)
        return;

    // Corsika has already produced and written the wavelength
    if (operator[](0).GetWavelength()>0)
        return;

    for (UInt_t i=0; i<n; i++)
        operator[](i).SimWavelength(wmin, wmax);
}

Int_t MPhotonEvent::ReadEventIoEvtCompact(MCorsikaFormat *fInFormat)
{
   Int_t bunchHeader[3];
   fInFormat->Read(bunchHeader, 3 * sizeof(Int_t));

   Int_t n = 0;

   for (int bunch = 0; bunch < bunchHeader[2]; bunch++)
   {
       Short_t buffer[8];
       fInFormat->Read(buffer, 8 * sizeof(Short_t));

       if (Add(n).FillEventIO(buffer))
           n++;
   }

   Resize(n);
   fData.UnSort();

   SetReadyToSave();

   //*fLog << all << "Number of photon bunches: " << fData.GetEntriesFast() << endl;
   return kTRUE;

}

Int_t MPhotonEvent::ReadEventIoEvt(MCorsikaFormat *fInFormat)
{
   Int_t  bunchHeader[3];
   fInFormat->Read(bunchHeader, 3 * sizeof(Int_t));

   Int_t n = 0;

   for (int bunch = 0; bunch < bunchHeader[2]; bunch++)
      {
      Float_t buffer[8];
      fInFormat->Read(buffer, 8 * sizeof(Float_t));

      if (Add(n).FillEventIO(buffer))
         n++;
      }

   Resize(n);
   fData.UnSort();

   SetReadyToSave();

   //*fLog << all << "Number of photon bunches: " << fData.GetEntriesFast() << endl;
   return kTRUE;

}

Int_t MPhotonEvent::ReadCorsikaEvt(Float_t * data, Int_t numEvents, Int_t arrayIdx)
{
   Int_t n = 0;

   for (Int_t event = 0; event < numEvents; event++)
      {
      const Int_t rc = Add(n).FillCorsika(data + 7 * event, arrayIdx);

      switch (rc)
      {
      case kCONTINUE:  continue;        // No data in this bunch... skip it.
      case kERROR:     return kERROR;   // Error occured
      }

      // This is a photon we would like to keep later.
      // Increase the counter by one
      n++;
      }

   Resize(n);
   fData.UnSort();

   SetReadyToSave();

   //*fLog << all << "Number of photon bunches: " << fData.GetEntriesFast() << endl;
   return kTRUE;

}

// --------------------------------------------------------------------------
//
// Print the array
//
void MPhotonEvent::Print(Option_t *) const
{
    // This is much faster than looping over all entries and discarding
    // the empty ones
    const UInt_t n = GetNumPhotons();
    for (UInt_t i=0; i<n; i++)
        operator[](i).Print();
}

// ------------------------------------------------------------------------
//
// You can call Draw() to add the photons to the current pad.
// The photons are painted each tim ethe pad is updated.
// Make sure that you use the right (world) coordinate system,
// like created, eg. by the MHCamera histogram.
//
void MPhotonEvent::Paint(Option_t *)
{
    MPhotonData *ph=NULL;

    TMarker m;
    m.SetMarkerStyle(kFullDotMedium); // Gtypes.h

    TIter Next(&fData);
    while ((ph=(MPhotonData*)Next()))
    {
        m.SetX(ph->GetPosY()*10);  // north
        m.SetY(ph->GetPosX()*10);  // east
        m.Paint();
    }
}
