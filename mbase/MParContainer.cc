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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MParContainer
//
// The MParContainer class is the base class for all MARS parameter
// containers. At the moment it is almost the same than ROOT's TNamed.
// A TNamed contains the essential elements (name, title)
// to identify a derived object in lists like our MParList or MTaskList.
// The main difference is that the name and title isn't stored and read
// to and from root files ("//!")
//
// MParContainer has several enhancements compared to TNamed:
//  - GetDescriptor():        returns name and class type
//  - GetUniqueName():        returns a unique name (used in StreamPrimitive)
//  - SetLogStream(MLog *lg): Set a logging stream to which loggingis stored
//  - Reset():                Reset content of class in an eventloop
//  - IsReadyToSave():        The contents are ready to be saved to a file
//  - IsSavedAsPrimitive():   A unique name for this instance is already
//                            existing
//  - SetVariables():         Can be overloaded if the containers stores
//                            coefficients (to be used in fits)
//  - SetDisplay():           Set a display for redirecting graphical output
//  - GetNames():             Get Name/Title from instance and store it in
//                            a TObjArray (used to store the names of the
//                            conteiners in a file
//  - SetNames():             vice versa
//  - ReadEnv(), WriteEnv():  Function which is used for automatical setup
//    IsEnvDefined()          from a TEnv file
//
//////////////////////////////////////////////////////////////////////////////
#include "MParContainer.h"

#include <ctype.h>        // isdigit
#include <fstream>        // ofstream

#include <TEnv.h>         // Env::Lookup
#include <TClass.h>       // IsA
#include <TObjArray.h>    // TObjArray
#include <TBaseClass.h>   // GetClassPointer
#include <TMethodCall.h>  // TMethodCall, AsciiWrite
#include <TDataMember.h>  // TDataMember, AsciiWrite
#include <TVirtualPad.h>  // gPad

#include "MString.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MStatusDisplay.h"

TList *gListOfPrimitives; // forard declaration in MParContainer.h

#undef DEBUG
//#define DEBUG

ClassImp(MParContainer);

using namespace std;

TObjArray MParContainer::fgListMethodCall;

MParContainer::MParContainer(const char *name, const char *title) :
    fName(name), fTitle(title), fLog(&gLog), fDisplay(NULL), fReadyToSave(kFALSE)
{
    fgListMethodCall.SetOwner();
}

MParContainer::MParContainer(const TString &name, const TString &title) :
    fName(name), fTitle(title), fLog(&gLog), fDisplay(NULL), fReadyToSave(kFALSE)
{
    fgListMethodCall.SetOwner();
}

// --------------------------------------------------------------------------
//
//  MParContainer copy ctor
//
MParContainer::MParContainer(const MParContainer &named) : TObject()
{
    fName  = named.fName;
    fTitle = named.fTitle;

    fLog = named.fLog;

    fReadyToSave = named.fReadyToSave;

    fDisplay = named.fDisplay;
}

MParContainer::~MParContainer()
{
#ifdef DEBUG
    if (fName.IsNull() || fName==(TString)"MTime")
        return;

    *fLog << all << "Deleting " << this << " " << GetDescriptor() << endl;
    if (TestBit(kMustCleanup) && gROOT && gROOT->MustClean())
    {
        *fLog << "Recursive Remove..." << flush;
        if (TestBit(kCanDelete))
            *fLog << "kCanDelete..." << flush;
        TIter Next(gROOT->GetListOfCleanups());
        TObject *o=0;
        while ((o=Next()))
            *fLog << dbg << o->GetName() << " [" << o->ClassName() << "]" << endl;
        *fLog << dbg << "Removing..." << flush;
        gROOT->GetListOfCleanups()->RecursiveRemove(this);
        *fLog << "Removed." << endl;
    }
#endif
}

// --------------------------------------------------------------------------
//
//  MParContainer assignment operator.
//
MParContainer& MParContainer::operator=(const MParContainer& rhs)
{
    if (this == &rhs)
        return *this;

    TObject::operator=(rhs);

    fName  = rhs.fName;
    fTitle = rhs.fTitle;

    fLog = rhs.fLog;
    fReadyToSave = rhs.fReadyToSave;

    return *this;
}

// --------------------------------------------------------------------------
//
// Make a clone of an object using the Streamer facility.
// If newname is specified, this will be the name of the new object
//
TObject *MParContainer::Clone(const char *newname) const
{

   MParContainer *named = (MParContainer*)TObject::Clone();
   if (newname && strlen(newname)) named->SetName(newname);
   return named;
}

// --------------------------------------------------------------------------
//
//  Compare two MParContainer objects. Returns 0 when equal, -1 when this is
//  smaller and +1 when bigger (like strcmp).
//
Int_t MParContainer::Compare(const TObject *obj) const
{
    if (this == obj) return 0;
    return fName.CompareTo(obj->GetName());
}

// --------------------------------------------------------------------------
//
//  Copy this to obj.
//
void MParContainer::Copy(TObject &obj)
#if ROOT_VERSION_CODE > ROOT_VERSION(3,04,01)
const
#endif
{
    MParContainer &cont = (MParContainer&)obj;

    TObject::Copy(obj);

    cont.fName  = fName;
    cont.fTitle = fTitle;

    cont.fLog = fLog;
    cont.fReadyToSave = fReadyToSave;
}

// --------------------------------------------------------------------------
//
//  Encode MParContainer into output buffer.
//
void MParContainer::FillBuffer(char *&buffer)
{
    fName.FillBuffer(buffer);
    fTitle.FillBuffer(buffer);
}

// --------------------------------------------------------------------------
//
// Returns the name of the object. If the name of the object is not the
// class name it returns the object name and in []-brackets the class name.
//
const TString MParContainer::GetDescriptor() const
{
    return GetDescriptor(*this);
}

// --------------------------------------------------------------------------
//
// Returns the name of the object. If the name of the object is not the
// class name it returns the object name and in []-brackets the class name.
//
const TString MParContainer::GetDescriptor(const TObject &o)
{
    //
    // Because it returns a (const char*) we cannot return a casted
    // local TString. The pointer would - immediatly after return -
    // point to a random memory segment, because the TString has gone.
    //
    return (TString)o.GetName()==o.ClassName() ? (TString)o.ClassName() :
        MString::Format("%s [%s]", o.GetName(), o.ClassName());
}

// --------------------------------------------------------------------------
//
// Check down to base whether the class given in the argument
// overwrites the function given by name.
//
// This function calls itself recursively. If you want to call it,
// leave out the argument.
//
Bool_t MParContainer::Overwrites(const TClass *base, const TObject &obj, const char *name, TClass *cls)
{
    if (!cls)
        cls = obj.IsA();

    //
    // Check whether we reached the base class MTask
    //
    if (cls==base)
        return kFALSE;

    //
    // Check whether the class cls overwrites Process
    //
    if (cls->GetMethodAny(name))
        return kTRUE;

    //
    // If the class itself doesn't overload it check all it's base classes
    //
    TBaseClass *basecls=NULL;
    TIter NextBase(cls->GetListOfBases());
    while ((basecls=(TBaseClass*)NextBase()))
    {
        if (Overwrites(base, obj, name, basecls->GetClassPointer()))
            return kTRUE;
    }

    return kFALSE;
}


// --------------------------------------------------------------------------
//
//  Return a unique name for this container. It is created from
//  the container name and the unique Id. (This is mostly used
//  in the StreamPrimitive member functions)
//
const TString MParContainer::GetUniqueName() const
{
    TString ret = ToLower(fName);

    if (isdigit(ret[ret.Length()-1]))
        ret+="_";

    ret+=GetUniqueID();

    return ret;
}

// --------------------------------------------------------------------------
//
//  List MParContainer name and title.
//
void MParContainer::ls(Option_t *) const
{
    TROOT::IndentLevel();
    *fLog << all << GetDescriptor() << " " << GetTitle() << ": kCanDelete=";
    *fLog << Int_t(TestBit(kCanDelete)) << endl;
}

// --------------------------------------------------------------------------
//
//  Print MParContainer name and title.
//
void MParContainer::Print(Option_t *) const
{
    *fLog << all << GetDescriptor() << " " << GetTitle() << endl;
}

// --------------------------------------------------------------------------
//
//  Change (i.e. set) the name of the MParContainer.
//  WARNING !!
//  If the object is a member of a THashTable, THashList container
//  The HashTable must be Rehashed after SetName
//  For example the list of objects in the current directory is a THashList
//
void MParContainer::SetName(const char *name)
{
    fName = name;
    ResetBit(kIsSavedAsPrimitive);
    if (gPad && TestBit(kMustCleanup)) gPad->Modified();
}

// --------------------------------------------------------------------------
//
//  Change (i.e. set) all the MParContainer parameters (name and title).
//  See also WARNING in SetName
//
void MParContainer::SetObject(const char *name, const char *title)
{
    fName  = name;
    fTitle = title;
    ResetBit(kIsSavedAsPrimitive);
    if (gPad && TestBit(kMustCleanup)) gPad->Modified();
}

// --------------------------------------------------------------------------
//
//  Change (i.e. set) the title of the MParContainer.
//
void MParContainer::SetTitle(const char *title)
{
    fTitle = title;
    ResetBit(kIsSavedAsPrimitive);
    if (gPad && TestBit(kMustCleanup)) gPad->Modified();
}

// --------------------------------------------------------------------------
//
//  Return size of the MParContainer part of the TObject.
//
Int_t MParContainer::Sizeof() const
{
    Int_t nbytes = fName.Sizeof() + fTitle.Sizeof();
    return nbytes;
}

// --------------------------------------------------------------------------
//
//  Read an object from the current directory. If no name is given
// the name of this object is used. The final object will have the
// name of the object read from file.
//
Int_t MParContainer::Read(const char *name)
{
    const Int_t rc = TObject::Read(name?name:(const char*)fName);
    if (name)
        SetName(name);
    return rc;
}

// --------------------------------------------------------------------------
//
//  If you want to use Ascii-Input/-Output (eg. MWriteAsciiFile) of a
//  container, overload this function.
//
void MParContainer::AsciiRead(istream &)
{
    *fLog << warn << "To use the the ascii input of " << GetName();
    *fLog << " you have to overload " << ClassName() << "::AsciiRead." << endl;
}

// --------------------------------------------------------------------------
//
//  Write out a data member given as a TDataMember object to an output stream.
//
Bool_t MParContainer::WriteDataMember(ostream &out, const TDataMember *member, Double_t scale) const
{
    if (!member)
        return kFALSE;

    if (!member->IsPersistent() || member->Property()&kIsStatic)
        return kFALSE;

    /*const*/ TMethodCall *call = const_cast<TDataMember*>(member)->GetterMethod(); //FIXME: Root
    if (!call)
    {
        *fLog << warn << "Sorry, no getter method found for " << member->GetName() << endl;
        return kFALSE;
    }

    // For debugging: out << member->GetName() << ":";

    switch (call->ReturnType())
    {
    case TMethodCall::kLong:
        Long_t l;
        call->Execute((void*)this, l); // FIXME: const, root
        out << l << " ";
        return kTRUE;

    case TMethodCall::kDouble:
        Double_t d;
        call->Execute((void*)this, d); // FIXME: const, root
        out << (scale*d) << " ";
        return kTRUE;

    default:
    //case TMethodCall::kString:
    //case TMethodCall::kOther:
        /* someone may want to enhance this? */
        return kFALSE;
    }
}

// --------------------------------------------------------------------------
//
//  Write out a data member given by name to an output stream.
//
Bool_t MParContainer::WriteDataMember(ostream &out, const char *member, Double_t scale) const
{
    /*const*/ TClass *cls = IsA()->GetBaseDataMember(member);
    if (!cls)
        return kFALSE;

    return WriteDataMember(out, cls->GetDataMember(member), scale);
}

// --------------------------------------------------------------------------
//
//  Write out a data member from a given TList of TDataMembers.
//  returns kTRUE when at least one member was successfully written
//
Bool_t MParContainer::WriteDataMember(ostream &out, const TList *list) const
{
    Bool_t rc = kFALSE;

    TDataMember *data = NULL;

    TIter Next(list);
    while ((data=(TDataMember*)Next()))
        rc |= WriteDataMember(out, data);

    return rc;
}

// --------------------------------------------------------------------------
//
//  If you want to use Ascii-Input/-Output (eg. MWriteAsciiFile) of a
//  container, you may overload this function. If you don't overload it
//  the data member of a class are written to the file in the order of
//  appearance in the class header (be more specfic: root dictionary)
//  Only data members which are of integer (Bool_t, Int_t, ...) or
//  floating point (Float_t, Double_t, ...) type are written.
//  returns kTRUE when at least one member was successfully written
//
Bool_t MParContainer::AsciiWrite(ostream &out) const
{
    // *fLog << warn << "To use the the ascii output of " << GetName();
    // *fLog << " you have to overload " << ClassName() << "::AsciiWrite." << endl;

    Bool_t rc = WriteDataMember(out, IsA()->GetListOfDataMembers());

    TIter NextBaseClass(IsA()->GetListOfBases());
    TBaseClass *base;
    while ((base = (TBaseClass*) NextBaseClass()))
    {
        /*const*/ TClass *cls = base->GetClassPointer();

        if (!cls)
            continue;

        if (cls->GetClassVersion())
            rc |= WriteDataMember(out, cls->GetListOfDataMembers());
    }

    return rc;
}

// --------------------------------------------------------------------------
//
// This virtual function is called for all parameter containers which are
// found in the parameter list automatically each time the tasklist is
// executed.
//
// By overwriting this function you can invalidate the contents of a
// container before each execution of the tasklist:
//
// For example:
//   void MHillas::Reset()
//   {
//      fWidth = -1;
//   }
//
// (While -1 is obviously a impossible value for fWidth you can immediatly
// see - if you Print() the contents of this container - that MHillasCalc
// has not caluclated the width in this runthrough of the tasklist)
//
// Overwriting MParConatiner::Reset() in your container makes it
// unnecessary to call any Clear() or Reset() manually in your task and
// you make sure, that you don't keep results of previous runs of your
// tasklist by chance.
//
// MParContainer::Reset() itself does nothing.
//
void MParContainer::Reset()
{
}

// --------------------------------------------------------------------------
//
// Return the pointer to the TClass (from the root dictionary) which
// corresponds to the class with name name.
//
// Make sure, that a new object of this type can be created.
//
// In case of failure return NULL
//
TClass *MParContainer::GetConstructor(const char *name) const
{
    //
    // try to get class from root environment
    //
    TClass *cls = gROOT->GetClass(name);
    Int_t rc = 0;
    if (!cls)
        rc =1;
    else
    {
        if (!cls->Property())
            rc = 5;
        if (!cls->Size())
            rc = 4;
        if (!cls->IsLoaded())
            rc = 3;
        if (!cls->HasDefaultConstructor())
            rc = 2;
    }

    if (!rc)
        return cls;

    *fLog << err << dbginf << GetDescriptor() << " - Cannot create new instance of class '" << name << "': ";
    switch (rc)
    {
    case 1:
        *fLog << "gROOT->GetClass() returned NULL." << endl;
        return NULL;
    case 2:
        *fLog << "no default constructor." << endl;
        return NULL;
    case 3:
        *fLog << "not loaded." << endl;
        return NULL;
    case 4:
        *fLog << "zero size." << endl;
        return NULL;
    case 5:
        *fLog << "no property." << endl;
        return NULL;
    }

    *fLog << "rtlprmft." << endl;

    return NULL;
}

// --------------------------------------------------------------------------
//
// Return a new object of class 'name'. Make sure that the object
// derives from the class base.
//
// In case of failure return NULL
//
// The caller is responsible of deleting the object!
//
MParContainer *MParContainer::GetNewObject(const char *name, TClass *base) const
{
    return base ? GetNewObject(name, base->GetName()) : 0;
}

// --------------------------------------------------------------------------
//
// Return a new object of class 'name'. Make sure that the object
// derives from the class base.
//
// In case of failure return NULL
//
// The caller is responsible of deleting the object!
//
MParContainer *MParContainer::GetNewObject(const char *name, const char *base) const
{
    TClass *cls = GetConstructor(name);
    if (!cls || !base)
        return NULL;

    if (!cls->InheritsFrom(base))
    {
        *fLog << err;
        *fLog << dbginf << GetDescriptor() << "Cannot create new instance of class '" << name << "': " << endl;
        *fLog << "Class " << cls->GetName() << " doesn't inherit from " << base << endl;
        return NULL;
    }

    //
    // create the parameter container of the the given class type
    //
    TObject *obj = (TObject*)cls->New();
    if (!obj)
    {
        *fLog << err;
        *fLog << dbginf << GetDescriptor() << " - Cannot create new instance of class '" << name << "': " << endl;
        *fLog << " - Class " << cls->GetName() << " has no default constructor." << endl;
        *fLog << " - An abstract member functions of a base class is not overwritten." << endl;
        return NULL;
    }

    return (MParContainer*)obj;
}

TMethodCall *MParContainer::GetterMethod(const char *name) const
{
    const TString n(name);
    const Int_t pos1 = n.First('.');

    const TString part1 = pos1<0 ? n : n(0, pos1);
    const TString part2 = pos1<0 ? TString("") : n(pos1+1, n.Length());

    TClass *cls = IsA()->GetBaseDataMember(part1);
    if (cls)
    {
        TDataMember *member = cls->GetDataMember(part1);
        if (!member)
        {
            *fLog << err << "Datamember '" << part1 << "' not in " << GetDescriptor() << endl;
            return NULL;
        }

        // This handles returning references of contained objects, eg
        // class X { TObject fO; TObject &GetO() { return fO; } };
        if (!member->IsBasic() && !part2.IsNull())
        {
            cls = gROOT->GetClass(member->GetTypeName());
            if (!cls)
            {
                *fLog << err << "Datamember " << part1 << " [";
                *fLog << member->GetTypeName() << "] not in dictionary." << endl;
                return NULL;
            }
            if (!cls->InheritsFrom(MParContainer::Class()))
            {
                *fLog << err << "Datamember " << part1 << " [";
                *fLog << member->GetTypeName() << "] does not inherit from ";
                *fLog << "MParContainer." << endl;
                return NULL;
            }

            const MParContainer *sub = (MParContainer*)((ULong_t)this+member->GetOffset());
            return sub->GetterMethod(part2);
        }

        if (member->IsaPointer())
        {
            *fLog << warn << "Data-member " << part1 << " is a pointer..." << endl;
            *fLog << dbginf << "Not yet implemented!" << endl;
            //TClass *test = gROOT->GetClass(member->GetTypeName());
            return 0;
        }

        TMethodCall *call = member->GetterMethod();
        if (call)
            return call;
    }

    *fLog << inf << "No standard access for '" << part1 << "' in ";
    *fLog << GetDescriptor() << " or one of its base classes." << endl;

    TMethodCall *call = NULL;

    *fLog << "Trying to find MethodCall '" << ClassName();
    *fLog << "::" << part1 << "' instead..." << flush;
    call = new TMethodCall(IsA(), part1, "");
    if (call->GetMethod())
    {
        fgListMethodCall.Add(call);
        *fLog << "found." << endl;
        return call;
    }
    *fLog << endl;

    delete call;

    *fLog << "Trying to find MethodCall '" << ClassName();
    *fLog << "::Get" << part1 << "' instead..." << flush;
    call = new TMethodCall(IsA(), (TString)"Get"+part1, "");
    if (call->GetMethod())
    {
        fgListMethodCall.Add(call);
        *fLog << "found." << endl;
        return call;
    }
    *fLog << endl;

    delete call;

    *fLog << err << "Sorry, no getter method found for " << part1 << endl;
    return NULL;
}

void *MParContainer::DataMember(const char *member)
{
    TDataMember *m = IsA()->GetDataMember(member);
    if (m)
    {
        const Long_t off = m->GetOffset();
        if (off>0)
            return reinterpret_cast<char*>(this)+off;
    }

    *fLog << err << "Data member '" << member << "' not found in " << GetDescriptor() << endl;
    return 0;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MParContainer::SavePrimitive(ostream &out, Option_t *)
{
    static UInt_t uid = 0;

    if (IsSavedAsPrimitive())
        return;

    SetUniqueID(uid++);
    SetBit(kIsSavedAsPrimitive);

    if (gListOfPrimitives && !gListOfPrimitives->FindObject(this))
        gListOfPrimitives->Add(this);

    StreamPrimitive(out);
}

void MParContainer::SavePrimitive(ofstream &out, Option_t *)
{
    SavePrimitive(static_cast<ostream&>(out));
}

// --------------------------------------------------------------------------
//
// Creates the string written by SavePrimitive and returns it.
//
void MParContainer::StreamPrimitive(ostream &out) const
{
    out << "   // Using MParContainer::StreamPrimitive" << endl;
    out << "   " << ClassName() << " " << GetUniqueName() << "(\"";
    out << fName << "\", \"" << fTitle << "\");" << endl;
}

void MParContainer::GetNames(TObjArray &arr) const
{
    arr.AddLast(new TNamed(fName, fTitle));
}

void MParContainer::SetNames(TObjArray &arr)
{
    TNamed *name = (TNamed*)arr.First();

    fName  = name->GetName();
    fTitle = name->GetTitle();

    delete arr.Remove(name);
    arr.Compress();
}

// --------------------------------------------------------------------------
//
// Creates a new instance of this class. The idea is to create a clone of
// this class in its initial state.
//
MParContainer *MParContainer::New() const
{
    return (MParContainer*)IsA()->New();
}

// --------------------------------------------------------------------------
//
// Check if an object can be created through gROOT->GetClass(classname)
// return the correspodning TClass or NULL if this is not possible.
// A message containing the reason is returned in msg.
//
TClass *MParContainer::GetClass(const char *classname, TString &msg)
{
    TClass *cls = gROOT->GetClass(classname);
    Int_t rcc = 0;
    if (!cls)
        rcc = 1;
    else
    {
        if (!cls->Property())
            rcc = 5;
        if (!cls->Size())
            rcc = 4;
        if (!cls->IsLoaded())
            rcc = 3;
        if (!cls->HasDefaultConstructor())
            rcc = 2;
    }

    // Everything is ok.
    if (rcc==0)
        return cls;

    msg += "Cannot create instance of class '";
    msg += classname;
    msg += "': ";

    switch (rcc)
    {
    case 1:
        msg += "gROOT->GetClass() returned NULL.";
        break;
    case 2:
        msg += "no default constructor.";
        break;
    case 3:
        msg += "not loaded.";
        break;
    case 4:
        msg += "zero size.";
        break;
    case 5:
        msg += "no property.";
        break;
    default:
        msg += "Unknown error.";
        break;
    }

    return 0;
}

// --------------------------------------------------------------------------
//
// Check if an object can be created through gROOT->GetClass(classname)
// return the correspodning TClass or NULL if this is not possible.
// A message with the reason is ouput.
//
TClass *MParContainer::GetClass(const char *classname, MLog *log)
{
    TString msg;
    TClass *cls = GetClass(classname, msg);

    if (!cls && log)
        *log << msg << endl;

    return cls;
}


// --------------------------------------------------------------------------
//
// Read the contents/setup of a parameter container/task from a TEnv
// instance (steering card/setup file).
// The key to search for in the file should be of the syntax:
//    prefix.vname
// While vname is a name which is specific for a single setup date
// (variable) of this container and prefix is something like:
//    evtloopname.name
// While name is the name of the containers/tasks in the parlist/tasklist
//
// eg.  Job4.MImgCleanStd.CleaningLevel1:  3.0
//      Job4.MImgCleanStd.CleaningLevel2:  2.5
//
// If this cannot be found the next step is to search for
//      MImgCleanStd.CleaningLevel1:  3.0
// And if this doesn't exist, too, we should search for:
//      CleaningLevel1:  3.0
//
// Warning: The programmer is responsible for the names to be unique in
//          all Mars classes.
//
// Return values:
//  kTRUE:  Environment string found
//  kFALSE: Environment string not found
//  kERROR: Error occured, eg. environment invalid
//
// Overload this if you don't want to control the level of setup-string. In
// this case ReadEnv gets called with the different possibilities, see TestEnv.
//
Int_t MParContainer::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    if (!IsEnvDefined(env, prefix, "", print))
        return kFALSE;

    *fLog << warn << "WARNING - " << fName << ": Resource " << prefix << " found, but no " << ClassName() << "::ReadEnv." << endl;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Write the contents/setup of a parameter container/task to a TEnv
// instance (steering card/setup file).
// The key to search for in the file should be of the syntax:
//    prefix.vname
// While vname is a name which is specific for a single setup date
// (variable) of this container and prefix is something like:
//    evtloopname.name
// While name is the name of the containers/tasks in the parlist/tasklist
//
// eg.  Job4.MImgCleanStd.CleaningLevel1:  3.0
//      Job4.MImgCleanStd.CleaningLevel2:  2.5
//
// If this cannot be found the next step is to search for
//      MImgCleanStd.CleaningLevel1:  3.0
// And if this doesn't exist, too, we should search for:
//      CleaningLevel1:  3.0
//
// Warning: The programmer is responsible for the names to be unique in
//          all Mars classes.
//
Bool_t MParContainer::WriteEnv(TEnv &env, TString prefix, Bool_t print) const
{
    if (!IsEnvDefined(env, prefix, "", print))
        return kFALSE;

    *fLog << warn << "WARNING - Resource " << prefix+fName << " found, but " << ClassName() << "::WriteEnv not overloaded." << endl;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Take the prefix and call ReadEnv for:
//   prefix.containername
//   prefix.classname
//   containername
//   classname
//
//  The existance of an environment variable is done in this order. If
//  ReadEnv return kTRUE the existance of the container setup is assumed and
//  the other tests are skipped. If kFALSE is assumed the sequence is
//  continued. In case of kERROR failing of the setup from a file is assumed.
//
// Overload this if you want to control the handling of level of setup-string
// mentioned above. In this case ReadEnv gets never called if you don't call
// it explicitly.
//
Int_t MParContainer::TestEnv(const TEnv &env, TString prefix, Bool_t print)
{
    if (print)
        *fLog << all << "Testing Prefix+ContName: " << prefix+GetName() << endl;
    Int_t rc = ReadEnv(env, prefix+GetName(), print);
    if (rc==kERROR || rc==kTRUE)
        return rc;

    // Check For: Job4.MClassName.Varname
    if (print)
        *fLog << all << "Testing Prefix+ClassName: " << prefix+ClassName() << endl;
    rc = ReadEnv(env, prefix+ClassName(), print);
    if (rc==kERROR || rc==kTRUE)
        return rc;

    // Check For: ContainerName.Varname
    if (print)
        *fLog << all << "Testing ContName: " << GetName() << endl;
    rc = ReadEnv(env, GetName(), print);
    if (rc==kERROR || rc==kTRUE)
        return rc;

    // Check For: MClassName.Varname
    if (print)
        *fLog << all << "Testing ClassName: " << ClassName() << endl;
    rc = ReadEnv(env, ClassName(), print);
    if (rc==kERROR || rc==kTRUE)
        return rc;

    // Not found
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Check if the given resource is defined. If there is a postfix, prefix
// the postfix with a dot. Calls IsEnvDefined(env, name, print)
//
Bool_t MParContainer::IsEnvDefined(const TEnv &env, TString prefix, TString postfix, Bool_t print) const
{
    if (!postfix.IsNull())
        postfix.Insert(0, ".");

    return IsEnvDefined(env, prefix+postfix, print);
}

// --------------------------------------------------------------------------
//
// If print==kTRUE print information about what's going on. This is necessary
// to debug parsing of resource files. Check if a resource "name" is defined
// and return kFALSE/kTRUE depending on the result.
//
Bool_t MParContainer::IsEnvDefined(const TEnv &env, TString name, Bool_t print) const
{
    if (print)
        *fLog << all << GetDescriptor() << " - " << name << "... " << flush;

    if (!const_cast<TEnv&>(env).Defined(name))
    {
        if (print)
            *fLog << "not found." << endl;
        return kFALSE;
    }

    if (print)
        *fLog << "found." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Return the resource prefix+"."+postfix from env or deftl if not available.
// If prefix IsNull search for postfix only.
//
Int_t MParContainer::GetEnvValue(const TEnv &env, TString prefix, TString postfix, Int_t dflt) const
{
    return GetEnvValue(env, prefix.IsNull()?postfix:(prefix+"."+postfix), dflt);
}

// --------------------------------------------------------------------------
//
// Return the resource prefix+"."+postfix from env or deftl if not available.
// If prefix IsNull search for postfix only.
//
Double_t MParContainer::GetEnvValue(const TEnv &env, TString prefix, TString postfix, Double_t dflt) const
{
    return GetEnvValue(env, prefix.IsNull()?postfix:(prefix+"."+postfix), dflt);
}

// --------------------------------------------------------------------------
//
// Return the resource prefix+"."+postfix from env or deftl if not available.
// If prefix IsNull search for postfix only.
//
const char *MParContainer::GetEnvValue(const TEnv &env, TString prefix, TString postfix, const char *dflt) const
{
    return GetEnvValue(env, prefix.IsNull()?postfix:(prefix+"."+postfix), dflt);
}

// --------------------------------------------------------------------------
//
// Return the resource prefix from env or deftl if not available.
//
Int_t MParContainer::GetEnvValue(const TEnv &env, TString prefix, Int_t dflt) const
{
    return const_cast<TEnv&>(env).GetValue(prefix, dflt);
}

// --------------------------------------------------------------------------
//
// Return the resource prefix from env or deftl if not available.
//
Double_t MParContainer::GetEnvValue(const TEnv &env, TString prefix, Double_t dflt) const
{
    return const_cast<TEnv&>(env).GetValue(prefix, dflt);
}

// --------------------------------------------------------------------------
//
// Return the resource prefix from env or deftl if not available.
//
const char *MParContainer::GetEnvValue(const TEnv &env, TString prefix, const char *dflt) const
{
    return const_cast<TEnv&>(env).GetValue(prefix, dflt);
}

// --------------------------------------------------------------------------
//
// Check for the resource prefix+"."+postfix. If it is not available or
// prefix IsNull check for the more common resource postfix. If none
// is found return the default.
//
template <class T>
T MParContainer::GetEnvValue2Imp(const TEnv &env, const TString &prefix, const TString &postfix, T dftl, Bool_t print) const
{
    // Check for a dedicated resource (prefix.postfix) first
    if (!prefix.IsNull())
    {
        if (IsEnvDefined(env, prefix, postfix, print))
            return GetEnvValue(env, prefix, postfix, dftl);
    }

    // check for a general resource (postfix)
    if (IsEnvDefined(env, postfix, print))
        return GetEnvValue(env, postfix, dftl);

    // return default
    return dftl;
}

// --------------------------------------------------------------------------
//
// see template GetEnvValue2Imp
//
const char *MParContainer::GetEnvValue2(const TEnv &env, const TString &prefix, const TString &postfix, const char *dftl, Bool_t print) const
{
    return GetEnvValue2Imp(env, prefix, postfix, dftl, print);
}

// --------------------------------------------------------------------------
//
// see template GetEnvValue2Imp
//
Int_t MParContainer::GetEnvValue2(const TEnv &env, const TString &prefix, const TString &postfix, Int_t dftl, Bool_t print) const
{
    return GetEnvValue2Imp(env, prefix, postfix, dftl, print);
}

// --------------------------------------------------------------------------
//
// see template GetEnvValue2Imp
//
Double_t MParContainer::GetEnvValue2(const TEnv &env, const TString &prefix, const TString &postfix, Double_t dftl, Bool_t print) const
{
    return GetEnvValue2Imp(env, prefix, postfix, dftl, print);
}

// --------------------------------------------------------------------------
//
// This is a wrapper which checks the resource file for an id containing
// a %d with different numbers of leading zeros (1 to 8).
//
// If athe entries in the resource file are not unambiguous a warning
// is printed.
//
TString MParContainer::GetEnvValue3(const TEnv &env, const TString &prefix, TString id, UInt_t num) const
{
    id.ReplaceAll("%d", "%%0%dd");

    TString rc;
    for (int i=1; i<9; i++)
    {
        const TString form = MString::Format(id.Data(), i);
        const TString res  = MString::Format(form.Data(), num);

        const TString str  = GetEnvValue2(env, prefix, res, "");

        if (str.IsNull())
            continue;

        if (rc.IsNull())
            rc = str;
        else
            *fLog << warn << "Entry " << res << " ambigous (was also found with less leading zeros)... ignored." << endl;
    }

    return rc;
}

// --------------------------------------------------------------------------
//
// If object to remove is fDisplay set fDisplay to NULL.
// If object to remove is fLog     set fLog     to NULL.
// Call TObject::RecursiveRemove
//
void MParContainer::RecursiveRemove(TObject *obj)
{
    if (obj==fDisplay)
        fDisplay=NULL;

    if (obj==fLog)
        fLog=NULL;

    if (fDisplay)
        fDisplay->RecursiveRemove(obj);

    if (fLog)
        fLog->RecursiveRemove(obj);

    TObject::RecursiveRemove(obj);
}

