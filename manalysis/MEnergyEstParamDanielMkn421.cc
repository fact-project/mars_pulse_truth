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
!              Wolfgang Wittek  7/2003 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MEnergyEstParamDanielMkn421                                             //
//                                                                         //
// Task to estimate the energy using a parametrization.                    //
// Make sure, that all source dependant containers are based on the same   //
// set of basic hillas parameters                                          //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MEnergyEstParamDanielMkn421.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MMcEvt.hxx"
#include "MHMatrix.h"
#include "MHillasSrc.h"
#include "MNewImagePar.h"
#include "MEnergyEst.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MEnergyEstParamDanielMkn421);

using namespace std;

// --------------------------------------------------------------------------
//
// Initialize the coefficients with Daniel's values
// for Mkn421
//
//
void MEnergyEstParamDanielMkn421::InitCoefficients()
{
    // parameters for the impact parameter
    fA.Set(4);
    fA[0] = 12.0322;  // 
    fA[1] =  8.29911; // 
    fA[2] = 73.0699;  // 
    fA[3] =  0.311375;


    // parameters for the energy
    fB.Set(6);
    fB[0] =  1.23138;    // 
    fB[1] =  0.00276497; // 
    fB[2] = -0.0110667;  // 
    fB[3] =  7.47538;    // 
    fB[4] = -1.25619;    // 
    fB[5] =  0.627699;   // 
}

// --------------------------------------------------------------------------
//
// Default constructor. Give the name of the parameter container (MHillas)
// storing width, length and size (Default="MHillas").
// For the Zenith Angle MMcEvt.fTheta is used.
//
MEnergyEstParamDanielMkn421::MEnergyEstParamDanielMkn421(const char *hillas, const char *name, const char *title)
   : fMatrix(NULL)
{
    fName  = name  ? name  : "MEnergyEstParamDanielMkn421";
    fTitle = title ? title : "Task to estimate the energy (Daniel Mkn421)";

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
    AddToBranchList("MNewImagePar.fLeakage1");
}


// --------------------------------------------------------------------------
//
// Destructor.
//
MEnergyEstParamDanielMkn421::~MEnergyEstParamDanielMkn421()
{
    delete fPairs;
    delete fEnergy;
    delete fHillasSrc;
}

// --------------------------------------------------------------------------
//
// Check for all necessary parameter containers.
//
Int_t MEnergyEstParamDanielMkn421::PreProcess(MParList *plist)
{
    const MGeomCam *geom = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!geom)
    {
        *fLog << warn << dbginf << "No Camera Geometry available" << endl;
        return kFALSE;
    }
    else
    {
        fMm2Deg = geom->GetConvMm2Deg();
    }


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

        fNewImagePar = (MNewImagePar*)plist->FindObject("MNewImagePar");
        if (!fNewImagePar)
        {
            *fLog << err << dbginf << " object 'MNewImagePar' not found... aborting." << endl;
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
void MEnergyEstParamDanielMkn421::SetCoeffA(const TArrayD &arr)
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
void MEnergyEstParamDanielMkn421::SetCoeffB(const TArrayD &arr)
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
void MEnergyEstParamDanielMkn421::SetCoeff(const TArrayD &arr)
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
Double_t MEnergyEstParamDanielMkn421::GetVal(Int_t i) const
{
    return (*fMatrix)[fMap[i]];
}

// --------------------------------------------------------------------------
//
// You can use this function if you want to use a MHMatrix instead of the
// given containers. This function adds all necessary columns to the
// given matrix. Afterward you should fill the matrix with the corresponding
// data (eg from a file by using MHMatrix::Fill). If you now loop
// through the matrix (eg using MMatrixLoop) MEnergyEstParamDanielMkn421::Process
// will take the values from the matrix instead of the containers.
//
void MEnergyEstParamDanielMkn421::InitMapping(MHMatrix *mat)
{
    if (fMatrix)
        return;

    fMatrix = mat;

    fMap[0] = fMatrix->AddColumn("MMcEvt.fTelescopeTheta");
    fMap[1] = fMatrix->AddColumn(fHillasName+".fWidth");
    fMap[2] = fMatrix->AddColumn(fHillasName+".fLength");
    fMap[3] = fMatrix->AddColumn(fHillasName+".fSize");
    fMap[4] = fMatrix->AddColumn("MNewImagePar.fLeakage1");

    Int_t col = 5;
    TIter Next(fPairs);
    TObject *o = NULL;
    while ((o=Next()))
        fMap[col++] = fMatrix->AddColumn(TString(o->GetName())+".fDist");
}

void MEnergyEstParamDanielMkn421::StopMapping()
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
void MEnergyEstParamDanielMkn421::Add(const TString hillas, const TString energy)
{
    fPairs->Add(new TNamed(hillas, energy));

    AddToBranchList(hillas+".fDist");
}

void MEnergyEstParamDanielMkn421::Print(Option_t *opt) const
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
Int_t MEnergyEstParamDanielMkn421::Process()
{

    const Double_t theta  = fMatrix ? GetVal(0) : fMc->GetTelescopeTheta();

    Double_t width  = fMatrix ? GetVal(1) : fHillas->GetWidth();
    width *= fMm2Deg;

    Double_t length = fMatrix ? GetVal(2) : fHillas->GetLength();
    length *= fMm2Deg;

    const Double_t size   = fMatrix ? GetVal(3) : fHillas->GetSize();
    Double_t leakage= fMatrix ? GetVal(4) : fNewImagePar->GetLeakage1();
    leakage = (leakage-0.1)<0. ? 0. : leakage-0.1;

    const Double_t k   = 1./cos(theta);
    const Double_t k2  = k*k;

    const Double_t i0 = k  * (fA[3]*k + 1);
    const Double_t e0 = k2 * (fB[4]*k + fB[5]*k2+1);

    TIter NextH(fHillasSrc);
    TIter NextE(fEnergy);

    Int_t col = 5;

    while (1)
    {
        MEnergyEst *est = (MEnergyEst*)NextE();
        if (!est)
            break;

        Double_t dist = fMatrix ? GetVal(col++) : ((MHillasSrc*)NextH())->GetDist();
        dist *= fMm2Deg;

        Double_t ir = i0 * (fA[0] + fA[1]*dist/width + fA[2]*leakage); 
        Double_t er = e0 * (fB[0] + fB[1]*size/width + fB[2]*ir + fB[3]*leakage);
        er *= 1000.0;  // conversion to GeV

        if (ir<0.  || er<0.)
	{
          ir = 0.;
          er = 0.;
	}

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

























