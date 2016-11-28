<?php
{
    function CreateQuery($_GET, $alias)
    {

        $query0 = "SELECT SQL_CALC_FOUND_ROWS ";
        if ($_GET["fLinks"]=="Off" || !empty($_GET["fSendTxt"]))
            $query0 .= " fSourceName as 'Source' ";
        else
        {
            $query0 .= " CONCAT('<A&ws;HREF=\"sequinfo-aio.php?', ";
            $query0 .= " 'fRunStart=On', '&fZenithDistanceMin=On', '&fNumEvents=On', '&fRunTime%2F60=On', '&fSequenceLast=On', ";
            $query0 .= " '&fSequenceFileWrittenStatus=0', '&fAllFilesAvailStatus=0', '&fCallistoStatus=0', '&fStar=On', '&fStarStatus=0', '&fFillCallistoStatus=0', '&fFillStarStatus=0', ";
            $query0 .= " '&fSourceName=On', '&fSourceKEY=', fSourceKEY, "; //'&fRunMin=0', '&fRunMax=1000000', ";
            $query0 .= " '&fNumResults=500\">', fSourcename, '</A>') ";
            $query0 .= " as 'Source' ";
        }
        $query0 .= " , fSourceKEY as 'KEY', fTest as 'Test' ";
        $query0 .= " FROM Source ";

        if ($_GET["fTest"]=="Off")
            $query0 .= " WHERE fTest='no'";

        if ($_GET["fOff"]=="Off")
        {
            if (strpos($query0, " WHERE ")==FALSE)
                $query0 .= " WHERE ";
            else
                $query0 .= " AND ";

            $query0 .= " NOT (fSourceName like '%Off%')";
        }

        if (!empty($_GET["fSourceN"]))
        {
            if (strpos($query0, " WHERE ")==FALSE)
                $query0 .= " WHERE ";
            else
                $query0 .= " AND ";

            $query0 .= " fSourceName REGEXP \"^" . $_GET["fSourceN"] . "\" ";
        }


        if (!empty($_GET["fSortBy"]))
        {
            $query0 .= " ORDER BY " . substr($_GET["fSortBy"], 0, -1) . " ";
            if (substr($_GET["fSortBy"], -1)=="-")
                $query0 .= "DESC";
        }
        else
            $query0 .= " ORDER BY fSourceKEY ";

        return $query0;
    }

    function InitGet($_GET)
    {

        if (empty($_GET["fOff"]))
            $_GET["fOff"]="Off";

        if (empty($_GET["fTest"]))
            $_GET["fTest"]="Off";

        if (empty($_GET["fLinks"]))
            $_GET["fLinks"]="On";

    }

    function PrintForm($_GET,$host,$user,$pw,$db)
    {
        printf("<center>\n");
        printf("<form action=\"sources.php\" METHOD=\"GET\">\n");
        printf(" <p><table>\n");
        printf("  <tr>\n");

        CheckBox("fOff",   "incl. offsources");
        CheckBox("fTest",  "incl. test-sources");
        CheckBox("fLinks", "links");

        printf(" </table></p>\n");
        printf("<p>");
        PrintSourceMenu($host,$user,$pw,$db);

        ini_set("mysql.trace_mode", "Off");
        ini_set("display_errors", "Off");

        printf("<input class='Width' type='submit' value='Query Table'>&nbsp;&nbsp;&nbsp;\n");
        printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"sources.php\"'>&nbsp;&nbsp;&nbsp;\n");
        if (strchr($_SERVER["REQUEST_URI"], '?')!=FALSE)
            printf("<input class='Width' type='button' value='Get .txt' onClick='self.location.href=\"%s&fSendTxt=1\"'>&nbsp;&nbsp;&nbsp;\n", $_SERVER["REQUEST_URI"]);
        printf("</form>\n");
        printf("</center>\n");
        printf("</td>\n");
        printf("</tr>\n");
        printf("<tr class='Block'>\n");
        printf("<td>\n");
    }

    function PrintPage($html, $host, $user, $pw, $db, $alias, $rightalign)
    {
        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);

        $query0 = CreateQuery($_GET, $alias);

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
    include ("menu.php");
    include ("db.php");
    include ("magicdefs.php");

    ini_set("display_errors", "On");
    ini_set("mysql.trace_mode", "On");

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-type: application/octet");
        header("Content-Disposition: attachment; filename=query-result.txt");

        PrintPage("0", $host, $user, $pw, $db, $alias, $rightalign);
    }
    else
    {
        echo (file_get_contents("index-header.html"));

        $environment = sizeof($_GET);

        InitGet($_GET);
        PrintForm($_GET,$host,$user,$pw,$db);

        if ($environment==0)
            printf("No query submitted yet.<BR>");
        else
            PrintPage("1", $host, $user, $pw, $db, $alias, $rightalign);

        echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
