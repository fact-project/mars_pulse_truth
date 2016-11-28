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
// MHn
//
//
// Initialization
// --------------
//
// MHn is a histogram class which derives from MH as all Mars histogram
// classes do, i.e. to fill the histogram use MFillH. (Example below)
//
// After instantisation of MHn add the histograms of your interest using
// AddHist. Before the next AddHist is called you can now set the options
// of your histogram using InitName, InitTitle, SetLog, SetAutoRange,
// SetScale and SetDrawOption.
//
//
// Layout
// ------
//
// The layout of the histograms on the screen depends on the number of
// initialized histograms and the option SetLayout. For details see below.
//
//  SetLayout(MHn::kSimple)
//       < default >          SetLayout(MHn::kComplex)
// ========================= ==========================
//    +-----------------+
//    |1                |
//    |                 |
//    |                 |
//    |                 |
//    |                 |
//    +-----------------+
//
//    +-----------------+        +--------+--------+
//    |1                |        |1       |2       |
//    |                 |        |        |        |
//    +-----------------+        |        |        |
//    |2                |        |        |        |
//    |                 |        |        |        |
//    +-----------------+        +--------+--------+
//
//    +--------+--------+        +-----+-----+-----+
//    |1       |2       |        |1    |3          |
//    |        |        |        |     |           |
//    +--------+--------+        +-----+           +
//    |3       |        |        |2    |           |
//    |        |        |        |     |           |
//    +--------+--------+        +-----+-----+-----+
//
//    +--------+--------+        +------+----------+
//    |1       |2       |        |1     |4         |
//    |        |        |        +------+          |
//    +--------+--------+        |2     |          |
//    |3       |4       |        +------+          |
//    |        |        |        |3     |          |
//    +--------+--------+        +------+----------+
//
//    +-----+-----+-----+        +--------+--------+
//    |1    |2    |3    |        |1       |2       |
//    |     |     |     |        +--------+--------|
//    +-----+-----+-----+        |3       |4       |
//    |4    |5    |     |        +--------+        |
//    |     |     |     |        |5       |        |
//    +-----+-----+-----+        +--------+--------+
//
//    +-----+-----+-----+        +--------+--------+
//    |1    |2    |3    |        |1       |2       |
//    |     |     |     |        +--------+--------+
//    +-----+-----+-----+        |3       |4       |
//    |4    |5    |6    |        +--------+--------+
//    |     |     |     |        |5       |6       |
//    +-----+-----+-----+        +--------+--------+
//
//
// For example:
// ------------
//    // Instatiate the MHn class with default name and title
//    MHn hres("Energy2", "Energy Residual (lg E_{est} - lg E_{mc})");
//
//    // Initialize your first histogram (here a 2D histogram with x- and y-axis)
//    hres.AddHist("MMcEvt.fEnergy", "log10(MEnergyEst.fVal)-log10(MMcEvt.fEnergy)");
//    // Initialize the name of the histogram (ResEmc) and the title of the
//    // binnings ((here BinningEnergy for x-axis and BinningEnergyResidual for y-axis)
//    hres.InitName("ResEmc;Energy;EnergyResidual");
//    // Initialize the title of the histogram and the axis titles
//    hres.InitTitle(";E_{mc} [GeV];\\Delta lg E;");
//    // Initialize the draw option for the histogram
//    hres.SetDrawOption("colz profx");
//    // for more details on the options and more options see below
//    // or the class reference of MH3
//
//    // Initialize a second histogram
//    hres.AddHist("MPointingPos.fZd", "log10(MEnergyEst.fVal)-log10(MMcEvt.fEnergy)");
//    hres.InitName("ResTheta;Theta;EnergyResidual");
//    hres.InitTitle(";Zd [\\circ];\\Delta lg E;");
//    hres.SetDrawOption("colz profx");
//
//    // Note that only AddHist is mandatory. All other options can be omitted
//
//    // Initialize the filling task which can now be added to the tasklist
//    MFillH fill(&hres);
//    ...
//
/////////////////////////////////////////////////////////////////////////////
#include "MHn.h"

#include <TCanvas.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MH3.h"
#include "MString.h"

ClassImp(MHn);

using namespace std;

MHn::MHn(const char *name, const char *title) : fLayout(kSimple), fNum(0)
{
    fName  = name  ? name  : "MHn";
    fTitle = title ? title : "Generalized histogram class for up to six histograms";

    memset(fHist, 0, 6*sizeof(MH3*));
}

MHn::~MHn()
{
    for (int i=0; i<fNum; i++)
        if (fHist[i])
            delete fHist[i];
}

// --------------------------------------------------------------------------
//
// If a histogram with the same name is found, the pad-number with a
// training underscore is added.
//
// FIXME: How do we handle identical names with different axes?
//
void MHn::InitHist()
{
    TString name = fName;

    for (int i=0; i<fNum; i++)
        if (name==fHist[i]->GetName())
        {
            name += MString::Format("_%d", fNum);
            break;
        }

    fHist[fNum]->SetName(fName);
    fHist[fNum]->SetTitle(fTitle);

    fHist[fNum]->SetAutoRangeX();

    fNum++;
}

// --------------------------------------------------------------------------
//
// Add a new 1D-MH3 histogram. An internal pointer is set to it, so that
// InitName and InitTitle can be used for this histogram until a new
// histogram is added using AddHist
//
//  e.g. AddHist("MHillas.fSize")
//
Bool_t MHn::AddHist(const char *memberx)
{
    if (fNum==8)
    {
        *fLog << err << "ERROR - MHn doesn't support more than six histograms... AddHist ignored." << endl;
        return kFALSE;
    }

    fHist[fNum] = new MH3(memberx);

    InitHist();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Add a new 2D-MH3 histogram. An internal pointer is set to it, so that
// InitName and InitTitle can be used for this histogram until a new
// histogram is added using AddHist
//
//  e.g. AddHist("MHillas.fLength", "MHillas.fSize")
//
Bool_t MHn::AddHist(const char *memberx, const char *membery, MH3::Type_t type)
{
    if (fNum==8)
    {
        *fLog << err << "ERROR - MHn doesn't support more than six histograms... AddHist ignored." << endl;
        return kFALSE;
    }

    fHist[fNum] = new MH3(memberx, membery, type);

    InitHist();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Add a new 3D-MH3 histogram. An internal pointer is set to it, so that
// InitName and InitTitle can be used for this histogram until a new
// histogram is added using AddHist
//
//  e.g. AddHist("MHillas.fWidth", "MHillas.fLength", "MHillas.fSize")
//
Bool_t MHn::AddHist(const char *memberx, const char *membery, const char *memberz, MH3::Type_t type)
{
    if (fNum==8)
    {
        *fLog << err << "ERROR - MHn doesn't support more than six histograms... AddHist ignored." << endl;
        return kFALSE;
    }

    fHist[fNum] = new MH3(memberx, membery, memberz, type);

    InitHist();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Add a new 3D-MH3 histogram. An internal pointer is set to it, so that
// InitName and InitTitle can be used for this histogram until a new
// histogram is added using AddHist
//
//  e.g. AddHist("MHillas.fWidth", "MHillas.fLength", "MHillas.fSize", "MWeight.fVal")
//
Bool_t MHn::AddHist(const char *memberx, const char *membery, const char *memberz, const char *weight)
{
    if (fNum==8)
    {
        *fLog << err << "ERROR - MHn doesn't support more than six histograms... AddHist ignored." << endl;
        return kFALSE;
    }

    fHist[fNum] = new MH3(memberx, membery, memberz, weight);

    InitHist();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set the draw option of the n-th MH3. See MH3 for more details of the
// meaning of it.
//
Bool_t MHn::SetDrawOption(Int_t n, const char *opt)
{
    if (n<0 || n>=fNum)
    {
        *fLog << err << "ERROR - Histogram " << n << " not yet initialized... SetDrawOption ignored." << endl;
        return kFALSE;
    }

    fDrawOption[n] = opt;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set the name of the n-th MH3. See MH3 for more details of the meaning
// of the name. If the given name starts with a semicolon fName is prepend
// to the string.
//
Bool_t MHn::InitName(Int_t n, const char *nam)
{
    if (n<0 || n>=fNum)
    {
        *fLog << err << "ERROR - Histogram " << n << " not yet initialized... InitName ignored." << endl;
        return kFALSE;
    }

    TString name = TString(nam).Strip(TString::kBoth);

    if (name[0]==';')
        name.Prepend(fName);

    fHist[n]->SetName(name);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set the title of the n-th MH3. See MH3 for more details of the meaning
// of the title. If the given title starts with a semicolon fTitle is prepend
// to the string.
//
Bool_t MHn::InitTitle(Int_t n, const char *tit)
{
    if (n<0 || n>=fNum)
    {
        *fLog << err << "ERROR - Histogram " << n << " not yet initialized... InitTitle ignored." << endl;
        return kFALSE;
    }

    TString title = TString(tit).Strip(TString::kBoth);

    if (title[0]==';')
        title.Prepend(fTitle);

    fHist[n]->SetTitle(title);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set additional scale factors for the current histogram
//
void MHn::SetScale(Double_t x, Double_t y, Double_t z) const
{
    if (fHist[fNum-1])
        fHist[fNum-1]->SetScale(x, y, z);
}

// --------------------------------------------------------------------------
//
// Enable or disable displaying log-scale for the current histogram.
// Normally it is retrieved from the corresponding MBinning
//
void MHn::SetLog(Bool_t x, Bool_t y, Bool_t z) const
{
    if (fHist[fNum-1])
        fHist[fNum-1]->SetLog(x, y, z);
}

// --------------------------------------------------------------------------
//
// Set the axis range in Finalize automatically to the histogram region
// containing contents. This is the default for the x-axis.
// This function can be used to enable this behaviour also for the other
// axis, or disable it for the x-axis of the current histogram.
//
void MHn::SetAutoRange(Bool_t x, Bool_t y, Bool_t z) const
{
    if (fHist[fNum-1])
        fHist[fNum-1]->SetAutoRange(x, y, z);
}

// --------------------------------------------------------------------------
//
// Call Sumw2 for trhe current histogram, enabling errors.
//
void MHn::Sumw2() const
{
    if (fHist[fNum-1])
        fHist[fNum-1]->Sumw2();
}

// --------------------------------------------------------------------------
//
// Force the given binning(s) for the current histogram. The binnings
// do not get owned. The user is responsible for deleting them.
//
void MHn::SetBinnings(MBinning *x, MBinning *y, MBinning *z) const
{
    if (fHist[fNum-1])
        fHist[fNum-1]->SetBinnings(x, y, z);
}

// --------------------------------------------------------------------------
//
// call MH3::InitLabels for the current histogram
//
void MHn::InitLabels(MH3::Labels_t labels) const
{
    if (fHist[fNum-1])
        fHist[fNum-1]->InitLabels(labels);
}

// --------------------------------------------------------------------------
//
// call MH3::DefaultLabelX for the current histogram
//
void MHn::DefaultLabelX(const char *name)
{
    if (fHist[fNum-1])
        fHist[fNum-1]->DefaultLabelX(name);
}

// --------------------------------------------------------------------------
//
// call MH3::DefaultLabelY for the current histogram
//
void MHn::DefaultLabelY(const char *name)
{
    if (fHist[fNum-1])
        fHist[fNum-1]->DefaultLabelY(name);
}

// --------------------------------------------------------------------------
//
// call MH3::DefaultLabelZ for the current histogram
//
void MHn::DefaultLabelZ(const char *name)
{
    if (fHist[fNum-1])
        fHist[fNum-1]->DefaultLabelZ(name);
}

// --------------------------------------------------------------------------
//
// call MH3::DefineLabelX for the current histogram
//
void MHn::DefineLabelX(Int_t label, const char *name)
{
    if (fHist[fNum-1])
        fHist[fNum-1]->DefineLabelX(label, name);
}

// --------------------------------------------------------------------------
//
// call MH3::DefineLabelY for the current histogram
//
void MHn::DefineLabelY(Int_t label, const char *name)
{
    if (fHist[fNum-1])
        fHist[fNum-1]->DefineLabelY(label, name);
}

// --------------------------------------------------------------------------
//
// call MH3::DefineLabelZ for the current histogram
//
void MHn::DefineLabelZ(Int_t label, const char *name)
{
    if (fHist[fNum-1])
        fHist[fNum-1]->DefineLabelZ(label, name);
}

// --------------------------------------------------------------------------
//
// call MH3::DefineLabelsX for the current histogram
//
void MHn::DefineLabelsX(const TString &labels)
{
    if (fHist[fNum-1])
        fHist[fNum-1]->DefineLabelsX(labels);
}

// --------------------------------------------------------------------------
//
// call MH3::DefineLabelsY for the current histogram
//
void MHn::DefineLabelsY(const TString &labels)
{
    if (fHist[fNum-1])
        fHist[fNum-1]->DefineLabelsY(labels);
}

// --------------------------------------------------------------------------
//
// call MH3::DefineLabelsZ for the current histogram
//
void MHn::DefineLabelsZ(const TString &labels)
{
    if (fHist[fNum-1])
        fHist[fNum-1]->DefineLabelsZ(labels);
}

// --------------------------------------------------------------------------
//
// call MH3::SetWeight for the current histogram
//
void MHn::SetWeight(const char *phrase)
{
    if (fHist[fNum-1])
        fHist[fNum-1]->SetWeight(phrase);
}

// --------------------------------------------------------------------------
//
// call MH3::SetConversion for the current histogram
//
void MHn::SetConversion(const char *func)
{
    if (fHist[fNum-1])
        fHist[fNum-1]->SetConversion(func);
}

// --------------------------------------------------------------------------
//
// Call SetupFill for all initialized histograms
//
Bool_t MHn::SetupFill(const MParList *plist)
{
    for (int i=0; i<fNum; i++)
    {
        TestBit(kDoNotReset) ? fHist[i]->SetBit(kDoNotReset) : fHist[i]->ResetBit(kDoNotReset);

        if (!fHist[i]->SetupFill(plist))
            return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Call Fill for all initialized histograms
//
Int_t MHn::Fill(const MParContainer *par, const Stat_t w)
{
    for (int i=0; i<fNum; i++)
    {
        const Int_t rc = fHist[i]->Fill(par, w);
        if (rc!=kTRUE)
            return rc;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Call Finalize for all initialized histograms
//
Bool_t MHn::Finalize()
{
    for (int i=0; i<fNum; i++)
        if (!fHist[i]->Finalize())
            return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
void MHn::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    const Bool_t same = TString(opt).Contains("same");

    if (!same)
    {
        AppendPad();

        const Int_t id = fNum + fLayout*10;

        switch (id)
        {
        case 0:   // 0
        case 10:  // 0
            return;
        case 1:   // 1
        case 11:  // 1
            break;

        case 2:   // 2
            pad->Divide(1,2, 1e-5, 1e-5);
            break;
        case 3:   // 3
            pad->Divide(2,2, 1e-5, 1e-5);
            delete pad->GetPad(4);
            break;
         case 4:   // 4
            pad->Divide(2,2, 1e-5, 1e-5);
            break;
        case 5:   // 5
            pad->Divide(3,2, 1e-5, 1e-5);
            delete pad->GetPad(6);
            break;
        case 6:   // 6
            pad->Divide(3,2, 1e-5, 1e-5);
            break;

        case 12:  // 2
            pad->Divide(2,1, 1e-5, 1e-5);
            break;
        case 13:  // 3
            pad->Divide(2,2, 1e-5, 1e-5);
            pad->GetPad(1)->SetPad(0.01, 0.505, 0.33, 0.99);
            pad->GetPad(2)->SetPad(0.01, 0.01,  0.33, 0.495);
            pad->GetPad(3)->SetPad(0.34, 0.01,  0.99, 0.99);
            delete pad->GetPad(4);
            break;
        case 14:  // 4
            pad->Divide(2,2, 1e-5, 1e-5);
            break;
        case 15:  // 5
            pad->Divide(2,3, 1e-5, 1e-5);
            pad->GetPad(4)->SetPad(0.51, 0.01, 0.99, 0.65);
            delete pad->GetPad(6);
            break;
        case 16:  // 6
            pad->Divide(2,3);
            break;
        }
    }

    for (int i=0; i<fNum; i++)
    {
        TString dopt(fDrawOption[i]);
        if (same)
            dopt += " same";

        pad->cd(i+1);
        fHist[i]->Draw(dopt);
    }
}

void MHn::RecursiveRemove(TObject *obj)
{
    for (int i=0; i<6; i++)
    {
        if (!fHist[i])
            continue;

        if (obj==fHist[i])
            fHist[i] = 0;
        else
            fHist[i]->RecursiveRemove(obj);
    }

    MH::RecursiveRemove(obj);
}
