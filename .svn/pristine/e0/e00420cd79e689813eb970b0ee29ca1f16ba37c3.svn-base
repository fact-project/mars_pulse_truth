/* ======================================================================== *\
! $Name: not supported by cvs2svn $:$Id: plotusage.C,v 1.2 2008-08-15 12:14:52 dorner Exp $
! --------------------------------------------------------------------------
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
!   Author(s): Thomas Bretz, 06/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniela Droner, 08/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// plotusage.C
// ===========
//
// This macro is used to read parameters (condor usage of the cluster) from
// the DB and plot them.
// 
// Usage:
//   .x plotusage.C   --> all values in the DB are plotted
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
/////////////////////////////////////////////////////////////////////////////
#include "plotdb.C"

void plotallusage(MPlot &plot)
{
    // plot.SetGroupBy(MPlot::kGroupByDay);

    plot.SetPrimaryDate("CondorStatus.fTime");

    // plot.SetPrimaryNumber("Sequences.fSequenceFirst");
    // plot.SetCondition("((Sequences.fSequenceFirst>95000 AND Sequences.fSequenceFirst<261988) OR Sequences.fSequenceFirst>267750)"
    //                   "AND Star.fAvgTemperature<25 AND Star.fMuonNumber>500");

    plot.SetSecondary("CondorStatus.fLoadTotal");
    plot.SetDescription("Total number of available CPUs;Available CPUs", "TotCPU");
    plot.Plot("CondorStatus.fNumTotal as CPUs",  0,  50, 1);

    plot.SetSecondary("CondorStatus.fLoadClaimed");
    plot.SetDescription("Total number of claimed CPUs;Claimed CPUs", "ClaimedCPU");
    plot.Plot("CondorStatus.fNumClaimed as CPUs",  0,  50, 1);

    plot.SetSecondary("CondorStatus.fNumClaimed");
    plot.SetDescription("Total load of available CPUs;Total load", "AbsTotLoad");
    plot.Plot("CondorStatus.fLoadTotal",  0,  50, 1);

    plot.SetSecondary("CondorStatus.fNumClaimed");
    plot.SetDescription("Total load of claimed CPUs;Claimed load", "AbsClaimedLoad");
    plot.Plot("CondorStatus.fLoadClaimed",  0,  50, 1);

    plot.SetSecondary("CondorStatus.fNumClaimed");
    plot.SetDescription("Relative load per available CPUs;Relative load", "RelTotLoad");
    plot.Plot("CondorStatus.fLoadTotal/CondorStatus.fNumTotal",  0,  1.2, 0.02);

    plot.SetSecondary("CondorStatus.fNumClaimed");
    plot.SetDescription("Relative load per claimed CPUs;Relative load", "RelClaimedLoad");
    plot.Plot("CondorStatus.fLoadClaimed/CondorStatus.fNumClaimed",  0,  1.2, 0.02);
}

int plotusage(TString path="")
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotusage" << endl;
    cout << "---------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    MStatusDisplay *d = new MStatusDisplay;
    d->SetWindowName(serv.GetName());
    d->SetTitle(serv.GetName());

    MPlot plot(serv);
    plot.SetDisplay(d);

    plotallusage(plot);

    d->SaveAsRoot(path+"plotusage.root");
//    d->SaveAsPS("plotusage.ps");

    return 1;
}

