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
!   Author(s): Wolfgang Wittek, 04/2003 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MFSupercuts
//
//   this class calculates the hadronness for the supercuts
//   the parameters of the supercuts are taken
//                  from the container MSupercuts
//
//
/////////////////////////////////////////////////////////////////////////////
#include "MFSupercuts.h"

#include <math.h>
#include <fstream>

#include "TFile.h"
#include "TArrayD.h"

#include "MParList.h"
#include "MHillasExt.h"
#include "MHillasSrc.h"
#include "MNewImagePar.h"
#include "MGeomCam.h"
#include "MHMatrix.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MFSupercuts);

using namespace std;


// --------------------------------------------------------------------------
//
// constructor
//

MFSupercuts::MFSupercuts(const char *name, const char *title)
    : fHil(0), fHilSrc(0), fHilExt(0), fNewPar(0), fMatrix(0), fVariables(84)
{
    fName  = name  ? name  : "MFSupercuts";
    fTitle = title ? title : "Class to evaluate the Supercuts";

    fMatrix = NULL;

    AddToBranchList("MHillas.fWidth");
    AddToBranchList("MHillas.fLength");
    AddToBranchList("MHillasSrc.fDist");
    AddToBranchList("MHillas.fSize");
}

// --------------------------------------------------------------------------
//
Int_t MFSupercuts::PreProcess(MParList *pList)
{
    MGeomCam *cam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!cam)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    fMm2Deg = cam->GetConvMm2Deg();

    if (fMatrix)
        return kTRUE;

    //-----------------------------------------------------------
    fHil = (MHillas*)pList->FindObject("MHillas");
    if (!fHil)
    {
        *fLog << err << "MHillas not found... aborting." << endl;
        return kFALSE;
    }

    fHilExt = (MHillasExt*)pList->FindObject("MHillasExt");
    if (!fHilExt)
    {
        *fLog << err << "MHillasExt not found... aborting." << endl;
        return kFALSE;
    }

    fHilSrc = (MHillasSrc*)pList->FindObject("MHillasSrc");
    if (!fHilSrc)
    {
        *fLog << err << "MHillasSrc not found... aborting." << endl;
        return kFALSE;
    }

    fNewPar = (MNewImagePar*)pList->FindObject("MNewImagePar");
    if (!fNewPar)
    {
        *fLog << err << "MNewImagePar not found... aborting." << endl;
        return kFALSE;
    }
/*
    fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
    if (!fMcEvt)
    {
        *fLog << err << "MMcEvt not found... aborting." << endl;
        return kFALSE;
    }
  */
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Returns the mapped value from the Matrix
//
Double_t MFSupercuts::GetVal(Int_t i) const
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
void MFSupercuts::InitMapping(MHMatrix *mat)
{
    if (fMatrix)
      return;

    fMatrix = mat;

    //fMap[0]  = fMatrix->AddColumn("MPointingPos.fZd");
    fMap[0]  = fMatrix->AddColumn("MHillas.fWidth*MGeomCam.fConvMm2Deg");
    fMap[1]  = fMatrix->AddColumn("MHillas.fLength*MGeomCam.fConvMm2Deg");
    fMap[2]  = fMatrix->AddColumn("MHillasSrc.fDist*MGeomCam.fConvMm2Deg");
    fMap[3]  = fMatrix->AddColumn("log10(MHillas.fSize)");
    //fMap[7]  = fMatrix->AddColumn("fabs(MHillasSrc.fAlpha)");
    /*
     fMap[8]  = fMatrix->AddColumn("sgn(MHillasSrc.fCosDeltaAlpha)*(MHillasExt.fM3Long)");
     fMap[9]  = fMatrix->AddColumn("MNewImagePar.fConc");
     fMap[10] = fMatrix->AddColumn("MNewImagePar.fLeakage1");
     */
}

// --------------------------------------------------------------------------
//
// Calculation of upper and lower limits
//
Double_t MFSupercuts::CtsMCut(const Double_t* a,  Double_t ls, Double_t ct,
                                    Double_t ls2, Double_t dd2) const
{
    // define cut-function
    //
    //    dNOMLOGSIZE = 5.0 (=log(150.0)
    //    dNOMCOSZA   = 1.0
    //
    //      a: array of cut parameters
    //     ls: log(SIZE) - dNOMLOGSIZE
    //    ls2: ls^2
    //     ct: Cos(ZA.) - dNOMCOSZA
    //    dd2: DIST^2
    const Double_t limit =
        a[0] + a[1] * dd2 + a[2] * ct  +
        ls  * (a[3] + a[4] * dd2 + a[5] * ct) +
        ls2 * (a[6] + a[7] * dd2);

    return limit;
}

// ---------------------------------------------------------------------------
//
// Evaluate dynamical supercuts 
// 
//          set hadronness to 0.25 if cuts are fullfilled
//                            0.75 otherwise
//
Int_t MFSupercuts::Process()
{
    //  const Double_t kNomLogSize = 4.1;
    //  const Double_t kNomCosZA   = 1.0;

    //  const Double_t theta   = fMatrix ? GetVal(0) : fMcEvt->GetTelescopeTheta();
    //  const Double_t meanx   = fMatrix ? GetVal(4) : fHil->GetMeanX()*fMm2Deg;
    //  const Double_t meany   = fMatrix ? GetVal(5) : fHil->GetMeanY()*fMm2Deg;
    //  const Double_t asym0   = fMatrix ? GetVal(8) : TMath::Sign(fHilExt->GetM3Long(), fHilSrc->GetCosDeltaAlpha());;
    //  const Double_t conc    = fMatrix ? GetVal(9) : fNewPar->GetConc();
    //  const Double_t leakage = fMatrix ? GetVal(10): fNewPar->GetLeakage1();
    //  const Double_t asym    = asym0   * fMm2Deg;

    const Double_t width   = fMatrix ? GetVal(0) : fHil->GetWidth()*fMm2Deg;
    const Double_t length  = fMatrix ? GetVal(1) : fHil->GetLength()*fMm2Deg;
    const Double_t dist    = fMatrix ? GetVal(2) : fHilSrc->GetDist()*fMm2Deg;
    const Double_t lgsize  = fMatrix ? GetVal(3) : log10(fHil->GetSize());

    const Double_t dist2     = dist*dist;
    const Double_t lgsize2   = lgsize * lgsize;
    const Double_t cost      = 0; // cos(theta*TMath::DegToRad()) - kNomCosZA;

    fResult =
        // Length cuts
        length > CtsMCut(fVariables.GetArray()+ 0, lgsize, cost, lgsize2, dist2) &&
        length < CtsMCut(fVariables.GetArray()+ 8, lgsize, cost, lgsize2, dist2) &&
        // Width cuts
        width  > CtsMCut(fVariables.GetArray()+16, lgsize, cost, lgsize2, dist2) &&
        width  < CtsMCut(fVariables.GetArray()+24, lgsize, cost, lgsize2, dist2) &&
        // Dist cuts
        dist   > CtsMCut(fVariables.GetArray()+32, lgsize, cost, lgsize2, dist2) &&
        dist   < CtsMCut(fVariables.GetArray()+40, lgsize, cost, lgsize2, dist2);

        /*
         asym    < CtsMCut(&fVariables[42], dmls, dmcza, dmls2, dd2) &&
         asym    > CtsMCut(&fVariables[49], dmls, dmcza, dmls2, dd2) &&

         conc    < CtsMCut(&fVariables[56], dmls, dmcza, dmls2, dd2) &&
         conc    > CtsMCut(&fVariables[63], dmls, dmcza, dmls2, dd2) &&

         leakage < CtsMCut(&fVariables[70], dmls, dmcza, dmls2, dd2) &&
         leakage > CtsMCut(&fVariables[77], dmls, dmcza, dmls2, dd2))
         */

    return kTRUE;
}

TString MFSupercuts::GetDataMember() const
{
    return "MHillas.fWidth,MHillas.fLength,MHillasSrc.fDist,MHillas.fSize";
}

void MFSupercuts::SetVariables(const TArrayD &arr)
{
    fVariables = arr;
}

Bool_t MFSupercuts::CoefficentsRead(const char *fname)
{
    ifstream fin(fname);
    if (!fin)
    {
        *fLog << err << "Cannot open file " << fname << ": ";
        *fLog << strerror(errno) << endl;
        return kFALSE;
    }

    for (int i=0; i<fVariables.GetSize(); i++)
    {
        fin >> fVariables[i];
        if (!fin)
        {
            *fLog << err << "ERROR - Not enough coefficients in file " << fname << endl;
            return kERROR;
        }
    }
    return kTRUE;
}

Bool_t MFSupercuts::CoefficentsWrite(const char *fname) const
{
    ofstream fout(fname);
    if (!fout)
    {
        *fLog << err << "Cannot open file " << fname << ": ";
        *fLog << strerror(errno) << endl;
        return kFALSE;
    }

    for (int i=0; i<fVariables.GetSize(); i++)
        fout << setw(11) << fVariables[i] << endl;

    return kTRUE;
}

Int_t MFSupercuts::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    if (MFilter::ReadEnv(env, prefix, print)==kERROR)
        return kERROR;

    if (IsEnvDefined(env, prefix, "File", print))
    {
        const TString fname = GetEnvValue(env, prefix, "File", "");
        if (!CoefficentsRead(fname))
            return kERROR;

        return kTRUE;
    }

    Bool_t rc = kFALSE;
    for (int i=0; i<fVariables.GetSize(); i++)
    {
        if (IsEnvDefined(env, prefix, Form("Param%d", i), print))
        {
            fVariables[i] = GetEnvValue(env, prefix, Form("Param%d", i), 0);
            rc = kTRUE;
        }
    }
    return rc;
}
