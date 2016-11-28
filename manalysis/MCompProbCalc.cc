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
!   Author(s): Abelardo Moralejo <mailto:moralejo@pd.infn.it>
!   Author(s): Thomas Bretz, 5/2002 <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCompProbCalc
//
// FIXME: Use A huge matrix to evaluate variable bins instead of a
// histogram
//
////////////////////////////////////////////////////////////////////////////
#include "MCompProbCalc.h"

#include <TH1.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MDataPhrase.h"

#include "MHCompProb.h"
#include "MParameters.h"

ClassImp(MCompProbCalc);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor
//
MCompProbCalc::MCompProbCalc(const char *name, const char *title)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MCompProbCalc";
    fTitle = title ? title : "Composite Probabilities Loop 1/2";

    fData = new TList;
    fData->SetOwner();
}

// --------------------------------------------------------------------------
//
// Destructor
//
MCompProbCalc::~MCompProbCalc()
{
    delete fData;
}

// --------------------------------------------------------------------------
//
// Needs:
//  - MHCompProb
//  - all data values which were used to build the MHCompProb
//
Int_t MCompProbCalc::PreProcess(MParList *plist)
{
    MHCompProb *p = (MHCompProb*)plist->FindObject("MHCompProb");
    if (!p)
    {
        *fLog << err << dbginf << "MHCompProb not found... aborting." << endl;
        return kFALSE;
    }

    fHistVar = p->GetHistVar();
    if (fHistVar->GetSize()==0)
    {
        *fLog << err << dbginf << "No Entries in MHCompProb::fHistVar... aborting." << endl;
        return kFALSE;
    }

    const TList *rules = p->GetRules();
    if (rules->GetSize()==0)
    {
        *fLog << err << dbginf << "No Entries in MHCompProb::fRules... aborting." << endl;
        return kFALSE;
    }

    if (fHistVar->GetSize() != rules->GetSize())
    {
        *fLog << err << dbginf << "Number of rules doesn't match number of var.bin histograms.. aborting." << endl;
        return kFALSE;
    }

    TIter Next(rules);
    TObject *data=NULL;
    while ((data=Next()))
    {
        MDataPhrase *chain = new MDataPhrase(data->GetName());
        if (!chain->PreProcess(plist))
        {
            delete chain;
            return kFALSE;
        }
        fData->Add(chain);
    }

    fHadronness = (MParameterD*)plist->FindCreateObj("MParameterD", "MHadronness");
    if (!fHadronness)
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calculate the hadroness (composite propability)
//  - For each data member search the bin corresponding to the data value.
//  - The value describes with which probability this value corresponds to
//    a hadron
//  - For all data members multiply the probabilities.
//  - For normalization take the n-th root of the result.
//  - This is the hadroness stored in the Hadronness container
//
Int_t MCompProbCalc::Process()
{
    Double_t p = 1;

    TIter NextH(fHistVar);
    TIter NextD(fData);

    TH1D *hist=NULL;

    Int_t n = 0;

    while ((hist=(TH1D*)NextH()))
    {
        MData *data = (MData*)NextD();

        Int_t ibin = hist->FindBin(data->GetValue());

        p *= hist->GetBinContent(ibin) / hist->GetEntries();
        n++;
    }

    fHadronness->SetVal(pow(p, 1./n));
    return kTRUE;
}

