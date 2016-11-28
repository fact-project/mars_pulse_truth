
//functions and arrays needed for showing and hiding the menu on the db websites
ids = new Array( "all", "info", "info2", "stat", "fail", "cal", "star", "limits", "ranges", "showquery");
statarr = new Array( 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);

//function to get the input for fShowHide from the page
//called each time, when an element is changed with showhide()
//and when the page is reloaded
function setdisplays ()
{
    //reset variable for fShowHide
    var sh="";
    for (var i = 0 ; i < ids.length ; i++)
    {
        //get element for each possible menu element
        var d = document.getElementById(ids[i]);
        //if element exists, check if it is currently displayed
        //set the value in the status array accordingly
        //0: element is not shown
        //1: element is shown
        if (d)
            d.style.display == 'none' ? statarr[i]=0 : statarr[i]=1;
        //set the value of 'all' to 0
        //to make sure, that the whole menu is hidden in case of a query
        if (i==0)
            statarr[i]=0;
        //add value to the variable for fShowHide
        sh=sh+statarr[i];
    }
    //return variable for fShowHide
    return sh;
}

//function to hide one element of the menu and adapt the button accordingly
function hide(el,img)
{
    el.style.display="none";
    if (img)
    {
        img.src='plus.png';
        img.alt='+';
    }
}

//function to show one element of the menu and adapt the button accordingly
function show(el,img)
{
    el.style.display="block";
    if (img)
    {
        img.src='minus.png';
        img.alt='-';
    }
}

//function to show or hide one element of the menu
function showhide (id)
{
    //get menu element and button for the element that has to be changed
    var el = document.getElementById(id);
    var img = document.getElementById(id+"button");
    //query current status and change it accordingly
    if (el.style.display != 'none')
        hide(el,img);
    else
    {
        show(el,img);
        //expand the whole menu, when one element is shown
        if (id!="showquery")
        {
            var el2 = document.getElementById("all");
            var img2 = document.getElementById("allbutton");
            show(el2,img2);
        }
    }
    //update the value of fShowHide (id sh)
    var fShowHide = document.getElementById("sh");
    fShowHide.value = setdisplays();
}

//function getting the info, how the menu is from fShowHide
//is called when the page is loaded (s. index-header.html)
function getdisplay()
{
    //get value of fShowHide
    var stat = document.getElementById("sh");
    statvalue=stat.value;
    //set menu variables for each element accordingly
    for (var i = 0 ; i < ids.length ; i++)
    {
        //get value of this element from fShowHide
        var status = statvalue.slice(i,i+1);
        //get elements of menu and button
        var el = document.getElementById(ids[i]);
        var img = document.getElementById(ids[i]+"button");
        //not all element exist on each page
        if (!el)
            continue;
        //show or hide element according to the status value
        status==0 ? hide(el,img) : show(el,img);
    }
    //set the value of fShowHide (id sh)
    // needed here to make sure, that the whole menu is hidden
    //  even if nothing has been expanded (needed since 'ranges'
    //  is expanded by default)
    var fShowHide = document.getElementById("sh");
    fShowHide.value = setdisplays();
}

//show all lines on db website
function showalllines(num)
{
    for (var i = 1 ; i < num+1 ; i++)
    {
        var el = document.getElementById("line"+i);
        var img = document.getElementById("line"+i+"button");
        if (el.style.display == 'none')
        {
            el.style.display='';
            if (img)
            {
                img.src='minus.png';
                img.alt='-';
            }
        }
    }
}


//functions and arrays needed for the menu of plotdb.php
primaries = new Array( "fSequenceFirst", "fRunNumber", "fDataSetNumber");

//for each primary (run#, seq#, dataset#) two pulldowns exist
//the first is needed when plotting versus primary ('prim' is chosen)
//the second is only needed when plotting versus a second value ('val' is chosen)

//shows the first pulldown for a given primary
// and hides the ones of the other primaries
// and hides the second one for the primary if needed
//is called when a primary is chosen from the pulldown
function showpulldown (id2)
{
    //loop over primaries
    for (var i = 0 ; i < primaries.length ; i++)
    {
        //if primary is given id
        if (primaries[i]==id2)
        {
            //show first pulldown
            var el2 = document.getElementById(primaries[i]);
            el2.style.display="inline";
            //show pulldowns for limits
            var el4 = document.getElementById(primaries[i]+"3");
            el4.style.display="inline";
            var el6 = document.getElementById(primaries[i]+"4");
            el6.style.display="inline";
        }
        else
        {
            //hide first pulldown
            var el2 = document.getElementById(primaries[i]);
            el2.style.display="none";
            //get element for second pulldown
            var el3 = document.getElementById(primaries[i]+"2");
            //if second pulldown is shown,
            // hide second pulldown
            // and show second pulldown for chosen primary (id2)
            if (el3.style.display=='inline')
            {
                var el6 = document.getElementById(id2+"2");
                el6.style.display="inline";
                el3.style.display="none";
            }
            //hide pulldowns for limits
            var el4 = document.getElementById(primaries[i]+"3");
            el4.style.display="none";
            var el5 = document.getElementById(primaries[i]+"4");
            el5.style.display="none";
        }
    }
}

//shows the second pulldown and inputs for the ranges
function showpulldown2 ()
{
    count=0;
    for (var i = 0 ; i < primaries.length ; i++)
    {
        //get element
        var el = document.getElementById(primaries[i]);
        //if the first pulldown is shown, show also the second one
        if (el.style.display=='inline')
        {
            var el2 = document.getElementById(primaries[i]+"2");
            el2.style.display="inline";
        }
        else
            count++;
    }
    //if no first pulldown is shown, i.e. the primary has not yet been chosen,
    //all second pulldowns are shown
    if (count==3)
        for (var i = 0 ; i < primaries.length ; i++)
        {
            var el2 = document.getElementById(primaries[i]+"2");
            el2.style.display="inline";
        }

    //show the inputs for the ranges for the second pulldown
    //there is only one element for this (not one for each primary)
    var el3 = document.getElementById("Range2");
    el3.style.display="inline";
}

//hides the second pulldown and inputs for the ranges
function hidepulldown2 ()
{
    for (var i = 0 ; i < primaries.length ; i++)
    {
        var el2 = document.getElementById(primaries[i]+"2");
        el2.style.display="none";
    }
    var el3 = document.getElementById("Range2");
    el3.style.display="none";
}

//show the element for inserting the second set
function showset2()
{
    var el = document.getElementById("set2");
    el.style.display="inline";
}

//hide the element for inserting the second set
function hideset2()
{
    var el = document.getElementById("set2");
    el.style.display="none";
}



//
//functions for builddatasets.php
//

function selectallsequences(val)
{
    var sequs = document.getElementsByTagName("input");
    for (var i=0; i<sequs.length; i++)
    {
        var sequname = sequs[i].name;
        if (!sequname.match("^DSSeq[0-9]*$"))
            continue;
        var sequval = sequs[i].value;
        if (sequval != val)
            sequs[i].checked = false;
        else
            sequs[i].checked = true;
    }
}

