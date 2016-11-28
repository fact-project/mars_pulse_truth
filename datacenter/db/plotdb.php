<?php
{
    function GetSequencesFromDataSet($onoff)
    {
        include("db.php");
        $db_id = mysql_pconnect($host, $user, $pw);
        if ($db_id==FALSE)
        {
            printf("mysql_connect returned the following error: %s\n", mysql_error());
            die("");
        }
        mysql_select_db($db);
        $query="SELECT fSequenceFirst FROM DataSetSequenceMapping WHERE fDataSetNumber IN (";
        $query .= str_replace(" ", ",", trim($_GET["Set"])) . ") AND fOnOff=" . $onoff;
//        printf("Q: %s<br>", $query);
        $result=mysql_query($query, $db_id);

        $sequences="";
        while ($row = mysql_fetch_row($result))
            $sequences.= $row[0] . " ";

        mysql_free_result($result);
        mysql_close($db_id);

        $text="";
        if (!empty($sequences))
            $text.= str_replace(" ", "+", trim($sequences));
        return $text;
    }
  
    function PrintDiv($host, $user, $pw, $db, $column, $tables, $num)
    {
        $list=GetList($host, $user, $pw, $db, $column, $tables);

        if (($_GET["prim"]==$column && empty($num))
            || ($_GET["prim"]==$column && $num==3)
            || ($_GET["prim"]==$column && $num==4)
            || ($_GET["prim"]==$column && $_GET["plot"]=="val" && $num==2)
            || (empty($_GET["prim"]) && $column=="fSequenceFirst"))
            printf("<div id='%s%s' style='display:inline'>\n", $column, $num);
        else
            printf("<div id='%s%s' style='display:none'>\n", $column, $num);

        if ($num==2)
            $vs="vs";

        printf("<select name='%sCol%s' size='1' class='Width'>\n", $column, $num);
        foreach($list as $column2)
            if ($_GET[$column."Col".$num]==$column2)
                printf("<option value='%s' selected>%s %s</option>\n", $column2, $vs, $column2);
            else
                printf("<option value='%s'>%s %s</option>\n", $column2, $vs, $column2);

        printf("</select>\n<br>\n");
        printf("</div>\n");
    }

    ini_set("display_errors", "On");
    ini_set("mysql.trace_mode", "On");

    echo (file_get_contents("index-header.html"));

    include("plotinclude.php");

    //init
    if (empty($_GET["plot"]))
        $_GET["plot"]="";
    if (empty($_GET["primstart"]))
        $_GET["primstart"]="";
    if (empty($_GET["primstop"]))
        $_GET["primstop"]="";
    if (empty($_GET["prim"]))
        $_GET["prim"]="";
    if (empty($_GET["fSequenceFirstCol"]))
        $_GET["fSequenceFirstCol"]="";
    if (empty($_GET["fSequenceFirstCol2"]))
        $_GET["fSequenceFirstCol2"]="";
    if (empty($_GET["fSequenceFirstCol3"]))
        $_GET["fSequenceFirstCol3"]="";
    if (empty($_GET["fRunNumberCol"]))
        $_GET["fRunNumberCol"]="";
    if (empty($_GET["fRunNumberCol2"]))
        $_GET["fRunNumberCol2"]="";
    if (empty($_GET["fRunNumberCol3"]))
        $_GET["fRunNumberCol3"]="";
    if (empty($_GET["fDataSetNumberCol"]))
        $_GET["fDataSetNumberCol"]="";
    if (empty($_GET["fDataSetNumberCol2"]))
        $_GET["fDataSetNumberCol2"]="";
    if (empty($_GET["fDataSetNumberCol3"]))
        $_GET["fDataSetNumberCol3"]="";
    if (empty($_GET["Start"]))
        $_GET["Start"]="";
    if (empty($_GET["Stop"]))
        $_GET["Stop"]="";
    if (empty($_GET["Start2"]))
        $_GET["Start2"]="";
    if (empty($_GET["Stop2"]))
        $_GET["Stop2"]="";
    if (empty($_GET["Start3"]))
        $_GET["Start3"]="";
    if (empty($_GET["Stop3"]))
        $_GET["Stop3"]="";
    if (empty($_GET["Start4"]))
        $_GET["Start4"]="";
    if (empty($_GET["Stop4"]))
        $_GET["Stop4"]="";
    if (empty($_GET["Set"]))
        $_GET["Set"]="";
    if (empty($_GET["Set2"]))
        $_GET["Set2"]="";
    if (empty($_GET["set"]))
        $_GET["set"]="";

    if (empty($_GET["x"]))
        $totalx=640+410+60;
    else
        $totalx=$_GET["x"]+410+60;
    
    printf("<center>\n<table cellpadding='0' width='%s' border='0'>\n", $totalx);
    printf("<tr><td align='left'>\n");
    printf("<form style='margin-bottom:0;' action='plotdb.php' METHOD='GET'>\n");

    printf("&nbsp;<input type='radio' name='plot' value='prim' onclick='hidepulldown2()' %s> vs primary &nbsp;", $_GET["plot"]=="prim" ? "checked" : "");
    printf("&nbsp;<input type='radio' name='plot' value='val' onclick='showpulldown2()' %s> vs value <br><br>", ($_GET["plot"]=="val" || empty($_GET["plot"])) ? "checked" : "");

    printf("<table>\n");
    printf("<tr><td>\n");
    printf("<select name='prim' size='1' class='Width'>\n");
    foreach($prims as $primary => $column)
        if ($_GET["prim"]==$column)
            printf("<option onclick=\"showpulldown('%s')\" value='%s' selected>%s</option>\n", $column, $column, $primary);
        else
            printf("<option onclick=\"showpulldown('%s')\" value='%s'>%s</option>\n", $column, $column, $primary);
    printf("</select>\n");
    printf(" </td><td valign='top'>\n ");
    printf(" from&nbsp;<input name='primstart' type='text' size='8' maxlength='8' value='%s'>", $_GET["primstart"]);
    printf(" to&nbsp;<input name='primstop' type='text' size='8' maxlength='8' value='%s'>", $_GET["primstop"]);
    printf("</td>\n</tr>\n");

    printf("<tr><td>\n");
    foreach($prims as $primary => $column)
        PrintDiv($host, $user, $pw, $db, $column, $tables, "");
    printf(" </td><td valign='top'>\n <div id='Range' style='display:inline'>\n ");
    printf(" from&nbsp;<input name='Start' type='text' size='8' maxlength='8' value='%s'>", $_GET["Start"]);
    printf(" to&nbsp;<input name='Stop' type='text' size='8' maxlength='8' value='%s'>\n", $_GET["Stop"]);
    printf("</div></td>\n</tr>\n");

    printf("<tr><td>\n");
    foreach($prims as $primary => $column)
        PrintDiv($host, $user, $pw, $db, $column, $tables, "2");
//    if ($_GET["plot"]=="val")
    if ($_GET["plot"]=="val" || empty($_GET["plot"]))
        printf(" </td><td valign='top'>\n <div id='Range2' style='display:inline'>\n ");
    else
        printf(" </td><td valign='top'>\n <div id='Range2' style='display:none'>\n ");
    printf(" from&nbsp;<input name='Start2' type='text' size='8' maxlength='8' value='%s'>", $_GET["Start2"]);
    printf(" to&nbsp;<input name='Stop2' type='text' size='8' maxlength='8' value='%s'>\n", $_GET["Stop2"]);
    printf("</div></td>\n</tr>\n");

    printf("<tr>\n<td colspan='2'>\nLimits</td>\n</tr>\n<tr>\n<td>\n");
    foreach($prims as $primary => $column)
        PrintDiv($host, $user, $pw, $db, $column, $tables, "3");
    printf(" </td><td valign='top'>\n <div id='Range3' style='display:inline'>\n ");
    printf(" from&nbsp;<input name='Start3' type='text' size='8' maxlength='8' value='%s'>", $_GET["Start3"]);
    printf(" to&nbsp;<input name='Stop3' type='text' size='8' maxlength='8' value='%s'>\n", $_GET["Stop3"]);
    printf("</div></td>\n</tr>\n");

    printf("<tr>\n<td>\n");
    foreach($prims as $primary => $column)
        PrintDiv($host, $user, $pw, $db, $column, $tables, "4");
    printf(" </td><td valign='top'>\n <div id='Range4' style='display:inline'>\n ");
    printf(" from&nbsp;<input name='Start4' type='text' size='8' maxlength='8' value='%s'>", $_GET["Start4"]);
    printf(" to&nbsp;<input name='Stop4' type='text' size='8' maxlength='8' value='%s'>\n", $_GET["Stop4"]);
    printf("</div></td>\n</tr>\n");

    printf("</table>\n");
    printf("<br>\n");

    printf("&nbsp;<input type='radio' name='set' value='sequences' onclick='showset2()' %s> sequences &nbsp;\n", $_GET["set"]=="sequences" ? "checked" : "");
    printf("&nbsp;<input type='radio' name='set' value='datasets' onclick='hideset2()' %s> datasets <br>\n", $_GET["set"]=="datasets" ? "checked" : "");

    printf("<div id='set1' style='display:inline'>Set1: <input name='Set' type='text' size='20' maxlength='150' value='%s'>\n</div>\n<br>\n", $_GET["Set"]);
    if (empty($_GET["set"]) || $_GET["set"]=="sequences")
        printf("<div id='set2' style='display:inline'>Set2: <input name='Set2' type='text' size='20' maxlength='100' value='%s'>\n</div>\n<br>\n", $_GET["Set2"]);
    else
        printf("<div id='set2' style='display:none'>Set2: <input name='Set2' type='text' size='20' maxlength='150' value='%s'>\n</div>\n<br>\n", $_GET["Set2"]);

    printf("<br>\nSource&nbsp;(<A HREF=\"regexp.html\">regexp</A>)&nbsp;<input name=\"fSourceN\" type=\"text\" size=\"15\" maxlength=\"15\" value=\"");
    if (!empty($_GET["fSourceN"]))
        printf("%s", $_GET["fSourceN"]);
    printf("\">\n<br>\n");

    printf("<br>\nPlot format: &nbsp; x = <select name='x' size='1'>\n");
    $x = array( 740, 640, 540, 440);
    foreach($x as $key => $el)
        if ($_GET["x"]==$el || (empty($_GET["x"]) && $el==640))
            printf("<option value='%s' selected>%s</option>\n", $el, $el);
        else
            printf("<option value='%s'>%s</option>\n", $el, $el);
    printf("</select>\n ");

    printf("&nbsp; y = <select name='y' size='1'>\n");
    $y= array( 600, 500, 400, 300);
    foreach($y as $key => $el)
        if ($_GET["y"]==$el || (empty($_GET["y"]) && $el==400))
            printf("<option value='%s' selected>%s</option>\n", $el, $el);
        else
            printf("<option value='%s'>%s</option>\n", $el, $el);
    printf("</select>\n");

    printf("<br><br><input class='Width' type='submit' value='Query Plot'> &nbsp;\n");
    printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"plotdb.php\"'><br>\n");

    printf("</form>");
    printf("</td>\n");

//    printf("<td align='right' width='640px' height='400px'>\n");
    printf("<td align='right'>\n");
    if (!empty($_GET))
    {
        $img="plot.php?par=hallo";
        foreach($_GET as $var => $val)
        {
            if ($var=="Set2" && $_GET["set"]=="datasets")
                continue;
            if ($var=="Set" && $_GET["set"]=="datasets" && !empty($val))
            {
                $img.= "&amp;Set=" . GetSequencesFromDataSet("1");
                $img.= "&amp;Set2=" . GetSequencesFromDataSet("2");
                continue;
            }
            $img.="&amp;".$var ."=".$val;
        }
//        printf("link: %s<br>", $img);
        printf("<img src='%s' alt='plot'>", $img);
    }
    printf("</td></tr>\n");
    printf("<tr><td colspan='2'>\n");
    printf("<br>some remarks:\n<ul>");
    printf("<li>For Runs plotting the whole range (i.e. without giving a range or when giving a too large range) \n");
    printf("doesn't work with ChartDirector.</li>\n");
    printf("<li>In Set1 and Set2 Sets can be given: <br>\n ");
    printf("If 'sequences' is selected e.g. on sequences in Set1 (displayed in blue) and off sequences in Set2 \n");
    printf("(diplayed in red). <br>\n ");
    printf("If 'datasets' is selected you can enter a dataset number as Set1. The on data is displayed in blue, \n");
    printf("the off data in red. <br>\n");
    printf("&nbsp;&nbsp;&nbsp;Please give the numbers with space as separator (e.g. 40004 40022).<br>\n</<li>\n");
    printf("<li>0 is not accepted in the ranges fields, please write .0 or 00</li>\n");
    printf("<li>Be aware, that plotting timestamps, e.g. fRunStart, and strings, e.g. fComment doesn't work. </li>\n");
    printf("</ul>\n</td>\n</tr>\n");

    /*
    printf("<tr><td colspan='2'>\n");
    $query=GetQuery($tables, $joins, "");
    printf("Q: %s<br>", $query);
    if (!empty($_GET["Set"]))
    {
        $query=GetQuery($tables, $joins, "Set");
        printf("Q: %s<br>", $query);
    }
    if (!empty($_GET["Set2"]))
    {
        $query=GetQuery($tables, $joins, "Set2");
        printf("Q: %s<br>", $query);
    }
    printf("</td></tr>");
    */

    printf("</table>\n");
    printf("</center>\n");

    echo (file_get_contents("index-footer.html"));

    ini_set("display_errors", "Off");
    ini_set("mysql.trace_mode", "Off");
}
?>
