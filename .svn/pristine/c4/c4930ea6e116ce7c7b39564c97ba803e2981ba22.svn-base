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
!   Author(s): Markus Gaug   02/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHPedestalCam
//
// Fills the extracted pedestals of MExtractedSignalCam into
// the MHCalibrationPix-classes MHPedestalPix for every:
//
// - Pixel, stored in the TObjArray's MHCalibrationCam::fHiGainArray
//   or MHCalibrationCam::fHiGainArray, respectively
//
// - Average pixel per AREA index (e.g. inner and outer for the MAGIC camera),
//   stored in the TObjArray's MHCalibrationCam::fAverageHiGainAreas and
//   MHCalibrationCam::fAverageHiGainAreas
//
// - Average pixel per camera SECTOR (e.g. sectors 1-6 for the MAGIC camera),
//   stored in the TObjArray's MHCalibrationCam::fAverageHiGainSectors
//   and MHCalibrationCam::fAverageHiGainSectors
//
// Every pedestal is directly taken from MExtractedSignalCam, filled by the
// appropriate extractor.
//
// The pedestals are filled into a histogram and an array, in order to perform
// a Fourier analysis (see MHGausEvents). The signals are moreover averaged on an
// event-by-event basis and written into the corresponding average pixels.
//
// The histograms are fitted to a Gaussian, mean and sigma with its errors
// and the fit probability are extracted. If none of these values are NaN's and
// if the probability is bigger than MHGausEvents::fProbLimit (default: 0.5%),
// the fit is declared valid.
// Otherwise, the fit is repeated within ranges of the previous mean
// +- MHCalibrationPix::fPickupLimit (default: 5) sigma (see MHCalibrationPix::RepeatFit())
// In case this does not make the fit valid, the histogram means and RMS's are
// taken directly (see MHCalibrationPix::BypassFit()).
//
// Outliers of more than MHCalibrationPix::fPickupLimit (default: 5) sigmas
// from the mean are counted as Pickup events (stored in MHCalibrationPix::fPickup)
//
// The number of summed FADC slices is taken to re-normalize
// the result to a pedestal per pixel with the formulas (see MHPedestalPix::Renorm()):
// - Mean Pedestal        / slice = Mean Pedestal        / Number slices
// - Mean Pedestal Error  / slice = Mean Pedestal Error  / Number slices
// - Sigma Pedestal       / slice = Sigma Pedestal       / Sqrt (Number slices)
// - Sigma Pedestal Error / slice = Sigma Pedestal Error / Sqrt (Number slices)
//
// The class also fills arrays with the signal vs. event number, creates a fourier
// spectrum (see MHGausEvents::CreateFourierSpectrum()) and investigates if the
// projected fourier components follow an exponential distribution.
//
// This same procedure is performed for the average pixels.
//
// The following results are written into MPedestalCam:
//
// - MCalibrationPix::SetMean()
// - MCalibrationPix::SetMeanErr()
// - MCalibrationPix::SetSigma()
// - MCalibrationPix::SetSigmaErr()
// - MCalibrationPix::SetProb()
// - MCalibrationPix::SetNumPickup()
//
// For all averaged areas, the fitted sigma is multiplied with the square root of
// the number involved pixels in order to be able to compare it to the average of
// sigmas in the camera.
//
/////////////////////////////////////////////////////////////////////////////
#include "MHPedestalCam.h"
#include "MHPedestalPix.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MExtractedSignalCam.h"
#include "MExtractedSignalPix.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

#include "MCalibrationPix.h"

#include "MBadPixelsCam.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include <TH1F.h>
#include <TOrdCollection.h>

ClassImp(MHPedestalCam);

using namespace std;

const Int_t   MHPedestalCam::fgNbins      =  50;
const Axis_t  MHPedestalCam::fgFirst      = -57.5;
const Axis_t  MHPedestalCam::fgLast       = 192.5;
const TString MHPedestalCam::gsHistName   = "Pedestal";
const TString MHPedestalCam::gsHistTitle  = "Pedestal";
const TString MHPedestalCam::gsHistXTitle = "Charge [FADC slices]";
const TString MHPedestalCam::gsHistYTitle = "Nr. events";
const TString MHPedestalCam::fgNamePedestalCam = "MPedestalCam";

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Initializes:
// - fExtractHiGainSlices to 0.
// - the event frequency to 1200 Hz.
// - the fRenormflag to kFALSE
//
// - fNbins to fgNbins
// - fFirst to fgFirst
// - fLast  to fgLast 
//
// - fHistName   to gsHistName  
// - fHistTitle  to gsHistTitle 
// - fHistXTitle to gsHistXTitle
// - fHistYTitle to gsHistYTitle
//
MHPedestalCam::MHPedestalCam(const char *name, const char *title) 
    : fNamePedestalCamOut(fgNamePedestalCam), fNumEvents(0), 
      fExtractHiGainSlices(0.), fPedestalsOut(NULL)
{

  fName  = name  ? name  : "MHPedestalCam";
  fTitle = title ? title : "Class to fill the pedestal histograms";
  
  SetPulserFrequency(1200);
  SetRenorm(kFALSE);
  SetLoGain(kFALSE);

  SetBinning(fgNbins, fgFirst, fgLast);

  SetHistName  (gsHistName  .Data());
  SetHistTitle (gsHistTitle .Data());
  SetHistXTitle(gsHistXTitle.Data());
  SetHistYTitle(gsHistYTitle.Data());

  SetFitStart();
}

// --------------------------------------------------------------------------
//
// Calls MHCalibrationCam::ResetHists()
//
// Resets:
// - fSum
// - fSumSquare
// - fAreaSum
// - fAreaSumSquare
// - fAreaNum
// - fSectorSum
// - fSectorSumSquare
// - fSectorNum
//
void MHPedestalCam::ResetHists()
{
  
  MHCalibrationCam::ResetHists();
  
  fNumEvents = 0;
  
  // If the size is yet set, set the size
  if (fSum.GetSize()>0)
    {
      // Reset contents of arrays.
      fSum.Reset();
      fSumSquare.Reset();
      
      fAreaSum. Reset();
      fAreaSumSquare.Reset();
      fAreaNum.Reset();
      
      fSectorSum. Reset();
      fSectorSumSquare.Reset();
      fSectorNum.Reset();
    }
}
// --------------------------------------------------------------------------
//
// Creates new MHCalibrationChargeCam only with the averaged areas:
// the rest has to be retrieved directly, e.g. via: 
//     MHPedestalCam *cam = MParList::FindObject("MHPedestalCam");
//  -  cam->GetAverageSector(5).DrawClone();
//  -  (*cam)[100].DrawClone()
//
TObject *MHPedestalCam::Clone(const char *) const
{

  MHPedestalCam *cam = new MHPedestalCam();

  //
  // Copy the data members
  //
  cam->fRunNumbers             = fRunNumbers;
  cam->fPulserFrequency        = fPulserFrequency;
  cam->fFlags                  = fFlags;
  cam->fNbins                  = fNbins;
  cam->fFirst                  = fFirst;
  cam->fLast                   = fLast;

  if (!IsAverageing())
    return cam;

  const Int_t navhi   =  fAverageHiGainAreas->GetSize();

  for (int i=0; i<navhi; i++)
    cam->fAverageHiGainAreas->AddAt(GetAverageHiGainArea(i).Clone(),i);

  return cam;
}

// --------------------------------------------------------------------------
//
// Searches pointer to:
// - MPedestalCam
// - MExtractedSignalCam
//
// Searches or creates:
// - MCalibrationPedCam
//
// Retrieves from MExtractedSignalCam:
// - number of used High Gain FADC slices (MExtractedSignalCam::GetNumUsedHiGainFADCSlices())
//
// Initializes, if empty to MGeomCam::GetNumPixels():
// - MHCalibrationCam::fHiGainArray
//
// Initializes, if empty to MGeomCam::GetNumAreas() for:
// - MHCalibrationCam::fAverageHiGainAreas
//
// Initializes, if empty to MGeomCam::GetNumSectors() for:
// - MHCalibrationCam::fAverageHiGainSectors
// 
// Calls MHCalibrationCam::InitPedHists() for every entry in:
// - MHCalibrationCam::fHiGainArray
// - MHCalibrationCam::fAverageHiGainAreas
// - MHCalibrationCam::fAverageHiGainSectors
//
// Sets Titles and Names for the Histograms 
// - MHCalibrationCam::fAverageHiGainAreas
// 
Bool_t MHPedestalCam::ReInitHists(MParList *pList)
{

  MExtractedSignalCam *signal = (MExtractedSignalCam*)pList->FindObject("MExtractedSignalCam");
  if (!signal && fRenorm)
    {
      *fLog << err << "Cannot find MExtractedSignalCam, but re-normalization switched on..."
            << " Cannot find number of used slices ...abort." << endl;
      return kFALSE;
    }
  

  if (!fPedestalsOut)
    fPedestalsOut = (MPedestalCam*)pList->FindObject(AddSerialNumber(fNamePedestalCamOut),"MPedestalCam");

  if (!fPedestalsOut)
    {
      *fLog << err << "Cannot find nor create outgoing MPedestalCam ..." << endl;
      return kFALSE;
    }

  const Int_t npixels  = fGeom->GetNumPixels();
  const Int_t nsectors = fGeom->GetNumSectors();
  const Int_t nareas   = fGeom->GetNumAreas();

  fCam = (MCalibrationCam*)pList->FindObject("MCalibrationPedCam", "MCalibrationCam");
  if (!fCam)
  {
      fCam = (MCalibrationCam*)pList->FindCreateObj("MCalibrationCam", AddSerialNumber("MCalibrationPedCam"));
      if (!fCam)
          return kFALSE;

      fCam->Init(*fGeom);
  }
  
  InitHiGainArrays(npixels,nareas,nsectors);

  if (fSum.GetSize()==0)
    {
      fSum.       Set(npixels);
      fSumSquare.      Set(npixels);
      fAreaSum.   Set(nareas);
      fAreaSumSquare.  Set(nareas);
      fAreaNum.  Set(nareas);
      fSectorSum. Set(nsectors);
      fSectorSumSquare.Set(nsectors);
      fSectorNum.Set(nsectors);
    }

  fNumEvents = 0;

  if (!signal)
    return kTRUE;
  
  
  Float_t sliceshi = signal->GetNumUsedHiGainFADCSlices();
  
  if (sliceshi == 0.)
    {
      *fLog << err << "Number of used signal slices in MExtractedSignalCam is zero  ... abort." 
            << endl;
      return kFALSE;
    }
  
  if (fExtractHiGainSlices != 0. && sliceshi != fExtractHiGainSlices )
    {
      *fLog << err << "Number of used High Gain signal slices changed in MExtractedSignalCam  ... abort." 
            << endl;
      return kFALSE;
    }
  
  fExtractHiGainSlices = sliceshi;
  *fLog << endl;
  *fLog << inf << GetDescriptor() 
        << ": Number of found High Gain signal slices: " << fExtractHiGainSlices << endl;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Initializes the High Gain Arrays:
//
// - For every entry in the expanded arrays: 
//   * Initialize an MHCalibrationChargePix
//   * Set Binning from  fNbins, fFirst and fLast
//   * Set Binning of Abs Times histogram from  fAbsNbins, fAbsFirst and fAbsLast
//   * Set Histgram names and titles from fHistName and fHistTitle
//   * Set Abs Times Histgram names and titles from fAbsHistName and fAbsHistTitle
//   * Set X-axis and Y-axis titles from fHistXTitle and fHistYTitle
//   * Set X-axis and Y-axis titles of Abs Times Histogram from fAbsHistXTitle and fAbsHistYTitle
//   * Call InitHists
//
//
void MHPedestalCam::InitHiGainArrays(const Int_t npixels, const Int_t nareas, const Int_t nsectors)
{
  
  if (fHiGainArray->GetSize()==0)
  {
      for (Int_t i=0; i<npixels; i++)
      {
        fHiGainArray->AddAt(new MHPedestalPix(Form("%sHiGainPix%04d",fHistName.Data(),i),
                                                       Form("%s High Gain Pixel%04d",fHistTitle.Data(),i)),i);

        MHPedestalPix &pix = (MHPedestalPix&)(*this)[i];
        pix.SetBinning(fNbins, fFirst, fLast);
        pix.SetProbLimit(fProbLimit);

        InitHists(pix, (*fBadPixels)[i], i);
      }
  }

  if (!IsAverageing())
    return;

  if (fAverageHiGainAreas->GetSize()==0)
  {
    for (Int_t j=0; j<nareas; j++)
      {
        fAverageHiGainAreas->AddAt(new MHPedestalPix(Form("%sHiGainArea%d",fHistName.Data(),j),
                                                  Form("%s High Gain Area Idx %d",fHistTitle.Data(),j)),j);

        MHPedestalPix &pix = (MHPedestalPix&)GetAverageHiGainArea(j);

        pix.SetBinning(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas),
                       fFirst, fLast);
        
        InitHists(pix, fCam->GetAverageBadArea(j), j);

      }
  }
  
  if (fAverageHiGainSectors->GetSize()==0)
  {
      for (Int_t j=0; j<nsectors; j++)
        {
	  fAverageHiGainSectors->AddAt(new MHPedestalPix(Form("%sHiGainSector%02d",fHistName.Data(),j),
                                                      Form("%s High Gain Sector %02d",fHistTitle.Data(),j)),j);

          MHPedestalPix &pix = (MHPedestalPix&)GetAverageHiGainSector(j);

          pix.SetBinning(fNbins*(Int_t)TMath::Sqrt((Float_t)npixels/nareas),
                         fFirst, fLast);

          InitHists(pix, fCam->GetAverageBadSector(j), j);

      }
  }
}

// -------------------------------------------------------------------------------
//
// Retrieves pointer to MExtractedSignalCam:
//
// Retrieves from MGeomCam:
// - number of pixels
// - number of pixel areas
// - number of sectors
//
// Fills HiGain histograms (MHGausEvents::FillHistAndArray()), respectively
// with the signals MExtractedSignalCam::GetExtractedSignalHiGain().
//
Bool_t MHPedestalCam::FillHists(const MParContainer *par, const Stat_t w)
{

  MPedestalCam *pedestal = (MPedestalCam*)par;
  if (!pedestal)
    {
      *fLog << err << "No argument in MHPedestalCam::Fill... abort." << endl;
      return kFALSE;
    }
  
  const UInt_t npixels  = fGeom->GetNumPixels();
  const UInt_t nareas   = fGeom->GetNumAreas();
  const UInt_t nsectors = fGeom->GetNumSectors();

  MArrayF sumareahi(nareas);
  MArrayF sumsectorhi(nsectors);
  MArrayI numareahi(nareas);
  MArrayI numsectorhi(nsectors);

  for (UInt_t i=0; i<npixels; i++)
  {

      MHCalibrationPix &histhi = (*this)[i];

      if (histhi.IsExcluded())
          continue;

      const MPedestalPix &ped = (*pedestal)[i];

      const Float_t pedes = ped.GetPedestal();

      const Int_t aidx   = (*fGeom)[i].GetAidx();
      const Int_t sector = (*fGeom)[i].GetSector();

      histhi.FillHistAndArray(pedes) ;

      fSum[i]            += pedes;
      fAreaSum[aidx]     += pedes;
      fSectorSum[sector] += pedes;      

      const Float_t sqrsum  = pedes*pedes;
      fSumSquare[i]            += sqrsum;
      fAreaSumSquare[aidx]     += sqrsum;
      fSectorSumSquare[sector] += sqrsum;      

      sumareahi  [aidx]   += pedes;
      numareahi  [aidx]   ++;
      sumsectorhi[sector] += pedes;
      numsectorhi[sector] ++;

    }
  
  for (UInt_t j=0; j<nareas; j++)
    {
      MHCalibrationPix &histhi = GetAverageHiGainArea(j);
      histhi.FillHistAndArray(numareahi[j] == 0 ? 0. : sumareahi[j]/numareahi[j]);
    }
  
  for (UInt_t j=0; j<nsectors; j++)
    {
      MHCalibrationPix &histhi = GetAverageHiGainSector(j);
      histhi.FillHistAndArray(numsectorhi[j] == 0 ? 0. : sumsectorhi[j]/numsectorhi[j]);

    }
  
  fNumEvents++;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calls:
// - MHCalibrationCam::FitHiGainArrays() with Bad Pixels flags 0
// 
Bool_t MHPedestalCam::FinalizeHists()
{
  
  if (fNumEvents <= 1)
    {
      *fLog << err << GetDescriptor()
            << ": Number of processed events smaller or equal to 1" << endl;
      return kFALSE;
    }
  
  FitHists();

  if (fRenorm)
    RenormResults();

  return kTRUE;
  
}

void MHPedestalCam::FitHists()
{

  for (Int_t i=0; i<fHiGainArray->GetSize(); i++)
    {
      
      MHPedestalPix &hist = (MHPedestalPix&)(*this)[i];
      MCalibrationPix &pix   = (*fCam)[i];
      
      //
      // 1) Store calculated means and variances in the low-gain slices
      //
      pix.SetLoGainMean    ( fSum[i] / fNumEvents  );
      const Double_t diff = fSumSquare[i] - fSum[i]*fSum[i]/fNumEvents;
      pix.SetLoGainSigma   ( diff > 0. ? TMath::Sqrt( diff / (fNumEvents-1) ) : 0.);
      pix.SetLoGainMeanVar ( pix.GetLoGainSigma() * pix.GetLoGainSigma() / fNumEvents  );
      pix.SetLoGainSigmaVar( pix.GetLoGainMeanVar() / 4. );

      if (hist.IsEmpty() || hist.IsOnlyOverflow() || hist.IsOnlyUnderflow())
        continue;
      
      //
      // 2) Fit the Hi Gain histograms with a Gaussian
      //
      //      if (i%100)
      //        hist.FitTripleGaus();
      //      else
      TH1F *gaush = hist.GetHGausHist();
      hist.FitGaus("RQ0",fFitStart,gaush->GetBinCenter(gaush->GetXaxis()->GetLast()));
      //
      // 4) Check for oscillations
      // 
      if (IsOscillations())
        hist.CreateFourierSpectrum();
      //
      // 5) Retrieve the results and store them in this class
      //
      pix.SetHiGainMean       ( hist.GetMean()      );
      pix.SetHiGainMeanVar    ( hist.GetMeanErr() * hist.GetMeanErr()   );
      pix.SetHiGainRms        ( hist.GetHistRms()   );
      pix.SetHiGainSigma      ( hist.GetSigma()     );
      pix.SetHiGainSigmaVar   ( hist.GetSigmaErr()* hist.GetSigmaErr()  );
      pix.SetHiGainProb       ( hist.GetProb()      );
      pix.SetHiGainNumBlackout( hist.GetBlackout()  );
      pix.SetHiGainNumPickup  ( hist.GetPickup()    );
    }

  if (!IsAverageing())
    return;
  
  for (Int_t j=0; j<fAverageHiGainAreas->GetSize(); j++)
    {
      
      MHCalibrationPix &hist = GetAverageHiGainArea(j);      
      MCalibrationPix  &pix  = fCam->GetAverageArea(j);

      if (hist.IsEmpty() || hist.IsOnlyOverflow() || hist.IsOnlyUnderflow())
        continue;
      
      //
      // 2) Fit the Hi Gain histograms with a Gaussian
      //
      TH1F *gaush = hist.GetHGausHist();
      hist.FitGaus("RQ0",fFitStart,gaush->GetBinCenter(gaush->GetXaxis()->GetLast()));
      //
      // 4) Check for oscillations
      // 
      if (IsOscillations())
        hist.CreateFourierSpectrum();
      //
      // 5) Retrieve the results and store them in this class
      //
      pix.SetHiGainMean       ( hist.GetMean()      );
      pix.SetHiGainMeanVar    ( hist.GetMeanErr() * hist.GetMeanErr()   );
      pix.SetHiGainRms        ( hist.GetHistRms()   );
      pix.SetHiGainSigma      ( hist.GetSigma()     );
      pix.SetHiGainSigmaVar   ( hist.GetSigmaErr()* hist.GetSigmaErr()  );
      pix.SetHiGainProb       ( hist.GetProb()      );
      pix.SetHiGainNumBlackout( hist.GetBlackout()  );
      pix.SetHiGainNumPickup  ( hist.GetPickup()    );
      //
      // 6) Store calculated means and variances in the low-gain slices
      //
      const ULong_t aevts = fNumEvents * fAreaNum[j];
      if (aevts <= 1)
        continue;

      pix.SetLoGainMean ( fAreaSum[j] / aevts  );
      const Double_t diff = fAreaSumSquare[j] - fAreaSum[j]*fAreaSum[j]/aevts ;
      pix.SetLoGainSigma( diff > 0. ? TMath::Sqrt( diff / (aevts-1) ) : 0.);
    }
  
  for (Int_t j=0; j<fAverageHiGainSectors->GetSize(); j++)
    {
      MHCalibrationPix &hist = GetAverageHiGainSector(j);      
      MCalibrationPix  &pix  = fCam->GetAverageSector(j);

      if (hist.IsEmpty() || hist.IsOnlyOverflow() || hist.IsOnlyUnderflow())
        continue;
      
      //
      // 2) Fit the Hi Gain histograms with a Gaussian
      //
      TH1F *gaush = hist.GetHGausHist();
      hist.FitGaus("RQ0",fFitStart,gaush->GetBinCenter(gaush->GetXaxis()->GetLast()));
      //
      // 4) Check for oscillations
      // 
      if (IsOscillations())
        hist.CreateFourierSpectrum();
      //
      // 5) Retrieve the results and store them in this class
      //
      pix.SetHiGainMean       ( hist.GetMean()      );
      pix.SetHiGainMeanVar    ( hist.GetMeanErr() * hist.GetMeanErr()   );
      pix.SetHiGainRms        ( hist.GetHistRms()   );
      pix.SetHiGainSigma      ( hist.GetSigma()     );
      pix.SetHiGainSigmaVar   ( hist.GetSigmaErr()* hist.GetSigmaErr()  );
      pix.SetHiGainProb       ( hist.GetProb()      );
      pix.SetHiGainNumBlackout( hist.GetBlackout()  );
      pix.SetHiGainNumPickup  ( hist.GetPickup()    );
      //
      // 6) Store calculated means and variances in the low-gain slices
      //
      const ULong_t sevts = fNumEvents * fSectorNum[j];
      if (sevts <= 1)
        continue;

      pix.SetLoGainMean ( fSectorSum[j] / sevts  );
      const Double_t diff = fSectorSumSquare[j] - fSectorSum[j]*fSectorSum[j]/sevts ;
      pix.SetLoGainSigma( diff > 0. ? TMath::Sqrt( diff / (sevts-1) ) : 0.);
    }
}

// --------------------------------------------------------------------------
//
// Renormalizes the pedestal fit results by the following formulae:
//
// - Mean Pedestal        / slice -> Mean Pedestal        / Number slices
// - Mean Pedestal Error  / slice -> Mean Pedestal Error  / Number slices
// - Sigma Pedestal       / slice -> Sigma Pedestal       / Sqrt (Number slices)
// - Sigma Pedestal Error / slice -> Sigma Pedestal Error / Sqrt (Number slices)
// 
void MHPedestalCam::RenormResults()
{

  //
  // One never knows...
  //
  if (fExtractHiGainSlices <= 0)
    return;

  const Float_t sqslices = TMath::Sqrt(fExtractHiGainSlices);

  for (Int_t i=0; i<fHiGainArray->GetSize(); i++)
    {
      
      MCalibrationPix &pix = (*fCam)[i];
      MPedestalPix    &ped = (*fPedestalsOut)[i];
      pix.SetHiGainMean     ( pix.GetHiGainMean()     / fExtractHiGainSlices  );
      pix.SetLoGainMean     ( pix.GetLoGainMean()     / fExtractHiGainSlices  );

      ped.SetPedestal(pix.GetHiGainMean());
      //
      // Mean error goes with PedestalRMS/Sqrt(entries) -> scale with sqrt(slices)
      // 
      pix.SetHiGainMeanVar  ( pix.GetHiGainMeanVar()  / fExtractHiGainSlices );
      pix.SetLoGainMeanVar  ( pix.GetHiGainMeanVar()  / fExtractHiGainSlices );

      //
      // Sigma goes like PedestalRMS -> scale with sqrt(slices)    
      //
      pix.SetHiGainSigma    ( pix.GetHiGainSigma()    / sqslices  );
      pix.SetLoGainSigma    ( pix.GetLoGainSigma()    / sqslices  );

      ped.SetPedestalRms(pix.GetHiGainSigma());
      //
      // Sigma error goes like PedestalRMS/2.(entries) -> scale with sqrt(slices)
      //
      pix.SetHiGainSigmaVar ( pix.GetHiGainSigmaVar() / fExtractHiGainSlices );
      pix.SetLoGainSigmaVar ( pix.GetLoGainSigmaVar() / fExtractHiGainSlices );
    }

  if (!IsAverageing())
    return;
  
  for (Int_t j=0; j<fAverageHiGainAreas->GetSize(); j++)
    {
      
      MCalibrationPix  &pix  = fCam->GetAverageArea(j);
      pix.SetHiGainMean     ( pix.GetHiGainMean()     / fExtractHiGainSlices  );
      pix.SetLoGainMean     ( pix.GetLoGainMean()     / fExtractHiGainSlices  );
      pix.SetHiGainMeanVar  ( pix.GetHiGainMeanVar()  / fExtractHiGainSlices  );
      pix.SetHiGainSigma    ( pix.GetHiGainSigma()    / sqslices  );
      pix.SetLoGainSigma    ( pix.GetLoGainSigma()    / sqslices  );
      pix.SetHiGainSigmaVar ( pix.GetHiGainSigmaVar() / fExtractHiGainSlices  );
  }
  
  for (Int_t j=0; j<fAverageHiGainSectors->GetSize(); j++)
    {
      MCalibrationPix  &pix  = fCam->GetAverageSector(j);
      pix.SetHiGainMean     ( pix.GetHiGainMean()     / fExtractHiGainSlices  );
      pix.SetLoGainMean     ( pix.GetLoGainMean()     / fExtractHiGainSlices  );
      pix.SetHiGainMeanVar  ( pix.GetHiGainMeanVar()  / fExtractHiGainSlices  );
      pix.SetHiGainSigma    ( pix.GetHiGainSigma()    / sqslices  );
      pix.SetLoGainSigma    ( pix.GetLoGainSigma()    / sqslices  );
      pix.SetHiGainSigmaVar ( pix.GetHiGainSigmaVar() / fExtractHiGainSlices  );
    }

}


// ------------------------------------------------------------------
//
// The types are as follows:
// 
// Fitted values:
// ============== 
//
// 0: Fitted Charge
// 1: Error of fitted Charge
// 2: Sigma of fitted Charge
// 3: Error of Sigma of fitted Charge
//
//
// Useful variables derived from the fit results:
// =============================================
//
// 4: Returned probability of Gauss fit to Charge distribution
// 5: Relative differenc of calculated pedestal (per slice) and fitted (per slice)
// 6: Error of the Relative differenc of calculated pedestal (per slice) and fitted (per slice)
// 7: Relative difference of the error of the mean pedestal (per slice) - calculated and fitted
// 8: Relative differenc of calculated pedestal RMS (per slice) and fitted sigma (per slice)
// 9: Error of Relative differenc of calculated pedestal RMS (per slice) and fitted sigma (per slice)
// 10: Relative difference of the error of the pedestal RMS (per slice) - calculated and fitted
//
// Localized defects:
// ==================
//
// 11: Gaus fit not OK
// 12: Fourier spectrum not OK
//
Bool_t MHPedestalCam::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{

  if (fHiGainArray->GetSize() <= idx)
    return kFALSE;

  if ((*this)[idx].IsExcluded())
    return kFALSE;

  const MPedestalPix &ppix = (*fPedestalsOut)[idx];

  const Float_t ped = ppix.GetPedestal();
  const Float_t rms = ppix.GetPedestalRms();

  const Float_t entsqr =  TMath::Sqrt((Float_t)fPedestalsOut->GetNumSlices()*ppix.GetNumEvents());

  const Float_t pederr   = rms/entsqr;
  const Float_t rmserr   = rms/entsqr/2.;

  const Float_t mean     = (*this)[idx].GetMean();
  const Float_t meanerr  = (*this)[idx].GetMeanErr();
  const Float_t sigma    = (*this)[idx].GetSigma() ;
  const Float_t sigmaerr = (*this)[idx].GetSigmaErr();

  switch (type)
    {
    case 0:
      val = mean;
      break;
    case 1:
      val = meanerr;
      break;
    case 2:
      val = sigma;
      break;
    case 3:
      val = sigmaerr;
      break;
    case 4:
      val = (*this)[idx].GetProb();
      break;
    case 5:
      val = 2.*(mean-ped)/(ped+mean);
      break;
    case 6:
      val = TMath::Sqrt((pederr*pederr + meanerr*meanerr) * (ped*ped + mean*mean))
            *2./(ped+mean)/(ped+mean);
      break;
    case 7:
      val = 2.*(meanerr-pederr)/(pederr + meanerr);
      break;
    case 8:
      val = 2.*(sigma-rms)/(sigma+rms);
      break;
    case 9:
      val = TMath::Sqrt((rmserr*rmserr + sigmaerr*sigmaerr) * (rms*rms + sigma*sigma))
            *2./(rms+sigma)/(rms+sigma);
      break;
    case 10:
      val = 2.*(sigmaerr - rmserr)/(sigmaerr + rmserr);
      break;
    case 11:
      if (!(*this)[idx].IsGausFitOK())
        val = 1.;
      break;
    case 12:
      if (!(*this)[idx].IsFourierSpectrumOK())
        val = 1.;
      break;
    default:
      return kFALSE;
    }
  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calls MHGausEvents::DrawClone() for pixel idx
//
void MHPedestalCam::DrawPixelContent(Int_t idx) const
{
  (*this)[idx].DrawClone();
}
