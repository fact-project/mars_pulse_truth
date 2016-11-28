<?php

function EnumQuery($name)
{
    $var  = $name . "Enum";
    $txt  = "";
    switch ($_GET[$var])
    {
    case 0: $txt .= ""; break;
    case 1: $txt .= $name . "='yes' AND "; break;
    case 2: $txt .= $name . "='no' AND "; break;
    case 3: $txt .= ""; break;
    }
    return $txt;
}

function PrintEnumMenu($name, $text)
{
    $var = $name . "Enum";

    if ($_GET[$name]=="On")
        $checked = "checked";
    else
        $checked = "";

    printf("    <input type='checkbox' name='%s' value='On' %s>%s\n", $name, $checked, $text);
    printf("<BR>");

    printf(" <select name='%s'>\n", $var);

    $status = array
        ( 0 => "all",
          1 => "yes",
          2 => "no",
          3 => "group by"
        );

    $stat=$_GET[$var];
    for ($i=0; $i<4; $i++)
    {
        if ($stat==$i)
            printf("<option value='%d' selected>%s</option>\n", $i, $status[$i]);
        else
            printf("<option value='%d'>%s</option>\n", $i, $status[$i]);
    }

    printf(" </select>\n");
    printf(" &nbsp;&nbsp;&nbsp;\n");

}

function StatusQuery($name, $timelimits)
{
    if (empty($timelimits[$name]))
        $timelimit="12";
    else
        $timelimit=$timelimits[$name];
    $var  = $name . "Status";
    $txt  = "";
    switch ($_GET[$var])
    {
    case 0://all
        $txt .= " ";
        break;
    case 1://done and avail
        $txt .= " NOT ISNULL(" . $name . "Status.fStartTime) AND "
            ." NOT ISNULL(" . $name . "Status.fStopTime) AND "
            ." NOT ISNULL(" . $name . "Status.fAvailable) AND "
            ." ISNULL(" . $name . "Status.fReturnCode) AND ";
        break;
    case 2://done
        $txt .= " NOT ISNULL(" . $name . "Status.fStartTime) AND "
            ." NOT ISNULL(" . $name . "Status.fStopTime) AND "
            ." ISNULL(" . $name . "Status.fAvailable) AND "
            ." ISNULL(" . $name . "Status.fReturnCode) AND ";
        break;
    case 3://not done
        $txt .= " ISNULL(" . $name . "Status.fStartTime) AND "
            ." ISNULL(" . $name . "Status.fStopTime) AND "
            ." ISNULL(" . $name . "Status.fReturnCode) AND ";
        break;
    case 4://not to be done
        $txt .= $name ."Status.fStartTime='1970-01-01 00:00:00' AND "
            . $name ."Status.fStopTime='1970-01-01 00:00:00' AND ";
        break;
    case 5://running
        $txt .= " NOT ISNULL(" . $name . "Status.fStartTime) AND "
            ." DATE_SUB(Now(),INTERVAL " . $timelimit . " HOUR) < " . $name . "Status.fStartTime AND "
            ." ISNULL(" . $name . "Status.fStopTime) AND "
            ." ISNULL(" . $name . "Status.fReturnCode) AND ";
        break;
    case 6://failed
        $txt .= " NOT ISNULL(" . $name . "Status.fStartTime) AND "
            ." NOT ISNULL(" . $name . "Status.fStopTime) AND "
            ." NOT ISNULL(" . $name . "Status.fReturnCode) AND ";
        break;
    case 7://crashed
        $txt .= " NOT ISNULL(" . $name . "Status.fStartTime) AND "
            ." DATE_SUB(Now(),INTERVAL " . $timelimit . " HOUR) > " . $name . "Status.fStartTime AND "
            ." ISNULL(" . $name . "Status.fStopTime) AND "
            ." ISNULL(" . $name . "Status.fReturnCode) AND ";
        break;
    case 8://group by
        $txt .= "";
        break;
    }
    return $txt;
}

function PrintStatusMenu($name, $text)
{
    $var = $name . "Status";

    if ($_GET[$name]=="On")
        $checked = "checked";
    else
        $checked = "";

    printf("    <input type='checkbox' name='%s' value='On' %s>%s\n", $name, $checked, $text);
    printf("<BR>");

    printf(" <select name='%s'>\n", $var);

    $status = array
        ( 0 => "ALL",
          1 => "done and avail",
          2 => "done",
          3 => "not done",
          4 => "not to be done",
          5 => "running",
          6 => "failed",
          7 => "crashed",
          8 => "GROUP BY",
        );

    $stat=$_GET[$var];
    for ($i=0; $i<9; $i++)
    {
        if ($stat==$i)
            printf("<option value='%d' selected>%s</option>\n", $i, $status[$i]);
        else
            printf("<option value='%d'>%s</option>\n", $i, $status[$i]);
    }

    printf(" </select>\n");
    printf(" &nbsp;&nbsp;&nbsp;\n");

}

function PrintPullDown($host, $user, $pw, $db, $table, $name, $index, $descr)
{
    $db_id = mysql_connect($host, $user, $pw);
    if ($db_id==FALSE)
    {
            printf("mysql_connect returned the following error:<br>");
            printf("%s<br>", mysql_error());
            die("");
    }

    $query   = "SELECT " . $index . ", " . $name . " FROM " . $db . "." . $table . " ORDER BY " . $name;
    $result  = mysql_query($query);

    if (!$result)
    {
        printf("-N/A-");
        return;
    }

    $numrows = mysql_num_rows($result);

    if ($_GET[$name]=="On")
        $checked = "checked";
    else
        $checked = "";

    printf("    <input type='checkbox' name='%s' value='On' %s><A HREF='printtable.php?fTable=%s'>%s</A>\n", $name, $checked, $table, $descr);

    printf("   <BR>\n");

    printf("   <select name='%s' size='1' class='Width'>\n", $index);

    if (empty($_GET[$index]) || $_GET[$index]==0)
        printf("    <option value='0' selected>--- ALL ---</option>\n");
    else
        printf("    <option value='0'>--- ALL ---</option>\n");

    if (!empty($_GET[$index]) && $_GET[$index]==-1)
        printf("    <option value='-1' selected>--- GROUP BY ---</option>\n");
    else
        printf("    <option value='-1'>--- GROUP BY ---</option>\n");

    while ($row = mysql_fetch_row($result))
    {
        if (!empty($_GET[$index]) && $_GET[$index]==$row[0])
            printf("    <option value='%s' selected>%s</option>\n", $row[0], $row[1]);
        else
            printf("    <option value='%s'>%s</option>\n", $row[0], $row[1]);
    }
    printf("   </select>\n");
    printf("   &nbsp;\n", $index);
    
    mysql_free_result($result);

    mysql_close($db_id);
}

function GetMin($field, $table, $host, $user, $pw, $db)
{
    $db_id = mysql_connect($host, $user, $pw);
    if ($db_id==FALSE)
    {
            printf("mysql_connect returned the following error:<br>");
            printf("%s<br>", mysql_error());
            die("");
    }

    $query  = "SELECT MIN(" . $field . ") FROM " . $db . "." . $table;
    $result = mysql_query($query);
    if (!$result)
        return "0";

    $row = mysql_fetch_row($result);

    $min = $row[0];

    mysql_free_result($result);
    mysql_close($db_id);

    return $min;
}

function GetMax($field, $table, $host, $user, $pw, $db)
{
    $db_id = mysql_connect($host, $user, $pw);
    if ($db_id==FALSE)
    {
            printf("mysql_connect returned the following error:<br>");
            printf("%s<br>", mysql_error());
            die("");
    }

    $query  = "SELECT MAX(" . $field . ") FROM " . $db . "." . $table;
    $result = mysql_query($query);
    if (!$result)
        return "0";

    $row = mysql_fetch_row($result);

    $max = $row[0];

    mysql_free_result($result);
    mysql_close($db_id);

    return $max;
}

function GetMaxDate($field, $table, $host, $user, $pw, $db)
{
    $db_id = mysql_connect($host, $user, $pw);
    if ($db_id==FALSE)
    {
            printf("mysql_connect returned the following error:<br>");
            printf("%s<br>", mysql_error());
            die("");
    }

    $query  = "SELECT DATE_FORMAT(if(MAX(" . $field . ")<'13:00:00', MAX(" . $field . "), ADDDATE(MAX(" . $field . "), INTERVAL +1 DAY)), '%Y-%m-%d') FROM " . $db . "." . $table;
    $result = mysql_query($query);
    if (!$result)
        return "0";

    $row = mysql_fetch_row($result);

    $maxdate = $row[0];

    mysql_free_result($result);
    mysql_close($db_id);

    return $maxdate;
}

//for download of output
function PrintText($result0)
{
    while ($row0 = mysql_fetch_assoc($result0))
    {
        foreach ($row0 as $key => $element)
            printf("%s\t", $element);
        printf("\n");
    }
}

function PrintSubmittedQuery($query0, $db, $old)
{
    //diplay query on old websites
    if (empty($old))
        printf("<tr class='Block' id='showquery' style='display:none'><td>\n");
    else
        printf("<tr class='Block' id='showquery' style='display:block'><td>\n");
    printf("<b>DB:</b> %s <br>\n", $db);
    printf("<U><B>submitted query:</B></U><BR>%s<BR>\n", htmlspecialchars($query0));
    printf("</td></tr>\n");
}

function Checkbox($value, $text)
{
    if ($_GET[$value]=="On")
        $checked = "checked";
    else
        $checked = "";

    printf("<td><input type='checkbox' name='%s' value='On' %s>%s</td>\n", $value, $checked, $text);
}

function RadioButton($name, $value, $text)
{
    if ($_SESSION[$name]==$value)
        $checked = "checked";
    else
        $checked = "";

    printf("<td><input type='radio' name='%s' value='%s' %s>%s</td>\n", $name, $value, $checked, $text);
}

function CheckWhere($column)
{
    foreach ($_GET as $key => $element)
    {
        if ($key==$column)
        {
//            if ($element>0)
//                printf ("FIXED: %s<BR>", $column);
            return $element;
        }
    }
    return 0;
}

function CheckGroup($column)
{
    foreach ($_GET as $key => $element)
    {
        if ($key==$column)
        {
            //if ($element==-1)
            //    printf ("GROUP: %s<BR>", $column);
            return $element;
        }
    }
    return 0;
}

function CheckStatusGroup($column)
{
    foreach ($_GET as $key => $element)
        if ($key==$column."Status")
            if ($element==8)
                return -1;
    return 0;
}

function CheckEnumGroup($column)
{
    foreach ($_GET as $key => $element)
        if ($key==$column)
            if ($element==3)
                return -1;
    return 0;
}

function RemoveSortBy()
{
    $menu = "";

    $uri = $_SERVER["REQUEST_URI"];
    $pos = strpos($uri, "fSortBy");
    $amp3=FALSE;
    if ($pos!=FALSE)
    {
        $amp1 = substr($uri, 0, $pos-1);
        $amp2 = substr($uri, $pos);
        $amp3 = strchr($amp2, "&");

        $uri = $amp1;
    }

    return $uri;
}

function FindAlias($alias, $search)
{
    foreach ($alias as $key => $element)
        if ($element==$search)
            return $key;

    if ($search=="# Runs")
        return "NumRuns";
    if ($search=="# Sequ")
        return "NumSequ";
    if ($search=="# Datasets")
        return "NumDS";
    if ($search=="# days")
        return "NumDays";
    return $search;
    return "";
}

//function for button in builddatasets.php
function GetClearedURL($all)
{
    $url=$_SERVER["REQUEST_URI"];
    if ($all=="yes")
    {
        $url=str_replace("&DisplaySelected=yes",     "", $url);
        $url=str_replace("&DisplaySelected=no",      "", $url);
        $url=str_replace("&DisplaySelected=inverse", "", $url);
        //reset fNumStart
        $url=preg_replace("/&fNumStart[=][0-9]*/", "", $url);
    }
    $url=str_replace("&insert=yes", "", $url);
    $url=str_replace("&fSendTxt=2", "", $url);
    $url=str_replace("&fSendTxt=1", "", $url);
    $url=htmlspecialchars($url);
    return $url;
}

function PrintUpdateDataSetButton()
{
    if (empty($_SESSION["insert"]))
        printf("<input type='submit' value='Update Selection'>&nbsp;&nbsp;&nbsp;\n");
    else
        printf("<input type='button' value='Continue' onClick='self.location.href=\"%s\"'>&nbsp;&nbsp;&nbsp;\n", GetClearedURL());

}

//function for button in builddatasets.php
function PrintDisplaySequencesButtons()
{
    if (!empty($_SESSION["DataSetSelection"]) && empty($_SESSION["DataSetAcknowledged"]))
        return;

    if ((empty($_SESSION["DisplaySelected"]) || $_SESSION["DisplaySelected"]=="no")
        && !(empty($_SESSION["sequon"]) && empty($_SESSION["sequoff"])))
    {
        PrintUpdateDataSetButton();
        printf("<input type='button' value='Display Selected Sequences' onClick='self.location.href=\"%s&DisplaySelected=yes\"'>\n", GetClearedURL("yes"));
        printf("&nbsp;&nbsp;&nbsp;<input type='button' value='Display Not-Selected Sequences' onClick='self.location.href=\"%s&DisplaySelected=inverse\"'>\n", GetClearedURL("yes"));
        printf("<br><br>\n");
    }

    if ($_SESSION["DisplaySelected"]=="yes"
       && !(empty($_SESSION["sequon"]) && empty($_SESSION["sequoff"])))
    {
        PrintUpdateDataSetButton();
        printf("<input type='button' value='Display Not-Selected Sequences' onClick='self.location.href=\"%s&DisplaySelected=inverse\"'>\n", GetClearedURL("yes"));
        printf("&nbsp;&nbsp;&nbsp;<input type='button' value='Display All Sequences' onClick='self.location.href=\"%s&DisplaySelected=no\"'>\n", GetClearedURL("yes"));
        printf("&nbsp;&nbsp;&nbsp;<i>Currently only selected sequences are displayed.</i><br><br>\n");
    }

    if ($_SESSION["DisplaySelected"]=="inverse"
       && !(empty($_SESSION["sequon"]) && empty($_SESSION["sequoff"])))
    {
        PrintUpdateDataSetButton();
        printf("<input type='button' value='Display Selected Sequences' onClick='self.location.href=\"%s&DisplaySelected=yes\"'>\n", GetClearedURL("yes"));
        printf("&nbsp;&nbsp;&nbsp;<input type='button' value='Display All Sequences' onClick='self.location.href=\"%s&DisplaySelected=no\"'>\n", GetClearedURL("yes"));
        printf("&nbsp;&nbsp;&nbsp;<i>Currently only NOT selected sequences are displayed.</i><br><br>\n");
    }
}


function ReplaceInUri($name, $rows, $direction, $totalnumrows=0)
{
    //direction:
    // 0: Prev Link
    // 1: Next Link
    // 2: First Link
    // 3: Last Link

    $uri = htmlspecialchars($_SERVER["REQUEST_URI"]);
    //append string in case it is not in url
    if (!preg_match("/&fNumStart[=][0-9]*/", $uri))
        $uri.="&amp;fNumStart=";

    switch($direction)
    {
    case 0:
        $pos = $_GET["fNumStart"]-$rows;
        if ($pos<0)
            $pos=0;
        $link .= " <A HREF='" . preg_replace("/&amp;fNumStart[=][0-9]*/", "&amp;fNumStart=".$pos, $uri) . "'>< Prev</A> \n";
        break;
    case 1:
        //display link only if more results available
        if ($_GET["fNumStart"]+$rows==$totalnumrows)
            break;
        $pos = $_GET["fNumStart"]+$rows;
        $link .= " <A HREF='" . preg_replace("/&amp;fNumStart[=][0-9]*/", "&amp;fNumStart=".$pos, $uri) . "'>Next ></A> \n";
        break;
    case 2:
        $pos = 0;
        $link .= " <A HREF='" . preg_replace("/&amp;fNumStart[=][0-9]*/", "&amp;fNumStart=".$pos, $uri) . "'><< First</A> \n";
        break;
    case 3:
        //display link only if more results available
        if ($_GET["fNumStart"]+$rows==$totalnumrows)
            break;
        $pos = $totalnumrows-$rows+1;
        $link .= " <A HREF='" . preg_replace("/&amp;fNumStart[=][0-9]*/", "&amp;fNumStart=".$pos, $uri) . "'>Last >></A> \n";
        break;
    }

    return $link;
}

function CreateMenu($rows, $totalnumrows)
{
    $menu = "";

    if (empty($_GET["fNumResults"]))
        return;

    if ($_GET["fNumStart"]!=0)
    {
        $menu .= ReplaceInUri("fNumStart", $rows, 2, $totalnumrows);
        $menu .= ReplaceInUri("fNumStart", $rows, 0, $totalnumrows);
    }

    $menu .= "&nbsp;&nbsp;&nbsp;---&nbsp;<B>";
    if (empty($_GET["fNumStart"]))
        $menu .= "0";
    else
        $menu .= $_GET["fNumStart"];
    $menu .= "</B>&nbsp;---&nbsp;&nbsp;&nbsp;\n";

    if ($rows==$_GET["fNumResults"])
    {
        $menu .= ReplaceInUri("fNumStart", $rows, 1, $totalnumrows);
        $menu .= ReplaceInUri("fNumStart", $rows, 3, $totalnumrows);
    }
    return $menu;
}

function PrintMagicTable($result0, $alias, $rightalign, $limitsmean, $limitsmin, $limitsmax, $result1, $form="")
{
    $row1 = mysql_fetch_assoc($result1);
    $totalnumrows=$row1["FOUND_ROWS()"];

    $col   = FALSE;
    $first = TRUE;

    $sigma = array
        (
         1    => "#33CC00",
         2    => "#FFFF66",
         3    => "#FF9900",
         5    => "#FF0000",
        );
    $okcolour="#006600";


    $menu = CreateMenu(mysql_num_rows($result0), $totalnumrows);

    if ($form)
    {
        printf("<form method='POST'>");
        PrintDisplaySequencesButtons();
    }
    printf("\n<center>\n");
    if (empty($_GET["fPrintTable"]))
        printf("%s\n", $menu);

    printf("<table BORDER='0' style='margin-top:1ex'>\n");
    $counter=0;
    while ($row0 = mysql_fetch_assoc($result0))
    {
        if ($first)
        {
            printf(" <tr BGCOLOR='#C0C0C0'>\n<td BGCOLOR='#F0F0F0'><img src='plus.png' alt='+' onClick='showalllines(%d)'></td>\n", mysql_num_rows($result0));
            $first = FALSE;
            if ($form)
            {
                printf("<td>ON<br><input type='radio' name='SelectAllSequForDS' value='ON' onclick='selectallsequences(\"ON\");' %s></td>\n",
                       $_SESSION["SelectAllSequForDS"]=="ON"?"checked":"");
                printf("<td>Off<br><input type='radio' name='SelectAllSequForDS' value='Off' onclick='selectallsequences(\"Off\");' %s></td>\n",
                       $_SESSION["SelectAllSequForDS"]=="Off"?"checked":"");
                printf("<td>Not<br><input type='radio' name='SelectAllSequForDS' value='Not' onclick='selectallsequences(\"Not\");' %s></td>\n",
                       $_SESSION["SelectAllSequForDS"]=="Not"?"checked":"");
            }
            foreach ($row0 as $key => $element)
            {
                $col = FindAlias($alias, $key);

                $ord="-";
                $issort = "";
                if (!empty($_GET["fSortBy"]) && substr($_GET["fSortBy"], 0, -1)==$col)
                {
                    if (substr($_GET["fSortBy"], -1)=="-")
                    {
                        $ord="+";
                        $issort="&nbsp;<IMG SRC='down.gif'>";
                    }
                    else
                        $issort="&nbsp;<IMG SRC='up.gif'>";
                }
                printf("  <th>&nbsp;<A HREF='%s&amp;fSortBy=%s%s'>%s</A>%s&nbsp;</th>\n",
                       htmlspecialchars(RemoveSortBy()), $col, $ord, $key, $issort);
            }
            printf(" </tr>\n\n");
        }

        $counter++;
        if (!$col)
            printf(" <tr id='line%s' BGCOLOR='#E0E0E0'>\n<td BGCOLOR='#F0F0F0'>\n<img id='line%sbutton' src='minus.png' alt='-' onClick='showhide(\"line%s\")'>\n</td>\n", $counter, $counter, $counter);
        else
            printf(" <tr id='line%s' BGCOLOR='#D0D0D0'>\n<td BGCOLOR='#F0F0F0'>\n<img id='line%sbutton' src='minus.png' alt='-' onClick='showhide(\"line%s\")'>\n</td>\n", $counter, $counter, $counter);
        $col = !$col;

        if ($form)
        {
            RadioButton("DSSeq".$row0["Sequ"],  "ON", "");
            RadioButton("DSSeq".$row0["Sequ"], "Off", "");
            RadioButton("DSSeq".$row0["Sequ"], "Not", "");
        }
        foreach ($row0 as $key => $element)
        {
            if (empty($rightalign[$key]))
                printf("  <td align='left' valign='top'>");
            else
                printf("  <td align='right' valign='top'>");

            $colour='#000000';
            //determine color of text in cell
            if (!empty($limitsmean))
            {
                foreach($limitsmean as $key2 => $element2)
                {
                    $mean=$key2 . "Mean";
                    $rms2=$key2 . "Rms";
                    if ($key==$alias[$element2] && !empty($_GET[$mean]) && !empty($_GET[$rms2]))
                    {
                        $colour=$okcolour;
                        foreach ($sigma as $margin => $newcolour)
                        {
                            $min=$_GET[$mean] - ($margin * $_GET[$rms2]);
                            $max=$_GET[$mean] + ($margin * $_GET[$rms2]);
                            if (!($min < $element && $element < $max))
                                $colour=$newcolour;
                        }
                    }
                }
            }
            if (!empty($limitsmin))
            {
                foreach($limitsmin as $key2 => $element2)
                {
                    $limit1=$key2 . "1";
                    $limit2=$key2 . "2";
                    if ($key==$alias[$element2] && !empty($_GET[$limit1]))
                    {
                        if ($colour=='#000000')
                            $colour=$okcolour;

                        if (!empty($_GET[$limit2]) && $_GET[$limit2] > $element)
                                $colour=$sigma[5];

                        if ($_GET[$limit1] > $element && $_GET[$limit2] <= $element)
                            $colour=$sigma[3];
                    }
                }
            }

            if (!empty($limitsmax))
            {
                foreach($limitsmax as $key2 => $element2)
                {
                    $limit1=$key2 . "1";
                    $limit2=$key2 . "2";
                    if ($key==$alias[$element2] && !empty($_GET[$limit1]))
                    {
                        if ($colour=='#000000')
                            $colour=$okcolour;

                        if (!empty($_GET[$limit2]) && $_GET[$limit2] < $element)
                                $colour=$sigma[5];

                        if ($_GET[$limit1] < $element && $_GET[$limit2] >= $element)
                            $colour=$sigma[3];
                    }
                }
            }
            if ($colour!='#000000' && (!empty($limitsmean) || !empty($limitsmin) || !empty($limitsmax)))
                printf("<font color='%s' style='font-weight:bold'>", $colour);

            //fill text in cell
            printf("&nbsp;%s&nbsp;</td>\n", str_replace("&ws;", " ", str_replace(" ", "&nbsp;", $element)));

            if ($colour!='#000000' && (!empty($limitsmean) || !empty($limitsmin) || !empty($limitsmax)))
                printf("</font>");
        }
        printf(" </tr>\n");
    }
    printf("</table>\n");

    /*
     $info = mysql_info();
     if (!empty($info))
     printf("%s<BR>\n", $info);
     */

    printf("<P><B>Number of displayed results: %d of %s in total</B><P><P>\n", mysql_num_rows($result0), $totalnumrows);
    if (empty($_GET["fPrintTable"]))
        printf("%s\n", $menu);
    printf("<P>\n");
    printf("</center>\n");

    if (!$form)
    {
        printf("</td>\n");
        printf("</tr>\n");
    }
    else
        PrintDisplaySequencesButtons();
}

?>
