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
!   Author(s): Thomas Bretz 5/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// starfield.C
// ===========
//
// This is a macro demonstrating how to use Mars to display a starfield.
// It also is a tutorial for MAstroCatalog and MAstroCamera.
//
// For more details on this classes see the class-reference.
//
// If you want to display stars in a camera you need the camera geometry
// and the mirror definition. You can get this either from a Monte Carlo
// file containing both or create your MGeomCam object manually and
// read a magic.def file which you will find in the camera simulation
// program.
//
///////////////////////////////////////////////////////////////////////////

void ReadSetup(MAstroCamera &cam)
{
    /*
     // Name of a MC file having MGeomCam and MMcConfigRunHeader
     TString fname = "../Mars/Gamma_zbin7_90_7_53395to53399_w0.root";

     MMcConfigRunHeader *config=0;
     MGeomCam           *geom=0;

     TFile file(fname);
     TTree *tree = (TTree*)file.Get("RunHeaders");
     tree->SetBranchAddress("MMcConfigRunHeader", &config);
     if (tree->GetBranch("MGeomCam"))
         tree->SetBranchAddress("MGeomCam", &geom);
     tree->GetEntry(0);

     cam.SetMirrors(*config->GetMirrors());
     cam.SetGeom(*geom);
     */

    MGeomCamMagic geom;
    cam.SetGeom(geom);
    cam.SetMirrors("resources/mirrors.def");
}

void starfield()
{
    // Time for which to get the picture
    MTime time;
    time.Set(2004, 2, 28, 01, 32, 15);

    // Current observatory
    MObservatory magic1;

    // Right Ascension [h] and declination [deg] of source
    // Currently 'perfect' pointing is assumed
    const Double_t ra  = MAstro::Hms2Rad(5, 34, 31.9);
    const Double_t dec = MAstro::Dms2Rad(22, 0, 52.0);

    new TCanvas;

    // --------------------------------------------------------------------------
    // Create camera display from geometry
    //MAstroCatalog stars;     // Use this top display a catalog
    MAstroCamera stars;        // Use this top display a catalog on top of a camera
    ReadSetup(stars);   // Use this top display a catalog on top of a camera
    stars.SetLimMag(6);
    stars.SetRadiusFOV(3);
    stars.SetRaDec(ra, dec);

    // Read the stars from a star catalog. The catalogs can be downloaded
    // from the Mars web page. For more information see class reference.
    stars.ReadBSC("bsc5.dat");

    stars.SetObservatory(magic1);
    stars.SetTime(time);
    stars.SetGuiActive();

    // See the cooresponding Draw-function for more options
    TObject *o = stars.Clone();
    o->SetBit(kCanDelete);
    o->Draw();

}
