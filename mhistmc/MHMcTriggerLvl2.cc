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
!   Author(s): Nicola Galante, 2003 <mailto:nicola.galante@pi.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHMcTriggerLvl2
//
// This class contains histograms for the Trigger Level2 image parameters
//
/////////////////////////////////////////////////////////////////////////////

#include "MHMcTriggerLvl2.h"

#include <TMath.h>

#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TPad.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TPaveLabel.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MMcTriggerLvl2.h"

using namespace std;

/* Use this insteadif you want to have some value which is the same for all
 your instances of MHMcTriggerLvl2, if not define the values in the constructor
 and remove the 'static'
 */

Int_t MHMcTriggerLvl2::fColorLps = 1;
Int_t MHMcTriggerLvl2::fColorCps = 1;
Int_t MHMcTriggerLvl2::fColorSbc = 1;
Int_t MHMcTriggerLvl2::fColorPs = 1;
Int_t MHMcTriggerLvl2::fColorPsE = 1;
Int_t MHMcTriggerLvl2::fColorCPsE = 1;
Int_t MHMcTriggerLvl2::fColorLPsE = 1;
Int_t MHMcTriggerLvl2::fColorSBCE = 1;


ClassImp(MHMcTriggerLvl2);

// --------------------------------------------------------------------------
//
// Setup three histograms for fLutPseudoSize, fPseudoSize, fSizeBiggerCell
//
MHMcTriggerLvl2::MHMcTriggerLvl2(const char *name, const char *title)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHMcTriggerLvl2";
    fTitle = title ? title : "Trigger L2 image parameters";

    fHistLutPseudoSize = new TH1F("fHistLutPseudoSize",  "Lut Pseudo Size",                13,   0, 13);
    fHistPseudoSize = new TH1F("fHistPseudoSize",   "Pseudo Size",    397,   0, 397);
    fHistCellPseudoSize = new TH1F("fHistCellPseudoSize",   "Cell Pseudo Size",    37,   0, 37);
    fHistSizeBiggerCell = new TH1F("fHistSizeBiggerCell",   "Size in Bigger Cell",            37,   0, 37);

    fHistLutPseudoSizeNorm = new TH1F("fHistLutPseudoSizeNorm",  "Normalized Lut Pseudo Size",                13,   0, 13);
    fHistPseudoSizeNorm = new TH1F("fHistPseudoSizeNorm",   "Normalized Pseudo Size",    397,   0, 397);
    fHistSizeBiggerCellNorm = new TH1F("fHistSizeBiggerCellNorm",   "Normalized Size in Bigger Cell",            37,   0, 37);
    fHistCellPseudoSizeNorm = new TH1F("fHistCellPseudoSizeNorm",   "Normalized Cell Pseudo Size",    37,   0, 37);

    fHistLutPseudoSize->SetDirectory(NULL);
    fHistLutPseudoSizeNorm->SetDirectory(NULL);
    fHistPseudoSize->SetDirectory(NULL);
    fHistPseudoSizeNorm->SetDirectory(NULL);
    fHistSizeBiggerCell->SetDirectory(NULL);
    fHistSizeBiggerCellNorm->SetDirectory(NULL);
    fHistCellPseudoSize->SetDirectory(NULL);
    fHistCellPseudoSizeNorm->SetDirectory(NULL);

    fHistLutPseudoSize->SetXTitle("Number of Pixels");
    fHistLutPseudoSizeNorm->SetXTitle("Number of Pixels");
    fHistPseudoSize->SetXTitle("Number of Pixels");
    fHistPseudoSizeNorm->SetXTitle("Number of Pixels");
    fHistSizeBiggerCell->SetXTitle("Number of Pixels");
    fHistSizeBiggerCellNorm->SetXTitle("Number of Pixels");
    fHistCellPseudoSize->SetXTitle("Number of Pixels");
    fHistCellPseudoSizeNorm->SetXTitle("Number of Pixels");

    fHistLutPseudoSize->SetYTitle("Counts");
    fHistLutPseudoSizeNorm->SetYTitle("Counts/Total Counts");
    fHistPseudoSize->SetYTitle("Counts");
    fHistPseudoSizeNorm->SetYTitle("Counts/Total Counts");
    fHistSizeBiggerCell->SetYTitle("Counts");
    fHistSizeBiggerCellNorm->SetYTitle("Counts/Total Counts");
    fHistCellPseudoSize->SetYTitle("Counts");
    fHistCellPseudoSizeNorm->SetYTitle("Counts/Total Counts");

    fHistPseudoSizeEnergy = new TH2D("fHistPseudoSizeEnergy","Ps Size vs Energy", 40, 1, 5, 397, 0,397);
    fHistCellPseudoSizeEnergy = new TH2D("fHistCellPseudoSizeEnergy","CPS Size vs Energy", 40, 1, 5, 397, 0,397);
    fHistLutPseudoSizeEnergy = new TH2D("fHistLutPseudoSizeEnergy","Ps Size vs Energy", 40, 1, 5, 397, 0,397);
    fHistSizeBiggerCellEnergy = new TH2D("fHistSizeBiggerCellEnergy","Ps Size vs Energy", 40, 1, 5, 397, 0,397);
 
    fHistPseudoSizeEnergy->SetName("fHistPseudoSizeEnergy");
    fHistPseudoSizeEnergy->SetTitle("PseudoSize vs. Energy");
    fHistPseudoSizeEnergy->SetXTitle("Log(E[GeV])");
    fHistPseudoSizeEnergy->SetYTitle("PseudoSize");

    fHistCellPseudoSizeEnergy->SetName("fHistCellPseudoSizeEnergy");
    fHistCellPseudoSizeEnergy->SetTitle("CellPseudoSize vs. Energy");
    fHistCellPseudoSizeEnergy->SetXTitle("Log(E[GeV])");
    fHistCellPseudoSizeEnergy->SetYTitle("CellPseudoSize");

    fHistLutPseudoSizeEnergy->SetName("fHistLutPseudoSizeEnergy");
    fHistLutPseudoSizeEnergy->SetTitle("LutPseudoSize vs. Energy");
    fHistLutPseudoSizeEnergy->SetXTitle("Log(E[GeV])");
    fHistLutPseudoSizeEnergy->SetYTitle("LutPseudoSize");

    fHistSizeBiggerCellEnergy->SetName("fHistSizeBiggerCellEnergy");
    fHistSizeBiggerCellEnergy->SetTitle("Size Bigger Cell vs. Energy");
    fHistSizeBiggerCellEnergy->SetXTitle("Log(E[GeV])");
    fHistSizeBiggerCellEnergy->SetYTitle("Size Bigger Cell");

		

		
    fFNorm = new TF1("FNorm", "1", -1, 397);
}

// --------------------------------------------------------------------------
//
// Delete the histograms
//
MHMcTriggerLvl2::~MHMcTriggerLvl2()
{
    delete fHistLutPseudoSize;
    delete fHistPseudoSize;
    delete fHistCellPseudoSize;
    delete fHistSizeBiggerCell;
    delete fHistLutPseudoSizeNorm;
    delete fHistPseudoSizeNorm;
    delete fHistCellPseudoSizeNorm;
    delete fHistSizeBiggerCellNorm;
    delete fHistPseudoSizeEnergy;
    delete fHistCellPseudoSizeEnergy;
    delete fHistLutPseudoSizeEnergy;
    delete fHistSizeBiggerCellEnergy;
    delete fFNorm;
}


// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MMcTriggerLvl2-Container.
// Be careful: Only call this with an object of type MMcTriggerLvl2
//
Bool_t MHMcTriggerLvl2::Fill(const MParContainer *par, const Stat_t w)
{
  const MMcTriggerLvl2 &h = *(MMcTriggerLvl2 *)par;
 
  fHistLutPseudoSize->Fill(h.GetLutPseudoSize());
  fHistPseudoSize->Fill(h.GetPseudoSize());
  fHistCellPseudoSize->Fill(h.GetCellPseudoSize());
  fHistSizeBiggerCell->Fill(h.GetSizeBiggerCell());
  fHistPseudoSizeEnergy->Fill(TMath::Log10(h.GetEnergy()), h.GetPseudoSize());
  fHistCellPseudoSizeEnergy->Fill(TMath::Log10(h.GetEnergy()), h.GetCellPseudoSize());
  fHistLutPseudoSizeEnergy->Fill(TMath::Log10(h.GetEnergy()), h.GetLutPseudoSize());
  fHistSizeBiggerCellEnergy->Fill(TMath::Log10(h.GetEnergy()), h.GetSizeBiggerCell());

  return kTRUE;
}




// --------------------------------------------------------------------------
//
// This is the private function member which draw a clone of a histogram. 
// This method is called by the DrawClone method.
//
TObject *MHMcTriggerLvl2::DrawHist(TH1 &hist, TH1 &histNorm, const TString &canvasname, Int_t &col) const
{
    col++;

    TCanvas *c = (TCanvas*)gROOT->FindObject(canvasname);

    Bool_t same = kTRUE;
    if (!c)
    {
        c = MakeDefCanvas(canvasname,canvasname, 800, 610);
        c->Divide(2,1);
        same = kFALSE;
    }

    c->cd(1);
    hist.SetLineColor(col);
    hist.DrawCopy(same?"same":"");

    c->cd(2);
    histNorm.SetLineColor(col);
    histNorm.DrawCopy(same?"same":"");

    return c;
}


// --------------------------------------------------------------------------
//
// This is the private function member which draw a clone of a 2D-histogram. 
// This method is called by the DrawClone method.
//
TObject *MHMcTriggerLvl2::Draw2DHist(TH1 &hist, const TString &canvasname, Int_t &col) const
{
    col++;

    TCanvas *c = (TCanvas*)gROOT->FindObject(canvasname);

    Bool_t same = kTRUE;
    if (!c)
    {
        c = MakeDefCanvas(canvasname,canvasname, 800, 600);
        same = kFALSE;
    }

    hist.SetLineColor(col);
    hist.DrawCopy(same?"same":"");

    return c;
}


// --------------------------------------------------------------------------
//
// Draw a clone of a data member histogram. So that the object can be deleted
// and the histogram is still visible in the canvas.
// The cloned object are deleted together with the canvas if the canvas is
// destroyed. If you want to handle dostroying the canvas you can get a
// pointer to it from this function
// Possible options are:
//      "lps" for the fLutPseudoSize histogram;
//      "sbc" for the fSizeBiggerCell histogram;
//      "ps" for the fPseudoSize histogram;
//      "lut-energy" for the fLutPseudoSize vs. energy 2D histogram
//      "size-energy" for the fPseudoSize vs. energy 2D histogram
//      "big-energy" for the fSizeBiggerCell vs. energy 2D histogram
//      
//      default: "ps"
//
TObject *MHMcTriggerLvl2::DrawClone(Option_t *opt) const
{
    TString str(opt);

    if (str.IsNull())
        str = "ps";

    if (!str.Contains("lps", TString::kIgnoreCase) &&
	!str.Contains("cps", TString::kIgnoreCase) &&
        !str.Contains("sbc", TString::kIgnoreCase) &&
        !str.Contains("ps",  TString::kIgnoreCase) &&
        !str.Contains("lut-energy",  TString::kIgnoreCase) &&
	!str.Contains("size-energy",  TString::kIgnoreCase) &&
	!str.Contains("cellsize-energy",  TString::kIgnoreCase) &&
	!str.Contains("big-energy",  TString::kIgnoreCase))
    {
        *fLog << "ARGH!@! Possible options are \"lps\", \"cps\", \"sbc\", \"ps\", \"lut-energy\", \"size-energy\", \"cellsize-energy\", \"big-energy\" or NULL!" <<endl;
        return NULL;
    }

    if (str.Contains("lps",TString::kIgnoreCase)){
      TH1 *hist=NormalizeHist(*fHistLutPseudoSizeNorm, fHistLutPseudoSize);
      return DrawHist(*fHistLutPseudoSize, *hist, "CanvasLPS", fColorLps);
    }

    if (str.Contains("cps",TString::kIgnoreCase)){
      TH1 *hist=NormalizeHist(*fHistCellPseudoSizeNorm, fHistCellPseudoSize);
      return DrawHist(*fHistCellPseudoSize, *hist, "CanvasCPS", fColorCps);
    }

    if (str.Contains("sbc",TString::kIgnoreCase)){
      TH1 *hist=NormalizeHist(*fHistSizeBiggerCellNorm, fHistSizeBiggerCell);
      return DrawHist(*fHistSizeBiggerCell, *hist, "CanvasSBC", fColorSbc);
    }

    if (str.Contains("ps",TString::kIgnoreCase)){
      TH1 *hist=NormalizeHist(*fHistPseudoSizeNorm, fHistPseudoSize);
      return DrawHist(*fHistPseudoSize, *hist, "CanvasPS", fColorPs);
    }

    if (str.Contains("size-energy",TString::kIgnoreCase))
      return Draw2DHist(*fHistPseudoSizeEnergy, "CanvasPSE", fColorPsE);

    if (str.Contains("cellsize-energy",TString::kIgnoreCase))
      return Draw2DHist(*fHistCellPseudoSizeEnergy, "CanvasCPSE", fColorCPsE);

    if (str.Contains("lut-energy",TString::kIgnoreCase))
      return Draw2DHist(*fHistLutPseudoSizeEnergy, "CanvasLPSE", fColorLPsE);

    if (str.Contains("big-energy",TString::kIgnoreCase))
      return Draw2DHist(*fHistSizeBiggerCellEnergy, "CanvasSBCE", fColorSBCE);
    

    return NULL;
}



// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the three histograms into it.
// Be careful: The histograms belongs to this object and won't get deleted
// together with the canvas.
//
void MHMcTriggerLvl2::Draw(Option_t *)
{
  MakeDefCanvas("c","L2T Parameters", 720, 810);
  
  TPad* pad1 = new TPad("pad1","Pad with fLutPseudoSize", 0.003, 0.7, 0.4, 0.997);
  TPad* pad2 = new TPad("pad2","Pad with fSizeBiggerCell", 0.403, 0.7, 0.997, 0.997);
  TPad* pad3 = new TPad("pad3","Pad with fPseudoSize", 0.003, 0.003, 0.997, 0.697);
  pad1->Draw();
  pad2->Draw();
  pad3->Draw();
  
  pad1->cd();
  fHistLutPseudoSize->Draw();
  
  pad2->cd();
  fHistSizeBiggerCell->Draw();
  
  pad3->cd();
  fHistPseudoSize->Draw();
}



// --------------------------------------------------------------------------
//
//  Return the histogram by its name. This method returns a (TObject *)
//  so remember to cast the returned object if you want to work with it.
//
TH1 *MHMcTriggerLvl2::GetHistByName(const TString name) const
{
  
    if (name.Contains("fHistLutPseudoSizeEnergy", TString::kIgnoreCase))
	return fHistLutPseudoSizeEnergy;
    if (name.Contains("fHistSizeBiggerCellEnergy", TString::kIgnoreCase))
        return fHistSizeBiggerCellEnergy;
    if (name.Contains("fHistPseudoSizeEnergy", TString::kIgnoreCase))
        return fHistPseudoSizeEnergy;
    if (name.Contains("fHistCellPseudoSizeEnergy", TString::kIgnoreCase))
        return fHistCellPseudoSizeEnergy;

    if (name.Contains("fHistLutPseudoSizeNorm", TString::kIgnoreCase))
      return fHistLutPseudoSizeNorm;
    if (name.Contains("fHistSizeBiggerCellNorm", TString::kIgnoreCase))
        return fHistSizeBiggerCellNorm;
    if (name.Contains("fHistPseudoSizeNorm", TString::kIgnoreCase))
        return fHistPseudoSizeNorm;
    if (name.Contains("fHistCellPseudoSizeNorm", TString::kIgnoreCase))
        return fHistCellPseudoSizeNorm;
	    
    if (name.Contains("fHistLutPseudoSize", TString::kIgnoreCase))
      return fHistLutPseudoSize;
    if (name.Contains("fHistSizeBiggerCell", TString::kIgnoreCase))
        return fHistSizeBiggerCell;
    if (name.Contains("fHistPseudoSize", TString::kIgnoreCase))
        return fHistPseudoSize;
    if (name.Contains("fHistCellPseudoSize", TString::kIgnoreCase))
        return fHistCellPseudoSize;

    
    return NULL;
}



// --------------------------------------------------------------------------
//
// Normalize the histogram on its integral, i.e.  normalize the
// values of the histogram on the statistics. This method creates
// a copy (histNorm) of the histogram to normalize (hist) and normalize
// the copy (histNorm) on its integral. It returns histNorm.
//
TH1 *MHMcTriggerLvl2::NormalizeHist(TH1 &histNorm, TH1 *hist) const
{
  if (&histNorm == hist){
    *fLog << "ARGH!@! You cannot pass the same histogram into each argument!" << endl;
    return NULL;
  }

  if ((hist == NULL) || (hist->Integral() == (Stat_t)0)){
    *fLog << "ARGH!@! You are trying to normalize the histogram to 0!" << endl;
    return NULL;
  }
  
  histNorm.Reset("ICE");
  histNorm.Add(hist, 1);
  histNorm.Divide(fFNorm, (Double_t)(hist->Integral()));

  return &histNorm;
}
