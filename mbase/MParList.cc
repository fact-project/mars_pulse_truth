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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@uni-sw.gwdg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MParList                                                                //
//                                                                         //
// This class contains a list of different parameter containers.           //
//                                                                         //
// A parameter container is an object which is derived from                //
// MParContainer.                                                          //
//                                                                         //
// Normally a parameter container is used for data exchange between two    //
// tasks at runtime.                                                       //
//                                                                         //
// You can add every parameter container (Named object) to the             //
// instance and access it from somewhere else via its Name.                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MParList.h"

#include <fstream>     // ofstream, SavePrimitive

#include <TNamed.h>
#include <TClass.h>
#include <TOrdCollection.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MIter.h"
#include "MParEnv.h"
#include "MTaskList.h"

ClassImp(MParList);

using namespace std;

static const TString gsDefName  = "MParList";
static const TString gsDefTitle = "A list of Parameter Containers";

// --------------------------------------------------------------------------
//
//  creates an empty list
//
void MParList::Init(const char *name, const char *title)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    //
    // This sets a flag that the list is the owner, which means
    // that the destructor of the list deletes all it's objects
    //
    fContainer  = new TOrdCollection;
    fAutodelete = new TOrdCollection;

    gROOT->GetListOfCleanups()->Add(fContainer);
    gROOT->GetListOfCleanups()->Add(fAutodelete);
    fContainer->SetBit(kMustCleanup);
    fAutodelete->SetBit(kMustCleanup);
}


// --------------------------------------------------------------------------
//
//  default constructor
//  creates an empty list
//
MParList::MParList(const char *name, const char *title)
{
    Init(name, title);
}

// --------------------------------------------------------------------------
//
//  Copy constructor. It copies all entries of the parameter list, but it
//  takes care of, that the automatically created entries are only deleted
//  once. (doesn't copy the list which holds the automatically created
//  entries)
//
MParList::MParList(const MParList &ts, const char *name, const char *title) : MParContainer()
{
    Init(name, title);

    fContainer->AddAll(ts.fContainer);
}

// --------------------------------------------------------------------------
//
//  If the 'IsOwner' bit is set (via SetOwner()) all containers are deleted
//  by the destructor
//
MParList::~MParList()
{
    //
    // Case:
    //  1) MParList is owner of the containers:
    //     All container are stored in fContainer, and become deleted by
    //     'delete fContainer'. Some of these containers, which were
    //     created automatically are stored in fAutodelete, too. To prevent
    //     double deletion this containers are not deleted by the destructor
    //     of fAutodelete.
    //  2) MParList is not owner of the containers:
    //     The containers which were Added by AddToList are not touched.
    //     Only the containers which were created automatically are also
    //     automatically deleted.
    //
    IsOwner() ? fContainer->SetOwner() : fAutodelete->SetOwner();

    TIter Next(fContainer);
    TObject *o=0;
    while ((o=Next()))
        if (o->TestBit(kCanDelete))
            delete fContainer->Remove(o);

    // FIXME? If fContainer is owner do we have to remove the object
    //   from fAutodelete due to the access when checking for a
    //   garbage collection?
    delete fContainer;
    delete fAutodelete;
}

// --------------------------------------------------------------------------
//
//  If the 'IsOwner' bit is set (via SetOwner()) all containers are deleted
//  by the destructor
//
void MParList::SetOwner(Bool_t enable)
{
    enable ? SetBit(kIsOwner) : ResetBit(kIsOwner);
}

// --------------------------------------------------------------------------
//
//  Set the logging streamer of the parameter list and all contained
//  parameter containers
//
void MParList::SetLogStream(MLog *log)
{
    fContainer->R__FOR_EACH(MParContainer, SetLogStream)(log);
    MParContainer::SetLogStream(log);
}

void MParList::SetDisplay(MStatusDisplay *d)
{
    fContainer->R__FOR_EACH(MParContainer, SetDisplay)(d);
    MParContainer::SetDisplay(d);
}

// --------------------------------------------------------------------------
//
//  Add a single container to the list.
//
//  If 'where' is given, the object will be added after this.
//
Bool_t MParList::AddToList(MParContainer *cont, MParContainer *where)
{
    //
    //  check if the object (you want to add) exists
    //
    if (!cont)
        return kFALSE;

    if (cont==this)
    {
        *fLog << err << dbginf << "Error: It is not allowed to add a parameter list to itself." << endl;
        return kFALSE;
    }

    //
    // Get Name of new container
    //
    const char *name = cont->GetName();

    //
    // Check if the new container is already existing in the list
    //
    const TObject *objn = fContainer->FindObject(name);
    const TObject *objt = fContainer->FindObject(cont);

    if (objn || objt)
    {
        //
        // If the container is already in the list ignore it.
        //
        if (objt || objn==cont)
        {
            *fLog << warn << dbginf << "Warning: Container '" << cont->GetName() << ", 0x" << (void*)cont;
            *fLog << "' already existing in '" << GetName() << "'... ignoring." << endl;
            return kTRUE;
        }

        //
        // Otherwise add it to the list, but print a warning message
        //
        *fLog << warn << dbginf << "Warning: Container with the same name '" << cont->GetName();
        *fLog << "' already existing in '" << GetName() << "'." << endl;
        *fLog << "You may not be able to get a pointer to container task by name." << endl;
    }

    //
    //  check if you want to add the new parameter container somewhere
    //  special (in that case you specify "where")
    //
    if (where)
    {
        if (!fContainer->FindObject(where))
        {
            *fLog << err << dbginf << "Error: Cannot find parameter container after which the new one should be added!" << endl;
            return kFALSE;
        }
    }

    if (!cont->InheritsFrom(MParContainer::Class()))
    {
        *fLog << err << dbginf << "Error: Cantainer MUST derive from MParContainer!" << endl;
        return kFALSE;
    }

    *fLog << inf3 << "Adding " << name << " to " << GetName() << "... " << flush;

    cont->SetBit(kMustCleanup);
    fContainer->Add(cont);
    *fLog << "done." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Add all entries which derive from MParContainer
// of the TObjArray to the list.
//
void MParList::AddToList(TObjArray *list)
{
    //
    //  check if the object (you want to add) exists
    //
    if (!list)
        return;

    TIter Next(list);

    TObject *cont = NULL;
    while ((cont=Next()))
    {
        MParContainer *par = dynamic_cast<MParContainer*>(cont);
        if (par)
            AddToList(par);
    }
}

// --------------------------------------------------------------------------
//
//  Find an object with the same name in the list and replace it with
//  the new one. If the kIsOwner flag is set and the object was not
//  created automatically, the object is deleted.
//
Bool_t MParList::Replace(MParContainer *cont)
{
    //
    //  check if the object (you want to add) exists
    //
    if (!cont)
        return kFALSE;

    TObject *obj = FindObject(cont->GetName());
    if (!obj)
    {
        *fLog << warn << "No object with the same name '";
        *fLog << cont->GetName() << "' in list... adding." << endl;
        return AddToList(cont);
    }

    fContainer->Remove(obj);

    if (IsOwner() && !fAutodelete->FindObject(obj))
        delete obj;

    *fLog << inf2 << "MParContainer '" << cont->GetName() << "' found and replaced..." << endl;

    return AddToList(cont);
}

// --------------------------------------------------------------------------
//
//  Find an object with the same name in the list and remove it.
//  If the kIsOwner flag is set and the object was not created
//  automatically, the object is deleted.
//
void MParList::Remove(MParContainer *cont)
{
    //
    //  check if the object (you want to add) exists
    //
    if (!cont)
        return;

    TObject *obj = fContainer->Remove(cont);

    fContainer->RecursiveRemove(obj);

    // if (!obj)
    // {
        //        *fLog << warn << "Object not found in list..." << endl;
    //    return;
    //}

//    *fLog << inf << "MParContainer '" << cont->GetName() << "' removed..." << endl;

    if (obj && IsOwner() && !fAutodelete->FindObject(obj))
        delete obj;
}

// --------------------------------------------------------------------------
//
// Call MParContainer::RecursiveRemove
// Call fContainer->RecursiveRemove
//
void MParList::RecursiveRemove(TObject *obj)
{
    MParContainer::RecursiveRemove(obj);

    if (obj==fContainer)
        fContainer = NULL;

    if (fContainer)
        fContainer->RecursiveRemove(obj);
}

// --------------------------------------------------------------------------
//
//  Find an object in the list.
//  'name' is the name of the object you are searching for.
//
TObject *MParList::FindObject(const char *name) const
{
    TObject *obj = fContainer->FindObject(name);
    const MParEnv *env = dynamic_cast<const MParEnv*>(obj);
    return env ? env->GetCont() : obj;
}

// --------------------------------------------------------------------------
//
//  check if the object is in the list or not
//
TObject *MParList::FindObject(const TObject *obj) const
{
    TObject *ret = fContainer->FindObject(obj);
    const MParEnv *env = dynamic_cast<const MParEnv*>(ret);
    return env ? env->GetCont() : ret;
}

// --------------------------------------------------------------------------
//
//  Find an object in the list and check for the correct inheritance.
//  'name' is the name of the object you are searching for.
//
// In words: Find object name and check whether it inherits from classname
//
TObject *MParList::FindObject(const char *name, const char *classname) const
{
    TObject *obj = FindObject(name);

    if (!obj)
        return NULL;

    if (obj->InheritsFrom(classname))
        return obj;

    *fLog << dbginf << warn << "Found object '" << name << "' doesn't ";
    *fLog << "inherit from " << "'" << classname << "' but from '";
    *fLog << obj->ClassName() << "'" << endl;
    return NULL;
}

// --------------------------------------------------------------------------
//
//  check if the object is in the list or not and check for the correct
//  inheritance
//
TObject *MParList::FindObject(const TObject *obj, const char *classname) const
{
    TObject *nobj = FindObject(obj);

    if (!nobj)
        return NULL;

    if (nobj->InheritsFrom(classname))
        return nobj;

    *fLog << dbginf << warn << "Found object '" << nobj->GetName() << "' ";
    *fLog << "doesn't inherit from " << "'" << classname << "'" << endl;
    return NULL;
}

// --------------------------------------------------------------------------
//
//  Searches for the tasklist tlist (default: MTaskList) and returns
//  a task with the given name found in this list. If one of both isn't
//  found NULL is returned
//
MTask *MParList::FindTask(const char *name, const char *tlist) const
{
    TObject *l = FindObject(tlist, "MTaskList");
    return (MTask*)(l ? l->FindObject(name) : NULL);
}

// --------------------------------------------------------------------------
//
//  Find a tasklist which contains a task with name name
//
MTaskList *MParList::FindTaskListWithTask(const char *name) const
{
    TIter Next(fContainer);
    TObject *o=0;
    while ((o=Next()))
    {
        MTaskList *l1 = dynamic_cast<MTaskList*>(o);
        if (!l1)
            continue;

        MTaskList *l2 = l1->FindTaskList(name);
        if (l2)
            return l2;
    }
    return 0;
}

// --------------------------------------------------------------------------
//
//  Find a tasklist which contains a task task
//
MTaskList *MParList::FindTaskListWithTask(const MTask *task) const
{
    TIter Next(fContainer);
    TObject *o=0;
    while ((o=Next()))
    {
        MTaskList *l1 = dynamic_cast<MTaskList*>(o);
        if (!l1)
            continue;

        MTaskList *l2 = l1->FindTaskList(task);
        if (l2)
            return l2;
    }
    return 0;
}

// --------------------------------------------------------------------------
//
//  returns the ClassName without anything which is behind that last ';' in
//  string.
//
TString MParList::GetClassName(const char *classname)
{
    TString cname(classname);
    const char *semicolon = strrchr(cname, ';');

    if (semicolon)
        cname.Remove(semicolon-cname);

    return cname;
}

// --------------------------------------------------------------------------
//
//  returns the ObjectName. It is created from a class and object name.
//  If no object name is given the objectname is the same than the
//  class name. Leading dots are removed from the object name
//
TString MParList::GetObjectName(const char *classname, const char *objname)
{
    TString cname(classname);
    const char *semicolon = strrchr(cname, ';');

    TString oname(objname ? objname : classname);

    if (semicolon)
    {
        //
        // Remove leading dots from objectname (eg. "MMcTrig;5.")
        //
        Int_t sz = oname.Sizeof()-2;

        while (sz>=0 && oname[sz]=='.')
            oname.Remove(sz--);
    }
    return oname;
}

// --------------------------------------------------------------------------
//
//  Find an object in the list.
//  'name' is the name of the object you are searching for.
//  If the object doesn't exist we try to create one from the
//  dictionary. If this isn't possible NULL is returned.
//
//  An object which was created automatically is deleted automatically in
//  the destructor of the parameter list, too. This means, that if an
//  object should survive (eg. Histograms) you MUST create it by yourself
//  and add it to the parameter list.
//
//  By default (you don't specify an object name) the object name is
//  the same as the classname
//
//  If the classname (default classname) is of the structure
//  "Name;something" - containing a semicolon - evarything which is
//  after the last appearance of a semicolon is stripped to get the
//  Name of the Class. Normally this is used to number your objects.
//  "Name;1", "Name;2", ... If a semicolon is detected leading dots
//  are stripped from the object-name (eg. "name;5.")
//
// In words: Create object of type classname and set its name to objname.
//           If an object with objname already exists return it.
//
MParContainer *MParList::FindCreateObj(const char *classname, const char *objname)
{
    //
    // If now object name (name of the object to identify it in the
    // List) is given use it's classname as the objectname
    //

    //
    // Check if the classname is a 'numbered' name (like: "MTime;2")
    // if so strip the number from the classname.
    //
    // Becareful: We check for the last occurance of a ';' only and we
    // also don't check if a number follows or something else.
    //
    // Rem: I use a TString to make the code more readyble and to get
    // the new object deleted automatically
    //
    TString cname = GetClassName(classname);
    TString oname = GetObjectName(classname, objname);

    //
    // Try to find a object with this object name which is already
    // in the List. If we can find one we are done.
    //
    MParContainer *pcont = (MParContainer*)FindObject(oname);

    if (pcont)
    {
        if (pcont->InheritsFrom(cname))
            return pcont;

        *fLog << err << "Warning: Object '" << oname << "' found in list doesn't inherit from " << cname << "." << endl;
        return NULL;
    }

    //
    // if object is not existing in the list try to create one
    //
    *fLog << inf2 << "Object '" << oname << "' ";
    if (oname!=cname)
        *fLog << "[" << cname << "] ";
    *fLog << "not yet in " << GetName() << "... creating." << endl;

    //
    // try to get class from root environment
    //
    *fLog << err;
    TClass *cls = GetClass(cname, fLog);
    if (!cls)
        return NULL;

    if (!cls->InheritsFrom(MParContainer::Class()))
    {
        *fLog << err << dbginf << "Cannot create new instance of class '" << cname << "': " << endl;
        *fLog << "Class doesn't inherit from MParContainer." << endl;
        return NULL;
    }

    //
    // create the parameter container of the the given class type
    //
    pcont = (MParContainer*)cls->New();
    if (!pcont)
    {
        *fLog << err << dbginf << "Cannot create new instance of class '" << cname << "': " << endl;
        *fLog << " - Class has no default constructor." << endl;
        *fLog << " - An abstract member functions of a base class is not overwritten." << endl;
        return NULL;
    }

    //
    // Set the name of the container
    //
    pcont->SetName(oname);

    //
    // Now add the object to the parameter list
    //
    AddToList(pcont);

    //
    // The object was automatically created. This makes sure, that such an
    // object is deleted together with the list
    //
    fAutodelete->Add(pcont);

    //
    //  Find an object in the list.
    //  'name' is the name of the object you are searching for.
    //
    return pcont;
}

// --------------------------------------------------------------------------
//
//   print some information about the current status of MParList
//
void MParList::Print(Option_t *) const
{
    *fLog << all << underline << GetDescriptor() << ":" << endl;

    MParContainer *obj = NULL;
    MIter Next(fContainer);
    while ((obj=Next()))
    {
        *fLog << " " << obj->GetDescriptor();
        if (fAutodelete->FindObject(obj))
            *fLog << " <autodel>";
        *fLog << endl;
    }
    *fLog << endl;
}

// --------------------------------------------------------------------------
//
//   Sets the flags off all containers in the list (and the list
//   itself) to unchanged
//
void MParList::SetReadyToSave(Bool_t flag)
{
    fContainer->R__FOR_EACH(MParContainer, SetReadyToSave)(flag);
    MParContainer::SetReadyToSave(flag);
}

// --------------------------------------------------------------------------
//
//   Reset all containers in the list
//
void MParList::Reset()
{
    fContainer->R__FOR_EACH(MParContainer, Reset)();
}

// --------------------------------------------------------------------------
//
//  This finds numbered objects. The objects are returned in a copy of a
//  TObjArray.
//
//  If from only is given (or to=0) object are assumed numbered
//  from 1 to from.
//
TObjArray MParList::FindObjectList(const char *name, UInt_t first, const UInt_t last) const
{
    TObjArray list;

    if (first>0 && last<first)
    {
        *fLog << err << dbginf << "Cannot create entries backwards (last<first)...skipped." << endl;
        return list;
    }

    const UInt_t len = strlen(name);

    char *auxname = new char[len+7];
    strcpy(auxname, name);

    if (first==0 && last!=0)
        first = 1;

    //
    // If only 'from' is specified the number of entries are ment
    //
    for (UInt_t i=first; i<=last; i++)
    {
        if (first!=0 || last!=0)
            sprintf(auxname+len, ";%d", i);

        TObject *obj = FindObject(auxname);
        if (!obj)
            continue;

        list.AddLast(obj);
    }
    delete auxname;

    return list;
}

// --------------------------------------------------------------------------
//
//  This finds numbered objects. The objects are returned in a copy of a
//  TObjArray. If one of the objects doesn't exist it is created from the
//  meaning of cname and oname (s. FindCreateObj)
//
//  If from only is given (or to=0) object are assumed numbered
//  from 1 to from.
//
TObjArray MParList::FindCreateObjList(const char *cname, UInt_t first, const UInt_t last, const char *oname)
{
    TObjArray list;

    if (first>0 && last<first)
    {
        *fLog << err << dbginf << "Cannot create entries backwards (last<first)...skipped." << endl;
        return list;
    }

    const UInt_t len = strlen(cname);

    char *auxname = new char[len+7];
    strcpy(auxname, cname);

    //
    // If only 'from' is specified the number of entries are ment
    //
    if (first==0 && last!=0)
        first = 1;

    for (UInt_t i=first; i<=last; i++)
    {
        if (first!=0 || last!=0)
            sprintf(auxname+len, ";%d", i);

        TObject *obj = FindCreateObj(auxname, oname);
        if (!obj)
            break;

        list.AddLast(obj);
    }
    delete auxname;

    return list;
}

// --------------------------------------------------------------------------
//
//  This finds numbered objects. The objects are returned in a copy of a
//  TObjArray. If one of the objects doesn't exist it is created from the
//  meaning of cname and oname (s. FindCreateObj)
//
//  If from only is given (or to=0) object are assumed numbered
//  from 1 to from.
//
//  Remark: Because it is static the object are only created and not added to
//  the parameter list. You must also take care of deleting these objects!
//  This function is mainly made for use in root macros. Don't use it in
//  compiled programs if you are not 100% sure what you are doing.
//
TObjArray MParList::CreateObjList(const char *cname, UInt_t first, const UInt_t last, const char *)
{
    TObjArray list;

    if (first>0 && last<first)
    {
        gLog << err << dbginf << "Cannot create entries backwards (last<first)...skipped." << endl;
        return list;
    }

    //
    // try to get class from root environment
    //
    gLog << err;
    TClass *cls = GetClass(cname, &gLog);
    if (!cls)
        return list;

    if (!cls->InheritsFrom(MParContainer::Class()))
    {
        gLog << err << dbginf << "Cannot create new instance of class '" << cname << "': " << endl;
        gLog << "Class doesn't inherit from MParContainer." << endl;
        return list;
    }

    const UInt_t len = strlen(cname);

    char *auxname = new char[len+7];
    strcpy(auxname, cname);

    //
    // If only 'from' is specified the number of entries are ment
    //
    if (first==0 && last!=0)
        first = 1;

    for (UInt_t i=first; i<=last; i++)
    {
        if (first!=0 || last!=0)
            sprintf(auxname+len, ";%d", i);

        //
        // create the parameter container of the the given class type
        //
        MParContainer *pcont = (MParContainer*)cls->New();
        if (!pcont)
        {
            gLog << err << dbginf << "Cannot create new instance of class '" << cname << "' (Maybe no def. constructor)" << endl;
            return list;
        }

        //
        // Set the name of the container
        //
        pcont->SetName(auxname);

        //
        // Add new object to the return list
        //
        list.AddLast(pcont);
    }
    delete auxname;

    return list;
}

void MParList::SavePrimitive(ostream &out, Option_t *)
{
    Bool_t saved = IsSavedAsPrimitive();

    MParContainer::SavePrimitive(out);

    MIter Next(fContainer);

    MParContainer *cont = NULL;
    while ((cont=Next()))
    {
        //
        // Because it was automatically created don't store its primitive
        // I guess it will be automatically created again
        //
        if (fAutodelete->FindObject(cont) || cont->IsSavedAsPrimitive())
            continue;

        cont->SavePrimitive(out, "");

        out << "   " << GetUniqueName() << ".";
        out << (cont->InheritsFrom("MTaskList") && saved ? "Replace" : "AddToList");
        out << "(&" << cont->GetUniqueName() << ");" << endl << endl;
    }
}

void MParList::SavePrimitive(ofstream &out, Option_t *o)
{
    SavePrimitive(static_cast<ostream&>(out), o);
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MParList::StreamPrimitive(ostream &out) const
{
    out << "   MParList " << GetUniqueName();
    if (fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << "(\"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\"";
        out <<")";
    }
    out << ";" << endl << endl;
}

// --------------------------------------------------------------------------
//
// Adds one TNamed object per object in the list. The TNamed object must
// be deleted by the user.
//
void MParList::GetNames(TObjArray &arr) const
{
    MParContainer::GetNames(arr);
    fContainer->R__FOR_EACH(MParContainer, GetNames)(arr);
}

// --------------------------------------------------------------------------
//
// Sets name and title of each object in the list from the objects in
// the array.
//
void MParList::SetNames(TObjArray &arr)
{
    MParContainer::SetNames(arr);
    fContainer->R__FOR_EACH(MParContainer, SetNames)(arr);
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
// And if this doesn't exist, too, we search for:
//      CleaningLevel1:  3.0
//
// Warning: The programmer is responsible for the names to be unique in
//          all Mars classes.
//
Int_t MParList::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    if (print)
        *fLog << all << "MParList::ReadEnv: " << prefix << endl;

    MParContainer *cont = NULL;

    MIter Next(fContainer);
    while ((cont=Next()))
    {
        if (cont->InheritsFrom("MTaskList"))
        {
            if (cont->ReadEnv(env, prefix, print)==kERROR)
                return kERROR;
            continue;
        }

        if (cont->TestEnv(env, prefix, print)==kERROR)
            return kERROR;
    }

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
// And if this doesn't exist, too, we search for:
//      CleaningLevel1:  3.0
//
// Warning: The programmer is responsible for the names to be unique in
//          all Mars classes.
//
Bool_t MParList::WriteEnv(TEnv &env, TString prefix, Bool_t print) const
{
    MParContainer *cont = NULL;

    MIter Next(fContainer);
    while ((cont=Next()))
        if (!cont->WriteEnv(env, prefix, print))
            return kFALSE;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Can be used to create an iterator over all containers, eg:
//   MParList plist;
//   TIter Next(plist); // Be aware: Use a object here rather than a pointer!
//   TObject *o=0;
//   while ((o=Next()))
//   {
//       [...]
//   }
//
MParList::operator TIterator*() const
{
    return new TOrdCollectionIter(fContainer);
}
