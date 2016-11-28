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
!   Author(s): Thomas Bretz, 3/2004 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2012
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHSectorVsTime
//
// Display the mean and its variance vs time of the whole camera or a
// single sector
//
// 1) If no other title is given the rule for the y-axis is used.
// 2) If the MH3 has a non-default title (MH3::SetTitle called)
//    this title is set as the histogram title. It can be used to overwrite
//    the axis titles. For more information see TH1::SetTitle, eg.
//       SetTitle("MyGraph;;Counts");
//    The title for the x-axis is ignored and set automatically (MAKE SURE
//    YOU HAVE TWO SEMICOLON!)
//
// Example:
// --------
//     // Initialize histogram
//     MHSectorVsTime hist1;
//     hist1.SetNameTime("MTimeCurrents");
//     hist1.SetTitle("Title for your Graph;;Q [phe]");
//
//     // Define sectors you want to display the mean from
//     TArrayI s0(3);
//     s0[0] = 6;
//     s0[1] = 1;
//     s0[2] = 2;
// 
//     // Define area index [0=inner, 1=outer]
//     TArrayI inner(1);
//     inner[0] = 0;
// 
//     // Don't call this if you want to have all sectors
//     hist1.SetSectors(s0);
// 
//     // Don't call this if you want to have all area indices
//     hist1.SetAreaIndex(inner);
// 
//     // Task to fill the histogram
//     MFillH fill1(&hist1, "MCameraDC");
//
//
// Class Version 2:
// ----------------
//  + Double_t fMinimum;      // User defined minimum
//  + Double_t fMaximum;      // User defined maximum
//
// Class Version 3:
// ----------------
//  + Bool_t fUseMedian;     
//
//
/////////////////////////////////////////////////////////////////////////////
#include "MHSectorVsTime.h"

#include <TCanvas.h>
#include <TGraphErrors.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MCamEvent.h"

#include "MGeomCam.h"

#include "MRawEvtHeader.h"
#include "MTime.h"

ClassImp(MHSectorVsTime);

using namespace std;

const TString MHSectorVsTime::gsDefName  = "MHSectorVsTime";
const TString MHSectorVsTime::gsDefTitle = "Graph of sector mean vs. time";

// --------------------------------------------------------------------------
//
// Initialize the name and title of the task. If fErrType>=0 the variance is
// taken into account.
//
MHSectorVsTime::MHSectorVsTime(const char *name, const char *title)
    : fGraph(0), fEvt(NULL), fMinimum(-1111), fMaximum(-1111), fUseMedian(kFALSE),
    fType(0), fTypeErr(-1)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
// Delete the fGraph
//
MHSectorVsTime::~MHSectorVsTime()
{
    if (fGraph)
        delete fGraph;
}

// --------------------------------------------------------------------------
//
// Set the name of the TGraph and the MHVsTime container
//
void MHSectorVsTime::SetName(const char *name)
{
    if (fGraph)
        fGraph->SetName(name);
    MParContainer::SetName(name);
}

// --------------------------------------------------------------------------
//
// Set the title of the TGraph and the MHVsTime container
//
void MHSectorVsTime::SetTitle(const char *title)
{
    if (fGraph)
        fGraph->SetTitle(title);
    MParContainer::SetTitle(title);
}


// --------------------------------------------------------------------------
//
// Get the event (MPixVsTime) the histogram might be filled with. If
// it is not given, it is assumed, that it is filled with the argument
// of the Fill function.
// Looks for the camera geometry MGeomCam and resets the sum histogram.
// Create and/or initialize fGraph
//
Bool_t MHSectorVsTime::SetupFill(const MParList *plist)
{
    fEvt = dynamic_cast<MCamEvent*>(plist->FindObject(fNameEvt, "MCamEvent"));
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

    fHCamera.SetGeometry(*fCam);

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

    if (fGraph)
        delete fGraph;

    fGraph = fTypeErr<0 ? new TGraph : new TGraphErrors;
    fGraph->SetName(fEvt ? dynamic_cast<TObject*>(fEvt)->GetName() : "MCamEvent");
    fGraph->SetTitle(fTitle==gsDefTitle?"Camera":fTitle.Data());
    fGraph->SetMarkerStyle(kFullDotMedium);

    fMin =  FLT_MAX;
    fMax = -FLT_MAX;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MCamEvent
//
Int_t MHSectorVsTime::Fill(const MParContainer *par, const Stat_t w)
{
    const MCamEvent *evt = par ? dynamic_cast<const MCamEvent*>(par) : fEvt;
    if (!evt)
    {
        *fLog << err << dbginf << "No MCamEvent found..." << endl;
        return kERROR;
    }

    Double_t t = 0;
    if (!fNameTime.IsNull())
        t = fTime->GetAxisTime();
    else
        t = fHeader ? fHeader->GetDAQEvtNumber() : fGraph->GetN();


    fHCamera.SetCamContent(*evt, fType);

    const Double_t val0 = fUseMedian ?
        fHCamera.GetMedianSectors(fSectors, fAreaIndex) :
        fHCamera.GetMeanSectors(fSectors, fAreaIndex);

    if (!TMath::Finite(val0))
        return kTRUE;

    fGraph->SetPoint(fGraph->GetN(), t, val0);

    if (fTypeErr>=0)
    {
        const Double_t rms0 = fUseMedian ?
            fHCamera.GetDevSectors(fSectors, fAreaIndex) :

            fHCamera.GetRmsSectors(fSectors, fAreaIndex);
        if (!TMath::Finite(rms0))
            return kTRUE;

        ((TGraphErrors*)fGraph)->SetPointError(fGraph->GetN()-1, 0, rms0);
    }

    fMin = TMath::Min(fMin, val0);
    fMax = TMath::Max(fMax, val0);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set Minimum and Maximum;
//
Bool_t MHSectorVsTime::Finalize()
{
    const Double_t add = (fMax-fMin)*0.15;

    if (fMinimum==-1111)
        fGraph->SetMinimum(fMin-add);
    if (fMaximum==-1111)
        fGraph->SetMaximum(fMax+add);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Return fHistogram from TGraph
//
TH1 *MHSectorVsTime::GetHistByName(const TString name) const
{
    return fGraph->GetHistogram();
}

// --------------------------------------------------------------------------
//
// This displays the TGraph like you expect it to be (eg. time on the axis)
// It should also make sure, that the displayed time always is UTC and
// not local time...
//
void MHSectorVsTime::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(fGraph);
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
        axe->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
        axe->SetTimeDisplay(1);
        axe->SetLabelSize(0.033);
        h->GetYaxis()->SetTitleOffset(1.15);
    }

    // If this is set to early the plot remains empty in root 5.12/00
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

    fGraph->Draw(str.Data());
}

void MHSectorVsTime::RecursiveRemove(TObject *obj)
{
    if (obj==fGraph)
        fGraph = 0;
}
