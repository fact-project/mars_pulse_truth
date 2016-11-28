
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
!   Author(s): Wolfgang Wittek 02/2004 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MSourcePosfromStarPos
//
//  This is a task which
//  - calculates the position of the source in the camera
//    from the position of a known star in the camera
//  - and puts the source position into the container MSrcPosCam
//
//  Input :
//   ASCII file containing for each run 
//   - the run number
//   - the direction (theta, phi) the telescope is pointing to in [deg]
//   - the position  (xStar, yStar)   of a known star in the camera in [mm]
//   - the error     (dxStar, dyStar) of this position in [mm]
//
//  Output Containers :
//   MSrcPosCam
//
/////////////////////////////////////////////////////////////////////////////
#include <TList.h>
#include <TSystem.h>
#include <TMatrixD.h>

#include <fstream>

#include "MSourcePosfromStarPos.h"

#include "MParList.h"
#include "MRawRunHeader.h"
#include "MGeomCam.h"
#include "MSrcPosCam.h"
#include "MPointingPos.h"
#include "MMcEvt.hxx"

#include "MLog.h"
#include "MLogManip.h"
#include "MObservatory.h"

ClassImp(MSourcePosfromStarPos);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MSourcePosfromStarPos::MSourcePosfromStarPos(
                                         const char *name, const char *title)
  : fIn(NULL)
{
    fName  = name  ? name  : "MSourcePosfromStarPos";
    fTitle = title ? title : "Calculate source position from star position";

    fFileNames = new TList;
    fFileNames->SetOwner();

    fRuns  = 0;
    fSize  = 100;

    fRunNr.Set(fSize);

    fThetaTel.Set(fSize);
    fPhiTel.Set(fSize);
    fdThetaTel.Set(fSize);
    fdPhiTel.Set(fSize);

    Int_t fRows = 1;
    fDecStar.Set(fRows);
    fRaStar.Set(fRows);
    fxStar.ResizeTo(fRows,fSize);
    fyStar.ResizeTo(fRows,fSize);
    fdxStar.ResizeTo(fRows,fSize);
    fdyStar.ResizeTo(fRows,fSize);

    fStars     = 0;
    fStarsRead = 0;
    fDecSource = 0.0;
    fRaSource  = 0.0;

    // these are the default values when starting the excution;
    // later these locations contain the values of the last event
    fxSourceold  =  25.0;
    fySourceold  = -40.0;
    fThetaradold =  25.0/kRad2Deg;
    fPhiradold   = 180.0/kRad2Deg;
}

// --------------------------------------------------------------------------
//
// Delete the filename list and the input stream if one exists.
//
MSourcePosfromStarPos::~MSourcePosfromStarPos()
{
    delete fFileNames;
    if (fIn)
        delete fIn;
}


// --------------------------------------------------------------------------
//
// Set the sky coordinates of the source and of the star
//
// Input :
// declination in units of     (Deg,  Min, Sec)
// right ascension in units of (Hour, Min, Sec)
//

void MSourcePosfromStarPos::SetSourceAndStarPosition(
	 TString  nameSource,
	 Double_t decSourceDeg, Double_t decSourceMin, Double_t decSourceSec, 
         Double_t raSourceHour, Double_t raSourceMin,  Double_t raSourceSec,
	 TString nameStar,
	 Double_t decStarDeg,   Double_t decStarMin,   Double_t decStarSec, 
         Double_t raStarHour,   Double_t raStarMin,    Double_t raStarSec  )
{
  *fLog << "MSourcePosfromStarPos::SetSourceAndStarPosition :" << endl;
  *fLog << "       Source (dec) : "  << nameSource << "   " << decSourceDeg << ":" 
        << decSourceMin << ":" << decSourceSec << endl;
  *fLog << "       Source (ra)  : "  << nameSource  << "   " << raSourceHour << ":" 
        << raSourceMin  << ":" << raSourceSec << endl;

  *fLog << "       Star  (dec) : "  << nameStar   << "   " << decStarDeg << ":" 
        << decStarMin << ":"   << decStarSec << endl;
  *fLog << "       Star  (ra)  : "  << nameStar   << "   " << raStarHour << ":" 
        << raStarMin << ":"    << raStarSec  << endl;

  // convert into radians
  fDecSource = (decSourceDeg + decSourceMin/60.0 + decSourceSec/3600.0)
               / kRad2Deg;
  fRaSource  = (raSourceHour + raSourceMin/60.0  + raSourceSec/3600.0)
               * 360.0 / (24.0 * kRad2Deg);

  fStars += 1;
  fDecStar.Set(fStars);
  fRaStar.Set(fStars);
  fxStar.ResizeTo(fStars,fSize);
  fyStar.ResizeTo(fStars,fSize);
  fdxStar.ResizeTo(fStars,fSize);
  fdyStar.ResizeTo(fStars,fSize);

  fDecStar[fStars-1] = (decStarDeg + decStarMin/60.0 + decStarSec/3600.0)
                       / kRad2Deg;
  fRaStar[fStars-1]  = (raStarHour + raStarMin/60.0  + raStarSec/3600.0)
                       * 360.0 / (24.0 * kRad2Deg);

  *fLog << all << "MSourcePosfromStarPos::SetSourceAndStarPosition; fStars = "
        << fStars << endl;
  *fLog << all << "       fDecSource, fRaSource, fDecStar, fRaStar were set to : [radians]  "
        << fDecSource << ",  " << fRaSource << ",  "
        << fDecStar[fStars-1] << ",  " << fRaStar[fStars-1] << endl;
}

// --------------------------------------------------------------------------
//
// Set the sky coordinates of another star
//
// Input :
// declination in units of     (Deg,  Min, Sec)
// right ascension in units of (Hour, Min, Sec)
//

void MSourcePosfromStarPos::AddStar(
	 TString nameStar,
	 Double_t decStarDeg,   Double_t decStarMin,   Double_t decStarSec, 
         Double_t raStarHour,   Double_t raStarMin,    Double_t raStarSec  )
{
  *fLog << "MSourcePosfromStarPos::AddStar :" << endl;
  *fLog << "       Star (dec)  : "  << nameStar   << "   " << decStarDeg << ":" 
        << decStarMin << ":"   << decStarSec << endl;
  *fLog << "       Star (ra)   : "  << nameStar   << "   " << raStarHour << ":" 
        << raStarMin << ":"    << raStarSec  << endl;

  // convert into radians
  fStars += 1;
  fDecStar.Set(fStars);
  fRaStar.Set(fStars);
  fxStar.ResizeTo(fStars,fSize);
  fyStar.ResizeTo(fStars,fSize);
  fdxStar.ResizeTo(fStars,fSize);
  fdyStar.ResizeTo(fStars,fSize);

  fDecStar[fStars-1] = (decStarDeg + decStarMin/60.0 + decStarSec/3600.0)
                       / kRad2Deg;
  fRaStar[fStars-1]  = (raStarHour + raStarMin/60.0  + raStarSec/3600.0)
                       * 360.0 / (24.0 * kRad2Deg);

  *fLog << all << "MSourcePosfromStarPos::AddStar; fStars = " << fStars 
        << endl;
  *fLog << all << "       fDecStar, fRaStar were set to : [radians]  "
        << fDecStar[fStars-1] << ",  " << fRaStar[fStars-1] << endl;
}

// --------------------------------------------------------------------------
//
//
Int_t MSourcePosfromStarPos::PreProcess(MParList *pList)
{
    MGeomCam *geom = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
    if (!geom)
    {
        *fLog << err << "MSourcePosfromStarPos : MGeomCam (Camera Geometry) missing in Parameter List... aborting." << endl;
        return kFALSE;
    }
    fMm2Deg = geom->GetConvMm2Deg();
    // fDistCameraReflector is the distance of the camera from the reflector center in [mm]
    fDistCameraReflector = kRad2Deg / fMm2Deg;   
        *fLog << all << "MSourcePosfromStarPos::PreProcess; fMm2Deg, fDistCameraReflector = " 
              << fMm2Deg << ",  " << fDistCameraReflector << endl;

    fObservatory = (MObservatory*)pList->FindObject(AddSerialNumber("MObservatory"));
    if (!fObservatory)
    {
        *fLog << err << "MObservatory not found...  aborting" << endl;
        return kFALSE;
    }

    fRun = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRun)
    {
        *fLog << err << "MSourcePosfromStarPos::PreProcess; MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }



   fPointPos = (MPointingPos*)pList->FindCreateObj("MPointingPos");
   if (!fPointPos)
   {
       *fLog << err << "MSourcePosfromStarPos::PreProcess; MPointingPos not found... aborting." << endl;
       return kFALSE;
   }

   fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
   if (!fMcEvt)
   {
       *fLog << all << "MSourcePosfromStarPos::PreProcess; MMcEvt not found... continue." << endl;
   }
   else
   {
       *fLog << all << "MSourcePosfromStarPos::PreProcess; MMcEvt was found... continue." << endl;
   }


    fSrcPos = (MSrcPosCam*)pList->FindCreateObj("MSrcPosCam");
    if (!fSrcPos)
    {
        *fLog << err << "MSourcePosfromStarPos::PreProcess; MSrcPosCam not found...  aborting" << endl;
        return kFALSE;
    }

    //---------------------------------------------------------------------
    // read all files and call ReadData() to read and store the information
    //

    *fLog << all << "---------------------------------" << endl;
    while(1)
    {
      if (!OpenNextFile()) 
      {
        *fLog << "there is no more file to open" << endl;
        break;
      }

      *fLog << "read data" << endl;

      // read "fStarsRead" = no.of (x,y) pairs to be read
      *fIn >> fStarsRead;

      while (1)
      {
        if (fIn->eof())
        {
          *fLog << "eof encountered; open next file" << endl;

          if (!OpenNextFile()) break;
        }

        // FIXME! Set InputStreamID
      
        ReadData();
      }
    }

    *fLog << "all data were read" << endl;
    FixSize();

    if (fDecSource == 0.0  ||  fRaSource == 0.0  ||  fStars == 0) 
    {
      *fLog << warn << "MSourcePosfromStarPos::PreProcess; there are no sky coordinates defined for the source or from stars; fStars, fStarsRead = " 
            << fStars << ",  " << fStarsRead 
            << endl;
    }
    *fLog << all << "---------------------------------" << endl;

    //-------------------------------------------------------------

    return kTRUE;
}

//=========================================================================
//
// SourcefromStar
//
// this routine calculates the position of a source (for example Crab) in the camera
// from the position of a star (for example ZetaTauri) in the camera. The latter 
// position may have been determined by analysing the DC currents in the different
// pixels.
//
// Input  : thetaTel, phiTel          the direction the telescope is pointing to,
//                                    in local coordinates
//          f                         the distance between camera and reflector
//          decStar, raStar           the position of the star in sky coordinates
//          decSource, raSource       the position of the source in sky coordinates
//          xStar, yStar              the position of the star in the camera
//          dxStar, dyStar            error of the position of the star in the camera
//
// Output : xSource, ySource       the calculated position of the source in the camera
//          dxSource, dySource     error of the calculated position of the source in 
//                                 the camera
//
// Useful formulas can be found in TDAS 00-11 and TDAS 01-05
//

void MSourcePosfromStarPos::SourcefromStar(Double_t &f,
		    TArrayD  &decStar,     TArrayD  &raStar,
		    Double_t &decSource,   Double_t &raSource,
                    Double_t &thetaTel,    Double_t &phiTel,    
		    TArrayD  &xStar,       TArrayD  &yStar,
		    TArrayD  &dxStar,      TArrayD  &dyStar,
		    Double_t &xSource,     Double_t &ySource,
		    Double_t &dxSource,    Double_t &dySource)
{
  /*
  *fLog << "MSourcePosfromStarPos::SourcefromStar :  printout in degrees" << endl;
  *fLog << "       decStar, raStar = " << decStar[0]*kRad2Deg << ",  " 
        << raStar[0]*kRad2Deg << endl;
  *fLog << "       decSource, raSource = " << decSource*kRad2Deg << ",  " 
        << raSource*kRad2Deg << endl;
  *fLog << "       thetaTel, phiTel = " << thetaTel*kRad2Deg << ",  " 
        << phiTel*kRad2Deg << endl;
  *fLog << "       xStar, yStar = " << xStar[0]*fMm2Deg << ",  " 
        << yStar[0]*fMm2Deg << endl;

  *fLog << "MSourcePosfromStarPos::SourcefromStar :  printout in radians and mm" << endl;
  *fLog << "       decStar, raStar = " << decStar[0] << ",  " 
        << raStar[0] << endl;
  *fLog << "       decSource, raSource = " << decSource << ",  " 
        << raSource << endl;
  *fLog << "       thetaTel, phiTel = " << thetaTel << ",  " 
        << phiTel << endl;
  *fLog << "       xStar, yStar = " << xStar[0] << ",  " 
        << yStar[0] << endl;
  */

  // the units are assumed to be radians for theta, phi, dec and ra
  //            and                   mm for f, x and y


  // calculate rotation angle alpha of sky image in camera 
  // (see TDAS 00-11, eqs. (18) and (20))
  // a1 = cos(Lat), a3 = -sin(Lat), where Lat is the geographical latitude of La Palma
  Double_t a1 =  0.876627;
  Double_t a3 = -0.481171;

  Double_t denom =  1./ sqrt( sin(thetaTel)*sin(phiTel) * sin(thetaTel)*sin(phiTel) +
			      ( a1*cos(thetaTel)+a3*sin(thetaTel)*cos(phiTel) ) * 
                              ( a1*cos(thetaTel)+a3*sin(thetaTel)*cos(phiTel) )   );
  Double_t cosal = - (a3 * sin(thetaTel) + a1 * cos(thetaTel) * cos(phiTel)) * denom;
  Double_t sinal =    a1 * sin(phiTel) * denom;

  *fLog << "old thetaTel, phiTel, cosal, sinal = " << thetaTel << ",  "
        << phiTel << ",  " << cosal << ",  " << sinal << endl;


  fObservatory->RotationAngle(thetaTel, phiTel, sinal, cosal);

  *fLog << "new thetaTel, phiTel, cosal, sinal = " << thetaTel << ",  "
        << phiTel << ",  " << cosal << ",  " << sinal << endl;


  // calculate coordinates of source in system B (see TDAS 00-11, eqs. (2))
  // note that right ascension and hour angle go into opposite directions
  Double_t xB0 =  cos(decSource) * cos(-raSource);
  Double_t yB0 =  cos(decSource) * sin(-raSource);
  Double_t zB0 = -sin(decSource);

  //*fLog << "xB0, yB0, zB0 = " << xB0 << ",  " << yB0 << ",  "
  //      << zB0 << endl;

  //-----------------------------------------------------
  // loop over stars
  Double_t sumx  = 0.0;
  Double_t sumy  = 0.0;
  Double_t sumwx = 0.0;
  Double_t sumwy = 0.0;

  for (Int_t i=0; i<decStar.GetSize(); i++)
  {
    // calculate weights
    Double_t weightx = 1.0 / (dxStar[i]*dxStar[i]);
    Double_t weighty = 1.0 / (dyStar[i]*dyStar[i]);
    sumwx += weightx;
    sumwy += weighty;

    //*fLog << "weightx, weighty = " << weightx << ",  " << weighty << endl;

    // calculate coordinates of star in system B (see TDAS 00-11, eqs. (2))
    // note that right ascension and hour angle go into opposite directions
    Double_t xB  =  cos(decStar[i]) * cos(-raStar[i]);
    Double_t yB  =  cos(decStar[i]) * sin(-raStar[i]);
    Double_t zB  = -sin(decStar[i]);


    //*fLog << "xB, yB, zB = " << xB << ",  " << yB << ",  "
    //    << zB << endl;

 
    // calculate coordinates of star in a system with the basis vectors e1, e2, e3
    // where  e1 is in the direction (r0 x a)
    //        e2 is in the direction (e1 x r0)
    //   and  e3 is in the direction -r0;
    // r0 is the direction to the source
    // and a is the earth rotation axis (pointing to the celestial north pole)
    // 
    Double_t x = (-xB*yB0 + xB0*yB) / sqrt( xB0*xB0 + yB0*yB0 );
    Double_t y = ( xB*xB0*zB0 + yB*yB0*zB0 - zB*(xB0*xB0 + yB0*yB0) ) 
                                    / sqrt( xB0*xB0 + yB0*yB0 );
    Double_t z = -(xB*xB0 + yB*yB0 + zB*zB0);

    //*fLog << "x, y, z = " << x << ",  " << y << ",  "
    //    << z << endl;


    // calculate coordinates of star in camera
    Double_t xtilde = -f/z * (cosal*x - sinal*y);
    Double_t ytilde = -f/z * (sinal*x + cosal*y);

    //*fLog << "i, xtilde, ytile = " << i << " : " << xtilde << ",  " 
    //      << ytilde << endl;


    // calculate coordinates of source in camera
    // note : in real camera signs are inverted (therefore s = -1.0)
    Double_t s = -1.0;

    Double_t xs = xStar[i] - s * xtilde;
    Double_t ys = yStar[i] - s * ytilde;

    *fLog << "i, xs, ys = " << i << " : " << xs << ",  " 
          << ys << endl;

    sumx += xs * weightx;
    sumy += ys * weighty;
  }
  //-----------------------------------------------------

  xSource  = sumx / sumwx;
  ySource  = sumy / sumwy;
  dxSource = 1.0 / sqrt(sumwx);
  dySource = 1.0 / sqrt(sumwy);
    
  /*
  Int_t run = fRun->GetRunNumber();
  *fLog << all << "MSourcePosfromStarPos::SourcefromStar; run, xSource, ySource = "
        << run << " : "
        << xSource << " +- " << dxSource << ",   " 
        << ySource << " +- " << dySource << endl; 
   */
}

// --------------------------------------------------------------------------
//
// Get the source position and put it into MSrcPosCam
//
//
Bool_t MSourcePosfromStarPos::ReInit(MParList *pList)
{
  Int_t run = fRun->GetRunNumber();
  *fLog << all << "MSourcePosfromStarPos::ReInit; run = " << run << endl;

  // temporary solution for MC
  // copy (theta, phi) from MMcEvt into MPointingPos
  if (fRun->GetRunType() > 255.5)
  {
    *fLog << all << "                     these are MC data" << endl;
    Double_t thetarad = fMcEvt->GetTelescopeTheta();
    Double_t phirad   = fMcEvt->GetTelescopePhi();
    fPointPos->SetLocalPosition(thetarad*kRad2Deg, phirad*kRad2Deg);
    fPointPos->SetReadyToSave();
    return kTRUE;    
  }

  //-------------------------------------------------------------------
  // search this run in the list 
  for (Int_t i=0; i<fSize; i++)
  {
    if (run == fRunNr[i])
    {
      //-----------------------------------------
      // put the zenith angle into MPointingPos

      Double_t thetarad = fThetaTel[i];
      Double_t phirad   = fPhiTel[i];

      if (fabs(thetarad*kRad2Deg+1.0) < 0.001)
        thetarad = fThetaradold;
      else
        fThetaradold = thetarad;
      if (fabs(phirad*kRad2Deg+1.0) < 0.001)
        phirad = fPhiradold;
      else
        fPhiradold = phirad;

      fPointPos->SetLocalPosition(thetarad*kRad2Deg, phirad*kRad2Deg);
      fPointPos->SetReadyToSave();

      *fLog << all << "theta, phi = " << thetarad*kRad2Deg << ",  "
            << phirad*kRad2Deg << " deg" << endl;
       
      //-----------------------------------------
      // Get source position and put it into MSrcPosCam

      
      if (fStars > 0  && fStars == fStarsRead)
      {
        TArrayD xStar(fxStar.GetNrows());
        TArrayD dxStar(fdxStar.GetNrows());
        TArrayD yStar(fyStar.GetNrows());
        TArrayD dyStar(fdyStar.GetNrows());
        for (Int_t j=0; j<fxStar.GetNrows(); j++)
        {
          xStar[j]  = fxStar(j, i);
          dxStar[j] = fdxStar(j, i);
          yStar[j]  = fyStar(j, i);
          dyStar[j] = fdyStar(j, i);
        }

        SourcefromStar( fDistCameraReflector,
                        fDecStar, fRaStar, fDecSource, fRaSource,
                        thetarad, phirad,   
         	        xStar,          yStar,
		        dxStar,         dyStar,
		        fxSource,       fySource,
		        fdxSource,      fdySource);
      
        fSrcPos->SetXY(fxSource, fySource);

        fxSourceold = fxSource;
        fySourceold = fySource;

        *fLog << all << "MSourcePosfromStarPos::ReInit; fRunNr, fxSource, fySource = "
              << fRunNr[i] << ",  " << fxSource << " +- " << fdxSource 
              << ",  " << fySource  << " +- "  << fdySource << endl;
       
        fSrcPos->SetReadyToSave();       
      }
      else
      {
        // set default values
        fxSource = fxSourceold;
        fySource = fySourceold;
        fSrcPos->SetXY(fxSource, fySource);
        fSrcPos->SetReadyToSave();       


        *fLog << warn << "MSourcePosfromStarPos::ReInit;  no information on source position for run number = "
              << run << endl;
        *fLog << warn << "       set xSource, ySource = " << fxSource << ",  "
              << fySource << " mm" << endl;
      }
      

      return kTRUE;
    }
  }
  //-------------------------------------------------------------------  

    // set default values
    fxSource = fxSourceold;
    fySource = fySourceold;
    fSrcPos->SetXY(fxSource, fySource);
    fSrcPos->SetReadyToSave();       

    Double_t thetarad = fThetaradold;
    Double_t phirad = fPhiradold;
    fPointPos->SetLocalPosition(thetarad*kRad2Deg, phirad*kRad2Deg);
    fPointPos->SetReadyToSave();


    *fLog << warn << "MSourcePosfromStarPos::ReInit;  no information on theta, phi and source position for run number = "
          << run << endl;
    *fLog << warn << "       set xSource, ySource = " << fxSource << ",  "
          << fySource << " mm" << endl;
    *fLog << warn << "       set theta, phi = " << thetarad*kRad2Deg << ",  "
          << phirad*kRad2Deg << " deg" << endl;


    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
Int_t MSourcePosfromStarPos::Process()
{
  //Int_t run = fRun->GetRunNumber();
  //*fLog << "MSourcePosfromStarPos::Process; run = " << run << endl;
    

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//
Int_t MSourcePosfromStarPos::PostProcess()
{

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// read the data from the ASCII file and store them
//
void MSourcePosfromStarPos::FixSize()
{
    fSize = fRuns;
    if (fRuns <= 0)
      fSize = 1;

    fRunNr.Set(fSize);

    fThetaTel.Set(fSize);
    fPhiTel.Set(fSize);
    fdThetaTel.Set(fSize);
    fdPhiTel.Set(fSize);

    Int_t fRows = fxStar.GetNrows();
    fxStar.ResizeTo(fRows, fSize);
    fyStar.ResizeTo(fRows, fSize);
    fdxStar.ResizeTo(fRows, fSize);
    fdyStar.ResizeTo(fRows, fSize);

  *fLog << "MSourcePosfromStarPos::FixSize; fix size of arrays : fStars = "
        << fStars << ",  fRuns = " << fRuns << ",  fRows = " << fRows
        << ",  fSize = " << fSize << endl;
  if (fRuns > 0)
  {
    *fLog << "       first run : " << fRunNr[0] << ",  last run : "
          << fRunNr[fRuns-1] << endl;
  }
}

// --------------------------------------------------------------------------
//
// read the data from the ASCII file and store them
//
void MSourcePosfromStarPos::ReadData()
{
  Float_t val;
  Int_t   ival;

  // extend size of arrays if necessary
  if ( fRuns >= fSize )
  {
    fSize += 100;

    fRunNr.Set(fSize);

    fThetaTel.Set(fSize);
    fPhiTel.Set(fSize);
    fdThetaTel.Set(fSize);
    fdPhiTel.Set(fSize);
    
    Int_t fRows = fxStar.GetNrows();
    fxStar.ResizeTo(fRows, fSize);
    fyStar.ResizeTo(fRows, fSize);
    fdxStar.ResizeTo(fRows, fSize);
    fdyStar.ResizeTo(fRows, fSize);

    *fLog << "MSourcePosfromStarPos::ReadData(); size of arrays has been increased to (fRows, fSize) = ("
          << fRows << ",  " << fSize << ")" << endl;
  }

  //-------------------
  // read header line
  //*fIn >> val;

  //*fLog << "val =" << val << endl;

  //*fIn >> val;
  //*fIn >> val;
  //*fIn >> val;
  //*fIn >> val;
  //*fIn >> val;
  //*fIn >> val;
  //*fIn >> val;
  //*fIn >> val;
  //*fIn >> val;
  //*fIn >> val;
  //-------------------



  while(1)
  {
    *fIn >> ival;

    if (fIn->eof())
      return;

    // run number must be greater than 10000
    if (TMath::Abs(ival) < 10000)
    {
      *fLog << err << "===========> Error when reading file with theta and phi <========="
	    << "             ival = " << ival << endl;
    }          
    else
      break;
  }  

  fRuns += 1;
  //*fLog << fRuns <<"th run : " << ival << endl;  

  fRunNr.AddAt(ival, fRuns-1);

  //*fLog << "check : fRuns, fRunNr[fRuns-1], fRunNr[fRuns] = " << fRuns << ",  "
  //      << fRunNr[fRuns-1] << ",  " << fRunNr[fRuns] << endl;

 
  // read mjdS, hmsS, mjdE, hmsE
  // these data are present only for ON data (fStars > 0)
  /*
  if (fStars > 0)
  {
    *fIn >> val; 
    *fIn >> val; 
    *fIn >> val; 
    *fIn >> val; 

    *fIn >> val; 
    *fIn >> val; 
    *fIn >> val; 
    *fIn >> val; 
  }
  */

  *fIn >> val;
  fThetaTel.AddAt(val/kRad2Deg, fRuns-1);
  //*fLog << "val, fThetaTel[fRuns-1] = " << val << ",  "
  //      << fThetaTel[fRuns-1] << endl;


  *fIn >> val;
  fPhiTel.AddAt(val/kRad2Deg, fRuns-1);
  //*fLog << "val, fPhiTel[fRuns-1] = " << val << ",  "
  //      << fPhiTel[fRuns-1] << endl;


  //*fIn >> val;
  //fdThetaTel.AddAt(val/kRad2Deg, fRuns-1);
  //*fIn >> val;
  //fdPhiTel.AddAt(val/kRad2Deg, fRuns-1);

  // input is in [deg], convert to [mm]

  //*fLog << "ReadData : fStarsRead = " << fStarsRead << endl;

    for (Int_t i=0; i<fStarsRead; i++)
    {
      *fIn >> val;
      if (i<fStars) fxStar(i, fRuns-1) = val;

      *fIn >> val;
      if (i<fStars) fyStar(i, fRuns-1) = val;

      //*fIn >> val;
      // if (i < fStars) fdxStar(i, fRuns-1) = val;
      if (i < fStars) fdxStar(i, fRuns-1) = 1.0;

      //*fIn >> val;
      // if (i < fStars) fdyStar(i, fRuns-1) = val;
      if (i < fStars) fdyStar(i, fRuns-1) = 1.0;
    }
}

// --------------------------------------------------------------------------
//
// Add this file as the last entry in the chain
//
Int_t MSourcePosfromStarPos::AddFile(const char *txt, Int_t)
{
    TNamed *name = new TNamed(txt, "");
    fFileNames->AddLast(name);

    *fLog << "MSourcePosfromStarPos::AddFile; add file '" << txt << "'"
          << endl;

    return 1;
}

// --------------------------------------------------------------------------
//
// This opens the next file in the list and deletes its name from the list.
//
Bool_t MSourcePosfromStarPos::OpenNextFile()
{
    //
    // open the input stream and check if it is really open (file exists?)
    //
    if (fIn)
        delete fIn;
    fIn = NULL;

    //
    // Check for the existence of a next file to read
    //
    TNamed *file = (TNamed*)fFileNames->First();
    if (!file)
        return kFALSE;

    //
    // open the file which is the first one in the chain
    //
    const char *name = file->GetName();

    const char *expname = gSystem->ExpandPathName(name);
    fIn = new ifstream(expname);
    delete [] expname;

    const Bool_t noexist = !(*fIn);


    if (noexist)
        *fLog << dbginf << "Cannot open file '" << name << "'" << endl;
    else
        *fLog << "Open file: '" << name << "'" << endl;

    //
    // Remove this file from the list of pending files
    //
    fFileNames->Remove(file);

    return !noexist;
}

// --------------------------------------------------------------------------














