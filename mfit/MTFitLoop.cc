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
!   Author(s): Thomas Bretz, 3/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MTFitLoop
//
// This class optimized parameters which are calculated in an eventloop.
// For this it is unimportant whether the loop reads from a file a
// matrix or from somewhere else...
//
// The parameters which are optimized must be stored in a MParContainer
// which overwrites SetVariables(). Eg. a MDataPhrase or MF overwrites
// SetVariables(). In a MF all arguments given as [0], [1] are
// set by SetVariables (in MDataValue).
//  eg: In you loop you have a cut like this:
//      MF filter("MHillas.fWidth<[0]");
//      filter.SetName("MyParameters");
//
//  Now for each time the eventloop is executed
//  (MEvtLoop::Eventloop(fNumEvents)) the parameters from TMinuit are
//  passed to MF::SetVariables and changed.
//
//
/////////////////////////////////////////////////////////////////////////////
#include "MTFitLoop.h"

#include <TArrayD.h>
#include <TMinuit.h>
#include <TStopwatch.h>

#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MParameters.h"

#include "MRead.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MTFitLoop);

using namespace std;

//------------------------------------------------------------------------
//
// fcn calculates the function to be minimized (using TMinuit::Migrad)
//
void MTFitLoop::fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
    MTFitLoop *optim = (MTFitLoop*)gMinuit->GetObjectFit();

    TMinuit *minuit = gMinuit;
    f = optim->Fcn(npar, gin, par, iflag);
    gMinuit = minuit;

}
Double_t MTFitLoop::Fcn(Int_t &npar, Double_t *gin, Double_t *par, Int_t iflag)
{
    MParList *plist = fEvtLoop->GetParList();

    MParameterD   *eval = (MParameterD*)  plist->FindCreateObj("MParameterD", fFitParameter);
    MParContainer *pars = (MParContainer*)plist->FindObject(fParametersName);

    MRead *read = (MRead*)plist->FindObject("MTaskList")->FindObject("MRead");
    if (read)
        read->Rewind();

    if (fDebug>=1)
    {
        Double_t fmin, fedm, errdef;
        Int_t n1, n2, istat;
        gMinuit->mnstat(fmin, fedm, errdef, n1, n2, istat);
        *fLog << inf << underline << "Minimization Status so far:" << endl;
        *fLog << " Calls:     " << gMinuit->fNfcn << endl;
        *fLog << " Func min:  " << fmin << endl;
        *fLog << " Found edm: " << fedm << endl;
        *fLog << " ErrDef:    " << errdef << endl;
        *fLog << " Status:    ";
        switch (istat)
        {
        case 0:  *fLog << "n/a" << endl; break;
        case 1:  *fLog << "approximation only, not accurate" << endl; break;
        case 2:  *fLog << "full matrix, but forced positive-definite" << endl; break;
        case 3:  *fLog << "full accurate covariance matrix" << endl; break;
        default: *fLog << "undefined" << endl; break;
        }
    }

    if (fDebug>=0)
    {
        *fLog << inf << "Set(" << gMinuit->fMaxpar << "): ";
        for (Int_t i=0; i<gMinuit->fMaxpar; i++)
            *fLog << par[i] << " ";
        *fLog << endl;
    }
 
    pars->SetVariables(TArrayD(gMinuit->fMaxpar, par));

    if (fDebug<3)
        gLog.SetNullOutput(kTRUE);
    fEvtLoop->Eventloop(fNumEvents);
    if (fDebug<3)
        gLog.SetNullOutput(kFALSE);

    const Double_t f = eval->GetVal();

    if (fDebug>=0)
        *fLog << inf << "F=" << f << endl;

    if (fDebug>=1)
        fEvtLoop->GetTaskList()->PrintStatistics();

    return f;
}

MTFitLoop::MTFitLoop(Int_t num) : fNum(num), fMaxIterations(1000)
{
    fDebug     = -1;
    fNumEvents = -1;
}

void MTFitLoop::Optimize(MEvtLoop &loop, TArrayD &pars)
{
    *fLog << inf << "Event loop was setup" << endl;
    MParList *parlist = loop.GetParList();
    if (!parlist)
        return;

//    MParContainer *pars = (MParContainer*)parlist->FindObject(fParametersName);
//    if (!pars)
//        return;

    fEvtLoop = &loop;

//    MParContainer &parameters = *pars;

    TMinuit *minsave = gMinuit;

    gMinuit = new TMinuit(pars.GetSize());
    gMinuit->SetPrintLevel(-1);
    gMinuit->SetMaxIterations(fMaxIterations);

    gMinuit->SetFCN(fcn);
    gMinuit->SetObjectFit(this);

     // For chisq fits call this // seems to be something like %)
     //
     // The default tolerance is 0.1, and the minimization will stop");
     // when the estimated vertical distance to the minimum (EDM) is");
     // less than 0.001*[tolerance]*UP (see [SET ERRordef]).");
     //
    if (gMinuit->SetErrorDef(1))
    {
        *fLog << err << dbginf << "SetErrorDef failed." << endl;
        return;
    }

    //
    // Set starting values and step sizes for parameters
    //
    for (Int_t i=0; i<pars.GetSize(); i++)
    {
        TString name = "par[";
        name += i;
        name += "]";
        Double_t vinit = pars[i];
        Double_t step  = fabs(pars[i]/3);

        Double_t limlo = 0; // limlo=limup=0: no limits
        Double_t limup = 2*vinit;

        Bool_t rc = gMinuit->DefineParameter(i, name, vinit, step, 0, limup);
        if (!rc)
            continue;

        *fLog << err << dbginf << "Error in defining parameter #" << i << endl;
        return;
    }

    for (int i=0; i<pars.GetSize() && i<fFixedParams.GetSize(); i++)
        if (fFixedParams[i]!=0)
            gMinuit->FixParameter(i);

    // Now ready for minimization step:

    TStopwatch clock;
    clock.Start();
    const Bool_t rc = gMinuit->Migrad();
    clock.Stop();
    clock.Print();

    if (rc)
    {
        *fLog << err << dbginf << "Migrad failed." << endl;
        return;
    }

    *fLog << inf << "Resulting Chisq: " << gMinuit->fAmin << endl;

    //
    // Update values of fA, fB:
    //
    for (Int_t i=0; i<pars.GetSize(); i++)
    {
        Double_t x1, x2;
        gMinuit->GetParameter(i,x1,x2);
        pars[i] = x1;
        cout << i << ": " << pars[i] << endl;
    }

    //list.SetVariables(pars);

    gMinuit = minsave;
}
