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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Harald Kornmayer, 1/2001
!   Author(s): Nadia Tonello, 4/2003 <mailto:tonello@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MImgCleanTGB
//
//
//  Input Containers:
//   MGeomCam, MCerPhotEvt, MSigmabar
//
//  Output Containers:
//   MCerPhotEvt
//
/////////////////////////////////////////////////////////////////////////////
#include "MImgCleanTGB.h"

#include <stdlib.h>       // atof					  
#include <fstream>        // ofstream, SavePrimitive

#include <TGFrame.h>      // TGFrame
#include <TGLabel.h>      // TGLabel
#include <TArrayC.h>      // TArrayC
#include <TGTextEntry.h>  // TGTextEntry

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MSigmabar.h"

#include "MGeomPix.h"
#include "MGeomCam.h"

#include "MCerPhotPix.h"
#include "MCerPhotEvt.h"

#include "MPedPhotPix.h"
#include "MPedPhotCam.h"

#include "MGGroupFrame.h" // MGGroupFrame

ClassImp(MImgCleanTGB);

using namespace std;

enum {
    kImgCleanLvl1,
    kImgCleanLvl2
};

static const TString gsDefName  = "MImgCleanTGB";
static const TString gsDefTitle = "Task to perform image cleaning";

// --------------------------------------------------------------------------
//
// Default constructor. Here you can specify the cleaning method and levels. 
// If you don't specify them the 'common standard' values 3.0 and 2.5 (sigma
// above mean) are used.
// Here you can also specify how many rings around the core pixels you want 
// to analyze (with the fixed lvl2). The default value for "rings" is 1.
//
MImgCleanTGB::MImgCleanTGB(const Float_t lvl1, const Float_t lvl2,
                               const char *name, const char *title)
    : fSgb(NULL), fCleaningMethod(kStandard), fCleanLvl1(lvl1),
    fCleanLvl2(lvl2), fCleanRings(1)

{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    Print();
}


Int_t MImgCleanTGB::CleanStep3b(MCerPhotPix &pix)
{
    const Int_t id = pix.GetPixId();

    //
    // check if the pixel's next neighbor is a core pixel.
    // if it is a core pixel set pixel state to: used.
    //
    MGeomPix   &gpix  = (*fCam)[id];
    const Int_t nnmax = gpix.GetNumNeighbors();

    Int_t rc = 0;

    for (Int_t j=0; j<nnmax; j++)
    {
        const Int_t id2 = gpix.GetNeighbor(j);

	if (fEvt->GetPixById(id2) && fEvt->IsPixelUsed(id2))
            rc++;
    }
    return rc;
}

// --------------------------------------------------------------------------
//
//   Look for the boundary pixels around the core pixels
//   if a pixel has more than 2.5 (clean level 2.5) sigma, and
//   a core neigbor, it is declared as used.
//
void MImgCleanTGB::CleanStep3(Int_t num1, Int_t num2)
{
    const Int_t entries = fEvt->GetNumPixels();

    Int_t *u = new Int_t[entries];

    for (Int_t i=0; i<entries; i++)
    {
        //
        // get pixel as entry il from list
        //
        MCerPhotPix &pix = (*fEvt)[i];
        u[i] = CleanStep3b(pix);
    }

    for (Int_t i=0; i<entries; i++)
    {
        MCerPhotPix &pix = (*fEvt)[i];
        if (u[i]<num1)
            pix.SetPixelUnused();
        if (u[i]>num2)
            pix.SetPixelUsed();
    }

    delete u;
}

void MImgCleanTGB::CleanStep3(Byte_t *nb, Int_t num1, Int_t num2)
{
    const Int_t entries = fEvt->GetNumPixels();

    for (Int_t i=0; i<entries; i++)
    {
        MCerPhotPix &pix = (*fEvt)[i];

        const Int_t idx = pix.GetPixId();

        if (nb[idx]<num1 && pix.IsPixelUsed())
        {
            const MGeomPix &gpix = (*fCam)[idx];
            const Int_t nnmax = gpix.GetNumNeighbors();
            for (Int_t j=0; j<nnmax; j++)
                nb[gpix.GetNeighbor(j)]--;

            pix.SetPixelUnused();
        }
    }

    for (Int_t i=0; i<entries; i++)
    {
        MCerPhotPix &pix = (*fEvt)[i];

        const Int_t idx = pix.GetPixId();
        if (nb[idx]>num2 && !pix.IsPixelUsed())
        {
            const MGeomPix &gpix = (*fCam)[idx];
            const Int_t nnmax = gpix.GetNumNeighbors();
            for (Int_t j=0; j<nnmax; j++)
                nb[gpix.GetNeighbor(j)]++;

            pix.SetPixelUsed();
        }
    }
}

// --------------------------------------------------------------------------
//
//  Check if MEvtHeader exists in the Parameter list already.
//  if not create one and add them to the list
//
Int_t MImgCleanTGB::PreProcess (MParList *pList)
{
    fCam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fCam)
    {
        *fLog << dbginf << "MGeomCam not found (no geometry information available)... aborting." << endl;
        return kFALSE;
    }

    fEvt = (MCerPhotEvt*)pList->FindObject("MCerPhotEvt");
    if (!fEvt)
    {
        *fLog << dbginf << "MCerPhotEvt not found... aborting." << endl;
        return kFALSE;
    }

    if (fCleaningMethod == kDemocratic)
    {
        fSgb = (MSigmabar*)pList->FindObject("MSigmabar");
        if (!fSgb)
        {
            *fLog << dbginf << "MSigmabar not found... aborting." << endl;
            return kFALSE;
        }
    }
    else
    {
        fPed = (MPedPhotCam*)pList->FindObject("MPedPhotCam");
        if (!fPed)
        {
            *fLog << dbginf << "MPedPhotCam not found... aborting." << endl;
            return kFALSE;
        }
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Cleans the image.
//
Int_t MImgCleanTGB::Process()
{
    const Int_t entries = fEvt->GetNumPixels();

    Double_t sum = 0;
    Double_t sq  = 0;
    Double_t w   = 0;
    Double_t w2  = 0;
    for (Int_t i=0; i<entries; i++)
    {
        //
        // get pixel as entry il from list
        //
        MCerPhotPix &pix = (*fEvt)[i];

        const Int_t    idx        = pix.GetPixId();
        const Double_t entry      = pix.GetNumPhotons();
        const Double_t factor     = fCam->GetPixRatio(idx);
        const Double_t factorsqrt = fCam->GetPixRatioSqrt(idx);
        const Float_t  noise      = (*fPed)[idx].GetRms();

        if (entry * factorsqrt <= fCleanLvl2 * noise)
        {
            sum += entry*factor;
            sq  += entry*entry*factor*factor;
            w   += factor;
            w2  += factor*factor;
        }
    }

    Double_t mean = sum/w;
    Double_t sdev = sqrt(sq/w2 - mean*mean);

    TArrayC n(fCam->GetNumPixels());
    Byte_t *nb = (Byte_t*)n.GetArray();
    //Byte_t *nb = new Byte_t[1000];
    //memset(nb, 0, 577);

    for (Int_t i=0; i<entries; i++)
    {
        //
        // get pixel as entry il from list
        //
        MCerPhotPix &pix = (*fEvt)[i];
        const Int_t idx = pix.GetPixId();

        const Double_t entry  = pix.GetNumPhotons();
        const Double_t factor = fCam->GetPixRatio(idx);

        if (entry*factor > fCleanLvl1*sdev)
        {
            pix.SetPixelUsed();

            const MGeomPix &gpix = (*fCam)[idx];
            const Int_t nnmax = gpix.GetNumNeighbors();
            for (Int_t j=0; j<nnmax; j++)
                nb[gpix.GetNeighbor(j)]++;
        }
        else
            pix.SetPixelUnused();
    }

    CleanStep3(nb, 2, 3);
    //CleanStep3(nb, 2, 3);
    //CleanStep3(nb, 2, 3);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Print descriptor and cleaning levels.
//
void MImgCleanTGB::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << " using ";
    switch (fCleaningMethod)
    {
    case kDemocratic:
        *fLog << "democratic";
        break;
    case kStandard:
        *fLog << "standard";
        break;
    }
    *fLog << " cleaning initialized with noise level " << fCleanLvl1 << " and " << fCleanLvl2;
    *fLog << " (CleanRings=" << fCleanRings << ")" << endl;
}

// --------------------------------------------------------------------------
//
//  Create two text entry fields, one for each cleaning level and a
//  describing text line.
//
void MImgCleanTGB::CreateGuiElements(MGGroupFrame *f)
{
  //
  // Create a frame for line 3 and 4 to be able
  // to align entry field and label in one line
  //
  TGHorizontalFrame *f1 = new TGHorizontalFrame(f, 0, 0);
  TGHorizontalFrame *f2 = new TGHorizontalFrame(f, 0, 0);
  
  /*
   * --> use with root >=3.02 <--
   *
   
   TGNumberEntry *fNumEntry1 = new TGNumberEntry(frame, 3.0, 2, M_NENT_LVL1, kNESRealOne, kNEANonNegative);
   TGNumberEntry *fNumEntry2 = new TGNumberEntry(frame, 2.5, 2, M_NENT_LVL1, kNESRealOne, kNEANonNegative);

  */
  TGTextEntry *entry1 = new TGTextEntry(f1, "****", kImgCleanLvl1);
  TGTextEntry *entry2 = new TGTextEntry(f2, "****", kImgCleanLvl2);
  
  // --- doesn't work like expected (until root 3.02?) --- fNumEntry1->SetAlignment(kTextRight);
  // --- doesn't work like expected (until root 3.02?) --- fNumEntry2->SetAlignment(kTextRight);
  
  entry1->SetText("3.0");
  entry2->SetText("2.5");
  
  entry1->Associate(f);
  entry2->Associate(f);
  
  TGLabel *l1 = new TGLabel(f1, "Cleaning Level 1");
  TGLabel *l2 = new TGLabel(f2, "Cleaning Level 2");
  
  l1->SetTextJustify(kTextLeft);
  l2->SetTextJustify(kTextLeft);
  
  //
  // Align the text of the label centered, left in the row
  // with a left padding of 10
  //
  TGLayoutHints *laylabel = new TGLayoutHints(kLHintsCenterY|kLHintsLeft, 10);
  TGLayoutHints *layframe = new TGLayoutHints(kLHintsCenterY|kLHintsLeft,  5, 0, 10);
  
  //
  // Add one entry field and the corresponding label to each line
  //
  f1->AddFrame(entry1);
  f2->AddFrame(entry2);
  
  f1->AddFrame(l1, laylabel);
  f2->AddFrame(l2, laylabel);
  
  f->AddFrame(f1, layframe);
  f->AddFrame(f2, layframe);
  
  f->AddToList(entry1);
  f->AddToList(entry2);
  f->AddToList(l1);
  f->AddToList(l2);
  f->AddToList(laylabel);
  f->AddToList(layframe);
}

// --------------------------------------------------------------------------
//
//  Process the GUI Events comming from the two text entry fields.
//
Bool_t MImgCleanTGB::ProcessMessage(Int_t msg, Int_t submsg, Long_t param1, Long_t param2)
{
    if (msg!=kC_TEXTENTRY || submsg!=kTE_ENTER)
        return kTRUE;

    TGTextEntry *txt = (TGTextEntry*)FindWidget(param1);

    if (!txt)
        return kTRUE;

    Float_t lvl = atof(txt->GetText());

    switch (param1)
    {
    case kImgCleanLvl1:
        fCleanLvl1 = lvl;
        *fLog << "Cleaning level 1 set to " << lvl << " sigma." << endl;
        return kTRUE;

    case kImgCleanLvl2:
        fCleanLvl2 = lvl;
        *fLog << "Cleaning level 2 set to " << lvl << " sigma." << endl;
        return kTRUE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MImgCleanTGB::StreamPrimitive(ostream &out) const
{
    out << "   MImgCleanTGB " << GetUniqueName() << "(";
    out << fCleanLvl1 << ", " << fCleanLvl2;

    if (fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << ", \"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\"";
    }
    out << ");" << endl;

    if (fCleaningMethod!=kDemocratic)
        return;

    out << "   " << GetUniqueName() << ".SetMethod(MImgCleanTGB::kDemocratic);" << endl;

    if (fCleanRings==1)
        return;

    out << "   " << GetUniqueName() << ".SetCleanRings(" << fCleanRings << ");" << endl;
}
