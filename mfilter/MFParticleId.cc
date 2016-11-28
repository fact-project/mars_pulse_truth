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
!   Author(s): Thomas Bretz  07/2001 <mailto:tbretz@uni-sw.gwdg.de>
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//   MFParticleId
//
//  A filter to choose between different particle types, identified by
//  their monte carlo particle type. For a list of idetifiers see
//  mmc/MMcEvt.h
//
/////////////////////////////////////////////////////////////////////////////
#include "MFParticleId.h"

#include <fstream>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MMcEvt.hxx"

ClassImp(MFParticleId);

using namespace std;

// --------------------------------------------------------------------------
//
MFParticleId::MFParticleId(const char *cname, const char type, const Int_t val,
                           const char *name, const char *title) : fMcEvt(NULL)
{
    fContName = cname;
    Init(type, val, name, title);
}

// --------------------------------------------------------------------------
//
MFParticleId::MFParticleId(MMcEvt *mcevt, const char type, const Int_t val,
                           const char *name, const char *title) : fMcEvt(mcevt)
{
    Init(type, val, name, title);
}

// --------------------------------------------------------------------------
//
void MFParticleId::Init(const char type, const Int_t val,
                        const char *name, const char *title)
{
    fName  = name  ? name  : "MFParticleId";
    fTitle = title ? title : "Filter using monte carlo particle id";

    fFilterType = (type=='=' ? kEEqual : kENotEqual);

    if (type!='=' && type!='!')
        *fLog << warn << dbginf << "Warning: Neither '=' nor '!' specified... using '>'." << endl;

    fValue = val;

    AddToBranchList(Form("%s.fPartId", (const char*)fContName));
}

// --------------------------------------------------------------------------
//
Bool_t MFParticleId::IsExpressionTrue() const
{
    return fResult;
}

// --------------------------------------------------------------------------
//
Int_t MFParticleId::PreProcess(MParList *pList)
{
    if (fMcEvt)
        return kTRUE;

    fMcEvt = (MMcEvt*)pList->FindObject(fContName);
    if (fMcEvt)
        return kTRUE;

    *fLog << err << dbginf << fContName << " [MMcEvt] not found... aborting." << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
Int_t MFParticleId::Process()
{
    const Int_t id = fMcEvt->GetPartId();

    switch (fFilterType)
    {
    case kEEqual:
        fResult = (id == fValue);
        return kTRUE;
    case kENotEqual:
        fResult = (id != fValue);
        return kTRUE;
    }

    *fLog << err << dbginf << "Operation unknown..." << endl;
    return kFALSE;
}

void MFParticleId::StreamPrimitive(ostream &out) const
{
    if (fMcEvt)
        fMcEvt->SavePrimitive(out);

    out << "   MFParticleId " << GetUniqueName() << "(";

    if (fMcEvt)
        out << "&" << fMcEvt->GetUniqueName();
    else
        out << "\"" << fContName << "\"";

    out << ", '" << (fFilterType==kEEqual?"=":"!") << "', ";

    switch (fValue)
    {
    case MMcEvt::kGAMMA:    out << "MMcEvt::kGAMMA";    break;
    case MMcEvt::kPOSITRON: out << "MMcEvt::kPOSITRON"; break;
    case MMcEvt::kELECTRON: out << "MMcEvt::kELECTRON"; break;
    case MMcEvt::kPROTON:   out << "MMcEvt::kPROTON";   break;
    case MMcEvt::kHELIUM:   out << "MMcEvt::kHELIUM";   break;
    case MMcEvt::kOXYGEN:   out << "MMcEvt::kOXYGEN";   break;
    case MMcEvt::kIRON:     out << "MMcEvt::kIRON";     break;
    default:
        out << fValue;
    }
    out << ");" << endl;
}
