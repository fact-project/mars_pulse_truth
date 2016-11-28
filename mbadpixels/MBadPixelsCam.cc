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
!   Author(s): Thomas Bretz 1/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Markus Gaug  3/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MBadPixelsCam                           
//
//
// Storage container to store bad pixel of the camera...
//
/////////////////////////////////////////////////////////////////////////////
#include "MBadPixelsCam.h"

#include <iostream>

#include <TArrayC.h>
#include <TClonesArray.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MBadPixelsPix.h"

#include "MGeomCam.h"
#include "MGeom.h"

ClassImp(MBadPixelsCam);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MBadPixelsCam::MBadPixelsCam(const char *name, const char *title)
{
    fName  = name  ? name  : "MBadPixelsCam";
    fTitle = title ? title : "Storage container to store bad pixel information";

    fArray = new TClonesArray("MBadPixelsPix", 1);
}

MBadPixelsCam::MBadPixelsCam(const MBadPixelsCam &cam)
{
    fName  = "MBadPixelsCam";
    fTitle = "Storage container to store bad pixel information";

    fArray = new TClonesArray("MBadPixelsPix", 1);
    cam.Copy(*this);
}

// --------------------------------------------------------------------------
//
// Delete the array conatining the bad pixel information
//
MBadPixelsCam::~MBadPixelsCam()
{
    delete fArray;
}

// --------------------------------------------------------------------------
//
// Set the size of the camera
//
void MBadPixelsCam::InitSize(const UInt_t i)
{
    fArray->ExpandCreate(i);
}

// --------------------------------------------------------------------------
//
// Get the size of the MBadPixelsCam
//
Int_t MBadPixelsCam::GetSize() const
{
    return fArray->GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Copy 'constructor'
//
void MBadPixelsCam::Copy(TObject &object) const
{
    MBadPixelsCam &cam = (MBadPixelsCam&)object;

    const Int_t n = GetSize();

    if (n==0)
        return;

    cam.InitSize(n);
    for (int i=0; i<n; i++)
        (*this)[i].Copy(cam[i]);
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel number)
//
MBadPixelsPix &MBadPixelsCam::operator[](Int_t i)
{
    return *static_cast<MBadPixelsPix*>(fArray->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Get i-th pixel (pixel number)
//
const MBadPixelsPix &MBadPixelsCam::operator[](Int_t i) const
{
    return *static_cast<MBadPixelsPix*>(fArray->UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
// Merge MBadPixelsCam cam into this, see also MBadPixelsPix::Merge
//
void MBadPixelsCam::Merge(const MBadPixelsCam &cam)
{
    const Int_t n = cam.GetSize();
    if (n==0)
    {
        *fLog << inf << "MBadPixelsCam::Merge: Container empty." << endl;
        return;
    }

    if (GetSize()==0)
        InitSize(n);

    if (GetSize()<n)
    {
        *fLog << warn << "MBadPixelsCam::Merge: Size mismatch (" << n << "," << GetSize() << ")... ignored." << endl;
        return;
    }

    for (int i=0; i<n; i++)
        (*this)[i].Merge(cam[i]);
}

// --------------------------------------------------------------------------
//
// Clear the contents of all bad pixels (set=0 means Ok)
//
void MBadPixelsCam::Clear(Option_t *o)
{
    fArray->R__FOR_EACH(TObject, Clear)(o);
}

// --------------------------------------------------------------------------
//
// Reset event depending bits
//
void MBadPixelsCam::Reset()
{
    fArray->R__FOR_EACH(MParContainer, Reset)();
}

// --------------------------------------------------------------------------
//
// Calculate the number of pixels without the given type-flags.
//
// The second argument aidx is the area index (see MGeomCam, MGeom)
// The default (or any value less than 0) means: all
//
// Returns -1 if the geometry doesn't match.
//
Short_t MBadPixelsCam::GetNumSuitable(MBadPixelsPix::UnsuitableType_t type, const MGeomCam *geom, Int_t aidx) const
{
    const UInt_t n = GetSize();

    if (aidx>=0 && (!geom || geom->GetNumPixels()!=n))
    {
        *fLog << err << GetDescriptor() << "ERROR - Geometry (" << geom->ClassName() << ") size mismatch!" << endl;
        return -1;
    }

    Short_t rc = 0;
    for (UInt_t i=0; i<n; i++)
    {
        if (aidx>=0 && geom && (*geom)[i].GetAidx()!=aidx)
            continue;

        if (!(*this)[i].IsUnsuitable(type))
            rc++;
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// Calculate the number of pixels with the given type-flags.
//
// The second argument aidx is the area index (see MGeomCam, MGeom)
// The default (or any value less than 0) means: all
//
// Returns -1 if the geometry doesn't match.
//
Short_t MBadPixelsCam::GetNumUnsuitable(MBadPixelsPix::UnsuitableType_t type, const MGeomCam *geom, Int_t aidx) const
{
    const UInt_t n = GetSize();

    if (aidx>=0 && geom && geom->GetNumPixels()!=n)
    {
        *fLog << err << GetDescriptor() << "ERROR - Geometry (" << geom->ClassName() << ") size mismatch!" << endl;
        return -1;
    }

    Short_t rc = 0;
    for (UInt_t i=0; i<n; i++)
    {
        if (aidx>=0 && geom && (*geom)[i].GetAidx()!=aidx)
            continue;

        if ((*this)[i].IsUnsuitable(type))
            rc++;
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// Count the number of unsuitable pixels.
//
Short_t MBadPixelsCam::GetNumUnsuitable() const
{
    const UInt_t n = GetSize();

    Short_t rc = 0;
    for (UInt_t i=0; i<n; i++)
        if ((*this)[i].IsUnsuitable())
            rc++;

    return rc;
}

// --------------------------------------------------------------------------
//
// Counts the number of neighbors matching NOT UnsuitableType type
//
Short_t MBadPixelsCam::GetNumSuitableNeighbors(MBadPixelsPix::UnsuitableType_t type, const MGeom &pix) const
{
    const Int_t n2 = pix.GetNumNeighbors();

    Int_t cnt=0;
    for (int j=0; j<n2; j++)
    {
        const Int_t id2 = pix.GetNeighbor(j);
        if (!(*this)[id2].IsUnsuitable(type))
            cnt++;
    }

    return cnt;
}

// --------------------------------------------------------------------------
//
// Calculate the number of pixels which are - under no circumstances -
// interpolatable, called isolated. This means that a pixel (its own status
// doesn't matter) has less than two reliable neighbor pixels.
//
// The second argument aidx is the area index (see MGeomCam, MGeom)
// The default (or any value less than 0) means: all
//
// Returns -1 if the geometry doesn't match.
//
Short_t MBadPixelsCam::GetNumIsolated(MBadPixelsPix::UnsuitableType_t type, const MGeomCam &geom, Int_t aidx) const
{
    const Int_t n = geom.GetNumPixels();

    if (n!=GetSize())
    {
        *fLog << err << GetDescriptor() << "ERROR - Geometry (" << geom.ClassName() << ") size mismatch!" << endl;
        return -1;
    }

    Short_t rc = 0;
    for (int i=0; i<n; i++)
    {
        const MGeom &pix = geom[i];
        if (aidx>=0 && pix.GetAidx()!=aidx)
            continue;

        if (GetNumSuitableNeighbors(type, pix)<2)
            rc++;
    }
    return rc;
}

// --------------------------------------------------------------------------
//
// This is a helper function which calculates the size of a single cluster
// by iterative calling.
//
// If a pixel matches the criterias the counter is increased by 1 and
// the function is called for all its neighbors. If
//
// The second argument aidx is the area index (see MGeomCam, MGeom)
// The default (or any value less than 0) means: all
//
// Returns -1 if the geometry doesn't match.
//
Short_t MBadPixelsCam::GetNumMaxCluster(MBadPixelsPix::UnsuitableType_t type, TObjArray &list, Int_t idx, Int_t aidx) const
{
    const MGeom *pix = (MGeom*)list[idx];
    if (!pix)
        return 0;

    // Check whether more than one neighbor contains useful information,
    // which mean it can later be interpolated
    if (GetNumSuitableNeighbors(type, *pix)>1)
        return 0;

    // If the requested area-index is valid check whether it is the requested one
    if (aidx>=0 && pix->GetAidx()!=aidx)
        return 1;

    // Remove the pixel from the list of pixels to be checked
    list.RemoveAt(idx);

    // Do the same for the neighbor pixels recursively and count the 1-results
    Short_t cnt = 1;
    const Int_t n = pix->GetNumNeighbors();
    for (int i=0; i<n; i++)
        cnt += GetNumMaxCluster(type, list, pix->GetNeighbor(i), aidx);

    // return the number of neighbor pixels/clusters which have unsuitable-type type
    return cnt;
}

// --------------------------------------------------------------------------
//
// Returns the size of the biggest cluster with the given UnsuitableType
// type and the given area index.
//
// The second argument aidx is the area index (see MGeomCam, MGeom)
// The default (or any value less than 0) means: all
//
// Returns -1 if the geometry doesn't match.
//
Short_t MBadPixelsCam::GetNumMaxCluster(MBadPixelsPix::UnsuitableType_t type, const MGeomCam &geom, Int_t aidx) const
{
    const Int_t n = geom.GetNumPixels();

    if (n!=GetSize())
    {
        *fLog << err << GetDescriptor() << "ERROR - Geometry (" << geom.ClassName() << ") size mismatch!" << endl;
        return -1;
    }

    TObjArray list(n);
    for (int i=0; i<n; i++)
        list.AddAt(&geom[i], i);

    Short_t max = 0;
    for (int i=0; i<n; i++)
        max = TMath::Max(GetNumMaxCluster(type, list, i, aidx), max);

    return max;
}

// --------------------------------------------------------------------------
//
// Print the contents of all bad pixels
//
void MBadPixelsCam::Print(Option_t *o) const
{
  *fLog << all << GetDescriptor() << ":" << endl;
  *fLog << "Pixels without problems:" << endl;
    
  Int_t count = 0;

  for (Int_t i=0; i<GetSize(); i++)
    {
      if (!(*this)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        {
          *fLog << i << " ";
          count ++;
        }
    }
  *fLog << count << " normal pixels" << endl;
  *fLog << endl;

  *fLog << "Pixels unsuited for the whole run:" << endl;

  count = 0;
  for (Int_t i=0; i<GetSize(); i++)
    {
      if ((*this)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        {
          *fLog << i << " ";
          count ++;
        }
    }

  *fLog << count << " unsuited pixels per run :-(" << endl;
  *fLog << endl;

  *fLog << "Pixels unsuited for this event:" << endl;

  count = 0;
  for (Int_t i=0; i<GetSize(); i++)
    {
      if ((*this)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableEvt))
        {
          *fLog << i << " ";
          count ++;
        }
    }

  *fLog << count << " unsuited pixels per event :-(" << endl;
  *fLog << endl;

  count = 0;

  *fLog << all << "Pixels unreliable for the whole run:" << endl;

  for (Int_t i=0; i<GetSize(); i++)
    {
      if ((*this)[i].IsUnsuitable(MBadPixelsPix::kUnreliableRun))
        {
          *fLog << i << " ";
          count ++;
        }
    }

  *fLog << count << " unreliable pixels :-(" << endl;
  *fLog << endl;
  *fLog << endl;
  *fLog << all << "Unsuited pixels statistics:" << endl;
  *fLog << endl;

  PrintBadPixels(MBadPixelsPix::kPreviouslyExcluded,"Previously excluded");
  PrintBadPixels(MBadPixelsPix::kChargeIsPedestal,"Signal smaller 3 Pedestal RMS");
  PrintBadPixels(MBadPixelsPix::kChargeRelErrNotValid,"Signal Rel. error too large");
  PrintBadPixels(MBadPixelsPix::kLoGainSaturation,"Low Gain Saturation");
  PrintBadPixels(MBadPixelsPix::kMeanTimeInFirstBin,"Mean Arr. Time In First Extraction Bin");
  PrintBadPixels(MBadPixelsPix::kMeanTimeInLast2Bins,"Mean Arr. Time In Last 2 Extraction Bins");
  PrintBadPixels(MBadPixelsPix::kDeviatingNumPhes,"Deviating Number of Photo-electrons");
  PrintBadPixels(MBadPixelsPix::kDeviatingNumPhots,"Deviating Number of Photons");
  PrintBadPixels(MBadPixelsPix::kHiGainOverFlow,"High-Gain Histogram Overflow");
  PrintBadPixels(MBadPixelsPix::kLoGainOverFlow,"Low-Gain Histogram Overflow");
  PrintBadPixels(MBadPixelsPix::kDeadPedestalRms,"Presumably dead from Ped. Rms");
  PrintBadPixels(MBadPixelsPix::kDeviatingAbsTimeResolution,"Deviating abs. time resolution");
  PrintBadPixels(MBadPixelsPix::kDeviatingRelTimeResolution,"Deviating rel. time resolution");
  //  PrintBadPixels(MBadPixelsPix::kFluctuatingArrivalTimes,"Fluctuating Pulse Arrival Times");

  *fLog << endl;
  *fLog << all << "Unreliable pixels statistics:" << endl;
  *fLog << endl;

  PrintBadPixels(MBadPixelsPix::kChargeSigmaNotValid,"Signal Sigma smaller Pedestal RMS");
  PrintBadPixels(MBadPixelsPix::kHiGainNotFitted    ,"High Gain Signals could not be fitted");
  PrintBadPixels(MBadPixelsPix::kLoGainNotFitted    ,"Low  Gain Signals could not be fitted");
  PrintBadPixels(MBadPixelsPix::kRelTimeNotFitted   ,"Relative Arr. Times could not be fitted");
  PrintBadPixels(MBadPixelsPix::kHiGainOscillating  ,"High Gain Signals Oscillation");
  PrintBadPixels(MBadPixelsPix::kLoGainOscillating  ,"Low  Gain Signals Oscillation");
  PrintBadPixels(MBadPixelsPix::kRelTimeOscillating ,"Relative Arr. Times Oscillation");
  PrintBadPixels(MBadPixelsPix::kDeviatingFFactor   ,"Deviating global F-Factor");
}

TArrayC MBadPixelsCam::GetUnsuitable(MBadPixelsPix::UnsuitableType_t typ) const
{
    TArrayC rc(GetSize());

    for (Int_t i=0; i<rc.GetSize(); i++)
        rc[i] = (*this)[i].IsUnsuitable(typ) ? 0 : 1;

    return rc;
}

TArrayC MBadPixelsCam::GetUncalibrated(MBadPixelsPix::UncalibratedType_t typ) const
{
    TArrayC rc(GetSize());

    for (Int_t i=0; i<rc.GetSize(); i++)
        rc[i] = (*this)[i].IsUncalibrated(typ) ? 0 : 1;

    return rc;
}

void MBadPixelsCam::PrintBadPixels( MBadPixelsPix::UncalibratedType_t typ, const char *text) const 
{
  *fLog << "Pixels with " << text << ": " << endl;
  UInt_t count = 0;

  for (Int_t i=0; i<GetSize(); i++)
    {
      if ((*this)[i].IsUncalibrated(typ))
        {
          *fLog << i << " ";
          count++;
        }
    }
  
  *fLog << Form("%3i",count) << " pixels in total " << endl;
}

// --------------------------------------------------------------------------
//
// Read from an ascii file of the format:
//    pixel1
//    pixel2
//    pixel3
//    pixel4
// while pixel1,2,3,4 are the pixel indices used in the software.
//
void MBadPixelsCam::AsciiRead(istream &fin)
{
    TString str;

    while (1)
    {
        str.ReadLine(fin);
        if (!fin)
            break;

        if (str[0]=='#')
            continue;

        const Int_t idx = str.Atoi();

        if (idx>=GetSize())
            InitSize(idx+1);

        (*this)[idx].SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
        (*this)[idx].SetUncalibrated(MBadPixelsPix::kPreviouslyExcluded);
    }
}

// --------------------------------------------------------------------------
//
// Write the information into an ascii file.
//
Bool_t MBadPixelsCam::AsciiWrite(ostream &fout) const
{
    for (int i=0; i<GetSize(); i++)
        if ((*this)[i].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
            fout << i << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The types are the following:
// 0: MBadPixelsPix::GetInfo()[0]
// 1: MBadPixelsPix::IsUnsuitable(MBadPixelsPix::kUnsuitableRun)
// 2: MBadPixelsPix::IsUnsuitable(MBadPixelsPix::kUnsuitableEvt)
// 3: MBadPixelsPix::IsUnsuitable(MBadPixelsPix::kUnreliableRun)
// 4: MBadPixelsPix::IsHiGainBad()
// 5: MBadPixelsPix::IsLoGainBad()
// 6: MBadPixelsPix::GetUnsuitableCalLevel()
// 7: MBadPixelsPix::GetUnreliableCalLevel()
// 8: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kHiGainNotFitted)
// 9: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kLoGainNotFitted)
// 10: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kHiGainOscillating)
// 11: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kLoGainOscillating)
// 12: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kLoGainSaturation )
// 13: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kChargeIsPedestal )
// 14: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kChargeErrNotValid)
// 15: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kChargeRelErrNotValid)
// 16: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kChargeSigmaNotValid )
// 17: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kMeanTimeInFirstBin  )
// 18: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kMeanTimeInLast2Bins )
// 19: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kDeviatingNumPhes    )
// 20: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kRelTimeNotFitted    )
// 21: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kRelTimeOscillating  )
// 22: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kDeviatingNumPhots   )
// 23: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kHiGainOverFlow      )
// 24: MBadPixelsPix::IsUncalibrated(MBadPixelsPix::kLoGainOverFlow      )
// 102: MBadPixelsPix::IsUnsuitable()
//
Bool_t MBadPixelsCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{

  if (idx >= GetSize())
    return kFALSE;

  switch (type)
    {
    case 0:
      return (*this)[idx].GetInfo()[0];
    case 1:
      if  (!(*this)[idx].IsUnsuitable(MBadPixelsPix::kUnsuitableRun))
        return kFALSE;
      val = 1;
      break;
    case 2:
      if  (!(*this)[idx].IsUnsuitable(MBadPixelsPix::kUnsuitableEvt))
        return kFALSE;
      val = 1;
      break;
    case 3:
      if  (!(*this)[idx].IsUnsuitable(MBadPixelsPix::kUnreliableRun))
          return kFALSE;
      val = 1;
      break;
    case 4:
      if  (!(*this)[idx].IsHiGainBad())
        return kFALSE;
      val = 1;
      break;
    case 5:
      if  (!(*this)[idx].IsLoGainBad())
        return kFALSE;
      val = 1;
      break;
    case 6:
      val = (*this)[idx].GetUnsuitableCalLevel();
      return val>0;
    case 7:
      val = (*this)[idx].GetUnreliableCalLevel();
      return val>0;
    case 8:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kHiGainNotFitted))
        return kFALSE;
      val = 1;
      break;
    case 9:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kLoGainNotFitted))
        return kFALSE;
      val = 1;
      break;
    case 10:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kHiGainOscillating))
        return kFALSE;
      val = 1;
      break;
    case 11:
      if (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kLoGainOscillating))
        return kFALSE;
      val = 1;
      break;
    case 12:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kLoGainSaturation ))
        return kFALSE;
      val = 1;
      break;
    case 13:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kChargeIsPedestal ))
        return kFALSE;
      val = 1;
      break;
    case 14:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kChargeErrNotValid))
        return kFALSE;
      val = 1;
      break;
    case 15:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kChargeRelErrNotValid))
        return kFALSE;
      val = 1;
      break;
    case 16:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kChargeSigmaNotValid ))
        return kFALSE;
      val = 1;
      break;
    case 17:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kMeanTimeInFirstBin  ))
        return kFALSE;
      val = 1;
      break;
    case 18:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kMeanTimeInLast2Bins ))
        return kFALSE;
      val = 1;
      break;
    case 19:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kDeviatingNumPhes ))
        return kFALSE;
      val = 1;
      break;
    case 20:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kRelTimeNotFitted ))
        return kFALSE;
      val = 1;
      break;
    case 21:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kRelTimeOscillating))
        return kFALSE;
      val = 1;
      break;
    case 22:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kDeviatingNumPhots))
        return kFALSE;
      val = 1;
      break;
    case 23:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kHiGainOverFlow    ))
        return kFALSE;
      val = 1;
      break;
    case 24:
      if  (!(*this)[idx].IsUncalibrated(MBadPixelsPix::kLoGainOverFlow   ))
        return kFALSE;
      val = 1;
      break;
    case 102:
      if  (!(*this)[idx].IsUnsuitable())
          return kFALSE;
      val = 1;
      break;
    default:
      return kFALSE;
    }
  
  return kTRUE;
}

void MBadPixelsCam::DrawPixelContent(Int_t num) const
{
    *fLog << warn << "MBadPixelsCam::DrawPixelContent - not available." << endl;
}
