#include <iostream>
#include <iomanip>
#include <fstream>

#include <TCanvas.h>
#include <TSQLRow.h>
#include <TSQLResult.h>
#include <TEnv.h>
#include <TH2F.h>
#include <TPad.h>

#include "MSQLMagic.h"
#include "MStatusDisplay.h"
#include "MTime.h"

using namespace std;

int DataEfficiency()
{
    MSQLServer serv("/home/fact/fast_setup_for_automatic_analysis/Mars.von.Thomas.2012.06.22/sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    // get first night from DB
    TString query="SELECT Min(fNight) from ObservationTimes";
    TSQLResult *res0 = serv.Query(query);
    if (!res0)
        return kFALSE;
    TSQLRow *row0=res0->Next();
    Int_t nightmin=atoi((*row0)[0]);
    delete res0;

    query="SELECT Min(fRunStart), Max(fRunStop) from RunInfo";
    res0 = serv.Query(query);
    if (!res0)
        return kFALSE;
    row0=res0->Next();
    float start=MTime((*row0)[0]).GetAxisTime();
    float stop=MTime((*row0)[1]).GetAxisTime();
    delete res0;

    cout << start << " " << stop << endl;

    query="SELECT fNight, ";
    //time from start to end of astronomical twilight
    query+=" Time_to_sec(Timediff(fStopObservation, fStartObservation)) as astrotime, ";
    //time from start of first to end of last run
    query+=" Time_to_sec(Timediff(Max(fRunStop),Min(fRunStart))) as obstime, ";
    // sum of durations of all runs with duration shorter than 30 min
    query+=" Sum(if (Timediff(fRunStop,fRunStart)<'00:30:00', Time_to_sec(Timediff(fRunStop,fRunStart)), 0)) as runtime, ";
    // sum of durations of all data runs with duration shorter than 30 min
    query+=" Sum(if (Timediff(fRunStop,fRunStart)<'00:30:00' and fRunTypeKey=1, Time_to_sec(Timediff(fRunStop,fRunStart)), 0)) as runtime2, ";
    //ratio all runs / runtime
    query+=" Sum(Time_to_sec(if (Timediff(fRunStop,fRunStart)<'00:30:00', Time_to_sec(Timediff(fRunStop,fRunStart)), 0)))/Time_to_sec(Timediff(Max(fRunStop),Min(fRunStart))) as ratio, ";
    //ratio all runs / astronomical twilight time
    query+=" Sum(Time_to_sec(if (Timediff(fRunStop,fRunStart)<'00:30:00', Time_to_sec(Timediff(fRunStop,fRunStart)), 0)))/Time_to_sec(Timediff(fStopObservation, fStartObservation)) as ratio2, ";
    //ratio data runs / runtime
    query+=" Sum(Time_to_sec(if (Timediff(fRunStop,fRunStart)<'00:30:00' and fRunTypeKey=1, Time_to_sec(Timediff(fRunStop,fRunStart)), 0)))/Time_to_sec(Timediff(Max(fRunStop),Min(fRunStart))) as ratio3, ";
    //ratio data runs / astronomical twilight time
    query+=" Sum(Time_to_sec(if (Timediff(fRunStop,fRunStart)<'00:30:00' and fRunTypeKey=1, Time_to_sec(Timediff(fRunStop,fRunStart)), 0)))/Time_to_sec(Timediff(fStopObservation, fStartObservation)) as ratio4, ";
    // time first to last run / range astronomical twilight
    query+=" Time_to_sec(Timediff(Max(fRunStop),Min(fRunStart)))/Time_to_sec(Timediff(fStopObservation, fStartObservation)) as ratio5 ";
    query+=" FROM ObservationTimes LEFT JOIN RunInfo USING(fNight) ";
    //query+=" WHERE not isnull(fRunStart) and not isnull(fRunStop) group by fNight having not isnull(obstime) order by fNight; ";
    query+=" group by fNight order by fNight; ";

    cout << "Q: " << query << endl;
    TSQLResult *res = serv.Query(query);
    if (!res)
        return kFALSE;

    Int_t numrows=res->GetRowCount();

    TH1F ratio("ratio","sum of time of data runs / time from start to stop of astronomical twilight", 21, 0.025, 1.025);
    TH1F ratio2("ratio2","sum of time of data runs  / time from beginning of first to end of last run", 21, 0.025, 1.025);
    TH1F ratio3("ratio3","time from beginning of first to end of last run / time from start to stop of astronomical twilight", 21, 0.025, 1.025);

    Float_t axis = MTime(nightmin/10000, (nightmin/100)%100, nightmin%100).GetAxisTime();
    Float_t days = numrows*24*3600;

    TH1F time1("time1","hours astronomical twilight", numrows, axis, axis+days);
    TH1F time2("time2","hours from first to last run", numrows, axis, axis+days);
    TH1F time3("time3","hours all runs", numrows, axis, axis+days);
    TH1F time4("time4","hours all data runs", numrows, axis, axis+days);

    TH1F eff2("eff2","hours from first to last run", numrows, axis, axis+days);
    TH1F eff3("eff3","hours all runs", numrows, axis, axis+days);
    TH1F eff4("eff4","hours all data runs", numrows, axis, axis+days);

    //time1.SetRangeUser(start, stop);
    //eff2.SetRangeUser(start, stop);


    Int_t counter=0;
    Int_t counter2=0;
    TSQLRow *row=0;
    while ((row=res->Next()))
    {
        counter++;
        //0: night
        //1: astronomical twilight range
        //2: time when data was taken
        //3: runtime all runs
        //4: runtime data runs
        //5: ratio runtime all runs / runtime
        //6: ratio runtime all runs / astronomical twilight range
        //7: ratio runtime data runs / runtime
        //8: ratio runtime data runs / astronomical twilight range
        if ((*row)[7])
            ratio2.Fill(atof((*row)[7]));
        if ((*row)[8])
            ratio.Fill(atof((*row)[8]));
        if ((*row)[9])
            ratio3.Fill(atof((*row)[9]));
        //if (atof((*row)[2])/60./60.>12)
        //{
        //    cout << (*row)[0] << " " << atof((*row)[2])/60./60. << endl;
        //    counter2++;
        //}
        //cout << "c: " << counter << " night: " << (*row)[0] << " " << atof((*row)[1])/60./60.<< " " << atof((*row)[2])/60./60.<< " " << atof((*row)[3])/60./60.<< " " << atof((*row)[4])/60./60. << endl;
        if (!(*row)[1])
            continue;

        float twilight = atof((*row)[1]);

        time1.SetBinContent(counter, twilight/60./60.);
        if ((*row)[2])
        {
            time2.SetBinContent(counter, atof((*row)[2])/60./60.);
            eff2.SetBinContent(counter, atof((*row)[2])/twilight);
        }
        if ((*row)[3])
        {
            time3.SetBinContent(counter, atof((*row)[3])/60./60.);
            eff3.SetBinContent(counter, atof((*row)[3])/twilight);
        }
        if ((*row)[4])
        {
            time4.SetBinContent(counter, atof((*row)[4])/60./60.);
            eff4.SetBinContent(counter, atof((*row)[4])/twilight);
        }
    }
    cout << "For " << counter2 << " nights the time from beginning of first to the end of the last run was > 12 hours." << endl;

    delete res;

    MStatusDisplay *d = new MStatusDisplay;

    TCanvas &c1 = d->AddTab("Ratio Hist", "Ratio Hist");
    c1.Divide(1,3);
    c1.cd(1);
    ratio.DrawCopy("hist");
    c1.cd(2);
    ratio2.DrawCopy("hist");
    c1.cd(3);
    ratio3.DrawCopy("hist");

    TCanvas &c2 = d->AddTab("Times");
    time1.GetXaxis()->SetTimeDisplay(true);
    time1.GetXaxis()->SetTimeFormat("%m/%y %F1995-01-01 00:00:00 GMT");
    time1.GetXaxis()->SetLabelSize(0.045);
    time1.GetYaxis()->SetLabelSize(0.045);
    time1.GetXaxis()->CenterTitle();
    time1.GetYaxis()->CenterTitle();
    time1.SetXTitle("Date");
    time1.SetYTitle("Observation time / hours");
    time1.SetFillColor(kBlue);
    time1.SetTitle("Observation Times");
    time1.SetMinimum(0);
    time1.SetStats(kFALSE);
    time1.DrawCopy("hist");
    time2.SetFillColor(kGreen);
    time2.DrawCopy("histsame");
    time3.SetFillColor(kYellow);
    time3.DrawCopy("histsame");
    time4.SetFillColor(kRed+2);
    time4.DrawCopy("histsame");

    TCanvas &c3 = d->AddTab("Times2");
    time1.SetFillColor(kBlue);
    time1.SetTitle("Observation Times");
    time1.SetStats(kFALSE);
    time1.DrawCopy("hist");
    time3.SetFillColor(kYellow);
    time3.DrawCopy("histsame");
    time4.SetFillColor(kRed+2);
    time4.DrawCopy("histsame");

    TCanvas &c4 = d->AddTab("Eff");
    c4.SetGridy();
    eff2.GetXaxis()->SetTimeDisplay(true);
    eff2.GetXaxis()->SetTimeFormat("%m/%y %F1995-01-01 00:00:00 GMT");
    eff2.GetXaxis()->SetLabelSize(0.045);
    eff2.GetYaxis()->SetLabelSize(0.045);
    eff2.GetXaxis()->CenterTitle();
    eff2.GetYaxis()->CenterTitle();
    eff2.SetXTitle("Date");
    eff2.SetYTitle("Efficiency");
    eff2.SetTitle("Observation efficiency");
    eff2.SetMinimum(0);
    eff2.SetMaximum(1);
    eff2.SetStats(kFALSE);
    eff2.SetLineColor(kGreen);
    eff2.SetFillColor(kGreen);
    eff2.DrawCopy("hist");
    eff3.SetFillColor(kYellow);
    eff3.SetLineColor(kYellow);
    eff3.DrawCopy("histsame");
    eff4.SetFillColor(kRed+2);
    eff4.SetLineColor(kRed+2);
    eff4.DrawCopy("histsame");

    TCanvas &c5 = d->AddTab("Eff2", "Eff2");
    c5.SetGridy();
    eff3.GetXaxis()->SetTimeDisplay(true);
    eff3.GetXaxis()->SetTimeFormat("%m/%y %F1995-01-01 00:00:00 GMT");
    eff3.GetXaxis()->SetLabelSize(0.045);
    eff3.GetYaxis()->SetLabelSize(0.045);
    eff3.GetXaxis()->CenterTitle();
    eff3.GetYaxis()->CenterTitle();
    eff3.SetXTitle("Date");
    eff3.SetYTitle("Efficiency");
    eff3.SetTitle("Observation efficiency");
    eff3.SetMinimum(0);
    eff3.SetMaximum(1);
    eff3.SetStats(kFALSE);
    eff3.DrawCopy("hist");
    eff4.DrawCopy("histsame");

    d->SaveAs("DataEfficiency.root");

    return 0;
}
