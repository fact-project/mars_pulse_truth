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
!   Author(s): Josep Flix   09/2004 <mailto:jflix@ifae.es>
!
!   Copyright: MAGIC Software Development, 2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MTopology
//
// Storage Container for topology parameters:
//
// fDistance = SUM_(i,j) D_ij , where i,j<UsedPixels and d_ij are distances
//                              between pixels. This characterizes topology.
//
// fUsed = Used Pixels after image cleaning.
//
/////////////////////////////////////////////////////////////////////////////

#include "MTopology.h"

#include <iostream>

#include "MGeomPix.h"
#include "MGeomCam.h"
#include "MCerPhotPix.h"
#include "MCerPhotEvt.h"

#include "MLog.h"
#include "MLogManip.h"

using namespace std;
ClassImp(MTopology);


// --------------------------------------------------------------------------
//
// Default constructor.
//

MTopology::MTopology(const char *name, const char *title)
{
    fName  = name  ? name  : "MTopology";
    fTitle = title ? title : "Parameters related to Topology of images after image cleaning";

    Reset();
}

void MTopology::Reset()
{
    fDistance = -1;
    fUsed = -1;
}

void MTopology::Print(Option_t *opt) const
{
      *fLog << all << GetDescriptor() << ":" << endl;
      *fLog << "Topology Distance [mm] = " << fDistance << ": ";
      *fLog << "Used Pixels = " << fUsed << ": " << endl;

}

Int_t MTopology::Calc(const MGeomCam &geom, const MCerPhotEvt &evt)
{
    const Int_t Pixels = evt.GetNumPixels();
   
    Double_t X[Pixels];
    Double_t Y[Pixels];
    Int_t NPixels[Pixels];
 
    if (Pixels < 3)
    {
	Reset();
	return 1;
    };

    MCerPhotPix *pix = 0;
    
    TIter Next(evt);
    
    fN_Pixels = 0;

    Double_t fDist = 0.;
    
    while ((pix=(MCerPhotPix*)Next()))
    {
	const MGeomPix &gpix = geom[pix->GetPixId()];
	
	NPixels[fN_Pixels] = pix->GetPixId();
	X[fN_Pixels] = gpix.GetX();
	Y[fN_Pixels] = gpix.GetY();
	
	fN_Pixels++;
    };

    for (int i = 0; i < fN_Pixels ; i++){
	for (int j = 0; j < fN_Pixels ; j++){
	    fDist += sqrt(pow(X[j]-X[i],2) + pow(Y[j]-Y[i],2));
	};		
    };

    fDistance = (Int_t)(fDist+.5);

    SetDistance(fDistance);
    SetUsedPixels(fN_Pixels);

    SetReadyToSave();

    return 0;
	    
}
