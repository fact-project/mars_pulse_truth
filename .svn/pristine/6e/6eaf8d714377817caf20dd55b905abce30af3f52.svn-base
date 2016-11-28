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
!   Author(s): Javier Rico     04/2004 <mailto:jrico@ifae.es>
!              Ester Aliu      10/2004 <mailto:aliu@ifae.es> (update according
!                                        the new classes of the islands)
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MControlPlots
//
//
//////////////////////////////////////////////////////////////////////////////

#include <fstream>

#include "TStyle.h"

#include "MParList.h"
#include "MControlPlots.h"
#include "MIslands.h"
#include "MImgIsland.h"
#include "MHCamera.h"
#include "MGeomCamMagic.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MControlPlots);

using namespace std;

static const TString gsDefName  = "MControlPlots";
static const TString gsDefTitle = "Produce some control plots";

// -------------------------------------------------------------------------
//
// Constructor
//
MControlPlots::MControlPlots(TString filename,const char* name, const char* title)
  : fMode(kOn), fFileName(filename), fGeomCam(NULL), fIslands(NULL), fProduceFile(kTRUE)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    fCameraHisto[kOn] = NULL;
    fCameraHisto[kOff] = NULL;

}
// -------------------------------------------------------------------------
//
// Destructor
//
MControlPlots::~MControlPlots()
{
  Clear();

  if(fCameraHisto[kOn])
    delete fCameraHisto[kOn];
  if(fCameraHisto[kOff])
    delete fCameraHisto[kOff];
}

void MControlPlots::Clear(const Option_t *o)
{

  if(fGeomCam)
    delete fGeomCam;

  fGeomCam = NULL;
}


// -------------------------------------------------------------------------
//
// Look for needed containers.
//
Int_t MControlPlots::PreProcess(MParList* pList)
{ 

  Reset();

  // FIXME! only valid for Magic geometry for the time being!
  fGeomCam = new MGeomCamMagic;

  // look for MIslands object
  fIslands = (MIslands*)pList->FindObject("MIslands");
  if (!fIslands)
    *fLog << warn << AddSerialNumber("MIslands") << " [MIslands] not found... Some control plots will not be produced" << endl;
  else
    {  
      if (fCameraHisto[fMode])
	{
	  *fLog << err << GetDescriptor() 
		<< "Camera with mode " << fMode << " already existing " << endl;
	  return kFALSE;
	}
      TString name = "";
      switch (fMode)
	{
	case kOn:
	  name += "On";
	  break;
	case kOff:
	  name += "Off";
	  break;
	}
      fCameraHisto[fMode] = new MHCamera(*fGeomCam,
					 name.Data(),
					 "Pixels surviving Image Cleaning");
    }
  return kTRUE;
}

// -------------------------------------------------------------------------
//
//
Int_t MControlPlots::Process()
{ 
  if(!fIslands) return kTRUE;

  MImgIsland *imgIsl = NULL;
  TIter Next(fIslands->GetList());
  
  Int_t pixNum = 0;  
  Int_t idPix = -1;
  
  while ((imgIsl=(MImgIsland*)Next())) 
    {
      pixNum = imgIsl->GetPixNum();
      
      for(Int_t k = 0; k<pixNum; k++)
	{
	  idPix = imgIsl->GetPixList(k);
	  fCameraHisto[fMode]->Fill(idPix,1.);
	  fCameraHisto[fMode]->SetUsed(idPix);
	}
    }

  /*  for (UInt_t i=0;i<fGeomCam->GetNumPixels();i++)
    {
      //      cout << fIslands->GetIslId(i) << " ";
      if (fIslands->GetIslId(i)>=0)
	{
	  fCameraHisto[fMode]->Fill(i,1);
	  fCameraHisto[fMode]->SetUsed(i);
	}
	}*/

  //  cout << endl;
  return kTRUE;
}

// -------------------------------------------------------------------------
//
//
Int_t MControlPlots::PostProcess()
{
  if(!fProduceFile) return kTRUE;
  if(fProduceFile && !fFileName.Length())
    {
      *fLog << warn <<  "MControlPlots::PostProcess Warning: output file requested but no name for it" << endl;
      return kTRUE;
    }

  // Canvas style
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetStatColor(0);
  gStyle->SetTitleFillColor(0);

  TCanvas* c = new TCanvas("survivals","Pixels surviving Image Cleaning",800,800);
  MHCamera* diff=NULL;

  // in case both on and off histos are present, print both and the difference between them
  if(fCameraHisto[kOn] && fCameraHisto[kOff])
    {      
      diff = new MHCamera(*fGeomCam,"Diff","Difference of pixels surviving Image Cleaning");

      // Normalize Off to On
      Float_t NormOn=0;
      Float_t NormOff=0;
      for(Int_t i=1;i<diff->GetSize()-2;i++)
	{
	  NormOff+=fCameraHisto[kOff]->GetBinContent(i);
	  NormOn+=fCameraHisto[kOn]->GetBinContent(i);
	}      

      fCameraHisto[kOff]->Scale(NormOn/NormOff);

      for(Int_t i=1;i<diff->GetSize()-2;i++)
	{
	  diff->SetBinContent(i,(Double_t)fCameraHisto[kOn]->GetBinContent(i)-fCameraHisto[kOff]->GetBinContent(i));
	  diff->SetUsed(i);
	}      
      fCameraHisto[kOn]->SetPrettyPalette();
      fCameraHisto[kOff]->SetPrettyPalette();
      diff->SetPrettyPalette();

      c->Divide(2,2);

      Float_t max = TMath::Max(fCameraHisto[kOn]->GetMaximum(),fCameraHisto[kOff]->GetMaximum());
      Float_t min = TMath::Min(fCameraHisto[kOn]->GetMinimum(),fCameraHisto[kOff]->GetMinimum());
      fCameraHisto[kOn]->SetMaximum(max);
      fCameraHisto[kOn]->SetMinimum(min); 
      fCameraHisto[kOff]->SetMaximum(max);
      fCameraHisto[kOff]->SetMinimum(min); 
      
      c->cd(1);
      fCameraHisto[kOn]->Draw();
      gPad->Modified();
      gPad->Update();

      c->cd(2);
      fCameraHisto[kOff]->Draw();
      gPad->Modified();
      gPad->Update();

      c->cd(3);
      diff->Draw();
      gPad->Modified();
      gPad->Update();      

      c->cd(4);
      diff->DrawProjection();
      gPad->Modified();
      gPad->Update();
    }
  // plot the existing histo
  else
    {
      c->cd(1);
      fCameraHisto[fMode]->Draw();
      gPad->Modified();
      gPad->Update();
    }
  
  c->SaveAs(fFileName);
  delete c;  
  if(diff)
    delete diff;
  return kTRUE;
}
