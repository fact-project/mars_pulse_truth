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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// rootlogon.C
// ===========
//
// This file is the startup script ("autoexec.bat") executed when root is
// started. The definition which file to start is done in ".rootrc".
// There are three files which are automatically processed by root at
// startup: A systemwide .rootrc, one in your home directory and one
// in the current directory.
// So rootlogon.C is correctly executed if your start root from your
// Mars directory.
//
// The script setupts some small environmental things and makes
// sure that the Mars shared object (libmars.so) is loaded. This shared
// object gives you access to all Mars features from within the root
// interpreter.
//
// If libmars.so is not found in the current directory we search in the
// directory given in "MARSSYS" environment variable.
//
///////////////////////////////////////////////////////////////////////////
#include <iomanip>


Bool_t isloaded()
{
    TString str = gSystem->GetLibraries("libmars.so");

    return !str.IsNull();
}

void unload()
{
    if (!isloaded())
        return;

    cout << "Unloading 'libmars.so'... " << flush;
    if (gSystem->Unload("libmars.so"))
        cout << "error." << endl;
    else
        cout << "done." << endl << endl;
}

bool load(TString &dir)
{
    if (isloaded())
        return;

    cout << "\033[33m\033[1m" << "Loading '" << dir << "libmars.so'... " << "\033[0m" << flush;

    if (dir.IsNull())
        dir = "./";

    if (gSystem->Load(dir+"libmars.so")!=0)
    {
        cout << "\033[33m\033[1m" << "error." << endl;
        cout << "\033[0m" << endl;
        return false;
    }
    else
    {
        MParContainer::Class()->IgnoreTObjectStreamer();
        MArray::Class()->IgnoreTObjectStreamer();
        cout << "\033[33m\033[1m" << "done." << endl;
        cout << "\033[0m" << endl;
        return true;
    }
}
/*
void make()
{
    unload();

    if (gSystem->Exec("make"))
    {
        cout << "Error calling make..." << endl;
        return;
    }

    load();
}
*/
void rootlogon()
{
    // This is a workaround to make axis behave as they
    // are UTC and not local time
    //    gSystem->Setenv("TZ", "UTC");

    cout << endl;

    const Bool_t fileexist = !gSystem->AccessPathName("libmars.so", kFileExists);

    TString dir = fileexist ? "" : gSystem->Getenv("MARSSYS");
    if (!dir.IsNull())
    {
        cout << "\033[34m\033[1m" << "Searching Mars in " << dir << " (MARSSYS)" << "\033[0m" << endl << endl;

        if (!dir.EndsWith("/"))
            dir += "/";
    }

    if (!load(dir))
        return;

    MLog::RedirectErrorHandler(MLog::kColor);

    // This initialized the thread factory. This is needed to supress
    // an error which is displayed if the thread factory is initialized
    // from another than the main thread (e.g. in the constructor
    // of MStatusDisplay)
    TThread::Self();

    gInterpreter->AddIncludePath(dir+"macros");
    gInterpreter->AddIncludePath(dir+"manalysis");
    gInterpreter->AddIncludePath(dir+"mastro");
    gInterpreter->AddIncludePath(dir+"mbase");
    gInterpreter->AddIncludePath(dir+"mbadpixels");
    gInterpreter->AddIncludePath(dir+"mcamera");
    gInterpreter->AddIncludePath(dir+"mcalib");
    gInterpreter->AddIncludePath(dir+"mcore");
    gInterpreter->AddIncludePath(dir+"mdrs");
    gInterpreter->AddIncludePath(dir+"mhcalib");
    gInterpreter->AddIncludePath(dir+"mdata");
    gInterpreter->AddIncludePath(dir+"mfbase");
    gInterpreter->AddIncludePath(dir+"mfileio");
    gInterpreter->AddIncludePath(dir+"mfilter");
    gInterpreter->AddIncludePath(dir+"mgeom");
    gInterpreter->AddIncludePath(dir+"mgui");
    gInterpreter->AddIncludePath(dir+"mhbase");
    gInterpreter->AddIncludePath(dir+"mhflux");
    gInterpreter->AddIncludePath(dir+"mhft");
    gInterpreter->AddIncludePath(dir+"mhist");
    gInterpreter->AddIncludePath(dir+"mhistmc");
    gInterpreter->AddIncludePath(dir+"mhvstime");
    gInterpreter->AddIncludePath(dir+"mimage");
    gInterpreter->AddIncludePath(dir+"mjobs");
    gInterpreter->AddIncludePath(dir+"mjoptim");
    gInterpreter->AddIncludePath(dir+"mjtrain");
    gInterpreter->AddIncludePath(dir+"mmain");
    gInterpreter->AddIncludePath(dir+"mmc");
    gInterpreter->AddIncludePath(dir+"mmontecarlo");
    gInterpreter->AddIncludePath(dir+"mmuon");
    gInterpreter->AddIncludePath(dir+"mpedestal");
    gInterpreter->AddIncludePath(dir+"mpointing");
    gInterpreter->AddIncludePath(dir+"mranforest");
    gInterpreter->AddIncludePath(dir+"mraw");
    gInterpreter->AddIncludePath(dir+"mreflector");
    gInterpreter->AddIncludePath(dir+"mreport");
    gInterpreter->AddIncludePath(dir+"msignal");
    gInterpreter->AddIncludePath(dir+"msql");
    gInterpreter->AddIncludePath(dir+"mstarcam");
    gInterpreter->AddIncludePath(dir+"mtools");
    gInterpreter->AddIncludePath(dir+"mtrigger");
    gInterpreter->AddIncludePath(dir+"melectronics");
    gInterpreter->AddIncludePath(dir+"msim");
    gInterpreter->AddIncludePath(dir+"msimreflector");
    gInterpreter->AddIncludePath(dir+"msimcamera");
    gInterpreter->AddIncludePath(dir+"mcorsika");
    gInterpreter->AddIncludePath(dir+"mextralgo");

    TString opt(gSystem->GetFlagsOpt());
    TString dbg(gSystem->GetFlagsDebug());
    TString lib(gSystem->GetLinkedLibs());

    TString add = " -O5 -std=c++0x -Wall -Winit-self -fPIC -Wpointer-arith -Wcast-align -Woverloaded-virtual";
    add += " -D__MARS__";
    add += " -DHAVE_ZLIB";
    add += " -DPACKAGE_NAME='\""+MARS::GetPackageName()+"\"'";
    add += " -DPACKAGE_VERSION='\""+MARS::GetPackageName()+"\"'";
    add += " -DREVISION='\""+MARS::GetRevision()+"\"'";

    opt += add;
    dbg += add;

    lib += " -lnova -lz";

    gSystem->SetLinkedLibs(lib);
    gSystem->SetFlagsOpt(opt);
    gSystem->SetFlagsDebug(dbg);


    cout << "\033[32m" << "Welcome to the Mars Root environment." << "\033[0m" << endl;
    cout << endl;
}
