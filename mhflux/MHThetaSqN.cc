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
!   Author(s): Thomas Bretz, 5/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MHThetaSqN
//
// For more detailes see MHAlpha.
//
// How to use this class:
//  - in your ganymed.rc add:
//      MJCut.NameHist: MHThetaSqN
//  - setup the number of off-source regions in your ganymed.rc by:
//      MHThetaSqN.NumOffSourcePos: 2
//  - switch on/off whether an off-theta cut should be done:
//      MHThetaSqN.DoOffCut: Yes,No
//  - and if necessary switch off the Theta cut in your Magic cuts:
//      Cut1.ThetaCut: None
//
//////////////////////////////////////////////////////////////////////////////
#include "MHThetaSqN.h"

#include <TVector2.h>

#include "MParameters.h"
#include "MSrcPosCam.h"
#include "MGeomCam.h"
#include "MHMatrix.h"
#include "MHillas.h"

#include "MBinning.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MParameters.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHThetaSqN);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor
//
MHThetaSqN::MHThetaSqN(const char *name, const char *title)
    : MHAlpha(name, title), fDisp(0), fSrcPosCam(0), fSignificanceCutLevel(1.7),
    fNumBinsSignal(3), fNumBinsTotal(75), fNumOffSourcePos(3), fDoOffCut(kTRUE),
    fCounter(4)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHThetaSqN";
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

    MBinning binsa, binse, binst;
    binsa.SetEdges(75, 0, 1.5);
    //binsa.SetEdges(arr);
    binse.SetEdgesLog(15, 10, 100000);
    binst.SetEdgesASin(67, -0.005, 0.665);
    binsa.Apply(fHistTime);

    MH::SetBinning(fHist, binst, binse, binsa);

    fParameter = new MParameterD;
}

MHThetaSqN::~MHThetaSqN()
{
    delete fParameter;
}

// --------------------------------------------------------------------------
//
// Overwrites the binning in Alpha (ThetaSq) with a binning for which
// the upper edge of the 5th bin (bin=5) fit the signal integration window.
// In total 75 bins are setup.
//
// In case of fOffData!=NULL the binnings are taken later from fOffData anyhow.
//
Bool_t MHThetaSqN::SetupFill(const MParList *pl)
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
    if (!MHAlpha::SetupFill(pl))
        return kFALSE;

    fDisp = (MParameterD*)pl->FindObject("Disp", "MParameterD");
    if (!fDisp)
    {
        *fLog << err << "Disp [MParameterD] not found... abort." << endl;
        return kFALSE;
    }
    fHillas = (MHillas*)pl->FindObject("MHillas");
    if (!fHillas)
    {
        *fLog << err << "MHillas not found... abort." << endl;
        return kFALSE;
    }
    fSrcPosCam = (MSrcPosCam*)pl->FindObject("MSrcPosCam");
    if (!fSrcPosCam)
    {
        *fLog << err << "MSrcPosCam not found... abort." << endl;
        return kFALSE;
    }

    fGeom = (MGeomCam*)pl->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "MGeomCam not found... abort." << endl;
        return kFALSE;
    }

    if (fFit.GetScaleMode()==MAlphaFitter::kNone)
        fFit.SetScaleUser(1./fNumOffSourcePos);

    fThetaSqCut = fSignificanceCutLevel*fFit.GetSignalIntegralMax()/1.7;

    fCounter.Reset();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Return the value from fParemeter which should be filled into the plots
//
Double_t MHThetaSqN::GetVal() const
{
    return static_cast<const MParameterD*>(fParameter)->GetVal();
}

// --------------------------------------------------------------------------
//
// Abbreviation to set the value used by MHAlpha to fill the histogram
//
void MHThetaSqN::SetVal(Double_t val)
{
    static_cast<MParameterD*>(fParameter)->SetVal(val);
}

// --------------------------------------------------------------------------
//
// Abbreviation to set the value used by MHAlpha to fill the histogram
//
TVector2 MHThetaSqN::GetVec(const TVector2 &v, Int_t n1) const
{
    if (!fMatrix)
        return v;

    return TVector2( (*fMatrix)[fMap[n1]], (*fMatrix)[fMap[n1+1]] );
}

Int_t MHThetaSqN::Fill(const MParContainer *par, const Stat_t weight)
{
    const Double_t fMm2Deg = fGeom->GetConvMm2Deg();

    const TVector2 norm(GetVec(fHillas->GetNormAxis(), 6));
    const TVector2 mean(GetVec(fHillas->GetMean(),     8));

    const TVector2 org = mean*fMm2Deg + norm*fDisp->GetVal();

    // In the case we are filling off-data src0 contains the anti-source position
    TVector2 src0(GetVec(fSrcPosCam->GetXY(), 10)*fMm2Deg);
    if (!fOffData)
        src0 *= -1;

    const UInt_t  n   = fNumOffSourcePos+1;
    const Float_t rad = TMath::TwoPi()/n;

    // Calculate distance (theta-sq) to all (off-)source regions
    TArrayD dist(n);
    for (UInt_t i=0; i<n; i++)
    {
        const TVector2 src = const_cast<TVector2&>(src0).Rotate(i*rad);
        dist[i] = (src-org).Mod2();
    }

    // Processing off-data
    // Check if event's origin is in the on-regions
    if (!fOffData && fDoOffCut && dist[0]<fThetaSqCut)
        return kTRUE;

    for (UInt_t i=0; i<n; i++)
    {
        //   off: is in src region   on: is in off regions
        /// if (!fOffData && i==0) || (fOffData && i!=0)
        if ((bool)fOffData ^ (i==0) )
            continue;

        Stat_t w = weight;

        // Processing on-data
        if (fOffData && fDoOffCut)
        {
            /*
             static int cnt=0;
             if (dist[1+(cnt++%fNumOffSourcePos)]<fFit.GetSignalIntegralMax())
                continue;
             */

            // Check if event's origin is in one of the off-regions
            for (UInt_t j=1; j<n; j++)
                if (dist[j]<fThetaSqCut)
                {
                    w *= (float)(fNumOffSourcePos-1)/fNumOffSourcePos;
                    break;
                }
        }

        SetVal(dist[i]);

        const Int_t rc = MHAlpha::Fill(NULL, w);
        if (rc!=kTRUE)
            return rc;
    }

    if (!fOffData)
    {
        // off    0.4deg    0.3deg
        //  5: 0.200  ~86%   0.150
        //  4: 0.235  ~92%   0.176
        //  3: 0.283  ~96%   0.212
        //  2: 0.346  ~98%   0.260

        const Double_t dis = src0.Mod()*TMath::Sin(rad/2);
        const Double_t cut = TMath::Sqrt(fFit.GetSignalIntegralMax());
        if (dis<cut)
        {
            *fLog << warn << "WARNING - Source regions overlap: distance ";
            *fLog << dis << " less than theta-sq cut " << cut << "!" << endl;
            if (dis*1.7<cut*1.0)
                fCounter[3]++;
            else
                if (dis*1.7<cut*1.5)
                    fCounter[2]++;
                else
                    fCounter[1]++;

        }
        else
            fCounter[0]++;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print execution statistics
//
Bool_t MHThetaSqN::Finalize()
{
    if (GetNumExecutions()>0)
    {
        *fLog << inf << endl;
        *fLog << GetDescriptor() << " execution statistics:" << endl;
        PrintSkipped(fCounter[3], "Region distance below 1.0sigma (68.3%)");
        PrintSkipped(fCounter[2], "Region distance below 1.5sigma (86.6%)");
        PrintSkipped(fCounter[1], "Region distance below 1.7sigma (91.1%)");
        *fLog << " " << (int)fCounter[0] << " (" << Form("%5.1f", 100.*fCounter[0]/GetNumExecutions()) << "%) Evts ok!" << endl;
        //*fLog << " " << (int)fCounter[0] << " (" << Form("%5.1f", 100.*fCounter[0]/GetNumExecutions()) << "%) Evts survived calculation!" << endl;
        *fLog << endl;
    }

    return MHAlpha::Finalize();
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
void MHThetaSqN::InitMapping(MHMatrix *mat, Int_t type)
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

    fMap[5]  = -1;
    fMap[6]  = fMatrix->AddColumn("MHillas.fCosDelta");
    fMap[7]  = fMatrix->AddColumn("MHillas.fSinDelta");
    fMap[8]  = fMatrix->AddColumn("MHillas.fMeanX");
    fMap[9]  = fMatrix->AddColumn("MHillas.fMeanY");
    fMap[10] = fMatrix->AddColumn("MSrcPosCam.fX");
    fMap[11] = fMatrix->AddColumn("MSrcPosCam.fY");
}

Int_t MHThetaSqN::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
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
    if (IsEnvDefined(env, prefix, "NumOffSourcePos", print))
    {
        SetNumOffSourcePos(GetEnvValue(env, prefix, "NumOffSourcePos", (Int_t)fNumOffSourcePos));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "DoOffCut", print))
    {
        SetDoOffCut(GetEnvValue(env, prefix, "DoOffCut", fDoOffCut));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "SignificanceCutLevel", print))
    {
        SetSignificanceCutLevel(GetEnvValue(env, prefix, "SignificanceCutLevel", fSignificanceCutLevel));
        rc = kTRUE;
    }
    return rc;
}
