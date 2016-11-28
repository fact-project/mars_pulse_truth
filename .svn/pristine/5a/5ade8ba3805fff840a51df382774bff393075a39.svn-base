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
!   Author(s): Antonio Stamerra  02/2003 <mailto:antonio.stamerra@pi.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   MFTriggerLvl2                                                         //
//                                                                         //
//  A Filter for the 2nd Level trigger, using the MMcTriggerLvl2 Class     //
//
/////////////////////////////////////////////////////////////////////////////
#include "MFTriggerLvl2.h"

#include <fstream>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MMcTriggerLvl2.h"

ClassImp(MFTriggerLvl2);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor
//
MFTriggerLvl2::MFTriggerLvl2(const char *cname, const char type, const Int_t val,
                             const char *name, const char *title) : fcell(NULL)
{
    fContName = cname;
    Init(type, val, name, title);
}

// --------------------------------------------------------------------------
 //
MFTriggerLvl2::MFTriggerLvl2(MMcTriggerLvl2 *triglvl2, const char type, const Int_t val,
                             const char *name, const char *title) : fcell(triglvl2)
{
    Init(type, val, name, title);
}

// --------------------------------------------------------------------------
//
void MFTriggerLvl2::Init(const char type, const Int_t val,
                         const char *name, const char *title)
{
    fName  = name  ? name  : "MFTriggerLvl2";
    fTitle = title ? title : "Filter using 2nd level trigger selection";

    fFilterType = (type=='<' ? kELowerThan : kEGreaterThan);

    if (type!='<' && type!='>')
        *fLog << warn << dbginf << "Warning: Neither '<' nor '>' specified... using '>'." << endl;

    fValue = val;

} 
// --------------------------------------------------------------------------
//
Bool_t MFTriggerLvl2::IsExpressionTrue() const
{
    return fResult;
}

// --------------------------------------------------------------------------
//
Int_t MFTriggerLvl2::PreProcess(MParList *pList)
{
    if (fcell)
        return kTRUE;

    fcell = (MMcTriggerLvl2*)pList->FindObject(fContName);
    if (fcell)
        return kTRUE;

    *fLog << err << dbginf << fContName << " [MMcTriggerLvl2] not found... aborting." << endl;

        return kFALSE;
}

// --------------------------------------------------------------------------
//
Int_t MFTriggerLvl2::Process()
{

  //
  //  The variable fLutPseudoSize of the class MMcTriggerLvl2 is used 
  //   for the selection
  //

  const Int_t lvl2 = fcell->GetLutPseudoSize();

    switch (fFilterType)
    {
    case kELowerThan:
        fResult = (lvl2 < fValue);
        break;
    case kEGreaterThan:
        fResult = (lvl2 > fValue);
        break;
    }

    return kTRUE;
}

void MFTriggerLvl2::StreamPrimitive(ostream &out) const
{
   if (fcell)
        fcell->SavePrimitive(out);

    out << "   MFTriggerLvl2 " << GetUniqueName() << "(";

    if (fcell)
        out << "&" << fcell->GetUniqueName();
    else
        out << "\"" << fContName << "\"";

    out << ", '" << (fFilterType==kELowerThan?"<":">") << "', " << fValue << ");" << endl;

}
