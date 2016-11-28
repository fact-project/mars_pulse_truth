/* ======================================================================== *\
! $Name: not supported by cvs2svn $:$Id: plotoptical.C,v 1.11 2009-01-27 13:56:15 tbretz Exp $
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
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// plotoptical.C
// =============
//
// This macro is used to read optical data from the DB and plot them.
// 
// In the DB these values are stored in the tables Calibration and Star.
//
//
// To plot the whole database simple use:
//   .x plotdb.C+
//
// Plot only data for one source. For the available sources see the database
//   .x plotdb.C+("source")
//
// You can chose are certain MAGIC-period:
//   .x plotdb.C+(25)   --> all values from period 25 are plotted
//
// MAGIC periods correspond to one moon-phase and are defined as
// moon period-284. For details see MAstro::MoonPeriod and
// MAstro::MagicPeriod.
//
// or a time period from a certain date to a certain date
//   .x plotdb.C+("2004-11-14 00:00:00", "2005-02-28 00:00:00")
//  --> all values from 14.11.2004 0h to 28.2.2005 0h are plotted
//   .x plotdb.C+("2004-11-14 00:00:00", "2005-02-28 00:00:00", "source")
//  --> all values from 14.11.2004 0h to 28.2.2005 0h of "source" are plotted
//
//
// For details of the plots produced see the function plotall() below.
//
//
// The plot title and axis-titles are created by:
//    plot.SetDescription("Title;x", "TabName");
//
// Drawing the plot is initiated by
//    plot.Plot("OpticalData.fExposure", min, max, width);
//
// While OpticalData.fExposure can be any kind of variable to plot.
// min and max are the minimum and maximum of the histogram which is
// filled and width is the bin-width of this histogram.
//
// To group data (average) of a certain period use:
//    plot.GroupBy(MPlot::kGroupByNight);
// before initiating the plot.
//
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
/////////////////////////////////////////////////////////////////////////////
#include "plotdb.C"

void plotalloptical(MPlot &plot, TString source)
{
    // Setup here the values for timestamp and secondary (upper/right) plot
    plot.SetPrimaryDate("OpticalData.fTimestamp");
    plot.SetPrimaryNumber("OpticalData.fTimestamp");
    plot.SetSecondary("OpticalData.fZenithDistance");

    // This is the condition to take only the "ok" flagged data
    // and to restrict the query to a given source (if any)
    TString cond = "fStatusKEY=1 AND fTelescopeKEY=1 AND fCCDKEY=1 AND fFilterKEY=1";
    if (!source.IsNull())
    {
        const Int_t key = plot.GetServer().QueryKeyOfName("Object", source, kFALSE);
        if (key<0)
            return;
        cond += Form(" AND Object.fObjectKEY=%d", key);
    }

    plot.SetCondition(cond);

    // Plot exposure
    plot.SetDescription("Exposure;T_{E} [s]", "Expo");
    plot.Plot("OpticalData.fExposure",  0, 900, 60);

    // plot sky level
    plot.SetDescription("Sky Level;B [s^{-1}]", "SkyLvl");
    plot.Plot("OpticalData.fSkyLevel/OpticalData.fExposure", 0, 5.0, 0.01);

    // plot FWHM
    plot.SetDescription("Full Width Half Maximum;FWHM [s^{-1}]", "Fwhm");
    plot.Plot("OpticalData.fFWHM/OpticalData.fExposure",  0, 0.05, 0.001);

    // plot Aperture Radius
    plot.SetDescription("Aperture Radius;R_{A}", "ApRad");
    plot.Plot("OpticalData.fApertureRadius", 0, 10, 1);

    /*
     // Plot instrumental magnitude
     plot.SetDescription("Instrumental Magnitude;M_{I}", "InstMag");
     plot.Plot("OpticalData.fInstrumentalMag",  0, 30, 0.5);

     // Plot error of instrumental magnitude
     plot.SetDescription("Instrumental Magnitude Error;\\sigma_{M}", "MagErr");
     plot.Plot("OpticalData.fInstrumentalMagErr",  0, 1, 0.01);
     */

    // Plot magnitude corrected for the exposure
    plot.SetDescription("m_{1};m_{1}", "M1");
    plot.Plot("OpticalData.fInstrumentalMag+2.5*log10(OpticalData.fExposure)", 10, 35, 0.2);

    // Now take out all points named */BL from further queries
    // And skip all sources the magnitude is not known
    cond += " AND Object.fObjectName NOT LIKE '%/BL' AND NOT ISNULL(Object.fMagnitude) ";
    plot.SetCondition(cond);

    // Formula to calculate the extinction
    TString ext("3080/25.0*pow(10, (OpticalData.fInstrumentalMag+2.5*log10(OpticalData.fExposure)-Object.fMagnitude)/-2.5)");
    // Add this to correct for the ZA dependancy
    //    ext += "+0.0028*fZenithDistance-0.08";

    // Group all data of one image together and plot extinction
    plot.SetGroupBy(MPlot::kGroupByPrimary);
    plot.SetDescription("m_{1}-m_{true} (Extinction per Image);m_{1}-m_{true}", "ExtImg");
    plot.Plot(ext, 0.05, 1.2, 0.01);

    // Group data hourly together and plot extinction
    plot.SetGroupBy(MPlot::kGroupByHour);
    plot.SetDescription("m_{1}-m_{true} (Extinction per Hour);m_{1}-m_{true}", "ExtHour");
    plot.Plot(ext, 0.5, 1.2, 0.01);

    // Group data hourly together and plot extinction
    plot.SetGroupBy(MPlot::kGroupByNight);
    plot.SetDescription("m_{1}-m_{true} (Extinction per Night);m_{1}-m_{true}", "ExtNight");
    plot.Plot(ext, 0.5, 1.2, 0.01);

    // Group data monthly together and plot extinction
    plot.SetGroupBy(MPlot::kGroupByMonth);
    plot.SetDescription("m_{1}-m_{true} (Extinction per Month);m_{1}-m_{true}", "ExtMonth");
    plot.Plot(ext, 0.5, 1.2, 0.01);

    // Group data yearly together and plot extinction
    plot.SetGroupBy(MPlot::kGroupByYear);
    plot.SetDescription("m_{1}-m_{true} (Extinction per Year);m_{1}-m_{true}", "ExtYear");
    plot.Plot(ext, 0.5, 1.2, 0.01);
}

int plotoptical(TString from, TString to, const char *source=0)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotoptical" << endl;
    cout << "-----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    MStatusDisplay *d = new MStatusDisplay;
    d->SetWindowName(serv.GetName());
    d->SetTitle(serv.GetName());

    MPlot plot(serv);
    // plot.SetDataSet(dataset);
    plot.SetDisplay(d);
    plot.SetRequestRange(from, to);
    plotalloptical(plot, source);
    // Use this to create output plots automatically
    d->SaveAsRoot("plotoptical.root");
    //  d->SaveAsPS("plotoptical.ps");

    return 1;
}

int plotoptical(const char *source, TString path)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotoptical" << endl;
    cout << "-----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    MStatusDisplay *d = new MStatusDisplay;
    d->SetWindowName(serv.GetName());
    d->SetTitle(serv.GetName());

    MPlot plot(serv);
    // plot.SetDataSet(ds);
    plot.SetDisplay(d);
    plot.SetRequestRange("", "");
    plotalloptical(plot, source);
    // Use this to create output plots automatically
    d->SaveAsRoot("plotoptical.root");
    //  d->SaveAsPS("plotoptical.ps");

    return 1;
}

int plotoptical(Int_t period, const char *source="")
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotoptical" << endl;
    cout << "-----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    MStatusDisplay *d = new MStatusDisplay;
    d->SetWindowName(serv.GetName());
    d->SetTitle(serv.GetName());

    MPlot plot(serv);
    // plot.SetDataSet(dataset);
    plot.SetDisplay(d);
    plot.SetRequestPeriod(period);
    plotalloptical(plot, source);

    // Use this to create output plots automatically
    d->SaveAsRoot("plotoptical.root");
    //  d->SaveAsPS("plotoptical.ps");

    return 1;
}

int plotoptical(TString path)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotoptical" << endl;
    cout << "-----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    MStatusDisplay *d = new MStatusDisplay;
    d->SetWindowName(serv.GetName());
    d->SetTitle(serv.GetName());

    MPlot plot(serv);
    // plot.SetDataSet(dataset);
    plot.SetDisplay(d);
    //plot.SetRequestPeriod(period);
    plotalloptical(plot, 0);

    // Use this to create output plots automatically
    d->SaveAsRoot(path+"plotoptical.root");
    //  d->SaveAsPS("plotoptical.ps");

    return 1;
}

int plotoptical()
{
    return plotoptical("", "");
}
