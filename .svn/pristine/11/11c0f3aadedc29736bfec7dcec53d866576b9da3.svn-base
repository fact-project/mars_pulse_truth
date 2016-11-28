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
!   Author(s): Thomas Bretz, 5/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MHThetaSq
//
// This is a MHAlpha using "ThetaSquared [MParameterD]" as 'alpha'
//
// The default binning is determined from the integration range set in
// MAlphaFitter.
//
// For more detailes see MHAlpha.
//
// Version 2:
// ---------
//  + UInt_t fNumBinsSignal;
//  + UInt_t fNumBinsTotal;
//
//////////////////////////////////////////////////////////////////////////////
#include "MHThetaSq.h"

#include "MParameters.h"
#include "MHMatrix.h"

#include "MBinning.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MParameters.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHThetaSq);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor
//
MHThetaSq::MHThetaSq(const char *name, const char *title)
    : MHAlpha(name, title), fNumBinsSignal(3), fNumBinsTotal(75)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHThetaSq";
    fTitle = title ? title : "Theta Squared plot";

    fNameParameter = "ThetaSquared";

    fHist.SetName("Theta");
    fHist.SetTitle("Theta");
    fHist.SetZTitle("\\vartheta^{2} [deg^{2}]");
    fHist.SetDirectory(NULL);

    // Main histogram
    fHistTime.SetName("Theta");
    fHistTime.SetXTitle("\\vartheta^{2} [deg^{2}]");
    fHistTime.SetDirectory(NULL);

    //fHistTime.SetYTitle("\\theta^{2] [deg^{2}]");
    /*
    TArrayD arr(50);
    for (int i=1; i<50; i++)
        arr[i] = sqrt((arr[i-1]+1)*(arr[i-1]+1) + 1.1)-1;
    */
    MBinning binsa, binse, binst;
    binsa.SetEdges(75, 0, 1.5);
    //binsa.SetEdges(arr);
    binse.SetEdgesLog(15, 10, 100000);
    binst.SetEdgesASin(67, -0.005, 0.665);
    binsa.Apply(fHistTime);

    MH::SetBinning(fHist, binst, binse, binsa);
}

// --------------------------------------------------------------------------
//
// Overwrites the binning in Alpha (ThetaSq) with a binning for which
// the upper edge of the 5th bin (bin=5) fit the signal integration window.
// In total 75 bins are setup.
//
// In case of fOffData!=NULL the binnings are taken later from fOffData anyhow.
//
Bool_t MHThetaSq::SetupFill(const MParList *pl)
{
    // Default is from default fitter
    // if a user defined fitter is in the parlist: use this range
    MAlphaFitter *fit = (MAlphaFitter*)pl->FindObject("MAlphaFitter");
    if (!fit)
        fit = &fFit;

    MParameterD *cut = (MParameterD*)pl->FindObject("ThetaSquaredCut", "MParameterD");
    if (cut)
        fit->SetSignalIntegralMax(cut->GetVal());

    // Get Histogram binnings
    MBinning binst, binse;
    binst.SetEdges(fHist, 'x');
    binse.SetEdges(fHist, 'y');

    // Calculate bining which fits alpha-cut
    const Double_t intmax = fit->GetSignalIntegralMax();
    const UInt_t   nbins  = fNumBinsTotal;
    const UInt_t   nsig   = fNumBinsSignal;

    MBinning binsa(nbins, 0, nbins*intmax/nsig);

    // Apply binning
    binsa.Apply(fHistTime);
    MH::SetBinning(fHist, binst, binse, binsa);

    // Remark: Binnings might be overwritten in MHAlpha::SetupFill
    return MHAlpha::SetupFill(pl);
}

// --------------------------------------------------------------------------
//
// Store the pointer to the parameter container storing the plotted value
// (here ThetaSq) in fParameter.
//
// return whether it was found or not.
//
Bool_t MHThetaSq::GetParameter(const MParList &pl)
{
    fParameter = (MParContainer*)pl.FindObject(fNameParameter, "MParameterD");
    if (fParameter)
        return kTRUE;

    *fLog << err << fNameParameter << " [MParameterD] not found... abort." << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Return the value from fParemeter which should be filled into the plots
//
Double_t MHThetaSq::GetVal() const
{
    return static_cast<const MParameterD*>(fParameter)->GetVal();
}

// --------------------------------------------------------------------------
//
// You can use this function if you want to use a MHMatrix instead of
// MMcEvt. This function adds all necessary columns to the
// given matrix. Afterward you should fill the matrix with the corresponding
// data (eg from a file by using MHMatrix::Fill). If you now loop
// through the matrix (eg using MMatrixLoop) MHHadronness::Fill
// will take the values from the matrix instead of the containers.
//
// It takes fSkipHist* into account!
//
void MHThetaSq::InitMapping(MHMatrix *mat, Int_t type)
{
    if (fMatrix)
        return;

    fMatrix = mat;

    fMap[0] = -1;
    fMap[1] = -1;
    fMap[2] = -1;
    fMap[3] = -1;
    fMap[4] = -1;

    if (!fSkipHistEnergy)
    {
        fMap[1] = type==0 ? fMatrix->AddColumn("MEnergyEst.fVal") : -1;
        fMap[2] = type==0 ? -1 : fMatrix->AddColumn("MHillas.fSize");
    }

    if (!fSkipHistTheta)
        fMap[3] = fMatrix->AddColumn("MPointingPos.fZd");

   // if (!fSkipHistTime)
   //     fMap[4] = fMatrix->AddColumn("MTime.GetAxisTime");
}

Int_t MHThetaSq::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Int_t rc = MHAlpha::ReadEnv(env, prefix, print);
    if (rc==kERROR)
        return kERROR;

    if (IsEnvDefined(env, prefix, "NumBinsSignal", print))
    {
        SetNumBinsSignal(GetEnvValue(env, prefix, "NumBinsSignal", (Int_t)fNumBinsSignal));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "NumBinsTotal", print))
    {
        SetNumBinsTotal(GetEnvValue(env, prefix, "NumBinsTotal", (Int_t)fNumBinsTotal));
        rc = kTRUE;
    }
    return rc;
}
