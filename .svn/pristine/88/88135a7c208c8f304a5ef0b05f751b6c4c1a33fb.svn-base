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
!   Author(s): Robert Wagner, 10/2002   <mailto:magicsoft@rwagner.de>
!   Author(s): Wolfgang Wittek, 01/2003 <mailto:wittek@mppmu.mpg.de>
!   Author(s): Thomas Bretz, 04/2003    <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  MPadding                                                               //
//  (developped from MApplyPadding)                                        //
//                                                                         //
//  This task applies padding to a given Sigmabar target value.            //
//  The task checks whether the data stream it is applied to has to be     //
//  padded or not and if so, Gaussian noise with the difference in Sigma   //
//  is produced and added to the particular event. The number of photons,  // 
//  its error and the pedestal sigmas are altered.                         //
//                                                                         //
//  The padding has to be done before the image cleaning because the       //
//  image cleaning depends on the pedestal sigmas.                         //
//                                                                         //
//  There are several ways of defining the sigmabar value to which the     // 
//  events are padded:                                                     //
//                                                                         //
//  1) Set a fixed level (fFixedSigmabar) by calling 'SetTargetLevel'.     //
//                                                                         //
//  2) By calling 'SetDefiningHistogram', give a TH1D histogram            //
//     (fHSigmabarMax) which defines the Sigmabar as a function of Theta.  //
//                                                                         //
//  3) By calling 'SetSigmaThetaHist', give a TH2D histogram               //
//     (fHSigmaTheta) which contains the Sigmabar distribution for the     //
//     different bins in Theta. For a given event, the sigmabar value to   //
//     be used for the padding is thrown from this distribution.           //
//                                                                         //
//  Workaround :                                                           //  
//  If none of these options is specified then PreProcess will try to read // 
//  in a propriety format ASCII database for the CT1 test. The name of     // 
//  this file is set by 'SetDatabaseFile'. From the data in this file a    //
//  TH1D histogram (fHSigmabarMax) is generated.                           //
//                                                                         //
//  This implementation is still PRELIMINARY and requires some workarounds //
//  put in SPECIFICALLY FOR THE CT1 TESTS, since a database to access is   //
//  missing. It is not the FINAL MAGIC VERSION.                            //
//                                                                         //
//  For random numbers gRandom is used.                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MPadding.h"

#include <stdio.h>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <TProfile.h>

#include "MH.h"
#include "MBinning.h"

#include "MSigmabar.h"

#include "MMcEvt.hxx"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"

#include "MCerPhotEvt.h"
#include "MCerPhotPix.h"

#include "MPedPhotCam.h"
#include "MPedPhotPix.h"

ClassImp(MPadding);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MPadding::MPadding(const char *name, const char *title)
    : fRunType(0), fGroup(0), fFixedSigmabar(0), fHSigMaxAllocated(kFALSE), fHSigmabarMax(NULL), fHSigmaTheta(NULL)
{
    fName  = name  ? name  : "MPadding";
    fTitle = title ? title : "Task for the padding";

    //--------------------------------------------------------------------
    // plot pedestal sigmas for testing purposes
    fHSigmaPedestal = new TH2D("SigPed", "Padded vs orig. sigma",
                               100, 0.0, 5.0, 100, 0.0, 5.0);
   fHSigmaPedestal->SetXTitle("Orig. Pedestal sigma");
   fHSigmaPedestal->SetYTitle("Padded Pedestal sigma");

   // plot no.of photons (before vs. after padding) for testing purposes
   fHPhotons = new TH2D("Photons", "Photons after vs.before padding",
                        100, -10.0, 90.0, 100, -10, 90);
   fHPhotons->SetXTitle("No.of photons before padding");
   fHPhotons->SetYTitle("No.of photons after padding");

   // plot of added NSB
   fHNSB = new TH1D("NSB", "Distribution of added NSB", 100, -10.0, 10.0);
   fHNSB->SetXTitle("No.of added NSB photons");
   fHNSB->SetYTitle("No.of pixels");

   fHSigmaOld = new TH2D;
   fHSigmaOld->SetNameTitle("fHSigmaOld", "Sigma before padding");
   fHSigmaOld->SetXTitle("Theta");
   fHSigmaOld->SetYTitle("Sigma");

   fHSigmaNew = new TH2D;
   fHSigmaNew->SetNameTitle("fHSigmaNew", "Sigma after padding");
   fHSigmaNew->SetXTitle("Theta");
   fHSigmaNew->SetYTitle("Sigma");
}

// --------------------------------------------------------------------------
//
// Destructor. 
//
MPadding::~MPadding()
{
   delete fHSigmaPedestal;
   delete fHPhotons;
   delete fHNSB;
   delete fHSigmaOld;
   delete fHSigmaNew;
   if (fHSigMaxAllocated && fHSigmabarMax)
       delete fHSigmabarMax;
}

// --------------------------------------------------------------------------
//
// You can provide a TH1D* histogram containing 
//     - the target Sigmabar in bins of theta. 
// Be sure to use the same binning as for the analysis
//
Bool_t MPadding::SetDefiningHistogram(TH1D *histo)
{
    if (fHSigmabarMax)
    {
        *fLog << warn << "MPadding - SigmabarMax already set.";
        return kFALSE;
    }

    fHSigmabarMax  = histo;

    fFixedSigmabar = 0;
    fHSigmaTheta   = NULL;

    *fLog << inf << "MPadding - Use Defining Histogram.";

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// You can provide a TH2D* histogram containing 
//     - the Sigmabar distribution in bins of theta. 
//
Bool_t MPadding::SetSigmaThetaHist(TH2D *histo)
{
    if (fHSigmaTheta)
    {
        *fLog << warn << "MPadding - SigmaTheta already set.";
        return kFALSE;
    }

    fHSigmaTheta   = histo;

    fFixedSigmabar = 0;
    if (fHSigMaxAllocated)
    {
        fHSigMaxAllocated = kFALSE;
        delete fHSigmabarMax;
    }
    fHSigmabarMax  = NULL;

    *fLog << inf << "MPadding - Use Sigma Theta Histogram.";

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
void MPadding::SetTargetLevel(Double_t sigmabar)
{
    fFixedSigmabar = sigmabar;

    fHSigmaTheta   = NULL;
    if (fHSigMaxAllocated)
    {
        fHSigMaxAllocated = kFALSE;
        delete fHSigmabarMax;
    }
    fHSigmabarMax = NULL;

    *fLog << inf << "MPadding - Use fixed sigmabar: fFixedSigmabar = ";
    *fLog << fFixedSigmabar << endl;
}

// --------------------------------------------------------------------------
//
//  check if MEvtHeader exists in the Parameter list already.
//  if not create one and add them to the list
//
Int_t MPadding::PreProcess(MParList *pList)
{
  fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
  if (!fMcEvt)
    {
       *fLog << err << dbginf << "MMcEvt not found... aborting." << endl;
       return kFALSE;
     }
  
   fPed = (MPedPhotCam*)pList->FindObject("MPedPhotCam");
   if (!fPed)
     {
       *fLog << err << dbginf << "MPedPhotCam not found... aborting." << endl;
       return kFALSE;
     }
  
   fCam = (MGeomCam*)pList->FindObject("MGeomCam");
   if (!fCam)
     {
       *fLog << err << dbginf << "MGeomCam not found (no geometry information available)... aborting." << endl;
       return kFALSE;
     }
  
   fEvt = (MCerPhotEvt*)pList->FindObject("MCerPhotEvt");
   if (!fEvt)
     {
       *fLog << err << dbginf << "MCerPhotEvt not found... aborting." << endl;
       return kFALSE;
     }

   fSigmabar = (MSigmabar*)pList->FindCreateObj("MSigmabar");
   if (!fSigmabar)
     {
       *fLog << err << dbginf << "MSigmabar not found... aborting." << endl;
       return kFALSE;
     }
   
   // Get Theta Binning  
   MBinning* binstheta  = (MBinning*)pList->FindObject("BinningTheta");
   if (!binstheta)
     {
       *fLog << err << dbginf << "BinningTheta not found... aborting." << endl;
       return kFALSE;      
     }

   // Get Sigma Binning  
   MBinning* binssigma  = (MBinning*)pList->FindObject("BinningSigmabar");
   if (!binssigma)
     {
       *fLog << err << dbginf << "BinningSigmabar not found... aborting." << endl;
       return kFALSE;      
     }

   MH::SetBinning(fHSigmaOld, binstheta, binssigma);
   MH::SetBinning(fHSigmaNew, binstheta, binssigma);

   //************************************************************************
   // Create fSigmabarMax histogram
   // (only if no fixed Sigmabar target value and no histogram have been 
   // provided)
   //
   if (fFixedSigmabar==0 && !fHSigmabarMax && !fHSigmaTheta)
   {
       *fLog << inf << "MPadding - Creating fSigmabarMax histogram: ";
       *fLog << "fFixedSigmabar=" << fFixedSigmabar << ", ";
       *fLog << "fHSigmabarMax = " << fHSigmabarMax << endl;

       // FIXME: Not deleted
     fHSigmabarMax = new TH1D;
     fHSigmabarMax->SetNameTitle("fHSigmabarMax", "Sigmabarmax for this analysis");

     fHSigMaxAllocated = kTRUE;

     MH::SetBinning(fHSigmabarMax, binstheta);

     // -------------------------------------------------
     // read in SigmabarParams
     // workaround--proprietary file format--CT1test only BEGIN
     // -------------------------------------------------
     
     FILE *f=fopen(fDatabaseFilename, "r");
     if(!f) {
         *fLog << err << dbginf << "Database file '" << fDatabaseFilename;
         *fLog << "' was not found (specified by MPadding::SetDatabaseFile) ...aborting." << endl;
         return kFALSE;
     }

     TAxis &axe = *fHSigmabarMax->GetXaxis();

     char line[80];
     while ( fgets(line, sizeof(line), f) != NULL) {
         if (line[0]=='#')
             continue;

         Float_t sigmabarMin, sigmabarMax, thetaMin, thetaMax, ra, dec2;
         Int_t type, group, mjd, nr;

         sscanf(line,"%d %d %f %f %d %d %f %f %f %f",
                &type, &group, &ra, &dec2, &mjd, &nr,
                &sigmabarMin, &sigmabarMax, &thetaMin, &thetaMax);

         if (group!=fGroup && type!=1) //selected ON group or OFF
             continue;

         const Int_t from = axe.FindFixBin(thetaMin);
         const Int_t to   = axe.FindFixBin(thetaMax);

         // find out which bin(s) we have to look at
         for (Int_t i=from; i<to+1; i++)
             if (sigmabarMax > fHSigmabarMax->GetBinContent(i))
                 fHSigmabarMax->SetBinContent(i, sigmabarMax);
     }//while
  
   } //fFixedSigmabar
   //************************************************************************

   if (!fHSigmabarMax && !fHSigmaTheta && fFixedSigmabar==0)
   {
       *fLog << err << "ERROR: Sigmabar for padding not defined... aborting." << endl;
       return kFALSE;
   }

   return kTRUE;
}

Double_t MPadding::CalcOtherSig(const Double_t mySig, const Double_t theta) const
{
  //
  // Get sigmabar which we have to pad to
  //
  const TAxis   &axe     = *fHSigmabarMax->GetXaxis();
  const Int_t    binnum  =  axe.FindFixBin(theta);
  const Bool_t   inrange =  theta>=axe.GetXmin() && theta<=axe.GetXmax();

  if ((fHSigmabarMax || fHSigmaTheta) && !inrange)
  {
      *fLog << err << dbginf;
      *fLog << "Theta of current event is beyond the limits, Theta = ";
      *fLog << theta << " ...skipping." <<endl;
      return -1;
  }


  //
  // get target sigma for the current Theta from the histogram fHSigmabarMax
  //
  if (fHSigmabarMax != NULL) 
      return fHSigmabarMax->GetBinContent(binnum);

  //
  // for the current Theta, 
  // generate a sigma according to the histogram fHSigmaTheta
  //
  if (fHSigmaTheta != NULL)
  {
      Double_t otherSig = -1;

      TH1D* fHSigma = fHSigmaTheta->ProjectionY("", binnum, binnum, "");

      if (fHSigma->GetEntries()>0)
          otherSig = fHSigma->GetRandom();

      delete fHSigma;

      return otherSig;
  }

  //
  // use a fixed target sigma
  //
  return fFixedSigmabar;
}

// --------------------------------------------------------------------------
//
// Do the padding  (mySig ==> otherSig)
// 
Int_t MPadding::Padding(const Double_t quadraticDiff, const Double_t theta)
{
   const UInt_t npix = fEvt->GetNumPixels();

   // pad only pixels   - which are used (before image cleaning)
   //                   - and for which the no.of photons is != 0.0
   //
   for (UInt_t i=0; i<npix; i++) 
   {   
     MCerPhotPix &pix = (*fEvt)[i];
     if ( !pix.IsPixelUsed() )
       continue;
/*
     if ( pix.GetNumPhotons() == 0)
     {
       *fLog << "MPadding::Process(); no.of photons is 0 for used pixel" 
             << endl;
       continue;
     }
*/
     const Double_t area = fCam->GetPixRatio(pix.GetPixId());

     // add additional NSB to the number of photons
     const Double_t NSB = sqrt(quadraticDiff*area) * gRandom->Gaus(0, 1);
     const Double_t oldphotons = pix.GetNumPhotons();
     const Double_t newphotons = oldphotons + NSB;
     pix.SetNumPhotons(	newphotons );

     fHNSB->Fill( NSB/sqrt(area) );
     fHPhotons->Fill( newphotons/sqrt(area), oldphotons/sqrt(area) );

     // error: add sigma of padded noise quadratically
     const Double_t olderror = pix.GetErrorPhot();
     const Double_t newerror = sqrt( olderror*olderror + quadraticDiff*area );
     pix.SetErrorPhot( newerror );

     MPedPhotPix &ppix = (*fPed)[i];

     ppix.SetRms(0);

     const Double_t oldsigma = ppix.GetRms();
     const Double_t newsigma = sqrt( oldsigma*oldsigma + quadraticDiff*area );
     ppix.SetRms( newsigma );

     fHSigmaPedestal->Fill( oldsigma, newsigma );
     fHSigmaOld->Fill( theta, oldsigma );
     fHSigmaNew->Fill( theta, newsigma );
   } //for

   return kTRUE;
}


// --------------------------------------------------------------------------
//
// Calculate Sigmabar for current event
// Then apply padding
// 
// 1) have current event
// 2) get sigmabar(theta)
// 3) pad event
//
Int_t MPadding::Process()
{
    const Double_t theta = kRad2Deg*fMcEvt->GetTelescopeTheta();

    //
    // Calculate sigmabar of event
    //
    Double_t mySig = fSigmabar->Calc(*fCam, *fPed, *fEvt);

    //$$$$$$$$$$$$$$$$$$$$$$$$$$
    mySig = 0.0;  // FIXME?
    //$$$$$$$$$$$$$$$$$$$$$$$$$$

    const Double_t otherSig = CalcOtherSig(mySig, theta);

    // Skip event if target sigma is zero
    if (otherSig<=0)
        return kCONTINUE;

    // Determine quadratic difference other-mine
    const Double_t quadraticDiff = otherSig*otherSig - mySig*mySig;

    if (quadraticDiff < 0) {
        *fLog << err << "ERROR - MPadding: Event has higher Sigmabar=" << mySig;
        *fLog << " than Sigmabarmax=" << otherSig << " @ Theta =" << theta;
        *fLog << " ...skipping." << endl;
        return kCONTINUE; //skip
    }

    if (quadraticDiff == 0)
        return kTRUE; //no padding necessary.

    //
    // quadratic difference is > 0, do the padding;
    //
    Padding(quadraticDiff, theta);

    // Calculate Sigmabar again and crosscheck
    //mySig = fSigmabar->Calc(*fCam, *fPed, *fEvt);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Draws some histograms if IsGraphicalOutputEnabled
//
Bool_t MPadding::PostProcess()
{
    if (!IsGraphicalOutputEnabled())
        return kTRUE;

    TCanvas &c = *MH::MakeDefCanvas("Padding", "", 600, 900);
    c.Divide(2,3);
    gROOT->SetSelectedPad(NULL);

    if (fHSigmabarMax != NULL)
    {
        c.cd(1);
        fHSigmabarMax->DrawClone();
    }
    else if (fHSigmaTheta != NULL)
    {
        c.cd(1);
        fHSigmaTheta->DrawClone();
    }

    c.cd(3);
    fHSigmaPedestal->DrawClone();

    c.cd(5);
    fHPhotons->DrawClone();

    c.cd(2);
    fHNSB->DrawClone();

    c.cd(4);
    fHSigmaOld->DrawClone();     

    c.cd(6);
    fHSigmaNew->DrawClone();     

    return kTRUE;
}
