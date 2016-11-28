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
!   Author(s): Thomas Bretz  10/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MARS
//
/////////////////////////////////////////////////////////////////////////////
#include "MAGIC.h"

#include <stdlib.h> // getenv (Ubuntu 8.10)

#include <iostream>

//NamespaceImp(MARS);

using namespace std;

bool MARS::CheckRootVer()
{
    if (strcmp(gROOT->GetVersion(), "5.22/00")>0 && strcmp(gROOT->GetVersion(), "5.26/00")<0)
    {
        cout << "WARNING - Especially root 5.24/00 has a severe bug in projection 2D histograms" << endl;
        cout << "          which might cause unexpected crashes and could also affect the results." << endl;
        cout << "          Please, avoid using these root versions." << endl;
    }

    if (!strcmp(gROOT->GetVersion(), ROOT_RELEASE))
        return true;

    cout << "FATAL ERROR - ROOT version mismatch." << endl;
    cout << "  Your mars has been compiled using root v" << ROOT_RELEASE << endl;
    cout << "  The libraries you are using are from root v" << gROOT->GetVersion() << endl;
    cout << "  Your environment contains:" << endl;
    cout << "    ROOTSYS         = " << getenv("ROOTSYS") << endl;
    cout << "    PATH            = " << getenv("PATH") << endl;
    cout << "    LD_LIBRARY_PATH = " << getenv("LD_LIBRARY_PATH") << endl << endl;
    cout << "  If this is the right root-version please do" << endl;
    cout << "    make mrproper && make" << endl;
    cout << "  for a complete recompilation." << endl << endl;
    cout << "  If this is the wrong root-version make sure that the file ~/.bashrc contains:" << endl;
    cout << "    export ROOTSYS=/PathTo/Your/RootVersion" << endl;
    cout << "    export PATH=$ROOTSYS/bin:$PATH" << endl;
    cout << "    export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH" << endl;
    cout << "  and start a new shell. Check it by typing \"export\"" << endl;
    cout << endl;

    return false;
}

TString MARS::GetPackageName() { return PACKAGE_NAME; }
TString MARS::GetPackageVersion() { return PACKAGE_VERSION; }
TString MARS::GetRevision() { return REVISION; }
