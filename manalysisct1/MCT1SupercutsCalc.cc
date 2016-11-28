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
//                                                                         //
//   MCT1SupercutsCalc                                                     //
//                                                                         //
//   this class calculates the hadronness for the supercuts                //
//   the parameters of the supercuts are taken                             //
//                  from the container MCT1Supercuts                       //
//                                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MCT1SupercutsCalc.h"

#include <math.h>
#include <fstream>

#include "TFile.h"
#include "TArrayD.h"

#include "MParList.h"
#include "MHillasExt.h"
#include "MHillasSrc.h"
#include "MNewImagePar.h"
#include "MMcEvt.hxx"
#include "MCerPhotEvt.h"
#include "MGeomCam.h"
#include "MHadronness.h"
#include "MHMatrix.h"
#include "MCT1Supercuts.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCT1SupercutsCalc);

using namespace std;


// --------------------------------------------------------------------------
//
// constructor
//

MCT1SupercutsCalc::MCT1SupercutsCalc(const char *hilname, 
                                     const char *hilsrcname, 
                                     const char *name, const char *title)
  : fHadronnessName("MHadronness"), fHilName(hilname), fHilSrcName(hilsrcname),
    fHilExtName("MHillasExt"), fNewParName("MNewImagePar"), 
    fSuperName("MCT1Supercuts") 
{
    fName  = name  ? name  : "MCT1SupercutsCalc";
    fTitle = title ? title : "Class to evaluate the Supercuts";

    fMatrix = NULL;
}

// --------------------------------------------------------------------------
//
Int_t MCT1SupercutsCalc::PreProcess(MParList *pList)
{
    MGeomCam *cam = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!cam)
    {
        *fLog << err << "MGeomCam (Camera Geometry) not found... aborting." << endl;
        return kFALSE;
    }

    fMm2Deg = cam->GetConvMm2Deg();

    fHadronness = (MHadronness*)pList->FindCreateObj("MHadronness", fHadronnessName);
    if (!fHadronness)
    {
        *fLog << err << fHadronnessName << " [MHadronness] not found... aborting." << endl;
        return kFALSE;
    }

    fSuper = (MCT1Supercuts*)pList->FindObject(fSuperName, "MCT1Supercuts");
    if (!fSuper)
    {
        *fLog << err << fSuperName << " [MCT1Supercuts] not found... aborting." << endl;
        return kFALSE;
    }

    if (fMatrix)
        return kTRUE;

    //-----------------------------------------------------------
    fHil = (MHillas*)pList->FindObject(fHilName, "MHillas");
    if (!fHil)
    {
        *fLog << err << fHilName << " [MHillas] not found... aborting." << endl;
        return kFALSE;
    }

    fHilExt = (MHillasExt*)pList->FindObject(fHilExtName, "MHillasExt");
    if (!fHilExt)
    {
        *fLog << err << fHilExtName << " [MHillasExt] not found... aborting." << endl;
        return kFALSE;
    }

    fHilSrc = (MHillasSrc*)pList->FindObject(fHilSrcName, "MHillasSrc");
    if (!fHilSrc)
    {
        *fLog << err << fHilSrcName << " [MHillasSrc] not found... aborting." << endl;
        return kFALSE;
    }

    fNewPar = (MNewImagePar*)pList->FindObject(fNewParName, "MNewImagePar");
    if (!fNewPar)
    {
        *fLog << err << fNewParName << " [MNewImagePar] not found... aborting." << endl;
        return kFALSE;
    }

    fMcEvt = (MMcEvt*)pList->FindObject("MMcEvt");
    if (!fMcEvt)
    {
        *fLog << err << "MMcEvt not found... aborting." << endl;
        return kFALSE;
    }


    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Calculation of upper and lower limits
//
Double_t MCT1SupercutsCalc::CtsMCut(const Double_t* a,  Double_t ls, Double_t ct,
                                    Double_t ls2, Double_t dd2) const
{
    // define cut-function
    //
    //    dNOMLOGSIZE = 4.1 (=log(60.0)
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

    //*fLog << "MCT1SupercutsCalc::CtsMCut; *a = "
    //      << *a     << ",  " << *(a+1) << ",  " << *(a+2) << ",  "
    //      << *(a+3) << ",  " << *(a+4) << ",  " << *(a+5) << ",  "
    //      << *(a+6) << ",  " << *(a+7) << endl;

    //*fLog << "MCT1SupercutsCalc::CtsMCut; ls, ls2, ct, dd2, limit = " << ls
    //      << ",  " << ls2 << ",  " << ct << ",  " << dd2 << ",  "
    //      << limit << endl;

    return limit;
}

// --------------------------------------------------------------------------
//
// Returns the mapped value from the Matrix
//
Double_t MCT1SupercutsCalc::GetVal(Int_t i) const
{

    Double_t val = (*fMatrix)[fMap[i]];


    //*fLog << "MCT1SupercutsCalc::GetVal; i, fMatrix, fMap, val = "
    //    << i << ",  " << fMatrix << ",  " << fMap[i] << ",  " << val << endl;


    return val;
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
void MCT1SupercutsCalc::InitMapping(MHMatrix *mat)
{
    if (fMatrix)
      return;

    fMatrix = mat;

    fMap[0] = fMatrix->AddColumn("MMcEvt.fTelescopeTheta");
    fMap[1] = fMatrix->AddColumn("MHillas.fWidth");
    fMap[2] = fMatrix->AddColumn("MHillas.fLength");
    fMap[3] = fMatrix->AddColumn("MHillas.fSize");
    fMap[4] = fMatrix->AddColumn("MHillas.fMeanX");
    fMap[5] = fMatrix->AddColumn("MHillas.fMeanY");
    fMap[6] = fMatrix->AddColumn("MHillasSrc.fDist");
    fMap[7] = fMatrix->AddColumn("fabs(MHillasSrc.fAlpha)");
    fMap[8] = fMatrix->AddColumn("sgn(MHillasSrc.fCosDeltaAlpha)*(MHillasExt.fM3Long)");
    fMap[9] = fMatrix->AddColumn("MNewImagePar.fConc");
    fMap[10]= fMatrix->AddColumn("MNewImagePar.fLeakage1");
}

// ---------------------------------------------------------------------------
//
// Evaluate dynamical supercuts 
// 
//          set hadronness to 0.25 if cuts are fullfilled
//                            0.75 otherwise
//
Int_t MCT1SupercutsCalc::Process()
{
    const Double_t kNomLogSize = 4.1;
    const Double_t kNomCosZA   = 1.0;

    const Double_t theta   = fMatrix ? GetVal(0) : fMcEvt->GetTelescopeTheta();
    const Double_t width0  = fMatrix ? GetVal(1) : fHil->GetWidth();
    const Double_t length0 = fMatrix ? GetVal(2) : fHil->GetLength();
    const Double_t size    = fMatrix ? GetVal(3) : fHil->GetSize();
    const Double_t meanx   = fMatrix ? GetVal(4) : fHil->GetMeanX();
    const Double_t meany   = fMatrix ? GetVal(5) : fHil->GetMeanY();
    const Double_t dist0   = fMatrix ? GetVal(6) : fHilSrc->GetDist();

    Double_t help;
    if (!fMatrix)
      help = TMath::Sign(fHilExt->GetM3Long(), 
	      		 fHilSrc->GetCosDeltaAlpha());
    const Double_t asym0   = fMatrix ? GetVal(8) : help;
    const Double_t conc    = fMatrix ? GetVal(9) : fNewPar->GetConc();
    const Double_t leakage = fMatrix ? GetVal(10): fNewPar->GetLeakage1();

    const Double_t newdist = dist0 * fMm2Deg;

    const Double_t dist2   = meanx*meanx + meany*meany;
    const Double_t dist    = sqrt(dist2) * fMm2Deg;
    const Double_t dd2     = dist*dist;


    const Double_t dmls    = log(size) - kNomLogSize;
    const Double_t dmls2   = dmls * dmls;

    const Double_t dmcza   = cos(theta) - kNomCosZA;

    const Double_t length  = length0 * fMm2Deg;
    const Double_t width   = width0  * fMm2Deg;
    const Double_t asym    = asym0   * fMm2Deg;

    /*
    *fLog << "newdist, length, width, asym, dist, conc, leakage = " 
          << newdist << ",  " << length << ",  " << width << ",  "
          << asym    << ",  " << dist   << ",  " << conc  << ",  " << leakage
          << endl;
  
    *fLog << "upper cuts in newdist, length, width, asym, dist, conc, leakage = " 
          << CtsMCut (fSuper->GetDistUp(),   dmls, dmcza, dmls2, dd2) << ",  "
          << CtsMCut (fSuper->GetDistLo(),   dmls, dmcza, dmls2, dd2) << ",  "

          << CtsMCut (fSuper->GetLengthUp(),   dmls, dmcza, dmls2, dd2) << ",  "
          << CtsMCut (fSuper->GetLengthLo(),   dmls, dmcza, dmls2, dd2) << ",  "

          << CtsMCut (fSuper->GetWidthUp(),   dmls, dmcza, dmls2, dd2) << ",  "
          << CtsMCut (fSuper->GetWidthLo(),   dmls, dmcza, dmls2, dd2) << ",  "

          << CtsMCut (fSuper->GetAsymUp(),   dmls, dmcza, dmls2, dd2) << ",  "
          << CtsMCut (fSuper->GetAsymLo(),   dmls, dmcza, dmls2, dd2) << ",  "

          << CtsMCut (fSuper->GetDistUp(),   dmls, dmcza, dmls2, dd2) << ",  "
          << CtsMCut (fSuper->GetDistLo(),   dmls, dmcza, dmls2, dd2) << ",  "

          << CtsMCut (fSuper->GetConcUp(),   dmls, dmcza, dmls2, dd2) << ",  "
          << CtsMCut (fSuper->GetConcLo(),   dmls, dmcza, dmls2, dd2) << ",  "

          << CtsMCut (fSuper->GetLeakage1Up(),   dmls, dmcza, dmls2, dd2) << ",  "
          << CtsMCut (fSuper->GetLeakage1Lo(),   dmls, dmcza, dmls2, dd2) << ",  "
          << endl;
    */


    if (
        //dist    < 1.05                                                     &&
        //newdist < 1.05                                                     &&

        newdist < CtsMCut (fSuper->GetDistUp(),   dmls, dmcza, dmls2, dd2) &&
        newdist > CtsMCut (fSuper->GetDistLo(),   dmls, dmcza, dmls2, dd2) &&

        length  < CtsMCut (fSuper->GetLengthUp(), dmls, dmcza, dmls2, dd2) &&
        length  > CtsMCut (fSuper->GetLengthLo(), dmls, dmcza, dmls2, dd2) &&

        width   < CtsMCut (fSuper->GetWidthUp(),  dmls, dmcza, dmls2, dd2) &&
        width   > CtsMCut (fSuper->GetWidthLo(),  dmls, dmcza, dmls2, dd2) &&

        asym    < CtsMCut (fSuper->GetAsymUp(),   dmls, dmcza, dmls2, dd2) &&
        asym    > CtsMCut (fSuper->GetAsymLo(),   dmls, dmcza, dmls2, dd2) &&

        dist    < CtsMCut (fSuper->GetDistUp(),   dmls, dmcza, dmls2, dd2) &&
        dist    > CtsMCut (fSuper->GetDistLo(),   dmls, dmcza, dmls2, dd2) &&

        conc    < CtsMCut (fSuper->GetConcUp(),   dmls, dmcza, dmls2, dd2) &&
        conc    > CtsMCut (fSuper->GetConcLo(),   dmls, dmcza, dmls2, dd2) &&

        leakage < CtsMCut (fSuper->GetLeakage1Up(),dmls, dmcza, dmls2, dd2) &&
        leakage > CtsMCut (fSuper->GetLeakage1Lo(),dmls, dmcza, dmls2, dd2)  ) 

      fHadronness->SetHadronness(0.25);
    else
      fHadronness->SetHadronness(0.75);

    //*fLog << "SChadroness = " << fHadronness->GetHadronness() << endl;

    fHadronness->SetReadyToSave();

    return kTRUE;
}









