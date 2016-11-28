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
!   Author(s): Thomas Bretz 11/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2005-2010
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MJOptimizeBase
//
// Base class for classes training a random forest
//
// The order when reading a file is:
//   1) Execution of PreTasks
//   2) Execution of PreCuts, TestCuts, TrainCuts
//   3) Selector               (calculated from number of requested events)
//   4) Splitter               (if sample is split automatically in test/train)
//   5) PostTasks              (set by user)
//
// The split into Pre- and PostTasks is done for speed reason. So, if
// you calculate a vlue which is not needed for your PreCuts, you can
// calculate it afterwards, which will speed up execution.
//
// void AddPreTask(MTask *t)
//    Add a task which is executed just after reading the events from the file
//    this can be any kind of task or even a task list. You can use dedicated
//    tasks performing some analysis, but also cuts (e.g. MContinue) or a
//    general task to evaluate new parameters (e.g. MParameterCalc)
//
// void AddPreTask(const char *rule, const char *name="MWeight")
//    This is a short cut to AddPreTask(MTask*) which will add a
//    MParameterCalc to the list evaluating the given rule and storing the
//    result in a MParameterD with the given name. For the default "MWeight",
//    for example, the value can later be accessed by MWeight.fVal.
//
// The same functions are available as
//    void AddPostTask(MTask *t)
//    void AddPostTask(const char *rule, const char *name="MWeight")
//
// It is advicable to use these function if the task is not needed for your
// event selection. The event selection (by number and by PreCuts) is done
// in between PreTasks and PostTasks, i.e. a task which is not needed for the
// event selection but needed later is only executed as often as necessary.
//
// The event selection (apart from the number of target events) can be setup
// by the user. Therefore two functions are available
//    void AddPreCut(const char *rule)
//    void AddPreCut(MFilter *f)
// Both add cuts which are executed after the PreTasks but before the
// PostTasks. They are executed in both training and testing. To execute cuts
// only in training or testing use
//    void AddTrainCut(const char *rule)
//    void AddTrainCut(MFilter *f)
// or
//    void AddTestCut(const char *rule)
//    void AddTestCut(MFilter *f)
//
// To weight the events of your train- and test sample the following functions
// can be used
//    void SetWeights(const char *rule)
//    void SetWeights(MTask *t)
// This either adds a MParameterCalc to the list of post tasks or a MTask,
// which is supposed to fill a MParameterD named "MWeight".
//
// Last but not least a function if available to add additional tasks to the
// end of the task list for testing. This can for example be used to add
// the filling and display of an addition histogram (e.g. add a MFillH)
// or to do some calculation and write out the result. Basically everything
// can be done in this way.
//    void AddTestTask(MTask *t)
//    void AddTestTask(const char *rule, const char *name="MWeight")
//
//
// REMARK: Note that the actual behavior might still vary a bit
//         depeding of the implementation in the derived class.
//
/////////////////////////////////////////////////////////////////////////////
#include "MJOptimizeBase.h"

#include <TFile.h>
#include <TRandom3.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MFDataPhrase.h"
#include "MParameterCalc.h"

#include "MStatusDisplay.h"

ClassImp(MJOptimizeBase);

using namespace std;

//------------------------------------------------------------------------
//
MJOptimizeBase::MJOptimizeBase() : fDebug(-1), fEnableWeights(kFALSE)
{
    if (gRandom && gRandom->IsA()==TRandom::Class())
        *fLog << warn << "WARNING - gRandom is a TRandom instance. It is highly adviced to use another random generator!" << endl;
}

//------------------------------------------------------------------------
//
// Add a cut which is used to fill the matrix, eg "MMcEvt.fPartId<1.5"
// (The rule is applied, not inverted: The matrix is filled with
// the events fullfilling the condition)
//
void MJOptimizeBase::AddCut(TList &l, const char *rule)
{
    MFilter *f = new MFDataPhrase(rule);
    f->SetBit(kCanDelete); //FIXME!!!! Why does not any other list delete it???
    Add(l, f);
}

//------------------------------------------------------------------------
//
// Add an additional parameter (MParameterCalc), eg "0.5", "MWeight"
// The default container name is "MWeight"
//
void MJOptimizeBase::AddPar(TList &l, const char *rule, const char *pname)
{
    TString tname(pname);
    tname += "Calc";

    MParameterCalc *par = new MParameterCalc(rule, tname);
    par->SetNameParameter(pname);
//    par->SetBit(kCanDelete);  //FIXME!!!! MTaskList is deleting it
    Add(l, par);
}

//------------------------------------------------------------------------
//
// Add a task/cut which is used to fill the matrix. If kCanDelete is set
// MJOptimize takes the ownership.
//
void MJOptimizeBase::Add(TList &l, MTask *f)
{
    l.Add(f);
}

//------------------------------------------------------------------------
//
// Add a parameter to the list of parameters. The index in the list is
// returned.
//
//   Int_t idx = AddParameter("log10(MHillas.fSize)");
//
// Indices are starting with 0.
//
Int_t MJOptimizeBase::AddParameter(const char *rule)
{
    fRules.Add(new TNamed(rule, ""));
    return fRules.GetSize()-1;
}
