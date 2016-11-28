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
! * in supporting documentation. It is provided "as is" without expressed
! * or implied warranty.
! *
!
!
!   Author(s): Javier López ,   4/2004 <mailto:jlopez@ifae.es>
!              Robert Wagner,   7/2004 <mailto:rwagner@mppmu.mpg.de>
!              Wolfgang Wittek, 8/2004 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
#include "MStarPos.h"

#include <TEllipse.h>
#include <TMarker.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MStarPos);

using namespace std;

MStarPos::MStarPos(const char *name, const char *title)
{

    fName  = name  ? name  : "MStarPos";
    fTitle = title ? title : "";
    
    Reset();
}

void MStarPos::Reset()
{

    //Expected position on camera
     fMagExp = 0.;
     fXExp   = 0.;
     fYExp   = 0.;

    //Ideal position on camera
     fMagIdeal = 0.;
     fXIdeal   = 0.;
     fYIdeal   = 0.;

    //Info from calculation
     fMagCalc    = 0.;
     fMaxCalc    = 0.;
     fMeanXCalc  = 0.;
     fMeanYCalc  = 0.;
     fSigmaXCalc = 0.;
     fSigmaYCalc = 0.;
     fCorrXYCalc = 0.;
     fXXErrCalc  = 0.;
     fXYErrCalc  = 0.;
     fYYErrCalc  = 0.;

    //Info from Gauss fit
     fMagFit    = 0.;
     fMaxFit    = 0.;
     fMeanXFit  = 0.;
     fMeanYFit  = 0.;
     fSigmaXFit = 0.;
     fSigmaYFit = 0.;
     fCorrXYFit = 0.;
     fXXErrFit  = 0.;
     fXYErrFit  = 0.;
     fYYErrFit  = 0.;

     fChiSquareFit = 0.;
     fNdofFit      = 1;

}

void MStarPos::SetExpValues(Float_t mag, Float_t x, Float_t y)
{
     fMagExp = mag;
     fXExp = x;
     fYExp = y;
}

void MStarPos::SetIdealValues(Float_t mag, Float_t x, Float_t y)
{
     fMagIdeal = mag;
     fXIdeal = x;
     fYIdeal = y;
}

void MStarPos::SetCalcValues(Float_t mag, Float_t max, Float_t x, Float_t y, 
               Float_t sigmaX,    Float_t sigmaY, Float_t correlation, 
	       Float_t xx,        Float_t xy,     Float_t yy)
{
     fMagCalc    = mag;
     fMaxCalc    = max;
     fMeanXCalc  = x;
     fMeanYCalc  = y;
     fSigmaXCalc = sigmaX;
     fSigmaYCalc = sigmaY;
     fCorrXYCalc = correlation;
     fXXErrCalc  = xx;
     fXYErrCalc  = xy;
     fYYErrCalc  = yy;
}


void MStarPos::SetFitValues(
               Float_t mag,       Float_t max,    Float_t x,    Float_t y, 
               Float_t sigmaX,    Float_t sigmaY, Float_t correlation, 
               Float_t xx,        Float_t xy,     Float_t yy,
               Float_t chiSquare, Int_t ndof)
{
     fMagFit    = mag;
     fMaxFit    = max;
     fMeanXFit  = x;
     fMeanYFit  = y;
     fSigmaXFit = sigmaX;
     fSigmaYFit = sigmaY;
     fCorrXYFit = correlation;
     fXXErrFit  = xx;
     fXYErrFit  = xy;
     fYYErrFit  = yy;

     fChiSquareFit = chiSquare;
     fNdofFit      = ndof;
}


// --------------------------------------------------------------------------
//
// Paint the ellipse corresponding to the parameters
//
void MStarPos::Paint(Option_t *opt)
{
  //Print a cross in the expected position
  TMarker mexp(fXExp, fYExp, 29);
  mexp.SetMarkerSize(3);
  mexp.SetMarkerColor(94);
  mexp.Paint(); 

  if (fSigmaXCalc>0. && fSigmaYCalc>0.)
    {
      TEllipse ecalc(fMeanXCalc, fMeanYCalc, fSigmaXCalc, fSigmaYCalc, 
                     0, 360, 0);
      ecalc.SetLineWidth(3);
      //ecalc.SetLineColor(kBlue);
      ecalc.SetLineColor(kMagenta);
      ecalc.Paint();
    }

  if (fSigmaXFit>0. && fSigmaYFit>0.)
    {
      //Print a cross in the fitted position
      //TMarker mFit(fMeanXFit, fMeanYFit, 3);
      //mFit.SetMarkerSize(3);
      //mFit.SetMarkerColor(1);
      //mFit.Paint(); 

      Double_t cxx = fSigmaXFit*fSigmaXFit;
      Double_t cyy = fSigmaYFit*fSigmaYFit;
      Double_t d   = cyy - cxx;
      Double_t cxy = fCorrXYFit * fSigmaXFit * fSigmaYFit;
      Double_t tandel;
      if (cxy != 0.0)
        tandel = ( d + sqrt(d*d + 4.0*cxy*cxy) ) / (2.0*cxy); 
      else
        tandel = 0.0;

      Double_t sindel = tandel / sqrt(1.0 + tandel*tandel);
      Double_t delta = TMath::ASin(sindel);

      Double_t major =   (cxx + 2.0*tandel*cxy + tandel*tandel*cyy)
	                / (1.0 + tandel*tandel);  

      Double_t minor =   (tandel*tandel*cxx - 2.0*tandel*cxy + cyy)
	                / (1.0 + tandel*tandel);  

      TEllipse efit(fMeanXFit, fMeanYFit, sqrt(major), sqrt(minor), 
                    0, 360,      delta*kRad2Deg);
      efit.SetLineWidth(3);
      //efit.SetLineColor(kMagenta);
      //efit.SetLineColor(kBlack);
      efit.Paint();
    }
}
  
void MStarPos::Print(Option_t *opt) const
{
  TString o = opt;

  if (o.Contains("name", TString::kIgnoreCase) || opt == NULL)
    {
      *fLog << inf << "Star Name: \"" << this->GetName() << "\"" << endl;
    }
      
  if (o.Contains("mag", TString::kIgnoreCase) || opt == NULL)
    {
  
      *fLog << inf << "Star magnitude:" << endl;
      *fLog << inf << " Ideal \t" << setw(4) << fMagIdeal << endl;
      *fLog << inf << " Expected \t" << setw(4) << fMagExp << endl;
      *fLog << inf << " Calcultated \t " << setw(4) << fMagCalc << endl;
      *fLog << inf << " Fitted \t " << setw(4) << fMagFit << endl;
    }
  
  if (o.Contains("max", TString::kIgnoreCase) || opt == NULL)
    {
      *fLog << inf << "Star Maximum:" << endl;
      *fLog << inf << " Calcultated \t " << setw(4) << fMaxCalc << " uA" 
            << endl;
      *fLog << inf << " Fitted \t " << setw(4) << fMaxFit << " uA" << endl;
    }
  
  if (o.Contains("pos", TString::kIgnoreCase) || opt == NULL)
    {
      *fLog << inf << "Star position:" << endl;
      *fLog << inf << " Ideal \t X " << setw(4) << fXIdeal 
            << " mm \tY " << setw(4) << fYIdeal << " mm" << endl;
      *fLog << inf << " Expected \t X " << setw(4) << fXExp 
            << " mm \tY " << setw(4) << fYExp << " mm" << endl;
      *fLog << inf << " Calcultated \t X " << setw(4) << fMeanXCalc 
            << " mm \tY " << setw(4) << fMeanYCalc << " mm" << endl;
      *fLog << inf << " Fitted \t X " << setw(4) << fMeanXFit 
            << " mm \tY " << setw(4) << fMeanYFit << " mm" << endl;
    }

  if (o.Contains("siz", TString::kIgnoreCase) || opt == NULL)
    {
      *fLog << inf << "Star size:" << endl;
      *fLog << inf << " Calcultated \t X " << setw(4) << fSigmaXCalc 
            << " mm \tY " << setw(4) << fSigmaYCalc << " mm \t correlation " 
            << setw(4) << fCorrXYCalc << endl;
      *fLog << inf << " Fitted \t X " << setw(4) << fSigmaXFit 
            << " mm \tY " << setw(4) << fSigmaYFit << " mm \t correlation " 
            << setw(4) << fCorrXYFit << endl;
    }

  if (o.Contains("chi", TString::kIgnoreCase) || opt == NULL)
    {
      *fLog << inf << "Star Fit Quality:" << endl;
      *fLog << inf << " ChiSquareFit/NdofFit \t " << setw(3) 
            << fChiSquareFit << "/" << fNdofFit << endl;
    }

  if (o.Contains("err", TString::kIgnoreCase) || opt == NULL)
    {
      *fLog << inf << "Error Matrix of (fMeanX,fMeanY) :" 
            << endl;
      *fLog << inf << " xxCalc,xyCalc,yyCalc \t " << setw(3) << fXXErrCalc 
            << ", " << fXYErrCalc << ", " << fYYErrCalc << endl;
      *fLog << inf << " xxFit,xyFit,yyFit \t " << setw(3) << fXXErrFit << ", " 
            << fXYErrFit << ", " << fYYErrFit << endl;
    }


}
//--------------------------------------------------------------------------









