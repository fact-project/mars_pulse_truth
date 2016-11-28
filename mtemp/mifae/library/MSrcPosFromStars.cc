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
!   Author(s): Roger Firpo   04/2004 <mailto:jlopez@ifae.es>
!   Author(s): Javier López  05/2004 <mailto:jlopez@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MSrcPosFromStars
//
// Task to set the position of the source using the positions of the stars
// in the field of view of the source
//
//  Output Containers:
//    MSrcPosCam
//
//////////////////////////////////////////////////////////////////////////////


#include "MSrcPosFromStars.h"

#include <TList.h>
#include <TH2F.h>
#include <TF2.h>
#include <TTimer.h>
#include <TString.h>
#include <TCanvas.h>

#include "MSrcPosCam.h"
#include "MStarLocalCam.h"
#include "MStarLocalPos.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

ClassImp(MSrcPosFromStars);

using namespace std;

static const TString gsDefName  = "MSrcPosFromStars";
static const TString gsDefTitle = "task to calculate the position of the source using the position of stars";

// -------------------------------------------------------------------------
//
// Default constructor. 
//
MSrcPosFromStars::MSrcPosFromStars(const char *name, const char *title)
    : fStars(NULL)
{
  fName  = name  ? name  : gsDefName.Data();
  fTitle = title ? title : gsDefTitle.Data();
  
  fDistances.Set(0);

}

// -------------------------------------------------------------------------
//
Int_t MSrcPosFromStars::PreProcess(MParList *pList)
{

  if(!MSrcPlace::PreProcess(pList))
    return kFALSE;
  
  fStars = (MStarLocalCam*)pList->FindObject(AddSerialNumber("MStarLocalCam"));
  if (!fStars)
    {
      *fLog << err << AddSerialNumber("MStarLocalCam") << " not found ... aborting" << endl;
      return kFALSE;
    }
  
  fNumStars = fDistances.GetSize();
  
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// 
static Bool_t HandleInput()
{
    TTimer timer("gSystem->ProcessEvents();", 50, kFALSE);
    while (1)
    {
        //
        // While reading the input process gui events asynchronously
        //
        timer.TurnOn();
        cout << "Type 'q' to exit, <return> to go on: " << endl;
        TString input;
        input = getchar();
        timer.TurnOff();

        if (input=="q\n")
            return kFALSE;

        if (input=="\n")
            return kTRUE;
    };

    return kFALSE;
}

Int_t MSrcPosFromStars::ComputeNewSrcPosition()
{

  if (fNumStars == 0)
    {
      if (fStars->GetNumStars() > 0)
        {

          //Look for the star closer to the center of the camera
          TIter Next(fStars->GetList());
          MStarLocalPos* star;
          Float_t mindist = 600; //mm
          UInt_t starnum = 0;
          Int_t select = -1;
          while ((star=(MStarLocalPos*)Next())) 
            {
              Float_t dist = TMath::Sqrt(star->GetMeanX()*star->GetMeanX() +
                                         star->GetMeanY()*star->GetMeanY());
              if (dist < mindist)
                {
                  mindist = dist;
                  select = starnum;
                }
              
              starnum++;
            }

          if (select < 0)
            {
              *fLog << err << "Not found star closer to center" << endl;
              return kFALSE;
            }
          
          MStarLocalPos& selecStar = (*fStars)[select];

          if (selecStar.GetChiSquareNdof() > 0. && selecStar.GetChiSquareNdof() < 10.)
            {
              
              Float_t selecStarPosX = selecStar.GetMeanX();
              Float_t selecStarPosY = selecStar.GetMeanY();
        
              GetOutputSrcPosCam()->SetXY(selecStarPosX,selecStarPosY);
            }
        }
    }
  else if (fStars->GetNumStars() >= fNumStars)
    {
      
      Float_t diameterInnerPixel = 30; //[mm]
      Float_t diameterOuterPixel = 60; //[mm]
      
      Double_t probability = 1.;

      // Resolution and computing time are proportional to bins^2
      const Int_t bins  = 200;
      Double_t max_x_mm = 600;
      Double_t min_x_mm = -max_x_mm;
      Double_t max_y_mm = max_x_mm;
      Double_t min_y_mm = -max_x_mm;
      
      // bins to mmrees
      const Double_t bin2mm = 2 * max_x_mm / bins;
      
      // First run, to find the maximum peak and define the area
      TH2F *hgrid = new TH2F("hgrid", "", bins, min_x_mm, max_x_mm, bins, min_y_mm, max_y_mm);
      
      for (Int_t ny = 1; ny <= bins; ny++)
        for (Int_t nx = 1; nx <= bins; nx++)
          hgrid->SetBinContent(nx, ny, 1.);

      for (UInt_t numstar = 0; numstar < fNumStars; numstar++)
        {
          probability = 1;
          
          MStarLocalPos& star  = (*fStars)[numstar];
          
          if (star.GetChiSquareNdof() > 0. && star.GetChiSquareNdof() < 10.)
            {
              
              Float_t starPosX  = star.GetMeanX();
              Float_t starPosY  = star.GetMeanY();
              Float_t starDist  = Dist(0.,0.,starPosX,starPosY);
              Float_t starSigma = (starDist<350.?diameterInnerPixel:diameterOuterPixel);
              
//               *fLog << dbg << "Star[" << numstar << "] pos (" << starPosX << "," << starPosY << ") dist " << starDist << " size " << starSigma << endl;
              
              if (starSigma != 0)
                {
                  for (Int_t ny = 1; ny < bins + 1; ny++)
                    {
                      for (Int_t nx = 0; nx < bins + 1; nx++)
                        {
                          Float_t dist = Dist(min_x_mm + nx * bin2mm, starPosX, min_y_mm + ny * bin2mm, starPosY);
                          Float_t prob = Prob(dist, fDistances[numstar], starSigma);
                          
//                           if ( prob > 0.8)
//                             *fLog << dbg << " x " << min_x_mm + nx * bin2mm << " y " << min_y_mm + ny * bin2mm << " dist " << dist << " stardist " << fDistances[numstar] << " prob " << prob << endl;

                          probability = hgrid->GetBinContent(nx, ny)*prob;
                          hgrid->SetBinContent(nx, ny, probability);

                        }
                    }
                }
              else probability *= 1;
              
            }
        }
      
      // Finding the peak
      Double_t peak[2] = {0,0};
      Double_t peak_value = 0;
      
      for (Int_t ny = 0; ny < bins + 1; ny++)
        {
          for (Int_t nx = 0; nx < bins + 1; nx++)
            {
              if (hgrid->GetBinContent(nx, ny) > peak_value)
                {
                  peak_value = hgrid->GetBinContent(nx, ny);
                  peak[0] = min_x_mm + nx * bin2mm;
                  peak[1] = min_y_mm + ny * bin2mm;
                }
            }
        }
      
      *fLog << dbg << "The peak is at (x, y) = (" << peak[0] << ", " << peak[1] << ") mm" << endl;
      
      
//       TCanvas c1;
//       hgrid->Draw("lego");
//       if(!HandleInput())
//         exit(1);
      
      
      // Here we define a small area surrounding the peak to avoid wasting time and resolution anywhere else
      
      min_x_mm = peak[0] - diameterInnerPixel;
      max_x_mm = peak[0] + diameterInnerPixel;
      min_y_mm = peak[1] - diameterInnerPixel;
      max_y_mm = peak[1] + diameterInnerPixel;
      
      const Double_t xbin2mm = (max_x_mm - min_x_mm) / bins;
      const Double_t ybin2mm = (max_y_mm - min_y_mm) / bins;
      
      // Other run centered in the peak for more precision
      TH2F *hagrid = new TH2F("hagrid", "", bins, min_x_mm, max_x_mm, bins, min_y_mm, max_y_mm);

      for (Int_t ny = 1; ny <= bins; ny++)
        for (Int_t nx = 1; nx <= bins; nx++)
          hagrid->SetBinContent(nx, ny, 1.);
          
      
      for (UInt_t numstar = 0; numstar < fNumStars; numstar++)
        {
          probability = 1;
          
          MStarLocalPos& star  = (*fStars)[numstar];
          
          if (star.GetChiSquareNdof() > 0. && star.GetChiSquareNdof() < 10.)
            {
              
              Float_t starPosX  = star.GetMeanX();
              Float_t starPosY  = star.GetMeanY();
              Float_t starDist  = Dist(0.,0.,starPosX,starPosY);
              Float_t starSigma = (starDist<350.?diameterInnerPixel:diameterOuterPixel);
              
              if (starSigma != 0)
                {
                  for (Int_t ny = 0; ny < bins; ny++)
                    {
                      for (Int_t nx = 0; nx < bins; nx++)
                        {
                          Float_t prob = Prob(Dist(min_x_mm + nx * xbin2mm, starPosX, min_y_mm + ny * ybin2mm, starPosY), fDistances[numstar], starSigma);
                          
                          probability = hagrid->GetBinContent(nx, ny)*prob;
                          hagrid->SetBinContent(nx, ny, probability);
                        }
                    }
                }
              else probability *= 1;                          
              
            }
        }
      
      // This time we fit the histogram with a 2D gaussian
      // Although we don't expect our function to be gaussian...
      TF2 *gauss2d = new TF2("gauss2d","[0]*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))*exp(-(y-[3])*(y-[3])/(2*[4]*[4]))", min_x_mm, max_x_mm, min_y_mm, max_y_mm);
      
      gauss2d->SetParName(0,"f0");
      gauss2d->SetParName(1,"meanx");
      gauss2d->SetParName(2,"sigmax");
      gauss2d->SetParName(3,"meany");
      gauss2d->SetParName(4,"sigmay");
      
      gauss2d->SetParameter(0,10);
      gauss2d->SetParameter(1,peak[0]);
      gauss2d->SetParameter(2,0.002);
      gauss2d->SetParameter(3,peak[1]);
      gauss2d->SetParameter(4,0.002);
      
      GetOutputSrcPosCam()->SetXY(gauss2d->GetParameter(1), gauss2d->GetParameter(3));
      
      delete hgrid;
      delete hagrid;
    }
  

  return kTRUE;
}

