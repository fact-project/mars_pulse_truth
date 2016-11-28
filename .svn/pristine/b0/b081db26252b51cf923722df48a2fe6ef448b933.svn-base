<?php
{
    function CreateQuery($alias, $checkwhere, $checkgroup, $checkstatusgroup, $needs, $timelimits)
    {
        $fromtable="MCRunData";

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
                /*
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
                 */
            }
            if ($statusgroups>0)
            {
                foreach ($checkstatusgroup as $key => $element)
                    if ($element==-1)
                        $query0 .= " (if(IsNull(" . $key . "), if(isnull(fStartTime), 'not done', if(isnull(fFailedTime),if(isnull(" . $needs[$key] . "),'not done',if(date_sub(Now(),interval " . $timelimits[$key] . " hour) < fStartTime,'running','crashed')),if(isnull(" . $needs[$key] . "),'not done','failed'))) ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "', ";
                //                        $query0 .= " (if(IsNull(" . $key . "), 'not done' ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "', ";

//                $query0 .= " count(*) as '# Runs'";

            }
            $query0 .= " COUNT(*) as '# Runs' ";
            $query0 .= ", SUM(fNumEvents) as '" . $alias["SUM(fNumEvents)"] . "' ";
            $query0 .= ", SUM(fNumEvents) DIV 1000 as '" . $alias["SUM(fNumEvents) DIV 1000"] . "' ";
        }
        else
        {
            $query0 .= " MCRunData.fMCRunNumber as 'Run' ";
//                $query0 .= " Concat(RunData.fTelescopeNumber,':', RunData.fRunNumber,':', RunData.fFileNumber) as 'Run' ";
//                $query0 .= " Concat('M', RunData.fTelescopeNumber,':', LPAD(RunData.fRunNumber,8, ' '),'/', LPAD(RunData.fFileNumber, 3, ' ')) as 'Tel:Run/File' ";

            if (empty($_GET["fSendTxt"]) && !empty($_GET["fLinks"]))
            {
                $query0 .= ", CONCAT('<A&ws;HREF=\"http://cluster.isdc.unige.ch/cta/compute-0-', fProductionHostKEY , '/simtel/cta-ultra3/0.0deg/Data/', LCASE(fParticleTypeName), '_20deg_90deg_run', LPAD(MCRunData.fMCRunNumber, 6,'0'), '___cta-ultra3_desert.simtel.gz\">simtel.gz</A> <A&ws;HREF=\"http://cluster.isdc.unige.ch/cta/compute-0-', fProductionHostKEY , '/chimp/cta-ultra3/0.0deg/Data/', LCASE(fParticleTypeName), '_20deg_90deg_run', LPAD(MCRunData.fMCRunNumber, 6,'0'), '___cta-ultra3_desert.root\">root</A>') ";
                $query0 .= " as 'Download'";
            }

            foreach ($_GET as $key => $element)
                if ($key!="fLinks" && $_GET[$key]=="On" && $key!="fTest")
                    if (empty($checkwhere[$key]) || $checkwhere[$key]==0)
                        $query0 .= ", " . $key . " as '" . $alias[$key] . "' ";
        }

        $query0 .= " FROM MCRunData ";

        $query0 .= " LEFT JOIN MCRunProcessStatus USING(fMCRunNumber) ";

        foreach ($_GET as $key => $element)
            if (($_GET[$key]=="On" || $groups>0))// && !empty($joins[$key]))
                $query0 .= GetJoin($fromtable,$key);

        /*
        if ($_GET["fTest"]!="On")
        {
            if ($_GET["fSourceName"]!="On")
                $query0 .= " LEFT JOIN Source USING(fSourceKEY) ";
            $query0 .= " WHERE fTest='no'";
        }
        */

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

        $query0 .= StatusQuery("fCorsikaSimTelarray", $needs, $timelimits);
        $query0 .= StatusQuery("fChimp", $needs, $timelimits);
        $query0 .= StatusQuery("fCTAStar", $needs, $timelimits);
        $query0 .= StatusQuery("fStereoB", $needs, $timelimits);
        $query0 .= StatusQuery("fStereoC", $needs, $timelimits);
        $query0 .= StatusQuery("fStereoG", $needs, $timelimits);

        if ((!empty($_GET["fRunMin"]) || $_GET["fRunMin"]=="0") && !empty($_GET["fRunMax"]))
            $query0 .= "MCRunData.fMCRunNumber BETWEEN " . $_GET["fRunMin"] . " AND " . $_GET["fRunMax"] . " ";

//        if ((!empty($_GET["fZDMin"]) || $_GET["fZDMin"]==0) && !empty($_GET["fZDMax"]))
//            $query0 .= "AND fZenithDistance BETWEEN " . $_GET["fZDMin"] . " AND  " . $_GET["fZDMax"] . " ";

/*
        if (!empty($_GET["fDate"]))
            $query0 .= " AND fRunStart REGEXP \"^" . $_GET["fDate"] . "\" ";
*/

        /*
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
        */

        //remove WHERE or AND in the end of the query
        $query0=ereg_replace(" WHERE \$", " ", $query0);
        $query0=ereg_replace(" AND \$", " ", $query0);

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
//            $query0 .= " GROUP BY ";
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
            $query0 .=" ORDER BY MCRunData.fMCRunNumber ASC";

        if (empty($_GET["fNumStart"]))
            $start=0;
        else
            $start=$_GET["fNumStart"];

        if (empty($_GET["fSendTxt"]))
            $query0 .= " LIMIT " . $start . ", " . $_GET["fNumResults"];

        return $query0;
    }

    function InitGet()
    {
        // Find out whether it is the first call to the php script
        $first = empty($_GET["fRunMin"]) && empty($_GET["fRunMax"]);

        InitCTAMCRunInfo($first);
    }

    function PrintForm($host, $user, $pw, $db)
    {
        printf("<center>\n");
        printf("<form action=\"ctamcinfo.php\" METHOD=\"GET\">\n");

        printf("<input id='sh' type='hidden' name='fShowHide' value='");
        if (!empty($_GET["fShowHide"]))
            printf("%s", $_GET["fShowHide"]);
        else
            printf("1000000010");
        printf("'>\n");
        printf("<img id='allbutton' src='minus.png' alt='-' onclick=\"showhide('all')\"> <b>Menu</b>&nbsp;&nbsp;&nbsp;&nbsp;\n");
        printf("&nbsp;&nbsp;<img id='infobutton'   src='plus.png' alt='+' onClick=\"showhide('info');showhide('info2')\"> MCRunInfo  \n");
        printf("&nbsp;&nbsp;<img id='statbutton'   src='plus.png' alt='+' onClick=\"showhide('stat');showhide('fail')\">  StatusInfo \n");
        printf("&nbsp;&nbsp;<img id='rangesbutton' src='plus.png' alt='+' onClick=\"showhide('ranges')\">                 Ranges     \n");

        printf(" <div id='all' style='display:block'>");
        PrintCTAMCRunInfoMenu($host,$user,$pw,$db);
//        PrintRunInfo2Menu($host,$user,$pw,$db);
        PrintCTAMCRunStatMenu();
        PrintFailMenu();
        printf(" <div id='ranges' style='display:none'>");
//        PrintSequMenu($host,$user,$pw,$db);
//        PrintNightRangeMenu($host,$user,$pw,$db, "RunData");
        printf("<p>");
//        PrintZdRangeMenu($host,$user,$pw,$db);
        PrintCTAMCRunRangeMenu($host,$user,$pw,$db);
//        printf(" <P>\n");
//        PrintSourceMenu($host,$user,$pw,$db);
        printf("<p>");
        printf("</div>");
        printf("</div>");
        printf(" <P>\n");

        ini_set("mysql.trace_mode", "Off");
        ini_set("display_errors", "Off");

        PrintNumResPullDown();
        PrintButtons("ctamcinfo.php");

        printf("</form>\n");
        printf("</center>\n");
        printf("</td>\n");
        printf("</tr>\n");
        printf("<tr class='Block'>\n");
        printf("<td>\n");
    }

    function PrintPage($html, $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $needs, $timelimits)
    {
        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);
        mysql_query("SET BIG_TABLES=1"); // necessary for mySQL <= 4

        $query0 = CreateQuery($alias, $checkwhere, $checkgroup, $checkstatusgroup, $needs, $timelimits);

        $result0 = mysql_query($query0, $db_id);
        $result1 = mysql_query("SELECT FOUND_ROWS()", $db_id);

        if ($result0)
        {
            if ($html=="1" || $html=="2")
                PrintMagicTable($result0, $alias, $rightalign, "", "", "", $result1, "");
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
    include ("ctadefs.php");

    ini_set("display_errors",   "On");
    ini_set("mysql.trace_mode", "On");

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-type: application/octet");
        header("Content-Disposition: attachment; filename=query-result.txt");

        PrintPage("0", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $needs, $timelimits);
    }
    else
    {
        if (empty($_GET["fPrintTable"]))
            echo (file_get_contents("index-header.html"));

        $environment = sizeof($_GET);

        InitGet();
        if (empty($_GET["fPrintTable"]))
            PrintForm($host, $user, $pw, $db);

        if ($environment==0)
            printf("No query submitted yet.<BR>");
        else
        {
            if (empty($_GET["fPrintTable"]))
                PrintPage("1", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $needs, $timelimits);
            else
                PrintPage("2", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $needs, $timelimits);
        }

        if (empty($_GET["fPrintTable"]))
            echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors",   "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
