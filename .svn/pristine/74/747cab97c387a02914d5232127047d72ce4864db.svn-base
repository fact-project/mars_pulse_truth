<?php

$timelimits = array
    (
     "Corsika" => "60",
     "Ceres" => "2",
     "SequenceFile" => "1",
     "Callisto" => "1",
     "Star" => "1",
     );


$checkwhere = array
    (
     "fParticleTypeName"                 => CheckWhere("fParticleTypeKEY"),
     "fRunTypeName"                      => CheckWhere("fRunTypeKEY"),
     "fAtmosphericModelName"             => CheckWhere("fAtmosphericModelKEY"),
     "fCeresSetupName"                   => CheckWhere("fCeresSetupKEY"),
    );

$checkgroup = array
    (
     "fParticleTypeName"                 => CheckGroup("fParticleTypeKEY"),
     "fRunTypeName"                      => CheckGroup("fRunTypeKEY"),
     "fAtmosphericModelName"             => CheckGroup("fAtmosphericModelKEY"),
     "fCeresSetupName"                   => CheckGroup("fCeresSetupKEY"),
    );


$checkstatusgroup = array
    (
     "Corsika"      => CheckStatusGroup("Corsika"),
     "Ceres"        => CheckStatusGroup("Ceres"),
     "SequenceFile" => CheckStatusGroup("SequenceFile"),
     "Callisto"     => CheckStatusGroup("Callisto"),
     "Star"         => CheckStatusGroup("Star"),
    );



$alias = array
    (
     "Run.File"                         => "Run.File",
     "fSequenceNumber"                  => "Sequ",
     "fNumEvents"                       => "#Showers",
     "fNumEvents*fNumReUseShowers"      => "#Evts",
     "fNumReUseShowers"                 => "reused",
     "SUM(fNumEvents)"                  => "Showers",
     "SUM(fNumEvents*fNumReUseShowers)" => "Evts",
     "fParticleTypeName"                => "Particle",
     "fRunTypeName"                     => "RunType",
     "fAtmosphericModelName"            => "Atm.Model",
     "fCeresSetupName"                  => "CeresSetup",
     "fCorsikaInputCreated"             => "Input",
     "Corsika"                          => "Corsika",
     "Ceres"                            => "Ceres",
     "SequenceFile"                     => "SequenceFile",
     "Callisto"                         => "Callisto",
     "Star"                             => "Star",
     "fZenithDistanceMin"               => "Zd<br>Min",
     "fZenithDistanceMax"               => "Zd<br>Max",
     "fAzimuthMin"                      => "Az<br>Min",
     "fAzimuthMax"                      => "Az<br>Max",
     "Min(fZenithDistanceMin)"          => "ZDMin",
     "Max(fZenithDistanceMax)"          => "ZDMax",
     "fEnergyMin"                       => "EMin<br>[GeV]",
     "fEnergyMax"                       => "EMax<br>[GeV]",
     "fImpactMax"                       => "Impact<br>[cm]",
     "fViewConeMax"                     => "View<br>cone<br>[deg]",
     "fEnergySlope"                     => "Slope",
     "fStartingAltitude"                => "Altitude<br>[g/sqcm]",
     "fMirrorDiameter"                  => "Mirror<br>diam.<br>[cm]",
    );


$rightalign = array
    (
     $alias["fSequenceNumber"]              => "1",
     $alias["fNumEvents"]                   => "1",
     $alias["fNumEvents*fNumReUseShowers"]  => "1",
     $alias["fZenithDistanceMin"]           => "1",
     $alias["fZenithDistanceMax"]           => "1",
     $alias["fAzimuthMin"]                  => "1",
     $alias["fAzimuthMax"]                  => "1",
     $alias["fEnergyMin"]                   => "1",
     $alias["fEnergyMax"]                   => "1",
     $alias["fImpactMax"]                   => "1",
     $alias["fViewConeMax"]                 => "1",
     $alias["fEnergySlope"]                 => "1",
     $alias["fStartingAltitude"]            => "1",
     $alias["fMirrorDiameter"]              => "1",
     "Time"                                 => "1",
     "Runs"                                 => "1",
     "Evts"                                 => "1",
);

function GetCheck($fromtable, $val)
{
    $checks = array
        (
         "fParticleTypeName"         => "ParticleType.fParticleTypeKEY",
         "fRunTypeName"              => "RunType.fRunTypeKEY",
         "fAtmosphericModelName"     => "AtmosphericModel.fAtmosphericModelKEY",
         "fCeresSetupName"           => "CeresSetup.fCeresSetupKEY",
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
         "fNumEvents"                        => "CorsikaInfo.fNumEvents",
         "fMCRunNumber"                      => "MCCorsikaRunData.fMCRunNumber",
         "fRawFileAvail"                     => "RunProcessStatus.fRawFileAvail",
         "SUM(fRunTime)/3600"                => "'Time [h]'",
         "SUM(fNumEvents)"                   => "'Evts'",
         "fParticleTypeName"                 => "ParticleType.fParticleTypeName",
         "fRunTypeName"                      => "RunType.fRunTypeName",
         "fAtmosphericModelName"             => "AtmosphericModel.fAtmosphericModelName",
         "fCeresSetupName"                   => "CeresSetup.fCeresSetupName",
         "fAzimuthMin"                       => "CorsikaInfo.fAzimuthMin",
         "fAzimuthMax"                       => "CorsikaInfo.fAzimuthMax",
         "fZenithDistanceMin"                => "CorsikaInfo.fZenithDistanceMin",
         "fZenithDistanceMax"                => "CorsikaInfo.fZenithDistanceMax",
         "fEnergyMin"                        => "CorsikaInfo.fEnergyMin",
         "fEnergyMax"                        => "CorsikaInfo.fEnergyMax",
         "fImpactMax"                        => "CorsikaInfo.fImpactMax",
         "fViewConeMax"                      => "CorsikaInfo.fViewConeMax",
         "fEnergySlope"                      => "CorsikaInfo.fEnergySlope",
         "fStartingAltitude"                 => "CorsikaInfo.fStartingAltitude",
         "fMirrorDiameter"                   => "CorsikaInfo.fMirrorDiameter",
         "Min(fZenithDistanceMin)"           => "'ZDMin'",
         "Max(fZenithDistanceMax)"           => "'ZDMax'",
        );

    $table="";
    if (!empty($tables[$val]))
        $table=$tables[$val];
    if ($val=="Run.File")
        $table="'Run.File'";
    if ($val=="Tel:Sequ")
        $table="CONCAT('M', Sequences.fTelescopeNumber, ':', LPAD(Sequences.fSequenceFirst, 8, '0'))";
    if ($val=="NumRuns")
        $table="'# Runs'";
    if ($val=="NumSequ")
        $table="'# Sequ'";
    if ($val=="NumDS")
        $table="'# Datasets'";
    if ($val=="# Files")
        $table="'# Files'";
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
         "fParticleTypeKEY"           => " LEFT JOIN ParticleType         USING(fParticleTypeKEY) ",
         "fRunTypeKEY"                => " LEFT JOIN RunType              USING(fRunTypeKEY) ",
         "fAtmosphericModelKEY"       => " LEFT JOIN AtmosphericModel     USING(fAtmosphericModelKEY) ",
         "fCeresSetupKEY"             => " LEFT JOIN CeresSetup           USING(fCeresSetupKEY) ",
         "fParticleTypeName"          => " LEFT JOIN ParticleType         USING(fParticleTypeKEY) ",
         "fRunTypeName"               => " LEFT JOIN RunType              USING(fRunTypeKEY) ",
         "fAtmosphericModelName"      => " LEFT JOIN AtmosphericModel     USING(fAtmosphericModelKEY) ",
         "fCeresSetupName"            => " LEFT JOIN CeresSetup           USING(fCeresSetupKEY) ",
         "CorsikaStatus"              => " LEFT JOIN CorsikaStatus        USING(fRunNumber, fFileNumber) ",
         "CeresStatus"                => " LEFT JOIN CeresStatus          USING(fRunNumber, fCeresSetupKEY) ",
         "SequenceFileStatus"         => " LEFT JOIN SequenceFileStatus   USING(fSequenceNumber) ",
         "CallistoStatus"             => " LEFT JOIN CallistoStatus       USING(fSequenceNumber, fCeresSetupKEY) ",
         "StarStatus"                 => " LEFT JOIN StarStatus           USING(fSequenceNumber, fCeresSetupKEY) ",
         "Corsika"                    => " LEFT JOIN CorsikaStatus        USING(fRunNumber, fFileNumber) ",
         "Ceres"                      => " LEFT JOIN CeresStatus          USING(fRunNumber, fCeresSetupKEY) ",
         "SequenceFile"               => " LEFT JOIN SequenceFileStatus   USING(fSequenceNumber) ",
         "Callisto"                   => " LEFT JOIN CallistoStatus       USING(fSequenceNumber, fCeresSetupKEY) ",
         "Star"                       => " LEFT JOIN StarStatus           USING(fSequenceNumber, fCeresSetupKEY) ",
        );


    $join="";
    if (!empty($joins[$val]))
        $join=$joins[$val];
    return $join;
}


?>
