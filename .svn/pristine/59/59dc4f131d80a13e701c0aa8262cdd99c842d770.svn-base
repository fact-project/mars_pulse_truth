/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Javier López, 04/2004 <mailto:jlopez@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
/*
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <TString.h>
#include <TArrayS.h>

#include "MParList.h"
#include "MTaskList.h"
#include "MGeomCamMagic.h"
#include "MCameraDC.h"

#include "MReadReports.h"
#include "MGeomApply.h"
#include "MEvtLoop.h"
*/
const Int_t gsNpix = 577;
MGeomCamMagic geomcam;

using namespace std;

void AnalCurrents(const TString filename)
{
    //UInt_t numEvents = 1000000;
    UInt_t numEvents = 1;
    
    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;  
    MTaskList tlist;
    plist.AddToList(&tlist);
    
    
    MCameraDC     dccam;
    
    plist.AddToList(&geomcam);
    plist.AddToList(&dccam);
    
    //
    // Now setup the tasks and tasklist:
    // ---------------------------------
    //
    // Reads the trees of the root file and the analysed branches
    MReadReports read;
    read.AddTree("Currents"); 
    read.AddFile(filename);     // after the reading of the trees!!!
    read.AddToBranchList("MReportCurrents.*");
    
    MGeomApply geomapl;
    
    tlist.AddToList(&geomapl);
    tlist.AddToList(&read);

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);
     
    MCerPhotEvt dcevt;
    
    const Int_t NSteps = 5;  // Number of steps to average DC over events
    Int_t count = 0;         // counter for averaging NSteps events
    Int_t NumAverEvt = 1;    // counter for averaged events stored into dcevt
    Double_t val[gsNpix];    // array for managing the averaging of gsNpix
                             // DC values over NSteps events

    // Initialize to 0 array val
    for(Int_t i=0; i<gsNpix; i++)
	val[i] = 0.0;
    //
    // Execute your analysis
    //
    //if (!evtloop.Eventloop(numEvents))
    //  return kFALSE;
    if (!evtloop.PreProcess())
	return;
    
    while (tlist.Process())
    {
	++count;
	for(Int_t i=0; i<gsNpix; i++)
	{
	    val[i] += dccam.GetCurrent(i);

	    if( (count%NSteps) == 0 )
	    {
		dcevt.AddPixel(i,val[i]/(Double_t)NSteps,0);
		dcevt.GetPixelContent(val[i],i,geomcam,0);
		val[i] = 0.0;
	    }
	}

	// Reset count and call findstar function on this macroevent
	if( (count%NSteps) == 0 )
	{
	    count = 0;
	    ++NumAverEvt;
	    cout << "Calling test_findstar function" << endl;
	    test_findstar(&dcevt);
	}
    }

    // Last loop overall pixels to fill with the average 
    // of last count events
    if(count!=0)
    {
	for(Int_t i=0; i<gsNpix; i++)
	    dcevt.AddPixel(i,val[i]/(Double_t)count,0);

	cout << "Calling test_findstar function" << endl;
	test_findstar(&dcevt);
    }

    cout << "Number of averaged events stored into container = " << NumAverEvt << endl;
    tlist.PrintStatistics();
    
}


// This macro creates a fake MCerPhotEvt container (with some clusters
// that simulate the starfield seen by the DC currents) and then applies
// the image cleaning and the Hillas algorithm to recognize and classify
// the clusters.
//
void test_findstar(MCerPhotEvt *wDCEvt)
{
    //MGeomCamMagic *geom; 
    MCerPhotEvt *DCEvt = new MCerPhotEvt();
    Int_t cluster[gsNpix];
    Float_t startp[gsNpix];
    
    memset(cluster,-1,gsNpix);
    memset(startp,-1,gsNpix);
    
    TVectorD *startpixs = new TVectorD();
    Double_t tempDC;
    Int_t pixid;
    // fill a fake MCerPhotEvt 
    // loop over all pixels
    for (pixid=0; pixid<gsNpix ; pixid++)
    {
	switch (pixid)
	{
	    case 9:
		DCEvt->AddPixel(pixid,1.1,0.0);
		break;
	    case 2:
	    case 8:
	    case 21:
	    case 22:
	    case 23:
	    case 10:
		DCEvt->AddPixel(pixid,1.1,0.0);
		break;
	    case 39:
		DCEvt->AddPixel(pixid,1.2,0.0);
		break;
	    case 64:
		DCEvt->AddPixel(pixid,1.71,0.0);
		break;
	    case 65:
		DCEvt->AddPixel(pixid,1.70,0.0);
		break;
	    case 40:
		DCEvt->AddPixel(pixid,1.72,0.0);
		break;
	    default:
		DCEvt->AddPixel(pixid);
		break;
	}
    } // end loop over pixels

    if(!(FindStartPixels(wDCEvt, startpixs)))
	cout << "ARGH!@!! In function FindStartPixel(): no starting pixel found" << endl;
    
}

Int_t FindStartPixels(MCerPhotEvt *evt, TVectorD *startpixs)
{
    Double_t currDC;
    Int_t i = 0;
    Double_t DCthr = 10.0;
    
    MCerPhotPix *dcpix;
    MGeomPix *pix;
    
    // look for all the pixels with a DC higher than the DC of neighbour pixels 
    // loop over all pixels
    for (Int_t pixid=0; pixid<gsNpix; pixid++)
    {
	Double_t tempDC;

	pix = (MGeomPix *)geomcam[pixid]; // MGeomCamMagic pixel
	
	// Get the DC value of the current pixel
	evt->GetPixelContent(currDC,pixid,geomcam,0);
	
	Float_t maxDC = 0.0;
	// look for the max DC in the neighbors pixels
	for (Int_t j=0; j < pix->GetNumNeighbors(); j++)
	{
	    evt->GetPixelContent(tempDC,pix->GetNeighbor(j),geomcam,0);
	    if(tempDC > maxDC) 
		maxDC = tempDC;
	}
	
	// a starting pixel was found: it is added to the array
	if ((currDC>maxDC) && (currDC>DCthr))
	{
	    startpixs->ResizeTo(++i);
	    startpixs(i-1)=pixid;
	    cout << "Starting pixel PixID=" << startpixs(i-1) << " curr=" << currDC << endl;
	}
    }
    
    if(startpixs->GetNrows()>0)
    {
	cout << "Number of starting pixels = " << startpixs->GetNrows() << endl;
	return 1;
    }
    else
	return 0;
}
/*
Int_t FindCluster(Int_t startpix, Int_t cluster, MGeomCam *geom)
{
  return 1;

}
*/

