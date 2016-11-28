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
!   Author(s): Javier López 05/2004 <mailto:jlopezs@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//                                                               
// MHPSFFromStars                                               
//
/////////////////////////////////////////////////////////////////////////////
#include "MHPSFFromStars.h"

#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TText.h>
#include <TPaveText.h>
#include <TF1.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeomPix.h"
#include "MCameraDC.h"

#include "MStarLocalCam.h"
#include "MStarLocalPos.h"

#include "MParList.h"

ClassImp(MHPSFFromStars);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor. 
//
MHPSFFromStars::MHPSFFromStars(Int_t starpos, const char *name, const char *title)
    : fGeom(NULL), fCurr(NULL), fSelectedStarPos(starpos)
{
    fName  = name  ? name  : "MHPSFFromStars";
    fTitle = title ? title : "Class to fill the Point Spread Function histograms";

    fNumEvents=0;

    fHistMeanX.SetName("MeanX");
    fHistMeanX.SetTitle("Mean X of the Fit");
    fHistMeanX.SetDirectory(NULL);
    fHistMeanX.UseCurrentStyle();
    fHistMeanX.SetXTitle("Mean X [\\circ]");
    fHistMeanX.SetYTitle("Counts [#]");
    fHistMeanX.SetBins(800,-2.0,2.0);

    fHistMeanY.SetName("MeanY");
    fHistMeanY.SetTitle("Mean Y of the Fit");
    fHistMeanY.SetDirectory(NULL);
    fHistMeanY.UseCurrentStyle();
    fHistMeanY.SetXTitle("Mean Y [\\circ]");
    fHistMeanY.SetYTitle("Counts [#]");
    fHistMeanY.SetBins(800,-2.0,2.0);

    fHistSigmaMinor.SetName("SigmaMinor");
    fHistSigmaMinor.SetTitle("Sigma X of the Fit");
    fHistSigmaMinor.SetDirectory(NULL);
    fHistSigmaMinor.UseCurrentStyle();
    fHistSigmaMinor.SetXTitle("Sigma X [\\circ]");
    fHistSigmaMinor.SetYTitle("Counts [#]");
    fHistSigmaMinor.SetBins(160,0,0.8);
    
    fHistSigmaMajor.SetName("SigmaMajor");
    fHistSigmaMajor.SetTitle("Sigma Y of the Fit");
    fHistSigmaMajor.SetDirectory(NULL);
    fHistSigmaMajor.UseCurrentStyle();
    fHistSigmaMajor.SetXTitle("Sigma Y [\\circ]");
    fHistSigmaMajor.SetYTitle("Counts [#]");
    fHistSigmaMajor.SetBins(160,0,0.8);

    fProjectionX.SetName("ProjetionX");
    fProjectionX.SetTitle("Star projection in X axis");
    fProjectionX.SetDirectory(NULL);
    fProjectionX.UseCurrentStyle();
    fProjectionX.SetXTitle("X [\\circ]");
    fProjectionX.SetYTitle("DC [uA]");
    fProjectionX.SetBins(16,-0.8,0.8);

    fProjectionY.SetName("ProjetionY");
    fProjectionY.SetTitle("Star projection in Y axis");
    fProjectionY.SetDirectory(NULL);
    fProjectionY.UseCurrentStyle();
    fProjectionY.SetXTitle("Y [\\circ]");
    fProjectionY.SetYTitle("DC [uA]");
    fProjectionY.SetBins(16,-0.8,0.8);

    fProfile.SetName("Profile");
    fProfile.SetTitle("Star profile");
    fProfile.SetDirectory(NULL);
    fProfile.UseCurrentStyle();
    fProfile.SetXTitle("X [\\circ]");
    fProfile.SetYTitle("X [\\circ]");
    fProfile.SetZTitle("DC [uA]");
    fProfile.SetBins(16,-0.8,0.8,16,-0.8,0.8);

    fvsTimeMeanX.Set(0);
    fvsTimeMeanY.Set(0);
    fvsTimeSigmaMinor.Set(0);
    fvsTimeSigmaMajor.Set(0);

    //  fTime.Set(0);
    fEvent.Set(0);

}

MHPSFFromStars::~MHPSFFromStars()
{
  delete fGraphMeanX;
  delete fGraphMeanY;
  delete fGraphSigmaMinor;
  delete fGraphSigmaMajor;
}


// --------------------------------------------------------------------------
//
// TObject *MHPSFFromStars::lone(const char *) const
// {
//   MHPSFFromStars *hpsf = new MHPSFFromStars();
  
//   return hpsf;
// }

// --------------------------------------------------------------------------
//
// Gets the pointers to:
//
Bool_t MHPSFFromStars::SetupFill(const MParList *pList)
{
  
  fGeom = (MGeomCam*)pList->FindObject("MGeomCam");
  if (!fGeom)
  {
      *fLog << err << GetDescriptor() 
            << ": MGeomCam not found... aborting." << endl;
      return kFALSE;
  }

  TArrayC PixelsUsed;
  PixelsUsed.Set(577);
  PixelsUsed.Reset((Char_t)kTRUE);
  fCamera.SetGeometry(*fGeom,"StarsDisplay","StarsDisplay");
  fCamera.SetUsed(PixelsUsed);

  fCurr = (MCameraDC*)pList->FindObject(AddSerialNumber("MCameraDC"));
  
  if (!fCurr)
    {
      *fLog << err << AddSerialNumber("MCameraDC") << " not found ... aborting" << endl;
      return kFALSE;
    }

  return kTRUE;
}

//--------------------------------------------------------------------------------
//
//
Bool_t MHPSFFromStars::Fill(const MParContainer *par, const Stat_t w)
{
  const UInt_t numPixels = fGeom->GetNumPixels();

  MStarLocalCam* stars = (MStarLocalCam*)par;
  if (!stars)
    {
      *fLog << err << "No argument in " << GetName() << "::Fill... abort." << endl;
      return kFALSE;
    }

  //
  MStarLocalPos* star = SelectStar(stars);
  if (star == NULL)
    return kCONTINUE;
  

  //Check good conditions of the fitting procedure
  Float_t expectedStarXPos = star->GetMeanX();
  Float_t expectedStarYPos = star->GetMeanY();

  if ( star->GetChiSquareNdof() == 0 
       || star->GetChiSquareNdof()>5.)
//       || TMath::Sqrt(expectedStarXPos*expectedStarXPos+expectedStarYPos*expectedStarYPos)>150.)
    {
      *fLog << warn << "Star selected were bad reconstructed" << endl;
      return kCONTINUE;
    }
  


  //Fill Histograms
  fHistMeanX.Fill(star->GetMeanX()*fGeom->GetConvMm2Deg());
  fHistMeanY.Fill(star->GetMeanY()*fGeom->GetConvMm2Deg());
  if (star->GetSigmaMinorAxis() < 0)
    *fLog << err << GetName() << " Sigma of the fit " << star->GetSigmaMinorAxis() << " negative" << endl;
  if (star->GetSigmaMajorAxis() < 0)
    *fLog << err << GetName() << " Sigma of the fit " << star->GetSigmaMajorAxis() << " negative" << endl;
  fHistSigmaMinor.Fill(star->GetSigmaMinorAxis()*fGeom->GetConvMm2Deg());
  fHistSigmaMajor.Fill(star->GetSigmaMajorAxis()*fGeom->GetConvMm2Deg());
  
  fCamera.AddCamContent(*fCurr);
  
  //Selected star X(andY) projections
  Float_t ringofinterest = 4*(star->GetSigmaMajorAxis()>star->GetSigmaMinorAxis()?star->GetSigmaMajorAxis():star->GetSigmaMinorAxis());

  // First define a area of interest around the expected position of the star
  for (UInt_t pix=1; pix<numPixels; pix++)
    {
      Float_t pixelXCenter = (*fGeom)[pix].GetX();
      Float_t pixelYCenter = (*fGeom)[pix].GetY();
      Float_t dist = TMath::Sqrt((pixelXCenter-expectedStarXPos)*(pixelXCenter-expectedStarXPos)+
                          (pixelYCenter-expectedStarYPos)*(pixelYCenter-expectedStarYPos));
      
      if ((dist < ringofinterest))
        {
          fProjectionX.Fill((pixelXCenter-expectedStarXPos)*fGeom->GetConvMm2Deg(), (*fCurr)[pix]);
          fProjectionY.Fill((pixelYCenter-expectedStarYPos)*fGeom->GetConvMm2Deg(), (*fCurr)[pix]);
          //FIXME          fProfile.Fill((pixelXCenter-expectedStarXPos)*fGeom->GetConvMm2Deg(),(pixelYCenter-expectedStarYPos)*fGeom->GetConvMm2Deg(), (*fCurr)[pix]);
        }
    }
  
  
  //
  fvsTimeMeanX.Set(fNumEvents+1);
  fvsTimeMeanY.Set(fNumEvents+1);
  fvsTimeSigmaMinor.Set(fNumEvents+1);
  fvsTimeSigmaMajor.Set(fNumEvents+1);
  
  //  fTime.Set(fNumEvents+1);
  fEvent.Set(fNumEvents+1);

  fvsTimeMeanX[fNumEvents] = star->GetMeanX()*fGeom->GetConvMm2Deg();
  fvsTimeMeanY[fNumEvents] = star->GetMeanY()*fGeom->GetConvMm2Deg();
  fvsTimeSigmaMinor[fNumEvents] = star->GetSigmaMinorAxis()*fGeom->GetConvMm2Deg();
  fvsTimeSigmaMajor[fNumEvents] = star->GetSigmaMajorAxis()*fGeom->GetConvMm2Deg();

  //  fTime[fNumEvents] = ?;
  fEvent[fNumEvents] = fNumEvents;

  fNumEvents++;
  
//   fGraphMeanX.SetPoint(fNumEvents,fNumEvents,star->GetMeanX()*fGeom->GetConvMm2Deg());
//   fGraphMeanY.SetPoint(fNumEvents,fNumEvents,star->GetMeanY()*fGeom->GetConvMm2Deg());
//   fGraphSigmaMinor.SetPoint(fNumEvents,fNumEvents,star->GetSigmaMinorAxis()*fGeom->GetConvMm2Deg());
//   fGraphSigmaMajor.SetPoint(fNumEvents,fNumEvents,star->GetSigmaMajorAxis()*fGeom->GetConvMm2Deg());

  return kTRUE;
}

// --------------------------------------------------------------------------
//
//
Bool_t MHPSFFromStars::Finalize()
{
  *fLog << inf << GetName() << " Statistics: " << fNumEvents << " events" << endl;
  //Fit  the profile plot with a gaussian(+baseline)

  return kTRUE;
}

MStarLocalPos* MHPSFFromStars::SelectStar(MStarLocalCam* stars)
{
  TIter Next(stars->GetList());
  MStarLocalPos* star = NULL;
  MStarLocalPos* selectedStar = NULL;
  
  Float_t dist;
  
  if (fSelectedStarPos >= 0)
    for (Int_t i=0; i<=fSelectedStarPos; i++)
        star = (MStarLocalPos*)Next();
  else //Look for the closer star to the center
    {
      Float_t mindist = 600; //mm
      while ((star=(MStarLocalPos*)Next())) 
        {
          dist = TMath::Sqrt(star->GetMeanX()*star->GetMeanX() +
                             star->GetMeanY()*star->GetMeanY());
          if (dist < mindist)
            {
              selectedStar = star;
              mindist = dist;
            }
        }
      
      star = selectedStar;
      if (star == NULL)
          *fLog << warn << "Not found star closer to center" << endl;
      
    }
      
  return star;
}

static Double_t fitfunc(Double_t *x, Double_t *par)
{
   Double_t fitval = 
     par[0] + par[1]*TMath::Exp(-0.5*(x[0]-par[2])*(x[0]-par[2])/(par[3]*par[3]));

   return fitval;
}

// -----------------------------------------------------------------------------
// 
// Default draw:
//
//
void MHPSFFromStars::Draw(const Option_t *opt)
{

  TString option(opt);
  option.ToLower();

  TVirtualPad *pad = gPad ? gPad : MH::MakeDefCanvas(this);  
  pad->SetBorderMode(0);

  if (fNumEvents==0)
    {
      *fLog << err << GetName() << " no entries to be drawn" << endl;
      return;
    }
  
    

  if (option.Contains("mean"))
    {
      fGraphMeanX = new TGraph((Int_t)fNumEvents,fEvent.GetArray(),fvsTimeMeanX.GetArray());
      fGraphMeanX->SetName("GraphMeanX");
      fGraphMeanX->SetTitle("Star X position v.s. event ");
      fGraphMeanX->UseCurrentStyle();
      fGraphMeanX->GetXaxis()->SetTitle("Event [#]");
      fGraphMeanX->GetYaxis()->SetTitle("X [\\circ]");

      fGraphMeanY = new TGraph((Int_t)fNumEvents,fEvent.GetArray(),fvsTimeMeanY.GetArray());
      fGraphMeanY->SetName("GraphMeanY");
      fGraphMeanY->SetTitle("Star Y position v.s. event ");
      fGraphMeanY->UseCurrentStyle();
      fGraphMeanY->GetXaxis()->SetTitle("Event [#]");
      fGraphMeanY->GetYaxis()->SetTitle("Y [\\circ]");

      pad->Divide(2,2);

      pad->cd(1);
      fHistMeanX.DrawClone();
      pad->cd(2);
      fHistMeanY.DrawClone();
      pad->cd(3);
      fGraphMeanX->SetMarkerStyle(20);
      fGraphMeanX->SetMarkerSize(0.4);
      fGraphMeanX->Draw("AP");
      pad->cd(4);
      fGraphMeanY->SetMarkerStyle(20);
      fGraphMeanY->SetMarkerSize(0.4);
      fGraphMeanY->Draw("AP");
    }
  else if (option.Contains("sig"))
    {

      fGraphSigmaMinor = new TGraph((Int_t)fNumEvents,fEvent.GetArray(),fvsTimeSigmaMinor.GetArray());
      fGraphSigmaMinor->SetName("GraphSigmaMinor");
      fGraphSigmaMinor->SetTitle("Star X sigma v.s. event ");
      fGraphSigmaMinor->UseCurrentStyle();
      fGraphSigmaMinor->GetXaxis()->SetTitle("Event [#]");
      fGraphSigmaMinor->GetYaxis()->SetTitle("Sigma X [\\circ]");

      fGraphSigmaMajor = new TGraph((Int_t)fNumEvents,fEvent.GetArray(),fvsTimeSigmaMajor.GetArray());
      fGraphSigmaMajor->SetName("GraphSigmaMajor");
      fGraphSigmaMajor->SetTitle("Star Y sigma v.s. event ");
      fGraphSigmaMajor->UseCurrentStyle();
      fGraphSigmaMajor->GetXaxis()->SetTitle("Event [#]");
      fGraphSigmaMajor->GetYaxis()->SetTitle("Sigma Y [\\circ]");

      pad->Divide(2,2);

      pad->cd(1);
      fHistSigmaMinor.DrawClone();
      pad->cd(2);
      fHistSigmaMajor.DrawClone();
      pad->cd(3);
      fGraphSigmaMinor->SetMarkerStyle(21);
      fGraphSigmaMinor->SetMarkerSize(0.4);
//       fGraphSigmaMinor->SetMarkerColor(kBlue);
      fGraphSigmaMinor->Draw("AP");
      fGraphSigmaMajor->SetMarkerStyle(21);
      fGraphSigmaMajor->SetMarkerSize(0.4);
      pad->cd(4);
      fGraphSigmaMajor->Draw("AP");
    }
  else if (option.Contains("proj"))
  {
      pad->Divide(2,1);

      pad->cd(1);
// Creates a Root function based on function fitf above
      TF1 *funcX = new TF1("fitfuncX",fitfunc,-0.4,0.4,4);

      Float_t sigguess = fHistSigmaMinor.GetMean();
      Float_t max = fProjectionX.GetMaximum();
// Sets initial values and parameter names
      funcX->SetParNames("base","Max","Mean","Sigma");
      funcX->SetParameters(1.,max,0.,sigguess);

// Fit histogram in range defined by function
      fProjectionX.Fit("fitfuncX","QR");
      //      fProjectionX.DrawClone();

      pad->cd(2);
      TF1 *funcY = new TF1("fitfuncY",fitfunc,-0.4,0.4,4);

      sigguess = fHistSigmaMajor.GetMean();
      max = fProjectionY.GetMaximum();
// Sets initial values and parameter names
      funcY->SetParNames("base","Max","Mean","Sigma");
      funcY->SetParameters(1.,max,0.,sigguess);

// Fit histogram in range defined by function
      fProjectionY.Fit("fitfuncX","QR");
      fProjectionY.DrawClone();
  }
//   else if (option.Contains("prof"))
//   {
//       fProfile.DrawClone();
//   }
  else if (option.Contains("cam"))
    {
      pad->cd(1);

      TArrayF x(fNumEvents);
      TArrayF y(fNumEvents);
      for (UInt_t i=0; i<fNumEvents; i++)
        {
          x[i] = fvsTimeMeanX[i]/fGeom->GetConvMm2Deg();
          y[i] = fvsTimeMeanY[i]/fGeom->GetConvMm2Deg();
        }
      
      fGraphPath = new TGraph((Int_t)fNumEvents,x.GetArray(),y.GetArray());
      fGraphPath->SetName("GraphPath");
      fGraphPath->UseCurrentStyle();

      fCamera.DrawClone();
      fCamera.SetPrettyPalette();
      fGraphPath->Draw("P");
      fGraphPath->SetMarkerStyle(21);
      fGraphPath->SetMarkerSize(0.4);
    }
  else
    *fLog << err << GetName() << "::Draw Uknown option " << option << endl;
  
}

