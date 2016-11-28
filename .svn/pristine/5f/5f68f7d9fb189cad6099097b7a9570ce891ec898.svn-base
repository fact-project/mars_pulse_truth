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
!   Author(s): Thomas Bretz, 9/2002 <mailto:tbretz@astro-uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MProgressBar
//
// Simple window with progress bar. Can be used in an eventloop.
//
/////////////////////////////////////////////////////////////////////////////
#include "MProgressBar.h"

#include <stdlib.h>         // rand (Ubuntu 8.10)

#include <TSystem.h>        // gSystem
#include <TGProgressBar.h>  // TGHProgressBar

#include "MGList.h"

ClassImp(MProgressBar);

// --------------------------------------------------------------------------
//
// Default constructor. Opens a window with a progress bar. Get a pointer
// to the bar by calling GetBar. This pointer can be used for the
// eventloop.
//
// Be carefull: killing or closing the window while the progress meter
//   is still in use may cause segmentation faults. Please kill the window
//   always by deleting the corresponding object.
//
MProgressBar::MProgressBar() : TGTransientFrame(gClient->GetRoot(), gClient->GetRoot(), 1, 1)
{
    fList = new MGList;
    fList->SetOwner();

    //SetMWMHints(0, 0, 0);

    SetWMSizeHints(150, 15, 640, 480, 10, 10); // set the smallest and biggest size of the Main frame
    Move(rand()%100+50, rand()%100+50);
    Resize(150, 30);

    TGLayoutHints *laybar=new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 2,2,2,2);
    fList->Add(laybar);

    fBar=new TGHProgressBar(this);
    fBar->ShowPosition();
    AddFrame(fBar, laybar);
    fList->Add(fBar);

    Layout();

    MapSubwindows();

    SetWindowName("Progress");
    SetIconName("Progress");

    MapWindow();
}

// --------------------------------------------------------------------------
//
// Destruct the window with all its tiles. Also the Progress Bar object
// is deleted.
//
MProgressBar::~MProgressBar()
{
    delete fList;
} 

