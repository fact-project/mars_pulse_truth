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
!   Author(s): Thomas Bretz 03/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MStatusArray
//
//  Helper class for MStatusDisplay
//
// If you want to read a MStatusArray (normally with name MStatusDisplay)
// it is recommended to do it like this:
//     TFile f("myfile.root", "read");
//     MStatusArray arr;
//     arr.Read();
//
// If you want to use TFile::Get or TFile::GetObject you should switch off
// addding Histograms automatically to the current directory first:
//     TFile f("myfile.root", "read");
//     TH1::AddDirectory(kFALSE);
//     f.Get("MStatusDisplay");
//
//////////////////////////////////////////////////////////////////////////////
#include "MStatusArray.h"

#include <TH1.h>              // TH1::AddDirectoryStatus();
#include <TFile.h>            // gFile
#include <TClass.h>
#include <TCanvas.h>

#include <TGraph.h>           // For the TGraph workaround
#include <TPaveText.h>        // For the TPaveText workaround

#include "MLog.h"
#include "MLogManip.h"

#include "MParContainer.h"   // MParContainer::GetClass
#include "MStatusDisplay.h"

//#define DEBUG

ClassImp(MStatusArray);

using namespace std;

// --------------------------------------------------------------------------
//
// Initialize the MStatusArray from an MStatusDisplay. Note, the contents
// still owned by MStatusDisplay and will vanish if the display changes
// or is deleted without further notice.
//
MStatusArray::MStatusArray(const MStatusDisplay &d) : TObjArray()
{
    d.FillArray(*this);
}

#ifdef DEBUG
static void *ptr = 0;
#endif

// --------------------------------------------------------------------------
//
// Remove objects matching the id (the first character of their class
// name) recuresively
//
Bool_t MStatusArray::RecursiveDelete(TVirtualPad *p, const char id) const
{
    if (!p)
        return kTRUE;

    // It seems that it is necessary to keep the ListOfPrimitve consistent
    // while deleting - reason unknown.
    TIter Next(p->GetListOfPrimitives());
    TObject *o=0;
    while ((o=Next()))
    {
        if (!dynamic_cast<TVirtualPad*>(o) && (o->ClassName()[0]==id || id==0))
        {
            while (p->GetListOfPrimitives()->Remove(o));

            // This is necessary because for unknown reasons TPaveText
            // as a member of a class (e.g. MHRanForestGini) doesn't
            // get removed from the pad if deleted, and therefore
            // gets deleted twice (FIXME: To be investigated)
            //if (dynamic_cast<TPaveText*>(o) && !o->TestBit(kCanDelete))
            //    continue;

#ifdef DEBUG
            cout << "Start Delete " << o << " " << o->GetName() << " " << o->ClassName() << endl;
#endif
            delete o;

#ifdef DEBUG
            cout << "Done  Delete " << o<< endl;
#endif
            return kFALSE;
        }

        while (!RecursiveDelete(dynamic_cast<TVirtualPad*>(o), id));
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Make sure to set the kMustCleanup for all object in our tree
// which will later be deleted when the array is destructed.
//
void MStatusArray::SetCleanup(TObject *obj) const
{
    if (!obj)
        return;

    TVirtualPad *pad = dynamic_cast<TVirtualPad*>(obj);

    // Do not set the bit for pads because it would end in
    // endless recursions
    if (pad && !dynamic_cast<TCanvas*>(obj))
        obj->ResetBit(kMustCleanup);
    else
    {
        // If this is no pad or TCanvas set the kMustCleanup bit
        // to make sure that the object is not by chance deleted twice.
        obj->SetBit(kMustCleanup);

        // Unfortunately, this can still happen to the fHistogram and
        // fFunctions in a TGraph (because for some reason if a TGraph
        // is stored twice in a file they might later share the same
        // objects)
        TGraph *g = dynamic_cast<TGraph*>(obj);
        if (g)
        {
#ifdef DEBUG
            cout << "Found TGraph " << g->GetName() << " " << g << " while deleting " << ptr << endl;
#endif
            if (g->GetHistogram())
                g->GetHistogram()->SetBit(kMustCleanup);
            if (g->GetListOfFunctions())
                g->GetListOfFunctions()->SetBit(kMustCleanup);
        }
    }

    if (!pad)
        return;

    // If object was a pad go one layer deeper
    TIter Next(pad->GetListOfPrimitives());
    TObject *o=0;
    while ((o=Next()))
        SetCleanup(o);
}

// This is a stupid C++ trick which allos us to access the
// protected data-mebers of TGraph
class MyGraph : public TGraph
{
public:
    void ResetListOfFunctions()
    {
        fFunctions = 0;
    }
};

void MStatusArray::RecursiveRemove(TCollection *col, TObject *obj)
{
    // First remove the object from the collection as often as possible
    // (The collection is always a ListOfPrimitives(). This code is
    // based on TCollection::RecursiveRemove and TPad::RecursiveRemove)
    while (col->Remove(obj));

    TObject *object=0;

    // Loop over all objects in the collection
    TIter Next(col);
    while ((object=Next()))
    {
        // Check if this object is a pad. If it is a pad go one
        // layer deeper.
        TPad *pad = dynamic_cast<TPad*>(object);
        if (pad)
        {
#ifdef DEBUG
            cout << "Start RecursiveRemove in " << pad->GetName() << " " << pad << endl;
#endif
            RecursiveRemove(pad->GetListOfPrimitives(), obj);
#ifdef DEBUG
            cout << "Done  RecursiveRemove in " << pad->GetName() << " " << pad << endl;
#endif
            continue;
        }

        // If it is not a TPad check if it is a TGraph
        TGraph *g = dynamic_cast<TGraph*>(object);
        if (g/* && g!=ptr*/)
        {
#ifdef DEBUG
            gLog << all << "MStatusArray::RecursiveRemove " << obj << " from TGraph " << g << endl;
#endif

            // If the object to be removed is the histogram from the
            // TGraph the histogram got already deleted so we have to
            // reset the pointer
            if (g->GetHistogram()==obj)
            {
#ifdef DEBUG
                gLog << all << "       SetHist(0)" << endl;
#endif
                g->SetHistogram(0);
            }

            // If the object to be removed is the ListOfFunction from the
            // TGraph the ListOfFunction got already deleted so we have to
            // reset the pointer. This is done by a stupid C++ trick
            // (a derived class which can access the protected data mebers)
            if (g->GetListOfFunctions()==obj)
            {
#ifdef DEBUG
                gLog << all << "       SetFunc(0)" << endl;
#endif
                static_cast<MyGraph*>(g)->ResetListOfFunctions();
            }
        }

#ifdef DEBUG
        cout << " Call RecursiveRemove for " << object->GetName() << " " << object << endl;
#endif

        // Now we go on like TCollction would do calling the RecursiveRemove
        // of the objects (which are no pads)
        if (object->TestBit(TObject::kNotDeleted))
            object->RecursiveRemove(obj);
    }
}

// --------------------------------------------------------------------------
//
// Use our own implementation of RecursiveRemove.
//
void MStatusArray::RecursiveRemove(TObject *obj)
{
#ifdef DEBUG
    cout << "RecursiveRemove " << obj << endl;
#endif
    RecursiveRemove(this, obj);
}

// --------------------------------------------------------------------------
//
// The problem is that our objects (e.g. MHRate) can own histograms and
// graphs (or other objects), which are data members rather than allocated
// objects (pointers), and which are drawn into a pad. If such a pad gets
// read from a file we have to set the kCanDelete bits for all objects
// in all pads to ensure that they are properly deleted (since we
// don't know who owns them). Unfortunately, this might result in the
// data member deleted first and then second in the destructor of the
// owning class. One simple workaround is to delete all our own objects
// ("M*") first without touching the root objects ("T*") and delete the
// remaining ones later. This might not work if teh owned objects are
// MARS classes. Fortunately, none of our classes yets owns another of
// our class not dynamically allocated. (This case is covered by
// the RecursiveRemove facility).
//
// Furthermore TGraphs which are read from a file as a data member
// of a class and which are drawn to a pad share the same fFunctions
// and fHistogram. To avoid double deleting (the second TGraph won't
// be signaled if the first one is deleted) we implement our own
// RecursiveRemove facility. (This gets a bit better in root 5.28
// because TGraph::RecursiveRemove will than also check fHistogram)
//
void MStatusArray::Delete(Option_t *)
{
    // Add this to the list of cleanups to ensure as many cleaning
    // operations as possible are propagated
    gROOT->GetListOfCleanups()->Add(this);

    // First make sure that all kMustCleanup bits are set, esepcially
    // the ones of TGraph::fHistogram and TGraph::fFunctions
    TIter Next(this);
    TObject *o=0;
    while ((o=Next()))
        SetCleanup(o);

#if ROOT_VERSION_CODE < ROOT_VERSION(5,26,00)
    // Now delete the MARS object first because we have full control
    // of them
    TIter Next2(this);
    while ((o=Next2()))
        while (!RecursiveDelete(dynamic_cast<TVirtualPad*>(o), 'M'));

    // Now delete all root objects
    TIter Next3(this);
    while ((o=Next3()))
        while (!RecursiveDelete(dynamic_cast<TVirtualPad*>(o)));
#endif

    // And delete all the rest
    TObjArray::Delete();

    // Remove it from the list again
    gROOT->GetListOfCleanups()->Remove(this);
}

// --------------------------------------------------------------------------
//
// If o==NULL a new status display is created, otherwise the one with name o
// is searched in gROOT->GetListOfSpecials().
// In this display the contents of the MStatusArray is displayed.
//
TObject *MStatusArray::DisplayIn(Option_t *o) const
{
    MStatusDisplay *d = 0;
    if (TString(o).IsNull())
        d = new MStatusDisplay;

    if (!d)
        d = (MStatusDisplay*)gROOT->GetListOfSpecials()->FindObject(o);

    if (!d)
        return 0;

    if (d->Display(*this))
        return d;

    delete d;
    return 0;
}

// --------------------------------------------------------------------------
//
// Display the contents of the given tab in the display given as argument.
//
void MStatusArray::DisplayIn(MStatusDisplay &d, const char *tab) const
{
    d.Display(*this, tab);
}

TObject *MStatusArray::FindObjectInPad(TVirtualPad *pad, const char *object, TClass *cls) const
{
    TObject *o = NULL;//pad->FindObject(object);
//    if (o && o->InheritsFrom(cls))
//        return o;

    TIter Next(pad->GetListOfPrimitives());
    while ((o=Next()))
    {
        if (o->GetName()==(TString)object && o->InheritsFrom(cls))
            return o;

        if (o==pad || !o->InheritsFrom(TVirtualPad::Class()))
            continue;

        if ((o = FindObjectInPad((TVirtualPad*)o, object, cls)))
            return o;
//            if (o->InheritsFrom(cls))
//                return o;
    }
    return 0;
}

TCanvas *MStatusArray::FindCanvas(const char *name) const
{
    TObject *o = TObjArray::FindObject(name);
    if (!o)
        return 0;

    return o->InheritsFrom(TCanvas::Class()) ? (TCanvas*)o : 0;
}


TObject *MStatusArray::FindObjectInCanvas(const char *object, const char *base, const char *canvas) const
{
    gLog << err;
    TClass *cls = MParContainer::GetClass(base, &gLog);
    if (!cls)
        return 0;

    TCanvas *c = canvas ? FindCanvas(canvas) : 0;
    if (canvas)
    {
        if (!c)
        {
            gLog << warn << "Canvas '" << canvas << "' not found..." << endl;
            return 0;
        }

        TObject *o = FindObjectInPad(c, object, cls);
        if (!o)
        {
            gLog << warn << "Object '" << object << "' [" << base << "] not found in canvas '" << canvas << "'..." << endl;
            return 0;
        }

        return o; //o->InheritsFrom(cls) ? o : 0;
    }

    TObject *o=0;
    TIter Next(this);
    while ((o=Next()))
    {
        if (!o->InheritsFrom(TVirtualPad::Class()))
            continue;

        if ((o=FindObjectInPad((TVirtualPad*)c, object, cls)))
            return o;
    }

    gLog << warn << "Object '" << object << "' [" << base << "] not found in canvas '" << canvas << "'..." << endl;
    return NULL;
}

TObject *MStatusArray::FindObjectInCanvas(const char *object, const char *canvas) const
{
    return FindObjectInCanvas(object, object, canvas);
}

TObject *MStatusArray::FindObject(const char *object, const char *base) const
{
    return FindObjectInCanvas(object, base, 0);
}

TObject *MStatusArray::FindObject(const char *object) const
{
    return FindObjectInCanvas(object, object, 0);
}

// --------------------------------------------------------------------------
//
// Print recursively all objects in this and sub-pads
//
void MStatusArray::PrintObjectsInPad(const TCollection *list, const TString &name, Int_t lvl) const
{
    TIter Next(list);
    TObject *o=0;
    while ((o=Next()))
    {
        const Bool_t print = name.IsNull() || name==(TString)o->GetName();
        if (print)
        {
            if (lvl>0)
                gLog << setw(lvl) << ' ';
            gLog << " " << o->ClassName() << ": " << o->GetName() << " <" << Next.GetOption() << "> (" << o << ") " << (int)o->TestBit(kCanDelete) << endl;
        }

        if (o->InheritsFrom(TVirtualPad::Class()))
            PrintObjectsInPad(((TVirtualPad*)o)->GetListOfPrimitives(), print?TString():name, lvl+1);
    }
}

// --------------------------------------------------------------------------
//
// Print recursively all objects in this and sub-pads. If !option.IsNull()
// only objects in the corresponding pad are printed.
//
void MStatusArray::Print(Option_t *option) const
{
    gLog << all;

    PrintObjectsInPad(this, TString(option));
}

/*
// --------------------------------------------------------------------------
//
// Make sure that kCanDelete is properly set for all directly contained
// objects. Some kCanDelete bits might not be properly set or get lost when
// the MParContainer is stored.
//
void MStatusArray::SetCanDelete(const TCollection *list) const
{
    TIter Next(list);
    TObject *o=0;
    while ((o=Next()))
    {
        if (o->InheritsFrom(TVirtualPad::Class()))
            SetCanDelete(((TVirtualPad*)o)->GetListOfPrimitives());
        else
            o->SetBit(kCanDelete|kMustCleanup);
    }
}
*/

// --------------------------------------------------------------------------
//
// Set kCanDelete for all objects for which kMyCanDelete is set. This
// is a STUPID workaruond for an ANNOYING root bug which is that
// the streamer function of TH1 resets the KCanDelete bit after reading.
//
void MStatusArray::SetCanDelete(const TCollection *list) const
{
    TIter Next(list);
    TObject *o=0;
    while ((o=Next()))
    {
        if (o->InheritsFrom(TVirtualPad::Class()))
            SetCanDelete(((TVirtualPad*)o)->GetListOfPrimitives());
        else
        {
            if (o->TestBit(kMyCanDelete) && o->InheritsFrom("TH1"))
            {
                o->SetBit(kCanDelete);
                o->ResetBit(kMyCanDelete);
            }
        }
    }
}

void MStatusArray::EnableTH1Workaround(const TCollection *list) const
{
    TIter Next(list?list:this);
    TObject *o=0;
    while ((o=Next()))
    {
        if (o->InheritsFrom(TVirtualPad::Class()))
            EnableTH1Workaround(((TVirtualPad*)o)->GetListOfPrimitives());
        else
            if (o->InheritsFrom("TH1"))
                o->SetBit(kCanDelete);
    }
}

// --------------------------------------------------------------------------
//
// Set kMyCanDelete for all objects for which kCanDelete is set. This
// is a STUPID workaruond for an ANNOYING root bug which is that
// the streamer function of TH1 resets the KCanDelete bit after reading.
//
void MStatusArray::SetMyCanDelete(const TCollection *list) const
{
    TIter Next(list);
    TObject *o=0;
    while ((o=Next()))
    {
        if (o->InheritsFrom(TVirtualPad::Class()))
            SetMyCanDelete(((TVirtualPad*)o)->GetListOfPrimitives());
        else
        {
            if (o->TestBit(kMyCanDelete) && o->InheritsFrom("TH1"))
                gLog << warn << "WARNING - MStatusArray::Write - " << o->GetName() << " [" << o->ClassName() << "] has BIT(30) already set!" << endl;

            if (o->TestBit(kCanDelete) && o->InheritsFrom("TH1"))
                o->SetBit(kMyCanDelete);
        }
    }
}

// --------------------------------------------------------------------------
//
// Reset kMyCanDelete for all objects for which kMyCanDelete is set. This
// is a STUPID workaruond for an ANNOYING root bug which is that
// the streamer function of TH1 resets the KCanDelete bit after reading.
//
void MStatusArray::ResetMyCanDelete(const TCollection *list) const
{
    TIter Next(list);
    TObject *o=0;
    while ((o=Next()))
    {
        if (o->InheritsFrom(TVirtualPad::Class()))
            ResetMyCanDelete(((TVirtualPad*)o)->GetListOfPrimitives());
        else
        {
            if (o->TestBit(kMyCanDelete) && o->InheritsFrom("TH1"))
                o->ResetBit(kMyCanDelete);
        }
    }
}

MStatusArray::~MStatusArray()
{
    // This is the destructor from TObjArray...
    // It must be here, because for some reason I don't know it
    // is otherwise not correctly executed from the Interpreter
    // (root 5.12/00f)
    if (IsOwner())
        Delete();

    TStorage::Dealloc(fCont);

    fCont = 0;
    fSize = 0;
}

// --------------------------------------------------------------------------
//
// Switch off adding histograms to current directory before reading.
// Switch back
//
Int_t MStatusArray::Read(const char *name)
{
    // It seems that the contents are not properly deleted by TObjArray::Read
    Delete();

    SetOwner();

    const TString keyname = name?name:"MStatusDisplay";

    // Check if key exists (to suppress an error on the console
    if (!gDirectory->GetListOfKeys())
    {
        gLog << inf << keyname << " [MStatusArray] not found (no open file?)" << endl;
        return 0;
    }

    // Check if key exists (to suppress an error on the console
    if (!gDirectory->GetListOfKeys()->FindObject(keyname))
    {
        gLog << inf << keyname << " [MStatusArray] not found." << endl;
        return 0;
    }

    // Make sure newly read histograms are not added to the current directory
    const Bool_t store = TH1::AddDirectoryStatus();
    TH1::AddDirectory(kFALSE);
    const Int_t rc = TObjArray::Read(keyname);
    TH1::AddDirectory(store);

    // All objects in the list (TNamed, TCanvas, etc) do not have
    // the kCanDelete bit set. Make sure that it is set to make
    // them deleted by the destructor of this list
    TIter Next(this);
    TObject *o=0;
    while ((o=Next()))
    {
        if (o->InheritsFrom(TVirtualPad::Class()))
        {
            TIter Next2(((TVirtualPad*)o)->GetListOfPrimitives());
            TObject *o2=0;
            while ((o2=Next2()))
                if (o2->InheritsFrom(MParContainer::Class()))
                    o2->SetBit(kCanDelete);
        }
        o->SetBit(kCanDelete);
    }

    // Make sure that all kCanDelete bits are properly set
    SetCanDelete(this);
    SetOwner();

    return rc;
}

// --------------------------------------------------------------------------
//
// Switch off adding histograms to current directory before reading.
// Switch back
//
Int_t MStatusArray::Write(const char *name, Int_t option, Int_t bufsize) const
{
    SetMyCanDelete(this);
    const Int_t rc = TObjArray::Write(name, option, bufsize);
    ResetMyCanDelete(this);

    return rc;
}
