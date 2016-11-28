<?php
{
    function PrintForm($_GET)
    {
        if (empty($_GET["fDate"]))
            $_GET["fDate"]=date("Y-m-d");

        printf("<center>\n");
        printf("<form action=\"queryrbk.php\" METHOD=\"GET\">\n");

        printf("Date&nbsp;(yyyy-mm-dd)&nbsp;<input name=\"fDate\" type=\"text\" size=\"10\" maxlength=\"10\" value=\"");
        printf("%s\">&nbsp;&nbsp;&nbsp;\n", $_GET["fDate"]);

        printf("Search&nbsp;<A HREF=\"regexp.html\">regexp:</A>&nbsp;<input name=\"fRegexp\" type=\"text\" size=\"32\" maxlength=\"128\" value=\"");
        if (!empty($_GET["fRegexp"]))
            printf("%s", $_GET["fRegexp"]);
        printf("\">&nbsp;&nbsp;&nbsp;\n<P>\n");

        ini_set("mysql.trace_mode", "Off");
        ini_set("display_errors", "Off");

        printf("Night&nbsp;(yyyy-mm-dd)&nbsp;<input name=\"fNight\" type=\"text\" size=\"10\" maxlength=\"10\" value=\"");
        printf("%s\">&nbsp;&nbsp;&nbsp;\n", $_GET["fNight"]);


        printf("<input class='Width' type='submit' value='Query Table'>&nbsp;&nbsp;&nbsp;\n");
        printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"queryrbk.php\"'>&nbsp;&nbsp;&nbsp;\n");
        if (strchr($_SERVER["REQUEST_URI"], '?')!=FALSE)
        {
            printf("<input class='Width' type='button' value='Get .txt' onClick='self.location.href=\"%s&fSendTxt=1\"'>&nbsp;&nbsp;&nbsp;\n", $_SERVER["REQUEST_URI"]);
            printf("<input class='Width' type='button' value='Print' onClick='self.location.href=\"%s&amp;fPrintTable=1\"'>&nbsp;&nbsp;&nbsp;\n", htmlspecialchars($_SERVER["REQUEST_URI"]));
        }

        printf("</form>\n");
        printf("</center>\n");
        printf("</td>\n");
        printf("</tr>\n");
        printf("<tr class='Block'>\n");
        printf("<td>\n");
    }

    function PrintPage($html, $host, $user, $pw, $db)
    {
        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);

        $query0  = "SELECT SQL_CALC_FOUND_ROWS fRunBookDate AS Date, fRunBookText AS Entry ";
        $query0 .= "FROM " . $db . ".RunBook ";
        $query0 .= "WHERE fRunBookDate REGEXP '^" . $_GET["fDate"] . "' ";

        if (!empty($_GET["fRegexp"]))
            $query0 .= "AND fRunBookText REGEXP '" . $_GET["fRegexp"] . "' ";

        if (!empty($_GET["fNight"]))
            $query0 .= "AND fRunBookDate < '" . $_GET["fNight"] . " 13:00:00' AND fRunBookDate > ADDDATE( '" . $_GET["fNight"] . " 13:00:00', INTERVAL -1 DAY) ";

        $query0 .= "ORDER BY fRunBookDate";

        $result0 = mysql_query($query0, $db_id);
        $result1 = mysql_query("SELECT FOUND_ROWS()", $db_id);

        if ($result0)
        {
            if ($html=="1" || $html=="2")
            {
                $alias = array
                (
                 "fRunBookDate"  => "Date",
                 "fRunBookText"  => "Entry"
                );
                $rightalign = array
                (
                );
                PrintMagicTable($result0, $alias, $rightalign, "", "", "", $result1);
            }
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

    ini_set("display_errors", "On");
    ini_set("mysql.trace_mode", "On");

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-type: application/octet");
        header("Content-Disposition: attachment; filename=query-result.txt");

        PrintPage("0", $host, $user, $pw, $db);
    }
    else
    {
        if (empty($_GET["fPrintTable"]))
            echo (file_get_contents("index-header.html"));

        $environment = sizeof($_GET);

        if (empty($_GET["fPrintTable"]))
            PrintForm($_GET);

        if ($environment==0)
            printf("No query submitted yet.<BR>");
        else
        {
            if (empty($_GET["fPrintTable"]))
                PrintPage("1", $host, $user, $pw, $db);
            else
                PrintPage("2", $host, $user, $pw, $db);
        }

        if (empty($_GET["fPrintTable"]))
            echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
