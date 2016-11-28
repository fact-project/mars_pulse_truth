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
!   Author(s): Thomas Bretz, 09/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!              Markus Gaug,  03/2004 <mailto:markus@ifae.es>
!              Hendrik Bartko, 07/2003 <mailto:hbartko@mppmu.mpg.de>
!         
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
//  MGeomApply
//
//  Applies the geometry to geometry dependant containers.
//
//  It changes the size of the arrays in the containers to a size
//  matching the number of pixels, eg:
//
//   MPedestalCam
//   MCalibrationChargeCam
//   MCalibrationRelTimeCam
//   MCalibrationQECam
//   MCalibrationPedCam
//   MPedPhotCam
//   MExtractedSignalCam
//   MArrivalTime
//
//  It uses the geometry (MGeomCam) found in the parameter list.
//  If it is not found the task tries to create the geometry
//  specified in the constructor. The default geometry is
//  MGeomCamMagic.
//
// In a standard setup all containers in the parameter list which derive
// from MCamEvent are processed automatically in ReInit. To allow having
// two parallel geometries in the parameter list or for MCamEvent in the
// parameter list you can switch off the automatic procedure by adding
// the containers to be processed using AddCamEvent().
//
// 
//  Input Containers:
//   [MGeomCam]
//   [all MCamEvent]
//
//  Output Containers:
//   [all MCamEvent]
//
//////////////////////////////////////////////////////////////////////////////
#include "MGeomApply.h"

#include <stdlib.h> // atoi (Ubuntu 8.10)

#include <fstream>

#include <TObjString.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MGeomCam.h"
#include "MCamEvent.h"
#include "MRawRunHeader.h"

ClassImp(MGeomApply);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default constructor. MGeomCamMagic is the default geometry.
//
MGeomApply::MGeomApply(const char *name, const char *title)
    : fGeomName("MGeomCamMagic"), fNamesList(0), fList(0)
{
    fName  = name  ? name  : "MGeomApply";
    fTitle = title ? title : "Task to apply geometry settings";
}

// --------------------------------------------------------------------------
//
//  Delete fList if available.
//
MGeomApply::~MGeomApply()
{
    if (fList)
        delete fList;
    if (fNamesList)
        delete fNamesList;
}

// --------------------------------------------------------------------------
//
//  Try to find 'MGeomCam' in the Parameter List. If it is not found,
//  try to create a fGeomName object.
//
Int_t MGeomApply::PreProcess(MParList *pList)
{
    MGeomCam *cam = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
    if (cam)
        return kTRUE;

    MRawRunHeader *h = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (h->GetCameraVersion()==0xfac7 && fGeomName=="MGeomCamMagic")
        fGeomName = "MGeomCamFACT";

    cam = (MGeomCam*)pList->FindCreateObj(AddSerialNumber(fGeomName), "MGeomCam");

    return cam ? kTRUE : kFALSE;
}

// --------------------------------------------------------------------------
//
//  Check the whole parameter list for MCamEvent. For all MCamEvent
//  MCamEvent::Init(MGeomCam&) is called.
//
void MGeomApply::ProcessAutomatic(MParList &list, const MGeomCam &geom) const
{
    TIter Next(list);
    TObject *o = 0;

    while ((o=Next()))
    {
        MCamEvent *cam = dynamic_cast<MCamEvent*>(o);
        if (!cam)
            continue;

        // If the MGeomApply task has a serial number >0 (indicating most likely
        // a specific telescope in a multi-telescope file), then apply the
        // geometry only to objects with the same serial number. This is important
        // for stereo setups in which the telescopes have cameras with different
        // numbers of pixels. If the MGeomApply task has serial number 0 (default),
        // it will apply the geometry to all found objects as it used to do.
        const TString name(o->GetName());

        // Extract serial number from name:
        const Int_t serial = atoi(name.Data()+name.Last(';')+1);

        // Compare with the serial number of this task:
        if (serial>0 && serial!=GetSerialNumber())
            continue;

        // Initialize object according to camera geometry:
        cam->Init(geom);
    }
}

// --------------------------------------------------------------------------
//
//  Check all containers in fNamesList and fList. For all MCamEvent
//  MCamEvent::Init(MGeomCam&) is called.
//
Bool_t MGeomApply::ProcessManual(MParList &list, const MGeomCam &geom) const
{
    TIter NextN(fNamesList);
    TObject *o = 0;

    while ((o=NextN()))
    {
        TObject *cont = list.FindObject(o->GetName(), "MCamEvent");
        if (!cont)
        {
            *fLog << err << o->GetName() << " [MCamEvent] not found... abort." << endl;
            return kFALSE;
        }

        MCamEvent *cam = dynamic_cast<MCamEvent*>(o);
        cam->Init(geom);
    }

    TIter NextL(fList);
    while ((o=NextL()))
    {
        MCamEvent *cam = dynamic_cast<MCamEvent*>(o);
        cam->Init(geom);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Try to find 'MGeomCam' in the Parameter List. If it is not found,
//  processing is stopped.
//
Bool_t MGeomApply::ReInit(MParList *pList)
{
    MGeomCam *geom = (MGeomCam*)pList->FindObject(AddSerialNumber("MGeomCam"));
    if (!geom)
    {
       *fLog << err << GetDescriptor() << ": No MGeomCam found... aborting." << endl;
        return kFALSE;
    }

    // FIXME (workaround): this call to CalcPixRatio is here just to allow
    // the use of some MC camera files from the 0.7 beta version in which the 
    // array containing pixel ratios is not initialized.
    geom->StreamerWorkaround();
    geom->CalcPixRatio();

    if (fList)
        return ProcessManual(*pList, *geom);

    ProcessAutomatic(*pList, *geom);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MGeomApply::StreamPrimitive(ostream &out) const
{
    out << "   " << ClassName() << " " << GetUniqueName() << "(\"";
    out << "\"" << fName << "\", \"" << fTitle << "\");" << endl;

    if (fGeomName.IsNull())
        return;

    out << "   " << GetUniqueName() << ".SetGeometry(\"";
    out << fGeomName << "\");" << endl;
}

// --------------------------------------------------------------------------
//
// Add a MCamEvent to be processed. This switches off the automatic
// processing of all MCamEvent in the parameter list completely!
//
void MGeomApply::AddCamEvent(TObject *obj)
{
    if (!obj->InheritsFrom(MCamEvent::Class()))
    {
        *fLog << warn << "MGeomApply::AddCamEvent - WARNING: Object doesn't inherit from MCamEvent... ignored." << endl;
        return;
    }

    if (!fList)
    {
        fList = new TList;
        fNamesList = new TList;

        fNamesList->SetOwner();
    }

    fList->Add(obj);
}

// --------------------------------------------------------------------------
//
// Add a MCamEvent to be processed. This switches off the automatic
// processing of all MCamEvent in the parameter list completely!
//
void MGeomApply::AddCamEvent(const char *name)
{
    if (!fList)
    {
        fList = new TList;
        fNamesList = new TList;

        fNamesList->SetOwner();
    }

    fNamesList->Add(new TObjString(name));
}
