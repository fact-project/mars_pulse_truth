<?php
{
    function PrintPage($host, $user, $pw, $db)
    {
        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);

        $query1="SELECT fMarsVersionName, fStartDate FROM MarsVersion ORDER BY fMarsVersion DESC LIMIT 0,1";
        $marsresult=mysql_query($query1);
        $row = mysql_fetch_row($marsresult);
        $mars=$row[0];
        $date=$row[1];
        if (mysql_errno()>0)
            printf("&nbsp; Error-Number: %s <br>", mysql_errno());
        if (mysql_error()!="")
            printf("&nbsp; Error-Message: %s <br>", mysql_error());
        if (mysql_info()!="")
            printf("&nbsp; Mysql-Info: %s <br>", mysql_info());
        mysql_free_result($marsresult);
        if (empty($mars))
        {
            printf("Could not get information for latest Mars version from the database.");
            return;
        }

        if (empty($_GET["insert"]))
        {
            $query0  = "SELECT SequenceProcessStatus.fSequenceFirst, ";
//            $query0 .= "if (fCallisto " . " < '" . $date . "', 'to be resetted', ";
            $query0 .= "if (fCallisto " . " < '" . $date . "', CONCAT('outdated (', (SELECT fMarsVersionName FROM MarsVersion WHERE fCallisto > fStartDate ORDER BY fMarsVersion DESC LIMIT 0,1), ')'), ";
            $query0 .= "if (not isnull(fCallisto), 'up to date (" . $mars . ")', ";
            $query0 .= "if (fStartTime > ADDDATE(Now(), INTERVAL -1 DAY) AND isnull(fFailedTime), 'running (" . $mars . ")',";
            $query0 .= "if (isnull(fFailedTime), 'already resetted', ";
            $query0 .= " 'crashed or failed')))) as 'calstat', ";
//            $query0 .= "if (fStar " . " < '" . $date . "', 'to be resetted', ";
            $query0 .= "if (fStar " . " < '" . $date . "', CONCAT('outdated (', (SELECT fMarsVersionName FROM MarsVersion WHERE fStar > fStartDate ORDER BY fMarsVersion DESC LIMIT 0,1), ')'), ";
            $query0 .= "if (not isnull(fStar), 'up to date (" . $mars . ")', ";
            $query0 .= "if (fStartTime > ADDDATE(Now(), INTERVAL -1 DAY) AND isnull(fFailedTime) AND not isnull(fCallisto), 'running (" . $mars . ")',";
            $query0 .= "if (isnull(fFailedTime), 'already resetted', ";
            $query0 .= " 'crashed or failed')))) as 'starstat', fProjectName, fRunStart ";
            $query0 .= "FROM SequenceProcessStatus ";
            $query0 .= "LEFT JOIN Sequences USING(fSequenceFirst,fTelescopeNumber) ";
            $query0 .= "LEFT JOIN Project USING(fProjectKEY) ";
        }
        else
        {
            $query0 = "UPDATE SequenceProcessStatus SET ";
            if ($_GET["step"]=="callisto")
                $query0 .="fCallisto=NULL, fFillCallisto=NULL, fStar=NULL, fFillStar=NULL, ";
            else
                if ($_GET["step"]=="star")
                    $query0 .="fStar=NULL, fFillStar=NULL, ";
            $query0 .="fStartTime=NULL, fFailedTime=NULL, fProgramId=NULL, fReturnCode=NULL ";

            if ($_GET["step"]=="callisto")
                $query0 .= " WHERE fCallisto " . " < '" . $date . "'";
            else
                if ($_GET["step"]=="star")
                    $query0 .= " WHERE fStar " . " < '" . $date . "'";
                else
                    $query0 .= " WHERE (NOT ISNULL(fFailedTime) OR fStartTime < ADDDATE(Now(), INTERVAL -1 DAY))";
        }
        if (ereg("^([ ]*[0-9]{5,8}[ ]*[,])*[ ]*[0-9]{5,8}[ ]*$",$_GET["Seqs"]))
        {
            if (strpos($query0, " WHERE ")==FALSE || (strpos($query0, " CONCAT(")==TRUE && strpos($query0, " WHERE ")==TRUE))
                $query0 .= " WHERE ";
            else
                $query0 .= " AND ";
            $query0 .=" SequenceProcessStatus.fSequenceFirst IN (" . $_GET["Seqs"] . ")";
        }
        else
        {
            echo "You have to give the sequences, which you want to reset, in the correct format, i.e. sequence numbers separated by ','.<br>";
            return;
        }
        if (empty($_GET["step"]))
        {
            echo "Please choose what you want to reset: <br>";
            echo "crashed processes (i.e. running longer than 1 day or failed), callisto or star.<br>";
            return;
        }

        $result0=mysql_query($query0);
        if (mysql_errno()>0)
            printf("&nbsp; Errorno: %s <br>", mysql_errno());
        if (mysql_error()!="")
            printf("&nbsp; Errormsg: %s <br>", mysql_error());
        if (!empty($_GET["insert"]) && !empty($_GET["step"]))
        {
            printf("%s Sequences have been resetted. <br><br>\n", mysql_affected_rows());
            if (mysql_info()!="")
                printf("Mysql-Info: %s <br><br>\n", mysql_info());
        }
        else
        {
            if (mysql_affected_rows()==0)
            {
                echo "No sequence to reset.<br>";
                echo "The sequences you listed are already resetted.<br>";
                mysql_free_result($result0);
                return;
            }
//            echo "The status of the sequences you entered: <br>\n";
            $star="";
            $callisto="";
            $crashed="";
            $calcolor='black';
            $starcolor='black';
            $style='normal';
            $col   = FALSE;
            $bgcolor='#C0C0C0';
            $checked=0;
            printf("<table border='0' align='center'>\n");
            printf("<tr bgcolor='%s'>\n<th style='padding-left:1ex;padding-right:1ex'>\n<u>Sequence</u>\n</th>\n", $bgcolor);
            printf("\n<th>\n<u>ProjectName</u>\n</th>\n");
            printf("<th>\n<u>Status Callisto</u>\n</th>");
            printf("\n<th>\n<u>Status Star</u>\n</th>\n");
            printf("\n<th>\n<u>Starttime</u>\n</th>\n");
            printf("</tr>\n");
            while ($row = mysql_fetch_row($result0))
            {
                if (ereg('outdated', $row[2]) && $_GET["step"]=='star')
                {
                    $star[]=$row[0];
                    $starcolor='green';
                    $style='bold';
                    $row[2]=str_replace('outdated', 'marked for reset', $row[2]);
                }
                if (ereg('outdated', $row[1]) && $_GET["step"]=='callisto')
                {
                    $callisto[]=$row[0];
                    $calcolor='green';
                    $starcolor='green';
                    $style='bold';
                }
                if (($row[1]=='crashed or failed' || $row[2]=='crashed or failed') && $_GET["step"]=='crashed')
                {
                    $crashed[]=$row[0];
                    if ($row[1]=='crashed or failed')
                    {
                        $calcolor='green';
                        $style='bold';
                    }
                    else
                    {
                        $starcolor='green';
                        $style='bold';
                    }
                }
                if (!$col)
                    $bgcolor='#E0E0E0';
                else
                    $bgcolor='#D0D0D0';
                $col = !$col;
                printf("<tr bgcolor='%s'>\n<td align='center' style='font-weight:%s'>\n%s \n</td>\n", $bgcolor, $style, $row[0]);
                printf("<td style='padding-left:1ex;padding-right:1ex'>\n %s \n</td>\n", $row[3]);
                printf("<td style='padding-left:1ex;padding-right:1ex'>\n <font color='%s'>%s </font>\n</td>\n", $calcolor, $row[1]);
                printf("<td style='padding-left:1ex;padding-right:1ex'>\n <font color='%s'>%s </font>\n</td>\n", $starcolor, $row[2]);
                printf("<td style='padding-left:1ex;padding-right:1ex'>\n %s \n</td>\n", $row[4]);
                printf("</tr>\n");
                $calcolor='black';
                $starcolor='black';
                $style='normal';
                $checked+=1;
            }
            printf("</table>\n");

            if ($_GET["step"]=="crashed")
                printf("<br>\n You chose to reset only the sequences which are crashed or failed.<br>\n");
            else
                printf("<br>\n Only the sequences which are not 'up to date', 'running' or 'resetted', can be resetted.<br>\n");
            if (empty($star) && empty($callisto) && empty($crashed))
                echo "With your choice no sequences would be resetted.";
            else
                echo "With your choice the following sequences would be resetted:\n ";
            $marked=0;
            if (!empty($star))
            {
                foreach($star as $num => $seq)
                    echo $seq . " ";
                $marked+=count($star);
            }
            if (!empty($callisto))
            {
                foreach($callisto as $num => $seq)
                    echo $seq . " ";
                $marked+=count($callisto);
            }
            if (!empty($crashed))
            {
                foreach($crashed as $num => $seq)
                    echo $seq . " ";
                $marked+=count($crashed);
            }
            printf("<br>\n Listed: %d <br> Marked: %d", $checked, $marked);
            printf("<br>\n<br>\nTo <b>reset</b> the <font color='green'>green marked entries</font>, please press the button 'Reset Sequences'.<br><br>\n");

            mysql_free_result($result0);
            printf("<input type='button' value='Reset Sequences' onClick='self.location.href=\"%s&amp;insert=On\"'>&nbsp;&nbsp;&nbsp;\n", htmlspecialchars($_SERVER["REQUEST_URI"]));

        }
        mysql_close($db_id);

//        PrintSubmittedQuery($query0, "1", $db, "old");

    }

    include ("include.php");
    include ("menu.php");
    include ("db.php");
    include ("magicdefs.php");

    ini_set("display_errors", "On");
    ini_set("mysql.trace_mode", "On");

    echo (file_get_contents("index-header.html"));

    $ips=array("132.187.47.10", //Thomas
               "129.194.169.21" //Dani ISDC
              );
    if (!in_array($_SERVER['REMOTE_ADDR'], $ips))
    {
        echo "You are not allowed to reset sequences.<br>";
        echo "If you want to reset your sequences in the automatic analysis, please contact Daniela Dorner.<br>";
        echo (file_get_contents("index-footer.html"));
        return;
    }

    $environment = sizeof($_GET);

    //init
    if (empty($_GET["Seqs"]))
        $_GET["Seqs"]="";
    if (empty($_GET["step"]))
        $_GET["step"]="";

    //print form
    printf("<center>\n");
    printf("<form action=\"resetseq.php\" METHOD=\"GET\">\n");

    printf("<input type='radio' name='step' value='crashed' %s> crashed processes &nbsp;", $_GET["step"]=="crashed" ? "checked" : "");
    printf("<input type='radio' name='step' value='callisto' %s> callisto &nbsp;", $_GET["step"]=="callisto" ? "checked" : "");
    printf("<input type='radio' name='step' value='star' %s> star<br>", $_GET["step"]=="star" ? "checked" : "");

    printf("<table>\n<tr>\n<td align='left'>\nSequences: \n</td>\n</tr>\n<tr>\n<td>\n");
    printf("<input name='Seqs' type='text' size='80' maxlength='300' value='%s'>\n</td>\n</tr>\n</table>\n<br>\n", $_GET["Seqs"]);

    ini_set("mysql.trace_mode", "Off");
//    ini_set("display_errors", "Off");

    printf("<input type='submit' value='Check Sequence Status'>&nbsp;&nbsp;&nbsp;\n");
    printf("<input class='Width' type='button' value='Clear Form' onClick='self.location.href=\"resetseq.php\"'>&nbsp;&nbsp;&nbsp;\n");
//    if (!empty($_GET["step"]) && ereg("^([ ]*[0-9]{5,8}[ ]*[,])*[ ]*[0-9]{5,8}[ ]*$",$_GET["Seqs"]))
//        printf("<input type='checkbox' name='insert' value='On'>insert\n");
    printf("<br>\n");
    printf("</form>\n");
    printf("</center>\n");
    printf("</td>\n</tr>\n");
    printf("<tr class='Block'>\n<td>\n");

    if ($environment==0)
    {
        printf("No query submitted yet.<br>\n");
        printf("To reset sequences, please insert them in the following format: <i>12345,87654321</i><br>\n");
        printf("and choose whether you want to reset just crashed processes, callisto (this includes star) or just star.<br>\n");
        printf("You can enter up to 300 characters in the sequences field.<br>\n");
    }
    else
        PrintPage($host, $user, $pw, $db);

    echo (file_get_contents("index-footer.html"));

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
