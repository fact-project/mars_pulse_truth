<?php
{
    function CreateQuery($_GET)
    {
        $query0  = "SELECT ";
        //main (0-18)
        $query0 .= " Sequences.fSequenceFirst, fSequenceLast, ";//0,1
        $query0 .= " fSourceName, fProjectName, ";//2,3
        $query0 .= " fRunStart, fRunTime, fNumEvents, ";//4,5,6
        $query0 .= " fZenithDistanceMin, fZenithDistanceMax, ";//7,8
        $query0 .= " fAzimuthMin, fAzimuthMax, ";//9,10
        $query0 .= " fHvSettingsName, fDiscriminatorThresholdTableName, ";//11,12
        $query0 .= " fL1TriggerTableName, fL2TriggerTableName, ";//13,14
        $query0 .= " fTriggerDelayTableName, fTestFlagName, ";//15,16
        $query0 .= " fLightConditionsName, fManuallyChangedName, ";//17,18
        //steps (19-24)
        $query0 .= " fSequenceFileWritten, fAllFilesAvail, ";//19,20
        $query0 .= " fCallisto, fFillCallisto, ";//21,22
        $query0 .= " fStar, fFillStar, ";//23,24
        //calibration (25-42)
        $query0 .= " fUnsuitableInner, fUnsuitableOuter, ";//25,26
        $query0 .= " fUnreliableInner, fUnreliableOuter, ";//27,28
        $query0 .= " fIsolatedInner, fIsolatedOuter, fIsolatedMaxCluster, ";//29,30,31
        $query0 .= " fArrTimeMeanInner, fArrTimeMeanOuter, ";//32,33
        $query0 .= " fArrTimeRmsInner, fArrTimeRmsOuter, ";//34,35
        $query0 .= " fConvFactorInner, fConvFactorOuter, ";//36,37
        $query0 .= " fMeanPedRmsInner, fMeanPedRmsOuter, ";//38,39
        $query0 .= " fMeanSignalInner, fMeanSignalOuter, ";//40,41
        $query0 .= " fPulsePosMean, ";//42
        //myon,star (43-51)
        $query0 .= " fPSF, fRatio, fMuonRate, ";//43,44,45
        $query0 .= " fMuonNumber, fEffOnTime, fDataRate, ";//46,47,48
        $query0 .= " fMaxHumidity, fMeanNumberIslands, fInhomogeneity, fEffOnTime/fRunTime, ";//49,50,51,52
        $query0 .= " CONCAT('<a href=\"http://www.astro.uni-wuerzburg.de/datacenter/callisto/' , LEFT(LPAD(CONCAT(Sequences.fSequenceFirst, '.'), 9,'0'),4), '/', LPAD(CONCAT(Sequences.fSequenceFirst, '/'), 9,'0'), 'callisto', LPAD(CONCAT(Sequences.fSequenceFirst , '.html'), 13,'0') , '\">cal-log</A>') as 'CalLink', ";//53
        $query0 .= " CONCAT('<a href=\"http://www.astro.uni-wuerzburg.de/datacenter/star/' , LEFT(LPAD(CONCAT(Sequences.fSequenceFirst, '.'), 9,'0'),4), '/', LPAD(CONCAT(Sequences.fSequenceFirst, '/'), 9,'0'), 'star', LPAD(CONCAT(Sequences.fSequenceFirst , '.html'), 13,'0') , '\">star-log</A>') as 'StarLink', ";//54
        $query0 .= " CONCAT('<a href=\"http://www.astro.uni-wuerzburg.de/datacenter/sequences/' , LEFT(LPAD(CONCAT(Sequences.fSequenceFirst, '.'), 9,'0'),4), '/sequence', LPAD(CONCAT(Sequences.fSequenceFirst , '.txt'), 12,'0') , '\">', 'sequ-file</a>') as 'SequLink' ";//55

        $query0 .= " FROM Sequences ";

        $query0 .= " LEFT JOIN Source USING(fSourceKEY) ";
        $query0 .= " LEFT JOIN Project USING(fProjectKEY) ";
        $query0 .= " LEFT JOIN HvSettings USING(fHvSettingsKEY) ";
        $query0 .= " LEFT JOIN DiscriminatorThresholdTable USING(fDiscriminatorThresholdTableKEY) ";
        $query0 .= " LEFT JOIN L1TriggerTable USING(fL1TriggerTableKEY) ";
        $query0 .= " LEFT JOIN L2TriggerTable USING(fL2TriggerTableKEY) ";
        $query0 .= " LEFT JOIN TriggerDelayTable USING(fTriggerDelayTableKEY) ";
        $query0 .= " LEFT JOIN TestFlag USING(fTestFlagKEY) ";
        $query0 .= " LEFT JOIN LightConditions USING(fLightConditionsKEY) ";
        $query0 .= " LEFT JOIN ManuallyChanged USING(fManuallyChangedKEY) ";
        $query0 .= " LEFT JOIN SequenceProcessStatus USING(fSequenceFirst,fTelescopeNumber) ";
        $query0 .= " LEFT JOIN Calibration USING(fSequenceFirst,fTelescopeNumber) ";
        $query0 .= " LEFT JOIN Star USING(fSequenceFirst,fTelescopeNumber) ";


        if (!empty($_GET["fRunMin"]) && !empty($_GET["fRunMax"]))
            $query0 .= " WHERE Sequences.fSequenceFirst between " . $_GET["fRunMin"] . " and " . $_GET["fRunMax"] ;
        if (!empty($_GET["fSequenceNo"]))
        {
            if (strpos($query0, " WHERE ")==FALSE)
                $query0 .= " WHERE ";
            else
                $query0 .= " AND ";
            $query0 .= " Sequences.fSequenceFirst='" . $_GET["fSequenceNo"] . "'";
        }
        if (!empty($_GET["fSourceN"]))
        {
            if (strpos($query0, " WHERE ")==FALSE)
                $query0 .= " WHERE ";
            else
                $query0 .= " AND ";
            $query0 .= " fSourceName REGEXP \"^" . $_GET["fSourceN"] . "\" ";
        }
        $query0 .= " ORDER BY Sequences.fSequenceFirst ";

        return $query0;
    }

    function InitGet($_GET)
    {
        // Find out whether it is the first call to the php script
        $first = empty($_GET["fRunMin"]) && empty($_GET["fRunMax"]);

        if (empty($_GET["fNumResults"]))
            $_GET["fNumResults"]="20";

        if (empty($_GET["fAll"]))
            $_GET["fAll"]=$first?"On":"";

    }

    function PrintForm($_GET, $host, $user, $pw, $db)
    {
        printf("<center>\n");
        printf("<form action=\"sequence.php\" METHOD=\"GET\">\n");

        printf("Sequ#&nbsp;<input name=\"fSequenceNo\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"");
        if (!empty($_GET["fSequenceNo"]))
            printf("%s", $_GET["fSequenceNo"]);
        printf("\">&nbsp;&nbsp;&nbsp;\n");
//        printf("Sequence&nbsp;&nbsp;<input name=\"fSequence\" type=\"text\" size=\"10\" maxlength=\"10\" value=\"\">&nbsp;&nbsp;\n");

        if (empty($_GET["fRunMin"]))
            $min = GetMin("fSequenceFirst", "Sequences", $host, $user, $pw, $db);
        else
            $min = $_GET["fRunMin"];

        if (empty($_GET["fRunMax"]))
            $max = GetMax("fSequenceFirst", "Sequences", $host, $user, $pw, $db);
        else
            $max = $_GET["fRunMax"];

        printf("Sequences&nbsp;from&nbsp;<input name=\"fRunMin\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\">\n", $min);
        printf("to&nbsp;<input name=\"fRunMax\" type=\"text\" size=\"6\" maxlength=\"6\" value=\"%s\">&nbsp;&nbsp;&nbsp;\n", $max);

        if ($_GET["fAll"]=="On")
            $checked = "checked";
        else
            $checked = "";
        printf("<input type=\"checkbox\" name=\"fAll\" value=\"On\" %s>all information\n", $checked);

        printf(" <P>\n");

        printf("Source&nbsp;(<A HREF=\"regexp.html\">regexp</A>)&nbsp;<input name=\"fSourceN\" type=\"text\" size=\"15\" maxlength=\"15\" value=\"");
        if (!empty($_GET["fSourceN"]))
            printf("%s", $_GET["fSourceN"]);
        printf("\">&nbsp;&nbsp;&nbsp;\n");


        ini_set("mysql.trace_mode", "Off");
        ini_set("display_errors", "Off");

        printf("<input class='Width' type='submit' value='Query Table'>&nbsp;&nbsp;&nbsp;\n");
        printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"sequence.php\"'>&nbsp;&nbsp;&nbsp;\n");
//        if (strchr($_SERVER["REQUEST_URI"], '?')!=FALSE)
//            printf("<input class='Width' type='button' value='Get .txt' onClick='self.location.href=\"%s&fSendTxt=1\"'>&nbsp;&nbsp;&nbsp;\n", $_SERVER["REQUEST_URI"]);
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

    function GetCulm($azmin, $azmax)
    {
        $cul1=0;
        $cul2=180;
        if (($azmin < $cul1 && $azmax > $cul1) || ($azmin < $cul2 && $azmax > $cul2))
            return "<font color='#FF0000'>yes</font>";
        else
            return "no";
    }

    function QueryRuns($type, $sequ, $db_id, $db)
    {
        $query1  = "SELECT fRunNumber, fCalibrationScriptName FROM RunData ";
        $query1 .= " LEFT JOIN RunType USING(fRunTypeKEY) ";
        $query1 .= " LEFT JOIN CalibrationScript USING(fCalibrationScriptKEY) ";
        $query1 .= " WHERE fSequenceFirst=" . $sequ . " AND fRunTypeName='" . $type . "'";

        mysql_select_db($db);
        $runs = mysql_query($query1, $db_id);
        $numruns = mysql_num_rows($runs);
        $calscript=mysql_result($runs, 0, 1);
        printf(" <td align='left'>");
        for ($i=0 ; $i<$numruns ; $i++)
        {
            $run=mysql_result($runs, $i, 0);
            if ($i!=0)
                printf(", ");
            printf(" %s ", $run);
        }
        printf(" </td>\n");
        printf(" <td> %s ", $calscript);
        printf(" </td>\n");
     }

    function QueryMissingFiles($column, $sequ, $db_id, $db)
    {
        $query2  = "SELECT RunData.fRunNumber FROM RunData ";
        $query2 .= " LEFT JOIN RunProcessStatus USING(fRunNumber,fTelescopeNumber,fFileNumber) ";
        $query2 .= " WHERE fSequenceFirst=" . $sequ . " AND IsNull(" . $column . ")";

        mysql_select_db($db);
        $files = mysql_query($query2, $db_id);
        $numfiles = mysql_num_rows($files);
        printf(" <td align='left'>");
        for ($i=0 ; $i<$numfiles ; $i++)
        {
            $file=mysql_result($files, $i, 0);
            if ($i!=0)
                printf(", ");
            printf(" %s ", $file);
        }
        printf(" </td>\n");
     }


    function PrintSequenceInfo($result0, $db_id, $db, $calibtabs, $signaltabs, $startabs)
    {
        $numres = mysql_num_rows($result0);

        printf("<center>");
        printf("<table BORDER=\"0\">\n");
        printf("<tr><td>");
        printf("# of Sequences: %d ", $numres);
        printf("</td></tr>");

        for ($i=0 ; $i<$numres ; $i++)
        {
            $firstrun=mysql_result($result0, $i, 0);
            $num=sprintf("%08d",$firstrun);
            $num2=substr($num,0,4);
            $lastrun=mysql_result($result0, $i, 1);
            $source=mysql_result($result0, $i, 2);
            $project=mysql_result($result0, $i, 3);
            $starttime=mysql_result($result0, $i, 4);
            $duration=mysql_result($result0, $i, 5);
            $numevents=mysql_result($result0, $i, 6);
            $zdmin=mysql_result($result0, $i, 7);
            $zdmax=mysql_result($result0, $i, 8);
            $azmin=mysql_result($result0, $i, 9);
            $azmax=mysql_result($result0, $i, 10);
            $hv=mysql_result($result0, $i, 11);
            $dt=mysql_result($result0, $i, 12);
            $l1tt=mysql_result($result0, $i, 13);
            $l2tt=mysql_result($result0, $i, 14);
            $td=mysql_result($result0, $i, 15);
            $testflag=mysql_result($result0, $i, 16);
            $lightcond=mysql_result($result0, $i, 17);
            $manuallychanged=mysql_result($result0, $i, 18);
            $sequfile=mysql_result($result0, $i, 19);
            $filesavail=mysql_result($result0, $i, 20);
            $callisto=mysql_result($result0, $i, 21);
            $star=mysql_result($result0, $i, 22);
            $fillcallisto=mysql_result($result0, $i, 23);
            $fillstar=mysql_result($result0, $i, 24);
            $unsi=mysql_result($result0, $i, 25);
            $unso=mysql_result($result0, $i, 26);
            $unri=mysql_result($result0, $i, 27);
            $unro=mysql_result($result0, $i, 28);
            $isoi=mysql_result($result0, $i, 29);
            $isoo=mysql_result($result0, $i, 30);
            $imc=mysql_result($result0, $i, 31);
            $arrtimei=mysql_result($result0, $i, 32);
            $arrtimeo=mysql_result($result0, $i, 33);
            $arrtimermsi=mysql_result($result0, $i, 34);
            $arrtimermso=mysql_result($result0, $i, 35);
            $convi=mysql_result($result0, $i, 36);
            $convo=mysql_result($result0, $i, 37);
            $meanpedrmsi=mysql_result($result0, $i, 38);
            $meanpedrmso=mysql_result($result0, $i, 39);
            $meansignali=mysql_result($result0, $i, 40);
            $meansignalo=mysql_result($result0, $i, 41);
            $pulsposmean=mysql_result($result0, $i, 42);
            $psf=mysql_result($result0, $i, 43);
            $ratio=mysql_result($result0, $i, 44);
            $muonrate=mysql_result($result0, $i, 45);
            $muonnum=mysql_result($result0, $i, 46);
            $effontime=mysql_result($result0, $i, 47);
            $datarate=mysql_result($result0, $i, 48);
            $maxhum=mysql_result($result0, $i, 49);
            $meannumislands=mysql_result($result0, $i, 50);
            $inhom=mysql_result($result0, $i, 51);
            $relontime=mysql_result($result0, $i, 52);
            $callink=mysql_result($result0, $i, 53);
            $starlink=mysql_result($result0, $i, 54);
            $sequlink=mysql_result($result0, $i, 55);

            printf("<tr><td>");

            printf(" <table BORDER=\"1\">");
            printf(" <tr BGCOLOR='#C0C0C0'>\n");
            printf(" <th>Sequence#: <a href=\"http://www.astro.uni-wuerzburg.de/datacenter/sequences/%s/sequence%s.txt\">%s</a> </th>", $num2, $num, $firstrun);
            printf(" </tr><tr BGCOLOR='#D0D0D0' ALIGN='left'>\n");
            printf(" <th> %s, %s, zd: %s&deg;-%s&deg;, %.2fmin ",
                   $source, $starttime, $zdmin, $zdmax, $duration/60);
            if (!is_null($star))
                printf(", <font color='green'>image files available</font> ");
            printf(" </td>\n");
            printf(" </tr><tr>\n");
            printf(" <td>\n");

            printf("</td></tr><tr><td>\n");

            printf(" <table BORDER=\"1\">");
            printf(" <tr BGCOLOR='#C0C0C0'>\n");
            printf(" <th colspan=\"4\">conditions</th>");
            printf(" <th colspan=\"3\">muon parameter</th>\n");
            printf(" </tr><tr BGCOLOR='#E0E0E0'>\n");
            printf(" <td>light</td>\n");
            printf(" <td>max hum</td>\n");
            printf(" <td>zd</td>\n");
            printf(" <td>culmination</td>");
            if (!is_null($fillstar))
            {
                printf(" <td>muon rate</td>\n");
                printf(" <td>ratio</td>\n");
                printf(" <td>psf</td>\n");
            }
            printf(" </tr><tr BGCOLOR='#E0E0E0' align='right'>");
            printf(" <td>%s</td>\n", $lightcond);
            printf(" <td>%s %%</td>\n", $maxhum);
            printf(" <td>%s&deg;-%s&deg;</td>\n", $zdmin, $zdmax);
            printf(" <td>%s</td>\n", GetCulm($azmin, $azmax));
            if (!is_null($fillstar))
            {
                printf(" <td> %sHz </td>\n", $muonrate);
                printf(" <td> %s </td>\n", $ratio);
                printf(" <td> %smm </td>\n", $psf);
            }
            printf(" </tr></table>");


            if ($_GET["fAll"]=="On")
            {
                printf("</td></tr><tr><td>\n");
                printf("</td></tr><tr><td>\n");

                printf(" <table BORDER=\"1\">");
                printf(" <tr BGCOLOR='#C0C0C0'>\n");
                printf(" <th colspan=\"6\">general information</th>");
                printf(" </tr>\n");
                printf(" <tr BGCOLOR='#E0E0E0'>\n");
                printf(" <td>source name</td>\n");
                printf(" <td># events</td>\n");
                printf(" <td>project name</td>\n");
                printf(" <td>light conditions</td>\n");
                printf(" <td>manually changed</td>\n");
                printf(" <td>test flag</td>");
                printf(" </tr><tr BGCOLOR='#E0E0E0'>");
                printf(" <td> %s </td>\n", $source);
                printf(" <td> %s </td>\n", $numevents);
                printf(" <td> %s </td>\n", $project);
                printf(" <td> %s </td>\n", $lightcond);
                printf(" <td> %s </td>\n", $manuallychanged);
                printf(" <td> %s </td>\n", $testflag);
                printf(" </tr></table>");

                printf("</td></tr><tr><td>\n");

                printf(" <table BORDER=\"1\">");
                printf(" <tr BGCOLOR='#C0C0C0'>\n");
                printf(" <th colspan=\"5\">trigger & camera</th>");
                printf(" </tr>\n");
                printf(" <tr BGCOLOR='#E0E0E0'>\n");
                printf(" <td>l1 trigger</td>\n");
                printf(" <td>l2 trigger</td>\n");
                printf(" <td>trigger delay</td>\n");
                printf(" <td>HvSettings</td>\n");
                printf(" <td>DT</td>");
                printf(" </tr><tr BGCOLOR='#E0E0E0'>");
                printf(" <td> %s </td>\n", $l1tt);
                printf(" <td> %s </td>\n", $l2tt);
                printf(" <td> %s </td>\n", $td);
                printf(" <td> %s </td>\n", $hv);
                printf(" <td> %s </td>\n", $dt);
                printf(" </tr></table>");

                printf("</td></tr><tr><td>\n");

                printf(" <table BORDER=\"1\">");
                printf(" <tr BGCOLOR='#C0C0C0'>\n");
                printf(" <th colspan=\"6\">steps</th>");
                printf(" </tr>\n");
                printf(" <tr BGCOLOR='#E0E0E0' align='center'>\n");
                printf(" <td>sequencefile</td>\n");
                printf(" <td>files avail</td>\n");
                printf(" <td>callisto</td>\n");
                printf(" <td>fillcal</td>\n");
                printf(" <td>star</td>");
                printf(" <td>fillstar</td>");
                printf(" </tr><tr BGCOLOR='#E0E0E0' align='right'>");
                printf(" <td> %s </td>\n", GetStatus($sequfile));
                printf(" <td> %s </td>\n", GetStatus($filesavail));
                printf(" <td> %s </td>\n", GetStatus($callisto));
                printf(" <td> %s </td>\n", GetStatus($fillcallisto));
                printf(" <td> %s </td>\n", GetStatus($star));
                printf(" <td> %s </td>\n", GetStatus($fillstar));
                printf(" </tr>\n");
                printf(" <tr BGCOLOR='#E0E0E0' align='center'>\n");
                printf(" <td>%s</td>\n", $sequlink);
                printf(" <td><br></td>\n");
                printf(" <td>%s</td>\n", $callink);
                printf(" <td><br></td>\n");
                printf(" <td>%s</td>", $starlink);
                printf(" <td><br></td>");
                printf(" </tr></table>");

                printf("</td></tr><tr><td>\n");

                if (!is_null($fillcallisto))
                {
                    printf(" <table BORDER=\"1\">");
                    printf(" <tr BGCOLOR='#D0D0D0'>");
                    printf(" <th BGCOLOR='#C0C0C0'>calibration (<a href=\"reference.html\">ref</a>)</th>\n");
                    printf(" <td>inner</td>\n");
                    printf(" <td>outer</td>");
                    printf(" </tr><tr BGCOLOR='#E0E0E0'>");
                    printf(" <td>unsuitable pixel</td>\n");
                    printf(" <td align='right'> %s </td>\n", $unsi);
                    printf(" <td align='right'> %s </td>\n", $unso);
                    printf(" </tr><tr BGCOLOR='#D0D0D0'>");
                    printf(" <td>unreliable pixel</td>\n");
                    printf(" <td align='right'> %s </td>\n", $unri);
                    printf(" <td align='right'> %s </td>\n", $unro);
                    printf(" </tr><tr BGCOLOR='#E0E0E0'>");
                    printf(" <td>isolated pixel</td>\n");
                    printf(" <td align='right'> %s </td>\n", $isoi);
                    printf(" <td align='right'> %s </td>\n", $isoo);
                    printf(" </tr><tr BGCOLOR='#D0D0D0'>");
                    printf(" <td>isolated max cluster</td>\n");
                    printf(" <td colspan=\"2\" align='right'> %s </td>\n", $imc);
                    printf(" </tr><tr BGCOLOR='#E0E0E0'>");
                    printf(" <td>arrival time</td>\n");
                    printf(" <td align='right'> %s &plusmn; %s </td>\n", $arrtimei, $arrtimermsi);
                    printf(" <td align='right'> %s &plusmn; %s </td>\n", $arrtimeo, $arrtimermso);
                    printf(" </tr><tr BGCOLOR='#D0D0D0'>");
                    printf(" <td>conversion factor</td>\n");
                    printf(" <td align='right'> %s </td>\n", $convi);
                    printf(" <td align='right'> %s </td>\n", $convo);
                    printf(" </tr><tr BGCOLOR='#E0E0E0'>");
                    printf(" <td>mean ped rms</td>\n");
                    printf(" <td align='right'> %s </td>\n", $meanpedrmsi);
                    printf(" <td align='right'> %s </td>\n", $meanpedrmso);
                    printf(" </tr><tr BGCOLOR='#D0D0D0'>");
                    printf(" <td>mean signal</td>\n");
                    printf(" <td align='right'> %s </td>\n", $meansignali);
                    printf(" <td align='right'> %s </td>\n", $meansignalo);
                    printf(" </tr><tr BGCOLOR='#E0E0E0'>");
                    printf(" <td>mean pulse pos</td>\n");
                    printf(" <td colspan=\"2\" align='right'> %s </td>\n", $pulsposmean);
                    printf(" </tr>");
                    printf(" </table>");

                    printf("</td></tr><tr><td>\n");
                }

                if (!is_null($fillstar))
                {
                    printf(" <table BORDER=\"1\">");
                    printf(" <tr BGCOLOR='#C0C0C0'>");
                    printf(" <th colspan=\"9\">star</th>\n");
                    printf(" </tr><tr BGCOLOR='#D0D0D0' ALIGN='center'>");
                    printf(" <td># islands</td>\n");
                    printf(" <td># muons</td>\n");
                    printf(" <td>effontime</td>\n");
                    printf(" <td>datarate</td>\n");
                    printf(" <td>muon rate</td>\n");
                    printf(" <td>ratio</td>\n");
                    printf(" <td>psf</td>\n");
                    printf(" <td>inhom</td>\n");
                    printf(" <td>rel. ontime</td>\n");
                    printf(" <tr BGCOLOR='#E0E0E0' ALIGN='right'>");
                    printf(" <td> %s </td>\n", $meannumislands);
                    printf(" <td> %s </td>\n", $muonnum);
                    printf(" <td> %ss </td>\n", $effontime);
                    printf(" <td> %sHz </td>\n", $datarate);
                    printf(" <td> %sHz </td>\n", $muonrate);
                    printf(" <td> %s </td>\n", $ratio);
                    printf(" <td> %smm </td>\n", $psf);
                    printf(" <td> %s </td>\n", $inhom);
                    printf(" <td> %s </td>\n", $relontime);
                    printf(" </tr>");
                    printf(" </table>");

                    printf("</td></tr><tr><td>");
                }


                printf(" <table BORDER=\"1\">");
                printf(" <tr BGCOLOR='#C0C0C0'>");
                printf(" <th>plots</th>\n");
                printf(" <tr><td>calib: ");
                foreach($calibtabs as $key => $element)
                    if (!$key==0)
                        printf("<a href=\"http://www.astro.uni-wuerzburg.de/datacenter/callisto/%s/%s/calib%s-tab%s.png\">%s</a> ", $num2, $num, $num, $key, $element);
                printf("</td></tr>");
                printf(" <tr><td>signal: ");
                foreach($signaltabs as $key => $element)
                    if (!$key==0)
                        printf("<a href=\"http://www.astro.uni-wuerzburg.de/datacenter/callisto/%s/%s/signal%s-tab%s.png\">%s</a> ", $num2, $num, $num, $key, $element);
                printf("</td></tr>");
                printf(" <tr><td>star: ");
                foreach($startabs as $key => $element)
                    if (!$key==0)
                        printf("<a href=\"http://www.astro.uni-wuerzburg.de/datacenter/star/%s/%s/star%s-tab%s.png\">%s</a> ", $num2, $num, $num, $key, $element);
                printf("</td></tr>");

                printf(" </tr>");
                printf(" </table>");

                printf(" <table BORDER=\"1\">");
                printf(" <tr BGCOLOR='#C0C0C0'>");
                printf(" <th><A HREF=\"runinfo.php?fRunStart=On&fZenithDistance=On&fRunMin=%s&fMeanTriggerRate=On&fRunTypeName=On&fRunMax=%s&fNumEvents=On&fSourceName=On&fExcludedFDAKEY=1&fSequenceFirst=On&fRawFileAvail=On&fCCFileAvail=On&fCaCoFileAvail=On&fNumResults=500\">runs</A></th>", $firstrun, $lastrun);
                printf(" <td>runs</td>\n");
                printf(" <td>calscript</td>\n");
                printf(" </tr><tr BGCOLOR='#D0D0D0' ALIGN='center'>");
                printf(" <td>dataruns</td>\n");
                QueryRuns("Data", $firstrun, $db_id, $db);
                printf(" </tr><tr BGCOLOR='#E0E0E0' ALIGN='center'>");
                printf(" <td>pedruns</td>\n");
                QueryRuns("Pedestal", $firstrun, $db_id, $db);
                printf(" </tr><tr BGCOLOR='#D0D0D0' ALIGN='center'>");
                printf(" <td>calruns</td>\n");
                QueryRuns("Calibration", $firstrun, $db_id, $db);
                printf(" </tr>");
                printf(" </table>");

                if (is_null($filesavail))
                {
                    printf(" <table BORDER=\"1\">");
                    printf(" <tr BGCOLOR='#C0C0C0'>");
                    printf(" <th>missing files</th>");
                    printf(" <td>run# of missing files</td>\n");
                    printf(" </tr><tr BGCOLOR='#D0D0D0' ALIGN='center'>");
                    printf(" <td>rawfiles</td>\n");
                    QueryMissingFiles("fRawFileAvail", $firstrun, $db_id, $db);
                    printf(" </tr><tr BGCOLOR='#E0E0E0' ALIGN='center'>");
                    printf(" <td>ccfiles</td>\n");
                    QueryMissingFiles("fCCFileAvail", $firstrun, $db_id, $db);
                    printf(" </tr><tr BGCOLOR='#D0D0D0' ALIGN='center'>");
                    printf(" <td>cacofiles</td>\n");
                    QueryMissingFiles("fCaCoFileAvail", $firstrun, $db_id, $db);
                    printf(" </tr>");
                    printf(" </table>");
                }


            }
            printf(" </table>");
            printf("</td></tr><tr><td><br></td></tr>");

        }
        printf("</table>\n");

        printf("</center>\n");
        printf("</tr><tr class='Block'><td>\n");
    }

    function PrintPage($html, $host, $user, $pw, $db, $calibtabs, $signaltabs, $startabs)
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
                PrintSequenceInfo($result0, $db_id, $db, $calibtabs, $signaltabs, $startabs);
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

        PrintPage("0", $host, $user, $pw, $db, $calibtabs, $signaltabs, $startabs);
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
                PrintPage("1", $host, $user, $pw, $db, $calibtabs, $signaltabs, $startabs);
            else
                PrintPage("2", $host, $user, $pw, $db, $calibtabs, $signaltabs, $startabs);
        }

        if (empty($_GET["fPrintTable"]))
            echo (file_get_contents("index-footer.html"));
    }

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
