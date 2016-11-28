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
!   Author(s): Oscar Blanch  11/2002 (blanch@ifae.es)
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MMcConfigRunHeader
//
// Root storage container for the MONTE CARLO CONFIGURATION information
//
// It saves in a root file all the infromation about values in the
// configuration files used in the Monte Carlo production: MagicDef
// (definition of the teslescope), Reflectivity.dat (mirror reflectivities),
// qe.dat (PMT QEs), axisdev.dat (mirrors deviations) and lightguides.dat
// (Effect of the Light Guides).
//
//
// Class Version 2:
// ----------------
// removed obsolete variables which are no longer used by reflector, 
// nor present in the magic.def file:  fFocalStdev, fPointStdev, fDevAdjust
//
// Class Version 3:
// ----------------
// Added member fLightCollectionFactorOuter so that we can store
// the data on the simulatedlight collection efficiency (light
// guides + plexiglas +...) as a function of incidence angle for
// outer and inner pixels independently.
//
// Class Version 4:
// ----------------
// Added member fMirrorFraction, the "active" fraction of the
// mirror dish, to account for reflectivity losses, missing
// mirrors, etc.
//
// Class Version 5:
// ----------------
// Added member fPmtTimeJitter, the time jitter of the PMTs (sigma
// of gaussian) for each photoelectron.
//
// Class Version 6:
// ----------------
// Added fParaboloidFocal
//
/////////////////////////////////////////////////////////////////////////////////

//
////////////////////////////////////////////////////////////////////////////
#include "MMcConfigRunHeader.h"

#include <fstream>
#include <iomanip>

#include <TArrayF.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MMcConfigRunHeader);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
//
MMcConfigRunHeader::MMcConfigRunHeader(const char *name, const char *title)
    : fNumMirrors(0), fNumPMTs(0), fIncidentTheta(181), fLightCollectionFactor(181), fLightCollectionFactorOuter(181)
{
    fName  = name  ? name  : "MMcConfigRunHeader";
    fTitle = title ? title : "Mc Configuration Information";

    fRadiusMirror    = -1;
    fFocalDist       = -1;
    fPointSpread     = -1;
    fBlackSpot       = -1;
    fCameraWidth     = -1;
    fParaboloidFocal = -1;

    fMirrors = new TClonesArray("MGeomMirror", 0);
    fPMTs = new TClonesArray("MGeomPMT", 0);

}

// --------------------------------------------------------------------------
//
// DESCRIPTION MISSING Please contact Oscar
//
void MMcConfigRunHeader::SetMagicDef(Float_t radius,
				     Float_t focal,
				     Float_t point,
				     Float_t spot,
				     Float_t camwidth)
{
    fRadiusMirror=radius;
    fFocalDist=focal;
    fPointSpread=point;
    fBlackSpot=spot;
    fCameraWidth=camwidth;
}

// --------------------------------------------------------------------------
//
// Set the values corresponding to the light collection efficiency due to
// light guides, plexiglas, double crossing, collection of 1st dynode. These
// are fed to the camera program via de Data/LightCollection.dat file (see
// camera manual)
// 
void  MMcConfigRunHeader::SetLightCollection(const TArrayF &theta, const TArrayF &factor, const TArrayF &factor_outer)
{
    if (fIncidentTheta.GetSize()    !=theta.GetSize() ||
        fLightCollectionFactor.GetSize()!=factor.GetSize() ||
        fLightCollectionFactorOuter.GetSize()!=factor_outer.GetSize())
    {
        *fLog<< err << dbginf << "fIncidentTheta or fLightCollectionFactor";
        *fLog << "or fLightCollectionFactorOuter" << endl;
        *fLog << "do not have size of setting arrays" << endl;
        return;
    }

    fIncidentTheta = theta;
    fLightCollectionFactor = factor;
    fLightCollectionFactorOuter = factor_outer;
}
