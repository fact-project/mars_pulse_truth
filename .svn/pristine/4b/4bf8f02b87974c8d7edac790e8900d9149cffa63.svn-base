<?php
{
    function CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup, $needs)
    {
        $fromtable="DataSets";

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
                $query0 .= " COUNT(*) as '# Datasets', ";
            }

            //--------------------------------------------------
            $query0 .= "SUM(fRunTime)/3600 as '" . $alias["SUM(fRunTime)/3600"] . "', ";
            //--------------------------------------------------
            $query0 .= " Min(fZenithDistanceMin) as '" . $alias["Min(fZenithDistanceMin)"] . "', ";
            $query0 .= " Max(fZenithDistanceMax) as '" . $alias["Max(fZenithDistanceMax)"] . "', ";
            $query0 .= " COUNT(*) as '# Sequ' ";

            if ($statusgroups>0)
            {
                foreach ($checkstatusgroup as $key => $element)
                    if ($element==-1)
                        $query0 .= ", (if(IsNull(" . $key . "), if(isnull(fStartTime), 'not done', if(isnull(fFailedTime),if(isnull(" . $needs[$key] . "),'not done',if(date_sub(Now(),interval 12 hour) < fStartTime,'running','crashed')),if(isnull(" . $needs[$key] . "),'not done','failed'))) ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "' ";
//                        $query0 .= " (if(IsNull(" . $key . "), 'not done' ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "', ";

                $query0 .= ", COUNT(*) as '# Datasets'";

            }
        }
        else
        {
            $query0 .= " " . $fromtable . ".fDataSetNumber as '" . $alias["fDataSetNumber"] . "' ";
            if (!empty($_GET["fLinks"]))
            {
                //$query0 .= ", CONCAT('<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/datasets/' , LEFT(LPAD(CONCAT(DataSets.fDataSetNumber, '.'), 9,'0'),5), '/dataset', LPAD(CONCAT(DataSets.fDataSetNumber , '.txt'), 12,'0') , '\">f</A>' ";
                //$query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/ganymed/' , LEFT(LPAD(CONCAT(DataSets.fDataSetNumber, '.'), 9,'0'),5), '/', LEFT(LPAD(CONCAT(DataSets.fDataSetNumber, '.'), 9,'0'), 8), '\">g</A>'";
                //$query0 .= ", '&nbsp;<A&ws;HREF=\"ganymed.php?fDataSetNo=', DataSets.fDataSetNumber, '\">i</A>') ";
                $query0 .= ", CONCAT('<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/datasets/' , LEFT(LPAD(CONCAT(DataSets.fDataSetNumber, '.'), 9,'0'),5), '/dataset', LPAD(CONCAT(DataSets.fDataSetNumber , '.txt'), 12,'0') , '\">f</A>' ";
                $query0 .= ", '&nbsp;<A&ws;HREF=\"tabs.php?t=ganymed&n=' , fDataSetNumber, '\">g</A>'";
                $query0 .= ", '&nbsp;<A&ws;HREF=\"tabs.php?t=gplotdb&n=' , fDataSetNumber, '\">pdb</A>'";
                //$query0 .= ", '&nbsp;<A&ws;HREF=\"ganymed.php?fDataSetNo=', DataSets.fDataSetNumber, '\">i</A>' ";
                $query0 .= ") as 'Links'";
            }

            foreach ($_GET as $key => $element)
                if ($_GET[$key]=="On")
                    if ($key!="fLinks")
                        if (empty($checkwhere[$key]) || $checkwhere[$key]==0)
                            $query0 .= ", " . $key . " as '" . $alias[$key] . "' ";
        }

        $query0 .= " FROM " . $fromtable;

        $query0 .= " LEFT JOIN DataSetProcessStatus USING(fDataSetNumber) ";
        $query0 .= " LEFT JOIN Ganymed USING(fDataSetNumber) ";

        foreach ($_GET as $key => $element)
            if (($_GET[$key]=="On" || $groups>0))// && !empty($joins[$key]))
                $query0 .= GetJoin($fromtable,$key);

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

        $query0 .= StatusQuery("fDataSetInserted", $needs, $timelimits);
        $query0 .= StatusQuery("fStarFilesAvail", $needs, $timelimits);
        $query0 .= StatusQuery("fGanymed", $needs, $timelimits);
        $query0 .= StatusQuery("fFillGanymed", $needs, $timelimits);

        if (!empty($_GET["fRunMin"]) && !empty($_GET["fRunMax"]))
            $query0 .= "DataSets.fDataSetNumber BETWEEN " . $_GET["fRunMin"] . " AND " . $_GET["fRunMax"] . " ";

/*
        if (!empty($_GET["fDate"]))
            $query0 .= " AND fRunStart REGEXP \"^" . $_GET["fDate"] . "\" ";
*/

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

        if (empty($_GET["fSortBy"]) && $groups==0 && $statusgroups==0)
            $query0 .= "ORDER BY DataSets.fDataSetNumber ASC ";

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
            $_GET["fNumResults"]="50";

        InitDataSetInfo($_GET, $first);
    }

    function PrintForm($_GET, $host, $user, $pw, $db)
    {
        printf("<center>\n");
        printf("<form action=\"datasetinfo-aio.php\" METHOD=\"GET\">\n");

        printf("<input id='sh' type='hidden' name='fShowHide' value='");
        if (!empty($_GET["fShowHide"]))
            printf("%s", $_GET["fShowHide"]);
        else
            printf("1000000000");
        printf("'>\n");
        printf("<img id='allbutton' src='minus.png' alt='-' onclick=showhide('all')> <b>Menu</b>&nbsp;&nbsp;&nbsp;&nbsp;\n");
        printf("&nbsp;&nbsp;<img id='infobutton'   src='plus.png' alt='+' onClick=showhide('info');showhide('info2')> DataSetInfo\n");
        printf("&nbsp;&nbsp;<img id='statbutton'   src='plus.png' alt='+' onClick=showhide('stat');showhide('fail')>  StatusInfo \n");
        printf("&nbsp;&nbsp;<img id='rangesbutton' src='plus.png' alt='+' onClick=showhide('ranges')>                 Ranges     \n");

        printf(" <div id='all' style='display:block'>");
        PrintDataSetInfoMenu($host, $user, $pw, $db);
        PrintDataSetInfoMenu2($host, $user, $pw, $db);
        printf(" <p>\n");
        PrintFailMenu($host, $user, $pw, $db);
        PrintDataSetStatMenu($host, $user, $pw, $db);

        printf(" <p>\n");
        printf(" <div id='ranges' style='display:none'>");
        PrintSourceMenu($host,$user,$pw,$db);
        PrintDataSetRangeMenu($host,$user,$pw,$db);
        printf("</div>");
        printf("</div>");
        printf(" <P>\n");

        PrintNumResPullDown();

        ini_set("mysql.trace_mode", "Off");
        ini_set("display_errors", "Off");

        PrintButtons("datasetinfo-aio.php");

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

    ini_set("display_errors", "On");
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

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
