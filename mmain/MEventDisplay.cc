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
!   Author(s): Thomas Bretz, 10/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */
#include "MEventDisplay.h"

//
// C-lib
//
#include <stdlib.h>              // atoi

//
// root
//
#include <TFile.h>               // TFile
#include <TTree.h>               // TTree
#include <TList.h>               // TList::Add
#include <TStyle.h>              // gStyle->SetOptStat
#include <TCanvas.h>             // TCanvas::cd
#include <TSystem.h>             // TSystem::BaseName

//
// root GUI
//
#include <TGLabel.h>             // TGLabel
#include <TGButton.h>            // TGPictureButton
#include <TG3DLine.h>            // TGHorizontal3DLine
#include <TGTextEntry.h>         // TGTextEntry
#include <TGButtonGroup.h>       // TGVButtonGroup
#include <TRootEmbeddedCanvas.h> // TRootEmbeddedCanvas

//
// General
//
#include "MGList.h"              // MGList
#include "MString.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"                  // MParList::AddToList
#include "MEvtLoop.h"                  // MEvtLoop::GetParList
#include "MTaskList.h"                 // MTaskList::AddToList

//
// Tasks
//
#include "MReadMarsFile.h"             // MReadMarsFile
#include "MGeomApply.h"                // MGeomApply
#include "MPedestalSubtract.h"         // MPedestalSubtract
#include "MFDataMember.h"              // MFDataMember
#include "MFEvtNumber.h"               // MFEvtNumber
#include "MMcPedestalCopy.h"           // MMcPedestalCopy
#include "MMcPedestalNSBAdd.h"         // MMcPedestalNSBAdd

#include "MSignalCalc.h"               // MPedestalCalc
#include "MTaskEnv.h"                  // MTaskEnv
#include "MExtractTimeAndChargeDigitalFilter.h"
#include "MImgCleanStd.h"              // MImgCleanStd
#include "MHillasCalc.h"               // MHillasCalc
#include "MCalibrationRelTimeCalc.h"   // MCalibrationRelTimeCalc
#include "MCalibrationPatternDecode.h" // MCalibrationPatternDecode
#include "MPrint.h"               
#include "MBadPixelsCalc.h"            // MBadPixelsCalc
#include "MBadPixelsTreat.h"           // MBadPixelsTreat
#include "MFillH.h"                    // MFillH
#include "MMcCalibrationUpdate.h"      // MMcCalibrationUpdate
#include "MCalibrateData.h"            // MCalibrateData
#include "MMuonSearchParCalc.h"        // MMuonSearchParCalc
#include "MMuonCalibParCalc.h"         // MMuonCalibParCalc
#include "MContinue.h"                 // MContinue
#include "MTriggerPatternDecode.h"     // MTriggerPatternDecode
#include "MSimSignalCam.h"             // MSimSignalCam
#include "MSimGeomCam.h"               // MSimGeomCam
//#include "MMcTriggerLvl2Calc.h"        // MMcTriggerLvl2Calc

//
// Container
//
#include "MSequence.h"                 // MSequence
#include "MDirIter.h"                  // MDirIter
#include "MHillas.h"                   // MHillas::Print(const MGeomCam&)
#include "MHillasExt.h"                // MHillasExt::Print(const MGeomCam&)
#include "MHillasSrc.h"                // MHillasSrc::Print(const MGeomCam&)
#include "MImagePar.h"                 // MImagePar::Print(const MGeomCam&)
#include "MNewImagePar.h"              // MNewImagePar::Print(const MGeomCam&)
#include "MHEvent.h"                   // MHEvent
#include "MHCamera.h"                  // MHCamera
#include "MRawEvtData.h"               // MRawEvtData
#include "MArrivalTimeCam.h"           // MArrivalTimeCam
#include "MBadPixelsCam.h"             // MBadPixelsCam
#include "MPedPhotCam.h"               // MPedPhotCam
#include "MCalibrationChargeCam.h"     // MCalibrationChargeCam
#include "MMuonSearchPar.h"            // MMuonCalibPar
#include "MHPhotonEvent.h"             // MHPhotonEvent
//#include "MMcTriggerLvl2.h"            // MMcTriggerLvl2

using namespace std;

ClassImp(MEventDisplay);

// --------------------------------------------------------------------------
//
//  Constructor.
//
MEventDisplay::MEventDisplay(const char *fname, const char *path) : MStatusDisplay(756), fEvtLoop(0)
{
    //
    // Setup Task list for hillas calculation
    //
    SetupTaskList("Events", fname, path);

    //
    // Add MEventDisplay GUI elements to the display
    //
    AddUserFrame(fname);

    //
    // Show new part of the window, resize to correct aspect ratio
    //
    // FIXME: This should be done by MStatusDisplay automatically
    Resize(768, 576 + fUserFrame->GetDefaultHeight());//TMath::Nint(GetWidth()*1.2), TMath::Nint(GetHeight()*1.2 + fUserFrame->GetDefaultHeight()));
    SetWindowName("Event Display");
    MapSubwindows();

    //
    // Readin first event and display it
    //
    if (fEvtLoop)
        ReadFirstEvent();

    SetNoContextMenu(kFALSE);
}

// --------------------------------------------------------------------------
//
//  Destructor: PostProcess eventloop, delete eventloop with all containers
//
MEventDisplay::~MEventDisplay()
{
    if (fEvtLoop)
    {
        fEvtLoop->PostProcess();
        delete fEvtLoop;
    }
}

Int_t MEventDisplay::GetFileType(const char *tree, const char *fname) const
{
    TFile f(fname, "READ");
    if (f.IsZombie())
        return 0; // Not a root file

    TTree *t = (TTree*)f.Get(tree);
    if (!t)
        return -1;

    if (t->FindBranch("MSignalCam."))
        return 1;

    if (t->FindBranch("MRawEvtData."))
        return 2;

    if (t->FindBranch("MPhotonEvent."))
        return 3;

    return -2;
}

// --------------------------------------------------------------------------
//
//  Setup Task and parameter list for hillas calculation,
//  preprocess tasks and read in first event (process)
//
void MEventDisplay::SetupTaskList(const char *tname, TString fname, const char *path)
{
    MBadPixelsCam *badpix = new MBadPixelsCam;

    Int_t type = GetFileType(tname, fname);
    switch (type)
    {
    case  1: gLog << all << "Calibrated Data File detected..." << endl; break;
    case  2: gLog << all << "Raw Data File detected..." << endl; break;
    case  3: gLog << all << "Ceres reflector file detected..." << endl; break;
    case -2: gLog << err << "File type unknown... abort." << endl; return;
    case -1: gLog << err << "Tree " << tname << " not found... abort." << endl; return;
    case  0: gLog << inf << "Assuming Sequence file." << endl; break;
    }

    if (type==0 && !MSequence::InflateSeq(fname))
        return;

    // Check if file is a sequence...
    MSequence *seq = type==0 ? new MSequence(fname, path) : new MSequence;
    if (type==0)
        type = 1;

    //
    // Setup an empty job, with a reader task only.
    // All tasks and parameter containers are deleted automatically
    // (via SetOwner())
    //
    MTaskList *tlist = new MTaskList;
    tlist->SetOwner();

    MParList *plist = new MParList;
    plist->SetOwner();
    plist->AddToList(tlist);
    plist->AddToList(badpix);
    plist->AddToList(seq);

    MReadMarsFile *read = new MReadMarsFile(tname);
    read->DisableAutoScheme();
    if (seq->IsValid())
    {
        MDirIter iter;
        if (seq->GetRuns(iter, MSequence::kCalibrated)<=0)
            return;
        read->AddFiles(iter);
    }
    else
        read->AddFile(fname);
    tlist->AddToList(read);

    MGeomApply *apl = new MGeomApply;
    tlist->AddToList(apl);

    if (type==3)
    {
        MHPhotonEvent *phevt = new MHPhotonEvent;
        phevt->PermanentReset();
        plist->AddToList(phevt);

        MFillH *fillh = new MFillH(phevt, "MPhotonEvent");
        fillh->SetNameTab("Detector");
        tlist->AddToList(fillh);

        MSimGeomCam *simgeom = new MSimGeomCam;
        tlist->AddToList(simgeom);

        MSimSignalCam *sim = new MSimSignalCam;
        tlist->AddToList(sim);
    }

    MContinue *mcevts = new MContinue("MRawEvtData.GetNumPixels==0", "ContEmpty");
    if (type==2)
        tlist->AddToList(mcevts);

    MFEvtNumber *evtnum = new MFEvtNumber;
    tlist->AddToList(evtnum);

    MContinue *contevt = new MContinue(evtnum);
    contevt->SetInverted();
    tlist->AddToList(contevt);

    MTriggerPatternDecode *decode = new MTriggerPatternDecode;
    tlist->AddToList(decode);

    //MArrivalTime *atime = new MArrivalTime;
    //plist->AddToList(atime);

    MHEvent *evt01 = new MHEvent(MHEvent::kEvtSignalDensity);
    MHEvent *evt02 = new MHEvent(MHEvent::kEvtSignalDensity);
    MHEvent *evt03 = new MHEvent(type==1?MHEvent::kEvtPedPhot:MHEvent::kEvtPedestal);
    MHEvent *evt04 = new MHEvent(type==1?MHEvent::kEvtPedPhotRMS:MHEvent::kEvtPedestalRMS);
    MHEvent *evt06a= new MHEvent(MHEvent::kEvtCleaningData);
    MHEvent *evt06b= new MHEvent(MHEvent::kEvtCleaningLevels);
    //MHEvent *evt07 = new MHEvent(MHEvent::kEvtIdxMax);
    MHEvent *evt08 = new MHEvent(MHEvent::kEvtArrTime);
    MHEvent *evt09 = new MHEvent(MHEvent::kEvtArrTimeCleaned);
    //MHEvent *evt09 = new MHEvent(MHEvent::kEvtTrigPix);
    MHEvent *evt10 = new MHEvent(MHEvent::kEvtIslandIndex);

    evt01->SetName("Signal");
    evt02->SetName("Cleaned");
    evt03->SetName("Ped");
    evt04->SetName("PedRMS");
    evt06a->SetName("CleanData");
    evt06b->SetName("Levels");
    //evt07->SetName("MaxSliceIdx");
    evt08->SetName("ArrTime");
    evt09->SetName("Time");
    evt10->SetName("Islands");

    evt01->SetMinimum(0);
    evt03->SetMinimum(0);
    evt04->SetMinimum(0);
    evt06a->SetMinimum(0);

    // This makes sure, that the containers are deleted...
    plist->AddToList(evt01);
    plist->AddToList(evt02);
    plist->AddToList(evt03);
    plist->AddToList(evt04);
    plist->AddToList(evt06a);
    plist->AddToList(evt06b);
    //plist->AddToList(evt07);
    plist->AddToList(evt08);
    plist->AddToList(evt09);
    plist->AddToList(evt10);

    MFillH          *fill01  = new MFillH(evt01, "MSignalCam", "MFillH01");
    MImgCleanStd    *clean   = new MImgCleanStd;
    MFillH          *fill02  = new MFillH(evt02, "MSignalCam", "MFillH02");
    MFillH          *fill03  = new MFillH(evt03, type==1?"MPedPhotFundamental":"MPedestalCam",       "MFillH03");
    MFillH          *fill04  = new MFillH(evt04, type==1?"MPedPhotFromExtractorRndm":"MPedestalCam", "MFillH04");
    MFillH          *fill06a = new MFillH(evt06a, "MCameraData", "MFillH06a");
    MFillH          *fill06b = new MFillH(evt06b, "MCameraData", "MFillH06b");
    MHillasCalc     *hcalc   = new MHillasCalc;
    //MMcTriggerLvl2Calc *trcal  = new MMcTriggerLvl2Calc;
    //MFillH             *fill09 = new MFillH(evt09, "MMcTriggerLvl2", "MFillH09");
    MFillH          *fill10  = new MFillH(evt10, "MSignalCam",    "MFillH10");

    MExtractTimeAndChargeDigitalFilter *digf = new MExtractTimeAndChargeDigitalFilter;
    digf->SetNameWeightsFile("msignal/cosmics_weights46.dat");

    MTaskEnv *taskenv1=new MTaskEnv("ExtractSignal");
    taskenv1->SetDefault(digf);
    taskenv1->SetOwner();

    MSignalCalc     *nanal   = new MSignalCalc;

    MBadPixelsCalc  *bcalc   = new MBadPixelsCalc;
    MBadPixelsTreat *btreat  = new MBadPixelsTreat;
    btreat->SetProcessTimes(kFALSE);
    if (type==1)
    {
        bcalc->SetNamePedPhotCam("MPedPhotFromExtractor");
        btreat->AddNamePedPhotCam("MPedPhotFromExtractorRndm");
        clean->SetNamePedPhotCam("MPedPhotFromExtractorRndm");
    }

    // If no pedestal or no calibration file is availble
    if (type==2)
    {
        MFilter *f1=new MFDataMember("MRawRunHeader.fRunType", '>', 255.5);
        f1->SetName("MFMonteCarlo");

        MMcPedestalCopy   *pcopy = new MMcPedestalCopy;
        MMcPedestalNSBAdd *pdnsb = new MMcPedestalNSBAdd;

        MCalibrationPatternDecode *patdecode = new MCalibrationPatternDecode;

        MPedestalSubtract *pedsub1  = new MPedestalSubtract("PedSubstract1");
        MPedestalSubtract *pedsub2  = new MPedestalSubtract("PedSubstract2");
        pedsub2->SetNamePedestalCam();

        MMcCalibrationUpdate* mcupd = new MMcCalibrationUpdate;
        mcupd->SetOuterPixelsGainScaling(kFALSE);

        MCalibrateData* calib = new MCalibrateData;
        calib->SetCalibrationMode(MCalibrateData::kFlatCharge);
        calib->SetPedestalFlag(MCalibrateData::kEvent);

        // MPedestalCam is extracted using the simple MSignalCalc!

        // MC
        mcupd->SetFilter(f1);
        //trcal->SetFilter(f1);

        // TaskList
        tlist->AddToList(f1);
        tlist->AddToList(pcopy);
        tlist->AddToList(pdnsb);

        tlist->AddToList(patdecode);

        tlist->AddToList(pedsub1);   // Produce arrays
        tlist->AddToList(nanal);     // Estimate pedestal!
        tlist->AddToList(pedsub2);   // subtract pedestal

        tlist->AddToList(taskenv1); // Calculates MExtractedSignalCam, MArrivalTimeCam
        tlist->AddToList(mcupd);
        tlist->AddToList(calib);    // MExtractedSignalCam --> MSignalCam

        //tlist->AddToList(bcalc);    // Produce MBadPixelsCam
        //tlist->AddToList(btreat);   // Treat MBadPixelsCam
    }

    tlist->AddToList(fill01);
    tlist->AddToList(clean);
    tlist->AddToList(fill02);
    if (type==2)
        tlist->AddToList(fill03);
    if (type!=3)
        tlist->AddToList(fill04);
    tlist->AddToList(fill06a);
    tlist->AddToList(fill06b);
    tlist->AddToList(fill10);
    if (type==2)
    {
        //MFillH *fill07 = new MFillH(evt07, "MRawEvtData",     "MFillH7");
        MFillH *fill08 = new MFillH(evt08, "MArrivalTimeCam", "MFillH8");
        //tlist->AddToList(fill07);
        tlist->AddToList(fill08);

	//tlist->AddToList(trcal);
	//tlist->AddToList(fill09);
    }
    if (type==1 || type==3)
    {
        MFillH *fill08 = new MFillH(evt08, "MSignalCam", "MFillH8");
        MFillH *fill09 = new MFillH(evt09, "MSignalCam", "MFillH9");
        tlist->AddToList(fill08);
        tlist->AddToList(fill09);
    }
    tlist->AddToList(hcalc);

    // --------------------------------------------------
    // Muon Analysis
    MMuonSearchParCalc *muscalc = new MMuonSearchParCalc;
    MMuonCalibParCalc  *mcalc   = new MMuonCalibParCalc;
    MFillH *fillmuon = new MFillH("MHSingleMuon", "", "FillMuon");
    fillmuon->SetNameTab("Muon");
    tlist->AddToList(muscalc);
    tlist->AddToList(fillmuon);
    tlist->AddToList(mcalc);

    // --------------------------------------------------
    // Cut to steer the display
    MContinue *cont0=new MContinue("", "Cut");
    cont0->SetAllowEmpty();
    tlist->AddToList(cont0);

    //
    // Now distribute Display to all tasks
    //
    fEvtLoop = new MEvtLoop(gSystem->BaseName(fname));
    fEvtLoop->SetOwner();
    fEvtLoop->SetParList(plist);
    fEvtLoop->SetDisplay(this);
    fEvtLoop->ReadEnv("mars.rc");

}

// --------------------------------------------------------------------------
//
//  Add the top part of the frame: This is filename and treename display
//
void MEventDisplay::AddTopFramePart1(TGCompositeFrame *frame,
                                    const char *filename,
                                    const char *treename)
{
    //
    //  --- the top1 part of the window ---
    //
    TGHorizontalFrame *top1 = new TGHorizontalFrame(frame, 1, 1);
    fList->Add(top1);

    //
    // create gui elements
    //
    TGLabel *file = new TGLabel(top1, new TGString(MString::Format("%s#%s", filename, treename)));
    fList->Add(file);

    //
    // layout and add gui elements in/to frame
    //
    TGLayoutHints *laystd = new TGLayoutHints(kLHintsCenterX, 5, 5);
    fList->Add(laystd);

    top1->AddFrame(file,  laystd);

    //
    //  --- the top1 part of the window ---
    //
    TGHorizontalFrame *top2 = new TGHorizontalFrame(frame, 1, 1);
    fList->Add(top2);

    //
    // layout and add frames
    //
    TGLayoutHints *laytop1 = new TGLayoutHints(kLHintsCenterX, 5, 5, 5);
    fList->Add(laytop1);
    frame->AddFrame(top1, laytop1);
    frame->AddFrame(top2, laytop1);
}

// --------------------------------------------------------------------------
//
//  Add the second part of the top frame: This are the event number controls
//
void MEventDisplay::AddTopFramePart2(TGCompositeFrame *frame)
{
    //
    // --- the top2 part of the window ---
    //
    TGHorizontalFrame *top2 = new TGHorizontalFrame(frame, 1, 1);
    fList->Add(top2);

    //
    // Create the gui elements
    //
    TGTextButton *prevevt = new TGTextButton(top2, " << ", kEvtPrev);
    prevevt->Associate(this);

    TGLabel *evtnr = new TGLabel(top2, new TGString("Event:"));

    TGTextEntry *entry=new TGTextEntry(top2, new TGTextBuffer(100), kEvtNumber);
    entry->Resize(60, entry->GetDefaultHeight());
    entry->Associate(this);

    fNumOfEvts = new TGLabel(top2, "of           .");

    TGTextButton *nextevt = new TGTextButton (top2, " >> ", kEvtNext);
    nextevt->Associate(this);

    //
    // Add gui elements to 'atotodel'
    //
    fList->Add(prevevt);
    fList->Add(evtnr);
    fList->Add(entry);
    fList->Add(fNumOfEvts);
    fList->Add(nextevt);

    //
    // add the gui elements to the frame
    //
    TGLayoutHints *laystd = new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 5, 5);
    fList->Add(laystd);

    top2->AddFrame(prevevt,    laystd);
    top2->AddFrame(evtnr,      laystd);
    top2->AddFrame(entry,      laystd);
    top2->AddFrame(fNumOfEvts, laystd);
    top2->AddFrame(nextevt,    laystd);

    TGLayoutHints *laystd2 = new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5);
    fList->Add(laystd2);
    frame->AddFrame(top2, laystd2);
}

void MEventDisplay::AddMiddleFrame(TGCompositeFrame *frame)
{
    //
    // Create the gui elements
    //
    TGCheckButton *imgpar = new TGCheckButton(frame, "ImgPar", kShowImgPar);
    imgpar->SetState(kButtonDown);
    imgpar->Associate(this);
    fList->Add(imgpar);

    TGCheckButton *muon = new TGCheckButton(frame, "Muon", kShowMuon);
    muon->Associate(this);
    muon->SetState(kButtonDown);
    fList->Add(muon);

    //
    // add the gui elements to the frame
    //
//    TGLayoutHints *laystd = new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 5, 5);
//    fList->Add(laystd);
//    top2->AddFrame(prevevt,    laystd);

    frame->AddFrame(imgpar);
    frame->AddFrame(muon);
}

// --------------------------------------------------------------------------
//
//  Add the user frame part of the display
//
void MEventDisplay::AddUserFrame(const char* filename)
{
    fUserFrame->ChangeOptions(kHorizontalFrame);

    TGCompositeFrame *vf1 = new TGVerticalFrame(fUserFrame, 1, 1);
    TGCompositeFrame *vf2 = new TGVerticalFrame(fUserFrame, 1, 1);
    TGCompositeFrame *hor = new TGHorizontalFrame(vf1, 1, 1);
    TGCompositeFrame *ver = new TGVerticalFrame(hor, 1, 1);
    TGCompositeFrame *vfb = new TGVerticalFrame(hor, 1, 1);

    TGHorizontal3DLine *line = new TGHorizontal3DLine(vf1);
    TGLayoutHints *expx = new TGLayoutHints(kLHintsExpandX);
    TGLayoutHints *right = new TGLayoutHints(kLHintsRight|kLHintsTop);
    fList->Add(line);
    fList->Add(expx);
    fList->Add(right);

    hor->AddFrame(ver);
    hor->AddFrame(vfb, right);

    AddMiddleFrame(vfb);

    //
    // Add trailing line...
    //
    vf1->AddFrame(hor,  expx);
    vf1->AddFrame(line, expx);

    AddTopFramePart1(ver, filename, "Events");
    AddTopFramePart2(ver);

    // create root embedded canvas and add it to the tab
    TRootEmbeddedCanvas *ec = new TRootEmbeddedCanvas("Slices", vf2, vf1->GetDefaultHeight()*3/2, vf1->GetDefaultHeight(), 0);
    vf2->AddFrame(ec);
    fList->Add(ec);

    // set background and border mode of the canvas
    fCanvas = ec->GetCanvas();
    fCanvas->SetBorderMode(0);
    gROOT->GetListOfCanvases()->Add(fCanvas);
    //fCanvas->SetBorderSize(1);
    //fCanvas->SetBit(kNoContextMenu);
    //fCanvas->SetFillColor(16);

    fUserFrame->AddFrame(vf1, expx);
    fUserFrame->AddFrame(vf2);
}

// --------------------------------------------------------------------------
//
//  Checks if the event number is valid, and if so reads the new event
//  and updates the display
//
void MEventDisplay::ReadinEvent(Int_t dir)
{
    MParList    *plist = (MParList*)   fEvtLoop->GetParList();
    MTaskList   *tlist = (MTaskList*)  fEvtLoop->GetTaskList();
    MGeomCam    *geom  = (MGeomCam*)   plist->FindObject("MGeomCam");
    //MRawEvtData *raw   = (MRawEvtData*)plist->FindObject("MRawEvtData");

    //
    // Read first event.
    //
    MReadTree *reader = (MReadTree*)fEvtLoop->FindTask("MRead");

    const Int_t num = reader->GetNumEntry();

    Int_t rc;
    do
    {
        if (dir<0 && !reader->DecEventNum())
        {
            reader->SetEventNum(num);
            return;
        }
        if (dir>0 && !reader->IncEventNum())
        {
            reader->SetEventNum(num);
            return;
        }

        rc = tlist->Process();
        if (rc==kFALSE || rc==kERROR)
            return;

        reader->DecEventNum();

        // Define other continue conditions
        //if (raw && raw->GetNumPixels()<1)
        //    rc = kCONTINUE;

    } while (rc==kCONTINUE && dir!=0);

    //
    // Cleare the 'FADC canvas'
    //
    fCanvas->Clear();
    fCanvas->Modified();
    fCanvas->Update();

    //
    // Print parameters
    //
    *fLog << all;
    fLog->Separator(MString::Format("Entry %d", reader->GetNumEntry()+1));

    ((MHillas*)     plist->FindObject("MHillas"))->Print(*geom);
    ((MHillasExt*)  plist->FindObject("MHillasExt"))->Print(*geom);
    ((MHillasSrc*)  plist->FindObject("MHillasSrc"))->Print(*geom);
    plist->FindObject("MImagePar")->Print();
    ((MNewImagePar*)plist->FindObject("MNewImagePar"))->Print(*geom);
    plist->FindObject("MMuonCalibPar")->Print();
    ((MMuonSearchPar*)plist->FindObject("MMuonSearchPar"))->Print(*geom);

    //
    // UpdateDisplay
    //
    gStyle->SetOptStat(1101);
    Update();

    TGTextEntry *entry = (TGTextEntry*)fList->FindWidget(kEvtNumber);
    entry->SetText(MString::Format("%d", reader->GetNumEntry()+1));
}

void MEventDisplay::ShowHide()
{
    TGCheckButton *but1 = (TGCheckButton*)fList->FindWidget(kShowImgPar);
    TGCheckButton *but2 = (TGCheckButton*)fList->FindWidget(kShowMuon);

    const Bool_t imgpar = but1 && but1->IsDown();
    const Bool_t muon   = but2 && but2->IsDown();

    //
    // Get parlist
    //
    MParList *plist = fEvtLoop->GetParList();
    if (!plist)
        return;

    //
    // Draw ellipse on top of all pads
    //
    const Int_t n = 5;

    TObject *obj[n] = {
        plist->FindObject("MHillas"),
        plist->FindObject("MHillasSrc"),
        plist->FindObject("MHillasExt"),
        plist->FindObject("MSrcPosCam"),
        plist->FindObject("MMuonSearchPar")
    };

    const Bool_t state[n] = {
        imgpar,
        imgpar,
        imgpar,
        imgpar,
        muon,
    };

    const Int_t ismc = plist->FindObject("MHPhotonEvent") ? 1 : 0;

    for (int i=1; i<7; i++)
    {
        TCanvas *c = GetCanvas(i+ismc);
        if (!c)
            continue;

        c->cd(1);

        TList *list = gPad->GetListOfPrimitives();

        for (int j=0;j<n; j++)
            if (obj[j])
            {
                if (state[j] && !list->FindObject(obj[j]))
                    list->Add(obj[j]);
                if (!state[j] && list->FindObject(obj[j]))
                    list->Remove(obj[j]);
            }

        gPad->Modified(kTRUE);
    }
}

// --------------------------------------------------------------------------
//
//  Read first event to get display booted
//
void MEventDisplay::ReadFirstEvent()
{
    const Int_t rc = fEvtLoop->PreProcess();
    if (rc==kFALSE || rc==kERROR)
        return;

    UnLock();

    //
    // Now read event...
    //
    // FIXME: Can we safely replace ReadinEvent() by RedinEvent(1)?
    ReadinEvent();

    //
    // After ReInit (MGeomCam might be read from RunHeaders): Add Geometry tab
    //
    AddGeometryTabs();

    MReadTree *reader = (MReadTree*)fEvtLoop->FindTask("MRead");
    TGString *txt = new TGString(MString::Format("of %d", reader->GetEntries()));
    fNumOfEvts->SetText(txt);

    ShowHide();
}

// --------------------------------------------------------------------------
//
//  Adds the geometry tab 
//
void MEventDisplay::AddGeometryTabs()
{
    MGeomCam *geom = (MGeomCam*)fEvtLoop->GetParList()->FindObject("MGeomCam");
    if (!geom)
        return;

    TCanvas &c1=AddTab("Geometry");

    MHCamera *cam = new MHCamera(*geom);
    cam->SetBit(TH1::kNoStats|MHCamera::kNoLegend|kCanDelete);
    cam->Draw("pixelindex");

    c1.Modified();
    c1.Update();

    TCanvas &c2=AddTab("Sectors");

    cam = new MHCamera(*geom);
    cam->SetBit(TH1::kNoStats|MHCamera::kNoLegend|kCanDelete);
    cam->Draw("sectorindex");

    c2.Modified();
    c2.Update();
}

// --------------------------------------------------------------------------
//
//  ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
//
// Processes information from all GUI items.
// Selecting an item usually generates an event with 4 parameters.
// The first two are packed into msg (first and second bytes).
// The other two are parm1 and parm2.
//
Bool_t MEventDisplay::ProcessMessage(Long_t msg, Long_t mp1, Long_t mp2)
{
    switch (GET_MSG(msg))
    {
    case kC_TEXTENTRY:
        switch(GET_SUBMSG(msg))
        {
        case kTE_ENTER:
            switch(GET_SUBMSG(msg))
            {
            case kTE_ENTER:
                {
                    TGTextEntry *entry = (TGTextEntry*)fList->FindWidget(kEvtNumber);
                    MReadTree *reader  = (MReadTree*)fEvtLoop->FindTask("MRead");
                    if (reader->SetEventNum(atoi(entry->GetText())-1))
                        ReadinEvent();
                }
                return kTRUE;
            }
            return kTRUE;
        }
        break;

    case kC_COMMAND:
        switch (GET_SUBMSG(msg))
        {
        case kCM_TAB:
            {
                //
                // Set name for 'FADC canvas'. The name is the anchor for MHCamera.
                // and clear the canvas
                TCanvas *c = GetCanvas(mp1);
                if (!c)
                    break;
                MHEvent *o = (MHEvent*)fEvtLoop->GetParList()->FindObject(c->GetName());
                if (!o)
                    break;
                fCanvas->SetName(MString::Format("%p;%p;PixelContent", o->GetHist(), c->GetPad(1)));
            }
            break;

        case kCM_BUTTON:
            switch (mp1)
            {
            case kEvtPrev:
                ReadinEvent(-1);
                return kTRUE;

            case kEvtNext:
                ReadinEvent(+1);
                return kTRUE;
            }
            return kTRUE;

        case kCM_CHECKBUTTON:
            switch (mp1)
            {
            case kShowMuon:
            case kShowImgPar:
                ShowHide();
                return kTRUE;
            }
            return kTRUE;
        }
        break;
    }

    return MStatusDisplay::ProcessMessage(msg, mp1, mp2);
}
