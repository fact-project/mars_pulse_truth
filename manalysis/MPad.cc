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
!   Author(s): Wolfgang Wittek, 10/2004 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MPad
//
//  The aim of the padding is to make the noise level (NSB + electronic noise)
//  equal for the MC, ON and OFF data samples
//  
//  The target noise level is determined by 'merging' the (sigmabar vs. Theta)
//  distributions of MC, ON and OFF data.
//
//  The prescription on which fraction of events has to padded from
//  bin k of sigmabar to bin j is stored in the matrices 
//  fHgMC, fHgON and fHgOFF.
//
//  By the padding the number of photons, its error and the pedestal sigmas 
//  are altered. On average, the number of photons added is zero.
//
//  The formulas used can be found in Thomas Schweizer's Thesis,
//                                    Section 2.2.1
//
//  The padding is done as follows :
//
//     Increase the sigmabar according to the matrices fHg.... Then generate 
//     a pedestal sigma for each pixel using the 3D-histogram Theta, pixel no.,
//     Sigma^2-Sigmabar^2 (fHDiffPixTheta).
//
//  The padding has to be done before the image cleaning because the
//  image cleaning depends on the pedestal sigmas.
//
/////////////////////////////////////////////////////////////////////////////
#include "MPad.h"

#include <math.h>
#include <stdio.h>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TVirtualPad.h>

#include "MBinning.h"
#include "MPointingPos.h"
#include "MLog.h"
#include "MLogManip.h"
#include "MParList.h"
#include "MGeomCam.h"

#include "MCerPhotPix.h"
#include "MCerPhotEvt.h"

#include "MH.h"

#include "MPedPhotCam.h"
#include "MPedPhotPix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MRead.h"
#include "MFilterList.h"
#include "MTaskList.h"
#include "MHBadPixels.h"
#include "MFillH.h"
#include "MHSigmaTheta.h"
#include "MEvtLoop.h"

ClassImp(MPad);

using namespace std;

// --------------------------------------------------------------------------
//
// Constructor. 
//
MPad::MPad(const char *name, const char *title) 
{
  fName  = name  ? name  : "MPad";
  fTitle = title ? title : "Task for the padding";

  fType = "";

  fHSigmaTheta       = NULL;
  fHSigmaThetaOuter  = NULL;

  fHSigmaThetaMC         = NULL;
  fHSigmaThetaOuterMC    = NULL;
  fHDiffPixThetaMC       = NULL;
  fHDiffPixThetaTargetMC = NULL;

  fHSigmaThetaON          = NULL;
  fHSigmaThetaOuterON     = NULL;
  fHDiffPixThetaON        = NULL;
  fHDiffPixThetaTargetON  = NULL;

  fHSigmaThetaOFF         = NULL;
  fHSigmaThetaOuterOFF    = NULL;
  fHDiffPixThetaOFF       = NULL;
  fHDiffPixThetaTargetOFF = NULL;

  fHgMC  = NULL;
  fHgON  = NULL;
  fHgOFF = NULL;

  fHgOuterMC  = NULL;
  fHgOuterON  = NULL;
  fHgOuterOFF = NULL;

  fHSigmaPedestal = NULL;
  fHPhotons       = NULL;
  fHNSB           = NULL;

  fNamePedPhotCam = "MPedPhotCamFromData";
}

// --------------------------------------------------------------------------
//
// Destructor. 
//
MPad::~MPad()
{
  delete fHSigmaTheta;
  delete fHSigmaThetaOuter;

  if (fHSigmaThetaMC)
  {
    delete fHSigmaThetaMC;
    delete fHSigmaThetaOuterMC;
    delete fHDiffPixThetaMC;
    delete fHDiffPixThetaTargetMC;
    delete fHgMC;
    delete fHgOuterMC;
  }

  if (fHSigmaThetaON)
  {
    delete fHSigmaThetaON;
    delete fHSigmaThetaOuterON;
    delete fHDiffPixThetaON;
    delete fHDiffPixThetaTargetON;
    delete fHgON;
    delete fHgOuterON;
  }

  if (fHSigmaThetaOFF)
  {
    delete fHSigmaThetaOFF;
    delete fHSigmaThetaOuterOFF;
    delete fHDiffPixThetaOFF;
    delete fHDiffPixThetaTargetOFF;
    delete fHgOFF;
    delete fHgOuterOFF;
  }

  delete fHSigmaPedestal;
  delete fHPhotons;
  delete fHNSB;

  delete fInfile;
}

// --------------------------------------------------------------------------
//
// SetNamePedPhotCam 
//
//    set the name of the MPedPhotCam container

void MPad::SetNamePedPhotCam(const char *name) 
{
  fNamePedPhotCam = name;
}

// --------------------------------------------------------------------------
//
// Set type of data to be padded
//
//     this is not necessary if the type of the data can be recognized
//     directly from the input
//
//
void MPad::SetDataType(const char* type)
{
  fType = type;

  return;
}

// --------------------------------------------------------------------------
//
// Read pad histograms from a file
//      these are the histograms generated by MMakePadHistograms
//
Bool_t MPad::ReadPadHistograms(TString type, TString namepad)
{

  //------------------------------------
  Int_t OK = 0;

  if (type == "ON")
  {
      *fLog << inf << "MPad : Read pad histograms for " << type 
            << " data from file " << namepad << endl;

      TH2D *sigth    = new TH2D;
      TH2D *sigthout = new TH2D;
      TH3D *diff     = new TH3D;

      TFile *fInfile = new TFile(namepad);
      fInfile->ls();

      OK = sigth->Read("2D-ThetaSigmabar(Inner)"); 
      fHSigmaThetaON = new TH2D( (const TH2D&)(*sigth) );
      if (!OK)
      {
        *fLog << all 
              << "        MPad : Object '2D-ThetaSigmabar(Inner)' not found" 
              << endl;
        return kFALSE;
      }

      fHSigmaThetaON->SetNameTitle("2D-ThetaSigmabarON", 
                                   "2D-ThetaSigmabarON (orig.)");
      *fLog << all 
            << "        MPad : Object '2D-ThetaSigmabar(Inner)' was read in" 
            << endl;


      OK = sigthout->Read("2D-ThetaSigmabar(Outer)");
      fHSigmaThetaOuterON = new TH2D( (const TH2D&)(*sigthout) );
      if (!OK)
      {
        *fLog << all 
              << "        MPad : Object '2D-ThetaSigmabar(Outer)' not found" 
              << endl;
        return kFALSE;
      }

      fHSigmaThetaOuterON->SetNameTitle("2D-ThetaSigmabarOuterON", 
                                   "2D-ThetaSigmabarOuterON (orig.)");
      *fLog << all 
            << "        MPad : Object '2D-ThetaSigmabar(Outer)' was read in" 
            << endl;


      OK = diff->Read("3D-ThetaPixDiff");
      fHDiffPixThetaON = new TH3D( (const TH3D&)(*diff) );
      if (!OK)
      {
        *fLog << all 
              << "        MPad : Object '3D-ThetaPixDiff' not found" 
              << endl;
        return kFALSE;
      }

      fHDiffPixThetaON->SetNameTitle("3D-ThetaPixDiffON", 
                                     "3D-ThetaPixDiffON (orig.)");
      *fLog << all 
            << "        MPad : Object '3D-ThetaPixDiff' was read in" << endl;


      return kTRUE;
  }

  //------------------------------------
  if (type == "OFF")
  {
      *fLog << inf << "MPad : Read pad histograms for " << type 
            << " data from file " << namepad << endl;


      TH2D *sigth    = new TH2D;
      TH2D *sigthout = new TH2D;
      TH3D *diff     = new TH3D;

      TFile *fInfile = new TFile(namepad);
      fInfile->ls();

      OK = sigth->Read("2D-ThetaSigmabar(Inner)");
      fHSigmaThetaOFF = new TH2D( (const TH2D&)(*sigth) );
      if (!OK)
      {
        *fLog << all 
              << "        MPad : Object '2D-ThetaSigmabar(Inner)' not found" 
              << endl;
        return kFALSE;
      }

      fHSigmaThetaOFF->SetNameTitle("2D-ThetaSigmabarOFF", 
                                    "2D-ThetaSigmabarOFF (orig.)");
      *fLog << all 
            << "        MPad : Object '2D-ThetaSigmabar(Inner)' was read in" 
            << endl;


      OK = sigthout->Read("2D-ThetaSigmabar(Outer)");
      fHSigmaThetaOuterOFF = new TH2D( (const TH2D&)(*sigthout) );
      if (!OK)
      {
        *fLog << all 
              << "        MPad : Object '2D-ThetaSigmabar(Outer)' not found" 
              << endl;
        return kFALSE;
      }

      fHSigmaThetaOuterOFF->SetNameTitle("2D-ThetaSigmabarOuterOFF", 
                                    "2D-ThetaSigmabarOuterOFF (orig.)");
      *fLog << all 
            << "        MPad : Object '2D-ThetaSigmabar(Outer)' was read in" 
            << endl;


      OK = diff->Read("3D-ThetaPixDiff");
      fHDiffPixThetaOFF = new TH3D( (const TH3D&)(*diff) );
      if (!OK)
      {
        *fLog << all 
              << "        MPad : Object '3D-ThetaPixDiff' not found" 
              << endl;
        return kFALSE;
      }

      fHDiffPixThetaOFF->SetNameTitle("3D-ThetaPixDiffOFF", 
                                      "3D-ThetaPixDiffOFF (orig.)");
      *fLog << all 
            << "        MPad : Object '3D-ThetaPixDiff' was read in" << endl;



      return kTRUE;
  }

    //------------------------------------
  if (type == "MC")
  {

      TH2D *sigth    = new TH2D;
      TH2D *sigthout = new TH2D;
      TH3D *diff     = new TH3D;

      *fLog << inf << "MPad : Read pad histograms for " << type 
            << " data from file " << namepad << endl;

      TFile *fInfile = new TFile(namepad);
      fInfile->ls();

      OK = sigth->Read("2D-ThetaSigmabar(Inner)");
      fHSigmaThetaMC = new TH2D( (const TH2D&)(*sigth) );
      if (!OK)
      {
        *fLog << all 
              << "        MPad : Object '2D-ThetaSigmabar(Inner)' not found" 
              << endl;
        return kFALSE;
      }

      fHSigmaThetaMC->SetNameTitle("2D-ThetaSigmabarMC", 
                                   "2D-ThetaSigmabarMC (orig.)");
      *fLog << all 
            << "       MPad : Object '2D-ThetaSigmabar(Inner)' was read in" 
            << endl;

      OK = sigthout->Read("2D-ThetaSigmabar(Outer)");
      fHSigmaThetaOuterMC = new TH2D( (const TH2D&)(*sigthout) );
      if (!OK)
      {
        *fLog << all 
              << "        MPad : Object '2D-ThetaSigmabar(Outer)' not found" 
              << endl;
        return kFALSE;
      }

      fHSigmaThetaOuterMC->SetNameTitle("2D-ThetaSigmabarOuterMC", 
                                   "2D-ThetaSigmabarOuterMC (orig.)");
      *fLog << all 
            << "       MPad : Object '2D-ThetaSigmabar(Outer)' was read in" 
            << endl;


      
      OK = diff->Read("3D-ThetaPixDiff");
      fHDiffPixThetaMC = new TH3D( (const TH3D&)(*diff) );
      if (!OK)
      {
        *fLog << all 
              << "        MPad : Object '3D-ThetaPixDiff' not found" 
              << endl;
        return kFALSE;
      }

      fHDiffPixThetaMC->SetNameTitle("3D-ThetaPixDiffMC", 
                                     "3D-ThetaPixDiffMC (orig.)");

      *fLog << all 
            << "       MPad : Object '3D-ThetaPixDiff' was read in" << endl;
      
      return kTRUE;
  }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Merge the distributions 
//   fHSigmaTheta      2D-histogram  (Theta, sigmabar)
//
// ===>   of ON, OFF and MC data   <==============
//
// and define the matrices fHgMC, fHgON and fHgOFF 
//
// These matrices tell which fraction of events should be padded 
// from bin k of sigmabar to bin j
//

Bool_t MPad::MergeONOFFMC(TString nameon, TString nameoff,
                          TString namemc, TString fileout)
{
  // read the histograms produced by MMakePadHistograms
  TH2D *hist2patternin  = NULL;
  TH2D *hist2patternout = NULL;
  TH3D *hist3pattern = NULL;

  Bool_t OK = 0;

  if (namemc != "")
  {
    OK = ReadPadHistograms("MC",  namemc);  
    if (!OK) 
    {
      *fLog << err << "MPad::MergeONOFFMC : Pad histograms for MC not available"
            << endl;
      return kFALSE;
    }

    hist2patternin  = fHSigmaThetaMC;
    hist2patternout = fHSigmaThetaOuterMC;
    hist3pattern = fHDiffPixThetaMC;
    *fLog << inf << "" << endl;
    *fLog << inf << "MPad::MergeONOFFMC : Pad histograms for MC data read from file "
             << namemc << endl;
  }

  if (nameoff != "")
  {
    OK = ReadPadHistograms("OFF", nameoff);
    if (!OK) 
    {
      *fLog << err << "MPad::MergeONOFFMC : Pad histograms for OFF not available"
            << endl;
      return kFALSE;
    }

    hist2patternin  = fHSigmaThetaOFF;
    hist2patternout = fHSigmaThetaOuterOFF;
    hist3pattern = fHDiffPixThetaOFF;  
    *fLog << inf << "" << endl;
    *fLog << inf << "MPad::MergeONOFFMC : Pad histograms for OFF data read from file "
             << nameoff << endl;
  }

  if (nameon != "")
  {
    OK = ReadPadHistograms("ON",  nameon);
    if (!OK) 
    {
      *fLog << err << "MPad::MergeONOFFMC : Pad histograms for ON not available"
            << endl;
      return kFALSE;
    }

    hist2patternin  = fHSigmaThetaON;
    hist2patternout = fHSigmaThetaOuterON;
    hist3pattern = fHDiffPixThetaON;
    *fLog << inf << "" << endl;
    *fLog << inf << "MPad::MergeONOFFMC : Pad histograms for ON data read from file "
             << nameon << endl;
  }  

  //*fLog << "hist2patternin = "  << hist2patternin << endl;
  //*fLog << "hist2patternout = " << hist2patternout << endl;
  //*fLog << "hist3pattern = " << hist3pattern << endl;


  TAxis *axs = hist2patternin->GetXaxis();
  Int_t nbinsthetas = axs->GetNbins();
  //*fLog << "vor fHSigmaTheta : nbinsthetas = " << nbinsthetas << endl;

  TArrayD edgess;
  edgess.Set(nbinsthetas+1);
  for (Int_t i=0; i<=nbinsthetas; i++)
  {
    edgess[i] = axs->GetBinLowEdge(i+1);
    //*fLog << all << "i, theta low edge = " << i << ",  " << edgess[i] 
    //      << endl; 
  }


  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // for testing
  /*
  TAxis *xa = sigthon.GetXaxis();
  Int_t nbitheta = xa->GetNbins();

  TAxis *ya = sigthon.GetYaxis();
  Int_t nbisig = ya->GetNbins();
  for (Int_t i=1; i<=nbitheta; i++)
  {
    for (Int_t j=1; j<=nbisig; j++)
    {
      if (i>0)
      {
        sigthmc.SetBinContent( i, j, (Float_t) (625000.0+(nbisig*nbisig*nbisig-j*j*j)) );
        sigthon.SetBinContent( i, j, (Float_t)(1.0) );
        sigthoff.SetBinContent(  i, j, (Float_t)( (0.5*nbisig-j)*(0.5*nbisig-j)            ) );
      }
      else
      {
        sigthmc.SetBinContent( i, j, 0.0);
        sigthon.SetBinContent( i, j, 0.0);
        sigthoff.SetBinContent(i, j, 0.0);
      }
    }
  }
  */
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

  *fLog << all << "----------------------------------------------------------------------------------" << endl;
  *fLog << all << "MPad::MergeONOFFMC(); Merge the ON, OFF and MC histograms to obtain the target distributions for the padding"
        << endl;


  //-------------------------
  fHSigmaTheta = new TH2D;
  hist2patternin->Copy(*fHSigmaTheta);
  fHSigmaTheta->SetNameTitle("2D-ThetaSigmabar", "2D-ThetaSigmabar (target)");

  //-------------------------
  fHSigmaThetaOuter = new TH2D;
  hist2patternout->Copy(*fHSigmaThetaOuter);
  fHSigmaThetaOuter->SetNameTitle("2D-ThetaSigmabarOuter", "2D-ThetaSigmabarOuter (target)");

  //--------------------------
  fHDiffPixThetaTargetMC = new TH3D;
  hist3pattern->Copy(*fHDiffPixThetaTargetMC);
  fHDiffPixThetaTargetMC->SetNameTitle("3D-ThetaPixDiffTargetMC", "3D-ThetaPixDiffMC (target)");

  //-------------------------  
  fHDiffPixThetaTargetON = new TH3D;
  hist3pattern->Copy(*fHDiffPixThetaTargetON);
  fHDiffPixThetaTargetON->SetNameTitle("3D-ThetaPixDiffTargetON", "3D-ThetaPixDiffON (target)");

  //-------------------------
  fHDiffPixThetaTargetOFF = new TH3D;
  hist3pattern->Copy(*fHDiffPixThetaTargetOFF);
  fHDiffPixThetaTargetOFF->SetNameTitle("3D-ThetaPixDiffTargetOFF", "3D-ThetaPixDiffOFF (target)");

  *fLog << all << "MPad::MergeONOFFMC(); Histograms for the merged padding plots were booked" 
        << endl;


  //--------------------------

  // get binning for fHgON, fHgOFF and fHgMC  from fHSigmaTheta
  // binning in Theta
  TAxis *ax = hist2patternin->GetXaxis();
  Int_t nbinstheta = ax->GetNbins();

  //*fLog << "nbinstheta = " << nbinstheta << endl;

  TArrayD edgesx;
  edgesx.Set(nbinstheta+1);
  for (Int_t i=0; i<=nbinstheta; i++)
  {
    edgesx[i] = ax->GetBinLowEdge(i+1);
    //*fLog << all << "i, theta low edge = " << i << ",  " << edgesx[i] 
    //      << endl; 
  }
  MBinning binth;
  binth.SetEdges(edgesx);

  // binning in sigmabar
  TAxis *ay = hist2patternin->GetYaxis();
  Int_t nbinssig = ay->GetNbins();

  //*fLog << "nbinssig = " << nbinssig << endl;

  TArrayD edgesy;
  edgesy.Set(nbinssig+1);
  for (Int_t i=0; i<=nbinssig; i++)
  {
    edgesy[i] = ay->GetBinLowEdge(i+1); 
    //*fLog << all << "i, sigmabar low edge = " << i << ",  " << edgesy[i] 
    //      << endl; 
  }
  MBinning binsg;
  binsg.SetEdges(edgesy);


  fHgMC = new TH3D;
  MH::SetBinning(fHgMC, &binth, &binsg, &binsg);
  fHgMC->SetNameTitle("3D-PaddingMatrixMC", "3D-PadMatrixThetaMC");

  fHgON = new TH3D;
  MH::SetBinning(fHgON, &binth, &binsg, &binsg);
  fHgON->SetNameTitle("3D-PaddingMatrixON", "3D-PadMatrixThetaON");

  fHgOFF = new TH3D;
  MH::SetBinning(fHgOFF, &binth, &binsg, &binsg);
  fHgOFF->SetNameTitle("3D-PaddingMatrixOFF", "3D-PadMatrixThetaOFF");

  *fLog << all << "fHg histograms were booked" << endl;

  //-----------------------
  // get binning for fHgOuterON, fHgOuterOFF and fHgOuterMC  
  // from fHSigmaThetaOuter
  // binning in sigmabar
  TAxis *ayout = hist2patternout->GetYaxis();
  Int_t nbinssigout = ayout->GetNbins();
  TArrayD edgesyout;
  edgesyout.Set(nbinssigout+1);
  for (Int_t i=0; i<=nbinssigout; i++)
  {
    edgesyout[i] = ayout->GetBinLowEdge(i+1); 
    //*fLog << all << "i, sigmabar low edge = " << i << ",  " << edgesyout[i] 
    //      << endl; 
  }
  MBinning binsgout;
  binsgout.SetEdges(edgesyout);


  fHgOuterMC = new TH3D;
  MH::SetBinning(fHgOuterMC, &binth, &binsgout, &binsgout);
  fHgOuterMC->SetNameTitle("3D-PaddingMatrixOuterMC", "3D-PadMatrixThetaOuterMC");

  fHgOuterON = new TH3D;
  MH::SetBinning(fHgOuterON, &binth, &binsgout, &binsgout);
  fHgOuterON->SetNameTitle("3D-PaddingMatrixOuterON", "3D-PadMatrixThetaOuterON");

  fHgOuterOFF = new TH3D;
  MH::SetBinning(fHgOuterOFF, &binth, &binsgout, &binsgout);
  fHgOuterOFF->SetNameTitle("3D-PaddingMatrixOuterOFF", "3D-PadMatrixThetaOuterOFF");

  *fLog << all << "fHgOuter histograms were booked" << endl;


  //--------------------------------------------------------------------
  // define the first (A), second (B) and third (C) data set
  // (A, B, C) may be (ON, OFF, MC)
  //               or (ON, OFF, "")
  //               or (ON, MC,  "")
  //               or (OFF,MC,  "")
  // or the same sets in any other order

  TString tagA = "";
  TString tagB = "";
  TString tagC = "";

  TH2D *histA = NULL;
  TH2D *histB = NULL;
  TH2D *histC = NULL;

  TH2D *histOuterA = NULL;
  TH2D *histOuterB = NULL;
  TH2D *histOuterC = NULL;

  TH3D *histDiffA = NULL;
  TH3D *histDiffB = NULL;
  TH3D *histDiffC = NULL;

  TH3D *fHgA = NULL;
  TH3D *fHgB = NULL;
  TH3D *fHgC = NULL;

  TH3D *fHgOuterA = NULL;
  TH3D *fHgOuterB = NULL;
  TH3D *fHgOuterC = NULL;

  if (fHSigmaThetaON)
  {
    tagA = "ON";
    histA      = fHSigmaThetaON;
    histOuterA = fHSigmaThetaOuterON;
    histDiffA  = fHDiffPixThetaON;
    fHgA       = fHgON;
    fHgOuterA  = fHgOuterON;

    if (fHSigmaThetaOFF)
    {
      tagB = "OFF";
      histB      = fHSigmaThetaOFF;
      histOuterB = fHSigmaThetaOuterOFF;
      histDiffB  = fHDiffPixThetaOFF;
      fHgB       = fHgOFF;
      fHgOuterB  = fHgOuterOFF;

      if (fHSigmaThetaMC)
      {
        tagC = "MC";
        histC      = fHSigmaThetaMC;
        histOuterC = fHSigmaThetaOuterMC;
        histDiffC  = fHDiffPixThetaMC;
        fHgC       = fHgMC;
        fHgOuterC  = fHgOuterMC;
      }
    }

    else if (fHSigmaThetaMC)
    {
      tagB = "MC";
      histB      = fHSigmaThetaMC;
      histOuterB = fHSigmaThetaOuterMC;
      histDiffB  = fHDiffPixThetaMC;
      fHgB       = fHgMC;
      fHgOuterB  = fHgOuterMC;
    }
  }
  
  else if (fHSigmaThetaOFF)
  {
    tagA = "OFF";
    histA      = fHSigmaThetaOFF;
    histOuterA = fHSigmaThetaOuterOFF;
    histDiffA  = fHDiffPixThetaOFF;
    fHgA       = fHgOFF;
    fHgOuterA  = fHgOuterOFF;

    if (fHSigmaThetaMC)
    {
      tagB = "MC";
      histB      = fHSigmaThetaMC;
      histOuterB = fHSigmaThetaOuterMC;
      histDiffB  = fHDiffPixThetaMC;
      fHgB       = fHgMC;
      fHgOuterB  = fHgOuterMC;
    }

    else
    {
      *fLog << err << "MPad::MergeONOFFMC; there are no data sets to be merged : tagA, tagB, tagC = "
            << tagA << ",  " << tagB << ",  " << tagC << endl;
      return kFALSE;
    }
  }
 
  else
  {
    *fLog << err << "MPad::MergeONOFFMC; there are no data sets to be merged"
          << endl;
    return kFALSE;
  }

  *fLog << inf << "MPad::MergeONOFFMC; the following data sets will be merged : "
        << tagA << "   " << tagB << "   " << tagC << endl;


  //-------------------------------------------
  // merge the 2D histograms (theta, Sigmabar)
  //       from the data sets A, B and C
  // for the inner pixels and for the outer pixels

  TString canv("Inner");
  MergeABC(tagA, tagB, tagC,  histA, histB, histC, 
           fHSigmaTheta,      fHgA, fHgB, fHgC,    canv);

  TString canvout("Outer");
  MergeABC(tagA, tagB, tagC,  histOuterA, histOuterB, histOuterC, 
           fHSigmaThetaOuter, fHgOuterA,  fHgOuterB,  fHgOuterC,  canvout);


  //-------------------------------------------  
  // Define the target distributions    fHDiffPixThetaTargetON, ..OFF,  ..MC
  // (fHDiffPixThetaTarget.. will be used in the padding)


  //............   start of new loop over Theta bins   ....................
  for (Int_t j=1; j<=nbinstheta; j++)
  {
    // fraction of A/B/C events to be padded : fracA, fracB, fracC
    Double_t fracA = 0.0;
    Double_t fracB = 0.0;
    Double_t fracC = 0.0;

    fracA = fHgA->Integral(j, j, 1, nbinssig, 1, nbinssig, "");
    fracB = fHgB->Integral(j, j, 1, nbinssig, 1, nbinssig, "");
    if (tagC != "") fracC = fHgC->Integral(j, j, 1, nbinssig, 1, nbinssig, "");

    *fLog << all << "Theta bin j : tagA, fracA, tagB, fracB, tagC, fracC = " 
          << j << ".    " << tagA << ": " << fracA << ";  " << tagB << ": " 
	  << fracB << ";  " << tagC << ": " << fracC << endl; 


    //------------------------------------------------------------------
    // for the current theta bin :
    // define target distribution  'sigma^2-sigmabar^2 vs. pixel number'
    // take the distribution from that sample which has the lowest 'frac.'
    // At present, for all samples (ON, OFF, MC) the same target distribution 
    // is used

    Double_t fracmin = fracA;
    TString tag = tagA;

    if (tagB != "")
    {
      if (fracB < fracmin)
      {
        fracmin = fracB;
        tag = tagB;
      }
      if (tagC != "")
      {
        if (fracC < fracmin)
        {
          fracmin = fracC;
          tag = tagC;
        }
      }
    }

    *fLog << all << "MPad::MergeONOFFMC; thetabin " << j 
          << " : sample with smallest frac = " << tag << endl; 

    TH3D *hist3 = NULL;
    if (tag == "ON")
      hist3 = fHDiffPixThetaON;
    else if (tag == "OFF")
      hist3 = fHDiffPixThetaOFF;
    else if (tag == "MC")
      hist3 = fHDiffPixThetaMC;

    ay = hist3->GetYaxis();
    Int_t nbinspixel = ay->GetNbins();

    TAxis *az = hist3->GetZaxis();
    Int_t nbinsdiff = az->GetNbins();

    for (Int_t k=1; k<=nbinspixel; k++)
    {
      for (Int_t l=1; l<=nbinsdiff; l++)
      {
        Double_t cont = hist3->GetBinContent(j, k,l);
        fHDiffPixThetaTargetON-> SetBinContent(j, k, l, cont);  
        fHDiffPixThetaTargetOFF->SetBinContent(j, k, l, cont);  
        fHDiffPixThetaTargetMC-> SetBinContent(j, k, l, cont);  
      }
    }
  }
  //............   end of new loop over Theta bins   ....................

  *fLog << all 
        << "MPad::MergeONOFFMC(); The target distributions for the padding have been created" 
        << endl;
  *fLog << all << "----------------------------------------------------------" 
        << endl;
  //--------------------------------------------

  fHSigmaTheta->SetDirectory(NULL);
  fHSigmaThetaOuter->SetDirectory(NULL);

  if (tagA == "MC"  ||  tagB == "MC"  ||  tagC == "MC")
  { 
    fHSigmaThetaMC->SetDirectory(NULL);
    fHSigmaThetaOuterMC->SetDirectory(NULL);
    fHDiffPixThetaMC->SetDirectory(NULL);
    fHgMC->SetDirectory(NULL);
    fHgOuterMC->SetDirectory(NULL);
  }

  if (tagA == "ON"  ||  tagB == "ON"  ||  tagC == "ON")
  { 
    fHSigmaThetaON->SetDirectory(NULL);
    fHSigmaThetaOuterON->SetDirectory(NULL);
    fHDiffPixThetaON->SetDirectory(NULL);
    fHgON->SetDirectory(NULL);
    fHgOuterON->SetDirectory(NULL);
  }

  if (tagA == "OFF"  ||  tagB == "OFF"  ||  tagC == "OFF")
  { 
    fHSigmaThetaOFF->SetDirectory(NULL);
    fHSigmaThetaOuterOFF->SetDirectory(NULL);
    fHDiffPixThetaOFF->SetDirectory(NULL);
    fHgOFF->SetDirectory(NULL);
    fHgOuterOFF->SetDirectory(NULL);
  }

  // write the target padding histograms onto a file  ---------
  WritePaddingDist(fileout);     

  //-------------------------------------------------------
  TCanvas *pad = MH::MakeDefCanvas("target", "target", 600, 300); 
  gROOT->SetSelectedPad(NULL);

  pad->Divide(2, 1);

  pad->cd(1);
  gPad->SetBorderMode(0);
  fHSigmaTheta->SetDirectory(NULL);
  fHSigmaTheta->UseCurrentStyle();
  fHSigmaTheta->DrawClone();
  fHSigmaTheta->SetBit(kCanDelete);    

  pad->cd(2);
  gPad->SetBorderMode(0);
  fHSigmaThetaOuter->SetDirectory(NULL);
  fHSigmaThetaOuter->UseCurrentStyle();
  fHSigmaThetaOuter->DrawClone();
  fHSigmaThetaOuter->SetBit(kCanDelete);    

  pad->Draw();

  return kTRUE;
}


// --------------------------------------------------------------------------
//
// Merge the 2D-histograms  (Theta, sigmabar) 
//       hA, hB and hC
//
// - put the merged distribution into hM
// - and define the matrices hgA, hgB and hgC 
//
// These matrices tell which fraction of events should be padded 
// from bin k of sigmabar to bin j
//

Bool_t MPad::MergeABC(TString tA, TString tB, TString tC,
                      TH2D *hA,   TH2D *hB,   TH2D *hC,   TH2D *hM,
                      TH3D *hgA,  TH3D *hgB,  TH3D *hgC, TString canv)
{
  *fLog << all << "MPad::MergeABC();  Entry" << endl;

  TAxis *ax = hM->GetXaxis();
  Int_t nbinstheta = ax->GetNbins();

  TAxis *ay = hM->GetYaxis();
  Int_t nbinssig = ay->GetNbins();

  TArrayD edgesy;
  edgesy.Set(nbinssig+1);
  for (Int_t i=0; i<=nbinssig; i++)
  {
    edgesy[i] = ay->GetBinLowEdge(i+1); 
    //*fLog << all << "i, sigmabar low edge = " << i << ",  " << edgesy[i] 
    //      << endl; 
  }
  MBinning binsg;
  binsg.SetEdges(edgesy);


  //............   loop over Theta bins   ...........................

  //*fLog << all << "MPad::MergeABC(); bins of Theta, Sigmabarold, Sigmabarnew, fraction of events to be padded" << endl;
  for (Int_t l=1; l<=nbinstheta; l++)
  {
    //-------------------------------------------
    // merge A (ON) and B (OFF) distributions
    // input : hista is the normalized 1D distr. of sigmabar for A (ON)  data
    //         histb is the normalized 1D distr. of sigmabar for B (OFF) data
    // output : histap    will be the merged distribution (AB)
    //          fHga(k,j) will tell which fraction of A (ON) events should be 
    //                    padded from bin k to bin j
    //          fHgb(k,j) will tell which fraction of B (OFF) events should be 
    //                    padded from bin k to bin j

    TH2D * fHga = new TH2D;
    MH::SetBinning(fHga, &binsg, &binsg);
    TH2D * fHgb = new TH2D;
    MH::SetBinning(fHgb, &binsg, &binsg);

    TH1D *hista  = hA->ProjectionY("sigon_y", l, l, "");
    TString tita(canv);
    tita += "-A (orig)-";
    tita += l;
    hista->SetNameTitle(tita, tita); 
    Stat_t suma = hista->Integral();
    if (suma != 0.0) hista->Scale(1./suma);

    TH1D *histap  = new TH1D( (const TH1D&)*hista );
    TString titab(canv);
    titab += "-AB (merged)-";
    titab += l;
    histap->SetNameTitle(titab, titab); 

    TH1D *histb  = hB->ProjectionY("sigoff_y", l, l, "");
    TString titb(canv);
    titb += "-B (orig)-";
    titb += l;
    histb->SetNameTitle(titb, titb); 
    Stat_t sumb = histb->Integral();
    if (sumb != 0.0) histb->Scale(1./sumb);

    if (suma == 0.0  ||  sumb == 0.0)
    {
      delete hista;
      delete histb;
      delete fHga;
      delete fHgb;
      delete histap;

      *fLog << err 
            << "cannot call Merge2Distributions(A=" << tA << ", B="
            << tB << ")  for theta bin l = " 
            << l << ";  NevA, NevB = " << suma << ",  " << sumb << endl;

      // clear the corresponding area of fHSigmaTheta
      for (Int_t j=1; j<=nbinssig; j++)
      {
        Double_t a = 0.0;
        hM->SetBinContent(l, j, a);

        //*fLog << "MPad::MergeABC; l, j, a = " << l << ",  " << j << ",  "
        //      << a << endl;
      }

      // go to next theta bin (l)
      continue;
    }

    //*fLog << "call Merge2Distributions(A=" << tA << ", B="
    //      << tB << ")  for theta bin l = "
    //      << l << endl;

    TString canvab(canv);
    canvab += "AB-";
    canvab += l;
    Merge2Distributions(hista, histb, histap, fHga, fHgb, nbinssig, canvab);

    // fill fHgA and fHgB
    for (Int_t k=1; k<=nbinssig; k++)
      for (Int_t j=1; j<=nbinssig; j++)
      {
        Double_t a = fHga->GetBinContent(k,j);
        hgA->SetBinContent(l, k, j, a);

        Double_t b = fHgb->GetBinContent(k,j);
        hgB->SetBinContent(l, k, j, b);
      }


    //-------------------------------------------------
    // if there is no further distribution to be merged
    // fill target distribution fHSigmaTheta 
    // 
    if (!hC)
    {
      for (Int_t j=1; j<=nbinssig; j++)
      {
        Double_t a = histap->GetBinContent(j);
        hM->SetBinContent(l, j, a);

        //*fLog << "MPad::MergeABC; l, j, a = " << l << ",  " << j << ",  "
        //      << a << endl;
      }

      delete fHga;
      delete fHgb;
      delete histap;
    }

    //--------------   next merge (start) ----------------------------------
    if (hC)
    {

    // now merge AB (ON-OFF) and C (MC) distributions
    // input : histe is the result of the merging of A (ON) and B (OFF)
    //                  distributions  
    //         histf is the normalized 1D distr. of sigmabar for C (MC) data
    // output : histep    will be the merged distribution (target distribution)
    //          fHge(k,j) will tell which fraction of A (ON), B (OFF) events 
    //                    should be padded from bin k to bin j
    //          fHgf(k,j) will tell which fraction of C (MC) events should be 
    //                    padded from bin k to bin j

    TH2D * fHge = new TH2D;
    MH::SetBinning(fHge, &binsg, &binsg);
    TH2D * fHgf = new TH2D;
    MH::SetBinning(fHgf, &binsg, &binsg);

    TH1D *histe  = new TH1D( (const TH1D&)*histap);
    TString titabm(canv);
    titabm += "-AB (merged)-";
    titabm += l;
    histe->SetNameTitle(titabm, titabm); 

    TH1D *histep  = new TH1D( (const TH1D&)*histe);
    TString titabcm(canv);
    titabcm +="-ABC (merged)-";
    titabcm += l;
    histep->SetNameTitle(titabcm, titabcm); 

    TH1D *histf  = hC->ProjectionY("sigmc_y", l, l, "");
    TString titc(canv);
    titc += "-C (orig)-";
    titc += l;
    histf->SetNameTitle(titc, titc); 
    Double_t sumf = histf->Integral();
    if (sumf != 0.0) histf->Scale(1./sumf);

    if (sumf == 0.0)
    {
      delete hista;
      delete histb;
      delete fHga;
      delete fHgb;
      delete histap;

      delete histe;
      delete histf;
      delete fHge;
      delete fHgf;
      delete histep;

      *fLog << err 
            << "cannot call Merge2Distributions(AB=" << tA << "-" << tB 
            << ", C=" << tC << ")  for theta bin l = " 
            << l << ";  NevC = " << sumf << endl;

      // go to next theta bin (l)
      continue;
    }

    //*fLog << "call Merge2Distributions(AB=" << tA << "-" << tB 
    //      << ", C=" << tC << ")  for theta bin l = "
    //      << l << endl;

    TString canvabc(canv);
    canvabc += "ABC-";
    canvabc += l;
    Merge2Distributions(histe, histf, histep, fHge, fHgf, nbinssig, canvabc);

    // update fHgA and fHgB
    UpdateHg(fHga, histap, fHge, hgA,  nbinssig, l);
    UpdateHg(fHgb, histap, fHge, hgB, nbinssig, l);

    // fill fHgC
    for (Int_t k=1; k<=nbinssig; k++)
      for (Int_t j=1; j<=nbinssig; j++)
      {
        Double_t f = fHgf->GetBinContent(k,j);
        hgC->SetBinContent(l, k, j, f);
      }

    // fill target distribution fHSigmaTheta 
    // 
    for (Int_t j=1; j<=nbinssig; j++)
    {
      Double_t ep = histep->GetBinContent(j);
      hM->SetBinContent(l, j, ep);

      //*fLog << "MPad::MergeABC; l, j, ep = " << l << ",  " << j << ",  "
      //      << ep << endl;
    }


    //-------------------------------------------

    delete hista;
    delete histb;
    delete fHga;
    delete fHgb;
    delete histap;

    delete histe;
    delete histf;
    delete fHge;
    delete fHgf;
    delete histep;

    }
    //--------------   next merge (end) ----------------------------------

  }
  //............   end of loop over Theta bins   ....................

  *fLog << all << "MPad::MergeABC();  Return" << endl;

  return kTRUE;
}
// --------------------------------------------------------------------------
//
// Merge the sigmabar distributions of 2 samples (samples A and B)
//
// input : the original distributions for samples A and B (hista, histb)
//
// output : the prescription which fraction of events should be padded
//          from the sigmabar bin k to bin j (fHgMC, fHgON, fHgOFF)
//

Bool_t MPad::Merge2Distributions(TH1D *hista, TH1D *histb, TH1D *histap,
                                 TH2D *fHga,  TH2D *fHgb, Int_t nbinssig,
                                 TString canv )
{
  *fLog << all << "MPad::Merge2Distributions();  Entry" << endl;


  // hista is the normalized 1D histogram of sigmabar for sample A
  // histb is the normalized 1D histogram of sigmabar for sample B
  // histc is the difference A-B

  // at the beginning, histap is a copy of hista
  // at the end, it will be the 1D histogram for sample A after padding

  // at the beginning, histbp is a copy of histb
  // at the end, it will be the 1D histogram for sample B after padding

  // at the beginning, histcp is a copy of histc
  // at the end, it should be the difference histap-histbp,
  //             which should be zero

  // fHga[k][j]  tells which fraction of events from sample A should be padded
  //             from sigma_k to sigma_j


  // fHgb[k][j]  tells which fraction of events from sample B should be padded
  //             from sigma_k to sigma_j

  //*fLog << all << "MPad::Merge2Distributions(); Entry" << endl;

  Double_t eps = 1.e-10;

    TH1D *histbp  = new TH1D( (const TH1D&)*histb );

    TH1D *histc   = new TH1D( (const TH1D&)*hista );
    histc->Add(histb, -1.0);

    TH1D *histcp  = new TH1D( (const TH1D&)*histc );    


  //--------   start j loop   ------------------------------------------------
  // loop over bins in histc, 
  // starting from the end (i.e. at the largest sigmabar)
  Double_t v, s, w, t, x, u, a, b, arg;

  //*fLog << "Content of histcp : " << endl;

  for (Int_t j=nbinssig; j >= 1; j--)
  {
    //*fLog << "j, hista, histb , histap : " << j << ",  " 
    //                              <<  hista->GetBinContent(j) << ",  "
    //                              <<  histb->GetBinContent(j) << ",  "
    //                              <<  histap->GetBinContent(j) << endl;

    v = histcp->GetBinContent(j);
    //*fLog << "cp : j, v = " << j << ",  " << v << endl;

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

      // if s==t bin k cannot compensate, go to next k bin
      if (s == t) continue;

      x = v + w;
      if (x >= 0.0) 
        u = 1.0;
      else
        u = -1.0;

      // if u==s bin k will partly compensate : pad the whole fraction
      // w from bin k to bin j
      if (u == s)
        arg = -w;

      // if u!=s bin k would overcompensate : pad only the fraction
      // v from bin k to bin j
      else
        arg = v;

      if (arg <=0.0)
      {
        fHga->SetBinContent(k, j, -arg);
        fHgb->SetBinContent(k, j,  0.0);
      }
      else
      {
        fHga->SetBinContent(k, j,  0.0);
        fHgb->SetBinContent(k, j,  arg);
      }

      //*fLog << all << "k, j, arg = " << k << ",  " << j 
      //      << ",  " << arg << endl;

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

      if (u == s)
      {
        histcp->SetBinContent(k, 0.0);
        histcp->SetBinContent(j,   x);

        v = histcp->GetBinContent(j);
        if ( fabs(v) < eps ) break;

        // bin j was only partly compensated : go to next k bin
        continue;
      }
      else
      {
        histcp->SetBinContent(k,   x);
        histcp->SetBinContent(j, 0.0);

        // bin j was completely compensated : go to next j bin
        break;
      }

    }
    //................   end k loop   ......................................
  } 
  //--------   end j loop   ------------------------------------------------

  // check results 

  //*fLog << "Content of histap, histbp, histcp : " << endl;

  for (Int_t j=1; j<=nbinssig; j++)
  {
    Double_t a = histap->GetBinContent(j);
    Double_t b = histbp->GetBinContent(j);
    Double_t c = histcp->GetBinContent(j);

    //*fLog << "j, a, b, c = " << j << ":  " << a << ",  " << b << ",  "
    //      << c << endl;

    if( fabs(a-b)>3.0*eps  ||  fabs(c)>3.0*eps )
      *fLog << err << "MPad::Merge2Distributions(); inconsistency in results; j, a, b, c = "
            << j << ",  " << a << ",  " << b << ",  " << c << endl;
  }

  //---------------------------------------------------------------
  TCanvas *pad = MH::MakeDefCanvas(canv, canv, 600, 600); 
  gROOT->SetSelectedPad(NULL);

  pad->Divide(2, 2);

  pad->cd(1);
  gPad->SetBorderMode(0);
  hista->SetDirectory(NULL);
  hista->UseCurrentStyle();
  hista->DrawClone();
  hista->SetBit(kCanDelete);    

  pad->cd(2);
  gPad->SetBorderMode(0);
  histb->SetDirectory(NULL);
  histb->UseCurrentStyle();
  histb->DrawClone();
  histb->SetBit(kCanDelete);    

  pad->cd(3);
  gPad->SetBorderMode(0);
  histap->SetDirectory(NULL);
  histap->UseCurrentStyle();
  histap->DrawClone();
  histap->SetBit(kCanDelete);    

  pad->Draw();

  //--------------------------------------------------------------------

  delete histc;
  delete histbp;
  delete histcp;

  *fLog << all << "MPad::Merge2Distributions();  Return" << endl;

  return kTRUE;
}



// --------------------------------------------------------------------------
//
// Update the matrix fHgA
// which contains the final padding prescriptions
//

Bool_t MPad::UpdateHg(TH2D *fHga, TH1D *histap, TH2D *fHge, TH3D *fHgA,
                      Int_t nbinssig, Int_t l)
{
  // histap  target distribution after ON-OFF merging
  // fHga    padding prescription for ON (or OFF) data to get to histap
  // fHge    padding prescription to get from histap to the target
  //         distribution after the ON-OFF-MC merging
  // fHgA    updated padding prescription for ON (or OFF) data to get
  //         from the original ON (or OFF) histogram to the target
  //         distribution after the ON-OFF-MC merging
  // l       Theta bin

  Double_t eps = 1.e-10;

  for (Int_t k=1; k<=nbinssig; k++)
  {
    Double_t na  = fHga->Integral(1, nbinssig, k, k, " ");
    Double_t ne  = fHge->Integral(k, k, 1, nbinssig, " ");
    Double_t nap = histap->GetBinContent(k);

    if (ne <= eps) 
    {
      // go to next k
      continue;
    }

    Double_t r = nap - na;

    if (r >= ne-eps)
    {
      for (Int_t j=k+1; j<=nbinssig; j++)
      {
        Double_t e = fHge->GetBinContent(k,j);
        Double_t A = fHgA->GetBinContent(l,k,j);
        fHgA->SetBinContent(l, k, j, A + e);
      }
      // go to next k
      continue;
    }

    for (Int_t j=k+1; j<=nbinssig; j++)
    {
      Double_t e = fHge->GetBinContent(k,j);
      Double_t A = fHgA->GetBinContent(l,k,j);
      fHgA->SetBinContent(l, k, j, A + r*e/ne);
    }

    if (na <= eps) 
    {
      // go to next k
      continue;
    }

    for (Int_t i=1; i<k; i++)
    {
      Double_t a = fHga->GetBinContent(i,k);
      Double_t A = fHgA->GetBinContent(l,i,k);
      fHgA->SetBinContent(l, i, k, A - (ne-r)*a/na);
    }

    for (Int_t i=1; i<=nbinssig; i++)
    {
      if (i == k) continue;
      for (Int_t j=i+1; j<=nbinssig; j++)
      {
        if (j == k) continue;
        Double_t a = fHga->GetBinContent(i,k);
        Double_t e = fHge->GetBinContent(k,j);
        Double_t A = fHgA->GetBinContent(l,i,j);
        fHgA->SetBinContent(l, i, j, A + (ne-r)*a*e/(na*ne)  );
      }
    }
  }
  
  return kTRUE; 
}

// --------------------------------------------------------------------------
//
// Write padding distributions onto a file
//       these are the histograms produced in the member function
//       MergeONOFFMC
//       plus the distributions produced by MMakePadHistograms
//
Bool_t MPad::WritePaddingDist(TString namefileout)
{
  *fLog << all << "MPad::WritePaddingDist();  Padding distributions for  "; 

  TFile outfile(namefileout, "RECREATE");

  fHSigmaTheta->Write();
  fHSigmaThetaOuter->Write();

  if (fHSigmaThetaMC)
  {
    *fLog << all << " MC ";
    fHSigmaThetaMC->Write();
    fHSigmaThetaOuterMC->Write();
    fHDiffPixThetaMC->Write();
    fHDiffPixThetaTargetMC->Write();
    fHgMC->Write();
    fHgOuterMC->Write();
  }

  if (fHSigmaThetaON)
  {
    *fLog << all << " ON ";
    fHSigmaThetaON->Write();
    fHSigmaThetaOuterON->Write();
    fHDiffPixThetaON->Write();
    fHDiffPixThetaTargetON->Write();
    fHgON->Write();
    fHgOuterON->Write();
  }

  if (fHSigmaThetaOFF)
  {
    *fLog << all << " OFF ";
    fHSigmaThetaOFF->Write();
    fHSigmaThetaOuterOFF->Write();
    fHDiffPixThetaOFF->Write();
    fHDiffPixThetaTargetOFF->Write();
    fHgOFF->Write();
    fHgOuterOFF->Write();
  }

  *fLog << all << "  data were written onto file " 
        << namefileout << endl;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read padding distributions from a file
//      thes are the distributions which were written out by
//      the member function WritePaddingDist
//
Bool_t MPad::ReadPaddingDist(TString namefilein)
{
  *fLog << all << "MPad : Read padding histograms from file " 
        << namefilein << endl;

  Int_t OK = 0;

  fInfile = new TFile(namefilein);
  fInfile->ls();

    //------------------------------------
  
      fHSigmaTheta = new TH2D; 
      OK = fHSigmaTheta->Read("2D-ThetaSigmabar");
      if (OK) 
      {
        *fLog << all 
              << "MPad : Object '2D-ThetaSigmabar' was read in" << endl;
      }

      fHSigmaThetaOuter = new TH2D; 
      OK =fHSigmaThetaOuter->Read("2D-ThetaSigmabarOuter");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '2D-ThetaSigmabarOuter' was read in" << endl;
      }


    //------------------------------------

      fHSigmaThetaMC = new TH2D; 
      OK = fHSigmaThetaMC->Read("2D-ThetaSigmabarMC");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '2D-ThetaSigmabarMC' was read in" << endl;
      }

      fHSigmaThetaOuterMC = new TH2D; 
      OK = fHSigmaThetaOuterMC->Read("2D-ThetaSigmabarOuterMC");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '2D-ThetaSigmabarOuterMC' was read in" << endl;
      }

      fHDiffPixThetaMC = new TH3D;
      OK = fHDiffPixThetaMC->Read("3D-ThetaPixDiffMC");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-ThetaPixDiffMC' was read in" << endl;
      }

      fHDiffPixThetaTargetMC = new TH3D;
      OK = fHDiffPixThetaTargetMC->Read("3D-ThetaPixDiffTargetMC");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-ThetaPixDiffTargetMC' was read in" << endl;
      }

      fHgMC = new TH3D;
      OK = fHgMC->Read("3D-PaddingMatrixMC");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-PaddingMatrixMC' was read in" << endl;
      }

      fHgOuterMC = new TH3D;
      OK = fHgOuterMC->Read("3D-PaddingMatrixOuterMC");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-PaddingMatrixOuterMC' was read in" << endl;
      }

    //------------------------------------

      fHSigmaThetaON = new TH2D;
      OK =fHSigmaThetaON->Read("2D-ThetaSigmabarON");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '2D-ThetaSigmabarON' was read in" << endl;
      }

      fHSigmaThetaOuterON = new TH2D;
      OK = fHSigmaThetaOuterON->Read("2D-ThetaSigmabarOuterON");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '2D-ThetaSigmabarOuterON' was read in" << endl;
      }

      fHDiffPixThetaON = new TH3D;
      OK = fHDiffPixThetaON->Read("3D-ThetaPixDiffON");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-ThetaPixDiffON' was read in" << endl;
      }

      fHDiffPixThetaTargetON = new TH3D;
      OK = fHDiffPixThetaTargetON->Read("3D-ThetaPixDiffTargetON");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-ThetaPixDiffTargetON' was read in" << endl;
      }

      fHgON = new TH3D;
      OK = fHgON->Read("3D-PaddingMatrixON");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-PaddingMatrixON' was read in" << endl;
      }

      fHgOuterON = new TH3D;
      OK = fHgOuterON->Read("3D-PaddingMatrixOuterON");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-PaddingMatrixOuterON' was read in" << endl;
      }

    //------------------------------------

      fHSigmaThetaOFF = new TH2D;
      OK = fHSigmaThetaOFF->Read("2D-ThetaSigmabarOFF");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '2D-ThetaSigmabarOFF' was read in" << endl;
      }

      fHSigmaThetaOuterOFF = new TH2D;
      OK = fHSigmaThetaOuterOFF->Read("2D-ThetaSigmabarOuterOFF");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '2D-ThetaSigmabarOuterOFF' was read in" << endl;
      }

      fHDiffPixThetaOFF = new TH3D;
      OK = fHDiffPixThetaOFF->Read("3D-ThetaPixDiffOFF");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-ThetaPixDiffOFF' was read in" << endl;
      }

      fHDiffPixThetaTargetOFF = new TH3D;
      OK = fHDiffPixThetaTargetOFF->Read("3D-ThetaPixDiffTargetOFF");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-ThetaPixDiffTargetOFF' was read in" << endl;
      }

      fHgOFF = new TH3D;
      OK = fHgOFF->Read("3D-PaddingMatrixOFF");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-PaddingMatrixOFF' was read in" << endl;
      }

      fHgOuterOFF = new TH3D;
      OK = fHgOuterOFF->Read("3D-PaddingMatrixOuterOFF");
      if (OK)
      {
        *fLog << all 
              << "MPad : Object '3D-PaddingMatrixOuterOFF' was read in" << endl;
      }

    //------------------------------------

  return kTRUE;
}




// --------------------------------------------------------------------------
//
//  Set the pointers and prepare the histograms
//
Int_t MPad::PreProcess(MParList *pList)
{
  if ( !fHSigmaThetaMC     && !fHSigmaThetaON     && !fHSigmaThetaOFF      ||  
       !fHDiffPixThetaMC   && !fHDiffPixThetaON   && !fHDiffPixThetaOFF    ||
       !fHgMC              && !fHgON              && !fHgOFF                 )
  { 
       *fLog << err 
             << "MPad : There are no input histograms for the padding ... aborting." 
             << endl;
       return kFALSE;
  }

  fPointPos = (MPointingPos*)pList->FindObject("MPointingPos");
  if (!fPointPos)
    {
       *fLog << err << dbginf << "MPad : MPointingPos not found... aborting." 
             << endl;
       return kFALSE;
    }
  
    fPed = (MPedPhotCam*)pList->FindObject(AddSerialNumber(fNamePedPhotCam), "MPedPhotCam");
    if (!fPed)
    {
        *fLog << err << AddSerialNumber(fNamePedPhotCam) 
              << "[MPedPhotCam] not found... aborting." << endl;
        return kFALSE;
    }    
    *fLog << all << "MPad::PreProcess();  name of MPedPhotCam container = "
	  << fNamePedPhotCam << endl;  
   

   fCam = (MGeomCam*)pList->FindObject("MGeomCam");
   if (!fCam)
     {
       *fLog << err 
             << "MPad : MGeomCam not found (no geometry information available)... aborting." 
             << endl;
       return kFALSE;
     }
  
   fEvt = (MCerPhotEvt*)pList->FindObject("MCerPhotEvt");
   if (!fEvt)
     {
       *fLog << err << "MPad : MCerPhotEvt not found... aborting." 
             << endl;
       return kFALSE;
     }


   fBad = (MBadPixelsCam*)pList->FindObject("MBadPixelsCam");
   if (!fBad)
     {
       *fLog << inf 
             << "MPad : MBadPixelsCam container not present... continue." 
             << endl;
     }

   
   if (fType !="ON"  &&  fType !="OFF"  &&  fType !="MC")
     {
       *fLog << err 
             << "MPad : Type of data to be padded not defined... aborting." 
             << endl;
       return kFALSE;
     }


   //--------------------------------------------------------------------
   // histograms for checking the padding
   //
   // plot pedestal sigmas
   fHSigmaPedestal = new TH2D("SigPed","Sigma: after vs. before padding", 
                     100, 0.0, 75.0, 100, 0.0, 75.0);
   fHSigmaPedestal->SetXTitle("Pedestal sigma before padding");
   fHSigmaPedestal->SetYTitle("Pedestal sigma after padding");
   fHSigmaPedestal->SetDirectory(NULL);
   fHSigmaPedestal->UseCurrentStyle();
   fHSigmaPedestal->GetYaxis()->SetTitleOffset(1.25);

   // plot no.of photons (before vs. after padding) 
   fHPhotons = new TH2D("Photons/area",
                        "Photons/area: after vs.before padding", 
                        100, -100.0, 300.0, 100, -100, 300);
   fHPhotons->SetXTitle("no.of photons/area before padding");
   fHPhotons->SetYTitle("no.of photons/area after padding");
   fHPhotons->SetDirectory(NULL);
   fHPhotons->UseCurrentStyle();
   fHPhotons->GetYaxis()->SetTitleOffset(1.25);

   // plot of added NSB
   fHNSB = new TH1D("NSB/area","Distribution of added NSB/area", 
                    100, -100.0, 200.0);
   fHNSB->SetXTitle("no.of added NSB photons/area");
   fHNSB->SetYTitle("no.of pixels");
   fHNSB->SetDirectory(NULL);
   fHNSB->UseCurrentStyle();
   fHNSB->GetYaxis()->SetTitleOffset(1.25);

   //--------------------------------------------------------------------

  *fLog << inf << "Type of data to be padded : " << fType << endl; 
  *fLog << inf << "Name of MPedPhotCam container : " << fNamePedPhotCam 
        << endl; 

   fIter = 10;

   memset(fInf,      0, sizeof(fInf));
   memset(fErrors,   0, sizeof(fErrors));
   memset(fWarnings, 0, sizeof(fWarnings));

   return kTRUE;
}

// --------------------------------------------------------------------------
//
// Do the Padding (noise adjustment)
//
// input for the padding : 
//  - the matrices fHgON, fHgOFF, fHgMC
//  - the original distributions fHSigmaTheta, fHDiffPixTheta
//

Int_t MPad::Process()
{
  //*fLog << all << "Entry MPad::Process();" << endl;

  // this is the conversion factor from the number of photons
  //                                 to the number of photo electrons
  // later to be taken from MCalibration
  // fPEperPhoton = PW * LG * QE * 1D
  Double_t fPEperPhoton = 0.92 * 0.94 * 0.23 * 0.9;   //  (= 0.179)

  //------------------------------------------------
  // Add pixels to MCerPhotEvt which are not yet in;
  // set their number of photons equal to zero. 
  // Purpose : by the padding the number of photons is changed
  // so that cleaning and cuts may be affected.
  // However, no big effect is expectec unless the padding is strong
  // (strong increase of the noise level)
  // At present comment out this code

  /*
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
  */


  //-----------------------------------------
  Int_t rc=0;
  Int_t rw=0;

  const UInt_t npix = fEvt->GetNumPixels();

  //*fLog << all << "MPad::Process(); before padding : " << endl;

  //-------------------------------------------
  // Calculate average sigma of the event
  //
  fPed->ReCalc(*fCam, fBad);
  //*fLog << "pedestalRMS, inner and outer = " << (fPed->GetArea(0)).GetRms()
  //      << ",  " << (fPed->GetArea(1)).GetRms() << endl;


  // inner sigmabar/sqrt(area)
  Double_t ratioinn = fCam->GetPixRatio(0);
  Double_t sigbarInnerold_phot = (fPed->GetArea(0)).GetRms();
  Double_t sigbarInnerold  = sigbarInnerold_phot * fPEperPhoton * sqrt(ratioinn);
  Double_t sigbarInnerold2 = sigbarInnerold*sigbarInnerold;

  // outer sigmabar/sqrt(area)
  Double_t ratioout = fCam->GetPixRatio(500);
  Double_t sigbarOuterold_phot = (fPed->GetArea(1)).GetRms();
  Double_t sigbarOuterold  = sigbarOuterold_phot * fPEperPhoton * sqrt(ratioout);
  Double_t sigbarOuterold2 = sigbarOuterold*sigbarOuterold;

  const Double_t theta = fPointPos->GetZd();

  //*fLog << all << "theta = " << theta << endl;

  Int_t binTheta = fHSigmaTheta->GetXaxis()->FindBin(theta);
  if ( binTheta < 1  ||  binTheta > fHSigmaTheta->GetNbinsX() )
  {
    *fLog << warn 
          << "MPad::Process(); binNumber out of range : theta, binTheta = "
          << theta << ",  " << binTheta << endl;

    rc = 2;
    fErrors[rc]++;
    return kCONTINUE;
  }

  //*fLog << all << "binTheta = " << binTheta << endl;
  

  //-------------------------------------------
  // get number of events in this theta bin               (nTheta)
  // and number of events in this sigbarInnerold_phot bin (nSigma)

  Double_t nTheta;
  Double_t nSigma;

  TH2D *st = NULL;
  if      (fType == "MC")  st = fHSigmaThetaMC;
  else if (fType == "ON")  st = fHSigmaThetaON;
  else if (fType == "OFF") st = fHSigmaThetaOFF;
  else
  {
    *fLog << err << "MPad : illegal data type '" << fType 
          <<  "'" << endl;
    return kERROR;  
  }

  TH1D *hn;
  hn = st->ProjectionY("", binTheta, binTheta, "");

  //*fLog << "Title of histogram : " << st->GetTitle() << endl;
  //for (Int_t i=1; i<=hn->GetNbinsX(); i++)
  //{
  //  *fLog << "bin " << i << " : no.of entries = " << hn->GetBinContent(i)
  //        << endl;
  //}

  nTheta = hn->Integral();
  Int_t binSigma = hn->FindBin(sigbarInnerold_phot);
  nSigma = hn->GetBinContent(binSigma);

  //*fLog << all           
  //       << "Theta, sigbarold_phot, binTheta, binSigma, nTheta, nSigma = "
  //       << theta << ",  " << sigbarInnerold_phot << ",  " << binTheta 
  //       << ",  "
  //       << binSigma << ",  " << nTheta << ",  " << nSigma   << endl;      

  delete hn;
 
  //-------------------------------------------

  //******************************************************************
  // has the event to be padded ?
  // if yes : to which sigmabar should it be padded ?
  //

  TH3D *Hg = NULL;
  if      (fType == "MC")  Hg = fHgMC;
  else if (fType == "ON")  Hg = fHgON;
  else if (fType == "OFF") Hg = fHgOFF;
  else
  {
    *fLog << err << "MPad : illegal type of data '" << fType << "'"
          << endl;
    return kERROR;
  }

  Int_t binSig = Hg->GetYaxis()->FindBin(sigbarInnerold_phot);
  //*fLog << all << "binSig, sigbarInnerold_phot = " << binSig << ",  " 
  //        << sigbarInnerold_phot << endl;

  Double_t prob;
  TH1D *hpad = NULL;


  hpad = Hg->ProjectionZ("zON", binTheta, binTheta, binSig, binSig, "");

  //Int_t nb = hpad->GetNbinsX();
  //for (Int_t i=1; i<=nb; i++)
  //{
  //  if (hpad->GetBinContent(i) != 0.0)
  //    *fLog << all << "i, Hg = " << i << ",  " 
  //          << hpad->GetBinContent(i) << endl;
  //}

  if (nSigma != 0.0)
    prob = hpad->Integral() * nTheta/nSigma;
  else
    prob = 0.0;

  //*fLog << all << "nTheta, nSigma, prob = " << nTheta << ",  " << nSigma 
  //       << ",  " << prob << endl;

  if ( prob <= 0.0  ||  gRandom->Uniform() > prob )
  {
    delete hpad;
    // event should not be padded
    //*fLog << all << "event is not padded" << endl;

    rc = 0;
    fInf[rc]++;
    return kTRUE;
  }
  // event should be padded
  //*fLog << all << "event will be padded" << endl;  


  //-------------------------------------------
  // for the current theta, generate a sigmabar_inner :
  //     for MC/ON/OFF data according to the matrix fHgMC/ON/OFF
  //
  Double_t sigmabarInner_phot = 0;
  Double_t sigmabarInner      = 0;

  
  //Int_t nbinsx = hpad->GetNbinsX();
  //for (Int_t i=1; i<=nbinsx; i++)
  //{
  //  if (hpad->GetBinContent(i) != 0.0)
  //    *fLog << all << "i, fHg = " << i << ",  " << hpad->GetBinContent(i) 
  //          << endl;
  //}

  sigmabarInner_phot = hpad->GetRandom();
  sigmabarInner = sigmabarInner_phot * fPEperPhoton * sqrt(ratioinn); 

  //*fLog << all << "sigmabarInner_phot = " << sigmabarInner_phot << endl;

  delete hpad;
  
  // new inner sigmabar2/area
  const Double_t sigmabarInner2 = sigmabarInner*sigmabarInner;

  //*fLog << all << "MPad::Process(); sigbarInnerold, sigmabarInner = " 
  //      << sigbarInnerold << ",  "<< sigmabarInner << endl;

  // Stop if target sigmabar is <= sigbarold
  if (sigmabarInner <= sigbarInnerold)
  {
    *fLog << all << "MPad::Process(); target sigmabarInner is less than sigbarInnerold : "
          << sigmabarInner << ",  " << sigbarInnerold << ",   aborting" 
          << endl;

    rc = 4;
    fErrors[rc]++;
    return kCONTINUE;     
  }

  //-------------------------------------------
  // estimate a sigmabar_outer from sigmabar_inner :
  // using the target distributions fHSigmaTheta and fHSigmaThetaOuter
  // for sigmabar(inner) and sigmabar(outer)

  Double_t innerMeantarget = 0.0;
  Double_t outerMeantarget = 0.0;
  Double_t innerRMStarget  = 0.0;
  Double_t outerRMStarget  = 0.0;

  // projection doesn't work
  //TH1D *hi = fHSigmaTheta->ProjectionY("proinner", binTheta, binTheta, "e");
  //TH1D *ho = fHSigmaThetaOuter->ProjectionY("proouter", binTheta, binTheta, "e");
  //sigmabarOuter2 =   sigmabarInner2   + fPEperPhoton*fPEperPhoton * 
  //                 (   ho->GetMean()*ho->GetMean()*ratioout 
  //                   - hi->GetMean()*hi->GetMean()*ratioinn);

  //innerMeantarget = hi->GetMean()*sqrt(ratioinn)*fPEperPhoton;
  //outerMeantarget = ho->GetMean()*sqrt(ratioout)*fPEperPhoton;
  //innerRMStarget  = hi->GetRMS(1)*sqrt(ratioinn)*fPEperPhoton;
  //outerRMStarget  = ho->GetRMS(1)*sqrt(ratioout)*fPEperPhoton;

  Double_t y      = 0.0;
  Double_t ybar   = 0.0;
  Double_t y2bar  = 0.0;
  Double_t w      = 0.0;

  Double_t isum   = 0.0;
  Double_t isumy  = 0.0;
  Double_t isumy2 = 0.0;
  for (Int_t i=1; i<=fHSigmaTheta->GetNbinsY(); i++)
  {
    w = fHSigmaTheta->GetBinContent(binTheta, i);
    y = fHSigmaTheta->GetYaxis()->GetBinCenter(i);
    isum   += w;
    isumy  += w * y;
    isumy2 += w * y*y;
  }
  if (isum == 0.0)
  {
    innerMeantarget = 0.0;
    innerRMStarget  = 0.0;
  }    
  else
  {
    ybar  = isumy /isum;
    y2bar = isumy2/isum;
    innerMeantarget = ybar                      * sqrt(ratioinn)*fPEperPhoton;
    innerRMStarget  = sqrt( y2bar - ybar*ybar ) * sqrt(ratioinn)*fPEperPhoton;
  }

  Double_t osum   = 0.0;
  Double_t osumy  = 0.0;
  Double_t osumy2 = 0.0;
  for (Int_t i=1; i<=fHSigmaThetaOuter->GetNbinsY(); i++)
  {
    w = fHSigmaThetaOuter->GetBinContent(binTheta, i);
    y = fHSigmaThetaOuter->GetYaxis()->GetBinCenter(i);
    osum   += w;
    osumy  += w * y;
    osumy2 += w * y*y;
  }
  if (osum == 0.0)
  {
    outerMeantarget = 0.0;
    outerRMStarget  = 0.0;
  }    
  else
  {
    ybar  = osumy /osum;
    y2bar = osumy2/osum;
    outerMeantarget = ybar                      * sqrt(ratioout)*fPEperPhoton;
    outerRMStarget  = sqrt( y2bar - ybar*ybar ) * sqrt(ratioout)*fPEperPhoton;
  }


  // new outer sigmabar2/area
  Double_t sigmabarOuter2;

  Double_t scal = ( innerMeantarget*innerRMStarget == 0.0   ||
                    outerMeantarget*outerRMStarget == 0.0 )    ?  1.0 :
          (outerMeantarget*outerRMStarget)/(innerMeantarget*innerRMStarget);
  sigmabarOuter2 = outerMeantarget*outerMeantarget +
                   scal * (sigmabarInner2 - innerMeantarget*innerMeantarget);

  //*fLog << "innerMeantarget, innerRMStarget = " << innerMeantarget 
  //      << ",  " << innerRMStarget << endl;

  //*fLog << "outerMeantarget, outerRMStarget = " << outerMeantarget 
  //      << ",  " << outerRMStarget << endl;

  //*fLog << "sigmabarInner2, sigmabarOuter2, scal = " << sigmabarInner2 
  //      << ",  " << sigmabarOuter2 << ",  " << scal << endl;

  //delete hi;
  //delete ho;

  //-------------------------------------------
  //
  // Calculate average number of NSB photo electrons to be added (lambdabar)
  // from the value of sigmabar, 
  //  - using a fixed value (F2excess)  for the excess noise factor
  
  Double_t elNoise2;         // [photo electrons]  
  Double_t F2excess  = 1.3;
  Double_t lambdabar;        // [photo electrons]

  //----------------
  TH3D *sp = NULL;
  if      (fType == "MC")  sp = fHDiffPixThetaTargetMC;
  else if (fType == "ON")  sp = fHDiffPixThetaTargetON;
  else if (fType == "OFF") sp = fHDiffPixThetaTargetOFF;

  //----------------

  Int_t bincheck = sp->GetXaxis()->FindBin(theta);
  if (binTheta != bincheck)
  {
    *fLog << err 
          << "MPad::Process(); The binnings of the 2 histograms are not identical; aborting"
          << endl;
    return kERROR;
  }

  // In this Theta bin, get RMS of (Sigma^2-sigmabar^2)/area for inner pixels.
  // The average electronic noise (to be added) has to be in the order of
  // this RMS, otherwise the electronic noise of an individual pixel 
  // (elNoise2Pix) may become negative

  //----------------


  // Attention : maximum ID of inner pixels hard coded !!!
  Int_t idmaxpixinner = 396;
  Int_t binmaxpixinner = 
        sp->GetYaxis()->FindBin( (Double_t)idmaxpixinner );

  TH1D *hnoise = NULL;
    hnoise = sp->ProjectionZ("", binTheta, binTheta, 0, binmaxpixinner, "");

  Double_t RMS_phot = hnoise->GetRMS(1);  
  Double_t RMS = RMS_phot * fPEperPhoton * fPEperPhoton;
  delete hnoise;

  elNoise2 = TMath::Min(2.0*RMS,  sigmabarInner2 - sigbarInnerold2);
  //*fLog << all << "RMS_phot, elNoise2 = " << RMS_phot << ",  "
  //      << elNoise2 << endl; 

  lambdabar = (sigmabarInner2 - sigbarInnerold2 - elNoise2) / F2excess;  

  if (lambdabar <= 0.0)
  {
    rc = 3;
    fErrors[rc]++;
  }

  //*fLog << "lambdabar = " << lambdabar << endl;

  // This value of lambdabar is the same for all pixels;
  // note that lambdabar is the NSB p.e. density

  //----------   start loop over pixels   ---------------------------------
  // do the padding for each pixel
  //
  // pad only pixels   - which are used 
  //

  Double_t sigma2      = 0;

  Double_t diff_phot   = 0;
  Double_t diff        = 0;

  Double_t addSig2_phot= 0;
  Double_t addSig2     = 0;

  Double_t elNoise2Pix = 0;


  // throw the Sigma for the pixels from the distribution fHDiffPixTheta
  // MC  : from fHDiffPixThetaTargetMC
  // ON  : from fHDiffPixThetaTaregtON
  // OFF : from fHDiffPixThetaTargetOFF


  Double_t sigbarold2;
  Double_t sigmabar2;

  for (UInt_t i=0; i<npix; i++) 
  {   

    MCerPhotPix &pix = (*fEvt)[i];
    if ( !pix.IsPixelUsed() )
      continue;

    //if ( pix.GetNumPhotons() == 0.0)
    //{
    //  *fLog << warn 
    //        << "MPad::Process(); no.of photons is 0 for used pixel" 
    //        << endl;
    //  continue;
    //}

    Int_t j = pix.GetPixId();

    // GetPixRatio returns (area of pixel 0 / area of current pixel)
    Double_t ratio = fCam->GetPixRatio(j);

    if (ratio > 0.5)
    {
      sigbarold2 = sigbarInnerold2;
      sigmabar2  = sigmabarInner2;
    }
    else
    {
      sigbarold2 = sigbarOuterold2;
      sigmabar2  = sigmabarOuter2;
    }

    MPedPhotPix &ppix = (*fPed)[j];

    // count number of pixels treated
    fWarnings[0]++;


    Double_t oldsigma_phot = ppix.GetRms();
    Double_t oldsigma = oldsigma_phot * fPEperPhoton * sqrt(ratio);
    Double_t oldsigma2 = oldsigma*oldsigma;

    //---------------------------------
    // throw the Sigma for this pixel 
    //
    Int_t binPixel = sp->GetYaxis()->FindBin( (Double_t)j );

    Int_t count;
    Bool_t ok;

    TH1D *hdiff = NULL;

     hdiff = sp->ProjectionZ("", binTheta, binTheta,
                                 binPixel, binPixel, "");
     Double_t integral =  hdiff->Integral();
     // if there are no entries in hdiff, diff cannot be thrown
     // in this case diff will be set to the old difference
     if ( integral == 0 )
     {
       //*fLog << warn << "MPad::Process(); fType = " << fType 
       //      << ", projection of '"
       //      << sp->GetName() << "' for Theta bin " 
       //      << binTheta << "  and pixel " << j  
       //      << " has no entries; set diff equal to the old difference  " 
       //      << endl;

       diff = TMath::Max(oldsigma2 - sigbarold2,
                         lambdabar*F2excess + oldsigma2 - sigmabar2);

       rc = 2;
       fWarnings[rc]++;
     }

     // start of else -------------------
     else
     {
       count = 0;
       ok = kFALSE;
       for (Int_t m=0; m<fIter; m++)
       {
         count += 1;
         diff_phot = hdiff->GetRandom();

         //*fLog << "after GetRandom : j, m, diff_phot = " << j << " : "
         //      << m << ",  " << diff_phot << endl;

         diff = diff_phot * fPEperPhoton * fPEperPhoton;
 
         // the following condition ensures that elNoise2Pix > 0.0 
         if ( (diff + sigmabar2 - oldsigma2
                                - lambdabar*F2excess) > 0.0 )
         {
           ok = kTRUE;
           break;
         }
       }

       if (!ok)
       {
         //*fLog << all << "theta, j, count, sigmabar2, diff, oldsigma2, ratio, lambdabar = " 
         //      << theta << ",  " 
         //      << j << ",  " << count << ",  " << sigmabar2 << ",  " 
         //      << diff << ",  " << oldsigma2 << ",  " << ratio << ",  "
         //      << lambdabar << endl;
         diff = lambdabar*F2excess + oldsigma2 - sigmabar2; 

         rw = 1;
         fWarnings[rw]++;
       }
     }
     // end of else --------------------

    delete hdiff;
    sigma2 = diff + sigmabar2;


    //---------------------------------
    // get the additional sigma^2 for this pixel (due to the padding)

    addSig2 = (sigma2 - oldsigma2) / ratio;
    addSig2_phot = addSig2 / (fPEperPhoton * fPEperPhoton);

    //---------------------------------
    // get the additional electronic noise for this pixel

    elNoise2Pix = addSig2 - lambdabar*F2excess/ratio;


    //---------------------------------
    // throw actual number of additional NSB photo electrons (NSB)
    //       and its RMS (sigmaNSB) 

    Double_t NSB0 = gRandom->Poisson(lambdabar/ratio);
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
        *fLog << warn << "MPad::Process(); argument of sqrt < 0 : "
              << arg << endl;
      }
    }


    //---------------------------------
    // smear NSB0 according to sigmaNSB0
    // and subtract lambdabar because of AC coupling

    Double_t NSB = gRandom->Gaus(NSB0, sigmaNSB0) - lambdabar/ratio;
    Double_t NSB_phot = NSB / fPEperPhoton;

    //---------------------------------
 
    // add additional NSB to the number of photons
    Double_t oldphotons_phot = pix.GetNumPhotons();
    Double_t oldphotons = oldphotons_phot * fPEperPhoton;
    Double_t newphotons = oldphotons + NSB;
    Double_t newphotons_phot = newphotons / fPEperPhoton;    
    pix.SetNumPhotons(	newphotons_phot );


    fHNSB->Fill( NSB_phot*ratio );
    fHPhotons->Fill( oldphotons_phot*ratio, 
                     newphotons_phot*ratio );


    // error: add sigma of padded noise quadratically
    Double_t olderror_phot = pix.GetErrorPhot();
    Double_t newerror_phot = 
                           sqrt( olderror_phot*olderror_phot + addSig2_phot );
    pix.SetErrorPhot( newerror_phot );


    Double_t newsigma = sqrt( sigma2 / ratio ); 
    Double_t newsigma_phot = newsigma / fPEperPhoton; 
    ppix.SetRms( newsigma_phot );

    fHSigmaPedestal->Fill( oldsigma_phot, newsigma_phot );
  } 
  //----------   end of loop over pixels   ---------------------------------


  //*fLog << all << "MPad::Process(); after padding : " << endl;

  // Calculate sigmabar again and crosscheck
  fPed->ReCalc(*fCam, fBad);
  //*fLog << "pedestalRMS, inner and outer = " << (fPed->GetArea(0)).GetRms()
  //      << ",  " << (fPed->GetArea(1)).GetRms() << endl;

  //*fLog << all << "Exit MPad::Process();" << endl;

  rc = 0;
  fErrors[rc]++;

  return kTRUE;
  //******************************************************************
}

// --------------------------------------------------------------------------
//
//
Int_t MPad::PostProcess()
{
    if (GetNumExecutions() != 0)
    {

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << dec << setfill(' ');

    if (fWarnings[0] == 0 ) fWarnings[0] = 1;

    *fLog << " " << setw(7) << fWarnings[1] << " (" << setw(3) 
          << (int)(fWarnings[1]*100/fWarnings[0])
          << "%) Pixel: iteration to find acceptable sigma failed" 
          << ", fIter = " << fIter << endl;

    *fLog << " " << setw(7) << fWarnings[2] << " (" << setw(3) 
          << (int)(fWarnings[2]*100/fWarnings[0]) 
          << "%) Pixel: No data for generating Sigma^2-Sigmabar^2" << endl;


    *fLog << " " << setw(7) << fErrors[2] << " (" << setw(3) 
          << (int)(fErrors[2]*100/GetNumExecutions()) 
          << "%) Evts skipped due to: Zenith angle out of range" << endl;

    *fLog << " " << setw(7) << fErrors[4] << " (" << setw(3) 
          << (int)(fErrors[4]*100/GetNumExecutions()) 
          << "%) Evts skipped due to: new sigma <= old sigma" << endl;

    *fLog << " " << setw(7) << fErrors[3] << " (" << setw(3) 
          << (int)(fErrors[3]*100/GetNumExecutions()) 
          << "%) lambdabar = 0" << endl;

    *fLog << " " << setw(7) << fInf[0] << " (" << setw(3) 
          << (int)(fInf[0]*100/GetNumExecutions()) 
          << "%) Evts didn't have to be padded" << endl;

    *fLog << " " << fErrors[0] << " (" 
          << (int)(fErrors[0]*100/GetNumExecutions()) 
          << "%) Evts were successfully padded!" << endl;
    *fLog << endl;

    }

    //---------------------------------------------------------------
    TCanvas &c = *(MH::MakeDefCanvas("Pad", "", 900, 1200)); 
    c.Divide(3, 4);
    gROOT->SetSelectedPad(NULL);

    c.cd(1);
    fHNSB->DrawCopy();
    fHNSB->SetBit(kCanDelete);    

    c.cd(2);
    fHSigmaPedestal->DrawCopy();
    fHSigmaPedestal->SetBit(kCanDelete);    

    c.cd(3);
    fHPhotons->DrawCopy();
    fHPhotons->SetBit(kCanDelete);    

    //--------------------------------------------------------------------

    if (fHgON)
    {
      c.cd(4);
      TH2D *m1;
      m1 = (TH2D*) ((TH3*)fHgON)->Project3D("zy");
      m1->SetDirectory(NULL);
      m1->UseCurrentStyle();
      m1->SetTitle("(fHgON) Sigmabar-new vs. Sigmabar-old (ON, all  \\Theta)");
      m1->SetXTitle("Sigmabar-old");
      m1->SetYTitle("Sigmabar-new");

      m1->DrawCopy("box");
      m1->SetBit(kCanDelete);;
    }

    if (fHgOFF)
    {
      c.cd(5);
      TH2D *m2;
      m2 = (TH2D*) ((TH3*)fHgOFF)->Project3D("zy");
      m2->SetDirectory(NULL);
      m2->UseCurrentStyle();
      m2->SetTitle("(fHgOFF) Sigmabar-new vs. Sigmabar-old (OFF, all  \\Theta)");
      m2->SetXTitle("Sigmabar-old");
      m2->SetYTitle("Sigmabar-new");

      m2->DrawCopy("box");
      m2->SetBit(kCanDelete);;
    }

    if (fHgMC)
    {
      c.cd(6);
      TH2D *m3;
      m3 = (TH2D*) ((TH3*)fHgMC)->Project3D("zy");
      m3->SetDirectory(NULL);
      m3->UseCurrentStyle();
      m3->SetTitle("(fHgMC) Sigmabar-new vs. Sigmabar-old (MC, all  \\Theta)");
      m3->SetXTitle("Sigmabar-old");
      m3->SetYTitle("Sigmabar-new");

      m3->DrawCopy("box");
      m3->SetBit(kCanDelete);;
    }

    //--------------------------------------------------------------------

    if (fHgOuterON)
    {
      c.cd(7);
      TH2D *m1;
      m1 = (TH2D*) ((TH3*)fHgOuterON)->Project3D("zy");
      m1->SetDirectory(NULL);
      m1->UseCurrentStyle();
      m1->SetTitle("(fHgOuterON) Sigmabar-new vs. Sigmabar-old (ON, all  \\Theta)");
      m1->SetXTitle("Sigmabar-old");
      m1->SetYTitle("Sigmabar-new");

      m1->DrawCopy("box");
      m1->SetBit(kCanDelete);;
    }

    if (fHgOuterOFF)
    {
      c.cd(8);
      TH2D *m2;
      m2 = (TH2D*) ((TH3*)fHgOuterOFF)->Project3D("zy");
      m2->SetDirectory(NULL);
      m2->UseCurrentStyle();
      m2->SetTitle("(fHgOuterOFF) Sigmabar-new vs. Sigmabar-old (OFF, all  \\Theta)");
      m2->SetXTitle("Sigmabar-old");
      m2->SetYTitle("Sigmabar-new");

      m2->DrawCopy("box");
      m2->SetBit(kCanDelete);;
    }

    if (fHgOuterMC)
    {
      c.cd(9);
      TH2D *m3;
      m3 = (TH2D*) ((TH3*)fHgOuterMC)->Project3D("zy");
      m3->SetDirectory(NULL);
      m3->UseCurrentStyle();
      m3->SetTitle("(fHgOuterMC) Sigmabar-new vs. Sigmabar-old (MC, all  \\Theta)");
      m3->SetXTitle("Sigmabar-old");
      m3->SetYTitle("Sigmabar-new");

      m3->DrawCopy("box");
      m3->SetBit(kCanDelete);;
    }

    //--------------------------------------------------------------------

    c.cd(10);
    fHSigmaTheta->SetDirectory(NULL);
    fHSigmaTheta->UseCurrentStyle();
    fHSigmaTheta->DrawCopy();
    fHSigmaTheta->SetBit(kCanDelete);    

    c.cd(11);
    fHSigmaThetaOuter->SetDirectory(NULL);
    fHSigmaThetaOuter->UseCurrentStyle();
    fHSigmaThetaOuter->DrawCopy();
    fHSigmaThetaOuter->SetBit(kCanDelete);    


  return kTRUE;
}

// --------------------------------------------------------------------------










