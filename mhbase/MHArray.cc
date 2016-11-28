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
!   Author(s): Thomas Bretz  07/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MHArray
//
//  is a sequential collection of mars histograms. If the given index to
//  call the Fill function of the histogram excceeds the size of the
//  array by 1 a new entry is created.
//
//  With Set/Inc/DecIndex you may specify the actual index of the histogram
//  wich should be filles by Fill.
//
//  Use GetH to get the current histogram, the []-operator get the histogram
//  by its index.
//
//  To access the histograms by a key instead of an index use SetIndexByKey
//  instead of Set/Inc/DecIndex. It will take the integerpart of the
//  floating point value (2 in case of 2.9). For each new key a new
//  index in the Mapping Table is created. So that you can access your
//  histograms by the key (eg in case of the Angle Theta=23.2deg use
//  SetIndexByKey(23.2)
//
//  If the index is equal to the number of histograms in the array a call
//  to the Fill-member-function will create a new histogram.
//
//  In the constructor istempl leads to two different behaviours of the
//  MHArray:
//
//  - istempl=kTRUE tells MHArray to use the first histogram retrieved from
//    the Parameterlist by hname to be used as a template. New histograms
//    are not created using the root dictionary, but the New-member function
//    (see MParConatiner)
//  - In the case istempl=kFALSE new histograms are created using the root
//    dictionary while hname is the class name. For the creation their
//    default constructor is used.
//
//////////////////////////////////////////////////////////////////////////////
#include "MHArray.h"

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TClass.h>
#include <TStyle.h>
#include <TGaxis.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveStats.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MParContainer.h"

#include "MBinning.h"

ClassImp(MHArray);

using namespace std;

//////////////////////////////////////////////////////////////////////////////
//
// MMap
//
// This class maps a key-value to a given value. In its simple versions it
// maps a key to an index.
//
//////////////////////////////////////////////////////////////////////////////
#include <TArrayI.h>

class MMap
{
private:
    TArrayI fKeys;
    TArrayI fValues;

    Int_t K(Int_t i) const { return ((TArrayI)fKeys)[i]; }
    Int_t V(Int_t i) const { return ((TArrayI)fValues)[i]; }

public:
    // --------------------------------------------------------------------------
    //
    // Return the size of the table
    //
    Int_t GetSize() const
    {
        return fKeys.GetSize();
    }

    // --------------------------------------------------------------------------
    //
    // Get the value which corresponds to the given key-value
    //
    Int_t GetValue(Int_t key) const
    {
        const Int_t n = fKeys.GetSize();
        for (int i=0; i<n; i++)
        {
            if (K(i)==key)
                return V(i);
        }
        return -1;
    }

    // --------------------------------------------------------------------------
    //
    // Get the key which corresponds to the given index
    //
    Int_t GetKey(Int_t value) const
    {
        const Int_t n = fKeys.GetSize();
        for (int i=0; i<n; i++)
        {
            if (V(i)==value)
                return K(i);
        }
        return -1;
    }

    // --------------------------------------------------------------------------
    //
    // Adds a new pair key-value. While the key is the key to the value.
    // if the key already exists the pair is ignored.
    //
    void Add(Int_t key, Int_t value)
    {
        if (GetValue(key)>=0)
            return;

        const Int_t n = fKeys.GetSize();

        fKeys.Set(n+1);
        fValues.Set(n+1);

        fKeys[n] = key;
        fValues[n] = value;
    }

    // --------------------------------------------------------------------------
    //
    // Adds a new pair key-value. While the key is the key to the value.
    // In this case the value is an automatically sequential created index.
    // if the key already exists the pair is ignored.
    //
    Int_t Add(Int_t key)
    {
        const Int_t k = GetValue(key);
        if (k>=0)
            return k;

        const Int_t n = fKeys.GetSize();

        fKeys.Set(n+1);
        fValues.Set(n+1);

        fKeys[n] = key;
        fValues[n] = n;

        return n;
    }
};

void MHArray::Init(const char *name)
{
    fName  = name  ? name  : "MHArray";

    fMapIdx = new MMap;

    fArray = new TList;
    fArray->SetOwner();
}

// --------------------------------------------------------------------------
//
// Can replace a constructor. Use the default constructor and afterwards
// the Set function of your need.
//
void MHArray::Set(const TString hname, Bool_t istempl)
{
    if (fTemplate || fClass || fTemplateName!="<dummy>")
    {
        *fLog << warn << "WARNING - MHArray already setup... Set ignored." << endl;
        return;
    }

    if (istempl)
    {
        fTemplateName = hname;
        return;
    }

    //
    // try to get class from root environment
    //
    fClass = gROOT->GetClass(hname);
    if (!fClass)
    {
        //
        // if class is not existing in the root environment
        //
        *fLog << err << dbginf << "Class '" << hname << "' not existing in dictionary." << endl;
    }

    //
    // check for ineritance from MH
    //
    if (!fClass->InheritsFrom(MH::Class()))
    {
        //
        // if class doesn't inherit from MH --> error
        //
        *fLog << err << dbginf << "Class '" << hname << "' doesn't inherit from MH." << endl;
        fClass = NULL;
    }
}

// --------------------------------------------------------------------------
//
// Can replace a constructor. Use the default constructor and afterwards
// the Set function of your need.
//
void MHArray::Set(const MH *hist)
{
    fIdx=0;
    fClass=NULL;
    fTemplate=hist;
    fTemplateName="<dummy>";
}


// --------------------------------------------------------------------------
//
// hname is the name of the histogram class which is in the array.
//
// istempl=kTRUE tells MHArray to use the first histogram retrieved from the
// ParameterList by hname to be used as a template. New histograms are not
// created using the root dictionary, but the New-member function (see
// MParConatiner)
// In the case istempl=kFALSE new histograms are created using the root
// dictionary while hname is the class name. For the creation their
// default constructor is used.
//
MHArray::MHArray(const TString hname, Bool_t istempl, const char *name, const char *title)
    : fIdx(0), fClass(NULL), fTemplate(NULL)
{
    //
    //   set the name and title of this object
    //
    Init(name);
    fTitle = title ? TString(title) : (TString("Base class for Mars histogram arrays:") + hname);

    Set(hname, istempl);
}

// --------------------------------------------------------------------------
//
// Default constructor. Use MHArray::Set to setup the MHArray afterwards
//
MHArray::MHArray(const char *name, const char *title)
    : fIdx(0), fClass(NULL), fTemplate(NULL), fTemplateName("<dummy>")
{
    //
    //   set the name and title of this object
    //
    Init(name);
    fTitle = title ? title : "A Mars histogram array";
}

// --------------------------------------------------------------------------
//
// hname is the name of the histogram class which is in the array.
//
// istempl=kTRUE tells MHArray to use the first histogram retrieved from the
// ParameterList by hname to be used as a template. New histograms are not
// created using the root dictionary, but the New-member function (see
// MParConatiner)
// In the case istempl=kFALSE new histograms are created using the root
// dictionary while hname is the class name. For the creation their
// default constructor is used.
//
MHArray::MHArray(const MH *hist, const char *name, const char *title)
    : fIdx(0), fClass(NULL), fTemplate(hist), fTemplateName("<dummy>")
{
    //
    //   set the name and title of this object
    //
    Init(name);
    fTitle = title ? TString(title) : (TString("Base class for Mars histogram arrays:") + hist->GetName());
}

// --------------------------------------------------------------------------
//
// Destructor: Deleteing the array and all histograms which are part of the
// array.
//
MHArray::~MHArray()
{
    fArray->Delete();
    delete fArray;
    delete fMapIdx;
}

// --------------------------------------------------------------------------
//
//  Use this to access the histograms by a key. If you use values like
//   (in this order) 2.5, 7.2, 2.5, 9.3, 9.3, 3.3, 2.2, 1.1
//  it will be mapped to the following indices internally:
//                    0    1    0    2    2    3    4    5
//
//  WARNING: Make sure that you don't create new histograms by setting
//           a new index (SetIndex or IncIndex) which is equal the size
//           of the array and create new histogram by CreateH. In this
//           case you will confuse the mapping completely.
//
void MHArray::SetIndexByKey(Double_t key)
{
    fIdx = fMapIdx->Add((Int_t)key);
}

// --------------------------------------------------------------------------
//
//  Use this function to access a histogram by its index in the array.
//  Becarefull the index isn't checked!
//
MH &MHArray::operator[](Int_t i)
{
    return *(MH*)fArray->At(i);
}

// --------------------------------------------------------------------------
//
//  Use this function to access a histogram by its index in the array.
//  Becarefull the index isn't checked!
//
MH *MHArray::At(Int_t i)
{
    return (MH*)fArray->At(i);
}

// --------------------------------------------------------------------------
//
//  Use this function to access the histogram corresponding to the
//  currently set index (by Set/Inc/DecIndex or SetIndexByKey)
//  Becarefull the index set isn't checked!
//
MH *MHArray::GetH()
{
    return (MH*)fArray->At(fIdx);
}

// --------------------------------------------------------------------------
//
// Tries to create a new histogram, adds it as last entry to the array
// and tries to call SetupFill for it. In case of success the last entry
// in the array is the new histogram and kTRUE is returned. Otherwise
// kFALSE is returned.
//
Bool_t MHArray::CreateH()
{
    TString cname = fClass ? fClass->GetName() : fTemplate->IsA()->GetName();

    MH *hist = NULL;
    if (fTemplate)
    {
        //
        // create the parameter container as a clone of the existing
        // template histogram.
        //
        hist = (MH*)fTemplate->New();
    }
    else
    {
        //
        // create the parameter container of the the given class type
        //
        hist = (MH*)fClass->New();
    }
    if (!hist)
    {
        *fLog << err << dbginf << "Cannot create new instance of class '";
        *fLog << cname << "' (Maybe no def. constructor)" << endl;
        return kFALSE;
    }

    //
    // Set the name of the container
    //
    if (!fTemplate)
    {
        TString name = TString(hist->GetName())+";";
        name += fIdx;

        hist->SetName(name);
    }

    //
    // Try to setup filling for the histogram
    //
    if (!hist->SetupFill(fParList))
    {
        *fLog << err << dbginf << "SetupFill for new histogram of type '";
        *fLog << cname << "' with Index #" << fIdx << " failed." << endl;
        delete hist;
        return kFALSE;
    }

    fArray->AddLast(hist);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Returns kFALSE if the class couldn't be found in the root dictionary or
// if it doesn't inherit from MH.
// The parameter list is remembert to be used for SetupFill in case a new
// histogram is created.
// The index is reset to 0
//
Bool_t MHArray::SetupFill(const MParList *pList)
{
    fParList = pList;
    fIdx = 0;

    if (fTemplate)
        return kTRUE;

    if (!fTemplateName.IsNull())
    {
        fTemplate = (MH*)pList->FindObject(fTemplateName, "MH");
        return fTemplate ? kTRUE : kFALSE;
    }

    return fClass ? kTRUE : kFALSE;
}

// --------------------------------------------------------------------------
//
// Call Fill for the present histogram index. If the index is out of
// bounds the event is skipped. If the index is the number of current
// histograms in the array a new histogram is created and if creation was
// successfull filled.
//
Int_t MHArray::Fill(const MParContainer *par, const Stat_t w)
{
    const Int_t n = fArray->GetSize();

    if (fIdx<0 || fIdx>n)
    {
        *fLog << warn << "Histogram Index #" << fIdx << " out of bounds (>";
        *fLog << n << ")... skipped." << endl;
        return kERROR;
    }

    if (fIdx==n)
        if (!CreateH())
            return kERROR;

    return GetH()->Fill(par, w);
}

Bool_t MHArray::AddHistogram()
{
    fIdx=fArray->GetSize();

    return CreateH();
}

// --------------------------------------------------------------------------
//
// Calls Finalize for all histograms in the list. If at least one Finalize
// fails kFALSE is returned.
//
Bool_t MHArray::Finalize()
{
    Bool_t rc = kTRUE;

    TIter Next(fArray);
    MH *hist = NULL;

    while ((hist=(MH*)Next()))
        if (!hist->Finalize())
            rc = kFALSE;

    return rc;
}

// --------------------------------------------------------------------------
//
// Print the number of entries in the array
//
void MHArray::Print(Option_t *option) const
{
    *fLog << all << GetDescriptor() << " contains " << fArray->GetSize();
    *fLog << " histograms." << endl;

    if (fMapIdx->GetSize()<=0)
        return;

    *fLog << " idx\t     key" << endl;
    for (int i=0; i<fMapIdx->GetSize(); i++)
        *fLog << "  " << i << "\t<-->  " << fMapIdx->GetKey(i) << endl;
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Adds the given object to the given legend (if != NULL). The Legend
// entry name is created from the key...
//
void MHArray::AddLegendEntry(TLegend *leg, TObject *obj, Int_t idx) const
{
    if (!leg)
        return;

    TString name = " ";
    name += fMapIdx->GetKey(idx);
    leg->AddEntry(obj, name, "lpf"); // l=line, p=polymarker, f=fill
}


// --------------------------------------------------------------------------
//
// The option is the name of the histogram, used to get a histogram from
// the MH entries by calling their GetHist function.
//
void MHArray::Draw(Option_t *opt)
{
    if (!gPad)
        MH::MakeDefCanvas(this);

    const Int_t sstyle = gStyle->GetOptStat();
    gStyle->SetOptStat(0);

    //
    // if the keymapping is used create a legend to identify the histograms
    //
    TLegend *leg = NULL;
    if (fMapIdx->GetSize()>0)
    {
        leg = new TLegend(0.85, 0.80, 0.99, 0.99);
        leg->SetBit(kCanDelete);
    }

    TIter Next(fArray);
    MH *hist = (MH*)Next();

    Int_t idx=0;
    Double_t max=0;
    Double_t min=0;

    TH1 *h1=NULL;

    //
    // If the array has at least one entry:
    //  - find the starting boundaries
    //  - draw it and set its line color
    //
    if (hist)
    {
        if ((h1 = hist->GetHistByName(opt)))
        {
            h1->Draw();
            h1->SetLineColor(idx+2);
            max = h1->GetMaximum();
            min = h1->GetMinimum();

            AddLegendEntry(leg, h1, idx);
        }
    }

    //
    // For all following histograms:
    //  - update the boundaries
    //  - draw it and set its line color
    //
    while ((hist=(MH*)Next()))
    {
        TH1 *h=NULL;

        if (!(h = hist->GetHistByName(opt)))
            continue;

        h->Draw("same");
        h->SetLineColor(idx+2);
        if (max<h->GetMaximum())
            max = h->GetMaximum();
        if (min>h->GetMinimum())
            min = h->GetMinimum();

        AddLegendEntry(leg, h, idx++);
    }

    //
    // Now update the drawing region so that everything is displayed
    //
    if (h1)
    {
        h1->SetMinimum(min>0 ? min*0.95 : min*1.05);
        h1->SetMaximum(max>0 ? max*1.05 : max*0.95);
    }

    if (leg)
        leg->Draw();

    gPad->Modified();
    gPad->Update();

    gStyle->SetOptStat(sstyle);
}

// --------------------------------------------------------------------------
//
// The option is the name of the histogram, used to get a histogram from
// the MH entries by calling their GetHistByName function.
// If the option also contains 'nonew' no new canvas is created.
// The option "Scale=1" scales the area of all histogram to 1
// The option "Scale=max" scales the maximum of all histogram to 1
//
TObject *MHArray::DrawClone(Option_t *opt) const
{
    TString o(opt);

    TCanvas *c = NULL;

    Int_t scale1   = o.Index("scale=1",   TString::kIgnoreCase);
    Int_t scalemax = o.Index("scale=max", TString::kIgnoreCase);
    Int_t nonew    = o.Index("nonew",     TString::kIgnoreCase);

    if (o.BeginsWith("scale=1", TString::kIgnoreCase))
        scale1 = 0;
    if (o.BeginsWith("scale=max", TString::kIgnoreCase))
        scalemax = 0;
    if (o.BeginsWith("nonew", TString::kIgnoreCase))
        nonew = 0;

    if (nonew>=0)
    {
        c = MH::MakeDefCanvas(this);

        //
        // This is necessary to get the expected bahviour of DrawClone
        //
        gROOT->SetSelectedPad(NULL);

        o.Remove(nonew, 5);
    }
    if (scale1>=0)
        o.Remove(scale1, 7);
    if (scalemax>=0)
        o.Remove(scalemax, 9);

    const Int_t sstyle = gStyle->GetOptStat();
    gStyle->SetOptStat(0);

    //
    // if the keymapping is used create a legend to identify the histograms
    //
    TLegend *leg = NULL;
    if (fMapIdx->GetSize()>0)
    {
        leg = new TLegend(0.85, 0.80, 0.99, 0.99);
        leg->SetBit(kCanDelete);
    }

    TIter Next(fArray);
    MH *hist = (MH*)Next();

    Int_t idx=0;
    Double_t max=0;
    Double_t min=0;

    TH1 *h1=NULL;

     //
    // If the array has at least one entry:
    //  - find the starting boundaries
    //  - draw it and set its line color
    //
    if (hist)
    {
        if ((h1 = hist->GetHistByName(o)))
        {
            h1 = (TH1*)h1->DrawCopy();

            if (scale1>=0)
                h1->Scale(1./h1->Integral());
            if (scalemax>=0)
                h1->Scale(1./h1->GetMaximum());

            h1->SetMarkerColor(idx);
            h1->SetLineColor(idx+2);
            h1->SetFillStyle(4000);
            max = h1->GetMaximum();
            min = h1->GetMinimum();

            AddLegendEntry(leg, h1, idx++);
        }
    }

    //
    // For all following histograms:
    //  - update the boundaries
    //  - draw it and set its line color
    //
    while ((hist=(MH*)Next()))
    {
        TH1 *h=NULL;

        if (!(h = hist->GetHistByName(o)))
            continue;

        h = (TH1*)h->DrawCopy("same");

        if (scale1>=0)
            h->Scale(1./h->Integral());
        if (scalemax>=0)
            h->Scale(1./h->GetMaximum());

        h->SetMarkerColor(idx);
        h->SetLineColor(idx+2);
        h->SetFillStyle(4000); // transperent (why is this necessary?)
        if (max<h->GetMaximum())
            max = h->GetMaximum();
        if (min>h->GetMinimum())
            min = h->GetMinimum();

        AddLegendEntry(leg, h, idx++);
    }

    //
    // Now update the drawing region so that everything is displayed
    //
    if (h1)
    {
        h1->SetMinimum(min>0 ? min*0.95 : min*1.05);
        h1->SetMaximum(max>0 ? max*1.05 : max*0.95);
    }

    if (leg)
        leg->Draw();

    gPad->Modified();
    gPad->Update();

    gStyle->SetOptStat(sstyle);

    return c;
}
