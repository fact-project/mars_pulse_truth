#include "MOnlineDump.h"

#include <TCanvas.h>
#include <TSystem.h>

#include <TGButton.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MTime.h"
#include "MRawRunHeader.h"
#include "MRawEvtHeader.h"

#include "MOnlineDisplay.h"

#include "MFRealTimePeriod.h"
#include "MHEvent.h"
#include "MHCamera.h"
#include "MHCamEvent.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MFillH.h"
#include "MEventRate.h"

ClassImp(MOnlineDump);

using namespace std;

void MOnlineDump::SetNoContextMenu(TObject *c)
{
    if (fDisplay->HasCanvas((TCanvas*)c))
        c->ResetBit(kNoContextMenu);
}

void MOnlineDump::DisplayTriggerRate()
{
    Bool_t set = kFALSE;

    if (!*fEvtTime)
    {
        fEvtTime->Now();
        set = kTRUE;
    }

    const UInt_t  evts = fRawEvtHeader->GetDAQEvtNumber() - fEvtNumber;
    const Double_t sec = *fEvtTime-fTime;

    fEvtNumber = fRawEvtHeader->GetDAQEvtNumber();
    fTime      = *fEvtTime;

    if (evts>0 && sec>0 && fDisplay)
        fDisplay->SetStatusLine2(Form("%sTrigger Rate: %.1fHz / Event Rate: %.1fHz",
                                      (set?"Arb. ":""), evts/sec, fRate->GetRate()));

    if (set)
        fEvtTime->Reset();
}

Bool_t MOnlineDump::ProcessMessage(Int_t msg, Int_t submsg, Long_t mp1, Long_t mp2)
{
    switch (msg)
    {
    case kC_VSLIDER:
        switch (submsg)
        {
        case kSL_POS:
            if (mp1==MOnlineDisplay::kSlider && GetFilter())
            {
                ((MFRealTimePeriod*)GetFilter())->SetTime(mp2*100);
                *fLog << dbg << "Update Time: " << Form("%.1fs", mp2/10.) << endl;
            }
            return kTRUE;
        }
        return kFALSE;
    case kC_COMMAND:
        switch(submsg)
        {
        case kCM_TAB:
            {
                //
                // Set name for 'FADC canvas'. The name is the anchor for MHCamera.
                // and clear the canvas
                //
                if (!fPlist || !fDisplay)
                    return kTRUE;

                TCanvas *c = fDisplay->GetCanvas(mp1);
                if (!c)
                    return kTRUE;

                MHEvent *o = (MHEvent*)fPlist->FindObject(c->GetName());
                if (o)
                    ((MOnlineDisplay*)fDisplay)->fCanvas->SetName(Form("%p;%p;PixelContent", o->GetHist(),
                                                                       c->GetPad(1)));
            }
            break;

        case kCM_BUTTON:
            if (mp1==MOnlineDisplay::kFreeze)
            {
                TGButton *but = (TGButton*)mp2;
                if (!but->IsDown())
                {
                    but->AllowStayDown(kTRUE);
                    fCamEvent->SetFreezed();
                }
                else
                {
                    but->AllowStayDown(kFALSE);
                    fCamEvent->SetFreezed(kFALSE);
                }
                but->Toggle();
            }
            return kTRUE;
        }
        return kFALSE;
    }
    return kFALSE;
}

MOnlineDump::~MOnlineDump()
{
    if (fDisplay)
        ((MOnlineDisplay*)fDisplay)->SetTask(0);
}

Int_t MOnlineDump::PreProcess(MParList *pList)
{
    fPlist = pList;

    if (gROOT->IsBatch())
    {
        *fLog << err << "We are in batch mode!" << endl;
        return kFALSE;
    }
    if (!fDisplay)
    {
        *fLog << err << "fDisplay not set." << endl;
        return kFALSE;
    }

    MTaskList *tasks = (MTaskList*)pList->FindObject("MTaskList");
    if (!tasks)
    {
        *fLog << err << "MTaskList not found... abort." << endl;
        return kFALSE;
    }

    fFill1 = (MFillH*)tasks->FindObject("MFillCamEvent");
    if (!fFill1)
    {
        *fLog << err << "MFillCamEvent not found... abort." << endl;
        return kFALSE;
    }
    fFill2 = (MFillH*)tasks->FindObject("MFillEvent");
    if (!fFill2)
    {
        *fLog << err << "MFillEvent not found... abort." << endl;
        return kFALSE;
    }
    fFill3 = (MFillH*)tasks->FindObject("MFillTriggerLvl0");
    if (!fFill3)
    {
        *fLog << err << "MFillTriggerLvl0 not found... abort." << endl;
        return kFALSE;
    }

    MHCamEvent *idxhi = (MHCamEvent*)pList->FindObject("MaxIdxHi");
    MHCamEvent *idxlo = (MHCamEvent*)pList->FindObject("MaxIdxLo");
    if (!idxhi || !idxlo)
    {
        *fLog << err << "MaxIdxHi or MaxIdxLo not found... abort." << endl;
        return kFALSE;
    }

    idxhi->GetHistByName("sum")->SetMinimum(0);
    idxlo->GetHistByName("sum")->SetMinimum(0);
    idxhi->GetHistByName("sum")->SetMaximum(15);
    idxlo->GetHistByName("sum")->SetMaximum(15);


    fRawEvtHeader = (MRawEvtHeader*)pList->FindObject("MRawEvtHeader");
    if (!fRawEvtHeader)
    {
        *fLog << err << "MRawEvtHeader not found... abort." << endl;
        return kFALSE;
    }

    fRawRunHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRawRunHeader)
    {
        *fLog << err << "MRawRunHeader not found... abort." << endl;
        return kFALSE;
    }

    fEvtTime = (MTime*)pList->FindObject("MTime");
    if (!fEvtTime)
    {
        *fLog << err << "MTime not found... abort." << endl;
        return kFALSE;
    }

    fRate = (MEventRate*)pList->FindObject("MEventRate");
    if (!fRate)
    {
        *fLog << err << "MEventRate not found... abort." << endl;
        return kFALSE;
    }

    MHEvent *hevent = (MHEvent*)pList->FindObject("MHEvent");
    if (!hevent)
    {
        *fLog << err << "MHEvent not found... abort." << endl;
        return kFALSE;
    }
    fCamEvent = hevent->GetHist();

    fRunNumber = 0xffffffff;
    fEvtNumber = 0;

    SetNoContextMenu((TObject*)fFill1->GetCanvas());
    SetNoContextMenu((TObject*)fFill2->GetCanvas());
    SetNoContextMenu((TObject*)fFill3->GetCanvas());

    return kTRUE;
}

Int_t MOnlineDump::Process()
{
    DisplayTriggerRate();

    if (fDisplay && fRawRunHeader->GetNumEvents())
        fDisplay->SetProgressBarPosition((Float_t)fEvtNumber/fRawRunHeader->GetNumEvents());

    return kTRUE;
}

Int_t MOnlineDump::PostProcess()
{
    if (fDisplay)
    {
        fDisplay->SetProgressBarPosition(1);
        //fDisplay->Reset();
    }

    return kTRUE;
}
