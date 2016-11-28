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
!   Author(s): Thomas Bretz, 6/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MWriteRootFile
//
// This is a writer to store several containers to a root file.
// The containers are added with AddContainer.
// To understand how it works, see base class MWriteFile
//
// Warning: Look at the Warning in MTaskList.
//
// There is a special mode of operation which opens a new file for each new
// file read by the reading task (opening the new file is initiated by
// ReInit()) For more details see the corresponding constructor.
//
// Memory based trees
// ------------------
// It is possible to store the data into memory (TTrees) instead of
// writing the data into a file. To do this either call the default
// constructor or specify 'memory' as option in the constructor.
//
// Afterwards the tree can be found using
// gROOT->GetListOfFiles()->FindObject("treename")
//
// Currently(!) the tree is not deleted at all! Please make sure to
// delete it if it is not used anymore - otherwise you could wast a LOT
// of memory. Please consider that this behaviour might change in the
// future.
//
// Such trees are usefull if you want to use more basic root-tools
// like TMultiLayerPerceptron or TEventList.
//
// If you want to process such memory based Trees using Mars make sure,
// that you don't need data from the RunHeader tree because you can
// only use MReadTree but not MReadMarsFile with such a tree.
//
/////////////////////////////////////////////////////////////////////////////
#include "MWriteRootFile.h"

#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TPRegexp.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MRead.h"
#include "MParList.h"
#include "MStatusDisplay.h"

ClassImp(MRootFileBranch);
ClassImp(MWriteRootFile);

using namespace std;

const TString MWriteRootFile::gsDefName  = "MWriteRootFile";
const TString MWriteRootFile::gsDefTitle = "Task which writes a root-output file";

void MWriteRootFile::Init(const char *name, const char *title)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    //
    // Set the Arrays the owner of its entries. This means, that the
    // destructor of the arrays will delete all its entries.
    //
    fBranches.SetOwner();
    fCopies.SetOwner();

    //
    // Believing the root user guide, TTree instances are owned by the
    // directory (file) in which they are. This means we don't have to
    // care about their destruction.
    //
    //fTrees.SetOwner();

    gROOT->GetListOfCleanups()->Add(this); // To remove fOut if deleted
    SetBit(kMustCleanup);
}

// --------------------------------------------------------------------------
//
// Try to get the file from gROOT->GetListOfFiles. (In case the file name
// is /dev/null we look for a file with name /dev/null and the given title)
// If it is found fOut is set to it and returned.
// Otherwise a new file is opened and returned.
//
TFile *MWriteRootFile::OpenFile(const char *name, Option_t *option, const char *title, Int_t comp)
{
    TFile *file = 0;

    if (TString(name)=="/dev/null")
    {
        TIter Next(gROOT->GetListOfFiles());
        TObject *obj = 0;
        while ((obj=Next()))
            if (TString(obj->GetName())=="/dev/null" && TString(obj->GetTitle())==title)
            {
                *fLog << inf2 << "Found open file '/dev/null' <Title=" << title << ">... re-using." << endl;
                file = dynamic_cast<TFile*>(obj);
                break;
            }
    }
    else
    {
        file = dynamic_cast<TFile*>(gROOT->GetListOfFiles()->FindObject(name));

        // If the file was not found with its name try its expanded name
        if (!file)
        {
            TString fqp(name);
            gSystem->ExpandPathName(fqp);
            file = dynamic_cast<TFile*>(gROOT->GetListOfFiles()->FindObject(fqp));
        }

        if (file)
        {
            *fLog << inf2;
            *fLog << "Found open file '" << name << "'... re-using." << endl;
            *fLog << inf3;
            *fLog << "Make sure that you do NOT write to trees which are" << endl;
            *fLog << "scheduled already by a different MWriteRootFile..." << endl;
        }
    }

    if (!file)
    {
        file = new TFile(name, option, title, comp);
        if (!file->IsOpen())
        {
            delete file;
            return NULL;
        }

        *fLog << inf3 << "File '" << name << "' ";
        if (!TString(title).IsNull())
            *fLog << "<Title=" << title << "> ";
        *fLog << "opened [" << option << "]" << endl;

        file->SetOption(option); // IMPORTANT!
        file->SetBit(kMustCleanup);
        ResetBit(kIsNotOwner);
        return file;
    }

    fOut = file;
    fOut->SetBit(kMustCleanup);
    SetBit(kIsNotOwner);

    return fOut;
}

// --------------------------------------------------------------------------
//
// Default constructor. It is there to support some root stuff.
// Don't use it.
//
MWriteRootFile::MWriteRootFile() : fOut(NULL)
{
    Init();
}

// --------------------------------------------------------------------------
//
// Use this constructor to run in a special mode.
//
// In this mode for each input file a new output file is written. This
// happens in ReInit.
//
// comp:        Compression Level (see TFile, TBranch)
// rule:        Rule to create output file name (see SubstituteName())
// overwrite:   Allow newly created file to overwrite old files ("RECREATE")
// ftitle:      File title stored in the file (see TFile)
// name, title: Name and title of this object
//
// Until the first file is opened a dummy file with name /dev/null is
// opened to allow creation of trees and branches in the file.
// To distinguish between different /dev/null-files the given title is used.
//
MWriteRootFile::MWriteRootFile(const Int_t comp,
                               const char *rule,
                               const Option_t *option,
                               const char *ftitle,
                               const char *name,
                               const char *title) : fSplitRule(rule)
{
    Init(name, title);

    const Bool_t hasrule = fSplitRule.BeginsWith("s/") && fSplitRule.EndsWith("/");
    if (!hasrule)
        fSplitRule = "";

    //
    // Open a TFile in dummy mode! This is necessary to be able to create
    // the trees and branches, which are then (in ReInit) moved to
    // a valid file. (Stupid workaround - but does a good job)
    //
    const TString fname = hasrule ? "/dev/null" : rule;
    fOut = OpenFile(fname, option, ftitle, comp);
}

// --------------------------------------------------------------------------
//
// Specify the name of the root file. You can also give an option ("UPDATE"
// and "RECREATE" would make sense only) as well as the file title and
// compression factor. To a more detaild description of the options see
// TFile.
//
// To create a memory based TTree use
//   fname  = name of TTree
//   option = "memory"
// Make sure you do not read from a tree with the same name!
//
MWriteRootFile::MWriteRootFile(const char *fname,
                               const Option_t *option,
                               const char *ftitle,
                               const Int_t comp,
                               const char *name,
                               const char *title) : fOut(NULL)
{
    Init(name, title);

    TString opt(option);
    opt.ToLower();

    //
    // Check if we are writing to memory
    //
    if (opt=="memory")
    {
        fSplitRule = fname;
        return;
    }

    //
    // If no name is given we open the TFile in some kind of dummy mode...
    //
    TString str(fname);
    if (str.IsNull())
    {
        fOut = new TFile("/dev/null", "READ", ftitle, comp);
        fOut->SetBit(kMustCleanup);
        return;
    }

    if (!str.EndsWith(".root", TString::kIgnoreCase))
        str += ".root";

    //
    // Open the rootfile
    //
    fOut = OpenFile(str, opt, ftitle, comp);
}

// --------------------------------------------------------------------------
//
// Prints some statistics about the file to the screen. And closes the file
// properly.
//
void MWriteRootFile::Close()
{
    //
    // Print some statistics to the looging out.
    //
    if (fOut && !TestBit(kIsNotOwner))
    {
        Print();

        //
        // If the file is still open (no error) write the keys. This is necessary
        // for appearance of the all trees and branches.
        //
        if (IsFileOpen())
            fOut->Write();

        //
        // Delete the file. This'll also close the file (if open)
        //
        *fLog << inf3 << "Closing file " << fOut->GetName() << "." << endl;
        delete fOut;

        //
        // Remark:
        // - Trees are automatically deleted by the the file
        //   (unless file.SetDirectory(0) was called)
        // - Branches are automatically deleted by the tree destructor
        //
    }

    fOut = 0;
}

// --------------------------------------------------------------------------
//
// call Close()
//
MWriteRootFile::~MWriteRootFile()
{
    Close();
}

// --------------------------------------------------------------------------
//
// Prints all trees with the actually number of written entries to log-out.
//
void MWriteRootFile::Print(Option_t *) const
{
    if (!fOut)
        return;

    *fLog << all << underline << "File: " << GetFileName() << dec << endl;

    Bool_t cont = kFALSE;

    TObject *obj;
    TIter NextBranch(&fBranches);
    while ((obj=NextBranch()))
    {
        MRootFileBranch *b = (MRootFileBranch*)obj;

        if (!b->GetTree() || b->GetTree()->TestBit(kIsNewTree))
            continue;

        TBranch *branch = b->GetBranch();

        TString name = b->GetTree()->GetName();
        name += '.';
        name += branch->GetName();

        *fLog << " + " << name.Strip(TString::kTrailing, '.') << ": \t" << (ULong_t)branch->GetEntries() << " entries." << endl;
        cont = kTRUE;
    }

    TTree *t = NULL;
    TIter NextTree(&fTrees);
    while ((t=(TTree*)NextTree()))
        if (t->TestBit(kIsNewTree))
        {
            *fLog << " + " << t->GetName() << ": \t" << (ULong_t)t->GetEntries() << " entries." << endl;
            cont = kTRUE;
        }

    TIter NextKey(fOut->GetList());
    
    while ((obj=NextKey()))
    {
        if (!obj->InheritsFrom(TTree::Class()))
            continue;

        if (fTrees.FindObject(obj) && obj->TestBit(kIsNewTree))
            continue;

        *fLog << " - " << obj->GetName() << ": \t" << (ULong_t)((TTree*)obj)->GetEntries() << " entries." << endl;
        cont = kTRUE;
    }

    if (!cont)
        *fLog << "  No contents." << endl;

    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Add a new Container to list of containers which should be written to the
// file. Give the name of the container which will identify the container
// in the parameterlist. tname is the name of the tree to which the
// container should be written (Remark: one tree can hold more than one
// container). The default is the same name as the container name.
// You can slso specify a title for the tree. This is only
// used the first time this tree in 'mentioned'. As default the title
// is the name of the tree.
//
void MWriteRootFile::AddContainer(const char *cname, const char *tname, Bool_t must, Long64_t max)
{
    if (!fOut && !tname)
        tname = fSplitRule;

    TIter Next(&fBranches);
    TObject *o=0;
    while ((o=Next()))
        if (TString(o->GetName())==TString(tname) && TString(o->GetTitle())==TString(cname))
        {
            *fLog << warn;
            *fLog << "WARNING - Container '" << cname << "' in Tree '" << tname << "' already scheduled... ignored." << endl;
            return;
        }

    //
    // create a new entry in the list of branches to write and
    // add the entry to the list.
    //
    MRootFileBranch *entry = new MRootFileBranch(AddSerialNumber(cname), tname, must, max);
    fBranches.AddLast(entry);

    if (tname && tname[0])
        AddToBranchList(Form("%s.%s", (const char*)AddSerialNumber(cname), tname));
}

// --------------------------------------------------------------------------
//
// Add a new Container to list of containers which should be written to the
// file. Give the pointer to the container. tname is the name of the tree to
// which the container should be written (Remark: one tree can hold more than
// one container). The default is the same name as the container name.
// You can slso specify a title for the tree. This is only
// used the first time this tree in 'mentioned'. As default the title
// is the name of the tree.
//
void MWriteRootFile::AddContainer(MParContainer *cont, const char *tname, Bool_t must, Long64_t max)
{
    if (!fOut && !tname)
        tname = fSplitRule;

    TIter Next(&fBranches);
    TObject *o=0;
    while ((o=Next()))
        if (TString(o->GetName())==TString(tname) &&
            static_cast<MRootFileBranch*>(o)->GetContainer()==cont)
        {
            *fLog << warn;
            *fLog << "WARNING - Container " << cont << " in Tree '" << tname << "' already scheduled... ignored." << endl;
            return;
        }

    //
    // create a new entry in the list of branches to write and
    // add the entry to the list.
    //
    MRootFileBranch *entry = new MRootFileBranch(cont, tname, must, max);
    fBranches.AddLast(entry);
}

// --------------------------------------------------------------------------
//
// If you want to copy a full tree (or some branches of some trees)
// completely from one file to another one you can use this
//
void MWriteRootFile::AddCopySource(const char *tname, const char *bname, Bool_t force)
{
    TObject *obj = new TNamed(tname, bname?bname:"*");
    if (force)
        obj->SetBit(kForced);

    fCopies.Add(obj);
    fCopies.Sort();
}

// --------------------------------------------------------------------------
//
// Add a new Container to list of containers which should be written to the
// file. Give the pointer to the container. tname is the name of the tree to
// which the container should be written (Remark: one tree can hold more than
// one container). The default is the same name as the container name.
// You can slso specify a title for the tree. This is only
// used the first time this tree in 'mentioned'. As default the title
// is the name of the tree.
//
Bool_t MWriteRootFile::GetContainer(MParList *pList)
{
    //
    // loop over all branches which are 'marked' as branches to get written.
    //
    MRootFileBranch *entry;

    TIter Next(&fBranches);
    while ((entry=(MRootFileBranch*)Next()))
    {
        //
        // Get the pointer to the container. If the pointer is NULL it seems,
        // that the user identified the container by name.
        //
        MParContainer *cont = entry->GetContainer();
        if (!cont)
        {
            //
            // Get the name and try to find a container with this name
            // in the parameter list.
            //
            const char *cname = entry->GetContName();
            cont = (MParContainer*)pList->FindObject(cname);
            if (!cont)
            {
                //
                // No corresponding container is found
                //
                if (entry->MustHave())
                {
                    *fLog << err << "Cannot find parameter container '" << cname << "'." << endl;
                    return kFALSE;
                }

                *fLog << inf2 << "Unnecessary parameter container '" << cname << "' not found..." << endl;
                delete fBranches.Remove(entry);
                continue;
            }

            //
            // The container is found. Put the pointer into the entry.
            //
            entry->SetContainer(cont);
        }

        //
        // Get container name, tree name and tree title of this entry.
        //
        const char *cname = cont->GetName();
        const char *tname = entry->GetName();
        const TString ttitle(Form("Tree containing %s", cont->GetDescriptor().Data()));

        //
        // if the tree name is NULL this idetifies it to use the default:
        // the container name.
        //
        if (tname[0] == '\0')
            tname = cname;

        //
        // Check if the tree is already existing (part of the file or memory)
        //
        TTree *tree = fOut ? (TTree*)fOut->Get(tname) : dynamic_cast<TTree*>(gROOT->FindObject(tname));
        if (!fOut && tree)
        {
            if (tree->GetCurrentFile())
            {
                *fLog << err;
                *fLog << "ERROR - You are trying to write data into a memory stored root tree," << endl;
                *fLog << "        because you either called the default constructor  or  have" << endl;
                *fLog << "        instantiated MWriteRootFile using the write option 'memory'." << endl;
                *fLog << "        This  tree   '" << tname << "'   is  already  existing  in" << endl;
                *fLog << "        memory  (gROOT->FindObject)  and is already belonging  to a" << endl;
                *fLog << "        file (" << tree->GetCurrentFile()->GetName() << ")." << endl;
                *fLog << "        This can  - for example -  happen if you are reading from a" << endl;
                *fLog << "        tree with the same name.  The easiest solution in this case" << endl;
                *fLog << "        is to change the name of the tree you want to write to." << endl;
                *fLog << endl;
                return kFALSE;
            }
            *fLog << inf << "Tree '" << tname << "' found already in Memory... using." << endl;
        }

        if (!tree)
        {
            //
            // if the tree doesn't exist create a new tree. Use the tree
            // name as title if title is NULL.
            // And add the tree to the list of trees
            //
            TDirectory *save = gDirectory;
            if (fOut)
                fOut->cd();
            else
                gROOT->cd();

            tree = new TTree(tname, ttitle, fOut ? 99 : 1);
            fTrees.AddLast(tree);

            //
            // If the tree does not already exist in the file mark this
            // tree as a branch created by MWriteRootFile
            //
            tree->SetBit(kIsNewTree);

            *fLog << inf << "Tree " << tname << " created in " << gDirectory->GetName() << endl;

            gDirectory = save;
        }

        //
        // In case the file is opened as 'UPDATE' the tree may still not
        // be in the list. Because it neither was created previously,
        // nor this time, so the corresponding entries is marked as a
        // single branch to be filled. --> Add it to the list of trees.
        //
        if (!fTrees.FindObject(tree))
            fTrees.AddLast(tree);

        //
        // Now we have a valid tree. Search the list of trees for this tree
        // Add a pointer to the entry in the tree list to this branch-entry
        //
        entry->SetTree(tree);

        TString branchname(cname);
        branchname.Append(".");

        //
        // Try to get the branch from the file. 
        // If the branch already exists the user specified one branch twice.
        //
        TBranch *branch = tree->GetBranch(branchname);
        if (branch)
        {
            *fLog << inf << "Branch '" << cname << "' already existing... updating." << endl;
            branch->SetAddress(entry->GetAddress());

            if (!fSplitRule.IsNull() && fOut)
            {
                *fLog << warn << endl;
                *fLog << "WARNING: You are updating an existing branch. For this case" << endl;
                *fLog << "         file-splitting mode  is  not  allowed...  disabled!" << endl;
                *fLog << endl;
                fSplitRule = "";
            }
        }
        else
        {
            //
            // Create a new branch in the actual tree. The branch has the name
            // container name. The type of the container is given by the
            // ClassName entry in the container. The Address is the address of a
            // pointer to the container (gotten from the branch entry). As
            // Basket size we specify a (more or less) common default value.
            // The containers should be written in Splitlevel=1
            //
            *fLog << inf << "Creating Branch '" << cname << "' ";
            if ((TString)cname!=(TString)cont->ClassName())
                *fLog << "[" << cont->ClassName() << "] ";
            *fLog << "in tree " << tree->GetName() << "... " << flush;

            branch = tree->Branch(branchname, cont->ClassName(), entry->GetAddress());

            //
            // If the branch couldn't be created we have a problem.
            //
            if (!branch)
            {
                *fLog << endl;
                *fLog << err << "Unable to create branch '" << cname << "'." << endl;
                return kFALSE;
            }

            *fLog << "done." << endl;

            if (!tree->TestBit(kIsNewTree) && !fSplitRule.IsNull())
            {
                *fLog << warn << endl;
                *fLog << "WARNING:   You  have  created  a new branch  in  an existing tree." << endl;
                *fLog << "     For this case file-splitting mode is not allowed... disabled!" << endl;
                *fLog << endl;
                fSplitRule= "";
            }
        }

        //
        // Tell the entry also which branch belongs to it (this is necessary
        // for branches belonging to already existing tree, UPDATE-mode)
        //
        entry->SetBranch(branch);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Checks all given containers (branch entries) for the write flag.
// If the write flag is set the corresponding Tree is marked to get filled.
// All Trees which are marked to be filled are filled with all their
// branches.
// In case of a file opened in 'UPDATE' mode, single branches can be
// filled, too. WARNING - for the moment there is no check whether
// you filled the correct number of events into the branch, so that
// each of the other branches in the tree has the correct corresponding
// number of new entries in the new branch!
// Be carefull: If only one container (corresponding to a branch) of a tree
// has the write flag, all containers in this tree are filled!
//
Bool_t MWriteRootFile::CheckAndWrite()
{
    // This is the special case if we only copy a tre but do not
    // write containers to it
    const Int_t n = fTrees.GetEntriesFast();
    if (n==0)
        return kTRUE;

    TObject *obj;

    //
    // Loop over all branch entries
    //
    TIter NextBranch(&fBranches);
    while ((obj=NextBranch()))
    {
        MRootFileBranch *b = (MRootFileBranch*)obj;

        //
        // Check for the Write flag
        //
        if (!b->GetContainer()->IsReadyToSave())
            continue;

        if (b->GetMaxEntries()==b->GetTree()->GetEntries())
            continue;

        //
        // If the write flag of the branch entry is set, set the write flag of
        // the corresponding tree entry.
        //
        if (b->GetTree()->TestBit(kIsNewTree))
            b->GetTree()->SetBit(kFillTree);
        else
        {
            if (!b->GetBranch()->Fill())
            {
                *fLog << err << "ERROR - MWriteRootFile: Zero bytes written to branch '" << b->GetBranch()->GetName() << "'... abort." << endl;
                return kFALSE;
            }
        }
    }

    //
    // Loop over all tree entries
    //
    for (int idx=0; idx<n; idx++)
    {
        TTree *t = (TTree*)fTrees[idx];
        if (!t)
        {
            *fLog << err << "ERROR - MWriteRootFile: The Tree with index " << idx << endl;
            *fLog << " in fTrees is NULL. This should never happen. Please send" << endl;
            *fLog << " a bug report." << endl;
            return kFALSE;
        }

        //
        // Check the write flag of the tree
        //
        if (!t->TestBit(kFillTree))
            continue;

        //
        // If the write flag is set, fill the tree (with the corresponding
        // branches/containers), delete the write flag and increase the number
        // of written/filled entries.
        //
        t->ResetBit(kFillTree);

        if (!t->Fill())
        {
            *fLog << err << "ERROR - MWriteRootFiole: Zero bytes written to tree '" << t->GetName() << "'... abort." << endl;
            return kFALSE;
        }
    }

    //
    // If we are writing into memory we don't split into seperate files
    //
    if (!fOut || TestBit(kIsNotOwner))
        return kTRUE;

    //
    // For more information see TTree:ChangeFile()
    //
    TTree *t0 = (TTree*)fTrees[0];
    if (!t0 || fOut==t0->GetCurrentFile())
        return kTRUE;

    // FIXME: THIS IS EMITTED FOR ALL CONSEQUTIVE EVENTS!
    *fLog << warn << endl;
    *fLog << "WARNING - MWriteRootFile:   Root's  TTree/TFile   has  opened   a  new  file" << endl;
    *fLog << "  automatically.  You can change this behaviour using TTree::SetMaxTreeSize." << endl;
    *fLog << "  You won't be able to read splitted  files  correctly with MReadMarsFile if" << endl;
    *fLog << "  they have more than one entry in 'RunHeaders' or you try to read more than" << endl;
    *fLog << "  one of such sequences at once." << endl;
    *fLog << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Open a new file with the name fname. Move all trees and branches from the
// old file to the new file.
//
Bool_t MWriteRootFile::ChangeFile(const char *fname)
{
    const Int_t   compr = fOut ? fOut->GetCompressionLevel() : 0;
    const TString title = fOut ? fOut->GetTitle()            : "";
    const TString opt   = fOut ? fOut->GetOption()           : "";

    // Open new file with old setup
    TFile *newfile = OpenFile(fname, opt, title, compr);
    if (newfile && newfile==fOut)
    {
        *fLog << inf << "Found open file '" << fname << "'... using." << endl;
        return kTRUE;
    }
    if (!newfile)
    {
        *fLog << err << "ERROR - Cannot open new file '" << fname << "'" << endl;
         return kFALSE;
    }

    if (!fOut)
    {
        *fLog << err << "ERROR - MWriteRootFile::ChangeFile... something went terribly wrong!" << endl;
        *fLog <<        "        fname: " << fname << endl;
        *fLog <<        "        Please start debugging!" << endl;
        return kFALSE;
    }

    *fLog << inf << "Open new file " << fname << " (Title=" << title << ", Option=" << opt << ", Compression=" << compr << ")" << endl;

    // Print statistics of old file
    const TString n = GetFileName();
    if (!n.IsNull() && n!=TString("/dev/null"))
        Print();

    if (fOut->IsOpen())
        fOut->Write();

    // Move all trees from the old file to the new file
    TObject *obj=0;
    while ((obj = fOut->GetList()->First()))
    {
        // Remove obj from old file (otherwise deleting
        // the old file will delete the objs)
        fOut->GetList()->Remove(obj);

        // If this is not a tree do nothing.
        if (!obj->InheritsFrom(TTree::Class()))
            continue;

        // process all trees in the old file
        TTree *t = (TTree*)obj;

        // reset and move to new file (this is done implicitly for all branches)
        t->Reset();
        t->SetDirectory(newfile);
    }

    // Close/delete the old file (keys already written above)
    *fLog << inf3 << "Closing file " << fOut->GetName() << "." << endl;
    delete fOut;
 
    // Replace current with new file
    fOut = newfile;

    // Change current directory to new file
    gFile = fOut;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// A rule looks like:
//   "s/source/destination/"
//
// For more details on regular expression see a proper documentation,
// for example, "man 7 regex" if installed, or TPRegexp.
//
// Here is an example:
//
// Example:
//   inputfile: /data/MAGIC/Period016/rootdata/20040621_23210_D_Mkn421_E.root
//   rule:      /([0-9]+_[0-9]+)_D_(.*[.]root)/\\/outpath\\/$1_Y_$2/
//   outfile:   /outpath/20040621_23210_Y_Mkn421_E.root
//
// Please make sure that all / in your rules are correctly escaped, i.e.
// in the string stored in memory it must look like \/ and in the string
// your set in your program it must look \\/.
//
// Note, that this function has been made static to allow your to
// test your split rules, i.e. regular expressions.
//
TString MWriteRootFile::SubstituteName(const char *regexp, TString fname)
{
    // Remove the path from the filename
    if (fname.Last('/')>=0)
        fname.Remove(0, fname.Last('/')+1);

    // Regular expression to split the rule into its contents
    static const TString sed("s/((\\\\/|[^/])*)/((\\\\/|[^/])*)/([gimosxd]*)");

    // Do splitting
    TObjArray *subStrL = TPRegexp(sed).MatchS(regexp);
    if (subStrL->GetEntries()!=6)
    {
        gLog << err << "ERROR - SubstituteName: Evaluating regexp " << regexp << " failed." << endl;
        subStrL->Print();
        delete subStrL;
        return "";
    }

    /*const*/ TString reg = (*subStrL)[1]->GetName(); // Regular expression to search for
    /*const*/ TString tar = (*subStrL)[3]->GetName(); // Regular expression for replacing
      const   TString mod = (*subStrL)[5]->GetName(); // Possible modifiers (e.g. 'a')

    delete subStrL;

    // Unescpae slashes in paths
    reg.ReplaceAll("\\/", "/");
    tar.ReplaceAll("\\/", "/");

    // Do substitution
    const Int_t nrSub = TPRegexp(reg).Substitute(fname, tar, mod);
    if (nrSub==0)
    {
        gLog << err << "ERROR - Substituting due to SplitRule failed." << endl;
        gLog << " Source FileName:  " << fname << endl;
        gLog << " Search  Rexexp:   " << reg   << endl;
        gLog << " Replace Rexexp:   " << tar   << endl;
        gLog << " Modifiers:        " << mod   << endl;
        return "";
    }

    return fname;
}

// --------------------------------------------------------------------------
//
// Writes a copy of the TTree t to the currently open file using
// TTree::CloneTree()
//
void MWriteRootFile::CopyTree(TTree &t) const
{
    TString out = "Copy of tree ";
    out += t.GetName();
    out += " in progress...";

    if (fDisplay)
        fDisplay->SetStatusLine2(out);

    *fLog << inf << out << flush;

    // When a new file has been opened the old clone (if existing) has
    // been moved to the new file. We could now use CopyTree but then
    // we would have to unpack all data and repack it. Instead
    // we delete the moved old tree.
    //  FIXME: In priciple we could delete a "wrong" tree with the same name.
    //         Should we flag the clones and delete them in ChangeFile?
    TObject *old = fOut->GetList()->Remove(fOut->GetList()->FindObject(t.GetName()));
    delete old;

    // Now we clone the tree without unpacking and repacking.
    // When it has not been moved it will be deleted in the TFile destructor
    /*TTree *clone =*/ t.CloneTree(-1, "fast");
    //clone->Write();
    //delete clone;

    *fLog << "done." << endl;

    if (fDisplay)
    {
        out += " done.";
        fDisplay->SetStatusLine2(out);
    }
}

// --------------------------------------------------------------------------
//
// Make all copies requested from the currently open file into the new
// file.
//
Bool_t MWriteRootFile::MakeCopies(const char *fname) const
{
    if (fCopies.GetEntries()==0)
        return kTRUE;

    TFile *file = dynamic_cast<TFile*>(gROOT->GetListOfFiles()->FindObject(fname));
    if (!file)
    {
        *fLog << err << "ERROR - MakeCopies: File " << fname << " not found in gROOT->GetListOfFiles()... abort." << endl;
        return kFALSE;
    }

    TIter Next(&fCopies);
    TObject *o=0;
    TTree   *t=0;

    fOut->cd();
    while ((o=Next()))
    {
        TTree *gettree = dynamic_cast<TTree*>(file->Get(o->GetName()));
        if (!gettree)
        {
            const Bool_t force = o->TestBit(kForced);
            if (force)
                *fLog << err << "ERROR - ";
            else
                *fLog << inf3;

            *fLog << "MakeCopies: Tree " << o->GetName() << " not found in file " << fname << ".";
            if (force)
                *fLog << ".. abort.";
            *fLog << endl;

            if (force)
                return kFALSE;

            continue;
        }

        gettree->SetBranchStatus(o->GetTitle(), 1);

        // First Execution
        if (t==gettree)
            continue;

        // Check if its the first call
        if (t)
            CopyTree(*t);
        t = gettree;
    }

    if (t)
        CopyTree(*t);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// ReInit. If file splitting is not allowed call MWriteFile::ReInit.
//
// In other cases get MRead from the TaskList (splitting is switched of if
// this is impossible).
//
// Convert the input- into a new output file-name.
//
// Open a new file, change all trees to the new file (calling ChangeFile()),
// and close the old one.
//
// Call MWriteFile::ReInit()
//
Bool_t MWriteRootFile::ReInit(MParList *pList)
{
    MRead *read = (MRead*)pList->FindTask("MRead");
    if (fSplitRule.IsNull() && fCopies.GetEntries()>0 && fOut)
    {
        if (!read)
        {
            *fLog << err;
            *fLog << "ERROR: No Task 'MRead' found in the tasklist.  This task is" << endl;
            *fLog << "  necessary to  get the filename.  Without  a filename file" << endl;
            *fLog << "  AddCopySource cannot be used... abort." << endl;
            *fLog << endl;
            return kFALSE;
        }
        if (!MakeCopies(read->GetFullFileName()))
            return kFALSE;

    }

    if (fSplitRule.IsNull() || !(fOut || TestBit(kIsNotOwner)))
        return MWriteFile::ReInit(pList);

    if (!read)
    {
        *fLog << warn;
        *fLog << "WARNING: No Task 'MRead' found in the tasklist.  This task is" << endl;
        *fLog << "  necessary  to  get  the filename.  Without  a filename file" << endl;
        *fLog << "  file splitting is not allowed... disabled!" << endl;
        *fLog << endl;
        fSplitRule = "";
        return kTRUE;
    }


    const TString oldname = read->GetFullFileName();
    const TString newname = SubstituteName(fSplitRule, oldname);
    if (!ChangeFile(newname))
        return kFALSE;

    if (!MakeCopies(oldname))
        return kFALSE;

    return MWriteFile::ReInit(pList);
}

// --------------------------------------------------------------------------
//
// return open state of the root file. If the file is 'memory' kTRUE is
// returned.
//
Bool_t MWriteRootFile::IsFileOpen() const
{
    if (!fOut)
        return kTRUE;

    const char *n = fOut->GetName();
    return n==0 || *n==0 ? kTRUE : fOut->IsOpen();
}

// --------------------------------------------------------------------------
//
// return name of the root-file. If the file is "memory" "<memory>" is
// returned.
//
const char *MWriteRootFile::GetFileName() const
{
    if (!fOut)
        return "<memory>";

    const char *n = fOut->GetName();
    return n==0 || *n==0 ? "<dummy>" : n;
}

// --------------------------------------------------------------------------
//
// cd into file. See TFile::cd(). If the file is "memory" kTRUE is returned.
//
Bool_t MWriteRootFile::cd(const char *path)
{
    return fOut ? fOut->cd(path) : kTRUE;
}

// --------------------------------------------------------------------------
//
// If the output file is deleted set fOut to NULL.
// Call MTask::RecursiveRemove
//
void MWriteRootFile::RecursiveRemove(TObject *obj)
{
    if (obj==fOut)
        fOut=NULL;

    MWriteFile::RecursiveRemove(obj);
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MWriteRootFile::StreamPrimitive(ostream &out) const
{
    out << "   MWriteRootFile " << GetUniqueName();
    if (fOut)
    {
        out << "(\"";
        out << fOut->GetName() << "\", \"";
        out << fOut->GetOption() << "\", \"";
        out << fOut->GetTitle() << "\", ";
        out << fOut->GetCompressionLevel();
        out << ")";
    }
    out << ";" << endl;

    if (fName!=gsDefName)
        out << "   " << GetUniqueName() << ".SetName(\"" << fName << "\");" << endl;
    if (fTitle!=gsDefTitle)
        out << "   " << GetUniqueName() << ".SetTitle(\"" << fTitle << "\");" << endl;

    MRootFileBranch *entry;
    TIter Next(&fBranches);
    while ((entry=(MRootFileBranch*)Next()))
    {
        out << "   " << GetUniqueName() << ".AddContainer(";

        if  (entry->GetContainer())
        {
            entry->GetContainer()->SavePrimitive(out);
            out << "&" << entry->GetContainer()->GetUniqueName();
        }
        else
            out << "\"" << entry->GetContName() << "\"";

        out << ", \"" << entry->GetName() << "\"";
        if (!entry->MustHave())
            out << ", kFALSE";

        out <<");" << endl;
    }
}

