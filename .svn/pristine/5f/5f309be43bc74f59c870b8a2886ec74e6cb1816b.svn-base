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
   !   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
   !   Author(s): Harald Kornmayer 1/2001
   !   Author(s): Abelardo Moralejo 2/2005 <mailto:moralejo@pd.infn.it>
   !
   !   Copyright: MAGIC Software Development, 2000-2005
   !
   !
   \* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  MHMcCollectionArea                                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include "MHMcCollectionArea.h" 

#include <TH2.h>
#include <TH3.h>
#include <TCanvas.h>
#include <THStack.h>
#include <TLegend.h>
#include <TArrayD.h>

#include "MH.h"
#include "MBinning.h"

#include "MLog.h"
#include "MLogManip.h"


ClassImp(MHMcCollectionArea);

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
//  Constructor. Creates the three necessary histograms:
//   - selected showers (input)
//   - all showers (input)
//   - collection area (result)
//
MHMcCollectionArea::MHMcCollectionArea(const char *name, const char *title):
  fImpactBins(50), fImpactMax(500.), fMinEvents(10)
{ 
  fName  = name  ? name  : "MHMcCollectionArea";
  fTitle = title ? title : "Collection Area vs. Theta vs. Energy";

  //
  //   Initialize the histogram for the distribution 
  //   Theta vs impact parameter vs E (z, y, x)
  //
  //   As default we set the energy range from 2 Gev to 20000 GeV (in log 4 
  //   orders of magnitude) and for each order we take 25 subdivisions --> 
  //   100 xbins. We set the radius range from 0 m to 500 m with 10 m bin --> 
  //   50 ybins. We make bins equally spaced in cos(theta)
  //
  //   The coarse binning (of fHistColCoarse) is not set by default, the
  //   PreProcess of mmc/MMcCollectionAreaCalc will do it with the binnings
  //   found in the parameter list.
  //

  MBinning binsx;
  MBinning binsy;
  MBinning binsz;

  Int_t nbins = 32;
  TArrayD edges(nbins+1);
    
  edges[0] = 0;
    
  for(int i = 0; i < nbins; i++)
    {
      Double_t x = 1 - i*0.01;  // x = cos(theta) 
      edges[i+1] = acos(x-0.005)*kRad2Deg;
    }

  binsx.SetEdgesLog(100, 2., 20000);               // Energy [GeV]
  binsy.SetEdges   (fImpactBins, 0, fImpactMax);   // Impact parameter [m]
  binsz.SetEdges   (edges);                        // Theta [deg]

  fHistAll = new TH3D();
  fHistSel = new TH3D();
  fHistCol = new TH2D();
  fHistColCoarse = new TH2D();
 
  MH::SetBinning(fHistAll, &binsx, &binsy, &binsz);
  MH::SetBinning(fHistSel, &binsx, &binsy, &binsz);

  fHistColCoarse->SetName("CollectionArea");
  fHistCol->SetName("CollAreaFineBins");
  fHistAll->SetName("AllEvents");
  fHistSel->SetName("SelectedEvents");

  fHistAll->Sumw2();
  fHistSel->Sumw2();

  fHistColCoarse->SetTitle(fTitle);
  fHistCol->SetTitle(fTitle);
  fHistAll->SetTitle("All showers - Theta vs Radius vs Energy");
  fHistSel->SetTitle("Selected showers - Theta vs Radius vs Energy");

  fHistAll->SetDirectory(NULL);
  fHistSel->SetDirectory(NULL);
  fHistCol->SetDirectory(NULL);
  fHistColCoarse->SetDirectory(NULL);

  fHistAll->UseCurrentStyle();
  fHistSel->UseCurrentStyle();
  fHistCol->UseCurrentStyle();
  fHistColCoarse->UseCurrentStyle();

  fHistAll->SetXTitle("E [GeV]");
  fHistAll->SetYTitle("r [m]"); 
  fHistAll->SetZTitle("\\theta [\\circ]");

  fHistSel->SetXTitle("E [GeV]");
  fHistSel->SetYTitle("r [m]");
  fHistSel->SetZTitle("\\theta [\\circ]");

  fHistCol->SetXTitle("E [GeV]");
  fHistCol->SetYTitle("\\theta [\\circ]");
  fHistCol->SetZTitle("A [m^{2}]");

  fHistColCoarse->SetXTitle("E [GeV]");
  fHistColCoarse->SetYTitle("\\theta [\\circ]");
  fHistColCoarse->SetZTitle("A [m^{2}]");
}

// --------------------------------------------------------------------------
//
// Delete the three histograms
//
MHMcCollectionArea::~MHMcCollectionArea()
{
  delete fHistAll;
  delete fHistSel;
  delete fHistCol;
}

// --------------------------------------------------------------------------
//
// Set the (fine) binnings of histograms fHistAll, fHistSel used in the 
// calculations. We do not need to change impact parameter binning.
//
void MHMcCollectionArea::SetBinnings(const MBinning &binsEnergy, 
				     const MBinning &binsTheta)
{
  MBinning binsImpact;
  binsImpact.SetEdges(fImpactBins, 0., fImpactMax);

  MH::SetBinning(fHistAll, &binsEnergy, &binsImpact, &binsTheta);
  MH::SetBinning(fHistSel, &binsEnergy, &binsImpact, &binsTheta);

  fHistAll->Sumw2();
  fHistSel->Sumw2();
}


// --------------------------------------------------------------------------
//
// Set the binnings of the histogram fHistColCoarse, the effective areas
// in the coarse bins used in the analysis.
//
//
void MHMcCollectionArea::SetCoarseBinnings(const MBinning &binsEnergy, 
					   const MBinning &binsTheta)
{
  MH::SetBinning(fHistColCoarse, &binsEnergy, &binsTheta);
}

// --------------------------------------------------------------------------
//
// Fill data into the histogram which contains all showers
//
void MHMcCollectionArea::FillAll(Double_t energy, Double_t radius, Double_t theta)
{
  fHistAll->Fill(energy, radius, theta);
}

// --------------------------------------------------------------------------
//
// Fill data into the histogram which contains the selected showers
//
void MHMcCollectionArea::FillSel(Double_t energy, Double_t radius, Double_t theta)
{
  fHistSel->Fill(energy, radius, theta);
}

// --------------------------------------------------------------------------
//
// Draw
//
void MHMcCollectionArea::Draw(Option_t* option)
{
  //
  // Lego plot
  //
  TCanvas *c1 = new TCanvas();  
  c1->SetLogx();
  c1->SetLogz();
  c1->SetGridx();
  c1->SetGridy();

  fHistCol->Draw("lego2");
    
  //
  // Averagye Aeff
  //
  TCanvas *c2 = new TCanvas();
  c2->SetLogx();
  c2->SetLogy();
  c2->SetGridx();
  c2->SetGridy();

  TH1D* harea = fHistCol->ProjectionX();
  harea->Draw("e1");
  
  //
  // Plot the Aeff for the different theta
  //
  TCanvas *c3 = new TCanvas();
  c3->SetLogx();
  c3->SetLogy();
  c3->SetGridx();
  c3->SetGridy();

  TLegend * leg = new TLegend(0.73,0.65,0.89,0.89);
   
  TAxis* yaxis = fHistCol->GetYaxis();
  const Int_t nbiny = fHistCol->GetYaxis()->GetNbins();
    
  THStack* hs = new THStack("aa","aa");

  hs->Add(harea,"e1");
  leg->AddEntry(harea,"All","l");

  for(Int_t iy=1; iy<=nbiny; iy++)
    {

      TH1D* h1=  fHistCol->ProjectionX(Form("%d",iy),iy,iy);

      if(h1->GetEntries()==0)
	continue;

      cout <<h1->GetEntries() << endl;

      leg->AddEntry(h1,Form("\\theta = %.0f",yaxis->GetBinCenter(iy)),"l");
      h1->SetLineColor(iy);
      hs->Add(h1,"e1");
    }
  hs->SetMinimum(1);
  
  hs->Draw("nostack");
  leg->Draw();

}

// --------------------------------------------------------------------------
//
// Calculate the collection area and set the 'ReadyToSave' flag
// We first calculate the area in fine energy bins, and then do a
// weighted mean to obtain the area in coarse bins. The weights in
// the coarse bins are intended to account for the effect of the 
// energy spectrum in the effective area itself. The weights 
// are taken from the tentative differential spectrum dN_gam/dE given 
// through the function "spectrum". If no such function is supplied, 
// then no weights are applied (and hence the spectrum will be as a 
// flat spectrum in dN_gam/dE). Of course we have a "generated" MC
// spectrum, but within each fine bin the differences in spectrum 
// should not change the result (if bins are fine enough). With no 
// supplied tentative spectrum, each fine bin is weighted equally in
// calculating the area in the coarse bin, and so it is like having a 
// flat spectrum.
//
// You can run this Calc procedure on an already existing 
// MHMcCollectionArea object, as long as it is filled.
//
void MHMcCollectionArea::Calc(TF1 *spectrum)
{
  // Search last impact parameter bin containing events
  // FIXME: this should be done independently for each theta angle.
  //
  TH1D &himpact = *(TH1D*)fHistAll->Project3D("y");

  Int_t impbin;
  for (impbin = himpact.GetNbinsX(); impbin > 0; impbin--)
    if (himpact.GetBinContent(impbin)>0)
      break;

  Float_t max_radius = himpact.GetBinLowEdge(impbin);

  Float_t total_area = TMath::Pi()*max_radius*max_radius;

  for (Int_t ix = 1; ix <= fHistAll->GetNbinsX(); ix++)
    for (Int_t iz = 1; iz <= fHistAll->GetNbinsZ(); iz++)
      {
	fHistAll->SetBinContent(ix, impbin, iz, 0.);
	fHistSel->SetBinContent(ix, impbin, iz, 0.);
	fHistAll->SetBinError(ix, impbin, iz, 0.);
	fHistSel->SetBinError(ix, impbin, iz, 0.);
      }

  TH2D &histsel = *(TH2D*)fHistSel->Project3D("zx,e");
  TH2D &histall = *(TH2D*)fHistAll->Project3D("zx,e"); 
  // "e" option means that errors are computed!


  TAxis &xaxis = *histsel.GetXaxis();
  TAxis &yaxis = *histsel.GetYaxis();
  MH::SetBinning(fHistCol, &xaxis, &yaxis);

  cout << "Total considered MC area = pi * " << max_radius 
       << "^2 square meters" << endl;

  fHistCol->Sumw2();
  fHistCol->Divide(&histsel, &histall, total_area, 1., "b");

  //
  // Now get the effective area in the selected coarse bins. Weight
  // the values in the small bins according the supplied tentative
  // spectrum, if it has been supplied as argument of Calc.
  //

  for (Int_t ibin = 1; ibin <= fHistColCoarse->GetNbinsX(); ibin++)
    for (Int_t jbin = 1; jbin <= fHistColCoarse->GetNbinsY(); jbin++)
      {
	Float_t maxenergy = fHistColCoarse->GetXaxis()->GetBinUpEdge(ibin);
	Float_t minenergy = fHistColCoarse->GetXaxis()->GetBinLowEdge(ibin);

	Float_t maxtheta = fHistColCoarse->GetYaxis()->GetBinUpEdge(jbin);
	Float_t mintheta = fHistColCoarse->GetYaxis()->GetBinLowEdge(jbin);

	// Fine bins ranges covered by the coarse bin ibin, jbin:
	Int_t ibin2max = fHistCol->GetXaxis()->FindBin(maxenergy);
	Int_t ibin2min = fHistCol->GetXaxis()->FindBin(minenergy);

	Int_t jbin2max = fHistCol->GetYaxis()->FindBin(maxtheta);
	Int_t jbin2min = fHistCol->GetYaxis()->FindBin(mintheta);

	Float_t area = 0.;
	Float_t errarea = 0.;
	Float_t norm = 0;

	for (Int_t ibin2 = ibin2min; ibin2 <= ibin2max; ibin2++)
	  {
  	    Float_t weight = spectrum? spectrum->
  	      Eval(fHistCol->GetXaxis()->GetBinCenter(ibin2)) : 1.;

	    for (Int_t jbin2 = jbin2min; jbin2 <= jbin2max; jbin2++)
	      {
		// Skip bins with too few produced MC events
		if (histall.GetBinContent(ibin2,jbin2) < fMinEvents)
		  continue;

		area += weight * fHistCol->GetBinContent(ibin2,jbin2);
		norm += weight;
		errarea += pow(weight * fHistCol->
			       GetBinError(ibin2,jbin2), 2.);
	      }
	  }
	if (norm > 0.)
	  {
	    area /= norm;
	    errarea = sqrt(errarea)/norm;
	  }

	fHistColCoarse->SetBinContent(ibin, jbin, area);
	fHistColCoarse->SetBinError(ibin, jbin, errarea);
      }

  SetReadyToSave();
}
     

