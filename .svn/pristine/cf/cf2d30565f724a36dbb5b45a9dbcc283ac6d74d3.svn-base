<?php
{
    function CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup, $needs)
    {
        $fromtable="Sequences";

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
            if ($groups>0)
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
            $query0 .= "SUM(fRunTime)/3600 as '" . $alias["SUM(fRunTime)/3600"] . "', ";
            //--------------------------------------------------
            $query0 .= " SUM(fNumEvents) as '" . $alias["SUM(fNumEvents)"] . "', ";
            $query0 .= " Min(fZenithDistanceMin) as '" . $alias["Min(fZenithDistanceMin)"] . "', ";
            $query0 .= " Max(fZenithDistanceMax) as '" . $alias["Max(fZenithDistanceMax)"] . "', ";
            $query0 .= " COUNT(*) as '# Sequ' ";

            if ($statusgroups>0)
            {
                foreach ($checkstatusgroup as $key => $element)
                    if ($element==-1)
                        $query0 .= ", (if(IsNull(" . $key . "), if(isnull(fStartTime), 'not done', if(isnull(fFailedTime),if(isnull(" . $needs[$key] . "),'not done',if(date_sub(Now(),interval 12 hour) < fStartTime,'running','crashed')),if(isnull(" . $needs[$key] . "),'not done','failed'))) ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "' ";
//                        $query0 .= " (if(IsNull(" . $key . "), 'not done' ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "', ";

                $query0 .= ", COUNT(*) as '# Sequ'";

            }
        }
        else
        {
            if (!empty($_GET["fSendTxt"]))
                $query0 .= " " . $fromtable . ".fSequenceFirst as '" . $alias["fSequenceFirst"] . "' ";
            else
            {
                //old version using fRunMin, fRunMax
                //$query0 .= " CONCAT('<A&ws;HREF=\"runinfo-aio.php?',  'fRunStart=On', '&amp;fZenithDistance=On', '&amp;fRunMin=', Sequences.fSequenceFirst, '&amp;fMeanTriggerRate=On', '&amp;fTest=On', '&amp;fRunTypeName=On', '&amp;fRunMax=', fSequenceLast, '&amp;fNumEvents=On', '&amp;fSourceName=On&amp;', 'fExcludedFDAKEY=1', '&amp;fSequenceFirst=On', '&amp;fRawFileAvail=On', '&amp;fCCFileAvail=On', '&amp;fCaCoFileAvail=On', '&amp;fNumResults=500\">', Sequences.fSequenceFirst, '</A>') ";
                //new version using fSequenceNo
                $query0 .= " CONCAT('<A&ws;HREF=\"runinfo-aio.php?',  'fRunStart=On', '&amp;fZenithDistance=On', '&amp;fSequenceNo=', Sequences.fSequenceFirst, '&amp;fMeanTriggerRate=On', '&amp;fTest=On', '&amp;fRunTypeName=On', '&amp;fNumEvents=On', '&amp;fSourceName=On&amp;', 'fExcludedFDAKEY=1', '&amp;fSequenceFirst=On', '&amp;fRawFileAvail=On', '&amp;fCCFileAvail=On', '&amp;fCaCoFileAvail=On', '&amp;fNumResults=500\">', 'M', Sequences.fTelescopeNumber, ':', LPAD(Sequences.fSequenceFirst, 8, ' '), '</A>') ";
                //$query0 .= " as " . $alias["fSequenceFirst"];
                $query0 .= " as 'Tel:Sequ'";
                if (!empty($_GET["fLinks"]))
                {
                    $query0 .= ", CONCAT('<A&ws;HREF=\"tabs.php?t=calib&n=', fSequenceFirst, '\">cal</A>'";
                    $query0 .= ", '&nbsp;<A&ws;HREF=\"tabs.php?t=signal&n=', fSequenceFirst, '\">sig</A>'";
                    $query0 .= ", '&nbsp;<A&ws;HREF=\"tabs.php?t=star&n=', fSequenceFirst, '\">star</A>'";
                    $query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/sequences/' , LEFT(LPAD(CONCAT(Sequences.fSequenceFirst, '.'), 9,'0'),4), '/sequence', LPAD(CONCAT(Sequences.fSequenceFirst , '.txt'), 12,'0') , '\">', 'f </A>' ";
                    //$query0 .= ", '&nbsp;<A&ws;HREF=\"sequence.php?fSequenceNo=', Sequences.fSequenceFirst , '&amp;fAll=On\">i</A>'";
                    $query0 .= ", '&nbsp;<A&ws;HREF=\"queryrbk.php?fNight=', date_format(adddate(fRunStart, interval +13 HOUR), '%Y-%m-%d') , '&amp;fDate=2\">rbk</A>') ";
                    //$query0 .= ", CONCAT('<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/callisto/' , LEFT(LPAD(CONCAT(Sequences.fSequenceFirst, '.'), 9,'0'),4), '/', LPAD(CONCAT(Sequences.fSequenceFirst, '/'), 9,'0'), 'callisto', LPAD(CONCAT(Sequences.fSequenceFirst , '.html'), 13,'0') , '\">cl</A>'";
                    //$query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/callisto/' , LEFT(LPAD(CONCAT(Sequences.fSequenceFirst, '.'), 9,'0'),4), '/', LPAD(CONCAT(Sequences.fSequenceFirst, '/'), 9,'0'), '\">c</A>'";
                    //$query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/star/' , LEFT(LPAD(CONCAT(Sequences.fSequenceFirst, '.'), 9,'0'),4), '/', LPAD(CONCAT(Sequences.fSequenceFirst, '/'), 9,'0'), 'star', LPAD(CONCAT(Sequences.fSequenceFirst , '.html'), 13,'0') , '\">sl</A>'";
                    //$query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/star/' , LEFT(LPAD(CONCAT(Sequences.fSequenceFirst, '.'), 9,'0'),4), '/', LPAD(CONCAT(Sequences.fSequenceFirst, '/'), 9,'0'), '\">s</A>'";
                    //$query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/sequences/' , LEFT(LPAD(CONCAT(Sequences.fSequenceFirst, '.'), 9,'0'),4), '/sequence', LPAD(CONCAT(Sequences.fSequenceFirst , '.txt'), 12,'0') , '\">', 'f </A>' ";
                    //$query0 .= ", '&nbsp;<A&ws;HREF=\"sequence.php?fSequenceNo=', Sequences.fSequenceFirst , '&amp;fAll=On\">i</A>'";
                    //$query0 .= ", '&nbsp;<A&ws;HREF=\"queryrbk.php?fNight=', date_format(adddate(fRunStart, interval +13 HOUR), '%Y-%m-%d') , '&amp;fDate=2\">rbk</A>') ";
                    $query0 .= " as 'Links'";
                }
            }

            foreach ($_GET as $key => $element)
                if ($_GET[$key]=="On")
                    if ($key!="fLinks" && $key!="fOff" && $key!="fOnlyOff" && $key!="fOnlySum")
                        if (empty($checkwhere[$key]) || $checkwhere[$key]==0)
                            $query0 .= ", " . $key . " as '" . $alias[$key] . "' ";
        }

        $query0 .= " FROM " . $fromtable;

        $query0 .= " LEFT JOIN SequenceProcessStatus USING(fSequenceFirst,fTelescopeNumber) ";
        $query0 .= " LEFT JOIN Calibration USING(fSequenceFirst,fTelescopeNumber) ";
        $query0 .= " LEFT JOIN Star USING(fSequenceFirst,fTelescopeNumber) ";

        foreach ($_GET as $key => $element)
            if (($_GET[$key]=="On" || $groups>0))// && !empty(GetJoin($fromtable,$key)))
                $query0 .= GetJoin($fromtable,$key);

        if ($_GET["fTest"]!="On")
        {
            if ($_GET["fSourceName"]!="On")
                $query0 .= " LEFT JOIN Source USING(fSourceKEY) ";
            $query0 .= " WHERE fTest='no'";
        }

        if ($_GET["fOff"]=="Off")
        {
            if (strpos($query0, " WHERE ")==FALSE)
                $query0 .= " WHERE ";
            else
                $query0 .= " AND ";

            $query0 .= " NOT (fSourceName like '%Off%')";
        }

        if ($_GET["fOnlyOff"]=="On")
        {
            if (strpos($query0, " WHERE ")==FALSE)
                $query0 .= " WHERE ";
            else
                $query0 .= " AND ";

            $query0 .= " fSourceName like '%Off%'";
        }

        if ($_GET["fOnlySum"]=="On")
        {
            if (strpos($query0, " WHERE ")==FALSE)
                $query0 .= " WHERE ";
            else
                $query0 .= " AND ";

            $query0 .= " NOT ISNULL(fRateSumEvts) ";
        }

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

        $query0 .= StatusQuery("fSequenceFileWritten", $needs, $timelimits);
        $query0 .= StatusQuery("fAllFilesAvail", $needs, $timelimits);
        $query0 .= StatusQuery("fCallisto", $needs, $timelimits);
        $query0 .= StatusQuery("fFillCallisto", $needs, $timelimits);
        $query0 .= StatusQuery("fStar", $needs, $timelimits);
        $query0 .= StatusQuery("fFillStar", $needs, $timelimits);

        if (!empty($_GET["fRunMin"]) && !empty($_GET["fRunMax"]))
            $query0 .= "Sequences.fSequenceFirst BETWEEN " . $_GET["fRunMin"] . " AND " . $_GET["fRunMax"] . " ";
        else
            $query0 = substr($query0, 0, -4);

        if ((!empty($_GET["fZDMin"]) || $_GET["fZDMin"]==0) && !empty($_GET["fZDMax"]))
            $query0 .= "AND (fZenithDistanceMin >= " . $_GET["fZDMin"] . " AND fZenithDistanceMax <= " . $_GET["fZDMax"] . ") ";

        if (!empty($_GET["fSourceN"]))
            $query0 .= " AND fSourceName REGEXP \"^" . $_GET["fSourceN"] . "\" ";

        if (!empty($_GET["fStartDate"]))
        {
            if (strpos(strrev($query0), " DNA ")!=0 || !strpos(strrev($query0), " DNA "))
                $query0 .= " AND ";

            $startdate=substr($_GET["fStartDate"], 0, 10);
            if ($startdate=="0000-00-00")
                $query0 .=" fRunStart >= '" . $startdate . " 00:00:00' ";
            else
                $query0 .= " fRunStart >= ADDDATE('" . $startdate . " 13:00:00', INTERVAL -1 DAY) ";
        }

        if (!empty($_GET["fStopDate"]))
        {
            if (strpos(strrev($query0), " DNA ")!=0 || !strpos(strrev($query0), " DNA "))
                $query0 .= " AND ";

            $stopdate=substr($_GET["fStopDate"], 0, 10);
            $query0 .= " fRunStart < '" . $stopdate . " 13:00:00' ";
        }

        if (!empty($_GET["fStarStart"]))
        {
            if (strpos(strrev($query0), " DNA ")!=0 || !strpos(strrev($query0), " DNA "))
                $query0 .= " AND ";

            $starstart=substr($_GET["fStarStart"], 0, 10);
            $query0 .=" fStar >= '" . $starstart . " 00:00:00' ";
        }

        if (!empty($_GET["fStarStop"]))
        {
            if (strpos(strrev($query0), " DNA ")!=0 || !strpos(strrev($query0), " DNA "))
                $query0 .= " AND ";

            $starstop=substr($_GET["fStarStop"], 0, 10);
            $query0 .= " fStar < '" . $starstop . " 23:59:59' ";
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
            $query0 .= " ORDER BY " . GetTable($fromtable,$val) . " ";
            if (substr($_GET["fSortBy"], -1)=="-")
                $query0 .= "DESC";
        }

        if (empty($_GET["fSortBy"]) && $groups==0 && $statusgroups==0)
            $query0 .= "ORDER BY Sequences.fSequenceFirst ASC ";

        if (empty($_GET["fNumStart"]))
            $_GET["fNumStart"]=0;

        if (empty($_GET["fSendTxt"]))
            $query0 .= " LIMIT " . $_GET["fNumStart"] . ", " . $_GET["fNumResults"];

        return $query0;
    }

    function InitGet()
    {
        // Find out whether it is the first call to the php script
        $first = empty($_GET["fRunMin"]) && empty($_GET["fRunMax"]);

        if (empty($_GET["fNumResults"]))
            $_GET["fNumResults"]="50";

        if (empty($_GET["fRunStart"]))
            $_GET["fRunStart"]=$first?"On":"";

        if (empty($_GET["fRunTime/60"]))
            $_GET["fRunTime/60"]=$first?"On":"";

        if (empty($_GET["fZenithDistanceMin"]))
            $_GET["fZenithDistanceMin"]=$first?"On":"";

        if (empty($_GET["fSourceName"]))
            $_GET["fSourceName"]=$first?"On":"";

        if (empty($_GET["fStar"]))
            $_GET["fStar"]=$first?"On":"";

        if (empty($_GET["fInhomogeneity"]))
            $_GET["fInhomogeneity"]=$first?"On":"";

        if (empty($_GET["fPSF"]))
            $_GET["fPSF"]=$first?"On":"";

        if (empty($_GET["fMeanPedRmsInner"]))
            $_GET["fMeanPedRmsInner"]=$first?"On":"";

        if (empty($_GET["fOff"]))
            $_GET["fOff"]=$first?"On":"";

        if (empty($_GET["fLinks"]))
            $_GET["fLinks"]=$first?"On":"";

        if (empty($_GET["fOnlyOff"]))
            $_GET["fOnlyOff"]=$first?"Off":"";

        if (empty($_GET["fOnlySum"]))
            $_GET["fOnlySum"]=$first?"Off":"";

        InitFindOffData($first);
    }

    function PrintForm($_GET, $host, $user, $pw, $db, $limitsmean, $limitsmin, $limitsmax, $alias)
    {
        printf("<center>\n");
        printf("<form action='sequinfo-aio.php' METHOD='GET'>\n");
        printf("<input id='sh' type='hidden' name='fShowHide' value='");
        if (!empty($_GET["fShowHide"]))
            printf("%s", $_GET["fShowHide"]);
        else
            printf("1000000010");
        printf("'>\n");
        printf("<img id='allbutton' src='minus.png' alt='-' onclick='showhide(\"all\")'> <b>Menu</b>&nbsp;&nbsp;&nbsp;&nbsp; \n");
        printf("&nbsp;&nbsp;<img id='infobutton'   src='plus.png'  alt='+' onClick='showhide(\"info\");showhide(\"info2\")'> SequInfo   \n");
        printf("&nbsp;&nbsp;<img id='statbutton'   src='plus.png'  alt='+' onClick='showhide(\"stat\");showhide(\"fail\")'>  StatusInfo \n");
        printf("&nbsp;&nbsp;<img id='calbutton'    src='plus.png'  alt='+' onClick='showhide(\"cal\")'>                      CalInfo    \n");
        printf("&nbsp;&nbsp;<img id='starbutton'   src='plus.png'  alt='+' onClick='showhide(\"star\")'>                     StarInfo   \n");
        printf("&nbsp;&nbsp;<img id='limitsbutton' src='plus.png'  alt='+' onClick='showhide(\"limits\")'>                   Limits     \n");
        printf("&nbsp;&nbsp;<img id='rangesbutton' src='minus.png' alt='-' onClick='showhide(\"ranges\")'>                   Ranges     \n");

        printf(" <div id='all' style='display:block'>");

        PrintSequInfoMenu();
        PrintSequInfo2Menu($host,$user,$pw,$db);
        PrintSequStatMenu();
        PrintFailMenu();
        PrintCalMenu();
        PrintStarMenu();
        PrintLimitsMenu($limitsmean, $limitsmin, $limitsmax, $alias, "");

        printf(" <div id='ranges' style='display:block'>");
        PrintZdRangeMenu($host,$user,$pw,$db);
        PrintSequRangeMenu($host,$user,$pw,$db);
        printf("<p>");
        PrintSourceMenu($host,$user,$pw,$db);
        PrintGroupByDateMenu();
        printf("<p>");
        PrintNightRangeMenu($host,$user,$pw,$db, "Sequences");
        printf("<p>");
        printf("</div>");

        printf("</div>");
        printf(" <P>\n");


        ini_set("mysql.trace_mode", "Off");
        ini_set("display_errors", "Off");

        PrintNumResPullDown();
        PrintButtons("sequinfo-aio.php");

        printf("</form>\n");
        printf("</center>\n");
        printf("</td>\n");
        printf("</tr>\n");
        printf("<tr class='Block'>\n");
        printf("<td>\n");
    }

    function PrintPage($html, $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $limitsmean, $limitsmin, $limitsmax, $needs)
    {
        $db_id = mysql_pconnect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);

        $query0 = CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup, $needs);

        $result0 = mysql_query($query0, $db_id);
        $result1 = mysql_query("SELECT FOUND_ROWS()", $db_id);

        if ($result0)
        {
            if ($html=="1" || $html=="2")
                PrintMagicTable($result0, $alias, $rightalign, $limitsmean, $limitsmin, $limitsmax, $result1, "");
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
    include ("menu.php");
    include ("db.php");
    include ("magicdefs.php");

    ini_set("display_errors", "On");
    ini_set("mysql.trace_mode", "On");

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-type: application/octet");
        header("Content-Disposition: attachment; filename=query-result.txt");

        PrintPage("0", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $limitsmean, $limitsmin, $limitsmax, $needs);
    }
    else
    {
        if (empty($_GET["fPrintTable"]))
            echo (file_get_contents("index-header.html"));

        $environment = sizeof($_GET);

        InitGet();
        if (empty($_GET["fPrintTable"]))
            PrintForm($_GET, $host, $user, $pw, $db, $limitsmean, $limitsmin, $limitsmax, $alias);

        if ($environment==0)
            printf("No query submitted yet.<BR>");
        else
        {
            if (empty($_GET["fPrintTable"]))
                PrintPage("1", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $limitsmean, $limitsmin, $limitsmax, $needs);
            else
                PrintPage("2", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $limitsmean, $limitsmin, $limitsmax, $needs);
        }

        if (empty($_GET["fPrintTable"]))
            echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
