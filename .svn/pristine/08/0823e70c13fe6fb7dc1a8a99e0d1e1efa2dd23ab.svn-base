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
!   Author(s): Wolfgang Wittek, 08/2004 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MStarCamTrans
// ---------------
//
// this is a collection of transformations between
//
// a) celestial (declination, hourangle)      = (  dec,   h); units (deg,hour)
// b) local     (zenith angle, azimuth angle) = (theta, phi); units (deg, deg)
// c) camera    (x-coordinate, y-coordinate)  = (    X,   Y); units ( mm,  mm)
//
// coordinates. As to the definition of the coordinates, the conventions
// of TDAS 00-11 are used. 
//
// The transformations use 
//   - cos(Lat) and sin(Lat) from an MObservatory container, 
//     where 'Lat' is the geographical latitude of the telescope.
//   - fDistCam from an MGeomCam container,
//     which is the distance of the camera from the reflector center
//   
//   in order to apply one of the transformations the  
//      'MGeomCam' and 'MObservatory' containers have to be present;
//      the corresponding objects 'mgeom' and 'mobserv' have to be used as 
//      arguments of the constructor
//
//               MStarCamTrans(mgeom, mobserv);
//
// The tranformations ignore effects like nutation, precession, refraction, ...
// This is not a real problem as long as relative positions are considered, as
// in all the transformations, except in CelToLoc and LocToCel.
//
// The camera coordinates (X, Y) are assumed to be the coordinates for an ideal
// imaging, without imaging errors. The x-axis is assumed to be horizonthal,
// the y-axis is pointing upwards.
//
// the azimuthal angle is defined as :   0 degrees  = north
//                                      90 degrees  = east
//                                     180 degrees  = south
//                                     270 degrees  = west  
// 
// Examples for the use of the transformations :
//
// - if the local coordinates (theta1, phi1) of a point (X1, Y1) in the camera
//   are known one may caculate the local coordinates (theta2, phi2) of any 
//   other point (X2, Y2) in the camera :
//
//         LocCamCamToLoc(theta1, phi1, X1, Y1, X2, Y2, theta2, phi2);
//
//   if (X1, Y1) = (0, 0) one may use
//                        Loc0CamToLoc(theta1, phi1, X2, Y2, theta2, phi2);
//

// - if the local coordinates (theta1, phi1) of a point (X1, Y1) in the camera
//   are known one may caculate the position (X2, Y2) for another direction
//   (theta2, phi2) :
//
//         LocCamLocToCam(theta1, phi1, X1, Y1, theta2, phi2, X2, Y2);
//
//   if (X1, Y1) = (0, 0) one may use
//                        Loc0LocToCam(theta1, phi1, X2, Y2, theta2, phi2);
//
// - if the celestial coordinates (dec1, h1) of a point (X1, Y1) in the camera
//   are known one may caculate the celestial coordinates (dec2, h2) of any 
//   other point (X2, Y2) in the camera :
//
//         CelCamCamToCel(dec1, h1, X1, Y1, X2, Y2, dec2, h2);
//
//   if (X1, Y1) = (0, 0) one may use
//                        Cel0CamToCel(dec1, h1, X2, Y2, dec2, h2);
//
//
// - if the celestial coordinates (dec1, h1) of a point (X1, Y1) in the camera
//   are known one may caculate the position (X2, Y2) for any other direction :
//
//         CelCamCelToCam(dec1, h1, X1, Y1, dec2, h2, X2, Y2);
//
//   if (X1, Y1) = (0, 0) one may use
//                        Cel0CelToCam(dec1, h1, dec2, h2, X2, Y2);
//
//
//
////////////////////////////////////////////////////////////////////////////
#include "MStarCamTrans.h"

#include <math.h>

#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TCanvas.h>

//#include "MH.h"
#include "MLog.h"
#include "MLogManip.h"
#include "MGeomCam.h"
#include "MObservatory.h"

using namespace std;

ClassImp(MStarCamTrans);


// --------------------------------------------------------------------------
//
// Constructor
// 
//    get distance between camera and reflector from an MGeomCam     container
//    get cos(Lat) and sin(Lat) from                 an MObservatory container
//
MStarCamTrans::MStarCamTrans(const MGeomCam &cam, const MObservatory &obs) 
{
  if (&cam == NULL)
  {
    gLog << err << "MStarCamTrans::MStarCamTrans;  MGeomCam container is not available"
          << endl;
    return;
  }

  if (&obs == NULL)
  {
    gLog << err << "MStarCamTrans::MStarCamTrans;  MObservatory container is not available"
          << endl;
    return;
  }

  fDistCam = cam.GetCameraDist() * 1000.0;     //  [mm]
  fCLat    = obs.GetCosPhi();
  fSLat    = obs.GetSinPhi();

  if (fDistCam*fCLat*fSLat == 0.0)
  {
    gLog << "MStarCamTrans::TransCelLocCam; one of 'fDistCam, fCLat, fSLat' is zero !!!$$$$$$$$$$$$$$$$$ : "
        << fDistCam << ",  " << fCLat << ",  " << fSLat << endl;
  }


  fGridBinning = 0.50;   // degrees
  fGridFineBin = 0.01;   // degrees

}

// --------------------------------------------------------------------------
//
// Loc0CamToLoc
//
// Input :   (theta0, phi0)    direction for the position (0,0) in the camera  
//           (     X,    Y)    a position in the camera
// 
// Output :  ( theta,  phi)    direction for the position (X,Y) in the camera
//
Bool_t MStarCamTrans::Loc0CamToLoc(Double_t theta0deg, Double_t phi0deg,
                                     Double_t X,         Double_t Y,
                                     Double_t &thetadeg, Double_t &phideg) 
{
  Double_t theta0 = theta0deg / kRad2Deg;
  Double_t phi0   = phi0deg   / kRad2Deg;

  Double_t XC = X / fDistCam;
  Double_t YC = Y / fDistCam;

  Double_t theta;
  Double_t phiminphi0;

  //--------------------------------------------
  Double_t sip = -XC;
  Double_t cop = sin(theta0) + YC*cos(theta0);

  Double_t sit = sqrt(cop*cop + XC*XC);
  Double_t cot = cos(theta0) - YC*sin(theta0);

  // there is an ambiguity in the sign of cos(theta)
  // - if theta0 is close to 0 or Pi 
  //   choose that theta which is closer to theta0,
  //   i.e. require the same sign for cos(theta) and cos(theta0)
  // - otherwise choose that theta which is compatible with a small
  //   difference |phi-phi0|, i.e. cos(phi-phi0) > 0

  if( fabs(theta0deg - 90.0) > 45.0 )
    theta = TMath::ATan2( sit, TMath::Sign(cot, cos(theta0)) );
  else
    theta = TMath::ATan2( sit, TMath::Sign(cot, cop/cot) );

  Double_t sig = TMath::Sign(1.0, cot*tan(theta));
  phiminphi0 = TMath::ATan2(sig*sip, sig*cop);

  //--------------------------------------------
  phideg   = (phi0 + phiminphi0) * kRad2Deg;
  thetadeg =               theta * kRad2Deg;

  //gLog << "MStarCamTrans::Loc0Cam2Log; theta0deg, phi0deg, X, Y, thetadeg, phideg = " 
  //      << theta0deg << ",  " << phi0deg << ",  " << X << ",  " << Y << ",  "
  //      << thetadeg << ",  " << phideg << endl;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Loc0LocToCam
//
// Input :   (theta0, phi0)   direction for the position (0,0) in the camera  
//           ( theta,  phi)   some other direction
// 
// Output :  (X, Y)      position in the camera corresponding to (theta, phi)
//
//  (see also MAstro::GetDistOnPlain())
//
Bool_t MStarCamTrans::Loc0LocToCam(Double_t theta0deg, Double_t phi0deg,
                                     Double_t thetadeg,  Double_t phideg,
                                     Double_t &X,        Double_t &Y)
{
  Double_t theta0 = theta0deg / kRad2Deg;
  Double_t phi0   = phi0deg   / kRad2Deg;

  Double_t theta = thetadeg / kRad2Deg;
  Double_t phi   = phideg   / kRad2Deg;

  //--------------------------------------------
  Double_t YC =   (cos(theta0)*tan(theta)*cos(phi-phi0) - sin(theta0))
                / (cos(theta0) + sin(theta0)*tan(theta)); 
  Double_t XC = -sin(phi-phi0) * (cos(theta0) - YC*sin(theta0)) * tan(theta);

  //--------------------------------------------
  X = XC * fDistCam;
  Y = YC * fDistCam;


  //gLog << "MStarCamTrans::Loc0LocToCam; theta0deg, phi0deg, X, Y, thetadeg, phideg = " 
  //      << theta0deg << ",  " << phi0deg << ",  " << X << ",  " << Y << ",  "
  //      << thetadeg << ",  " << phideg << endl;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// LocCamToLoc0
//
// Input :   ( X, Y)         a position in the camera
//           (theta, phi)    direction for the position (X,Y) in the camera
// 
// Output :  ( theta0,  phi0)    direction for the position (0,0) in the camera
//
Bool_t MStarCamTrans::LocCamToLoc0(Double_t thetadeg,   Double_t phideg,
                                     Double_t X,          Double_t Y,
                                     Double_t &theta0deg, Double_t &phi0deg) 
{
  Double_t theta = thetadeg / kRad2Deg;
  Double_t phi   = phideg   / kRad2Deg;

  Double_t XC = X / fDistCam;
  Double_t YC = Y / fDistCam;

  //--------------------------------------------

  Double_t theta0;
  Double_t phiminphi0;

  Double_t tant1 = 0.0;
  Double_t tant2 = 0.0;

  Double_t ip;

  // calculate tan(theta0)
  // dummy loop to avoid a 'go to'
  for (Int_t i=0; i<1; i++)
  {
    if ( tan(theta) == 0.0 )
    {
      if (XC != 0.0)
      {
        // this can never occur
        gLog << "MStarCamTrans::LocCam2Loc0; thetadeg, XC = " << thetadeg
              << ",  " << XC << endl;
         return kFALSE;
      }
      tant1 = -YC;

      theta0 = TMath::Pi() *
               modf( (atan(tant1)+TMath::Pi()) / TMath::Pi(), &ip );
      phiminphi0 = 0.0;

      break;
    }

    Double_t a = 1.0   + XC*XC -  YC*YC*tan(theta)*tan(theta);
    Double_t b = 2.0   * YC    * (1.0 + tan(theta)*tan(theta));
    Double_t c = XC*XC + YC*YC -        tan(theta)*tan(theta);

    if (a == 0.0)
    {
      if (b == 0.0)
      {
        // this can never occur
        gLog << "MStarCamTrans::LocCam2Loc0; a, b = " << a << ",  "
              << b << endl;
         return kFALSE;
      }
      tant1 = -c/b;  
    }
    else
    {
      Double_t discr = b*b - 4.0*a*c;
      if (discr < 0.0)
      {
        gLog << "MStarCamTrans::LocCam2Loc0; discr = " << discr << endl;
        return kFALSE;
      }

      // two solutions for tan(theta0)
      tant1 = (-b + sqrt(discr)) / (2.0*a);
      tant2 = (-b - sqrt(discr)) / (2.0*a);

      if (fabs(tant1-tant2) < 1.e-5)
        tant2 = 0.0;
    }

    // define the sign of tan(theta0) and 
    // reject the solution with the wrong sign

    if ( fabs(thetadeg - 90.0) > 45.0 )
    {
      Double_t sig = TMath::Sign(1.0, cos(theta));
      if ( tant1 != 0.0  &&  TMath::Sign(1.0, tant1) != sig )
	   tant1 = 0.0; 

      if ( tant2 != 0.0  &&  TMath::Sign(1.0, tant2) != sig )
	   tant2 = 0.0; 
    }
    else
    {
      // require sign of cos(phi-phi0) to be > 0
      if ( tant1 != 0.0  &&
           TMath::Sign( 1.0, (tant1+YC)/ ((1.0-YC*tant1)*tan(theta)) ) != 1.0 )
        tant1 = 0.0;

      if ( tant2 != 0.0  &&
           TMath::Sign( 1.0, (tant2+YC)/ ((1.0-YC*tant2)*tan(theta)) ) != 1.0 )
        tant2 = 0.0;
    }

    if (tant1 != 0.0  &&  tant2 != 0.0)
    {
      gLog << "MStarCamTrans::LocCam2Loc0; there are 2 solutions for tan(theta0),  tant1, tant2 = " 
            << tant1 << ",  " << tant2 << endl;
    }

    if (tant1 != 0.0)
      theta0 = TMath::Pi() *
               modf( (atan(tant1)+TMath::Pi()) / TMath::Pi(), &ip );
    else if (tant2 != 0.0)
      theta0 = TMath::Pi() *
               modf( (atan(tant2)+TMath::Pi()) / TMath::Pi(), &ip );
    else
    {
      theta0 = theta;
      gLog << "MStarCamTrans::LocCam2Loc0; there is no solution for tan(theta0)"
            << endl;
    }

    Double_t sip = -XC;
    Double_t cop = sin(theta0) + YC*cos(theta0);
    Double_t cot = cos(theta0) - YC*sin(theta0);
    Double_t sig = TMath::Sign(1.0, cot*tan(theta));

    phiminphi0 = TMath::ATan2(sig*sip, sig*cop);
  } // end of dummy loop

  //--------------------------------------------
  phi0deg   = (phi - phiminphi0) * kRad2Deg;
  theta0deg =             theta0 * kRad2Deg;

  //gLog << "MStarCamTrans::LocCamToLoc0; theta0deg, phi0deg, X, Y, thetadeg, phideg = " 
  //      << theta0deg << ",  " << phi0deg << ",  " << X << ",  " << Y << ",  "
  //      << thetadeg << ",  " << phideg << endl;


  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Cel0CamToCel
//
// Input :   (dec0, h0)    direction for the position (0,0) in the camera  
//           (   X,  Y)    a position in the camera
// 
// Output :  (dec,   h)    direction for the position (X,Y) in the camera
//
Bool_t MStarCamTrans::Cel0CamToCel(Double_t dec0deg, Double_t h0hour,
                                     Double_t X,       Double_t Y,
                                     Double_t &decdeg, Double_t &hhour) 
{
  //--------------------------------------------

  // transform celestial coordinates (  dec0,   h0) 
  //      into local coordinates     (theta0, phi0)
  Double_t theta0deg;
  Double_t phi0deg;
  CelToLoc(dec0deg, h0hour, theta0deg, phi0deg);

  // get the local coordinates (theta, phi) 
  //         for the position (X, Y) in the camera
  Double_t thetadeg;
  Double_t phideg;
  Loc0CamToLoc(theta0deg, phi0deg, X, Y, thetadeg, phideg);

  // transform local coordinates     (theta, phi)
  //      into celestial coordinates (  dec,   h) 
  LocToCel(thetadeg, phideg, decdeg, hhour);

  //--------------------------------------------


  //gLog << "MStarCamTrans::Cel0CamToCel; theta0deg, phi0deg, X, Y, thetadeg, phideg = " 
  //      << theta0deg << ",  " << phi0deg << ",  " << X << ",  " << Y << ",  "
  //      << thetadeg << ",  " << phideg << endl;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Cel0CelToCam
//
// Input :   (dec0, h0)   direction for the position (0,0) in the camera  
//           (dec,   h)   some other direction
// 
// Output :  (X, Y)      position in the camera corresponding to (dec, h)
//
Bool_t MStarCamTrans::Cel0CelToCam(Double_t dec0deg, Double_t h0hour,
                                     Double_t decdeg,  Double_t hhour,
                                     Double_t &X,        Double_t &Y)
{
  //--------------------------------------------

  // transform celestial coordinates (  dec0,   h0) 
  //      into local coordinates     (theta0, phi0)
  Double_t theta0deg;
  Double_t phi0deg;
  CelToLoc(dec0deg, h0hour, theta0deg, phi0deg);

  // transform celestial coordinates (  dec,   h) 
  //      into local coordinates     (theta, phi)
  Double_t thetadeg;
  Double_t phideg;
  CelToLoc(decdeg, hhour, thetadeg, phideg);

  // get the position (X, Y) in the camera
  // from the local coordinates (theta, phi) 
  Loc0LocToCam(theta0deg, phi0deg, thetadeg, phideg, X, Y);

  //--------------------------------------------

  //gLog << "MStarCamTrans::Cel0CelToCam; theta0deg, phi0deg, X, Y, thetadeg, phideg = " 
  //      << theta0deg << ",  " << phi0deg << ",  " << X << ",  " << Y << ",  "
  //      << thetadeg << ",  " << phideg << endl;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// CelCamToCel0
//
// Input :   ( X,    Y)    a position in the camera
//           (dec,   h)    direction for the position (X,Y) in the camera
// 
// Output :  (dec0, h0)    direction for the position (0,0) in the camera
//

Bool_t MStarCamTrans::CelCamToCel0(Double_t decdeg,   Double_t hhour, 
                                     Double_t X,        Double_t Y,
                                     Double_t &dec0deg, Double_t &h0hour) 
{
  //--------------------------------------------

  // transform celestial coordinates (  dec,   h) 
  //      into local coordinates     (theta, phi)
  Double_t thetadeg;
  Double_t phideg;
  CelToLoc(decdeg, hhour, thetadeg, phideg);

  // get the local coordinates (theta, phi) 
  //         for the position (0, 0) in the camera
  Double_t theta0deg;
  Double_t phi0deg;
  LocCamToLoc0(thetadeg, phideg, X, Y, theta0deg, phi0deg);

  // transform local coordinates     (theta0, phi0)
  //      into celestial coordinates (  dec0,   h0) 
  LocToCel(theta0deg, phi0deg, dec0deg, h0hour);

  //--------------------------------------------


  //gLog << "MStarCamTrans::CelCamToCel0; theta0deg, phi0deg, X, Y, thetadeg, phideg = " 
  //      << theta0deg << ",  " << phi0deg << ",  " << X << ",  " << Y << ",  "
  //      << thetadeg << ",  " << phideg << endl;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// LocCamCamToLoc
//
// Input :   (theta1, phi1)  direction for the position (X1,Y1) in the camera  
//           (    X1,   Y1)  a position in the camera
//           (    X2,   Y2)  another position in the camera 
// 
// Output :  (theta2, phi2)  direction for the position (X2,Y2) in the camera
//
Bool_t MStarCamTrans::LocCamCamToLoc(Double_t theta1deg, Double_t phi1deg,
                                      Double_t X1,         Double_t Y1,
                                      Double_t X2,         Double_t Y2,
                                      Double_t &theta2deg, Double_t &phi2deg)
{
  if (X1 == 0.0  && Y1 == 0.0)
  {
    Loc0CamToLoc(theta1deg, phi1deg, X2, Y2, theta2deg, phi2deg); 
    return kTRUE;
  }

  if (X2 == 0.0  && Y2 == 0.0)
  {
    LocCamToLoc0(theta1deg, phi1deg, X1, Y1, theta2deg, phi2deg); 
    return kTRUE;
  }

  Double_t theta0deg;
  Double_t phi0deg;
  LocCamToLoc0(theta1deg, phi1deg, X1, Y1, theta0deg, phi0deg); 

  Loc0CamToLoc(theta0deg, phi0deg, X2, Y2, theta2deg, phi2deg); 

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// LocCamLocToCam
//
// Input :   (theta1, phi1)  direction for the position (X1,Y1) in the camera  
//           (    X1,   Y1)  a position in the camera
//           (theta2, phi2)  another direction
// 
// Output :  (    X2,   Y2)  position corresponding to (theta2, phi2)
//
Bool_t MStarCamTrans::LocCamLocToCam(Double_t theta1deg, Double_t phi1deg,
                                       Double_t X1,        Double_t Y1,
				       Double_t theta2deg, Double_t phi2deg,
				       Double_t &X2,       Double_t &Y2)

{
  if (X1 == 0.0  && Y1 == 0.0)
  {
    Loc0LocToCam(theta1deg, phi1deg, theta2deg, phi2deg, X2, Y2);

    return kTRUE;
  }


  Double_t theta0deg;
  Double_t phi0deg;
  LocCamToLoc0(theta1deg, phi1deg, X1, Y1, theta0deg, phi0deg); 

  Loc0LocToCam(theta0deg, phi0deg, theta2deg, phi2deg, X2, Y2);

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// CelCamCamToCel
//
// Input :   (dec1, h1)  direction for the position (X1,Y1) in the camera  
//           (  X1, Y1)  a position in the camera
//           (  X2, Y2)  another position in the camera 
// 
// Output :  (dec2, h2)  direction for the position (X2,Y2) in the camera
//
Bool_t MStarCamTrans::CelCamCamToCel(Double_t dec1deg, Double_t h1deg,
                                      Double_t X1,       Double_t Y1,
                                      Double_t X2,       Double_t Y2,
                                      Double_t &dec2deg, Double_t &h2deg) 
{
  if (X1 == 0.0  && Y1 == 0.0)
  {
    Cel0CamToCel(dec1deg, h1deg, X2, Y2, dec2deg, h2deg); 
    return kTRUE;
  }

  if (X2 == 0.0  && Y2 == 0.0)
  {
    CelCamToCel0(dec1deg, h1deg, X1, Y1, dec2deg, h2deg); 
    return kTRUE;
  }

  Double_t dec0deg;
  Double_t h0deg;
  CelCamToCel0(dec1deg, h1deg, X1, Y1, dec0deg, h0deg); 

  Cel0CamToCel(dec0deg, h0deg, X2, Y2, dec2deg, h2deg); 

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// CelCamCelToCam
//
// Input :   (dec1, h1)  direction for the position (X1,Y1) in the camera  
//           (  X1, Y1)  a position in the camera
//           (dec2, h2)  another direction
// 
// Output :  (  X2, Y2)  position corresponding to (dec2, h2)
//
Bool_t MStarCamTrans::CelCamCelToCam(Double_t dec1deg, Double_t h1deg,
                                       Double_t X1,      Double_t Y1,
				       Double_t dec2deg, Double_t h2deg,
				       Double_t &X2,     Double_t &Y2)

{
  if (X1 == 0.0  && Y1 == 0.0)
  {
    Cel0CelToCam(dec1deg, h1deg, dec2deg, h2deg, X2, Y2);

    return kTRUE;
  }


  Double_t dec0deg;
  Double_t h0deg;
  CelCamToCel0(dec1deg, h1deg, X1, Y1, dec0deg, h0deg); 

  Cel0CelToCam(dec0deg, h0deg, dec2deg, h2deg, X2, Y2);

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// CelToLoc
//
// Input :   (dec,     h)    celestial coordinates
// 
// Output :  (theta, phi)    corresponding local coordinates
//
// (see also MAstroCatalog and MAstroSky2Local)
//
Bool_t MStarCamTrans::CelToLoc(Double_t decdeg,    Double_t hhour,
                                 Double_t &thetadeg, Double_t &phideg) 
{
  Double_t a1 =  fCLat;
  Double_t a3 = -fSLat;

  Double_t dec = decdeg / kRad2Deg;
  Double_t h   =  hhour / 24.0 * TMath::TwoPi();

  // transform celestial coordinates (  dec,   h) 
  //      into local coordinates     (theta, phi)
  Double_t xB =   cos(dec) * cos(h);
  Double_t yB =   cos(dec) * sin(h);
  Double_t zB = - sin(dec);

  Double_t xA =  a3*xB        - a1*zB;
  Double_t yA =         - yB;
  Double_t zA = -a1*xB        - a3*zB;

  thetadeg = acos(-zA)             * kRad2Deg;
  phideg   = TMath::ATan2(yA, xA)  * kRad2Deg;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// LocToCel
// 
// Input :   (theta, phi)    local coordinates
//
// Output :  (dec,     h)    corresponding celestial coordinates
//
// (see also MAstroCatalog and MAstroSky2Local)
//
Bool_t MStarCamTrans::LocToCel(Double_t thetadeg, Double_t phideg,
                                 Double_t &decdeg,  Double_t &hhour) 
{
  Double_t a1 =  fCLat;
  Double_t a3 = -fSLat;

  Double_t theta = thetadeg / kRad2Deg;
  Double_t phi   =   phideg / kRad2Deg;

  //--------------------------------------------

  // transform local coordinates     (theta, phi)
  //      into celestial coordinates (  dec,   h) 
  Double_t xA =   sin(theta) * cos(phi);
  Double_t yA =   sin(theta) * sin(phi);
  Double_t zA = - cos(theta);

  Double_t xB =  a3*xA        - a1*zA;
  Double_t yB =         - yA;
  Double_t zB = -a1*xA        - a3*zA;
  
  Double_t dec = asin(-zB);
  Double_t h   = TMath::ATan2(yB, xB);

  //--------------------------------------------
  decdeg = dec * kRad2Deg;
  hhour  = h * 24.0 / TMath::TwoPi();


  return kTRUE;
}

// --------------------------------------------------------------------------
//
// PlotGridAtDec0H0
// 
//     set the celestial coordinates of the camera center     
//     and plot the lines of constant (Theta, Phi)
//          and the lines of constant (Dec,   H  )
//
// (see also MAstroCatalog::Draw and MAstroCamera::Draw)
//
// Warning: Leaks Memory!
//
Bool_t MStarCamTrans::PlotGridAtDec0H0(TString name,
                                         Double_t dec0deg, Double_t h0hour) 
{
  Double_t theta0deg;
  Double_t phi0deg;
  CelToLoc(dec0deg, h0hour, theta0deg, phi0deg);

  PlotGrid(name, theta0deg, phi0deg, dec0deg, h0hour);

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// PlotGridAtTheta0Phi0
// 
//     set the local coordinates of the camera center     
//     and plot the lines of constant (Theta, Phi)
//          and the lines of constant (Dec,   H  )
//
// (see also MAstroCatalog::Draw and MAstroCamera::Draw)
//
// Warning: Leaks Memory!
//
Bool_t MStarCamTrans::PlotGridAtTheta0Phi0(TString name,
                                    Double_t theta0deg, Double_t phi0deg) 
{
  Double_t dec0deg;
  Double_t h0hour;
  LocToCel(theta0deg, phi0deg, dec0deg, h0hour);

  PlotGrid(name, theta0deg, phi0deg, dec0deg, h0hour);

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// SetGridParameters
// 
//     set the binning for the grid    (fGridBinning)
//     set the binning along the lines (fGridFineBin)
//
Bool_t MStarCamTrans::SetGridParameters(
             Double_t gridbinning, Double_t gridfinebin) 
{
  fGridBinning = gridbinning;
  fGridFineBin = gridfinebin;

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// PlotGrid
// 
// - plot the lines of constant (Theta, Phi) 
//                              with the camera center at (Theta0, Phi0)
// -  and the lines of constant (Dec,   H  ) 
//                              with the camera center at (Dec0,   H0  )
//
// (see also MAstroCatalog::Draw and MAstroCamera::Draw)
//
// Warning: Leaks Memory! 
//
Bool_t MStarCamTrans::PlotGrid(TString name,
                                 Double_t theta0deg, Double_t phi0deg,
                                 Double_t dec0deg,   Double_t h0hour) 
{
    Double_t mmtodeg = 180.0 / TMath::Pi() / fDistCam ;
    //gLog << "mmtodeg = " << mmtodeg << endl;

    // aberr   is the ratio r_optaberr/r_ideal between 
    //         the distance from the camera center with optical aberration and
    //         the distance from the camera center with an ideal imaging
    // the value 1.07 is valid if the expected position (with aberration)
    // in the camera is calculated as the average of the positions obtained
    // from the reflections at the individual mirrors
    Double_t aberr = 1.07;

    //--------------------------------------------------------------------

    TCanvas *c1 = new TCanvas(name, name, 0, 0, 300, 600);

    //gROOT->Reset();
    gStyle->SetCanvasColor(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetFrameBorderMode(0);
    gStyle->SetOptStat(0000000);
    gStyle->SetPalette(1);

    c1->Divide(1,2);
    c1->SetBorderMode(0);

    Double_t xlo = -534.0 * mmtodeg;
    Double_t xup =  534.0 * mmtodeg;

    Double_t ylo = -534.0 * mmtodeg;
    Double_t yup =  534.0 * mmtodeg;

    TString nam1 = name;
    nam1 += "_:_Theta-Phi";
    TString tit1 = name;
    tit1 += "_:_Theta-Phi-lines";
    TH2D *plot1 = new TH2D(nam1, tit1, 1, xlo, xup, 1, ylo, yup);
    plot1->GetXaxis()->SetTitle("x [deg]");
    plot1->GetYaxis()->SetTitle("y [deg]");


    TString nam2 = name;
    nam2 += "_:_Dec-Hour";
    TString tit2 = name;
    tit2 += "_:_Dec-Hour-lines";
    TH2D *plot2 = new TH2D(nam2, tit2, 1, xlo, xup, 1, ylo, yup);
    plot2->GetXaxis()->SetTitle("x [deg]");
    plot2->GetYaxis()->SetTitle("y [deg]");


    c1->cd(1);
    plot1->Draw();
    //delete plot1;

    c1->cd(2);
    plot2->Draw();
    //delete plot2;

    TGraph *graph1;
    TLatex *pix;

    Char_t tit[100];
    Double_t xtxt;
    Double_t ytxt;

    Double_t xx;
    Double_t yy;

    Double_t gridbinning = fGridBinning;
    Double_t gridfinebin = fGridFineBin;
    Int_t    numgridlines = (Int_t)(4.0/gridbinning);


    //--------------------------------------------------------------------
    // Theta-Phi lines

    // direction for the center of the camera
    Double_t theta0 = theta0deg;
    Double_t phi0   = phi0deg;
    

    //-----   lines for fixed theta   ------------------------------------

    const Int_t Ntheta = numgridlines;
    Double_t theta[Ntheta];
    Double_t dtheta = gridbinning;

    Int_t nphi = (Int_t)(4.0/gridfinebin);
    const Int_t Nphi   = nphi+1;
    Double_t phi[Nphi];
    Double_t dphi = gridfinebin/sin(theta0/180.0*3.1415926);
    if ( dphi > 360.0/((Double_t)(Nphi-1)) )
      dphi = 360.0/((Double_t)(Nphi-1));

    for (Int_t j=0; j<Ntheta; j++)
    {
      theta[j] = theta0 + ((Double_t)j)*dtheta 
                        - ((Double_t)(Ntheta/2)-1.0)*dtheta;
    }

    for (Int_t k=0; k<Nphi; k++)
    {
      phi[k] = phi0 + ((Double_t)k)*dphi - ((Double_t)(Nphi/2)-1.0)*dphi;
    }


    Double_t x[Nphi];
    Double_t y[Nphi];
	
    

    for (Int_t j=0; j<Ntheta; j++)
    {
      if (theta[j] < 0.0) continue;

      for (Int_t k=0; k<Nphi; k++)
      {
        Loc0LocToCam(theta0, phi0, theta[j], phi[k],
                           xx, yy);
        x[k] = xx * mmtodeg * aberr;
        y[k] = yy * mmtodeg * aberr;

        //gLog << "theta0, phi0 = " << theta0 << ",  " << phi0 
        //     << " : theta, phi, x, y = " << theta[j] << ",  "
        //     << phi[k] << ";   " << x[k] << ",  " << y[k] << endl;  
      }

      c1->cd(1);
      graph1 = new TGraph(Nphi,x,y);
      graph1->SetLineColor(j+1);
      graph1->SetLineStyle(1);
      graph1->SetMarkerColor(j+1);
      graph1->SetMarkerSize(.2);
      graph1->SetMarkerStyle(20);
      graph1->Draw("PL");
      //delete graph1;

      sprintf(tit,"Theta = %6.2f", theta[j]);
      xtxt = xlo + (xup-xlo)*0.1;
      ytxt = ylo + (yup-ylo)*0.80 - ((Double_t)j) *(yup-ylo)/20.0;
      pix = new TLatex(xtxt, ytxt, tit);
      pix->SetTextColor(j+1);
      pix->SetTextSize(.03);
      pix->Draw("");
      //delete pix;

    }

    //-----   lines for fixed phi   ------------------------------------

    Int_t ntheta1 = (Int_t)(4.0/gridfinebin);
    const Int_t Ntheta1 = ntheta1;
    Double_t theta1[Ntheta1];
    Double_t dtheta1 = gridfinebin;

    const Int_t Nphi1   = numgridlines;
    Double_t phi1[Nphi1];
    Double_t dphi1 = gridbinning/sin(theta0/180.0*3.1415926);
    if ( dphi1 > 360.0/((Double_t)Nphi1) )
      dphi1 = 360.0/((Double_t)Nphi1);

    for (Int_t j=0; j<Ntheta1; j++)
    {
      theta1[j] = theta0 + ((Double_t)j)*dtheta1 
                        - ((Double_t)(Ntheta1/2)-1.0)*dtheta1;
    }

    for (Int_t k=0; k<Nphi1; k++)
    {
      phi1[k] = phi0 + ((Double_t)k)*dphi1 - ((Double_t)(Nphi1/2)-1.0)*dphi1;
    }


    Double_t x1[Ntheta1];
    Double_t y1[Ntheta1];
	
    for (Int_t k=0; k<Nphi1; k++)
    {
      Int_t count = 0;
      for (Int_t j=0; j<Ntheta1; j++)
      {
        if (theta1[j] < 0.0) continue;
                 
        Loc0LocToCam(theta0, phi0, theta1[j], phi1[k],
                           xx, yy);
        x1[count] = xx * mmtodeg * aberr;
        y1[count] = yy * mmtodeg * aberr;

        //gLog << "theta0, phi0 = " << theta0 << ",  " << phi0 
        //     << " : theta1, phi1, x1, y1 = " << theta1[j] << ",  "
        //     << phi1[k] << ";   " << x1[count] << ",  " << y1[count] << endl;  
        count++;
      }

      c1->cd(1);
      graph1 = new TGraph(count,x1,y1);
      graph1->SetLineColor(k+1);
      graph1->SetLineStyle(2);
      graph1->SetMarkerColor(k+1);
      graph1->SetMarkerSize(.2);
      graph1->SetMarkerStyle(20);
      graph1->Draw("PL");
      //delete graph1;

      sprintf(tit,"Phi = %6.2f", phi1[k]);
      Double_t xtxt = xlo + (xup-xlo)*0.75;
      Double_t ytxt = ylo + (yup-ylo)*0.80 - ((Double_t)k) *(yup-ylo)/20.0;
      pix = new TLatex(xtxt, ytxt, tit);
      pix->SetTextColor(k+1);
      pix->SetTextSize(.03);
      pix->Draw("");
      //delete pix;

    }

    c1->cd(1);
    sprintf(tit,"Theta0 = %6.2f    Phi0 = %6.2f", theta0, phi0);
    xtxt = xlo + (xup-xlo)*0.05;
    ytxt = ylo + (yup-ylo)*0.92;
    pix = new TLatex(xtxt, ytxt, tit);
    pix->SetTextColor(1);
    pix->SetTextSize(.06);
    pix->Draw("");
    //delete pix;

    sprintf(tit,"                 [deg]                [deg]");
    xtxt = xlo + (xup-xlo)*0.05;
    ytxt = ylo + (yup-ylo)*0.86;
    pix = new TLatex(xtxt, ytxt, tit);
    pix->SetTextColor(1);
    pix->SetTextSize(.06);
    pix->Draw("");
    //delete pix;


    //--------------------------------------------------------------------
    // Dec-Hour lines 

    // direction for the center of the camera
    Double_t dec0 = dec0deg;
    Double_t h0   = h0hour;
    //trans.LocToCel(theta0, phi0, dec0, h0);
    

    //-----   lines for fixed dec   ------------------------------------

    const Int_t Ndec = numgridlines;
    Double_t dec[Ndec];
    Double_t ddec = gridbinning;

    Int_t nh = (Int_t)(4.0/gridfinebin);
    const Int_t Nh   = nh+1;
    Double_t h[Nh];
    Double_t dh = gridfinebin/cos(dec0/180.0*3.1415926);
    if ( dh > 360.0/((Double_t)(Nh-1)) )
      dh = 360.0/((Double_t)(Nh-1));


    for (Int_t j=0; j<Ndec; j++)
    {
      dec[j] = dec0 + ((Double_t)j)*ddec 
                        - ((Double_t)(Ndec/2)-1.0)*ddec;
    }

    for (Int_t k=0; k<Nh; k++)
    {
      h[k] = h0 + ((Double_t)k)*dh - ((Double_t)(Nh/2)-1.0)*dh;
    }


    Double_t xh[Nh];
    Double_t yh[Nh];
	
    

    for (Int_t j=0; j<Ndec; j++)
    {
      if (fabs(dec[j]) > 90.0) continue;

      for (Int_t k=0; k<Nh; k++)
      {
        Double_t hh0 = h0   *24.0/360.0;                 
        Double_t hx = h[k]*24.0/360.0;
        Cel0CelToCam(dec0, hh0, dec[j], hx,
                           xx, yy);
        xh[k] = xx * mmtodeg * aberr;
        yh[k] = yy * mmtodeg * aberr;

        //gLog << "dec0, h0 = " << dec0 << ",  " << h0 
        //     << " : dec, h, xh, yh = " << dec[j] << ",  "
        //     << h[k] << ";   " << xh[k] << ",  " << yh[k] << endl;  
      }

      c1->cd(2);
      graph1 = new TGraph(Nh,xh,yh);
      graph1->SetLineColor(j+1);
      graph1->SetLineStyle(1);
      graph1->SetMarkerColor(j+1);
      graph1->SetMarkerSize(.2);
      graph1->SetMarkerStyle(20);
      graph1->Draw("PL");
      //delete graph1;

      sprintf(tit,"Dec = %6.2f", dec[j]);
      xtxt = xlo + (xup-xlo)*0.1;
      ytxt = ylo + (yup-ylo)*0.80 - ((Double_t)j) *(yup-ylo)/20.0;
      pix = new TLatex(xtxt, ytxt, tit);
      pix->SetTextColor(j+1);
      pix->SetTextSize(.03);
      pix->Draw("");
      //delete pix;

    }

    //-----   lines for fixed hour angle   ------------------------------------

    Int_t ndec1 = (Int_t)(4.0/gridfinebin);
    const Int_t Ndec1 = ndec1;
    Double_t dec1[Ndec1];
    Double_t ddec1 = gridfinebin;

    const Int_t Nh1   = numgridlines;
    Double_t h1[Nh1];
    Double_t dh1 = gridbinning/cos(dec0/180.0*3.1415926);
    if ( dh1 > 360.0/((Double_t)Nh1) )
      dh1 = 360.0/((Double_t)Nh1);


    for (Int_t j=0; j<Ndec1; j++)
    {
      dec1[j] = dec0 + ((Double_t)j)*ddec1 
                        - ((Double_t)(Ndec1/2)-1.0)*ddec1;
    }

    for (Int_t k=0; k<Nh1; k++)
    {
      h1[k] = h0 + ((Double_t)k)*dh1 - ((Double_t)(Nh1/2)-1.0)*dh1;
    }


    Double_t xd[Ndec1];
    Double_t yd[Ndec1];
	
    for (Int_t k=0; k<Nh1; k++)
    {
      Int_t count = 0;
      for (Int_t j=0; j<Ndec1; j++)
      {
        if (fabs(dec1[j]) > 90.0) continue;

        Double_t hh0 = h0   *24.0/360.0;                 
        Double_t hhx = h1[k]*24.0/360.0;   
        Cel0CelToCam(dec0, hh0, dec1[j], hhx,
                           xx, yy);
        xd[count] = xx * mmtodeg * aberr;
        yd[count] = yy * mmtodeg * aberr;

        //gLog << "dec0, h0 = " << dec0 << ",  " << h0 
        //     << " : dec1, h1, xd, yd = " << dec1[j] << ",  "
        //     << h1[k] << ";   " << xd[count] << ",  " << yd[count] << endl;  

        count++;
      }

      c1->cd(2);
      graph1 = new TGraph(count,xd,yd);
      graph1->SetLineColor(k+1);
      graph1->SetLineStyle(2);
      graph1->SetMarkerColor(k+1);
      graph1->SetMarkerSize(.2);
      graph1->SetMarkerStyle(20);
      graph1->Draw("PL");
      //delete graph1;

      sprintf(tit,"H = %6.2f", h1[k]);
      Double_t xtxt = xlo + (xup-xlo)*0.75;
      Double_t ytxt = ylo + (yup-ylo)*0.80 - ((Double_t)k) *(yup-ylo)/20.0;
      pix = new TLatex(xtxt, ytxt, tit);
      pix->SetTextColor(k+1);
      pix->SetTextSize(.03);
      pix->Draw("");
      //delete pix;

    }

    c1->cd(2);
    sprintf(tit,"Dec0 = %6.2f    H0 = %6.2f", dec0, h0);
    xtxt = xlo + (xup-xlo)*0.05;
    ytxt = ylo + (yup-ylo)*0.92;
    pix = new TLatex(xtxt, ytxt, tit);
    pix->SetTextColor(1);
    pix->SetTextSize(.06);
    pix->Draw("");
    //delete pix;

    sprintf(tit,"              [deg]             [deg]");
    xtxt = xlo + (xup-xlo)*0.05;
    ytxt = ylo + (yup-ylo)*0.86;
    pix = new TLatex(xtxt, ytxt, tit);
    pix->SetTextColor(1);
    pix->SetTextSize(.06);
    pix->Draw("");
    //delete pix;
 
    return kTRUE;
}

