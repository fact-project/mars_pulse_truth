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
!   Author(s): Thomas Bretz, 1/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MJSimulation
//
//
// Force reading a corsika file even if the footer (RUNE-section) is missing
// by setting fForceMode to kTRUE or from the resource file by
//
//    ForceMode: Yes
//
//
// To switch off the simulation of the camera electronics, use:
//
//    Camera: Off
//
// Note, that the border of the camera and the propertied of the cones
// are still simulated (simply because it is fast). Furthermore, this
// switches off the trigger for the output, i.e. all data which deposits
// at least one photon in at least one pixel is written to the output file.
//
//
// In case of a pedestal or calibration run the artificial trigger can
// be "switched off" and the cosmics trrigger "switched on" by setting
// fForceTrigger to kTRUE or from the resource file by
//
//    ForceTrigger: Yes
//
//
/////////////////////////////////////////////////////////////////////////////
#include "MJSimulation.h"

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

// Tasks
#include "MCorsikaRead.h"
#include "MContinue.h"
#include "MFillH.h"
#include "MGeomApply.h"
#include "MParameterCalc.h"
#include "MSrcPosCalc.h"
#include "MHillasCalc.h"
#include "MImgCleanStd.h"
#include "MWriteRootFile.h"
#include "MWriteFitsFile.h"

#include "MSimMMCS.h"
#include "MSimAbsorption.h"
#include "MSimAtmosphere.h"
#include "MSimReflector.h"
#include "MSimPointingPos.h"
#include "MSimPSF.h"
#include "MSimGeomCam.h"
#include "MSimSignalCam.h"
#include "MSimAPD.h"
#include "MSimExcessNoise.h"
#include "MSimCamera.h"
#include "MSimTrigger.h"
#include "MSimReadout.h"
#include "MSimRandomPhotons.h"
#include "MSimBundlePhotons.h"
#include "MSimCalibrationSignal.h"

// Histograms
#include "MBinning.h"

#include "MHn.h"
#include "MHCamera.h"
#include "MHCamEvent.h"
#include "MHPhotonEvent.h"

// Container
#include "MRawRunHeader.h"
#include "MParameters.h"
#include "MReflector.h"
#include "MParEnv.h"
#include "MSpline3.h"
#include "MParSpline.h"
#include "MGeomCam.h"
#include "MPedestalCam.h"
#include "MPedestalPix.h"

ClassImp(MJSimulation);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
// Sets fRuns to 0, fExtractor to NULL, fDataCheck to kFALSE
//
MJSimulation::MJSimulation(const char *name, const char *title)
    : fForceMode(kFALSE), fCamera(kTRUE), fForceTrigger(kFALSE),
    fWriteFitsFile(kFALSE), fOperationMode(kModeData), fRunNumber(-1)
{
    fName  = name  ? name  : "MJSimulation";
    fTitle = title ? title : "Standard analysis and reconstruction";
}

Bool_t MJSimulation::CheckEnvLocal()
{
    fForceMode    = GetEnv("ForceMode",    fForceMode);
    fForceTrigger = GetEnv("ForceTrigger", fForceTrigger);
    fCamera       = GetEnv("Camera",       fCamera);

    return kTRUE;
}

Bool_t MJSimulation::WriteResult(const MParList &plist, const MSequence &seq, Int_t num)
{
    if (fPathOut.IsNull())
    {
        *fLog << inf << "No output path specified via SetPathOut - no output written." << endl;
        return kTRUE;
    }

    TObjArray cont;
    cont.Add(const_cast<TEnv*>(GetEnv()));
    if (seq.IsValid())
        cont.Add(const_cast<MSequence*>(&seq));

    cont.Add(plist.FindObject("PulseShape"));
    cont.Add(plist.FindObject("Reflector"));
    cont.Add(plist.FindObject("MGeomCam"));
    cont.Add(plist.FindObject("GeomCones"));

    TNamed cmdline("CommandLine", fCommandLine.Data());
    cont.Add(&cmdline);

    if (fDisplay)
    {
        TString title = "--  Ceres";
        if (seq.IsValid())
        {
            title += ": ";
            title += seq.GetSequence();
        }
        title += "  --";
        fDisplay->SetTitle("Ceres", kFALSE);

        cont.Add(fDisplay);
    }

    const TString name =  Form("ceres%08d.root", num);
    return WriteContainer(cont, name, "RECREATE");
}

void MJSimulation::SetupHist(MHn &hist) const
{
    hist.AddHist("MCorsikaEvtHeader.fTotalEnergy");
    hist.InitName("Energy");
    hist.InitTitle("Energy;E [GeV]");
    hist.SetLog(kTRUE, kTRUE, kFALSE);

    hist.AddHist("MPhotonEvent.GetNumExternal");
    hist.InitName("Size");
    hist.InitTitle("Size;S [#]");
    hist.SetLog(kTRUE, kTRUE, kFALSE);

    /*
     hist.AddHist("-MCorsikaEvtHeader.fX/100","-MCorsikaEvtHeader.fY/100");
     hist.SetDrawOption("colz");
     hist.InitName("Impact;Impact;Impact");
     hist.InitTitle("Impact;West <--> East [m];South <--> North [m]");
     hist.SetAutoRange();
     */

    hist.AddHist("MCorsikaEvtHeader.GetImpact/100");
    hist.InitName("Impact");
    hist.InitTitle("Impact;Impact [m]");
    hist.SetAutoRange();

    hist.AddHist("MCorsikaEvtHeader.fFirstInteractionHeight/100000");
    hist.InitName("Height");
    hist.InitTitle("FirstInteractionHeight;h [km]");

    hist.AddHist("(MCorsikaEvtHeader.fAz+MCorsikaRunHeader.fMagneticFieldAz)*TMath::RadToDeg()", "MCorsikaEvtHeader.fZd*TMath::RadToDeg()");
    hist.InitName("SkyOrigin;Az;Zd");
    hist.InitTitle("Sky Origin;Az [\\circ];Zd [\\circ]");
    hist.SetDrawOption("colz");
    hist.SetAutoRange();

    hist.AddHist("IncidentAngle.fVal");
    hist.InitName("ViewCone");
    hist.InitTitle("Incident Angle;\\alpha [\\circ]");
}

template<class T>
void MJSimulation::SetupCommonFileStructure(T &write) const
{
    // Common run headers
    write.AddContainer("MMcCorsikaRunHeader", "RunHeaders", kFALSE, 1);
    write.AddContainer("MCorsikaRunHeader",   "RunHeaders", kFALSE, 1);
    write.AddContainer("MRawRunHeader",       "RunHeaders", kTRUE,  1);
    write.AddContainer("MGeomCam",            "RunHeaders", kTRUE,  1);
    write.AddContainer("MMcRunHeader",        "RunHeaders", kTRUE,  1);

    // Common events
    write.AddContainer("MCorsikaEvtHeader",   "Events", kFALSE);
    write.AddContainer("MRawEvtHeader",       "Events");
    write.AddContainer("MMcEvt",              "Events", kFALSE);
    write.AddContainer("MMcEvtBasic",         "Events", kFALSE);
    write.AddContainer("IncidentAngle",       "Events", kFALSE);
    write.AddContainer("MPointingPos",        "Events", kFALSE);
    write.AddContainer("MSimSourcePos",       "Events", kFALSE);
    write.AddContainer("MSrcPosCam",          "Events", kFALSE);
}

void MJSimulation::SetupHeaderKeys(MWriteFitsFile &write,MRawRunHeader &header) const
{
    const MTime now(-1);
    write.SetHeaderKey("ISMC",true,"Bool if File is Montecarlo File");
    write.SetHeaderKey("TELESCOP", "FACT", "");
    write.SetHeaderKey("PACKAGE", "MARS Cheobs", "");
    write.SetHeaderKey("VERSION", "1.0", "");
    write.SetHeaderKey("CREATOR", "Ceres", "");
    write.SetHeaderKey("EXTREL", 1., "");
    write.SetHeaderKey("COMPILED", __DATE__" " __TIME__, "");
    write.SetHeaderKey("REVISION", "0", "");
    write.SetHeaderKey("ORIGIN", "FACT", "");
    write.SetHeaderKey("DATE", now.GetStringFmt("%Y-%m-%dT%H:%M:%S").Data(), "");
    write.SetHeaderKey("NIGHT", now.GetNightAsInt(), "");
    write.SetHeaderKey("TIMESYS", "UTC", "");
    write.SetHeaderKey("TIMEUNIT", "d", "");
    write.SetHeaderKey("MJDREF", 40587, "");
    //write.SetHeaderKey("BLDVER", 1, "");
    write.SetHeaderKey("RUNID", header.GetRunNumber(), "");
    write.SetHeaderKey("NBOARD", 40, "");
    write.SetHeaderKey("NPIX", header.GetNumPixel(), "");
    write.SetHeaderKey("NROI", header.GetNumSamplesHiGain(), "");
    write.SetHeaderKey("NROITM", 0, "");
    write.SetHeaderKey("TMSHIFT", 0, "");
    write.SetHeaderKey("CAMERA", "MGeomCamFACT", "");
    write.SetHeaderKey("DAQ", "DRS4", "");

    // FTemme: ADCRANGE and ADC have to be calculated, using the values for
    // the fadctype.
//    write.SetHeaderKey("ADCRANGE", 2000, "Dynamic range in mV");
//    write.SetHeaderKey("ADC", 12, "Resolution in bits");

    switch(header.GetRunType())
    {
    case MRawRunHeader::kRTData|MRawRunHeader::kRTMonteCarlo:
        write.SetHeaderKey("RUNTYPE", "data", "");
        break;
    case MRawRunHeader::kRTPedestal|MRawRunHeader::kRTMonteCarlo:
        write.SetHeaderKey("RUNTYPE", "pedestal", "");
        break;
    case MRawRunHeader::kRTCalibration|MRawRunHeader::kRTMonteCarlo:
        write.SetHeaderKey("RUNTYPE", "calibration", "");
        break;
    }
//    write.SetHeaderKey("ID", 777, "Board  0: Board ID");
//    write.SetHeaderKey("FMVER", 532, "Board  0: Firmware Version");
//    write.SetHeaderKey("DNA", "0", "");
//    write.SetHeaderKey("BOARD", 0, "");
//    write.SetHeaderKey("PRESC", 40, "");
//    write.SetHeaderKey("PHASE", 0, "");
//    write.SetHeaderKey("DAC0", 26500, "");
//    write.SetHeaderKey("DAC1", 0, "");
//    write.SetHeaderKey("DAC2", 0, "");
//    write.SetHeaderKey("DAC3", 0, "");
//    write.SetHeaderKey("DAC4", 28800, "");
//    write.SetHeaderKey("DAC5", 28800, "");
//    write.SetHeaderKey("DAC6", 28800, "");
//    write.SetHeaderKey("DAC7", 28800, "");
    write.SetHeaderKey("REFCLK", header.GetFreqSampling(), "");
    write.SetHeaderKey("DRSCALIB", false, "");
//    write.SetHeaderKey("TSTARTI", 0, "");
//    write.SetHeaderKey("TSTARTF", 0., "");
//    write.SetHeaderKey("TSTOPI", 0, "");
//    write.SetHeaderKey("TSTOPF", 0., "");
//    write.SetHeaderKey("DATE-OBS", "1970-01-01T00:00:00", "");
//    write.SetHeaderKey("DATE-END", "1970-01-01T00:00:00", "");
//    write.SetHeaderKey("NTRG", 0, "");
//    write.SetHeaderKey("NTRGPED", 0, "");
//    write.SetHeaderKey("NTRGLPE", 0, "");
//    write.SetHeaderKey("NTRGTIM", 0, "");
//    write.SetHeaderKey("NTRGLPI", 0, "");
//    write.SetHeaderKey("NTRGEXT1", 0, "");
//    write.SetHeaderKey("NTRGEXT2", 0, "");
//    write.SetHeaderKey("NTRGMISC", 0, "");
}

void MJSimulation::SetupVetoColumns(MWriteFitsFile &write) const
{
    write.VetoColumn("MParameterD.fVal");
    write.VetoColumn("MRawEvtData.fLoGainPixId");
    write.VetoColumn("MRawEvtData.fLoGainFadcSamples");
    write.VetoColumn("MRawEvtData.fABFlags");
    write.VetoColumn("MRawEvtHeader.fNumTrigLvl2");
    //write.VetoColumn("MRawEvtHeader.fTrigPattern");
    write.VetoColumn("MRawEvtHeader.fNumLoGainOn");
}

// Setup the header accordingly to the used operation mode
void MJSimulation::SetupHeaderOperationMode(MRawRunHeader &header) const
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

void MJSimulation::CreateBinningObjects(MParList &plist) const
{
    MBinning *binse = (MBinning*) plist.FindCreateObj("MBinning","BinningEnergy");
    binse->SetEdges(120,      1,  1000000,"log");
    MBinning *binsth = (MBinning*) plist.FindCreateObj("MBinning","BinningThreshold");
    binsth->SetEdges( 60,   0.9,   900000, "log");
    MBinning *binsee = (MBinning*) plist.FindCreateObj("MBinning","BinningEnergyEst");
    binsee->SetEdges( 36,   0.9,   900000, "log");
    MBinning *binss = (MBinning*) plist.FindCreateObj("MBinning","BinningSize");
    binss->SetEdges( 100,     1, 10000000,      "log");
    MBinning *binsi = (MBinning*) plist.FindCreateObj("MBinning","BinningImpact");
    binsi->SetEdges(  32,     0,      800);
    MBinning *binsh = (MBinning*) plist.FindCreateObj("MBinning","BinningHeight");
    binsh->SetEdges( 150,     0,       50);
    MBinning *binsaz = (MBinning*) plist.FindCreateObj("MBinning","BinningAz");
    binsaz->SetEdges(720,  -360,      360);
    MBinning *binszd = (MBinning*) plist.FindCreateObj("MBinning","BinningZd");
    binszd->SetEdges( 70,     0,       70);
    MBinning *binsvc = (MBinning*) plist.FindCreateObj("MBinning","BinningViewCone");
    binsvc->SetEdges( 35,     0,        7);
    MBinning *binsel = (MBinning*) plist.FindCreateObj("MBinning","BinningTotLength");
    binsel->SetEdges(150,     0,       50);
    MBinning *binsew = (MBinning*) plist.FindCreateObj("MBinning","BinningMedLength");
    binsew->SetEdges(150,     0,       15);
    plist.FindCreateObj("MBinning","BinningDistC");
    plist.FindCreateObj("MBinning","BinningDist");
    plist.FindCreateObj("MBinning","BinningTrigPos");
}

Bool_t MJSimulation::Process(const MArgs &args, const MSequence &seq)
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
    // - list of the to processed sequence
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
    // FIXME: Allow empty GeomCones!
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
    // Setup container for the "camera array" of the extracted number of photons
    // --------------------------------------------------------------------------------
    // from ExtractorRndm
    plist.FindCreateObj("MPedPhotCam", "MPedPhotFromExtractorRndm");
    // --------------------------------------------------------------------------------
    // Setup spline containers for:
    // --------------------------------------------------------------------------------
    // - the pulse shape
    // - the different photon acceptance splines
    MParSpline shape("PulseShape");
    MParSpline splinepde("PhotonDetectionEfficiency");
    MParSpline splinemirror("MirrorReflectivity");
    MParSpline splinecones("ConesAngularAcceptance");
    MParSpline splinecones2("ConesTransmission");
    MParSpline splineAdditionalPhotonAcceptance("AdditionalPhotonAcceptance");
    plist.AddToList(&shape);
    plist.AddToList(&splinepde);
    plist.AddToList(&splinemirror);
    plist.AddToList(&splinecones);
    plist.AddToList(&splinecones2);
    plist.AddToList(&splineAdditionalPhotonAcceptance);

    // --------------------------------------------------------------------------------
    // Setup container for the MC run header and the Raw run header
    // --------------------------------------------------------------------------------
    plist.FindCreateObj("MMcRunHeader");

    MRawRunHeader header;
    header.SetValidMagicNumber();
    SetupHeaderOperationMode(header);
    // FIXME: Move to MSimPointingPos, MSimCalibrationSignal
    //        Can we use this as input for MSimPointingPos?
    header.SetObservation("On", "MonteCarlo");
    plist.AddToList(&header);

    // --------------------------------------------------------------------------------
    // Setup container for the intended pulse position and for the trigger position
    // --------------------------------------------------------------------------------
    MParameterD pulpos("IntendedPulsePos");
    // FIXME: Set a default which could be 1/3 of the digitization window
    //    pulpos.SetVal(40);  // [ns]
    plist.AddToList(&pulpos);

    MParameterD trigger("TriggerPos");
    trigger.SetVal(0);
    plist.AddToList(&trigger);


    // --------------------------------------------------------------------------------
    // Setup container for the fix time offset, for residual time spread and for gapd time jitter
    // --------------------------------------------------------------------------------
    // Dominik and Sebastian on: fix time offsets
    MMatrix fix_time_offsets_between_pixels_in_ns(
        "MFixTimeOffset","titel"
    );
    plist.AddToList(&fix_time_offsets_between_pixels_in_ns);

    // Jens Buss on: residual time spread
    MParameterD resTimeSpread("ResidualTimeSpread");
    resTimeSpread.SetVal(0.0);
    plist.AddToList(&resTimeSpread);
    // Jens Buss on: residual time spread
    MParameterD gapdTimeJitter("GapdTimeJitter");
    gapdTimeJitter.SetVal(0.0);
    plist.AddToList(&gapdTimeJitter);

    // --------------------------------------------------------------------------------
    // Creation of binning objects and apply default settings
    // --------------------------------------------------------------------------------
    CreateBinningObjects(plist);

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // Tasks Setup (Reading, Absorption, Reflector)
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
    // Precut
    // --------------------------------------------------------------------------------
    MContinue precut("", "PreCut");
    precut.IsInverted();
    precut.SetAllowEmpty();

    // --------------------------------------------------------------------------------
    // MSimMMCS (don't know what this is doing) and calculation of the incident angle
    // --------------------------------------------------------------------------------
    MSimMMCS simmmcs;
    const TString sin2 = "sin(MCorsikaEvtHeader.fZd)*sin(MCorsikaRunHeader.fZdMin*TMath::DegToRad())";
    const TString cos2 = "cos(MCorsikaEvtHeader.fZd)*cos(MCorsikaRunHeader.fZdMin*TMath::DegToRad())";
    const TString cos  = "cos(MCorsikaEvtHeader.fAz-MCorsikaRunHeader.fAzMin*TMath::DegToRad())";

    const TString form = "acos("+sin2+"*"+cos+"+"+cos2+")*TMath::RadToDeg()";

    MParameterCalc calcangle(form, "CalcIncidentAngle");
    calcangle.SetNameParameter("IncidentAngle");

    // --------------------------------------------------------------------------------
    // Absorption tasks
    // --------------------------------------------------------------------------------
    // - Atmosphere (simatm)
    // - photon detection efficiency (absapd)
    // - mirror reflectivity (absmir)
    // - angular acceptance of winston cones (cones)
    // - transmission of winston cones (cones2)
    // - additional photon acceptance (only motivated, not measured) (additionalPhotonAcceptance)
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
    additionalPhotonAcceptance.SetForce(kTRUE);
 
    // --------------------------------------------------------------------------------
    // Simulating pointing position and simulating reflector
    // --------------------------------------------------------------------------------
    MSimPointingPos pointing;
    MSrcPosCalc     srcposcam;

    MSimReflector reflect;
    reflect.SetNameGeomCam("GeomCones");
    reflect.SetNameReflector("Reflector");
//  MSimStarField stars;

    // --------------------------------------------------------------------------------
    // Continue tasks
    // --------------------------------------------------------------------------------
    // - Processing of the current events stops, if there aren't at least two photons (see cont3)
    MContinue cont1("MPhotonEvent.GetNumPhotons<1", "ContEmpty1");
    MContinue cont2("MPhotonEvent.GetNumPhotons<1", "ContEmpty2");
    MContinue cont3("MPhotonEvent.GetNumPhotons<2", "ContEmpty3");
    cont1.SetAllowEmpty();
    cont2.SetAllowEmpty();
    cont3.SetAllowEmpty();

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // Tasks Setup (Cones, SiPM-Simulation, RandomPhotons, Camera, Trigger, DAQ)
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // --------------------------------------------------------------------------------
    // GeomApply, SimPSF, SimGeomCam, SimCalibrationSignal
    // --------------------------------------------------------------------------------
    // - MGeomApply only resizes all MGeomCam parameter container to the actual
    //   number of pixels
    // - MSimPSF applies an additional smearing of the photons on the camera window
    //   on default it is switched of (set to 0), but can be applied by setting:
    //   MSimPSF.fSigma: <value>
    //   in the config file.
    //   Be aware, that there is also a smearing of the photons implemented in
    //   MSimReflector by setting:
    //   MReflector.SetSigmaPSF: <value>
    //   in the config file. This is at the moment done in the default config file.
    // - MSimGeomCam identifies which pixel was hit by which photon
    // - MSimCalibrationSignal is only used for simulated calibration runs
    MGeomApply apply;

    MSimPSF simpsf;

    MSimGeomCam simgeom;
    simgeom.SetNameGeomCam("GeomCones");

    MSimCalibrationSignal simcal;
    simcal.SetNameGeomCam("GeomCones");

    // --------------------------------------------------------------------------------
    // Simulation of random photons
    // --------------------------------------------------------------------------------
    // - be aware that here default values for the nsb rate and for the dark
    //   count rate are set. They will be overwritten if the values are defined
    //   in the config file
    // - if you want to simulate a specific nsb rate you have to set the filename
    //   in the config file to an empty string and then you can specify the NSB rate:
    //   MSimRandomPhotons.FileNameNSB:
    //   MSimRandomPhotons.FrequencyNSB: 0.0

    // Sky Quality Meter: 21.82M = 2.02e-4cd/m^2
    // 1cd = 12.566 lm / 4pi sr
    // FIXME: Simulate photons before cones and QE!
    MSimRandomPhotons  simnsb;
    simnsb.SetFreq(5.8, 0.004);  // ph/m^2/nm/sr/ns NSB, 4MHz dark count rate
    simnsb.SetNameGeomCam("GeomCones");
    // FIXME: How do we handle star-light? We need the rate but we also
    //        need to process it through the mirror!

    // --------------------------------------------------------------------------------
    // Simulation from the SiPM to the DAQ
    // --------------------------------------------------------------------------------
    // - MSimAPD simulates the whole behaviour of the SiPMs:
    //   (dead time of cells, crosstalk, afterpulses)
    // - MSimExcessNoise adds a spread on the weight of each signal in the SiPMs
    // - MSimBundlePhotons restructured the photon list of MPhotonEvent via a look
    //   up table. At the moment the tasks is skipped, due to the fact that there
    //   is no file name specified in the config file
    // - MSimSignalCam only fills a SignalCam container for the cherenkov photons
    // - MSimCamera simulates the behaviour of the camera:
    //   (electronic noise, accoupling, time smearing and offset for the photons,
    //   pulses injected by all photons)
    // - MSimTrigger simulates the behaviour of the Trigger:
    //   (Adding patch voltages, applying clipping, applying discriminator, applying
    //   time over threshold and a possible trigger logic)
    // - MContinue conttrig stops the processing of the current event if the trigger
    //   position is negativ (and therefore not valid)
    // - MSimReadout simulates the behaviour of the readout:
    //   (Digitization and saturation of the ADC)
    MSimAPD simapd;
    simapd.SetNameGeomCam("GeomCones");

    MSimExcessNoise   simexcnoise;
    MSimBundlePhotons simsum;
    MSimSignalCam     simsignal;
    MSimCamera        simcam;
    MSimTrigger       simtrig;
    MContinue conttrig("TriggerPos.fVal<0", "ContTrigger");
    MSimReadout       simdaq;

    // --------------------------------------------------------------------------------
    // Standard analysis with hillas parameters for the true cherenkov photons
    // --------------------------------------------------------------------------------
    // Remove isolated pixels
    MImgCleanStd clean(0, 0);
    //clean.SetCleanLvl0(0); // The level above which isolated pixels are kept
    clean.SetCleanRings(0);
    clean.SetMethod(MImgCleanStd::kAbsolute);

    //clean.SetNamePedPhotCam("MPedPhotFromExtractorRndm");

    MHillasCalc hcalc;
    hcalc.Disable(MHillasCalc::kCalcConc);


    // --------------------------------------------------------------------------------
    // Setup of histogram containers and the corresponding fill tasks
    // --------------------------------------------------------------------------------
    // Here is no functionality for the simulation, it is only visualization via
    // showplot
    MHn mhn1, mhn2, mhn3, mhn4;
    SetupHist(mhn1);
    SetupHist(mhn2);
    SetupHist(mhn3);
    SetupHist(mhn4);

    MH3 mhtp("TriggerPos.fVal-IntendedPulsePos.fVal-PulseShape.GetWidth");
    mhtp.SetName("TrigPos");
    mhtp.SetTitle("Trigger position w.r.t. the first photon hitting a detector");

    MH3 mhew("MPhotonStatistics.fLength");
    mhew.SetName("TotLength");
    mhew.SetTitle("Time between first and last photon hitting a detector;L [ns]");

    MH3 mhed("MPhotonStatistics.fTimeMedDev");
    mhed.SetName("MedLength");
    mhed.SetTitle("Median deviation (1\\sigma);L [ns]");

    MFillH fillh1(&mhn1, "", "FillCorsika");
    MFillH fillh2(&mhn2, "", "FillH2");
    MFillH fillh3(&mhn3, "", "FillH3");
    MFillH fillh4(&mhn4, "", "FillH4");
    MFillH filltp(&mhtp, "", "FillTriggerPos");
    MFillH fillew(&mhew, "", "FillEvtWidth");
    MFillH filled(&mhed, "", "FillMedDev");
    fillh1.SetNameTab("Corsika",    "Distribution as simulated by Corsika");
    fillh2.SetNameTab("Detectable", "Distribution of events hit the detector");
    fillh3.SetNameTab("Triggered",  "Distribution of triggered events");
    fillh4.SetNameTab("Cleaned",    "Distribution after cleaning and cuts");
    filltp.SetNameTab("TrigPos",    "TriggerPosition w.r.t the first photon");
    fillew.SetNameTab("EvtWidth",   "Time between first and last photon hitting a detector");
    filled.SetNameTab("MedDev",     "Median deviation of arrival time of photons hitting a detector");

    MHPhotonEvent planeG(1, "HPhotonEventGround");     // Get from MaxImpact
    MHPhotonEvent plane0(2, "HMirrorPlane0");     // Get from MReflector
    //MHPhotonEvent plane1(2, "HMirrorPlane1");
    MHPhotonEvent plane2(2, "HMirrorPlane2");
    MHPhotonEvent plane3(2, "HMirrorPlane3");
    MHPhotonEvent plane4(2, "HMirrorPlane4");
    MHPhotonEvent planeF1(6, "HPhotonEventCamera");   // Get from MGeomCam
    MHPhotonEvent planeF2(header.IsPointRun()?4:6, "HPhotonEventCones"); // Get from MGeomCam

    plist.AddToList(&planeG);
    plist.AddToList(&plane0);
    plist.AddToList(&plane2);
    plist.AddToList(&plane3);
    plist.AddToList(&plane4);
    plist.AddToList(&planeF1);
    plist.AddToList(&planeF2);

    //MHPSF psf;

    MFillH fillG(&planeG,   "MPhotonEvent", "FillGround");
    MFillH fill0(&plane0,   "MirrorPlane0", "FillReflector");
    //MFillH fill1(&plane1,   "MirrorPlane1", "FillCamShadow");
    MFillH fill2(&plane2,   "MirrorPlane2", "FillCandidates");
    MFillH fill3(&plane3,   "MirrorPlane3", "FillReflected");
    MFillH fill4(&plane4,   "MirrorPlane4", "FillFocal");
    MFillH fillF1(&planeF1, "MPhotonEvent", "FillCamera");
    MFillH fillF2(&planeF2, "MPhotonEvent", "FillCones");
    //MFillH fillP(&psf,      "MPhotonEvent", "FillPSF");

    fillG.SetNameTab("Ground",     "Photon distribution at ground");
    fill0.SetNameTab("Reflector",  "Photon distribution at reflector plane w/o camera shadow");
    //fill1.SetNameTab("CamShadow",  "Photon distribution at reflector plane w/ camera shadow");
    fill2.SetNameTab("Candidates", "*Can hit* photon distribution at reflector plane w/ camera shadow");
    fill3.SetNameTab("Reflected",  "Photon distribution after reflector projected to reflector plane");
    fill4.SetNameTab("Focal",      "Photon distribution at focal plane");
    fillF1.SetNameTab("Camera",    "Photon distribution which hit the detector");
    fillF2.SetNameTab("Cones",     "Photon distribution after cones");
    //fillP.SetNameTab("PSF",        "Photon distribution after cones for all mirrors");


    MHCamEvent evt0a(/*10*/3, "Signal",    "Average signal (absolute);;S [ph]");
    MHCamEvent evt0c(/*10*/3, "MaxSignal", "Maximum signal (absolute);;S [ph]");
    MHCamEvent evt0d(/*11*/8, "ArrTm",     "Time after first photon;;T [ns]");
    evt0a.SetErrorSpread(kFALSE);
    evt0c.SetCollectMax();

    MContinue cut("", "Cut");

    MFillH fillx0a(&evt0a,             "MSignalCam",      "FillAvgSignal");
    MFillH fillx0c(&evt0c,             "MSignalCam",      "FillMaxSignal");
    MFillH fillx0d(&evt0d,             "MSignalCam",      "FillArrTm");
    MFillH fillx1("MHHillas",          "MHillas",         "FillHillas");
    MFillH fillx3("MHHillasSrc",       "MHillasSrc",      "FillHillasSrc");
    MFillH fillx4("MHNewImagePar",     "MNewImagePar",    "FillNewImagePar");
    MFillH fillth("MHThreshold",       "",                "FillThreshold");
    MFillH fillca("MHCollectionArea",  "",                "FillTrigArea");

    fillth.SetNameTab("Threshold");
    fillca.SetNameTab("TrigArea");

    // --------------------------------------------------------------------------------
    // Formating of the outputfilepath
    // --------------------------------------------------------------------------------
    if (!fFileOut.IsNull())
    {
        const Ssiz_t dot   = fFileOut.Last('.');
        const Ssiz_t slash = fFileOut.Last('/');
        if (dot>slash)
            fFileOut = fFileOut.Remove(dot);
    }
    const char *fmt = fFileOut.IsNull() ?
        Form("s/[cC][eE][rR]([0-9]+)([0-9][0-9][0-9])/%s\\/%08d.$2%%s_MonteCarlo$1.root/", Esc(fPathOut).Data(), header.GetRunNumber()) :
        Form("%s/%s%%s.root", Esc(fPathOut).Data(), Esc(fFileOut).Data());

    const TString rule1(Form(fmt, fFileOut.IsNull()?"_R":""));
    const TString rule2(Form(fmt, "_Y"));
    const TString rule4(Form(fmt, "_I"));
    TString rule3(Form(fmt, Form("_%c", header.GetRunTypeChar())));

    // --------------------------------------------------------------------------------
    // Setup of the outputfile tasks
    // --------------------------------------------------------------------------------
    MWriteRootFile write4a( 2, rule4, fOverwrite?"RECREATE":"NEW", "Star file");
    MWriteRootFile write4b( 2, rule4, fOverwrite?"RECREATE":"NEW", "Star file");
    MWriteRootFile write3b( 2, rule3, fOverwrite?"RECREATE":"NEW", "Camera file");
    MWriteRootFile write2a( 2, rule2, fOverwrite?"RECREATE":"NEW", "Signal file");
    MWriteRootFile write2b( 2, rule2, fOverwrite?"RECREATE":"NEW", "Signal file");
    MWriteRootFile write1a( 2, rule1, fOverwrite?"RECREATE":"NEW", "Reflector file");
    MWriteRootFile write1b( 2, rule1, fOverwrite?"RECREATE":"NEW", "Reflector file");

    if (fWriteFitsFile)
        rule3.ReplaceAll(".root", ".fits");

    MWriteFitsFile write3af( 2, rule3, fOverwrite?"RECREATE":"NEW", "Camera file");
    MWriteRootFile write3ar( 2, rule3, fOverwrite?"RECREATE":"NEW", "Camera file");

    MTask &write3a = fWriteFitsFile ? static_cast<MTask&>(write3af) : static_cast<MTask&>(write3ar);

    //SetupHeaderKeys(write3af,header);
    SetupVetoColumns(write3af);

    write3af.SetBytesPerSample("Data", 2);

    write1a.SetName("WriteRefData");
    write1b.SetName("WriteRefMC");
    write2a.SetName("WriteSigData");
    write2b.SetName("WriteSigMC");
    write3a.SetName("WriteCamData");
    write3b.SetName("WriteCamMC");
    write4a.SetName("WriteImgData");
    write4b.SetName("WriteImgMC");

    SetupCommonFileStructure(write1a);
    SetupCommonFileStructure(write2a);
    SetupCommonFileStructure(write3ar);
    SetupCommonFileStructure(write3af);
    SetupCommonFileStructure(write4a);

    // R: Dedicated file structure
    write1a.AddContainer("MPhotonEvent", "Events");

    // Y: Dedicated file structure
    write2a.AddContainer("MPedPhotFromExtractorRndm", "RunHeaders", kTRUE, 1); // FIXME: Needed for the signal files to be display in MARS
    write2a.AddContainer("MSignalCam", "Events");

    // D: Dedicated file structure
    write3af.AddContainer("ElectronicNoise",  "RunHeaders", kTRUE, 1);
    write3af.AddContainer("IntendedPulsePos", "RunHeaders", kTRUE, 1);
    write3af.AddContainer("ResidualTimeSpread", "RunHeaders", kTRUE, 1);
    write3af.AddContainer("GapdTimeJitter", "RunHeaders", kTRUE, 1);
    write3af.AddContainer("MRawEvtData",      "Events");
    write3af.AddContainer("MTruePhotonsPerPixelCont", "Events");

    write3ar.AddContainer("ElectronicNoise",  "RunHeaders", kTRUE, 1);
    write3ar.AddContainer("IntendedPulsePos", "RunHeaders", kTRUE, 1);
    write3ar.AddContainer("MRawEvtData",      "Events");
    // It doesn't make much sene to write this information
    // to the file because the units are internal units not
    // related to the output samples
    //    if (header.IsDataRun() || fForceTrigger)
    //        write3a.AddContainer("TriggerPos",   "Events");

    // I: Dedicated file structure
    write4a.AddContainer("MHillas",       "Events");
    write4a.AddContainer("MHillasSrc",    "Events");
    write4a.AddContainer("MImagePar",     "Events");
    write4a.AddContainer("MNewImagePar",  "Events");

    // Basic MC data
    write1b.AddContainer("MMcEvtBasic", "OriginalMC");
    write2b.AddContainer("MMcEvtBasic", "OriginalMC");
    write3b.AddContainer("MMcEvtBasic", "OriginalMC");
    write4b.AddContainer("MMcEvtBasic", "OriginalMC");


    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // Filling of tasks in tasklist
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    if (header.IsDataRun())
    {
        tasks.AddToList(&read);  // Reading Corsika
        tasks.AddToList(&precut);  // Precut
        tasks.AddToList(&pointing);  // Simulating pointing
        tasks.AddToList(&srcposcam);  // calculate origin in camera
        tasks.AddToList(&simmmcs);  // Simulating MMCS
        if (!fPathOut.IsNull() && !HasNullOut())  // Write Tasks for corsika infos
        {
            //tasks.AddToList(&write1b);
            tasks.AddToList(&write2b);
            if (fCamera)
                tasks.AddToList(&write3b);
            if (header.IsDataRun())
                tasks.AddToList(&write4b);
        }
        //    if (header.IsPointRun())
        //        tasks.AddToList(&stars);
        if (1)
            tasks.AddToList(&simatm); // Here because before fillh1  // atmosphere absorption
        tasks.AddToList(&calcangle);  // calculation incident angle
        tasks.AddToList(&fillh1);  // fill histogram task
        tasks.AddToList(&fillG);  // fill histogram task
        if (!header.IsPointRun())
        {
            tasks.AddToList(&absapd);  // photon detection efficiency of the apds
            tasks.AddToList(&absmir);  // mirror reflectivity
            tasks.AddToList(&additionalPhotonAcceptance);  // addition photon acceptance
            if (0)
                tasks.AddToList(&simatm); // FASTER?  // be aware this is 'commented'
        }
        tasks.AddToList(&reflect);  // Simulation of the reflector
        if (!header.IsPointRun())
        {
            tasks.AddToList(&fill0);  // fill histogram task
            //tasks.AddToList(&fill1);
            tasks.AddToList(&fill2);  // fill histogram task
            tasks.AddToList(&fill3);  // fill histogram task
            tasks.AddToList(&fill4);  // fill histogram task
            tasks.AddToList(&fillF1);  // fill histogram task
        }
        tasks.AddToList(&cones);  // angular acceptance of winston cones
        tasks.AddToList(&cones2);  // transmission of winston cones
        //if (header.IsPointRun())
        //    tasks.AddToList(&fillP);
        tasks.AddToList(&cont1);  // continue if at least 2 photons
    }
    // -------------------------------
    tasks.AddToList(&apply);  // apply geometry to MGeomCam containers
    if (header.IsDataRun())
    {
        tasks.AddToList(&simpsf);  // simulates additional psf (NOT used in default mode)
        tasks.AddToList(&simgeom);  // tag which pixel is hit by which photon
        tasks.AddToList(&cont2);  // continue if at least 2 photons
        if (!header.IsPointRun())
        {
            tasks.AddToList(&fillF2);  // fill histogram task
            tasks.AddToList(&fillh2);  // fill histogram task
        }
        tasks.AddToList(&cont3);  // continue if at least 3 photons
    }
    if (fCamera)
    {
        if (header.IsPedestalRun() || header.IsCalibrationRun())
            tasks.AddToList(&simcal);  // add calibration signal for calibration runs
        tasks.AddToList(&simnsb);  // simulate nsb and dark counts
        tasks.AddToList(&simapd);  // simulate SiPM behaviour (dead time, crosstalk ...)
        tasks.AddToList(&simexcnoise);  // add excess noise
    }
    tasks.AddToList(&simsum);  // bundle photons (NOT used in default mode)
    if (fCamera)
    {
        tasks.AddToList(&simcam);  // simulate camera behaviour (creates analog signal)
        if (header.IsDataRun() || fForceTrigger)
            tasks.AddToList(&simtrig);  // simulate trigger
        tasks.AddToList(&conttrig);  // continue if trigger pos is valid
        tasks.AddToList(&simdaq);  // simulate data aquisition
    }
    tasks.AddToList(&simsignal);  // What do we do if signal<0?  // fill MSimSignal container
    if (!fPathOut.IsNull() && !HasNullOut())
    {
        //tasks.AddToList(&write1a);
        if (!header.IsPedestalRun())
            tasks.AddToList(&write2a);  // outputtask
        if (fCamera)
            tasks.AddToList(&write3a);  // outputtask (this is the raw data writing tasks)
    }
    // -------------------------------
    if (fCamera)
    {
        // FIXME: MHCollectionArea Trigger Area!
        if (header.IsDataRun())
            tasks.AddToList(&fillh3);  // fill histogram task
        tasks.AddToList(&filltp);  // fill histogram task
    }
    if (header.IsDataRun())
    {
        tasks.AddToList(&fillew);  // fill histogram task
        tasks.AddToList(&filled);  // fill histogram task
    }
    if (!header.IsPedestalRun())
    {
        tasks.AddToList(&fillx0a);  // fill histogram task
        tasks.AddToList(&fillx0c);  // fill histogram task
    }
    if (header.IsDataRun())
    {
        tasks.AddToList(&clean);  // Cleaning for standard analysis
        tasks.AddToList(&hcalc);  // Hillas parameter calculation
        tasks.AddToList(&cut);
        tasks.AddToList(&fillx0d);  // fill histogram task
        tasks.AddToList(&fillx1);  // fill histogram task
        //tasks.AddToList(&fillx2);
        tasks.AddToList(&fillx3);  // fill histogram task
        tasks.AddToList(&fillx4);  // fill histogram task
        if (!HasNullOut())
            tasks.AddToList(&write4a);
        //tasks.AddToList(&fillx5);

        tasks.AddToList(&fillh4);  // fill histogram task
        tasks.AddToList(&fillth);  // fill histogram task
        tasks.AddToList(&fillca);  // fill histogram task
    }


    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // Event loop and processing display
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // --------------------------------------------------------------------------------
    // Creation of event loop
    // --------------------------------------------------------------------------------
    tasks.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    // Create and setup the eventloop
    MEvtLoop evtloop(fName);
    evtloop.SetParList(&plist);
    evtloop.SetDisplay(fDisplay);
    evtloop.SetLogStream(&gLog);
    if (!SetupEnv(evtloop))
        return kFALSE;

    // FIXME: If pedestal file given use the values from this file


    // --------------------------------------------------------------------------------
    // Preparation of the graphicalk display which is shown while processing
    // --------------------------------------------------------------------------------

    MGeomCam *cam = static_cast<MGeomCam*>(env2.GetCont());


    MBinning *binsd  = (MBinning*) plist.FindObject("BinningDistC");
    MBinning *binsd0 = (MBinning*) plist.FindObject("BinningDist");
    MBinning *binstr = (MBinning*) plist.FindObject("BinningTrigPos");
    if (binstr->IsDefault())
        binstr->SetEdgesLin(150, -shape.GetWidth(),
                           header.GetFreqSampling()/1000.*header.GetNumSamples()+shape.GetWidth());

    if (binsd->IsDefault() && cam)
        binsd->SetEdgesLin(100, 0, cam->GetMaxRadius()*cam->GetConvMm2Deg());

    if (binsd0->IsDefault() && cam)
        binsd0->SetEdgesLin(100, 0, cam->GetMaxRadius()*cam->GetConvMm2Deg());


    header.Print();

    // FIXME: Display acceptance curves!

    if (fDisplay)
    {
        TCanvas &c = fDisplay->AddTab("Info");
        c.Divide(2,2);

        c.cd(1);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        gROOT->SetSelectedPad(0);
        shape.DrawClone()->SetBit(kCanDelete);

        if (env0.GetCont() && (header.IsDataRun() || header.IsPointRun()))
        {
            c.cd(3);
            gPad->SetBorderMode(0);
            gPad->SetFrameBorderMode(0);
            gPad->SetGridx();
            gPad->SetGridy();
            gROOT->SetSelectedPad(0);
            static_cast<MReflector*>(env0.GetCont())->DrawClone("line")->SetBit(kCanDelete);
        }

        if (fCamera)
        {
            if (env1.GetCont())
            {
                c.cd(2);
                gPad->SetBorderMode(0);
                gPad->SetFrameBorderMode(0);
                gPad->SetGridx();
                gPad->SetGridy();
                gROOT->SetSelectedPad(0);
                MHCamera *c = new MHCamera(static_cast<MGeomCam&>(*env1.GetCont()));
                c->SetStats(kFALSE);
                c->SetBit(MHCamera::kNoLegend);
                c->SetBit(kCanDelete);
                c->Draw();
            }

            if (cam)
            {
                c.cd(4);
                gPad->SetBorderMode(0);
                gPad->SetFrameBorderMode(0);
                gPad->SetGridx();
                gPad->SetGridy();
                gROOT->SetSelectedPad(0);
                MHCamera *c = new MHCamera(*cam);
                c->SetStats(kFALSE);
                c->SetBit(MHCamera::kNoLegend);
                c->SetBit(kCanDelete);
                c->Draw();
            }
        }

        TCanvas &d = fDisplay->AddTab("Info2");
        d.Divide(2,3);

        d.cd(1);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        gROOT->SetSelectedPad(0);
        splinepde.DrawClone()->SetBit(kCanDelete);

        d.cd(3);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        gROOT->SetSelectedPad(0);
        splinecones2.DrawClone()->SetBit(kCanDelete);

        d.cd(5);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        gROOT->SetSelectedPad(0);
        splinemirror.DrawClone()->SetBit(kCanDelete);

        d.cd(2);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        gROOT->SetSelectedPad(0);
        splinecones.DrawClone()->SetBit(kCanDelete);
        //splinecones2.DrawClone("same")->SetBit(kCanDelete);

        d.cd(6);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        gROOT->SetSelectedPad(0);
        MParSpline *all = (MParSpline*)splinepde.DrawClone();
        //all->SetTitle("Combined acceptance");
        all->SetBit(kCanDelete);
        if (splinemirror.GetSpline())
            all->Multiply(*splinemirror.GetSpline());
        if (splinecones2.GetSpline())
            all->Multiply(*splinecones2.GetSpline());
    }

    // --------------------------------------------------------------------------------
    // Perform event loop
    // --------------------------------------------------------------------------------

    // Execute first analysis
    if (!evtloop.Eventloop(fMaxEvents))
    {
        gLog << err << GetDescriptor() << ": Failed." << endl;
        return kFALSE;
    }

    //-------------------------------------------
    // FIXME: Display Spline     in tab
    // FIXME: Display Reflector  in tab
    // FIXME: Display Routing(?) in tab
    // FIXME: Display Camera(s)  in tab
    //-------------------------------------------

    if (!WriteResult(plist, seq, header.GetRunNumber()))
        return kFALSE;

    *fLog << all << GetDescriptor() << ": Done." << endl << endl << endl;;

    return kTRUE;
}
