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
!   Author(s): Abelardo Moralejo <mailto:moralejo@pd.infn.it>
!   Author(s): Thomas Bretz, 5/2002 <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////
//
// MHCompProb
//
// This class contains different histograms of the Hillas parameters
//   and composite probabilities based on them.
//
///////////////////////////////////////////////////////////////////////

#include "MHCompProb.h"

#include <TH2.h>
#include <TPad.h>
#include <TText.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TProfile.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MBinning.h"
#include "MDataPhrase.h"

#include "MMcEvt.hxx"

ClassImp(MHCompProb);

using namespace std;

// --------------------------------------------------------------------------
//
// Setup histograms
//
MHCompProb::MHCompProb(Int_t nbins, const char *name, const char *title)
    : fNumLoop(0)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHCompProb";
    fTitle = title ? title : "Gamma/Hadron Separation Quality Histograms";

    fData    = new TList;
    fRules   = new TList;
    fHists   = new TList;
    fHistVar = new TList;

    fData->SetOwner();
    fRules->SetOwner();
    fHists->SetOwner();
    fHistVar->SetOwner();
}

// --------------------------------------------------------------------------
//
// Delete the histograms
//
MHCompProb::~MHCompProb()
{
    delete fData;
    delete fRules;
    delete fHists;
    delete fHistVar;
}

// --------------------------------------------------------------------------
//
//
//
void MHCompProb::Add(const char *rule, Int_t n, Float_t min, Float_t max)
{
    MDataPhrase &chain = *new MDataPhrase(rule);
    fData->Add(&chain);

    TNamed &name = *new TNamed(rule, "");
    fRules->Add(&name);

    TH1D &hist = *new TH1D(Form("Hist_%s", rule), rule, n, min, max);
    hist.SetXTitle(rule);
    hist.SetYTitle("Counts");
    hist.SetDirectory(NULL);
    fHists->Add(&hist);

    TH1D &varhist = *new TH1D;
    varhist.SetName(Form("Var_%s", rule));
    varhist.SetTitle(rule);
    varhist.SetXTitle(rule);
    varhist.SetYTitle("Counts");
    varhist.SetDirectory(NULL);
    fHistVar->Add(&varhist);
}

// --------------------------------------------------------------------------
//
//
//
Bool_t MHCompProb::SetupFill(const MParList *plist)
{
    if (fData->GetSize()==0)
    {
        *fLog << err << "No data members spcified for usage... aborting." << endl;
        return kFALSE;
    }

    TIter Next(fData);
    MData *data=NULL;
    while ((data=(MData*)Next()))
        if (!data->PreProcess(plist))
            return kFALSE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
//
void MHCompProb::Fill(TList &list)
{
    MData *data = NULL;

    TIter NextD(fData);
    TIter NextH(&list);

    while ((data=(MData*)NextD()))
    {
        TH1D *hist = (TH1D*)NextH();
        hist->Fill(data->GetValue());
    }
}

// --------------------------------------------------------------------------
//
//
//
Bool_t MHCompProb::Fill(const MParContainer *par, const Stat_t w)
{
    const MMcEvt &mcevt = *(MMcEvt*)par;

    switch (fNumLoop)
    {
    case 0:  // First loop : fill the fixed-bin histograms with gammas.
        if (mcevt.GetPartId() == MMcEvt::kGAMMA)
            Fill(*fHists);
        return kTRUE;

    case 1:   //  Second Loop: fill the variable-bin histograms with protons.
        if (mcevt.GetPartId() != MMcEvt::kGAMMA)
            Fill(*fHistVar);
        return kTRUE;
    default:
        *fLog << err << "Error - Invalid Loop Number... aborting." << endl;
        return kFALSE;
    }
}

// --------------------------------------------------------------------------
//
//
//
Bool_t MHCompProb::Finalize()
{
    switch (fNumLoop++)
    {
    case 0:
        *fLog << inf << "Finished filling fixed bin size histograms with gamma-data." << endl;
        SetBinningHistVar();
        fHists->Delete();
        return kTRUE;
    case 1:
        *fLog << inf << "Finished filling variable bin size histogram with proton data." << endl;
        return kTRUE;
    default:
        *fLog << err << "Error - Invalid Loop Number... aborting." << endl;
        return kFALSE;
    }
}

// --------------------------------------------------------------------------
//
//
//
void MHCompProb::SetBinningHistVar()
{
    Int_t nedges = 51; // Number of bins in variable-bin histograms.

    TIter NextH(fHists);
    TIter NextV(fHistVar);
    TH1D *hist = NULL;
    while ((hist=(TH1D*)NextH()))
    {
        Int_t n = hist->GetNbinsX();

        TArrayD edges(nedges);

        edges[0]        = hist->GetBinLowEdge(1);
        edges[nedges-1] = hist->GetBinLowEdge(n+1);

        Float_t newwidth = hist->Integral(1, n)/nedges;

        Int_t jbin = 1;
        for (Int_t j=1; j<n && jbin<nedges-1; j++)
        {
            if (hist->Integral(1, j) <= jbin*newwidth)
                continue;

            edges[jbin++] = hist->GetBinLowEdge(j+1);
        }

        MBinning bins;
        bins.SetEdges(edges);

        SetBinning((TH1D*)NextV(), &bins);
    }
}

