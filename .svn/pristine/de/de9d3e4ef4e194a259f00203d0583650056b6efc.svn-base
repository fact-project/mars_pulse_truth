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
!   Author(s): Robert Wagner, 10/2002 <mailto:magicsoft@rwagner.de>
!   Author(s): Wolfgang Wittek, 02/2003 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MCT1PadSchweizer
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
#include "MCT1PadSchweizer.h"

#include <stdio.h>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TRandom.h>
#include <TCanvas.h>

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

ClassImp(MCT1PadSchweizer);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MCT1PadSchweizer::MCT1PadSchweizer(const char *name, const char *title) 
{
  fName  = name  ? name  : "MCT1PadSchweizer";
  fTitle = title ? title : "Task for the padding (Schweizer)";

  fHSigmaTheta    = NULL;
  fHSigmaPixTheta = NULL;
  fHDiffPixTheta  = NULL;
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
MCT1PadSchweizer::~MCT1PadSchweizer()
{
  if (fHSigmaPedestal != NULL) delete fHSigmaPedestal;
  if (fHPhotons != NULL)       delete fHPhotons;
  if (fHNSB != NULL)           delete fHNSB;
}

// --------------------------------------------------------------------------
//
// Set the references to the histograms to be used in the padding
// 
// fHSigmaTheta    2D-histogram  (Theta, sigmabar)
// fHSigmaPixTheta 3D-hiostogram (Theta, pixel, sigma)
// fHDiffPixTheta  3D-histogram  (Theta, pixel, sigma^2-sigmabar^2)
// fHBlindPixIdTheta 2D-histogram  (Theta, blind pixel Id)
// fHBlindPixNTheta  2D-histogram  (Theta, no.of blind pixels )
//
//
void MCT1PadSchweizer::SetHistograms(TH2D *hist2, TH3D *hist3, TH3D *hist3Diff,
                                  TH2D *hist2Pix, TH2D *hist2PixN)
{
    fHSigmaTheta    = hist2;
    fHSigmaPixTheta = hist3;
    fHDiffPixTheta  = hist3Diff;
    fHBlindPixIdTheta = hist2Pix;
    fHBlindPixNTheta  = hist2PixN;

    fHSigmaTheta->SetDirectory(NULL);
    fHSigmaPixTheta->SetDirectory(NULL);
    fHDiffPixTheta->SetDirectory(NULL);
    fHBlindPixIdTheta->SetDirectory(NULL);
    fHBlindPixNTheta->SetDirectory(NULL);

    Print();
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
void MCT1PadSchweizer::SetPadFlag(Int_t padflag)
{
  fPadFlag = padflag;
  *fLog << "MCT1PadSchweizer::SetPadFlag(); choose option " << fPadFlag << endl; 
}

// --------------------------------------------------------------------------
//
//  Set the pointers and prepare the histograms
//
Int_t MCT1PadSchweizer::PreProcess(MParList *pList)
{
  if ( !fHSigmaTheta  || !fHSigmaPixTheta  || !fHDiffPixTheta  ||
       !fHBlindPixIdTheta  ||  !fHBlindPixNTheta)
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

   memset(fErrors, 0, sizeof(fErrors));

   return kTRUE;
}

// --------------------------------------------------------------------------
//
// Do the Padding
// idealy the events to be padded should have been generated without NSB
// 
Int_t MCT1PadSchweizer::Process()
{
  //*fLog << "Entry MCT1PadSchweizer::Process();" << endl;

  //------------------------------------------------
  // add pixels to MCerPhotEvt which are not yet in;
  // set their numnber of photons equal to zero

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

  const UInt_t npix = fEvt->GetNumPixels();

  //fSigmabar->Calc(*fCam, *fPed, *fEvt);
  //*fLog << "before padding : " << endl;
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

  if (sigbarold > 0)
  {
    //*fLog << "MCT1PadSchweizer::Process(); sigmabar of event to be padded is > 0 : "
    //      << sigbarold << ". Stop event loop " << endl;
    // input data should have sigmabar = 0; stop event loop
  
    rc = 1;
    fErrors[rc]++;
    return kCONTINUE; 
  }

  const Double_t theta = kRad2Deg*fMcEvt->GetTelescopeTheta();
  // *fLog << "theta = " << theta << endl;


  //-------------------------------------------
  // for the current theta, 
  // generate blind pixels according to the histograms 
  //          fHBlindPixNTheta and fHBlindPixIDTheta
  //


  Int_t binPix = fHBlindPixNTheta->GetXaxis()->FindBin(theta);

  if ( binPix < 1  ||  binPix > fHBlindPixNTheta->GetNbinsX() )
  {
    //*fLog << "MCT1PadSchweizer::Process(); binNumber out of range : theta, binPix = "
    //      << theta << ",  " << binPix << ";  Skip event " << endl;
    // event cannot be padded; skip event

    rc = 2;
    fErrors[rc]++;
    return kCONTINUE;
  }

  // numBlind is the number of blind pixels in this event
  TH1D *nblind;
  UInt_t numBlind;

  nblind = fHBlindPixNTheta->ProjectionY("", binPix, binPix, "");
  if ( nblind->GetEntries() == 0.0 )
  {
    *fLog << "MCT1PadSchweizer::Process(); projection for Theta bin " 
          << binPix << " has no entries; Skip event " << endl;
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


  // throw the Id of numBlind different pixels in this event
  TH1D *hblind;
  UInt_t idBlind;
  UInt_t listId[npix];
  UInt_t nlist = 0;
  Bool_t equal;

  hblind = fHBlindPixIdTheta->ProjectionY("", binPix, binPix, "");
  if ( hblind->GetEntries() > 0.0 )
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
      //*fLog << "idBlind = " << idBlind << endl;
    }
  fBlinds->SetReadyToSave();

  delete hblind;


  //-------------------------------------------
  // for the current theta, 
  // generate a sigmabar according to the histogram fHSigmaTheta
  //
  Double_t sigmabar=0;
  Int_t binNumber = fHSigmaTheta->GetXaxis()->FindBin(theta);

  if (binPix != binNumber)
  {
    cout << "The binnings of the 2 histograms are not identical; aborting"
         << endl;
    return kERROR;
  }

  TH1D *hsigma;

  hsigma = fHSigmaTheta->ProjectionY("", binNumber, binNumber, "");
  if ( hsigma->GetEntries() == 0.0 )
  {
    *fLog << "MCT1PadSchweizer::Process(); projection for Theta bin " 
          << binNumber << " has no entries; Skip event " << endl;
    // event cannot be padded; skip event
    delete hsigma;

    rc = 3;
    fErrors[rc]++;
    return kCONTINUE;
  }
  else
  {
    sigmabar = hsigma->GetRandom();
     //*fLog << "Theta, bin number = " << theta << ",  " << binNumber      //      << ",  sigmabar = " << sigmabar << endl
  }
  delete hsigma;

  const Double_t sigmabar2 = sigmabar*sigmabar;

  //-------------------------------------------

  //*fLog << "MCT1PadSchweizer::Process(); sigbarold, sigmabar = " 
  //      << sigbarold << ",  "<< sigmabar << endl;

  // Skip event if target sigmabar is <= sigbarold
  if (sigmabar <= sigbarold)
  {
    *fLog << "MCT1PadSchweizer::Process(); target sigmabar is less than sigbarold : "
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



  Int_t binTheta = fHDiffPixTheta->GetXaxis()->FindBin(theta);
  if (binTheta != binNumber)
  {
    cout << "The binnings of the 2 histograms are not identical; aborting"
         << endl;
    return kERROR;
  }

  // Get RMS of (Sigma^2-sigmabar^2) in this Theta bin.
  // The average electronic noise (to be added) has to be well above this RMS,
  // otherwise the electronic noise of an individual pixel (elNoise2Pix)
  // may become negative

  TH1D *hnoise = fHDiffPixTheta->ProjectionZ("", binTheta, binTheta,
                                                          0, 9999, "");
  Double_t RMS = hnoise->GetRMS(1);  
  delete hnoise;

  elNoise2 = TMath::Min(RMS,  sigmabar2 - sigbarold2);
  //*fLog << "elNoise2 = " << elNoise2 << endl; 

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
    //  *fLog << "MCT1PadSchweizer::Process(); no.of photons is 0 for used pixel" 
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

      hdiff = fHDiffPixTheta->ProjectionZ("", binTheta, binTheta,
                                              binPixel, binPixel, "");
      if ( hdiff->GetEntries() == 0 )
      {
        *fLog << "MCT1PadSchweizer::Process(); projection for Theta bin " 
              << binTheta << "  and pixel bin " << binPixel  
              << " has no entries;  aborting " << endl;
        delete hdiff;

        rc = 5;
        fErrors[rc]++;
        return kCONTINUE;     
      }

      count = 0;
      ok = kFALSE;
      for (Int_t m=0; m<20; m++)
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

        *fLog << "theta, j, count, sigmabar, diff = " << theta << ",  " 
              << j << ",  " << count << ",  " << sigmabar << ",  " 
              << diff << endl;
        diff = lambdabar*F2excess + oldsigma2/ratioArea - sigmabar2;
      }
      delete hdiff;
      sigma2 = diff + sigmabar2;
      break;

    case 2 :
      // throw the Sigma for this pixel from the distribution fHSigmaPixTheta

      hsig = fHSigmaPixTheta->ProjectionZ("", binTheta, binTheta,
                                              binPixel, binPixel, "");
      if ( hsig->GetEntries() == 0 )
      {
        *fLog << "MCT1PadSchweizer::Process(); projection for Theta bin " 
              << binTheta << "  and pixel bin " << binPixel  
              << " has no entries;  aborting " << endl;
        delete hsig;

        rc = 6;
        fErrors[rc]++;
        return kCONTINUE;     
      }

      count = 0;
      ok = kFALSE;
      for (Int_t m=0; m<20; m++)
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

        *fLog << "theta, j, count, sigmabar, sig = " << theta << ",  " 
              << j << ",  " << count << ",  " << sigmabar << ",  " 
              << sig << endl;
        sigma2 = lambdabar*F2excess + oldsigma2/ratioArea; 
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

    if (arg >= 0)
    {
      sigmaNSB0 = sqrt( arg );
    }
    else
    {
      *fLog << "MCT1PadSchweizer::Process(); argument of sqrt < 0 : "
            << arg << endl;
      sigmaNSB0 = 0.0000001;      
    }


    //---------------------------------
    // smear NSB0 according to sigmaNSB0
    // and subtract lambdabar because of AC coupling

    Double_t NSB = gRandom->Gaus(NSB0, sigmaNSB0) - lambdabar*ratioArea;

    //---------------------------------
 
    // add additional NSB to the number of photons
    Double_t oldphotons = pix.GetNumPhotons();
    Double_t newphotons = oldphotons + NSB;
    pix.SetNumPhotons(newphotons);


    fHNSB->Fill( NSB/sqrt(ratioArea) );
    fHPhotons->Fill( oldphotons/sqrt(ratioArea), newphotons/sqrt(ratioArea) );


    // error: add sigma of padded noise quadratically
    Double_t olderror = pix.GetErrorPhot();
    Double_t newerror = sqrt(olderror*olderror + addSig2);
    pix.SetErrorPhot(newerror);


    Double_t newsigma = sqrt(oldsigma2 + addSig2);
    ppix.SetRms(newsigma);

    fHSigmaPedestal->Fill(oldsigma, newsigma);
  } 
  //----------   end of loop over pixels   ---------------------------------

  // Calculate sigmabar again and crosscheck


  //fSigmabar->Calc(*fCam, *fPed, *fEvt);
  //*fLog << "after padding : " << endl;
  //fSigmabar->Print("");


  //*fLog << "Exit MCT1PadSchweizer::Process();" << endl;

  rc = 0;
  fErrors[rc]++;

  return kTRUE;

}

// --------------------------------------------------------------------------
//
//
Int_t MCT1PadSchweizer::PostProcess()
{
    if (GetNumExecutions() != 0)
    {

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << dec << setfill(' ');
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

    *fLog << " " << fErrors[0] << " (" 
          << (int)(fErrors[0]*100/GetNumExecutions()) 
          << "%) Evts survived the padding!" << endl;
    *fLog << endl;

    }

    //---------------------------------------------------------------
    TCanvas &c = *(MH::MakeDefCanvas("PadSchweizer", "", 900, 1200)); 
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
    fHSigmaTheta->SetTitle("(Input) 2D : Sigmabar, \\Theta");
    fHSigmaTheta->DrawCopy();     
    fHSigmaTheta->SetBit(kCanDelete);     

    //--------------------------------------------------------------------


    c.cd(7);
    fHBlindPixNTheta->SetDirectory(NULL);
    fHBlindPixNTheta->SetTitle("(Input) 2D : no.of blind pixels, \\Theta");
    fHBlindPixNTheta->DrawCopy();     
    fHBlindPixNTheta->SetBit(kCanDelete);     

    //--------------------------------------------------------------------


    c.cd(10);
    fHBlindPixIdTheta->SetDirectory(NULL);
    fHBlindPixIdTheta->SetTitle("(Input) 2D : blind pixel Id, \\Theta");
    fHBlindPixIdTheta->DrawCopy();     
    fHBlindPixIdTheta->SetBit(kCanDelete);     


    //--------------------------------------------------------------------
    // draw the 3D histogram (input): Theta, pixel, Sigma^2-Sigmabar^2

    c.cd(5);
    TH2D *l1;
    l1 = (TH2D*) ((TH3*)fHDiffPixTheta)->Project3D("zx");
    l1->SetDirectory(NULL);
    l1->SetTitle("(Input) Sigma^2-Sigmabar^2 vs. \\Theta (all pixels)");
    l1->SetXTitle("\\Theta [\\circ]");
    l1->SetYTitle("Sigma^2-Sigmabar^2");

    l1->DrawCopy("box");
    l1->SetBit(kCanDelete);;

    c.cd(8);
    TH2D *l2;
    l2 = (TH2D*) ((TH3*)fHDiffPixTheta)->Project3D("zy");
    l2->SetDirectory(NULL);
    l2->SetTitle("(Input) Sigma^2-Sigmabar^2 vs. pixel number (all \\Theta)");
    l2->SetXTitle("pixel");
    l2->SetYTitle("Sigma^2-Sigmabar^2");

    l2->DrawCopy("box");
    l2->SetBit(kCanDelete);;

    //--------------------------------------------------------------------
    // draw the 3D histogram (input): Theta, pixel, Sigma

    c.cd(6);
    TH2D *k1;
    k1 = (TH2D*) ((TH3*)fHSigmaPixTheta)->Project3D("zx");
    k1->SetDirectory(NULL);
    k1->SetTitle("(Input) Sigma vs. \\Theta (all pixels)");
    k1->SetXTitle("\\Theta [\\circ]");
    k1->SetYTitle("Sigma");

    k1->DrawCopy("box");
    k1->SetBit(kCanDelete);

    c.cd(9);
    TH2D *k2;
    k2 = (TH2D*) ((TH3*)fHSigmaPixTheta)->Project3D("zy");
    k2->SetDirectory(NULL);
    k2->SetTitle("(Input) Sigma vs. pixel number (all \\Theta)");
    k2->SetXTitle("pixel");
    k2->SetYTitle("Sigma");

    k2->DrawCopy("box");
    k2->SetBit(kCanDelete);;


    //--------------------------------------------------------------------


  return kTRUE;
}

// --------------------------------------------------------------------------














