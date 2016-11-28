// This macro creates a fake MCerPhotEvt container (with some clusters
// that simulate the starfield seen by the DC currents) and then applies
// the image cleaning and the Hillas algorithm to recognize and classify
// the clusters.
//
void test_findstar();
{
  // Empty container and task lists
  MParList plist;
  MTaskList tlist;

  //  cout << "0" <<endl;  
  plist.AddToList(&tlist);

  MCerPhotEvt *DCEvt;

  MGeomCamMagic *geom; 

  const Int_t gsNpix = 577;

  TArrayF *startpixs(gsNpix);
  Int_t cluster[gsNpix];
  memset(cluster,-1,gsNpix*sizeof(Int_t));
  memset(startpixs,-1,gsNpix*sizeof(Int_t));

  // fill a fake MCerPhotEvt 

// loop over all pixels
  
  for (Int_t pixid = 0; pixid <gsNpix ; pixid++)
    {
      MCerPhotPix dcpix =  DCEvt[pixid];

      cout << "1" <<endl;  
      // a fake cluster:
      dcpix->SetNumPhotons(0.);
      
      switch (pixid)
      {
      case 10:
	dcpix->SetNumPhotons(1.5);
      case 3:
	dcpix->SetNumPhotons(1.1);
      case 9:
	dcpix->SetNumPhotons(1.1);
      case 22:
	dcpix->SetNumPhotons(1.1);
      case 23:
	dcpix->SetNumPhotons(1.1);
      case 24:
	dcpix->SetNumPhotons(1.1);
      case 11:
	dcpix->SetNumPhotons(1.1);
      };
    }; // end loop over pixels

  
  if (!(FindStartPixels(DCEvt, startpixs, geom)))
    cout << "Error calling the FindStartPixels function!!" << endl;
  
  return;
};  

Int_t FindStartPixels(MCerPhotEvt *evt, TArrayF *startpixs, MGeomCam *geom)
{
  // look for all the pixels with a DC higher than the DC of neighbour pixels 

  // loop over all pixels
  MCerPhotEvtIter Next(evt, kFALSE);
  
  for (Int_t pixid=0; pixid<gsNPix; pixid++)
    {
      MCerPhotPix dcpix =  DCEvt[pixid]; // curren pix with DC

      const MGeomPix &pix = (*geom)[pixid]; // MGeomCam pixel
      
      currDC = dcpix->GetNumPhotons();  // DC of current pixel

      // look for the max DC in the neighbors pixels
      Float_t macDC = 0;
      for (Int_t j=0; j<pix.GetNumNeighbors()-1; j++) 
	if ( evt[pix.GetNeighbor(j)]->GetNumPhotons() > maxDC) 
	  maxDC = evt[pix.GetNeighbor(j)]->GetNumPhotons();
     
      // a starting pixel was found: it is added to the array and the pointer
      // to the array is increased by 1
      if ( currDC > maxDC)
	*(startpixs++) = currDC;
    }

  return 1;
}

Int_t FindCluster(Int_t startpix, Int_t cluster, MGeomCam *geom)
{
  return 1;

} ;
