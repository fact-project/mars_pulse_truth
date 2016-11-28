<?php
{
    function CreateQuery($_GET, $alias, $checkwhere, $checkgroup, $checkstatusgroup)
    {
        $fromtable="OpticalData";

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
            $query0 .= " COUNT(*) as '# Runs' ";
        }
        else
        {
            $query0 .= " fTimeStamp as 'Time' ";

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
            if ($startdate=="0000-00-00")
                $query0 .=" fTimestamp >= '" . $startdate . " 00:00:00' ";
            else
                $query0 .= " fTimestamp >= ADDDATE('" . $startdate . " 13:00:00', INTERVAL -1 DAY) ";
        }

        if (!empty($_GET["fStopDate"]))
        {
//            if (strpos(strrev($query0), " DNA ")!=0)
//                $query0 .= " AND ";

            $stopdate=substr($_GET["fStopDate"], 0, 10);
            $query0 .= " AND fTimestamp < '" . $stopdate . " 13:00:00' ";
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
            
        if (empty($_GET["fTimestamp"]))
            $_GET["fTimestamp"]=$first?"On":"";

        if (empty($_GET["fExposure"]))
            $_GET["fExposure"]=$first?"On":"";

        if (empty($_GET["fSkyLevel"]))
            $_GET["fSkyLevel"]=$first?"On":"";

        if (empty($_GET["fFWHM"]))
            $_GET["fFWHM"]=$first?"On":"";

        if (empty($_GET["fApertureRadius"]))
            $_GET["fApertureRadius"]=$first?"On":"";

        if (empty($_GET["fInstrumentalMag"]))
            $_GET["fInstrumentalMag"]=$first?"On":"";

        if (empty($_GET["fInstrumentalMagErr"]))
            $_GET["fInstrumentalMagErr"]=$first?"On":"";

        if (empty($_GET["fFitsFileName"]))
            $_GET["fFitsFileName"]="Off";

        if (empty($_GET["fObjectName"]))
            $_GET["fObjectName"]=$first?"On":"";

        if (empty($_GET["fStatusName"]))
            $_GET["fStatusName"]="Off";

        if (empty($_GET["fBandName"]))
            $_GET["fBandName"]=$first?"On":"";

        if (empty($_GET["fTelescopeName"]))
            $_GET["fTelescopeName"]=$first?"On":"";

//        if (empty($_GET["fRunStop"]))
//            $_GET["fRunStop"]="Off";

    }

    function PrintForm($_GET, $host, $user, $pw, $db)
    {
        printf("<center>\n");
        printf("<form action=\"opticaldata.php\" METHOD=\"GET\">\n");
        printf(" <table>\n");
        printf("  <tr>\n");

        CheckBox("fTimestamp",         "Time");
        CheckBox("fExposure",      "Exposure");
        CheckBox("fSkyLevel",     "Skylevel");
        CheckBox("fFWHM",          "FWHM");

        printf("  </tr><tr>\n");

        CheckBox("fApertureRadius",     "Aperture radius");
        CheckBox("fInstrumentalMag",    "instrumental magnitude");
        CheckBox("fInstrumentalMagErr", "instrum. mag. error");

        printf("  </tr>\n");
        printf(" </table>\n");
        printf(" <p>\n");

        // pull down boxes

        printf(" <table>\n");
        printf("  <tr><td>\n");
        PrintPullDown($host, $user, $pw, $db, "Telescope",  "fTelescopeName",  "fTelescopeKEY", "Telescope");
        printf("  </td><td>\n");
        PrintPullDown($host, $user, $pw, $db, "Object",      "fObjectName",      "fObjectKEY", "Object Name");
        printf("  </td><td>\n");
        PrintPullDown($host, $user, $pw, $db, "Band",     "fBandName",     "fBandKEY", "Band");
        printf("  </td></tr><tr><td>\n");
        PrintPullDown($host, $user, $pw, $db, "FitsFile",     "fFitsFileName",     "fFitsFileKEY", "Fits File");
        printf("  </td><td>\n");
        PrintPullDown($host, $user, $pw, $db, "Status",  "fStatusName",  "fStatusKEY", "Status Code");
        printf(" </td></tr></table>\n");
        printf(" <p>\n");

        if (empty($_GET["fStartDate"]))
            $timemin = GetMin("fTimestamp", "OpticalData", $host, $user, $pw, $db);
        else
            $timemin = $_GET["fStartDate"];

        if (empty($_GET["fStopDate"]))
            $timemax = GetMax("fTimestamp", "OpticalData", $host, $user, $pw, $db);
        else
            $timemax = $_GET["fStopDate"];

        printf("Night&nbsp;(yyyy-mm-dd)&nbsp;from&nbsp;<input name=\"fStartDate\" type=\"text\" size=\"10\" maxlength=\"10\" value=\"%s\">\n", $timemin);
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
        printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"opticaldata.php\"'>&nbsp;&nbsp;&nbsp;\n");
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
    include ("magicdefs.php");

    ini_set("display_errors",   "On");
    ini_set("mysql.trace_mode", "On");

    $sitepw="\$1\$jfBkkHx1\$WmxNVaOWPCBC8asfKDfBZ/";
    $siteuser="optical";

    if (!isset($_SERVER['PHP_AUTH_USER']) || $_SERVER['PHP_AUTH_USER']!=$siteuser)
    {
        header('WWW-Authenticate: Basic realm="Optical Data - Tuorla Observatory"');
        header('HTTP/1.0 401 Unauthorized');
        echo 'Action cancelled.';
        return;
    }
    else
    {
//    echo "<p>Hello {$_SERVER['PHP_AUTH_USER']}.</p>";
//    echo "<p>You entered {$_SERVER['PHP_AUTH_PW']} as your password.</p>";
//    printf("pw: %s", crypt($_SERVER['PHP_AUTH_PW']));
        if (crypt($_SERVER['PHP_AUTH_PW'], $sitepw)!=$sitepw || $_SERVER['PHP_AUTH_USER']!=$siteuser)
        {
            printf("<br>pw or user incorrect<br>");
            return;
        }
    }

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
