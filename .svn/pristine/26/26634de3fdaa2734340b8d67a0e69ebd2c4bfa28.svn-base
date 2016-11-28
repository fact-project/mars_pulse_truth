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
// MMcCorsikaRunHeader
//
// Root storage container for the CORSIKA run header
//
// It saves in a root file all variable that are in the CORSIKA run header, except
// dummies.
//
////////////////////////////////////////////////////////////////////////////


#include "MGeomCorsikaCT.h"
#include "MMcCorsikaRunHeader.h"
#include "MLog.h"
#include "MLogManip.h"

ClassImp(MMcCorsikaRunHeader);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
//
MMcCorsikaRunHeader::MMcCorsikaRunHeader(const char *name, const char *title,
					 int NumCT)
  : fTelescopes(NumCT)
{
    fName  = name  ? name  : "MMcCorsikaRunHeader";
    fTitle = title ? title : "Translation of the CORSIKA header";

    fRunNumber = 0;
    fDate = 0;
    fCorsikaVersion = 0;
    fNumObsLev = 0;

    for (int i=0; i<10; i++)
        fHeightLev[i]=0;

    fSlopeSpec = 0;
    fELowLim = 0;
    fEUppLim = 0;
    fEGS4flag = 0;
    fNKGflag = 0;
    fEcutoffh = 0;
    fEcutoffm = 0;
    fEcutoffe = 0;
    fEcutoffg = 0;

    for(int i=0; i<50; i++)  fC[i] = 0;
    for(int i=0; i<40; i++)  fCKA[i] = 0;
    for(int i=0; i<5; i++)  fCETA[i] = 0;
    for(int i=0; i<11; i++)  fCSTRBA[i] = 0;
    for(int i=0; i<5; i++)
    {
        fAATM[i] = 0;
        fBATM[i] = 0;
        fCATM[i] = 0;
    }
    for (int i=0;i<4; i++)  fNFL[i] = 0;

    fViewconeAngles[0]=0.0;
    fViewconeAngles[1]=0.0;

    fWobbleMode=0;
    fAtmosphericModel=0;

    fNumCT=NumCT;

    fTelescopes.SetOwner();

    for  (int i=0;i<NumCT;i++)
      {
	MGeomCorsikaCT* dummy = new  MGeomCorsikaCT();
	fTelescopes.Add(dummy);
      }

    SetReadyToSave();
}

// -------------------------------------------------------------------------
//
// Fill Corsika Run Header
//
void MMcCorsikaRunHeader::Fill(const Float_t  runnumber,
                               const Float_t  date,
			       const Float_t  vers,
			       const Float_t  numobslev,
			       const Float_t  height[10],
			       const Float_t  slope,
			       const Float_t  elow,
			       const Float_t  eupp,
			       const Float_t  egs4,
			       const Float_t  nkg,
			       const Float_t  eh,
			       const Float_t  em,
			       const Float_t  ee,
			       const Float_t  eg,
			       const Float_t  c[50],
			       const Float_t  cka[40],
			       const Float_t  ceta[5],
			       const Float_t  cstrba[11],
			       const Float_t  aatm[5],
			       const Float_t  batm[5],
			       const Float_t  catm[5],
			       const Float_t  nfl[4],
			       const Float_t  viewcone[2],
			       const Float_t  wobble,
			       const Float_t  atmospher
			       )
{
    fRunNumber = runnumber;
    fDate = date;
    fCorsikaVersion = vers;
    fNumObsLev = numobslev;

    for (int i=0; i<10; i++)
        fHeightLev[i]=height[i];

    fSlopeSpec = slope;
    fELowLim = elow;
    fEUppLim = eupp;
    fEGS4flag = egs4;
    fNKGflag = nkg;
    fEcutoffh = eh;
    fEcutoffm = em;
    fEcutoffe = ee;
    fEcutoffg = eg;

    for (int i=0; i<50; i++)  fC[i] = c[i];
    for (int i=0; i<40; i++)  fCKA[i] = cka[i];
    for (int i=0; i<5 ; i++)  fCETA[i] = ceta[i];
    for (int i=0; i<11; i++)  fCSTRBA[i] = cstrba[i];
    for (int i=0; i<5; i++)
    {
        fAATM[i] = aatm[i];
        fBATM[i] = batm[i];
        fCATM[i] = catm[i];
    }
    for (int i=0; i<4; i++)  fNFL[i] = nfl[i];

    fViewconeAngles[0]=viewcone[0];
    fViewconeAngles[1]=viewcone[1];

    fWobbleMode=wobble;
    fAtmosphericModel=atmospher;

}

// -------------------------------------------------------------------------
//
// Fill C Telescope information in Corsika Run Header
//
void MMcCorsikaRunHeader::FillCT(Float_t ctx, Float_t cty, Float_t ctz,
				 Float_t cttheta, Float_t ctphi,
				 Float_t ctdiam, Float_t ctfocal,
				 Int_t CTnum){

  static_cast<MGeomCorsikaCT*>(fTelescopes.UncheckedAt(CTnum))
    ->Fill(ctx, cty, ctz, cttheta, ctphi, ctdiam, ctfocal);

}

// -------------------------------------------------------------------------
//
// Returns a reference of the i-th entry (the telescope with the index i)
//
MGeomCorsikaCT &MMcCorsikaRunHeader::operator[](Int_t i) const
{
    return *static_cast<MGeomCorsikaCT*>(fTelescopes.UncheckedAt(i));
}

// --------------------------------------------------------------------------
//
//  Prints the information of all telescopes
//
void MMcCorsikaRunHeader::Print(Option_t *) const
{
    //
    //   Print Information about the Geometry of the camera
    //
    *fLog << all << GetTitle() <<":" << endl;
    *fLog << " Spectral Slope: " << fSlopeSpec << " from " << fELowLim << "GeV to " << fEUppLim << "GeV" << endl;
    *fLog << " Number of Telescopes: " << fNumCT << endl;
    fTelescopes.Print();
} 
