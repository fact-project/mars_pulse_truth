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
!   Author(s): Eva Domingo    , 12/2004 <mailto:domingo@ifae.es>
!              Wolfgang Wittek, 12/2004 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   MDispCalc                                                             //
//                                                                         //
//   This task calculates Disp with a given parameterization               //
//   (parameters are stored in the MDispParameters container)              //
//   Also the matrix of variables to be used in the Disp                   //
//   parameterization is defined                                           //
//                                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MDispCalc.h"

#include <math.h>
#include <TArray.h>

#include "MGeomCam.h"
#include "MSrcPosCam.h"
#include "MHillas.h"
#include "MHillasExt.h"
#include "MNewImagePar.h"
#include "MMcEvt.hxx"
#include "MPointingPos.h"
#include "MImageParDisp.h"
#include "MDispParameters.h"

#include "MHMatrix.h"

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"


ClassImp(MDispCalc);

using namespace std;

static const Int_t nPars=5;   // number of parameters used in the Disp expression

enum dispVar_t {kSize,kWidth,kLength,kConc,kLeakage1,kLeakage2,kTotVar};  // enum variables for the
                                                                          // matrix columns mapping

// --------------------------------------------------------------------------
//
// constructor
//
MDispCalc::MDispCalc(const char *imagepardispname, 
		     const char *dispparametersname, 
                     const char *name, const char *title)
  :     fImageParDispName(imagepardispname), 
	fDispParametersName(dispparametersname),
	fLogSize0(3.), fLength0(0.1), fWidth0(0.05), 
	fConc0(0.35), fLeakage10(0.05), fLeakage20(0.1)
{
    fName  = name  ? name  : "MDispCalc";
    fTitle = title ? title : "Class to calculate Disp";

    // object of MDispParamteres that will hold the Disp parameters
    fDispParameters = new MDispParameters(fDispParametersName);
    // initialize the size of the Disp parameters and parameters stepsizes arrays
    fDispParameters->GetParameters().Set(nPars);
    fDispParameters->GetStepsizes().Set(nPars);
    // set Disp parameters to their default values
    fDispParameters->InitParameters();

    fMatrix = NULL;

    // initialize mapping array dimension to the number of columns of fMatrix
    fMap.Set(kTotVar);
}


// --------------------------------------------------------------------------
//
// Default destructor.
//
MDispCalc::~MDispCalc()
{
    delete fDispParameters;
}


// --------------------------------------------------------------------------
//
Int_t MDispCalc::PreProcess(MParList *pList)
{
    // look for the defined camera geometry to get mm to deg conversion factor
    MGeomCam *cam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!cam)
    {
        *fLog << err << "MGeomCam (Camera Geometry) not found... aborting." 
              << endl;
        return kFALSE;
    }

    fMm2Deg = cam->GetConvMm2Deg();


    // look for Disp related containers
    fImageParDisp = (MImageParDisp*)pList->FindCreateObj("MImageParDisp", 
                                                         fImageParDispName);
    if (!fImageParDisp)
    {
        *fLog << err << fImageParDispName 
              << " [MImageParDisp] not found... aborting." << endl;
        return kFALSE;
    }


    //-----------------------------------------------------------
    // if fMatrix exists, skip preprocessing and just read events from matrix;
    // if doesn't exist, read variables values from containers, so look for them
    if (fMatrix)
        return kTRUE;

    fSrcPos = (MSrcPosCam*)pList->FindObject("MSrcPosCam");
    if (!fSrcPos)
    {
        *fLog << err << "MSrcPosCam not found... aborting." << endl;
        return kFALSE;
    }

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

    fNewPar = (MNewImagePar*)pList->FindObject("MNewImagePar");
    if (!fNewPar)
    {
        *fLog << err << "MNewImagePar not found... aborting." << endl;
        return kFALSE;
    }

    fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
    if (!fMcEvt)
    {
        *fLog << err << "MMcEvt not found... This is not a MC file,"
	      << " you are not trying to optimize Disp, just calculating it."
	      << endl;
	//        return kFALSE;
    }

    fPointing = (MPointingPos*)pList->FindObject("MPointingPos");
    if (!fPointing)
    {
        *fLog << err << "MPointingPos not found... aborting." << endl;
	return kFALSE;
    }

    //------------------------------------------

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// You can use this function if you want to use a MHMatrix instead of the
// given containers for the Disp parameterization. This function adds all 
// necessary columns to the given matrix. Afterwards, you should fill 
// the matrix with the corresponding data (eg from a file by using 
// MHMatrix::Fill). Then, if you loop through the matrix (eg using 
// MMatrixLoop), MDispCalc::Calc will take the values from the matrix 
// instead of the containers.
//
void MDispCalc::InitMapping(MHMatrix *mat)
{
    if (fMatrix)
      return;

    fMatrix = mat;

    fMap[kSize]      = fMatrix->AddColumn("MHillas.fSize");
    fMap[kWidth]     = fMatrix->AddColumn("MHillas.fWidth");
    fMap[kLength]    = fMatrix->AddColumn("MHillas.fLength");

    fMap[kConc]      = fMatrix->AddColumn("MNewImagePar.fConc");
    fMap[kLeakage1]  = fMatrix->AddColumn("MNewImagePar.fLeakage1");
    fMap[kLeakage2]  = fMatrix->AddColumn("MNewImagePar.fLeakage2");
}


// --------------------------------------------------------------------------
//
// Returns a mapped value from the Matrix
//
Double_t MDispCalc::GetVal(Int_t i) const
{
    Double_t val = (*fMatrix)[fMap[i]];

    //    *fLog << "MDispCalc::GetVal; i, fMatrix, fMap, val = "
    //          << i << ",  " << fMatrix << ",  " << fMap[i] << ",  " << val << endl;

    return val;
}


// ---------------------------------------------------------------------------
//
// Calculate Disp 
// 
//
Int_t MDispCalc::Process()
{
    // get variables needed to compute disp from the matrix or the containers
    const Double_t size     = fMatrix ? GetVal(kSize)     : fHil->GetSize();
    const Double_t width0   = fMatrix ? GetVal(kWidth)    : fHil->GetWidth();
    const Double_t length0  = fMatrix ? GetVal(kLength)   : fHil->GetLength();
    const Double_t conc     = fMatrix ? GetVal(kConc)     : fNewPar->GetConc();
    const Double_t leakage1 = fMatrix ? GetVal(kLeakage1) : fNewPar->GetLeakage1();
    const Double_t leakage2 = fMatrix ? GetVal(kLeakage2) : fNewPar->GetLeakage2();

    // convert to deg
    const Double_t width   = width0  * fMm2Deg;
    const Double_t length  = length0 * fMm2Deg;

    // create an array to pass the variables to MDispCalc::Calc
    TArrayD imagevars(kTotVar);
    imagevars[kSize]      = log10(size);
    imagevars[kWidth]     = width;
    imagevars[kLength]    = length;
    imagevars[kConc]      = conc;
    imagevars[kLeakage1]  = leakage1;
    imagevars[kLeakage2]  = leakage2;

    // compute Disp
    Double_t disp = Calc(imagevars); 

    // save value into MImageParDisp container
    fImageParDisp->SetDisp(disp);
    fImageParDisp->SetReadyToSave();

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Set the Disp parameterization and Calculate Disp
//
//
Double_t MDispCalc::Calc(TArrayD &imagevar)
{
    // get parameters
    const TArrayD& p = fDispParameters->GetParameters();

    // get image variables to be used in the Disp parameterization
    Double_t logsize  = imagevar[0];
    Double_t width    = imagevar[1];
    Double_t length   = imagevar[2];
    //    Double_t conc     = imagevar[3];
    //    Double_t leakage1 = imagevar[4];
    //    Double_t leakage2 = imagevar[5];
    
    // Disp parameterization to be optimized
    //  Note: fLogSize0, fLength0... variables are introduced to uncorrelate as much
    //        as possible the parameters in the Disp expression, with the purpose of
    //        helping the minimization algorithm to converge. They are set approx.
    //        to their distribution mean value in the MDisp constructor, but can be 
    //        changed using the corresponding set function.
    //

    //    Double_t disp = p[0] + p[1]*(logsize-fLogSize0) 
    //      + (p[2] + p[3]*(logsize-fLogSize0))*width/length;

//    Double_t disp = p[0] + p[1]*(logsize-fLogSize0) + p[4]*(length-fLength0) 
//      + (p[2] + p[3]*(logsize-fLogSize0))*width/length;

    //    Double_t disp = p[0] + p[1]*(logsize-fLogSize0) + p[4]*(length-fLength0) 
    //      + (p[2] + p[3]*(logsize-fLogSize0))*length/width;

    //    Double_t disp = p[0] + p[1]*(logsize-fLogSize0) + p[4]*(length-fLength0) 
    //      + (p[2] + p[3]*(logsize-fLogSize0) + p[5]*(length-fLength0))*width/length;

    //    Double_t disp = p[0] + p[1]*(logsize-fLogSize0) + p[4]*(width-fWidth0) 
    //      + (p[2] + p[3]*(logsize-fLogSize0))*width/length;   // + p[5]*(width-fWidth0))*width/length;

    //    Double_t disp = p[0] + p[1]*(logsize-fLogSize0) + p[4]*(conc-fConc0) 
    //      + (p[2] + p[3]*(logsize-fLogSize0))*width/length;   // + p[5]*(conc-fConc0))*width/length;

    //    Double_t disp = ( p[0] + p[1]*(logsize-fLogSize0) 
    //		      + p[2]*pow(logsize-fLogSize0,2)
    //		      + p[3]*pow(logsize-fLogSize0,3) 
    //		      + p[4]*pow(logsize-fLogSize0,4) )
    //                    *(1-width/length);

    
    Double_t disp = ( p[0] + p[1]*(logsize-fLogSize0) 
    		    + p[2]*pow(logsize-fLogSize0,2)
    		    + p[3]*pow(logsize-fLogSize0,3) 
    		    + p[4]*pow(logsize-fLogSize0,4) )
                        *( 1./(1.+width/length) );

    /*
    Double_t disp = ( p[0] + p[1]*(logsize) 
		    + p[2]*pow(logsize,2)
    		    + p[3]*pow(logsize,3) 
		    + p[4]*pow(logsize,4) )
                        *( 1./(1.+width/length) );
    */

    return disp;
}

//===========================================================================




