/* ======================================================================== *\
! $Name: not supported by cvs2svn $:$Id: plotrundb.C,v 1.9 2009-03-16 15:13:43 tbretz Exp $
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
!   Author(s): Thomas Bretz, 05/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniela Dorner, 05/2005 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// plotundb.C
// ==========
//
// This macro is used to read quality parameters from the DB and plot them.
// 
// The parameters are from the following files:
// calib*.root:mean conversion factor, mean arrival time, rms arrival time
// (each parameter for inner and outer camera)
// signal*.root: mean pedestal rms (for inner and outer camera)
// star*.root: PSF, # of Muons, Effective OnTime, Muon rate,
// Ratio MC/Data(MuonSize) and mean number of islands
// 
// In the DB these values are stored in the tables Calibration and Star.
// 
// Usage:
//   .x plotundb.C   --> all values in the DB are plotted
// You can chose are certain period:
//   .x plotrundb.C(25)   --> all values from period 25 are plotted
// or a time period from a certain date to a certain date
//   .x plotrundb.C("2004-11-14 00:00:00", "2005-02-28 00:00:00")
//  --> all values from 14.11.2004 0h to 28.2.2005 0h are plotted
// or all data, but with dataset data highlighted
//   .x plotrundb.C("dataset.txt")
//  --> the sequences defined in dataset.txt are highlighted (blue:on, red:off)
//  --> You can also add a dataset-name as last argument to one of the
//      calls above
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
/////////////////////////////////////////////////////////////////////////////
#include "plotdb.C"

void plotall(MPlot &plot, Int_t tel=0)
{
    //    plot.SetGroupBy(MPlot::kGroupByNight);

    plot.SetPrimaryDate("RunData.fRunStart");
    plot.SetPrimaryNumber("RunData.fRunNumber");
    plot.SetSecondary("RunData.fZenithDistance");

    MTime t(-1);
    TString cond = "RunData.fExcludedFDAKEY=1 AND RunData.fRunStart BETWEEN '2004-01-01' AND '";
    cond += t.GetSqlDateTime();
    cond += "'";
    if (tel>0)
    {
        cond += " AND fTelescopeNumber=";
        cond += tel;
    }

    //cond +=" AND (RunData.fSourceKEY=403 OR RunData.fSourceKEY=871) ";

    plot.SetCondition(cond);

    //inner camera
    //from calib*.root
    plot.SetDescription("DAQ Storage Rate;R_{DAQ,S} [Hz]", "StoreRate");
    plot.Plot("RunData.fDaqStoreRate",  -0.5, 999.5, 5);
    plot.SetDescription("DAQ Trigger Rate;R_{DAQ,T} [Hz]", "TrigRate");
    plot.Plot("RunData.fDaqTriggerRate", -0.5, 999.5, 5);
    plot.SetDescription("Mean Trigger Rate;<R> [Hz]", "MeanTrig");
    plot.Plot("RunData.fMeanTriggerRate", -0.5, 999.5, 5);
    plot.SetDescription("L2 Trigger rate after prescaler;R_{L2,P}", "Presc");
    plot.Plot("RunData.fL2RatePresc", -0.5, 999.5, 5);
    plot.SetDescription("L2 Trigger rate before prescaler;R_{L2,U}", "Unpresc");
    plot.Plot("RunData.fL2RateUnpresc", -0.5, 999.5, 5);

    plot.SetDescription("L1 trigger rate after cleaning;R_{L1}", "CleanL1");
    plot.Plot("RunDataCheck.fRateCleanedTrig", -0.5, 399.5, 1);
    plot.SetDescription("Sum trigger rate after cleaning;R_{sum}", "CleanSum");
    plot.Plot("RunDataCheck.fRateCleanedSum", -0.5, 399.5, 1);
    plot.SetDescription("Unknown events trigger rate after cleaning;R_{0}", "Clean0");
    plot.Plot("RunDataCheck.fRateCleanedNull", -0.5, 399.5, 1);

/*
    //from signal*.root
    plot.SetDescription("Signal Position;<T_{S}> [sl]", "PosMean");
    plot.Plot("DataCheck.fPositionSignal",  -0.5, 29.5, 1);
    plot.SetDescription("Signal Position FWHM;\\sigma_{S} [sl]", "PosFWHM");
    plot.Plot("DataCheck.fPositionFWHM",  -0.5, 29.5, 1);
    plot.SetDescription("Signal Height;<H_{s}> [counts]", "SigMean");
    plot.Plot("DataCheck.fHeightSignal",  -0.5, 256.5, 1);
    plot.SetDescription("Signal Height FWHM;\\sigma_{H} [counts]", "SigFWHM");
    plot.Plot("DataCheck.fHeightFWHM",  -0.5, 155.5, 1);

    plot.SetDescription("Interleaved Signal Position;<T_{S}> [sl]", "IPosMean");
    plot.Plot("DataCheck.fPositionSignalInterlaced", -0.5, 29.5, 1);
    plot.SetDescription("Interleaved Signal Position FWHM;\\sigma_{S} [sl]", "IPosFWHM");
    plot.Plot("DataCheck.fPositionFWHMInterlaced", -0.5, 29.5, 1);
    plot.SetDescription("Interleaved Signal Height;<H_{s}> [counts]", "ISigMean");
    plot.Plot("DataCheck.fHeightSignalInterlaced", -0.5, 256.5, 1);
    plot.SetDescription("Interleaved Signal Height FWHM;\\sigma_{H} [counts]", "ISigFWHM");
    plot.Plot("DataCheck.fHeightFWHMInterlaced",  -0.5, 155.5, 1);
*/
}

int plotrundb(TString from, TString to, const char *dataset=0, UInt_t num=(UInt_t)-1)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotrundb" << endl;
    cout << "---------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    MStatusDisplay *d = new MStatusDisplay;
    d->SetWindowName(serv.GetName());
    d->SetTitle(serv.GetName());

    MPlot plot(serv);
    plot.SetDataSet(dataset, num);
    plot.SetDisplay(d);
    plot.SetRequestRange(from, to);
    plotall(plot);
    d->SaveAsRoot("plotrundb.root");
    d->SaveAsPS("plotrundb.ps");

    return 1;
}

int plotrundb(const char *ds, UInt_t num=(UInt_t)-1)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotrundb" << endl;
    cout << "---------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    MStatusDisplay *d = new MStatusDisplay;
    d->SetWindowName(serv.GetName());
    d->SetTitle(serv.GetName());

    MPlot plot(serv);
    plot.SetDataSet(ds, num);
    plot.SetDisplay(d);
    plot.SetRequestRange("", "");
    plotall(plot);
    d->SaveAsRoot("plotrundb.root");
    d->SaveAsPS("plotrundb.ps");

    return 1;
}

int plotrundb(Int_t period, const char *dataset="")
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotrundb" << endl;
    cout << "---------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    MStatusDisplay *d = new MStatusDisplay;
    d->SetWindowName(serv.GetName());
    d->SetTitle(serv.GetName());

    MPlot plot(serv);
    plot.SetDataSet(dataset);
    plot.SetDisplay(d);
    plot.SetRequestPeriod(period);
    plotall(plot);
    d->SaveAsRoot("plotrundb.root");
    d->SaveAsPS("plotrundb.ps");

    return 1;
}

int plotrundb()
{
    return plotrundb("", "");
}
