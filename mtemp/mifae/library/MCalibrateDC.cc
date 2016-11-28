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
!   Author(s): Javier López , 5/2004 <mailto:jlopez@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MCalibrateDC
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrateDC.h"

#include <TString.h>
#include <TH1F.h>
#include <TF1.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MGeomCam.h"
#include "MGeomPix.h"
#include "MCameraDC.h"
#include "MTime.h"

#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MReadReports.h"
#include "MGeomApply.h"

ClassImp(MCalibrateDC);
using namespace std;

MCalibrateDC::MCalibrateDC(TString filename, const char *name, const char *title) 
{
  fName  = name  ? name  : "MCalibrateDC";
  fTitle = title ? title : "Taks to intercalibrate the DC of all pmts from a continuos light run";

  fFileName = filename;
  
  fStartingMissCalibration.Set(2004,3,8);
  fEndingMissCalibration.Set(2004,4,15);
  
  Int_t   nbins = 120;
  Float_t min = 0;
  Float_t max = 30.;
  fCalHist = new TH1F("calhist","",nbins,min,max);

  fDCCalibration = 1.;
  fDCCalibrationRMS = 0.;

  fDCMissCalibrationFactor = 30./40.95;
}
MCalibrateDC::~MCalibrateDC()
{
  delete  fCalHist;
}

Int_t MCalibrateDC::PreProcess(MParList *pList)
{

    fGeomCam = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));

    if (!fGeomCam)
    {
      *fLog << err << AddSerialNumber("MGeomCam") << " not found ... aborting" << endl;
      return kFALSE;
    }

    // Initialization of objects that need the information from MGeomCam
    fDisplay.SetGeometry(*fGeomCam);
    fNumPixels = fGeomCam->GetNumPixels();

    fCurr = (MCameraDC*)pList->FindObject(AddSerialNumber("MCameraDC"));

    if (!fCurr)
    {
      *fLog << err << AddSerialNumber("MCameraDC") << " not found ... aborting" << endl;
      return kFALSE;
    }

    fTimeCurr = (MTime*)pList->FindObject(AddSerialNumber("MTimeCurrents"));

    if (!fTimeCurr)
    {
      *fLog << err << AddSerialNumber("MTimeCurrents") << " not found ... aborting" << endl;
      return kFALSE;
    }

    // Run over the continuos light run to get the DC intercalibration factors
    fDCCalibrationFactor.Set(fNumPixels);
    fDCCalibrationFactor.Reset(1.);

    if ( fFileName != "" )
      {
        ProcessFile();
        DCCalibrationCalc();
 
        UInt_t numPixelsSetUnsuedForDC = 0;
        
        for (UInt_t pix=1; pix<fNumPixels; pix++)
          {
            if (fDisplay.GetBinContent(pix+1) > fMinDCAllowed)
              fDCCalibrationFactor[pix] = fDCCalibration/fDisplay.GetBinContent(pix+1);
            else
              {
                numPixelsSetUnsuedForDC++;
                fDCCalibrationFactor[pix] = 0.; //FIXME: Maybe to introduce a setunused in MCameraDC ?
              }
          }

        *fLog << inf << GetName() << " set unused " << numPixelsSetUnsuedForDC << " because too low dc." << endl;
        
      }

    return kTRUE;
}

Int_t MCalibrateDC::Process()
{

  if (*fTimeCurr >= fStartingMissCalibration && *fTimeCurr <= fEndingMissCalibration)
    {
      for (UInt_t pix=1; pix<fNumPixels; pix++)
        {
          MGeomPix& pixel = (*fGeomCam)[pix];
          if (pixel.GetSector() >=3 && pixel.GetSector() <=5)
            (*fCurr)[pix] *= fDCCalibrationFactor[pix]*fDCMissCalibrationFactor;
          else
            (*fCurr)[pix] *= fDCCalibrationFactor[pix];

        }
    }
  else
    for (UInt_t pix=1; pix<fNumPixels; pix++)
      (*fCurr)[pix] *= fDCCalibrationFactor[pix];
	  
  return kTRUE;
}

Bool_t MCalibrateDC::ProcessFile()
{

    MParList plist;
    MTaskList tlist;
    plist.AddToList(&tlist);

    MCameraDC     dccam;
    plist.AddToList(&dccam);

    // Reads the trees of the root file and the analysed branches
    MReadReports read;
    read.AddTree("Currents"); 
    read.AddFile(fFileName);     // after the reading of the trees!!!
    read.AddToBranchList("MReportCurrents.*");
    
    MGeomApply geomapl;

    tlist.AddToList(&geomapl);
    tlist.AddToList(&read);

    // Enable logging to file
    //*fLog.SetOutputFile(lname, kTRUE);

    //
    // Execute the eventloop
    //
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetLogStream(fLog);

    // Execute first analysis
      if (!evtloop.PreProcess())
        {
          *fLog << err << GetDescriptor() << ": Failed." << endl;
	  return kFALSE;
        }
        
      while (tlist.Process())
        fDisplay.AddCamContent(dccam);
        
      evtloop.PostProcess();

    tlist.PrintStatistics();

    UInt_t numexecutions = read.GetNumExecutions();
    UInt_t numPixels = fDisplay.GetNumPixels();
    for (UInt_t pix = 1; pix <= numPixels; pix++)
      fDisplay[pix] /= numexecutions;
    

    *fLog << inf << GetDescriptor() << ": Done." << endl;

    return kTRUE;
}

Bool_t MCalibrateDC::DCCalibrationCalc()
{

   for (UInt_t pix=1; pix<fNumPixels; pix++)
       fCalHist->Fill(fDisplay.GetBinContent(pix+1));

   Float_t nummaxprobdc = fCalHist->GetBinContent(fCalHist->GetMaximumBin());
   Float_t maxprobdc = fCalHist->GetBinCenter(fCalHist->GetMaximumBin());
   UInt_t bin = fCalHist->GetMaximumBin();
   do
   {
       bin++;
   }
   while(fCalHist->GetBinContent(bin)/nummaxprobdc > 0.5);
   Float_t halfmaxprobdc = fCalHist->GetBinCenter(bin);

   *fLog << dbg << " maxprobdc[high] " << maxprobdc << "[" << nummaxprobdc << "] ";
   *fLog << dbg << " halfmaxprobdc[high] " << halfmaxprobdc << "[" << fCalHist->GetBinContent(bin) << "]" << endl;

   Float_t rmsguess = TMath::Abs(maxprobdc-halfmaxprobdc);
   Float_t min = maxprobdc-3*rmsguess;
   min = (min<0.?0.:min);
   Float_t max = maxprobdc+3*rmsguess;

   *fLog << dbg << " maxprobdc " << maxprobdc << " rmsguess " << rmsguess << endl;

   TF1 func("func","gaus",min,max);
   func.SetParameters(nummaxprobdc, maxprobdc, rmsguess);
   
   fCalHist->Fit("func","QR0");

   UInt_t aproxnumdegrees = 6*(bin-fCalHist->GetMaximumBin());
   Float_t chiq = func.GetChisquare();
   fDCCalibration = func.GetParameter(1);
   fDCCalibrationRMS = func.GetParameter(2);

   *fLog << dbg << " fDCCalibration " << fDCCalibration << " fDCCalibrationRMS " << fDCCalibrationRMS << " chiq/ndof " << chiq << "/" << aproxnumdegrees << endl;

   Int_t numsigmas = 5;
   Axis_t minbin = fDCCalibration-numsigmas*fDCCalibrationRMS/fCalHist->GetBinWidth(1);
   minbin=minbin<1?1:minbin;
   Axis_t maxbin = fDCCalibration+numsigmas*fDCCalibrationRMS/fCalHist->GetBinWidth(1);
   *fLog << dbg << " Number of pixels with dc under " << numsigmas << " sigmas = " << fCalHist->Integral((int)minbin,(int)maxbin) << endl;

    //Check results from the fit are consistent
    if (TMath::Abs(fDCCalibration-maxprobdc) > rmsguess || fDCCalibrationRMS > rmsguess)
      {
        *fLog << warn << GetName() << " Calibration DC fit give non consistent results." << endl;
        *fLog << warn << " maxprobdc " << maxprobdc << " rmsguess " << rmsguess << endl;
        *fLog << warn << " fDCCalibration " << fDCCalibration << " fDCCalibrationRMS " << fDCCalibrationRMS << " chiq/ndof " << chiq << "/" << aproxnumdegrees << endl;
        fDCCalibration = maxprobdc;
        fDCCalibrationRMS = rmsguess/1.175; // FWHM=2.35*rms
      }

   return kTRUE;
}

