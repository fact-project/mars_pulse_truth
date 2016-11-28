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
!   Author(s): Josep Flix 11/2002 <mailto:jflix@ifae.es>
!   Author(s): Thomas Bretz 11/2002 <mailto:tbretz@astro.uni-wuerburg.de>
!
!   Copyright: MAGIC Software Development, 2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MPedestalCalc
//
//  Task to calculate the pedestals from the event stream.
//
//  From the events two streams of pedestals are created like in the
//  following table
//  MRawEvtData:    1   2   3   4   5   6   7   8   9  10  11  12
//  MPedestalCam;1: ------1------   ------2------   ------3------...
//  MPedestalCam;2:         ------1------   ------2------  ------...
//
//  Input Containers:
//   MRawEvtData
//
//  Output Containers:
//   MPedestalCam;1
//   MPedestalCam;2
//
/////////////////////////////////////////////////////////////////////////////
#include "MPedestalCalc.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MTime.h"
#include "MHFadcCam.h"
#include "MRawEvtPixelIter.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

ClassImp(MPedestalCalc);

MPedestalCalc::MPedestalCalc(const char *name, const char *title)
    : fNumBranches(2), fTimeSlot(6), fHists(NULL)
{
    fName  = name  ? name  : "MPedestalCalc";
    fTitle = title ? title : "Task to calculate pedestals from pedestal runs raw data";

    AddToBranchList("fHiGainPixId");
    AddToBranchList("fLoGainPixId");
    AddToBranchList("fHiGainFadcSamples");
    AddToBranchList("fLoGainFadcSamples");
}

Int_t MPedestalCalc::PreProcess(MParList *pList)
{
    if (fHists)
    {
        *fLog << err << "ERROR - Previous PostProcess not called." << endl;
        return kFALSE;
    }

    fHists = new MHFadcCam[fNumBranches];
    fStart = new MTime    [fNumBranches];

    fRawEvt = (MRawEvtData*)pList->FindObject("MRawEvtData");
    if (!fRawEvt)
    {
        *fLog << dbginf << "MRawEvtData not found... aborting." << endl;
        return kFALSE;
    }

    fPedestals = (MPedestalCam*)pList->FindCreateObj("MPedestalCam");
    if (!fPedestals)
        return kFALSE;

    fPedTime = (MTime*)pList->FindCreateObj("MTime", "MPedestalTime");
    if (!fPedTime)
        return kFALSE;

    //
    // Fixme: FindCreateObj --> FindObject
    //
    fEvtTime = (MTime*)pList->FindCreateObj("MTime", "MRawEvtTime");
    if (!fEvtTime)
        return kFALSE;

    for (int i=0; i<fNumBranches; i++)
        fStart[i].SetTime(fTimeSlot*10/fNumBranches*i, 0);

    fPedTime->SetDuration(fTimeSlot);
    fEvtTime->SetTime(0, 0);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Checks whether the current event exceed the validity range of a pedestal
//  in one of the streams. If this is the case the data from the histogram
//  is dumped as pedestals into the corresponding containers and the
//  histograms are reset.
//
//  Then the current event is filled into the histograms.
//
Int_t MPedestalCalc::Process()
{
    //
    // Time when filling of stream a/b must be restarted
    //
    for (int i=0; i<fNumBranches; i++)
    {
        Check(i);
        Fill(i);
    }

    fEvtTime->SetTime(fEvtTime->GetTimeLo()+10, 0);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Fill the event in the histogram if the current time of the
//  event is _after_ the start time for filling
//
void MPedestalCalc::Fill(Int_t i)
{
    if (*fEvtTime < fStart[i])
        return;

    fHists[i].Fill(fRawEvt);
}

// --------------------------------------------------------------------------
//
//  If the current time of the event is equal or after the time
//  which is given by the start time plus the validity range
//  calculate the corresponding set of pedestals and reset the
//  histograms
//
void MPedestalCalc::Check(Int_t i)
{
    if (*fEvtTime-fStart[i] < fTimeSlot*10)
        return;

    Calc(i);

    fStart[i] = *fEvtTime;
    fHists[i].ResetHistograms();
}

// --------------------------------------------------------------------------
//
//  Deletes all dynamicly allocated arrays
//
Bool_t MPedestalCalc::PostProcess()
{
    delete fHists;
    delete fStart;

    fHists=NULL;

    return kTRUE;
}

/*
 #include "MRawEvtData.h"
 */
// --------------------------------------------------------------------------
//
//  Dumps the mean, rms and their errors from the filled histograms into
//  the Pedestal container. Sets the ReadyToSaveFlag of the MPedestalCam
//  container.
//
void MPedestalCalc::Calc(Int_t i) const
{
    // FIXME! Fit +- 3 sigma to avoid outliers...

    MRawEvtPixelIter pixel(fRawEvt);
    while (pixel.Next())
    {
        const UInt_t pixid = pixel.GetPixelId();

        const TH1 &h = *fHists[i].GetHistHi(pixid);

        const Int_t   entries = (Int_t)h.GetEntries();
        const Float_t meanhi  = h.GetMean();
        const Float_t rmshi   = h.GetRMS();

        const Float_t meanhierr = rmshi/sqrt(entries);
        const Float_t rmshierr  = rmshi/sqrt(entries*2);

        MPedestalPix &pix = (*fPedestals)[pixid];
        pix.SetPedestal(meanhi, rmshi);
        pix.SetPedestalRms(meanhierr, rmshierr);
    }

    *fPedTime = fStart[i];

    fPedTime->SetReadyToSave();
    fPedestals->SetReadyToSave();

    /*
     *fLog << i << "[" << fHists[i].GetHistHi(0)->GetEntries()/fRawEvt->GetNumHiGainSamples() << "]:  ";
     *fLog << fEvtTime->GetTimeLo() << ": Calc [";
     *fLog << fStart[i].GetTimeLo() << " < ";
     *fLog << fStart[i].GetTimeLo()+fTimeSlot*10 << "]" << endl;
     */
}
