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
!   Author(s): R.K.Bock 11/2003 <mailto:rkb@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MFGeomag
//
//  A filter to reject Monte Carlo events based on phi/theta/charge of the
//  incident particle. Uses tables calculated by Adrian Biland, which contain
//  three parameters, used with rigidity (= particle momentum / charge) :
//         rig < min_rig:   reject unconditionally
//         rig > max_rig:   accept unconditionally
//         rig in between:  reject it with 'probability'
//  the two tables, for + and - rigidity, are stored in ASCII form in mfilter/
//
/////////////////////////////////////////////////////////////////////////////
#include "MFGeomag.h"

#include <fstream>        //for ifstream

#include <TRandom.h>      //for gRandom
#include <TSystem.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MMcEvt.hxx"

ClassImp(MFGeomag);

using namespace std;

// --------------------------------------------------------------------------
//
MFGeomag::MFGeomag(const char *name, const char *title) : fMcEvt(NULL)
{
    fName  = name  ? name  : "MFGeomag";
    fTitle = title ? title : "Filter using geomagnetic field";

    fGammaElectron = kFALSE;  // logical variable, will not take gammas as electrons (default)

    AddToBranchList("MMcEvt.fPartId");
}

// --------------------------------------------------------------------------
//
Int_t MFGeomag::PreProcess(MParList *pList)
{
    //  reading of tables (variables defined as 'private')
    TString marssys(gSystem->Getenv("MARSSYS"));
    if (!marssys.IsNull() && !marssys.EndsWith("/"))
        marssys += "/";

    //
    // Read gcminus.txt
    //
    TString filename(marssys);
    filename += "mfilter/gcplus.txt";

    ifstream geomagp(filename);

    if (!geomagp)
    {
        *fLog << err << "ERROR - file " << filename << " not found." << endl;
        return kFALSE;
    }
    for (int i=0; i<1152; i++)
    {
        Float_t dummy;
        geomagp >> dummy >> dummy >> fRigMin[i] >> fRigMax[i] >> fProb[i];
    }
    *fLog << inf << endl;
    *fLog << "gcplus.txt - first line: ";
    *fLog << Form ("FRigMin=%8f  fRigMax=%8f  fProb=%8f",
                   fRigMin[0], fRigMax[0], fProb[0]) << endl;

    //
    // Read gcminus.txt
    //
    filename = marssys;
    filename += "mfilter/gcminus.txt";

    ifstream geomagm(filename);
    if (!geomagm)
    {
        *fLog << err << "ERROR - file " << filename << " not found." << endl;
        return kFALSE;
    }
    for (int i=1152; i<2304; i++)
    {
        Float_t dummy;
        geomagm >> dummy >> dummy >> fRigMin[i] >> fRigMax[i] >> fProb[i];
    }
    *fLog << "gcminus.txt - first line: ";
    *fLog << Form ("fRigMin=%8f  fRigMax=%8f  fProb=%8f",
                   fRigMin[1152], fRigMax[1152], fProb[1152]) << endl;

    //
    if (fMcEvt)
        return kTRUE;

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
void MFGeomag::SetGammElec()
{
    fGammaElectron = kTRUE;  // logical variable, will take gammas as electrons
    *fLog <<" MFGeomag called to treat gammas as electrons" << endl;
    return;
}

// --------------------------------------------------------------------------
//
Int_t MFGeomag::Process()
{
    fResult = kFALSE;

    const Float_t en =  fMcEvt->GetEnergy();       // for rigidity (set P = E)
    Float_t rig = en;
    const Float_t az =  fMcEvt->GetTelescopePhi(); // charge theta phi are entries in table
    const Float_t th =  fMcEvt->GetTelescopeTheta();

    Int_t indadd=0;              //first part of table (positive particles)
    switch (fMcEvt->GetPartId())
    {
    case MMcEvt::kGAMMA:
        if (!fGammaElectron)     //accept gammas if not set to electrons
            return kTRUE;
        indadd = 1152;           //second part of table (negative particles)
        break;

    case MMcEvt::kHELIUM:
        rig /= 2;                //double charge
        break;

    case MMcEvt::kPROTON:                //protons
    case MMcEvt::kPOSITRON:              //positrons
        break;

    case MMcEvt::kELECTRON:              //electrons
        indadd = 1152;           //second part of table (negative particles)
        break;

    default:
        *fLog << err << " Unknown Monte Carlo Particle Id#: "<< fMcEvt->GetPartId() << endl;
        return kFALSE;
    }

    // here is the cut for charged particles using the table

    int it=(int)(th*11.459156);    // table steps are in 5 deg = 1/11.459 rads
    int ia=(int)(az*11.459156);

    ia = (ia+36) % 72;             // azimuth definitions differ by 180 deg

    const Float_t r1=fRigMin[72*it+ia+indadd];
    if (rig<=r1)
    {
        fResult=kTRUE;   // reject
        return kTRUE;
    }

    const Float_t r2=fRigMax[72*it+ia+indadd];
    if (rig>=r2)
        return kTRUE;   // accept

    const Float_t pr=fProb[72*it+ia+indadd];

    // accept if above intermediate threshold
    const Float_t rnd = (r2-r1)/2 * gRandom->Rndm(0);

    if ((rig-r1)*pr < rnd)
        fResult = kTRUE;                // pretty good approximation

    return kTRUE;
}
