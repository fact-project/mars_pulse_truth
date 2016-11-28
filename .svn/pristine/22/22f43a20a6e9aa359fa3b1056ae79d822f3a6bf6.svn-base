<?php

//arrays and functions for plot.php and plotdb.php
include("include.php");
include("magicdefs.php");
include("db.php");

$tables=array("fSequenceFirst" => "Sequences",
              "fRunNumber"     => "RunData",
              "fDataSetNumber" => "DataSets");

$prims=array("SequenceNumber" => "fSequenceFirst",
             "RunNumber"      => "fRunNumber",
             "DataSetNumber"  => "fDataSetNumber");

$joins=array("fSequenceFirst" => " LEFT JOIN Star USING(fSequenceFirst,fTelescopeNumber) "
             ."LEFT JOIN Calibration USING(fSequenceFirst,fTelescopeNumber) ",
             "fRunNumber" => " LEFT JOIN DataCheck USING(fRunNumber) ",
             "fDataSetNumber" =>" LEFT JOIN Ganymed USING(fDataSetNumber) ");

function GetList($host, $user, $pw, $db, $column, $tables)
{
    $tab_fSequenceFirst=array("Sequences", "Calibration", "Star");
    $tab_fRunNumber=array("RunData");
    $tab_fDataSetNumber=array("DataSets", "Ganymed");

    $table=$tables[$column];
    foreach(${"tab_".$column} as $tab)
    {
        $query="EXPLAIN " . $tab;

        mysql_connect($host, $user, $pw);
        mysql_select_db($db);

        $result=mysql_query($query);

        while ($row = mysql_fetch_row($result))
            $list[] = $row[0];
    }
    return $list;
}

function CheckQuery($query)
{
    if (strpos($query, " WHERE ")==FALSE)
        return " WHERE ";
    else
        return " AND ";
}

function GetQuery($tables, $joins, $set)
{
    if (empty($_GET["prim"]))
        $_GET["prim"]="";
    $primary=$_GET["prim"];
    if (empty($primary))
        return "";
    if (empty($_GET[$primary."Col"]))
        $_GET[$primary."Col"]="";
    $column=$_GET[$primary."Col"];
    $table=$tables[$primary];
    if (empty($_GET["primstart"]))
        $_GET["primstart"]="";
    $primstart=$_GET["primstart"];
    if (empty($_GET["primstop"]))
        $_GET["primstop"]="";
    $primstop=$_GET["primstop"];
    if (empty($_GET["Start"]))
        $_GET["Start"]="";
    $start1=$_GET["Start"];
    if (empty($_GET["Stop"]))
        $_GET["Stop"]="";
    $stop1=$_GET["Stop"];

    if (empty($_GET["fSourceN"]))
        $_GET["fSourceN"]="";
    $source=$_GET["fSourceN"];

    $query="SELECT " . $table . "." . $primary . ", ";
    if ($column==$primary)
        $query.=$table . "." . $column;
    else
        $query.=$column;

    if ($_GET["plot"]=="val")
    {
        if (empty($_GET[$primary."Col2"]))
            $_GET[$primary."Col2"]="";
        $column2=$_GET[$primary."Col2"];
        if (empty($_GET["Start2"]))
            $_GET["Start2"]="";
        $start2=$_GET["Start2"];
        if (empty($_GET["Stop2"]))
            $_GET["Stop2"]="";
        $stop2=$_GET["Stop2"];

        if ($column2==$primary)
            $query.=", " . $table . "." . $column2;
        else
            $query.=", " . $column2;
    }
    $query.=" FROM " . $table;
    $query.=$joins[$primary];

    if (!empty($source))
        $query.= " LEFT JOIN Source USING(fSourceKEY) ";

    if (!empty($primstart))
        $query.= " WHERE " . $table . "." . $primary . " > " . $primstart;

    if (!empty($primstop))
        $query.= CheckQuery($query) . $table . "." . $primary . " < " . $primstop;

    if (!empty($start1))
        $query.= CheckQuery($query) . GetTable($table, $column) . " > " . $start1;

    if (!empty($stop1))
        $query.= CheckQuery($query) . GetTable($table, $column) . " < " . $stop1;

    if ($_GET["plot"]=="val")
    {
        if (!empty($start2))
            $query.= CheckQuery($query) . GetTable($table, $column2) . " > " . $start2;

        if (!empty($stop2))
            $query.= CheckQuery($query) . GetTable($table, $column2) . " < " . $stop2;
    }

    if (!empty($set) && !empty($_GET[$set]))
        $query.= CheckQuery($query) . $table . "." . $primary . " IN (" . str_replace(" ", ",", $_GET[$set]) . ") ";

    if (empty($_GET[$primary."Col3"]))
        $_GET[$primary."Col3"]="";
    $column3=$_GET[$primary."Col3"];
    if (empty($_GET["Start3"]))
        $_GET["Start3"]="";
    $start3=$_GET["Start3"];
    if (empty($_GET["Stop3"]))
        $_GET["Stop3"]="";
    $stop3=$_GET["Stop3"];

    if (!empty($start3))
        $query.= CheckQuery($query) . GetTable($table, $column3) . " > " . $start3;

    if (!empty($stop3))
        $query.= CheckQuery($query) . GetTable($table, $column3) . " < " . $stop3;

    if (empty($_GET[$primary."Col4"]))
        $_GET[$primary."Col4"]="";
    $column4=$_GET[$primary."Col4"];
    if (empty($_GET["Start4"]))
        $_GET["Start4"]="";
    $start4=$_GET["Start4"];
    if (empty($_GET["Stop4"]))
        $_GET["Stop4"]="";
    $stop4=$_GET["Stop4"];

    if (!empty($start4))
        $query.= CheckQuery($query) . GetTable($table, $column4) . " > " . $start4;

    if (!empty($stop4))
        $query.= CheckQuery($query) . GetTable($table, $column4) . " < " . $stop4;

    if (!empty($source))
        $query.= CheckQuery($query) . " fSourceName REGEXP \"^" . $_GET["fSourceN"] . "\" ";

    return $query;
}

//functions for showplots*.php
function gettypename($type2)
{
    switch ($type2)
    {
    case "calib":
        $type="callisto";
        break;
    case "signal":
        $type="callisto";
        break;
    case "star":
        $type="star";
        break;
    }
    return $type;
}

function getplotname($seq, $tabnum, $type, $type2)
{
    $seqlong=str_pad($seq, 8, "0", STR_PAD_LEFT);
    $seqshort=substr($seqlong, 0, 4);
    $plot  = "http://datacenter.astro.uni-wuerzburg.de/";
    $plot .= $type . "/" . str_pad( $seqshort, 4, "0", STR_PAD_LEFT);
//    $plot .= "/" . $seqlong . "/" . $type2 . $seqlong . "-tab" . $tabnum . ".png";
    $plot .= "/" . $seqlong . "/" . $type2 . $tabnum . ".png";
    return $plot;
}

function getinfofromdb($seq)
{
    include("db.php");

    $db_id = mysql_pconnect($host, $user, $pw);
    if ($db_id==FALSE)
    {
        printf("mysql_connect returned the following error: %s\n", mysql_error());
        die("");
    }
    mysql_select_db($db);

    $query0  = "SELECT fSequenceFirst ";
    $query0 .= ", DATE_FORMAT(ADDDATE(fRunStart,Interval 12 hour), '%Y-%m-%d') as 'ObsNight'";
    $query0 .= ", fSourceName ";
    $query0 .= ", fZenithDistanceMin ";
    $query0 .= ", fZenithDistanceMax ";
    $query0 .= ", fMeanPedRmsInner ";
    $query0 .= ", fRunTime/60 as 'UpTime' ";
    $query0 .= ", if(isnull(fStar), 'N/A', if(isnull(fRateSumEvts), 'no', CONCAT('yes (',fRateSumEvts, ')'))) as 'Sum' ";
    $query0 .= "FROM Sequences ";
    $query0 .= "LEFT JOIN Source USING(fSourceKEY) ";
    $query0 .= "LEFT JOIN Calibration USING(fSequenceFirst) ";
    $query0 .= "LEFT JOIN Star USING(fSequenceFirst) ";
    $query0 .= "LEFT JOIN SequenceProcessStatus USING(fSequenceFirst) ";
    $query0 .= "WHERE fSequenceFirst=".$seq;

//    echo $query0;

    $result0 = mysql_query($query0, $db_id);
    while ($row0 = mysql_fetch_assoc($result0))
        $info="<b>".$row0["fSourceName"]."</b> ".$row0["ObsNight"]
            ." (".$row0["UpTime"]."min)"
            ." Zd: ".$row0["fZenithDistanceMin"]."-".$row0["fZenithDistanceMax"]
            ." PedRmsIn: ".$row0["fMeanPedRmsInner"]
            ." SumTrigger: ".$row0["Sum"];

    mysql_free_result($result0);
    mysql_close($db_id);

    return $info;
}

function PrintType2Pulldown($type2)
{
    $types=array("calib", "signal", "star");
    printf("<select name='type2'>\n");
    foreach($types as $t => $typename)
        if ($type2==$typename)
            printf("<option value='%s' selected>%s </option>\n", $type2, $type2);
        else
            printf("<option value='%s'>%s </option>\n", $typename, $typename);
    print("</select>\n");
}

function PrintHomeHelp()
{
    printf("<td valign='top'>\n");
    printf("<a href='http://www.astro.uni-wuerzburg.de/wiki/Database_Websites'>home</a>\n");
    printf("<a href='http://www.astro.uni-wuerzburg.de/wiki/Database_Tutorial#showplots:_calib.2C_signal.2C_star'>help</a>\n");
    printf("</td>\n");
}


?>
