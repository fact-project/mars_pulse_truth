<?php
{
    function CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup)
    {
        $fromtable="RunInfo";

        $groups = 0;
        foreach ($checkgroup as $element)
            if ($element==-1)
                $groups++;

        $query0 = "SELECT SQL_CALC_FOUND_ROWS ";

        if ($groups>0)
        {
            foreach ($checkgroup as $key => $element)
                if ($element==-1)
                    $query0 .=  $key . " as '" . $alias[$key] . "' " . ", ";
            //don't put a '#' at the beginning of the alias, else SortBy won't work anymore
            $query0 .= " COUNT(*) as 'NumRuns', ";
            //$query0 .= " Round(Sum(TimeDiff(fRunStop,fRunStart))/60./60.,2) as 'Time [h]' ";
            $query0 .= " Sec_to_time(Sum(Time_to_sec(TimeDiff(fRunStop,fRunStart)))) as 'Time<br>[hh:mm:ss]' ";
        }
        else
        {
            $query0 .= " Concat(fNight,'_', LPAD(fRunID, 3, 0)) as 'Run' ";

            foreach ($_GET as $key => $element)
                if ($_GET[$key]=="On")
                    if ($key!="fComments")
                        if (empty($checkwhere[$key]) || $checkwhere[$key]==0)
                            $query0 .= ", " . $key . " as '" . $alias[$key] . "' ";

            if (!empty($_GET["fComments"]))
            {
                //$query0 .= ", if (Count(fComment)>0, Concat('<a&ws;id=\'commentcell', fNight, fRunID, '\'&ws;href=\'run_comment.php?fNight=',fNight,'&fRunID=', fRunID, '\'&ws;onmouseover=\'get_comments(', fNight, ',', fRunID, ')\'>', Count(fComment), '</a>'), ' ') ";
                $query0 .= ", CONCAT(if (Count(fComment)>0, Concat('<a&ws;id=\'commentcell', fNight, fRunID, '\'&ws;href=\'run_comment.php?fNight=',fNight,'&fRunID=', fRunID, '\'&ws;onmouseover=\'get_comments(', fNight, ',', fRunID, ')\'>', Count(fComment), '</a>')";
                $query0 .= ", ' '), ' ', ";
                $query0 .= "CONCAT('<a&ws;id=\'commentcell', fNight, fRunID, '\'&ws;href=\'../../logbook/calendar.php?calendar=1&year=', left(fNight,4), '&month=', left(right(fNight,4),2), '&day=', right(fNight,2), '&action=dayview\'>lbk</a>', '')) ";
                //$query0 .= ", if (Count(fComment)>0, Concat('<a&ws;href=\'run_comment.php?fNight=',fNight,'&fRunID=', fRunID, '\'&ws;onmouseover=\'bla\'>', Count(fComment), '</a>'), ' ') ";
                $query0 .= " as 'Comments'";
            }

        }

        $query0 .= " FROM " . $fromtable;

        foreach ($_GET as $key => $element)
            if (($_GET[$key]=="On" || $groups>0))// && !empty($joins[$key]))
                $query0 .= GetJoin($fromtable, $key);
        $query0 .= " LEFT JOIN RunComments USING (fNight, fRunID) ";

        foreach ($checkwhere as $key => $element)
        {
            if (empty($element) || $element<=0)
                continue;

            if (strpos($query0, " WHERE ")==FALSE)
                $query0 .= " WHERE ";
            else
                if ($element!=-1)
                    if (strrpos($query0, " AND ")!=strlen($query0)-5)// this if clause doesn't work
                        $query0 .= " AND ";

            if ($element!=-1)
                $query0 .= GetCheck($fromtable, $key) . "=" . $element;
        }

        if (strpos($query0, " WHERE ")==FALSE)
            $query0 .= " WHERE ";
        else
            $query0 .= " AND ";

        if (!empty($_GET["fStartDate"]))
        {
            $startdate=substr($_GET["fStartDate"], 0, 10);
//            if ($startdate=="0000-00-00")
                $query0 .=" fNight >= " . $startdate . " ";
//            else
//                $query0 .= " fNight >= " . $startdate . " - 1 ";
        }

        if (!empty($_GET["fStopDate"]))
        {
//            if (strpos(strrev($query0), " DNA ")!=0)
//                $query0 .= " AND ";

            $stopdate=substr($_GET["fStopDate"], 0, 10);
            $query0 .= " AND fNight <= " . $stopdate . "  ";
        }

        if ($groups>0)
        {
            $query0 .= " GROUP BY ";
            $num = $groups;
            foreach ($checkgroup as $key => $element)
                if ($element==-1)
                {
                    $query0 .= GetCheck($fromtable, $key);
                    if ($num-->1)
                        $query0 .= ", ";
                }
        }
        else
            $query0 .= " GROUP BY fNight, fRunID ";



        if (!empty($_GET["fSortBy"]))
        {
            $query0 .= " ORDER BY " . substr($_GET["fSortBy"], 0, -1) . " ";
            if (substr($_GET["fSortBy"], -1)=="-")
                $query0 .= "DESC";
        }
        else
            if ($groups==0)
                $query0 .= " ORDER BY Run DESC";

        if (empty($_GET["fNumStart"]))
            $_GET["fNumStart"]=0;

        if (empty($_GET["fSendTxt"]))
            $query0 .= " LIMIT " . $_GET["fNumStart"] . ", " . $_GET["fNumResults"];

        return $query0;
    }

    function InitGet($_GET)
    {
        // Find out whether it is the first call to the php script
        $first = empty($_GET["fStartDate"]) && empty($_GET["fStopDate"]);

        if (empty($_GET["fNumResults"]))
            $_GET["fNumResults"]="200";
            
        if (empty($_GET["fNumEvents"]))
            $_GET["fNumEvents"]=$first?"Off":"";

        if (empty($_GET["fComments"]))
            $_GET["fComments"]=$first?"On":"";

        if (empty($_GET["fNumPhysicsTrigger"]))
            $_GET["fNumPhysicsTrigger"]=$first?"On":"";

        if (empty($_GET["Round(fNumPhysicsTrigger/Time_to_sec(TimeDiff(fRunStop,fRunStart)),1)"]))
            $_GET["Round(fNumPhysicsTrigger/Time_to_sec(TimeDiff(fRunStop,fRunStart)),1)"]=$first?"On":"";

        if (empty($_GET["fNumPedestalTrigger"]))
            $_GET["fNumPedestalTrigger"]=$first?"On":"";

        if (empty($_GET["fNumELPTrigger"]))
            $_GET["fNumELPTrigger"]=$first?"On":"";

        if (empty($_GET["fNumTimeTrigger"]))
            $_GET["fNumTimeTrigger"]=$first?"Off":"";

        if (empty($_GET["fNumExt1Trigger"]))
            $_GET["fNumExt1Trigger"]=$first?"Off":"";

        if (empty($_GET["fNumExt2Trigger"]))
            $_GET["fNumExt2Trigger"]=$first?"Off":"";

        if (empty($_GET["fNumILPTrigger"]))
            $_GET["fNumILPTrigger"]=$first?"Off":"";

        if (empty($_GET["fNumOtherTrigger"]))
            $_GET["fNumOtherTrigger"]=$first?"Off":"";

        if (empty($_GET["TimeDiff(fRunStop,fRunStart)"]))
            $_GET["TimeDiff(fRunStop,fRunStart)"]=$first?"On":"";

        if (empty($_GET["fRunStart"]))
            $_GET["fRunStart"]=$first?"On":"";

        if (empty($_GET["fRunStop"]))
            $_GET["fRunStop"]=$first?"Off":"";

        if (empty($_GET["fROI"]))
            $_GET["fROI"]=$first?"On":"";

        if (empty($_GET["fROITimeMarker"]))
            $_GET["fROITimeMarker"]=$first?"Off":"";

        if (empty($_GET["fFitsFileErrors"]))
            $_GET["fFitsFileErrors"]=$first?"Off":"";

        if (empty($_GET["fCompileTime"]))
            $_GET["fCompileTime"]=$first?"Off":"";

        if (empty($_GET["fRevisionNumber"]))
            $_GET["fRevisionNumber"]=$first?"Off":"";

        if (empty($_GET["if(fHasDrsFile=0,\"N\",fDrsStep)"]))
            $_GET["if(fHasDrsFile=0,\"N\",fDrsStep)"]=$first?"Off":"";

        if (empty($_GET["if(isnull(fCheckSum)+isnull(fDataSum),Concat(fMd5sumRaw,\"/\",fMd5sumRawZip),Concat(fCheckSum,\"/\",fDataSum))"]))
            $_GET["if(isnull(fCheckSum)+isnull(fDataSum),Concat(fMd5sumRaw,\"/\",fMd5sumRawZip),Concat(fCheckSum,\"/\",fDataSum))"]=$first?"Off":"";

        if (empty($_GET["fRunTypeName"]))
            $_GET["fRunTypeName"]=$first?"On":"";

        if (empty($_GET["fTriggerRateMedian"]))
            $_GET["fTriggerRateMedian"]=$first?"Off":"";

        if (empty($_GET["fThresholdMedian"]))
            $_GET["fThresholdMedian"]=$first?"On":"";

        if (empty($_GET["fBiasVoltageMedian"]))
            $_GET["fBiasVoltageMedian"]=$first?"Off":"";

        if (empty($_GET["fRightAscension"]))
            $_GET["fRightAscension"]=$first?"Off":"";

        if (empty($_GET["fDeclination"]))
            $_GET["fDeclination"]=$first?"Off":"";

        if (empty($_GET["fZenithDistanceMean"]))
            $_GET["fZenithDistanceMean"]=$first?"On":"";

        if (empty($_GET["fAzimuthMean"]))
            $_GET["fAzimuthMean"]=$first?"On":"";

        if (empty($_GET["fMoonZenithDistance"]))
            $_GET["fMoonZenithDistance"]=$first?"Off":"";

        if (empty($_GET["fMoonDisk"]))
            $_GET["fMoonDisk"]=$first?"Off":"";

        if (empty($_GET["fAngleToMoon"]))
            $_GET["fAngleToMoon"]=$first?"Off":"";

        if (empty($_GET["fSunZenithDistance"]))
            $_GET["fSunZenithDistance"]=$first?"Off":"";

        if (empty($_GET["fCameraTempMean"]))
            $_GET["fCameraTempMean"]=$first?"Off":"";

        if (empty($_GET["fCameraTempRms"]))
            $_GET["fCameraTempRms"]=$first?"Off":"";

        if (empty($_GET["fOutsideTempMean"]))
            $_GET["fOutsideTempMean"]=$first?"Off":"";

        if (empty($_GET["fOutsideTempRms"]))
            $_GET["fOutsideTempRms"]=$first?"Off":"";

    }

    function PrintForm($_GET, $host, $user, $pw, $db)
    {
        printf("<center>\n");
        printf("<form action=\"fact_runinfo.php\" METHOD=\"GET\">\n");

        // pull down boxes

        printf(" <table>\n");
        printf("  <tr><td>\n");
        PrintPullDown($host, $user, $pw, $db, "RunType",  "fRunTypeName",  "fRunTypeKEY", "RunType");
        printf(" </td></tr></table>\n");
        printf(" <p>\n");

        printf(" <table>\n");
        printf("  <tr>\n");

        CheckBox("fComments",                     "comments");
        CheckBox("fRunStart",                     "start time");
        CheckBox("fRunStop",                      "stop time");
        CheckBox("TimeDiff(fRunStop,fRunStart)",  "duration");
        CheckBox("fNumEvents",                    "#Evts");
        CheckBox("fROI",                          "roi");

        printf("  </tr>\n");
        printf("  <tr>\n");

        CheckBox("Round(fNumPhysicsTrigger/Time_to_sec(TimeDiff(fRunStop,fRunStart)),1)",    "phys rate");
        CheckBox("fNumPhysicsTrigger",    "#Phys");
        CheckBox("fNumPedestalTrigger",   "#Ped");
        CheckBox("fNumELPTrigger",        "#ELP");
        CheckBox("fNumTimeTrigger",       "#Time");

        printf("  </tr>\n");
        printf("  <tr>\n");

        CheckBox("fNumExt1Trigger",       "#Ext1");
        CheckBox("fNumExt2Trigger",       "#Ext2");
        CheckBox("fNumILPTrigger",        "#ILP");
        CheckBox("fNumOtherTrigger",      "#Other");

        printf("  </tr>\n");
        printf("  <tr>\n");

        CheckBox("fTriggerRateMedian",    "rate");
        CheckBox("fThresholdMedian",      "threshold");
        CheckBox("fBiasVoltageMedian",    "voltage");
        CheckBox("if(fHasDrsFile=0,\"N\",fDrsStep)", "drs");
        CheckBox("fROITimeMarker",                   "roi-tm");

        printf("  </tr>\n");
        printf("  <tr>\n");

        CheckBox("fZenithDistanceMean",   "ZdMean");
        CheckBox("fAzimuthMean",          "AzMean");
        CheckBox("fRightAscension",       "RA");
        CheckBox("fDeclination",          "Dec");

        printf("  </tr>\n");
        printf("  <tr>\n");

        CheckBox("fMoonZenithDistance",   "ZdMoon");
        CheckBox("fSunZenithDistance",    "ZdSun");
        CheckBox("fMoonDisk",             "MoonDisk");
        CheckBox("fAngleToMoon",          "Angle2Moon");

        printf("  </tr>\n");
        printf("  <tr>\n");

        CheckBox("fOutsideTempMean",      "temp mean");
        CheckBox("fOutsideTempRms",       "temp rms");
        CheckBox("fCameraTempMean",       "cam temp mean");
        CheckBox("fCameraTempRms",        "cam temp rms");

        printf("  </tr>\n");
        printf("  <tr>\n");

        CheckBox("fFitsFileErrors",                  "file err");
        CheckBox("fCompileTime",                     "compiled");
        CheckBox("fRevisionNumber",                  "rev#");
        CheckBox("if(isnull(fCheckSum)+isnull(fDataSum),Concat(fMd5sumRaw,\"/\",fMd5sumRawZip),Concat(fCheckSum,\"/\",fDataSum))",  "checksum");

        printf("  </tr>\n");
        printf(" </table>\n");
        printf(" <p>\n");

        if (empty($_GET["fStartDate"]))
            $timemin = GetMin("fNight", "RunInfo", $host, $user, $pw, $db);
        else
            $timemin = $_GET["fStartDate"];

        if (empty($_GET["fStopDate"]))
            $timemax = GetMax("fNight", "RunInfo", $host, $user, $pw, $db);
        else
            $timemax = $_GET["fStopDate"];

        printf("Run&nbsp;(yyyymmdd)&nbsp;from&nbsp;<input name=\"fStartDate\" type=\"text\" size=\"10\" maxlength=\"10\" value=\"%s\">\n", $timemin);
        printf("to&nbsp;<input name=\"fStopDate\" type=\"text\" size=\"10\" maxlength=\"10\" value=\"%s\">&nbsp;&nbsp;&nbsp;&nbsp;\n", $timemax);

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

        ini_set("mysql.trace_mode", "Off");
        ini_set("display_errors", "Off");

        printf("<input class='Width' type='submit' value='Query Table'>&nbsp;&nbsp;&nbsp;\n");
        printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"fact_runinfo.php\"'>&nbsp;&nbsp;&nbsp;\n");
        if (strchr($_SERVER["REQUEST_URI"], '?')!=FALSE)
            printf("<input class='Width' type='button' value='Get .txt' onClick='self.location.href=\"%s&fSendTxt=1\"'>&nbsp;&nbsp;&nbsp;\n", $_SERVER["REQUEST_URI"]);
        printf("</form>\n");
        printf("</center>\n");
        printf("</td>\n");
        printf("</tr>\n");
        printf("<tr class='Block'>\n");
        printf("<td>\n");
    }

    function PrintPage($html, $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup)
    {
        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);
        mysql_query("SET BIG_TABLES=1"); // necessary for mySQL <= 4


        $query0 = CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup);

        $result0 = mysql_query($query0, $db_id);
        $result1 = mysql_query("SELECT FOUND_ROWS()", $db_id);

        if ($result0)
        {
            if ($html=="1")
                PrintMagicTable($result0, $alias, $rightalign, "", "", "", $result1);
            else
                PrintText($result0);

            mysql_free_result($result0);
            mysql_free_result($result1);
        }
        mysql_close($db_id);

        if ($html=="1")
            PrintSubmittedQuery($query0, $db, "old");
    }

    include ("include.php");
    include ("db.php");
    include ("factdefs.php");

    ini_set("display_errors",   "On");
    ini_set("mysql.trace_mode", "On");

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-type: application/octet");
        header("Content-Disposition: attachment; filename=query-result.txt");

        PrintPage("0", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup);
    }
    else
    {
        echo (file_get_contents("index-header.html"));

        $environment = sizeof($_GET);

        InitGet($_GET);
        PrintForm($_GET, $host, $user, $pw, $db);

        if ($environment==0)
            printf("No query submitted yet.<BR>");
        else
            PrintPage("1", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup);

        echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors",   "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
