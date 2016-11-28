<?php
{
    function CreateQuery($_GET)
    {
        $query0  = "SELECT ";
        //datasets (0-3)
        $query0 .= " DataSets.fDataSetNumber, fDataSetName, fComment, ";//0,1,2
        $query0 .= " fSourceName, fObservationModeName, ";//3, 4
        //steps (5-8)
        $query0 .= " fDataSetInserted, fStarFilesAvail, ";//5,6
        $query0 .= " fGanymed, fFillGanymed, ";//7,8
        //ganymed (9-14)
        $query0 .= " fExcessEvents, fBackgroundEvents, fSignalEvents, ";//9,10,11
        $query0 .= " fSignificance, fScaleFactor, fEffOnTime/3600 ";//12,13, 14

        $query0 .= " FROM DataSets ";

        $query0 .= " LEFT JOIN Source USING(fSourceKEY) ";
        $query0 .= " LEFT JOIN ObservationMode USING(fObservationModeKEY) ";
        $query0 .= " LEFT JOIN DataSetProcessStatus USING(fDataSetNumber) ";
        $query0 .= " LEFT JOIN Ganymed USING(fDataSetNumber) ";

        if (!empty($_GET["fRunMin"]) && !empty($_GET["fRunMax"]))
            $query0 .= " WHERE DataSets.fDataSetNumber between " . $_GET["fRunMin"] . " and " . $_GET["fRunMax"] ;
        if (!empty($_GET["fDataSetNo"]))
        {
            if (strpos($query0, " WHERE ")==FALSE)
                $query0 .= " WHERE ";
            else
                $query0 .= " AND ";
            $query0 .= " DataSets.fDataSetNumber='" . $_GET["fDataSetNo"] . "'";
        }
        if (!empty($_GET["fSourceN"]))
        {
            if (strpos($query0, " WHERE ")==FALSE)
                $query0 .= " WHERE ";
            else
                $query0 .= " AND ";
            $query0 .= " fSourceName REGEXP \"^" . $_GET["fSourceN"] . "\" ";
        }
        $query0 .= " ORDER BY DataSets.fDataSetNumber ";

        return $query0;
    }

    function InitGet($_GET)
    {
        // Find out whether it is the first call to the php script
        $first = empty($_GET["fRunMin"]) && empty($_GET["fRunMax"]);

        if (empty($_GET["fNumResults"]))
            $_GET["fNumResults"]="20";

        if (empty($_GET["fAll"]))
            $_GET["fAll"]="Off";

        if (empty($_GET["fSequ"]))
            $_GET["fSequ"]="Off";

    }

    function PrintForm($_GET, $host, $user, $pw, $db)
    {
        printf("<center>\n");
        printf("<form action=\"ganymed.php\" METHOD=\"GET\">\n");

        printf("DataSet#&nbsp;<input name=\"fDataSetNo\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"");
        if (!empty($_GET["fDataSetNo"]))
            printf("%s", $_GET["fDataSetNo"]);
        printf("\">&nbsp;&nbsp;&nbsp;\n");

        if (empty($_GET["fRunMin"]))
//            $min = "100";
            $min = GetMin("fDataSetNumber", "DataSets", $host, $user, $pw, $db);
        else
            $min = $_GET["fRunMin"];

        if (empty($_GET["fRunMax"]))
            $max = GetMax("fDataSetNumber", "DataSets", $host, $user, $pw, $db);
        else
            $max = $_GET["fRunMax"];

        printf("DataSets&nbsp;from&nbsp;<input name=\"fRunMin\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\">\n", $min);
        printf("to&nbsp;<input name=\"fRunMax\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\">&nbsp;&nbsp;&nbsp;\n", $max);

        if ($_GET["fAll"]=="On")
            $checked = "checked";
        else
            $checked = "";
        printf("<input type=\"checkbox\" name=\"fAll\" value=\"On\" %s>plots\n", $checked);

        if ($_GET["fSequ"]=="On")
            $checked = "checked";
        else
            $checked = "";
        printf("<input type=\"checkbox\" name=\"fSequ\" value=\"On\" %s>sequences\n", $checked);

        printf(" <P>\n");

        printf("Source&nbsp;(<A HREF=\"regexp.html\">regexp</A>)&nbsp;<input name=\"fSourceN\" type=\"text\" size=\"15\" maxlength=\"15\" value=\"");
        if (!empty($_GET["fSourceN"]))
            printf("%s", $_GET["fSourceN"]);
        printf("\">&nbsp;&nbsp;&nbsp;\n");


        ini_set("mysql.trace_mode", "Off");
        ini_set("display_errors", "Off");

        printf("<input class='Width' type='submit' value='Query Table'>&nbsp;&nbsp;&nbsp;\n");
        printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"ganymed.php\"'>&nbsp;&nbsp;&nbsp;\n");
        if (strchr($_SERVER["REQUEST_URI"], '?')!=FALSE)
            printf("<input class='Width' type='button' value='Print' onClick='self.location.href=\"%s&fPrintTable=1\"'>&nbsp;&nbsp;&nbsp;\n", $_SERVER["REQUEST_URI"]);
        printf("</form>\n");
        printf("</center>\n");
        printf("</td>\n");
        printf("</tr>\n");
        printf("<tr class='Block'>\n");
        printf("<td>\n");
    }

    function GetStatus($step)
    {
        if (is_null($step))
            return "<font color='#FF0000'>not done</font>";
        else
        {
            if (strpos($step, "1970-01-01 00:00:00")==FALSE)
                return "done";
            else
                return "not to be done";
        }
        return "there is an error -> tell Dani";
    }

    function GetRanges($db_id, $sequences)
    {
        $query="SELECT Min(fZenithDistanceMin), Max(fZenithDistanceMax), Min(fRunStart), Max(fRunStart) FROM Sequences WHERE fSequenceFirst IN (";
        $sequlist=str_replace(" ", ",", trim($sequences));
//        printf("sequ-after: %s", $sequlist);
        $query .= $sequlist . ")";
        $result = mysql_query($query, $db_id);
//        printf("query: %s", $query);
        $zdmin=mysql_result($result, $i, 0);
        $zdmax=mysql_result($result, $i, 1);
        $starttime=str_replace("-", "/",substr(mysql_result($result, $i, 2),0,10));
        $stoptime=str_replace("-", "/",substr(mysql_result($result, $i, 3),0,10));
        printf("<br> Zd: %s - %s degree <br> Observationperiod: %s - %s ", $zdmin, $zdmax, $starttime, $stoptime);
    }

    function PrintDataSetInfo($db_id, $result0, $ganymedtabsw, $ganymedtabs)
    {
        $numres = mysql_num_rows($result0);

        printf("<center>");
        printf("<table BORDER=\"0\">\n");
        printf("<tr><td>");
        printf("# of DataSets: %d ", $numres);
        printf("</td></tr>");

        for ($i=0 ; $i<$numres ; $i++)
        {
            $datasetno=mysql_result($result0, $i, 0);
            $datasetname=mysql_result($result0, $i, 1);
            $comment=mysql_result($result0, $i, 2);
            $source=mysql_result($result0, $i, 3);
            $wobble=mysql_result($result0, $i, 4);
            $inserted=mysql_result($result0, $i, 5);
            $filesavail=mysql_result($result0, $i, 6);
            $ganymed=mysql_result($result0, $i, 7);
            $fillganymed=mysql_result($result0, $i, 8);
            $excess=mysql_result($result0, $i, 9);
            $bg=mysql_result($result0, $i, 10);
            $signal=mysql_result($result0, $i, 11);
            $signif=mysql_result($result0, $i, 12);
            $scale=mysql_result($result0, $i, 13);
            $effontime=mysql_result($result0, $i, 14);
            $num=sprintf("%08d",$datasetno);
            $num2=substr($num,0,5);
            $datasetfile="http://www.astro.uni-wuerzburg.de/datacenter/datasets/" . $num2 . "/dataset" . $num . ".txt";

            printf("<tr><td>");

            printf(" <table BORDER=\"1\">");
            printf(" <tr BGCOLOR='#C0C0C0'>\n");
            printf(" <th colspan=\"6\"><big>Source: <big>%s </big></big>(DataSet#:", $source, $datasetno);
            printf(" <a href=\"%s\">%s</a>) </th>", $datasetfile, $datasetno);
            printf(" </tr><tr BGCOLOR='#E0E0E0' ALIGN='left'>\n");
            printf(" <th colspan=\"6\"> <b>%s Mode</b>, %s ", $wobble, $comment);
            printf(" </th></tr><tr>\n");

            printf(" <td colspan=\"6\"> DatasetName: %s", $datasetname);
            if (!is_null($ganymed))
                printf(", <font color='green'> ganymed done</font> ");

            printf(" <tr>");

            if (!is_null($fillganymed))
            {
                printf("<td colspan=\"6\"></td></tr>\n");
                printf(" <tr BGCOLOR='#C0C0C0'><th colspan=\"6\">results ");
                printf(" (<a href=\"http://www.astro.uni-wuerzburg.de/datacenter/ganymed/%s/%s/\">plots</a>)",
                       $num2, $num);
                printf(" </th></tr><tr BGCOLOR='#E0E0E0'>\n");
                printf(" <td>excess events</td>\n");
                printf(" <td>background events</td>\n");
                printf(" <td>signal events</td>\n");
                printf(" <td>significance</td>\n");
                printf(" <td>scale factor</td>\n");
                printf(" <td>effective ontime</td>\n");
                printf(" </tr><tr BGCOLOR='#D0D0D0' align='right'>");
                printf(" <td>%s</td>\n", $excess);
                printf(" <td>%s</td>\n", $bg);
                printf(" <td>%s</td>\n", $signal);
                printf(" <td>%s</td>\n", $signif);
                printf(" <td>%s</td>\n", $scale);
                printf(" <td>%s h</td>\n", $effontime);
            }

            if ($_GET["fSequ"]=="On")
            {
                $dataset=file_get_contents($datasetfile);
                $onpos=strpos($dataset, "SequencesOn:");
                $offpos=strpos($dataset, "SequencesOff:");
                $possource=strpos($dataset, "SourceName:");
                $sequoff="";
                if (!empty($offpos))
                    $sequoff=substr($dataset, $offpos+13, $possource-($offpos+13));
                if (empty($offpos))
                    $offpos=$possource;
                $sequon=substr($dataset, $onpos+12, $offpos-($onpos+12));

                if (!empty($sequon))
                {
                    printf("<tr><td colspan=\"6\">\n SequencesOn: ");
                    $sequences=split(" ", trim($sequon));
                    foreach($sequences as $key => $sequ)
                        printf("<a href=\"sequence.php?fSequenceNo=%s&fAll=On\">%s</a> ", $sequ, $sequ);
                    GetRanges($db_id, $sequon);
                    printf("</td></tr>");

                    if (!empty($sequoff))
                    {
                        printf("<tr><td colspan=\"6\">\n SequencesOff: ");
                        $sequences=split(" ", trim($sequoff));
                        foreach($sequences as $key => $sequ)
                            printf("<a href=\"sequence.php?fSequenceNo=%s&fAll=On\">%s</a> ", $sequ, $sequ);
                        GetRanges($db_id, $sequoff);
                        printf("</td></tr>");
                    }
                }
            }
            printf("<tr><td colspan=\"6\">\n Plots: ");

            if ($wobble=='Wobble')
                $tabs=$ganymedtabsw;
            else
                $tabs=$ganymedtabs;
            foreach($tabs as $key => $element)
                if (!$key==0)
                    printf("<a href=\"http://www.astro.uni-wuerzburg.de/datacenter/ganymed/%s/%s/ganymed%s-tab%s.png\">%s</a> ", $num2, $num, $num, $key, $element);

            printf("</td></tr>");

            if ($_GET["fAll"]=="On" && !is_null($ganymed))
            {
                //plotting Hist and FS
                printf("<tr><td colspan=\"6\" align='center'>\n");
                printf(" <img src=\"http://www.astro.uni-wuerzburg.de/datacenter/ganymed/%s/%s/ganymed%s-tab14.png\">",
                       $num2, $num, $num);
                printf(" <img src=\"http://www.astro.uni-wuerzburg.de/datacenter/ganymed/%s/%s/ganymed%s-tab13.png\">",
                       $num2, $num, $num);
                printf(" </td></tr>");
            }

            printf(" </table>");
            printf("</td></tr><tr><td><br></td></tr>");
        }
        printf("</table>\n");

        printf("</center>\n");
        printf("</tr><tr class='Block'><td>\n");
    }

    function PrintPage($html, $host, $user, $pw, $db, $ganymedtabsw, $ganymedtabs)
    {
        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);

        $query0 = CreateQuery($_GET);

        $result0 = mysql_query($query0, $db_id);

        if ($result0)
        {
            if ($html=="1" || $html=="2")
                PrintDataSetInfo($db_id, $result0, $ganymedtabsw, $ganymedtabs);
            else
                PrintText($result0);

            mysql_free_result($result0);
        }
        mysql_close($db_id);

        if ($html=="1")
            PrintSubmittedQuery($query0, $db, "old");
    }

    include ("include.php");
    include ("db.php");
    include ("tabs.php");

    ini_set("display_errors", "On");
    ini_set("mysql.trace_mode", "On");

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-type: application/octet");
        header("Content-Disposition: attachment; filename=query-result.txt");

        PrintPage("0", $host, $user, $pw, $db, $ganymedtabsw, $ganymedtabs);
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
                PrintPage("1", $host, $user, $pw, $db, $ganymedtabsw, $ganymedtabs);
            else
                PrintPage("2", $host, $user, $pw, $db, $ganymedtabsw, $ganymedtabs);
        }

        if (empty($_GET["fPrintTable"]))
            echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
