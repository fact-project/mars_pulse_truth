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
!   Author(s): Markus Gaug, 05/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//
//  readCalibration.C
//
//  Needs as arguments the run number of a calibration file ("*_C_*.root") and 
//  the run number of the corresponding pedestal file ("*_P_*.root"). 
//
//  The TString inpath has to be set correctly.
//
//
/////////////////////////////////////////////////////////////////////////////
static const TString inpath = ".";
//
// the default start calibration run 
//
static const Int_t   calrun = 26209;
//
// Tell if you want to use the display:
//
static Bool_t useDisplay = kFALSE;
//
void readCalibration(const Int_t crun=calrun)
{

  const TString calname  = Form("%s/calib%08d.root", (const char*)inpath,crun);

  if (gSystem->AccessPathName(calname, kFileExists))
    {
      cout << "Input file " << calname << " doesn't exist." << endl;
      return;
    }

  MStatusDisplay = NULL;
  
  if (useDisplay)
    display = new MStatusDisplay;

  MPedestalCam                 pedcam;
  MCalibrationChargeCam        chargecam;  
  MCalibrationQECam            qecam;
  MCalibrationRelTimeCam       relcam;
  MBadPixelsCam                badcam;

  cout << "Reading from file: " << calname << endl;

  TFile calfile(calname, "READ");
  if (chargecam.Read()<=0)
    {
      cout << "Unable to read MCalibrationChargeCam from " << calname << endl;
      return;
    }
  if (qecam.Read()<=0)
    {
      cout << "Unable to read MCalibrationQECam from " << calname << endl;
      return;
    }

  if (calfile.FindKey("MBadPixelsCam"))
      {
        MBadPixelsCam bad;
        if (bad.Read()<=0)
          {
            cout << "Unable to read MBadPixelsCam from " << calname << endl;
            return;
          }
        badcam.Merge(bad);
      }
  if (relcam.Read()<=0)
    {
      cout << "Unable to read MCalibrationRelTimeCam from " << calname << endl;
      return;
    }

  cout << "Size of QECam: "      << qecam.GetSize() << endl;
  cout << "Size of Chargecam: "  << chargecam.GetSize() << endl;
  cout << "Size of RelTimeCam: "  << relcam.GetSize() << endl;

   MGeomCamMagic geom;

  if (useDisplay)
    display->Read();
  
  for (Int_t i=0; i<relcam.GetSize(); i++)
  {
      MCalibrationRelTimePix &rpix = (MCalibrationRelTimePix&)relcam[i];
      MBadPixelsPix &badpix = badcam[i];
      if (!badpix.IsUnsuitable())
        cout << "Number of outliers in time pixel: " << i << ": " << rpix.GetNumPickup()+rpix.GetNumBlackout() << endl;
  } 

  for (Int_t i=0; i<2; i++)
  {  
      TArrayF conv = chargecam.GetAveragedConvFADC2PhotPerArea(geom,qecam,i,&badcam);
      cout << "Average Conv Factor Area idx: " << i << ":  " << conv[0] << "+-" << conv[1] << endl;
  }

  for (Int_t i=0; i<qecam.GetSize(); i++)
    {
      MCalibrationChargePix &cpix = (MCalibrationChargePix&)chargecam[i];
      MCalibrationQEPix     &qpix = (MCalibrationQEPix&)    qecam[i];
      MBadPixelsPix         &bpix  = badcam[i];
      if (!bpix.IsUnsuitable())
        cout << "Conversion Factor: " << cpix.GetMeanConvFADC2Phe() / qpix.GetQECascadesFFactor() << endl;
    }

}

