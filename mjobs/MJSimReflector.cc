/////////////////////////////////////////////////////////////////////////////
//
//  MJSimReflector
//
//
// This process performs the simulation up to the resulting photons on the camera
// window:
//
// - Simulating of Pointing
// - Absorption in the atmosphere
// - Absorption by mirror reflectivity and additional photon acceptance
// - Simulation of the reflector
// - Absorption by angular acceptance of winston cones
// - Absorption by transmission acceptance of winston cones
// - Absorption by pde of the SiPMs
//
// An ASCII output file with the information of all photons is written to disk.
// The place in the simulation chain can be chosen by changing the fill point of the
// writeCherPhotonFile into the tasklist
//
//
/////////////////////////////////////////////////////////////////////////////

#include "MJSimReflector.h"

#include <TEnv.h>
#include <TCanvas.h>
#include <iostream>

// Core
#include "MLog.h"
#include "MLogManip.h"

#include "MArgs.h"
#include "MDirIter.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

#include "MStatusDisplay.h"

#include "MWriteAsciiFile.h"

// Tasks
#include "MCorsikaRead.h"
#include "MContinue.h"
#include "MGeomApply.h"
#include "MParameterCalc.h"

#include "MSimMMCS.h"
#include "MSimAbsorption.h"
#include "MSimAtmosphere.h"
#include "MSimReflector.h"
#include "MSimPointingPos.h"
#include "MSimPSF.h"
#include "MSimGeomCam.h"
#include "MSimRandomPhotons.h"
#include "MSimBundlePhotons.h"

// Container
#include "MRawRunHeader.h"
#include "MParameters.h"
#include "MReflector.h"
#include "MParEnv.h"
#include "MSpline3.h"
#include "MParSpline.h"
#include "MGeomCam.h"
#include "MMatrix.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

ClassImp(MJSimReflector);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
// Sets fRuns to 0, fExtractor to NULL, fDataCheck to kFALSE
//
MJSimReflector::MJSimReflector(const char *name, const char *title)
    : fForceMode(kFALSE), fOperationMode(kModeData), fRunNumber(-1)
{
    fName  = name  ? name  : "MJSimReflector";
    fTitle = title ? title : "Standard analysis and reconstruction";
}

Bool_t MJSimReflector::CheckEnvLocal()
{
    fForceMode    = GetEnv("ForceMode",    fForceMode);

    return kTRUE;
}

// Setup the header accordingly to the used operation mode
void MJSimReflector::SetupHeaderOperationMode(MRawRunHeader &header) const
{
    switch (fOperationMode)
    {
    case kModeData:
        header.SetRunType(MRawRunHeader::kRTMonteCarlo|MRawRunHeader::kRTData);
        header.SetRunInfo(0, fRunNumber<0 ? 3 : fRunNumber);
        break;

    case kModePed:
        header.SetRunType(MRawRunHeader::kRTMonteCarlo|MRawRunHeader::kRTPedestal);
        header.SetSourceInfo("Pedestal");
        header.SetRunInfo(0, fRunNumber<0 ? 1 : fRunNumber);
        break;

    case kModeCal:
        header.SetRunType(MRawRunHeader::kRTMonteCarlo|MRawRunHeader::kRTCalibration);
        header.SetSourceInfo("Calibration");
        header.SetRunInfo(0, fRunNumber<0 ? 2 : fRunNumber);
        break;

    case kModePointRun:
        header.SetRunType(MRawRunHeader::kRTMonteCarlo|MRawRunHeader::kRTPointRun);
        header.SetRunInfo(0, fRunNumber<0 ? 0 : fRunNumber);
        break;
    }
}


Bool_t MJSimReflector::Process(const MArgs &args, const MSequence &seq)
{
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // Initialization
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // --------------------------------------------------------------------------------
    // Logging output:
    // --------------------------------------------------------------------------------
    // - description of the job itself
    // - list of the
    *fLog << inf;
    fLog->Separator(GetDescriptor());

    if (!CheckEnv())
        return kFALSE;

    if (seq.IsValid())
        *fLog << fSequence.GetFileName() << endl;
    else
        *fLog << "Input: " << args.GetNumArguments() << "-files" << endl;
    *fLog << endl;

    MDirIter iter;
    if (seq.IsValid() && seq.GetRuns(iter, MSequence::kCorsika)<=0)
    {
        *fLog << err << "ERROR - Sequence valid but without files." << endl;
        return kFALSE;
    }
    // --------------------------------------------------------------------------------
    // Setup MParList and MTasklist
    // --------------------------------------------------------------------------------
    MParList plist;
    plist.AddToList(this); // take care of fDisplay!
    // setup TaskList
    MTaskList tasks;
    plist.AddToList(&tasks);

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // Parameter Container Setup
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // --------------------------------------------------------------------------------
    // Setup container for the reflector, the cones and the camera geometry
    // --------------------------------------------------------------------------------
    MParEnv env0("Reflector");
    MParEnv env1("GeomCones");
    MParEnv env2("MGeomCam");
    env0.SetClassName("MReflector");
    env1.SetClassName("MGeomCam");
    env2.SetClassName("MGeomCam");
    plist.AddToList(&env0);
    plist.AddToList(&env1);
    plist.AddToList(&env2);
    // --------------------------------------------------------------------------------
    // Setup spline containers for:
    // --------------------------------------------------------------------------------
    // - the different photon acceptance splines
    MParSpline splinepde("PhotonDetectionEfficiency");
    MParSpline splinemirror("MirrorReflectivity");
    MParSpline splinecones("ConesAngularAcceptance");
    MParSpline splinecones2("ConesTransmission");
    MParSpline splineAdditionalPhotonAcceptance("AdditionalPhotonAcceptance");
    plist.AddToList(&splinepde);
    plist.AddToList(&splinemirror);
    plist.AddToList(&splinecones);
    plist.AddToList(&splinecones2);
    plist.AddToList(&splineAdditionalPhotonAcceptance);

    // --------------------------------------------------------------------------------
    // Setup container for the MC Run Header and the Raw Run Header
    // --------------------------------------------------------------------------------
    plist.FindCreateObj("MMcRunHeader");

    MRawRunHeader header;
    header.SetValidMagicNumber();
    SetupHeaderOperationMode(header);
    header.SetObservation("On", "MonteCarlo");
    plist.AddToList(&header);

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // Tasks Setup
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // --------------------------------------------------------------------------------
    // Corsika read setup
    // --------------------------------------------------------------------------------
    MCorsikaRead read;
    read.SetForceMode(fForceMode);

    if (!seq.IsValid())
    {
        for (int i=0; i<args.GetNumArguments(); i++)
            read.AddFile(args.GetArgumentStr(i));
    }
    else
        read.AddFiles(iter);

    // --------------------------------------------------------------------------------
    // Precut after reading
    // --------------------------------------------------------------------------------
    // I am not sure if here there is a default for numPhotons < 10
    MContinue precut("", "PreCut");
    precut.IsInverted();
    precut.SetAllowEmpty();

    // --------------------------------------------------------------------------------
    // Simmmcs and calculation of the incident angle.
    // --------------------------------------------------------------------------------
    MSimMMCS simmmcs;

    // The different strings defines the calculation of the incident angle
    const TString sin2 = "sin(MCorsikaEvtHeader.fZd)*sin(MCorsikaRunHeader.fZdMin*TMath::DegToRad())";
    const TString cos2 = "cos(MCorsikaEvtHeader.fZd)*cos(MCorsikaRunHeader.fZdMin*TMath::DegToRad())";
    const TString cos  = "cos(MCorsikaEvtHeader.fAz-MCorsikaRunHeader.fAzMin*TMath::DegToRad())";
    const TString form = "acos("+sin2+"*"+cos+"+"+cos2+")*TMath::RadToDeg()";

    MParameterCalc calcangle(form, "CalcIncidentAngle");
    calcangle.SetNameParameter("IncidentAngle");

    // --------------------------------------------------------------------------------
    // Setup of the different Absorptions
    // --------------------------------------------------------------------------------
    // - atmosphere
    // - PDE of the SiPMs
    // - mirror reflectivity
    // - cones angular acceptance
    // - cones transmission
    // - additional photon acceptance
    MSimAtmosphere simatm;
    MSimAbsorption absapd("SimPhotonDetectionEfficiency");
    MSimAbsorption absmir("SimMirrorReflectivity");
    MSimAbsorption cones("SimConesAngularAcceptance");
    MSimAbsorption cones2("SimConesTransmission");
    MSimAbsorption additionalPhotonAcceptance("SimAdditionalPhotonAcceptance");
    absapd.SetParName("PhotonDetectionEfficiency");
    absmir.SetParName("MirrorReflectivity");
    cones.SetParName("ConesAngularAcceptance");
    cones.SetUseTheta();
    cones2.SetParName("ConesTransmission");
    additionalPhotonAcceptance.SetParName("AdditionalPhotonAcceptance");
    // --------------------------------------------------------------------------------
    // Pointing position and reflector simulation
    // --------------------------------------------------------------------------------
    MSimPointingPos pointing;
    MSimReflector reflect;
    reflect.SetNameGeomCam("GeomCones");
    reflect.SetNameReflector("Reflector");
    // --------------------------------------------------------------------------------
    // Seupt of some continue conditions
    // --------------------------------------------------------------------------------
    MContinue cont1("MPhotonEvent.GetNumPhotons<1", "ContEmpty1");
    MContinue cont2("MPhotonEvent.GetNumPhotons<1", "ContEmpty2");
    MContinue cont3("MPhotonEvent.GetNumPhotons<2", "ContEmpty3");
    cont1.SetAllowEmpty();
    cont2.SetAllowEmpty();
    cont3.SetAllowEmpty();

    // --------------------------------------------------------------------------------
    // Geom apply, Simulation psf, and simulation of the pixel geometry
    // --------------------------------------------------------------------------------
    // Be awere MGeomApply only resizes parameter container which heritates from
    // MGeomCam to the actual size of the camera
    MGeomApply apply;

    // individual single mirror psf
    MSimPSF simpsf;

    // Simulate the geometry of the pixels (so which photon hit which pixel)
    MSimGeomCam simgeom;
    simgeom.SetNameGeomCam("GeomCones");

    // --------------------------------------------------------------------------------
    // Setup of the Write Task
    // --------------------------------------------------------------------------------
    // Setup of the outputpath
    if (!fFileOut.IsNull())
    {
        const Ssiz_t dot   = fFileOut.Last('.');
        const Ssiz_t slash = fFileOut.Last('/');
        if (dot>slash)
            fFileOut = fFileOut.Remove(dot);
    }

    const char *fmt = Form("%s/%08d%%s.csv", fPathOut.Data(), header.GetRunNumber());
    TString outputFilePath(Form(fmt, Form("%s", "" )));

    MWriteAsciiFile writePhotonFile(outputFilePath,"MPhotonEvent");

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // Filling of tasks in tasklist
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    if (header.IsDataRun())
    {
        tasks.AddToList(&read);
        tasks.AddToList(&pointing);
        tasks.AddToList(&simmmcs);
        tasks.AddToList(&simatm);
        tasks.AddToList(&calcangle);
        if (!header.IsPointRun())
        {
            tasks.AddToList(&absmir);
            tasks.AddToList(&additionalPhotonAcceptance);
        }
        tasks.AddToList(&reflect);
    }
    tasks.AddToList(&apply);
    if (header.IsDataRun())
    {
//        tasks.AddToList(&simpsf);
        tasks.AddToList(&simgeom);
        tasks.AddToList(&writePhotonFile);
        tasks.AddToList(&cones);
        tasks.AddToList(&cones2);
        if (!header.IsPointRun())
        {
            tasks.AddToList(&absapd);
        }
    }
    tasks.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    // Create and setup the eventloop
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(&gLog);
    if (!SetupEnv(evtloop))
        return kFALSE;

    header.Print();

    // Execute first analysis
    if (!evtloop.Eventloop(fMaxEvents))
    {
        gLog << err << GetDescriptor() << ": Failed." << endl;
        return kFALSE;
    }

//    if (!WriteResult(plist, seq, header.GetRunNumber()))
//        return kFALSE;

    *fLog << all << GetDescriptor() << ": Done." << endl << endl << endl;;

    return kTRUE;
}
