<?php
    require_once("./phpchartdir.php");

include("plotinclude.php");

function GetData($db_id, $tables, $joins, $set, &$prim, &$val1, &$val2)
{
    $query=GetQuery($tables, $joins, $set);
//    printf("query:%s<br>", $query);
    $result=mysql_query($query, $db_id);

    while ($row = mysql_fetch_row($result))
    {
        $prim[] = $row[0];
        $val1[] = $row[1];
        if ($_GET["plot"]=="val")
            $val2[] = $row[2];
    }
    mysql_free_result($result);
}

$primary=$_GET["prim"];
$column=$_GET[$primary."Col"];

//get data from db
$db_id = mysql_pconnect($host, $user, $pw);
if ($db_id==FALSE)
{
    printf("mysql_connect returned the following error: %s\n", mysql_error());
    die("");
}
mysql_select_db($db);

GetData($db_id, $tables, $joins, "", $prim, $val1, $val2);
if (!empty($_GET["Set"]))
    GetData($db_id, $tables, $joins, "Set", $Setprim, $Setval1, $Setval2);
if (!empty($_GET["Set2"]))
    GetData($db_id, $tables, $joins, "Set2", $Set2prim, $Set2val1, $Set2val2);

mysql_close($db_id);

//set up plot
if (empty($_GET["x"]))
    $x=640;
else
    $x=$_GET["x"];
if (empty($_GET["y"]))
    $y=400;
else
    $y=$_GET["y"];

$c= new XYChart($x,$y);//, 0xeeeeee, 0x000000, 1);
$c->setPlotArea(70, 45, $x-100, $y-100, 0xffffff, -1, 0xcccccc, 0xccccccc);
//$c= new XYChart(440,250);//, 0xeeeeee, 0x000000, 1);
//$c->setPlotArea(70, 45, 340, 150, 0xffffff, -1, 0xcccccc, 0xccccccc);
//$c->setRoundedFrame();

//size of the symbols
$small=2;//normal data
$large=8;//set1, set2
//fonts
$titlesize=13;
$font="arialbd.ttf";
$axisfont="";
$axissize=10;
$labelfont="arialdb.ttf";
$labelsize=10;
//colours
$colour=0x000000;
$setcolour=0x0080ff;
$set2colour=0xff0000;

if ($_GET["plot"]=="val")
{
    $column2=$_GET[$primary."Col2"];
    $c->addScatterLayer($val2, $val1, $column, CircleSymbol, $small, $colour, $colour);
    if (!empty($_GET["Set"]))
        $c->addScatterLayer($Setval2, $Setval1, $column, CircleSymbol, $large, $setcolour, $setcolour);
    if (!empty($_GET["Set2"]))
        $c->addScatterLayer($Set2val2, $Set2val1, $column, CircleSymbol, $large, $set2colour, $set2colour);
    $c->yAxis->setTitle($column, $axisfont, $axissize);
    $c->xAxis->setTitle($column2, $axisfont, $axissize);
    $textBoxObj = $c->addTitle(ltrim($column, "f")."  vs  ".ltrim($column2, "f"), $font, $titlesize);
}
else
{
    $c->addScatterLayer($prim, $val1, $column, CircleSymbol, $small, $colour, $colour);
    if (!empty($_GET["Set"]))
        $c->addScatterLayer($Setprim, $Setval1, $column, CircleSymbol, $large, $setcolour, $setcolour);
    if (!empty($_GET["Set2"]))
        $c->addScatterLayer($Set2prim, $Set2val1, $column, CircleSymbol, $large, $set2colour, $set2colour);
    $c->yAxis->setTitle($column, $axisfont, $axissize);
    $c->xAxis->setTitle($primary, $axisfont, $axissize);
    $textBoxObj = $c->addTitle(ltrim($column,"f")."  vs  ".ltrim($primary,"f"), $font, $titlesize);
}
//get scaling of the yAxis
if (!empty($_GET[$column."Start"]) && !empty($_GET[$column."Stop"]))
{
    $start=$_GET[$column."Start"];
    $stop=$_GET[$column."Stop"];
    $numticks=($stop-$start)/4;
    $numticks2=($stop-$start)/8;
    $c->yAxis->setLinearScale($start, $stop, $numticks, $numticks2);
}
//background of title
$textBoxObj->setBackground(0xccccff,-1, softLighting());
$c->xAxis->setLabelStyle($labelfont, $labelsize);
$c->yAxis->setLabelStyle($labelfont, $labelsize);

header("Content-type: image/png");
print($c->makeChart2(PNG));

?>
