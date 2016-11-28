<?php

//MC stuff
function PrintMCRunInfoMenu($host,$user,$pw,$db)
{
    printf(" <div id='info' style='display:none'>");
    printf(" <table>\n");
    printf("  <tr valign='top'>\n");

    CheckBox("fNumEvents",                  "Num of showers");
    CheckBox("fNumEvents*fNumReUseShowers", "Num of events");
    CheckBox("fNumReUseShowers",            "reused");
    CheckBox("fSequenceNumber",             "Sequ#");

    printf("  </tr><tr>\n");

    CheckBox("fImpactMax",                  "Impact");
    CheckBox("fViewConeMax",                "Viewcone");
    CheckBox("fStartingAltitude",           "Starting alt.");
    CheckBox("fMirrorDiameter",             "Mirror diam.");

    printf("  </tr><tr>\n");

    CheckBox("fZenithDistanceMin",           "Zenith dist. min");
    CheckBox("fZenithDistanceMax",           "Zenith dist. max");
    CheckBox("fAzimuthMin",                  "Azimuth min");
    CheckBox("fAzimuthMax",                  "Azimuth max");

    printf("  </tr><tr>\n");

    CheckBox("fEnergyMin",                   "Emin");
    CheckBox("fEnergyMax",                   "Emax");
    CheckBox("fEnergySlope",                 "Slope");

    printf("  </tr><tr>\n");
    printf("  <td>\n");
    PrintPullDown($host, $user, $pw, $db, "ParticleType",     "fParticleTypeName",     "fParticleTypeKEY", "Particle type");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "RunType",     "fRunTypeName",     "fRunTypeKEY", "Run type");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "AtmosphericModel",     "fAtmosphericModelName",     "fAtmosphericModelKEY", "Atm. model");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "CeresSetup",     "fCeresSetupName",     "fCeresSetupKEY", "Ceres setup");
    printf("  </td>\n");

    printf("  </tr>\n");
    printf(" </table>\n");
    printf(" </div><p>\n");
}

function PrintMCRunStatMenu()
{
    printf(" <div id=\"stat\" style='display:none'>\n");
    printf(" <table>\n");
    printf("  <tr><td>\n");

    PrintStatusMenu("Corsika",    "Corsika");
    printf("  </td><td>\n");
    PrintStatusMenu("Ceres",    "Ceres");
    printf("  </td><td>\n");
    PrintStatusMenu("SequenceFile",    "SequenceFile");

    printf("  </td></tr><tr><td>\n");

    PrintStatusMenu("Callisto",    "Callisto");
    printf("  </td><td>\n");
    PrintStatusMenu("Star",    "Star");

    printf(" </td></tr></table>\n");
    printf(" <p>\n");
    printf(" </div><p>\n");
}

//cta mc stuff
function PrintCTAMCRunInfoMenu($host,$user,$pw,$db)
{
    printf(" <div id='info' style='display:none'>");
    printf(" <table>\n");
    printf("  <tr valign='top'>\n");

    CheckBox("fNumEvents",        "Num of events");
    CheckBox("fObsLevel",         "Observation level");
    CheckBox("fLinks",            "Links");

    printf("  </tr><tr><td>\n");
    PrintPullDown($host, $user, $pw, $db, "ParticleType",     "fParticleTypeName",     "fParticleTypeKEY", "Particle type");
    printf("  </td><td>\n");

    printf("  </tr>\n");
    printf(" </table>\n");
    printf(" </div><p>\n");
}

function PrintCTAMCRunStatMenu()
{
    printf(" <div id=\"stat\" style='display:none'>\n");
    printf(" <table>\n");
    printf("  <tr><td>\n");

    PrintStatusMenu("fCorsikaSimTelarray",    "Corsika and Simtel");
    printf("  </td><td>\n");
    PrintStatusMenu("fChimp",   "Chimp");
    printf("  </td><td>\n");
    PrintStatusMenu("fCTAStar", "Star");

    printf("  </td></tr><tr><td>\n");

    PrintStatusMenu("fStereoB", "StereoB");
    printf("  </td><td>\n");
    PrintStatusMenu("fStereoC", "StereoC");
    printf("  </td><td>\n");
    PrintStatusMenu("fStereoG", "StereoG");

    printf(" </td></tr></table>\n");
    printf(" <p>\n");
    printf(" </div><p>\n");
}

//magic data stuff
function PrintRunInfoMenu()
{
    printf(" <div id='info' style='display:none'>");
    printf(" <table>\n");
    printf("  <tr>\n");

    CheckBox("fRunStart",         "Run start time");
    CheckBox("fL2RatePresc",      "L2 presc Rate");
    CheckBox("fDaqStoreRate",     "DAQ Storage Rate");
    CheckBox("fAzimuth",          "Azimuth");

    printf("  </tr><tr>\n");

    CheckBox("fRunStop",          "Run stop time");
    CheckBox("fL2RateUnpresc",    "L2 unpresc Rate");
    CheckBox("fDaqTriggerRate",   "DAQ Trigger Rate");
    CheckBox("fZenithDistance",   "Zenith distance");

    printf("  </tr><tr>\n");

    //        CheckBox("fFormatVersion",    "File format");
    CheckBox("fNumEvents",        "Num of events");
    CheckBox("fMeanTriggerRate",  "Mean Trigger Rate");
    CheckBox("fL3TriggerRate",    "L3 Trigger Rate");
    CheckBox("fSequenceFirst",    "Sequence Number");

    printf("  </tr><tr>\n");

    CheckBox("fTest",             "incl. TestSources");
    CheckBox("fLinks",            "Links");
    CheckBox("fWheelPos1",        "Wheel Position 1");
    CheckBox("fWheelPos2",        "Wheel Position 2");

    printf("  </tr>\n");
    printf(" </table>\n");
    printf(" </div><p>\n");
}

function PrintRunStatMenu()
{
    printf(" <div id=\"stat\" style='display:none'>\n");
    printf(" <table>\n");
    printf("  <tr><td>\n");

    PrintStatusMenu("fCCFileAvail",    "CC File available");
    printf("  </td><td>\n");
    PrintStatusMenu("fCaCoFileAvail",  "Caco File available");
    printf("  </td><td>\n");
    PrintStatusMenu("fRawFileAvail",   "Rawfile available");

    printf("  </td></tr><tr><td>\n");

    PrintStatusMenu("fTimingCorrection",  "Timing Correction");
    printf("  </td><td>\n");
    PrintStatusMenu("fCaCoFileFound",     "Caco File");
    printf("  </td><td>\n");
    PrintStatusMenu("fDataCheckDone",        "DataCheck");

    printf(" </td></tr></table>\n");
    printf(" <p>\n");
    printf(" </div><p>\n");
}

function PrintRunInfo2Menu($host,$user,$pw,$db)
{
    printf(" <div id='info2' style='display:none'>");
    printf(" <table>\n");
    printf("  <tr><td>\n");
    PrintPullDown($host, $user, $pw, $db, "RunType",     "fRunTypeName",     "fRunTypeKEY", "Run type");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "Source",      "fSourceName",      "fSourceKEY", "Source Name");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "HvSettings",  "fHvSettingsName",  "fHvSettingsKEY", "HV Settings");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "L1TriggerTable", "fL1TriggerTableName", "fL1TriggerTableKEY", "L1 Trigger Table");
    printf("  </td></tr><tr><td>\n");
    PrintPullDown($host, $user, $pw, $db, "TestFlag",  "fTestFlagName",  "fTestFlagKEY", "Test Flag");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "Project",     "fProjectName",     "fProjectKEY", "Project Name");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "DiscriminatorThresholdTable",  "fDiscriminatorThresholdTableName",  "fDiscriminatorThresholdTableKEY", "DT Table");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "L2TriggerTable", "fL2TriggerTableName", "fL2TriggerTableKEY", "L2 Trigger Table");
    printf("  </td></tr><tr><td>\n");
    PrintPullDown($host, $user, $pw, $db, "ExcludedFDA", "fExcludedFDAName", "fExcludedFDAKEY", "Exclusions");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "LightConditions",  "fLightConditionsName",  "fLightConditionsKEY", "Light Conditions");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "CalibrationScript",  "fCalibrationScriptName",  "fCalibrationScriptKEY", "Cal Script");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "L3TriggerTable", "fL3TriggerTableName", "fL3TriggerTableKEY", "L3 Trigger Table");
    printf("  </td></tr><tr><td>\n");
    PrintPullDown($host, $user, $pw, $db, "MagicNumber", "fMagicNumberName", "fMagicNumberKEY", "Magic Number");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "ObservationMode",   "fObservationModeName", "fObservationModeKEY", "Obs. Mode");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "SumTriggerFlag",   "fSumTriggerFlagName", "fSumTriggerFlagKEY", "SumTrigger Flag");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "TriggerDelayTable",  "fTriggerDelayTableName",  "fTriggerDelayTableKEY", "Trigger Delay Table");
    printf("  </td></tr><tr><td>\n");
    PrintPullDown($host, $user, $pw, $db, "Cycle",   "fCycleName", "fCycleKEY", "Cycle");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "PI",   "fPIName", "fPIKEY", "PI");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "WorkingGroup",   "fWorkingGroupName", "fWorkingGroupKEY", "Working Group");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "Proposal",   "fProposalName", "fProposalKEY", "Proposal");
    printf(" </td></tr></table>\n");
    printf(" </div><p>\n");
}

function PrintSequInfoMenu()
{
    printf(" <div id='info' style='display:none'>");
    printf(" <table>\n");

    printf("<tr><td align='center'>\n");
    printf("  <tr>\n");

    CheckBox("fRunStart",          "Start time");
    CheckBox("fRunStop",           "Stop time");
    CheckBox("fNumEvents",         "Num of events");
    CheckBox("fRunTime/60",        "Duration");

    printf("  </tr><tr>\n");

    CheckBox("fZenithDistanceMin", "Zenith distance min");
    CheckBox("fZenithDistanceMax", "Zenith distance max");
    CheckBox("fAzimuthMin",        "Azimuth min");
    CheckBox("fAzimuthMax",        "Azimuth max");

    printf("  </tr><tr>\n");

    CheckBox("fSequenceLast",      "Last run");
    CheckBox("fLinks",             "Links");

    printf("  </tr><tr>\n");

    CheckBox("fOnlySum",           "only sumtrigger");
    CheckBox("fTest",              "incl. TestSources");
    CheckBox("fOff",               "incl. offsources");
    CheckBox("fOnlyOff",           "only offsources");

    printf("  </tr></table>\n");
    printf(" </div><p>\n");
}

function PrintSequInfo2Menu($host,$user,$pw,$db)
{
    printf(" <div id='info2' style='display:none'>\n");
    printf(" <table>\n");
    printf("  <tr><td>\n");
    PrintPullDown($host, $user, $pw, $db, "Source",                      "fSourceName",                      "fSourceKEY",                      "Source Name");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "Project",                     "fProjectName",                     "fProjectKEY",                     "Project Name");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "L1TriggerTable",              "fL1TriggerTableName",              "fL1TriggerTableKEY",              "L1Trigger Table");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "Cycle",   "fCycleName", "fCycleKEY", "Cycle");
    printf("  </td></tr><tr><td>\n");
    PrintPullDown($host, $user, $pw, $db, "LightConditions",             "fLightConditionsName",             "fLightConditionsKEY",             "Light Conditions");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "HvSettings",                  "fHvSettingsName",                  "fHvSettingsKEY",                  "HV Settings");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "L2TriggerTable",              "fL2TriggerTableName",              "fL2TriggerTableKEY",              "L2Trigger Table");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "PI",   "fPIName", "fPIKEY", "PI");
    printf("  </td></tr><tr><td>\n");
    PrintPullDown($host, $user, $pw, $db, "ObservationMode",             "fObservationModeName",             "fObservationModeKEY",             "Obs. Mode");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "DiscriminatorThresholdTable", "fDiscriminatorThresholdTableName", "fDiscriminatorThresholdTableKEY", "DT Table");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "L3TriggerTable",              "fL3TriggerTableName",              "fL3TriggerTableKEY",              "L3Trigger Table");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "WorkingGroup",   "fWorkingGroupName", "fWorkingGroupKEY", "Working Group");
    printf("  </td></tr><tr><td>\n");
    PrintPullDown($host, $user, $pw, $db, "SumTriggerFlag",              "fSumTriggerFlagName",              "fSumTriggerFlagKEY",              "SumTrigger Flag");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "TriggerDelayTable",           "fTriggerDelayTableName",           "fTriggerDelayTableKEY",           "Trigger Delay Table");
    printf("  </td><td>\n");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "Proposal",   "fProposalName", "fProposalKEY", "Proposal");
//    printf("  </td><td>\n");
//    PrintPullDown($host, $user, $pw, $db, "ManuallyChanged",             "fManuallyChangedName",             "fManuallyChangedKEY",             "Manually changed");
//    printf("  </td><td>\n");
//    PrintPullDown($host, $user, $pw, $db, "TestFlag",                    "fTestFlagName",                    "fTestFlagKEY",                    "Test Flag");
    printf("  </td></tr>\n");
    printf(" </table>\n");
    printf(" </div><p>\n");
}

function PrintLimitsMenu($limitsmean, $limitsmin, $limitsmax, $alias, $old)
{
    printf("<div id='limits' style='display:none'>\n");
    printf("  <table>\n");
    printf("  <tr><th colspan='3'>Limits</th></tr>\n");
    printf("  <tr><td valign='top'>\n");

    printf("   <table>\n");
    printf("  <tr><th>Name </th><th> Mean </th><th> Rms </th></tr>\n");

    foreach($limitsmean as $key => $element)
    {
        printf("<tr><td>%s</td>\n", $alias[$key]);
        $mean=$key . "Mean";
        $limitmean=$_GET[$mean];
        printf("<td><input name=\"%sMean\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\"></td>\n", $key, $limitmean);
        $rms2=$key . "Rms";
        $limitrms=$_GET[$rms2];
        printf("<td><input name=\"%sRms\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\"></td>\n", $key, $limitrms);
        printf("</tr>\n");
    }

    if (empty($old))
    {
        printf("  </table>\n");
        printf("  </td>\n");
        printf("  <td valign='top'>\n");
        printf("   <table>\n");
    }

    printf("  <tr><th>Name </th><th> Min </th><th> Min2 </th></tr>\n");
    foreach($limitsmin as $key => $element)
    {
        printf("<tr><td>%s</td>\n", $alias[$key]);
        $level1=$key . "1";
        $limit1=$_GET[$level1];
        printf("<td><input name=\"%s1\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\"></td>\n", $key, $limit1);
        $level2=$key . "2";
        $limit2=$_GET[$level2];
        printf("<td><input name=\"%s2\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\"></td>\n", $key, $limit2);
        printf("</tr>\n");
    }

    if (empty($old))
    {
        printf("  </table>\n");
        printf("  </td>\n");
        printf("  <td valign='top'>\n");
        printf("   <table>\n");
    }

    printf("  <tr><th>Name </th><th> Max </th><th> Max2 </th></tr>\n");
    foreach($limitsmax as $key => $element)
    {
        printf("<tr><td>%s</td>\n", $alias[$key]);
        $level1=$key . "1";
        $limit1=$_GET[$level1];
        printf("<td><input name=\"%s1\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\"></td>\n", $key, $limit1);
        $level2=$key . "2";
        $limit2=$_GET[$level2];
        printf("<td><input name=\"%s2\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\"></td>\n", $key, $limit2);
        printf("</tr>\n");
    }
    printf("  </table>\n");

    printf("  </td></tr></table>\n");
    printf("<p>\n");
    printf("</div>\n");
}

function PrintSequStatMenu()
{
    printf(" <div id='stat' style='display:none'>\n");
    printf(" <table><tr><td>\n");

    PrintStatusMenu("fAllFilesAvail",       "Files avail");
    printf("  </td><td>\n");
    PrintStatusMenu("fCallisto",            "Callisto");
    printf("  </td><td>\n");
    PrintStatusMenu("fStar",                "Star");
    printf("  </td></tr><tr><td>\n");
    PrintStatusMenu("fSequenceFileWritten", "Sequfile");
    printf("  </td><td>\n");
    PrintStatusMenu("fFillCallisto",        "Fillcallisto");
    printf("  </td><td>\n");
    PrintStatusMenu("fFillStar",            "Fillstar");

    printf("</td></tr> </table>\n");
    printf(" </div><p>\n");
}

function PrintFailMenu()
{
    printf(" <div id='fail' style='display:none'>\n");
    printf(" <table>\n");
    printf("  <tr>\n");

    CheckBox("fStartTime",     "ProcessTime");
    CheckBox("fFailedTime",    "FailTime");
    CheckBox("fReturnCode",    "RetCode");
    CheckBox("fProgramId",     "ProgramId");

    printf("  </tr>\n");
    printf(" </table>\n");
    printf(" </div><p>\n");
}

function PrintCalMenu()
{
    printf(" <div id='cal' style='display:none'>\n");
    printf(" <table>\n");
    printf("  <tr>\n");

    CheckBox("fUnsuitableInner",     "Unsuitable inner");
    CheckBox("fUnsuitableOuter",     "Unsuitable outer");
    CheckBox("fUnreliableInner",     "Unreliable inner");
    CheckBox("fUnreliableOuter",     "Unreliable outer");

    printf("  </tr><tr>\n");

    CheckBox("fUnsuitable50",        "Unsuitable (50%)");
    CheckBox("fUnsuitable01",        "Unsuitable (1%)");
    CheckBox("fUnsuitableMax",       "UnsuitableMax");
    CheckBox("fDeadMax",             "DeadMax");

    printf("  </tr><tr>\n");

    CheckBox("fIsolatedInner",       "Isolated inner");
    CheckBox("fIsolatedOuter",       "Isolated outer");
    CheckBox("fIsolatedMaxCluster",  "IsolatedMaxCluster");

    printf("  </tr><tr>\n");

    CheckBox("fArrTimeMeanInner",    "ArrTimeMean inner ");
    CheckBox("fArrTimeMeanOuter",    "ArrTimeMean outer ");
    CheckBox("fArrTimeRmsInner",     "ArrTimeRms inner ");
    CheckBox("fArrTimeRmsOuter",     "ArrTimeRms outer ");

    printf("  </tr><tr>\n");

    CheckBox("fMeanPedRmsInner",     "MeanPedRms inner");
    CheckBox("fMeanPedRmsOuter",     "MeanPedRms outer");
    CheckBox("fMeanSignalInner",     "MeanSignal inner");
    CheckBox("fMeanSignalOuter",     "MeanSignal outer");

    printf("  </tr><tr>\n");

    CheckBox("fConvFactorInner",     "Conv inner ");
    CheckBox("fConvFactorOuter",     "Conv outer ");
    CheckBox("fPulsePosMean",        "Mean PulsePos");
    CheckBox("fPulsePosCalib",       "Cal PulsePos");

    printf("  </tr><tr>\n");

    CheckBox("fRateTrigEvts",        "Rate Trig evts");
    CheckBox("fRateSumEvts",         "Rate Sum Trig evts");
    CheckBox("fRatePedEvts",         "Rate Ped evts");
    CheckBox("fRatePedTrigEvts",     "Rate Ped Trig evts");

    printf("  </tr><tr>\n");

    CheckBox("fRateCalEvts",         "Rate Cal evts");
    CheckBox("fRateNullEvts",        "Rate Null evts");
    CheckBox("fRateUnknownEvts",     "Rate Unknown evts");
    CheckBox("fRatioCalEvents",      "Ratio Cal evts");

    printf(" </tr></table>\n");
    printf(" </div><p>\n");
}

function PrintStarMenu()
{
    printf(" <div id='star' style='display:none'>\n");
    printf(" <table>\n");
    printf("  <tr>\n");

    CheckBox("fDataRate",           "CleanedEvtRate");
    CheckBox("fMeanNumberIslands",  "MeanNumIslands");
    CheckBox("fMaxHumidity",        "MaxHumidity");
    CheckBox("fInhomogeneity",      "Inhomogeneity");

    printf("  </tr><tr>\n");

    CheckBox("fMuonRate",           "MuonRate");
    CheckBox("fMuonNumber",         "MuonNumber");
    CheckBox("fRatio",              "Ratio");
    CheckBox("fPSF",                "PSF");

    printf("  </tr><tr>\n");

    CheckBox("fSparkRate",      "SparkRate");

    printf("  </tr><tr>\n");

    CheckBox("fEffOnTime",          "EffOnTime");
    CheckBox("fEffOnTime/fRunTime", "RelOnTime");
    CheckBox("fBrightnessMed",      "SkyBrightnessMed");
    CheckBox("fBrightnessRMS",      "SkyBrightnessRMS");

    printf("  </tr><tr>\n");

    CheckBox("fNumStarsMed",        "# id. Stars");
    CheckBox("fNumStarsRMS",        "RMS id. Stars");
    CheckBox("fNumStarsCorMed",     "# cor. Stars");
    CheckBox("fNumStarsCorRMS",     "RMS cor. Stars");

    printf("  </tr><tr>\n");

    CheckBox("fAvgWindSpeed",       "Avg wind speed");
    CheckBox("fAvgTemperature",     "Avg temperature");
    CheckBox("fAvgHumidity",        "Avg humidity");
    CheckBox("fAvgTempSky",         "Avg sky temperature");

    printf("  </tr><tr>\n");

    CheckBox("fAvgCloudiness",      "Avg cloudiness");
    CheckBox("fRmsCloudiness",      "RMS cloudiness");

    printf("  </tr>\n");
    printf(" </table>\n");
    printf(" </div><p>\n");
}

function PrintDataSetInfoMenu($host,$user,$pw,$db)
{
    printf(" <div id=\"info\" style='display:none'>");
    printf(" <table>\n");
    printf("  <tr>\n");

    CheckBox("fDataSetName",       "Name");
    CheckBox("fComment",           "Comment");
    CheckBox("fRunTime",           "Uptime");
    CheckBox("fRunStart",          "Start");
    CheckBox("fRunStop",           "Stop");

    printf("  </tr><tr>\n");

    CheckBox("fZenithDistanceMin", "ZdMin");
    CheckBox("fZenithDistanceMax", "ZdMax");
    CheckBox("fLinks",             "Links");

    printf("  </tr><tr>\n");

    CheckBox("fEffOnTime/3600",    "EffOnTime");
    CheckBox("fExcessEvents",      "ExcEvts");
    CheckBox("fBackgroundEvents",  "BgEvts");
    CheckBox("fSignalEvents",      "SignEvts");
    CheckBox("fSignificance",      "Sign");

    printf("  </tr><tr>\n");

    CheckBox("fScaleFactor",                                  "Scale");
    CheckBox("fExcessEvents*60/fEffOnTime",                   "ExcRate");
    CheckBox("fBackgroundEvents*60/fEffOnTime",               "BgRate");
    CheckBox("fSignalEvents*60/fEffOnTime",                   "SignRate");
    CheckBox("Round(fSignificance/Sqrt(fEffOnTime/3600),2)",  "SignfRate");

    printf("  </tr>\n");
    printf(" </table>\n");
    printf(" </div><p>\n");
}

function PrintDataSetInfoMenu2($host,$user,$pw,$db)
{
    printf(" <div id='info2' style='display:none'>");
    printf("  <table><tr><td>\n");
    PrintPullDown($host, $user, $pw, $db, "Source",          "fSourceName",          "fSourceKEY",          "Source Name");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "ObservationMode", "fObservationModeName", "fObservationModeKEY", "Observation Mode");
    printf("  </td><td>\n");
    PrintPullDown($host, $user, $pw, $db, "User",            "fUserName",            "fUserKEY",            "User");
    printf("  </td></tr>\n");
    printf(" </table>\n");
    printf("</div>");
}

function PrintDataSetStatMenu($host,$user,$pw,$db)
{
    printf(" <div id=\"stat\" style='display:none'>\n");
    printf(" <table>\n");
    printf("  <tr>\n");

    printf("  <td>\n");
    PrintStatusMenu("fDataSetInserted", "DataSet Inserted");
    printf("  </td><td>\n");
    PrintStatusMenu("fDataSetFileWritten", "DataSetFile");
    printf("  </td><td>\n");
    PrintStatusMenu("fStarFilesAvail",  "FilesAvail");
    printf("  </td><td>\n");
    PrintStatusMenu("fGanymed",         "Ganymed");
    printf("  </td><td>\n");
    PrintStatusMenu("fFillGanymed",     "FillGanymed");
    printf("  </tr>\n");
    printf(" </table>\n");
    printf(" </div><p>\n");
}

function PrintRunRangeMenu($host,$user,$pw,$db)
{
    if (empty($_GET["fRunMin"]))
        $min = GetMin("fRunNumber", "RunData", $host, $user, $pw, $db);
    else
        $min = $_GET["fRunMin"];

    if (empty($_GET["fRunMax"]))
        $max = GetMax("fRunNumber", "RunData", $host, $user, $pw, $db);
    else
        $max = $_GET["fRunMax"];

    printf("Runs&nbsp;from&nbsp;<input name=\"fRunMin\" type=\"text\" size=\"8\" maxlength=\"8\" value=\"%s\">\n", $min);
    printf("to&nbsp;<input name=\"fRunMax\" type=\"text\" size=\"8\" maxlength=\"8\" value=\"%s\">&nbsp;&nbsp;&nbsp;\n", $max);
}

function PrintCTAMCRunRangeMenu($host,$user,$pw,$db)
{
    if (empty($_GET["fRunMin"]))
        $min = GetMin("fMCRunNumber", "MCRunData", $host, $user, $pw, $db);
    else
        $min = $_GET["fRunMin"];

    if (empty($_GET["fRunMax"]))
        $max = GetMax("fMCRunNumber", "MCRunData", $host, $user, $pw, $db);
    else
        $max = $_GET["fRunMax"];

    printf("Runs&nbsp;from&nbsp;<input name=\"fRunMin\" type=\"text\" size=\"8\" maxlength=\"8\" value=\"%s\">\n", $min);
    printf("to&nbsp;<input name=\"fRunMax\" type=\"text\" size=\"8\" maxlength=\"8\" value=\"%s\">&nbsp;&nbsp;&nbsp;\n", $max);
}

function PrintMCRunRangeMenu($host,$user,$pw,$db)
{
    if (empty($_GET["fRunMin"]))
        $min = GetMin("fRunNumber", "CeresInfo", $host, $user, $pw, $db);
    else
        $min = $_GET["fRunMin"];

    if (empty($_GET["fRunMax"]))
        $max = GetMax("fRunNumber", "CeresInfo", $host, $user, $pw, $db);
    else
        $max = $_GET["fRunMax"];

    printf("Runs&nbsp;from&nbsp;<input name=\"fRunMin\" type=\"text\" size=\"8\" maxlength=\"8\" value=\"%s\">\n", $min);
    printf("to&nbsp;<input name=\"fRunMax\" type=\"text\" size=\"8\" maxlength=\"8\" value=\"%s\">&nbsp;&nbsp;&nbsp;\n", $max);
}

function PrintSequRangeMenu($host,$user,$pw,$db)
{
    if (empty($_GET["fRunMin"]))
        $min = GetMin("fSequenceFirst", "Sequences", $host, $user, $pw, $db);
    else
        $min = $_GET["fRunMin"];

    if (empty($_GET["fRunMax"]))
        $max = GetMax("fSequenceFirst", "Sequences", $host, $user, $pw, $db);
    else
        $max = $_GET["fRunMax"];

    printf("Sequences&nbsp;from&nbsp;<input name=\"fRunMin\" type=\"text\" size=\"8\" maxlength=\"8\" value=\"%s\">\n", $min);
    printf("to&nbsp;<input name=\"fRunMax\" type=\"text\" size=\"8\" maxlength=\"8\" value=\"%s\">&nbsp;&nbsp;&nbsp;\n", $max);
}

function PrintDataSetRangeMenu($host,$user,$pw,$db)
{
    if (empty($_GET["fRunMin"]))
        $min = GetMin("fDataSetNumber", "DataSets", $host, $user, $pw, $db);
    else
        $min = $_GET["fRunMin"];

    if (empty($_GET["fRunMax"]))
        $max = GetMax("fDataSetNumber", "DataSets", $host, $user, $pw, $db);
    else
        $max = $_GET["fRunMax"];

    printf("DataSets&nbsp;from&nbsp;<input name=\"fRunMin\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\">\n", $min);
    printf("to&nbsp;<input name=\"fRunMax\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\">&nbsp;&nbsp;&nbsp;\n", $max);
}

function PrintZdRangeMenu($host,$user,$pw,$db)
{
    if (empty($_GET["fZDMin"]))
        $zdmin = GetMin("fZenithDistanceMin", "Sequences", $host, $user, $pw, $db);
    else
        $zdmin = $_GET["fZDMin"];

    if (empty($_GET["fZDMax"]))
        $zdmax = GetMax("fZenithDistanceMax", "Sequences", $host, $user, $pw, $db);
    else
        $zdmax = $_GET["fZDMax"];

    printf("ZenithDistance&nbsp;from&nbsp;<input name=\"fZDMin\" type=\"text\" size=\"2\" maxlength=\"2\" value=\"%s\">\n", $zdmin);
    printf("to&nbsp;<input name=\"fZDMax\" type=\"text\" size=\"2\" maxlength=\"2\" value=\"%s\">&nbsp;&nbsp;&nbsp;\n", $zdmax);
}

function PrintNightRangeMenu($host,$user,$pw,$db, $table)
{
    if (empty($_GET["fStartDate"]))
        $timemin = GetMin("Date_Format(fRunStart, '%Y-%m-%d')", $table, $host, $user, $pw, $db);
    else
        $timemin = $_GET["fStartDate"];

    if (empty($_GET["fStopDate"]))
        $timemax = GetMaxDate("Date_Format(fRunStart, '%Y-%m-%d')", $table, $host, $user, $pw, $db);
    else
        $timemax = $_GET["fStopDate"];

    printf("Night&nbsp;(yyyy-mm-dd)&nbsp;from&nbsp;<input name=\"fStartDate\" type=\"text\" size=\"10\" maxlength=\"10\" value=\"%s\">\n", $timemin);
    printf("to&nbsp;<input name=\"fStopDate\" type=\"text\" size=\"10\" maxlength=\"10\" value=\"%s\">&nbsp;&nbsp;&nbsp;&nbsp;\n", $timemax);
}

function PrintStarRangeMenu($host,$user,$pw,$db)
{
    if (empty($_GET["fStarStart"]))
        $starmin = GetMin("fStar", "SequenceProcessStatus", $host, $user, $pw, $db);
    else
        $starmin = $_GET["fStarStart"];

    if (empty($_GET["fStarStop"]))
        $starmax = GetMax("fStar", "SequenceProcessStatus", $host, $user, $pw, $db);
    else
        $starmax = $_GET["fStarStop"];

    printf("<p>StarDone&nbsp;(yyyy-mm-dd hh:mm:ss)&nbsp;from&nbsp;<input name=\"fStarStart\" type=\"text\" size=\"19\" maxlength=\"19\" value=\"%s\">\n", $starmin);
    printf("to&nbsp;<input name=\"fStarStop\" type=\"text\" size=\"19\" maxlength=\"19\" value=\"%s\">&nbsp;&nbsp;&nbsp;&nbsp;\n", $starmax);
}

function PrintGroupByDateMenu()
{
    printf("Group by Date ");
    printf("<select name='fGroupByDate' size='1' class='Width'>\n");
    if (empty($_GET["fGroupByDate"]) || $_GET["fGroupByDate"]==0)
        printf("    <option value='0' selected>--- NO GROUP BY ---</option>\n");
    else
        printf("    <option value='0'>--- NO GROUP BY ---</option>\n");
    $dates=array("Year", "Month","Night");
    foreach ($dates as $date)
    {
        if ($_GET["fGroupByDate"]==$date)
            printf("    <option value='%s' selected> %s </option>\n", $date, $date);
        else
            printf("    <option value='%s'> %s </option>\n", $date, $date);
    }
    printf("   </select>\n");
}

function PrintSourceMenu($host,$user,$pw,$db)
{
    printf("Source&nbsp;(<A HREF=\"regexp.html\">regexp</A>)&nbsp;<input name=\"fSourceN\" type=\"text\" size=\"15\" maxlength=\"15\" value=\"");
    if (!empty($_GET["fSourceN"]))
        printf("%s", $_GET["fSourceN"]);
    printf("\">&nbsp;&nbsp;&nbsp;\n");
}
function PrintSequMenu($host,$user,$pw,$db)
{
    printf("Sequ#&nbsp;<input name=\"fSequenceNo\" type=\"text\" size=\"8\" maxlength=\"8\" value=\"");
    if (!empty($_GET["fSequenceNo"]))
        printf("%s", $_GET["fSequenceNo"]);
    printf("\">&nbsp;&nbsp;&nbsp;\n");
}

function PrintNumResPullDown()
{
    printf(" Results:\n");
    printf(" <select name=\"fNumResults\">\n");

    $numres = array("10", "20", "50", "100", "200", "500", "1000", "2000");
    foreach ($numres as $element)
    {
        if ($element==$_GET["fNumResults"])
            printf("<option value=\"%s\" selected>%3s</option>\n", $element, $element);
        else
            printf("<option value=\"%s\">%3s</option>\n", $element, $element);
    }
    printf(" </select>\n");
    printf(" &nbsp;&nbsp;&nbsp;\n");
}

function PrintButtons($page)
{
    printf("<input class='Width' type='submit' value='Query Table'>&nbsp;&nbsp;&nbsp;\n");
    printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"%s\"'>&nbsp;&nbsp;&nbsp;\n", $page);
//    printf("<p>");
    if (strchr($_SERVER["REQUEST_URI"], '?')!=FALSE)
    {
        printf("<input class='Width' type='button' value='Get .txt' onClick='self.location.href=\"%s&amp;fSendTxt=1\"'>&nbsp;&nbsp;&nbsp;\n", htmlspecialchars($_SERVER["REQUEST_URI"]));
        printf("<input class='Width' type='button' value='Print' onClick='self.location.href=\"%s&amp;fPrintTable=1\"'>&nbsp;&nbsp;&nbsp;\n", htmlspecialchars($_SERVER["REQUEST_URI"]));
        printf("MySqlQuery: <img id='showquerybutton' src='../plus.png' alt='+' onClick='showhide(\"showquery\")'>\n");
    }
}

function InitInfo($first)
{
    if (empty($_GET["fNumResults"]))
        $_GET["fNumResults"]="20";

    if (empty($_GET["fRunStart"]))
        $_GET["fRunStart"]="Off";

    if (empty($_GET["fRunStop"]))
        $_GET["fRunStop"]="Off";

    if (empty($_GET["fTest"]))
        $_GET["fTest"]="Off";

    if (empty($_GET["fSourceName"]))
        $_GET["fSourceName"]="Off";

    if (empty($_GET["fProjectName"]))
        $_GET["fProjectName"]="Off";

    if (empty($_GET["fL1TriggerTableName"]))
        $_GET["fL1TriggerTableName"]="Off";

    if (empty($_GET["fL2TriggerTableName"]))
        $_GET["fL2TriggerTableName"]="Off";

    if (empty($_GET["fL3TriggerTableName"]))
        $_GET["fL3TriggerTableName"]="Off";

    if (empty($_GET["fHvSettingsName"]))
        $_GET["fHvSettingsName"]="Off";

    if (empty($_GET["fDiscriminatorThresholdTableName"]))
        $_GET["fDiscriminatorThresholdTableName"]="Off";

    if (empty($_GET["fTriggerDelayTableName"]))
        $_GET["fTriggerDelayTableName"]="Off";

    if (empty($_GET["fLightConditionsName"]))
        $_GET["fLightConditionsName"]="Off";

    if (empty($_GET["fTestFlagName"]))
        $_GET["fTestFlagName"]="Off";

    if (empty($_GET["fCycleName"]))
        $_GET["fCycleName"]="Off";

    if (empty($_GET["fPIName"]))
        $_GET["fPIName"]="Off";

    if (empty($_GET["fWorkingGroupName"]))
        $_GET["fWorkingGroupName"]="Off";

    if (empty($_GET["fProposalName"]))
        $_GET["fProposalName"]="Off";
}

function InitRunStatus($first)
{
   if (empty($_GET["fDataCheckDone"]))
        $_GET["fDataCheckDone"]="Off";

    if (empty($_GET["fDataCheckDoneStatus"]))
        $_GET["fDataCheckDoneStatus"]="0";

    if (empty($_GET["fCCFileAvail"]))
        $_GET["fCCFileAvail"]="Off";

    if (empty($_GET["fCCFileAvailStatus"]))
        $_GET["fCCFileAvailStatus"]="0";

    if (empty($_GET["fCaCoFileAvail"]))
        $_GET["fCaCoFileAvail"]=$first?"On":"";

    if (empty($_GET["fCaCoFileAvailStatus"]))
        $_GET["fCaCoFileAvailStatus"]="0";

    if (empty($_GET["fCaCoFileFound"]))
        $_GET["fCaCoFileFound"]="Off";

    if (empty($_GET["fCaCoFileFoundStatus"]))
        $_GET["fCaCoFileFoundStatus"]="0";

    if (empty($_GET["fRawFileAvail"]))
        $_GET["fRawFileAvail"]="Off";

    if (empty($_GET["fRawFileAvailStatus"]))
        $_GET["fRawFileAvailStatus"]="0";

    if (empty($_GET["fTimingCorrection"]))
        $_GET["fTimingCorrection"]="Off";

    if (empty($_GET["fTimingCorrectionStatus"]))
        $_GET["fTimingCorrectionStatus"]="0";
}

function InitRunInfo($first)
{
    InitRunStatus($first);
    InitInfo($first);

    if (empty($_GET["fNumEvents"]))
        $_GET["fNumEvents"]=$first?"Off":"";

    if (empty($_GET["fLinks"]))
        $_GET["fLinks"]=$first?"On":"";

    if (empty($_GET["fFormatVersion"]))
        $_GET["fFormatVersion"]="Off";

    if (empty($_GET["fAzimuth"]))
        $_GET["fAzimuth"]="Off";

    if (empty($_GET["fZenithDistance"]))
        $_GET["fZenithDistance"]="Off";

    if (empty($_GET["fRunTypeName"]))
        $_GET["fRunTypeName"]="Off";

    if (empty($_GET["fExcludedFDAName"]))
        $_GET["fExcludedFDAName"]="Off";

    if (empty($_GET["fMagicNumberName"]))
        $_GET["fMagicNumberName"]="Off";

    if (empty($_GET["fObservationModeName"]))
        $_GET["fObservationModeName"]="Off";

    if (empty($_GET["fSumTriggerFlagName"]))
        $_GET["fSumTriggerFlagName"]="Off";

    if (empty($_GET["fMeanTriggerRate"]))
        $_GET["fMeanTriggerRate"]="Off";

    if (empty($_GET["fCalibrationScriptName"]))
        $_GET["fCalibrationScriptName"]="Off";

    if (empty($_GET["fTestFlagName"]))
        $_GET["fTestFlagName"]="Off";

    if (empty($_GET["fDaqStoreRate"]))
        $_GET["fDaqStoreRate"]="Off";

    if (empty($_GET["fDaqTriggerRate"]))
        $_GET["fDaqTriggerRate"]="Off";

    if (empty($_GET["fL2RatePresc"]))
        $_GET["fL2RatePresc"]="Off";

    if (empty($_GET["fL3TriggerRate"]))
        $_GET["fL3TriggerRate"]="Off";

    if (empty($_GET["fWheelPos1"]))
        $_GET["fWheelPos1"]="Off";

    if (empty($_GET["fWheelPos2"]))
        $_GET["fWheelPos2"]="Off";

    if (empty($_GET["fL2RateUnpresc"]))
        $_GET["fL2RateUnpresc"]="Off";

    if (empty($_GET["fSequenceFirst"]))
        $_GET["fSequenceFirst"]="Off";

    if (empty($_GET["fSequenceNo"]))
        $_GET["fSequenceNo"]="";
}

function InitSequStatus($first)
{
    if (empty($_GET["fSequenceFileWritten"]))
        $_GET["fSequenceFileWritten"]="Off";

    if (empty($_GET["fSequenceFileWrittenStatus"]))
        $_GET["fSequenceFileWrittenStatus"]="0";

    if (empty($_GET["fAllFilesAvail"]))
        $_GET["fAllFilesAvail"]="Off";

    if (empty($_GET["fAllFilesAvailStatus"]))
        $_GET["fAllFilesAvailStatus"]="0";

    if (empty($_GET["fCallisto"]))
        $_GET["fCallisto"]="Off";

    if (empty($_GET["fCallistoStatus"]))
        $_GET["fCallistoStatus"]="0";

    if (empty($_GET["fFillCallisto"]))
        $_GET["fFillCallisto"]="Off";

    if (empty($_GET["fFillCallistoStatus"]))
        $_GET["fFillCallistoStatus"]="0";

    if (empty($_GET["fStar"]))
        $_GET["fStar"]="Off";

    if (empty($_GET["fStarStatus"]))
        $_GET["fStarStatus"]="0";

    if (empty($_GET["fFillStar"]))
        $_GET["fFillStar"]="Off";

    if (empty($_GET["fFillStarStatus"]))
        $_GET["fFillStarStatus"]="0";
}

function InitStarInfo($first)
{
    if (empty($_GET["fMeanNumberIslands"]))
        $_GET["fMeanNumberIslands"]="Off";

    if (empty($_GET["fPSF"]))
        $_GET["fPSF"]="Off";

    if (empty($_GET["fRatio"]))
        $_GET["fRatio"]="Off";

    if (empty($_GET["fMuonNumber"]))
        $_GET["fMuonNumber"]="Off";

    if (empty($_GET["fEffOnTime"]))
        $_GET["fEffOnTime"]="Off";

    if (empty($_GET["fEffOnTime/fRunTime"]))
        $_GET["fEffOnTime/fRunTime"]="Off";

    if (empty($_GET["fMuonRate"]))
        $_GET["fMuonRate"]="Off";

    if (empty($_GET["fInhomogeneity"]))
        $_GET["fInhomogeneity"]="Off";

    if (empty($_GET["fDataRate"]))
        $_GET["fDataRate"]="Off";

    if (empty($_GET["fSparkRate"]))
        $_GET["fSparkRate"]="Off";

    if (empty($_GET["fMaxHumidity"]))
        $_GET["fMaxHumidity"]="Off";

    if (empty($_GET["fBrightnessMed"]))
        $_GET["fBrightnessMed"]="Off";

    if (empty($_GET["fBrightnessRMS"]))
        $_GET["fBrightnessRMS"]="Off";

    if (empty($_GET["fNumStarsMed"]))
        $_GET["fNumStarsMed"]="Off";

    if (empty($_GET["fNumStarsRMS"]))
        $_GET["fNumStarsRMS"]="Off";

    if (empty($_GET["fNumStarsCorMed"]))
        $_GET["fNumStarsCorMed"]="Off";

    if (empty($_GET["fNumStarsCorRMS"]))
        $_GET["fNumStarsCorRMS"]="Off";

    if (empty($_GET["fAvgWindSpeed"]))
        $_GET["fAvgWindSpeed"]="Off";

    if (empty($_GET["fAvgTemperature"]))
        $_GET["fAvgTemperature"]="Off";

    if (empty($_GET["fAvgHumidity"]))
        $_GET["fAvgHumidity"]="Off";

    if (empty($_GET["fAvgTempSky"]))
        $_GET["fAvgTempSky"]="Off";

    if (empty($_GET["fAvgCloudiness"]))
        $_GET["fAvgCloudiness"]="Off";

    if (empty($_GET["fRmsCloudiness"]))
        $_GET["fRmsCloudiness"]="Off";
}

function InitCalInfo($first)
{
    if (empty($_GET["fUnreliableInner"]))
        $_GET["fUnreliableInner"]="Off";

    if (empty($_GET["fUnsuitableOuter"]))
        $_GET["fUnsuitableOuter"]="Off";

    if (empty($_GET["fUnreliableOuter"]))
        $_GET["fUnreliableOuter"]="Off";

    if (empty($_GET["fUnsuitableInner"]))
        $_GET["fUnsuitableInner"]="Off";

    if (empty($_GET["fUnsuitable50"]))
        $_GET["fUnsuitable50"]="Off";

    if (empty($_GET["fUnsuitable01"]))
        $_GET["fUnsuitable01"]="Off";

    if (empty($_GET["fUnsuitableMax"]))
        $_GET["fUnsuitableMax"]="Off";

    if (empty($_GET["fDeadMax"]))
        $_GET["fDeadMax"]="Off";

    if (empty($_GET["fRateTrigEvts"]))
        $_GET["fRateTrigEvts"]="Off";

    if (empty($_GET["fRateSumEvts"]))
        $_GET["fRateSumEvts"]="Off";

    if (empty($_GET["fRatePedEvts"]))
        $_GET["fRatePedEvts"]="Off";

    if (empty($_GET["fRatePedTrigEvts"]))
        $_GET["fRatePedTrigEvts"]="Off";

    if (empty($_GET["fRateCalEvts"]))
        $_GET["fRateCalEvts"]="Off";

    if (empty($_GET["fRateNullEvts"]))
        $_GET["fRateNullEvts"]="Off";

    if (empty($_GET["fRateUnknownEvts"]))
        $_GET["fRateUnknownEvts"]="Off";

    if (empty($_GET["fRatioCalEvents"]))
        $_GET["fRatioCalEvents"]="Off";

    if (empty($_GET["fPulsePosCalib"]))
        $_GET["fPulsePosCalib"]="Off";

    if (empty($_GET["fIsolatedInner"]))
        $_GET["fIsolatedInner"]="Off";

    if (empty($_GET["fIsolatedOuter"]))
        $_GET["fIsolatedOuter"]="Off";

    if (empty($_GET["fMeanPedRmsInner"]))
        $_GET["fMeanPedRmsInner"]="Off";

    if (empty($_GET["fMeanPedRmsOuter"]))
        $_GET["fMeanPedRmsOuter"]="Off";

    if (empty($_GET["fIsolatedMaxCluster"]))
        $_GET["fIsolatedMaxCluster"]="Off";

    if (empty($_GET["fArrTimeMeanInner"]))
        $_GET["fArrTimeMeanInner"]="Off";

    if (empty($_GET["fArrTimeMeanOuter"]))
        $_GET["fArrTimeMeanOuter"]="Off";

    if (empty($_GET["fArrTimeRmsInner"]))
        $_GET["fArrTimeRmsInner"]="Off";

    if (empty($_GET["fArrTimeRmsOuter"]))
        $_GET["fArrTimeRmsOuter"]="Off";

    if (empty($_GET["fMeanSignalInner"]))
        $_GET["fMeanSignalInner"]=$first?"Off":"";

    if (empty($_GET["fMeanSignalOuter"]))
        $_GET["fMeanSignalOuter"]=$first?"Off":"";

    if (empty($_GET["fPulsePosMean"]))
        $_GET["fPulsePosMean"]=$first?"Off":"";

    if (empty($_GET["fConvFactorInner"]))
        $_GET["fConvFactorInner"]="Off";

    if (empty($_GET["fConvFactorOuter"]))
        $_GET["fConvFactorOuter"]="Off";
}

function InitSequInfo($first)
{
    InitInfo($first);
    InitSequStatus($first);
    InitCalInfo($first);
    InitStarInfo($first);

    if (empty($_GET["fNumEvents"]))
        $_GET["fNumEvents"]=$first?"Off":"";

    if (empty($_GET["fRunTime/60"]))
        $_GET["fRunTime/60"]="Off";

    if (empty($_GET["fSequenceLast"]))
        $_GET["fSequenceLast"]="Off";

    if (empty($_GET["fAzimuthMin"]))
        $_GET["fAzimuthMin"]="Off";

    if (empty($_GET["fAzimuthMax"]))
        $_GET["fAzimuthMax"]="Off";

    if (empty($_GET["fZenithDistanceMin"]))
        $_GET["fZenithDistanceMin"]="Off";

    if (empty($_GET["fZenithDistanceMax"]))
        $_GET["fZenithDistanceMax"]="Off";

    if (empty($_GET["fObservationModeName"]))
        $_GET["fObservationModeName"]="Off";

    if (empty($_GET["fSumTriggerFlagName"]))
        $_GET["fSumTriggerFlagName"]="Off";

    if (empty($_GET["fOnlySum"]))
        $_GET["fOnlySum"]="Off";

//    if (empty($_GET["fManuallyChangedName"]))
//        $_GET["fManuallyChangedName"]="Off";
}

function InitDataSetStatus($first)
{
    if (empty($_GET["fDataSetInserted"]))
        $_GET["fDataSetInserted"]="Off";

    if (empty($_GET["fDataSetInsertedStatus"]))
        $_GET["fDataSetInsertedStatus"]="0";

    if (empty($_GET["fDataSetFileWritten"]))
        $_GET["fDataSetFileWritten"]=$first?"On":"";

    if (empty($_GET["fDataSetFileWrittenStatus"]))
        $_GET["fDataSetFileWrittenStatus"]="0";

    if (empty($_GET["fStarFilesAvail"]))
        $_GET["fStarFilesAvail"]="Off";

    if (empty($_GET["fStarFilesAvailStatus"]))
        $_GET["fStarFilesAvailStatus"]="0";

    if (empty($_GET["fGanymed"]))
        $_GET["fGanymed"]=$first?"On":"";

    if (empty($_GET["fGanymedStatus"]))
        $_GET["fGanymedStatus"]="0";

    if (empty($_GET["fFillGanymed"]))
        $_GET["fFillGanymed"]="Off";

    if (empty($_GET["fFillGanymedStatus"]))
        $_GET["fFillGanymedStatus"]="0";
}

function InitDataSetInfo($first)
{
    InitDataSetStatus($first);

    if (empty($_GET["fNumResults"]))
        $_GET["fNumResults"]="50";

    if (empty($_GET["fLinks"]))
        $_GET["fLinks"]=$first?"On":"";

    if (empty($_GET["fExcessEvents"]))
        $_GET["fExcessEvents"]=$first?"On":"";

    if (empty($_GET["fBackgroundEvents*60/fEffOnTime"]))
        $_GET["fBackgroundEvents*60/fEffOnTime"]=$first?"On":"";

    if (empty($_GET["fBackgroundEvents"]))
        $_GET["fBackgroundEvents"]=$first?"On":"";

    if (empty($_GET["fSignalEvents"]))
        $_GET["fSignalEvents"]=$first?"On":"";

    if (empty($_GET["fSignificance"]))
        $_GET["fSignificance"]=$first?"On":"";

    if (empty($_GET["fScaleFactor"]))
        $_GET["fScaleFactor"]=$first?"On":"";

    if (empty($_GET["fEffOnTime/3600"]))
        $_GET["fEffOnTime/3600"]=$first?"On":"";

    if (empty($_GET["fSourceName"]))
        $_GET["fSourceName"]=$first?"On":"";

    if (empty($_GET["fObservationModeName"]))
        $_GET["fObservationModeName"]=$first?"On":"";

    if (empty($_GET["fComment"]))
        $_GET["fComment"]="Off";

    if (empty($_GET["fUserName"]))
        $_GET["fUserName"]="Off";

    if (empty($_GET["fRunStart"]))
        $_GET["fRunStart"]="Off";

    if (empty($_GET["fRunStop"]))
        $_GET["fRunStop"]="Off";

    if (empty($_GET["fZenithDistanceMin"]))
        $_GET["fZenithDistanceMin"]=$first?"On":"";

    if (empty($_GET["fZenithDistanceMax"]))
        $_GET["fZenithDistanceMax"]=$first?"On":"";

    if (empty($_GET["fRunTime"]))
        $_GET["fRunTime"]="Off";

    if (empty($_GET["fDataSetName"]))
        $_GET["fDataSetName"]=$first?"On":"";

    if (empty($_GET["fExcessEvents*60/fEffOnTime"]))
        $_GET["fExcessEvents*60/fEffOnTime"]="Off";

    if (empty($_GET["fSignalEvents*60/fEffOnTime"]))
        $_GET["fSignalEvents*60/fEffOnTime"]="Off";

    if (empty($_GET["Round(fSignificance/Sqrt(fEffOnTime/3600),2)"]))
        $_GET["Round(fSignificance/Sqrt(fEffOnTime/3600),2)"]="Off";

}

function InitFindOffData($first)
{
    InitSequInfo($first);

    //init for limits

    if (empty($_GET["fArrTimeLimitMean"]))
        $_GET["fArrTimeLimitMean"]="";

    if (empty($_GET["fArrTimeLimitRms"]))
        $_GET["fArrTimeLimitRms"]="";

    if (empty($_GET["fSkyBrightLimitMean"]))
        $_GET["fSkyBrightLimitMean"]="";

    if (empty($_GET["fSkyBrightLimitRms"]))
        $_GET["fSkyBrightLimitRms"]="";

    if (empty($_GET["fMaxHumLimit1"]))
        $_GET["fMaxHumLimit1"]="";

    if (empty($_GET["fMaxHumLimit2"]))
        $_GET["fMaxHumLimit2"]="";

    if (empty($_GET["fCloudinessLimit1"]))
        $_GET["fCloudinessLimit1"]="40";

    if (empty($_GET["fCloudinessLimit2"]))
        $_GET["fCloudinessLimit2"]="50";

    //values that differ for different data
    if (empty($_GET["fPSFLimitMean"]))
        $_GET["fPSFLimitMean"]="";

    if (empty($_GET["fPedRmsInLimitMean"]))
        $_GET["fPedRmsInLimitMean"]="";

    if (empty($_GET["fZdMinLimit1"]))
        $_GET["fZdMinLimit1"]="";

    if (empty($_GET["fZdMinLimit2"]))
        $_GET["fZdMinLimit2"]="";

    if (empty($_GET["fZdMaxLimit1"]))
        $_GET["fZdMaxLimit1"]="";

    if (empty($_GET["fZdMaxLimit2"]))
        $_GET["fZdMaxLimit2"]="";

    // values taken from distribution or from experience
    if (empty($_GET["fUnsInLimit1"]))
        $_GET["fUnsInLimit1"]=$first?"12":"";

    if (empty($_GET["fUnsInLimit2"]))
        $_GET["fUnsInLimit2"]=$first?"20":"";

    if (empty($_GET["fRunTimeLimit1"]))
        $_GET["fRunTimeLimit1"]=$first?"5":"";

    if (empty($_GET["fRunTimeLimit2"]))
        $_GET["fRunTimeLimit2"]=$first?"2":"";

    if (empty($_GET["fPSFLimitRms"]))
        $_GET["fPSFLimitRms"]=$first?"1.0":"";

    if (empty($_GET["fPedRmsInLimitRms"]))
        $_GET["fPedRmsInLimitRms"]=$first?"0.08":"";

    if (empty($_GET["fNumIslLimitMean"]))
        $_GET["fNumIslLimitMean"]=$first?"1.230":"";

    if (empty($_GET["fNumIslLimitRms"]))
        $_GET["fNumIslLimitRms"]=$first?"0.013":"";

    if (empty($_GET["fMuonCalLimitMean"]))
        $_GET["fMuonCalLimitMean"]=$first?"100.0":"";

    if (empty($_GET["fMuonCalLimitRms"]))
        $_GET["fMuonCalLimitRms"]=$first?"1.6":"";

    if (empty($_GET["fRelTimeMaxLimit1"]))
        $_GET["fRelTimeMaxLimit1"]=$first?"1.0":"";

    if (empty($_GET["fRelTimeMaxLimit2"]))
        $_GET["fRelTimeMaxLimit2"]=$first?"1.02":"";

    if (empty($_GET["fRelTimeMinLimit1"]))
        $_GET["fRelTimeMinLimit1"]=$first?"0.97":"";

    if (empty($_GET["fRelTimeMinLimit2"]))
        $_GET["fRelTimeMinLimit2"]=$first?"0.93":"";

    if (empty($_GET["fIMCLimit1"]))
        $_GET["fIMCLimit1"]=$first?"00":"";

    if (empty($_GET["fIMCLimit2"]))
        $_GET["fIMCLimit2"]=$first?"00":"";

    if (empty($_GET["fIsoInLimit1"]))
        $_GET["fIsoInLimit1"]=$first?"00":"";

    if (empty($_GET["fIsoInLimit2"]))
        $_GET["fIsoInLimit2"]=$first?"00":"";

    if (empty($_GET["fInhomLimitMean"]))
        $_GET["fInhomLimitMean"]=$first?"10":"";

    if (empty($_GET["fInhomLimitRms"]))
        $_GET["fInhomLimitRms"]=$first?"3":"";

    if (empty($_GET["fImgRateLimitMean"]))
        $_GET["fImgRateLimitMean"]=$first?"180":"";

    if (empty($_GET["fImgRateLimitRms"]))
        $_GET["fImgRateLimitRms"]=$first?"30":"";

    if (empty($_GET["fMuonRateLimitMean"]))
        $_GET["fMuonRateLimitMean"]=$first?"1":"";

    if (empty($_GET["fMuonRateLimitRms"]))
        $_GET["fMuonRateLimitRms"]=$first?"0.1":"";

    if (empty($_GET["fMuonNumLimit1"]))
        $_GET["fMuonNumLimit1"]=$first?"180":"";

    if (empty($_GET["fMuonNumLimit2"]))
        $_GET["fMuonNumLimit2"]=$first?"120":"";

    if (empty($_GET["fPulsePosLimit1"]))
        $_GET["fPulsePosLimit1"]=$first?"3.5":"";

    if (empty($_GET["fPulsePosLimit2"]))
        $_GET["fPulsePosLimit2"]=$first?"2.5":"";

    if (empty($_GET["fNumStarsLimit1"]))
        $_GET["fNumStarsLimit1"]=$first?"30":"";

    if (empty($_GET["fNumStarsLimit2"]))
        $_GET["fNumStarsLimit2"]=$first?"20":"";

    if (empty($_GET["fNumStarsCorLimit1"]))
        $_GET["fNumStarsCorLimit1"]=$first?"10":"";

    if (empty($_GET["fNumStarsCorLimit2"]))
        $_GET["fNumStarsCorLimit2"]=$first?"5":"";

}

function InitBuildDataSets($first)
{
        if (empty($_GET["fNumResults"]))
//            $_GET["fNumResults"]="50";
            $_GET["fNumResults"]="10";

        if (empty($_GET["fRunStart"]))
            $_GET["fRunStart"]=$first?"On":"";

        if (empty($_GET["fRunTime/60"]))
            $_GET["fRunTime/60"]=$first?"On":"";

        if (empty($_GET["fZenithDistanceMin"]))
            $_GET["fZenithDistanceMin"]=$first?"On":"";

        if (empty($_GET["fZenithDistanceMax"]))
            $_GET["fZenithDistanceMax"]=$first?"On":"";

        if (empty($_GET["fSourceName"]))
            $_GET["fSourceName"]=$first?"On":"";

        if (empty($_GET["fObservationModeName"]))
            $_GET["fObservationModeName"]=$first?"On":"";

        if (empty($_GET["fUnsuitableInner"]))
            $_GET["fUnsuitableInner"]=$first?"On":"";

        if (empty($_GET["fUnreliableInner"]))
            $_GET["fUnreliableInner"]=$first?"On":"";

        if (empty($_GET["fIsolatedInner"]))
            $_GET["fIsolatedInner"]=$first?"On":"";

        if (empty($_GET["fIsolatedMaxCluster"]))
            $_GET["fIsolatedMaxCluster"]=$first?"On":"";

        if (empty($_GET["fArrTimeRmsInner"]))
            $_GET["fArrTimeRmsInner"]=$first?"On":"";

        if (empty($_GET["fMeanPedRmsInner"]))
            $_GET["fMeanPedRmsInner"]=$first?"On":"";

        if (empty($_GET["fPulsePosMean"]))
            $_GET["fPulsePosMean"]=$first?"On":"";

        if (empty($_GET["fConvFactorInner"]))
            $_GET["fConvFactorInner"]=$first?"On":"";

        if (empty($_GET["fInhomogeneity"]))
            $_GET["fInhomogeneity"]=$first?"On":"";

        if (empty($_GET["fPSF"]))
            $_GET["fPSF"]=$first?"On":"";

        if (empty($_GET["fMuonNumber"]))
            $_GET["fMuonNumber"]=$first?"On":"";

        if (empty($_GET["fEffOnTime/fRunTime"]))
            $_GET["fEffOnTime/fRunTime"]=$first?"On":"";

        if (empty($_GET["fMuonRate"]))
            $_GET["fMuonRate"]=$first?"On":"";

        if (empty($_GET["fDataRate"]))
            $_GET["fDataRate"]=$first?"On":"";

        if (empty($_GET["fAvgCloudiness"]))
            $_GET["fAvgCloudiness"]=$first?"On":"";

        if (empty($_GET["fNumStarsMed"]))
            $_GET["fNumStarsMed"]=$first?"On":"";

        if (empty($_GET["fNumStarsCorMed"]))
            $_GET["fNumStarsCorMed"]=$first?"On":"";

        if (empty($_GET["fOff"]))
            $_GET["fOff"]=$first?"On":"";

        if (empty($_GET["fLinks"]))
            $_GET["fLinks"]=$first?"On":"";

        if (empty($_GET["fOnlyOff"]))
            $_GET["fOnlyOff"]=$first?"Off":"";

        InitFindOffData($first);
}

// cta mc stuff
function InitCTAMCRunStatus($first)
{
    if (empty($_GET["fCorsikaSimTelarray"]))
        $_GET["fCorsikaSimTelarray"]="On";

    if (empty($_GET["fCorsikaSimTelarrayStatus"]))
        $_GET["fCorsikaSimTelarrayStatus"]="0";

    if (empty($_GET["fChimp"]))
        $_GET["fChimp"]="Off";

    if (empty($_GET["fChimpStatus"]))
        $_GET["fChimpStatus"]="0";

    if (empty($_GET["fCTAStar"]))
        $_GET["fCTAStar"]="Off";

    if (empty($_GET["fCTAStarStatus"]))
        $_GET["fCTAStarStatus"]="0";

    if (empty($_GET["fStereoB"]))
        $_GET["fStereoB"]="Off";

    if (empty($_GET["fStereoBStatus"]))
        $_GET["fStereoBStatus"]="0";

    if (empty($_GET["fStereoC"]))
        $_GET["fStereoC"]="Off";

    if (empty($_GET["fStereoCStatus"]))
        $_GET["fStereoCStatus"]="0";

    if (empty($_GET["fStereoG"]))
        $_GET["fStereoG"]="Off";

    if (empty($_GET["fStereoGStatus"]))
        $_GET["fStereoGStatus"]="0";

}

function InitCTAMCRunInfo($first)
{
    if (empty($_GET["fNumResults"]))
        $_GET["fNumResults"]="50";

    if (empty($_GET["fNumEvents"]))
        $_GET["fNumEvents"]="On";

    InitCTAMCRunStatus($first);
    InitInfo($first);

    if (empty($_GET["fNumEvents"]))
        $_GET["fNumEvents"]=$first?"Off":"";

    if (empty($_GET["fObsLevel"]))
        $_GET["fObsLevel"]="On";

    if (empty($_GET["fParticleTypeName"]))
        $_GET["fParticleTypeName"]="On";

    if (empty($_GET["fNumEvents"]))
        $_GET["fNumEvents"]=$first?"On":"";

    if (empty($_GET["fLinks"]))
        $_GET["fLinks"]=$first?"On":"";

}

//other MC stuff
function InitMCRunStatus($_GET, $first)
{

    if (empty($_GET["Corsika"]))
        $_GET["Corsika"]=$first?"On":"";

    if (empty($_GET["CorsikaStatus"]))
        $_GET["CorsikaStatus"]="0";

    if (empty($_GET["Ceres"]))
        $_GET["Ceres"]=$first?"On":"";

    if (empty($_GET["CeresStatus"]))
        $_GET["CeresStatus"]="0";

    if (empty($_GET["SequenceFile"]))
        $_GET["SequenceFile"]=$first?"Off":"";

    if (empty($_GET["SequenceFileStatus"]))
        $_GET["SequenceFileStatus"]="0";

    if (empty($_GET["Callisto"]))
        $_GET["Callisto"]=$first?"Off":"";

    if (empty($_GET["CallistoStatus"]))
        $_GET["CallistoStatus"]="0";

    if (empty($_GET["Star"]))
        $_GET["Star"]=$first?"On":"";

    if (empty($_GET["StarStatus"]))
        $_GET["StarStatus"]="0";

}

function InitMCRunInfo($_GET, $first)
{
    if (empty($_GET["fNumResults"]))
        $_GET["fNumResults"]="50";

    InitMCRunStatus($_GET, $first);
    InitInfo($_GET, $first);

    if (empty($_GET["fParticleTypeName"]))
        $_GET["fParticleTypeName"]=$first?"On":"";

    if (empty($_GET["fRunTypeName"]))
        $_GET["fRunTypeName"]=$first?"On":"";

    if (empty($_GET["fAtmosphericModelName"]))
        $_GET["fAtmosphericModelName"]="Off";

    if (empty($_GET["fCeresSetupName"]))
        $_GET["fCeresSetupName"]=$first?"On":"";

    if (empty($_GET["fNumEvents"]))
        $_GET["fNumEvents"]=$first?"Off":"";

    if (empty($_GET["fNumEvents*fNumReUseShowers"]))
        $_GET["fNumEvents*fNumReUseShowers"]=$first?"On":"";

    if (empty($_GET["fSequenceNumber"]))
        $_GET["fSequenceNumber"]=$first?"On":"";

    if (empty($_GET["fZenithDistanceMin"]))
        $_GET["fZenithDistanceMin"]=$first?"Off":"";

    if (empty($_GET["fZenithDistanceMax"]))
        $_GET["fZenithDistanceMax"]=$first?"Off":"";

    if (empty($_GET["fAzimuthMin"]))
        $_GET["fAzimuthMin"]=$first?"Off":"";

    if (empty($_GET["fAzimuthMax"]))
        $_GET["fAzimuthMax"]=$first?"Off":"";

    if (empty($_GET["fEnergyMin"]))
        $_GET["fEnergyMin"]=$first?"On":"";

    if (empty($_GET["fEnergyMax"]))
        $_GET["fEnergyMax"]=$first?"On":"";

    if (empty($_GET["fImpactMax"]))
        $_GET["fImpactMax"]=$first?"On":"";

    if (empty($_GET["fViewConeMax"]))
        $_GET["fViewConeMax"]=$first?"On":"";

    if (empty($_GET["fEnergySlope"]))
        $_GET["fEnergySlope"]=$first?"On":"";

    if (empty($_GET["fStartingAltitude"]))
        $_GET["fStartingAltitude"]=$first?"On":"";

    if (empty($_GET["fMirrorDiameter"]))
        $_GET["fMirrorDiameter"]=$first?"On":"";

    if (empty($_GET["fNumReUseShowers"]))
        $_GET["fNumReUseShowers"]=$first?"Off":"";

}


?>
