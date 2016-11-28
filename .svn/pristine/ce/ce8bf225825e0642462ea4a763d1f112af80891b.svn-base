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
!   Author(s): Thomas Bretz, 9/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MJOptimize
//
// Class for otimizing the parameters of the supercuts
//
// Minimization Control
// ====================
//
//   To choose the minimization algorithm use:
//         void SetOptimizer(Optimizer_t o);
//
//   Allowed options are:
//        enum Optimizer_t
//        {
//            kMigrad,      // Minimize by the method of Migrad
//            kSimplex,     // Minimize by the method of Simplex
//            kMinimize,    // Migrad + Simplex (if Migrad fails)
//            kMinos,       // Minos error determination
//            kImprove,     // Local minimum search
//            kSeek,        // Minimize by the method of Monte Carlo
//            kNone         // Skip optimization
//        };
//
//   For more details on the methods see TMinuit.
//
//
//   You can change the behaviour of the minimization using
//
//        void SetNumMaxCalls(UInt_t num=0);
//        void SetTolerance(Float_t tol=0);
//
//   While NumMaxCalls is the first, Tolerance the second arguement.
//   For more details start root and type
//
//        gMinuit->mnhelp("command")
//
//   while command can be
//        * MIGRAD
//        * SIMPLEX
//        * MINIMIZE
//        * MINOS
//        * IMPROVE
//        * SEEK
//
//   The default (num==0 and tol==0) should always give you the
//   corresponding defaults used in Minuit.
//
//
// FIXME: Implement changing cut in hadronness...
// FIXME: Show MHSignificance on MStatusDisplay during filling...
// FIXME: Choose step-size percentage as static data membewr
// FIXME: Choose minimization method
//
/////////////////////////////////////////////////////////////////////////////
#include "MJOptimize.h"

#include <TMinuit.h>
#include <TVirtualFitter.h>

#include <TStopwatch.h>

#include <TCanvas.h>

#include <TGraph.h>
#include <TMultiGraph.h>

#include "MHMatrix.h"

// environment
#include "MLog.h"
#include "MLogManip.h"

#include "MDirIter.h"
#include "MStatusDisplay.h"

// eventloop
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

// parameters
#include "MParameters.h"

// tasks
#include "MReadTree.h"
#include "MMatrixLoop.h"
#include "MFillH.h"
#include "MParameterCalc.h"
#include "MContinue.h"

// filters
#include "MFDataPhrase.h"
#include "MFilterList.h"

using namespace std;

//------------------------------------------------------------------------
//
// fcn calculates the function to be minimized (using TMinuit::Migrad)
//
void MJOptimize::fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
    MJOptimize *optim = (MJOptimize*)gMinuit->GetObjectFit();

    // WORKAROUND --- FOR WHAT?
    if (gMinuit->fEpsi<1e-2)
    {
        *optim->fLog << warn << "WARNING - For unknown reasons: fEspi<1e-100... resetting to 0.01." << endl;
        gMinuit->fEpsi = 0.01;
    }

    TMinuit *minuit = gMinuit;
    f = optim->Fcn(TArrayD(TMath::Min(gMinuit->fMaxpar, optim->fParameters.GetSize()), par), minuit);
    gMinuit = minuit;

}

Double_t MJOptimize::Fcn(const TArrayD &par, TMinuit *minuit)
{
    if (fEvtLoop->GetDisplay()!=fDisplay)
        return 0;
    /*
    switch(iflag)
    {
    case 1: // first call
    case 2: // calc derivative
        break;
    case 3:
        // last call
        MStatusDisplay *d = new MStatusDisplay;
        fEvtLoop->SetDisplay(d);
        break;
    }
    */
    MParList *plist = fEvtLoop->GetParList();

    MParameterD   *eval = (MParameterD*)plist->FindObject(fNameMinimizationValue, "MParameterD");
    MParContainer *pars = (MParContainer*)plist->FindObject("MParameters", "MParContainer");

    MRead *read = (MRead*)plist->FindObject("MTaskList")->FindObject("MRead");
    if (read)
        read->Rewind();

    if (fDebug>=0)
    {
        *fLog << inf << "New Set: ";
        for (Int_t i=0; i<fParameters.GetSize(); i++)
            *fLog << par[i] << " ";
        *fLog << endl;
    }

    pars->SetVariables(par);
    eval->SetVal(0);

    const Bool_t isnull = gLog.IsNullOutput();
    if (fDebug<3)
        gLog.SetNullOutput(kTRUE);

    TStopwatch clock;
    clock.Start();
    fEvtLoop->Eventloop(fNumEvents, MEvtLoop::kNoStatistics);
    clock.Stop();

    if (fDebug<3)
        gLog.SetNullOutput(isnull);

    const Double_t f = eval->GetVal();

    if (fDebug>=0)
        *fLog << inf << "Result F=" << f << endl;

    if (fDebug>=1 && minuit)
    {
        Double_t fmin, fedm, errdef;
        Int_t n1, n2, istat;
        minuit->mnstat(fmin, fedm, errdef, n1, n2, istat);
        *fLog << inf << underline << "Minimization Status so far:" << endl;
        *fLog << " Calls:      " << minuit->fNfcn << "  (max=" << gMinuit->fMaxIterations << ")" << endl;
        *fLog << " Parameters: fixed=" << gMinuit->fNpfix << ", free=" << gMinuit->fNpar << endl;
        *fLog << " Func min:   " << fmin << "  (Epsi=" << gMinuit->fEpsi << ", Apsi=" << gMinuit->fApsi << ")" << endl;
        *fLog << " Found edm:  " << fedm << endl;
        *fLog << " ErrDef:     " << errdef << endl;
        *fLog << " Status:     ";

        switch (istat)
        {
        case 0:  *fLog << "n/a" << endl; break;
        case 1:  *fLog << "approximation only, not accurate" << endl; break;
        case 2:  *fLog << "full matrix, but forced positive-definite" << endl; break;
        case 3:  *fLog << "full accurate covariance matrix" << endl; break;
        default: *fLog << "undefined" << endl; break;
        }
    }

    if (fDebug>=1)
    {
        clock.Print();
        fEvtLoop->GetTaskList()->PrintStatistics();
    }

    return f;
}

MJOptimize::MJOptimize() : /*fDebug(-1),*/ fNumEvents(0), fType(kSimplex), fNumMaxCalls(0), fTolerance(0), fTestTrain(0), fNameMinimizationValue("MinimizationValue")
{
    fRules.SetOwner();
    fFilter.SetOwner();

    fNamesOn.SetOwner();
    fNamesOff.SetOwner();
}

//------------------------------------------------------------------------
//
// Add sequences from list to reader
//
Bool_t MJOptimize::AddSequences(MRead &read, TList &list) const
{
    MDirIter files;

    TIter Next(&list);
    MSequence *seq=0;
    while ((seq=(MSequence*)Next()))
    {
        if (!seq->IsValid())
            return kFALSE;
        if (seq->GetRuns(files, MSequence::kRootDat)<=0)
            return kFALSE;
    }

    return read.AddFiles(files)>0;
}

//------------------------------------------------------------------------
//
// Add on-sequences:
//  - fname: sequence file name (with path)
//  - dir:   directory were image files are stored
//
void MJOptimize::AddSequenceOn(const char *fname, const char *dir)
{
    fNamesOn.Add(new MSequence(fname, dir));
}

//------------------------------------------------------------------------
//
// Add off-sequences:
//  - fname: sequence file name (with path)
//  - dir:   directory were image files are stored
//
void MJOptimize::AddSequenceOff(const char *fname, const char *dir)
{
    fNamesOff.Add(new MSequence(fname, dir));
}

//------------------------------------------------------------------------
//
// Empty list of on- and off-sequences
//
void MJOptimize::ResetSequences()
{
    fNamesOn.Delete();
    fNamesOff.Delete();
}


//------------------------------------------------------------------------
//
// Add a filter which can be applied in the optimization (for deatils
// see correspodning Run function) You can use the indices you got by
// AddParameter, eg
//
//   AddFilter("M[0] < 3.2");
//
// if used in optimization you can do
//
//   AddFilter("M[0] < [0]");
//
// for more details, see SetParameter and FixParameter
//
void MJOptimize::AddFilter(const char *rule)
{
    fFilter.Add(new MFDataPhrase(rule));
}

//------------------------------------------------------------------------
//
// Set the fParameters Array accoring to par.
//
void MJOptimize::SetParameters(const TArrayD &par)
{
    fParameters = par;
}

//------------------------------------------------------------------------
//
// Set the number of events processed by the eventloop. (Be carfull,
// if you are doing on-off analysis and you only process the first
// 1000 events which are on-events only the optimization may not work)
//
void MJOptimize::SetNumEvents(UInt_t n)
{
    fNumEvents=n;
}

//------------------------------------------------------------------------
//
// Set a debug level, which tells the optimization how much information
// is displayed about and in the running eventloop.
//
void MJOptimize::SetDebug(UInt_t n)
{
    fDebug=n;
}

//------------------------------------------------------------------------
//
// Set a optimization algorithm to be used. For more information see
// TMinuit.
//
// Available Algorithms are:
//    kMigrad,   // Minimize by the method of Migrad
//    kSimplex,  // Minimize by the method of Simplex
//    kSeek      // Minimize by the method of Monte Carlo
//
void MJOptimize::SetOptimizer(Optimizer_t o)
{
    fType = o;
}

//------------------------------------------------------------------------
//
// If a status didplay is set, search for tab "Optimizer".
// If not found, create it.
// In the tab search for TMultiGraph "Parameters".
// If not found create it.
// If empty create TGraphs.
// Check number of graphs vs. number of parameters.
// return TList with graphs.
//
TList *MJOptimize::GetPlots() const
{
    if (!fDisplay)
        return NULL;

    TCanvas *c=fDisplay->GetCanvas("Optimizer");
    if (!c)
        c = &fDisplay->AddTab("Optimizer");

    TMultiGraph *mg = dynamic_cast<TMultiGraph*>(c->FindObject("Parameters"));
    if (!mg)
        mg = new TMultiGraph("Parameters", "Parameters of optimization");

    TList *l = mg->GetListOfGraphs();
    if (!l)
    {
        const Int_t n = fParameters.GetSize();
        for (int i=0; i<n+1; i++)
        {
            TGraph *g = new TGraph;
            if (i==n)
                g->SetLineColor(kBlue);
            mg->Add(g, "");
            AddPoint(mg->GetListOfGraphs(), i, i==n?1:fParameters[i]);
        }
        mg->SetBit(kCanDelete);
        mg->Draw("al*");

        l = mg->GetListOfGraphs();
    }

    return l->GetSize() == fParameters.GetSize()+1 ? l : NULL;
}

//------------------------------------------------------------------------
//
// Add a point with y=val as last point in idx-th Tgraph of list l.
//
void MJOptimize::AddPoint(TList *l, Int_t idx, Float_t val) const
{
    if (!l)
        return;

    TGraph *gr = (TGraph*)l->At(idx);
    gr->SetPoint(gr->GetN(), gr->GetN(), val);
}

Int_t MJOptimize::Minuit(TMinuit &minuit, const char *cmd) const
{
    Int_t er;
    Double_t tmp[2] = { Double_t(fNumMaxCalls), fTolerance };
    minuit.mnexcm(cmd, tmp, 2, er);

    switch (er)
    {
    case 0:
        *fLog << inf << GetDescriptor() << " TMinuit::mnexcm excuted normally." << endl;
        break;
    case 1:
        *fLog << warn << GetDescriptor() << " TMinuit::mnexcm command is blank... ignored." << endl;
        break;
    case 2:
        *fLog << warn << GetDescriptor() << " TMinuit::mnexcm command-line syntax error... ignored." << endl;
        break;
    case 3:
        *fLog << warn << GetDescriptor() << " TMinuit::mnexcm unknown command... ignored." << endl;
        break;
    case 4:
        *fLog << warn << GetDescriptor() << " TMinuit::mnexcm - Abnormal termination (eg Migrad not converged)" << endl;
        break;
        /*
    case 5:
        *fLog << inf << GetDescriptor() << " TMinuit::mnexcm - Parameters requested." << endl;
        break;
    case 6:
        *fLog << inf << GetDescriptor() << " TMinuit::mnexcm - SET INPUT returned." << endl;
        break;
    case 7:
        *fLog << inf << GetDescriptor() << " TMinuit::mnexcm - SET TITLE returned." << endl;
        break;
    case 8:
        *fLog << inf << GetDescriptor() << " TMinuit::mnexcm - SET COVAR returned." << endl;
        break;
    case 9:
        *fLog << inf << GetDescriptor() << " TMinuit::mnexcm - reserved." << endl;
        break;
    case 10:
        *fLog << inf << GetDescriptor() << " TMinuit::mnexcm - END returned." << endl;
        break;
    case 11:
        *fLog << inf << GetDescriptor() << " TMinuit::mnexcm - EXIT or STOP returned." << endl;
        break;
    case 12:
        *fLog << inf << GetDescriptor() << " TMinuit::mnexcm - RETURN returned." << endl;
        break;*/
    }

    return er;
}

Bool_t MJOptimize::Optimize(MEvtLoop &evtloop)
{
    if (fParameters.GetSize()==0)
    {
        *fLog << err << GetDescriptor() << "::Optimize: ERROR - Sorry, no parameters defined." << endl;
        return kFALSE;
    }

    if (fType==kNone)
        return kTRUE;

    gMinuit = new TMinuit(fParameters.GetSize());

    gMinuit->SetFCN(fcn);
    gMinuit->SetObjectFit(this);
    gMinuit->SetPrintLevel(-1); // Don't print when DefineParameter

    //
    // Set starting values and step sizes for parameters
    //
    for (Int_t i=0; i<fParameters.GetSize(); i++)
    {
        TString name = "par[";
        name += i;
        name += "]";
        Double_t vinit = fParameters[i];
        Double_t step  = fStep[i];

        Double_t limlo = fLimLo[i];
        Double_t limup = fLimUp[i];

        Bool_t rc = gMinuit->DefineParameter(i, name, vinit, step, limlo, limup);
        if (rc)
        {
            *fLog << err << dbginf << "Error in defining parameter #" << i << endl;
            return kFALSE;
        }

        if (step==0)
            gMinuit->FixParameter(i);
    }

    gMinuit->SetPrintLevel(1); // Switch on pritning again
    gMinuit->mnprin(1,0);      // Print all parameters

    fEvtLoop = &evtloop;

    TList *g=GetPlots();

    // Now ready for minimization step:
    TStopwatch clock;
    clock.Start();
    switch (fType)
    {
    case kSimplex:
        Simplex(*gMinuit);
        break;
    case kMigrad:
        Migrad(*gMinuit);
        break;
    case kMinimize:
        Minimize(*gMinuit);
        break;
    case kMinos:
        Minos(*gMinuit);
        break;
    case kImprove:
        Improve(*gMinuit);
        break;
    case kSeek:
        Seek(*gMinuit);
        break;
    case kNone: // Should never happen
        return kFALSE;
    }
    clock.Stop();
    clock.Print();

    if (evtloop.GetDisplay()!=fDisplay)
    {
        *fLog << inf << "Optimization aborted by user." << endl;
        fDisplay = 0;
        return kFALSE;
    }

    *fLog << inf << "Resulting Chisq: " << gMinuit->fAmin << endl;

    //
    // Update values of fA, fB:
    //
    for (Int_t i=0; i<fParameters.GetSize(); i++)
    {
        Double_t x1, x2;
        gMinuit->GetParameter(i,x1,x2);
        fParameters[i] = x1;
        cout << i << ": " << fParameters[i] << endl;

        AddPoint(g, i, x1);
    }
    AddPoint(g, fParameters.GetSize(), gMinuit->fAmin);

    delete gMinuit;

    return kTRUE;
}

//------------------------------------------------------------------------
//
// Optimize allows to use the optimizing by an eventloop based on
// some requirements.
//
// 1) The tasklist to be executed must have the name MTaskList and
//    be an entry in the parameterlist.
//
// 2) The reading task (MReadMarsFile, MMatrixLoop) must have the name
//    "MRead". If it derives from MRead Rewind() must be implemented,
//    otherwise it must start reading from scratch each time its
//    PreProcess is called.
//
// 3) The parameters to be optimized must be accessed through (currently)
//    a single parameter container (MParContainer) called MParameters.
//    The parameters are set through SetVariables.
//
// 4) The result of a single function call for minimization (eg. chisquare)
//    must be available after the eventloop in a container of type
//    MParameterD with the name "MinimizationResult".
//
// 5) The parameters to start with must have been set using
//    MJOptimize::SetParameter or MJOptimize::SetParameters and
//    MJOptimize::FixParameter
//
// The behaviour of the optimization can be changed using:
//  void SetNumEvents(UInt_t n);
//  void SetDebug(UInt_t n);
//  void SetOptimizer(Optimizer_t o);
//
// After optimization the resulting parameters are set and another eventloop
// with a MStatusDisplay is set is called. The resulting parameters can be
// accessed using: GetParameters()
//
// To be fixed:
//  - MStatusDisplay should show status while optimization is running
//  - write result into MStatusDisplay
//  - save result
//
Bool_t MJOptimize::Optimize(MParList &parlist)
{
    // Checks to make sure, that fcn doesn't crash
    if (!parlist.FindCreateObj("MParameterD", fNameMinimizationValue))
        return kFALSE;

    if (!parlist.FindObject("MParameters", "MParContainer"))
    {
        *fLog << err << "MParameters [MParContainer] not found... abort." << endl;
        return kFALSE;
    }

    MTaskList *tlist = (MTaskList*)parlist.FindObject("MTaskList");
    if (!tlist)
    {
        *fLog << err << "MTaskList not found... abort." << endl;
        return kFALSE;
    }
    tlist->SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    MMatrixLoop *loop = dynamic_cast<MMatrixLoop*>(parlist.FindTask("MRead"));

    TString txt("Starting ");
    switch (fType)
    {
    case kMigrad:    txt += "Migrad";    break;
    case kMinimize:  txt += "Minimize";  break;
    case kMinos:     txt += "Minos";     break;
    case kImprove:   txt += "Improve";   break;
    case kSimplex:   txt += "Simplex";   break;
    case kSeek:      txt += "Seek";      break;
    case kNone:      txt += "no";        break;
    }
    txt += " optimization";

    fLog->Separator(txt);

    // Setup eventloop
    MEvtLoop evtloop(fTitle);
    evtloop.SetParList(&parlist);
    evtloop.SetDisplay(fDisplay); // set display for evtloop and all childs
    parlist.SetDisplay(0);        // reset display for all contents of parlist and tasklist
    evtloop.SetPrivateDisplay();  // prevent display from being cascaded again in PreProcess

    *fLog << inf << "Number of Parameters: " << fParameters.GetSize() << endl;

    // In case the reader is the matrix loop and testrain is enabled
    // switch on even mode...
    if (loop && TMath::Abs(fTestTrain)>0)
        loop->SetOperationMode(fTestTrain>0?MMatrixLoop::kEven:MMatrixLoop::kOdd);

    if (!Optimize(evtloop))
        return kFALSE;

    gMinuit = 0;

    // Test with training data
    fEvtLoop->SetDisplay(fDisplay);
    if (!Fcn(fParameters))
        return kFALSE;

    // In case the reader is the matrix loop and testrain is enabled
    // switch on odd mode...
    if (!loop || fTestTrain==0)
        return kTRUE;

    // Test with test-data
    loop->SetOperationMode(fTestTrain<0?MMatrixLoop::kEven:MMatrixLoop::kOdd);

    // Done already in Fcn
    // list.SetVariables(fParameters);
    return Fcn(fParameters);
}

void MJOptimize::AddRulesToMatrix(MHMatrix &m) const
{
    TIter Next1(&fRules);
    TObject *o1=0;
    while ((o1=Next1()))
        m.AddColumn(o1->GetName());
}

//------------------------------------------------------------------------
//
// Fill matrix m by using read. Use rules as a filter if userules.
//
Bool_t MJOptimize::FillMatrix(MReadTree &read, MParList &parlist, Bool_t userules)
{
    MHMatrix *m = (MHMatrix*)parlist.FindObject("M", "MHMatrix");
    if (!m)
    {
        *fLog << err << "MJOptimize::FillMatrix - ERROR: M [MHMatrix] not found in parlist... abort." << endl;
        return kFALSE;
    }

    m->Print("cols");

    //    MGeomCamMagic cam;
    //    parlist.AddToList(&cam);

    // Setup filter and apply filter to filling of matrix
    MFilterList list;
    if (!list.AddToList(fPreCuts))
        *fLog << err << "ERROR - Calling MFilterList::AddToList for fPreCuts failed!" << endl;
    if (userules)
        SetupFilters(list);
    list.SetName("PreCuts");  // reset Name      set by SetupFilters
    list.SetInverted(kTRUE);  // reset inversion set by SetupFilters

    MContinue cont(&list);

    // Setup filling of matrix
    MFillH fillh(m);

    // Setup tasklist
    MTaskList tlist;
    parlist.Replace(&tlist);

    // Create task list
    tlist.AddToList(&read);        // read data
    tlist.AddToList(fPreTasks);    // execute pre-filter tasks
    tlist.AddToList(&cont);        // execute filters
    tlist.AddToList(fPostTasks);   // execute post-filter tasks
    tlist.AddToList(&fillh);       // Fill matrix

    tlist.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    MEvtLoop fillloop(fTitle);
    fillloop.SetParList(&parlist);
    fillloop.SetDisplay(fDisplay);
    if (!fillloop.Eventloop(fNumEvents))
    {
        *fLog << err << "Filling matrix failed..." << endl;
        return kFALSE;
    }

    *fLog << inf << "Read events from file '" << read.GetFileName() << "'" << endl;

    if (fillloop.GetDisplay()!=fDisplay)
    {
        fDisplay = 0;
        *fLog << inf << "Optimization aborted by user." << endl;
        return kFALSE;
    }

    m->Print("size");

    return kTRUE;
}

//------------------------------------------------------------------------
//
// Adds all filters to MFilterList
//
void MJOptimize::SetupFilters(MFilterList &list, MFilter *filter) const
{
    list.SetName("MParameters");
    list.SetInverted();

    if (filter)
    {
        if (fFilter.GetSize()>0)
        {
            *fLog << inf;
            *fLog << "INFORMATION - You are using an  external filter and internal filters." << endl;
            *fLog << "              Please make sure that all parameters '[i]' are starting" << endl;
            *fLog << "              behind the number of parameters of the external filter." << endl;
        }
        list.AddToList(filter);
    }

    if (!list.AddToList(fFilter))
        *fLog << err << "ERROR - Calling MFilterList::AddToList fFilter failed!" << endl;

    *fLog << inf << "Filter: ";
    list.Print();
    *fLog << endl;
}
