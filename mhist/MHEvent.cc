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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//                                                                          
//  MHEvent
//                                                                          
//  Display a single event in a canvas with as many informations as
//  possible, such as event number, run number, ...
//
//  WARNING: This class is not yet ready!
//
//  Input Containers:
//   MGeomCam
//   [MTaskList]
//   [MImgCleanStd]
//   [MRawEvtData]
//   [MRawRunHeader]
//   [MRawEvtHeader]
//
//  Output Containers:
//   -/-
//
//////////////////////////////////////////////////////////////////////////////
#include "MHEvent.h"

#include <TStyle.h>
#include <TCanvas.h>
#include <TPaveText.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MMcEvt.hxx"
#include "MMcTrig.hxx"

#include "MHCamera.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MParList.h"
#include "MGeomCam.h"
#include "MSignalCam.h"
#include "MRawEvtHeader.h"
#include "MRawRunHeader.h"
#include "MRawEvtData.h"
#include "MImgCleanStd.h"
#include "MTriggerPattern.h"

ClassImp(MHEvent);

using namespace std;

// --------------------------------------------------------------------------
//
MHEvent::MHEvent(EventType_t type) : fHist(NULL), fType(type),
    fMinimum(-1111), fMaximum(-1111)
{
    fName = "MHEvent";
    fTitle = "Single Event display task";

    fClone   = new MRawEvtData("MHEventData");
    fTrigCpy = new MTriggerPattern;
}

// --------------------------------------------------------------------------
//
MHEvent::~MHEvent()
{
    if (fHist)
        delete fHist;

    delete fClone;
    delete fTrigCpy;
}

Bool_t MHEvent::SetupFill(const MParList *plist)
{
    MTaskList *tlist = (MTaskList*)plist->FindObject("MTaskList");

    fImgCleanStd = tlist ? (MImgCleanStd*)tlist->FindObject("MImgCleanStd") : NULL;
    fMcEvt       = (MMcEvt*)plist->FindObject("MMcEvt");
    fTime        = (MTime*)plist->FindObject("MTime");
    fTrigger     = (MTriggerPattern*)plist->FindObject("MTriggerPattern");

    fRawEvtData = (MRawEvtData*)plist->FindObject("MRawEvtData");
    if (!fRawEvtData)
        *fLog << warn << "MRawEvtData not found..." << endl;

    fRawRunHeader = (MRawRunHeader*)plist->FindObject("MRawRunHeader");
    if (!fRawRunHeader)
        *fLog << warn << dbginf << "MRawRunHeader not found..." << endl;

    fRawEvtHeader = (MRawEvtHeader*)plist->FindObject("MRawEvtHeader");
    if (!fRawEvtHeader)
        *fLog << warn << dbginf << "MRawEvtHeader not found..." << endl;

    MGeomCam *cam = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!cam)
    {
        *fLog << err << GetDescriptor() << ": No MGeomCam found... aborting." << endl;
        return kFALSE;
    }

    if (fHist)
        delete (fHist);

    fHist = new MHCamera(*cam);
    fHist->AddNotify(fClone);

    fHist->SetMinimum(fMinimum);
    fHist->SetMaximum(fMaximum);

    switch (fType)
    {
    case kEvtSignalRaw:
        fHist->SetName("Signal (raw)");
        fHist->SetYTitle("S [au]");
        break;
    case kEvtSignalDensity:
        fHist->SetName("Signal density");
        fHist->SetYTitle("S [au]");
        break;
    case kEvtSignalUncleaned:
        fHist->SetName("Signal");
        fHist->SetYTitle("S [au]");
        break;
    case kEvtPedestal:
        fHist->SetName("Pedestal");
        fHist->SetYTitle("P [au]");
        break;
    case kEvtPedestalRMS:
        fHist->SetName("Pedestal RMS");
        fHist->SetYTitle("\\sigma_{P} [au]");
        break;
    case kEvtPedPhot:
        fHist->SetName("Pedestal");
        fHist->SetYTitle("P [\\gamma]");
        break;
    case kEvtPedPhotRMS:
        fHist->SetName("Pedestal RMS");
        fHist->SetYTitle("\\sigma_{P} [\\gamma]");
        break;
    case kEvtCleaningLevels:
        if (!fImgCleanStd)
        {
            *fLog << err << "MImgCleanStd not found... aborting." << endl;
            return kFALSE;
        }
        fHist->SetName("CleanLevels");
        fHist->SetYTitle("L");
        break;
    case kEvtCleaningData:
        fHist->SetName("CleanData");
        fHist->SetYTitle("L");
        break;
     case kEvtIdxMax:
        fHist->SetName("Max Slice Idx");
        fHist->SetYTitle("t [slice id]");
	fHist->SetPrettyPalette();
        break;
     case kEvtArrTime:
     case kEvtArrTimeCleaned:
        fHist->SetName("Arrival Time");
        fHist->SetYTitle("t [slice id]");
	fHist->SetPrettyPalette();
        break;
     case kEvtTrigPix:
        fHist->SetName("Triggered pix");
        fHist->SetYTitle("ON/OFF");
        fHist->SetPrettyPalette();
        break;
     case kEvtIslandIndex:
        fHist->SetName("Island Index");
        fHist->SetYTitle("Index");
	fHist->SetPrettyPalette();
        break;
     case kEvtTimeSlope:
     case kEvtTimeSlopeCleaned:
        fHist->SetName("Time Slope");
        fHist->SetYTitle("delta_t [ns]");
	fHist->SetPrettyPalette();
        break;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The geometry read from the RunHeaders might have changed. This does not
// effect anything in PreProcess. So we set a new geometry. We don't move
// this away from PreProcess to support also loops without calling ReInit.
//
Bool_t MHEvent::ReInit(MParList *plist)
{
    MGeomCam *cam = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!cam)
    {
        *fLog << err << GetDescriptor() << ": No MGeomCam found... aborting." << endl;
        return kFALSE;
    }

    fHist->SetGeometry(*cam);

    return kTRUE;
}

Int_t MHEvent::Fill(const MParContainer *par, const Stat_t weight)
{
    if (fHist->IsFreezed())
        return kTRUE;

    if (!par)
    {
        *fLog << err << "ERROR - par==NULL... abort." << endl;
        return kERROR;
    }

    const MCamEvent *event = dynamic_cast<const MCamEvent*>(par);
    if (!event)
    {
        *fLog << err << par->GetDescriptor() << " doesn't inherit from MCamEvent... abort." << endl;
        return kERROR;
    }

    if (fTrigger)
        fTrigger->Copy(*fTrigCpy);

    if (fRawEvtData)
        fRawEvtData->Copy(*fClone);

    switch (fType)
    {
    case kEvtSignalRaw:   // Get Content without pixel-size scaling
        fHist->SetCamContent(*event, 3);
        break;
    case kEvtSignalDensity:
        fHist->SetCamContent(*event, 0);
        break;
    case kEvtSignalUncleaned:
        fHist->SetCamContent(*event, 12);
        break;
    case kEvtPedestal:
        fHist->SetCamContent(*event, 0);
        break;
    case kEvtPedestalRMS:
        fHist->SetCamContent(*event, 2);
        break;
    case kEvtPedPhot:
        fHist->SetCamContent(*event, 4);
        break;
    case kEvtPedPhotRMS:
        fHist->SetCamContent(*event, 5);
        break;
    case kEvtCleaningLevels:
        {
            TArrayF lvl(2);
            lvl[0] = fImgCleanStd->GetCleanLvl2();
            lvl[1] = fImgCleanStd->GetCleanLvl1();
            fHist->SetCamContent(*event, 0);
            fHist->SetLevels(lvl);
        }
        break;
    case kEvtCleaningData:
        fHist->SetCamContent(*event, 0);
        break;
//    case kEvtIdxMax:
//        fHist->SetCamContent(*event, 3/*5*/);
//        break;
    case kEvtArrTime:
        fHist->SetCamContent(*event, 6);
        break; 
    case kEvtArrTimeCleaned:
        fHist->SetCamContent(*event, 8);
        break; 
    case kEvtTrigPix:
        fHist->SetCamContent(*event, 0);
        break; 
    case kEvtIslandIndex:
        fHist->SetCamContent(*event, 5);
        break;
    case kEvtTimeSlope:
        fHist->SetCamContent(*event, 13);
        break; 
    case kEvtTimeSlopeCleaned:
        fHist->SetCamContent(*event, 14);
        break; 
    default:
        *fLog << "ERROR - Case " << (int)fType << " not implemented..." << endl;
    }

    TString s;
    if (fRawEvtHeader)
    {
        s += "Event #";
        s += fRawEvtHeader->GetDAQEvtNumber();
    }

    if (fRawEvtHeader && fRawRunHeader)
        s += " FileId #";

    if (fRawEvtHeader)
        s += fRawRunHeader->GetStringID();

    if (fTime)
    {
        s += " (";
        s += fTime->GetString();
        s += ")";
    }

    if (fMcEvt)
        s = fMcEvt->GetDescription(s);

    gPad=NULL;
    fHist->SetTitle(s);

    return kTRUE;
}

void MHEvent::Paint(Option_t *o)
{
    TVirtualPad *pad = gPad;

    if (TString(o).IsNull())
    {
        pad->GetPad(2)->cd(1);
        if (gPad->FindObject("Proj"))
        {
            TH1 *h=fHist->Projection("Proj");
            if (h->GetMaximum()>0)
                gPad->SetLogy();
        }

        pad->GetPad(2)->cd(2);
        if (gPad->FindObject("ProfRad"))
            fHist->RadialProfile("ProfRad");

        pad->GetPad(2)->cd(3);
        if (gPad->FindObject("ProfAz"))
            fHist->AzimuthProfile("ProfAz");
    }

    if (TString(o)=="1")
    {
        const Double_t r = fHist->GetGeometry()->GetMaxRadius();
        TPaveText s4(0.82*r, -r, 0.90*r, -0.92*r, "br");
        TPaveText s3(0.74*r, -r, 0.82*r, -0.92*r, "br");
        TPaveText s2(0.66*r, -r, 0.74*r, -0.92*r, "br");
        TPaveText s1(0.58*r, -r, 0.66*r, -0.92*r, "br");
        TPaveText s0(0.50*r, -r, 0.58*r, -0.92*r, "br");
        s0.SetBorderSize(1);
        s1.SetBorderSize(1);
        s2.SetBorderSize(1);
        s3.SetBorderSize(1);
        s4.SetBorderSize(1);
        s0.AddText("P");
        s1.AddText("C");
        s2.AddText("1");
        s3.AddText("2");
        s4.AddText("S");

        const Byte_t pat = fTrigCpy->GetUnprescaled();

        s0.SetFillColor(pat&MTriggerPattern::kPedestal    ? kBlue : kWhite);
        s1.SetFillColor(pat&MTriggerPattern::kCalibration ? kBlue : kWhite);
        s2.SetFillColor(pat&MTriggerPattern::kTriggerLvl1 ? kBlue : kWhite);
        s3.SetFillColor(pat&MTriggerPattern::kTriggerLvl2 ? kBlue : kWhite);
        s4.SetFillColor(pat&MTriggerPattern::kSumTrigger  ? kBlue : kWhite);

        s0.Paint();
        s1.Paint();
        s2.Paint();
        s3.Paint();
        s4.Paint();
    }
}

void MHEvent::Draw(Option_t *)
{
    if (!fHist)
    {
        *fLog << warn << "MHEvent::Draw - fHist==NULL not initialized." << endl;
        return;
    }

    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad();

    pad->Divide(2,1);

    pad->cd(1);
    gPad->SetPad(0.01, 0.01, 0.75, 0.99);
    gPad->SetBorderMode(0);
    fHist->Draw();

    AppendPad("1");

    pad->cd(2);
    gPad->SetPad(0.75, 0.01, 0.99, 0.99);
    gPad->SetBorderMode(0);
    gPad->Divide(1,3);

    pad = gPad;

    pad->cd(1);
    gPad->SetBorderMode(0);

    TH1 *h = fHist->Projection("Proj", 50);
    h->SetTitle("Projection");
    h->SetBit(kCanDelete);
    h->Draw();

    pad->cd(2);
    gPad->SetBorderMode(0);

    h = (TH1*)fHist->RadialProfile("ProfRad", 20);
    h->SetTitle("Radial Profile");
    h->SetBit(kCanDelete|TH1::kNoStats);
    h->Draw();

    pad->cd(3);
    gPad->SetBorderMode(0);
    h = (TH1*)fHist->AzimuthProfile("ProfAz", 30);
    h->SetTitle("Azimuth Profile");
    h->SetBit(kCanDelete|TH1::kNoStats);
    h->Draw();
}

