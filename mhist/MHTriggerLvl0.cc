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
!   Author(s): Abelardo Moralejo, 06/2003 <mailto:moralejo@pd.infn.it>
!   Author(s): Thomas Bretz, 06/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHTriggerLvl0
//
// This is intended to be a sort of "level 0 trigger display". What it
// really does is to store the number of events of a data file in which
// each pixel has gone above a given threshold (fPixelThreshold) which
// is chosen when calling the constructor. Displaying a camera view with
// these values can help identify noisy pixels. See the macro pixfixrate.C
// to see an example of its use.
//
/////////////////////////////////////////////////////////////////////////////
#include "MHTriggerLvl0.h"

#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MCamEvent.h"
#include "MHCamera.h"

#include "MGeomCam.h"

ClassImp(MHTriggerLvl0);

using namespace std;

// --------------------------------------------------------------------------
//
// Initialize the name and title of the task.
// Resets the sum histogram
//
MHTriggerLvl0::MHTriggerLvl0(Double_t t, const char *name, const char *title)
    : fSum(NULL), fEvt(NULL), fType(0), fThreshold(t)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHTriggerLvl0";
    fTitle = title ? title : "Number of hits above threshold per pixel";
}

// --------------------------------------------------------------------------
//
// Delete the corresponding camera display if available
//
MHTriggerLvl0::~MHTriggerLvl0()
{
    if (fSum)
        delete fSum;
}

// --------------------------------------------------------------------------
//
// Get the event (MRawEvtData) the histogram might be filled with. If
// it is not given, it is assumed, that it is filled with the argument
// of the Fill function.
// Looks for the camera geometry MGeomCam and resets the sum histogram.
//
Bool_t MHTriggerLvl0::SetupFill(const MParList *plist)
{
    fEvt = (MCamEvent*)plist->FindObject(fNameEvt, "MCamEvent");
    if (!fEvt)
    {
        if (!fNameEvt.IsNull())
        {
            *fLog << err << GetDescriptor() << ": No " << fNameEvt <<" [MCamEvent] available..." << endl;
            return kFALSE;
        }
        *fLog << warn << GetDescriptor() << ": No MCamEvent available..." << endl;
    }

    MGeomCam *cam = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!cam)
    {
        *fLog << err << GetDescriptor() << ": No MGeomCam found... aborting." << endl;
        return kFALSE;
    }

    if (fSum)
        delete (fSum);

    const TString name = fNameEvt.IsNull() ? fName : fNameEvt;

    fSum = new MHCamera(*cam, name+";avg", fTitle);
    fSum->SetBit(MHCamera::kProfile);
    fSum->SetYTitle("% [1]");

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MCamEvent-Container.
//
Int_t MHTriggerLvl0::Fill(const MParContainer *par, const Stat_t w)
{
    const MCamEvent *evt = par ? dynamic_cast<const MCamEvent*>(par) : fEvt;
    if (!evt)
    {
        *fLog << err << dbginf << "Got no MCamEvent as argument of Fill()..." << endl;
        return kERROR;
    }

    fSum->CntCamContent(*evt, fThreshold, fType);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Scale by the number of events
//
Bool_t MHTriggerLvl0::Finalize()
{
    //    if (fSum->GetEntries()>0)
    //        fSum->Scale(100);
    return kTRUE;
}

void MHTriggerLvl0::PrintOutliers(Float_t s) const
{
    const Double_t mean = fSum->GetMean();
    const Double_t rms  = fSum->GetRMS();

    *fLog << all << underline << GetDescriptor() << ": Mean=" << mean << ", Rms=" << rms << endl;

    for (unsigned int i=0; i<fSum->GetNumPixels(); i++)
    {
        if (!fSum->IsUsed(i))
            continue;

        if ((*fSum)[i+1]>mean+s*rms)
            *fLog << "Contents of Pixel-Index #" << i << ": " << (*fSum)[i+1] << " > " << s << "*rms" << endl;
        // if ((*fSum)[i+1]==0)
        //     *fLog << "Contents of Pixel-Index #" << i << ": " << (*fSum)[i+1] << " == 0" << endl;
        // if ((*fSum)[i+1]<fSum->GetMean()-s*fSum->GetRMS())
        //     *fLog << "Contents of Pixel-Index #" << i << ": " << (*fSum)[i+1] << " < " << s << "*rms" << endl;
    }
}

TH1 *MHTriggerLvl0::GetHistByName(const TString name) const
{
    return fSum;
}

void MHTriggerLvl0::Draw(Option_t *)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    pad->Divide(1,2);

    pad->cd(1);
    /*
     gPad->SetBorderMode(0);
     gPad->Divide(1,1);
     gPad->cd(1);
     gPad->SetBorderMode(0);
     */
    fSum->Draw();

    pad->cd(2);
    gPad->SetBorderMode(0);
    fSum->Draw("EPhist");
}
