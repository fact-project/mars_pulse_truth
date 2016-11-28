/* ======================================================================== *\
! $Name: not supported by cvs2svn $:$Id: plotstat.C,v 1.11 2009-01-30 14:58:40 tbretz Exp $
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
!   Author(s): Thomas Bretz, 02/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniela Dorner, 02/2006 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// plotstat.C
// ==========
//
// This macro is used to plot processing statistics from the db.
//
// Normally all period are processed. If you want to restric processing to
// less periods, use:
//   > root plotstat.C+(20, -1)
// means that all periods since 20 are taken into account. The two numbers
// denote the first and last period taken into account, -1 means
// open start or open end.
// 
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>

#include <TH2.h>
#include <TPad.h>
#include <TLine.h>
#include <TText.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TEllipse.h>
#include <TSQLRow.h>
#include <TSQLResult.h>

#include "MH.h"
#include "MTime.h"
#include "MBinning.h"
#include "MSQLServer.h"
#include "MStatusDisplay.h"

using namespace std;

TString GetFullQuery(TString query, TString from="", TString to="")
{
    if (from.IsNull())
        return query;

    if (!query.Contains("where", TString::kIgnoreCase))
        query += " where ";
    else
        query += " and ";

    query += " fRunStart>'";
    query += from;
    query += "' and fRunStart<'";
    query += to;
    query += "'";

    return query;
}

Double_t GetTime(MSQLServer &serv, TString query, TString from="", TString to="")
{
    query = GetFullQuery(query, from, to);

    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query << endl;
        return -1;
    }

    TSQLRow *row=res->Next();
    if (!row)
    {
        cout << "ERROR - Query " << query << " empty." << endl;
        delete res;
        return -1;
    }

    const char *time = (*row)[0];

    const Double_t rc = time ? atof(time) : 0;

    delete res;
    return rc<0 || rc>200 ? 0 : rc;
}

TArrayD GetObsDist(MSQLServer &serv, TString from="", TString to="")
{
    // 8: Sequenced RunTime per source and night
    //query[8]  = "select SUM(TIME_TO_SEC(TIMEDIFF(fRunStop,fRunStart)))/3600, ";
    //query[8] += "DATE_FORMAT(ADDDATE(fRunStart,Interval 12 hour), '%Y-%m-%d') as Start,";
    //query[8] += "from RunData where fRunTypeKEY=2 and fExcludedFDAKEY=1 group by Start, fSourceKEY";

    TString query;

    query  = "SELECT SUM(fRunTime)/3600, ";
    query += "DATE_FORMAT(ADDDATE(fRunStart, INTERVAL 12 hour), '%Y-%m-%d') AS Start ";
    query += "FROM Sequences ";

    query  = GetFullQuery(query, from, to);
    query += " GROUP BY Start, fSourceKEY";


    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query << endl;
        return -1;
    }

    TSQLRow *row = 0;

    TArrayD arr;

    while ((row=res->Next()))
    {
        const char *time = (*row)[0];

        const Double_t rc = time ? atof(time) : 0;

        if (rc>0 && rc<200)
        {
            arr.Set(arr.GetSize()+1);
            arr[arr.GetSize()-1] = rc;
        }
    }

    delete res;

    return arr;
}

void DrawYears()
{
    UInt_t year   = 2004;
    Int_t  period = 0;

    MTime past, from, now(-1);
    past.Set(2004, 1, 1, 13, 1);

    TLine l;
    l.SetLineStyle(kDotted);
    l.SetLineColor(kWhite);
    while (past<now)
    {
        if (period!=past.GetMagicPeriod())
        {
            period = past.GetMagicPeriod();
            from = past;
        }

        if (past.Year()!=year)
        {
            Double_t dx = (past.GetMjd()-from.GetMjd())/28;
            l.DrawLine(past.GetMagicPeriod()+dx, 0,
                       past.GetMagicPeriod()+dx, gPad->GetUymax());
            year = past.Year();
        }
        past.SetMjd(past.GetMjd()+1);
    }
}

void DrawCake(TH1F *h, Int_t p0=-1, Int_t p1=9999)
{
    gPad->Range(-1, -0.84, 1, 1.16);

    gPad->SetFillColor(kWhite);
    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);

    const Double_t r1 = 0.8;
    const Double_t r2 = 0.59;

    const Double_t tot0 = h[0].Integral(p0, p1);
    const Double_t tot1 = h[1].Integral(p0, p1);
    const Double_t tot2 = h[2].Integral(p0, p1);
    const Double_t tot3 = h[3].Integral(p0, p1);
    const Double_t tot4 = h[4].Integral(p0, p1);
    const Double_t tot5 = h[5].Integral(p0, p1);
    const Double_t tot6 = h[6].Integral(p0, p1);
    const Double_t tot7 = h[7].Integral(p0, p1);


    TPaveText *pave = new TPaveText(-0.99, 0.87, 0.99, 1.15);
    pave->SetBorderSize(1);

    TString title = Form("Total = %.1fh (excl=%.1fh)", tot1, tot1-tot2);
    if (p0>0 && p1<9000 && p0==p1)
        title.Prepend(Form("P%d: ", TMath::Nint(h[0].GetBinCenter(p0))));

    pave->AddText(title)->SetTextAlign(22);

    if (p0>0 && p1<9000)
    {
        MTime t0, t1;
        t0.SetMagicPeriodStart(p0);
        t1.SetMagicPeriodStart(p1+1);

        pave->AddText(Form("%s 12h  -  %s 12h", t0.GetStringFmt("%Y/%m/%d").Data(), t1.GetStringFmt("%Y/%m/%d").Data()));
    }
    pave->SetBit(kCanDelete);
    pave->Draw();

    if (tot1<0.1)
        return;

    TEllipse e;
    e.SetFillColor(15);
    //e.SetLineColor(15);
    e.DrawEllipse(0, 0, r1*1.1, r2*1.1, 90, tot0/tot1*360+90, 0);

    //    0    : hollow
    //    1001 : Solid
    //    2001 : hatch style
    //    3000+pattern_number (see below)
    //    4000 :the window is transparent.
    //    4000 to 4100 the window is 100% transparent to 100% opaque

    e.SetLineColor(10);//17);
    e.SetFillColor(10);//17);
    e.DrawEllipse(0, 0, r1, r2, 0, 360, 0);
    e.SetLineColor(kBlack);

    e.SetFillColor(kBlack);
    e.DrawEllipse(0, 0, r1, r2, 90, tot2/tot1*360+90, 0);
    //e.SetLineColor(kBlue);
    e.SetFillColor(kBlue);
    e.DrawEllipse(0, 0, r1, r2, 90, tot3/tot1*360+90, 0);
    //e.SetLineColor(kRed);
    e.SetFillColor(kRed);
    e.DrawEllipse(0, 0, r1, r2, 90, tot4/tot1*360+90, 0);
    //e.SetLineColor(kGreen);
    e.SetFillColor(kGreen);
    e.DrawEllipse(0, 0, r1, r2, 90, tot5/tot1*360+90, 0);
    //e.SetLineColor(kRed+100);
    e.SetFillColor(kRed+100);
    e.DrawEllipse(0, 0, r1, r2, 90, tot6/tot1*360+90, 0);
    //e.SetLineColor(kGreen+100);
    e.SetFillColor(kGreen+100);
    e.DrawEllipse(0, 0, r1, r2, 90, tot7/tot1*360+90, 0);

    TText txt;
    txt.SetTextSize(0.08);

    txt.SetTextAlign(32);

    txt.SetTextColor(kBlack);
    txt.DrawText(0.99, 0.65,    Form("%.1f%%", (tot2-tot3)/tot1*100));
    txt.SetTextColor(kBlue);
    txt.DrawText(0.99, -0.58,   Form("%.1f%%", (tot3-tot4)/tot1*100));
    txt.SetTextColor(kRed+100);
    txt.DrawText(-0.35, -0.70,  Form("%.1f%%", (tot6-tot7)/tot1*100));

    txt.SetTextAlign(12);

    txt.SetTextColor(kBlack);
    txt.DrawText(0, 0.77,       Form("%.1f%%", (tot1-tot2)/tot1*100));
    txt.SetTextColor(15);
    txt.DrawText(-0.99, 0.65,   Form("%.1f%%", tot0/tot1*100));
    txt.SetTextColor(kGreen+100);
    txt.DrawText(-0.99, -0.58,  Form("%.1f%%", tot7/tot1*100));
    txt.SetTextColor(kRed);
    txt.DrawText(0.35, -0.70,   Form("%.1f%%", (tot4-tot5)/tot1*100));

    txt.SetTextAlign(22);

    txt.SetTextColor(kGreen);
    txt.DrawText(0, -0.79,      Form("%.1f%%", (tot5-tot6)/tot1*100));
}

TObject *DrawLegend(TH1F *h)
{
    TLegend leg(0.01, 0.12, 0.99, 0.98, "Data Statistics");
    leg.SetBorderSize(1);
    leg.SetTextSize(0.1);

    TH1 *hc[8];

    for (int i=0; i<8; i++)
    {
        hc[i] = (TH1*)h[i].Clone();
        hc[i]->SetBit(kCanDelete);
        hc[i]->SetDirectory(0);
    }

    leg.AddEntry(hc[0], "Files available",     "L");
    leg.AddEntry(hc[1], "Excluded data",       "F");
    leg.AddEntry(hc[2], "Not sequenced",       "F");
    leg.AddEntry(hc[3], "Callisto not done",   "F");
    leg.AddEntry(hc[4], "Callisto failed",     "F");
    leg.AddEntry(hc[5], "Star not done",       "F");
    leg.AddEntry(hc[6], "Star failed",         "F");
    leg.AddEntry(hc[7], "Star OK",             "F");

    gROOT->SetSelectedPad(0);
    TObject *obj = leg.DrawClone();
    obj->SetBit(kCanDelete);;
    return obj;
}

Bool_t plot(MStatusDisplay &d, MSQLServer &serv, Int_t first, Int_t last)
{
    TString query[8];

    // 0: All data for which are files available
    query[0]  = "SELECT SUM(TIME_TO_SEC(TIMEDIFF(fRunStop, fRunStart)))/3600 ";
    query[0] += "FROM RunData ";
    query[0] += "LEFT JOIN RunProcessStatus USING (fTelescopeNumber, fRunNumber, fFileNumber) ";
    query[0] += "WHERE fRunTypeKey=2 AND NOT ISNULL(fRawFileAvail)";
    /*
    if (tel>0)
    {
        query[0] += " AND fTelescopeNumber=";
        query[0] += tel;
    }
    */

    // 1: All data
    query[1]  = "SELECT SUM(TIME_TO_SEC(TIMEDIFF(fRunStop,fRunStart)))/3600 ";
    query[1] += "FROM RunData WHERE fRunTypeKEY=2";

    // 2: All data which is not excluded
    query[2]  = "SELECT SUM(TIME_TO_SEC(TIMEDIFF(fRunStop,fRunStart)))/3600 ";
    query[2] += "FROM RunData WHERE fRunTypeKEY=2 AND fExcludedFDAKEY=1";

    // 3: All sequences
    query[3]  = "SELECT SUM(fRunTime)/3600 FROM Sequences";

    // 4: All sequences with callisto failed
    query[4]  = "SELECT SUM(fRunTime)/3600 FROM Sequences ";
    query[4] += "LEFT JOIN SequenceProcessStatus USING (fSequenceFirst) ";
    query[4] += "WHERE ISNULL(fCallisto) AND NOT ISNULL(fFailedTime) AND NOT ISNULL(fAllFilesAvail)";

    // 5: All sequences with callisto=OK
    query[5]  = "SELECT SUM(fRunTime)/3600 FROM Sequences ";
    query[5] += "LEFT JOIN SequenceProcessStatus USING (fSequenceFirst) ";
    query[5] += "WHERE NOT ISNULL(fCallisto)";

    // 6: All sequences with star failed
    query[6]  = "SELECT SUM(fRunTime)/3600 FROM Sequences ";
    query[6] += "LEFT JOIN SequenceProcessStatus USING (fSequenceFirst) ";
    query[6] += "WHERE ISNULL(fStar) AND NOT ISNULL(fFailedTime) AND NOT ISNULL(fCallisto)";

    // 7: All sequences with star=OK
    query[7]  = "SELECT SUM(fRunTime)/3600 FROM Sequences ";
    query[7] += "LEFT JOIN SequenceProcessStatus USING (fSequenceFirst) ";
    query[7] += "WHERE NOT ISNULL(fStar)";

    // 0: All data
    // 1: All data which is not excluded
    // 2: All data for which are files available
    // 3: All sequences
    // 4: All sequences with callisto=not done
    // 5: All sequences with callisto not done and failed
    // 6: All sequences with star=not done
    // 7: All sequences with star not done and failed

    MTime now(-1);
    MTime past;
    past.Set(2004, 1, 1, 13, 1);

    if (first<0)
        first=14;
    if (last<0)
        last=now.GetMagicPeriod();

    TH1F h[8];
    TH2F h8;

    MBinning binsp(last-first+1, first-0.5, last+0.5);
    MBinning binst(4*7, 0, 7);
    for (int i=0; i<8; i++)
    {
        binsp.Apply(h[i]);
        h[i].SetName(Form("H%d", i));
        h[i].SetDirectory(0);
    }

    MH::SetBinning(&h8, &binsp, &binst);
    h8.SetNameTitle("ObsTime", "Distribution of observation time per exposure");
    h8.SetXTitle("Observation Period");
    h8.SetYTitle("Obs. time per exposure [h]");
    h8.SetZTitle("Counts");
    h8.SetDirectory(0);

    Int_t period = 0;
    MTime from;

    while (1)
    {
        if (past.GetMagicPeriod()!=period)
        {
            period = past.GetMagicPeriod();

            if (period>first && period-1<=last)
            {
                TString a = from.GetSqlDateTime();
                TString b = past.GetSqlDateTime();

                for (int i=0; i<8; i++)
                    h[i].Fill(period-1, GetTime(serv, query[i], a, b));

                TArrayD arr(GetObsDist(serv, a, b));

                for (int i=0; i<arr.GetSize(); i++)
                    h8.Fill(period-1, arr[i]);
            }

            if (past>now)
                break;

            from = past;

        }
        past.SetMjd(past.GetMjd()+1);
    }

    for (int i=0; i<h[0].GetNbinsX(); i++)
        h[0].SetBinError(i+1, 0.001);


    h[4].Add(&h[5]);
    h[6].Add(&h[7]);

    // --------------------------------------------

    TCanvas &c0 = d.AddTab("ObsDist", h8.GetTitle());
    c0.SetFillColor(kWhite);

    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetFillColor(kWhite);
    gPad->SetRightMargin(0.01);
    //gPad->SetTopMargin(0.02);
    //gPad->SetLeftMargin(0.09);
    //gPad->SetBottomMargin(0.12);
    gPad->SetGridx();
    gPad->SetGridy();
    gPad->SetLogy();

    TH1 * p = h8.ProjectionY();

    p->SetYTitle("Counts");
    p->SetDirectory(0);
    p->SetBit(kCanDelete);
    p->Draw();

    // --------------------------------------------

    TCanvas &c1 = d.AddTab("Hist", "Bar chart of the processing status of all data");
    c1.SetFillColor(kWhite);
    c1.Divide(2,2);

    c1.cd(1);

    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetFillColor(kWhite);
    gPad->SetRightMargin(0.01);
    gPad->SetTopMargin(0.02);
    gPad->SetLeftMargin(0.09);
    gPad->SetBottomMargin(0.12);
    gPad->SetGridy();
    gPad->SetPad(0, 0.5, 0.75, 1.0);

    h[1].SetBit(TH1::kNoStats);
    // h[0].GetXaxis()->SetRangeUser(13.5, period+0.5);
    h[1].GetXaxis()->CenterTitle();
    h[1].GetXaxis()->SetTitleSize(0.06);
    h[1].GetYaxis()->SetTitleSize(0.06);
    h[1].GetYaxis()->SetTitleOffset(0.7);
    h[1].GetXaxis()->SetLabelSize(0.06);
    h[1].GetYaxis()->SetLabelSize(0.06);
    h[1].GetXaxis()->SetNdivisions(550);
    h[1].SetYTitle("Time [h]");
    h[1].SetFillColor(kWhite);
    h[1].SetLineColor(kBlack);
    h[1].DrawCopy("");

    h[2].SetLineColor(kBlack);
    h[2].SetFillColor(kBlack);
    h[2].DrawCopy("Bsame");

    h[3].SetLineColor(kBlue);
    h[3].SetFillColor(kBlue);
    h[3].DrawCopy("Bsame");

    h[4].SetLineColor(kRed);
    h[4].SetFillColor(kRed);
    h[4].DrawCopy("Bsame");

    h[5].SetLineColor(kGreen);
    h[5].SetFillColor(kGreen);
    h[5].DrawCopy("Bsame");

    h[6].SetLineColor(kRed+100);
    h[6].SetFillColor(kRed+100);
    h[6].DrawCopy("Bsame");

    h[7].SetLineColor(kGreen+100);
    h[7].SetFillColor(kGreen+100);
    h[7].DrawCopy("Bsame");

    h[0].SetMarkerSize(0);
    h[0].SetMarkerColor(15);
    h[0].SetLineWidth(3);
    h[0].SetLineColor(15);
    h[0].DrawCopy("EPsame");

    gPad->Update();
    DrawYears();

    c1.cd(4);

    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetPad(0.75, 0, 1.0, 0.5);

    DrawCake(h);

    // --------------------------------------------

    TCanvas &cx = d.AddTab("All", "Pie charts for all periods");
    cx.SetBorderMode(0);
    cx.SetFrameBorderMode(0);
    cx.SetFillColor(kWhite);
    cx.Divide(12,7);
    cx.cd(1);
    TObject *leg=DrawLegend(h);

    for (int i=h[0].GetNbinsX()-1; i>=0; i--)
    {
        const Int_t num = TMath::Nint(h[0].GetBinCenter(i+1));

        TCanvas &c = d.AddTab(Form("P%d", num), Form("Pie char for period %d", num));
        c.SetBorderMode(0);
        c.SetFrameBorderMode(0);
        c.SetFillColor(kWhite);

        c.cd();

        TPad *pad0 = new TPad(Form("Pad%dl", num), "", 0.59, 0.5, 1, 1);
        pad0->SetBorderMode(0);
        pad0->SetFrameBorderMode(0);
        pad0->SetFillColor(kWhite);
        pad0->SetBit(kCanDelete);
        pad0->Draw();
        pad0->cd();

        leg->Draw();

        c.cd();

        TPad *pad1 = new TPad(Form("Pad%da", num), "", 0.03, 0, 0.50, 1);
        pad1->SetBorderMode(0);
        pad1->SetFrameBorderMode(0);
        pad1->SetFillColor(kWhite);
        pad1->SetBit(kCanDelete);
        pad1->Draw();
        pad1->cd();

        DrawCake(h, i+1, i+1);

        cx.cd(i+2);
        DrawCake(h, i+1, i+1);

        c.cd();

        TPad *pad2 = new TPad(Form("Pad%db", num), "", 0.55, 0.02, 1, 0.48);
        pad2->SetBorderMode(0);
        pad2->SetFrameBorderMode(0);
        pad2->SetFillColor(kWhite);
        pad2->SetBit(kCanDelete);
        pad2->SetGridx();
        pad2->SetGridy();
        pad2->SetRightMargin(0.01);
        pad2->Draw();
        pad2->cd();

        TH1 * p = h8.ProjectionY(Form("Obs%d", num), i+1, i+1);

        p->Rebin(2);
        p->SetBit(TH1::kNoStats);
        p->SetTitle("");
        p->SetYTitle("Counts");
        p->SetDirectory(0);
        p->SetBit(kCanDelete);
        p->Draw();

        /*
        c.cd();

        TPaveText *pave = new TPaveText(0.6, 0.52, 0.98, 0.98);
        pave->SetBorderSize(1);
        pave->AddText(Form("Period: %d", num))->SetTextAlign(22);
        pave->AddSeperator();
        pave->AddText(Form("Start: %s", num));
        pave->AddText(Form("End:   %s", num));
        pave->SetBit(kCanDelete);
        pave->Draw();
        */
    }

    c1.cd(2);

    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetRightMargin(0.01);
    gPad->SetTopMargin(0.02);
    gPad->SetLeftMargin(0.09);
    gPad->SetBottomMargin(0.12);
    gPad->SetPad(0, 0, 0.75, 0.5);
    gPad->SetGridy();

    h[1].Scale(0.01);
    h[0].Divide(&h[1]);
    h[2].Divide(&h[1]);
    h[3].Divide(&h[1]);
    h[4].Divide(&h[1]);
    h[5].Divide(&h[1]);
    h[6].Divide(&h[1]);
    h[7].Divide(&h[1]);
    h[1].Divide(&h[1]);
    h[1].Scale(100);

    for (int i=0; i<h[0].GetNbinsX(); i++)
        h[0].SetBinError(i+1, 0.001);

    h[1].GetXaxis()->SetNdivisions(550);
    h[1].SetYTitle("%");
    h[1].SetXTitle("Period");
    h[1].GetYaxis()->SetRangeUser(0, 100);

    h[1].DrawCopy();
    h[2].DrawCopy("same");
    h[3].DrawCopy("same");
    h[4].DrawCopy("same");
    h[5].DrawCopy("same");
    h[6].DrawCopy("same");
    h[7].DrawCopy("same");
    h[0].DrawCopy("same");
/*
    TLine l;
    l.SetLineColor(kWhite);
    l.SetLineWidth(1);
    l.SetLineStyle(kDotted);
    for (int i=10; i<95; i+=10)
        l.DrawLine(13.5, i, period+0.5, i);
    l.SetLineStyle(kSolid);
    l.DrawLine(13.5, 50, period+0.5, 50);
 */
    gPad->Update();
    DrawYears();

    c1.cd(3);

    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetPad(0.75, 0.5, 1.0, 1.0);

    DrawLegend(h);

    return kTRUE;
}

int plotstat(Int_t first=-1, Int_t last=-1)
{
    MSQLServer serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotstat" << endl;
    cout << "--------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    MStatusDisplay *d = new MStatusDisplay;
    d->SetWindowName(serv.GetName());
    d->SetTitle(serv.GetName());

    plot(*d, serv, first, last);

    //d->SaveAsRoot("plotstat.root");
    //d->SaveAsPS("plotstat.ps");

    return 1;
}

int plotstat(TString path)
{
    MSQLServer serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotstat" << endl;
    cout << "--------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    MStatusDisplay *d = new MStatusDisplay;
    d->SetWindowName(serv.GetName());
    d->SetTitle(serv.GetName());

    plot(*d, serv, -1, -1);

    d->SaveAsRoot(path+"plotstat.root");
    //d->SaveAsPS(path+"plotstat.ps");

    return 1;
}

