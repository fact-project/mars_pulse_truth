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
!   Author(s): Thomas Bretz, 05/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MFiltercutsCalc
//
// This class sets the hadronness to 0.25 if the evaluttion of the cuts
// is true, otherwise 0.75.
//
// The cuts can be set by AddCut (see there for mor information)
//
// All single cuts are linked with a logical and ('&&')
//
/////////////////////////////////////////////////////////////////////////////
#include "MFiltercutsCalc.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MFDataPhrase.h"
#include "MParList.h"
#include "MFilterList.h"
#include "MHadronness.h"

ClassImp(MFiltercutsCalc);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Creates the filter list.
//
MFiltercutsCalc::MFiltercutsCalc(const char *name, const char *title)
    : fHadronnessName("MHadronness")
{
    fName  = name  ? name  : "MFiltercutsCalc";
    fTitle = title ? title : "Class to evaluate the Supercuts";

    fList = new MFilterList;
    fList->SetOwner();
    fList->SetBit(kMustCleanup);

    gROOT->GetListOfCleanups()->Add(fList);
}

// --------------------------------------------------------------------------
//
// Delete the filter list.
//
MFiltercutsCalc::~MFiltercutsCalc()
{
    delete fList;
}

// --------------------------------------------------------------------------
//
// Add the filter to the list. Set the rule as its name.
//
void MFiltercutsCalc::AddToList(MFilter *f)
{
    f->SetName(f->GetRule());
    f->SetBit(kMustCleanup);
    fList->AddToList(f);
}

// --------------------------------------------------------------------------
//
// Print the rule of the list.
//
void MFiltercutsCalc::Print(Option_t *opt) const
{
    *fLog << all << underline << GetDescriptor() << ":" << endl;
    fList->Print();
    *fLog << endl;
}

/*
// --------------------------------------------------------------------------
//
// There is MUCH space for speed improvement!
// Add MFDataPhrase(lo<name && name<up) to the filter list (&&),
// for more details see MFDataPhrase::MFDataPhrase(), too.
//
void MFiltercutsCalc::AddCut(const char *name, Double_t lo, Double_t up)
{
    AddToList(new MFDataPhrase(Form("%s>%.15f", name, lo));
    AddToList(new MFDataPhrase(name, '<', up));
}

// --------------------------------------------------------------------------
//
// There is MUCH space for speed improvement!
// Add MFDataPhrase(fabs(name)<val) to the filter list (&&),
// for more details see MFDataPhrase::MFDataPhrase(), too.
//
void MFiltercutsCalc::AddCut(const char *name, Double_t val)
{
    AddToList(new MFDataChain(Form("fabs(%s)", name), '<', val));
}
*/

// --------------------------------------------------------------------------
//
// There is MUCH space for speed improvement!
// Add 'cut' as MF(cut) to the filter list (&&),
// for more details see MFDataPhrase::MFDataPhrase(), too.
//
void MFiltercutsCalc::AddCut(const char *cut)
{
    AddToList(new MFDataPhrase(cut));
}

// --------------------------------------------------------------------------
//
// There is MUCH space for speed improvement!
// Add cut to filter list (&&), for more details see MFDataPhrase::MFDataPhrase(), too.
//
void MFiltercutsCalc::AddCut(MFilter *f)
{
    AddToList(f);
}

// --------------------------------------------------------------------------
//
// Search for fHadronnessName [MHadronness] to store the hadronness in
// there. PreProcess the filter list.
//
Int_t MFiltercutsCalc::PreProcess(MParList *pList)
{
    if (!fList->PreProcess(pList))
        return kFALSE;

    fHadronness = (MHadronness*)pList->FindCreateObj("MHadronness", fHadronnessName);
    if (!fHadronness)
        return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Evaluate the filter list. if the Expression is true set hadronness to
// 0.25, otherwise to 0.75.
//
Int_t MFiltercutsCalc::Process()
{
    if (!fList->Process())
        return kFALSE;

    if (fList->IsExpressionTrue())
        fHadronness->SetHadronness(0.25);
    else
        fHadronness->SetHadronness(0.75);

    fHadronness->SetReadyToSave();

    return kTRUE;
}
