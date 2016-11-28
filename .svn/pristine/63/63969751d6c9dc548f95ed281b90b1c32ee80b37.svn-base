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
!   Author(s): Thomas Bretz  12/2000 (tbretz@uni-sw.gwdg.de)
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MMcRunHeader
//
// Root storage container for the RUN MONTE CARLO HEADER information
//
// This the second version of this output class. Old root files, which have
// a previous version of this class, are still compatibles and can be used.
// But of course, you can no try to get infromatino in these old files about
// the new data members.
//
// The following data member have been introduced in this second version 
// and they do not exist in the previous one:
//
//  Float_t fMcRunNumber;      Run Number
//  UInt_t  fProductionSite;   code to know where the run was generated
//  Float_t fDateRunMMCs;      Date of the MMCs production
//  Float_t fDateRunCamera;    Date, when the Camera program is run.
//  Byte_t  fRawEvt;           RawEvt[Data,Hedaer] stored or not
//  Byte_t  fElecNoise;        Electronic Noise simulated or not
//  Byte_t  fStarFieldRotate;  Is the starfield rotation switched on (1) or
//                             off (0)
//  Float_t fCWaveLower;       Wavelength limits for the Cerenkov photons
//  Float_t fCWaveUpper;
//  UInt_t  fNumObsLev;        Observation levels
//  Float_t fHeightLev[10];
//  Float_t fSlopeSpec;        Spectral index
//
//  Third version:
//
//  Byte_t fOpticLinksNoise;   Flag to state if the optic noise is simualted
//                             or not
//
//
// Note: All the azimuth Phi angles in this and other MC classes follow
// the convention in the Corsika program (see Corsika manual). There, phi
// is the azimuth of the momentum vector of particles, and is measured
// from the north direction, anticlockwise (i.e, west is phi=90 degrees).
//
//                                                     
// Class Version 5: 
// ----------------
// removed members fSourceOffsetTheta, fSourceOffsetPhi (were no longer used)                                  //
//                   
// Class Version 6: 
// ----------------
// removed members fTelesTheta, fTelesPhi (were no longer used)                                  //
//
// Class Version 7:
// ----------------
// + Float_t fRandomPointingConeSemiAngle;
//
////////////////////////////////////////////////////////////////////////////
#include "MMcRunHeader.hxx"

#include <fstream>
#include <iomanip>

#include "MLog.h"

ClassImp(MMcRunHeader);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
//
MMcRunHeader::MMcRunHeader(const char *name, const char *title)
{
    fName  = name  ? name  : "MMcRunHeader";
    fTitle = title ? title : "Raw Run Header Information";

    fMcRunNumber =0;
    fProductionSite = 0;
    fDateRunMMCs = 0;
    fDateRunCamera = 0;
    fNumTrigCond = 0;
    fAllEvtsTriggered = 0 ;
    fMcEvt = 0;
    fMcTrig = 0;
    fMcFadc = 0;
    fRawEvt = 0;
    fElecNoise = 0;
    fStarFieldRotate = 0;
    fNumAnalisedPixels = 0;
    fNumSimulatedShowers = 0;
    fNumStoredShowers = 0;
    fNumEvents = 0;

    fStarFieldRaH = 0;
    fStarFieldRaM = 0;
    fStarFieldRaS = 0;
    fStarFieldDeD = 0;
    fStarFieldDeM = 0;
    fStarFieldDeS = 0;

    fNumPheFromDNSB = 0.0;
    fShowerThetaMax = 0.0;
    fShowerThetaMin = 0.0;
    fShowerPhiMax = 0.0;
    fShowerPhiMin = 0.0;

    fImpactMax = -1;

    fCWaveLower = 0.0;
    fCWaveUpper = 0.0;

    fNumObsLev = 0;
    for (int i=0; i<10; i++){
      fHeightLev[i]=0.0;
    }
    fSlopeSpec = 0.0;

    fCorsikaVersion = UShort_t(-1);
    fReflVersion    = UShort_t(-1);
    fCamVersion     = UShort_t(-1);

    fOpticLinksNoise= 0;

    fRandomPointingConeSemiAngle=0;
}

// -------------------------------------------------------------------------
//
// Fill. Put data in the container
//
void MMcRunHeader::Fill(const Float_t  runnumber,
			const UInt_t   productionsite,
			const Float_t  daterunMMCs,
			const Float_t   daterunCamera,
			const UInt_t   numtrigcond,
			const Byte_t   allevts,
			const Byte_t   mcevt,
			const Byte_t   mctrig,
			const Byte_t   mcfadc,
			const Byte_t   rawevt,
			const Byte_t   elecnoise, 
			const Int_t    numanalpixels,
			const UInt_t   numsim,
			const UInt_t   numsto,
			const Byte_t   starfieldrotate,
			const Int_t    sfRaH,
			const Int_t    sfRaM,
			const Int_t    sfRaS,
			const Int_t    sfDeD,
			const Int_t    sfDeM,
			const Int_t    sfDeS,
			const Float_t  numdnsb,
			const Float_t  shthetamax,
			const Float_t  shthetamin,
			const Float_t  shphimax,
			const Float_t  shphimin,
			const Float_t  impactmax,
			const Float_t  cwavelower,
			const Float_t  cwaveupper,
			const Float_t  slopespec,
			const UInt_t   numObslev,
			const Float_t  heightlev[10],
			const UInt_t   corsika,
			const UInt_t   refl,
			const UInt_t   cam,
                        const Byte_t   opticnoise,
                        const Float_t  conesmiangle)
{
    fMcRunNumber =runnumber;
    fProductionSite = productionsite;
    fDateRunMMCs = daterunMMCs;
    fDateRunCamera = daterunCamera;
    fNumTrigCond = numtrigcond;
    fAllEvtsTriggered = allevts;
    fMcEvt = mcevt;
    fMcTrig = mctrig;
    fMcFadc = mcfadc;
    fRawEvt = rawevt;
    fElecNoise = elecnoise;
    fStarFieldRotate = starfieldrotate;
    fNumAnalisedPixels = numanalpixels;
    fNumSimulatedShowers = numsim;
    fNumStoredShowers = numsto;
    fNumEvents = numsto;

    fStarFieldRaH = sfRaH;
    fStarFieldRaM = sfRaM;
    fStarFieldRaS = sfRaS;
    fStarFieldDeD = sfDeD;
    fStarFieldDeM = sfDeM;
    fStarFieldDeS = sfDeS;

    fNumPheFromDNSB = numdnsb;
    fShowerThetaMax = shthetamax;
    fShowerThetaMin = shthetamin;
    fShowerPhiMax = shphimax;
    fShowerPhiMin = shphimin;

    fImpactMax=impactmax;

    fCWaveLower = cwavelower;
    fCWaveUpper = cwaveupper;

    fNumObsLev = numObslev;
    for (UInt_t i=0; i<numObslev; i++){
      fHeightLev[i]=heightlev[i];
    }
    fSlopeSpec = slopespec;

    fCorsikaVersion = corsika;
    fReflVersion = refl;
    fCamVersion = cam;
    fOpticLinksNoise= opticnoise;

    fRandomPointingConeSemiAngle=conesmiangle;
}

// -------------------------------------------------------------------------
//
// GetStarFieldRa. Get RA coordinates of the starfield
//
void MMcRunHeader::GetStarFieldRa(Int_t *hour, Int_t *minute, Int_t *second) const
{
    *hour   = fStarFieldRaH;
    *minute = fStarFieldRaM;
    *second = fStarFieldRaS;
}
// -------------------------------------------------------------------------
//
// GetStarFieldDec. Get DE coordinates of the starfield
//
void MMcRunHeader::GetStarFieldDec(Int_t *degree, Int_t *minute, Int_t *second) const
{
    *degree = fStarFieldDeD;
    *minute = fStarFieldDeM;
    *second = fStarFieldDeS;
}
