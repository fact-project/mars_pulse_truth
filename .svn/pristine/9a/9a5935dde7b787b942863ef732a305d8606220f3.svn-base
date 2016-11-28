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
!   Author(s): Markus Gaug  08/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MCalibrationTestCalc
//
//   PreProcess(): Initialize pointers to MHCalibrationTestCam
//               
//   ReInit():     MCalibrationCam::InitSize(NumPixels) is called from MGeomApply (which allocates
//                 memory in a TClonesArray of type MCalibrationChargePix)
//                 Initializes pointer to MBadPixelsCam
//
//   Process():    Nothing to be done, histograms getting filled by MHCalibrationTestCam
//
//   PostProcess(): Print out interpolation results to file
//
//  Input Containers:
//   MHCalibrationTestCam
//   MBadPixelsCam
//   MGeomCam
//
//  Output Containers:
//   none
//  
//////////////////////////////////////////////////////////////////////////////
#include "MCalibrationTestCalc.h"

#include <TMath.h>
#include <TSystem.h>

#include <TH1.h>
#include <TF1.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MGeom.h"
#include "MHCamera.h"

#include "MHCalibrationTestCam.h"
#include "MHCalibrationPix.h"

#include "MCalibrationTestCam.h"
#include "MCalibrationTestPix.h"

#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

ClassImp(MCalibrationTestCalc);

using namespace std;

const Float_t MCalibrationTestCalc::fgPhotErrLimit = 4.5;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets the pointer to fHTestCam and fGeom to NULL
// Sets outputpath to "."
// Sets outputfile to "TestCalibStat.txt"
// Sets fPhotErrLimit to fgPhotErrLimit  
//
// Calls:
// - Clear()
//
MCalibrationTestCalc::MCalibrationTestCalc(const char *name, const char *title)
    : fBadPixels(NULL), fHTestCam(NULL), fCam(NULL), fGeom(NULL)
{
        
  fName  = name  ? name  : "MCalibrationTestCalc";
  fTitle = title ? title : "Task to output the results of MHCalibrationTestCam ";
  
  SetPhotErrLimit();

  SetOutputPath();
  SetOutputFile();

}


// --------------------------------------------------------------------------
//
// Search for the following input containers and abort if not existing:
// - MGeomCam
// - MHCalibrationTestCam
// - MCalibrationTestCam
// - MBadPixelsCam
// 
Bool_t MCalibrationTestCalc::ReInit(MParList *pList )
{

  fGeom = (MGeomCam*)pList->FindObject("MGeomCam");
  if (!fGeom)
    {
      *fLog << err << "No MGeomCam found... aborting." << endl;
      return kFALSE;
    }
  
  fHTestCam = (MHCalibrationTestCam*)pList->FindObject("MHCalibrationTestCam");
  if (!fHTestCam)
    {
      *fLog << err << "Cannot find MHCalibrationTestCam... aborting" << endl;
      *fLog << err << "Maybe you forget to call an MFillH for the MHCalibrationTestCam before..." << endl;
      return kFALSE;
    }

  fCam = (MCalibrationTestCam*)pList->FindObject(AddSerialNumber("MCalibrationTestCam"));
  if (!fCam)
  {
      *fLog << err << "Cannot find MCalibrationTestCam ... abort." << endl;
      return kFALSE;
  }

  fBadPixels = (MBadPixelsCam*)pList->FindObject(AddSerialNumber("MBadPixelsCam"));
  if (!fBadPixels)
  {
      *fLog << err << "Cannot find MBadPixelsCam ... abort." << endl;
      return kFALSE;
  }

  return kTRUE;
}

// -----------------------------------------------------------------------
//
// Return if number of executions is null.
//
// Print out some statistics
//
Int_t MCalibrationTestCalc::PostProcess()
{

  if (GetNumExecutions()==0)
    return kTRUE;

  //
  // Re-direct the output to an ascii-file from now on:
  //
  MLog asciilog;
  asciilog.SetOutputFile(GetOutputFile(),kTRUE);
  SetLogStream(&asciilog);
  //
  // Finalize calibration statistics
  //
  FinalizeCalibratedPhotons();
  FinalizeNotInterpolated();
  const Int_t maxbad = CalcMaxNumBadPixelsCluster();


  *fLog << inf << endl;
  *fLog << GetDescriptor() << ": Pixel Interpolation status:" << endl;

  if (fGeom->InheritsFrom("MGeomCamMagic"))
  {
      *fLog << " Not interpolateable Pixels:";
      *fLog << " Inner: " << Form("%3i", fCam->GetNumUninterpolated(0));
      *fLog << " Outer: " << Form("%3i", fCam->GetNumUninterpolated(1)) << endl;
      *fLog << " Biggest not-interpolateable cluster: " << maxbad << endl;
  }

  fCam->SetNumUninterpolatedInMaxCluster(maxbad);

  *fLog << endl;  
  SetLogStream(&gLog);

  return kTRUE;
}


// ------------------------------------------------------------------------
//
//
// First loop: Calculate a mean and mean RMS of calibrated photons per area index 
//             Include only MHCalibrationTestPix's which are not empty (not interpolated)
//              
// Second loop: Get weighted mean number of calibrated photons and its RMS
//              excluding those deviating by more than fPhotErrLimit 
//              sigmas from the mean (obtained in first loop). Set 
//              MBadPixelsPix::kDeviatingNumPhots if excluded.
// 
void MCalibrationTestCalc::FinalizeCalibratedPhotons() const
{

  const UInt_t npixels  = fGeom->GetNumPixels();
  const UInt_t nareas   = fGeom->GetNumAreas();
  const UInt_t nsectors = fGeom->GetNumSectors();

  TArrayD lowlim      (nareas);
  TArrayD upplim      (nareas);
  TArrayD areaphotons (nareas); 
  TArrayD sectorphotons(nsectors);
  TArrayD areavars    (nareas); 
  TArrayD sectorvars  (nsectors);
  TArrayD fittedmean  (nareas); 
  TArrayD fittedsigma (nareas);
  TArrayI numareavalid(nareas); 
  TArrayI numsectorvalid(nsectors);

  //
  // First loop: Get mean number of calibrated photons and the RMS
  //             The loop is only to recognize later pixels with very deviating numbers
  //
  MHCamera camphotons(*fGeom,"Camphotons","Photons in Camera");

  for (UInt_t i=0; i<npixels; i++)
    {
      
      MHCalibrationPix     &hist = (*fHTestCam)[i];
      MCalibrationTestPix  &pix =  (MCalibrationTestPix&)(*fCam)[i];
      //
      // We assume that the pixels have been interpolated so far. 
      // The MBadPixelsCam does not give any more information
      //
      if (hist.IsEmpty())
        {
          pix.SetExcluded();
          continue;
        }

      const Double_t nphot    = hist.GetMean();
      const Double_t nphoterr = hist.GetMeanErr();
      const Int_t    aidx     = (*fGeom)[i].GetAidx();

      camphotons.Fill(i,nphot);
      camphotons.SetUsed(i);

      pix.SetNumPhotons   ( nphot    );
      pix.SetNumPhotonsErr( nphoterr );

      areaphotons [aidx] += nphot;
      areavars    [aidx] += nphot*nphot;
      numareavalid[aidx] ++;
    } 

  for (UInt_t aidx=0; aidx<nareas; aidx++)
    {
      if (numareavalid[aidx] == 0)
        {
          *fLog << warn << GetDescriptor() << ": No pixels with valid number of calibrated photons found "
                << "in area index: " << aidx << endl;
          continue;
        }

      if (numareavalid[aidx] == 1)
        areavars[aidx]  = 0.;
      else if (numareavalid[aidx] == 0)
        {
          areaphotons[aidx] = -1.;
          areavars[aidx]    = -1.;
        }
      else
        {
          areavars[aidx]    = (areavars[aidx] - areaphotons[aidx]*areaphotons[aidx]/numareavalid[aidx]) 
                            / (numareavalid[aidx]-1.);
          areaphotons[aidx] = areaphotons[aidx] / numareavalid[aidx];
        }
      
      if (areavars[aidx] < 0.)
        {
          *fLog << warn << GetDescriptor() << ": No pixels with valid variance of calibrated photons found "
                << "in area index: " << aidx << endl;
          continue;
        }

      const Float_t areamean = areaphotons[aidx];
      const Float_t areaerr  = TMath::Sqrt(areavars[aidx]);
      
      MCalibrationTestPix &avpix = (MCalibrationTestPix&)fCam->GetAverageArea(aidx);
      avpix.SetNumPhotons   (areamean);
      avpix.SetNumPhotonsErr(areaerr );

      lowlim [aidx] = areamean - fPhotErrLimit*areaerr;
      upplim [aidx] = areamean + fPhotErrLimit*areaerr;

      TArrayI area(1);
      area[0] = aidx;

      TH1D *hist = camphotons.ProjectionS(TArrayI(),area,"_py",100);
      hist->Fit("gaus","Q");
      const Double_t mean  = hist->GetFunction("gaus")->GetParameter(1);
      const Double_t sigma = hist->GetFunction("gaus")->GetParameter(2);
      const Int_t    ndf   = hist->GetFunction("gaus")->GetNDF();

      if (ndf < 2)
        {
          *fLog << warn << GetDescriptor() << ": Cannot use a Gauss fit to the number of calibrated photons " 
                << "in the camera with area index: " << aidx << endl;
          *fLog << warn << GetDescriptor() << ": Number of dof.: " << ndf << " is smaller than 2 " << endl;
          *fLog << warn << GetDescriptor() << ": Will use the simple mean and rms " << endl;
          delete hist;
          continue;
        }
      
      const Double_t prob = hist->GetFunction("gaus")->GetProb();

      if (prob < 0.001)
        {
          *fLog << warn << GetDescriptor() << ": Cannot use a Gauss fit to the number of calibrated photons " 
                << "in the camera with area index: " << aidx << endl;
          *fLog << warn << GetDescriptor() << ": Fit probability " << prob 
                << " is smaller than 0.001 " << endl;
          *fLog << warn << GetDescriptor() << ": Will use the simple mean and rms " << endl;
          delete hist;
          continue;
        }
      
      fittedmean [aidx] = mean;
      fittedsigma[aidx] = sigma;
      
      avpix.SetNumPhotons   (mean );
      avpix.SetNumPhotonsErr(sigma);

      lowlim  [aidx] = mean  - fPhotErrLimit*sigma;
      upplim  [aidx] = mean  + fPhotErrLimit*sigma;

      *fLog << inf << GetDescriptor() 
            << ": Fitted number of calib. equiv. Cher. photons in area index " << aidx 
            << ": "  << Form("%7.2f +- %6.2f",fittedmean[aidx],fittedsigma[aidx]) << endl;

      delete hist;
    }

  *fLog << endl;

  numareavalid.Reset();
  areaphotons .Reset();
  areavars    .Reset();

  //
  // Second loop: Get mean number of calibrated photons and its RMS excluding 
  //              pixels deviating by more than fPhotErrLimit sigma. 
  // 
  for (UInt_t i=0; i<npixels; i++)
    {
      
      MHCalibrationPix &hist    = (*fHTestCam)[i];
      MCalibrationTestPix  &pix =  (MCalibrationTestPix&) (*fCam)[i];

      const Int_t    aidx         = (*fGeom)[i].GetAidx();
      const Int_t    sector       = (*fGeom)[i].GetSector();
      const Double_t nphot        = hist.GetMean();
      const Double_t nphotpera    = nphot            / (*fGeom)[i].GetA();
      const Double_t nphotperaerr = hist.GetMeanErr()/ (*fGeom)[i].GetA();

      pix.SetNumPhotonsPerArea   ( nphotpera    );
      pix.SetNumPhotonsPerAreaErr( nphotperaerr );

      if ( nphot < lowlim[aidx] || nphot > upplim[aidx] )
        {
          *fLog << warn << GetDescriptor() << ": Number of photons: " 
                << Form("%8.2f out of %3.1f sigma limit: ",nphot,fPhotErrLimit)
                << Form("[%8.2f,%8.2f] pixel%4i",lowlim[aidx],upplim[aidx],i) << endl;
          MBadPixelsPix &bad = (*fBadPixels)[i];
          bad.SetUncalibrated( MBadPixelsPix::kDeviatingNumPhots );
          bad.SetUnsuitable  ( MBadPixelsPix::kUnsuitableRun     );
          continue;
        }
      
      areavars     [aidx] += nphotpera*nphotpera;
      areaphotons  [aidx] += nphotpera;
      numareavalid [aidx] ++;

      sectorvars    [sector] += nphotpera*nphotpera;
      sectorphotons [sector] += nphotpera;
      numsectorvalid[sector] ++;
    } 

  *fLog << endl;

  for (UInt_t aidx=0; aidx<nareas; aidx++)
    {
      
      if (numareavalid[aidx] == 1)
        areavars[aidx] = 0.;
      else if (numareavalid[aidx] == 0)
        {
          areaphotons[aidx] = -1.;
          areavars[aidx]    = -1.;
        }
      else
        {
          areavars[aidx] = (areavars[aidx] - areaphotons[aidx]*areaphotons[aidx]/numareavalid[aidx]) 
                         / (numareavalid[aidx]-1.);
          areaphotons[aidx] /=  numareavalid[aidx];
        }
      

      MCalibrationTestPix &avpix = (MCalibrationTestPix&)fCam->GetAverageArea(aidx);

      if (areavars[aidx] < 0. || areaphotons[aidx] <= 0.)
        {
          *fLog << warn << GetDescriptor() 
                << ": Mean number of photons per area in area index " 
                << aidx << " could not be calculated! Mean: " << areaphotons[aidx] 
                << " Variance: " << areavars[aidx] << endl;
          avpix.SetExcluded();
          continue;
        }

      const Float_t areaerr = TMath::Sqrt(areavars[aidx]);
      
      avpix.SetNumPhotonsPerArea   (areaphotons[aidx]);
      avpix.SetNumPhotonsPerAreaErr(areaerr );

      *fLog << inf << GetDescriptor() << ": Mean number of equiv. Cher. photons "
            << "per area in area idx " << aidx << ": "  
            << Form("%5.3f+-%5.4f  [ph/mm^2]",areaphotons[aidx],areaerr) << endl;
    }

  *fLog << endl;

  for (UInt_t sector=0; sector<nsectors; sector++)
    {
      
      if (numsectorvalid[sector] == 1)
        sectorvars[sector] = 0.;
      else if (numsectorvalid[sector] == 0)
        {
          sectorphotons[sector]  = -1.;
          sectorvars[sector]     = -1.;
        }
      else
        {
          sectorvars[sector] = (sectorvars[sector] 
                               - sectorphotons[sector]*sectorphotons[sector]/numsectorvalid[sector]
                               ) 
                             / (numsectorvalid[sector]-1.);
          sectorphotons[sector] /=  numsectorvalid[sector];
        }
      
      MCalibrationTestPix &avpix = (MCalibrationTestPix&)fCam->GetAverageSector(sector);

      if (sectorvars[sector] < 0. || sectorphotons[sector] <= 0.)
        {
          *fLog << warn << GetDescriptor() 
                << ": Mean number of calibrated photons per area in sector " 
                << sector << " could not be calculated! Mean: " << sectorphotons[sector] 
                << " Variance: " << sectorvars[sector] << endl;
          avpix.SetExcluded();
          continue;
        }
      
  
      const Float_t sectorerr = TMath::Sqrt(sectorvars[sector]);

      avpix.SetNumPhotonsPerArea   (sectorphotons[sector]);
      avpix.SetNumPhotonsPerAreaErr(sectorerr );

      *fLog << inf << GetDescriptor() << ": Mean number of equiv. Cher. photons "
            << "per area in sector " << sector << ":   "  
            << Form("%5.3f+-%5.4f  [ph/mm^2]",sectorphotons[sector],sectorerr) << endl;
    }

  return;
}


// -----------------------------------------------------------------------------------------------
//
// Print out statistics about not interpolated pixels
// 
void MCalibrationTestCalc::FinalizeNotInterpolated() 
{
  
  const Int_t areas  = fGeom->GetNumAreas();
  TArrayI *newarr[areas];

  for (Int_t aidx=0; aidx<areas; aidx++)
    newarr[aidx] = new TArrayI(0);

  for (Int_t i=0; i<fCam->GetSize(); i++)
    {
      const Int_t aidx = (*fGeom)[i].GetAidx();
      MCalibrationTestPix &pix = (MCalibrationTestPix&)(*fCam)[i];
      if (pix.IsExcluded())
        {
          const Int_t size = newarr[aidx]->GetSize();
          newarr[aidx]->Set(size+1);
          newarr[aidx]->AddAt(i,size);
        }
    }

  Int_t num = 0;

  for (Int_t aidx = 0; aidx<areas; aidx++)
    {
      *fLog << endl;
      *fLog << " " << setw(7)
            << "Not interpolated pixels by in area idx " << aidx << ": ";
      for (Int_t i=0; i<newarr[aidx]->GetSize(); i++)
        {
          *fLog << newarr[aidx]->At(i) << " ";
          num++;
        }
      fCam->SetNumUninterpolated(newarr[aidx]->GetSize(),aidx);
      *fLog << endl;
    }
  
  *fLog << " " << setw(7) << num << " total not interpolateable pixels " << endl;
  
}

Int_t MCalibrationTestCalc::CalcMaxNumBadPixelsCluster()
{

  TArrayI arr(0);
  
  for (Int_t i=0; i<fCam->GetSize(); i++)
    {
      MCalibrationTestPix &pix = (MCalibrationTestPix&)(*fCam)[i];
      if (pix.IsExcluded())
        {
        const Int_t s = arr.GetSize();
        arr.Set(s+1);
        arr[s] = i;
        }
    }
  
  const Int_t size = arr.GetSize();
  
  if (size == 0)
    return 0;
  
  if (size == 1)
    return 1;

  TArrayI knownpixels(0);
  Int_t clustersize    = 1;
  Int_t oldclustersize = 0;
  //
  // Loop over the not-interpolateable pixels:
  //
  for (Int_t i=0; i<size; i++)
    {

      const Int_t id   = arr[i];
      const Int_t knownsize = knownpixels.GetSize();
      knownpixels.Set(knownsize+1);
      knownpixels[knownsize] = id;
      LoopNeighbours(arr, knownpixels, clustersize, id);
      if (clustersize > oldclustersize)
        oldclustersize = clustersize;
      clustersize = 1;
    }

  return oldclustersize; 

}


void MCalibrationTestCalc::LoopNeighbours( const TArrayI &arr, TArrayI &knownpixels, Int_t &clustersize, const Int_t idx )
{
  
  const MGeom &pix = (*fGeom)[idx];
  const Byte_t neighbours = pix.GetNumNeighbors();

  // 
  // Loop over the next neighbours: 
  // - Check if they are already in the list of known pixels
  // - If not, call loopneighbours for the rest
  // - grow clustersize for those
  //
  for (Int_t i=0;i<neighbours;i++)
    {
      const Int_t newid = pix.GetNeighbor(i);
      Bool_t known = kFALSE;

      for (Int_t j=knownpixels.GetSize()-1;j>=0;j--)
        if (newid == knownpixels.At(j))
          {
            known = kTRUE;
            break;
          }
      if (known)
        continue;

      for (Int_t k=0;k<arr.GetSize();k++)
        if (newid == arr.At(k))
          {
            // This is an unknown, new pixel in the cluster!!
            clustersize++;
            const Int_t knownsize = knownpixels.GetSize();
            knownpixels.Set(knownsize+1);
            knownpixels[knownsize] = newid;
            LoopNeighbours(arr, knownpixels, clustersize, newid);
          }
    }
}

// --------------------------------------------------------------------------
//
// Set the path for output file
// 
void MCalibrationTestCalc::SetOutputPath(TString path)
{
  fOutputPath = path;
  if (fOutputPath.EndsWith("/"))
    fOutputPath = fOutputPath(0, fOutputPath.Length()-1);
}

void MCalibrationTestCalc::SetOutputFile(TString file)
{ 
  fOutputFile        = file; 
}

// --------------------------------------------------------------------------
//
// Get the output file
// 
const char* MCalibrationTestCalc::GetOutputFile()
{
  return Form("%s/%s", (const char*)fOutputPath, (const char*)fOutputFile);
}

