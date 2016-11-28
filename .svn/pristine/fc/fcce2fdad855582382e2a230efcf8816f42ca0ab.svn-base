<?php
{
    function CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup, $needs)
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

        $query0 = "SELECT SQL_CALC_FOUND_ROWS ";

        if ($groups>0 || ((!empty($_GET["fGroupByDate"])) && !$_GET["fGroupByDate"]==0) || $statusgroups>0)
        {
            foreach ($checkgroup as $key => $element)
                if ($element==-1)
                    $query0 .=  $key . " as '" . $alias[$key] . "' " . ", ";

            if ((!empty($_GET["fGroupByDate"])) && !$_GET["fGroupByDate"]==0)
            {
                $query0 .= " DATE_FORMAT(ADDDATE(fRunStart,Interval 12 hour), '";
                switch($_GET["fGroupByDate"])
                {
                case "Year":
                    $query0 .= "%Y ";
                    break;
                case "Month":
                    $query0 .= "%Y-%m ";
                    break;
                case "Night":
                    $query0 .= "%Y-%m-%d ";
                    break;
                }
                $query0 .= "')  AS GroupByDate, ";
            }

            //--------------------------------------------------
            //$query0 .= " TIMEDIFF(fRunStop, fRunStart), ";
            // Only available in MySQL>4.1.1
            $query0 .= "SUM(if(TIME_TO_SEC(fRunStop)-TIME_TO_SEC(fRunStart)<0, " .
                "TIME_TO_SEC(fRunStop)-TIME_TO_SEC(fRunStart)+24*60*60, " .
                "TIME_TO_SEC(fRunStop)-TIME_TO_SEC(fRunStart)))/3600 as '" . $alias["SUM(fRunTime)/3600"] . "', ";
            //--------------------------------------------------
            $query0 .= " SUM(fNumEvents) as '" . $alias["SUM(fNumEvents)"] . "', ";
            $query0 .= " Min(fZenithDistance) as '" . $alias["Min(fZenithDistance)"] . "', ";
            $query0 .= " Max(fZenithDistance) as '" . $alias["Max(fZenithDistance)"] . "', ";
            $query0 .= " COUNT(*) as '# Runs' ";

            if ($statusgroups>0)
            {
                foreach ($checkstatusgroup as $key => $element)
                    if ($element==-1)
                        $query0 .= ", (if(IsNull(" . $key . "), if(isnull(fStartTime), 'not done', if(isnull(fFailedTime),if(isnull(" . $needs[$key] . "),'not done',if(date_sub(Now(),interval 12 hour) < fStartTime,'running','crashed')),if(isnull(" . $needs[$key] . "),'not done','failed'))) ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "' ";
//                        $query0 .= " (if(IsNull(" . $key . "), 'not done' ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "', ";

                $query0 .= ", COUNT(*) AS '# Runs'";

            }
        }
        else
        {
            //$query0 .= " RunData.fRunNumber as 'Run#' ";
            //$query0 .= " Concat(RunData.fTelescopeNumber,':', RunData.fRunNumber,':', RunData.fFileNumber) as 'Run' ";
            $query0 .= " Concat('M', RunData.fTelescopeNumber,':', LPAD(RunData.fRunNumber,8, ' '),'/', LPAD(RunData.fFileNumber, 3, ' ')) as 'Tel:Run/File' ";

            if (empty($_GET["fSendTxt"]) && !empty($_GET["fLinks"]))
            {
                $query0 .= ", CONCAT('<A&ws;HREF=\"queryrbk.php?fNight=', date_format(adddate(fRunStart, interval +13 HOUR), '%Y-%m-%d') , '&amp;fDate=2\">rbk</A>') ";
                $query0 .= " as 'Links'";
            }

            foreach ($_GET as $key => $element)
                if ($key!="fLinks" && $_GET[$key]=="On" && $key!="fTest")
                    if (empty($checkwhere[$key]) || $checkwhere[$key]==0)
                        $query0 .= ", " . $key . " as '" . $alias[$key] . "' ";
        }

        $query0 .= " FROM RunData ";

        $query0 .= " LEFT JOIN RunProcessStatus USING(fRunNumber,fTelescopeNumber,fFileNumber) ";

        foreach ($_GET as $key => $element)
            if (($_GET[$key]=="On" || $groups>0))// && !empty($joins[$key]))
                $query0 .= GetJoin($fromtable,$key);

        if ($_GET["fTest"]!="On")
        {
            if ($_GET["fSourceName"]!="On")
                $query0 .= " LEFT JOIN Source USING(fSourceKEY) ";
            $query0 .= " WHERE fTest='no'";
        }

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

        $query0 .= StatusQuery("fCCFileAvail", $needs, $timelimits);
        $query0 .= StatusQuery("fCaCoFileAvail", $needs, $timelimits);
        $query0 .= StatusQuery("fCaCoFileFound", $needs, $timelimits);
        $query0 .= StatusQuery("fRawFileAvail", $needs, $timelimits);
        $query0 .= StatusQuery("fDataCheckDone", $needs, $timelimits);
        $query0 .= StatusQuery("fTimingCorrection", $needs, $timelimits);

        if ((!empty($_GET["fRunMin"]) || $_GET["fRunMin"]=="0") && !empty($_GET["fRunMax"]))
            $query0 .= "RunData.fRunNumber BETWEEN " . $_GET["fRunMin"] . " AND " . $_GET["fRunMax"] . " ";

        if ((!empty($_GET["fZDMin"]) || $_GET["fZDMin"]==0) && !empty($_GET["fZDMax"]))
            $query0 .= "AND fZenithDistance BETWEEN " . $_GET["fZDMin"] . " AND  " . $_GET["fZDMax"] . " ";

/*
        if (!empty($_GET["fDate"]))
            $query0 .= " AND fRunStart REGEXP \"^" . $_GET["fDate"] . "\" ";
*/

        if (!empty($_GET["fSourceN"]))
            $query0 .= " AND fSourceName REGEXP \"^" . $_GET["fSourceN"] . "\" ";

        if (!empty($_GET["fStartDate"]))
        {
            if (strpos(strrev($query0), " DNA ")!=0)
                $query0 .= " AND ";

            $startdate=substr($_GET["fStartDate"], 0, 10);
            if ($startdate=="0000-00-00")
                $query0 .=" fRunStart >= '" . $startdate . " 00:00:00' ";
            else
                $query0 .= " fRunStart >= ADDDATE('" . $startdate . " 13:00:00', INTERVAL -1 DAY) ";
        }

        if (!empty($_GET["fStopDate"]))
        {
            if (strpos(strrev($query0), " DNA ")!=0)
                $query0 .= " AND ";

            $stopdate=substr($_GET["fStopDate"], 0, 10);
            $query0 .= " fRunStart < '" . $stopdate . " 13:00:00' ";
        }

        if (!empty($_GET["fSequenceNo"]) || $_GET["fSequenceNo"]=="0")
        {
            if (strpos(strrev($query0), " DNA ")!=0)
                $query0 .= " AND ";

            $query0 .= " fSequenceFirst = '" . $_GET["fSequenceNo"] . "' ";
        }


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

        if ((!empty($_GET["fGroupByDate"])) && !$_GET["fGroupByDate"]==0)
            if (strpos($query0, " GROUP BY ")==FALSE)
                $query0 .= " GROUP BY GroupByDate ";
            else
                $query0 .= ", GroupByDate ";

        if ($statusgroups>0)
        {
            if (strpos($query0, " GROUP BY ")==FALSE)
                $query0 .= " GROUP BY  ";
            else
                $query0 .= ",  ";
            $num = $statusgroups;
            foreach ($checkstatusgroup as $key => $element)
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
        else
            $query0 .=" ORDER BY RunData.fRunNumber ASC";

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

        if (empty($_GET["fTest"]))
            $_GET["fTest"]=$first?"On":"";

        if (empty($_GET["fNumEvents"]))
            $_GET["fNumEvents"]=$first?"On":"";

        if (empty($_GET["fRunStart"]))
            $_GET["fRunStart"]=$first?"On":"";

        if (empty($_GET["fZenithDistance"]))
            $_GET["fZenithDistance"]=$first?"On":"";

        if (empty($_GET["fRunTypeName"]))
            $_GET["fRunTypeName"]=$first?"On":"";

        if (empty($_GET["fSourceName"]))
            $_GET["fSourceName"]=$first?"On":"";

        if (empty($_GET["fMeanTriggerRate"]))
            $_GET["fMeanTriggerRate"]=$first?"On":"";

        if (empty($_GET["fExcludedFDAName"]))
            $_GET["fExcludedFDAName"]=$first?"On":"";

        if (empty($_GET["fSequenceNo"]))
            $_GET["fSequenceNo"]="";

        InitRunInfo($_GET, $first);
    }

    function PrintForm($_GET, $host, $user, $pw, $db)
    {
        printf("<center>\n");
        printf("<form action=\"runinfo-aio.php\" METHOD=\"GET\">\n");

        printf("<input id='sh' type='hidden' name='fShowHide' value='");
        if (!empty($_GET["fShowHide"]))
            printf("%s", $_GET["fShowHide"]);
        else
            printf("1000000010");
        printf("'>\n");
        printf("<img id='allbutton' src='minus.png' alt='-' onclick=\"showhide('all')\"> <b>Menu</b>&nbsp;&nbsp;&nbsp;&nbsp;\n");
        printf("&nbsp;&nbsp;<img id='infobutton'   src='plus.png' alt='+' onClick=\"showhide('info');showhide('info2')\"> RunInfo    \n");
        printf("&nbsp;&nbsp;<img id='statbutton'   src='plus.png' alt='+' onClick=\"showhide('stat');showhide('fail')\">  StatusInfo \n");
        printf("&nbsp;&nbsp;<img id='rangesbutton' src='plus.png' alt='+' onClick=\"showhide('ranges')\">                 Ranges     \n");

        printf(" <div id='all' style='display:block'>");
        PrintRunInfoMenu();
        PrintRunInfo2Menu($host,$user,$pw,$db);
        PrintRunStatMenu();
        PrintFailMenu();
        printf(" <div id='ranges' style='display:none'>");
        PrintSequMenu($host,$user,$pw,$db);
        PrintNightRangeMenu($host,$user,$pw,$db, "RunData");
        printf("<p>");
        PrintZdRangeMenu($host,$user,$pw,$db);
        PrintRunRangeMenu($host,$user,$pw,$db);
        printf(" <P>\n");
        PrintSourceMenu($host,$user,$pw,$db);
        PrintGroupByDateMenu();
        printf("<p>");
        printf("</div>");
        printf("</div>");
        printf(" <P>\n");

        ini_set("mysql.trace_mode", "Off");
        ini_set("display_errors", "Off");

        PrintNumResPullDown();
        PrintButtons("runinfo-aio.php");

        printf("</form>\n");
        printf("</center>\n");
        printf("</td>\n");
        printf("</tr>\n");
        printf("<tr class='Block'>\n");
        printf("<td>\n");
    }

    function PrintPage($html, $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $needs)
    {
        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);
        mysql_query("SET BIG_TABLES=1"); // necessary for mySQL <= 4

        $query0 = CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup, $needs);

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
            PrintSubmittedQuery($query0, $db, "");
    }

    include ("include.php");
    include ("db.php");
    include ("menu.php");
    include ("magicdefs.php");

    ini_set("display_errors",   "On");
    ini_set("mysql.trace_mode", "On");

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-type: application/octet");
        header("Content-Disposition: attachment; filename=query-result.txt");

        PrintPage("0", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $needs);
    }
    else
    {
        if (empty($_GET["fPrintTable"]))
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
                PrintPage("1", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $needs);
            else
                PrintPage("2", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $needs);
        }

        if (empty($_GET["fPrintTable"]))
            echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors",   "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
