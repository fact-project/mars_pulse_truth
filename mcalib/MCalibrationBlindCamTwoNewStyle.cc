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
// MCalibrationBlindCamTwoNewStyle                                               
//                                                               
// Blind Pixels Calibration camera after run 31693. The blind pixel camera 
// consisted then of two non-coated blind pixel with very well known 
// quantum efficiency read out in hardware ID 560 and 561.
//
// See also: MCalibrationBlindCam
//
/////////////////////////////////////////////////////////////////////////////
#include "MCalibrationBlindCamTwoNewStyle.h"
#include "MCalibrationBlindPix.h"

ClassImp(MCalibrationBlindCamTwoNewStyle);

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
MCalibrationBlindCamTwoNewStyle::MCalibrationBlindCamTwoNewStyle(const char *name)
    : MCalibrationBlindCam(2,name,"Two Blind Pixels in camera - new style")
{
  CreatePixs    ();    
  CreateAreas   ();   
  CreateAtts    ();   
  CreateQEs     ();     
  CreateCollEffs();
}


// --------------------------------------------------------------------------
//
// Two blind pixels with ID 559 and 560 (software)
//
void MCalibrationBlindCamTwoNewStyle::CreatePixs() 
{

  (*this)[0].SetPixId(559);
  (*this)[1].SetPixId(560);
  
}

// --------------------------------------------------------------------------
//
// Two blind pixels with exactly 100 mm^2 area
//
void MCalibrationBlindCamTwoNewStyle::CreateAreas() 
{

  MCalibrationBlindPix &pix0 = (MCalibrationBlindPix&)(*this)[0];
  pix0.SetArea(100.);

  MCalibrationBlindPix &pix1 = (MCalibrationBlindPix&)(*this)[1];
  pix1.SetArea(100.);
}


// --------------------------------------------------------------------------
//
// Two blind pixels with very well known qe's: David's measurments with 
// 1% error.
//
void MCalibrationBlindCamTwoNewStyle::CreateQEs() 
{

  MCalibrationBlindPix &pix0 = (MCalibrationBlindPix&)(*this)[0];  

  Float_t qe[4];

  qe[    MCalibrationCam::kGREEN ] = 0.166;
  qe[    MCalibrationCam::kBLUE  ] = 0.240;
  qe[    MCalibrationCam::kUV    ] = 0.273;
  qe[    MCalibrationCam::kCT1   ] = 0.273;

  pix0.SetQE(4,qe);  

  Float_t qeerr[4];

  qeerr[    MCalibrationCam::kGREEN ] = 0.004;
  qeerr[    MCalibrationCam::kBLUE  ] = 0.005;
  qeerr[    MCalibrationCam::kUV    ] = 0.006;
  qeerr[    MCalibrationCam::kCT1   ] = 0.006;

  pix0.SetQEErr(4,qeerr);  

  MCalibrationBlindPix &pix1 = (MCalibrationBlindPix&)(*this)[1];

  qe[    MCalibrationCam::kGREEN ] = 0.155;
  qe[    MCalibrationCam::kBLUE  ] = 0.228;
  qe[    MCalibrationCam::kUV    ] = 0.261;
  qe[    MCalibrationCam::kCT1   ] = 0.261;

  pix1.SetQE(4,qe);  

  qeerr[    MCalibrationCam::kGREEN ] = 0.004;
  qeerr[    MCalibrationCam::kBLUE  ] = 0.005;
  qeerr[    MCalibrationCam::kUV    ] = 0.006;
  qeerr[    MCalibrationCam::kCT1   ] = 0.006;

  pix1.SetQEErr(4,qeerr);  

}

// --------------------------------------------------------------------------
//
// One blind pixel with poorly known coll.eff's: email from Eckart with 
// estimates depending on colour, but 5% error (maybe more??)
//
void MCalibrationBlindCamTwoNewStyle::CreateCollEffs() 
{

  MCalibrationBlindPix &pix0 = (MCalibrationBlindPix&)(*this)[0];  

  Float_t colleff[4];

  colleff[    MCalibrationCam::kGREEN ] = 0.99;
  colleff[    MCalibrationCam::kBLUE  ] = 0.93;
  colleff[    MCalibrationCam::kUV    ] = 0.90;
  colleff[    MCalibrationCam::kCT1   ] = 0.90;

  pix0.SetCollEff(4,colleff);  

  Float_t collefferr[4];

  collefferr[    MCalibrationCam::kGREEN ] = 0.05;
  collefferr[    MCalibrationCam::kBLUE  ] = 0.05;
  collefferr[    MCalibrationCam::kUV    ] = 0.05;
  collefferr[    MCalibrationCam::kCT1   ] = 0.05;

  pix0.SetCollEffErr(4,collefferr);  

  MCalibrationBlindPix &pix1 = (MCalibrationBlindPix&)(*this)[1];
  
  colleff[    MCalibrationCam::kGREEN ] = 0.99;
  colleff[    MCalibrationCam::kBLUE  ] = 0.93;
  colleff[    MCalibrationCam::kUV    ] = 0.90;
  colleff[    MCalibrationCam::kCT1   ] = 0.90;

  pix1.SetCollEff(4,colleff);  

  collefferr[    MCalibrationCam::kGREEN ] = 0.05;
  collefferr[    MCalibrationCam::kBLUE  ] = 0.05;
  collefferr[    MCalibrationCam::kUV    ] = 0.05;
  collefferr[    MCalibrationCam::kCT1   ] = 0.05;
  
  pix1.SetCollEffErr(4,collefferr);    
}

// --------------------------------------------------------------------------
//
// One blind pixel has a very well known attenuation 0.01 (datasheet delivered 
// with filter, precision better than 1%
//
// The second blind pixel is not yet so well known, the company does not reply.
// Attenuation: 0.001 (datasheet not delivered with filter, precision guaranteed to 5%)
//
void MCalibrationBlindCamTwoNewStyle::CreateAtts() 
{

  MCalibrationBlindPix &pix0 = (MCalibrationBlindPix&)(*this)[0];  

  Float_t att[4];

  att[MCalibrationCam::kGREEN ] = 1.97;
  att[MCalibrationCam::kBLUE  ] = 1.96;
  att[MCalibrationCam::kUV    ] = 1.95;
  att[MCalibrationCam::kCT1   ] = 1.95;

  pix0.SetAtt(4,att);

  Float_t atterr[4];

  atterr[    MCalibrationCam::kGREEN ] = 0.01;
  atterr[    MCalibrationCam::kBLUE  ] = 0.01;
  atterr[    MCalibrationCam::kUV    ] = 0.01;
  atterr[    MCalibrationCam::kCT1   ] = 0.01;

  pix0.SetAttErr(4,atterr);

  MCalibrationBlindPix &pix1 = (MCalibrationBlindPix&)(*this)[1];  

  att[MCalibrationCam::kGREEN ] = 3.00;
  att[MCalibrationCam::kBLUE  ] = 3.00;
  att[MCalibrationCam::kUV    ] = 3.00;
  att[MCalibrationCam::kCT1   ] = 3.00;

  pix1.SetAtt(4,att);

  atterr[    MCalibrationCam::kGREEN ] = 0.15;
  atterr[    MCalibrationCam::kBLUE  ] = 0.15;
  atterr[    MCalibrationCam::kUV    ] = 0.15;
  atterr[    MCalibrationCam::kCT1   ] = 0.15;

  pix1.SetAttErr(4,atterr);
  
}

