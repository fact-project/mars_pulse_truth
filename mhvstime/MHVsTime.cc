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
!   Author(s): Thomas Bretz, 11/2003 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2012
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHVsTime
//
// Use this class if you want to display any rule vs time (or event number)
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
// eg.
//       MHVsTime hist("MHillas.fAlpha");
//       MHVsTime hist("MPointintPos.GetAbsErr");
//       MHVsTime hist("MPointintPos.GetAbsErr*kRad2Deg");
//
// To set a maximum number of data-points (eg. to display the last 20min
// only) call SetMaxPts(200)
//
// SetMaxPts(-1) disables this feature.
//
//
// Class Version 2:
// ----------------
//  + MData      *fData;      // Object from which the data is filled
//  - MDataChain *fData;      // Object from which the data is filled
//  + MData      *fError;     // Object from which the error is filled
//  - MDataChain *fError;     // Object from which the error is filled
//
// Class Version 3:
// ----------------
//  + Double_t fMinimum;      // User defined minimum
//  + Double_t fMaximum;      // User defined maximum
//
/////////////////////////////////////////////////////////////////////////////
#include "MHVsTime.h"

#include <ctype.h>   // tolower
#include <fstream>

#include <TPad.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <TH1.h>
#include <TGraphErrors.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MTime.h"
#include "MParList.h"
#include "MDataPhrase.h"
#include "MRawEvtHeader.h"

ClassImp(MHVsTime);

using namespace std;

const TString MHVsTime::gsDefName  = "MHVsTime";
const TString MHVsTime::gsDefTitle = "Container for a graph vs time/evtnumber";

// --------------------------------------------------------------------------
//
// Default constructor. For more informations about a valid rule
// see MDataPhrase.
//
MHVsTime::MHVsTime(const char *rule, const char *error)
    : fGraph(NULL), fData(NULL), fError(NULL), fScale(1), fMaxPts(-1),
    fNumEvents(1), fMinimum(-1111), fMaximum(-1111), fUseEventNumber(0)
{
    fName  = gsDefName;
    fTitle = gsDefTitle;

    if (!rule)
        return;

    fData = new MDataPhrase(rule);

    if (error)
        fError = new MDataPhrase(error);

    fGraph = error ? new TGraphErrors : new TGraph;
    fGraph->SetMarkerStyle(kFullDotMedium);
}

// --------------------------------------------------------------------------
//
// Deletes the histogram
//
MHVsTime::~MHVsTime()
{
    if (fGraph)
        delete fGraph;

    if (fData)
        delete fData;

    if (fError)
        delete fError;
}

// --------------------------------------------------------------------------
//
// Return the data members used by the data chain to be used in
// MTask::AddBranchToList
//
TString MHVsTime::GetDataMember() const
{
    return fData ? fData->GetDataMember() : (TString)"";
}

// --------------------------------------------------------------------------
//
// PreProcess the MDataPhrase. Create a new TGraph. Delete an old one if
// already allocated.
//
Bool_t MHVsTime::SetupFill(const MParList *plist)
{
    if (!fGraph || !fData)
    {
        *fLog << err << "ERROR - MHVsTime cannot be used with its default constructor!" << endl;
        return kFALSE;
    }

    if (!fData->PreProcess(plist))
        return kFALSE;

    if (fError && !fError->PreProcess(plist))
        return kFALSE;

    TString title(fData ? GetRule() : (TString)"Graph");
    title += " vs ";
    title += fUseEventNumber ? "Event Number" : "Time";

    fGraph->SetNameTitle(fName, fTitle==gsDefTitle?title:fTitle);

    fMean = 0;
    fN    = 0;

    fMin =  FLT_MAX;
    fMax = -FLT_MAX;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set the name of the TGraph and the MHVsTime container
//
void MHVsTime::SetName(const char *name)
{
    fGraph->SetName(name);
    MH::SetName(name);
}

// --------------------------------------------------------------------------
//
// Set the title of the TGraph and the MHVsTime container
//
void MHVsTime::SetTitle(const char *title)
{
    fGraph->SetTitle(title);
    MH::SetTitle(title);
}

// --------------------------------------------------------------------------
//
// Set the next data point. If the graph exceeds fMaxPts remove the first
//
Int_t MHVsTime::Fill(const MParContainer *par, const Stat_t w)
{
    Double_t t = 0;
    if (fUseEventNumber)
    {
        const MRawEvtHeader *h = dynamic_cast<const MRawEvtHeader*>(par);
        t = h ? h->GetDAQEvtNumber() : fGraph->GetN();
    }
    else
    {
        const MTime *tm = dynamic_cast<const MTime*>(par);
        if (!tm)
        {
            *fLog << err << dbginf << "No MTime found..." << endl;
            return kERROR;
        }
        // If the time is not valid skip this entry
	if (!*tm)
            return kTRUE;

        // Do not fill events with equal time
        if (*tm==fLast || *tm==MTime())
            return kTRUE;

        fLast = *tm;

        t = tm->GetAxisTime();
    }

    const Double_t v = fData->GetValue();
    const Double_t e = fError ? fError->GetValue() : 0;

    //*fLog << all << "ADD " << v << " " << e << endl;

    fMean    += v;
    fMeanErr += e;
    fN++;

    if (fN==fNumEvents)
    {
        /*
        if ((fMaxPts>0 && fGraph->GetN()>fMaxPts) || fGraph->IsEditable())
        {
            fGraph->RemovePoint(0);
            fGraph->SetEditable(kFALSE);
        }*/

        const Double_t val = fMean/fN*fScale;

        fGraph->SetPoint(fGraph->GetN(), t, val);

        if (fError)
            static_cast<TGraphErrors*>(fGraph)->SetPointError(fGraph->GetN()-1, 0, fMeanErr/fN*fScale);

        fMin = TMath::Min(fMin, val);
        fMax = TMath::Max(fMax, val);

        fMean = 0;
        fMeanErr = 0;
        fN = 0;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set Minimum and Maximum;
Bool_t MHVsTime::Finalize()
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
// This displays the TGraph like you expect it to be (eg. time on the axis)
// It should also make sure, that the displayed time always is UTC and
// not local time...
//
void MHVsTime::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(fGraph);
    pad->SetBorderMode(0);
    pad->SetGrid();

    AppendPad("");

    // This is not done automatically anymore since root 5.12/00
    // and it is necessary to force a proper update of the axis.
    TH1 *h = fGraph->GetHistogram();

    if (h)
    {
        TAxis *axe = h->GetXaxis();
        // SetPoint deletes the histogram!
        if (fUseEventNumber)
            axe->SetTitle("Event Number");
        else
        {
            axe->SetTitle("Time");
            axe->SetLabelSize(0.033);
            axe->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
            axe->SetTimeDisplay(1);
        }
    }

    if (TestBit(kIsLogy))
        gPad->SetLogy();

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

// --------------------------------------------------------------------------
//
// Used to rebuild a MHVsTime object of the same type (data members,
// dimension, ...)
//
MParContainer *MHVsTime::New() const
{
    MHVsTime *h=new MHVsTime(fData ? (const char*)GetRule() : NULL);
    h->SetScale(fScale);
    if (fUseEventNumber)
        h->SetUseEventNumber();
    h->SetMaxPts(fMaxPts);
    return h;
}

TString MHVsTime::GetRule() const
{
    return fData ? fData->GetRule() : (TString)"";
}

// --------------------------------------------------------------------------
//
// Returns the total number of bins in a histogram (excluding under- and
// overflow bins)
//
Int_t MHVsTime::GetNbins() const
{
    return fGraph->GetN();
}
/*
TH1 *MHVsTime::GetHist()
{
    return fGraph ? fGraph->GetHistogram() : 0;
}

const TH1 *MHVsTime::GetHist() const
{
    return fGraph ? fGraph->GetHistogram() : 0;
}

TH1 *MHVsTime::GetHistByName(const TString name)
{
    return GetHist();
}
*/
