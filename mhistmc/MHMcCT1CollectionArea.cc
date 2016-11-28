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
!   Author(s): A. Moralejo 3/2003  <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  MHMcCT1CollectionArea                                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include "MHMcCT1CollectionArea.h" 

#include <TH2.h>
#include <TCanvas.h>

#include "MMcEvt.hxx"
#include "MH.h"
#include "MBinning.h"
#include "MParList.h"
#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHMcCT1CollectionArea);

using namespace std;

// --------------------------------------------------------------------------
//
//  Creates the three necessary histograms:
//   - selected showers (input)
//   - all showers (input)
//   - collection area (result)
//
MHMcCT1CollectionArea::MHMcCT1CollectionArea(const char *name, const char *title) :
    fEaxis(kLog10)
{ 
  //
  //   nbins, minEnergy, maxEnergy defaults:
  //   we set the energy range from 100 Gev to 30000 GeV (in log, 3.5 orders
  //   of magnitude) and for each order we take 10 subdivisions --> 35 xbins
  //   we set the theta range from 12.5 to 48 deg, with 6 bins (the latter
  //   choice has been done to make the bin centers as close as possible to 
  //   the actual zenith angles in the CT1 MC sample).
  //

  fName  = name  ? name  : "MHMcCT1CollectionArea";
  fTitle = title ? title : "Collection Area vs. log10 Energy";

  fHistAll = new TH2D;
  fHistSel = new TH2D;
  fHistCol = new TH2D;

  fHistCol->SetName(fName);
  fHistAll->SetName("AllEvents");
  fHistSel->SetName("SelectedEvents");

  fHistCol->SetTitle(fTitle);
  fHistAll->SetTitle("All showers - Theta vs log10 Energy distribution");
  fHistSel->SetTitle("Selected showers - Theta vs log10 Energy distribution");

  fHistAll->SetDirectory(NULL);
  fHistSel->SetDirectory(NULL);
  fHistCol->SetDirectory(NULL);

  fHistAll->SetXTitle("log10(E [GeV])");
  fHistAll->SetYTitle("\\Theta [\\circ]");
  fHistAll->SetZTitle("Counts");

  fHistSel->SetXTitle("log10(E [GeV])");
  fHistSel->SetYTitle("\\Theta [\\circ]");
  fHistSel->SetZTitle("Counts");

  fHistCol->SetXTitle("log10(E [GeV])");
  fHistCol->SetYTitle("theta [deg]");
  fHistCol->SetZTitle("A [m^{2}]");

}

// --------------------------------------------------------------------------
//
// Delete the three histograms
//
MHMcCT1CollectionArea::~MHMcCT1CollectionArea()
{
  delete fHistAll;
  delete fHistSel;
  delete fHistCol;
}

// --------------------------------------------------------------------------
//
// Set the binnings and prepare the filling of the histograms
//
Bool_t MHMcCT1CollectionArea::SetupFill(const MParList *plist)
{
    const MBinning* binsenergy = (MBinning*)plist->FindObject("BinningE");
    const MBinning* binstheta  = (MBinning*)plist->FindObject("BinningTheta");

    if (!binsenergy || !binstheta)
    {
        *fLog << err << dbginf << "At least one MBinning not found... aborting.";
        *fLog << endl;
        return kFALSE;
    }

    SetBinning(fHistAll, binsenergy, binstheta);
    SetBinning(fHistSel, binsenergy, binstheta);
    SetBinning(fHistCol, binsenergy, binstheta);

    fHistAll->Sumw2();
    fHistSel->Sumw2();
    fHistCol->Sumw2();

    if (fEaxis == kLinear)
    {
        fTitle = "Collection Area vs. Energy";
        fHistCol->SetTitle(fTitle);
        fHistAll->SetTitle("All showers - Theta vs Energy distribution");
        fHistSel->SetTitle("Selected showers - Theta vs Energy distribution");
        fHistCol->SetXTitle("E [GeV]");
        fHistAll->SetXTitle("E [GeV]");
        fHistSel->SetXTitle("E [GeV]");
    }

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Fill data into the histogram which contains the selected showers
//
Bool_t MHMcCT1CollectionArea::Fill(const MParContainer *par, const Stat_t w)
{ 
  MMcEvt &mcevt = *(MMcEvt*)par;

  const Double_t E = fEaxis==kLinear ? mcevt.GetEnergy() : log10(mcevt.GetEnergy());

  fHistSel->Fill(E, kRad2Deg*mcevt.GetTelescopeTheta(), w);

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Draw the histogram with all showers
//
void MHMcCT1CollectionArea::DrawAll(Option_t* option)
{
  if (!gPad)
    MH::MakeDefCanvas(fHistAll);

  fHistAll->Draw(option);

  gPad->Modified();
  gPad->Update();
}

// --------------------------------------------------------------------------
//
// Draw the histogram with the selected showers only.
//
void MHMcCT1CollectionArea::DrawSel(Option_t* option)
{
  if (!gPad)
    MH::MakeDefCanvas(fHistSel);

  fHistSel->Draw(option);

  gPad->Modified();
  gPad->Update();
}

// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the histogram into it.
// Be careful: The histogram belongs to this object and won't get deleted
// together with the canvas.
//
TObject *MHMcCT1CollectionArea::DrawClone(Option_t* option) const
{
  TCanvas &c = *MakeDefCanvas("CollArea", "Collection area plots", 600, 600);
  c.Divide(2,2);

  //
  // This is necessary to get the expected behaviour of DrawClone
  //
  gROOT->SetSelectedPad(NULL);

  c.cd(1);
  if (fEaxis == kLinear)
    gPad->SetLogx();
  fHistCol->SetDirectory(NULL);
  fHistCol->DrawCopy(option);

  c.cd(2);
  if (fEaxis == kLinear)
    gPad->SetLogx();
  fHistSel->SetDirectory(NULL);
  fHistSel->DrawCopy(option);

  c.cd(3);
  if (fEaxis == kLinear)
    gPad->SetLogx();
  fHistAll->SetDirectory(NULL);
  fHistAll->DrawCopy(option);


  c.Modified();
  c.Update();

  return &c;
}

void MHMcCT1CollectionArea::Draw(Option_t* option)
{
  if (!gPad)
    MH::MakeDefCanvas(fHistCol);

  fHistCol->Draw(option);

  gPad->Modified();
  gPad->Update();
}

//
//  Calculate the Efficiency (collection area) for the CT1 MC sample
//  and set the 'ReadyToSave' flag
//
void MHMcCT1CollectionArea::CalcEfficiency()
{
  //
  // Here we estimate the total number of showers in each energy bin
  // from the known the energy range and spectral index of the generated 
  // showers. This procedure is intended for the CT1 MC files. The total 
  // number of generated events, collection area, spectral index etc will be 
  // set here by hand, so make sure that the MC sample you are using is the 
  // right one (check all these quantities in your files and compare with
  // what is written below. In some theta bins, there are two different 
  // productions, with different energy limits but with the same spectral 
  // slope. We account for this when calculating the original number of 
  // events in each energy bin.
  //
  // The theta angle with which the MC data (from D. Kranich) were produced 
  // is not exactly the center of the theta bins we are using (the bin limits 
  // should be 0.0, 17.5, 23.5, 29.5, 35.5, 42., 50.). The theta variable in 
  // the MC root file has nevertheless been changed (W.Wittek) to correspond 
  // to the centers of these bins. Only in the first bin is the difference big:
  // the data were produced at theta = 15 degrees, whreas the bin center is at
  // 8.75 degrees. Howeverm at such low z.a. the shower characteristics change 
  // very slowly with theta.
  //
  //
  //
  // Only for the binning taken from D. Kranich :
  //

  for (Int_t thetabin = 1; thetabin <= fHistAll->GetNbinsY(); thetabin++)
    {
      // This theta is not exactly the one of the MC events, just about 
      // the same (bins have been selected so):

      Float_t theta = fHistAll->GetYaxis()->GetBinCenter(thetabin);
      Float_t thetalo = fHistAll->GetYaxis()->GetBinLowEdge(thetabin);
      Float_t thetahi = fHistAll->GetYaxis()->GetBinLowEdge(thetabin+1);

      Float_t emin[4];       // Minimum energy in MC sample
      Float_t emax[4];       // Maximum energy in MC sample
      Float_t index[4];      // Spectral index
      Float_t numevts[4];    // Number of events
      Float_t multfactor[4]; // Factor by which the original number of events in an MC 
                             // sample has been multiplied to account for the differences
                             // in the generation areas of the various samples.
      Float_t rmax;          // Maximum impact parameter range (on ground up to 45 degrees, 
                             // on a plane perpendicular to Shower axis for 55 and 65 deg).

      memset(emin,    0, 4*sizeof(Float_t));
      memset(emax,    0, 4*sizeof(Float_t));
      memset(index,   0, 4*sizeof(Float_t));
      memset(numevts, 0, 4*sizeof(Float_t));
      rmax = 0.;

      multfactor[0] = 1.;
      multfactor[1] = 1.;
      multfactor[2] = 1.;
      multfactor[3] = 1.;

      //
      // rmin and rmax are the minimum and maximum values of the impact 
      // parameter of the shower on the ground (horizontal plane).
      //

      Int_t num_MC_samples = 0;

      //if (theta > 8 && theta < 9)   // 8.75 deg
      if (  thetalo<8.75  && 8.75<thetahi)   // 8.75 deg
	{
	  emin[0] = 300.;
	  emax[0] = 400.;  // Energies in GeV.
	  index[0] = 1.5;
	  numevts[0] = 4000.;

	  emin[1] = 400.;
	  emax[1] = 30000.;
	  index[1] = 1.5;
	  numevts[1] = 25740.;

	  rmax = 250.;     //meters
	  num_MC_samples = 2;
	}
      //else if (theta > 20 && theta < 21)  // 20.5 deg 
      else if (  thetalo<20.5  && 20.5<thetahi)   // 20.5 deg
	{
	  emin[0] = 300.;
	  emax[0] = 400.;  // Energies in GeV.
	  index[0] = 1.5;
	  numevts[0] = 6611.;

	  emin[1] = 400.;
	  emax[1] = 30000.;
	  index[1] = 1.5;
	  numevts[1] = 24448.;

	  rmax = 263.;
	  num_MC_samples = 2;
	}
      //else if (theta > 26 && theta < 27)  // 26.5 degrees
      else if (  thetalo<26.5  && 26.5<thetahi)   // 26.5 deg
	{
	  emin[0] = 300.;
	  emax[0] = 400.;  // Energies in GeV.
	  index[0] = 1.5;
	  numevts[0] = 4000.;

	  emin[1] = 400.;
	  emax[1] = 30000.;
	  index[1] = 1.5;
	  numevts[1] = 26316.;

	  rmax = 290.;     //meters
	  num_MC_samples = 2;
	}
      //else if (theta > 32 && theta < 33)  // 32.5 degrees
      else if (  thetalo<32.5  && 32.5<thetahi)   // 32.5 deg
	{
	  emin[0] = 300.;
	  emax[0] = 30000.;  // Energies in GeV.
	  index[0] = 1.5;
	  numevts[0] = 33646.;

	  rmax = 350.;     //meters
	  num_MC_samples = 1;
	}
      //else if (theta > 38 && theta < 39)  // 38.75 degrees
      else if (  thetalo<38.75  && 38.75<thetahi)   // 38.75 deg
	{
	  emin[0] = 300.;
	  emax[0] = 30000.;  // Energies in GeV.
	  index[0] = 1.5;
	  numevts[0] = 38415.;

	  rmax = 380.;     //meters
	  num_MC_samples = 1;
	}
      //else if (theta > 45 && theta < 47)  // 46 degrees
      else if (  thetalo<46  && 46<thetahi)   // 46 deg
	{
	  emin[0] = 300.;
	  emax[0] = 50000.;  // Energies in GeV.
	  index[0] = 1.5;
	  numevts[0] = 30197.;

	  rmax = 565.;     //meters
	  num_MC_samples = 1;
	}
      //else if (theta > 54 && theta < 56)  // 55 degrees
      else if (  thetalo<55  && 55<thetahi)   // 55 deg
	{
	  //
	  // The value of numevts in the first sample (below) has been
	  // changed to simplify calculations. We have multiplied it
	  // times 1.2808997 to convert it to the number it would be if 
	  // the generation area was equal to that of the other samples 
	  // at 55 degrees (pi*600**2 m2). This has to be taken into account 
	  // in the error in the number of events.
	  //

	  emin[0] = 500.;
	  emax[0] = 50000.;  // Energies in GeV.
	  index[0] = 1.5;
	  numevts[0] = 3298.;
	  multfactor[0] = 1.2808997;

	  emin[1] = 1500.;
	  emax[1] = 50000.;  // Energies in GeV.
	  index[1] = 1.5;
	  numevts[1] = 22229.;

	  emin[2] = 1500.;
	  emax[2] = 50000.;  // Energies in GeV.
	  index[2] = 1.7;
	  numevts[2] = 7553.;

	  rmax = 600;     //meters
	  num_MC_samples = 3;
	}

      //else if (theta > 64 && theta < 66)  // 65 degrees
      else if (  thetalo<65  && 65<thetahi)   // 65 deg
	{
	  emin[0] = 2000.;
	  emax[0] = 50000.;  // Energies in GeV.
	  index[0] = 1.5;
	  numevts[0] = 16310.;

	  emin[1] = 2000.;
	  emax[1] = 50000.;  // Energies in GeV.
	  index[1] = 1.7;
	  numevts[1] = 3000.;

	  //
	  // The value of numevts in the next two samples (below) has been
	  // changed to simplify calculations. We have converted them to the
	  // number it would be if the generation area was equal to that of 
	  // the first two samples at 65 degrees (pi*800**2 m2) (four times 
	  // as many, since the original maximum impact parameter was 400
	  // instead of 800. This is taken into account in the error too.
	  //

	  emin[2] = 5000.;
	  emax[2] = 50000.;  // Energies in GeV.
	  index[2] = 1.5;
	  numevts[2] = 56584.;
	  multfactor[2] = 4;

	  emin[3] = 5000.;
	  emax[3] = 50000.;  // Energies in GeV.
	  index[3] = 1.7;
	  numevts[3] = 11464;
	  multfactor[3] = 4;

	  rmax = 800;     // meters
	  num_MC_samples = 4;
	}


      for (Int_t i=1; i <= fHistAll->GetNbinsX(); i++)
	{
	  Float_t e1;
	  Float_t e2;

	  if (fEaxis == kLog10)
	    {
	      e1 = pow(10.,fHistAll->GetXaxis()->GetBinLowEdge(i));
	      e2 = pow(10.,fHistAll->GetXaxis()->GetBinLowEdge(i+1));
	    }
	  else
	    {
	      e1 = fHistAll->GetXaxis()->GetBinLowEdge(i);
	      e2 = fHistAll->GetXaxis()->GetBinLowEdge(i+1);
	    }

	  Float_t events = 0.;
	  Float_t errevents = 0.;

	  for (Int_t sample = 0; sample < num_MC_samples; sample++)
	    {
	      Float_t expo = 1.-index[sample];
	      Float_t k = numevts[sample] / (pow(emax[sample],expo) - pow(emin[sample],expo));

	      if (e2 < emin[sample] || e1 > emax[sample])
		continue;

	      if (emin[sample] > e1) 
		e1 = emin[sample];

	      if (emax[sample] < e2) 
		e2 = emax[sample];

	      events += k * (pow(e2, expo) - pow(e1, expo));
	      errevents += multfactor[sample] * events;
	    }

	  errevents= sqrt(errevents);

	  fHistAll->SetBinContent(i, thetabin, events);
	  fHistAll->SetBinError(i, thetabin, errevents);
	}

      // -----------------------------------------------------------

      const Float_t dr = TMath::Pi() * rmax * rmax;

      for (Int_t ix = 1; ix <= fHistAll->GetNbinsX(); ix++)
	{
	  const Float_t Na = fHistAll->GetBinContent(ix,thetabin);

	  if (Na <= 0)
	    {
	      //
	      // If energy is large, this case means that no or very few events
	      // were generated at this energy bin. In this case we assign it 
	      // the effective area of the bin below it in energy. If energy is
	      // below 1E4, it means that no events triggered -> eff area = 0
	      //
	      // NOW DISABLED: because collection area after analysis does not
	      // saturate at high E!
	      //

	      /*
	      if (fHistSel->GetXaxis()->GetBinLowEdge(ix) > 4.)
		{
		  fHistCol->SetBinContent(ix, thetabin, fHistCol->GetBinContent(ix-1, thetabin));
		  fHistCol->SetBinError(ix, thetabin, fHistCol->GetBinError(ix-1, thetabin));
		}
	      */
	      continue;
	    }

	  const Float_t Ns = fHistSel->GetBinContent(ix,thetabin);

	  // Since Na is an estimate of the total number of showers generated
	  // in the energy bin, it may happen that Ns (triggered showers) is
	  // larger than Na. In that case, the bin is skipped:

	  if (Na < Ns)
	    continue;

	  const Double_t eff = Ns/Na;
	  const Double_t efferr = sqrt((1.-eff)*Ns)/Na;

	  //
	  // Now we get the total area, perpendicular to the observation direction
	  // in which the events were generated (correct for cos theta):
	  //

	  Float_t area = dr;

	  if (theta < 50)
	    area *= cos(theta*TMath::Pi()/180.);

	  // Above 50 degrees MC was generated with Corsika 6.xx, and the cores
	  // were distributed on a circle perpendicular to the observation direction, 
	  // and not on ground, hence the correction for cos(theta) is not necessary.
	  //


	  fHistCol->SetBinContent(ix, thetabin, eff*area);
	  fHistCol->SetBinError(ix, thetabin, efferr*area);

	}
    }

  SetReadyToSave();
}
