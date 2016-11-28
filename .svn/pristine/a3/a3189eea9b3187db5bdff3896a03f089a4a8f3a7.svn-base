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
!   Author(s): Thomas Bretz, 03/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MFMagicCuts
//
//  Predefinitions:
//  ---------------
//      width/length = MHillas.fWidth/MHillas.fLength
//      area         = MHillas.GetArea*fMm2Deg*fMm2Deg
//      lgsize       = log10(MHillas.fSize)
//
//      leakage1     = MNewImagePar.fLeakage1
//
//      alpha        = MHillasSrc.fAlpha
//      dist         = MHillasSrc.fDist*fMm2Deg
//      m3long       = MHillasExt.fM3Long*sign(MHillasSrc.fCosDeltaAlpha)*fMm2Deg
//      slope        = MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)/fMm2Deg
//
//      antialpha    = MHillasSrcAnti.fAlpha
//      antidist     = MHillasSrcAnti.fDist*fMm2Deg
//      antim3long   = MHillasExt.fM3Long*sign(MHillasSrcAnti.fCosDeltaAlpha)*fMm2Deg
//      antislope    = MHillasExt.fSlopeLong*sign(MHillasSrcAnti.fCosDeltaAlpha)/fMm2Deg
//
//      had          = Hadronness.fVal
//
//  Coefficients/Cuts:
//  ------------------
//
//    c[0],  c[5], c[6], c[7],  c[8], c[9]:
//          xi          = c[0] + c[8]*slope + c[9]*(1-1/(1+c[10]*leak))
//          p           = xi*(1-width/length);
//          sign1       = m3long-c[5]
//          sign2       = (dist-c[7])*c[6]-slope
//          disp        = sign1<0 ||sign2<0 ? -disp : disp
//          thetasq     = disp^2 + dist^2 - 2*disp*dist*cos(alpha)
//
//    And the values with respect to the antisource position respectively.
//
//
//    c[1]:
//          thetasq < c[1]*c[1]
//
//    AreaCut:
//    c[2], c[3], c[4]:
//          A = c[2]*(1 - c[4]*(lgsize-c[3])*(lgsize-c[3]))
//          area < A
//
//    HadronnessCut:
//    c[13], c[14]:
//          had       <= c[13]
//          10^lgsize >= c[14]
//
//
//  Options:
//  --------
//
//   HadronnessCut:
//    - none/nocut: No area cut
//    - area:       Area cut <default>
//    - hadronness: Cut in size and hadronness (c[10], c[11])
//    - all:        area + hadronness
//
//   ThetaCut:
//    - none/nocut: no theta cut <default>
//    - on:         cut in theta only
//    - off:        anti-cut in anti-theta only
//    - wobble:     same as on|off (both)
//
//   CalcDisp:
//    - yes:        Disp is calculated as defined above <default>
//    - no:         abs(Disp.fVal) from the parameter list is used instead
//
//   CalcGhostbuster:
//    - yes:        The ghostbuster is calculated as defined above <default>
//    - no:         Ghostbuster.fVal<c[12] is used as ghostbusting condition
//
//
// Class Version 2:
// ----------------
//  + Bool_t fCalcDisp;        // Should we use Disp from the parameterlist?
//  + Bool_t fCalcGhostbuster; // Should we use Ghostbuster from the parameterlist?
//
//
//  Input Container:
//  ------
//   MGeomCam
//   MHillas
//   MHillasExt
//   MNewImagePar
//   MHillasSrc
//   [MHillasSrcAnti [MHillasSrc]]
//   [Disp [MParameterD]]
//   [Ghostbuster [MParameterD]]
//
//  Output:
//  -------
//   ThetaSquared [MParameterD]    // stores thetasq
//   Disp [MParameterD]            // stores -p*sign(MHillasSrc.fCosDeltaAlpha)
//   ThetaSquaredCut [MParameterD] // stores c[1]*c[1]
//
/////////////////////////////////////////////////////////////////////////////
#include "MFMagicCuts.h"

#include <fstream>
#include <errno.h> 

#include "MHMatrix.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MMath.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MHillas.h"
#include "MHillasSrc.h"
#include "MHillasExt.h"
#include "MNewImagePar.h"
#include "MParameters.h"

ClassImp(MFMagicCuts);

using namespace std;

// --------------------------------------------------------------------------
//
// constructor
//
MFMagicCuts::MFMagicCuts(const char *name, const char *title)
    : fGeom(0), fHil(0), fHilSrc(0), fHilAnti(0), fHilExt(0), fNewImgPar(0),
    fThetaSq(0), fDisp(0), fHadronness(0), fMatrix(0), fVariables(20),
    fThetaCut(kNone), fHadronnessCut(kArea), fCalcDisp(kTRUE),
    fCalcGhostbuster(kTRUE)
{
    fName  = name  ? name  : "MFMagicCuts";
    fTitle = title ? title : "Class to evaluate the MagicCuts";

    fMatrix = NULL;

    AddToBranchList("MHillas.fWidth");
    AddToBranchList("MHillas.fLength");
    AddToBranchList("MHillas.fSize");
    AddToBranchList("MHillasSrc.fAlpha");
    AddToBranchList("MHillasSrc.fDist");
    AddToBranchList("MHillasSrc.fCosDeltaAlpha");
    AddToBranchList("MHillasSrcAnti.fAlpha");
    AddToBranchList("MHillasSrcAnti.fDist");
    AddToBranchList("MHillasSrcAnti.fCosDeltaAlpha");
    AddToBranchList("MHillasExt.fM3Long");
    AddToBranchList("MHillasExt.fSlopeLong");
    AddToBranchList("MNewImagePar.fLeakage1");
    AddToBranchList("Hadronness.fVal");
    AddToBranchList("Disp.fVal");
    AddToBranchList("Ghostbuster.fVal");
}

// --------------------------------------------------------------------------
//
// The theta cut value GetThetaSqCut() is propageted to the parameter list
// as 'ThetaSquaredCut' as MParameterD so that it can be used somewhere else.
//
Int_t MFMagicCuts::PreProcess(MParList *pList)
{
    fGeom = (MGeomCam*)pList->FindObject("MGeomCam");
    if (!fGeom)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    fThetaSq = (MParameterD*)pList->FindCreateObj("MParameterD", "ThetaSquared");
    if (!fThetaSq)
        return kFALSE;

    if (!fCalcDisp)
        fDisp = (MParameterD*)pList->FindObject("Disp", "MParameterD");
    else
        fDisp = (MParameterD*)pList->FindCreateObj("MParameterD", "Disp");
    if (!fDisp)
    {
        *fLog << err << "Disp [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }

    if (!fCalcGhostbuster)
        fGhostbuster = (MParameterD*)pList->FindObject("Ghostbuster", "MParameterD");
    else
        fGhostbuster = (MParameterD*)pList->FindCreateObj("MParameterD", "Ghostbuster");
    if (!fGhostbuster)
    {
        *fLog << err << "Ghostbuster [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }

    // propagate Theta cut to the parameter list
    // so that it can be used somewhere else
    MParameterD *thetacut = (MParameterD*)pList->FindCreateObj("MParameterD", "ThetaSquaredCut");
    if (!thetacut)
        return kFALSE;
    thetacut->SetVal(GetThetaSqCut());

    Print();

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

    fNewImgPar = (MNewImagePar*)pList->FindObject("MNewImagePar");
    if (!fNewImgPar)
    {
        *fLog << err << "MNewImagePar not found... aborting." << endl;
        return kFALSE;
    }

    fHilSrc = (MHillasSrc*)pList->FindObject("MHillasSrc");
    if (!fHilSrc)
    {
        *fLog << err << "MHillasSrc not found... aborting." << endl;
        return kFALSE;
    }

    if (fThetaCut&kOff)
    {
        fHilAnti = (MHillasSrc*)pList->FindObject("MHillasSrcAnti", "MHillasSrc");
        if (!fHilAnti)
        {
            *fLog << warn << "MHillasSrcAnti [MHillasSrc] not found... aborting." << endl;
            return kFALSE;
        }
    }

    if (fHadronnessCut&kHadronness)
    {
        fHadronness = (MParameterD*)pList->FindObject("Hadronness", "MParameterD");
        if (!fHadronness)
        {
            *fLog << warn << "Hadronness [MParameterD] not found... aborting." << endl;
            return kFALSE;
        }
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Returns the mapped value from the Matrix
//
Double_t MFMagicCuts::GetVal(Int_t i) const
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
void MFMagicCuts::InitMapping(MHMatrix *mat)
{
    if (fMatrix)
      return;

    fMatrix = mat;

//    fMap[kELength] = fMatrix->AddColumn("MHillas.fLength*MGeomCam.fConvMm2Deg");
//    fMap[kEWidth]  = fMatrix->AddColumn("MHillas.fWidth*MGeomCam.fConvMm2Deg");

    fMap[kEWdivL]   = fMatrix->AddColumn("MHillas.fWidth/MHillas.fLength");
    fMap[kESize]    = fMatrix->AddColumn("log10(MHillas.fSize)");
    fMap[kEArea]    = fMatrix->AddColumn("MHillas.GetArea*MGeomCam.fConvMm2Deg*MGeomCam.fConvMm2Deg");

    //fMap[kELeakage] = fMatrix->AddColumn("log10(MNewImagePar.fLeakage1+1)");
    fMap[kELeakage] = fMatrix->AddColumn("MNewImagePar.fLeakage1");

    fMap[kEAlpha]   = fMatrix->AddColumn("MHillasSrc.fAlpha");
    fMap[kEDist]    = fMatrix->AddColumn("MHillasSrc.fDist*MGeomCam.fConvMm2Deg");
    fMap[kEM3Long]  = fMatrix->AddColumn("MHillasExt.fM3Long*sign(MHillasSrc.fCosDeltaAlpha)*MGeomCam.fConvMm2Deg");

    fMap[kESign]    = fMatrix->AddColumn("sign(MHillasSrc.fCosDeltaAlpha)");

    fMap[kESlope]   = fMatrix->AddColumn("MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)/MGeomCam.fConvMm2Deg");

    if (!fCalcDisp)
        fMap[kEDisp] = fMatrix->AddColumn("abs(Disp.fVal)");

    if (!fCalcGhostbuster)
        fMap[kEGhostbuster] = fMatrix->AddColumn("Ghostbuster.fVal");

    if (fThetaCut&kOff)
    {
        fMap[kEAlphaAnti]  = fMatrix->AddColumn("MHillasSrcAnti.fAlpha");
        fMap[kEDistAnti]   = fMatrix->AddColumn("MHillasSrcAnti.fDist*MGeomCam.fConvMm2Deg");
        fMap[kEM3LongAnti] = fMatrix->AddColumn("MHillasExt.fM3Long*sign(MHillasSrcAnti.fCosDeltaAlpha)*MGeomCam.fConvMm2Deg");
        fMap[kESlopeAnti]  = fMatrix->AddColumn("MHillasExt.fSlopeLong*sign(MHillasSrcAnti.fCosDeltaAlpha)/MGeomCam.fConvMm2Deg");
    }

    if (fHadronnessCut&kHadronness)
        fMap[kEHadronness] = fMatrix->AddColumn("Hadronness.fVal");
}

// --------------------------------------------------------------------------
//
// Returns theta squared based on the formula:
//    p := c*(1-width/length)
//    return d*d + p*p - 2*d*p*cos(a*TMath::DegToRad());
//
// If par!=NULL p is stored in this MParameterD
//
Double_t MFMagicCuts::GetThetaSq(Double_t p, Double_t d, Double_t a) const
{
    // wl = width/l [1]
    // d  = dist    [deg]
    // a  = alpha   [deg]
    return d*d + p*p - 2*d*p*TMath::Cos(a*TMath::DegToRad());
}

// --------------------------------------------------------------------------
//
// Returns squared cut value in theta: fVariables[1]^2
//
Double_t MFMagicCuts::GetThetaSqCut() const
{
    return fVariables[1]*fVariables[1];
}

// --------------------------------------------------------------------------
//
// Return abs(Disp.fVal) if disp calculation is switched off.
// Otherwise return (c0+c6*leak^c7)*(1-width/length)
//
Double_t MFMagicCuts::GetDisp(Double_t slope, Double_t lgsize) const
{
    if (!fCalcDisp)
        return fMatrix ? GetVal(kEDisp) : TMath::Abs(fDisp->GetVal());

    // Get some variables
    const Double_t wdivl = fMatrix ? GetVal(kEWdivL)   : fHil->GetWidth()/fHil->GetLength();
    //const Double_t leak  = fMatrix ? GetVal(kELeakage) : TMath::Log10(fNewImgPar->GetLeakage1()+1);
    const Double_t leak  = fMatrix ? GetVal(kELeakage) : fNewImgPar->GetLeakage1();

    // For simplicity
    const Double_t *c = fVariables.GetArray();

    // As rule for root or MDataPhrase:
    //   ((M[3]>[3])*[4]*(M[3]-[3])^2 + [2]*M[2] + [1]*M[1] + [0])*M[0]
    //char *r = "M[0]*([0] + [1]*M[1] + [2]*(1-1/(1+[3]*M[2])))";
    //
    Double_t xi = c[0] + c[8]*slope + c[9]*(1-1/(1+c[10]*leak));
//    if (lgsize>c[10])
//        xi += (lgsize-c[10])*(lgsize-c[10])*c[11];

    const Double_t disp = xi*(1-wdivl);

    return disp;
}

Bool_t MFMagicCuts::IsGhost(Double_t m3long, Double_t slope, Double_t dist) const
{
    // For simplicity
    const Double_t *c = fVariables.GetArray();

    if (!fCalcGhostbuster)
        return (fMatrix ? GetVal(kEGhostbuster) : fGhostbuster->GetVal())<c[12];

    // Do Ghostbusting with slope and m3l
    const Bool_t sign1 = m3long < c[5];
    const Bool_t sign2 = slope  > (dist-c[7])*c[6];

    return sign1 || sign2;
}

// ---------------------------------------------------------------------------
//
// Evaluate dynamical magic-cuts
//
Int_t MFMagicCuts::Process()
{
    // For simplicity
    const Double_t *c = fVariables.GetArray();

    const Float_t fMm2Deg = fGeom->GetConvMm2Deg();

    // Default if we return before the end
    fResult = kFALSE;

    // Value for Theta cut (Disp parametrization)
    const Double_t cut  = GetThetaSqCut();

    // ------------------- Most efficient cut -----------------------
    // ---------------------- Theta cut ON --------------------------
    const Double_t dist   = fMatrix ? GetVal(kEDist)   : fHilSrc->GetDist()*fMm2Deg;
    const Double_t alpha  = fMatrix ? GetVal(kEAlpha)  : fHilSrc->GetAlpha();
    const Double_t m3long = fMatrix ? GetVal(kEM3Long) : fHilExt->GetM3Long()*TMath::Sign(fMm2Deg, fHilSrc->GetCosDeltaAlpha());
    const Double_t slope  = fMatrix ? GetVal(kESlope)  : fHilExt->GetSlopeLong()/TMath::Sign(fMm2Deg, fHilSrc->GetCosDeltaAlpha());
    const Double_t sign   = fMatrix ? GetVal(kESign)   : MMath::Sgn(fHilSrc->GetCosDeltaAlpha());
    const Double_t lgsize = fMatrix ? GetVal(kESize)   : TMath::Log10(fHil->GetSize());

    // Calculate disp including sign
    const Double_t disp  = GetDisp(slope, lgsize);
    const Double_t ghost = IsGhost(m3long, slope, dist);

    const Double_t p = ghost ? -disp : disp;

    // Align sign of disp along shower axis like m3long
    fDisp->SetVal(-p*sign);

    // FIXME: Allow to use ThetaSq from INPUT!

    // Calculate corresponding Theta^2
    const Double_t thetasq = GetThetaSq(p, dist, alpha);
    fThetaSq->SetVal(thetasq);

    // Perform theta cut
    if (fThetaCut&kOn && thetasq >= cut)
        return kTRUE;

    // --------------------- Efficient  cut -------------------------
    // ---------------------- Area/M3l cut --------------------------
    if (fHadronnessCut&kArea)
    {
        const Double_t area = fMatrix ? GetVal(kEArea) : fHil->GetArea()*fMm2Deg*fMm2Deg;
        const Double_t A    = (c[2]*c[4]>0 ? (lgsize>c[3]) : (lgsize<c[3])) ?
            c[2] : c[2]*(1 - c[4]*(lgsize-c[3])*(lgsize-c[3]));
        //const Double_t A    = lgsize>c[3] ? c[2] : c[2] - c[4]/c[2]*(lgsize-c[3])*(lgsize-c[3]));
        if (area>=A)
            return kTRUE;
    }

    if (fHadronnessCut&kAreaLin)
    {
        const Double_t area = fMatrix ? GetVal(kEArea) : fHil->GetArea()*fMm2Deg*fMm2Deg;
        const Double_t A    = c[2] + c[4]*(lgsize-c[3]);
        if (area>=A)
            return kTRUE;
    }

    // ---------------------------------------------------------------
    // ---------------------------------------------------------------

    if (fHadronnessCut&kHadronness)
    {
        const Double_t had = fMatrix ? GetVal(kEHadronness) : fHadronness->GetVal();
        if (had>c[13])
            return kTRUE;

        if (TMath::Power(10, lgsize)<c[14])
            return kTRUE;
    }

    // ------------------- Least efficient cut -----------------------
    // ---------------------- Theta cut OFF --------------------------

    if (fThetaCut&kOff)
    {
        const Double_t distanti    = fMatrix ? GetVal(kEDistAnti)   : fHilAnti->GetDist()*fMm2Deg;
        const Double_t alphaanti   = fMatrix ? GetVal(kEAlphaAnti)  : fHilAnti->GetAlpha();
        const Double_t m3lanti     = fMatrix ? GetVal(kEM3LongAnti) : fHilExt->GetM3Long()*TMath::Sign(fMm2Deg, fHilAnti->GetCosDeltaAlpha());
        const Double_t slopeanti   = fMatrix ? GetVal(kESlopeAnti)  : fHilExt->GetSlopeLong()/TMath::Sign(fMm2Deg, fHilAnti->GetCosDeltaAlpha());

        const Double_t dispanti  = GetDisp(slopeanti, lgsize);
        const Double_t ghostanti = IsGhost(m3lanti, slopeanti, lgsize);

        const Double_t panti = ghostanti ? -dispanti : dispanti;

        // Align disp along source direction depending on third moment
        const Double_t thetasqanti = GetThetaSq(panti, distanti, alphaanti);

        if (thetasqanti<cut)
            return kTRUE;

        // This cut throws away gamma-like events which would be assigned to the
        // anti-source. It is not necessary but it offers the possibility to
        // fill the MHDisp plot in one run (Having a hole instead of eg. Crab
        // the data can be rotated and subtracted)
    }

    fResult = kTRUE;

    return kTRUE;
}

void MFMagicCuts::SetVariables(const TArrayD &arr)
{
    fVariables = arr;
}

TString MFMagicCuts::GetParam(Int_t i) const
{
    if (i>=fVariables.GetSize())
        return "";

    return Form("Param[%2d]=%+f", i, fVariables[i]);
}

void MFMagicCuts::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << ":" << setiosflags(ios::left) << endl;

    *fLog << "Using Theta cut: ";
    switch (fThetaCut)
    {
    case kNone:
        *fLog << "none" << endl;
        break;
    case kOn:
        *fLog << "on" << endl;
        break;
    case kOff:
        *fLog << "off" << endl;
        break;
    case kWobble:
        *fLog << "on and off (wobble)" << endl;
        break;
    }
    *fLog << "Using hadronness cut: ";
    switch (fHadronnessCut)
    {
    case kNoCut:
        *fLog << "none" << endl;
        break;
    case kArea:
        *fLog << "area" << endl;
        break;
    case kHadronness:
        *fLog << "hadronness" << endl;
        break;
    case kAreaLin:
        *fLog << "arealin" << endl;
        break;
    case kAll:
        *fLog << "all" << endl;
        break;
    }
    if (fCalcDisp)
        *fLog << "Disp is calculated from c0, c8-c11." << endl;
    else
        *fLog << "Disp.fVal from the parameter list is used as disp." << endl;
    if (fCalcGhostbuster)
        *fLog << "Ghostbusting is calculated from c5-c7." << endl;
    else
        *fLog << "Ghostbuster.fVal from the parameter list is used for ghostbusting." << endl;

    *fLog << "Filter is" << (IsInverted()?"":" not") << " inverted." << endl;

    const Int_t n = fVariables.GetSize();
    for (int i=0; i<n; i+=3)
    {
        *fLog << setw(25) << GetParam(i);
        *fLog << setw(25) << GetParam(i+1);
        *fLog << setw(25) << GetParam(i+2);
        *fLog << endl;
    }
    *fLog << setiosflags(ios::right);
}

Bool_t MFMagicCuts::CoefficentsRead(const char *fname)
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

Bool_t MFMagicCuts::CoefficentsWrite(const char *fname) const
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

Int_t MFMagicCuts::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    if (MFilter::ReadEnv(env, prefix, print)==kERROR)
        return kERROR;


    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "ThetaCut", print))
    {
        TString str = GetEnvValue(env, prefix, "ThetaCut", "");
        str.ToLower();
        str=str.Strip(TString::kBoth);

        if (str==(TString)"nocut" || str==(TString)"none")
            fThetaCut = kNone;
        if (str==(TString)"on")
            fThetaCut = kOn;
        if (str==(TString)"off")
            fThetaCut = kOff;
        if (str==(TString)"wobble")
            fThetaCut = kWobble;
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "HadronnessCut", print))
    {
        TString str = GetEnvValue(env, prefix, "HadronnessCut", "");
        str.ToLower();
        str=str.Strip(TString::kBoth);

        if (str==(TString)"nocut" || str==(TString)"none")
            fHadronnessCut = kNoCut;
        if (str==(TString)"area")
            fHadronnessCut = kArea;
        if (str==(TString)"hadronness")
            fHadronnessCut = kHadronness;
        if (str==(TString)"all")
            fHadronnessCut = kAll;

        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "CalcDisp", print))
    {
        fCalcDisp = GetEnvValue(env, prefix, "CalcDisp", fCalcDisp);
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "CalcGhostbuster", print))
    {
        fCalcGhostbuster = GetEnvValue(env, prefix, "CalcGhostbuster", fCalcGhostbuster);
        rc = kTRUE;
    }

    if (IsEnvDefined(env, prefix, "File", print))
    {
        const TString fname = GetEnvValue(env, prefix, "File", "");
        if (!CoefficentsRead(fname))
            return kERROR;

        return kTRUE;
    }

    for (int i=0; i<fVariables.GetSize(); i++)
    {
        if (IsEnvDefined(env, prefix, Form("Param%d", i), print))
        {
            // It is important that the last argument is a floating point
            fVariables[i] = GetEnvValue(env, prefix, Form("Param%d", i), 0.0);
            rc = kTRUE;
        }
    }
    return rc;
}
