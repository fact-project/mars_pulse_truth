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
!   Author(s): Thomas Bretz, 06/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MWriteAsciiFile
//
// If you want to store a single container into an Ascii file you have
// to use this class. You must know the name of the file you wanne write
// (you should know it) and the name of the container you want to write.
// This can be the name of the class or a given name, which identifies
// the container in a parameter container list (MParList).
// The container is written to the ascii file if its ReadyToSave flag is
// set (MParContainer)
//
// You can write more than one container in one line of the file, see
// AddContainer.
//
// You can also write single data members of a container (like fWidth
// of MHillas). For more details see AddContainer. Make sure, that a
// getter method for the data member exist. The name of the method
// must be the same than the data member itself, but the f must be
// replaced by a Get.
//
/////////////////////////////////////////////////////////////////////////////
#include "MWriteAsciiFile.h"

#include <fstream>

#include <TMethodCall.h> // TMethodCall, AsciiWrite

#include "MIter.h"

#include "MDataList.h"   // MDataList
#include "MDataPhrase.h" // MDataPhrase
#include "MDataValue.h"  // MDataValue
#include "MDataMember.h" // MDataMember

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

ClassImp(MWriteAsciiFile);

using namespace std;

// --------------------------------------------------------------------------
//
// Init. Replaces the same code used in all constructors.
//
void MWriteAsciiFile::Init(const char *filename, const char *name, const char *title)
{
    fName  = name  ? name  : "MWriteAsciiFile";
    fTitle = title ? title : "Task to write one container to an ascii file";

    fNameFile = filename;

    fOut = new ofstream(fNameFile);
}

// --------------------------------------------------------------------------
//
// Specify the name of the ascii output file 'filename' and the name
// of the container you want to write. (Normally this is the name
// of the class (eg. MHillas) but it can also be a different name which
// identifies the container in the parameter list.
// Because you cannot write more than one container there is no Add-function
// like in MWriteRootFile.
//
//  For Example: MWriteAsciiFile("file.txt", "MHillas");
//
MWriteAsciiFile::MWriteAsciiFile(const char *filename, const char *contname,
                                 const char *name, const char *title)
{
    Init(filename, name, title);

    if (contname)
       AddColumns(contname);
}

// --------------------------------------------------------------------------
//
// Specify a the name of the ascii output file 'filename' and a pointer to
// the container you want to write. 
// Because you cannot write more than one container there is no Add-function
// like in MWriteRootFile.
//
//  For Example: MHillas hillas;
//               MWriteAsciiFile("file.txt", &hillas);
//
//
MWriteAsciiFile::MWriteAsciiFile(const char *filename, MParContainer *cont,
                                 const char *name, const char *title)
{
    Init(filename, name, title);

    if (cont)
        AddColumns(cont);
}

// --------------------------------------------------------------------------
//
// Destructor. Delete the output file if necessary (it is closed
// automatically by its destructor.
//
MWriteAsciiFile::~MWriteAsciiFile()
{
    fAutoDel.SetOwner();
    delete fOut;
}

// --------------------------------------------------------------------------
//
// Return open state of the file
//
Bool_t MWriteAsciiFile::IsFileOpen() const
{
    return (bool)(*fOut);
}

// --------------------------------------------------------------------------
//
// Tries to get all containers from the ParList which were given by name
// adds them to the list of pointers to the container which should be
// written to the ascii file.
//
Bool_t MWriteAsciiFile::GetContainer(MParList *plist)
{
    TObject *obj=NULL;

    TIter Next(&fList);
    while ((obj=Next()))
    {
        //
        // MData is the highest class in the inheritance tree
        //
        if (obj->InheritsFrom(MData::Class()))
        {
            if (!((MData*)obj)->PreProcess(plist))
                return kFALSE;
            continue;
        }

        //
        // MParContainer is the next class in the inheritance tree
        //
        if (obj->InheritsFrom(MParContainer::Class()))
            continue;

        //
        // It is neither a MData nor a MParContainer, it must be a TNamed
        //
        TObject *o = plist->FindObject(obj->GetName());
        if (!o)
            return kFALSE;

        fList[fList.IndexOf(obj)] = o;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Check if the containers are ready for writing. If so write them.
// The containers are written in one line, one after each other.
// If not all containers are written (because of the IsReadyToSave-flag)
// a warning message is print.
//
Bool_t MWriteAsciiFile::CheckAndWrite()
{
    MParContainer *obj = NULL;

    //
    // Check for the Write flag
    //
    Bool_t write = kFALSE;
    MIter Next(&fList);
    while ((obj=Next()))
        if (obj->IsReadyToSave())
        {
            write = kTRUE;
            break;
        }

    //
    // Do not write if not at least one ReadyToSave-flag set
    //
    if (!write)
        return kTRUE;

    Bool_t written = kFALSE;
    Int_t num = fList.GetEntries();

    Next.Reset();
    while ((obj=Next()))
    {
        if (written)
            *fOut << " ";

        //
        // Write container to file
        //
        if (!obj->AsciiWrite(*fOut))
            continue;

        //
        // Store that at least one container was written
        //
        written = kTRUE;

        num--;
    }

    if (written)
    {
        *fOut << endl;

        if (num!=0)
            *fLog << warn << "WARNING - Number of objects written mismatch!" << endl;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Add a rule to be written as a column to the ascii file.
// For more information about rules see MDataPhrase.
//
//  eg: MWriteAsciiFile::AddColumn("log10(MHillas.fEnergy)/2")
//
void MWriteAsciiFile::AddColumn(const TString rule)
{
    MDataPhrase *chain = new MDataPhrase(rule);
    fList.Add(chain);
}

// --------------------------------------------------------------------------
//
// Add another column to be written to the ascii file. The columns will be
// output one after each other in one line.
// Specify the name of the data member to be written (eg fWidth) and
// a possible scale factor (eg. to transform millimeters to degrees)
//
//  eg:
//       MMcEvt evt;
//       MWriteAsciiFile::AddColumn(&evt, "fImpact", 0.01);
//
void MWriteAsciiFile::AddColumn(MParContainer *cont, const TString member, Double_t scale)
{
    MData *data = new MDataMember(cont, member);

    if (scale!=1)
    {
        MDataList  *list = new MDataList('*');
        MDataValue *val  = new MDataValue(scale);

        list->SetOwner();
        list->AddToList(data);
        list->AddToList(val);

        data = list;
    }
    fList.Add(data);
    fAutoDel.Add(data);
}

// --------------------------------------------------------------------------
//
// Add another container (by name) to be written to the ascii file.
// The container will be output one after each other in one line.
// The output will be done either by MParContainer::AsciiWrite or
// by the corresponding overloaded function.
//
//  eg: MWriteAsciiFile::AddColumns("MMcEvt")
//
void MWriteAsciiFile::AddColumns(const TString cont)
{
    TNamed *name = new TNamed(cont, cont);
    fList.Add(name);
    fAutoDel.Add(name);
}

// --------------------------------------------------------------------------
//
// Add another container (by pointer) to be written to the ascii file.
// The container will be output one after each other in one line.
// The output will be done either by MParContainer::AsciiWrite or
// by the corresponding overloaded function.
//
//  eg:
//      MMcEvt evt;
//      MWriteAsciiFile::AddColumns(&evt);
//
void MWriteAsciiFile::AddColumns(MParContainer *cont)
{
    fList.Add(cont);
}
