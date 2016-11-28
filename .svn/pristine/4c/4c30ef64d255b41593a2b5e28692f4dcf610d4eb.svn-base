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
!   Author(s): Thomas Bretz  08/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MDataArray
//
//   An Array of MData derived classes.
//   It can be used, eg, in MHMatrix for description of the columns.
//
/////////////////////////////////////////////////////////////////////////////
#include "MDataArray.h"

#include <fstream>

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MDataPhrase.h"

ClassImp(MDataArray);

using namespace std;

static const TString gsDefName  = "MDataArray";
static const TString gsDefTitle = "Array to store MData cntainers";

// --------------------------------------------------------------------------
//
// Default Constructor
//
MDataArray::MDataArray(const char *name, const char *title)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    gROOT->GetListOfCleanups()->Add(&fList);
    fList.SetBit(kMustCleanup);
}

// --------------------------------------------------------------------------
//
// Copy Constructor
//
MDataArray::MDataArray(MDataArray &a, const char *name, const char *title)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    gROOT->GetListOfCleanups()->Add(&fList);
    fList.SetBit(kMustCleanup);

    TIter Next(&a.fList);
    MData *data = NULL;
    while ((data=(MData*)Next()))
        AddEntry(data->GetRule());
}

// --------------------------------------------------------------------------
//
// Add a new data rule as a new entry (MDataPhrase)
//
void MDataArray::AddEntry(const TString rule)
{
    TObject *obj = new MDataPhrase(rule);
    obj->SetBit(kCanDelete);
    fList.Add(obj);
}

// --------------------------------------------------------------------------
//
// Add a new data as a new entry (MData). If the destructor of MDataArray
// should delete the object set its bit kCanDelete
//
void MDataArray::AddEntry(MData *data)
{
    data->SetBit(kMustCleanup);
    fList.Add(data);
}

// --------------------------------------------------------------------------
//
// Try to find a MData which has the same rule (GetRule()).
// Be carefull: This may already fail if the rule is A*B and you are
// searching for B*A - FIXME: A more intelligent comaparton (based on
// IsEqual()) should be developed.
//
// Returns the index in the array, -1 if rule was not found.
//
Int_t MDataArray::FindRule(const char *rule) const
{
    const MDataPhrase data(rule);
    if (!data.IsValid())
        return -1;

    const TString r(data.GetRule());

    TIter Next(&fList);
    const MData *d = NULL;
    while ((d=(MData*)Next()))
        if (d->GetRule()==r)
            return fList.IndexOf(d);

    return -1;
}

// --------------------------------------------------------------------------
//
// Return the i-th entry
//
MData &MDataArray::operator[](Int_t i) const
{
    return (MData&)*((TObjArray)fList)[i];
}

// --------------------------------------------------------------------------
//
// Return the data value of the i-th entry
//
Double_t MDataArray::operator()(Int_t i) const
{
    return (*this)[i].GetValue();
}

// --------------------------------------------------------------------------
//
// PreProcesses all members in the list
//
Bool_t MDataArray::PreProcess(const MParList *plist)
{
    if (fList.GetSize()==0)
    {
        *fLog << err << "Error - No Column specified... aborting." << endl;
        return kFALSE;
    }

    TIter Next(&fList);
    MData *data = NULL;
    while ((data=(MData*)Next()))
        if (!data->PreProcess(plist))
            return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print the rules for all entries of the array
//
void MDataArray::Print(Option_t *opt) const
{
    Int_t n=0;

    const Int_t w = 1+(Int_t)TMath::Log10(fList.GetEntries());

    TIter Next(&fList);
    MData *data = NULL;
    while ((data=(MData*)Next()))
    {
        *fLog << all << " " << fName << "[" << setw(w) << n++ << "] = " << flush;
        data->Print();
        *fLog << endl;
    }
}

Bool_t MDataArray::AsciiWrite(ostream &out) const
{
    ((TObjArray)fList).R__FOR_EACH(MParContainer, AsciiWrite)(out);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MDataArray::StreamPrimitive(ostream &out) const
{
    out << "   MDataArray " << GetUniqueName();

    if (fName!=gsDefName)
    {
        out << "(\"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\")";
    }
    out << ";" << endl;

    TIter Next(&fList);
    MData *data = NULL;
    while ((data=(MData*)Next()))
        out << "   " << GetUniqueName() << ".AddEntry(\"" << data->GetRule() << "\");" << endl;
}

TString MDataArray::GetRule(int i) const
{
    return (*this)[i].GetRule();
}

// --------------------------------------------------------------------------
//
// Return the data members existing in this array in a comma-seperated list
// (This is mainly used for MTask::AddToBranchList)
//
TString MDataArray::GetDataMember() const
{
    TString str;

    TIter Next(&fList);
    MData *data = NULL;
    while ((data=(MData*)Next()))
    {
        if (data->GetDataMember().IsNull())
            continue;

        str += ",";
        str += data->GetDataMember();
    }
    return str;
}

void MDataArray::GetData(TVector &v) const
{
    Int_t ncols = GetNumEntries();

    v.ResizeTo(ncols);

    // same as: for (int i=0; i<ncols; i++) <should be faster>
    while (ncols--)
        v(ncols) = (*this)(ncols);
}

void MDataArray::SetVariables(const TArrayD &arr)
{
    fList.R__FOR_EACH(MData, SetVariables)(arr);
}
