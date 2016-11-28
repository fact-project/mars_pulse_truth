<?php

    function PrintDataSetFile()
    {
        if (empty($_SESSION["AnalysisNumber"]))
            printf("AnalysisNumber: 1\n\n");
        else
            printf("AnalysisNumber: %s\n\n", $_SESSION["AnalysisNumber"]);
        printf("SeqencesOn: %s \n", $_SESSION["sequon"]);
        if ($_SESSION["sequoff"]!="")
            printf("SeqencesOff: %s \n\n", $_SESSION["sequoff"]);
        else
            printf("\n");

        printf("SourceName: %s \n", $_SESSION["realsourcename"]);
        printf("Catalog: /magic/datacenter/setup/magic_favorites_dc.edb \n");
        if ($_SESSION["sequoff"]==" ")
            printf("WobbleMode: On \n\n");
        printf("RunTime: %s \n", $_SESSION["runtime"]);
        printf("Name: %s \n", $_SESSION["name"]);
        printf("Comment: %s \n", $_SESSION["comment"]);
    }

    function CreateQuery($alias, $checkwhere, $checkgroup, $checkstatusgroup, $needs, $db_id)
    {
        $fromtable="Sequences";

        $groups = 0;
        foreach ($checkgroup as $element)
            if ($element==-1)
                $groups++;

        $statusgroups = 0;
        foreach ($checkstatusgroup as $element)
            if ($element==-1)
                $statusgroups++;

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
            $query0 .= " Min(fZenithDistanceMin) as '" . $alias["Min(fZenithDistanceMin)"] . "', ";
            $query0 .= " Max(fZenithDistanceMax) as '" . $alias["Max(fZenithDistanceMax)"] . "', ";
            $query0 .= " COUNT(*) as '# Sequ' ";
        }
        else
        {
            if ($statusgroups>0)
            {
                foreach ($checkstatusgroup as $key => $element)
                    if ($element==-1)
                        $query0 .= " (if(IsNull(" . $key . "), if(isnull(fStartTime), 'not done', if(isnull(fFailedTime),if(isnull(" . $needs[$key] . "),'not done',if(date_sub(Now(),interval 12 hour) < fStartTime,'running','crashed')),if(isnull(" . $needs[$key] . "),'not done','failed'))) ,if(" . $key . "='1970-01-01 00:00:00','dont do','done'))) as '" . $alias[$key] . "', ";

                $query0 .= " COUNT(*) as '# Sequ'";

            }
            else
            {
                $query0 .= " " . $fromtable . ".fSequenceFirst as '" . $alias["fSequenceFirst"] . "' ";
                if (!empty($_GET["fLinks"]))
                {
                    $query0 .= ", CONCAT('<A&ws;HREF=\"tabs.php?t=calib&n=', fSequenceFirst, '\">cal</A>'";
                    $query0 .= ", '&nbsp;<A&ws;HREF=\"tabs.php?t=signal&n=', fSequenceFirst, '\">sig</A>'";
                    $query0 .= ", '&nbsp;<A&ws;HREF=\"tabs.php?t=star&n=', fSequenceFirst, '\">star</A>'";
                    $query0 .= ", '&nbsp;<A&ws;HREF=\"http://www.astro.uni-wuerzburg.de/datacenter/sequences/' , LEFT(LPAD(CONCAT(Sequences.fSequenceFirst, '.'), 9,'0'),4), '/sequence', LPAD(CONCAT(Sequences.fSequenceFirst , '.txt'), 12,'0') , '\">', 'f </A>' ";
                    $query0 .= ", '&nbsp;<A&ws;HREF=\"queryrbk.php?fNight=', date_format(adddate(fRunStart, interval +13 HOUR), '%Y-%m-%d') , '&amp;fDate=2\">rbk</A>') ";
                    $query0 .= " as 'Links'";
                }

                foreach ($_GET as $key => $element)
                    if ($_GET[$key]=="On")
                        if ($key!="fLinks" && $key!="fOff" && $key!="fOnlyOff" && $key!="fNumStart")
                            if (!(ereg("^DSSeq[0-9]*$", $key)))
                                if (empty($checkwhere[$key]) || $checkwhere[$key]==0)
                                    $query0 .= ", " . $key . " as '" . $alias[$key] . "' ";
            }
        }

        $query0 .= " FROM " . $fromtable;

        $query0 .= " LEFT JOIN SequenceProcessStatus USING(fSequenceFirst,fTelescopeNumber) ";
        $query0 .= " LEFT JOIN Calibration USING(fSequenceFirst,fTelescopeNumber) ";
        $query0 .= " LEFT JOIN Star USING(fSequenceFirst,fTelescopeNumber) ";

        foreach ($_GET as $key => $element)
            if (($_GET[$key]=="On" || $groups>0))// && !empty(GetJoin($fromtable,$key)))
                $query0 .= GetJoin($fromtable,$key);

        if (($_SESSION["DisplaySelected"]!="yes"
             || ($_SESSION["sequon"]=="" && $_SESSION["sequoff"]==""))
            && (empty($_SESSION["DataSetSelection"]) || $_SESSION["DataSetAcknowledged"]=="yes"))
        {
            if ($_GET["fTest"]!="On")
            {
                if ($_GET["fSourceName"]!="On")
                    $query0 .= " LEFT JOIN Source USING(fSourceKEY) ";
                $query0 .= " WHERE fTest='no'";
            }

            if ($_GET["fOff"]=="Off")
            {
                if (strpos($query0, " WHERE ")==FALSE)
                    $query0 .= " WHERE ";
                else
                    $query0 .= " AND ";

                $query0 .= " NOT (fSourceName like '%Off%')";
            }

            if ($_GET["fOnlyOff"]=="On")
            {
                if (strpos($query0, " WHERE ")==FALSE)
                    $query0 .= " WHERE ";
                else
                    $query0 .= " AND ";

                $query0 .= " fSourceName like '%Off%'";
            }

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

            $query0 .= StatusQuery("fSequenceFileWritten", $needs, $timelimits);
            $query0 .= StatusQuery("fAllFilesAvail", $needs, $timelimits);
            $query0 .= StatusQuery("fCallisto", $needs, $timelimits);
            $query0 .= StatusQuery("fFillCallisto", $needs, $timelimits);
            $query0 .= StatusQuery("fStar", $needs, $timelimits);
            $query0 .= StatusQuery("fFillStar", $needs, $timelimits);

            if (!empty($_GET["fRunMin"]) && !empty($_GET["fRunMax"]))
                $query0 .= "Sequences.fSequenceFirst BETWEEN " . $_GET["fRunMin"] . " AND " . $_GET["fRunMax"] . " ";
            else
                $query0 = substr($query0, 0, -4);

            if ((!empty($_GET["fZDMin"]) || $_GET["fZDMin"]==0) && !empty($_GET["fZDMax"]))
                $query0 .= "AND (fZenithDistanceMin >= " . $_GET["fZDMin"] . " AND fZenithDistanceMax <= " . $_GET["fZDMax"] . ") ";

            if (!empty($_GET["fSourceN"]))
                $query0 .= " AND fSourceName REGEXP \"^" . $_GET["fSourceN"] . "\" ";

            if (!empty($_GET["fStartDate"]))
            {
                if (strpos(strrev($query0), " DNA ")!=0 || !strpos(strrev($query0), " DNA "))
                    $query0 .= " AND ";

                $startdate=substr($_GET["fStartDate"], 0, 10);
                if ($startdate=="0000-00-00")
                    $query0 .=" fRunStart >= '" . $startdate . " 00:00:00' ";
                else
                    $query0 .= " fRunStart >= ADDDATE('" . $startdate . " 13:00:00', INTERVAL -1 DAY) ";
            }

            if (!empty($_GET["fStopDate"]))
            {
                if (strpos(strrev($query0), " DNA ")!=0 || !strpos(strrev($query0), " DNA "))
                    $query0 .= " AND ";

                $stopdate=substr($_GET["fStopDate"], 0, 10);
                $query0 .= " fRunStart < '" . $stopdate . " 13:00:00' ";
            }

            if (!empty($_GET["fStarStart"]))
            {
                if (strpos(strrev($query0), " DNA ")!=0 || !strpos(strrev($query0), " DNA "))
                    $query0 .= " AND ";

                $starstart=substr($_GET["fStarStart"], 0, 10);
                $query0 .=" fStar >= '" . $starstart . " 00:00:00' ";
            }

            if (!empty($_GET["fStarStop"]))
            {
                if (strpos(strrev($query0), " DNA ")!=0 || !strpos(strrev($query0), " DNA "))
                    $query0 .= " AND ";

                $starstop=substr($_GET["fStarStop"], 0, 10);
                $query0 .= " fStar < '" . $starstop . " 23:59:59' ";
            }

            if ($_SESSION["DisplaySelected"]=="inverse")
            {
                $displsequ=str_replace("  ", " ", $_SESSION["sequon"].$_SESSION["sequoff"]);
                $displsequ=ereg_replace("^ ", "(", $displsequ);
                $displsequ=ereg_replace("$", ")", $displsequ);
                $displsequ=str_replace(" ",  ",", $displsequ);

                if (strpos(strrev($query0), " DNA ")!=0 || !strpos(strrev($query0), " DNA "))
                    $query0 .= " AND ";

                $query0 .= " fSequenceFirst NOT IN " . $displsequ;
            }

            if ($groups>0)
            {
                $query0 .= " GROUP BY ";
                $num = $groups;
                foreach ($checkgroup as $key => $element)
                    if ($element==-1)
                    {
                        $query0 .= GetCheck($fromtable,$key);
                        if ($num-->1)
                            $query0 .= ", ";
                    }
            }

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

        }
        else
        {
            if (empty($_SESSION["DataSetSelection"]) || $_SESSION["DataSetAcknowledged"]=="yes")
            {
                $displsequ=str_replace("  ", " ", $_SESSION["sequon"].$_SESSION["sequoff"]);
                $displsequ=ereg_replace("^ ", "(", $displsequ);
                $displsequ=ereg_replace("$", ")", $displsequ);
                $displsequ=str_replace(" ",  ",", $displsequ);
            }
            else
            {
                //get on sequences
                $query1 = "SELECT fSequenceFirst FROM DataSetSequenceMapping WHERE fOnOff=1 AND fDataSetNumber=".$_SESSION["SelectedDataSet"];
                $result1 = mysql_query($query1, $db_id);
                while ($row = mysql_fetch_assoc($result1))
                {
                    $_SESSION["DSSeq".$row["fSequenceFirst"]]="ON";
                    $onseq.=" " . $row["fSequenceFirst"];
                }
                mysql_free_result($result1);
                //get off sequences
                $query1 = "SELECT fSequenceFirst FROM DataSetSequenceMapping WHERE fOnOff=2 AND fDataSetNumber=".$_SESSION["SelectedDataSet"];
                $result1 = mysql_query($query1, $db_id);
                while ($row = mysql_fetch_assoc($result1))
                {
                    $_SESSION["DSSeq".$row["fSequenceFirst"]]="Off";
                    $offseq.=" " . $row["fSequenceFirst"];
                }
                mysql_free_result($result1);

                $displsequ=str_replace("  ", " ", $onseq.$offseq);
                $displsequ=ereg_replace("^ ", "(", $displsequ);
                $displsequ=ereg_replace("$", ")", $displsequ);
                $displsequ=str_replace(" ",  ",", $displsequ);

                if ($_SESSION["DataSetSelection"]=="UpdateDataSet")
                {
                    //get values of old data set from db
                    $queryold  = "SELECT fUserKEY, fComment, fDataSetName FROM DataSets WHERE fDataSetNumber=".$_SESSION["SelectedDataSet"];
                    $resultold = mysql_query($queryold);
                    $rowold = mysql_fetch_assoc($resultold);
                    $_SESSION["olduser"] = $rowold["fUserKEY"];
                    $_SESSION["comment"] = $rowold["fComment"];
                    $_SESSION["name"] = $rowold["fDataSetName"];
                    mysql_free_result($resultold);
                }
                $_SESSION["DisplaySelected"]="yes";
            }

            $query0 .= "WHERE fSequenceFirst IN " . $displsequ;
        }

        if (!empty($_GET["fSortBy"]))
        {
            $val=substr($_GET["fSortBy"], 0, -1);
            $query0 .= " ORDER BY " . GetTable($fromtable,$val) . " ";
            if (substr($_GET["fSortBy"], -1)=="-")
                $query0 .= "DESC";
        }

        if (empty($_GET["fSortBy"]) && $groups==0 && $statusgroups==0)
            $query0 .= " ORDER BY Sequences.fSequenceFirst DESC ";

        if (empty($_GET["fNumStart"]))
            $start=0;
        else
            $start=$_GET["fNumStart"];

        if (empty($_GET["fSendTxt"]))
            $query0 .= " LIMIT " . $start . ", " . $_GET["fNumResults"];

            return $query0;
    }

    function PrintForm($host, $user, $pw, $db, $limitsmean, $limitsmin, $limitsmax, $alias)
    {
        printf("<center>\n");
        printf("<form action='builddatasets.php' METHOD='GET'>\n");
        printf("<input id='sh' type='hidden' name='fShowHide' value='");
        if (!empty($_GET["fShowHide"]))
            printf("%s", $_GET["fShowHide"]);
        else
            printf("1000000010");//display Ranges
        printf("'>\n");
        printf("<img id='allbutton' src='minus.png' alt='-' onclick='showhide(\"all\")'> <b>Menu</b>&nbsp;&nbsp;&nbsp;&nbsp;\n");
        printf("&nbsp;&nbsp;<img id='infobutton'   src='plus.png' alt='+' onClick='showhide(\"info\");showhide(\"info2\")'> SequInfo   \n");
        printf("&nbsp;&nbsp;<img id='statbutton'   src='plus.png' alt='+' onClick='showhide(\"stat\");showhide(\"fail\")'>  StatusInfo \n");
        printf("&nbsp;&nbsp;<img id='calbutton'    src='plus.png' alt='+' onClick='showhide(\"cal\")'>                      CalInfo    \n");
        printf("&nbsp;&nbsp;<img id='starbutton'   src='plus.png' alt='+' onClick='showhide(\"star\")'>                     StarInfo   \n");
        printf("&nbsp;&nbsp;<img id='limitsbutton' src='plus.png' alt='+' onClick='showhide(\"limits\")'>                   Limits     \n");
        printf("&nbsp;&nbsp;<img id='rangesbutton' src='plus.png' alt='+' onClick='showhide(\"ranges\")'>                   Ranges     \n");

        printf(" <div id='all' style='display:block'>");

        PrintSequInfo2Menu($host,$user,$pw,$db);
        PrintSequInfoMenu();
        PrintSequStatMenu();
        PrintFailMenu();
        PrintCalMenu();
        PrintStarMenu();
        PrintLimitsMenu($limitsmean, $limitsmin, $limitsmax, $alias, "");

        printf(" <div id='ranges' style='display:none'>");
        PrintZdRangeMenu($host,$user,$pw,$db);
        PrintSequRangeMenu($host,$user,$pw,$db);
        printf("<p>");
        PrintSourceMenu($host,$user,$pw,$db);
        PrintNightRangeMenu($host,$user,$pw,$db, "Sequences");
        PrintStarRangeMenu($host,$user,$pw,$db);
        printf("<p>");
        printf("</div>");

        printf("</div>");
        printf(" <P>\n");

        PrintNumResPullDown();

        ini_set("mysql.trace_mode", "Off");
        ini_set("display_errors", "Off");

        PrintButtons("builddatasets.php");

//        printf("</form>\n");
        printf("</center>\n");
        printf("</td>\n");
        printf("</tr>\n");
        printf("<tr class='Block'>\n");
        printf("<td>\n");
    }

    function InsertUpdateDataSet($values, $dataset)
    {
        if ($_SESSION["DataSetSelection"]=="UpdateDataSet")
            $insquery[0]="UPDATE DataSets SET ";
        else
            $insquery[0]="INSERT DataSets SET fDataSetNumber=" . $dataset . ", fUserKEY=" . $_SESSION["user"] . ", ";
        $insquery[0].=" fComment='" . $_SESSION["comment"] . "', fObservationModeKEY=" . $_SESSION["obsmode"];
        $insquery[0].=", fDataSetName='" . $_SESSION["name"] . "', fSourceKEY= " . $_SESSION["realsourcekey"];
        $insquery[0].=", fRunStart='" . $values["Min(fRunStart)"]["on"];
        $insquery[0].="', fRunStop='" . $values["Max(fRunStop)"]["on"];
        $insquery[0].="', fZenithDistanceMin=" . $values["Min(fZenithDistanceMin)"]["on"];
        $insquery[0].=", fZenithDistanceMax=" . $values["Max(fZenithDistanceMax)"]["on"];
        $insquery[0].=", fRunTime=" . $_SESSION["runtime"];
        if ($_SESSION["DataSetSelection"]=="UpdateDataSet")
        {
            $insquery[0].=" WHERE fDataSetNumber=" . $_SESSION["SelectedDataSet"];

            $insquery[1]="UPDATE DataSetProcessStatus SET fDataSetInserted=Now(), fDataSetFileWritten=NULL, ";
            $insquery[1].=" fStarFilesAvail=NULL, fGanymed=NULL, fFillGanymed=NULL, fWebGanymed=NULL, ";
            $insquery[1].=" fWebPlotDBGanymed=NULL, fStartTime=NULL, fFailedTime=NULL, fProgramId=NULL, ";
            $insquery[1].=" fReturnCode=NULL WHERE fDataSetNumber=" . $_SESSION["SelectedDataSet"];

            $insquery[2]=" DELETE FROM DataSetSequenceMapping WHERE fDataSetNumber=" . $_SESSION["SelectedDataSet"];
            $i=3;
        }
        else
        {
            $insquery[1]="INSERT DataSetProcessStatus SET fDataSetNumber=" . $dataset . ", fDataSetInserted=Now()";
            $i=2;
        }
        foreach(explode(" ", trim($_SESSION["sequon"])) as $key => $sequ)
        {
            $insquery[$i]="INSERT DataSetSequenceMapping SET fDataSetNumber=";
            if ($_SESSION["DataSetSelection"]=="UpdateDataSet")
                $insquery[$i].=$_SESSION["SelectedDataSet"];
            else
                $insquery[$i].=$dataset;
            $insquery[$i].=", fSequenceFirst=" . $sequ . ", fOnOff=1";
            $i=$i+1;
        }
        if (!empty($off))
        {
            foreach(explode(" ", trim($_SESSION["sequoff"])) as $key => $sequ)
            {
                $insquery[$i]="INSERT DataSetSequenceMapping SET fDataSetNumber=";
                if ($_SESSION["DataSetSelection"]=="UpdateDataSet")
                    $insquery[$i].=$_SESSION["SelectedDataSet"];
                else
                    $insquery[$i].=$dataset;
                $insquery[$i].=", fSequenceFirst=" . $sequ . ", fOnOff=2";
                $i=$i+1;
            }
        }
        // insert
        if (!empty($_SESSION["insert"]) && $numerr==0)
        {
            printf("<br><br>\n");
            foreach($insquery as $num => $q)
            {
                printf("Executed Query #%s: %s<br>\n", $num, $q);
                $insresult=mysql_query($q);
                if (mysql_errno()>0)
                    printf("&nbsp; Errorno: %s <br>\n", mysql_errno());
                if (mysql_error()!="")
                    printf("&nbsp; Errormsg: %s <br>\n", mysql_error());
                if (mysql_info()!="")
                    printf("&nbsp; Info: %s <br>\n", mysql_info());
                printf("&nbsp; Affected Rows: %s <br><br>\n", mysql_affected_rows());
                mysql_free_result($insresult);
                if (mysql_errno()>0)
                {
                    printf("ERROR => stop inserting. <br><br>\n");
                    break;
                }
            }
            // reset after insert
            unset($_SESSION["insert"]);
//            unset($_SESSION["name"]);
//            unset($_SESSION["comment"]);
        }
        else
            if ($_SESSION["DataSetSelection"]=="UpdateDataSet")
                printf("<input type='button' value='Update Data Set in DB' onClick='self.location.href=\"%s&insert=yes\"'>&nbsp;&nbsp;&nbsp;\n", GetClearedURL());
            else
                printf("<input type='button' value='Insert Data Set in DB' onClick='self.location.href=\"%s&insert=yes\"'>&nbsp;&nbsp;&nbsp;\n", GetClearedURL());
    }

    function PrintPage($html, $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $limitsmean, $limitsmin, $limitsmax, $needs)
    {
        $db_id = mysql_pconnect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);

        $query0 = CreateQuery($alias, $checkwhere, $checkgroup, $checkstatusgroup, $needs, $db_id);

        $result0 = mysql_query($query0, $db_id);
        $result1 = mysql_query("SELECT FOUND_ROWS()", $db_id);

        if ($result0)
        {
            if ($html=="1" || $html=="2")
            {
                PrintMagicTable($result0, $alias, $rightalign, $limitsmean, $limitsmin, $limitsmax, $result1, "yes");

                if (!(empty($_SESSION["sequon"]) && empty($_SESSION["sequoff"]))
                   && (empty($_SESSION["olduser"]) || $_SESSION["olduser"]==$_SESSION["user"]))
                {
                    printf("<div align='left'>\n");
                    printf("Remark: To store your selection, you have to press 'Update Selection'<br> \n");
                    printf("<br>\n");

                    printf("<u>Your Data Set:</u> <br>\n");

                    $on=str_replace(" ", ",", trim($_SESSION["sequon"]));
                    $off=str_replace(" ", ",", trim($_SESSION["sequoff"]));

                    //set observation mode
                    if ($_SESSION["sequoff"]=="")
                        $obsmode="Wobble";
                    else
                        $obsmode="On";

                    //get key from db for insert query
                    $query   = "SELECT fObservationModeKEY FROM ObservationMode WHERE fObservationModeName='".$obsmode."'";
                    $result  = mysql_query($query);
                    $row = mysql_fetch_assoc($result);
                    $_SESSION["obsmode"] = $row["fObservationModeKEY"];
                    mysql_free_result($result);

                    $values=array("Sum(fRunTime)/60"         => $runtime,
                                  "Min(fRunTime)/60"         => $runtimemin,
                                  "Max(fUnsuitableInner)"    => $unsuitablemax,
                                  "Max(fUnreliableInner)"    => $unreliablemax,
                                  "Max(fIsolatedInner)"      => $isolatedmax,
                                  "Max(fIsolatedMaxCluster)" => $imcmax,
                                  "Max(fArrTimeMeanInner)"   => $arrtimemax,
                                  "Min(fArrTimeMeanInner)"   => $arrtimemin,
                                  "Max(fArrTimeRmsInner)"    => $arrtimermsmax,
                                  "Avg(fMeanPedRmsInner)"    => $pedrmsmean,
                                  "Std(fMeanPedRmsInner)"    => $pedrmsrms,
                                  "Min(fMeanPedRmsInner)"    => $pedrmsmin,
                                  "Max(fMeanPedRmsInner)"    => $pedrmsmax,
                                  "Min(fPulsePosMean)"       => $pulseposmin,
                                  "Max(fPulsePosMean)"       => $pulseposmax,
                                  "Max(fPulsePosRms)"        => $pulseposrmsmax,
                                  "Max(fMaxHumidity)"        => $hummax,
                                  "Max(fInhomogeneity)"      => $inhommax,
                                  "Avg(fInhomogeneity)"      => $inhommean,
                                  "Std(fInhomogeneity)"      => $inhomrms,
                                  "Min(fMeanNumberIslands)"  => $numislmin,
                                  "Max(fMeanNumberIslands)"  => $numislmax,
                                  "Avg(fDataRate)"           => $clratemean,
                                  "Min(fDataRate)"           => $clratemin,
                                  "Max(fDataRate)"           => $clratemax,
                                  "Min(fMuonRate)"           => $muonratemin,
                                  "Max(fMuonRate)"           => $muonratemax,
                                  "Min(fMuonNumber)"         => $muonnumbermin,
                                  "Avg(fPSF)"                => $psfmean,
                                  "Std(fPSF)"                => $psfrms,
                                  "Min(fPSF)"                => $psfmin,
                                  "Max(fPSF)"                => $psfmax,
                                  "Min(fEffOnTime/fRunTime)" => $relontimemin,
                                  "Max(fEffOnTime/fRunTime)" => $relontimemax,
                                  "Min(fBrightnessMed)"      => $brightmin,
                                  "Max(fBrightnessMed)"      => $brightmax,
                                  "Min(fNumStarsMed)"        => $idstarsmin,
                                  "Max(fNumStarsMed)"        => $idstarsmax,
                                  "Min(fNumStarsCorMed)"     => $corstarsmin,
                                  "Max(fNumStarsCorMed)"     => $corstarsmax,
                                  "Min(fZenithDistanceMin)"  => $zdmin,
                                  "Max(fZenithDistanceMax)"  => $zdmax,
                                  "Min(fRunStart)"           => $starttime,
                                  "Max(fRunStop)"            => $stoptime,
                                 );
                    //get runtimes of dataset
                    function GetSequenceValues($db_id, $sequences, $txt, &$values)
                    {
                        $query="SELECT COUNT(*) ";
                        foreach($values as $name => $varname)
                            $query.=", " . $name;
                        $query.=" FROM Sequences ";
                        $query.=" LEFT JOIN Calibration USING(fSequenceFirst,fTelescopeNumber) ";
                        $query.=" LEFT JOIN Star USING(fSequenceFirst,fTelescopeNumber) ";
                        $query.=" WHERE Sequences.fSequenceFirst IN (" . $sequences . ")";
                        $result = mysql_query($query, $db_id);
                        $row = mysql_fetch_assoc($result);
                        foreach($values as $name => $varname)
                            $values[$name][$txt]=$row[$name];
                        mysql_free_result($result);
                    }

                    //get next dataset#
                    $query="SELECT fDataSetNumber+1 FROM DataSets ORDER BY fDataSetNumber DESC LIMIT 0,1";
                    $result = mysql_query($query, $db_id);
                    $row = mysql_fetch_assoc($result);
                    $dataset=$row["fDataSetNumber+1"];
                    if (empty($dataset))
                        $dataset=1;
                    mysql_free_result($result);

                    //get sourcenames
                    function GetSources($db_id, $sequences)
                    {
                        $query="SELECT fSourceName FROM Source LEFT JOIN Sequences USING(fSourceKEY) ";
                        $query.="WHERE fSequenceFirst IN (" . $sequences . ") GROUP BY fSourceName";
                        $result = mysql_query($query, $db_id);
                        while ($row = mysql_fetch_assoc($result))
                            $sources.=" " . $row["fSourceName"];
                        mysql_free_result($result);
                        return $sources;
                    }
                    $sourceson=GetSources($db_id, $on);
                    $sourcesoff=GetSources($db_id, $off);

                    //get sourcename for dataset
                    $son=str_replace(" ", "','", trim($sourceson));
                    $query="SELECT fRealSourceKEY, Count(*), fSourceName FROM Source where fSourceName IN ('" . $son . "') GROUP BY fRealSourceKEY";
                    $result = mysql_query($query, $db_id);
                    $row = mysql_fetch_assoc($result);
                    $numsources=$row["Count(*)"];
                    $numrealkeys=mysql_num_rows($result);
                    $_SESSION["realsourcekey"]=$row["fRealSourceKEY"];
                    $_SESSION["realsourcename"]=$row["fSourceName"];
                    mysql_free_result($result);
                    //printf("found %s sources with real sourcekey %s", $numsources, $realsourcekey);
                    //printf("found %s keys", $numrealkeys);

                    //check observationmode for on-sequences
                    $query="SELECT fObservationModeKEY FROM Sequences where fSequenceFirst IN (" . $on . ") GROUP BY fObservationModeKEY";
                    $result = mysql_query($query, $db_id);
                    $row = mysql_fetch_assoc($result);
                    $obskey=$row["fObservationModeKEY"];
                    $numobskeys=mysql_num_rows($result);
                    mysql_free_result($result);

                    //check dt for on-sequences
                    $query="SELECT fDiscriminatorThresholdTableKEY FROM Sequences where fSequenceFirst IN (" . $on . ") GROUP BY fDiscriminatorThresholdTableKEY";
                    $result = mysql_query($query, $db_id);
                    $numdtkeys=mysql_num_rows($result);
                    mysql_free_result($result);


                    //set limit
                    $runtimelimit=5;
                    $unsuitablemaxlimit=15;
                    $isolatedlimit=0;
                    $imclimit=0;
                    $pedrmsrms=0.09;
                    $scalelimit=1.3;
                    $inhomlimit=13;
                    $numstarslimit=20;
                    $numstarscorlimit=10;


                    //check values for infos, warnings and errors
                    $numerr=0;
                    if (!empty($on))
                    {
                        $color=array("INFO"  => "#000000",
                                     "WARN"  => "#FF9900",
                                     "ERROR" => "#FF0000");
                        printf("<div align='left'><font><ul>\n");
                        $vals=array("on", "off");
                        foreach($vals as $num => $val)
                        {
                            if (empty(${$val}))
                                continue;
                            //get information from the database
                            GetSequenceValues($db_id, ${$val}, $val, $values);

                            if ($values["Min(fNumStarsCorMed)"][$val]<$numstarscorlimit)
                                printf("<li style='color:%s'>%s: At least one of your %s-sequences has less than %s correlated stars (%s). </li>\n",
                                       $color["WARN"], "WARN", $val, $numstarscorlimit, $values["Min(fNumStarsCorMed)"][$val]);
                            if ($values["Min(fNumStarsMed)"][$val]<$numstarslimit)
                                printf("<li style='color:%s'>%s: At least one of your %s-sequences has less than %s identified stars (%s). </li>\n",
                                       $color["WARN"], "WARN", $val, $numstarslimit, $values["Min(fNumStarsMed)"][$val]);
                            if ($values["Min(fRunTime)/60"][$val]<$runtimelimit)
                                printf("<li style='color:%s'>%s: At least one of your %s-sequences is shorter than %s min (%s min). </li>\n",
                                       $color["INFO"], "INFO", $val, $runtimelimit, $values["Min(fRunTime)/60"][$val]);
                            if ($values["Max(fInhomogeneity)"][$val]>$inhomlimit)
                                printf("<li style='color:%s'>%s: At least one of your %s-sequences has an inhomogeneity larger than %s (%s). </li>\n",
                                       $color["WARN"], "WARN", $val, $inhomlimit, $values["Max(fInhomogeneity)"][$val]);
                            if ($values["Max(fUnsuitableInner)"][$val]>$unsuitablemaxlimit)
                                printf("<li style='color:%s'>%s: At least one of your %s-sequences has more than %s unsuitable inner pixel (%s). </li>\n",
                                       $color["WARN"], "WARN", $val, $unsuitablemaxlimit, $values["Max(fUnsuitableInner)"][$val]);
                            if ($values["Max(fIsolatedInner)"][$val]>$isolatedlimit)
                                printf("<li style='color:%s'>%s: At least one of your %s-sequences has more than %s isolated inner pixel (%s). </li>\n",
                                       $color["WARN"], "WARN", $val, $isolatedlimit, $values["Max(fIsolatedInner)"][$val]);
                            if ($values["Max(fIsolatedMaxCluster)"][$val]>$imclimit)
                                printf("<li style='color:%s'>%s: At least one of your %s-sequences has more than %s isolated max cluster (%s). </li>\n",
                                       $color["WARN"], "WARN", $val, $imclimit, $values["Max(fIsolatedMaxCluster)"][$val]);
                        }
                        //check selected dataset for errors
                        if (!empty($on) && $values["Max(fMeanPedRmsInner)"]["on"]>$values["Avg(fMeanPedRmsInner)"]["on"]+$pedrmsrms)
                            printf("<li style='color:%s'>%s: At least one of your on-sequences has a PedRms larger than %s (%s).</li>\n",
                                   $color["WARN"], "WARN", $values["Avg(fMeanPedRmsInner)"]["on"]+$pedrmsrms, $values["Max(fMeanPedRmsInner)"]["on"]);
                        if (!empty($off) && $values["Max(fMeanPedRmsInner)"]["off"]>$values["Avg(fMeanPedRmsInner)"]["on"]+$pedrmsrms)
                            printf("<li style='color:%s'>%s: At least one of your off-sequences has a PedRms larger than %s (%s). </li>\n",
                                   $color["WARN"], "WARN", $values["Avg(fMeanPedRmsInner)"]["on"]+$pedrmsrms, $values["Max(fMeanPedRmsInner)"]["off"]);
                        if (!empty($off) && $values["Min(fMeanPedRmsInner)"]["off"]<$values["Avg(fMeanPedRmsInner)"]["on"]-$pedrmsrms)
                            printf("<li style='color:%s'>%s: At least one of your off-sequences has a PedRms smaller than %s (%s). </li>\n",
                                   $color["WARN"], "WARN", $values["Avg(fMeanPedRmsInner)"]["on"]-$pedrmsrms, $values["Min(fMeanPedRmsInner)"]["off"]);
                        $scale=$values["Sum(fRunTime)/60"]["on"]/$values["Sum(fRunTime)/60"]["off"];
                        if ($scale > $scalelimit)
                            printf("<li style='color:%s'>%s: Your scale factor is larger than %s (%0.2f). Try to find more offdata! </li>\n",
                                   $color["WARN"], "WARN", $scalelimit, $scale);
                        $doubleseq=0;
                        foreach(explode(" ", $_SESSION["sequon"]) as $n => $s)
                        {
                            if (ereg($s, $_SESSION["sequoff"]))
                                $doubleseq++;
                        }
                        //$obskey >1 makes sure that fObservationMode is not N/A (old data)
                        if ($obskey!=$_SESSION["obsmode"] && $obskey>1)
                        {
                            printf("<li style='color:%s'>%s: You have a mistake in your observation mode. </li>\n",
                                   $color["ERROR"], "ERROR");
                            $numerr=$numerr+1;
                        }
                        if ($doubleseq>0)
                        {
                            printf("<li style='color:%s'>%s: You have selected sequences (%s) as On AND Off. </li>\n",
                                   $color["ERROR"], "ERROR", $doubleseq);
                            $numerr=$numerr+1;
                        }
                        if ($numrealkeys>1)
                        {
                            printf("<li style='color:%s'>%s: You have selected more than one (%s) on source. </li>\n",
                                   $color["ERROR"], "ERROR", $numrealkeys);
                            $numerr=$numerr+1;
                        }
                        if ($numobskeys>1)
                        {
                            printf("<li style='color:%s'>%s: You have selected more than one (%s) different observation modes in your on sequences. </li>\n",
                                   $color["ERROR"], "WARN", $numobskeys);
                            //$numerr=$numerr+1;
                        }     
                        if ($numdtkeys>1)
                        {
                            printf("<li style='color:%s'>%s: Your selected on sequences have more than one (%s) different discriminator threshold tables. </li>\n",
                                   $color["ERROR"], "WARN", $numdtkeys);
                        }
                        if (empty($_SESSION["realsourcekey"]))
                        {
                            printf("<li style='color:%s'>%s: The source you selected does not have a real sourcekey, yet. Please insert fRealSourceKEY in the DB for %s (contact: datacenter@astro.uni-wuerzburg.de). </li>\n",
                                   $color["ERROR"], "ERROR", $sourceson);
                            $numerr=$numerr+1;
                        }
                        if (empty($_SESSION["name"]))
                        {
                            printf("<li style='color:%s'>%s: You have to choose a name. </li>\n",
                                   $color["ERROR"], "ERROR");
                            $numerr=$numerr+1;
                        }
                        if (empty($_SESSION["comment"]))
                        {
                            printf("<li style='color:%s'>%s: You have to comment your data set. </li>\n",
                                   $color["ERROR"], "ERROR");
                            $numerr=$numerr+1;
                        }
                        printf("</ul></font>\n");
                        printf("</div>");

                        //set runtime in session (has to be after calling GetSequenceValues())
                        $_SESSION["runtime"]  = $values["Sum(fRunTime)/60"]["on"];
                        $_SESSION["zdmin"]    = $values["Min(fZenithDistanceMin)"]["on"];
                        $_SESSION["zdmax"]    = $values["Max(fZenithDistanceMax)"]["on"];
                        $_SESSION["meanrate"] = $values["Avg(fDataRate)"]["on"];
                        $_SESSION["psfmean"]  = $values["Avg(fPSF)"]["on"];
                        $_SESSION["psfmin"]   = $values["Min(fPSF)"]["on"];
                        $_SESSION["psfmax"]   = $values["Max(fPSF)"]["on"];

                        printf("SequencesOn: %s<br>\n", $_SESSION["sequon"]);
                        if ($_SESSION["sequoff"]!="")
                            printf("SequencesOff: %s<br>\n", $_SESSION["sequoff"]);

                        printf("Observation mode: %s <br>\n", $obsmode);
                        printf("RunTime: %s min <br>\n", $_SESSION["runtime"]);
                        printf("Mean Rate after cleaning: %s Hz <br>\n", $_SESSION["meanrate"]);
                        printf("Zd: %s - %s deg <br>\n", $_SESSION["zdmin"], $_SESSION["zdmax"]);
                        printf("PSF: %s (%s - %s) mm <br>\n", $_SESSION["psfmean"], $_SESSION["psfmin"], $_SESSION["psfmax"]);

                        //user name
                        printf("User name: %s <br><br>\n", $_SERVER['PHP_AUTH_USER']);

                        // data set name and comment: to be inserted by the user
                        printf("Data set number: &nbsp;<input name='AnalysisNumber' type='text' size='8' maxlength='8' value='%s'> (only used for data set file) <br>\n", $_SESSION["AnalysisNumber"]);
                        printf("Data set name: &nbsp;<input name='name' type='text' size='20' maxlength='20' value='%s'><br>\n", $_SESSION["name"]);
                        printf("Comment: &nbsp;<input name='comment' type='text' size='50' maxlength='255' value='%s'><br><br>\n", $_SESSION["comment"]);


                        //PrintUpdateDataSetButton() has to be called before InsertUpdateDataSet()
                        //   to ensure that there are no problem with insert and $_SESSION["insert"]

                        //allow 'insert/get dataset' in case no error is found
                        if ($numerr!=0)
                        {
                            printf("Remark: Only data sets without errors can be inserted.<br>");
                            printf("<font color='red'>Your data set has %s error(s).</font><br><br>", $numerr);
                            PrintUpdateDataSetButton();
                        }
                        else
                        {
                            PrintUpdateDataSetButton();

                            if ($_SERVER['PHP_AUTH_USER']=="MAGIC")
                                printf("<br><br>\n Remark: As user MAGIC you are not allowed to insert data sets into the database. For an own account, please contact datacenter@astro.uni-wuerzburg.de <br><br>\n");
                            else
                                InsertUpdateDataSet($values, $dataset);

                            printf("<input type='button' value='Get Data Set File' onClick='self.location.href=\"%s&fSendTxt=2\"'>&nbsp;&nbsp;&nbsp;\n", GetClearedURL());
                        }


                        //print table with more information on dataset
                        printf("<br><br><br>\n");
                        printf("<table><tr><td colspan='2'><b>More Information on Your Data Set:<b></td></tr>");
                        printf("<tr><td>Next DataSet# in DB: </td><td>%s</td></tr>", $dataset);
                        printf("<tr><td>DataSet# for data set file: </td><td>%s</td></tr>", empty($_SESSION["AnalysisNumber"])?"1":$_SESSION["AnalysisNumber"]);
                        printf("<tr><td valign='top'>SequencesOn:</td><td> %s</td></tr>", $_SESSION["sequon"]);
                        if ($_SESSION["sequoff"]!="")
                        {
                            printf("<tr><td valign='top'>SequencesOff:</td><td> %s</td></tr>", $_SESSION["sequoff"]);
                            printf("<tr><td>Scale factor (On/Off): </td><td>%0.2f</td></tr>", $scale);
                        }
                        printf("<tr><td>SourcenamesOn [%s]: </td><td>%s</td></tr>", str_word_count($sourceson, 0, "1234567890+"), $sourceson);
                        if ($_SESSION["sequoff"]!="")
                            printf("<tr><td>SourcenamesOff [%s]: </td><td>%s</td></tr>", str_word_count($sourcesoff, 0, "1234567890+"), $sourcesoff);
                        printf("</table>\n");

                        printf("<table border='1'>\n");
                        printf("<tr><td>Value</td><td>On</td>");
                        if (!empty($off))
                            printf("<td>Off</td></tr>\n");
                        foreach($values as $name => $varname)
                        {
                            printf("<tr><td>%s</td>\n", $name);
                            foreach($vals as $num => $val)
                                if (!empty(${$val}))
                                    printf("<td>%s</td>\n", $varname[$val]);
                            printf("</tr>\n");
                        }
                        printf("</table>\n");
                    }
                }
                else
                {
                    if ($_SESSION["user"]!=$_SESSION["olduser"] && !empty($_SESSION["olduser"]))
                        printf("Remark: You (userkey %s) are not allowed to change data set # %d (created by userkey %s).<br> \n",
                               $_SESSION["user"], $_SESSION["SelectedDataSet"], $_SESSION["olduser"]);
                    else
                    {
                        printf("<input type='submit' value='Store Selection'><br><br>\n");
                        printf("Remark: To store your selection for the data set, you have to press 'Store Selection'<br> \n");
                    }

                }

                printf("</div>\n");
                printf("</td>\n");
                printf("</tr>\n");

                printf("</form>\n");
            }
            else
                PrintText($result0);

            mysql_free_result($result0);
            mysql_free_result($result1);
        }
        mysql_close($db_id);

        if ($html=="1")
            PrintSubmittedQuery($query0, $db, "");
    }

    include ("include.php");
    include ("menu.php");
    include ("db.php");
    include ("magicdefs.php");

//    $debug="yes";
    session_start();
    if ($debug)
    {
        echo "GET: ";
        print_r($_GET);
        echo " <br>";

        echo "POST: ";
        print_r($_POST);
        echo " <br>";
    }

    // set values given by $_GET
    foreach ($_GET as $element => $value)
        $_SESSION[$element]=$value;

    // unset values in case of 'Reset'
    if (empty($_GET))
        foreach($_SESSION as $element => $value)
            unset($_SESSION[$element]);

    // set values given by $_POST
    if (!empty($_POST))
        foreach ($_POST as $element => $value)
            $_SESSION[$element]=$value;
    else
        unset($_SESSION["SelectAllSequForDS"]);

    if ($debug)
        print_r($_SESSION);
    /*
    if ($_SESSION["DisplaySelected"]=="yes")
    {
        unset($_SESSION["fNumStart"]);
        $_GET["fNumStart"]=0;
    }
    */
    $_SESSION["sequon"]="";
    $_SESSION["sequoff"]="";
    foreach($_SESSION as $key => $val)
    {
        if ($val=="ON" && ereg("^DSSeq[0-9]*$", $key))
            $_SESSION["sequon"].=str_replace("DSSeq", " ", $key);

        if ($val=="Off" && ereg("^DSSeq[0-9]*$", $key))
            $_SESSION["sequoff"].=str_replace("DSSeq", " ", $key);

        if ($val=="Not" && ereg("^DSSeq[0-9]*$", $key))
            unset($_SESSION[$key]);
    }
    if (empty($_SESSION["sequon"]) && empty($_SESSION["sequoff"]))
        unset($_SESSION["DisplaySelected"]);

    ini_set("display_errors", "On");
    ini_set("mysql.trace_mode", "On");

    if (!isset($_SERVER['PHP_AUTH_USER']))
    {
        header('WWW-Authenticate: Basic realm="Build Datasets"');
        header('HTTP/1.0 401 Unauthorized');
        return;
    }
    else
    {
        $db_id = mysql_connect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error:<br>");
            printf("%s<br>", mysql_error());
            die("");
        }

        $userquery   = "SELECT fUserName, fPassword, fUserKEY FROM " . $db . ".User ORDER BY fUserKEY";
        $userresult  = mysql_query($userquery);
        if (!$userresult)
            echo "query failed";
        $validuser="no";
        while ($userrow = mysql_fetch_assoc($userresult))
        {
//            echo crypt($_SERVER['PHP_AUTH_PW']). "<br>";
//            echo "with crypt: " . crypt($_SERVER['PHP_AUTH_PW'], $userrow["fPassword"]) . " for user " . $userrow["fUserName"]. "<br>";
            if (crypt($_SERVER['PHP_AUTH_PW'], $userrow["fPassword"])==$userrow["fPassword"]
                && $_SERVER['PHP_AUTH_USER']==$userrow["fUserName"])
            {
                $validuser="yes";
                $_SESSION["user"]=$userrow["fUserKEY"];
            }
        }
        mysql_free_result($userresult);
        if ($validuser=="no")
        {
            printf("<br>Password or username incorrect<br>");
            return;
        }
    }

    if (!empty($_GET["fSendTxt"]))
    {
        header("Content-Type: application/octet");
        switch($_GET["fSendTxt"])
        {
        case 1:
            header("Content-Disposition: attachment; filename=query-result.txt");
            PrintPage("0", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $limitsmean, $limitsmin, $limitsmax, $needs);
            break;
        case 2:
            header("Content-Disposition: attachment; filename=dataset_".str_replace(" ", "_", $_SESSION["name"]).".txt");
            PrintDataSetFile();
        }
        unset($_SESSION["fSendTxt"]);
    }
    else
    {
        echo (file_get_contents("index-header.html"));

        $environment = sizeof($_GET);

        // Find out whether it is the first call to the php script
        $first = empty($_GET["fRunMin"]) && empty($_GET["fRunMax"]);
        InitBuildDataSets($first);
        if (empty($_GET["fPrintTable"]))
            PrintForm($host, $user, $pw, $db, $limitsmean, $limitsmin, $limitsmax, $alias);

        if ($environment==0)
        {
            printf("No query submitted yet.<br><br>\n");
            printf("Either query above, or you have also the option to start with an already existing data set: <br>\n");
            printf("Dataset: &nbsp;<input name='SelectedDataSet' type='text' size='8' maxlength='8' value='%s'><br><br>\n", $_SESSION["SelectedDataSet"]);
            printf("How do you want to process this data set? <br>\n");
            printf("<input type='radio' name='DataSetSelection' value='SelectSequences' %s>Use data set as starting point.<br>\n",
                   $_SESSION["DataSetSelection"]=="SelectSequences"?"checked":"");
            printf("<input type='radio' name='DataSetSelection' value='UpdateDataSet' %s>Update data set in database.<br><br>\n",
                   $_SESSION["DataSetSelection"]=="UpdateDataSet"?"checked":"");
            printf("<input type='submit' value='Query'><br>\n");
            printf("</form>\n");
        }
        else
        {
            printf("</form>\n");
            if (empty($_SESSION["DataSetSelection"]) && !empty($_SESSION["SelectedDataSet"]))
            {
                printf("Remark: You have inserted a data set number (%s), but not selected what you want to do with it. <br>\n",
                       $_SESSION["SelectedDataSet"]);
                printf("Please press 'Reset'. Then to choose a data set and what you want to do with it or simply 'Query Table' to start a new data set.<br>\n");
            }
            else
            {

                if (empty($_GET["fPrintTable"]))
                    PrintPage("1", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $limitsmean, $limitsmin, $limitsmax, $needs);
                else
                    PrintPage("2", $host, $user, $pw, $db, $alias, $rightalign, $checkwhere, $checkgroup, $checkstatusgroup, $limitsmean, $limitsmin, $limitsmax, $needs);
            }
        }

        echo (file_get_contents("index-footer.html"));
    }

    if (!empty($_SESSION["DataSetSelection"]))
        $_SESSION["DataSetAcknowledged"]="yes";

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");

?>
