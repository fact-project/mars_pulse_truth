<?php

ini_set("display_errors", "On");

include("plotinclude.php");
//print header
printf("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
printf("<html>\n<head>\n");
printf("<meta http-equiv='content-type' content='text/html; charset=ISO-8859-1'>\n");
printf("<title>show plots</title>\n");
printf("<link rel='stylesheet' type='text/css' href='lamouette.css'>\n");
printf("</head>\n");
printf("<body>\n");

//init
$ds=0;
if (!empty($_GET["ds"]))
    $ds=$_GET["ds"];
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

//check if dataset file is existing
if (!empty($ds))
{
    $num=sprintf("%08d",$ds);
    $num2=substr($num,0,5);
    $datasetfile="./datasets/" . $num2 . "/dataset" . $num . ".txt";
    if (!file_exists($datasetfile))
    {
        printf("Sorry, there's no file for dataset %s available.", $ds);
        //set dataset and sequence numbers to 0 for next checks
        $ds=0;
        $seq=0;
    }
}

//in case of a valid dataset
if (!empty($ds))
{
    //get sequences from dataset file
    // alternative: query it from db
    $dataset=file_get_contents($datasetfile);
    $onpos=strpos($dataset, "SequencesOn:");
    $offpos=strpos($dataset, "SequencesOff:");
    $possource=strpos($dataset, "SourceName:");
    $sequoff="";
    if (!empty($offpos))
        $sequoff=split(" ", trim(substr($dataset, $offpos+13, $possource-($offpos+13))));
    if (empty($offpos))
        $offpos=$possource;
    $sequon=split(" ", trim(substr($dataset, $onpos+12, $offpos-($onpos+12))));
    $sequences=$sequon;
    if (!empty($sequoff))
        $sequences=array_merge($sequences,$sequoff);
    if ($seq!=0 && !in_array($seq, $sequences))
        $seq=0;

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

//get link for plot
$type=gettypename($type2);
$plot=getplotname($seq, $tabnum, $type, $type2);

//print form for choosing dataset and plots
printf("<form action='showplots-ds.php' method='GET'>\n");
printf("<table width='100%%' border='0'>\n");
printf("<tr><td valign='top'>\n<tr>\n");
PrintHomeHelp();
printf("<td align='center'>\n");
if (!empty($prevseq))
    printf("%d &nbsp;", $prevseq);
if ($numseq>1)
    printf("<input type='submit' value='<< Prev Plot' name='prev'>\n");
printf("<input type='text' name='ds' size='5' maxlength='5' value='%s'>\n", $ds);
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
printf("<input class='Width' type='button' value='Reset' onClick='self.location.href=\"showplots-ds.php\"'>\n");
printf("</td>\n</tr>\n<tr>\n<td align='center' colspan='3'>\n");
if (!empty($seq))
{
    if (in_array($seq, $sequon))
        $color="green";
    else
        $color="red";

    printf("<div style='color:%s' onmouseover=\"this.innerHTML='%s';style.color='black'\" onmouseout=\"this.innerHTML='%s';style.color='%s'\"> %s </div>\n", $color, getinfofromdb($seq), $plot, $color, $plot);
    printf("<img src='%s'>\n", $plot);
}
else
    printf("You have to insert a dataset number into the first field.");
printf("</td>\n</tr>\n</table>\n");
printf("</form>\n");
printf("</body>\n");
printf("</html>\n");

ini_set("display_errors", "Off");

?>
