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
!   Author(s): Thomas Bretz  9/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MEnergyEstParam                                                         //
//                                                                         //
// Task to estimate the energy using a parametrization.                    //
// Make sure, that all source dependant containers are based on the same   //
// set of basic hillas parameters                                          //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MEnergyEstParam.h"

#include "MParList.h"

#include "MMcEvt.hxx"
#include "MHMatrix.h"
#include "MHillasSrc.h"
#include "MEnergyEst.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MEnergyEstParam);

using namespace std;

// --------------------------------------------------------------------------
//
// Initialize the coefficients with (nonsense) values
//
void MEnergyEstParam::InitCoefficients()
{
    fA.Set(5);
    fA[0] =  -2539; // [cm]
    fA[1] =    900; // [cm]
    fA[2] =   17.5; // [cm]
    fA[3] =  0.006;
    fA[4] =   58.3;

    /*
      fA[0] =   39.667402; // [cm]
      fA[1] =  143.081912; // [cm]
      fA[2] = -395.501677; // [cm]
      fA[3] =    0.006193;
      fA[4] =    0;
    */

    fB.Set(7);
    fB[0] =   -8.69;    // [GeV]
    fB[1] =  -0.069;    // [GeV]
    fB[2] =   0.000655; // [GeV]
    fB[3] =   0.0326;   // [GeV]
    fB[4] = 0.000225;   // [GeV]
    fB[5] =  4.13e-8;   // [GeV]
    fB[6] =     0.05;

    /*
      fB[0] =   45.037867; // [GeV]
      fB[1] =    0.087222; // [GeV]
      fB[2] =   -0.208065; // [GeV/cm]
      fB[3] =   78.164942; // [GeV]
      fB[4] = -159.361283; // [GeV]
      fB[5] =   -0.130455; // [GeV/cm]
      fB[6] =    0.051139;
    */
}

// --------------------------------------------------------------------------
//
// Default constructor. Give the name of the parameter container (MHillas)
// storing width, length and size (Default="MHillas").
// For the Zenith Angle MMcEvt.fTheta is used.
//
MEnergyEstParam::MEnergyEstParam(const char *hillas, const char *name, const char *title)
   : fMatrix(NULL)
{
    fName  = name  ? name  : "MEnergyEstParam";
    fTitle = title ? title : "Task to estimate the energy";

    fHillasName = hillas;

    fPairs     = new TList;
    fEnergy    = new TList;
    fHillasSrc = new TList;

    fPairs->SetOwner();

    InitCoefficients();

    AddToBranchList("MMcEvt.fTelescopeTheta");
    AddToBranchList(fHillasName+".fSize");
    AddToBranchList(fHillasName+".fWidth");
    AddToBranchList(fHillasName+".fLength");
}


// --------------------------------------------------------------------------
//
// Destructor.
//
MEnergyEstParam::~MEnergyEstParam()
{
    delete fPairs;
    delete fEnergy;
    delete fHillasSrc;
}

// --------------------------------------------------------------------------
//
// Check for all necessary parameter containers.
//
Int_t MEnergyEstParam::PreProcess(MParList *plist)
{
    if (!fMatrix)
    {
        fHillas = (MHillas*)plist->FindObject(fHillasName, "MHillas");
        if (!fHillas)
        {
            *fLog << err << dbginf << fHillasName << " [MHillas] not found... aborting." << endl;
            return kFALSE;
        }

        fMc = (MMcEvt*)plist->FindObject("MMcEvt");
        if (!fMc)
        {
            *fLog << err << dbginf << "MMcEvt not found... aborting." << endl;
            return kFALSE;
        }
    }

    TObject *obj = NULL;
    TIter Next(fPairs);

    while ((obj=Next()))
    {
        TObject *o = plist->FindCreateObj(obj->GetTitle(), "MEnergyEst");
        if (!o)
            return kFALSE;

        if (!fEnergy->FindObject(obj->GetTitle()))
            fEnergy->Add(o);

        if (fMatrix)
            continue;

        o = plist->FindObject(obj->GetName(), "MHillasSrc");
        if (!o)
        {
            *fLog << err << dbginf << obj->GetName() << " not found... aborting." << endl;
            return kFALSE;
        }
        fHillasSrc->Add(o);
    }

   return kTRUE;    
}

// --------------------------------------------------------------------------
//
// Set the five coefficients for the estimation of the impact parameter.
// Argument must ba a TArrayD of size 5.
//
void MEnergyEstParam::SetCoeffA(const TArrayD &arr)
{
    if (arr.GetSize() != fA.GetSize())
    {
        *fLog << err << dbginf << "Error - Wrong number of coefficients!" << endl;
        return;
    }

    fA = arr;
}

// --------------------------------------------------------------------------
//
// Set the seven coefficients for the estimation of the energy.
// Argument must ba a TArrayD of size 7.
//
void MEnergyEstParam::SetCoeffB(const TArrayD &arr)
{
    if (arr.GetSize() != fB.GetSize())
    {
        *fLog << err << dbginf << "Error - Wrong number of coefficients!" << endl;
        return;
    }

    fB = arr;
}

// --------------------------------------------------------------------------
//
// Set the twelve coefficients for the estimation of impact and energy.
// Argument must ba a TArrayD of size 12.
//
void MEnergyEstParam::SetCoeff(const TArrayD &arr)
{
    if (arr.GetSize() != fA.GetSize()+fB.GetSize())
    {
        *fLog << err << dbginf << "Error - Wrong number of coefficients!" << endl;
        return;
    }

    fA = TArrayD(fA.GetSize(), arr.GetArray());
    fB = TArrayD(fB.GetSize(), arr.GetArray() + fA.GetSize());
}

// --------------------------------------------------------------------------
//
// Returns the mapped value from the Matrix
//
Double_t MEnergyEstParam::GetVal(Int_t i) const
{
    return (*fMatrix)[fMap[i]];
}

// --------------------------------------------------------------------------
//
// You can use this function if you want to use a MHMatrix instead of the
// given containers. This function adds all necessary columns to the
// given matrix. Afterward you should fill the matrix with the corresponding
// data (eg from a file by using MHMatrix::Fill). If you now loop
// through the matrix (eg using MMatrixLoop) MEnergyEstParam::Process
// will take the values from the matrix instead of the containers.
//
void MEnergyEstParam::InitMapping(MHMatrix *mat)
{
    if (fMatrix)
        return;

    fMatrix = mat;

    fMap[0] = fMatrix->AddColumn("MMcEvt.fTelescopeTheta");
    fMap[1] = fMatrix->AddColumn(fHillasName+".fWidth");
    fMap[2] = fMatrix->AddColumn(fHillasName+".fLength");
    fMap[3] = fMatrix->AddColumn(fHillasName+".fSize");

    Int_t col = 4;
    TIter Next(fPairs);
    TObject *o = NULL;
    while ((o=Next()))
        fMap[col++] = fMatrix->AddColumn(TString(o->GetName())+".fDist");
}

void MEnergyEstParam::StopMapping()
{
    fMatrix = NULL; 
    fPairs->Clear();
    fHillasSrc->Clear();
    fEnergy->Clear();
}

// --------------------------------------------------------------------------
//
// Add a pair of input/output containers.
//   eg. Add("MHillasSrc", "MEnergyEst");
// Useful if you want to estimate the stuff for the source and antisource
//
void MEnergyEstParam::Add(const TString hillas, const TString energy)
{
    fPairs->Add(new TNamed(hillas, energy));

    AddToBranchList(hillas+".fDist");
}

void MEnergyEstParam::Print(Option_t *opt) const
{
    for (int i=0; i<fA.GetSize(); i++)
        *fLog << all << "fA[" << i << "]=" << const_cast<TArrayD&>(fA)[i] << endl;
    for (int i=0; i<fB.GetSize(); i++)
        *fLog << all << "fB[" << i << "]=" << const_cast<TArrayD&>(fB)[i] << endl;
}

// --------------------------------------------------------------------------
//
//  Estimates the impact parameter and energy using a parameterization
//  (see code)
//
Int_t MEnergyEstParam::Process()
{
    const Double_t theta  = fMatrix ? GetVal(0) : fMc->GetTelescopeTheta();
    const Double_t width  = fMatrix ? GetVal(1) : fHillas->GetWidth();
    const Double_t length = fMatrix ? GetVal(2) : fHillas->GetLength();
    const Double_t size   = fMatrix ? GetVal(3) : fHillas->GetSize();

    const Double_t k   = 1./cos(theta);
    const Double_t k2  = k*k;

    const Double_t i0 = k * (fA[3]*k+1)/(fA[3]+1);
    const Double_t i1 = fA[0] + fA[2]*width;


    const Double_t e0 = k2 * (fB[6]*k2+1)/(fB[6]+1);

    /* MY PARAM */
    //const Double_t e1 = fB[0] + fB[1]*size + fB[3]*length + fB[4]*size*width;
    //const Double_t e2 = fB[2] + fB[5]*size*width;

    /* MARCOS */
    /*
    const Double_t e1 = fB[0] + fB[1]*size + fB[3]*length + fB[4]*size*length;
    const Double_t e2 = fB[2] + fB[5]*length;
    */

    /* DANIEL */
    const Double_t e1 = fB[0] + fB[1]*size/(length*width) + 
      fB[3]*length + fB[4]*size/width;

    const Double_t e2 = fB[2] + fB[5]*length;

    TIter NextH(fHillasSrc);
    TIter NextE(fEnergy);

    Int_t col = 4;

    while (1)
    {
        MEnergyEst *est = (MEnergyEst*)NextE();
        if (!est)
            break;

        const Double_t dist = fMatrix ? GetVal(col++) : ((MHillasSrc*)NextH())->GetDist();

        /* DANIEL - MARCOS */
        const Double_t ir = i0 * (i1 + fA[1]*dist); // [cm]
        Double_t er = e0 * (e1 + e2*ir);       // [GeV]

        /* THOMAS BRETZ */
        // if (width==0) return kCONTINUE;
        // const Double_t ir = i0 * (i1 + dist*(fA[1]/width + fA[4]/log10(size))); // [cm]
        //Double_t er = e0 * (e1 + e2*ir);      // [GeV]

        /* MKA */
        //Double_t er = e0 * (fB[0] + fB[1]*size/width + fB[2]*ir /*+ d*leakage*/);

        if (width==0)
            return kCONTINUE;

        // !Finitite includes IsNaN
        if (!TMath::Finite(ir))
            *fLog << all << theta << " " << width << " " << length << " " << size << " " << dist << endl;
        if (!TMath::Finite(er))
        {
            *fLog << all << theta << " " << width << " " << length << " " << size << " " << dist << endl;
            er = 0;
        }

	est->SetEnergy(er);
        est->SetImpact(ir);
        est->SetReadyToSave();
    }

    return kTRUE;
}

