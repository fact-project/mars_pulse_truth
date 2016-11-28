var gFit    = true;
var gWidth  = 800;
var gHeight = 600;

function getStyle(element, strCssRule)
{
    if (document.defaultView && document.defaultView.getComputedStyle)
        return document.defaultView.getComputedStyle(element, "").getPropertyValue(strCssRule);

    if (!element.currentStyle)
        return "";

    strCssRule = strCssRule.replace(/\-(\w)/g, function (strMatch, p1) { return p1.toUpperCase(); });
    return element.currentStyle[strCssRule];
}

function getIntStyle(element, strCssRule)
{
    if (getStyle(element, strCssRule)=="auto")
        return 0;

    return parseInt(getStyle(element, strCssRule).replace("px", ""));
}

function HasClass(element, classname)
{
    if (!element)
        return "";

    var elemClasses = element.className.split(' '); // get list of classes
    for (var i=0; i<elemClasses.length; i++)
    {
        var elemClass = elemClasses[i];
        if (elemClass.substring(0, classname.length)==classname)
            return elemClass.substring(classname.length, elemClass.length);
    }
    return "";
}

function GetCurrentTab()
{
    var element = document.getElementById('current');

    var sub = HasClass(element, "tabno");
    return sub ? parseInt(sub) : -1;
}

function GetFirstVisibleTab()
{
    var currentTab = GetCurrentTab();
    if (currentTab<0)
        return -1;

    var elems = document.getElementsByTagName("li");
    for (var i=0; i<elems.length; i++)
    {
        var n = HasClass(elems[i], "tabno");
        if (!n || n=='0')
            continue;

        if (elems[i].style.display=='none')
            continue;

        return i;
    }

    return -1;
}

function Hide()
{
    var tab = GetFirstVisibleTab();
    if (tab<0)
        return;

    var elems = document.getElementsByTagName("li");
    if (tab<elems.length-1)
        elems[tab].style.display = 'none';
}

function Show()
{
    var tab = GetFirstVisibleTab();
    if (tab<0)
        return;

    var elems = document.getElementsByTagName("li");
    if (tab>0)
        elems[tab-1].style.display='block';
}

function Switch(tabnum)
{
    var cnt = 0;

    var elems = document.getElementsByTagName("li");
    for (var i=0; i<elems.length; i++)
    {
        if (HasClass(elems[i], "tabno"))
        {
            elems[i].id = "";
            cnt++;
        }
    }

    tabnum = (tabnum+cnt)%cnt;

    for (var i=0; i<elems.length; i++)
    {
        if (!HasClass(elems[i], "tabno"))
            continue;

        if (tabnum-->0)
            continue;

        elems[i].id = 'current';
        break;
    }

    var cur = document.getElementById('current');
    if (!cur)
        return;

    var elem = cur.getElementsByTagName('a')[0];
    var type = HasClass(elem, "load_");

    if (type=="png")
        LoadImg(elem.name);
    if (type=="html")
        LoadLog(elem.name);
    if (type=="rc")
        LoadRc(elem.name);
    if (type=="def")
        Mars();
}

function Add(i)
{
    var currentTab = GetCurrentTab();
    if (currentTab<0)
        return;

    Switch(currentTab+i);
}

function GetWidth()
{
    var dw = 0;

    var element1 = document.getElementById('tabcontent');
    dw += getIntStyle(element1, "padding-right");
    dw += getIntStyle(element1, "padding-left");

    dw += 2*14;   // width of box_mtl_topl.gif and box_mtl_topr.gif

    return gFit ? document.width-dw : gWidth;
}

function GetHeight()
{
    var element = document.getElementById('tabcontent');

    var dh = 0;
    dh += getIntStyle(element, "padding-top");
    dh += getIntStyle(element, "padding-bottom");

    return gFit ? (document.width-dh)*gHeight/gWidth : gHeight;
}

function ChangeCursor()
{
//    document.body.style.cursor='url(magnify-glass.cur)';
}

function LoadImg(fname)
{
    var img = new Image();
    img.src = fname;

    gWidth  = img.width;
    gHeight = img.height;

    document.getElementById('tabcontent').innerHTML =
        "<IMG id='mainpic' "+
        "onclick='ToggleSize();' style='background-color:#ffffff;' alt='"+fname+"' "+
        (GetWidth()>0 ?("width='"+GetWidth()+"'"):"")+
        (GetHeight()>0?("height='"+GetHeight()+"'"):"")+
        " src='"+fname+"'"+"></IMG>";
}

function LoadImgTT(fname)
{
    var img = new Image();
    img.src = fname;

    var w = 480;
    var h = 480*img.height/img.width;

    var elements = document.getElementsByTagName('span');
    for (var i=0; i<elements.length; i++)
    {
        var elemClasses = elements[i].className.split(' '); // get list of classes
        for (var j=0; j<elemClasses.length; j++)
        {
            if (elemClasses[j]=="preview")
            {
                elements[i].getElementsByTagName('span')[0].innerHTML =
                    "<IMG style='background-color:#ffffff;' alt='"+fname+"' width='"+w+"' height='"+h+"' src='"+fname+"'"+"></IMG>";
                break;
            }
        }

    }
}

function Mars()
{
    document.getElementById('tabcontent').innerHTML = "<div style='padding:1 5 5 15;'>"
        + document.getElementById('default').innerHTML + "</div>";
    // width:"+GetWidth()+";height:"+GetHeight()+"
}

function Size(fit)
{
    gFit = fit;

    Resize();

    var element = document.getElementById('mainpic');
    if (!element)
        return;

    element.width  = GetWidth();
    element.width  = GetWidth(); /* INTENTIONAL */
    element.height = GetHeight();
}

function ToggleSize()
{
    Size(!gFit);
    document.getElementById('magtxt').innerHTML = gFit ? "O" : "Z";
    Resize();
}

function Resize()
{
    var e = document.getElementById('tabsul');
    var h = getIntStyle(document.getElementById('magnify'), "height");
    var p = getIntStyle(e, "padding-right");

    // Order is important!
    e.style.height = h;  // +1 to show the bottom border
    e.style.width  = document.width-p*2;

/*
    var w = 0;

    w += getIntStyle(element, "padding-right");
    w += getIntStyle(element, "padding-left");

    document.getElementById('myContent').innerHTML = w+" ";

    var elements = document.getElementsByTagName('li');
    for (var i=0; i<elements.length; i++)
    {
        w += getIntStyle(elements[i], "padding-left");
        w += getIntStyle(elements[i], "padding-right");
        w += getIntStyle(elements[i], "width");
    }

    document.getElementById('tabs').style.width = w;*/
}

var xmlHttpObject = false;
if (typeof XMLHttpRequest != 'undefined')
    xmlHttpObject = new XMLHttpRequest();

if (!xmlHttpObject)
{
    try { xmlHttpObject = new ActiveXObject("Msxml2.XMLHTTP"); }
    catch(e) 
    {
        try { xmlHttpObject = new ActiveXObject("Microsoft.XMLHTTP"); }
        catch(e) { xmlHttpObject = null; }
    }
}

function LoadLog(file)
{

    document.getElementById('tabcontent').innerHTML = 
        '<iframe style="border-width:1px;border-style:solid;" frameborder="0" height="90%" width="100%" src="'+file+'">'+
        '<P>Please click <a href="'+file+'">here</a>.</P>'+
        '</iframe>';

    /*
    xmlHttpObject.open('get', file);
    xmlHttpObject.onreadystatechange = handleContentLog;
    xmlHttpObject.send(null);
    return false;*/
}

function handleContentLog()
{
    /*
       0: nicht initialisiert
       1: objekt bereit aber noch keine daten gesendet
       3: daten werden empfangen
       4: alle daten wurden empfangen
       */
    if (xmlHttpObject.readyState == 4)
    {
        document.getElementById('tabcontent').innerHTML = xmlHttpObject.responseText;
    }
}

function LoadRc($file)
{
    document.getElementById('tabcontent').innerHTML = 
        '<iframe style="border-width:1px;border-style:solid;" frameborder="0" height="90%" width="100%" src="'+file+'">'+
        '<P>Please click <a href="'+file+'">here</a>.</P>'+
        '</iframe>';
/*
    xmlHttpObject.open('get', $file);
    xmlHttpObject.onreadystatechange = handleContentRc;
    xmlHttpObject.send(null);
    return false;*/
}

function handleContentRc()
{
    /*
     0: nicht initialisiert
     1: objekt bereit aber noch keine daten gesendet
     3: daten werden empfangen
     4: alle daten wurden empfangen
     */
    if (xmlHttpObject.readyState == 4)
    {
        document.getElementById('tabcontent').innerHTML = "<pre>"+xmlHttpObject.responseText+"<pre>";
    }
}

function LoadLogTT(file)
{
    return false;

    xmlHttpObject.open('get', file);
    xmlHttpObject.onreadystatechange = handleContentTT;
    xmlHttpObject.send(null);
    return false;
}

function handleContentTT()
{
    /*
     0: nicht initialisiert
     1: objekt bereit aber noch keine daten gesendet
     3: daten werden empfangen
     4: alle daten wurden empfangen
     */
    if (xmlHttpObject.readyState == 4)
    {
        var elements = document.getElementsByTagName('span');
        for (var i=0; i<elements.length; i++)
        {
            var elemClasses = elements[i].className.split(' '); // get list of classes
            for (var j=0; j<elemClasses.length; j++)
            {
                if (elemClasses[j]=="preview")
                {
                    elements[i].innerHTML = xmlHttpObject.responseText;
                    break;
                }
            }

        }
    }
}

function Execute()
{
   Size(gFit);
}
