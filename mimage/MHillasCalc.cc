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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Harald Kornmayer, 1/2001
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MHillasCalc
//  ===========
//
//  This is a task to calculate the Hillas parameters from each event
//
//
//   Flags
//  --------
//
//  By default all flags are set:
//
//  To switch of the calculation you may use:
//   - Disable(MHillasCalc::kCalcHillas)
//   - Disable(MHillasCalc::kCalcHillasExt)
//   - Disable(MHillasCalc::kCalcNewImagePar)
//   - Disable(MHillasCalc::kCalcImagePar)
//   - Disable(MHillasCalc::kCalcImagePar2)
//   - Disable(MHillasCalc::kCalcSrcPosCam)
//   - Disable(MHillasCalc::kCalcConc)
//
//  If the calculation of MHillas is switched off a container MHillas
//  in the parameter list is nevertheless necessary for the calculation
//  of some other containers, see below.
//
//  If kCalcHillasSrc is set and no corresponding MSrcPosCam is found
//  in the parameter list an empty container (X=0, Y=0) is created.
//
//
//   Container names
//  -----------------
//
//  The names of the containers to be used can be set with:
//   - SetNameHillas("NewName")
//   - SetNameHillasExt("NewName")
//   - SetNameNewImgPar("NewName")
//   - SetNameImagePar("NewName")
//   - SetNameImagePar2("NewName")
//   - SetNameSrcPosCam("NewName")
//   - SetNameConc("NewName")
//   - SetNameHillasSrc("NewName")
//
//
//   Islands
//  ---------
//
//  You can change the islands for which the caluclations are done by:
//   - SetNumIsland()
//  The default is to use all used pixels (-1)
//
//  fIdxIslands effects the calculations:
//   - kCalcHillas
//   - kCalcHillasExt
//   - kCalcNewImgPar
//   - kCalcNewImgPar2
//
//
//   Example
//  ---------
//
//   MHillasCalc calc0; // calculate all image parameters except source dep.
//   MHillasCalc calc1; // calculate source dependant image parameters for 'Source'
//   MHillasCalc calc2; // calculate source dependant image parameters for 'AntiSource'
//   MHillasCalc calc3; // calculate hillas parameters only for biggest island
//   MHillasCalc calc4; // calculate hillas parameter for 2nd biggest island
//   // setup names of input-/output-containers
//   calc1.SetNameSrcPosCam("Source");
//   calc2.SetNameSrcPosCam("AntiSource");
//   calc1.SetNameHillasSrc("MHillasSource");
//   calc2.SetNameHillasSrc("MHillasAntiSource");
//   calc3.SetNameHillas("MHillas0");
//   calc4.SetNameHillas("MHillas1");
//   // setup calculations to be done
//   calc0.Disable(MHillasCalc::kCalcHillasSrc);
//   calc1.SetFlags(MHillasCalc::kCalcHillasSrc);
//   calc2.SetFlags(MHillasCalc::kCalcHillasSrc);
//   calc3.SetFlags(MHillasCalc::kCalcHillas);
//   calc4.SetFlags(MHillasCalc::kCalcHillas);
//   // choode index of island
//   calc3.SetNumIsland(0);
//   calc4.SetNumIsland(1);
//
//   // setup tasklist
//   MTaskList list;
//   list.Add(&calc0);
//   list.Add(&calc1);
//   list.Add(&calc2);
//   list.Add(&calc3);
//   list.Add(&calc4);
//
//
//   Input/Output Containers
//  -------------------------
//
//    1) MGeomCam          5) MHillas         8) MImagePar
//    2) MSignalCam        6) MHillasSrc      9) MNewImagePar
//    3) MSrcPosCam        7) MHillasExt     10) MNewImagePar2
//    4) fIdxIslands      11) MConcentration
//
//     Flag           | Input Container | Output
//   -----------------+-----------------+--------
//    kCalcHillas     |  1  2     4     |    5
//    kCalcHillasSrc  |        3  4  5  |    6
//    kCalcHillasExt  |  1  2     4  5  |    7
//    kCalcImagePar   |     2           |    8
//    kCalcNewImgPar  |  1  2     4  5  |    9
//    kCalcNewImgPar2 |  1  2     4     |   10
//    kCalcConc       |  1  2        5  |   11
//   -----------------+-----------------+--------
//
/////////////////////////////////////////////////////////////////////////////
#include "MHillasCalc.h"

#include <fstream> // StreamPrimitive

#include "MParList.h"

#include "MSignalCam.h"

#include "MHillas.h"
#include "MHillasExt.h"
#include "MHillasSrc.h"
#include "MImagePar.h"
#include "MNewImagePar.h"
#include "MNewImagePar2.h"
#include "MConcentration.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHillasCalc);

using namespace std;

const TString MHillasCalc::gsDefName         = "MHillasCalc";
const TString MHillasCalc::gsDefTitle        = "Calculate Hillas and other image parameters";

const TString MHillasCalc::gsNameHillas      = "MHillas";        // default name of the 'MHillas' container
const TString MHillasCalc::gsNameHillasExt   = "MHillasExt";     // default name of the 'MHillasExt' container
const TString MHillasCalc::gsNameNewImagePar = "MNewImagePar";   // default name of the 'MNewImagePar' container
const TString MHillasCalc::gsNameNewImagePar2= "MNewImagePar2";  // default name of the 'MNewImagePar2' container
const TString MHillasCalc::gsNameConc        = "MConcentration"; // default name of the 'MConcentration' container
const TString MHillasCalc::gsNameImagePar    = "MImagePar";      // default name of the 'MImagePar' container
const TString MHillasCalc::gsNameHillasSrc   = "MHillasSrc";     // default name of the 'MHillasSrc' container
const TString MHillasCalc::gsNameSrcPosCam   = "MSrcPosCam";     // default name of the 'MSrcPosCam' container

// --------------------------------------------------------------------------
//
// Default constructor.
//
MHillasCalc::MHillasCalc(const char *name, const char *title)
        : fNameHillas(gsNameHillas),       fNameHillasExt(gsNameHillasExt),
          fNameHillasSrc(gsNameHillasSrc), fNameSrcPosCam(gsNameSrcPosCam),
          fNameConc(gsNameConc),           fNameImagePar(gsNameImagePar),
          fNameNewImagePar(gsNameNewImagePar),
          fNameNewImagePar2(gsNameNewImagePar2),
          fErrors(7), fFlags(0xff), fIdxIsland(-1)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
// Check for in-/output containers, see class description
//
Int_t MHillasCalc::PreProcess(MParList *pList)
{

    if (TestFlags(~kCalcHillasSrc))
    {
        fCerPhotEvt = (MSignalCam*)pList->FindObject(AddSerialNumber("MSignalCam"));
        if (!fCerPhotEvt)
        {
            *fLog << err << "MSignalCam not found... aborting." << endl;
            return kFALSE;
        }
    }

    if (TestFlags(kCalcHillas|kCalcHillasExt|kCalcNewImagePar|kCalcNewImagePar2|kCalcConc))
    {
        fGeomCam = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
        if (!fGeomCam)
        {
            *fLog << err << "MGeomCam (Camera Geometry) missing in Parameter List... aborting." << endl;
            return kFALSE;
        }
    }

    // depend on whether MHillas is an in- or output container
    if (TestFlag(kCalcHillas))
    {
        fHillas = (MHillas*)pList->FindCreateObj("MHillas", AddSerialNumber(fNameHillas));
        if (!fHillas)
            return kFALSE;
    }

    if (TestFlags(kCalcHillasExt|kCalcNewImagePar|kCalcConc|kCalcHillasSrc))
    {
        fHillas = (MHillas*)pList->FindObject(AddSerialNumber(fNameHillas), "MHillas");
        if (!fHillas)
        {
            *fLog << err << fNameHillas << " [MHillas] not found... aborting." << endl;
            return kFALSE;
        }
    }

    // if enabled
    if (TestFlag(kCalcHillasExt))
    {
        fHillasExt = (MHillasExt*)pList->FindCreateObj("MHillasExt", AddSerialNumber(fNameHillasExt));
        if (!fHillasExt)
            return kFALSE;
    }

    // if enabled
    if (TestFlag(kCalcHillasSrc))
    {
        const MSrcPosCam *src = (MSrcPosCam*)pList->FindObject(AddSerialNumber(fNameSrcPosCam), "MSrcPosCam");
        if (!src)
        {
            *fLog << warn << AddSerialNumber(fNameSrcPosCam) << " [MSrcPosCam] not found... creating default container." << endl;
            src = (MSrcPosCam*)pList->FindCreateObj("MSrcPosCam", AddSerialNumber(fNameSrcPosCam));
        }
        if (!src)
            return kFALSE;

        fHillasSrc = (MHillasSrc*)pList->FindCreateObj("MHillasSrc", AddSerialNumber(fNameHillasSrc));
        if (!fHillasSrc)
            return kFALSE;

        fHillasSrc->SetSrcPos(src);
    }

    // if enabled
    if (TestFlag(kCalcNewImagePar))
    {
        fNewImgPar = (MNewImagePar*)pList->FindCreateObj("MNewImagePar", AddSerialNumber(fNameNewImagePar));
        if (!fNewImgPar)
            return kFALSE;
    }

    // if enabled
    if (TestFlag(kCalcNewImagePar2))
    {
        fNewImgPar2 = (MNewImagePar2*)pList->FindCreateObj("MNewImagePar2", AddSerialNumber(fNameNewImagePar2));
        if (!fNewImgPar2)
            return kFALSE;
    }

    // if enabled
    if (TestFlag(kCalcImagePar))
    {
        fImagePar = (MImagePar*)pList->FindCreateObj("MImagePar", AddSerialNumber(fNameImagePar));
        if (!fImagePar)
            return kFALSE;
    }

    // if enabled
    if (TestFlag(kCalcConc))
    {
        fConc = (MConcentration*)pList->FindCreateObj("MConcentration", fNameConc);
        if (!fConc)
            return kFALSE;
    }

    fErrors.Reset();

    Print();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// If you want do complex descisions inside the calculations
// we must move the calculation code inside this function
//
// If the calculation wasn't sucessfull skip this event
//
Int_t MHillasCalc::Process()
{
    if (TestFlag(kCalcHillas))
    {
        const Int_t rc = fHillas->Calc(*fGeomCam, *fCerPhotEvt, fIdxIsland);
        if (rc<0 || rc>4)
        {
            *fLog << err << dbginf << "MHillas::Calc returned unknown error code!" << endl;
            return kFALSE;
        }
        if (rc>0)
        {
            fErrors[rc]++;
            return kCONTINUE;
        }
    }

    if (TestFlag(kCalcHillasSrc))
    {
        const Int_t rc = fHillasSrc->Calc(*fHillas);
        if (rc<0 || rc>2)
        {
            *fLog << err << dbginf << "MHillasSrc::Calc returned unknown error code!" << endl;
            return kFALSE;
        }
        if (rc>0)
        {
            fErrors[rc+4]++;
            return kCONTINUE;
        }
    }
    fErrors[0]++;

    if (TestFlag(kCalcHillasExt))
        fHillasExt->Calc(*fGeomCam, *fCerPhotEvt, *fHillas, fIdxIsland);

    if (TestFlag(kCalcImagePar))
        fImagePar->Calc(*fCerPhotEvt);

    if (TestFlag(kCalcNewImagePar))
        fNewImgPar->Calc(*fGeomCam, *fCerPhotEvt, *fHillas, fIdxIsland);

    if (TestFlag(kCalcNewImagePar2))
        fNewImgPar2->Calc(*fGeomCam, *fCerPhotEvt, fIdxIsland);

    if (TestFlag(kCalcConc))
        fConc->Calc(*fGeomCam, *fCerPhotEvt, *fHillas);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Prints some statistics about the hillas calculation. The percentage
//  is calculated with respect to the number of executions of this task.
//
Int_t MHillasCalc::PostProcess()
{
    if (GetNumExecutions()==0)
        return kTRUE;

    if (!TestFlag(kCalcHillas) && !TestFlag(kCalcHillasSrc))
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    if (TestFlag(kCalcHillas))
    {
        PrintSkipped(fErrors[1], "0-Pixel Event (before cleaning, MC event?)");
        PrintSkipped(fErrors[2], "Calculated Size == 0 (after cleaning)");
        PrintSkipped(fErrors[3], "Number of used pixels < 3");
        //PrintSkipped(fErrors[4], "CorrXY==0");
    }
    if (TestFlag(kCalcHillasSrc))
    {
        PrintSkipped(fErrors[5], "Dist==0");
        PrintSkipped(fErrors[6], "Arg2==0");
    }
    *fLog << " " << (int)fErrors[0] << " (" << Form("%5.1f", 100.*fErrors[0]/GetNumExecutions()) << "%) Evts survived Hillas calculation!" << endl;
    *fLog << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print 'Dataflow'
//
void MHillasCalc::Print(Option_t *o) const
{
    *fLog << inf << GetDescriptor() << " calculating:" << endl;
    if (TestFlag(kCalcHillas))
        *fLog << " - " << fNameHillas << " from MGeomCam, MSignalCam and fIdxIsland=" << fIdxIsland << endl;
    if (TestFlag(kCalcHillasSrc))
        *fLog << " - " << fNameHillasSrc << " from " << fNameSrcPosCam << ", " << fNameHillas << " and fIdxIsland=" << fIdxIsland << endl;
    if (TestFlag(kCalcHillasExt))
        *fLog << " - " << fNameHillasExt << " from MGeomCam, MSignalCam, " << fNameHillas << " and fIdxIsland=" << fIdxIsland << endl;
    if (TestFlag(kCalcImagePar))
        *fLog << " - " << fNameImagePar << " from MSignalCam" << endl;
    if (TestFlag(kCalcNewImagePar))
        *fLog << " - " << fNameNewImagePar << " from MGeomCam, MSignalCam, " << fNameHillas << " and fIdxIsland=" << fIdxIsland << endl;
    if (TestFlag(kCalcNewImagePar2))
        *fLog << " - " << fNameNewImagePar2 << " from MGeomCam, MSignalCam, " << fNameHillas << " and fIdxIsland=" << fIdxIsland << endl;
    if (TestFlag(kCalcConc))
        *fLog << " - " << fNameConc << " from MGeomCam, MSignalCam and " << fNameHillas << endl;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MHillasCalc::StreamPrimitive(ostream &out) const
{
    out << "   MHillasCalc " << GetUniqueName() << "(";
    if (fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << ", \"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\"";
    }
    out << ");" << endl;

    if (TestFlags(kCalcHillasExt|kCalcNewImagePar|kCalcConc|kCalcHillasSrc))
    {
        if (fNameHillas!=gsNameHillas)
            out << "   " << GetUniqueName() << ".SetNameHillas(\"" << fNameHillas << "\");" << endl;
    }
    if (TestFlag(kCalcHillasSrc) && fNameHillasSrc!=gsNameHillasSrc)
    {
        out << "   " << GetUniqueName() << ".SetNameHillasSrc(\"" << fNameHillasSrc << "\");" << endl;
        out << "   " << GetUniqueName() << ".SetNameSrcPosCam(\"" << fNameSrcPosCam << "\");" << endl;
    }
    if (TestFlag(kCalcHillasExt) && fNameHillasExt!=gsNameHillasExt)
        out << "   " << GetUniqueName() << ".SetNameHillasExt(\"" << fNameHillasExt << "\");" << endl;
    if (TestFlag(kCalcConc) && fNameConc!=gsNameConc)
        out << "   " << GetUniqueName() << ".SetNameConc(\"" << fNameConc << "\");" << endl;
    if (TestFlag(kCalcImagePar) && fNameImagePar!=gsNameImagePar)
        out << "   " << GetUniqueName() << ".SetNameImagePar(\"" << fNameImagePar << "\");" << endl;
    if (TestFlag(kCalcNewImagePar) && fNameNewImagePar!=gsNameNewImagePar)
        out << "   " << GetUniqueName() << ".SetNameNewImagePar(\"" << fNameNewImagePar << "\");" << endl;
    if (TestFlag(kCalcNewImagePar2) && fNameNewImagePar2!=gsNameNewImagePar2)
        out << "   " << GetUniqueName() << ".SetNameNewImagePar2(\"" << fNameNewImagePar2 << "\");" << endl;

    if (!TestFlag(kCalcHillas))
        out << "   " << GetUniqueName() << ".Disable(MHilllasCalc::kCalcHillas);" << endl;
    if (!TestFlag(kCalcHillasExt))
        out << "   " << GetUniqueName() << ".Disable(MHilllasCalc::kCalcHillasExt);" << endl;
    if (!TestFlag(kCalcHillasSrc))
        out << "   " << GetUniqueName() << ".Disable(MHilllasCalc::kCalcHillasSrc);" << endl;
    if (!TestFlag(kCalcNewImagePar))
        out << "   " << GetUniqueName() << ".Disable(MHilllasCalc::kCalcNewImagePar);" << endl;
    if (!TestFlag(kCalcNewImagePar2))
        out << "   " << GetUniqueName() << ".Disable(MHilllasCalc::kCalcNewImagePar2);" << endl;
    if (!TestFlag(kCalcConc))
        out << "   " << GetUniqueName() << ".Disable(MHilllasCalc::kCalcConc);" << endl;
    if (!TestFlag(kCalcImagePar))
        out << "   " << GetUniqueName() << ".Disable(MHilllasCalc::kCalcImagePar);" << endl;

    if (fIdxIsland>=0)
        out << "   " << GetUniqueName() << ".SetNumIsland(" << fIdxIsland << ");" << endl;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv, eg:
//   MHillasCalc.IdxIsland: -1
//
Int_t MHillasCalc::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "IdxIsland", print))
    {
        rc = kTRUE;
        SetIdxIsland(GetEnvValue(env, prefix, "IdxIsland", fIdxIsland));
    }
    return rc;
}
