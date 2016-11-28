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
!   Author(s): Thomas Bretz 04/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MHBadPixels
//
////////////////////////////////////////////////////////////////////////////
#include "MHBadPixels.h" 

#include <TCanvas.h>

#include "MPointingPos.h"
#include "MBadPixelsCam.h"
#include "MGeomCam.h"
#include "MPedPhotCam.h"
#include "MParList.h"
#include "MBinning.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHBadPixels);

using namespace std;

// -------------------------------------------------------------------------
//
//  Constructor.
//
MHBadPixels::MHBadPixels(const char *name, const char *title)
{
    fName  = name  ? name  : "MHBadPixels";
    fTitle = title ? title : "Histogram for Bad Pixels vs. Theta";

    fBadId = new TH2D;
    fBadId->SetName("2D-IdBadPixels");
    fBadId->SetTitle("2D-IdBadPixels");
    fBadId->SetDirectory(NULL);
    fBadId->UseCurrentStyle();
    fBadId->SetXTitle("\\Theta [\\circ]");
    fBadId->SetYTitle("pixel Id");
    fBadId->SetTitleOffset(1.2, "Y");

    fBadN = new TH2D;
    fBadN->SetName("2D-NBadPixels");
    fBadN->SetTitle("2D-NBadPixels");
    fBadN->SetDirectory(NULL);
    fBadN->UseCurrentStyle();
    fBadN->SetXTitle("\\Theta [\\circ]");
    fBadN->SetYTitle("number of bad pixels");
    fBadN->SetTitleOffset(1.2, "Y");

    // define default binnings
    fBinsTheta = new MBinning;
    fBinsTheta->SetEdgesCos(10,  0.0,  90.0);    // theta

    fBinsPix = new MBinning;
    fBinsPix->SetEdges(578, -0.5, 577.5);    // pixel id

    SetBinning(*fBadId, *fBinsTheta, *fBinsPix);
    SetBinning(*fBadN,  *fBinsTheta, *fBinsPix);

    //-----------------------------------------
    fNamePedPhotCam = "MPedPhotCamFromData";
}


// -------------------------------------------------------------------------
//
//  Destructor.
//
MHBadPixels::~MHBadPixels()
{
  delete fBadId;
  delete fBadN;

  delete fBinsTheta;
  delete fBinsPix;
}

// --------------------------------------------------------------------------
//
// Set the binnings and prepare the filling of the histogram
//
Bool_t MHBadPixels::SetupFill(const MParList *plist)
{
    fCam = (MGeomCam*)plist->FindObject(AddSerialNumber("MGeomCam"));
    if (!fCam)    
    {
        *fLog << err << "MHBadPixels::SetupFill; MGeomCam not found... aborting." << endl;
        return kFALSE;
    }
    *fLog << "MHBadPixels::SetupFill; fCam = " << fCam << endl;

    fPointPos = (MPointingPos*)plist->FindObject("MPointingPos");
    if (!fPointPos)
    {
        *fLog << err << "MPointingPos not found... aborting." << endl;
        return kFALSE;
    }
    
    fPedPhot = (MPedPhotCam*)plist->FindObject(AddSerialNumber(fNamePedPhotCam), "MPedPhotCam");
    if (!fPedPhot)
    {
        *fLog << err << AddSerialNumber(fNamePedPhotCam) 
              << "[MPedPhotCam] not found... aborting." << endl;
        return kFALSE;
    }
    fPedPhot->InitSize(fCam->GetNumPixels());


    //----------------------------------------------------
    *fLog << inf << "Name of MPedPhotCam container : " << fNamePedPhotCam
          << endl;


    //----------------------------------------------------
    // redefine the binnings
    
    // Get Theta Binning
    const MBinning* binstheta  = (MBinning*)plist->FindObject("BinningTheta", "MBinning");
    if (!binstheta)
    {
        *fLog << err << "Object 'BinningTheta' [MBinning] not found... use default binning." << endl;
        binstheta = fBinsTheta;
    }

    // Define binning for pixel number
    const UInt_t npix1 = fPedPhot->GetSize()+1;
    //*fLog << "MHBadPixels::SetupFill(); npix1 = " << npix1 << endl;
    const MBinning binspix(npix1, -0.5, npix1-0.5);

    // Set binnings in histograms
    SetBinning(*fBadId, *binstheta, binspix);
    SetBinning(*fBadN,  *binstheta, binspix);

    *fLog << "MHBadPixels::SetupFill(); binnings were set" << endl;
    


    return kTRUE;
}

// ------------------------------------------------------------------------
// 
// Fill the histograms
//

Int_t MHBadPixels::Fill(const MParContainer *par, const Stat_t w)
{
    if (!par)
    {
        *fLog << err << "ERROR - par==NULL." << endl;
        return kERROR;
    }

    const Double_t theta = fPointPos->GetZd();

    const MBadPixelsCam *fBadPixels = (MBadPixelsCam*)par;

    const UInt_t entries = fPedPhot->GetSize(); 
    UInt_t nb = 0;
    for (UInt_t i=0; i<entries; i++)
    {
      //*fLog << "MHBadPixels::Fill; i = " << i << endl;

      if ( (*fBadPixels)[i].IsUnsuitable() )
      {
	//*fLog << "MHBadPixels::Fill; (UnSuitable) " << endl;

          fBadId->Fill(theta, i, w);
          nb++;
      }   
    }
    fBadN->Fill(theta, nb, w);

    return kTRUE;
}

// --------------------------------------------------------------------
//
// Draw the histograms
//
void MHBadPixels::Draw(Option_t *option)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    pad->Divide(2,2);

    TH1D *h;

    pad->cd(1);
    fBadId->Draw(option);

    pad->cd(2);
    fBadN->Draw(option);

    pad->cd(3);
    gPad->SetBorderMode(0);
    h = ((TH2*)fBadId)->ProjectionY("ProjY-pixId", -1, 9999, "");
    h->SetDirectory(NULL);
    h->SetTitle("Distribution of bad pixel Id");
    h->SetXTitle("Id of bad pixel");
    h->SetYTitle("No. of events");
    h->SetTitleOffset(1.2, "Y");
    h->Draw(option);
    h->SetBit(kCanDelete);

    pad->cd(4);
    gPad->SetBorderMode(0);
    h = ((TH2*)fBadN)->ProjectionY("ProjY-pixN", -1, 9999, "");
    h->SetDirectory(NULL);
    h->SetTitle("Distribution of no.of bad pixels");
    h->SetXTitle("No. of bad pixels");
    h->SetYTitle("No. of events");
    h->SetTitleOffset(1.2, "Y");
    h->Draw(option);
    h->SetBit(kCanDelete);

    pad->Modified();
    pad->Update();
}
//==========================================================================











