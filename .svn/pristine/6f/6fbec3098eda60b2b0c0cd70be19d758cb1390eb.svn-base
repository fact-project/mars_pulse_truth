#include <TApplication.h>

#include <TThread.h>
#include <TCanvas.h>
#include <TMethod.h>          // GetMenuItems

#include <TGButton.h>

#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MLogManip.h"

#include "MRawSocketRead.h"
#include "MGeomApply.h"
#include "MCerPhotAnal2.h"
#include "MFillH.h"
#include "MFRealTimePeriod.h"
#include "MHTriggerLvl0.h"
#include "MHCamera.h"         // ::Class(), SetFreezed
#include "MHCamEvent.h"       // MHCamEvent
#include "MHEvent.h"          // MHEvent::GetHist()

#include "MOnlineDump.h"
#include "MOnlineDisplay.h"

#include "MImgCleanStd.h"
#include "MHillasCalc.h"
#include "MHillasSrcCalc.h"
#include "MEventRateCalc.h"

#include "MArgs.h"

// --------------------------------------------------------------------

#include "MTime.h"
#include "MRawEvtHeader.h"       // MRawEvtHeader
#include "MRawRunHeader.h"       // MRawRunHeader

using namespace std;

void StartUpMessage()
{
    gLog << all << endl;

    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << "==================================================" << endl;
    gLog << "                    MONA V" << MARSVER << "                      " << endl;
    gLog << "              Magic Online Analysis               " << endl;
    gLog << "            Compiled on <" << __DATE__ << ">"       << endl;
    gLog << "               Using ROOT v" << ROOTVER             << endl;
    gLog << "==================================================" << endl;
    gLog << endl;
}

void Remove(TMethod *m, const char *name)
{
    if (TString(m->GetName()).BeginsWith(name))
        m->SetMenuItem(kMenuNoMenu);
}

void ChangeContextMenus()
{
    TList l;
    MHCamera::Class()->GetMenuItems(&l);
    TIter Next(&l);
    TMethod *m=NULL;
    while ((m=(TMethod*)Next()))
    {
        Remove(m, "DrawClone");
        Remove(m, "SetName");
        Remove(m, "Delete");
        Remove(m, "DrawClass");
        Remove(m, "Dump");
        Remove(m, "Inspect");
        Remove(m, "Smooth");
        Remove(m, "Fit");
        Remove(m, "Divide");
        Remove(m, "Add");
        Remove(m, "Multiply");
        Remove(m, "Delete");
        Remove(m, "SetLineAttributes");
        Remove(m, "SetFillAttributes");
        Remove(m, "SetMarkerAttributes");
        Remove(m, "SetDrawOption");
    }
}

#include "MPedPhotCam.h"
Bool_t Loop(MOnlineDisplay *display, Int_t port)
{
    display->Reset();

    //
    // create the tasks which should be executed and add them to the list
    // in the case you don't need parameter containers, all of them can
    // be created by MRawFileRead::PreProcess
    //
    MRawSocketRead reader;
    reader.SetPort(port);

    //
    // create a (empty) list of parameters which can be used by the tasks
    // and an (empty) list of tasks which should be executed
    //
    MParList plist;

    MTaskList tasks;
    plist.AddToList(&tasks);
    tasks.AddToList(&reader);

    MPedPhotCam pcam;
    plist.AddToList(&pcam);

    MGeomApply geomapl;
    MCerPhotAnal2 ncalc;
    tasks.AddToList(&geomapl);
    tasks.AddToList(&ncalc);

    MEventRateCalc tcalc;
    tcalc.SetNumEvents(100);
    tasks.AddToList(&tcalc);

    MFillH fill1("MHEvent",    "MCerPhotEvt", "MFillEvent");
    MFillH fill2("MHCamEvent", "MCerPhotEvt", "MFillCamEvent");

    MFRealTimePeriod filter;
    fill1.SetFilter(&filter);

    MHTriggerLvl0 trigmap(128, "Above 128");
    MFillH fill3(&trigmap, "MRawEvtData", "MFillTriggerLvl0");

    tasks.AddToList(&filter);

    tasks.AddToList(&fill1);
    tasks.AddToList(&fill2);
    tasks.AddToList(&fill3);

    MHCamEvent hist("PedestalRms");
    hist.SetType(1);
    plist.AddToList(&hist);

    // -------------------------------------------

    MHCamEvent maxhi("MaxIdxHi", "Index of slice with maximum content (hi-gain)");
    MHCamEvent maxlo("MaxIdxLo", "Index of slice with maximum content (lo-gain)");
    maxhi.SetType(3);
    maxlo.SetType(4);
    plist.AddToList(&maxhi);
    plist.AddToList(&maxlo);

    // -------------------------------------------

    MFillH hfilla("MaxIdxHi", "MRawEvtData", "FillMaxIdxHi");
    MFillH hfillb("MaxIdxLo", "MRawEvtData", "FillMaxIdxLo");
    MFillH hfillc("Pedestals [MHCamEvent]", "MPedPhotCam", "FillPedestal");
    MFillH hfilld("PedestalRms", "MPedPhotCam", "FillPedestalRms");
    tasks.AddToList(&hfilla);
    tasks.AddToList(&hfillb);
    tasks.AddToList(&hfillc);
    tasks.AddToList(&hfilld);

    MOnlineDump dump;
    dump.SetFilter(&filter);
    display->SetTask(&dump);
    tasks.AddToList(&dump);


    MImgCleanStd      clean;
    MHillasCalc       hcalc;
    MHillasSrcCalc    scalc; // !!Preliminary!! Will be removed later!
    MFillH hfill3("MHHillas",   "MHillas",    "MFillHillas");
    MFillH hfill4("MHHillasSrc","MHillasSrc", "MFillHillasSrc");
    tasks.AddToList(&clean);
    tasks.AddToList(&hcalc);
    tasks.AddToList(&scalc);
    tasks.AddToList(&hfill3);
    tasks.AddToList(&hfill4);

    MEvtLoop magic;
    magic.SetParList(&plist);

    magic.SetDisplay(display);
    magic.SetProgressBar((TGProgressBar*)NULL);

    if (!magic.Eventloop())
    {
        gLog << err << "Mona Eventloop error..." << endl;
        return kFALSE;
    }

    tasks.PrintStatistics();
    return kTRUE;
}

//
// By defining the function return type 'void' instead of
// 'void*' we tell TThread to create a detached thread,
// but calling Exit() in a detached thread results in a crash
// when the TGApplication is terminated.
//
void *thread(void *ptr)
{
    const Int_t port = *((Int_t**)ptr)[0];
    MOnlineDisplay &d = *((MOnlineDisplay**)ptr)[1];

    //
    // If a loop is stopped reinitialize a new loop until the
    // user requested to exit the program.
    //
    while (d.CheckStatus()!=MStatusDisplay::kFileExit)
        Loop(&d, port);


    gLog << dbg << "Loop stopped... exit thread()" << endl;

    return 0;
}

/*
#if ROOT_VERSION_CODE < ROOT_VERSION(3,10,02)
#include <TRootGuiFactory.h>
#include <TPluginManager.h>
void InitGuiFactory()
{
   if (gROOT->IsBatch())
       gROOT->SetBatch(kFALSE);

    //
    // Must be loaded by hand, because it is not loaded by TGApplication.
    // We could also use TApplication instead, but TApplication doesn't
    // deal with the DISPLAY variable in a convient way.
    //
    TPluginHandler *h;
    if ((h = gROOT->GetPluginManager()->FindHandler("TGuiFactory", "root")))
    {
        if (h->LoadPlugin() == -1)
            return;
        gGuiFactory = (TGuiFactory*)h->ExecPlugin(0);
    }
}
#endif
*/

static void Usage()
{
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << "   mona [-?] [-h] [-a0] [-h] [-pn] [-vn]" << endl << endl;
    gLog << "     -a0: Do not use Ansii codes." << endl;
    gLog << "     -pn: TCP/IP port n [default=7000]" << endl;
    gLog << "     -vn: Verbosity level n [default=2]" << endl << endl;
    gLog << "     -?/-h: This help" << endl << endl;
}


int main(int argc, char **argv)
{
    StartUpMessage();

    //
    // Evaluate arguments
    //
    MArgs arg(argc, argv);

    if (arg.HasOption("-?") || arg.HasOption("-h"))
    {
        Usage();
        return -1;
    }

    //
    // Set verbosity to highest level.
    //
    gLog.SetDebugLevel(arg.HasOption("-v") ? arg.GetIntAndRemove("-v") : 2);

    if (arg.HasOption("-a") && arg.GetIntAndRemove("-a")==0)
        gLog.SetNoColors();

    const Int_t port = arg.HasOption("-p") ? arg.GetIntAndRemove("-p") : 7000;

    //
    // check for the right usage of the program
    //
    if (arg.GetNumArguments()>0)
    {
        Usage();
        return -1;
    }

    TApplication app("Mona", &argc, argv);
    if (gROOT->IsBatch() || !gClient)
    {
        gLog << "Bombing... maybe your DISPLAY variable is not set correctly!" << endl;
        return 1;
    }

    /*
    TGApplication app("Mona", &argc, argv);

#if ROOT_VERSION_CODE < ROOT_VERSION(3,10,02)
    InitGuiFactory();
#endif
    */

    ChangeContextMenus();

    //
    // Starting MStatusDisplay in the thread results in hangs
    // if the thread is terminated (by return)
    //
    gLog << dbg << "Starting Display..." << flush;
    MOnlineDisplay d;
    d.SetWindowName("Magic ONline Analysis");
    d.SetIconName("Mona");

    d.SetBit(MStatusDisplay::kExitLoopOnExit);
    gLog << "done." << endl;

    // gDebug=1;

    gLog << dbg << "Starting Thread..." << flush;
    const void *ptr[2] = { &port, &d };
    TThread t(thread, ptr);
    t.Run();
    gLog << "done." << endl;

    gLog << dbg << "Starting System loop... " << endl;
    app.Run(kTRUE);
    gLog << dbg << "System loop stopped." << endl;

    d.UnmapWindow();

    gLog << dbg << "Waiting for termination of thread... (be patient)" << endl;
    while (t.GetState()!=TThread::kCanceledState)
        gSystem->ProcessEvents();

    gLog << dbg << "Thread terminated... joining." << endl;

    TThread::Join(t.GetId()); //- seems that it is implicitly done... why?

    gLog << dbg << "Exit MONA." << endl;

    return 0;
}
