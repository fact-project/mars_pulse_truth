#include <iostream>

#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TText.h>
#include <TFile.h>
#include <TSystem.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TPRegexp.h>

#include "MSQLMagic.h"
#include "MAlphaFitter.h"
#include "MHThetaSq.h"

#include "MObservatory.h"
#include "MAstro.h"
#include "MAstroSky2Local.h"
#include "MAstroCatalog.h"
#include "MVector3.h"
#include "MMath.h"
#include "MDirIter.h"
#include "MStatusArray.h"
#include "MStatusDisplay.h"
#include "MHCamera.h"
#include "MSequence.h"

using namespace std;


int lightcurve(Int_t sourcekey=1, Int_t nightmin=0, Int_t nightmax=0, Int_t minutes=20, TString table="AnalysisResultsRunLP", TString outfile="lc", Bool_t dch=kTRUE)
{

    cout << minutes << " min bins..." << endl;
    minutes*=60;
    cout << minutes << " sec bins..." << endl;

    MSQLServer serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    TString query;
    query=Form("SELECT Min(fNight), Max(fNight) FROM %s ", table.Data());
    query+=" LEFT JOIN RunInfo USING(fNight, fRunID) ";
    query+=Form(" WHERE fSourceKey=%d", sourcekey);
    if (nightmin)
        query+=Form(" AND fNight >=%d",nightmin);
    if (nightmax)
        query+=Form(" AND fNight <=%d",nightmax);

    // datacheck
    TString datacheck=" ";
    //remove data with wrong settings
    datacheck+=" AND fNight>20120420 AND NOT fNight IN (20120406,20120410,20120503) AND";//data with different bias voltage
    datacheck+=" NOT fNight BETWEEN 20121206 AND 20130110"; // broken bias channel
    //datacheck+=" AND NOT (fNight=20120608 AND fRunID=65) "; // something went wrong with tracking?
    // 24.6. new coefficients
    TString zdparam=" pow(0.753833 * cos(Radians(fZenithDistanceMean)), 7.647435) * exp(-5.753686*pow(Radians(fZenithDistanceMean),2.089609))";
    TString thparam=" pow((if(isnull(fThresholdMinSet),fThresholdMedian,fThresholdMinSet)-329.4203),2) * (-0.0000002044803) ";
    TString param=" (fNumEvtsAfterBgCuts/5-fNumSigEvts)/fOnTimeAfterCuts - "+zdparam+" - "+thparam+" ";
    datacheck+=" AND -0.085 < ("+param+") ";
    datacheck+=" AND 0.25 > ("+param+") ";


    if (dch)
        query+=datacheck;

    //cout << "Q: " << query << endl;

    TSQLResult *res = serv.Query(query);
    if (!res)
        return 1;

    TSQLRow *row=res->Next();
    TString nightstart=(*row)[0];
    TString nightstop=(*row)[1];
    delete res;

    query =" SELECT fNight FROM "+table;
    query+=" LEFT JOIN RunInfo USING(fNight, fRunID) ";
    query+=Form(" WHERE fSourceKey=%d", sourcekey);
    query+=Form(" AND fNight BETWEEN %s AND %s ", nightstart.Data(), nightstop.Data());
    query+=" GROUP BY fNight ";
    query+=" ORDER BY fNight ";

    cout << "Q: " << query << endl;


    TSQLResult *res3 = serv.Query(query);
    if (!res3)
        return 1;

    //counter
    Int_t counter=0;//counter for graphs
    Int_t counter2=0;//counter for loop

    //graphs and histograms
    TGraphErrors excessrate;
    TGraphErrors backgroundrate;
    TGraphErrors signif;
    TGraphErrors significancerate;
    TGraphErrors excratevsbgrate;
    TGraphErrors excvsbg;
    TGraphErrors excvszd;
    TGraphErrors bgvszd;
    TGraphErrors excvscur;
    TGraphErrors bgvscur;
    TGraphErrors excvsth;
    TGraphErrors bgvsth;
    TGraphErrors zdvstime;
    TGraphErrors curvstime;
    TGraphErrors thvstime;
    TH1F exc("exc","rates", 43, -12.5, 202.5);
    TH1F bg("bg","rates", 43, -12.5, 202.5);

    //variables for calculations and graphs
    Float_t significance;
    TString excevts;
    TString bgevts;
    TString sigevts;
    TString ontime;
    TString zd2;
    TString zd3;
    TString cur;
    TString th;
    Float_t excevtssum=0;
    Float_t bgevtssum=0;
    Float_t sigevtssum=0;
    Float_t ontimesum=0;
    Float_t excrate=0;
    Float_t bgrate=0;
    Float_t signifrate=0;
    Float_t excerr=0;
    Float_t bgerr=0;
    Float_t zdmean=0;
    Float_t curmean=0;
    Float_t thmean=0;
    Float_t zddiff=0;
    Float_t curdiff=0;
    Float_t thdiff=0;
    Float_t zdmin=0;
    Float_t zdmax=0;
    Float_t curmin=0;
    Float_t curmax=0;
    Float_t thmin=0;
    Float_t thmax=0;
    MTime start;
    MTime stop;
    Double_t mjdstart=0;
    Double_t mjdstop=0;
    Double_t mjd=0;
    Double_t mjddiff=0;

    TSQLRow *row3=0;
    while ((row3=res3->Next()))
    {
        cout << "NIGHT " << (*row3)[0] << endl;
        //query =" SELECT IF(ISNULL(fOnTimeAfterCuts), IF(ISNULL(fOnTime), Time_to_sec(Timediff(fRunStop,fRunStart)), fOnTime), fOnTimeAfterCuts), ";
        query =" SELECT fOnTimeAfterCuts, fNumBgEvts, fNumSigEvts, fNumExcEvts, ";
        query+=" fRunStart, fRunStop, ";
        query+=" fZenithDistanceMax, fZenithDistanceMin, ";
        query+=" fCurrentsMedMeanBeg, fThresholdMinSet FROM "+table;
        query+=" LEFT JOIN RunInfo USING(fNight, fRunID) ";
        query+=Form(" WHERE fSourceKey=%d", sourcekey);
        query+=Form(" AND fNight= %s ", (*row3)[0]);
        query+=" AND fOnTimeAfterCuts < 1000 "; //exclude runs with wrong/too high ontime
        query+=" AND NOT ISNULL(fNumExcEvts) ";// only where excess was extracted

        if (dch)
            query+=datacheck;

        query+=" ORDER BY fRunID ";

        cout << "Q: " << query << endl;

        TSQLResult *res2 = serv.Query(query);
        if (!res2)
            return 1;

        counter2=0;
        excevtssum=0;
        bgevtssum=0;
        sigevtssum=0;
        ontimesum=0;
        TSQLRow *row2=0;
        while ((row2=res2->Next()))
        {
            counter2++;
            ontime=(*row2)[0];
            bgevts=(*row2)[1];
            sigevts=(*row2)[2];
            excevts=(*row2)[3];
            zd2=(*row2)[6];//zdmin
            zd3=(*row2)[7];//zdmax
            cur=(*row2)[8];//currents
            th=(*row2)[9];//threshold
            if (counter2==1)
            {
                start.SetSqlDateTime((*row2)[4]);
                zdmin=zd3.Atof();
                zdmax=zd2.Atof();
                curmin=cur.Atof();
                curmax=cur.Atof();
                thmin=th.Atof();
                thmax=th.Atof();
            }

            if (ontimesum+ontime.Atof()>minutes && minutes>0)
            {
                //calculate values
                significance = MMath::SignificanceLiMaSigned(sigevtssum, bgevtssum*5, 0.2);
                signifrate = significance/sqrt(ontimesum/3600);
                bgrate = bgevtssum/ontimesum*3600;
                excrate = excevtssum/ontimesum*3600;
                excerr = MMath::ErrorExc(sigevtssum, bgevtssum*5, 0.2)/ontimesum*3600.;
                bgerr = sqrt(bgevtssum)/ontimesum*3600;
                zdmean = zdmin+(zdmax-zdmin)/2;
                zddiff = (zdmax-zdmin)/2;
                curmean = curmin+(curmax-curmin)/2;
                curdiff = (curmax-curmin)/2;
                thmean = thmin+(thmax-thmin)/2;
                thdiff = (thmax-thmin)/2;
                mjdstart = start.GetMjd();
                mjdstop = stop.GetMjd();
                mjd = mjdstart+(mjdstop-mjdstart)/2.;
                mjddiff = ontimesum/3600./24/2;

                //fill histograms and graphs
                exc.Fill(excrate);
                bg.Fill(bgrate);
                excessrate.SetPoint(counter, mjd, excrate);
                excessrate.SetPointError(counter, mjddiff, excerr);
                backgroundrate.SetPoint(counter, mjd, bgrate);
                backgroundrate.SetPointError(counter, mjddiff, bgerr);
                excvsbg.SetPoint(counter, bgrate, excrate);
                excvsbg.SetPointError(counter, bgerr, excerr);
                excvszd.SetPoint(counter, zdmean, excrate);
                excvszd.SetPointError(counter, zddiff, excerr);
                bgvszd.SetPoint(counter, zdmean, bgrate);
                bgvszd.SetPointError(counter, zddiff, bgerr);
                excvscur.SetPoint(counter, curmean, excrate);
                excvscur.SetPointError(counter, curdiff, excerr);
                bgvscur.SetPoint(counter, curmean, bgrate);
                bgvscur.SetPointError(counter, curdiff, bgerr);
                excvsth.SetPoint(counter, thmean, excrate);
                excvsth.SetPointError(counter, thdiff, excerr);
                bgvsth.SetPoint(counter, thmean, bgrate);
                bgvsth.SetPointError(counter, thdiff, bgerr);
                signif.SetPoint(counter, mjd, significance);
                signif.SetPointError(counter, mjddiff, 0);
                significancerate.SetPoint(counter, mjd, signifrate);
                significancerate.SetPointError(counter, mjddiff, 0);
                zdvstime.SetPoint(counter, mjd, zdmean);
                zdvstime.SetPointError(counter, mjddiff, zddiff);
                curvstime.SetPoint(counter, mjd, curmean);
                curvstime.SetPointError(counter, mjddiff, curdiff);
                thvstime.SetPoint(counter, mjd, thmean);
                thvstime.SetPointError(counter, mjddiff, thdiff);

                //set couter for graphs
                counter++;
                //reset counter for calculation
                counter2=0;
                //reset variables
                start.SetSqlDateTime((*row2)[4]);
                excevtssum=0;
                bgevtssum=0;
                sigevtssum=0;
                ontimesum=0;
            }
            //set stop time
            stop.SetSqlDateTime((*row2)[5]);
            //add up variables
            ontimesum+=ontime.Atof();
            excevtssum+=excevts.Atof();
            bgevtssum+=bgevts.Atof();
            sigevtssum+=sigevts.Atof();

            //update zdmin
            if (zdmin>zd3.Atof())
                zdmin=zd3.Atof();
            //update zdmax
            if (zdmax<zd2.Atof())
                zdmax=zd2.Atof();

            //update curmin
            if (curmin>cur.Atof())
                curmin=cur.Atof();
            //update curmax
            if (curmax<cur.Atof())
                curmax=cur.Atof();

            //update thmin
            if (thmin>th.Atof())
                thmin=th.Atof();
            //update thmax
            if (thmax<th.Atof())
                thmax=th.Atof();

        }
        //cout << "reached last run" << endl;
        // if ontime is larger than 90% of the timebin width, the last point is filled
        if (ontimesum>(minutes-0.1*minutes) || minutes<0)
        {
            if (minutes<0 && ontimesum>20*60 && bgevtssum!=0)
            {
                //calculate values
                significance = MMath::SignificanceLiMaSigned(sigevtssum, bgevtssum*5, 0.2);
                signifrate = significance/sqrt(ontimesum/3600);
                bgrate = bgevtssum/ontimesum*3600;
                excrate = excevtssum/ontimesum*3600;
                excerr = MMath::ErrorExc(sigevtssum, bgevtssum*5, 0.2)/ontimesum*3600.;
                bgerr = sqrt(bgevtssum)/ontimesum*3600;
                zdmean = zdmin+(zdmax-zdmin)/2;
                zddiff = (zdmax-zdmin)/2;
                curmean = curmin+(curmax-curmin)/2;
                curdiff = (curmax-curmin)/2;
                thmean = thmin+(thmax-thmin)/2;
                thdiff = (thmax-thmin)/2;
                mjdstart = start.GetMjd();
                mjdstop = stop.GetMjd();
                mjd = mjdstart+(mjdstop-mjdstart)/2.;
                mjddiff = ontimesum/3600./24/2;

                //fill histograms and graphs
                exc.Fill(excrate);
                bg.Fill(bgrate);
                excessrate.SetPoint(counter, mjd, excrate);
                excessrate.SetPointError(counter, mjddiff, excerr);
                backgroundrate.SetPoint(counter, mjd, bgrate);
                backgroundrate.SetPointError(counter, mjddiff, bgerr);
                excvsbg.SetPoint(counter, bgrate, excrate);
                excvsbg.SetPointError(counter, bgerr, excerr);
                excvszd.SetPoint(counter, zdmean, excrate);
                excvszd.SetPointError(counter, zddiff, excerr);
                bgvszd.SetPoint(counter, zdmean, bgrate);
                bgvszd.SetPointError(counter, zddiff, bgerr);
                excvscur.SetPoint(counter, curmean, excrate);
                excvscur.SetPointError(counter, curdiff, excerr);
                bgvscur.SetPoint(counter, curmean, bgrate);
                bgvscur.SetPointError(counter, curdiff, bgerr);
                excvsth.SetPoint(counter, thmean, excrate);
                excvsth.SetPointError(counter, thdiff, excerr);
                bgvsth.SetPoint(counter, thmean, bgrate);
                bgvsth.SetPointError(counter, thdiff, bgerr);
                signif.SetPoint(counter, mjd, significance);
                signif.SetPointError(counter, mjddiff, 0);
                significancerate.SetPoint(counter, mjd, signifrate);
                significancerate.SetPointError(counter, mjddiff, 0);
                zdvstime.SetPoint(counter, mjd, zdmean);
                zdvstime.SetPointError(counter, mjddiff, zddiff);
                curvstime.SetPoint(counter, mjd, curmean);
                curvstime.SetPointError(counter, mjddiff, curdiff);
                thvstime.SetPoint(counter, mjd, thmean);
                thvstime.SetPointError(counter, mjddiff, thdiff);

                //set counter for graphs
                counter++;

                //reset variables
                excevtssum=0;
                bgevtssum=0;
                sigevtssum=0;
                ontimesum=0;
            }

            //reset for calculations
            counter2=0;
        }
        delete res2;
    }
    delete res3;

    if (excessrate.GetN()<1)
        return 1;

    MStatusDisplay *d = new MStatusDisplay;

//    if (dch)
//        backgroundrate.SetTitle(Form("Rates vs MJD (dch: %s)", limit.Data()));
//    else


    Double_t x;
    Double_t y;
    Double_t min;
    Double_t max;

    TText txt;
    txt.SetTextColor(kGray+2);
    txt.SetTextSize(0.07);
    TString text="FACT Preliminary";



    gROOT->SetSelectedPad(0);
    TCanvas &c1 = d->AddTab("Rates vs MJD", "Rates vs MJD");
    gPad->SetGridy();

    excessrate.GetPoint(1,x,y);
    max = excessrate.GetHistogram()->GetMaximum();
    min = excessrate.GetHistogram()->GetMinimum();

    backgroundrate.SetTitle("Rates vs MJD");
    backgroundrate.SetMarkerStyle(8);
    backgroundrate.SetMarkerSize(0.8);
    backgroundrate.SetLineWidth(2);
    backgroundrate.SetMarkerColor(kBlue);
    backgroundrate.SetLineColor(kBlue);
    backgroundrate.GetYaxis()->SetTitle("evts/h");
    backgroundrate.GetXaxis()->SetTitle("MJD");
    backgroundrate.GetXaxis()->CenterTitle();
    backgroundrate.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    backgroundrate.DrawClone("AP");

    excessrate.SetMarkerStyle(8);
    excessrate.SetMarkerSize(0.8);
    excessrate.SetLineWidth(2);
    excessrate.DrawClone("Psame");

    txt.DrawText(x, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c2 = d->AddTab("Excess vs MJD", "Excess vs MJD");
    gPad->SetGridy();
    excessrate.SetTitle("Excess Rate vs MJD");
    excessrate.SetMarkerStyle(7);
    excessrate.SetMarkerSize(2);
    excessrate.GetYaxis()->SetTitle("excess evts/h");
    excessrate.GetXaxis()->SetTitle("MJD");
    excessrate.GetXaxis()->CenterTitle();
    excessrate.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    excessrate.DrawClone("AP");
    txt.DrawText(x, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c4 = d->AddTab("Exc vs Bg", "Exc vs Bg");

    backgroundrate.GetPoint(1,x,y);
    max = excvsbg.GetHistogram()->GetMaximum();
    min = excvsbg.GetHistogram()->GetMinimum();

    excvsbg.SetTitle("Excess Rate vs Background Rate");
    excvsbg.SetMarkerStyle(7);
    excvsbg.SetMarkerSize(2);
    excvsbg.GetYaxis()->SetTitle("excess evts/h");
    excvsbg.GetXaxis()->SetTitle("background evts/h");
    excvsbg.GetXaxis()->CenterTitle();
    excvsbg.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    excvsbg.DrawClone("AP");
    txt.DrawText(excvsbg.GetMean(1), max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c3 = d->AddTab("Exc vs Zd", "Exc vs Zd");

    excvszd.GetPoint(1,x,y);
    max = excvszd.GetHistogram()->GetMaximum();
    min = excvszd.GetHistogram()->GetMinimum();

    excvszd.SetTitle("Excess Rate vs Zd");
    excvszd.SetMarkerStyle(7);
    excvszd.SetMarkerSize(2);
    excvszd.GetYaxis()->SetTitle("excess evts/h");
    excvszd.GetXaxis()->SetTitle("deg");
    excvszd.GetXaxis()->CenterTitle();
    excvszd.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    excvszd.DrawClone("AP");
    txt.DrawText(x, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c5 = d->AddTab("Bg vs Zd", "Bg vs Zd");

    zdvstime.GetPoint(1,x,y);
    max = bgvszd.GetHistogram()->GetMaximum();
    min = bgvszd.GetHistogram()->GetMinimum();

    bgvszd.SetTitle("Background Rate vs Zd");
    bgvszd.SetMarkerStyle(7);
    bgvszd.SetMarkerSize(2);
    bgvszd.GetYaxis()->SetTitle("background evts/h");
    bgvszd.GetXaxis()->SetTitle("deg");
    bgvszd.GetXaxis()->CenterTitle();
    bgvszd.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    bgvszd.DrawClone("AP");
    txt.DrawText(y, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c6 = d->AddTab("Signifrate vs MJD", "Signifrate vs MJD");
    gPad->SetGridy();

    significancerate.GetPoint(1,x,y);
    max = significancerate.GetHistogram()->GetMaximum();
    min = significancerate.GetHistogram()->GetMinimum();

    significancerate.SetTitle("Significance Rate vs MJD");
    significancerate.SetMarkerStyle(7);
    significancerate.SetMarkerSize(2);
    significancerate.GetYaxis()->SetTitle("sigma/sqrt(h)");
    significancerate.GetXaxis()->SetTitle("MJD");
    significancerate.GetXaxis()->CenterTitle();
    significancerate.GetYaxis()->SetRangeUser(0,max*1.1);
    significancerate.DrawClone("AP");
    txt.DrawText(x, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c7 = d->AddTab("Signif vs MJD", "Signif vs MJD");

    signif.GetPoint(1,x,y);
    max = signif.GetHistogram()->GetMaximum();
    min = signif.GetHistogram()->GetMinimum();

    signif.SetTitle("Significance vs MJD");
    signif.SetMarkerStyle(7);
    signif.SetMarkerSize(2);
    signif.GetYaxis()->SetTitle("sigma");
    signif.GetXaxis()->SetTitle("MJD");
    signif.GetXaxis()->CenterTitle();
    signif.GetYaxis()->SetRangeUser(0,max*1.1);
    signif.DrawClone("AP");
    txt.DrawText(x, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c8 = d->AddTab("Rate Hist", "Rate Hist");

    max = bg.GetMaximum()*1.1;
    min = bg.GetMinimum();

    bg.SetLineColor(kBlue);
    bg.GetYaxis()->SetRangeUser(0,max*1.1);
    bg.GetXaxis()->SetTitle("evts/h");
    bg.DrawCopy("hist");
    exc.DrawCopy("histsame");
    txt.DrawText(min, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c9 = d->AddTab("Zd vs Time", "Zd vs Time");
    gPad->SetGridy();

    zdvstime.GetPoint(1,x,y);
    max = zdvstime.GetHistogram()->GetMaximum();
    min = zdvstime.GetHistogram()->GetMinimum();

    zdvstime.SetTitle("Zd vs MJD");
    zdvstime.SetMarkerStyle(7);
    zdvstime.SetMarkerSize(2);
    zdvstime.GetYaxis()->SetTitle("deg");
    zdvstime.GetXaxis()->SetTitle("MJD");
    zdvstime.GetXaxis()->CenterTitle();
    zdvstime.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    zdvstime.DrawClone("AP");
    txt.DrawText(x, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c10 = d->AddTab("Cur vs Time", "Cur vs Time");
    gPad->SetGridy();

    curvstime.GetPoint(1,x,y);
    max = curvstime.GetHistogram()->GetMaximum();
    min = curvstime.GetHistogram()->GetMinimum();

    curvstime.SetTitle("Currents vs MJD");
    curvstime.SetMarkerStyle(7);
    curvstime.SetMarkerSize(2);
    curvstime.GetYaxis()->SetTitle("#muA");
    curvstime.GetXaxis()->SetTitle("MJD");
    curvstime.GetXaxis()->CenterTitle();
    curvstime.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    curvstime.DrawClone("AP");
    txt.DrawText(x, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c11 = d->AddTab("Th vs Time", "Th vs Time");
    gPad->SetGridy();

    thvstime.GetPoint(1,x,y);
    max = thvstime.GetHistogram()->GetMaximum();
    min = thvstime.GetHistogram()->GetMinimum();

    thvstime.SetTitle("Threshold vs MJD");
    thvstime.SetMarkerStyle(7);
    thvstime.SetMarkerSize(2);
    thvstime.GetYaxis()->SetTitle("DAC counts");
    thvstime.GetXaxis()->SetTitle("MJD");
    thvstime.GetXaxis()->CenterTitle();
    thvstime.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    thvstime.DrawClone("AP");
    txt.DrawText(x, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c12 = d->AddTab("Exc vs Cur", "Exc vs Cur");

    excvscur.GetPoint(1,x,y);
    max = excvscur.GetHistogram()->GetMaximum();
    min = excvscur.GetHistogram()->GetMinimum();

    excvscur.SetTitle("Excess Rate vs Currents");
    excvscur.SetMarkerStyle(7);
    excvscur.SetMarkerSize(2);
    excvscur.GetYaxis()->SetTitle("excess evts/h");
    excvscur.GetXaxis()->SetTitle("#muA");
    excvscur.GetXaxis()->CenterTitle();
    excvscur.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    excvscur.DrawClone("AP");
    txt.DrawText(x, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c13 = d->AddTab("Bg vs Cur", "Bg vs Cur");

    curvstime.GetPoint(1,x,y);
    max = bgvscur.GetHistogram()->GetMaximum();
    min = bgvscur.GetHistogram()->GetMinimum();

    bgvscur.SetTitle("Background Rate vs Current");
    bgvscur.SetMarkerStyle(7);
    bgvscur.SetMarkerSize(2);
    bgvscur.GetYaxis()->SetTitle("background evts/h");
    bgvscur.GetXaxis()->SetTitle("#muA");
    bgvscur.GetXaxis()->CenterTitle();
    bgvscur.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    bgvscur.DrawClone("AP");
    txt.DrawText(y, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c14 = d->AddTab("Exc vs Th", "Exc vs Th");

    excvsth.GetPoint(1,x,y);
    max = excvsth.GetHistogram()->GetMaximum();
    min = excvsth.GetHistogram()->GetMinimum();

    excvsth.SetTitle("Excess Rate vs Threshold");
    excvsth.SetMarkerStyle(7);
    excvsth.SetMarkerSize(2);
    excvsth.GetYaxis()->SetTitle("excess evts/h");
    excvsth.GetXaxis()->SetTitle("DAC counts");
    excvsth.GetXaxis()->CenterTitle();
    excvsth.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    excvsth.DrawClone("AP");
    txt.DrawText(x, max, text);



    gROOT->SetSelectedPad(0);
    TCanvas &c15 = d->AddTab("Bg vs Th", "Bg vs Th");

    thvstime.GetPoint(1,x,y);
    max = bgvsth.GetHistogram()->GetMaximum();
    min = bgvsth.GetHistogram()->GetMinimum();

    bgvsth.SetTitle("Background Rate vs Threshold");
    bgvsth.SetMarkerStyle(7);
    bgvsth.SetMarkerSize(2);
    bgvsth.GetYaxis()->SetTitle("background evts/h");
    bgvsth.GetXaxis()->SetTitle("DAC counts");
    bgvsth.GetXaxis()->CenterTitle();
    bgvsth.GetYaxis()->SetRangeUser(min-min*0.1,max*1.1);
    bgvsth.DrawClone("AP");
    txt.DrawText(y, max, text);




    d->SaveAs(outfile);

    return 0;

}



