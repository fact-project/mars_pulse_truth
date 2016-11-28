<?php

include("plotinclude.php");

printf("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
printf("<html>\n<head>\n");
printf("<meta http-equiv='content-type' content='text/html; charset=ISO-8859-1'>\n");
printf("<title>show plots</title>\n");
printf("<link rel='stylesheet' type='text/css' href='lamouette.css'>\n");
printf("</head>\n");
printf("<body>\n");

//init
$seq=0;
if (!empty($_GET["seq"]))
    $seq=str_replace(" ", "", $_GET["seq"]);
$tabnum=1;
if (!empty($_GET["tabnum"]))
    $tabnum=$_GET["tabnum"];
$type2="calib";
if (!empty($_GET["type2"]))
    $type2=$_GET["type2"];
$next=0;
$prev=0;

if (!empty($seq))
{
    //decision whether next or previous plot has been chosen
    if (!empty($_GET["next"]))
        $tabnum=$tabnum+1;
    if (!empty($_GET["prev"]))
        $tabnum=$tabnum-1;

    //be careful: this has to be adapted in case tabs are removed or new tabs are added
    //number of tabs for calib*.root, signal*.root and star*.root
    if ($seq > 200000)
        $signaltabs=12;
    else
        $signaltabs=14;
    $calibtabs=11;
    $startabs=20;

    //in case the tab does not exist, because it's one the far
    //go to next type
    if (($type2=="calib" && $tabnum==($calibtabs+1))
        || ($type2=="signal" && $tabnum==($signaltabs+1))
        || ($type2=="star" && $tabnum==($startabs+1)))
    {
        $tabnum=1;
        switch ($type2)
        {
        case "calib":
            $type2="signal";
            break;
        case "signal":
            $type2="star";
            break;
        case "star":
            $type2="calib";
            break;
        }
    }
    //in case the tab does not exist, because it's 0
    //go to previous type
    if ($tabnum==0)
    {
        switch($type2)
        {
        case "calib":
            $type2="star";
            $tabnum=$startabs;
            break;
        case "signal":
            $type2="calib";
            $tabnum=$calibtabs;
            break;
        case "star":
            $type2="signal";
            $tabnum=$signaltabs;
            break;
        }
    }
    //get link for plot
    $type=gettypename($type2);
    $plot=getplotname($seq, $tabnum, $type, $type2);

    $next=$tabnum+1;
    $prev=$tabnum-1;
}
printf("<form action='showplots.php' method='GET'>\n");
printf("<table width='100%%' border='0'>\n<tr>\n");
PrintHomeHelp();
printf("<td align='center'>\n");
if (!empty($seq))
    printf("<input type='submit' value='<< %d Prev Plot' name='prev'>\n", $prev);
printf("<input type='text' name='seq' size='10' maxlength='10' value='%s'>\n", $seq);
PrintType2PullDown($type2);
printf("<input type='text' name='tabnum' size='2' maxlenght='2' value='%s'>\n", $tabnum);
printf("<input type='submit' value='Show Plot'>\n");
if (!empty($seq))
    printf("<input type='submit' value='Next Plot %d >>' name='next'>\n", $next);

printf("</td>\n</tr>\n<tr>\n<td align='center' colspan='2'>\n");
if (!empty($seq))
{
    printf("<div onmouseover=\"this.innerHTML='%s'\" onmouseout=\"this.innerHTML='%s'\"> %s </div>", getinfofromdb($seq), $plot, $plot);
    printf("<img src='%s'>", $plot);
}
else
    printf("You have to insert a sequence number into the first field.");
printf("</td>\n</tr>\n</table>\n");
printf("</form>\n");
printf("</body>\n");
printf("</html>\n");

?>
