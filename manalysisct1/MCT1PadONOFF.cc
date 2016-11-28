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
!   Author(s): Wolfgang Wittek, 06/2003 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MCT1PadONOFF
//
//  This task applies padding such that for a given pixel and for a given
//  Theta bin the resulting distribution of the pedestal sigma is identical
//  to the distributions given by fHSigmaPixTheta and fHDiffPixTheta.
//
//  The number of photons, its error and the pedestal sigmas are altered.
//  On average, the number of photons added is zero.
//
//  The formulas used can be found in Thomas Schweizer's Thesis,
//                                    Section 2.2.1
//
//  There are 2 options for the padding :
//
//  1) fPadFlag = 1 :
//     Generate first a Sigmabar using the 2D-histogram Sigmabar vs. Theta
//     (fHSigmaTheta). Then generate a pedestal sigma for each pixel using
//     the 3D-histogram Theta, pixel no., Sigma^2-Sigmabar^2
//     (fHDiffPixTheta).
//
//     This is the preferred option as it takes into account the
//     correlations between the Sigma of a pixel and Sigmabar.
//
//  2) fPadFlag = 2 :
//     Generate a pedestal sigma for each pixel using the 3D-histogram
//     Theta, pixel no., Sigma (fHSigmaPixTheta).
//
//
//  The padding has to be done before the image cleaning because the
//  image cleaning depends on the pedestal sigmas.
//
//  For random numbers gRandom is used.
//
//  This implementation has been tested for CT1 data. For MAGIC some
//  modifications are necessary.
//
/////////////////////////////////////////////////////////////////////////////
#include "MCT1PadONOFF.h"

#include <math.h>
#include <stdio.h>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <TFile.h>

#include "MBinning.h"
#include "MSigmabar.h"
#include "MMcEvt.hxx"
#include "MLog.h"
#include "MLogManip.h"
#include "MParList.h"
#include "MGeomCam.h"

#include "MCerPhotPix.h"
#include "MCerPhotEvt.h"

#include "MPedPhotCam.h"
#include "MPedPhotPix.h"

#include "MBlindPixels.h"

#include "MRead.h"
#include "MFilterList.h"
#include "MTaskList.h"
#include "MBlindPixelCalc.h"
#include "MHBlindPixels.h"
#include "MFillH.h"
#include "MHSigmaTheta.h"
#include "MEvtLoop.h"

ClassImp(MCT1PadONOFF);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCT1PadONOFF::MCT1PadONOFF(const char *name, const char *title) 
{
  fName  = name  ? name  : "MCT1PadONOFF";
  fTitle = title ? title : "Task for the ON-OFF padding";

  fPadFlag = 1;
  *fLog << "MCT1PadONOFF: fPadFlag = " << fPadFlag << endl;

  fType = "";

  fHSigmaTheta       = NULL;
  fHSigmaThetaON     = NULL;
  fHSigmaThetaOFF    = NULL;

  fHSigmaPixTheta    = NULL;
  fHSigmaPixThetaON  = NULL;
  fHSigmaPixThetaOFF = NULL;

  fHDiffPixTheta     = NULL;
  fHDiffPixThetaON   = NULL;
  fHDiffPixThetaOFF  = NULL;

  fHgON  = NULL;
  fHgOFF = NULL;

  fHBlindPixIdTheta = NULL;
  fHBlindPixNTheta  = NULL;

  fHSigmaPedestal = NULL;
  fHPhotons       = NULL;
  fHNSB           = NULL;
}

// --------------------------------------------------------------------------
//
// Destructor. 
//
MCT1PadONOFF::~MCT1PadONOFF()
{
  if (fHBlindPixIdTheta   != NULL) delete fHBlindPixIdTheta;
  if (fHBlindPixNTheta    != NULL) delete fHBlindPixNTheta;

  if (fHSigmaTheta    != NULL) delete fHSigmaTheta;
  if (fHSigmaPixTheta != NULL) delete fHSigmaPixTheta;
  if (fHDiffPixTheta  != NULL) delete fHDiffPixTheta;

  if (fHSigmaPedestal != NULL) delete fHSigmaPedestal;
  if (fHPhotons       != NULL) delete fHPhotons;
  if (fHNSB           != NULL) delete fHNSB;

  if (fInfile         != NULL) delete fInfile;
}

// --------------------------------------------------------------------------
//
// Merge the distributions of ON and OFF data
//
//   fHSigmaTheta    2D-histogram  (Theta, sigmabar)
//   fHSigmaPixTheta 3D-hiostogram (Theta, pixel, sigma)
//   fHDiffPixTheta  3D-histogram  (Theta, pixel, sigma^2-sigmabar^2)
//   fHBlindPixIdTheta 2D-histogram  (Theta, blind pixel Id)
//   fHBlindPixNTheta  2D-histogram  (Theta, no.of blind pixels )
//
// and define the target distributions for the padding
//
Bool_t MCT1PadONOFF::MergeHistograms(TH2D *sigthon,     TH2D *sigthoff,
                                  TH3D *sigpixthon,  TH3D *sigpixthoff,
                                  TH3D *diffpixthon, TH3D *diffpixthoff,
                                  TH2D *blindidthon, TH2D *blindidthoff,
                                  TH2D *blindnthon,  TH2D *blindnthoff)
{
  *fLog << "----------------------------------------------------------------------------------" << endl;
  *fLog << "Merge the ON and OFF histograms to obtain the target distributions for the padding"
        << endl;

  //-------------------------------------------------------------
  // merge the distributions of ON and OFF events
  // to obtain the target distribution fHSigmaTheta
  //

  Double_t eps = 1.e-10;

  fHSigmaTheta = new TH2D( (TH2D&)*sigthon );
  fHSigmaTheta->SetNameTitle("2D-ThetaSigmabar", "2D-ThetaSigmabar (target)");

  // get binning for fHgON and fHgOFF from sigthon
  // binning in Theta
  TAxis *ax = sigthon->GetXaxis();
  Int_t nbinstheta = ax->GetNbins();
  TArrayD edgesx;
  edgesx.Set(nbinstheta+1);
  for (Int_t i=0; i<=nbinstheta; i++)
  {
    edgesx[i] = ax->GetBinLowEdge(i+1);
    // *fLog << "i, theta low edge = " << i << ",  " << edgesx[i] << endl; 
  }
  MBinning binth;
  binth.SetEdges(edgesx);

  // binning in sigmabar
  TAxis *ay = sigthon->GetYaxis();
  Int_t nbinssig = ay->GetNbins();
  TArrayD edgesy;
  edgesy.Set(nbinssig+1);
  for (Int_t i=0; i<=nbinssig; i++)
  {
    edgesy[i] = ay->GetBinLowEdge(i+1); 
    // *fLog << "i, sigmabar low edge = " << i << ",  " << edgesy[i] << endl; 
  }
  MBinning binsg;
  binsg.SetEdges(edgesy);


  fHgON = new TH3D;
  MH::SetBinning(fHgON, &binth, &binsg, &binsg);
  fHgON->SetNameTitle("3D-PaddingMatrixON", "3D-PadMatrixThetaON");

  fHgOFF = new TH3D;
  MH::SetBinning(fHgOFF, &binth, &binsg, &binsg);
  fHgOFF->SetNameTitle("3D-PaddingMatrixOFF", "3D-PadMatrixThetaOFF");

  //............   loop over Theta bins   ...........................

  // hista is the normalized 1D histogram of sigmabar for ON data
  // histb is the normalized 1D histogram of sigmabar for OFF data
  // histc is the difference ON-OFF

  // at the beginning, histap is a copy of hista
  // at the end, it will be the 1D histogram for ON data after padding

  // at the beginning, histbp is a copy of histb
  // at the end, it will be the 1D histogram for OFF data after padding

  // at the beginning, histcp is a copy of histc
  // at the end, it should be zero

  *fLog << "MCT1PadONOFF::MergeHistograms; bins of Theta, Sigmabarold, Sigmabarnew, fraction of events to be padded" << endl;
  for (Int_t l=1; l<=nbinstheta; l++)
  {
    TH1D *hista  = sigthon->ProjectionY("sigon_y", l, l, "");
    Stat_t suma = hista->Integral();
    hista->Scale(1./suma);

    TH1D *histap  = new TH1D( (const TH1D&)*hista );

    TH1D *histb  = sigthoff->ProjectionY("sigoff_y", l, l, "");
    Stat_t sumb = histb->Integral();
    histb->Scale(1./sumb);

    TH1D *histbp  = new TH1D( (const TH1D&)*histb );

    TH1D *histc   = new TH1D( (const TH1D&)*hista );
    histc->Add(histb, -1.0);

    TH1D *histcp  = new TH1D( (const TH1D&)*histc );    


  // calculate matrix g

  // fHg[k][j] (if <0.0) tells how many ON events in bin k should be padded
  //              from sigma_k to sigma_j


  // fHg[k][j] (if >0.0) tells how many OFF events in bin k should be padded
  //              from sigma_k to sigma_j

  //--------   start j loop   ------------------------------------------------
  // loop over bins in histc, starting from the end
  Double_t v, s, w, t, x, u, a, b, c, arg;

  for (Int_t j=nbinssig; j >= 1; j--)
  {
    v = histcp->GetBinContent(j);
    if ( fabs(v) < eps ) continue;
    if (v >= 0.0) 
      s = 1.0;
    else
      s = -1.0;

    //................   start k loop   ......................................
    // look for a bin k which may compensate the content of bin j
    for (Int_t k=j-1; k >= 1; k--)
    {
      w = histcp->GetBinContent(k);
      if ( fabs(w) < eps ) continue;
      if (w >= 0.0) 
        t = 1.0;
      else
        t = -1.0;

      if (s==t) continue;

      x = v + w;
      if (x >= 0.0) 
        u = 1.0;
      else
        u = -1.0;

      if (u == s)
      {
        arg = -w;

        if (arg <=0.0)
	{
          fHgON->SetBinContent (l, k, j, -arg);
          fHgOFF->SetBinContent(l, k, j,  0.0);
	}
        else
	{
          fHgON->SetBinContent (l, k, j,  0.0);
          fHgOFF->SetBinContent(l, k, j,  arg);
	}


        *fLog << "l, k, j, arg = " << l << ",  " << k << ",  " << j 
              << ",  " << arg << endl;

	//        g(k-1, j-1)   = arg;
        //cout << "k-1, j-1, arg = " << k-1 << ",  " << j-1 << ",  " 
        //     << arg << endl;

        //......................................
        // this is for checking the procedure
        if (arg < 0.0)
        {
          a = histap->GetBinContent(k);
          histap->SetBinContent(k, a+arg);
          a = histap->GetBinContent(j);
          histap->SetBinContent(j, a-arg);
        }
        else
        {
          b = histbp->GetBinContent(k);
          histbp->SetBinContent(k, b-arg);
          b = histbp->GetBinContent(j);
          histbp->SetBinContent(j, b+arg);
        }
        //......................................

        histcp->SetBinContent(k, 0.0);
        histcp->SetBinContent(j,   x);

        //......................................
        // redefine v 
        v = histcp->GetBinContent(j);
        if ( fabs(v) < eps ) break;
        if (v >= 0.0) 
          s = 1.0;
        else
          s = -1.0;
        //......................................
       
        continue;
      }

      arg = v;

      if (arg <=0.0)
      {
        fHgON->SetBinContent (l, k, j, -arg);
        fHgOFF->SetBinContent(l, k, j,  0.0);
      }
      else
      {
        fHgON->SetBinContent (l, k, j,  0.0);
        fHgOFF->SetBinContent(l, k, j,  arg);
      }

      *fLog << "l, k, j, arg = " << l << ",  " << k << ",  " << j 
            << ",  " << arg << endl;

      //g(k-1, j-1) = arg;
      //cout << "k-1, j-1, arg = " << k-1 << ",  " << j-1 << ",  " 
      //     << arg << endl;

      //......................................
      // this is for checking the procedure
      if (arg < 0.0)
      {
        a = histap->GetBinContent(k);
        histap->SetBinContent(k, a+arg);
        a = histap->GetBinContent(j);
        histap->SetBinContent(j, a-arg);
      }
      else
      {
        b = histbp->GetBinContent(k);

        histbp->SetBinContent(k, b-arg);
        b = histbp->GetBinContent(j);
        histbp->SetBinContent(j, b+arg);
      }
      //......................................

      histcp->SetBinContent(k,   x);
      histcp->SetBinContent(j, 0.0);

      break;
    }
    //................   end k loop   ......................................
  } 
  //--------   end j loop   ------------------------------------------------

  // check results for this Theta bin
  for (Int_t j=1; j<=nbinssig; j++)
  {
    a = histap->GetBinContent(j);
    b = histbp->GetBinContent(j);
    c = histcp->GetBinContent(j);

    if( fabs(a-b)>3.0*eps  ||  fabs(c)>3.0*eps )
      *fLog << "MCT1PadONOFF::Read; inconsistency in results; a, b, c = "
            << a << ",  " << b << ",  " << c << endl;
  }
    

  // fill target distribution SigmaTheta
  // for this Theta bin
  //
  for (Int_t j=1; j<=nbinssig; j++)
  {
    a = histap->GetBinContent(j);
    fHSigmaTheta->SetBinContent(l, j, a);
  }

  delete hista;
  delete histb;
  delete histc;

  delete histap;
  delete histbp;
  delete histcp;
  }
  //............   end of loop over Theta bins   ....................

  
  // target distributions for MC
  //        SigmaPixTheta and DiffPixTheta
  //        BlindPixIdTheta and BlindPixNTheta     
  // are calculated as averages of the ON and OFF distributions

  fHSigmaThetaON = sigthon;
  fHSigmaThetaON->SetNameTitle("2D-ThetaSigmabarON", "2D-ThetaSigmabarON (target)");

  fHSigmaThetaOFF = sigthoff;
  fHSigmaThetaOFF->SetNameTitle("2D-ThetaSigmabarOFF", "2D-ThetaSigmabarOFF (target)");


  fHBlindPixNTheta = new TH2D( (TH2D&)*blindnthon );
  fHBlindPixNTheta->SetNameTitle("2D-ThetaBlindN", "2D-ThetaBlindN (target)");

  fHBlindPixIdTheta = new TH2D( (TH2D&)*blindidthon );
  fHBlindPixIdTheta->SetNameTitle("2D-ThetaBlindId", "2D-ThetaBlindId (target)");

  fHSigmaPixTheta = new TH3D( (TH3D&)*sigpixthon );
  fHSigmaPixTheta->SetNameTitle("3D-ThetaPixSigma", "3D-ThetaPixSigma (target)");

  fHSigmaPixThetaON = sigpixthon;
  fHSigmaPixThetaON->SetNameTitle("3D-ThetaPixSigmaON", "3D-ThetaPixSigmaON (target)");

  fHSigmaPixThetaOFF = sigpixthoff;
  fHSigmaPixThetaOFF->SetNameTitle("3D-ThetaPixSigmaOFF", "3D-ThetaPixSigmaOFF (target)");

  fHDiffPixTheta = new TH3D( (TH3D&)*diffpixthon );
  fHDiffPixTheta->SetNameTitle("3D-ThetaPixDiff", "3D-ThetaPixDiff (target)");

  fHDiffPixThetaON = diffpixthon;
  fHDiffPixThetaON->SetNameTitle("3D-ThetaPixDiffON", "3D-ThetaPixDiffON (target)");

  fHDiffPixThetaOFF = diffpixthoff;
  fHDiffPixThetaOFF->SetNameTitle("3D-ThetaPixDiffOFF", "3D-ThetaPixDiffOFF (target)");


  for (Int_t j=1; j<=nbinstheta; j++)
  {
    // fraction of ON/OFF events to be padded to a sigmabar 
    // of the OFF/ON events : fracON, fracOFF

    Double_t fracON  = fHgON->Integral (j, j, 1, nbinssig, 1, nbinssig, "");
    Double_t fracOFF = fHgOFF->Integral(j, j, 1, nbinssig, 1, nbinssig, "");

    TAxis *ax;
    TAxis *ay;
    TAxis *az;

    Double_t entON;
    Double_t entOFF;

    Double_t normON;
    Double_t normOFF;

    // set ranges for 2D-projections of 3D-histograms
    ax = sigpixthon->GetXaxis();
    ax->SetRange(j, j);
    ax = sigpixthoff->GetXaxis();
    ax->SetRange(j, j);

    ax = diffpixthon->GetXaxis();
    ax->SetRange(j, j);
    ax = diffpixthoff->GetXaxis();
    ax->SetRange(j, j);

    TH2D *hist;
    TH2D *histOFF;

    TH1D *hist1;
    TH1D *hist1OFF;

    // weights for ON and OFF distrubtions when
    // calculating the weighted average
    Double_t facON  = 0.5 * (1. - fracON + fracOFF);
    Double_t facOFF = 0.5 * (1. + fracON - fracOFF);
  
    *fLog << fracON << ",  " << fracOFF << ",  "
          << facON  << ",  " << facOFF  << endl; 
    //-------------------------------------------
    ay = blindnthon->GetYaxis();
    Int_t nbinsn = ay->GetNbins();

    hist1    = (TH1D*)blindnthon->ProjectionY ("", j, j, "");
    hist1->SetName("dummy");
    hist1OFF = (TH1D*)blindnthoff->ProjectionY("", j, j, "");

    // number of events in this theta bin
    entON  = hist1->Integral();
    entOFF = hist1OFF->Integral();

    *fLog << "BlindPixNTheta : j, entON, entOFF = " << j << ",  " 
          << entON  << ",  " << entOFF  << endl;

    normON  = entON<=0.0  ? 0.0 : 1.0/entON;
    normOFF = entOFF<=0.0 ? 0.0 : 1.0/entOFF;

    hist1->Scale(normON);
    hist1OFF->Scale(normOFF);

    // weighted average of ON and OFF distributions
    hist1->Scale(facON);
    hist1->Add(hist1OFF, facOFF); 

    for (Int_t k=1; k<=nbinsn; k++)
      {
        Double_t cont = hist1->GetBinContent(k);
        fHBlindPixNTheta->SetBinContent(j, k, cont);  
      }

    delete hist1;
    delete hist1OFF;

    //-------------------------------------------
    ay = blindidthon->GetYaxis();
    Int_t nbinsid = ay->GetNbins();

    hist1    = (TH1D*)blindidthon->ProjectionY ("", j, j, "");
    hist1->SetName("dummy");
    hist1OFF = (TH1D*)blindidthoff->ProjectionY("", j, j, "");

    hist1->Scale(normON);
    hist1OFF->Scale(normOFF);

    // weighted average of ON and OFF distributions
    hist1->Scale(facON);
    hist1->Add(hist1OFF, facOFF); 

    for (Int_t k=1; k<=nbinsid; k++)
      {
        Double_t cont = hist1->GetBinContent(k);
        fHBlindPixIdTheta->SetBinContent(j, k, cont);  
      }

    delete hist1;
    delete hist1OFF;

    //-------------------------------------------
    ay = sigpixthon->GetYaxis();
    Int_t nbinspix = ay->GetNbins();

    az = sigpixthon->GetZaxis();
    Int_t nbinssigma = az->GetNbins();

    hist    = (TH2D*)sigpixthon->Project3D("zy");
    hist->SetName("dummy");
    histOFF = (TH2D*)sigpixthoff->Project3D("zy");

    hist->Scale(normON);
    histOFF->Scale(normOFF);

    // weighted average of ON and OFF distributions

    hist->Scale(facON);
    hist->Add(histOFF, facOFF); 

    for (Int_t k=1; k<=nbinspix; k++)
      for (Int_t l=1; l<=nbinssigma; l++)
      {
        Double_t cont = hist->GetBinContent(k,l);
        fHSigmaPixTheta->SetBinContent(j, k, l, cont);  
      }

    delete hist;
    delete histOFF;

    //-------------------------------------------
    ay = diffpixthon->GetYaxis();
    Int_t nbinspixel = ay->GetNbins();

    az = diffpixthon->GetZaxis();
    Int_t nbinsdiff = az->GetNbins();

    hist    = (TH2D*)diffpixthon->Project3D("zy");
    hist->SetName("dummy");
    histOFF = (TH2D*)diffpixthoff->Project3D("zy");

    hist->Scale(normON);
    histOFF->Scale(normOFF);

    // weighted average of ON and OFF distributions
    hist->Scale(facON);
    hist->Add(histOFF, facOFF); 

    for (Int_t k=1; k<=nbinspixel; k++)
      for (Int_t l=1; l<=nbinsdiff; l++)
      {
        Double_t cont = hist->GetBinContent(k,l);
        fHDiffPixTheta->SetBinContent(j, k, l, cont);  
      }

    delete hist;
    delete histOFF;

    //-------------------------------------------
  }


  *fLog << "The target distributions for the padding have been created" 
        << endl;
  *fLog << "----------------------------------------------------------" 
        << endl;
  //--------------------------------------------

  fHSigmaTheta->SetDirectory(NULL);
  fHSigmaThetaON->SetDirectory(NULL);
  fHSigmaThetaOFF->SetDirectory(NULL);

  fHSigmaPixTheta->SetDirectory(NULL);
  fHSigmaPixThetaON->SetDirectory(NULL);
  fHSigmaPixThetaOFF->SetDirectory(NULL);

  fHDiffPixTheta->SetDirectory(NULL);
  fHDiffPixThetaON->SetDirectory(NULL);
  fHDiffPixThetaOFF->SetDirectory(NULL);

  fHBlindPixIdTheta->SetDirectory(NULL);
  fHBlindPixNTheta->SetDirectory(NULL);


  fHgON->SetDirectory(NULL);
  fHgOFF->SetDirectory(NULL);


  return kTRUE;
}


// --------------------------------------------------------------------------
//
// Read target distributions from a file
//
//
Bool_t MCT1PadONOFF::ReadTargetDist(const char* namefilein)
{
  *fLog << "Read padding histograms from file " << namefilein << endl;

  fInfile = new TFile(namefilein);
  fInfile->ls();

    //------------------------------------

      fHSigmaTheta = 
      (TH2D*) gROOT->FindObject("2D-ThetaSigmabar");
      if (!fHSigmaTheta)
	{
          *fLog << "Object '2D-ThetaSigmabar' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '2D-ThetaSigmabar' was read in" << endl;

      fHSigmaThetaON = 
      (TH2D*) gROOT->FindObject("2D-ThetaSigmabarON");
      if (!fHSigmaThetaON)
	{
          *fLog << "Object '2D-ThetaSigmabarON' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '2D-ThetaSigmabarON' was read in" << endl;

      fHSigmaThetaOFF = 
      (TH2D*) gROOT->FindObject("2D-ThetaSigmabarOFF");
      if (!fHSigmaThetaOFF)
	{
          *fLog << "Object '2D-ThetaSigmabarOFF' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '2D-ThetaSigmabarOFF' was read in" << endl;

      fHSigmaPixTheta = 
      (TH3D*) gROOT->FindObject("3D-ThetaPixSigma");
      if (!fHSigmaPixTheta)
	{
          *fLog << "Object '3D-ThetaPixSigma' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '3D-ThetaPixSigma' was read in" << endl;

      fHSigmaPixThetaON = 
      (TH3D*) gROOT->FindObject("3D-ThetaPixSigmaON");
      if (!fHSigmaPixThetaON)
	{
          *fLog << "Object '3D-ThetaPixSigmaON' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '3D-ThetaPixSigmaON' was read in" << endl;

      fHSigmaPixThetaOFF = 
      (TH3D*) gROOT->FindObject("3D-ThetaPixSigmaOFF");
      if (!fHSigmaPixThetaOFF)
	{
          *fLog << "Object '3D-ThetaPixSigmaOFF' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '3D-ThetaPixSigmaOFF' was read in" << endl;

      fHDiffPixTheta = 
      (TH3D*) gROOT->FindObject("3D-ThetaPixDiff");
      if (!fHDiffPixTheta)
	{
          *fLog << "Object '3D-ThetaPixDiff' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '3D-ThetaPixDiff' was read in" << endl;

      fHDiffPixThetaON = 
      (TH3D*) gROOT->FindObject("3D-ThetaPixDiffON");
      if (!fHDiffPixThetaON)
	{
          *fLog << "Object '3D-ThetaPixDiffON' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '3D-ThetaPixDiffON' was read in" << endl;

      fHDiffPixThetaOFF = 
      (TH3D*) gROOT->FindObject("3D-ThetaPixDiffOFF");
      if (!fHDiffPixThetaOFF)
	{
          *fLog << "Object '3D-ThetaPixDiffOFF' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '3D-ThetaPixDiffOFF' was read in" << endl;

      fHgON = 
      (TH3D*) gROOT->FindObject("3D-PaddingMatrixON");
      if (!fHgON)
	{
          *fLog << "Object '3D-PaddingMatrixON' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '3D-PaddingMatrixON' was read in" << endl;

      fHgOFF = 
      (TH3D*) gROOT->FindObject("3D-PaddingMatrixOFF");
      if (!fHgOFF)
	{
          *fLog << "Object '3D-PaddingMatrixOFF' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '3D-PaddingMatrixOFF' was read in" << endl;


      fHBlindPixIdTheta = 
      (TH2D*) gROOT->FindObject("2D-ThetaBlindId");
      if (!fHBlindPixIdTheta)
	{
          *fLog << "Object '2D-ThetaBlindId' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '2D-ThetaBlindId' was read in" << endl;

      fHBlindPixNTheta = 
      (TH2D*) gROOT->FindObject("2D-ThetaBlindN");
      if (!fHBlindPixNTheta)
	{
          *fLog << "Object '2D-ThetaBlindN' not found on root file" << endl;
          return kFALSE;
	}
      *fLog << "Object '2D-ThetaBlindN' was read in" << endl;


    //------------------------------------

  return kTRUE;
}


// --------------------------------------------------------------------------
//
// Write target distributions onto a file
//
//
Bool_t MCT1PadONOFF::WriteTargetDist(const char* namefileout)
{
  *fLog << "Write padding histograms onto file " << namefileout << endl;

  TFile outfile(namefileout, "RECREATE");

  fHBlindPixNTheta->Write();
  fHBlindPixIdTheta->Write();

  fHSigmaTheta->Write();
  fHSigmaThetaON->Write();
  fHSigmaThetaOFF->Write();

  fHSigmaPixTheta->Write();
  fHSigmaPixThetaON->Write();
  fHSigmaPixThetaOFF->Write();

  fHDiffPixTheta->Write();
  fHDiffPixThetaON->Write();
  fHDiffPixThetaOFF->Write();

  fHgON->Write();
  fHgOFF->Write();

  *fLog << "MCT1PadONOFF::WriteTargetDist; target histograms written onto file "
        << namefileout << endl;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set type of data to be padded
//
//     this is not necessary if the type of the data can be recognized
//     directly from the input
//
//
void MCT1PadONOFF::SetDataType(const char* type)
{
  fType = type;
  *fLog << "MCT1PadONOFF::SetDataType(); type of data to be padded : " 
        << fType << endl; 

  return;
}


// --------------------------------------------------------------------------
//
// Set the option for the padding
//
//  There are 2 options for the padding :
//
//  1) fPadFlag = 1 :
//     Generate first a Sigmabar using the 2D-histogram Sigmabar vs. Theta
//     (fHSigmaTheta). Then generate a pedestal sigma for each pixel using
//     the 3D-histogram Theta, pixel no., Sigma^2-Sigmabar^2
//     (fHDiffPixTheta).
//
//     This is the preferred option as it takes into account the
//     correlations between the Sigma of a pixel and Sigmabar.
//
//  2) fPadFlag = 2 :
//     Generate a pedestal sigma for each pixel using the 3D-histogram
//     Theta, pixel no., Sigma (fHSigmaPixTheta).
//
void MCT1PadONOFF::SetPadFlag(Int_t padflag)
{
  fPadFlag = padflag;
  *fLog << "MCT1PadONOFF::SetPadFlag(); choose option " << fPadFlag << endl; 
}

// --------------------------------------------------------------------------
//
//  Set the pointers and prepare the histograms
//
Int_t MCT1PadONOFF::PreProcess(MParList *pList)
{
  if ( !fHSigmaTheta       ||  !fHSigmaThetaON    ||  !fHSigmaThetaOFF    ||  
       !fHSigmaPixTheta    ||  !fHSigmaPixThetaON ||  !fHSigmaPixThetaOFF ||
       !fHDiffPixTheta     ||  !fHDiffPixThetaON  ||  !fHDiffPixThetaOFF  ||
       !fHBlindPixIdTheta  ||  !fHBlindPixNTheta  ||
       !fHgON              ||  !fHgOFF)
  { 
       *fLog << err << "At least one of the histograms needed for the padding is not defined ... aborting." << endl;
       return kFALSE;
  }

  fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
  if (!fMcEvt)
    {
       *fLog << err << dbginf << "MMcEvt not found... aborting." << endl;
       return kFALSE;
     }
  
   fPed = (MPedPhotCam*)pList->FindObject("MPedPhotCam");
   if (!fPed)
     {
       *fLog << err << "MPedPhotCam not found... aborting." << endl;
       return kFALSE;
     }

   fCam = (MGeomCam*)pList->FindObject("MGeomCam");
   if (!fCam)
     {
       *fLog << err << "MGeomCam not found (no geometry information available)... aborting." << endl;
       return kFALSE;
     }
  
   fEvt = (MCerPhotEvt*)pList->FindObject("MCerPhotEvt");
   if (!fEvt)
     {
       *fLog << err << "MCerPhotEvt not found... aborting." << endl;
       return kFALSE;
     }

   fSigmabar = (MSigmabar*)pList->FindCreateObj("MSigmabar");
   if (!fSigmabar)
     {
       *fLog << err << "MSigmabar not found... aborting." << endl;
       return kFALSE;
     }

   fBlinds = (MBlindPixels*)pList->FindCreateObj("MBlindPixels");
   if (!fBlinds)
     {
       *fLog << err << "MBlindPixels not found... aborting." << endl;
       return kFALSE;
     }
   
   if (fType !="ON"  &&  fType !="OFF"  &&  fType !="MC")
     {
       *fLog << err << "Type of data to be padded not defined... aborting." << endl;
       return kFALSE;
     }


   //--------------------------------------------------------------------
   // histograms for checking the padding
   //
   // plot pedestal sigmas
   fHSigmaPedestal = new TH2D("SigPed","Sigma: after vs. before padding", 
                     100, 0.0, 5.0, 100, 0.0, 5.0);
   fHSigmaPedestal->SetXTitle("Pedestal sigma before padding");
   fHSigmaPedestal->SetYTitle("Pedestal sigma after padding");

   // plot no.of photons (before vs. after padding) 
   fHPhotons = new TH2D("Photons","Photons: after vs.before padding", 
                        100, -10.0, 90.0, 100, -10, 90);
   fHPhotons->SetXTitle("no.of photons before padding");
   fHPhotons->SetYTitle("no.of photons after padding");

   // plot of added NSB
   fHNSB = new TH1D("NSB","Distribution of added NSB", 100, -10.0, 10.0);
   fHNSB->SetXTitle("no.of added NSB photons");
   fHNSB->SetYTitle("no.of pixels");


   //--------------------------------------------------------------------

   fIter = 20;

   memset(fErrors, 0, sizeof(fErrors));

   return kTRUE;
}

// --------------------------------------------------------------------------
//
// Do the Padding
// idealy the events to be padded should have been generated without NSB
// 
Int_t MCT1PadONOFF::Process()
{
  // *fLog << "Entry MCT1PadONOFF::Process();" << endl;

  //------------------------------------------------
  // add pixels to MCerPhotEvt which are not yet in;
  // set their number of photons equal to zero

  UInt_t imaxnumpix = fCam->GetNumPixels();

  for (UInt_t i=0; i<imaxnumpix; i++)
  {
    Bool_t alreadythere = kFALSE;
    UInt_t npix = fEvt->GetNumPixels();
    for (UInt_t j=0; j<npix; j++)
    {
      MCerPhotPix &pix = (*fEvt)[j];
      UInt_t id = pix.GetPixId();
      if (i==id)
      {
        alreadythere = kTRUE;
        break;
      }
    }
    if (alreadythere)
      continue;

    fEvt->AddPixel(i, 0.0, (*fPed)[i].GetRms());
  }



  //-----------------------------------------
  Int_t rc=0;
  Int_t rw=0;

  const UInt_t npix = fEvt->GetNumPixels();

  //fSigmabar->Calc(*fCam, *fPed, *fEvt);
  // *fLog << "before padding : " << endl;
  //fSigmabar->Print("");


  //$$$$$$$$$$$$$$$$$$$$$$$$$$
  // to simulate the situation that before the padding the NSB and 
  // electronic noise are zero : set Sigma = 0 for all pixels
  //for (UInt_t i=0; i<npix; i++) 
  //{   
  //  MCerPhotPix &pix = fEvt->operator[](i);      
  //  Int_t j = pix.GetPixId();

  //  MPedPhotPix &ppix = fPed->operator[](j);
  //  ppix.SetRms(0.0);
  //}
  //$$$$$$$$$$$$$$$$$$$$$$$$$$

  //-------------------------------------------
  // Calculate average sigma of the event
  //
  Double_t sigbarold = fSigmabar->Calc(*fCam, *fPed, *fEvt);
  Double_t sigbarold2 = sigbarold*sigbarold;
  //fSigmabar->Print("");

  // for MC data : expect sigmabar to be zero before the padding
  if (fType == "MC")
  {
    if (sigbarold > 0)
    {
      // *fLog << "MCT1PadONOFF::Process(); sigmabar of event to be padded is > 0 : "
      //      << sigbarold << ". Stop event loop " << endl;
      // input data should have sigmabar = 0; stop event loop
  
      rc = 1;
      fErrors[rc]++;
      return kCONTINUE; 
    }
  }

  const Double_t theta = kRad2Deg*fMcEvt->GetTelescopeTheta();
  // *fLog << "theta = " << theta << endl;

  Int_t binTheta = fHBlindPixNTheta->GetXaxis()->FindBin(theta);
  if ( binTheta < 1  ||  binTheta > fHBlindPixNTheta->GetNbinsX() )
  {
    // *fLog << "MCT1PadONOFF::Process(); binNumber out of range : theta, binTheta = "
    //      << theta << ",  " << binTheta << ";  Skip event " << endl;
    // event cannot be padded; skip event

    rc = 2;
    fErrors[rc]++;
    return kCONTINUE;
  }

  //-------------------------------------------
  // get number of events in this theta bin
  // and number of events in this sigbarold bin

  Double_t nTheta;
  Double_t nSigma;
  if (fType == "ON")
  {
    TH1D *hn;

    hn = fHSigmaThetaON->ProjectionY("", binTheta, binTheta, "");
    nTheta = hn->Integral();
    Int_t binSigma = hn->FindBin(sigbarold);
    nSigma = hn->GetBinContent(binSigma);

    // *fLog << "Theta, sigbarold, binTheta, binSigma, nTheta, nSigma = "
    //      << theta << ",  " << sigbarold << ",  " << binTheta << ",  "
    //      << binSigma << ",  " << nTheta << ",  " << nSigma   << endl;      

    delete hn;
  }

  else if (fType == "OFF")
  {
    TH1D *hn;

    hn = fHSigmaThetaOFF->ProjectionY("", binTheta, binTheta, "");
    nTheta = hn->Integral();
    Int_t binSigma = hn->FindBin(sigbarold);
    nSigma = hn->GetBinContent(binSigma);

    // *fLog << "Theta, sigbarold, binTheta, binSigma, nTheta, nSigma = "
    //      << theta << ",  " << sigbarold << ",  " << binTheta << ",  "
    //      << binSigma << ",  " << nTheta << ",  " << nSigma   << endl;      

    delete hn;
  }

  else
  {
    nTheta = 0.0;
    nSigma = 0.0;
  }

  //-------------------------------------------
  // for the current theta, 
  // generate blind pixels according to the histograms 
  //          fHBlindPixNTheta and fHBlindPixIDTheta
  // do this only for MC data


  if (fType == "MC")
  {

  // numBlind is the number of blind pixels in this event
  TH1D *nblind;
  UInt_t numBlind;

  nblind = fHBlindPixNTheta->ProjectionY("", binTheta, binTheta, "");
  if ( nblind->Integral() == 0.0 )
  {
    // *fLog << "MCT1PadONOFF::Process(); projection for Theta bin " 
    //      << binTheta << " has no entries; Skip event " << endl;
    // event cannot be padded; skip event
    delete nblind;

    rc = 7;
    fErrors[rc]++;
    return kCONTINUE;
  }
  else
  {
    numBlind = (Int_t) (nblind->GetRandom()+0.5);
  }
  delete nblind;

  //warn Code commented out due no compilation errors on Alpha OSF (tgb)

  // throw the Id of numBlind different pixels in this event
  TH1D *hblind;
  UInt_t idBlind;
  UInt_t listId[npix];
  UInt_t nlist = 0;
  Bool_t equal;

  hblind = fHBlindPixIdTheta->ProjectionY("", binTheta, binTheta, "");
  if ( hblind->Integral() > 0.0 )
    for (UInt_t i=0; i<numBlind; i++)
    {
      while(1)
      {
        idBlind = (Int_t) (hblind->GetRandom()+0.5);
        equal = kFALSE;
        for (UInt_t j=0; j<nlist; j++)
          if (idBlind == listId[j])
	  { 
            equal = kTRUE;
            break;
	  }
        if (!equal) break;
      }
      listId[nlist] = idBlind;
      nlist++;

      fBlinds->SetPixelBlind(idBlind);
      // *fLog << "idBlind = " << idBlind << endl;
    }
  fBlinds->SetReadyToSave();

  delete hblind;

  }

  //******************************************************************
  // has the event to be padded ?
  // if yes : to which sigmabar should it be padded ?
  //

  Int_t binSig = fHgON->GetYaxis()->FindBin(sigbarold);
  // *fLog << "binSig, sigbarold = " << binSig << ",  " << sigbarold << endl;

  Double_t prob;
  TH1D *hpad = NULL;
  if (fType == "ON")
  {
    hpad = fHgON->ProjectionZ("zON", binTheta, binTheta, binSig, binSig, "");

    //Int_t nb = hpad->GetNbinsX();
    //for (Int_t i=1; i<=nb; i++)
    //{
    //  if (hpad->GetBinContent(i) != 0.0)
    //    *fLog << "i, fHgON = " << i << ",  " << hpad->GetBinContent(i) << endl;
    //}

    if (nSigma != 0.0)
      prob = hpad->Integral() * nTheta/nSigma;
    else
      prob = 0.0;

    // *fLog << "nTheta, nSigma, prob = " << nTheta << ",  " << nSigma 
    //      << ",  " << prob << endl;
  }
  else if (fType == "OFF")
  {
    hpad = fHgOFF->ProjectionZ("zOFF", binTheta, binTheta, binSig, binSig, "");
    if (nSigma != 0.0)
      prob = hpad->Integral() * nTheta/nSigma;
    else
      prob = 0.0;
  }
  else
    prob = 1.0;

  if ( fType != "MC"  &&
       (prob <= 0.0  ||  gRandom->Uniform() > prob) )
  {
    delete hpad;
    // event should not be padded
    // *fLog << "event is not padded" << endl;

    rc = 8;
    fErrors[rc]++;
    return kTRUE;
  }
  // event should be padded
  // *fLog << "event is padded" << endl;  


  //-------------------------------------------
  // for the current theta, generate a sigmabar 
  //
  // for ON/OFF data according to the matrix fHgON/OFF
  // for MC data     according to the histogram fHSigmaTheta
  //
  Double_t sigmabar=0;
  if (fType == "ON"  ||  fType == "OFF")
  {
    //Int_t nbinsx = hpad->GetNbinsX();
    //for (Int_t i=1; i<=nbinsx; i++)
    //{
    //  if (hpad->GetBinContent(i) != 0.0)
    //    *fLog << "i, fHg = " << i << ",  " << hpad->GetBinContent(i) << endl;
    //}

    sigmabar = hpad->GetRandom();

    // *fLog << "sigmabar = " << sigmabar << endl;

    delete hpad;
  }

  else if (fType == "MC")
  {
    Int_t bincheck = fHSigmaTheta->GetXaxis()->FindBin(theta);

    if (binTheta != bincheck)
    {
      cout << "The binnings of the 2 histograms are not identical; aborting"
           << endl;
      return kERROR;
    }

    TH1D *hsigma;

    hsigma = fHSigmaTheta->ProjectionY("", binTheta, binTheta, "");
    if ( hsigma->Integral() == 0.0 )
    {
      *fLog << "MCT1PadONOFF::Process(); projection for Theta bin " 
            << binTheta << " has no entries; Skip event " << endl;
      // event cannot be padded; skip event
      delete hsigma;

      rc = 3;
      fErrors[rc]++;
      return kCONTINUE;
    }
    else
    {
      sigmabar = hsigma->GetRandom();
       // *fLog << "Theta, bin number = " << theta << ",  " << binTheta  
       //      << ",  sigmabar = " << sigmabar << endl
    }
    delete hsigma;
  }

  const Double_t sigmabar2 = sigmabar*sigmabar;

  //-------------------------------------------

  // *fLog << "MCT1PadONOFF::Process(); sigbarold, sigmabar = " 
  //      << sigbarold << ",  "<< sigmabar << endl;

  // Skip event if target sigmabar is <= sigbarold
  if (sigmabar <= sigbarold)
  {
    *fLog << "MCT1PadONOFF::Process(); target sigmabar is less than sigbarold : "
          << sigmabar << ",  " << sigbarold << ",   Skip event" << endl;

    rc = 4;
    fErrors[rc]++;
    return kCONTINUE;     
  }


  //-------------------------------------------
  //
  // Calculate average number of NSB photons to be added (lambdabar)
  // from the value of sigmabar, 
  //  - making assumptions about the average electronic noise (elNoise2) and
  //  - using a fixed value (F2excess)  for the excess noise factor
  
  Double_t elNoise2;         // [photons]  
  Double_t F2excess  = 1.3;
  Double_t lambdabar;        // [photons]



  Int_t bincheck = fHDiffPixTheta->GetXaxis()->FindBin(theta);
  if (binTheta != bincheck)
  {
    cout << "The binnings of the 2 histograms are not identical; aborting"
         << endl;
    return kERROR;
  }

  // Get RMS of (Sigma^2-sigmabar^2) in this Theta bin.
  // The average electronic noise (to be added) has to be well above this RMS,
  // otherwise the electronic noise of an individual pixel (elNoise2Pix)
  // may become negative

  TH1D *hnoise;
  if (fType == "MC")
    hnoise = fHDiffPixTheta->ProjectionZ("", binTheta, binTheta, 0, 9999, "");
  else if (fType == "ON")
    hnoise = fHDiffPixThetaOFF->ProjectionZ("", binTheta, binTheta, 0, 9999, "");
  else if (fType == "OFF")
    hnoise = fHDiffPixThetaON->ProjectionZ("", binTheta, binTheta, 0, 9999, "");
  else
  {
    *fLog << "MCT1PadONOFF::Process; illegal data type... aborting" << endl;
    return kERROR;
  }

  Double_t RMS = hnoise->GetRMS(1);  
  delete hnoise;

  elNoise2 = TMath::Min(RMS,  sigmabar2 - sigbarold2);
  // *fLog << "elNoise2 = " << elNoise2 << endl; 

  lambdabar = (sigmabar2 - sigbarold2 - elNoise2) / F2excess;     // [photons]

  // This value of lambdabar is the same for all pixels;
  // note that lambdabar is normalized to the area of pixel 0

  //----------   start loop over pixels   ---------------------------------
  // do the padding for each pixel
  //
  // pad only pixels   - which are used (before image cleaning)
  //
  Double_t sig         = 0;
  Double_t sigma2      = 0;
  Double_t diff        = 0;
  Double_t addSig2     = 0;
  Double_t elNoise2Pix = 0;


  for (UInt_t i=0; i<npix; i++) 
  {   
    MCerPhotPix &pix = (*fEvt)[i];
    if ( !pix.IsPixelUsed() )
      continue;

    //if ( pix.GetNumPhotons() == 0.0)
    //{
    //  *fLog << "MCT1PadONOFF::Process(); no.of photons is 0 for used pixel" 
    //        << endl;
    //  continue;
    //}

    Int_t j = pix.GetPixId();

    // GetPixRatio returns (area of pixel 0 / area of current pixel)
    Double_t ratioArea = 1.0 / fCam->GetPixRatio(j);

    MPedPhotPix &ppix = (*fPed)[j];
    Double_t oldsigma = ppix.GetRms();
    Double_t oldsigma2 = oldsigma*oldsigma;

    //---------------------------------
    // throw the Sigma for this pixel 
    //
    Int_t binPixel = fHDiffPixTheta->GetYaxis()->FindBin( (Double_t)j );

    Int_t count;
    Bool_t ok;

    TH1D *hdiff;
    TH1D *hsig;

    switch (fPadFlag)
    {
    case 1 :
      // throw the Sigma for this pixel from the distribution fHDiffPixTheta

      if (fType == "MC")
        hdiff = fHDiffPixTheta->ProjectionZ("", binTheta, binTheta,
                                                binPixel, binPixel, "");
      else if (fType == "ON") 
        hdiff = fHDiffPixThetaOFF->ProjectionZ("", binTheta, binTheta,
                                                   binPixel, binPixel, "");
      else 
        hdiff = fHDiffPixThetaON->ProjectionZ("", binTheta, binTheta,
                                                  binPixel, binPixel, "");

     if ( hdiff->Integral() == 0 )
      {
        *fLog << "MCT1PadONOFF::Process(); projection for Theta bin " 
              << binTheta << "  and pixel bin " << binPixel  
              << " has no entries;  aborting " << endl;
        delete hdiff;

        rc = 5;
        fErrors[rc]++;
        return kCONTINUE;     
      }

      count = 0;
      ok = kFALSE;
      for (Int_t m=0; m<fIter; m++)
      {
        count += 1;
        diff = hdiff->GetRandom();
        // the following condition ensures that elNoise2Pix > 0.0 

        if ( (diff + sigmabar2 - oldsigma2/ratioArea
                               - lambdabar*F2excess) > 0.0 )
	{
          ok = kTRUE;
          break;
	}
      }
      if (!ok)
      {
        // *fLog << "theta, j, count, sigmabar, diff = " << theta << ",  " 
        //      << j << ",  " << count << ",  " << sigmabar << ",  " 
        //      << diff << endl;
        diff = lambdabar*F2excess + oldsigma2/ratioArea - sigmabar2;

        rw = 1;
        fWarnings[rw]++;
      }
      else
      {
        rw = 0;
        fWarnings[rw]++;
      }

      delete hdiff;
      sigma2 = diff + sigmabar2;
      break;

    case 2 :
      // throw the Sigma for this pixel from the distribution fHSigmaPixTheta

      if (fType == "MC")
        hsig = fHSigmaPixTheta->ProjectionZ("", binTheta, binTheta,
                                                binPixel, binPixel, "");
      else if (fType == "ON")
        hsig = fHSigmaPixThetaOFF->ProjectionZ("", binTheta, binTheta,
                                                   binPixel, binPixel, "");
      else 
        hsig = fHSigmaPixThetaON->ProjectionZ("", binTheta, binTheta,
                                                  binPixel, binPixel, "");

      if ( hsig->Integral() == 0 )
      {
        *fLog << "MCT1PadONOFF::Process(); projection for Theta bin " 
              << binTheta << "  and pixel bin " << binPixel  
              << " has no entries;  aborting " << endl;
        delete hsig;

        rc = 6;
        fErrors[rc]++;
        return kCONTINUE;     
      }

      count = 0;
      ok = kFALSE;
      for (Int_t m=0; m<fIter; m++)
      {
        count += 1;

        sig = hsig->GetRandom();
        sigma2 = sig*sig/ratioArea;
        // the following condition ensures that elNoise2Pix > 0.0 

        if ( (sigma2-oldsigma2/ratioArea-lambdabar*F2excess) > 0.0 )
	{
          ok = kTRUE;
          break;
	}
      }
      if (!ok)
      {
        // *fLog << "theta, j, count, sigmabar, sig = " << theta << ",  " 
        //      << j << ",  " << count << ",  " << sigmabar << ",  " 
        //      << sig << endl;
        sigma2 = lambdabar*F2excess + oldsigma2/ratioArea; 

        rw = 1;
        fWarnings[rw]++;
      }
      else
      {
        rw = 0;
        fWarnings[rw]++;
      }

      delete hsig;
      break;
    }

    //---------------------------------
    // get the additional sigma^2 for this pixel (due to the padding)

    addSig2 = sigma2*ratioArea - oldsigma2;


    //---------------------------------
    // get the additional electronic noise for this pixel

    elNoise2Pix = addSig2 - lambdabar*F2excess*ratioArea;


    //---------------------------------
    // throw actual number of additional NSB photons (NSB)
    //       and its RMS (sigmaNSB) 

    Double_t NSB0 = gRandom->Poisson(lambdabar*ratioArea);
    Double_t arg  = NSB0*(F2excess-1.0) + elNoise2Pix;
    Double_t sigmaNSB0;

    if (arg >= 0.0)
    {
      sigmaNSB0 = sqrt( arg );
    }
    else
    {
      sigmaNSB0 = 0.0000001;      
      if (arg < -1.e-10)
      {
        *fLog << "MCT1PadONOFF::Process(); argument of sqrt < 0 : "
              << arg << endl;
      }
    }


    //---------------------------------
    // smear NSB0 according to sigmaNSB0
    // and subtract lambdabar because of AC coupling

    Double_t NSB = gRandom->Gaus(NSB0, sigmaNSB0) - lambdabar*ratioArea;

    //---------------------------------
 
    // add additional NSB to the number of photons
    Double_t oldphotons = pix.GetNumPhotons();
    Double_t newphotons = oldphotons + NSB;
    pix.SetNumPhotons(	newphotons );


    fHNSB->Fill( NSB/sqrt(ratioArea) );
    fHPhotons->Fill( oldphotons/sqrt(ratioArea), newphotons/sqrt(ratioArea) );


    // error: add sigma of padded noise quadratically
    Double_t olderror = pix.GetErrorPhot();
    Double_t newerror = sqrt( olderror*olderror + addSig2 );
    pix.SetErrorPhot( newerror );


    Double_t newsigma = sqrt( oldsigma2 + addSig2 ); 
    ppix.SetRms( newsigma );

    fHSigmaPedestal->Fill( oldsigma, newsigma );
  } 
  //----------   end of loop over pixels   ---------------------------------

  // Calculate sigmabar again and crosscheck

  //fSigmabar->Calc(*fCam, *fPed, *fEvt);
  // *fLog << "after padding : " << endl;
  //fSigmabar->Print("");

  rc = 0;
  fErrors[rc]++;
  return kTRUE;
  //******************************************************************
}

// --------------------------------------------------------------------------
//
//
Int_t MCT1PadONOFF::PostProcess()
{
    if (GetNumExecutions() != 0)
    {

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << dec << setfill(' ');

    int temp;
    if (fWarnings[0] != 0.0)    
      temp = (int)(fWarnings[1]*100/fWarnings[0]);
    else
      temp = 100;

    *fLog << " " << setw(7) << fWarnings[1] << " (" << setw(3) 
          << temp 
          << "%) Warning : iteration to find acceptable sigma failed" 
          << ", fIter = " << fIter << endl;

    *fLog << " " << setw(7) << fErrors[1] << " (" << setw(3) 
          << (int)(fErrors[1]*100/GetNumExecutions()) 
          << "%) Evts skipped due to: Sigmabar_old > 0" << endl;

    *fLog << " " << setw(7) << fErrors[2] << " (" << setw(3) 
          << (int)(fErrors[2]*100/GetNumExecutions()) 
          << "%) Evts skipped due to: Zenith angle out of range" << endl;

    *fLog << " " << setw(7) << fErrors[3] << " (" << setw(3) 
          << (int)(fErrors[3]*100/GetNumExecutions()) 
          << "%) Evts skipped due to: No data for generating Sigmabar" << endl;

    *fLog << " " << setw(7) << fErrors[4] << " (" << setw(3) 
          << (int)(fErrors[4]*100/GetNumExecutions()) 
          << "%) Evts skipped due to: Target sigma <= Sigmabar_old" << endl;

    *fLog << " " << setw(7) << fErrors[5] << " (" << setw(3) 
          << (int)(fErrors[5]*100/GetNumExecutions()) 
          << "%) Evts skipped due to: No data for generating Sigma^2-Sigmabar^2" << endl;

    *fLog << " " << setw(7) << fErrors[6] << " (" << setw(3) 
          << (int)(fErrors[6]*100/GetNumExecutions()) 
          << "%) Evts skipped due to: No data for generating Sigma" << endl;

    *fLog << " " << setw(7) << fErrors[7] << " (" << setw(3) 
          << (int)(fErrors[7]*100/GetNumExecutions()) 
          << "%) Evts skipped due to: No data for generating Blind pixels" << endl;

    *fLog << " " << setw(7) << fErrors[8] << " (" << setw(3) 
          << (int)(fErrors[8]*100/GetNumExecutions()) 
          << "%) Evts didn't have to be padded" << endl;

    *fLog << " " << fErrors[0] << " (" 
          << (int)(fErrors[0]*100/GetNumExecutions()) 
          << "%) Evts were successfully padded!" << endl;
    *fLog << endl;

    }

    //---------------------------------------------------------------
    TCanvas &c = *(MH::MakeDefCanvas("PadONOFF", "", 900, 1200)); 
    c.Divide(3, 4);
    gROOT->SetSelectedPad(NULL);

    c.cd(1);
    fHNSB->SetDirectory(NULL);
    fHNSB->DrawCopy();
    fHNSB->SetBit(kCanDelete);    

    c.cd(2);
    fHSigmaPedestal->SetDirectory(NULL);
    fHSigmaPedestal->DrawCopy();
    fHSigmaPedestal->SetBit(kCanDelete);    

    c.cd(3);
    fHPhotons->SetDirectory(NULL);
    fHPhotons->DrawCopy();
    fHPhotons->SetBit(kCanDelete);    

    //--------------------------------------------------------------------


    c.cd(4);
    fHSigmaTheta->SetDirectory(NULL);
    fHSigmaTheta->SetTitle("(Target) 2D : Sigmabar, \\Theta");
    fHSigmaTheta->DrawCopy();     
    fHSigmaTheta->SetBit(kCanDelete);     


    //--------------------------------------------------------------------


    c.cd(7);
    fHBlindPixNTheta->SetDirectory(NULL);
    fHBlindPixNTheta->SetTitle("(Target) 2D : no.of blind pixels, \\Theta");
    fHBlindPixNTheta->DrawCopy();     
    fHBlindPixNTheta->SetBit(kCanDelete);     

    //--------------------------------------------------------------------


    c.cd(10);
    fHBlindPixIdTheta->SetDirectory(NULL);
    fHBlindPixIdTheta->SetTitle("(Target) 2D : blind pixel Id, \\Theta");
    fHBlindPixIdTheta->DrawCopy();     
    fHBlindPixIdTheta->SetBit(kCanDelete);     


    //--------------------------------------------------------------------
    // draw the 3D histogram (target): Theta, pixel, Sigma^2-Sigmabar^2

    c.cd(5);
    TH2D *l1;
    l1 = (TH2D*) ((TH3*)fHDiffPixTheta)->Project3D("zx");
    l1->SetDirectory(NULL);
    l1->SetTitle("(Target) Sigma^2-Sigmabar^2 vs. \\Theta (all pixels)");
    l1->SetXTitle("\\Theta [\\circ]");
    l1->SetYTitle("Sigma^2-Sigmabar^2");

    l1->DrawCopy("box");
    l1->SetBit(kCanDelete);;

    c.cd(8);
    TH2D *l2;
    l2 = (TH2D*) ((TH3*)fHDiffPixTheta)->Project3D("zy");
    l2->SetDirectory(NULL);
    l2->SetTitle("(Target) Sigma^2-Sigmabar^2 vs. pixel number (all \\Theta)");
    l2->SetXTitle("pixel");
    l2->SetYTitle("Sigma^2-Sigmabar^2");

    l2->DrawCopy("box");
    l2->SetBit(kCanDelete);;

    //--------------------------------------------------------------------
    // draw the 3D histogram (target): Theta, pixel, Sigma

    c.cd(6);
    TH2D *k1;
    k1 = (TH2D*) ((TH3*)fHSigmaPixTheta)->Project3D("zx");
    k1->SetDirectory(NULL);
    k1->SetTitle("(Target) Sigma vs. \\Theta (all pixels)");
    k1->SetXTitle("\\Theta [\\circ]");
    k1->SetYTitle("Sigma");

    k1->DrawCopy("box");
    k1->SetBit(kCanDelete);

    c.cd(9);
    TH2D *k2;
    k2 = (TH2D*) ((TH3*)fHSigmaPixTheta)->Project3D("zy");
    k2->SetDirectory(NULL);
    k2->SetTitle("(Target) Sigma vs. pixel number (all \\Theta)");
    k2->SetXTitle("pixel");
    k2->SetYTitle("Sigma");

    k2->DrawCopy("box");
    k2->SetBit(kCanDelete);;


    //--------------------------------------------------------------------

    c.cd(11);
    TH2D *m1;
    m1 = (TH2D*) ((TH3*)fHgON)->Project3D("zy");
    m1->SetDirectory(NULL);
    m1->SetTitle("(Target) Sigmabar-new vs. Sigmabar-old (ON, all  \\Theta)");
    m1->SetXTitle("Sigmabar-old");
    m1->SetYTitle("Sigmabar-new");

    m1->DrawCopy("box");
    m1->SetBit(kCanDelete);;

    c.cd(12);
    TH2D *m2;
    m2 = (TH2D*) ((TH3*)fHgOFF)->Project3D("zy");
    m2->SetDirectory(NULL);
    m2->SetTitle("(Target) Sigmabar-new vs. Sigmabar-old (OFF, all  \\Theta)");
    m2->SetXTitle("Sigmabar-old");
    m2->SetYTitle("Sigmabar-new");

    m2->DrawCopy("box");
    m2->SetBit(kCanDelete);;


  return kTRUE;
}

// --------------------------------------------------------------------------






