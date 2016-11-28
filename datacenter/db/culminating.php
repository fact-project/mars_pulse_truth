<?php
{
    function CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup)
    {
        $fromtable="Sequences";

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
            //--------------------------------------------------
            $query0 .= "SUM(fRunTime)/3600 as '" . $alias["SUM(fRunTime)/3600"] . "', ";
            //--------------------------------------------------
            $query0 .= " SUM(fNumEvents) as '" . $alias["SUM(fNumEvents)"] . "', ";
            $query0 .= " COUNT(*) as '# Sequ' ";
        }
        else
        {
            if (!empty($_GET["fSendTxt"]))
                $query0 .= " SELECT fSequenceFirst ";
            else
                $query0 .= " CONCAT('<A&ws;HREF=\"index.php?',  'fRunStart=On', '&fZenithDistance=On', '&fRunMin=', fSequenceFirst, '&fMeanTriggerRate=On', '&fRunTypeName=On', '&fRunMax=', fSequenceLast, '&fNumEvents=On', '&fSourceName=On&', 'fExcludedFDAKEY=1', '&fSequenceFirst=On', '&fNumResults=500\">', fSequenceFirst, '</A>') ";
            $query0 .= " as 'Sequence#' ";

            foreach ($_GET as $key => $element)
                if ($_GET[$key]=="On")
                    if (empty($checkwhere[$key]) || $checkwhere[$key]==0)
                        $query0 .= ", " . $key . " as '" . $alias[$key] . "' ";
        }

        $query0 .= " FROM " . $fromtable;

        foreach ($_GET as $key => $element)
            if (($_GET[$key]=="On" || $groups>0))// && !empty($joins[$key]))
                $query0 .= GetJoin($fromtable, $key);

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

        $query0 .= "fSequenceFirst BETWEEN " . $_GET["fRunMin"] . " AND " . $_GET["fRunMax"] . " ";

        if (!empty($_GET["fSourceN"]))
            $query0 .= " AND fSourceName REGEXP \"^" . $_GET["fSourceN"] . "\" ";

        $query0 .= " AND ((fAzimuthMin<0 and fAzimuthMax>0) OR (fAzimuthMin<180 and fAzimuthMax>180)) ";

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

        if (!empty($_GET["fSortBy"]))
        {
            $query0 .= " ORDER BY " . substr($_GET["fSortBy"], 0, -1) . " ";
            if (substr($_GET["fSortBy"], -1)=="-")
                $query0 .= "DESC";
        }

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

        if (empty($_GET["fLastUpdate"]))
            $_GET["fLastUpdate"]="Off";

        if (empty($_GET["fSourceName"]))
            $_GET["fSourceName"]=$first?"On":"";

        if (empty($_GET["fRunStart"]))
            $_GET["fRunStart"]=$first?"On":"";

        if (empty($_GET["fAzimuthMin"]))
            $_GET["fAzimuthMin"]="Off";

        if (empty($_GET["fAzimuthMax"]))
            $_GET["fAzimuthMax"]="Off";

        if (empty($_GET["fZenithDistanceMin"]))
            $_GET["fZenithDistanceMin"]=$first?"On":"";

        if (empty($_GET["fZenithDistanceMax"]))
            $_GET["fZenithDistanceMax"]="Off";

        if (empty($_GET["fRunTime/60"]))
            $_GET["fRunTime/60"]="Off";

    }

    function PrintForm($_GET, $host, $user, $pw, $db)
    {
        printf("<center>\n");
        printf("<form action=\"culminating.php\" METHOD=\"GET\">\n");
        printf(" <table>\n");
        printf("  <tr>\n");

        CheckBox("fAzimuthMin",        "Azimuth min");
        CheckBox("fAzimuthMax",        "Azimuth max");
        CheckBox("fZenithDistanceMin", "Zenith distance min");
        CheckBox("fZenithDistanceMax", "Zenith distance max");

        printf("  </tr><tr>\n");

        CheckBox("fRunTime/60",        "Duration");

        printf(" </table>\n");
        printf(" <p>\n");

        printf(" <table>\n");
        printf("  <tr><td>\n");
        PrintPullDown($host, $user, $pw, $db, "Source",      "fSourceName",      "fSourceKEY", "Source Name");
        printf("  </td></tr>\n");
        printf(" </table>\n");
        printf(" <p>\n");

        if (empty($_GET["fRunMin"]))
            $min = GetMin("fSequenceFirst", "Sequences", $host, $user, $pw, $db);
        else
            $min = $_GET["fRunMin"];

        if (empty($_GET["fRunMax"]))
            $max = GetMax("fSequenceFirst", "Sequences", $host, $user, $pw, $db);
        else
            $max = $_GET["fRunMax"];


        printf("Source&nbsp;(<A HREF=\"regexp.html\">regexp</A>)&nbsp;<input name=\"fSourceN\" type=\"text\" size=\"15\" maxlength=\"15\" value=\"");
        if (!empty($_GET["fSourceN"]))
            printf("%s", $_GET["fSourceN"]);
        printf("\">&nbsp;&nbsp;&nbsp;\n");

        printf("Sequences&nbsp;from&nbsp;<input name=\"fRunMin\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\">\n", $min);
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
        printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"culminating.php\"'>&nbsp;&nbsp;&nbsp;\n");
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
    include ("magicdefs.php");

    ini_set("display_errors", "On");
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

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
