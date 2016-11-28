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
!   Author(s): Thomas Bretz  12/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHCerPhotEvt
//
/////////////////////////////////////////////////////////////////////////////
#include "MHCerPhotEvt.h"

#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MCamEvent.h"
#include "MCamDisplay.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

ClassImp(MHCerPhotEvt);

using namespace std;

// --------------------------------------------------------------------------
//
// Initialize the name and title of the task.
// Resets the sum histogram
//
MHCerPhotEvt::MHCerPhotEvt(const char *name, const char *title)
    : fSum(NULL), fEvt(NULL)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHCerPhotEvt";
    fTitle = title ? title : "Average of MCerPhotEvts";
}

// --------------------------------------------------------------------------
//
// Delete the corresponding camera display if available
//
MHCerPhotEvt::~MHCerPhotEvt()
{
    if (fSum)
        delete fSum;
}

// --------------------------------------------------------------------------
//
// Get the event (MCerPhotEvt) the histogram might be filled with. If
// it is not given, it is assumed, that it is filled with the argument
// of the Fill function.
// Looks for the camera geometry MGeomCam and resets the sum histogram.
//
Bool_t MHCerPhotEvt::SetupFill(const MParList *plist)
{
    fEvt = (MCamEvent*)plist->FindObject(fNameEvt, "MCamEvent");
    if (!fEvt)
        *fLog << warn << GetDescriptor() << ": No MCerPhotEvt available..." << endl;

    MGeomCam *cam = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!cam)
        *fLog << warn << GetDescriptor() << ": No MGeomCam found." << endl;

    if (fSum)
        delete (fSum);
    fSum = new MCamDisplay(*cam, fNameEvt+";avg", fNameEvt+" Avarage");

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MCerPhotEvt-Container.
//
Bool_t MHCerPhotEvt::Fill(const MParContainer *par, const Stat_t w)
{
    const MCamEvent *evt = par ? dynamic_cast<const MCamEvent*>(par) : fEvt;
    if (!evt)
    {
        *fLog << err << dbginf << "No MCerPhotEvt found..." << endl;
        return kFALSE;
    }
    fSum->AddCamContent(*evt);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Scale the sum container with the number of entries
//
Bool_t MHCerPhotEvt::Finalize()
{
    if (fSum->GetEntries()>0)
        fSum->Scale(1./fSum->GetEntries());
    return kTRUE;
}
/*
// --------------------------------------------------------------------------
//
// Draw the present 'fill status'
//
void MHCerPhotEvt::Draw(Option_t *)
{
    if (!fSum)
    {
        *fLog << warn << "WARNING - Cannot draw " << GetDescriptor() << ": No Camera Geometry available." << endl;
        return;
    }

    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this, 750, 600);
    pad->SetBorderMode(0);
    //pad->Divide(1,1);
    //gPad->SetBorderMode(0);

    AppendPad("");
}

// --------------------------------------------------------------------------
//
// If a camera display is not yet assigned, assign a new one.
//
void MHCerPhotEvt::Paint(Option_t *option)
{
    if (!fSum)
        return;

    fSum->Paint();
}
*/

TH1 *MHCerPhotEvt::GetHistByName(const TString name)
{
    return fSum;
}
