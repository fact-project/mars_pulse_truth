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
!   Author(s): Thomas Bretz, 12/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHCamEvent
//
// A histogram to store the sum of camera events. This can be photons,
// currents or enything else derived from MCamEvent
//
// Setup
// =====
//
// To count how often a certain pixel is above or below a threshold do:
//    MHCamEvent::SetThreshold(5.5);  // Default=LowerBound
//    MHCamEvent::SetThreshold(5.5, MHCamEvent::kIsUpperBound);
//
//
// Axis titles
// ===========
//
// 1) If no other title is given 'a.u.' is used.
// 2) If the title of MHCamEvent is different from the default,
//    it is used as histogram title. You can use this to set the
//    axis title, too. For more information see TH1::SetTitle, eg.
//       SetTitle("MyHist;;y[cm];Counts");
//    Make sure to set an empty x-axis title.
//
// For example:
//   MHCamEvent myhist("Titele;;y [cm]");
//
// Class Version 3:
// ----------------
//  + fNameSub
//  + fIsRatio
//
/////////////////////////////////////////////////////////////////////////////
#include "MHCamEvent.h"

#include <TF1.h>
#include <TPaveStats.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"
#include "MParList.h"
#include "MCamEvent.h"
#include "MHCamera.h"

#include "MGeomCam.h"

ClassImp(MHCamEvent);

using namespace std;

const TString MHCamEvent::gsDefName  = "MHCamEvent";
const TString MHCamEvent::gsDefTitle = "Average of MCamEvents";

// --------------------------------------------------------------------------
//
// Initialize the name and title of the task
//
void MHCamEvent::Init(const char *name, const char *title)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    fNameGeom = "MGeomCam";
}

// --------------------------------------------------------------------------
//
// Initialize the name and title of the task. Set fType to 0
//
MHCamEvent::MHCamEvent(const char *name, const char *title)
: fSum(NULL), fErr(NULL), fEvt(NULL), fSub(0), fType(0), fErrorSpread(kTRUE), fErrorRelative(kFALSE),
fThreshold(0), fUseThreshold(0), fIsRatio(false)
{
    Init(name, title);

    fSum = new MHCamera;
}

// --------------------------------------------------------------------------
//
// Initialize the name and title of the task. Set fType to type
//
MHCamEvent::MHCamEvent(Int_t type, const char *name, const char *title)
: fSum(NULL), fErr(NULL), fEvt(NULL), fSub(0), fType(type), fErrorSpread(kTRUE), fErrorRelative(kFALSE),
fThreshold(0), fUseThreshold(0), fIsRatio(false)
{
    Init(name, title);

    fSum = new MHCamera;
}

// --------------------------------------------------------------------------
//
// Delete the corresponding camera display if available
//
MHCamEvent::~MHCamEvent()
{
    if (fSum)
        delete fSum;
}

Bool_t MHCamEvent::InitGeom(const MParList &plist)
{
    MGeomCam *cam = (MGeomCam*)plist.FindObject(fNameGeom, "MGeomCam");
    if (!cam)
    {
        *fLog << err << fNameGeom << " [MGeomCam] not found... abort." << endl;
        return kFALSE;
    }

    // combine name
    const TString name = fNameEvt.IsNull() ? fName : fNameEvt;

    fSum->SetGeometry(*cam, name+";avg");
    if (fTitle!=gsDefTitle)
        fSum->SetTitle(fTitle);
    if (!fTitle.Contains(";"))
        fSum->SetYTitle("a.u.");

    if (fErr)
        fErr->SetGeometry(*fSum->GetGeometry(), fErr->GetName(), fErr->GetTitle());

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Get the event (MCamEvent) the histogram might be filled with. If
// it is not given, it is assumed, that it is filled with the argument
// of the Fill function.
// Looks for the camera geometry MGeomCam and resets the sum histogram.
//
Bool_t MHCamEvent::SetupFill(const MParList *plist)
{
    fEvt = dynamic_cast<MCamEvent*>(plist->FindObject(fNameEvt, "MCamEvent"));
    if (!fEvt)
    {
        if (!fNameEvt.IsNull())
        {
            *fLog << err << GetDescriptor() << ": No " << fNameEvt <<" [MCamEvent] available..." << endl;
            return kFALSE;
        }
        *fLog << inf << GetDescriptor() << ": Assuming to get MCamEvent from Fill()..." << endl;
    }

    fSub = 0;
    if (!fNameSub.IsNull())
    {
        fSub = dynamic_cast<MCamEvent*>(plist->FindObject(fNameSub, "MCamEvent"));
        if (!fSub)
        {
            *fLog << err << GetDescriptor() << ": No " << fNameSub <<" [MCamEvent] available..." << endl;
            return kFALSE;
        }
        *fLog << inf << GetDescriptor() << ": Will subtract '" << fNameSub << "'" << endl;
    }

    fSum->Reset();

    fNumReInit = 0;

    if (!InitGeom(*plist))
        return kFALSE;

    if (fUseThreshold!=kCollectMin && fUseThreshold!=kCollectMax)
        fSum->SetBit(MHCamera::kProfile);
    if (!fErrorSpread)
        fSum->SetBit(MHCamera::kErrorMean);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The geometry read from the RunHeaders might have changed. This does not
// effect anything in PreProcess. So we set a new geometry. We don't move
// this away from PreProcess to support also loops without calling ReInit.
//
Bool_t MHCamEvent::ReInit(MParList *plist)
{
    return fNumReInit++==0 ? InitGeom(*plist) : kTRUE;
}


// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MCamEvent-Container.
//
Int_t MHCamEvent::Fill(const MParContainer *par, const Stat_t w)
{
    const MCamEvent *evt = par ? dynamic_cast<const MCamEvent*>(par) : fEvt;
    if (!evt)
    {
        *fLog << err << dbginf << "Got no MCamEvent as argument of Fill()..." << endl;
        return kERROR;
    }

    switch (fUseThreshold)
    {
    case kNoBound:
        if (fSub)
        {
            if (fIsRatio)
                fSum->AddCamRatio(*evt, *fSub, fType, w);
            else
                fSum->AddCamDifference(*evt, *fSub, fType, w);
        }
        else
            fSum->AddCamContent(*evt, fType, w);
        break;

    case kIsLowerBound:
    case kIsUpperBound:
        fSum->CntCamContent(*evt, fThreshold, fType, fUseThreshold>0);
        break;

    case kCollectMin:
        fSum->SetMinCamContent(*evt, /*fThreshold,*/ fType);
        break;

    case kCollectMax:
        fSum->SetMaxCamContent(*evt, /*fThreshold,*/ fType);
        break;

    case kMeanShift:
        fSum->AddMeanShift(*evt, fType, w);
        break;

    case kMedianShift:
        fSum->AddMedianShift(*evt, fType, w);
        break;

    default:
        *fLog << err << "ERROR - MHCamEvent::Fill: Unknown type." << endl;
        return kERROR;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Take the mean of the sum histogram and print all pixel indices
// which are above sum+s*rms
//
void MHCamEvent::PrintOutliers(Float_t s) const
{
    const Double_t mean = fSum->GetMean();
    const Double_t rms  = fSum->GetRMS();

    *fLog << all << underline << GetDescriptor() << ": Mean=" << mean << ", Rms=" << rms << endl;

    for (UInt_t i=0; i<fSum->GetNumPixels(); i++)
    {
        if (!fSum->IsUsed(i))
            continue;

        if ((*fSum)[i+1]>mean+s*rms)
            *fLog << "Contents of Pixel-Index #" << i << ": " << (*fSum)[i+1] << " > " << s << "*rms" << endl;
    }
}

// --------------------------------------------------------------------------
//
// Return fSum for "sum" and fRms for "rms"
//
TH1 *MHCamEvent::GetHistByName(const TString name) const
{
    return fSum;
}

// --------------------------------------------------------------------------
//
// Set the camera histogram to a clone of cam
//
void MHCamEvent::SetHist(const MHCamera &cam)
{
    if (fSum)
        delete fSum;

    fSum = static_cast<MHCamera*>(cam.Clone());
}

TString MHCamEvent::Format(const char *ext) const
{
    TString n = fSum->GetName();

    const Ssiz_t pos = n.Last(';');
    if (pos<0)
        return "";

    n  = n(0, pos);
    n += ";";
    n += ext;
    return n;
}

void MHCamEvent::Paint(Option_t *opt)
{
    TVirtualPad *pad = gPad;

    pad->cd(2);
    if (gPad->FindObject("proj"))
    {
        TH1 *h=fSum->Projection("proj");
        if (h->GetMaximum()>0)
            gPad->SetLogy();

        if (TString(opt).Contains("gaus", TString::kIgnoreCase))
        {
            // Create a copy of the function if it is not yet there
            if (!h->GetFunction("gaus"))
                h->Fit("gaus", "q");

            TF1 *f = h->GetFunction("gaus");
            if (f)
            {
                f->SetParameters(h->GetMean(), h->GetRMS(), h->GetMaximum());
                f->SetLineWidth(1);
                h->Fit("gaus", "q");
            }

            TPaveStats *s1 = dynamic_cast<TPaveStats*>(h->GetListOfFunctions()->FindObject("stats"));
            if (s1)
            {
                if (s1->GetOptStat()==1111)
                    s1->SetOptStat(1000000001);
                s1->SetOptFit(1);
            }
        }
    }

    pad->cd(5);
    if (gPad->FindObject("rad"))
        fSum->RadialProfile("rad");

    pad->cd(6);
    if (gPad->FindObject("az"))
        fSum->AzimuthProfile("az");

    pad->cd(4);
    gPad->cd(1);
    MHCamera *cam = (MHCamera*)gPad->FindObject("err");
    if (cam)
        cam->SetCamContent(*fSum, fErrorRelative ? 2 : 1);
}

void MHCamEvent::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    const Int_t col = pad->GetFillColor();
    pad->SetBorderMode(0);

    AppendPad(opt);

    TString name = MString::Format("%s_1", pad->GetName());
    TPad *p = new TPad(name,name,0.005, 0.5, 0.66, 0.995,col,0,0);
    p->SetFrameBorderMode(0);
    p->SetNumber(1);
    p->Draw();
    p->cd();
    fSum->Draw("EPhist");

    pad->cd();
    name = MString::Format("%s_2", pad->GetName());
    p = new TPad(name,name,0.66, 0.5, 0.995, 0.995,col,0,0);
    p->SetFrameBorderMode(0);
    p->SetNumber(2);
    p->Draw();
    p->cd();
    TH1 *h = fSum->Projection("proj", 50);
    h->SetTitle("Projection");
    h->SetBit(kCanDelete);
    h->Draw();

    pad->cd();
    name = MString::Format("%s_3", pad->GetName());
    p = new TPad(name,name,0.005, 0.005, 3./8, 0.5,col,0,0);
    p->SetFrameBorderMode(0);
    p->SetNumber(3);
    p->Draw();
    p->cd();
    fSum->Draw();

    pad->cd();
    name = MString::Format("%s_4", pad->GetName());
    p = new TPad(name,name,3./8, 0.005, 6./8-0.005, 0.5,col,0,0);
    p->SetFrameBorderMode(0);
    p->SetNumber(4);
    p->Draw();
    p->cd();

    TString e = "Sqrt(Variance)";
    if (fSum->TestBit(MHCamera::kErrorMean))
        e += "/Sqrt(n_{i})";
    if (fErrorRelative)
        e += "/v_{i}";

    fErr = new MHCamera(*fSum->GetGeometry());
    fErr->SetName("err");
    fErr->SetTitle(e);
    fErr->SetYTitle(fSum->GetYaxis()->GetTitle());
    fErr->SetCamContent(*fSum, fErrorRelative ? 2 : 1);
    fErr->SetBit(kMustCleanup);
    fErr->SetBit(kCanDelete);
    fErr->Draw();

    pad->cd();
    name = MString::Format("%s_5", pad->GetName());
    p = new TPad(name,name,6./8,0.25,0.995,0.5,col,0,0);
    p->SetFrameBorderMode(0);
    p->SetNumber(5);
    p->Draw();
    p->cd();
    h = (TH1*)fSum->RadialProfile("rad", 20);
    h->SetTitle("Radial Profile");
    h->SetBit(kCanDelete|TH1::kNoStats);
    h->Draw();

    pad->cd();
    name = MString::Format("%s_6", pad->GetName());
    p = new TPad(name,name,6./8,0.005,0.995,0.25,col,0,0);
    p->SetFrameBorderMode(0);
    p->SetNumber(6);
    p->Draw();
    p->cd();
    h = (TH1*)fSum->AzimuthProfile("az", 30);
    h->SetTitle("Azimuth Profile");
    h->SetBit(kCanDelete|TH1::kNoStats);
    h->Draw();
}


void MHCamEvent::RecursiveRemove(TObject *obj)
{
    if (obj==fErr)
        fErr = 0;
    if (obj==fSum)
        fSum = 0;

    MH::RecursiveRemove(obj);
}
