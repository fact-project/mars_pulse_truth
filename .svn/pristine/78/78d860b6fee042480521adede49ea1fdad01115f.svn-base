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
!   Author(s): Thomas Bretz  2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2010
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MH3
//
// With this histogram you can fill a histogram with up to three
// variables from Mars parameter containers in an eventloop.
//
// In the constructor you can give up to three variables which should be
// filled in the histogram. Dependend on the number of given variables
// (data members) a TH1D, TH2D or TH3D is created.
// Specify the data mamber like the following:
//   "MHillas.fLength"
// Where MHillas is the name of the parameter container in the parameter
// list and fLength is the name of the data member which should be filled
// in the histogram. Assuming that your MHillas container has a different
// name (MyHillas) the name to give would be:
//   "MyHillas.fLength"
//
// If you want to use a different unit for histogramming use SetScaleX,
// SetScaleY and SetScaleZ.
//
//
// Binning/Binning  name
// =====================
//
// The axis binning is retrieved from the parameter list, too. Create a
// MBinning with the name "Binning" plus the name of your MH3 container
// plus the axis name ("X", "Y" or "Z") and add it to the parameter list.
//
// If the binning should have a different name than the histogram name
// the binning name can be added to the name, eg.:
//    SetName("MyHistName;MyXBins;MyYBins")
// Instead of BinningMyHistName[XYZ] the parameter list will be searched
// for BinningMyXBinning, BinningMyYBins and BinningMyHistNameZ
//
// If you don't want to use a MBinning object from the parameter list
// you can also set one directly, for example
//    MBinning bins(10, 0, 1);
//    mh3.SetBinningX(&bins);
// You must not delete the MBinning object before the class has been
// PreProcessed.
//
//
// Axis titles
// ===========
//
// 1) If no other title is given the rule for this axis is used.
// 2) If the MBinning used for this axis has a non-default Title
//    (MBinning::HasTitle) this title is used for the corresponding axis
// 3) If the MH3 has a non-default title (MH3::SetTitle called)
//    this title is set as the histogram title. It can be used to overwrite
//    the axis titles. For more information see TH1::SetTitle, eg.
//       SetTitle("MyHist;x[mm];y[cm];Counts");
//
//
// Labels
// ======
//
// To use labels at an axis you have to initialize this for the axis
// by either calling InitLabels(Labels_t) or setiting a DefaultLabel.
// For the axis for which the labels have been initialized the
// number returned by the given corresponding phrase is converted
// to int with TMath::Nint and used as a label. If you want to replace
// this id by a named label you can call DefineLabel to do that.
// Several ids can be replaced by the same label. If you define
// named labels for every label which was not defined the default
// is used, if any, otherwise an unnamed label is created.
//
// In the case of an axis with labels the axis-title cannot be
// set via a MBinning, because the MBinning is not evaluated.
//
// Please note that for some reason not all combinations of
// labels, dimensions and weights are available in the root-
// histogram classes. Please check the MH3::Fill function to see
// whether your combination is supported.
//
//
// Examples:
// =========
//
//   1) MH3 myhist("MHillas.fLength");
//      myhist.SetName("MyHist");
//      myhist.SetScaleX(geomcam.GetConvMm2Deg()); //convert length to degree
//      MBinning bins("BinningMyHistX", "Title for my x-axis [Hz]");
//      bins.SetEdges(10, 0, 150);
//      plist.AddToList(&bins);
//
//   2) MH3 myhist("MHillas.fLength");
//      myhist.SetName("MyHist;MyX");
//      myhist.SetTitle("Histogram Title;X-Title [mm];Counts");
//      MBinning bins("BinningMyX");
//      bins.SetEdges(10, 0, 150);
//      plist.AddToList(&bins);
//
//   3) MH3 myhist("MTriggerPatter.GetUnprescaled");
//      myhist.SetWeight("1./MRawRunHeader.GetRunLength");
//      myhist.SetTitle("Rate of the trigger pattern [Hz];Run Number;Trigger Pattern;Rate [Hz]");
//      myhist.InitLabels(MH3::kLabelsXY);
//      myhist.DefaultLabelY("UNKNOWN");     // Lvl1
//      myhist.DefineLabelY( 1, "Trig");     // Lvl1
//      myhist.DefineLabelY( 2, "Cal");      // Cal
//      myhist.DefineLabelY( 4, "Trig");     // Lvl2
//      myhist.DefineLabelY( 8, "Ped");      // Ped
//
//
// Class Version 1:
// ----------------
//   - MData      *fData[3];
//   + MData      *fData[4];
//
// Class Version 2:
// ----------------
//   - MDataChain *fData[3];        // Object from which the data is filled
//   + MData      *fData[3];        // Object from which the data is filled
//
// Class Version 3:
// ----------------
//   - Byte_t   fStyleBits
//   + MBinning fBins[3]
//
// Class Version 5:
// ----------------
//   + TFormula *fConversion
//
// Class Version 6:
// ----------------
//   + MData      *fWeight;
//
// Class Version 7:
// ----------------
//   +    MData      *fData[4]
//   +    Double_t    fScale[4]
//   -    MData      *fData[3]
//   -    Double_t    fScale[3]
//
/////////////////////////////////////////////////////////////////////////////
#include "MH3.h"

#include <ctype.h>    // tolower
#include <stdlib.h>   // atoi (Ubuntu 8.10)
#include <fstream>

#include <TMath.h>
#include <TFormula.h>

#include <THashList.h>
#include <TObjString.h>

//#include <TPad.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TProfile3D.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"

#include "MParList.h"
#include "MBinning.h"
#include "MDataPhrase.h"

ClassImp(MH3);

using namespace std;

const TString MH3::gsDefName  = "MH3";
const TString MH3::gsDefTitle = "Container for a n-D Mars Histogram";

// --------------------------------------------------------------------------
//
// Set fStyleBits to 0, Reset fBins to NULL, fScale to 1, set name and title
// to gsDefName and gsDefTitle and if fHist!=NULL UseCurrentStyle and
// SetDirectory(0)
//
void MH3::Init()
{
    fStyleBits = 0;

    fWeight  = NULL;

    fData[0] = NULL;
    fData[1] = NULL;
    fData[2] = NULL;
    fData[3] = NULL;

    fBins[0] = NULL;
    fBins[1] = NULL;
    fBins[2] = NULL;

    fScale[0] = 1;
    fScale[1] = 1;
    fScale[2] = 1;
    fScale[3] = 1;

    fConversion = NULL;

    fName  = gsDefName;
    fTitle = gsDefTitle;

    if (!fHist)
        return;

    fHist->UseCurrentStyle();
    fHist->SetDirectory(NULL);
}

// --------------------------------------------------------------------------
//
// Default constructor.
//
MH3::MH3(const Int_t dim, Type_t type) : fDimension(dim), fHist(NULL)
{
    // FIXME?
    switch (type)
    {
    case kHistogram:
        if (fDimension>3)
            fDimension=3;
        break;
    case kProfile:
    case kProfileSpread:
        fDimension = -TMath::Abs(fDimension);
        if (fDimension<-2)
            fDimension = -2;
        break;
    }

    switch (fDimension)
    {
    case 1:
        fHist = new TH1D;
        fHist->SetYTitle("Counts");
        break;
    case -1:
        fHist = new TProfile;
        fHist->SetYTitle("Average");
        if (type==kProfileSpread)
            static_cast<TProfile*>(fHist)->SetErrorOption("s");
        break;
    case 2:
        fHist = new TH2D;
        fHist->SetZTitle("Counts");
        break;
    case -2:
        fHist = new TProfile2D;
        fHist->SetZTitle("Average");
        static_cast<TProfile2D*>(fHist)->BuildOptions(0, 0, type==kProfileSpread?"s":"");
        break;
    case 3:
        fHist = new TH3D;
        break;
    case -3:
        fHist = new TProfile2D;
        fHist->SetZTitle("Average");
        static_cast<TProfile2D*>(fHist)->BuildOptions(0, 0, type==kProfileSpread?"s":"");
        break;
    }

    Init();
}

// --------------------------------------------------------------------------
//
// Creates an TH1D. memberx is filled into the X-bins. For a more detailed
// description see the class description above.
//
MH3::MH3(const char *memberx, Type_t type) : fDimension(1)
{
    fHist = new TH1D;
    fHist->SetYTitle("Counts");

    Init();

    fData[0] = new MDataPhrase(memberx);
}

// --------------------------------------------------------------------------
//
// Adapt a given histogram
//
MH3::MH3(const TH1 &h1) : fDimension(1)
{
    if (h1.InheritsFrom(TH3::Class()))
        fDimension = 3;
    if (h1.InheritsFrom(TH2::Class()))
        fDimension = 2;

    if (h1.InheritsFrom(TProfile3D::Class()))
    {
        fDimension = -3;
        *fLog << warn << "WARNING - MH3::MH3(TH1&) does not support TProfile3D." << endl;
    }
    if (h1.InheritsFrom(TProfile2D::Class()))
        fDimension = -2;
    if (h1.InheritsFrom(TProfile::Class()))
        fDimension = -1;

    fHist = (TH1*)h1.Clone();

    Init(); // Before without SeUseCurrentStyle!

    switch (fDimension)
    {
    case 3:
    case -2:
        fData[2] = new MDataPhrase(h1.GetZaxis()->GetTitle());
    case 2:
    case -1:
        fData[1] = new MDataPhrase(h1.GetYaxis()->GetTitle());
    case 1:
        fData[0] = new MDataPhrase(h1.GetXaxis()->GetTitle());
    }
}

// --------------------------------------------------------------------------
//
// Creates an TH2D. memberx is filled into the X-bins. membery is filled
// into the Y-bins. For a more detailed description see the class
// description above.
//
MH3::MH3(const char *memberx, const char *membery, Type_t type)
    : fDimension(type==kHistogram?2:-1)
{

    switch (fDimension)
    {
    case 2:
        fHist = static_cast<TH1*>(new TH2D);
        break;
    case -1:
        fHist = static_cast<TH1*>(new TProfile);
        if (type==kProfileSpread)
            static_cast<TProfile*>(fHist)->SetErrorOption("s");

        break;
    }

    fHist->SetZTitle(fDimension>0?"Counts":"Average");

    Init();

    fData[0] = new MDataPhrase(memberx);
    fData[1] = new MDataPhrase(membery);
}

// --------------------------------------------------------------------------
//
// Creates an TH3D. memberx is filled into the X-bins. membery is filled
// into the Y-bins. membery is filled into the Z-bins. For a more detailed
// description see the class description above.
//
MH3::MH3(const char *memberx, const char *membery, const char *memberz, Type_t type)
    : fDimension(type==kHistogram?3:-2)
{
    switch (fDimension)
    {
    case 3:
        fHist = static_cast<TH1*>(new TH3D);
        break;
    case -2:
        fHist = static_cast<TH1*>(new TProfile2D);
        static_cast<TProfile2D*>(fHist)->BuildOptions(0, 0, type==kProfileSpread?"s":"");
    }

    Init();

    fData[0] = new MDataPhrase(memberx);
    fData[1] = new MDataPhrase(membery);
    fData[2] = new MDataPhrase(memberz);
}

// --------------------------------------------------------------------------
//
// Creates an TH3D. memberx is filled into the X-bins. membery is filled
// into the Y-bins. membery is filled into the Z-bins. Weight is used as a
// weight for the profile histogram. For a more detailed description see the
// class description above.
//
MH3::MH3(const char *memberx, const char *membery, const char *memberz, const char *weight, Type_t type)
    : fDimension(-3)
{
    fHist = static_cast<TH1*>(new TProfile3D);
    static_cast<TProfile3D*>(fHist)->BuildOptions(0, 0, type==kProfileSpread?"s":"");

    Init();

    fData[0] = new MDataPhrase(memberx);
    fData[1] = new MDataPhrase(membery);
    fData[2] = new MDataPhrase(memberz);
    fData[3] = new MDataPhrase(weight);
}

// --------------------------------------------------------------------------
//
// Deletes the histogram
//
MH3::~MH3()
{
    if (fHist)
        delete fHist;

    if (fConversion)
        delete fConversion;

    if (fWeight)
        delete fWeight;

    for (int i=0; i<4; i++)
        if (fData[i])
            delete fData[i];

    for (int i=0; i<3; i++)
        if (fLabels[i].GetDefault())
            delete fLabels[i].GetDefault();
}

// --------------------------------------------------------------------------
//
// You can set a weight as a phrase additionally to the one given
// as an argument to Fill (most likely from MFillH). The two weights
// are multiplied together.
//
void MH3::SetWeight(const char *phrase)
{
    if (fWeight)
        delete fWeight;
    fWeight = new MDataPhrase(phrase);
}

// --------------------------------------------------------------------------
//
// Set a function which is applied to the histogram before it is displayed.
// Note, that it only effects the displayed histogram.
//
//   e.g. SetConversion("sqrt(x)");
//
Bool_t MH3::SetConversion(const char *func)
{
    if (TString(func).IsNull())
    {
        delete fConversion;
        fConversion = 0;
        return kTRUE;
    }

    fConversion = new TFormula;

    // Must have a name otherwise all axis labels disappear like a miracle
    fConversion->SetName("ConversionFunction");
    if (fConversion->Compile(func))
    {
        *fLog << err << dbginf << "Syntax Error: TFormula::Compile failed for " << func << endl;
        delete fConversion;
        fConversion = 0;
        return kFALSE;
    }

    gROOT->GetListOfFunctions()->Remove(fConversion);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// The axis label is centered and the labeling of the axis is initialized.
//
// This function must not be called after any label has been created!
//
void MH3::InitLabels(TAxis &x) const
{
    x.CenterTitle();
    x.SetBinLabel(1, "");
    x.LabelsOption("h");  // FIXME: Is "a" thread safe? (Paint and Fill?)
    x.GetLabels()->Delete();
}

// --------------------------------------------------------------------------
//
// Depending on the bits set the InitLabels(TAxis&) function for
// the corresponding axes are called. In any case the kCanRebin bit
// is set.
//
// This function must not be called after any label has been created!
//
void MH3::InitLabels(Labels_t type) const
{
    if (!fHist)
        return;

    if (type&kLabelsX && fHist->GetXaxis())
        InitLabels(*fHist->GetXaxis());

    if (type&kLabelsY && fHist->GetYaxis())
        InitLabels(*fHist->GetYaxis());

    if (type&kLabelsZ && fHist->GetZaxis())
        InitLabels(*fHist->GetZaxis());

    if (type&kLabelsXYZ)
        fHist->SetBit(TH1::kCanRebin);
}

// --------------------------------------------------------------------------
//
// Return the corresponding Labels_t describing for which axis
// axis-labels are switched on.
//
MH3::Labels_t MH3::GetLabels() const
{
    UInt_t type = kNoLabels;
    if (fHist->GetXaxis() && fHist->GetXaxis()->GetLabels())
        type |= kLabelsX;
    if (fHist->GetYaxis() && fHist->GetYaxis()->GetLabels())
        type |= kLabelsY;
    if (fHist->GetZaxis() && fHist->GetZaxis()->GetLabels())
        type |= kLabelsZ;
    return (Labels_t)type;
}

// --------------------------------------------------------------------------
//
// Calls the LabelsDeflate from the histogram for all axes.
// LabelsDeflate will just do nothing if the axis has no labels
// initialized.
//
void MH3::DeflateLabels() const
{
    fHist->LabelsDeflate("X");
    fHist->LabelsDeflate("Y");
    fHist->LabelsDeflate("Z");
}

// --------------------------------------------------------------------------
//
// Returns the named label corresponding to the given value
// and the given axis. The names are defined with the
// DefineLabel-functions. if no name is defined the value
// is converted to a string with %d and TMath::Nint.
// If names are defined, but not for the given value, the default
// label is returned instead. If no default is defined the
// %d-converted string is returned.
//
const char *MH3::GetLabel(Int_t axe, Double_t val) const
{
    const Int_t v = TMath::Nint(val);

    if (fLabels[axe].GetSize())
    {
        const char *l = fLabels[axe].GetObjName(v);
        if (l)
            return l;
    }

    return Form("%d", v);
}

// --------------------------------------------------------------------------
//
// Return the data members used by the data chain to be used in
// MTask::AddBranchToList
//
TString MH3::GetDataMember() const
{
    TString str=fData[0]->GetDataMember();

    for (int i=1; i<4; i++)
        if (fData[i])
        {
            str += ";";
            str += fData[i]->GetDataMember();
        }

    return str;
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning are found in the parameter list
// For a more detailed description see class description above.
//
Bool_t MH3::SetupFill(const MParList *plist)
{
    // reset histogram (necessary if the same eventloop is run more than once)
    if (!TestBit(kDoNotReset))
        fHist->Reset();

    // Tokenize name into name and binnings names
    TObjArray *tok = fName.Tokenize(";");

    const TString name = (*tok)[0] ? (*tok)[0]->GetName() : fName.Data();

    TString bx = (*tok)[1] ? (*tok)[1]->GetName() : Form("%sX", name.Data());
    TString by = (*tok)[2] ? (*tok)[2]->GetName() : Form("%sY", name.Data());
    TString bz = (*tok)[3] ? (*tok)[3]->GetName() : Form("%sZ", name.Data());

    bx.Prepend("Binning");
    by.Prepend("Binning");
    bz.Prepend("Binning");

    delete tok;

    MBinning *binsx = NULL;
    MBinning *binsy = NULL;
    MBinning *binsz = NULL;

    const Labels_t labels = GetLabels();

    switch (TMath::Abs(fDimension))
    {
    case 3:
        if (fData[2])
            fHist->SetZTitle(fData[2]->GetTitle());
        if (!(labels&kLabelsZ))
        {
            binsz = fBins[2] ? fBins[2] : (MBinning*)plist->FindObject(bz, "MBinning");
            if (!binsz)
            {
                *fLog << err << dbginf << "MBinning '" << bz << "' not found... aborting." << endl;
                return kFALSE;
            }
            if (binsz->HasTitle())
                fHist->SetZTitle(binsz->GetTitle());
            if (binsz->IsLogarithmic())
                fHist->SetBit(kIsLogz);
        }
    case 2:
        if (fData[1])
            fHist->SetYTitle(fData[1]->GetTitle());
        if (!(labels&kLabelsY))
        {
            binsy = fBins[1] ? fBins[1] : (MBinning*)plist->FindObject(by, "MBinning");
            if (!binsy)
            {
                *fLog << err << dbginf << "MBinning '" << by << "' not found... aborting." << endl;
                return kFALSE;
            }
            if (binsy->HasTitle())
                fHist->SetYTitle(binsy->GetTitle());
            if (binsy->IsLogarithmic())
                fHist->SetBit(kIsLogy);
        }
    case 1:
        if (fData[0]!=NULL)
            fHist->SetXTitle(fData[0]->GetTitle());
        if (!(labels&kLabelsX))
        {
            binsx = fBins[0] ? fBins[0] : (MBinning*)plist->FindObject(bx, "MBinning");
            if (!binsx)
            {
                if (fDimension==1)
                    binsx = (MBinning*)plist->FindObject("Binning"+fName, "MBinning");

                if (!binsx)
                {
                    *fLog << err << dbginf << "Neither '" << bx << "' nor 'Binning" << fName << "' found... aborting." << endl;
                    return kFALSE;
                }
            }
            if (binsx->HasTitle())
                fHist->SetXTitle(binsx->GetTitle());
            if (binsx->IsLogarithmic())
                fHist->SetBit(kIsLogx);
        }
    }

    // PreProcess existing fData members
    for (int i=0; i<4; i++)
        if (fData[i] && !fData[i]->PreProcess(plist))
            return kFALSE;

    if (fWeight && !fWeight->PreProcess(plist))
        return kFALSE;

    TString title(fDimension>0?"Histogram":"Profile");
    title += " for ";
    title += name;
    title += Form(" (%dD)", TMath::Abs(fDimension));

    fHist->SetName(name);
    fHist->SetTitle(fTitle==gsDefTitle ? title : fTitle);
    fHist->SetDirectory(0);

    // This is for the case we have set lables
    const MBinning def(1, 0, 1);
    if (!binsx)
        binsx = const_cast<MBinning*>(&def);
    if (!binsy)
        binsy = const_cast<MBinning*>(&def);
    if (!binsz)
        binsz = const_cast<MBinning*>(&def);

    // set binning
    switch (TMath::Abs(fDimension))
    {
    case 1:
        SetBinning(*fHist, *binsx);
        return kTRUE;
    case 2:
        SetBinning(static_cast<TH2&>(*fHist), *binsx, *binsy);
        return kTRUE;
    case 3:
        SetBinning(static_cast<TH3&>(*fHist), *binsx, *binsy, *binsz);
        return kTRUE;
    }

    *fLog << err << "ERROR - MH3 has " << TMath::Abs(fDimension) << " dimensions!" << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Set the name of the histogram ant the MH3 container
//
void MH3::SetName(const char *name)
{
    if (fHist)
    {
        if (gPad)
        {
            const TString pfx(MString::Format("%sProfX", fHist->GetName()));
            const TString pfy(MString::Format("%sProfY", fHist->GetName()));

            TProfile *p = 0;
            if ((p=dynamic_cast<TProfile*>(gPad->FindObject(pfx))))
                p->SetName(MString::Format("%sProfX", name));
            if ((p=dynamic_cast<TProfile*>(gPad->FindObject(pfy))))
                p->SetName(MString::Format("%sProfY", name));
        }

        fHist->SetName(name);
        fHist->SetDirectory(0);

    }
    MParContainer::SetName(name);
}

// --------------------------------------------------------------------------
//
// Set the title of the histogram ant the MH3 container
//
void MH3::SetTitle(const char *title)
{
    if (fHist)
        fHist->SetTitle(title);
    MParContainer::SetTitle(title);
}

// --------------------------------------------------------------------------
//
// Fills the one, two or three data members into our histogram
//
Int_t MH3::Fill(const MParContainer *par, const Stat_t ww)
{
    // Get Information about labels (UInt_t, to supress warning about
    // unhandeled cases in switch)
    const UInt_t type = GetLabels();

    // Get values for axis
    Double_t x=0;
    Double_t y=0;
    Double_t z=0;
    Double_t t=0;
    Double_t w=ww;

    switch (fDimension)
    {
    case -3:
        t = fData[3]->GetValue()*fScale[3];
    case -2:
    case  3:
        z = fData[2]->GetValue()*fScale[2];
    case -1:
    case  2:
        y = fData[1]->GetValue()*fScale[1];
    case  1:
        x = fData[0]->GetValue()*fScale[0];
    }

    if (fWeight)
        w *= fWeight->GetValue();

    // If label option is set, convert value to label
    TString labelx, labely, labelz;
    if (type&kLabelsX)
        labelx = GetLabel(0, x);
    if (type&kLabelsY)
        labely = GetLabel(1, y);
    if (type&kLabelsZ)
        labelz = GetLabel(2, z);

    // Fill histogram
    switch (fDimension)
    {
    case  3:
        switch (type)
        {
        case kNoLabels:
            static_cast<TH3*>(fHist)->Fill(x,      y,      z,      w);
            return kTRUE;
        case kLabelsX:
#if ROOT_VERSION_CODE < ROOT_VERSION(5,32,01)
            static_cast<TH3*>(fHist)->Fill(labelx, y,      z);
#endif
            return kTRUE;
        case kLabelsY:
            static_cast<TH3*>(fHist)->Fill(x,      labely, z,      w);
            return kTRUE;
        case kLabelsZ:
            static_cast<TH3*>(fHist)->Fill(x,      y,      labelz, w);
            return kTRUE;
        case kLabelsXY:
            static_cast<TH3*>(fHist)->Fill(labelx, labely, z,      w);
            return kTRUE;
        case kLabelsXZ:
            static_cast<TH3*>(fHist)->Fill(labelx, y,      labelz, w);
            return kTRUE;
        case kLabelsYZ:
            static_cast<TH3*>(fHist)->Fill(x,      labely, labelz, w);
            return kTRUE;
        case kLabelsXYZ:
            static_cast<TH3*>(fHist)->Fill(labelx, labely, labelz, w);
            return kTRUE;
        }
        break;
    case  2:
        switch (type)
        {
        case kNoLabels:
            static_cast<TH2*>(fHist)->Fill(x,      y,      w);
            return kTRUE;
        case kLabelsX:
            static_cast<TH2*>(fHist)->Fill(x,      labely, w);
            return kTRUE;
        case kLabelsY:
            static_cast<TH2*>(fHist)->Fill(labelx, y,      w);
            return kTRUE;
        case kLabelsXY:
            static_cast<TH2*>(fHist)->Fill(labelx, labely, w);
            return kTRUE;
        }
        break;
    case 1:
        switch (type)
        {
        case kNoLabels:
            fHist->Fill(x,      w);
            return kTRUE;
        case kLabelsX:
            fHist->Fill(labelx, w);
            return kTRUE;
        }
        break;
    case -1:
        switch (type)
        {
        case kNoLabels:
            static_cast<TProfile*>(fHist)->Fill(x,      y, w);
            return kTRUE;
        case kLabelsX:
            static_cast<TProfile*>(fHist)->Fill(labelx, y, w);
            return kTRUE;
        }
        break;
    case -2:
        switch (type)
        {
        case kNoLabels:
            static_cast<TProfile2D*>(fHist)->Fill(x,      y,      z, w);
            return kTRUE;
        case kLabelsX:
            static_cast<TProfile2D*>(fHist)->Fill(labelx, y,      z);
            return kTRUE;
        case kLabelsY:
            static_cast<TProfile2D*>(fHist)->Fill(x,      labely, z);
            return kTRUE;
        case kLabelsXY:
            static_cast<TProfile2D*>(fHist)->Fill(labelx, labely, z);
            return kTRUE;
        }
        break;
    case  -3:
        switch (type)
        {
        case kNoLabels:
            static_cast<TProfile3D*>(fHist)->Fill(x, y, z, t, w);
            return kTRUE;
        default:
            *fLog << err << "ERROR - Labels not supported in TProfile3D." << endl;
        }
        break;
    }

    *fLog << err << "MH3::Fill: ERROR - A fatal error occured." << endl;
    return kERROR;
}

// --------------------------------------------------------------------------
//
// If an auto range bit is set the histogram range of the corresponding
// axis is set to show only the non-empty bins (with a single empty bin
// on both sides)
//
Bool_t MH3::Finalize()
{
    DeflateLabels();

    Bool_t autorangex=TESTBIT(fStyleBits, 0);
    Bool_t autorangey=TESTBIT(fStyleBits, 1);
    //Bool_t autorangez=TESTBIT(fStyleBits, 2);

    Int_t lo, hi;
    if (autorangex)
    {
        GetRangeX(*fHist, lo, hi);
        fHist->GetXaxis()->SetRange(lo-2, hi+1);
    }
    if (autorangey)
    {
        GetRangeY(*fHist, lo, hi);
        fHist->GetYaxis()->SetRange(lo-2, hi+1);
    }
    /*
    if (autorangez)
    {
        GetRangeZ(*fHist, lo, hi);
        fHist->GetZaxis()->SetRange(lo-2, hi+1);
    }
    */
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Apply the conversion function to the contents stored in fHist and
// store the result in h.
//
// In the case of a TProfile we keep it a TProfile to keep the mean and
// rms in y displayed. To get the error correctly we have to reverse
// the calculation done in TProfile::GetBinError of course.
//
void MH3::Convert(TH1 &h) const
{
    const Bool_t prof = h.InheritsFrom(TProfile::Class()) || h.InheritsFrom(TProfile2D::Class()) || h.InheritsFrom(TProfile3D::Class());

    for (Int_t z=0; z<=h.GetNbinsZ()+1; z++)
        for (Int_t y=0; y<=h.GetNbinsY()+1; y++)
            for (Int_t x=0; x<=h.GetNbinsX()+1; x++)
            {
                h.SetBinContent(x, y, z, fConversion->Eval(fHist->GetBinContent(x, y, z)));

                if (prof)
                    h.SetBinError(x, y, z, TMath::Hypot(fConversion->Eval(fHist->GetBinContent(x, y, z)),
                                                        fConversion->Eval(fHist->GetBinError(  x, y, z))));
                else
                    h.SetBinError(x, y, z, fConversion->Eval(fHist->GetBinError(x, y, z)));
            }

    TProfile *p1 = dynamic_cast<TProfile*>(fHist);
    if (p1)
        for (Int_t i=0; i<p1->GetSize(); i++)
            static_cast<TProfile&>(h).SetBinEntries(i, p1->GetBinEntries(i)>0 ? 1 : 0);

    TProfile2D *p2 = dynamic_cast<TProfile2D*>(fHist);
    if (p2)
        for (Int_t i=0; i<p2->GetSize(); i++)
            static_cast<TProfile2D&>(h).SetBinEntries(i, p2->GetBinEntries(i)>0 ? 1 : 0);

    TProfile3D *p3 = dynamic_cast<TProfile3D*>(fHist);
    if (p3)
        for (Int_t i=0; i<p3->GetSize(); i++)
            static_cast<TProfile3D&>(h).SetBinEntries(i, p3->GetBinEntries(i)>0 ? 1 : 0);
}

// --------------------------------------------------------------------------
//
// FIXME
//
void MH3::Paint(Option_t *o)
{
    TProfile *p=0;

    if (TMath::Abs(fDimension)==2)
        MH::SetPalette("pretty");

    if (fConversion)
    {
        TH1 *h = 0;
        if ((h=dynamic_cast<TH1*>(gPad->FindObject(fHist->GetName()))))
            Convert(*h);
    }

    const TString pfx(MString::Format("%sProfX", fHist->GetName()));
    if ((p=dynamic_cast<TProfile*>(gPad->FindObject(pfx))))
    {
        Int_t col = p->GetLineColor();
        p = ((TH2*)fHist)->ProfileX(pfx, -1, -1, "s");
        p->SetLineColor(col);
    }

    const TString pfy(MString::Format("%sProfY", fHist->GetName()));
    if ((p=dynamic_cast<TProfile*>(gPad->FindObject(pfy))))
    {
        Int_t col = p->GetLineColor();
        p = ((TH2*)fHist)->ProfileY(pfy, -1, -1, "s");
        p->SetLineColor(col);
    }
/*
    if (fHist->TestBit(kIsLogx) && fHist->GetEntries()>0) gPad->SetLogx();
    if (fHist->TestBit(kIsLogy) && fHist->GetEntries()>0) gPad->SetLogy();
    if (fHist->TestBit(kIsLogz) && fHist->GetEntries()>0) gPad->SetLogz();
    */
}

// --------------------------------------------------------------------------
//
// If Xmax is < 3000*Xmin SetMoreLogLabels is called. If Xmax<5000
// the exponent is switched off (SetNoExponent)
//
void MH3::HandleLogAxis(TAxis &axe) const
{
    if (axe.GetXmax()>3000*axe.GetXmin())
        return;

    axe.SetMoreLogLabels();
    if (axe.GetXmax()<5000)
        axe.SetNoExponent();
}

// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the histogram into it.
//
// Possible options are:
//   PROFX: Draw a x-profile into the histogram (for 2D histograms only)
//   PROFY: Draw a y-profile into the histogram (for 2D histograms only)
//   ONLY:  Draw the profile histogram only (for 2D histograms only)
//   BLUE:  Draw the profile in blue color instead of the histograms
//          line color
//
// If the kIsLog?-Bit is set the axis is displayed lkogarithmically.
// eg this is set when applying a logarithmic MBinning
//
// Be careful: The histogram belongs to this object and won't get deleted
// together with the canvas.
//
void MH3::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(fHist);
    pad->SetBorderMode(0);
    pad->SetGridx();
    pad->SetGridy();

    AppendPad();

    if (fHist->TestBit(kIsLogx))
    {
        pad->SetLogx();
        HandleLogAxis(*fHist->GetXaxis());
    }
    if (fHist->TestBit(kIsLogy))
    {
        pad->SetLogy();
        HandleLogAxis(*fHist->GetYaxis());
    }
    if (fHist->TestBit(kIsLogz))
    {
        pad->SetLogz();
        HandleLogAxis(*fHist->GetZaxis());
    }

    fHist->SetFillStyle(4000);

    TString str(opt);
    str.ToLower();
    str.ReplaceAll(" ", "");

    if (GetLabels())
    {
        if (str.IsNull() && fDimension==2)
            str = "colz";

        if (str.Contains("box", TString::kIgnoreCase) && fDimension==2)
            fHist->SetLineColor(kBlue);
    }

    const Bool_t only  = str.Contains("only")  && TMath::Abs(fDimension)==2;
    const Bool_t same  = str.Contains("same")  && TMath::Abs(fDimension)<3;
    const Bool_t blue  = str.Contains("blue")  && TMath::Abs(fDimension)==2;
    const Bool_t profx = str.Contains("profx") && TMath::Abs(fDimension)==2;
    const Bool_t profy = str.Contains("profy") && TMath::Abs(fDimension)==2;

    str.ReplaceAll("only",  "");
    str.ReplaceAll("blue",  "");
    str.ReplaceAll("profx", "");
    str.ReplaceAll("profy", "");

    if (same && TMath::Abs(fDimension)==1)
    {
        fHist->SetLineColor(kBlue);
        fHist->SetMarkerColor(kBlue);
    }


    TH1 *h = fHist;

    if (fConversion)
    {
        h = static_cast<TH1*>(fHist->Clone());

        h->SetDirectory(0);
        h->SetBit(kCanDelete);

        Convert(*h);
    }

    // FIXME: We may have to remove all our own options from str!
    if (!only)
        h->Draw(str);

    TProfile *p=0;
    if (profx)
    {
        const TString pfx(MString::Format("%sProfX", h->GetName()));

        if (same && (p=dynamic_cast<TProfile*>(gPad->FindObject(pfx))))
            *fLog << warn << "TProfile " << pfx << " already in pad." << endl;

        p = ((TH2*)h)->ProfileX(pfx, -1, -1, "s");
        p->UseCurrentStyle();
        p->SetLineColor(blue ? kBlue : h->GetLineColor());
        p->SetBit(kCanDelete);
        p->SetDirectory(NULL);
        p->SetXTitle(h->GetXaxis()->GetTitle());
        p->SetYTitle(h->GetYaxis()->GetTitle());
        p->Draw(only&&!same?"":"same");
    }
    if (profy)
    {
        const TString pfy(MString::Format("%sProfY", h->GetName()));

        if (same && (p=dynamic_cast<TProfile*>(gPad->FindObject(pfy))))
            *fLog << warn << "TProfile " << pfy << " already in pad." << endl;

        p = ((TH2*)h)->ProfileY(pfy, -1, -1, "s");
        p->UseCurrentStyle();
        p->SetLineColor(blue ? kBlue : h->GetLineColor());
        p->SetBit(kCanDelete);
        p->SetDirectory(NULL);
        p->SetYTitle(h->GetXaxis()->GetTitle());
        p->SetXTitle(h->GetYaxis()->GetTitle());
        p->Draw(only&&!same?"":"same");
    }

    //AppendPad("log");
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MH3::StreamPrimitive(ostream &out) const
{
    TString name = GetUniqueName();

    out << "   MH3 " << name << "(\"";
    out << fData[0]->GetRule() << "\"";
    if (fDimension>1 || fDimension<0)
        out << ", \"" << fData[1]->GetRule() << "\"";
    if (fDimension>2 || fDimension<-1)
        out << ", \"" << fData[2]->GetRule() << "\"";

    out << ");" << endl;

    if (fName!=gsDefName)
        out << "   " << name << ".SetName(\"" << fName << "\");" << endl;

    if (fTitle!=gsDefTitle)
        out << "   " << name << ".SetTitle(\"" << fTitle << "\");" << endl;

    if (fWeight)
        out << "   " << name << ".SetWeight(\"" << fWeight->GetRule() << "\");" << endl;

    switch (fDimension)
    {
    case -3:
        if (fScale[3]!=1)
            out << "   " << name << ".SetScaleT(" << fScale[3] << ");" << endl;
    case -2:
    case 3:
        if (fScale[2]!=1)
            out << "   " << name << ".SetScaleZ(" << fScale[2] << ");" << endl;
    case -1:
    case 2:
        if (fScale[1]!=1)
            out << "   " << name << ".SetScaleY(" << fScale[1] << ");" << endl;
    case 1:
        if (fScale[0]!=1)
            out << "   " << name << ".SetScaleX(" << fScale[0] << ");" << endl;
    }
}

MH3::Type_t MH3::GetType() const
{
    switch (fDimension)
    {
    case -1:
        return TString(static_cast<TProfile*>(fHist)->GetErrorOption())=="s" ? kProfileSpread : kProfile;
    case -2:
        return TString(static_cast<TProfile2D*>(fHist)->GetErrorOption())=="s" ? kProfileSpread : kProfile;
    case -3:
        return TString(static_cast<TProfile3D*>(fHist)->GetErrorOption())=="s" ? kProfileSpread : kProfile;
    }
    return kHistogram;
}

// --------------------------------------------------------------------------
//
// Used to rebuild a MH3 object of the same type (data members,
// dimension, ...)
//
MParContainer *MH3::New() const
{
    // FIXME: TREAT THE NEW OPTIONS CORRECTLY (PROFILE, LABELS)

    MH3 *h = NULL;

    if (fData[0] == NULL)
        h=new MH3(fDimension);
    else
        switch (fDimension)
        {
        case 1:
            h=new MH3(fData[0]->GetRule());
            break;
        case 2:
        case -1:
            h=new MH3(fData[0]->GetRule(), fData[1]->GetRule(), GetType());
            break;
        case 3:
        case -2:
            h=new MH3(fData[0]->GetRule(), fData[1]->GetRule(), fData[2]->GetRule(), GetType());
            break;
        case -3:
            h=new MH3(fData[0]->GetRule(), fData[1]->GetRule(), fData[2]->GetRule(), fData[3]->GetRule());
            break;
        }

    switch (fDimension)
    {
    case -3:
        h->SetScaleZ(fScale[3]);
    case -2:
    case 3:
        h->SetScaleZ(fScale[2]);
    case 2:
    case -1:
        h->SetScaleY(fScale[1]);
    case 1:
        h->SetScaleX(fScale[0]);
    }

    if (fWeight)
        h->SetWeight(fWeight->GetRule());

    return h;
}

// --------------------------------------------------------------------------
//
// FIXME
//
TString MH3::GetRule(const Char_t axis) const
{
    switch (tolower(axis))
    {
    case 'x':
    case 'X':
        return fData[0] ? fData[0]->GetRule() : TString("");
    case 'y':
    case 'Y':
        return fData[1] ? fData[1]->GetRule() : TString("");
    case 'z':
    case 'Z':
        return fData[2] ? fData[2]->GetRule() : TString("");
    case 't':
    case 'T':
        return fData[3] ? fData[3]->GetRule() : TString("");
    case 'w':
    case 'W':
        return fWeight  ? fWeight->GetRule() : TString("");
    default:
        return "<n/a>";
    }
}

// --------------------------------------------------------------------------
//
// Returns the total number of bins in a histogram (excluding under- and
// overflow bins)
//
Int_t MH3::GetNbins() const
{
    Int_t num = 1;

    switch (TMath::Abs(fDimension))
    {
    case 3:
        num *= fHist->GetNbinsZ()+2;
    case 2:
        num *= fHist->GetNbinsY()+2;
    case 1:
        num *= fHist->GetNbinsX()+2;
    }

    return num;
}

// --------------------------------------------------------------------------
//
// Returns the total number of bins in a histogram (excluding under- and
// overflow bins) Return -1 if bin is underflow or overflow
//
Int_t MH3::FindFixBin(Double_t x, Double_t y, Double_t z) const
{
    const TAxis &axex = *fHist->GetXaxis();
    const TAxis &axey = *fHist->GetYaxis();
    const TAxis &axez = *fHist->GetZaxis();

    Int_t binz = 0;
    Int_t biny = 0;
    Int_t binx = 0;

    switch (fDimension)
    {
    case 3:
    case -2:
        binz = axez.FindFixBin(z);
        if (binz>axez.GetLast() || binz<axez.GetFirst())
            return -1;
    case 2:
    case -1:
        biny = axey.FindFixBin(y);
        if (biny>axey.GetLast() || biny<axey.GetFirst())
            return -1;
    case 1:
        binx = axex.FindFixBin(x);
        if (binx<axex.GetFirst() || binx>axex.GetLast())
            return -1;
    }

    const Int_t nx = fHist->GetNbinsX()+2;
    const Int_t ny = fHist->GetNbinsY()+2;

    return binx + nx*(biny +ny*binz);
}

// --------------------------------------------------------------------------
//
// Return the MObjLookup corresponding to the axis/character.
// Note that only lower-case charecters (x, y, z) are supported.
// If for the axis no labels were set, the corresponding
// InitLabels is called.
//
MObjLookup *MH3::GetLabels(char axe)
{
    if (!fHist)
        return 0;

    TAxis *x = 0;

    switch (axe)
    {
    case 'x':
        x = fHist->GetXaxis();
        break;
    case 'y':
        x = fHist->GetYaxis();
        break;
    case 'z':
        x = fHist->GetZaxis();
        break;
    }

    if (!x)
        return 0;

    const Int_t idx = axe-'x';

    if (!x->GetLabels())
        switch (idx)
        {
        case 0:
            InitLabels(kLabelsX);
            break;
        case 1:
            InitLabels(kLabelsY);
            break;
        case 2:
            InitLabels(kLabelsZ);
            break;
        }

    return &fLabels[idx];
}

// --------------------------------------------------------------------------
//
// Set a default label which is used if no other is found in the list
// of labels. if a default was set already it is overwritten. If the
// axis has not yet been initialized to use labels it it now.
//
void MH3::DefaultLabel(char axe, const char *name)
{
    MObjLookup *arr = GetLabels(axe);
    if (!arr)
        return;

    if (arr->GetDefault())
    {
        delete arr->GetDefault();
        arr->SetDefault(0);
    }

    if (name)
        arr->SetDefault(new TObjString(name));
}

// --------------------------------------------------------------------------
//
// Define a name for a label. More than one label can have the same
// name. If the axis has not yet been initialized to use labels
// it it now.
//
void MH3::DefineLabel(char axe, Int_t label, const char *name)
{
    MObjLookup *arr = GetLabels(axe);

    if (!arr || !name)
        return;

    if (arr->GetObj(label)!=arr->GetDefault())
        return;

    arr->Add(label, name);
}

// --------------------------------------------------------------------------
//
// Define names for labels, like
//    1=Trig;2=Cal;4=Ped;8=Lvl2
// More than one label can have the same name. If the axis has not
// yet been initialized to use labels it it now.
//
// A default cannot be set here. Use DefaultLabel instead.
//
void MH3::DefineLabels(char axe, const TString &labels)
{
    TObjArray *arr = labels.Tokenize(';');

    for (int i=0; i<arr->GetEntries(); i++)
    {
        const char *s = (*arr)[0]->GetName();
        const char *v = strchr(s, '=');

        if (v)
            DefineLabel(axe, atoi(s), v+1);
    }

    delete arr;
}

void MH3::RecursiveRemove(TObject *obj)
{
    if (obj==fHist)
        fHist = 0;

    MH::RecursiveRemove(obj);
}
