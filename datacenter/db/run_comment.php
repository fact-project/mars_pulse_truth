<?php
{
    ini_set("display_errors", "On");
    ini_set("mysql.trace_mode", "On");

    if (empty($_GET["fMode"]))
    {
        $_GET["fMode"]="view";
        $sign=strpos($_SERVER['REQUEST_URI'], "?")==false ? "?" : "&";
        $_SERVER['REQUEST_URI']=$_SERVER['REQUEST_URI'] . $sign . "fMode=view";
    }

    if (empty($_GET["fRunID"]))
        $_GET["fRunID"]="all";

    if (empty($_GET["fSequenceID"]))
        $_GET["fSequenceID"]="all";

    if (empty($_GET["fTable"]))
    {
        $_GET["fTable"]="RunComments";
//        $_GET["fTable"]="SequenceComments";
        $sign=strpos($_SERVER['REQUEST_URI'], "?")==false ? "?" : "&";
        $_SERVER['REQUEST_URI']=$_SERVER['REQUEST_URI'] . $sign . "fTable=RunComments";
    }

    if (empty($_GET["fNight"]))
        $_GET["fNight"]=date("Ymd", time()-(12*60*60));

    if (strcmp($_GET["fMode"], "tooltip")!=0)
        echo (file_get_contents("index-header.html"));


    //
    // insert/update part
    //
    include ("ldap_include.php");
    if (strcmp($_GET["fMode"], "insert")==0 || strcmp($_GET["fMode"], "update")==0)
    {
        if (!isset($_SERVER['PHP_AUTH_USER']))
        {
            header('WWW-Authenticate: Basic realm="Edit Run Comments"');
            header('HTTP/1.0 401 Unauthorized');
            return;
        }
        else
        {
            //echo "The password is not yet evaluated, but the username is inserted into the DB.<br>";
            if (!CheckUsernameAndPassword($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW'], GetLDAPOptions()))
                return;
        }
        if (!empty($_GET["fComment"]))
        {
            include("db2.php");
            $db_id = mysql_pconnect($host, $user, $pw);
            if ($db_id==FALSE)
            {
                printf("mysql_connect returned the following error: %s\n", mysql_error());
                die("");
            }
            mysql_select_db($db);
            //for insert
            if (strcmp($_GET["fMode"], "insert")==0)
                $query0 = "INSERT " . $_GET["fTable"] . " SET ";
            //for update
            if (strcmp($_GET["fMode"], "update")==0)
                $query0 = "UPDATE " . $_GET["fTable"] . " SET ";
            //for both
            $query0.= " fComment='".str_replace("'", "\'", $_GET["fComment"])."'";
            $query0.= ", fUser='".$_SERVER['PHP_AUTH_USER']."'";
            //for insert
            if (strcmp($_GET["fMode"], "insert")==0)
            {
                $query0.=", fNight=".$_GET["fNight"];
                if (strcmp($_GET["fTable"], "RunComments")==0)
                    $query0.= ", fRunID=".$_GET["fRunID"];
                if (strcmp($_GET["fTable"], "SequenceComments")==0)
                    $query0.= ", fSequenceID=".$_GET["fSequenceID"];
            }
            //for update
            if (strcmp($_GET["fMode"], "update")==0)
            {
                $query2="SELECT fCommentKEY FROM " . $_GET["fTable"];
                $query2.=" WHERE fNight=".$_GET["fNight"];
                if (strcmp($_GET["fTable"], "RunComments")==0)
                    $query2.=" AND fRunID=".$_GET["fRunID"];
                if (strcmp($_GET["fTable"], "SequenceComments")==0)
                    $query2.=" AND fSequenceID=".$_GET["fSequenceID"];
                $query2.=" AND fComment='".str_replace("'", "\'", $_GET["fOldComment"])."'";
                $result2=mysql_query($query2, $db_id);
                $row2 = mysql_fetch_row($result2);
                $commentkey=$row2[0];
                mysql_free_result($result2);
                $query0.=" WHERE fCommentKEY=".$commentkey;
            }
            //echo "insert query: " . $query0 . "<br>";
            $result0=mysql_query($query0, $db_id);
            mysql_close($db_id);
        }
    }



    //
    // display part
    //
    include("db.php");
    if (strcmp($_GET["fMode"], "tooltip")!=0)
    {
        //mode
        printf("With this page, you can \n");
        //view mode
        if (strcmp($_GET["fMode"], "view")==0)
            printf("<b>view</b>,\n");
        else
            printf("<a href='%s'>view</a>,\n", str_replace($_GET["fMode"], 'view', $_SERVER['REQUEST_URI']));
        //update mode
        if (strcmp($_GET["fMode"], "update")==0)
            printf("<b>update</b> and\n");
        else
            printf("<a href='%s'>update</a> and \n", str_replace($_GET["fMode"], 'update', $_SERVER['REQUEST_URI']));
        //insert mode
        if (strcmp($_GET["fMode"], "insert")==0)
            printf("<b>insert</b>\n");
        else
            printf("<a href='%s'>insert</a> \n", str_replace($_GET["fMode"], 'insert', $_SERVER['REQUEST_URI']));

        //table
        printf("entries in the tables <b>%s</b> \n", $_GET["fTable"]);
        if (strcmp($_GET["fTable"], "RunComments")==0)
            printf(" and <a href='%s'>SequenceComments</a> \n", str_replace($_GET["fTable"], 'SequenceComments', $_SERVER['REQUEST_URI']));
        else
            printf(" and <a href='%s'>RunComments</a> \n", str_replace($_GET["fTable"], 'RunComments', $_SERVER['REQUEST_URI']));
        //database
        printf("from the DB %s.<br><br>\n", $db);

        //insert field for run/sequence number
        printf("<form action=\"run_comment.php\" METHOD=\"GET\">\n");
        if (strcmp($_GET["fTable"], "RunComments")==0)
        {
            printf("Run#&nbsp;(YYYYMMDD_FFF)&nbsp;<input title='Enter here Night in the format YYYYMMDD or \"all\" to query all comments.' name=\"fNight\" type=\"text\" size=\"8\" maxlength=\"8\" value=\"%s\">\n", $_GET["fNight"]);
            printf("_ <input title='Enter here RunID in the format FFF or \"all\" (only viewing mode) to query all comments of one night.' name=\"fRunID\" type=\"text\" size=\"3\" maxlength=\"3\" value=\"%s\">\n", $_GET["fRunID"]);
        }
        if (strcmp($_GET["fTable"], "SequenceComments")==0)
        {
            printf("Sequence#&nbsp;(YYYYMMDD_FFF)&nbsp;<input title='Enter here Night in the format YYYYMMDD or \"all\" to query all comments.' name=\"fNight\" type=\"text\" size=\"8\" maxlength=\"8\" value=\"%s\">\n", $_GET["fNight"]);
            printf("_ <input title='Enter here SequenceID in the format FFF or \"all\" (only viewing mode) to query all comments of one night.' name=\"fSequenceID\" type=\"text\" size=\"3\" maxlength=\"3\" value=\"%s\">\n", $_GET["fSequenceID"]);
        }
        printf("<input type='hidden' name='fMode' value='%s'>", $_GET["fMode"]);
        printf("<input type='hidden' name='fTable' value='%s'>", $_GET["fTable"]);
        printf("<input type='submit' value='Get Comments'><br><br>\n");
        printf("</form>\n");
    }

    if (!(preg_match("/20[0-9][0-9][01][0-9][0-3][0-9]/",$_GET["fNight"])
          || (preg_match("/all/",$_GET["fNight"]) && strcmp($_GET["fMode"], "view")==0)))
    {
        printf("-%s-<br>", $_GET["fMode"]);
        printf("Please enter a valid number for the Night (format: YYYYMMDD).");
        return;
    }

    if (strcmp($_GET["fTable"], "RunComments")==0 &&
        (!(preg_match("/[0-9][0-9]?[0-9]?/",$_GET["fRunID"])
           || (preg_match("/all/",$_GET["fRunID"]) && strcmp($_GET["fMode"], "view")==0))))
    {
        printf("Please enter a valid number for the RunID (format: FFF, e.g. 001).");
        return;
    }

    if (strcmp($_GET["fTable"], "SequenceComments")==0 &&
        (!(preg_match("/[0-9][0-9]?[0-9]?/",$_GET["fSequenceID"])
          || (preg_match("/all/",$_GET["fSequenceID"]) && strcmp($_GET["fMode"], "view")==0))))
    {
        printf("Please enter a valid number for the SequenceID (format: FFF, e.g. 001).");
        return;
    }

    $db_id = mysql_pconnect($host, $user, $pw);
    if ($db_id==FALSE)
    {
        printf("mysql_connect returned the following error: %s\n", mysql_error());
        die("");
    }
    mysql_select_db($db);

    if (strcmp($_GET["fTable"], "RunComments")==0)
        $query1 = "SELECT CONCAT(fNight,'_', LPAD(fRunID, 3, 0))";
    if (strcmp($_GET["fTable"], "SequenceComments")==0)
        $query1 = "SELECT CONCAT(fNight,'_', LPAD(fSequenceID, 3, 0))";
    $query1 .= ", fComment FROM " . $_GET["fTable"] . "  ";

    if (strcmp($_GET["fNight"], "all")!=0)
    {
        $query1 .= "WHERE fNight=".$_GET["fNight"];
        if (strcmp($_GET["fTable"], "RunComments")==0)
            if (strcmp($_GET["fRunID"], "all")!=0)
                $query1 .=" AND fRunID=".$_GET["fRunID"];
        if (strcmp($_GET["fTable"], "SequenceComments")==0)
            if (strcmp($_GET["fSequenceID"], "all")!=0)
                $query1 .=" AND fSequenceID=".$_GET["fSequenceID"];
    }
    if (strcmp($_GET["fTable"], "RunComments")==0)
        $query1 .= " ORDER BY fNight, fRunID, fCommentKEY ";
    if (strcmp($_GET["fTable"], "SequenceComments")==0)
        $query1 .= " ORDER BY fNight, fSequenceID, fCommentKEY ";

    //printf("Query: %s <br>\n\n", $query1);
    $result1=mysql_query($query1, $db_id);
    printf("Found %d comment(s).\n\n", mysql_num_rows($result1));
    if (strcmp($_GET["fMode"], "tooltip")!=0)
    {
        printf("<br>\n");
        printf("<table BORDER='0' style='margin-top:1ex'>");
        $col   = FALSE;
        printf("<tr BGCOLOR='#C0C0C0'><td>Run#</td><td>Comment</td></tr>");
    }
    while ($row1 = mysql_fetch_row($result1))
    {
        if (strcmp($_GET["fMode"], "tooltip")!=0)
        {
            if (!$col)
                printf("<tr BGCOLOR='#E0E0E0'>\n");
            else
                printf("<tr BGCOLOR='#D0D0D0'>\n");
            $col = !$col;

            if (strcmp($_GET["fMode"], "update")==0)
                printf("<form style='display:inline' action=\"run_comment.php\" METHOD=\"GET\">\n");
            printf("<td>\n%s\n</td>\n", $row1[0]);
            if (strcmp($_GET["fMode"], "update")==0)
            {
                printf("<td>\n");
                printf("<input type='text' size='50' maxlength='255' name='fComment' value='%s'>\n", htmlspecialchars($row1[1],ENT_QUOTES));
                printf("<input type='hidden' name='fOldComment' value='%s'>\n", htmlspecialchars($row1[1],ENT_QUOTES));
                printf("<input type='hidden' name='fMode' value='%s'>\n", $_GET["fMode"]);
                printf("<input type='hidden' name='fNight' value='%s'>\n", $_GET["fNight"]);
                printf("<input type='hidden' name='fRunID' value='%s'>\n", $_GET["fRunID"]);
                printf("<input type='hidden' name='fSequenceID' value='%s'>\n", $_GET["fSequenceID"]);
                printf("<input type='hidden' name='fTable' value='%s'>\n", $_GET["fTable"]);
                printf("<input type='submit' value='Update Comment'>\n");
                printf("</td>\n");
                printf("</form>\n");
            }
            else
                printf("<td>\n%s\n</td>\n", $row1[1]);
            printf("</tr>\n");
        }
        else
        {
            if (strcmp($_GET["fTable"], "RunComments")==0)
                printf("%s_%03d: %s\n", $_GET["fNight"], $_GET["fRunID"], $row1[1]);
            if (strcmp($_GET["fTable"], "SequenceComments")==0)
                printf("%s_%03d: %s\n", $_GET["fNight"], $_GET["fSequenceID"], $row1[1]);
        }
    }
    //insert mode
    if (strcmp($_GET["fMode"], "insert")==0)
    {
        printf("<tr BGCOLOR='#C0C0C0'>");
        if (strcmp($_GET["fTable"], "RunComments")==0)
            printf("<td>%s_%s</td>\n", $_GET["fNight"], $_GET["fRunID"]);
        if (strcmp($_GET["fTable"], "SequenceComments")==0)
            printf("<td>%s_%s</td>\n", $_GET["fNight"], $_GET["fSequenceID"]);
        printf("<td>\n");
        printf("<form style='display:inline' action=\"run_comment.php\" METHOD=\"GET\">\n");
        printf("<input type='text' size='50' maxlength='255' name='fComment'>\n");
        printf("<input type='hidden' name='fMode' value='%s'>\n", $_GET["fMode"]);
        printf("<input type='hidden' name='fNight' value='%s'>\n", $_GET["fNight"]);
        printf("<input type='hidden' name='fRunID' value='%s'>\n", $_GET["fRunID"]);
        printf("<input type='hidden' name='fSequenceID' value='%s'>\n", $_GET["fSequenceID"]);
        printf("<input type='hidden' name='fTable' value='%s'>\n", $_GET["fTable"]);
        printf("<input type='submit' value='Insert Comment'>\n");
        printf("</form>\n");
        printf("</td>\n");
        printf("</tr>\n");
    }
    if (strcmp($_GET["fMode"], "tooltip")!=0)
        printf("</table>");

    if (strcmp($_GET["fMode"], "update")==0)
        printf("Remark: You can update only one comment at once.");

    mysql_free_result($result1);
    mysql_close($db_id);

    if (strcmp($_GET["fMode"], "tooltip")!=0)
        echo (file_get_contents("index-footer.html"));

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
