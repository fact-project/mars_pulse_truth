<?php
{
    function CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup, $checkenumgroup, $needs)
    {
        $fromtable="RunData";

        $groups = 0;
        foreach ($checkgroup as $element)
            if ($element==-1)
                $groups++;

        $statusgroups = 0;
        foreach ($checkstatusgroup as $element)
            if ($element==-1)
                $statusgroups++;

        $enumgroups = 0;
        foreach ($checkenumgroup as $element)
            if ($element==-1)
                $enumgroups++;

        $query0 = "SELECT SQL_CALC_FOUND_ROWS ";

        if ($groups>0)
        {
            foreach ($checkgroup as $key => $element)
                if ($element==-1)
                    $query0 .=  $key . " as '" . $alias[$key] . "' " . ", ";
            $query0 .= " COUNT(*) as 'Runs' ";
        }
        else
        {
            if ($statusgroups>0)
            {
                foreach ($checkstatusgroup as $key => $element)
                    if ($element==-1)
                        $query0 .= " (if(IsNull(" . $key . "), if(isnull(fStartTime), 'not done', if(isnull(fFailedTime),if(isnull(" . $needs[$key] . "),'not done',if(date_sub(Now(),interval 12 hour) < fStartTime,'running','crashed')),if(isnull(" . $needs[$key] . "),'not done','failed'))) ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "', ";
//                        $query0 .= " (if(IsNull(" . $key . "), 'not done' ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "', ";

                $query0 .= " count(*) as 'Runs'";

            }
            else
            {
                if ($enumgroups>0)
                {
                    foreach ($checkenumgroup as $key => $element)
                        if ($element==-1)
                            $query0 .=  $key . " as '" . $alias[$key] . "' " . ", ";
                    $query0 .= " COUNT(*) as 'Runs' ";
                }
                else
                {
                    $query0 .= " RunData.fRunNumber as 'RunNumber'";
                    if (!empty($_GET["fLinks"]))
                    {
                        $query0 .= ", CONCAT('<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/sinope/' , DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%Y') , '/', DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%m') , '/', DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%d') , '/sinope-dat' ,  LPAD(CONCAT(RunData.fRunNumber , '.html'), 13,'0') , '\">dl</A>' ";
                        $query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/sinope/' , DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%Y') , '/', DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%m') , '/', DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%d') , '/sinope-cal' ,  LPAD(CONCAT(RunData.fRunNumber , '.html'), 13,'0') , '\">cl</A>' ";
                        $query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/sinope/' , DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%Y') , '/', DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%m') , '/', DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%d') , '/sinope-dat' ,  LPAD(CONCAT(RunData.fRunNumber , '.txt'), 12,'0') , '\">dt</A>' ";
                        $query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/sinope/' , DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%Y') , '/', DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%m') , '/', DATE_FORMAT(ADDDATE(RunData.fRunStart, INTERVAL 13 HOUR), '%d') , '/sinope-cal' ,  LPAD(CONCAT(RunData.fRunNumber , '.txt'), 12,'0') , '\">ct</A>' ";
                        $query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/callisto/' , LEFT(LPAD(CONCAT(fSequenceFirst, '.'), 9,'0'),4), '/', LPAD(CONCAT(fSequenceFirst, '/'), 9,'0'), 'callisto', LPAD(CONCAT(fSequenceFirst , '.html'), 13,'0') , '\">c</A>'";
                        $query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/star/' , LEFT(LPAD(CONCAT(fSequenceFirst, '.'), 9,'0'),4), '/', LPAD(CONCAT(fSequenceFirst, '/'), 9,'0'), 'star', LPAD(CONCAT(fSequenceFirst , '.html'), 13,'0') , '\">s</A>'";
                        $query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/sequences/' , LEFT(LPAD(CONCAT(fSequenceFirst, '.'), 9,'0'),4), '/sequence', LPAD(CONCAT(fSequenceFirst , '.txt'), 12,'0') , '\">', 'f </A>') ";
                        $query0 .= " as 'Links'";
                    }

                    foreach ($_GET as $key => $element)
                        if ($_GET[$key]=="On")
                            if ($key!="fLinks")
                                if (empty($checkwhere[$key]) || $checkwhere[$key]==0)
                                    $query0 .= ", " . $key . " as '" . $alias[$key] . "' ";
                }
            }
        }

        $query0 .= " FROM RunData ";

        $query0 .= " LEFT JOIN RunProcessStatus USING(fRunNumber,fTelescopeNumber,fFileNumber) ";
        $query0 .= " LEFT JOIN DataCheck USING(fRunNumber) ";
        $query0 .= " LEFT JOIN RunType USING(fRunTypeKEY) ";

        foreach ($checkwhere as $key => $element)
        {
            if (empty($element) || $element<=0)
                continue;

            if (strpos($query0, " WHERE ")==FALSE)
                $query0 .= " WHERE ";
            else
                if ($element!=-1)
                    if (strrpos($query0, " AND ")!=strlen($query0)-5)
                        $query0 .= " AND ";

            if ($element!=-1)
                $query0 .= GetCheck($fromtable, $key) . "=" . $element;
        }

        if (strpos($query0, " WHERE ")==FALSE)
            $query0 .= " WHERE ";
        else
            $query0 .= " AND ";

        $query0 .= StatusQuery("fRawFileAvail", $needs, $timelimits);
        $query0 .= StatusQuery("fDataCheckDone", $needs, $timelimits);

        $query0 .= EnumQuery("fHasSignal");
        $query0 .= EnumQuery("fHasPedestal");
        $query0 .= EnumQuery("fHasSignalInterlaced");
        $query0 .= EnumQuery("fHasPedestalInterlaced");

        if (!empty($_GET["fRunMin"]) && !empty($_GET["fRunMax"]))
            $query0 .= "RunData.fRunNumber BETWEEN " . $_GET["fRunMin"] . " AND " . $_GET["fRunMax"] . " ";

        if (!empty($_GET["fSourceN"]))
            $query0 .= " AND fSourceName REGEXP \"^" . $_GET["fSourceN"] . "\" ";

        if ($groups>0)
        {
            $query0 .= " GROUP BY ";
            $num = $groups;
            foreach ($checkgroup as $key => $element)
                if ($element==-1)
                {
                    $query0 .= GetCheck($fromtable,$key);
                    if ($num-->1)
                        $query0 .= ", ";
                }
        }

        if ($statusgroups>0)
        {
            $query0 .= " GROUP BY ";
            $num = $statusgroups;
            foreach ($checkstatusgroup as $key => $element)
                if ($element==-1)
                {
                    $query0 .= $alias[$key];
                    if ($num-->1)
                        $query0 .= ", ";
                }
        }

        if ($enumgroups>0)
        {
            $query0 .= " GROUP BY ";
            $num = $enumgroups;
            foreach ($checkenumgroup as $key => $element)
                if ($element==-1)
                {
                    $query0 .= $alias[$key];
                    if ($num-->1)
                        $query0 .= ", ";
                }
        }

        if (!empty($_GET["fSortBy"]))
        {
            $val=substr($_GET["fSortBy"], 0, -1);
            $query0 .= " ORDER BY " . GetTable($fromtable, $val) . " ";
            if (substr($_GET["fSortBy"], -1)=="-")
                $query0 .= "DESC";
        }

        if (empty($_GET["fSortBy"]) && $groups==0 && $statusgroups==0)
            $query0 .= " ORDER BY RunData.fRunNumber ASC ";

        if (empty($_GET["fNumStart"]))
            $_GET["fNumStart"]=0;

        if (empty($_GET["fSendTxt"]))
            $query0 .= " LIMIT " . $_GET["fNumStart"] . ", " . $_GET["fNumResults"];

        return $query0;
    }

    function InitGet($_GET)
    {
        // Find out whether it is the first call to the php script
        $first = empty($_GET["fRunMin"]) && empty($_GET["fRunMax"]);

        if (empty($_GET["fNumResults"]))
            $_GET["fNumResults"]="20";

        if (empty($_GET["fLinks"]))
            $_GET["fLinks"]=$first?"On":"";

        if (empty($_GET["fSequenceFirst"]))
            $_GET["fSequenceFirst"]=$first?"On":"";

        if (empty($_GET["fEvents"]))
            $_GET["fEvents"]=$first?"On":"";

        if (empty($_GET["fPositionSignal"]))
            $_GET["fPositionSignal"]="Off";

        if (empty($_GET["fPositionFWHM"]))
            $_GET["fPositionFWHM"]="Off";

        if (empty($_GET["fHeightSignal"]))
            $_GET["fHeightSignal"]="Off";

        if (empty($_GET["fHeightFWHM"]))
            $_GET["fHeightFWHM"]="Off";

        if (empty($_GET["fHasSignal"]))
            $_GET["fHasSignal"]=$first?"On":"";

        if (empty($_GET["fHasSignalEnum"]))
            $_GET["fHasSignalEnum"]="0";

        if (empty($_GET["fHasPedestal"]))
            $_GET["fHasPedestal"]=$first?"On":"";

        if (empty($_GET["fHasPedestalEnum"]))
            $_GET["fHasPedestalEnum"]="0";

        if (empty($_GET["fPositionAsym"]))
            $_GET["fPositionAsym"]="Off";

        if (empty($_GET["fHeightAsym"]))
            $_GET["fHeightAsym"]="Off";

        if (empty($_GET["fEventsInterlaced"]))
            $_GET["fEventsInterlaced"]=$first?"On":"";

        if (empty($_GET["fPositionSignalInterlaced"]))
            $_GET["fPositionSignalInterlaced"]="Off";

        if (empty($_GET["fPositionFWHMInterlaced"]))
            $_GET["fPositionFWHMInterlaced"]="OFf";

        if (empty($_GET["fHeightSignalInterlaced"]))
            $_GET["fHeightSignalInterlaced"]="Off";

        if (empty($_GET["fHeightFWHMInterlaced"]))
            $_GET["fHeightFWHMInterlaced"]="Off";

        if (empty($_GET["fHasSignalInterlaced"]))
            $_GET["fHasSignalInterlaced"]=$first?"On":"";

        if (empty($_GET["fHasSignalInterlacedEnum"]))
            $_GET["fHasSignalInterlacedEnum"]="0";

        if (empty($_GET["fHasPedestalInterlaced"]))
            $_GET["fHasPedestalInterlaced"]=$first?"On":"";

        if (empty($_GET["fHasPedestalInterlacedEnum"]))
            $_GET["fHasPedestalInterlacedEnum"]="0";

        if (empty($_GET["fPositionAsymInterlaced"]))
            $_GET["fPositionAsymInterlaced"]="Off";

        if (empty($_GET["fHeightAsymInterlaced"]))
            $_GET["fHeightAsymInterlaced"]="Off";

        if (empty($_GET["fRunTypeName"]))
            $_GET["fRunTypeName"]=$first?"On":"";

        if (empty($_GET["fDataCheckDone"]))
            $_GET["fDataCheckDone"]="Off";

        if (empty($_GET["fDataCheckDoneStatus"]))
            $_GET["fDataCheckDoneStatus"]=$first?"1":"";

        if (empty($_GET["fRawFileAvail"]))
            $_GET["fRawFileAvail"]="Off";

        if (empty($_GET["fRawFileAvailStatus"]))
            $_GET["fRawFileAvailStatus"]=$first?"1":"";

        if (empty($_GET["fStartTime"]))
            $_GET["fStartTime"]="Off";

        if (empty($_GET["fFailedTime"]))
            $_GET["fFailedTime"]="Off";

        if (empty($_GET["fReturnCode"]))
            $_GET["fReturnCode"]="Off";

        if (empty($_GET["fProgramId"]))
            $_GET["fProgramId"]="Off";

    }

    function PrintForm($_GET, $host, $user, $pw, $db)
    {
        printf("<center>\n");
        printf("<form action=\"datacheck.php\" METHOD=\"GET\">\n");
        printf(" <table>\n");
        printf("  <tr>\n");

        CheckBox("fLinks",             "Links");
        CheckBox("fEvents",            "DataEvents");
        CheckBox("fEventsInterlaced",  "CalEvents");
        CheckBox("fSequenceFirst",     "Sequence#");

        printf("  </tr><tr>\n");

        CheckBox("fPositionSignal",            "ArrTime");
        CheckBox("fPositionFWHM",              "ArrTimeFWHM");
        CheckBox("fPositionSignalInterlaced",  "ArrTimeCal");
        CheckBox("fPositionFWHMInterlaced",    "ArrTimeFWHMCal");

        printf("  </tr><tr>\n");

        CheckBox("fHeightSignal",            "Signal");
        CheckBox("fPositionFWHM",            "SignalFWHM");
        CheckBox("fHeightSignalInterlaced",  "SignalCal");
        CheckBox("fPositionFWHMInterlaced",  "SignalFWHMCal");

        printf("  </tr><tr>\n");

        CheckBox("fPositionAsym",            "ArrTimeAsym");
        CheckBox("fHeightAsym",              "SignalAsym");
        CheckBox("fPositionAsymInterlaced",  "ArrTimeAsymCal");
        CheckBox("fHeightAsymInterlaced",    "SignalAsymCal");

        printf("  </tr><tr><td>\n");

        PrintEnumMenu("fHasSignal",               "HasSignal");
        printf("  </td><td>\n");
        PrintEnumMenu("fHasPedestal",             "HasPedestal");
        printf("  </td><td>\n");
        PrintEnumMenu("fHasSignalInterlaced",     "HasSignalCal");
        printf("  </td><td>\n");
        PrintEnumMenu("fHasPedestalInterlaced",   "HasPedestalCal");

        printf("  </td></tr><tr><td>\n");

        PrintStatusMenu("fDataCheckDone", "DataCheck");
        printf("  </td><td>\n");
        PrintStatusMenu("fRawFileAvail", "RawFile");
        printf("</td><td>\n");

        PrintPullDown($host, $user, $pw, $db, "RunType",      "fRunTypeName",      "fRunTypeKEY", "Run Type");

        printf("  </td></tr>\n");
        printf(" </table>\n");

        printf(" <table>\n");
        printf("  <tr>\n");

        CheckBox("fStartTime",     "StartTime");
        CheckBox("fFailedTime",    "FailedTime");
        CheckBox("fReturnCode",    "RetCode");
        CheckBox("fProgramId",     "ProgramId");

        printf("  </tr>\n");
        printf(" </table>\n");
        printf(" <p>\n");

        if (empty($_GET["fRunMin"]))
            $min = GetMin("fRunNumber", "RunData", $host, $user, $pw, $db);
        else
            $min = $_GET["fRunMin"];

        if (empty($_GET["fRunMax"]))
            $max = GetMax("fRunNumber", "RunData", $host, $user, $pw, $db);
        else
            $max = $_GET["fRunMax"];

/*
        printf("Date&nbsp;(yyyy-mm-dd)&nbsp;<input name=\"fDate\" type=\"text\" size=\"10\" maxlength=\"10\" value=\"");
        if (!empty($_GET["fDate"]))
            printf("%s", $_GET["fDate"]);
        printf("\">&nbsp;&nbsp;&nbsp;\n");
*/

        printf("DataCheck&nbsp;from&nbsp;<input name=\"fRunMin\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\">\n", $min);
        printf("to&nbsp;<input name=\"fRunMax\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\">&nbsp;&nbsp;&nbsp;\n", $max);

        printf(" <P>\n");

        printf(" Results:\n");
        printf(" <select name=\"fNumResults\">\n");

        $numres = array("10", "20", "50", "100", "200", "500");
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
        printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"datacheck.php\"'>&nbsp;&nbsp;&nbsp;\n");
        if (strchr($_SERVER["REQUEST_URI"], '?')!=FALSE)
            printf("<input class='Width' type='button' value='Get .txt' onClick='self.location.href=\"%s&fSendTxt=1\"'>&nbsp;&nbsp;&nbsp;\n", $_SERVER["REQUEST_URI"]);
        if (strchr($_SERVER["REQUEST_URI"], '?')!=FALSE)
            printf("<input class='Width' type='button' value='Print' onClick='self.location.href=\"%s&fPrintTable=1\"'>&nbsp;&nbsp;&nbsp;\n", $_SERVER["REQUEST_URI"]);
        printf("</form>\n");
        printf("</center>\n");
        printf("</td>\n");
        printf("</tr>\n");
        printf("<tr class='Block'>\n");
        printf("<td>\n");
    }

    function PrintPage($html, $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $checkenumgroup, $needs)
    {
        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);

        $query0 = CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup, $checkenumgroup, $needs);

        $result0 = mysql_query($query0, $db_id);
        $result1 = mysql_query("SELECT FOUND_ROWS()", $db_id);

        if ($result0)
        {
            if ($html=="1" || $html=="2")
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
    include ("magicdefs.php");

    ini_set("display_errors", "On");
    ini_set("mysql.trace_mode", "On");

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-type: application/octet");
        header("Content-Disposition: attachment; filename=query-result.txt");

        PrintPage("0", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $checkenumgroup, $needs);
    }
    else
    {
        echo (file_get_contents("index-header.html"));

        $environment = sizeof($_GET);

        InitGet($_GET);
        if (empty($_GET["fPrintTable"]))
            PrintForm($_GET, $host, $user, $pw, $db);

        if ($environment==0)
            printf("No query submitted yet.<BR>");
        else
        {
            if (empty($_GET["fPrintTable"]))
                PrintPage("1", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $checkenumgroup, $needs);
            else
                PrintPage("2", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $checkenumgroup, $needs);
        }

        echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
