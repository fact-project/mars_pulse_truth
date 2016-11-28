<?php

ini_set("display_errors", "On");

include("plotinclude.php");
include("db.php");

printf("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
printf("<html>\n<head>\n");
printf("<meta http-equiv='content-type' content='text/html; charset=ISO-8859-1'>\n");
printf("<title>show plots</title>\n");
printf("<link rel='stylesheet' type='text/css' href='lamouette.css'>\n");
printf("</head>\n");
printf("<body>\n");

//init
$source="";
if (!empty($_GET["source"]))
    $source=$_GET["source"];
$from="";
if (!empty($_GET["from"]))
    $from=$_GET["from"];
$to="";
if (!empty($_GET["to"]))
    $to=$_GET["to"];
$tabnum=1;
if (!empty($_GET["tabnum"]))
    $tabnum=$_GET["tabnum"];
$type2="calib";
if (!empty($_GET["type2"]))
    $type2=$_GET["type2"];
$plot="";
$nextseq=0;
$prevseq=0;
$seq=0;
if (!empty($_GET["prevseq"]) && !empty($_GET["prev"]))
    $seq=$_GET["prevseq"];
if (!empty($_GET["nextseq"]) && !empty($_GET["next"]))
    $seq=$_GET["nextseq"];
$next=0;
$prev=0;
$numseq=0;
//$sequences=array(0,10,20);
//$sequences=array();


if (!empty($from) || !empty($to))
{
    //get sequences from db
    $query="SELECT fSequenceFirst FROM Sequences ";
    if (!empty($from) && !empty($to))
        $query.="WHERE fSequenceFirst BETWEEN " . $from . " AND " . $to;
    else
    {
        if (!empty($from))
            $query.="WHERE fSequenceFirst > " . $from;
        if (!empty($to))
            $query.="WHERE fSequenceFirst < " . $to;
    }
    // WHERE has to be there as $from is given by default
    if (!empty($source))
        $query.=" AND fSourceKEY=" . $source;

    //connection to database
    $db_id = mysql_pconnect($host, $user, $pw);
    if ($db_id==FALSE)
    {
        printf("mysql_connect returned the following error: %s\n", mysql_error());
        die("");
    }
    mysql_select_db($db);

    $result=mysql_query($query, $db_id);
    while ($row = mysql_fetch_row($result))
        $sequences[] = $row[0];
    mysql_free_result($result);

    mysql_close($db_id);

    if (isset($sequences))
    {
        //get next, current and previous sequence
        foreach($sequences as $key => $sequ)
        {
            if ($seq==0)
                $seq=$sequ;
            if ($next==1)
            {
                $nextseq=$sequ;
                if ($prevseq!=0)
                    break;
                $next=0;
            }
            if ($sequ==$seq)
            {
                $next=1;
                continue;
            }
            $prevseq=$sequ;
        }
        $numseq=count($sequences);
        //in case a dataset consists of less than 3 sequences
        if ($numseq==2)
            $nextseq=$prevseq;
    }
}


$type=gettypename($type2);
$plot=getplotname($seq, $tabnum, $type, $type2);

printf("<form action='showplots-seq.php' method='GET'>\n");
printf("<table width='100%%' border='0'>\n<tr>\n");
PrintHomeHelp();
printf("<td align='center'>\n");
if (!empty($prevseq))
    printf("%d &nbsp;", $prevseq);
if ($numseq>1)
    printf("<input type='submit' value='<< Prev Plot' name='prev'>\n");
if (empty($from))
    $from=34099;//21.8.2004

$db_id = mysql_connect($host, $user, $pw);
if ($db_id==FALSE)
{
    printf("mysql_connect returned the following error:<br>");
    printf("%s<br>", mysql_error());
    die("");
}
    mysql_select_db($db);

$query="SELECT fSourceKEY, fSourceName FROM Source ORDER BY fSourceName";
$result=mysql_query($query);
if (!$result)
    printf("-N/A-");
printf("&nbsp;<select name='source' size='1' class='Width'>\n");
if (empty($source) || $source==0)
    printf("<option value='0' selected>--- ALL ---</option>\n");
else
    printf("<option value='0'>--- ALL ---</option>\n");
while ($row = mysql_fetch_row($result))
{
    if (!empty($source) && $source==$row[0])
        printf("<option value='%s' selected>%s</option>\n", $row[0], $row[1]);
    else
        printf("<option value='%s'>%s</option>\n", $row[0], $row[1]);
}
printf("</select>\n");
mysql_free_result($result);
mysql_close($db_id);

printf("from <input type='text' name='from' size='6' maxlength='8' value='%s'>\n", $from);
printf(" to <input type='text' name='to' size='6' maxlength='8' value='%s'>\n", $to);
PrintType2PullDown($type2);
printf("<input type='text' name='tabnum' size='2' maxlenght='2' value='%s'>\n", $tabnum);
printf("<input type='hidden' name='prevseq' size='2' maxlenght='2' value='%s'>\n", $prevseq);
printf("<input type='hidden' name='nextseq' size='2' maxlenght='2' value='%s'>\n", $nextseq);
if ($numseq>1)
    printf("<input type='submit' value='Next Plot >>' name='next'>\n");
else
    printf("<input type='submit' value='Plot' name='next'>\n");
if (!empty($nextseq))
    printf("&nbsp;%d ", $nextseq);
if ($numseq>0)
{
    printf("&nbsp; [ %d sequence", $numseq);
    if ($numseq>1)
        printf("s");
    print(" ] \n");
}
printf("</td><td align='right'>\n");
printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"showplots-seq.php\"'>\n");
printf("</td>\n</tr>\n<tr>\n<td align='center' colspan='3'>\n");
if (!empty($seq))
{
    printf("<div onmouseover=\"this.innerHTML='%s'\" onmouseout=\"this.innerHTML='%s'\"> %s </div>", getinfofromdb($seq), $plot, $plot);
    printf("<img src='%s'>", $plot);
}
else
    printf("Choose a source and/or a range of sequences or just click 'Plot' for going through all sequences.");
printf("</td>\n</tr>\n</table>\n");
printf("</form>\n");
printf("</body>\n");
printf("</html>\n");

ini_set("display_errors", "Off");

?>
