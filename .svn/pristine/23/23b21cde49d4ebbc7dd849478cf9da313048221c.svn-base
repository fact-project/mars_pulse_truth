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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MReadTree
//
// This tasks opens all branches in a specified tree and creates the
// corresponding parameter containers if not already existing in the
// parameter list.
//
// The Process function reads one events from the tree. To go through the
// events of one tree make sure that the event number is increased from
// outside. It makes also possible to go back by decreasing the number.
//
// If you don't want to start reading the first event you have to call
// MReadTree::SetEventNum after instantiating your MReadTree-object.
//
// To make reading much faster (up to a factor of 10 to 20) you can
// ensure that only the data you are really processing is enabled by
// calling MReadTree::UseLeaf.
//
// If the chain switches from one file to another file all
// TObject::Notify() functions are called of TObject objects which were
// added to the Notifier list view MReadTree::AddNotify. If MReadTree
// is the owner (viw MReadTree::SetOwner) all this objects are deleted
// by the destructor of MReadTree
//
//
// ToDo:
// -----
//  - Auto Scheme and Branch choosing doesn't work for memory trees
//
/////////////////////////////////////////////////////////////////////////////
#include "MReadTree.h"

#include <fstream>

#include <TFile.h>           // TFile::GetName
#include <TSystem.h>         // gSystem->ExpandPath
#include <TLeaf.h>
#include <TChainElement.h>
#include <TFriendElement.h>
#include <TOrdCollection.h>

#include "MChain.h"
#include "MFilter.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MParameters.h"
#include "MParEmulated.h"
#include "MStatusDisplay.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MReadTree);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default constructor. Use this constructor (ONLY) if you want to
//  access a memory tree (a TTree not stored in a file) created by
//  MWriteRootFile or manually.
//
MReadTree::MReadTree(TTree *tree)
    : fNumEntry(0), fNumEntries(0), fBranchChoosing(kFALSE), fAutoEnable(kTRUE)
{
    fName  = "MRead";
    fTitle = "Task to loop over all events in one single tree";

    fVetoList = new TList;
    fVetoList->SetOwner();

    fNotify = new TList;

    fChain = NULL;

    fTree = tree;
    SetBit(kChainWasChanged);

    if (fTree)
        fAutoEnable = kFALSE;
    /*
    if (!fTree)
        return;

    TIter Next(fTree->GetListOfBranches());
    TBranch *b=0;
    while ((b=(TBranch*)Next()))
        b->ResetAddress();
    */
}

// --------------------------------------------------------------------------
//
//  Constructor. It creates an TChain instance which represents the
//  the Tree you want to read and adds the given file (if you gave one).
//  More files can be added using MReadTree::AddFile.
//  Also an empty veto list is created. This list is used if you want to
//  veto (disable or "don't enable") a branch in the tree, it vetos also
//  the creation of the corresponding object.
//  An empty list of TObjects are also created. This objects are called
//  at any time the TChain starts to read from another file.
//
MReadTree::MReadTree(const char *tname, const char *fname,
                     const char *name, const char *title)
    : fNumEntry(0), fNumEntries(0), fBranchChoosing(kFALSE), fAutoEnable(kTRUE)
{
    fName  = name  ? name  : "MRead";
    fTitle = title ? title : "Task to loop over all events in one single tree";

    fVetoList = new TList;
    fVetoList->SetOwner();

    fNotify = new TList;

    //
    // open the input stream
    //
    fChain = new MChain(tname);
    fTree = fChain;

    // root 3.02:
    // In TChain::Addfile remove the limitation that the file name must contain
    // the string ".root". ".root" is necessary only in case one wants to specify
    // a Tree in a subdirectory of a Root file with eg, the format:

    if (fname)
        AddFile(fname);
}

Bool_t MReadTree::IsOwned(const TChainElement &e) const
{
    // We cannot be the owner, because it is a basic type like Int, etc.
    if (!e.GetBaddressIsPtr())
        return kFALSE;

    // We cannot be the owener, because there is no class
    // we could have created
    if (TString(e.GetBaddressClassName()).IsNull())
        return kFALSE;

    // Let's hope the best that we are the owner
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Destructor. It deletes the TChain and veto list object
//
MReadTree::~MReadTree()
{
    //
    // Delete all the pointers to pointers to the objects where the
    // branche data gets stored. FIXME: When PreProcessed twice this
    // creates a memory leak!
    //
    if (fChain) // FIXME: MEMORY LEAK for fTree!=0
    {
        TIter Next(fChain->GetStatus());

        // Pointers which we delete must be
        //  1) Really pointers
        //  2) Have a class name (belong to a loaded class)
        TChainElement *element = NULL;
        while ((element=(TChainElement*)Next()))
            if (IsOwned(*element))
                delete (void**)element->GetBaddress();

        //
        // Delete the chain and the veto list
        //
        delete fChain;
    }

    /* This is AND MUST be done in PostProcess. See there for more details
    else
    {
        TIter Next(fTree->GetListOfBranches());

        TBranch *element = NULL;
        while ((element=(TBranch*)Next()))
            if (element->GetAddress())
                delete (MParContainer**)element->GetAddress();
        }
    */

    delete fNotify;
    delete fVetoList;
}

Byte_t MReadTree::IsFileValid(const char *name)
{
    ifstream fin(name);
    if (!fin)
        return 0;

    Char_t c[4];
    fin.read(c, 4);
    if (!fin)
        return 0;

    return c[0]=='r'&& c[1]=='o' && c[2]=='o' && c[3]=='t' ? 1 : 0;
}

void MReadTree::GetListOfBranches(TList &list) const
{
    list.AddAll(fTree->GetListOfBranches());

    TIter NextF(fTree->GetListOfFriends());
    TFriendElement *e = 0;
    while ((e=(TFriendElement*)NextF()))
        list.AddAll(e->GetTree()->GetListOfBranches());
}

// --------------------------------------------------------------------------
//
// This check whether all branches in the tree have the same size. If
// this is not the case the events in the different branches cannot
// be ordered correctly.
//
Bool_t MReadTree::CheckBranchSize()
{
    Int_t entries = -1;

    TList list;
    GetListOfBranches(list);

    TIter Next(&list);
    TBranch *b = NULL;
    while ((b=(TBranch*)Next()))
    {
        if (entries>=0 && entries!=(Int_t)b->GetEntries())
        {
            *fLog << err << "ERROR - File corruption detected:" << endl;
            *fLog << "  Due to several circumstances  (such at a bug in MReadTree or wrong usgae of" << endl;
            *fLog << "  the file  UPDATE  mode)  you may have produced a file in which at least two" << endl;
            *fLog << "  branches have different number of  entries.  Sorry, but this combination of" << endl;
            *fLog << "  branches, trees and files is unusable." << endl;
            return kFALSE;
        }

        entries = (Int_t)b->GetEntries();
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  If the owner flag is set all TObjects which are scheduled via
//  AddNotify are deleted by the destructor of MReadTree
//
void MReadTree::SetOwner(Bool_t flag)
{
    flag ? fNotify->SetBit(kIsOwner) : fNotify->ResetBit(kIsOwner);
}

// --------------------------------------------------------------------------
//
//  This function is called each time MReadTree changes the file to read
//  from. It calls all TObject::Notify() functions which are scheduled
//  via AddNotify.
//
//  For unknown reasons root stores the SetAutoDelete-Flag in
//  a branch having a custom streamer (eg. MRawEvtData).
//  It is not allowed that a container which is in the parameter
//  list gets a new address. Therefor we reset all the autodel flags.
//
//  MAKE SURE THAT ALL YOUR CUSTOM STREAMERS TAKE CARE OF ALL MEMORY
//
Bool_t MReadTree::Notify()
{
    //
    // Do a consistency check for all branches
    //
    if (!CheckBranchSize())
        return kFALSE;

    *fLog << inf << GetDescriptor() << ": Next file #" << GetFileIndex();
    *fLog << " '" << GetFileName() << "' (next evt #";
    *fLog << GetNumEntry()-1 << ")" << endl;

    if (fDisplay)
    {
        TString txt = GetFileName();
        txt += " @ ";
        txt += GetNumEntry()-1;
        fDisplay->SetStatusLine2(txt);
    }

    //
    // For unknown reasons root stores the SetAutoDelete-Flag in
    // a branch having a custom streamer (eg. MRawEvtData).
    // It is not allowed that a container which is in the parameter
    // list gets a new address. Therefor we reset all the autodel flags.
    //
    // MAKE SURE THAT ALL YOUR CUSTOM STREAMERS TAKE CARE OF ALL MEMORY
    //
    TIter NextB(fTree->GetListOfBranches());
    TBranch *b=0;
    while ((b=(TBranch*)NextB()))
        if (b->IsAutoDelete())
        {
            *fLog << warn << "Branch " << b->GetName() << "->IsAutoDelete() set... resetting." << endl;
            b->SetAutoDelete(kFALSE);
        }

    if (!fNotify)
        return kTRUE;

    TIter NextN(fNotify);
    TObject *o=NULL;
    while ((o=NextN()))
        if (!o->Notify())
        {
            *fLog << err << "Calling Notify() for object " << o->GetName() << " failed... abort." << endl;
            return kFALSE;
        }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  If you want to read the given tree over several files you must add
//  the files here before PreProcess is called. Be careful: If the tree
//  doesn't have the same contents (branches) it may confuse your
//  program (trees which are are not existing in later files are not read
//  anymore, tree wich are not existing in the first file are never read)
//
//  Name may use the wildcarding notation, eg "xxx*.root" means all files
//  starting with xxx in the current file system directory.
//
//  AddFile returns the number of files added to the chain.
//
//  For more information see TChain::Add
//
Int_t MReadTree::AddFile(const char *fname, Int_t entries)
{
    if (!fChain)
    {
        *fLog << err << "MReadTree::AddFile - ERROR: You cannot add a file, because MReadTree" << endl;
        *fLog <<        "  is supposed to read its tree from memory or its default or its" << endl;
        *fLog <<        "  default constructor was called (no tree name was given)." << endl;
        return 0;
    }

    TString newname(fname); // char-array must overcome comming block
    gSystem->ExpandPathName(newname);

    //
    // FIXME! A check is missing whether the file already exists or not.
    //
    const Int_t numfiles = fChain->Add(newname, entries<0?TChain::kBigNumber:entries);

    //TIter Next(fChain->GetListOfFriends());
    //TFriendElement *el = 0;
    //while ((el=(TFriendElement*)Next()))
    //    static_cast<TChain*>(el->GetTree())->Add(newname, entries<0?TChain::kBigNumber:entries);

    if (numfiles>0)
    {
        SetBit(kChainWasChanged);
        if (numfiles>1)
            *fLog << inf3 << GetDescriptor() << ": Added " << fname << " <" << numfiles << ">" << endl;
    }
    else
        *fLog << warn << GetDescriptor() << ": WARNING - No files found at " << newname << endl;

    return numfiles;
}

Bool_t MReadTree::AddFriend(TChain &c)
{
//    if (!fChain)
//    {
//        *fLog << err << "MReadTree::AddFriend - ERROR: You cannot add a friend, because MReadTree" << endl;
//        *fLog <<        "  is supposed to read its tree from memory or its default or its" << endl;
//        *fLog <<        "  default constructor was called (no tree name was given)." << endl;
//        return 0;
//    }

    return fChain->AddFriend(&c, "")!=0;
}

/*
 // --------------------------------------------------------------------------
 //
 //
 Int_t MReadTree::AddFile(const TChainElement &obj)
 {
     return AddFile(obj.GetTitle(), obj.GetEntries());
 }
*/

// --------------------------------------------------------------------------
//
// Adds all files from another MReadTree to this instance
//
// Returns the number of file which were added
//
Int_t MReadTree::AddFiles(const MReadTree &read)
{
    if (!fChain)
    {
        *fLog << err << "MReadTree::AddFiles - ERROR: You cannot add a file, because MReadTree" << endl;
        *fLog <<        "                             handles a memory based tree or its default" << endl;
        *fLog <<        "                             constructor was called." << endl;
        return 0;
    }

    const Int_t rc = fChain->Add(read.fChain);

    if (rc>0)
        SetBit(kChainWasChanged);

    /*
     Int_t rc = 0;

     TIter Next(read.fChain->GetListOfFiles());
     TObject *obj = NULL;
     while ((obj=Next()))
         rc += AddFile(*(TChainElement*)obj);
    */

    return rc;
}

// --------------------------------------------------------------------------
//
//  Sort the files by their file-names
//
void MReadTree::SortFiles()
{
    if (fChain)
        fChain->GetListOfFiles()->Sort();
}

// --------------------------------------------------------------------------
//
//  This function is called if Branch choosing method should get enabled.
//  Branch choosing means, that only the enabled branches are read into
//  memory. To use an enableing scheme we have to disable all branches first.
//  This is done, if this function is called the first time.
//
void MReadTree::EnableBranchChoosing()
{
    if (fBranchChoosing)
        return;

    *fLog << inf << GetDescriptor() << ": Branch choosing enabled (only enabled branches are read)." << endl;
    fTree->SetBranchStatus("*", kFALSE); // *CHANGED-fChain-to-fTree*
    fBranchChoosing = kTRUE;
}

// --------------------------------------------------------------------------
//
// The first time this function is called all branches are disabled.
// The given branch is enabled. By enabling only the branches you
// are processing you can speed up your calculation many times (up to
// a factor of 10 or 20)
//
void MReadTree::EnableBranch(const char *name)
{
    if (!fChain)
    {
        *fLog << warn << "MReadTree::EnableBranch - WARNING: EnableBranch doesn't work with memory based trees... ignored." << endl;
        return;
    }

    if (fChain->GetListOfFiles()->GetEntries()==0)
    {
        *fLog << err << "Chain contains no file... Branch '";
        *fLog << name << "' ignored." << endl;
        return;
    }

    EnableBranchChoosing();

    TNamed branch(name, "");
    SetBranchStatus(&branch, kTRUE);
}

// --------------------------------------------------------------------------
//
// Set branch status of branch name
//
void MReadTree::SetBranchStatus(const char *name, Bool_t status)
{
    fTree->SetBranchStatus(name, status); // *CHANGED-fChain-to-fTree*

    *fLog << inf << (status ? "Enabled" : "Disabled");
    *fLog << " subbranch '" << name << "'." << endl;
}

// --------------------------------------------------------------------------
//
// Checks whether a branch with the given name exists in the chain
// and sets the branch status of this branch corresponding to status.
//
void MReadTree::SetBranchStatus(TObject *branch, Bool_t status)
{
    //
    // Get branch name
    //
    const char *name = branch->GetName();

    //
    // Check whether this branch really exists
    //
    TString bn(name);
    if (bn.EndsWith("*"))
        bn.Remove(bn.Length()-1);

    if (fTree->GetBranch(bn)) // *CHANGED-fChain-to-fTree*
        SetBranchStatus(name, status);

    //
    // Remove trailing '.' if one and try to enable the subbranch without
    // the master branch name. This is to be compatible with older mars
    // and camera files.
    //
    const char *dot = strrchr(name, '.');
    if (!dot)
        return;

    if (fTree->GetBranch(dot+1)) // *CHANGED-fChain-to-fTree*
        SetBranchStatus(dot+1, status);
}

// --------------------------------------------------------------------------
//
// Set the status of all branches in the list to status.
//
void MReadTree::SetBranchStatus(const TList *list, Bool_t status)
{
    //
    // Loop over all subbranches in this master branch
    //
    TIter Next(list);

    TObject *obj;
    while ((obj=Next()))
        SetBranchStatus(obj, status);
}

// --------------------------------------------------------------------------
//
//  This is the implementation of the Auto Enabling Scheme.
//  For more information see MTask::AddBranchToList.
//  This function loops over all tasks and its filters in the tasklist
//  and enables all branches which are requested by the tasks and its
//  filters.
//
//  To enable 'unknown' branches which are not in the branchlist of
//  the tasks you can call EnableBranch
//
void MReadTree::EnableBranches(MParList *plist)
{
    //
    // check whether branch choosing must be switched on
    //
    EnableBranchChoosing();

    //
    // request the tasklist from the parameter list.
    // FIXME: Tasklist can have a different name
    //
    const MTaskList *tlist = (MTaskList*)plist->FindObject("MTaskList");
    if (!tlist)
    {
        *fLog << warn << GetDescriptor() << "Cannot use auto enabeling scheme for branches. 'MTaskList' not found." << endl;
        return;
    }

    //
    // This loop is not necessary. We could do it like in the commented
    // loop below. But this loop makes sure, that we don't try to enable
    // one branch several times. This would not harm, but we would get
    // an output for each attempt. To have several outputs for one subbranch
    // may confuse the user, this we don't want.
    // This loop creates a new list of subbranches and for each branch
    // which is added we check before whether it already exists or not.
    //
    TList list;

    MTask *task;
    TIter NextTask(tlist->GetList());
    while ((task=(MTask*)NextTask()))
    {
        TObject *obj;

        TIter NextTBranch(task->GetListOfBranches());
        while ((obj=NextTBranch()))
            if (!list.FindObject(obj->GetName()))
                list.Add(obj);

        const MFilter *filter = task->GetFilter();

        if (!filter)
            continue;

        TIter NextFBranch(filter->GetListOfBranches());
        while ((obj=NextFBranch()))
            if (!list.FindObject(obj->GetName()))
                list.Add(obj);
    }

    SetBranchStatus(&list, kTRUE);
/*
    //
    // Loop over all tasks iand its filters n the task list.
    //
    MTask *task;
    TIter NextTask(tlist->GetList());
    while ((task=(MTask*)NextTask()))
    {
        SetBranchStatus(task->GetListOfBranches(), kTRUE);

        const MFilter *filter = task->GetFilter();
        if (!filter)
           continue;

        SetBranchStatus(filter->GetListOfBranches(), kTRUE);

    }
*/
}

// --------------------------------------------------------------------------
//
//  If the chain has been changed (by calling AddFile or using a file
//  in the constructors argument) the number of entries is newly
//  calculated from the files in the chain - this may take a while.
//  The number of entries is returned.
//
UInt_t MReadTree::GetEntries()
{
    if (TestBit(kChainWasChanged))
    {
        // *CHANGED-fChain-to-fTree*
        *fLog << inf << "Scanning chain " << fTree->GetName() << "... " << flush;
        fNumEntries = (UInt_t)fTree->GetEntries();
        *fLog << fNumEntries << " events found." << endl;
        ResetBit(kChainWasChanged);
    }
    return fNumEntries==TChain::kBigNumber ? 0 : fNumEntries;
}

// --------------------------------------------------------------------------
//
//  The disables all subbranches of the given master branch.
//
void MReadTree::DisableSubBranches(TBranch *branch)
{
    //
    // This is not necessary, it would work without. But the output
    // may confuse the user...
    //
    if (fAutoEnable || fBranchChoosing)
        return;

    SetBranchStatus(branch->GetListOfBranches(), kFALSE);
}

MParContainer *MReadTree::FindCreateObj(MParList &plist, const char *cname, const char *name)
{
    MParContainer *pcont=plist.FindCreateObj(cname, name);
    if (!pcont)
    {
        //
        // if class is not existing in the (root) environment
        // we cannot proceed reading this branch
        //
        *fLog << err << dbginf << "ERROR - Class '" << cname;
        *fLog << "' for " << name << " not existing in dictionary. Branch skipped." << endl;
        return 0;
    }

    fParList.Add(pcont);
    return pcont;
}

void *MReadTree::GetParameterPtr(MParList &plist, const TString &name, const char *parname)
{
    if (name=="Int_t"   || name=="UInt_t"   ||
        name=="Short_t" || name=="UShort_t" ||
        name=="Char_t"  || name=="UChar_t"  ||
        name=="Bool_t")
    {
        MParameterI *par = (MParameterI*)FindCreateObj(plist, "MParameterI", parname);
        return par ? par->GetPtr() : 0;
    }

    if (name=="Float_t" || name=="Double_t")
    {
        MParameterD *par = (MParameterD*)FindCreateObj(plist, "MParameterD", parname);
        return par ? par->GetPtr() : 0;
    }

    // MParContainer **pcont= new MParContainer*;
    // return FindCreateObj(plist, name, parname);

    MParEmulated *par = (MParEmulated*)FindCreateObj(plist, "MParEmulated", parname);
    if (!par)
        return 0;

    par->SetClassName(name);
    return par->GetPtr();
}

Bool_t MReadTree::SetBranchAddress(TBranch &b, void *ptr, const char *prefix, const char *type)
{
    if (!ptr)
    {
        DisableSubBranches(&b);
        return kFALSE;
    }

    if (fChain)
    {
        TChainElement *element = (TChainElement*)fChain->GetStatus()->FindObject(b.GetName());
        if (element && IsOwned(*element))
            *fLog << warn << "WARNING - Branch address for " << b.GetName() << " was already setup by the user." << endl;
    }

    const TString bname = b.GetName();
    const TString cname = type ? type : b.GetClassName();

    fTree->SetBranchAddress(bname, ptr);

    *fLog << inf2 << prefix << " address '" << bname << "' ";
    if (bname!=cname)
        *fLog << "[" << cname << "] ";
    *fLog << "setup for reading." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  The PreProcess loops (till now) over the branches in the given tree.
//  It checks if the corresponding containers (containers with the same
//  name than the branch name) are existing in the Parameter Container List.
//  If not, a container of objec type 'branch-name' is created (everything
//  after the last semicolon in the branch name is stripped). Only
//  branches which don't have a veto (see VetoBranch) are enabled If the
//  object isn't found in the root dictionary (a list of classes known by the
//  root environment) the branch is skipped and an error message is printed
//  out.
//  If a selector is specified it is preprocessed after the
//  MReadTree::PreProcess
//
Int_t MReadTree::PreProcess(MParList *pList)
{
    fTaskList = (MTaskList*)pList->FindObject("MTaskList");
    if (!fTaskList)
        *fLog << warn << "WARNING - Standard tasklist MTaskList not found... ignoring Stream-ID." << endl;

    if (!fTree || !fChain)
    {
        *fLog << err << "ERROR - Something went terribly wrong!" << endl;
        *fLog <<        "        Maybe you called the default constructor?" << endl;
        *fLog <<        "        Did you forget to give a tree name in the constructor?" << endl;
        return kFALSE;
    }

    //
    // Make sure, that all the following calls doesn't result in
    // Notifications. This may be dangerous, because the notified
    // tasks are not preprocessed.
    //
    fTree->SetNotify(NULL); //*CHANGED-fChain-to-fTree*

    //
    // It seems, that TFile and TTree are not completely independant if
    // many times the same file is opened (MReadReports) and some of
    // the files in the chains don't have one tree. TChain::fTreeNumber
    // is already set before LoadTree from GetFile is called and
    // it crashes. ResetTree makes sure, that the tree number is -1
    //
    if (fChain) // *CHANGED-fChain-to-fTree*
        fChain->ResetTree();
    // Maybe this would be enough, but the above might be safer...
    //if (fChain->GetTreeNumber()==0)
    //    fChain->ResetTree();

    //
    // check for files and for the tree!
    //
    if (fChain && !fChain->GetFile()) // *CHANGED-fChain-to-fTree*
    {
        *fLog << err << GetDescriptor() << ": No file or no tree with name " << fChain->GetName() << " in file." << endl;
        return kFALSE;
    }

    //
    // get number of events in this tree
    //
    if (!GetEntries())
    {
        *fLog << err << GetDescriptor() << ": No entries found in file(s)" << endl;
        return kFALSE;
    }

    //
    // output logging information
    //
    *fLog << inf << fNumEntries << " entries found in file(s)." << endl;

    // Get all branches of this tree and all friends
    TList blist;
    GetListOfBranches(blist);

    //
    // Get all branches of this tree and
    // create the Iterator to loop over all branches
    //
    TIter Next(&blist); // *CHANGED-fChain-to-fTree*
    TBranch *branch=NULL;

    Int_t num=0;

    //
    // loop over all tasks for processing
    //
    while ( (branch=(TBranch*)Next()) )
    {
        //
        // Get Name of Branch and Object
        //
        const char *bname = branch->GetName();

        TString oname(bname);
        if (oname.EndsWith("."))
            oname.Remove(oname.Length()-1);

        //
        // Check if enabeling the branch is allowed
        //
        if (fVetoList->FindObject(oname))
        {
            *fLog << inf << "Master branch " << bname << " has veto... skipped." << endl;
            DisableSubBranches(branch);
            continue;
        }

        // Branch already setup/ FIXME: WHAT IF THIS IS A TREE???
        if (fChain && fChain->GetStatus()->FindObject(branch->GetName()))
        {
            *fLog << warn << "WARNING - Branch " << branch->GetName() << " already setup." << endl;
            continue;
        }

        // Get the corresponding class
        const TString classname = branch->GetClassName();

        TClass *cls = gROOT->GetClass(classname);
        if (!cls)
        {
            // FIXME: With or without dot?
            TLeaf *l = branch->GetLeaf(branch->GetName()); // FIXME: 1st leaf?
            if (!l)
                continue;

            void *ptr = GetParameterPtr(*pList, l->GetTypeName(), oname);
            if (SetBranchAddress(*branch, ptr, "Leaf", l->GetTypeName()))
                num++;

            continue;
        }

        // The class is known in the dictionary and loaded!
        if (!cls->IsLoaded() || !cls->InheritsFrom(MParContainer::Class()))
        {
            void *ptr = GetParameterPtr(*pList, classname, oname);
            if (SetBranchAddress(*branch, ptr, "Emulated branch"))
                num++;
        }
        else
        {
            MParContainer **pcont= new MParContainer*;
            *pcont=FindCreateObj(*pList, classname, oname);
            if (SetBranchAddress(*branch, pcont, "Master branch"))
                num++;
        }
    }

    *fLog << inf2 << GetDescriptor() << " setup " << num << " master branches addresses." << endl;

    //
    // If auto enabling scheme isn't disabled, do auto enabling
    //
    if (fAutoEnable)
        EnableBranches(pList);

    //
    // Now we can start notifying. Reset tree makes sure, that TChain thinks
    // that the correct file is not yet initialized and reinitilizes it
    // as soon as the first event is read. This is necessary to call
    // the notifiers when the first event is read, but after the
    // PreProcess-function.
    //
    if (fChain)
        fChain->ResetTree(); // *CHANGED-fChain-to-fTree*

    fTree->SetNotify(this); // *CHANGED-fChain-to-fTree*

    const Int_t rc = GetSelector() ? GetSelector()->CallPreProcess(pList) : kTRUE;
    if (rc!=kTRUE || fChain)
        return rc;

    return Notify();
}

// --------------------------------------------------------------------------
//
//  Set the ready to save flag of all containers which branchaddresses are
//  set for. This is necessary to copy data.
//
void MReadTree::SetReadyToSave(Bool_t flag)
{
    fParList.R__FOR_EACH(MParContainer, SetReadyToSave)(flag);

    MTask::SetReadyToSave(flag);
}

// --------------------------------------------------------------------------
//
//  The Process-function reads one event from the tree (this contains all
//  enabled branches) and increases the position in the file by one event.
//  (Remark: The position can also be set by some member functions
//  If the end of the file is reached the Eventloop is stopped.
//  In case an event selector is given its value is checked before
//  reading the event. If it returns kAFLSE the event is skipped.
//
Int_t MReadTree::Process()
{
    if (GetSelector())
    {
        //
        // Make sure selector is processed
        //
        if (!GetSelector()->CallProcess())
        {
            *fLog << err << dbginf << "Processing Selector failed." << endl;
            return kFALSE;
        }

        //
        // Skip Event
        //
        if (!GetSelector()->IsConditionTrue())
        {
            fNumEntry++;
            return kCONTINUE;
        }
    }

    const Bool_t rc = fTree->GetEntry(fNumEntry++) != 0; // *CHANGED-fChain-to-fTree*

    if (fTaskList)
        fTaskList->SetStreamId(fTree->GetName()); // *CHANGED-fChain-to-fTree*

    if (rc)
    {
        SetReadyToSave();
        return kTRUE;
    }

    if (fChain)
    {
//        cout << "LE: " << (int)fChain->GetLastError() << endl;

        switch (fChain->GetLastError())
        {
        case MChain::kFatalError:
            *fLog << err << GetDescriptor() << " - ERROR: Notify() failed." << endl;
            return kERROR;
        case MChain::kCannotAccessFile:
            *fLog << err << GetDescriptor() << " - ERROR: TChain::LoadTree is unable to access requested file." << endl;
            return kERROR;
        case MChain::kCannotAccessTree:
            *fLog << err << GetDescriptor() << " - ERROR: TChain::LoadTree is unable to access requested tree." << endl;
            return kERROR;
        case MChain::kOutOfRange: // no more events available
            return kFALSE;
        case MChain::kNoError:    // go on!
            return kTRUE;
        }
    }

    return rc;
}

// --------------------------------------------------------------------------
//
//  If a selector is given the selector is post processed
//
Int_t MReadTree::PostProcess()
{
    // In the case of a memory tree I don't know how we can
    // make a decision in PreProcess between a self allocated
    // memory address or a pending address set long before.
    // So we delete the stuff in PostProcess and not the destructor
    // (which might seg fault if PreProcess has never been called)
    if (!fChain)
    {
        TIter Next(fTree->GetListOfBranches());
        TBranch *b=0;
        while ((b=(TBranch*)Next()))
        {
            if (b->GetAddress())
            {
                delete b->GetAddress();
                b->ResetAddress();
            }
        }
    }

    return GetSelector() ? GetSelector()->CallPostProcess() : kTRUE;
}

// --------------------------------------------------------------------------
//
// Get the Event with the current EventNumber fNumEntry
//
Bool_t MReadTree::GetEvent()
{
    Bool_t rc = fTree->GetEntry(fNumEntry) != 0; // *CHANGED-fChain-to-fTree*

    if (rc)
        SetReadyToSave();

    return rc;
}

// --------------------------------------------------------------------------
//
// Decrease the number of the event which is read by Process() next
// by one or more
//
Bool_t MReadTree::DecEventNum(UInt_t dec)
{
    if (fNumEntry-dec >= GetEntries())
    {
        *fLog << warn << GetDescriptor() << ": DecEventNum, WARNING - Event " << fNumEntry << "-";
        *fLog << dec << "=" << (Int_t)fNumEntry-dec << " out of Range (>=";
        *fLog << GetEntries() << ")." << endl;
        return kFALSE;
    }

    fNumEntry -= dec;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Increase the number of the event which is read by Process() next
// by one or more
//
Bool_t MReadTree::IncEventNum(UInt_t inc)
{
    if (fNumEntry+inc >= GetEntries())
    {
        *fLog << warn << GetDescriptor() << ": IncEventNum, WARNING - Event " << fNumEntry << "+";
        *fLog << inc << "=" << (Int_t)fNumEntry+inc << " out of Range (>=";
        *fLog << GetEntries() << ")." << endl;
        return kFALSE;
    }

    fNumEntry += inc;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// This function makes Process() read event number nr next
//
// Remark: You can use this function after instatiating you MReadTree-object
//         to set the event number from which you want to start reading.
//
Bool_t MReadTree::SetEventNum(UInt_t nr)
{
    if (nr!=0 && nr >= GetEntries())
    {
        *fLog << warn << GetDescriptor() << ": SetEventNum, WARNING - " << nr << " out of Range." << endl;
        return kFALSE;
    }

    fNumEntry = nr;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  For the branch with the given name:
//   1) no object is automatically created
//   2) the branch address for this branch is not set
//      (because we lack the object, see 1)
//   3) The whole branch (exactly: all its subbranches) are disabled
//      this means are not read in memory by TTree:GetEntry
//
void MReadTree::VetoBranch(const char *name)
{
    fVetoList->Add(new TNamed(name, ""));
}

// --------------------------------------------------------------------------
//
//  Return the name of the file we are actually reading from.
//
TString MReadTree::GetFullFileName() const
{
    const TFile *file = fChain ? fChain->GetFile() : fTree->GetCurrentFile();

    if (!file)
        return "<unknown>";

    return file->GetName();
}

// --------------------------------------------------------------------------
//
// Get number of files in chain. (-1 is returned if chain is not
// initialized.
//
Int_t MReadTree::GetNumFiles() const
{
    if (!fChain)
        return -1;

    return fChain->GetListOfFiles()->GetEntries();
}

// --------------------------------------------------------------------------
//
//  Return the number of the file in the chain, -1 in case of an error
//
Int_t MReadTree::GetFileIndex() const
{
    return fChain ? fChain->GetTreeNumber() : 0;
    /*
    const TString filename = fChain->GetFile()->GetName();

    int i=0;
    TObject *file = NULL;

    TIter Next(fChain->GetListOfFiles());
    while ((file=Next()))
    {
        if (filename==gSystem->ExpandPathName(file->GetTitle()))
            return i;
        i++;
    }
    return -1;
    */
}

// --------------------------------------------------------------------------
//
//  This schedules a TObject which Notify() function is called in case
//  of MReadTree (TChain) switches from one file in the chain to another
//  one.
//
void MReadTree::AddNotify(TObject *obj)
{
    fNotify->Add(obj);
}

void MReadTree::Print(Option_t *o) const
{
    *fLog << all << underline << GetDescriptor() << ":" << endl << dec;
    *fLog << " Files [Tree]:" << endl;

    int i = 0;
    TIter Next(fChain->GetListOfFiles());
    TObject *obj = NULL;
    while ((obj=Next()))
        *fLog << " " << i++ << ") " << obj->GetTitle() << " [" << obj->GetName() << "]" << endl;

    *fLog << " Total Number of Entries: " << fNumEntries << endl;
    *fLog << " Next Entry to read: " << fNumEntry << endl;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MReadTree::StreamPrimitive(ostream &out) const
{
    out << "   " << ClassName() << " " << GetUniqueName() << "(\"";
    out << fChain->GetName() << "\", \"" << fName << "\", \"" << fTitle << "\");" << endl;

    TIter Next(fChain->GetListOfFiles());
    TObject *obj = NULL;
    while ((obj=Next()))
        out << "   " << GetUniqueName() << ".AddFile(\"" << obj->GetTitle() << "\");" << endl;

    if (!fAutoEnable)
        out << "   " << GetUniqueName() << ".DisableAutoScheme();" << endl;

    if (fNumEntry!=0)
       out << "   " << GetUniqueName() << ".SetEventNum(" << fNumEntry << ");" << endl;


}
