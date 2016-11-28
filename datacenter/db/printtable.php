<?php
{
    function PrintForm($_GET)
    {
        if (empty($_GET["fTable"]))
        {
            printf("ERROR - No table selected.\n");
            return;
        }
    }

    function PrintPage($html, $host, $user, $pw, $db, $limits, $rms)
    {
        if (empty($_GET["fTable"]))
        {
            printf("ERROR - No table selected.\n");
            return;

        }

        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);

        $fromtable=$_GET["fTable"];
        $query0  = "SELECT SQL_CALC_FOUND_ROWS * FROM " . $fromtable;
        if (!empty($_GET["fSortBy"]))
        {
            $val=substr($_GET["fSortBy"], 0, -1);
            $query0 .= " ORDER BY " . GetTable($fromtable,$val) . " ";
            if (substr($_GET["fSortBy"], -1)=="-")
                $query0 .= "DESC";
        }


        $result0 = mysql_query($query0, $db_id);
        $result1 = mysql_query("SELECT FOUND_ROWS()", $db_id);

        if ($result0)
        {
            if ($html=="1")
            {
                $alias = array
                (
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
    include ("magicdefs.php");

    ini_set("display_errors", "On");
    ini_set("mysql.trace_mode", "On");

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-type: application/octet");
        header("Content-Disposition: attachment; filename=query-result.txt");

        PrintPage("0", $host, $user, $pw, $db, "", "");
    }
    else
    {
        echo (file_get_contents("index-header.html"));

        $environment = sizeof($_GET);

        PrintForm($_GET);

        if ($environment==0)
            printf("No query submitted yet.<BR>");
        else
            PrintPage("1", $host, $user, $pw, $db, "", "");

        echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
