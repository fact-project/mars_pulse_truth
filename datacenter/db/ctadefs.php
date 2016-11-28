<?php

$needs = array
    (
     "fCorsikaSimTelarray"   => "MCRunProcessStatus.fMCRunNumber",
     "fChimp"                => "MCRunProcessStatus.fCorsikaSimTelarray",
     "fCTAStar"              => "MCRunProcessStatus.fChimp",
     "fStereoB"              => "MCRunProcessStatus.fCTAStar",
     "fStereoC"              => "MCRunProcessStatus.fCTAStar",
     "fStereoG"              => "MCRunProcessStatus.fCTAStar",
     );


$timelimits = array
    (
     "fCorsikaSimTelarray"   => "48",
     "fChimp"                => "4",
     "fCTAStar"              => "2",
     "fStereoB"              => "1",
     "fStereoC"              => "1",
     "fStereoG"              => "1",
     );


$checkwhere = array
    (
     "fParticleTypeName"  => CheckWhere("fParticleTypeKEY"),
    );

$checkgroup = array
    (
     "fParticleTypeName"  => CheckGroup("fParticleTypeKEY"),
    );


$checkstatusgroup = array
    (
     "fCorsikaSimTelarray" => CheckStatusGroup("fCorsikaSimTelarrayStatus"),
     "fChimp"              => CheckStatusGroup("fChimpStatus"),
     "fCTAStar"            => CheckStatusGroup("fCTAStarStatus"),
     "fStereoB"            => CheckStatusGroup("fStereoBStatus"),
     "fStereoC"            => CheckStatusGroup("fStereoCStatus"),
     "fStereoG"            => CheckStatusGroup("fStereoGStatus"),
    );



$alias = array
    (
//     "fRunNumber"                       => "Run#",
     "Run#"                     => "Run#",
     "fNumEvents"               => "#Evts",
     "fObsLevel"                => "ObsLev<br>[m]",
     "SUM(fNumEvents)"          => "Evts",
     "SUM(fNumEvents) DIV 1000" => "kEvts",
     "fParticleTypeName"        => "Particle",
     "fCorsikaSimTelarray"      => "CorsikaSimtel",
     "fChimp"                   => "Chimp",
     "fCTAStar"                 => "Star",
     "fStereoB"                 => "StereoB",
     "fStereoC"                 => "StereoC",
     "fStereoG"                 => "StereoG",
     "fStartTime"               => "Process",
     "fFailedTime"              => "Failed",
     "fReturnCode"              => "Ret<br>Code",
     "fProgramId"               => "Prgr<br>Id",
    );


$rightalign = array
    (
//     $alias["fRunNumber"]                => "1",
     $alias["fNumEvents"]                => "1",
     $alias["fObsLevel"]                 => "1",
     $alias["SUM(fNumEvents) DIV 1000"]  => "1",
     "Time"                              => "1",
     "Runs"                              => "1",
     "Evts"                              => "1",
);

function GetCheck($fromtable, $val)
{
    $checks = array
        (
         "fParticleTypeName"                 => $fromtable . ".fParticleTypeKEY",
        );

    $check="";
    if (!empty($checks[$val]))
        $check=$checks[$val];
    return $check;
}


function GetTable($fromtable, $val)
{
    $tables = array
        (
         "fNumEvents"                        => $fromtable . ".fNumEvents",
         "fMCRunNumber"                      => "MCRunData.fMCRunNumber",
         "fRawFileAvail"                     => "RunProcessStatus.fRawFileAvail",
         "SUM(fRunTime)/3600"                => "'Time [h]'",
         "SUM(fNumEvents)"                   => "'Evts'",
         "fStartTime"                        => "fStartTime",
         "fFailedTime"                       => "fFailedTime",
         "fReturnCode"                       => "fReturnCode",
         "fProgramId"                        => "fProgramId",
         "fParticleTypeName"                 => "ParticleType.fParticleTypeName",
        );

    $table="";
    if (!empty($tables[$val]))
        $table=$tables[$val];
    if ($val=="Tel:Run/File")
        $table="'Tel:Run/File'";
    if ($val=="Tel:Sequ")
        $table="CONCAT('M', Sequences.fTelescopeNumber, ':', LPAD(Sequences.fSequenceFirst, 8, '0'))";
    if ($val=="NumRuns")
        $table="'# Runs'";
    if ($val=="NumSequ")
        $table="'# Sequ'";
    if ($val=="NumDS")
        $table="'# Datasets'";
    if ($val=="NumDays")
        $table="'# days'";
    if (empty($table))
        $table=$val;
    return $table;
}


function GetJoin($fromtable, $val)
{
    $joins = array
        (
         "fParticleTypeName"                => " LEFT JOIN ParticleType                USING(fParticleTypeKEY) ",
        );


    $join="";
    if (!empty($joins[$val]))
        $join=$joins[$val];
    return $join;
}


?>
