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
!   Author(s): Thomas Bretz,   01/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Wolfgang Wittek 11/2003 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MFEventSelector2
//
// This is a filter to make a selection of events from a file, according to
// a certain requested distribution in a given parameter (or combination
// of parameters). The distribution is passed to the class through a histogram
// of the relevant parameter(s) contained in an object of type MH3. The filter
// will return true or false in each event such that the final distribution
// of the parameter(s) for the events surviving the filter is the desired one. 
// The selection of which events are kept in each bin of the parameter(s) is 
// made at random (obviously the selection probability depends on the 
// values of the parameters, and is dictated by the input histogram).
//
// This procedure requires the determination of the original distribution
// of the given parameters for the total sample of events on the input file.
// If the event loop contains a filter with name "FilterSelector2", this
// filter will be applied when determining the original distribution.
//
// See Constructor for more instructions and also the example below:
//
// --------------------------------------------------------------------
//
// void select()
// {
//     MParList plist;
//     MTaskList tlist;
// 
//     MStatusDisplay *d=new MStatusDisplay;
// 
//     plist.AddToList(&tlist);
// 
//     MReadTree read("Events", "myinputfile.root");
//     read.DisableAutoScheme();
//     // Accelerate execution...
//     // read.EnableBranch("MMcEvt.fTelescopeTheta");
// 
//     // create nominal distribution (theta converted into degrees)
//     MH3 nomdist("r2d(MMcEvt.fTelescopeTheta)");
//     MBinning binsx;
//     binsx.SetEdges(5, 0, 45);   // five bins from 0deg to 45deg
//     MH::SetBinning(&nomdist.GetHist(), &binsx);
//
//     // use this to create a nominal distribution in 2D
//     //  MH3 nomdist("r2d(MMcEvt.fTelescopeTheta)", "MMcEvt.fEnergy");
//     //  MBinning binsy;
//     //  binsy.SetEdgesLog(5, 10, 10000);
//     //  MH::SetBinning((TH2*)&nomdist.GetHist(), &binsx, &binsy);
//
//     // Fill the nominal distribution with whatever you want:
//     for (int i=0; i<nomdist.GetNbins(); i++)
//         nomdist.GetHist().SetBinContent(i, i*i);
// 
//     MFEventSelector2 test(nomdist);
//     test.SetNumMax(9999);  // total number of events selected
//     MContinue cont(&test);
// 
//     MEvtLoop run;
//     run.SetDisplay(d);
//     run.SetParList(&plist);
//     tlist.AddToList(&read);
//     tlist.AddToList(&cont);
// 
//     if (!run.Eventloop())
//         return;
// 
//     tlist.PrintStatistics();
// }
// 
// --------------------------------------------------------------------
//
// The random number is generated using gRandom->Rndm(). You may
// control this procedure using the global object gRandom.
//
// Because of the random numbers this works best for huge samples...
//
// Don't try to use this filter for the reading task or as a selector
// in the reading task: This won't work!
//
// Remark: You can also use the filter together with MContinue
//
//
// FIXME: Merge MFEventSelector and MFEventSelector2
//
/////////////////////////////////////////////////////////////////////////////
#include "MFEventSelector2.h"

#include <TRandom.h>        // gRandom
#include <TCanvas.h>        // TCanvas

#include "MH3.h"            // MH3
#include "MRead.h"          // MRead
#include "MEvtLoop.h"       // MEvtLoop
#include "MTaskList.h"      // MTaskList
#include "MBinning.h"       // MBinning
#include "MContinue.h"      // 
#include "MFillH.h"         // MFillH
#include "MParList.h"       // MParList
#include "MStatusDisplay.h" // MStatusDisplay

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MFEventSelector2);

using namespace std;

const TString MFEventSelector2::gsDefName  = "MFEventSelector2";
const TString MFEventSelector2::gsDefTitle = "Filter to select events with a given distribution";

// --------------------------------------------------------------------------
//
// Constructor. Takes a reference to an MH3 which gives you
//  1) The nominal distribution. The distribution is renormalized, so
//     that the absolute values do not matter. To crop the distribution
//     to a nominal value of total events use SetNumMax
//  2) The parameters histogrammed in MH3 are those on which the
//     event selector will work, eg
//       MH3 hist("MMcEvt.fTelescopeTheta", "MMcEvt.fEnergy");
//     Would result in a redistribution of Theta and Energy.
//  3) Rules are also accepted in the argument of MH3, for instance: 
//       MH3 hist("MMcEvt.fTelescopeTheta");
//     would result in redistributing Theta, while
//       MH3 hist("cos(MMcEvt.fTelescopeTheta)");
//     would result in redistributing cos(Theta).
//
//  If the reference distribution doesn't contain entries (GetEntries()==0)
//     the original distribution will be used as the nominal distribution;
//     note that also in this case a dummy nominal distribution has to be
//     provided in the first argument (the dummy distribution defines the
//     variable(s) of interest, their binnings and their requested ranges;
//     events outside these ranges won't be accepted).
//
//  Set default name of filter to be applied when determining the original
//  distribution for all data on the input file to "FilterSelector2"
//
MFEventSelector2::MFEventSelector2(MH3 &hist, const char *name, const char *title)
    : fHistOrig(NULL), fHistNom(&hist), fHistRes(NULL),
    fDataX(hist.GetRule('x')), fDataY(hist.GetRule('y')),
    fDataZ(hist.GetRule('z')), fNumMax(-1), fCanvas(0),
    fFilterName("FilterSelector2"), fHistIsProbability(kFALSE),
    fUseOrigDist(kTRUE)
{
    fName  = name  ? (TString)name  : gsDefName;
    fTitle = title ? (TString)title : gsDefTitle;
}

// --------------------------------------------------------------------------
//
// Delete fHistRes if instatiated
//
MFEventSelector2::~MFEventSelector2()
{
    if (fHistRes)
        delete fHistRes;
}

//---------------------------------------------------------------------------
//
// Recreate a MH3 from fHistNom used as a template. Copy the Binning
// from fHistNom to the new histogram, and return a pointer to the TH1
// base class of the MH3.
//
TH1 &MFEventSelector2::InitHistogram(MH3* &hist)
{
    // if fHistRes is already allocated delete it first
    if (hist)
        delete hist;

    // duplicate the fHistNom histogram
    hist = static_cast<MH3*>(fHistNom->New());

    // copy binning from one histogram to the other one
    MH::SetBinning(hist->GetHist(), fHistNom->GetHist());

    return hist->GetHist();
}

// --------------------------------------------------------------------------
//
// Try to read the present distribution from the file. Therefore the
// Reading task of the present loop is used in a new eventloop.
//
Bool_t MFEventSelector2::ReadDistribution(MRead &read, MFilter *filter)
{
    if (read.GetEntries() > kMaxUInt) // FIXME: LONG_MAX ???
    {
        *fLog << err << "kIntMax exceeded." << endl;
        return kFALSE;
    }

    *fLog << inf;
    fLog->Separator("MFEventSelector2::ReadDistribution");
    *fLog << " - Start of eventloop to generate the original distribution..." << endl;

    if (filter != NULL)
        *fLog << " - filter used: " << filter->GetDescriptor() << endl;


    MEvtLoop run("ReadDistribution");

    MParList plist;
    MTaskList tlist;
    plist.AddToList(&tlist);
    run.SetParList(&plist);

    MBinning binsx("BinningMH3X");
    MBinning binsy("BinningMH3Y");
    MBinning binsz("BinningMH3Z");
    binsx.SetEdges(fHistNom->GetHist(), 'x');
    binsy.SetEdges(fHistNom->GetHist(), 'y');
    binsz.SetEdges(fHistNom->GetHist(), 'z');
    plist.AddToList(&binsx);
    plist.AddToList(&binsy);
    plist.AddToList(&binsz);

    MFillH fill(fHistOrig);
    fill.SetName("FillHistOrig");
    fill.SetBit(MFillH::kDoNotDisplay);
    tlist.AddToList(&read);

    MContinue contfilter(filter);
    if (filter != NULL)
    {
      contfilter.SetName("ContFilter");
      tlist.AddToList(&contfilter);
    }

    tlist.AddToList(&fill);
    run.SetDisplay(fDisplay);
    if (!run.Eventloop())
    {
        *fLog << err << dbginf << "Evtloop failed... abort." << endl;
        return kFALSE;
    }

    tlist.PrintStatistics();

    *fLog << inf;
    *fLog << "MFEventSelector2::ReadDistribution:" << endl;
    *fLog << " - Original distribution has " << fHistOrig->GetHist().GetEntries() << " entries." << endl;
    *fLog << " - End of eventloop to generate the original distribution." << endl;
    fLog->Separator();

    return read.Rewind();
}

// --------------------------------------------------------------------------
//
// After reading the histograms the arrays used for the random event
// selection are created. If a MStatusDisplay is set the histograms are
// displayed there.
//
void MFEventSelector2::PrepareHistograms()
{
    TH1 &ho = fHistOrig->GetHist();

    //-------------------
    // if requested
    // set the nominal distribution equal to the original distribution

    const Bool_t useorigdist = fHistNom->GetHist().GetEntries()==0;
    TH1 *hnp =  useorigdist ? (TH1*)(fHistOrig->GetHist()).Clone() :
                              &fHistNom->GetHist();

    TH1 &hn = *hnp;
    //--------------------

    // normalize to number of counts in primary distribution
    hn.Scale(1./hn.Integral());

    MH3 *h3 = NULL;
    TH1 &hist = InitHistogram(h3);

    hist.Divide(&hn, &ho);
    hist.Scale(1./hist.GetMaximum());

    if (fCanvas)
    {
        fCanvas->Clear();
        fCanvas->Divide(2,2);

        fCanvas->cd(1);
        gPad->SetBorderMode(0);
        hn.DrawCopy();

        fCanvas->cd(2);
        gPad->SetBorderMode(0);
        ho.DrawCopy();
    }
    hn.Multiply(&ho, &hist);
    hn.SetTitle("Resulting Nominal Distribution");

    if (fNumMax>0)
    {
        *fLog << inf;
        *fLog << "MFEventSelector2::PrepareHistograms:" << endl;
        *fLog << " - requested number of events = " << fNumMax << endl;
        *fLog << " - maximum number of events possible = " << hn.Integral() << endl;

        if (fNumMax > hn.Integral())
	{
            *fLog << warn << "WARNING - Requested no.of events (" << fNumMax;
            *fLog << ") is too high... reduced to " << hn.Integral() << endl;
	}
        else
            hn.Scale(fNumMax/hn.Integral());
    }

    hn.SetEntries(hn.Integral()+0.5);
    if (fCanvas)
    {
        fCanvas->cd(3);
        gPad->SetBorderMode(0);
        hn.DrawCopy();

        fCanvas->cd(4);
        gPad->SetBorderMode(0);
        fHistRes->Draw();
    }
    delete h3;

    const Int_t num = fHistRes->GetNbins();
    fIs.Set(num);
    fNom.Set(num);
    for (int i=0; i<num; i++)
    {
        fIs[i]  = (Long_t)(ho.GetBinContent(i+1)+0.5);
        fNom[i] = (Long_t)(hn.GetBinContent(i+1)+0.5);
    }

    if (useorigdist)
      delete hnp;
}

// --------------------------------------------------------------------------
//
// PreProcess the data rules extracted from the MH3 nominal distribution
//
Bool_t MFEventSelector2::PreProcessData(MParList *parlist)
{
    switch (fHistNom->GetDimension())
    {
    case 3:
        if (!fDataZ.PreProcess(parlist))
        {
            *fLog << err << "Preprocessing of rule for z-axis failed... abort." << endl;
            return kFALSE;
        }
        // FALLTHROUGH!
    case 2:
        if (!fDataY.PreProcess(parlist))
        {
            *fLog << err << "Preprocessing of rule for y-axis failed... abort." << endl;
            return kFALSE;
        }
        // FALLTHROUGH!
    case 1:
        if (!fDataX.PreProcess(parlist))
        {
            *fLog << err << "Preprocessing of rule for x-axis failed... abort." << endl;
            return kFALSE;
        }
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// PreProcess the filter. Means:
//  1) Preprocess the rules
//  2) Read The present distribution from the file.
//  3) Initialize the histogram for the resulting distribution
//  4) Prepare the random selection
//  5) Repreprocess the reading and filter task.
//
Int_t MFEventSelector2::PreProcess(MParList *parlist)
{
    memset(fCounter, 0, sizeof(fCounter));

    MTaskList *tasklist = (MTaskList*)parlist->FindObject("MTaskList");
    if (!tasklist)
    {
        *fLog << err << "MTaskList not found... abort." << endl;
        return kFALSE;
    }

    if (!PreProcessData(parlist))
        return kFALSE;

    fHistNom->SetTitle(fHistIsProbability ? "ProbabilityDistribution" : 
                                            "Users Nominal Distribution");

    if (fHistIsProbability)
        return kTRUE;

    InitHistogram(fHistOrig);
    InitHistogram(fHistRes);

    fHistOrig->SetTitle("Primary Distribution");
    fHistRes->SetTitle("Resulting Distribution");

    // Initialize online display if requested
    fCanvas = fDisplay ? &fDisplay->AddTab(GetName()) : NULL;
    if (fCanvas)
        fHistOrig->Draw();

    // Generate primary distribution
    MRead *read = (MRead*)tasklist->FindObject("MRead");
    if (!read)
    {
        *fLog << err << "MRead not found in tasklist... abort." << endl;
        return kFALSE;
    }

    MFilter *filter = (MFilter*)tasklist->FindObject(fFilterName);
    if (!filter || !filter->InheritsFrom(MFilter::Class()))
    {
        *fLog << inf << "No filter will be used when making the original distribution" << endl;
        filter = NULL;
    }

    if (!ReadDistribution(*read, filter))
        return kFALSE;

    // Prepare histograms and arrays for selection
    PrepareHistograms();

    *fLog << all << "PreProcess..." << flush;

    if (filter != NULL)
    {
      const Int_t rcf = filter->CallPreProcess(parlist);
      if (rcf!=kTRUE)
        return rcf;
    }

    const Int_t rcr = read->CallPreProcess(parlist);
    return rcr;
}

// --------------------------------------------------------------------------
//
// Part of Process(). Select() at the end checks whether a selection should
// be done or not. Under-/Overflowbins are rejected.
//
Bool_t MFEventSelector2::Select(Int_t bin)
{
    // under- and overflow bins are not accepted
    if (bin<0)
        return kFALSE;

    Bool_t rc = kFALSE;

    if (gRandom->Rndm()*fIs[bin]<=fNom[bin])
    {
        // how many events do we still want to read in this bin
        fNom[bin]--;
        rc = kTRUE;

        // fill bin (same as Fill(valx, valy, valz))
        TH1 &h = fHistRes->GetHist();
        h.AddBinContent(bin+1);
        h.SetEntries(h.GetEntries()+1);
    }

    // how many events are still pending to be read
    fIs[bin]--;

    return rc;
}

// --------------------------------------------------------------------------
//
Bool_t MFEventSelector2::SelectProb(Int_t ibin) const
{
    //
    // If value is outside histogram range, accept event
    //
    return ibin<0 ? kTRUE : 
           fHistNom->GetHist().GetBinContent(ibin+1) > gRandom->Uniform();
}

// --------------------------------------------------------------------------
//
// fIs[i] contains the distribution of the events still to be read from
// the file. fNom[i] contains the number of events in each bin which
// are requested.
// The events are selected by:
//     gRandom->Rndm()*fIs[bin]<=fNom[bin]
//
Int_t MFEventSelector2::Process()
{
    // get x,y and z (0 if fData not valid)
    const Double_t valx=fDataX.GetValue();
    const Double_t valy=fDataY.GetValue();
    const Double_t valz=fDataZ.GetValue();

    // don't except the event if it is outside the axis range
    // of the requested distribution
    const Int_t ibin = fHistNom->FindFixBin(valx, valy, valz)-1;
    if (!fHistIsProbability)
    {
      if (ibin < 0)
      {
        fResult = kFALSE;
        fCounter[1]++;
        return kTRUE;
      }
    }

    // check whether a selection should be made
    fResult = fHistIsProbability ? SelectProb(ibin) : Select(ibin);
    if (!fResult)
    {
      fCounter[2]++;
      return kTRUE;
    }

    fCounter[0]++;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Update online display if set.
//
Int_t MFEventSelector2::PostProcess()
{
    //---------------------------------

    if (GetNumExecutions()>0)
    {
        *fLog << inf << endl;
        *fLog << GetDescriptor() << " execution statistics:" << endl;
        *fLog << dec << setfill(' ');

        *fLog << " " << setw(7) << fCounter[1] << " (" << setw(3)
              << (int)((Float_t)(fCounter[1]*100)/(Float_t)(GetNumExecutions())+0.5)
              << "%) Events not selected due to under/over flow" << endl;

        *fLog << " " << setw(7) << fCounter[2] << " (" << setw(3)
              << (int)((Float_t)(fCounter[2]*100)/(Float_t)(GetNumExecutions())+0.5)
              << "%) Events not selected due to requested distribution" 
              << endl;

        *fLog << " " << fCounter[0] << " ("
              << (int)((Float_t)(fCounter[0]*100)/(Float_t)(GetNumExecutions())+0.5)
              << "%) Events selected" << endl;
        *fLog << endl;
    }

    //---------------------------------

    if (fDisplay && fDisplay->HasCanvas(fCanvas))
    {
        fCanvas->cd(4);
        fHistRes->DrawClone("nonew");
        fCanvas->Modified();
        fCanvas->Update();
    }

    return kTRUE;
}






