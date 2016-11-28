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
!   Author(s): Thomas Bretz, 07/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MFillH
//
//  This is a common interface (task) to fill mars histograms. Every mars
//  histogram which is derived from MH can be filled with this task.
//
//  There are two options to use:
//
//  1) You specifiy the parameter container with which data the
//     histogram container should be filled, and the histogram container
//     which has to be filled. This can be done by either specifing the
//     name of the objects in the parameter list or by specifiing a pointer
//     to the object. (s. Constructor)
//
//  2) You specify the name and/or type of the histogram to become filled.
//     Any other action imust be taken by the histogram class.
//
//  PreProcess: In the preprocessing of this task we setup all pointers
//              to instances which are needed and call FillSetup of the
//              histogram class with the parameter list as an argument.
//
//  Process: The process function calls the Fill member function of the
//           histogram class instance (inheriting from MH) with either
//           a NULL pointer or a pointer to the corresponding container
//           as an argument.
//
// To use a weight for each event filled in a histogram call
// SetWeight(). You can eithe use the name of a MParameterD container stored
// in the parameter list or a pointer to it as an argument.
//
//
//  WARNING:
//   Because MFillH is a generalized task to fill histograms it doesn't
//   know about which branches from a file are necessary to fill the
//   histograms. If you are reading data from a file which is directly
//   filled into a histogram via MFillH, please call either
//   MReadTree::DisableAutoScheme() or enable the necessary branches by
//   yourself, using MReadTree::EnableBranch()
//
//   Checkout the Warning in MTaskList.
//
//  Version 3:
//  ----------
//   - added fTitleTab
//
//  Version 2:
//  ----------
//   - added fNumExcutions
//
//
//  Input Containers:
//   A parameter container
//
//  Output Containers:
//   A histogram container
//
//////////////////////////////////////////////////////////////////////////////
#include "MFillH.h"

#include <fstream>

#include <TClass.h>
#include <TCanvas.h>

#include "MDataPhrase.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"

#include "MH.h"
#include "MHArray.h"

#include "MParameters.h"

#include "MParList.h"
#include "MStatusDisplay.h"

ClassImp(MFillH);

using namespace std;

// --------------------------------------------------------------------------
//
// Initializes name and title of the object. It is called by all
// constructors.
//
void MFillH::Init(const char *name, const char *title)
{
    fName  = name  ? name  : "MFillH";
    fTitle = title ? title : "Task to fill Mars histograms";

    fH            = NULL;
    fParContainer = NULL;

    fIndex  = NULL;
    fCanvas = NULL;

    fWeight     = NULL;
    fWeightName = "";
}

// --------------------------------------------------------------------------
//
// Default Constructor. This is to support some root-stuff.
// Never try to use it yourself!
//
MFillH::MFillH()
{
    Init(NULL, NULL);
}

// --------------------------------------------------------------------------
//
// Constructor.
//
// 1) - par is the name of the parameter container which should be filled into
//      the histogram
//    - hist is the name of the histogram container (which must have been
//      derived from MH)
//
//    In this case MH::Fill is called with a pointer to the corresponding
//    histogram instance.
//
// 2) - hist is the name and/or type of the histogram.
//      1) The name and type is identical, eg: "MHHillas"
//      2) They are not identical, eg: "MyHistogram [MHHillas]"
//         This searches for a class instance of MHHillas with the name
//         "MyHistogram". If it doesn't exist one is created.
//
//    In this case PreProcess calls MH::SetupFill with a pointer to the
//    parameter list and MH::Fill is called with a NULL-pointer.
//
MFillH::MFillH(const char *hist, const char *par, const char *name, const char *title)
{
    Init(name, title);

    fHName = hist;
    fParContainerName = par;

    AddToBranchList(Form("%s.*", (const char*)ExtractName(hist)));
    if (par)
        AddToBranchList(Form("%s.*", (const char*)ExtractName(par)));

    if (title)
        return;

    fTitle = Form("Fill %s", fHName.Data());
    if (fParContainerName.IsNull())
        return;

    fTitle += Form(" from %s", fParContainerName.Data());
}

// --------------------------------------------------------------------------
//
// Constructor.
//
// 1) - par is a pointer to the instance of your parameter container from which
//      the data should be used to fill the histogram.
//    - hist is the name of the histogram container (which must have been
//      derived from MH)
//
//    In this case MH::Fill is called with a pointer to the corresponding
//    histogram instance.
//
// 2) - hist is the name and/or type of the histogram.
//      1) The name and type is identical, eg: "MHHillas"
//      2) They are not identical, eg: "MyHistogram [MHHillas]"
//         This searches for a class instance of MHHillas with the name
//         "MyHistogram". If it doesn't exist one is created. Everything
//         which is between the first '[' and the last ']' in the string
//         is used as the histogram type.
//
//    In this case PreProcess calls MH::SetupFill with a pointer to the
//    parameter list and MH::Fill is called with a NULL-pointer.
//
//
MFillH::MFillH(const char *hist, MParContainer *par, const char *name, const char *title)
{
    Init(name, title);

    fHName = hist;
    fParContainer = par;
    if (par)
        fParContainerName = par->GetName();

    AddToBranchList(Form("%s.*", (const char*)ExtractName(hist)));
    if (par)
        AddToBranchList(Form("%s.*", par->GetName()));

    if (!title)
        fTitle = Form("Fill %s from %s", fName.Data(), par?par->GetDescriptor().Data():"NULL");
}

// --------------------------------------------------------------------------
//
// Constructor.
//
// - par is a pointer to the instance of your parameter container from which
//   the data should be used to fill the histogram.
// - hist is a pointer to the instance of your histogram container (which must
//   have been derived from MH) into which the data should flow
//
MFillH::MFillH(MH *hist, const char *par, const char *name, const char *title)
{
    Init(name, title);

    fH = hist;
    if (hist)
        fHName = hist->GetName();
    fParContainerName = par;

    if (fH)
        AddToBranchList(fH->GetDataMember());
    if (par)
        AddToBranchList(Form("%s.*", (const char*)ExtractName(par)));

    if (title)
        return;

    fTitle = Form("Fill %s", hist ? hist->GetDescriptor().Data() : "NULL");
    if (!par)
        return;

    fTitle += Form(" from %s", fParContainerName.Data());
}

// --------------------------------------------------------------------------
//
// Constructor.
//
// - par is a pointer to the instance of your parameter container from which
//   the data should be used to fill the histogram.
// - hist is the name of the histogram container (which must have been
//   derived from MH)
//
MFillH::MFillH(MH *hist, MParContainer *par, const char *name, const char *title)
{
    Init(name, title);

    fH = hist;
    if (hist)
        fHName = hist->GetName();
    fParContainer = par;
    if (par)
        fParContainerName = par->GetName();

    if (fH)
        AddToBranchList(fH->GetDataMember());
    if (par)
        AddToBranchList(Form("%s.*", par->GetName()));

    if (!title)
        fTitle = Form("Fill %s from %s",
                      hist?hist->GetDescriptor().Data():"NULL",
                      par ? par->GetDescriptor().Data():"NULL");
}

// --------------------------------------------------------------------------
//
// Destructor. Delete fData if existing and kCanDelete is set.
//
MFillH::~MFillH()
{
    if (fIndex)
        if (fIndex->TestBit(kCanDelete))
            delete fIndex;
}

// --------------------------------------------------------------------------
//
// If the histogram to be filles is a MHArray you can specify a 'rule'
// This rule is used to create an MDataPhrase. The return value of the chain
// is casted to int. Each int acts as a key. For each (new) key a new
// histogram is created in the array. (eg for the rule
// "MRawRunHeader::fRunNumber" you would get one histogram per run-number)
//
void MFillH::SetRuleForIdx(const TString rule)
{
    fIndex = new MDataPhrase(rule);
    fIndex->SetBit(kCanDelete);
}

// --------------------------------------------------------------------------
//
// If the histogram to be filles is a MHArray you can specify a MData-object
// The return value of the object is casted to int. Each int acts as a key.
// For each (new) key a new histogram is created in the array. (eg for
// MDataMember("MRawEvtHeader::fRunNumber") you would get one histogram per
// run-number)
//
void MFillH::SetRuleForIdx(MData *data)
{
    fIndex = data;
}

// --------------------------------------------------------------------------
//
// Extracts the name of the histogram from the MFillH argument
//
TString MFillH::ExtractName(const char *name) const
{
    TString type = name;

    const Ssiz_t first = type.First('[');
    const Ssiz_t last  = type.First(']');

    if (!first || !last || first>=last)
        return type;

    return type.Remove(first).Strip(TString::kBoth);
}

// --------------------------------------------------------------------------
//
// Extracts the class-name of the histogram from the MFillH argument
//
TString MFillH::ExtractClass(const char *name) const
{
    TString type = name;

    const Ssiz_t first = type.First('[');
    const Ssiz_t last  = type.First(']');

    if (!first || !last || first>=last)
        return type;

    const Ssiz_t length = last-first-1;

    TString strip = fHName(first+1, length);
    return strip.Strip(TString::kBoth);
}

// --------------------------------------------------------------------------
//
// Use this to set a draw option used when drawing automatically to the
// status display.
//
void MFillH::SetDrawOption(Option_t *option)
{
    fDrawOption = option;
}

// --------------------------------------------------------------------------
//
// Creates a new tab in a status display with the name of the MH class,
// if fDisplay is set and the MH-class overwrites the Draw function
//
// If the draw-option contains 'same' (case insensitive) a tab with the
// same name as the one which would be created is searched and the
// MH is drawn to this canvas. If it is not found a new tab is created.
//
Bool_t MFillH::DrawToDisplay()
{
    fCanvas = NULL;

    if (!fDisplay)
        return kTRUE;

    if (!fH->OverwritesDraw())
        return kTRUE;

    if (TestBit(kDoNotDisplay))
        return kTRUE;

    const TString tabname = fNameTab.IsNull() ? fH->GetName() : fNameTab.Data();

    fCanvas = 0;
    if (fDrawOption.Contains("same", TString::kIgnoreCase))
    {
        fCanvas = fDisplay->GetCanvas(tabname);
        if (!fCanvas)
            *fLog << warn << "WARNING - 'same' option given, but no tab with name '" << tabname << "' found." << endl;
    }

    if (!fCanvas)
    {
        const TString tabtitle = fTitleTab.IsNull() ? fH->GetTitle() : fTitleTab.Data();
        fCanvas = &fDisplay->AddTab(tabname, tabtitle);
    }

    fCanvas->cd();
    fH->Draw(fDrawOption);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Checks the parameter list for the existance of the parameter container. If
// the name of it was given in the constructor. It checks also for the
// existance of the histogram container in the parameter list if a name was
// given. If it is not available it tried to create a histogram container
// with the same type as the given object name.
//
Int_t MFillH::PreProcess(MParList *pList)
{
    if (fIndex)
    {
        if (!fIndex->PreProcess(pList))
        {
            *fLog << all << "PreProcessing of Index rule failed... aborting." << endl;
            return kFALSE;
        }

        if (!fIndex->IsValid())
        {
            *fLog << all << "Given Index rule invalid... aborting." << endl;
            return kFALSE;
        }
    }

    //
    // If the user defined the use of a weight: search for it.
    //
    if (!fWeight && !fWeightName.IsNull())
    {
        fWeight = (MParameterD*)pList->FindObject(fWeightName, "MParameterD");

	if (!fWeight)
        {
            *fLog << err << fWeightName << " [MParameterD] not found... aborting." << endl;
            return kFALSE;
        }
    }

    //
    // Try to get the histogram container with name fHName from list
    // or create one with this name
    //
    if (!fH)
    {
        const TString cls  = ExtractClass(fHName);
        const TString name = ExtractName(fHName);

        TObject *obj=NULL;
        if (cls==name)
            obj = pList->FindObject(fHName);

        if (!obj)
        {
            /*
            if (cls==name)
            *fLog << inf << "Object '" << fHName << "' not found in parlist... creating." << endl;
            */
            obj = pList->FindCreateObj(cls, name);
        }

        if (!obj)
            return kFALSE;

        //
        // We were successfull getting it. Check whether it really inherits
        // from MH, FindCreateObj does only check for inheritance from
        // 'type'.
        //
        TClass *tcls = fIndex ? MHArray::Class() : MH::Class();
        if (!obj->InheritsFrom(tcls))
        {
            *fLog << err << obj->GetName() << " doesn't inherit ";
            *fLog << "from " << tcls->GetName() << " - cannot be used for MFillH...";
            *fLog << "aborting." << endl;
            return kFALSE;
        }

        fH = (MH*)obj;
    }

    //
    // Now we have the histogram container available. Try to Setup Fill.
    //
    fH->SetSerialNumber(GetSerialNumber());
    fH->SetNumExecutions(0);
    if (!fH->SetupFill(pList))
    {
        *fLog << (TestBit(kCanSkip) ? warn : err);
        *fLog << (TestBit(kCanSkip) ? "WARNING" : "ERROR");
        *fLog << " - Calling SetupFill for " << fH->GetDescriptor() << "...";
        *fLog << (TestBit(kCanSkip) ? "skipped." : "aborting.") << endl;

        return TestBit(kCanSkip) ? kSKIP : kFALSE;
    }

    //
    // If also a parameter container is already set we are done.
    //
    if (fParContainer)
        return DrawToDisplay();

    //
    // This case means, that the MH sets up its container to be filled
    // by itself. Check there if it has something to be filled with!
    //
    if (fParContainerName.IsNull())
    {
        fParContainer = NULL;
        return DrawToDisplay();
    }

    fParContainer = (MParContainer*)pList->FindObject(fParContainerName);
    if (fParContainer)
        return DrawToDisplay();

    if (TestBit(kCanSkip))
    {
        *fLog << warn << fParContainerName << " [MParContainer] not found... skipped." << endl;
        return kSKIP;
    }

    *fLog << err << fParContainerName << " [MParContainer] not found... aborting." << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Call the ReInit function of the contained Histogram
//
Bool_t MFillH::ReInit(MParList *pList)
{
    return fH->ReInit(pList);
} 

// --------------------------------------------------------------------------
//
// Fills the data from the parameter conatiner into the histogram container
//
Int_t MFillH::Process()
{
    if (fIndex)
        ((MHArray*)fH)->SetIndexByKey(fIndex->GetValue());
    /*
     const Int_t key = (Int_t)fIndex->GetValue();
     const Int_t idx = fMapIdx->Add(key);
     ((MHArray*)fH)->SetIndex(idx);
     */

//    TVirtualPad *save = gPad;
//    if (fCanvas)
//        fCanvas->cd();

    fH->SetNumExecutions(GetNumExecutions());
    const Int_t rc = fH->Fill(fParContainer, fWeight?fWeight->GetVal():1);

//    if (save && fCanvas)
//        save->cd();
    return rc;
} 

// --------------------------------------------------------------------------
//
// Set the ReadyToSave flag of the histogram container, because now all data
// has been filled into the histogram.
//
Int_t MFillH::PostProcess()
{
    // Return if fH was never initialized.
    if (!fH)
        return kTRUE;

    //
    // Now all data is in the histogram. Maybe some final action is
    // necessary.
    //
    if (!fH->Finalize())
    {
        *fLog << err << "ERROR - Calling Finalize for ";
        *fLog << fH->GetDescriptor() << "... aborting." << endl;
        return kFALSE;
    }

    fH->SetReadyToSave();

    //
    // Check whether fDisplay has previously been used (fCanvas),
    // fDisplay is still open and the corresponding Canvas/Tab is
    // still existing.
    //
    if (fDisplay && fDisplay->HasCanvas(fCanvas))
    {
        const TString opt(MString::Format("nonew %s", fDrawOption.Data()));
        // fCanvas->cd();
        // fCanvas->Clear();

        // Remove the old class to prevent clashes calling
        // Paint-functions when the display is updated
        fCanvas->GetListOfPrimitives()->Remove(fH);
        fCanvas->GetListOfPrimitives()->Add(fH->Clone(), opt);
        fCanvas->Modified();
        fCanvas->Update();
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MFillH::StreamPrimitive(ostream &out) const
{
    if (fH)
        fH->SavePrimitive(out);

    if (fParContainer)
        fParContainer->SavePrimitive(out);

    if (fWeight)
        fWeight->SavePrimitive(out);

    out << "   MFillH " << GetUniqueName() << "(";

    if (fH)
        out << "&" << fH->GetUniqueName();
    else
        out << "\"" << fHName << "\"";

    if (fParContainer)
        out << ", &" << fParContainer->GetUniqueName();
    else
        if (!fParContainerName.IsNull())
            out << ", \"" << fParContainerName << "\"";

    out << ");" << endl;

    if (fWeight || !fWeightName.IsNull())
    {
        out << "   " << GetUniqueName() << ".SetWeight(";
        if (fWeight)
            out << "&" << fWeight->GetUniqueName() << ");" << endl;
        else
            if (!fWeightName.IsNull())
                out << "\"" << fWeightName << "\");" << endl;
    }

    if (fIndex)
    {
        out << "   " << GetUniqueName() << ".SetRuleForIdx(\"";
        out << fIndex->GetRule() << "\");" << endl;
    }
}
