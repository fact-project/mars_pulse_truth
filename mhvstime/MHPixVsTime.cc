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
!   Author(s): Thomas Bretz, 12/2002 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2012
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHPixVsTime
//
// Display the pixel content versus time or event number
//
// Axis titles
// ===========
//
// 1) If no other title is given the rule for the y-axis is used.
// 2) If the MH3 has a non-default title (MH3::SetTitle called)
//    this title is set as the histogram title. It can be used to overwrite
//    the axis titles. For more information see TH1::SetTitle, eg.
//       SetTitle("MyGraph;;Counts");
//    The title for the x-axis is ignored and set automatically (MAKE SURE
//    YOU HAVE TWO SEMICOLON!)
//
//
// Class Version 2:
// ----------------
//  + fMaximum
//  + fMinumum
//
/////////////////////////////////////////////////////////////////////////////
#include "MHPixVsTime.h"

#include <TH1.h>
#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MCamEvent.h"

#include "MGeomCam.h"

#include "MRawEvtHeader.h"
#include "MTime.h"

ClassImp(MHPixVsTime);

using namespace std;

const TString MHPixVsTime::gsDefName  = "MHPixVsTime";
const TString MHPixVsTime::gsDefTitle = "Graph of pixel content vs. time";

// --------------------------------------------------------------------------
//
// Initialize the name and title of the task.
//
MHPixVsTime::MHPixVsTime(Int_t idx, const char *name, const char *title)
    : fIndex(idx), fEvt(NULL), fType(0), fTypeErr(-1), fMinimum(-1111), fMaximum(-1111)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    TString t("Pixel Index #");
    t += idx;
    t += " vs Time";

    fGraph = new TGraphErrors;
    fGraph->SetName("MCamEvent");
    fGraph->SetTitle(t);
}

// --------------------------------------------------------------------------
//
// Delete the corresponding camera display if available
//
MHPixVsTime::~MHPixVsTime()
{
    if (fGraph)
        delete fGraph;
}

// --------------------------------------------------------------------------
//
// Set the name of the TGraph and the MHPixVsTime container
//
void MHPixVsTime::SetName(const char *name)
{
    fGraph->SetName(name);
    MParContainer::SetName(name);
}

// --------------------------------------------------------------------------
//
// Set the title of the TGraph and the MHPixVsTime container
//
void MHPixVsTime::SetTitle(const char *title)
{
    fGraph->SetTitle(title);
    MParContainer::SetTitle(title);
}

// --------------------------------------------------------------------------
//
// Get the event (MPixVsTime) the histogram might be filled with. If
// it is not given, it is assumed, that it is filled with the argument
// of the Fill function.
// Looks for the camera geometry MGeomCam and resets the sum histogram.
//
Bool_t MHPixVsTime::SetupFill(const MParList *plist)
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

    fCam = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fCam)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    if (!fNameTime.IsNull())
    {
        fTime = (MTime*)plist->FindObject(fNameTime, "MTime");
        if (!fTime)
        {
            *fLog << err << fNameTime << " [MTime] not found... abort." << endl;
            return kFALSE;
        }
    }
    else
    {
        fHeader = (MRawEvtHeader*)plist->FindObject("MRawEvtHeader");
        if (!fHeader)
            *fLog << warn << "MRawEvtHeader not found... using counter." << endl;
    }

    if (fTitle!=gsDefTitle)
        fGraph->SetTitle(fTitle);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MPixVsTime-Container.
//
Int_t MHPixVsTime::Fill(const MParContainer *par, const Stat_t w)
{
    const MCamEvent *evt = par ? dynamic_cast<const MCamEvent*>(par) : fEvt;
    if (!evt)
    {
        *fLog << err << dbginf << "No MCamEvent found..." << endl;
        return kERROR;
    }

    Double_t val = 0;
    Double_t rms = 0;
    evt->GetPixelContent(val, fIndex, *fCam, fType);
    if (!TMath::Finite(val))
        return kTRUE;  // Use kCONTINUE with extreme care!

    Double_t t = 0;
    if (!fNameTime.IsNull())
        t = fTime->GetAxisTime();
    else
        t = fHeader ? fHeader->GetDAQEvtNumber() : fGraph->GetN();

    fGraph->SetPoint(fGraph->GetN(), t, val);

    if (fTypeErr>=0)
    {
        evt->GetPixelContent(rms, fIndex, *fCam, fTypeErr);
        if (!TMath::Finite(rms))
            return kTRUE;  // Use kCONTINUE with extreme care!
    }

    fGraph->SetPointError(fGraph->GetN()-1, 0, rms);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Return histogram of TGraph
//
TH1 *MHPixVsTime::GetHistByName(const TString name) const
{
    return fGraph->GetHistogram();
}

void MHPixVsTime::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);
    pad->SetGrid();

    AppendPad("");

    // This is not done automatically anymore since root 5.12/00
    // and it is necessary to force a proper update of the axis.
    TH1 *h = fGraph->GetHistogram();

    h->SetXTitle("Time");

    if (!fNameTime.IsNull())
    {
        TAxis *axe = h->GetXaxis();
        axe->SetTimeFormat("%H:%M:%S %F1995-01-01 00:00:00 GMT");
        axe->SetTimeDisplay(1);
        axe->SetLabelSize(0.033);
    }

    if (fMinimum!=-1111)
        fGraph->SetMinimum(fMinimum);
    if (fMaximum!=-1111)
        fGraph->SetMaximum(fMaximum);

    TString str(opt);
    if (!str.Contains("A"))
        str += "A";
    if (!str.Contains("P"))
        str += "P";

    if (str.Contains("same", TString::kIgnoreCase))
    {
        str.ReplaceAll("same", "");
        str.ReplaceAll("A", "");
    }

    fGraph->Draw(str);
}
