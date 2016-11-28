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
!   Author(s): Markus Gaug   07/2004 <mailto:markus@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                               
// MCalibrationBlindCamOneOldStyle                                               
//                                                               
// Blind Pixels Calibration camera until run 31693. The blind pixel camera 
// consisted then only of one non-coated blind pixel with poorly known 
// quantum efficiency read out in hardware ID 560.
//
// See also: MCalibrationBlindCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationBlindCamOneOldStyle.h"
#include "MCalibrationBlindPix.h"

ClassImp(MCalibrationBlindCamOneOldStyle);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// - CreatePixs();
// - CreateAreas();   
// - CreateAtts();   
// - CreateQEs();     
// - CreateCollEffs();
//
MCalibrationBlindCamOneOldStyle::MCalibrationBlindCamOneOldStyle(const char *name)
    : MCalibrationBlindCam(1,name,"One Blind Pixel in camera - old style")
{

  CreatePixs    ();    
  CreateAreas   ();   
  CreateAtts    ();   
  CreateQEs     ();     
  CreateCollEffs();

}


// --------------------------------------------------------------------------
//
// Only one blind pixel with ID 559
//
void MCalibrationBlindCamOneOldStyle::CreatePixs() 
{

  (*this)[0].SetPixId(559);
  
}

// --------------------------------------------------------------------------
//
// Only one blind pixel with exactly 100 mm^2 area
//
void MCalibrationBlindCamOneOldStyle::CreateAreas() 
{
  
  MCalibrationBlindPix &pix = (MCalibrationBlindPix&)(*this)[0];
  pix.SetArea(100.);
  
}

// --------------------------------------------------------------------------
//
// One blind pixel with poorly known qe's: average of David measurments with 
// 4% error.
//
void MCalibrationBlindCamOneOldStyle::CreateQEs() 
{
  
  MCalibrationBlindPix &pix = (MCalibrationBlindPix&)(*this)[0];

  Float_t qe[4];

  qe[    MCalibrationCam::kGREEN ] = 0.154;
  qe[    MCalibrationCam::kBLUE  ] = 0.226;
  qe[    MCalibrationCam::kUV    ] = 0.247;
  qe[    MCalibrationCam::kCT1   ] = 0.247;

  pix.SetQE(4,qe);  

  Float_t qeerr[4];

  qeerr[ MCalibrationCam::kGREEN ] = 0.005;
  qeerr[ MCalibrationCam::kBLUE  ] = 0.007;
  qeerr[ MCalibrationCam::kUV    ] = 0.01;
  qeerr[ MCalibrationCam::kCT1   ] = 0.01;

  pix.SetQEErr(4,qeerr);  
}

// --------------------------------------------------------------------------
//
// One blind pixel with poorly known coll.eff's: email from Eckart with 
// estimates depending on colour, but 5% error (maybe more??)
//
void MCalibrationBlindCamOneOldStyle::CreateCollEffs() 
{

  MCalibrationBlindPix &pix = (MCalibrationBlindPix&)(*this)[0];

  Float_t colleff[4];

  colleff[MCalibrationCam::kGREEN ] = 0.99;
  colleff[MCalibrationCam::kBLUE  ] = 0.93;
  colleff[MCalibrationCam::kUV    ] = 0.90;
  colleff[MCalibrationCam::kCT1   ] = 0.90;

  pix.SetCollEff(4,colleff);  

  Float_t collefferr[4];

  collefferr[MCalibrationCam::kGREEN ] = 0.05;
  collefferr[MCalibrationCam::kBLUE  ] = 0.05;
  collefferr[MCalibrationCam::kUV    ] = 0.05;
  collefferr[MCalibrationCam::kCT1   ] = 0.05;
  
  pix.SetCollEffErr(4,collefferr);  
}

// --------------------------------------------------------------------------
//
// One blind pixel with very well known attenuation 0.01 (datasheet delivered 
// with filter, precision better than 1%
//
void MCalibrationBlindCamOneOldStyle::CreateAtts() 
{

  MCalibrationBlindPix &pix = (MCalibrationBlindPix&)(*this)[0];

  Float_t att[4];

  att[MCalibrationCam::kGREEN ] = 1.97;
  att[MCalibrationCam::kBLUE  ] = 1.96;
  att[MCalibrationCam::kUV    ] = 1.95;
  att[MCalibrationCam::kCT1   ] = 1.95;

  pix.SetAtt(4,att);

  Float_t atterr[4];

  atterr[MCalibrationCam::kGREEN ] = 0.01;
  atterr[MCalibrationCam::kBLUE  ] = 0.01;
  atterr[MCalibrationCam::kUV    ] = 0.01;
  atterr[MCalibrationCam::kCT1   ] = 0.01;
  
  pix.SetAttErr(4,atterr);

}

