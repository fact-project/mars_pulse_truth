<?php
{
    function CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup, $timelimits)
    {
        $fromtable="CeresInfo";

        $groups = 0;
        foreach ($checkgroup as $element)
            if ($element==-1)
                $groups++;

        $statusgroups = 0;
        foreach ($checkstatusgroup as $element)
            if ($element==-1)
                $statusgroups++;

        $query0 = "SELECT SQL_CALC_FOUND_ROWS ";

        if ($groups>0 || $statusgroups>0)
        {
            if ($groups>0)
            {
                foreach ($checkgroup as $key => $element)
                    if ($element==-1)
                        $query0 .=  $key . " as '" . $alias[$key] . "' " . ", ";
            }

            if ($statusgroups>0)
            {
                foreach ($checkstatusgroup as $key => $element)
                    if ($element==-1)
                        $query0 .= " IF( ISNULL(" . $key . "Status.fStartTime) "
                            ." AND ISNULL(" . $key . "Status.fStopTime) "
                            ." AND ISNULL(" . $key . "Status.fReturnCode), 'not done', "
                            //crashed
                            ." IF (NOT ISNULL(" . $key . "Status.fStartTime) "
                            ." AND DATE_SUB(Now(),INTERVAL " . $timelimits[$key] . " HOUR) > " . $key . "Status.fStartTime "
                            ." AND ISNULL(" . $key . "Status.fStopTime) "
                            ." AND ISNULL(" . $key . "Status.fReturnCode), 'crashed', "
                            //running
                            ." IF (NOT ISNULL(" . $key . "Status.fStartTime) "
                            ." AND DATE_SUB(Now(),INTERVAL " . $timelimits[$key] . " HOUR) < " . $key . "Status.fStartTime "
                            ." AND ISNULL(" . $key . "Status.fStopTime) "
                            ." AND ISNULL(" . $key . "Status.fReturnCode), 'running', "
                            //failed
                            ." IF (NOT ISNULL(" . $key . "Status.fStartTime) "
                            ." AND NOT ISNULL(" . $key . "Status.fStopTime) "
                            ." AND NOT ISNULL(" . $key . "Status.fReturnCode), 'failed', "
                            //done or done and avail
                            ." IF (NOT ISNULL(" . $key . "Status.fStartTime) "
                            ." AND NOT ISNULL(" . $key . "Status.fStopTime) "
                            ." AND ISNULL(" . $key . "Status.fReturnCode)"
                            ." AND ISNULL(" . $key . "Status.fAvailable), 'done', 'done and avail' "
                            ." ))))) AS '". $key . "', ";
            }
            $query0 .= " COUNT(*) as '# Files' ";
            $query0 .= ", SUM(fNumEvents) as '" . $alias["SUM(fNumEvents)"] . "' ";
            $query0 .= ", SUM(fNumEvents*fNumReUseShowers) as '" . $alias["SUM(fNumEvents*fNumReUseShowers)"] . "' ";
        }
        else
        {
            $query0 .= " CONCAT(LPAD(" . $fromtable . ".fRunNumber,8, ' '),'.', ";
            $query0 .= " RIGHT(LPAD(" . $fromtable . ".fFileNumber, 6, 0), 3)) as 'Run.File' ";

            if (empty($_GET["fSendTxt"]) && !empty($_GET["fLinks"]))
            {
                $query0 .= ", CONCAT('<A&ws;HREF=\"queryrbk.php?fNight=', date_format(adddate(fRunStart, interval +13 HOUR), '%Y-%m-%d') , '&amp;fDate=2\">rbk</A>') ";
                $query0 .= " as 'Links'";
            }

            foreach ($_GET as $key => $element)
                if ($_GET[$key]=="On" && $key!="fTest" && $key!="fLinks")
                    if (!ereg("^f", $key))
                    {

                        $query0 .= ", IF (ISNULL(" . $key . "Status.fStartTime) AND ";
                        $query0 .= " ISNULL(" . $key . "Status.fStopTime) AND ";
                        $query0 .= " ISNULL(" . $key . "Status.fReturnCode), 'not done', ";

                        $query0 .= " IF (NOT ISNULL(" . $key . "Status.fStartTime) ";
                        $query0 .= " AND ISNULL(" . $key . "Status.fStopTime) ";
                        $query0 .= " AND ISNULL(" . $key . "Status.fReturnCode) ";
                        $query0 .= " AND DATE_SUB(Now(),INTERVAL " . $timelimits[$key] . " HOUR) > " . $key . "Status.fStartTime, ";
                        $query0 .= " CONCAT('maybe crashed (started at ', " . $key . "Status.fStartTime, ')'), ";

                        $query0 .= " IF (NOT ISNULL(" . $key . "Status.fStartTime) ";
                        $query0 .= " AND ISNULL(" . $key . "Status.fStopTime) ";
                        $query0 .= " AND ISNULL(" . $key . "Status.fReturnCode) ";
                        $query0 .= " AND DATE_SUB(Now(),INTERVAL " . $timelimits[$key] . " HOUR) < " . $key . "Status.fStartTime, ";
                        $query0 .= " CONCAT('running (started at ', " . $key . "Status.fStartTime, ')'), ";

                        $query0 .= " IF (NOT ISNULL(" . $key . "Status.fReturnCode), ";
                        $query0 .= " CONCAT('failed (', " . $key . "Status.fReturnCode, ', ', " . $key . "Status.fStopTime, ')'), ";
                        $query0 .= " CONCAT('done (', " . $key . "Status.fStopTime, ";
                        $query0 .= " ', ', TIMEDIFF(" . $key . "Status.fStopTime, " . $key . "Status.fStartTime), ')'))))) ";

                        $query0 .= " AS '" . $alias[$key] . "' ";
                    }
                    else
                        if (empty($checkwhere[$key]) || $checkwhere[$key]==0)
                            $query0 .= ", " . $key . " as '" . $alias[$key] . "' ";
        }

        $query0 .= " FROM " . $fromtable;

        $query0 .= " LEFT JOIN CorsikaInfo USING(fRunNumber,fFileNumber) ";

        foreach ($_GET as $key => $element)
            if ($_GET[$key]=="On" || $_GET[$key]!=0)
                if (strpos($query0, GetJoin($fromtable,$key))==FALSE)
                    $query0 .= GetJoin($fromtable,$key);

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
            $query0 .= " WHERE";
        else
            $query0 .= " AND ";

        $query0 .= StatusQuery("Corsika",      $timelimits);
        $query0 .= StatusQuery("Ceres",        $timelimits);
        $query0 .= StatusQuery("SequenceFile", $timelimits);
        $query0 .= StatusQuery("Callisto",     $timelimits);
        $query0 .= StatusQuery("Star",         $timelimits);

        if ((!empty($_GET["fRunMin"]) || $_GET["fRunMin"]=="0") && !empty($_GET["fRunMax"]))
            $query0 .= " " . $fromtable . ".fRunNumber BETWEEN " . $_GET["fRunMin"] . " AND " . $_GET["fRunMax"] . " ";


        //remove WHERE or AND in the end of the query
        $query0=preg_replace("/ WHERE\s+$/", " ", $query0);
        $query0=preg_replace("/ AND\s+$/", " ", $query0);

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
            if (strpos($query0, " GROUP BY ")==FALSE)
                $query0 .= " GROUP BY ";
            else
                $query0 .= ", ";
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
            $query0 .=" ORDER BY " . $fromtable . ".fRunNumber ASC";

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

        InitMCRunInfo($_GET, $first);
    }

    function PrintForm($_GET, $host, $user, $pw, $db)
    {
        printf("<center>\n");
        printf("<form action=\"mcinfo.php\" METHOD=\"GET\">\n");

        printf("<input id='sh' type='hidden' name='fShowHide' value='");
        if (!empty($_GET["fShowHide"]))
            printf("%s", $_GET["fShowHide"]);
        else
            printf("1000000010");
        printf("'>\n");
        printf("<img id='allbutton' src='minus.png' alt='-' onclick=\"showhide('all')\"> <b>Menu</b>&nbsp;&nbsp;&nbsp;&nbsp;\n");
        printf("&nbsp;&nbsp;<img id='infobutton'   src='plus.png' alt='+' onClick=\"showhide('info');showhide('info2')\"> MCRunInfo  \n");
        printf("&nbsp;&nbsp;<img id='statbutton'   src='plus.png' alt='+' onClick=\"showhide('stat')\">  StatusInfo \n");
        printf("&nbsp;&nbsp;<img id='rangesbutton' src='plus.png' alt='+' onClick=\"showhide('ranges')\">                 Ranges     \n");

        printf(" <div id='all' style='display:block'>");
        PrintMCRunInfoMenu($host,$user,$pw,$db);
        PrintMCRunStatMenu();
        printf(" <div id='ranges' style='display:none'>");
        printf("<p>");
        PrintMCRunRangeMenu($host,$user,$pw,$db);
        printf("<p>");
        printf("</div>");
        printf("</div>");
        printf(" <P>\n");

        ini_set("mysql.trace_mode", "Off");
        ini_set("display_errors", "Off");

        PrintNumResPullDown();
        PrintButtons("mcinfo.php");

        printf("</form>\n");
        printf("</center>\n");
        printf("</td>\n");
        printf("</tr>\n");
        printf("<tr class='Block'>\n");
        printf("<td>\n");
    }

    function PrintPage($html, $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $timelimits)
    {
        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);
        mysql_query("SET BIG_TABLES=1"); // necessary for mySQL <= 4

        $query0 = CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup, $timelimits);

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
    include ("db2.php");
    include ("menu.php");
    include ("mcdefs.php");

    ini_set("display_errors",   "On");
    ini_set("mysql.trace_mode", "On");

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-type: application/octet");
        header("Content-Disposition: attachment; filename=query-result.txt");

        PrintPage("0", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $timelimits);
    }
    else
    {
        if (empty($_GET["fPrintTable"]))
            echo (file_get_contents("index-header-fact.html"));

        $environment = sizeof($_GET);

        InitGet($_GET);
        if (empty($_GET["fPrintTable"]))
            PrintForm($_GET, $host, $user, $pw, $db);

        if ($environment==0)
            printf("No query submitted yet.<BR>");
        else
        {
            if (empty($_GET["fPrintTable"]))
                PrintPage("1", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $timelimits);
            else
                PrintPage("2", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $timelimits);
        }

        if (empty($_GET["fPrintTable"]))
            echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors",   "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
