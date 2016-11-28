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
!   Author(s): Javier Lopez 05/2001 <mailto:jlopez@ifae.es>
!   Author(s): Thomas Bretz 05/2001 <mailto:tbretz@uni-sw.gwdg.de>
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MHMcEnergyImpact
//
// This class holds the information (histogram and fit function)
// about the energy threshold for a particular trigger condition.
//
////////////////////////////////////////////////////////////////////////////
#include "MHMcEnergyImpact.h" 

#include <TH2.h>
#include <TCanvas.h>

#include "MParList.h"
#include "MBinning.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MMcEvt.hxx"

ClassImp(MHMcEnergyImpact);

// -------------------------------------------------------------------------
//
//  Default Constructor.
//
MHMcEnergyImpact::MHMcEnergyImpact(const char *name, const char *title)
    : fHist()
{
    fName  = name  ? name  : "MHMcEnergyImpact";
    fTitle = title ? title : "Impact (radius) vs Energy distribution";

    //  - we initialize the histogram
    //  - we have to give diferent names for the diferent histograms because
    //    root don't allow us to have diferent histograms with the same name

    fHist.SetName(fName);
    fHist.SetTitle(fTitle);

    fHist.SetDirectory(NULL);

    fHist.SetXTitle("E [GeV]");
    fHist.SetYTitle("r [m]");
    fHist.SetZTitle("N");

    MBinning binsx;
    binsx.SetEdgesLog(10, 1, 100000); // [GeV]

    MBinning binsy;
    binsy.SetEdges(9, 0, 450);       // [m]

    SetBinning(&fHist, &binsx, &binsy);

}

void MHMcEnergyImpact::SetName(const char *name)
{
    fName = name;
    fHist.SetName(name);
    fHist.SetDirectory(NULL);
}

void MHMcEnergyImpact::SetTitle(const char *title)
{
    fTitle = title;
    fHist.SetName(title);
}

Bool_t MHMcEnergyImpact::SetupFill(const MParList *pList)
{
    const MBinning *binsx = (MBinning*)pList->FindObject("BinningEnergy", "MBinning");
    const MBinning *binsy = (MBinning*)pList->FindObject("BinningImpact", "MBinning");

    if (!binsx || !binsy)
        return kTRUE;

    SetBinning(&fHist, binsx, binsy);

    return kTRUE;
}

//--------------------------------------------------------------------------
//
//  Fill the histogram with the log10 of the energy for triggered events.
//
Bool_t MHMcEnergyImpact::Fill(const MParContainer *cont, const Stat_t w)
{
    const MMcEvt &mcevt = *(MMcEvt*)cont;

    const Float_t energy = mcevt.GetEnergy();
    const Float_t impact = mcevt.GetImpact()/100.;

    fHist.Fill(energy, impact, w);

    return kTRUE;
}

// ------------------------------------------------------------------------
// 
// Drawing function. It creates its own canvas.
//
void MHMcEnergyImpact::Draw(Option_t *option)
{
    TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    pad->SetLogx();

    fHist.Draw(option);

    pad->Modified();
    pad->Update();
}
