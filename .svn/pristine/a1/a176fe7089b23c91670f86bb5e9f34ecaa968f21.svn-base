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
!   Author(s): Wolfgang Wittek 4/2002 <mailto:wittek@mppmu.mpg.de>
!              Abelardo Moralejo 5/2003 <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  MHMcEnergyMigration                                                     //
//                                                                          //
//  calculates the migration matrix E-est vs. E-true                        //
//  for different bins in Theta                                             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include "MHMcEnergyMigration.h"

#include <TCanvas.h>

#include "MMcEvt.hxx"

#include "MEnergyEst.h"
#include "MBinning.h"
#include "MHillasSrc.h"
#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"
#include <TProfile.h>

ClassImp(MHMcEnergyMigration);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor. It sets name and title of the histogram.
//
MHMcEnergyMigration::MHMcEnergyMigration(const char *name, const char *title)
  : fHist(), fHist2(), fHistImp()
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHMcEnergyMigration";
    fTitle = title ? title : "3-D histogram   E-true E-est Theta";

    fHist.SetDirectory(NULL);
    fHist.SetTitle("3D-plot  E_{EST}  E_{TRUE}  \\Theta");
    fHist.SetXTitle("E_{EST} [GeV]");
    fHist.SetYTitle("E_{TRUE} [GeV]");
    fHist.SetZTitle("\\Theta [\\circ]");

    fHist2.SetDirectory(NULL);
    fHist2.SetTitle("3D-plot \\Delta E / E  vs E_{EST} E_{TRUE}");
    fHist2.SetXTitle("E_{EST} [GeV]");
    fHist2.SetYTitle("E_{TRUE} [GeV]");
    fHist2.SetZTitle("\\frac{E_{EST} - E_{TRUE}}{E_{TRUE}}");

    fHistImp.SetDirectory(NULL);
    fHistImp.SetTitle("\\Delta E / E  vs Impact parameter");
    fHistImp.SetXTitle("Impact parameter (m)");
    fHistImp.SetYTitle("\\frac{E_{EST} - E_{TRUE}}{E_{TRUE}}");

}

// --------------------------------------------------------------------------
//
// Set the binnings and prepare the filling of the histograms
//
Bool_t MHMcEnergyMigration::SetupFill(const MParList *plist)
{
    fEnergy = (MEnergyEst*)plist->FindObject("MEnergyEst");
    if (!fEnergy)
    {
        *fLog << err << dbginf << "MEnergyEst not found... aborting." << endl;
        return kFALSE;
    }

    fMcEvt = (MMcEvt*)plist->FindObject("MMcEvt");
    if (!fMcEvt)
    {
        *fLog << err << dbginf << "MMcEvt not found... aborting." << endl;
        return kFALSE;
    }

    const MBinning* binsenergy = (MBinning*)plist->FindObject("BinningE");
    const MBinning* binstheta  = (MBinning*)plist->FindObject("BinningTheta");
    if (!binsenergy || !binstheta)
    {
        *fLog << err << dbginf << "At least one MBinning not found... aborting." << endl;
        return kFALSE;
    }

    SetBinning(&fHist, binsenergy, binsenergy, binstheta);
    fHist.Sumw2();

    const MBinning* binsde = (MBinning*)plist->FindObject("BinningDE");
    const MBinning* binsimpact = (MBinning*)plist->FindObject("BinningImpact");
    SetBinning(&fHistImp, binsimpact, binsde);

    fHistImp.Sumw2();

    SetBinning(&fHist2, binsenergy, binsenergy, binsde);
    fHist2.Sumw2();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Draw the histogram
//
void MHMcEnergyMigration::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    pad->Divide(2,2);
    
    TH1 *h;

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    h = fHist.Project3D("ey_pro");
    h->SetTitle("Distribution of E_{TRUE}");
    h->SetXTitle("E_{TRUE} [GeV]");
    h->SetBit(kCanDelete);
    h->Draw(opt);
    h->SetDirectory(NULL);

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    h = fHist.Project3D("ex_pro");
    h->SetTitle("Distribution of E_{EST}");
    h->SetXTitle("E_{est} [GeV]");
    h->SetBit(kCanDelete);
    h->Draw(opt);
    Double_t minEest = h->GetXaxis()->GetXmin();
    h->SetDirectory(NULL);

    pad->cd(3);
    gPad->SetBorderMode(0);
    h = fHist.Project3D("z_pro");
    h->SetTitle("Distribution of \\Theta");
    h->SetXTitle("\\Theta [\\circ]");
    h->SetBit(kCanDelete);
    h->SetLineWidth(2);
    h->Draw(opt);
    h->SetDirectory(NULL);

    pad->cd(4);
    gPad->SetBorderMode(0);
    TH1D* hpx;
    hpx = fHistImp.ProjectionX("_px", 1, fHistImp.GetNbinsY(),"e");
    hpx->SetTitle("Distribution of Impact parameter");
    hpx->SetXTitle("Impact parameter (m)");
    hpx->SetBit(kCanDelete);
    hpx->Draw(opt);
    hpx->SetDirectory(NULL);
    fHistImp.SetDirectory(NULL);

    pad->Modified();
    pad->Update();

    TVirtualPad *pad2 = MakeDefCanvas((TString)GetName()+"2");
    pad2->SetBorderMode(0);

    AppendPad("");

    pad2->Divide(2,2);
    
    TH2D *h2;

    pad2->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetLogy();
    h2 = (TH2D*) fHist.Project3D("yx");

    TProfile* h2pfx;
    h2pfx = h2->ProfileX("_pfx", 1, h2->GetNbinsY(),"S");
    h2pfx->SetXTitle("E_{TRUE} (GeV)");
    h2pfx->SetYTitle("E_{EST} (GeV)");
    h2pfx->SetTitle("E_{EST} vs E_{TRUE}");
    h2pfx->SetBit(kCanDelete);
    h2pfx->SetFillColor(41);
    h2pfx->SetFillStyle(1001);
    h2pfx->SetMinimum(minEest);
    h2pfx->SetStats(kFALSE);
    h2pfx->DrawCopy("E3");

    h2->SetBit(kCanDelete);
    h2->SetFillColor(1);
    h2->Draw("box,same");
    h2->SetDirectory(NULL);

    h2pfx->SetLineColor(2);
    h2pfx->SetLineWidth(2);
    h2pfx->SetFillColor(0);
    h2pfx->DrawCopy("C,hist,same");
    h2pfx->SetDirectory(NULL);

    pad2->cd(2);

    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetLeftMargin(0.15);
    h2 = (TH2D*) fHist2.Project3D("zy");
    h2->SetBit(kCanDelete);
    h2pfx = h2->ProfileX("_pfx", 1, h2->GetNbinsY(),"S");
    h2pfx->SetTitle("\\Delta E / E vs E_{TRUE}");
    h2pfx->SetXTitle("E_{TRUE} (GeV)");
    h2pfx->SetYTitle("\\frac{E_{EST} - E_{TRUE}}{E_{TRUE}}");
    h2pfx->SetBit(kCanDelete);
    h2pfx->SetFillColor(41);
    h2pfx->SetFillStyle(1001);
    h2pfx->GetYaxis()->SetTitleOffset(1.4);
    h2pfx->SetStats(kFALSE);
    h2pfx->DrawCopy("E3");
    h2->SetFillColor(1);
    h2->Draw("same,box");
    h2->SetDirectory(NULL);
    h2pfx->SetLineColor(2);
    h2pfx->SetLineWidth(2);
    h2pfx->SetFillColor(0);
    h2pfx->DrawCopy("C,hist,same");
    h2pfx->SetDirectory(NULL);
    
    pad2->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetLeftMargin(0.15);
    h2 = (TH2D*) fHist2.Project3D("zx");
    h2->SetBit(kCanDelete);
    h2pfx = h2->ProfileX("_pfx", 1, h2->GetNbinsY(),"S");
    h2pfx->SetTitle("\\Delta E / E vs E_{EST}");
    h2pfx->SetXTitle("E_{EST} (GeV)");
    h2pfx->SetYTitle("\\frac{E_{EST} - E_{TRUE}}{E_{TRUE}}");
    h2pfx->SetBit(kCanDelete);
    h2pfx->SetFillColor(41);
    h2pfx->SetFillStyle(1001);
    h2pfx->GetYaxis()->SetTitleOffset(1.4);
    h2pfx->SetStats(kFALSE);
    h2pfx->SetMinimum(-1.);
    h2pfx->SetMaximum(1.);
    h2pfx->DrawCopy("E3");
    
    h2->SetFillColor(1);
    h2->Draw("same,box");
    h2->SetDirectory(NULL);
    h2pfx->SetLineColor(2);
    h2pfx->SetLineWidth(2);
    h2pfx->SetFillColor(0);
    h2pfx->DrawCopy("L,histo,same");
    h2pfx->SetDirectory(NULL);
    
    pad2->cd(4);
    gPad->SetBorderMode(0);
    h = fHist2.ProjectionZ("_pz",1,fHist2.GetNbinsX(),1,fHist2.GetNbinsY(),"e");
    h->SetBit(kCanDelete);
    h->Draw();
    h->SetDirectory(NULL);

    pad2->Modified();
    pad2->Update();

    fHist.SetDirectory(NULL);
    fHist2.SetDirectory(NULL);

}

// --------------------------------------------------------------------------
//
// Fill the histogram
//
Bool_t MHMcEnergyMigration::Fill(const MParContainer *par, const Stat_t w)
{
    // get E-true from fMcEvt and E-est from fEnergy

    fHist.Fill(fEnergy->GetEnergy(), fMcEvt->GetEnergy(), fMcEvt->GetTelescopeTheta()*kRad2Deg, w);

    fHist2.Fill(fEnergy->GetEnergy(), fMcEvt->GetEnergy(), (fEnergy->GetEnergy()-fMcEvt->GetEnergy())/fMcEvt->GetEnergy(), w);

    fHistImp.Fill(fMcEvt->GetImpact()/100., (fEnergy->GetEnergy()-fMcEvt->GetEnergy())/fMcEvt->GetEnergy(), w);

    return kTRUE;
}
