<?php

$checkwhere = array
    (
     "fRunTypeName"                      => CheckWhere("fRunTypeKEY"),
    );

$checkgroup = array
    (
     "fRunTypeName"                      => CheckGroup("fRunTypeKEY"),
    );


            //don't put a '#' at the beginning of the alias, else SortBy won't work anymore
$alias = array
    (
     "NumRuns"                          => "#Runs",
     "Run"                              => "Run",
     "fRunTypeName"                     => "RunType",
     "if(isnull(fCheckSum)+isnull(fDataSum),Concat(fMd5sumRaw,\"/\",fMd5sumRawZip),Concat(fCheckSum,\"/\",fDataSum))" => "checksum",
     "fROI"                             => "roi",
     "fROITimeMarker"                   => "roi-tm",
     "fLastUpdate"                      => "LastUpd",
     //"SUM(fRunTime)/3600"               => "Time [h]",
     "SUM(fNumEvents)"                  => "Evts",
     "Min(fZenithDistanceMin)"          => "ZDMin",
     "Max(fZenithDistanceMax)"          => "ZDMax",
     "Min(fZenithDistance)"             => "ZdMin",
     "Max(fZenithDistance)"             => "ZdMax",
     "fNumEvents"                       => "#Evts",
     "fRunStart"                        => "start",
     "fRunStop"                         => "stop",
     "fFitsFileErrors"                  => "file err",
     "fCompileTime"                     => "compiled",
     "fRevisionNumber"                  => "rev#",
     "if(fHasDrsFile=0,\"N\",fDrsStep)" => "drs",
     "TimeDiff(fRunStop,fRunStart)"     => "duration",
     "Round(fNumPhysicsTrigger/Time_to_sec(TimeDiff(fRunStop,fRunStart)),1)"  => "phys<br>rate<br>[Hz]",
     "fNumPhysicsTrigger"               => "#Phys",
     "fNumExt1Trigger"                  => "#Ext1",
     "fNumExt2Trigger"                  => "#Ext2",
     "fNumELPTrigger"                   => "#ELP",
     "fNumILPTrigger"                   => "#ILP",
     "fNumPedestalTrigger"              => "#Ped",
     "fNumTimeTrigger"                  => "#Time",
     "fNumOtherTrigger"                 => "#Other",
     //"Round(Sum(TimeDiff(fRunStop,fRunStart))/60./60.,1)"  => "Time [h]",
     "fTriggerRateMedian"               => "median rate",
     "fBiasVoltageMedian"               => "median bias",
     "fThresholdMedian"                 => "median<br>threshold",
     "fRightAscension"                  => "RA",
     "fDeclination"                     => "Decl",
     "fZenithDistanceMean"              => "Zd",
     "fMoonZenithDistance"              => "ZdMoon",
     "fMoonDisk"                        => "MoonDisk",
     "fAngleToMoon"                     => "Angle2Moon",
     "fSunZenithDistance"               => "ZdSun",
     "fAzimuthMean"                     => "Az",
     "fCameraTempMean"                  => "CamTemp",
     "fCameraTempRms"                   => "CamTempRms",
     "fOutsideTempMean"                 => "Temp",
     "fOutsideTempRms"                  => "TempRms",
     //"Sec_to_time(Sum(Time_to_sec(TimeDiff(fRunStop,fRunStart))))" => "Time<br>[hh:mm:ss]",
     //"Time [hh:mm:ss]" => "Time<br>[hh:mm:ss]",
    );


$rightalign = array
    (
     //$alias["fRunTypeName"]              => "1",
     $alias["fNumEvents"]                => "1",
     $alias["Round(fNumPhysicsTrigger/Time_to_sec(TimeDiff(fRunStop,fRunStart)),1)"] => "1",
     $alias["fNumPhysicsTrigger"]        => "1",
     $alias["fNumExt1Trigger"]           => "1",
     $alias["fNumExt2Trigger"]           => "1",
     $alias["fNumELPTrigger"]            => "1",
     $alias["fNumILPTrigger"]            => "1",
     $alias["fNumPedestalTrigger"]       => "1",
     $alias["fNumTimeTrigger"]           => "1",
     $alias["fNumOtherTrigger"]          => "1",
     $alias["fFitsFileErrors"]           => "1",
     $alias["fRevisionNumber"]           => "1",
     $alias["fROI"]                      => "1",
     $alias["fROITimeMarker"]            => "1",
     //"Time [h]"                          => "1",
     "Time<br>[hh:mm:ss]"                          => "1",
     "NumRuns"                           => "1",
     "Evts"                              => "1",
);

function GetCheck($fromtable, $val)
{
    $checks = array
        (
         "fRunTypeName"                      => $fromtable . ".fRunTypeKEY",
        );

    $check="";
    if (!empty($checks[$val]))
        $check=$checks[$val];
    return $check;
}


function GetJoin($fromtable, $val)
{
    $joins = array
        (
         "fRunTypeName"                     => " LEFT JOIN RunType                     USING(fRunTypeKEY) ",
        );


    $join="";
    if (!empty($joins[$val]))
        $join=$joins[$val];
    return $join;
}


?>
