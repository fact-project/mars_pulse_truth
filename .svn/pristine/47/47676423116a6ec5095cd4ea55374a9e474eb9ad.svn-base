/* ======================================================================== *\
! $Name: not supported by cvs2svn $:$Id: plotdb.C,v 1.56 2009-03-16 15:13:43 tbretz Exp $
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
// plotdb.C
// ========
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
//   .x plotdb.C   --> all values in the DB are plotted
// You can chose are certain period:
//   .x plotdb.C(25)   --> all values from period 25 are plotted
// or a time period from a certain date to a certain date
//   .x plotdb.C("2004-11-14 00:00:00", "2005-02-28 00:00:00")
//  --> all values from 14.11.2004 0h to 28.2.2005 0h are plotted
// or all data, but with dataset data highlighted
//   .x plotdb.C("dataset.txt", "outpath")
//  --> the sequences defined in dataset.txt are highlighted (blue:on, red:off)
//  --> You can also add a dataset-name as last argument to one of the
//      calls above
//  --> You must give an outpath (but can be "") where to store plotdb.root
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>

#include <TH1.h>
#include <TPad.h>
#include <TLine.h>
#include <TText.h>
#include <TFrame.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TPRegexp.h>
#include <TSQLRow.h>
#include <TSQLResult.h>
#include <TGraphErrors.h>

#include "MTime.h"
#include "MAstro.h"
#include "MDataSet.h"
#include "MSQLMagic.h"
#include "MStatusDisplay.h"

class MPlot : public MParContainer
{
public:
    // Possible constants to group-by (average) over a certain period
    enum GroupBy_t
    {
        kNone,
        kGroupByPrimary,
        kGroupByHour,
        kGroupByDay,
        kGroupByNight,
        kGroupByWeek,
        kGroupByMonth,
        kGroupBySeason,
        kGroupByYear
    };

private:
    MSQLMagic &fServer;         // Reference to the sql-server class

    MDataSet *fDataSet;         // A possible dtaset to highlite single points

    TString   fPrimaryDate;     // The name of the data we plot
    TString   fPrimaryNumber;   // The corresponding name for the key number
    TString   fSecondary;       // The value versus which the second plot is made

    TString   fRequestFrom;     // Start of a requested date range
    TString   fRequestTo;       // End of a requested date range
    Int_t     fRequestPeriod;   // A possible requested period

    Float_t   fPlotMin;
    Float_t   fPlotMax;

    Float_t   fHistMin;
    Float_t   fHistMax;

    TString   fDescription;     // The description (title) of the plot
    TString   fNameTab;         // The name of the tab in the display

    TString   fCondition;       // An additional condition added to the query
    GroupBy_t fGroupBy;         // A possible Group-By flag

    // --------------------------------------------------------------------------
    //
    // Function to plot the result of the query
    //
    void PlotTable(TSQLResult &res, TString name, Float_t fmin, Float_t fmax, Float_t resolution)
    {
        // Enable all otions in the statistics box
        gStyle->SetOptStat(111111);

        // Create TGraph objects
        TGraph &gt = res.GetFieldCount()>4 ? *new TGraphErrors : *new TGraph;
        gt.SetNameTitle(Form("%s_time", res.GetFieldName(2)), Form("%s vs %s", res.GetFieldName(2), res.GetFieldName(0)));
        gt.SetMarkerStyle(kFullDotMedium);

        TGraph gz;
        gz.SetNameTitle(res.GetFieldName(2), Form("%s vs %s", res.GetFieldName(2), res.GetFieldName(1)));
        gz.SetMarkerStyle(kFullDotMedium);

        TGraph gt0, gt1;
        gt0.SetMarkerColor(kRed);
        gt1.SetMarkerColor(kBlue);
        gt0.SetMarkerStyle(kFullDotLarge);
        gt1.SetMarkerStyle(kFullDotLarge);

        TGraph gz0, gz1;
        gz0.SetMarkerColor(kRed);
        gz1.SetMarkerColor(kBlue);
        gz0.SetMarkerStyle(kFullDotLarge);
        gz1.SetMarkerStyle(kFullDotLarge);

        Int_t first = -1;
        Int_t last  = -1;

        // Loop over the data
        TSQLRow *row = 0;
        while ((row=res.Next()))
        {
            Int_t pos=0;
            // Get all fields of this row
            const char *date  = (*row)[pos++];
            const char *zd    = (*row)[pos++];
            const char *val   = (*row)[pos++];
            const char *zderr = fGroupBy!=kNone ? (*row)[pos++] : 0;
            const char *verr  = fGroupBy!=kNone ? (*row)[pos++] : 0;
            const char *snum  = fDataSet        ? (*row)[pos++] : 0;

            delete row;

            if (!date || !val || !zd)
                continue;

            // check if date is valid
            MTime t(date);
            if (!t.SetSqlDateTime(date))
                continue;

            // check if it belongs to the requested MAGIC period
            if (fRequestPeriod>0 && MAstro::GetMagicPeriod(t.GetMjd())!=fRequestPeriod)
                continue;

            // Get axis range
            if (first<0)
                first = TMath::Nint(TMath::Floor(t.GetMjd()));
            last = TMath::Nint(TMath::Ceil(t.GetMjd()));

            // Convert a possible key number into a integer
            UInt_t seq = snum ? atoi(snum) : 0;

            // convert primary and secondary value into floats
            Float_t value = atof(val);
            Float_t zenith = atof(zd);

            // If a datset is given add the point to the special TGraphs
            // used for highliting these dates
            if (fDataSet)
            {
                if (fDataSet->HasOnSequence(seq))
                {
                    gt1.SetPoint(gt1.GetN(), t.GetAxisTime(), value);
                    gz1.SetPoint(gz1.GetN(), zenith, value);
                }

                if (fDataSet->HasOffSequence(seq))
                {
                    gt0.SetPoint(gt0.GetN(), t.GetAxisTime(), value);
                    gz0.SetPoint(gz0.GetN(), zenith, value);
                }
            }

            // Add Data to TGraph
            gt.SetPoint(gt.GetN(), t.GetAxisTime(), value);
            gz.SetPoint(gz.GetN(), zenith, value);

            // Set error-bar, if one
            if (verr)
                static_cast<TGraphErrors&>(gt).SetPointError(gt.GetN()-1, 0, atof(verr));
        }

        // If this is done earlier the plots remain empty since root 5.12/00
        if (fmax>fmin)
        {
            gt.SetMinimum(fmin);
            gt.SetMaximum(fmax);
            gz.SetMinimum(fmin);
            gz.SetMaximum(fmax);
        }

        gROOT->SetSelectedPad(0);

        TString cname  = fNameTab.IsNull() ? name(name.First('.')+2, name.Length()) : fNameTab;
        TString ctitle = fDescription.IsNull() ? name.Data() : fDescription.Data();
        if (ctitle.First(';')>0)
            ctitle.Remove(ctitle.First(';'), ctitle.Length());

        // Create a TCanvas or open a new tab
        TCanvas &c = fDisplay ? fDisplay->AddTab(cname, ctitle) : *new TCanvas(cname, ctitle);
        // Set fillcolor, remove border and divide pad
        c.SetFillColor(kWhite);
        c.SetFrameBorderMode(0);
        c.SetBorderMode(0);
        c.Divide(1,2);

        // Output mean and rms to console
        cerr << setprecision(4) << setw(10) << cname << ":   ";
        if (gt.GetN()==0)
        {
            cerr << "     <empty>" << endl;
            return;
        }
        cerr << setw(8) << gt.GetMean(2) << "+-" << setw(8) << gt.GetRMS(2) << "   ";
        if (gt0.GetN()>0 || gt1.GetN()>0)
        {
            cerr << setw(8) << gt1.GetMean(2) << "+-" << setw(8) << gt1.GetRMS(2) << "   ";
            cerr << setw(8) << gt0.GetMean(2) << "+-" << setw(8) << gt0.GetRMS(2);
        }
        cerr << endl;

        // format axis
        TH1 *h = gt.GetHistogram();

        const Double_t min = fHistMin>fHistMax ? h->GetMinimum()-resolution/2 : fHistMin;
        const Double_t max = fHistMin>fHistMax ? h->GetMaximum()+resolution/2 : fHistMax;

        // Create histogram
        const Int_t n = resolution>0 ? TMath::Nint((max-min)/resolution) : 50;

        TH1F hist("Hist", Form("Distribution of %s", fDescription.IsNull() ? name.Data() : fDescription.Data()), n, min, max);
        hist.SetDirectory(0);

        // Fill data into histogra,
        for (int i=0; i<gt.GetN(); i++)
            hist.Fill(gt.GetY()[i]);

        // Format histogram
        if (fDescription.IsNull())
            hist.SetXTitle(name);
        hist.SetYTitle("Counts");

        TVirtualPad *pad = gPad;

        // draw contants of pad 2 (counting starts at 0)
        pad->cd(2);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetGridy();

        gPad->SetLeftMargin(0.06);
        gPad->SetRightMargin(0.06);
        gPad->SetBottomMargin(0.08);

        h->SetXTitle("Time");
        h->SetYTitle(hist.GetXaxis()->GetTitle());
        h->GetXaxis()->SetTimeDisplay(1);
        h->GetYaxis()->SetTitleOffset(0.8);
        h->GetXaxis()->SetTitleOffset(1.0);
        h->GetXaxis()->SetLabelOffset(0.01);

        // draw TGraph
        gROOT->SetSelectedPad(0);
        gt.DrawClone("AP");
        if (gt0.GetN()>0)
            gt0.DrawClone("P");
        if (gt1.GetN()>0)
            gt1.DrawClone("P");

        // Add lines and text showing the MAGIC periods
        TLine l;
        TText t;
        Int_t num=0;
        l.SetLineStyle(kDotted);
        l.SetLineColor(kBlue);
        t.SetTextColor(kBlue);
        l.SetLineWidth(1);
        t.SetTextSize(h->GetXaxis()->GetLabelSize());
        t.SetTextAlign(21);
        Int_t p0 = MAstro::GetMagicPeriod(first);
        for (Int_t p = first; p<last; p++)
        {
            Int_t p1 = MAstro::GetMagicPeriod(p);
            if (p1!=p0)
            {
                l.DrawLine(MTime(p).GetAxisTime(), h->GetMinimum(), MTime(p).GetAxisTime(), h->GetMaximum());
                t.DrawText(MTime(p+15).GetAxisTime(), h->GetMaximum(), Form("%d", p1));
                num++;
            }
            p0 = p1;
        }
        if (num<4)
            gPad->SetGridx();

        // Go back to first (upper) pad, format it and divide it again
        pad->cd(1);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->Divide(2,1);

        TVirtualPad *pad2 = gPad;

        // format left pad
        pad2->cd(1);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();

        // plot histogram
        hist.DrawCopy("");

        // format right pad
        pad2->cd(2);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetGridy();

        // draw graph
        gROOT->SetSelectedPad(0);
        gz.GetXaxis()->SetTitle(res.GetFieldName(1));
        gz.GetYaxis()->SetTitle(hist.GetXaxis()->GetTitle());
        gz.DrawClone("AP");

        if (gz0.GetN()>0)
            gz0.DrawClone("P");
        if (gz1.GetN()>0)
            gz1.DrawClone("P");
    }

public:
    MPlot(MSQLMagic &server) : fServer(server), fDataSet(NULL),
        fRequestPeriod(-1), fPlotMin(0), fPlotMax(-1), fHistMin(0), fHistMax(-1), fGroupBy(kNone)
    {
    }
    ~MPlot()
    {
        if (fDataSet)
            delete fDataSet;
    }
    void SetDataSet(const TString filename, UInt_t num=(UInt_t)-1)
    {
        if (fDataSet)
        {
            delete fDataSet;
            fDataSet = NULL;
        }

        fPrimaryNumber="";

        if (filename.IsNull())
            return;

        fDataSet = new MDataSet(filename, num);
        fPrimaryNumber="Sequences.fSequenceFirst";
    }
    void SetPlotRange(Float_t min, Float_t max/*, Int_t n=5*/) { fPlotMin = min; fPlotMax = max; }
    void SetHistRange(Float_t min, Float_t max) { fHistMin = min; fHistMax = max; }
    void SetRequestRange(const char *from="", const char *to="") { fRequestFrom = from; fRequestTo = to; }
    void SetRequestPeriod(Int_t n=-1) { fRequestPeriod = n; }
    void SetCondition(const char *cond="") { fCondition = cond; }
    void SetDescription(const char *d, const char *t=0) { fDescription = d; fNameTab = t; }
    void SetTabName(const char *t) { fNameTab = t; }
    void SetGroupBy(GroupBy_t b=kGroupByWeek) { fGroupBy=b; }
    void SetPrimaryDate(const char *ts) { fPrimaryDate=ts; }
    void SetPrimaryNumber(const char *ts) { }
    void SetSecondary(const char *ts) { fSecondary=ts; }

    MStatusDisplay *GetDisplay() { return fDisplay; }
    MSQLMagic &GetServer() { return fServer; }

    Bool_t Plot(const char *value, Float_t min=0, Float_t max=-1, Float_t resolution=0)
    {
        TString named  = fPrimaryDate;
        TString named2 = fSecondary;
        TString namev  = value;

        TString tablev = namev(0, namev.First('.'));
        TString valuev = namev(namev.First('.')+1, namev.Length());

        TString tabled = named(0, named.First('.'));
        TString valued = named(named.First('.')+1, named.Length());

        TString query="SELECT ";
        switch (fGroupBy)
        {
        case kNone:
        case kGroupByPrimary:
            query += Form("%s AS %s", named.Data(), valued.Data()+1);
            break;
        case kGroupByHour:
            query += Form("DATE_FORMAT(%s, '%%Y-%%m-%%d %%H:30:00') AS %s ", fPrimaryDate.Data(), valued.Data()+1);
            break;
        case kGroupByNight:
            query += Form("DATE_FORMAT(ADDDATE(%s,Interval 12 hour), '%%Y-%%m-%%d 00:00:00') AS %s ", fPrimaryDate.Data(), valued.Data()+1);
            break;
        case kGroupByDay:
            query += Form("DATE_FORMAT(%s, '%%Y-%%m-%%d 00:00:00') AS %s ", fPrimaryDate.Data(), valued.Data()+1);
            break;
        case kGroupByWeek:
            query += Form("DATE_FORMAT(ADDDATE(%s,Interval 12 hour), '%%x%%v') AS %s ", fPrimaryDate.Data(), valued.Data()+1);
            break;
        case kGroupByMonth:
            query += Form("DATE_FORMAT(ADDDATE(%s,Interval 12 hour), '%%Y-%%m-15 00:00:00') AS %s ", fPrimaryDate.Data(), valued.Data()+1);
            break;
        case kGroupBySeason:
            //query += Form("DATE_FORMAT(ADDDATE(%s,Interval 12 hour), '%%Y-%%m-15 00:00:00') AS %s ", fPrimaryDate.Data(), valued.Data()+1);
            break;
        case kGroupByYear:
            query += Form("DATE_FORMAT(ADDDATE(%s,Interval 12 hour), '%%Y-08-15 00:00:00') AS %s ", fPrimaryDate.Data(), valued.Data()+1);
            break;
        }

        if (fGroupBy==kNone)
        {
            query += ", ";
            query += fSecondary;
            query += ", ";
            query += value;
        }
        else
        {
            TString  v(value);
            TPRegexp regexp(" +[aA][sS] +[a-zA-Z0-9]+ *$");

            TString as = v(regexp);
            Ssiz_t  p  = v.Index(as);

            v.Prepend("(");
            v.Insert(p<=0?v.Length():p+1, ")");

            query += ", AVG(";
            query += fSecondary;
            query += "), AVG(";
            query += v;
            query += "), STD(";
            query += fSecondary;
            query += "), STD(";
            query += v;
            query += ")";
        }

        if (fDataSet)
        {
            query += ", ";
            query += fPrimaryNumber;
        }

        query += Form(" FROM %s ", tabled.Data());

        const Bool_t interval = !fRequestFrom.IsNull() && !fRequestTo.IsNull();

        TString where(fCondition);
        if (interval)
        {
            if (!where.IsNull())
                where += " AND ";
            where += Form("%s BETWEEN '%s' AND '%s' ",
                          fPrimaryDate.Data(), fRequestFrom.Data(), fRequestTo.Data());
        }

        // ------------------------------

        query += fServer.GetJoins(tabled, query+" "+where);

        if (!where.IsNull())
        {
            query += "WHERE ";
            query += where;
            query += " ";
        }

        if (fGroupBy!=kNone)
        {
            query += Form("GROUP BY %s ", valued.Data()+1);
            //query += Form(" HAVING COUNT(%s)=(COUNT(*)+1)/2 ", valuev.Data());
        }
        query += Form("ORDER BY %s ", valued.Data()+1);

        // ------------------------------
        TSQLResult *res = fServer.Query(query);
        if (!res)
        {
            cout << "ERROR - Query failed: " << query << endl;
            return kFALSE;
        }

        if (max>min)
            PlotTable(*res, namev, min, max, resolution);
        else
            PlotTable(*res, namev, fPlotMin, fPlotMax, resolution);


        delete res;
        return kTRUE;
    }
//    ClassDef(MPlot, 0)
};

//ClassImp(MPlot);

void plotalldb(MPlot &plot)
{
    //plot.SetGroupBy(MPlot::kGroupByMonth);
    //plot.SetDataSet("dataset.txt");

    plot.SetPrimaryDate("Sequences.fRunStart");
    plot.SetSecondary("(Sequences.fZenithDistanceMin+Sequences.fZenithDistanceMax)/2 as '<Zd>'");

    //inner camera
    //from calib*.root
    plot.SetDescription("Median number of calibration phe in inner pixels;C [phe]", "CalPheI");
    plot.Plot("Calibration.fMedNumPheInner", 0, 200, 1);
    plot.SetDescription("Relative rms of calibration charge inner pixels;\\sigma_C [%]", "CalRmsI");
    plot.Plot("Calibration.fRelChargeRmsInner", 0, 0.5, 0.01);
    plot.SetDescription("Conversion Factor inner Camera;C_{I} [phe/fadc cnts]", "ConvI");
    plot.Plot("Calibration.fConvFactorInner",  0, 0.7, 0.002);
    plot.SetDescription("Mean Arrival Time inner Camera;T_{I} [sl]", "ArrTmI");
    plot.Plot("Calibration.fArrTimeMeanInner", 0, 40.0, 0.1);
    plot.SetDescription("RMS Arrival Time inner Camera;\\sigma_{T,I} [sl]", "RmsArrTmI");
    plot.Plot("Calibration.fArrTimeRmsInner",  0, 6.5, 0.01);
    plot.SetDescription("Number of unsuitable pixels inner Camera;N_{I}", "UnsuitI");
    plot.Plot("Calibration.fUnsuitableInner",  0, 75, 1);
    plot.SetDescription("Number of unsuitable pixels >50%;N", "Unsuit50");
    plot.Plot("Calibration.fUnsuitable50",  0, 75, 1);
    plot.SetDescription("Number of unsuitable pixels >1%;N", "Unsuit01");
    plot.Plot("Calibration.fUnsuitable01",  0, 200, 5);

    //from signal*.root
    plot.SetDescription("Mean Pedestal RMS inner Camera;\\sigma_{P,I} [phe]", "PedRmsI");
    plot.Plot("Calibration.fMeanPedRmsInner",  0, 3.5, 0.05);
    plot.SetDescription("Mean Signal inner Camera;S_{I} [phe]", "SignalI");
    plot.Plot("Calibration.fMeanSignalInner",  0, 7.0, 0.05);

    plot.SetDescription("Mean PulsePosCheck (maximum slice) inner camera;T [sl]", "ChkPos");
    plot.Plot("Calibration.fPulsePosCheckMean", 1, 50.0, 0.1);
    plot.SetDescription("Rms PulsePosCheck (maximum slice) inner camera;T [sl]", "ChkRms");
    plot.Plot("Calibration.fPulsePosCheckRms", 0, 12.0, 0.1);
    plot.SetDescription("Mean calibrated PulsePos (as extracted);T [ns]", "PulPos");
    plot.Plot("Calibration.fPulsePosMean", 1, 40.0, 0.1);
    plot.SetDescription("Rms calibrated PulsePos (as extracted);T [ns]", "PulRms");
    plot.Plot("Calibration.fPulsePosRms", 0, 3.0, 0.02);

    plot.SetDescription("Ratio of accepted calibration events in cal-run;f [%]", "RatioCal");
    plot.Plot("Calibration.fRatioCalEvents", 0, 105, 5);

    plot.SetDescription("Average rate of events with lvl1 trigger;R [Hz]", "RateTrig");
    plot.Plot("Calibration.fRateTrigEvts", 0, 500, 5);
    plot.SetDescription("Average rate of events with only Sum trigger;R [Hz]", "RateSum");
    plot.Plot("Calibration.fRateSumEvts", 0, 1500, 5);
    //plot.SetDescription("Ratio of only Sum to Lvl1 triggers;R [Hz]", "RateRatio");
    //plot.Plot("100*Calibration.fRateSumEvts/Calibration.fRateTrigEvts", 0, 500, 1);
    plot.SetDescription("Average rate of events with calibration trigger;R [Hz]", "RateCal");
    plot.Plot("Calibration.fRateCalEvts", 0, 75, 1);
    plot.SetDescription("Average rate of events with pedestal trigger;R [Hz]", "RatePed");
    plot.Plot("Calibration.fRatePedEvts", 0, 50, 1);
    plot.SetDescription("Average rate of events with ped+cosmics trigger;R [Hz]", "RatePT");
    plot.Plot("Calibration.fRatePedTrigEvts", 0, 4.0, 0.1);
    plot.SetDescription("Average rate of events without trigger pattern;R [Hz]", "Rate0");
    plot.Plot("Calibration.fRateNullEvts", 0, 3.5, 0.1);
    plot.SetDescription("Average rate of unknown trigger pattern;R [Hz]", "RateUnknown");
    plot.Plot("Calibration.fRateUnknownEvts", 0, 3.5, 0.1);

    plot.SetDescription("Hi-/Lo-Gain offset;", "PulOff");
    plot.Plot("Calibration.fPulsePosOffMed", -0.33, 0.5, 0.01);
    plot.SetDescription("Hi-/Lo-Gain ratio;", "HiLoRatio");
    plot.Plot("Calibration.fHiLoGainRatioMed", 10, 15, 0.05);

    //plot.SetDescription("Pulse Variance;", "PulVar");
    //plot.Plot("Calibration.fPulsePosVar", 0, 0.03, 0.001);

    //from star*.root
    //muon
    plot.SetCondition("Star.fMuonNumber>300");
    plot.SetDescription("Point Spred Function;PSF [mm]");
    plot.Plot("Star.fPSF",                     0,  30, 0.5);
    plot.SetDescription("Muon Calibration Ratio Data/MC;r [1]", "MuonCal");
    plot.Plot("Star.fRatio",                   50, 150, 0.5);
    plot.SetDescription("Muon Rate after Muon Cuts;R [Hz]");
    plot.Plot("Star.fMuonRate",                0, 2.0, 0.05);
    plot.SetCondition();

    //quality
    plot.SetDescription("Datarate [Hz]", "Rate");
    plot.Plot("Star.fDataRate",                0, 600, 10);
    plot.SetDescription("Camera Inhomogeneity;\\sigma [%]", "Inhom");
    plot.Plot("Star.fInhomogeneity",           0, 100, 1);
    plot.SetDescription("Camera Spark Rate;R [Hz]", "Sparks");
    plot.Plot("Star.fSparkRate",               0.075, 7.425, 0.05);
    plot.SetDescription("Relative effective on time;T_{eff}/T_{obs} [ratio]", "RelTime");
    plot.Plot("Star.fEffOnTime/Sequences.fRunTime",    0.006, 1.506, 0.01);
    //imgpar
    plot.SetDescription("Mean Number of Islands after cleaning;N [#]", "NumIsl");
    plot.Plot("Star.fMeanNumberIslands",       0.5, 4.5, 0.01);
    //weather
    plot.SetCondition("");
    plot.SetDescription("Average Temperature [\\circ C];T [\\circ C]", "Temp");
    plot.Plot("Star.fAvgTemperature",        -5, 25, 1);
    plot.SetDescription("Average Cloudiness [%]", "AvgClouds");
    plot.Plot("Star.fAvgCloudiness",           0, 100, 1);
    plot.SetDescription("RMS Cloudiness [%]", "RmsClouds");
    plot.Plot("Star.fRmsCloudiness",           0,  30, 1);
    plot.SetDescription("Sky Temperature [K]", "SkyTemp");
    plot.Plot("Star.fAvgTempSky",         180, 280, 1);
    plot.SetDescription("Maximum Humidity [%]", "MaxHum");
    plot.Plot("Star.fMaxHumidity",             0, 100, 1);
    plot.SetDescription("Average Humidity [%]", "AvgHum");
    plot.Plot("Star.fAvgHumidity",             0, 100, 1);
    plot.SetDescription("Average Wind Speed [km/h];v [km/h]", "Wind");
    plot.Plot("Star.fAvgWindSpeed",        0, 50, 1);

    //currents
    plot.SetSecondary("Calibration.fMeanPedRmsInner as '<PedRMS_{I}>'");
    plot.SetDescription("Minimum average DC current [nA];<I_{min}> [nA]", "MinDC");
    plot.Plot("Star.fMinCurrents",        0, 25, 0.1);
    plot.SetDescription("Median average DC current [nA];<I_{med}> [nA]", "MedDC");
    plot.Plot("Star.fMedCurrents",        0, 25, 0.1);
    plot.SetDescription("Maximum average DC current [nA];<I_{max}> [nA]", "MaxDC");
    plot.Plot("Star.fMaxCurrents",        0, 25, 0.1);
    plot.SetSecondary("(Sequences.fZenithDistanceMin+Sequences.fZenithDistanceMax)/2 as '<Zd>'");

    //muon
    //plot.SetDescription("Number of Muons after Muon Cuts;N [#]");
    //plot.Plot("Star.fMuonNumber",              0, 10000, 100);

    // starguider
    plot.SetDescription("Median No. Stars recognized by the starguider;N_{0}", "StarsMed");
    plot.Plot("Star.fNumStarsMed",                          0,  100,  1);
    plot.SetDescription("RMS No. Stars recognized by the starguider;\\sigma_{N_{0}}", "StarsRMS");
    plot.Plot("Star.fNumStarsRMS",                          0,   25,  1);
    plot.SetDescription("Median No. Stars correlated by the starguider;N", "CorMed");
    plot.Plot("Star.fNumStarsCorMed",                       0,  100,  1);
    plot.SetDescription("RMS No. Stars correlated by the starguider;\\sigma_{N}", "CorRMS");
    plot.Plot("Star.fNumStarsCorRMS",                       0,   25,  1);
    plot.SetDescription("Relative number of correlated stars;N/N_{0} [%]", "StarsRel");
    plot.Plot("Star.fNumStarsCorMed/Star.fNumStarsMed*100", 0, 100, 10);
    plot.SetDescription("Median skbrightess measured by the starguider;B [au]", "BrightMed");
    plot.Plot("Star.fBrightnessMed",                        0, 111,  1);
    plot.SetDescription("RMS skybrightess measured by the starguider;\\sigma_{B} [au]", "BrightRMS");
    plot.Plot("Star.fBrightnessRMS",                        0,  64,  1);

    //outer camera
    //from calib*.root
    plot.SetDescription("Median number of calibration phe in outer pixels;C [phe]", "CalPheO");
    plot.Plot("Calibration.fMedNumPheOuter", 0, 200, 1);
    plot.SetDescription("Relative rms of calibration charge outer pixels;\\sigma_C [%]", "CalRmsO");
    plot.Plot("Calibration.fRelChargeRmsOuter", 0, 0.5, 0.01);
    plot.SetDescription("Conversion Factor outer Camera;C_{O} [phe/fadc cnts]", "ConvO");
    plot.Plot("Calibration.fConvFactorOuter",  0, 3.0, 0.01);
    plot.SetDescription("Mean Arrival Time outer Camera;T_{O} [sl]", "ArrTmO");
    plot.Plot("Calibration.fArrTimeMeanOuter", 0, 45, 0.1);
    plot.SetDescription("RMS Arrival Time outer Camera;\\sigma_{T,O} [sl]", "RmsArrTmO");
    plot.Plot("Calibration.fArrTimeRmsOuter",  0, 4.5, 0.01);
    plot.SetDescription("Number of unsuitable pixels outer Camera;N_{O}", "UnsuitO");
    plot.Plot("Calibration.fUnsuitableOuter",  0, 25, 1);
    //from signal*.root
    plot.SetDescription("Mean Pedestal RMS outer Camera;\\sigma_{P,O} [phe]", "PedRmsO");
    plot.Plot("Calibration.fMeanPedRmsOuter",  0, 4.0, 0.05);
    plot.SetDescription("Mean Signal outer Camera;S_{O} [phe]", "SignalO");
    plot.Plot("Calibration.fMeanSignalOuter",  0, 4.0, 0.05);
}

int plotdb(TString from, TString to, const char *dataset=0, UInt_t num=(UInt_t)-1)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotdb" << endl;
    cout << "------" << endl;
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
    plotalldb(plot);
    d->SaveAsRoot("plotdb.root");
    //d->SaveAsPS("plotdb.ps");

    return 1;
}

int plotdb(const char *ds, UInt_t num, TString path="/magic/sequences")
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotdb" << endl;
    cout << "------" << endl;
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
    plotalldb(plot);
    d->SaveAsRoot(path+"plotdb.root");
//    d->SaveAsPS("plotdb.ps");

    return 1;
}

int plotdb(const char *ds, TString path="/magic/sequences")
{
    return plotdb(ds, (UInt_t)-1, path);
}

int plotdb(TString path)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotdb" << endl;
    cout << "------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    MStatusDisplay *d = new MStatusDisplay;
    d->SetWindowName(serv.GetName());
    d->SetTitle(serv.GetName());

    MPlot plot(serv);
//    plot.SetDataSet(ds);
    plot.SetDisplay(d);
    plot.SetRequestRange("", "");
    plotalldb(plot);
    d->SaveAsRoot(path+"plotdb.root");
//    d->SaveAsPS("plotdb.ps");

    return 1;
}

int plotdb(Int_t period, const char *dataset="")
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotdb" << endl;
    cout << "------" << endl;
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
    plotalldb(plot);
    d->SaveAsRoot("plotdb.root");
    //d->SaveAsPS("plotdb.ps");

    return 1;
}

int plotdb()
{
    return plotdb("", "", "");
}
