<?  // To be shifted to magicdefs.pho
$htpath = "/var/www/uni-wuerzburg.de/astro/db/htdocs/datacenter";
$htlink = "http://datacenter.astro.uni-wuerzburg.de";

$htlinks = dirname($_SERVER["HTTP_HOST"].$_SERVER["REQUEST_URI"]);

$paths = array
    (
     "star"    => "/star/",
     "calib"   => "/callisto/",
     "signal"  => "/callisto/",
     "ganymed" => "/ganymed/",
     "gplotdb" => "/ganymed/",
     "db"      => "/plotdb/",
     "status"  => "/status/",
     "condor"  => "/condor/",
     "optical" => "/optical/",
     );

$csvname = array
    (
     "star"    => "star",
     "calib"   => "calib",
     "signal"  => "signal",
     "ganymed" => "ganymed",
     "gplotdb" => "plotdb",
     "db"      => "plotdb",
     "status"  => "plotstat",
     "condor"  => "plotusage",
     "optical" => "plotoptical",
     );

$prefix = array
    (
     "star"    => "star",
     "calib"   => "calib",
     "signal"  => "signal",
     "ganymed" => "ganymed",
     "gplotdb" => "gplotdb",
     "db"      => "plotdb",
     "status"  => "status",
     "condor"  => "condor",
     "optical" => "optical",
     );

?>
<?   // Prepare

   $number = empty($_GET["n"]) ? -1   : $_GET["n"];
   $type   = empty($_GET["t"]) ? "db" : $_GET["t"];

   $path = $htpath.$paths[$type];
   if ($number>0)
       if ($type=="ganymed" || $type=="gplotdb")
           $path = sprintf($path."%05d/%08d/", $number/1000, $number);
       else
           $path = sprintf($path."%04d/%08d/", $number/10000, $number);
?>
<?

   $csvfile=$path.$csvname[$type].".csv";
   if (file_exists($csvfile))
   {
       // read the file into an array
       $lines = file($csvfile);

       // some replacement for the printf
       $search  = array ("%",  "\n", "\\"   );
       $replace = array ("%%", "",   "\\\\" );

       for ($i=0; $i<sizeof($lines); $i++)
       {
           //$arr = get_csv($lines[$i], "\t\n");
           $tok = strtok($lines[$i], "\t\n");
           $j=0;
           while ($tok!==false)
           {
               $arr[$i][$j++] = str_replace($search, $replace, $tok);
               $tok = strtok("\t");
           }
       }
   }

//    fclose($file);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html>
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
   <meta name="Author" content="Thomas Bretz">
   <link rel="stylesheet" type="text/css" href="tabs.css">
   <script type="text/javascript" src="tabs.js">You need JavaScript switched on for this page to work!</script>
   <title>Tabs: <? printf($arr[0][2]); ?></title>
</head>

<body onload="Resize();Switch(0);" onresize="Execute();">
<div class="tabs-blue" id="tabs">
<ul id="tabsul">
 <li id='prevnext'><A HREF="#" accesskey='alt-shift-P' style="width:6;" class='tooltip' onclick="Add(+1);">+<span class='info'>Switch to previous tab.</span></A></li>
 <li id='prevnext' style='margin-left:5px'><A HREF="#" accesskey='alt-shift-N' style="width:6;" class='tooltip' onclick="Add(-1);">-<span class='info'>Switch to next tab.</span></A></li>
 <li id='prevnext'><A HREF="#" accesskey='alt-shift-P' style="width:6;" class='tooltip' onclick="Hide();">&gt;<span class='info'>Step in shown tabs to right.</span></A></li>
 <li id='prevnext' style='margin-left:5px'><A HREF="#" accesskey='alt-shift-N' style="width:6;" class='tooltip' onclick="Show();">&lt;<span class='info'>Step in shown tabs to leaft.</span></A></li>
 <li id='magnify'><A HREF="#" style="width:8;" class='tooltip' id="magtxt" onclick="ToggleSize();">O<span class='info'>Toggle between original image size and resized to the document's width.</span></A></li>
<?
   $n=0;

   printf(" <li style='margin-right:3px' id='current' class='tabno%d'><A HREF=\"#\" class='tooltip load_def' onclick=\"Switch(%d);\">MARS<span class='info' style='text-align:center;'><P><font style='text-decoration:underline;'>Tab Overview</font><P>%s</span></A></li>\n",
          $n, $n, $arr[0][2]);

   $n++;

   for ($i=1; $i<sizeof($lines); $i++, $n++)
   {
       // Use blur() to remove the focus after the click

       foreach (glob($path.$prefix[$type].$i.".png") as $file)
       {
           $link = ereg_replace("^".$htpath, $htlink, $file);
           $name = basename($file);

           printf(" <li class='tabno".$n."'>");
           printf("<A class='tooltip load_png' name='".$link."' ");
           printf("onclick=\"Switch(".$n.");\" ");
           printf("onmouseover=\"LoadImgTT('".$link."');\" HREF='#'>");
           printf($arr[$i][1]."<span class='info'>".$arr[$i][2]."</span>");
           printf("<span class='preview' id='preview'><div>".$arr[$i][2]."</div>");
           printf("<span id='image'></span></span></A></li>\n");
       }
   }

   foreach (glob($path."/*") as $file)
   {
        $info = pathinfo($file); // dirname, extension, basename, filename

        $link   = ereg_replace("^".$htpath, $htlink, $file);
        $name   = $info['basename'];
        $exten  = $info['extension'];
        

        // Define list of valid extension
        if ($exten!="html" && $exten!="rc")
           continue;

        printf(" <li class='tabno".$n."'>");
        printf("<A class='tooltip load_".$exten."' name='".$link."' ");
        printf("onclick=\"Switch(".$n.");\" HREF='#'>");

        if ($exten=="html")
            printf(ereg_replace("[0-9]{8}\.html", "", $name).".log");
        if ($exten=="rc")
            printf($name);

        printf("<span class='info'>".$name."</span></A></li>\n");

        $n++;
   }
?>
</ul>
</div>
<table style="clear:both;" background="box_mtl_ctr.gif" border="0" cellpadding="0" cellspacing="0" width="100%">
<tbody>
  <tr height="12">
     <td width="12"><img src="box_mtl_topl.gif"></td>
     <td background="box_mtl_top.gif"><img src="spacer.gif"></td>
     <td width="12"><img src="box_mtl_topr.gif"></td>
  </tr>
  <tr>
     <td background="box_mtl_l.gif" width="12"><img src="spacer.gif"></td>
     <td align="center" background="bg_spacer.gif" valign="top">
       <div class="tabcontent" id="tabcontent"></div>
     </td>
     <td background="box_mtl_r.gif" width="12"><img src="spacer.gif"></td>
  </tr>
  <tr height="14">
     <td width="12"><img src="box_mtl_botl.gif"></td>
     <td background="box_mtl_bot.gif"><img src="spacer.gif"></td>
     <td width="12"><img src="box_mtl_botr.gif"></td>
  </tr>
</tbody>
</table>
<!--
<div class="tabcontent" id="tabcontent"></div>
-->
<div style="display:none" id="default">
<?
     printf("<p>\n");
     printf("<center>\n");
     printf(" <table class='marstable'>\n");
     printf("  <tr><td id='title' colspan='3'>%s</td></tr>\n", $arr[0][2]);

     for ($i=1; $i<sizeof($lines); $i++)
     {
         foreach (glob($path.$prefix[$type].$i.".png") as $file)
         {
             $link = ereg_replace("^".$htpath, $htlink, $file);
             $name = basename($file);

             printf("  <tr>");
             printf("<td id='left'>%d</td>", $i);
             printf("<td id='middle'><A HREF=\"#\" ");
             printf("onclick=\"Switch(".$i.");\">");
             printf(htmlspecialchars($arr[$i][1])."</A></td>");
             printf("<td id='right'>".htmlspecialchars($arr[$i][2])."</td>");
            printf("</tr>\n");
         }
     }

     foreach (glob($path."*") as $file)
        if (ereg("\.(root|log|html|pdf|ps|rc)$", $file))
        {
            $link = ereg_replace("^".$htpath, $htlink, $file);
            $name = basename($file);

            printf("  <tr>");
            printf("<td id='left'></td>");
            printf("<td id='middle'>Download</td>");
            printf("<td id='middle' colspan='1'><A HREF=\"".$link."\">".$name."</A></td>");
            printf("</tr>\n");
        }
        
     printf(" </table>\n");
     printf("</center>\n");
     printf("</p>\n");
?>
</div>
<p id="myContent">
<?/*printf(
"SERVER: ".$_SERVER["SERVER_NAME"]."<br>".
"HTTP_HOST: ".$_SERVER["HTTP_HOST"]."<br>".
"REQ_URI: ".$_SERVER["HTTP_HOST"].$_SERVER["REQUEST_URI"]."<br>".
"DIR: ".dirname($_SERVER["HTTP_HOST"].$_SERVER["REQUEST_URI"])."<br>".
"REQ_URI: ".basename($_SERVER["REQUEST_URI"])."<br>".
"PATH_TRANS: ".$_SERVER["PATH_TRANSLATED"]."<br>".
"PATH_INFO: ".$_SERVER["PATH_INFO"]."<br>".
"SCRIPT_NAME: ".$_SERVER["SCRIP_NAME"]."<br>".
"_FILE_: ".__FILE__."<br>".
"_DIR_: ".__DIR__."<br>".
$paths[$type]."\n".$path."\n".$_GET["number"]."\n".$_GET["type"]."\n".$number."\n".$type);
*/?>
</p>
</body>
</html>
