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
!   Author(s): Markus Gaug <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////
//
// MHSimulatedAnnealing
//
// This class contains different histograms of the Simulated Annealing 
//   Snapshort during optimization and the final results
//
///////////////////////////////////////////////////////////////////////
#include "MHSimulatedAnnealing.h"

#include <TObjArray.h>

#include <TStyle.h>
#include <TCanvas.h>

#include "MBinning.h"

ClassImp(MHSimulatedAnnealing);

// --------------------------------------------------------------------------
//
// Setup histograms
//
MHSimulatedAnnealing::MHSimulatedAnnealing(UShort_t moves, UShort_t ndim, 
	                                   const char *name, 
					   const char *title)
    : fDim(ndim), fMoves(moves), fTimeEvolution(NULL), fBestEver(), fBestFuncEval()
{

    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHSimulatedAnnealing";
    fTitle = title ? title : "Output Histograms of a Simulated Annealing Run";
    
    fBestEver.SetName("Hist_BestEver");
    fBestEver.SetTitle("Best Param. Combinations");
    fBestEver.SetXTitle("Run Duration");
    fBestEver.SetYTitle("Parameter Nr.");
    fBestEver.SetZTitle("Parameter Value");
    fBestEver.SetDirectory(NULL);
    
    fBestFuncEval.SetName("Hist_BestFuncEval");
    fBestFuncEval.SetTitle("Best Function Evaluation");
    fBestFuncEval.SetXTitle("Run Duration");
    fBestFuncEval.SetYTitle("Function Value");
    fBestFuncEval.SetDirectory(NULL);
    
    MBinning binsx, binsy;
    binsx.SetEdges(fMoves+1, 0, 1);
    binsy.SetEdges(fDim, 0.5, fDim+0.5);
    MH::SetBinning(fBestEver, binsx, binsy);
    
    // For better visibility, omit the first entry in fBestFuncEval
    // It has nothing significant, anyway
    binsx.SetEdges(fMoves,1./(fMoves+1), 1);
    binsx.Apply(fBestFuncEval);
    
}

void MHSimulatedAnnealing::InitFullSimplex()
{
    if (fTimeEvolution)
      delete fTimeEvolution;
    
    fTimeEvolution = new TObjArray;
    fTimeEvolution->SetOwner();
    
    for (Int_t i=0;i<fDim;i++) 
    {
        TH2F *hist = new TH2F(Form("Hist_%d", i), Form("Parameter %d", i), 
                              fMoves+1, 0., 1.,fDim+1,0.5,fDim+1.5);
        hist->SetXTitle("Run Duration");
        hist->SetYTitle("Point Nr. Simplex");
        hist->SetZTitle(Form("Value of Parameter %d",i));
        fTimeEvolution->Add(hist);
    }
}

Bool_t MHSimulatedAnnealing::StoreFullSimplex(const TMatrix &p, const UShort_t move) 
{

    if (!fTimeEvolution)
        return kFALSE;

    Int_t idx=0;
    const Axis_t bin = (move-0.5)/(fMoves+1);
    
    TIter Next(fTimeEvolution);
    TH2F *hist=NULL;
    while ((hist=(TH2F*)Next()))
      {
        for (Int_t i=0;i<fDim+1;i++)
          hist->Fill(bin,i,p(i,idx));
        idx++;
      }
    return kTRUE;
}

Bool_t MHSimulatedAnnealing::StoreBestValueEver(const TVector &y, const Float_t yb, const UShort_t move)
{
    if (y.GetNrows() != fDim) 
      return kFALSE;
    
    const Axis_t bin = (move-0.5)/(fMoves+1);
    
    for (Int_t i=0;i<fDim;i++)
      fBestEver.Fill(bin,0.5+i,((TVector)y)(i));
    
    fBestFuncEval.Fill(bin,yb);
    
    return kTRUE;
}

Bool_t MHSimulatedAnnealing::ChangeTitle(const UShort_t index, const char* title) 
{
    if (!fTimeEvolution) 
      return kFALSE;

    TH2F *hist = NULL;
    if (!(hist = (TH2F*)fTimeEvolution->At(index))) 
      return kFALSE;

    hist->SetNameTitle(Form("Hist_%s",title),title);
    hist->SetYTitle(Form("Value of Parameter %s",title));

    return kTRUE;
}

void MHSimulatedAnnealing::ChangeFuncTitle(const char* title)
{
  fBestFuncEval.SetTitle(title);
}

TObject *MHSimulatedAnnealing::DrawClone(Option_t *opt) const
{
    UShort_t i=2;
  
    TCanvas *c = MakeDefCanvas(this, 720, 810);
    if (fTimeEvolution)
      c->Divide(2,(int)(fDim/2.)+1);
    else
      gPad->Divide(1,2);
  
    gROOT->SetSelectedPad(NULL);
  
    c->cd(1);
    gStyle->SetOptStat(0);
    ((TH1&)fBestFuncEval).DrawCopy();   
    
    c->cd(2);
    gStyle->SetOptStat(10);
    ((TH2&)fBestEver).DrawCopy(opt);   
    
    if (fTimeEvolution)
    {
      TH2F *hist = NULL;
      TIter Next(fTimeEvolution);
      while ((hist=(TH2F*)Next())) 
        {
          c->cd(++i);
          hist->DrawCopy(opt);
        }
    }   
    c->Modified();
    c->Update();
    
    return c;
}



// --------------------------------------------------------------------------
//
// Draw all histograms. 
//
void MHSimulatedAnnealing::Draw(Option_t *opt) 
{
    UShort_t i=2;

    if (!gPad)
      MakeDefCanvas(this,780,940);

    if (fTimeEvolution) 
      gPad->Divide(2,(int)(fDim/2.)+1);
    else 
      gPad->Divide(1,2);

    gPad->cd(1);  
    gStyle->SetOptStat(0);
    fBestFuncEval.Draw();
    gPad->Modified();
    gPad->Update();
    
    gPad->cd(2);
    gStyle->SetOptStat(10);
    fBestEver.Draw(opt);
    gPad->Modified();
    gPad->Update();

    if (!fTimeEvolution)
        return;
    
    TH2F *hist = NULL;
    TIter Next(fTimeEvolution);
    while ((hist=(TH2F*)Next())) 
    {
        gPad->cd(++i);
        hist->Draw(opt);
    }
    gPad->Modified();
    gPad->Update();
}

// --------------------------------------------------------------------------
//
// Delete the histograms.
//
MHSimulatedAnnealing::~MHSimulatedAnnealing() 
{
  if (fTimeEvolution)
    delete fTimeEvolution;
}
  
