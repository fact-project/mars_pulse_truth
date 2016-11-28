<?php
{
    function CreateQuery($_GET, $alias, $checkstatusgroup, $checkwhere, $needs)
    {
        $statusgroups = 0;
        foreach ($checkstatusgroup as $element)
            if ($element==-1)
                $statusgroups++;

        $query0 = "SELECT SQL_CALC_FOUND_ROWS ";

        if ($statusgroups>0)
        {
            foreach ($checkstatusgroup as $key => $element)
                if ($element==-1)
                    $query0 .= " (if(IsNull(" . $key . "), if(isnull(fStartTime), 'not done', if(isnull(fFailedTime),if(isnull(" . $needs[$key] . "),'not done',if(date_sub(Now(),interval 12 hour) < fStartTime,'running','crashed')),if(isnull(" . $needs[$key] . "),'not done','failed'))) ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "', ";
//                    $query0 .= " (if(IsNull(" . $key . "), 'not done' ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "', ";

            $query0 .= " count(*) as '# days'";

        }
        else
        {
            if (!empty($_GET["fSendTxt"]))
                $query0 .= " fDate ";
            else
            {
                $query0 .= " CONCAT( fDate , ':  <A&ws;HREF=\"index.php?', 'fRunStart=On', '&fZenithDistance=On', '&fMeanTriggerRate=On', '&fRunTypeName=On', '&fNumEvents=On', '&fSourceName=On&', 'fExcludedFDAKEY=1', '&fSequenceFirst=On', '&fStartDate=', fDate, '&fStopDate=', fDate, '&fNumResults=500\">Runs</A> ' , ";
                $query0 .= " ' <A&ws;HREF=\"queryseq.php?', 'fRunStart=On', '&fZenithDistanceMin=On', '&fNumEvents=On', '&fSourceName=On&', '&fStartDate=', fDate, '&fStopDate=', fDate, '&fNumResults=500\">Sequ</A> ') ";
            }
            $query0 .= " as 'Date' ";

            foreach ($_GET as $key => $element)
                if ($_GET[$key]=="On")
                    if (empty($checkwhere[$key]) || $checkwhere[$key]==0)
                        $query0 .= ", " . $key . " as '" . $alias[$key] . "' ";
        }

        $query0 .= " FROM SequenceBuildStatus ";

        if (strpos($query0, " WHERE ")==FALSE)
            $query0 .= " WHERE ";
        else
            $query0 .= " AND ";

        $query0 .= StatusQuery("fCCFilled", $needs, $timelimits);
        $query0 .= StatusQuery("fExclusionsDone", $needs, $timelimits);
        $query0 .= StatusQuery("fSequenceEntriesBuilt", $needs, $timelimits);

        $query0 .= "fDate BETWEEN '" . $_GET["fRunMin"] . "' AND '" . $_GET["fRunMax"] . "' ";

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

        if (!empty($_GET["fSortBy"]))
        {
            $query0 .= " ORDER BY " . substr($_GET["fSortBy"], 0, -1) . " ";
            if (substr($_GET["fSortBy"], -1)=="-")
                $query0 .= "DESC";
        }

        if (empty($_GET["fSortBy"]) && $statusgroups==0)
            $query0 .= "ORDER BY SequenceBuildStatus.fDate ASC ";

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

        if (empty($_GET["fCCFilled"]))
            $_GET["fCCFilled"]=$first?"On":"";

        if (empty($_GET["fExclusionsDone"]))
            $_GET["fExclusionsDone"]=$first?"On":"";

        if (empty($_GET["fSequenceEntriesBuilt"]))
            $_GET["fSequenceEntriesBuilt"]=$first?"On":"";
    }

    function PrintForm($_GET, $host, $user, $pw, $db)
    {
        printf("<center>\n");
        printf("<form action=\"statussbs.php\" METHOD=\"GET\">\n");
        printf(" <table>\n");
        printf("  <tr><td>\n");

        PrintStatusMenu("fCCFilled", "CC Filled");

        printf("  </td><td>\n");

        PrintStatusMenu("fExclusionsDone", "Exclusions");

        printf("  </td><td>\n");

        PrintStatusMenu("fSequenceEntriesBuilt", "Sequences");

        printf(" </td></tr></table>\n");
        printf(" <p>\n");


        if (empty($_GET["fRunMin"]))
            $min = GetMin("fDate", "SequenceBuildStatus", $host, $user, $pw, $db);
        else
            $min = $_GET["fRunMin"];

        if (empty($_GET["fRunMax"]))
            $max = GetMax("fDate", "SequenceBuildStatus", $host, $user, $pw, $db);
        else
            $max = $_GET["fRunMax"];


        printf("Date&nbsp;from&nbsp;<input name=\"fRunMin\" type=\"text\" size=\"10\" maxlength=\"10\" value=\"%s\">\n", $min);
        printf("to&nbsp;<input name=\"fRunMax\" type=\"text\" size=\"10\" maxlength=\"10\" value=\"%s\">&nbsp;&nbsp;&nbsp;\n", $max);

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
        printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"statussbs.php\"'>&nbsp;&nbsp;&nbsp;\n");
        if (strchr($_SERVER["REQUEST_URI"], '?')!=FALSE)
            printf("<input class='Width' type='button' value='Get .txt' onClick='self.location.href=\"%s&fSendTxt=1\"'>&nbsp;&nbsp;&nbsp;\n", $_SERVER["REQUEST_URI"]);
        printf("</form>\n");
        printf("</center>\n");
        printf("</td>\n");
        printf("</tr>\n");
        printf("<tr class='Block'>\n");
        printf("<td>\n");
    }

    function PrintPage($html, $host, $user, $pw, $db, $alias, $rightalign, $checkstatusgroup, $checkwhere, $needs)
    {
        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);

        $query0 = CreateQuery($_GET, $alias, $checkstatusgroup, $checkwhere, $needs);

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
    include ("magicdefs.php");

    ini_set("display_errors", "On");
    ini_set("mysql.trace_mode", "On");

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-type: application/octet");
        header("Content-Disposition: attachment; filename=query-result.txt");

        PrintPage("0", $host, $user, $pw, $db, $alias, $rightalign, $checkstatusgroup, $checkwhere, $needs);
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
            PrintPage("1", $host, $user, $pw, $db, $alias, $rightalign, $checkstatusgroup, $checkwhere, $needs);

        echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
