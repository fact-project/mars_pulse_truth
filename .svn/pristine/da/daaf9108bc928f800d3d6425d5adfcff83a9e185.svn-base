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
!   Author(s): Thomas Bretz, 05/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHWeather
//
// Display weather and pyrometer data and the corresponding event rate
//
// Class Version 2:
// ----------------
//  - fSolarRadiation
//  + fCloudiness
//  + fTempSky
//  + fTempAir
//
////////////////////////////////////////////////////////////////////////////
#include "MHWeather.h"

#include <TH1.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TGaxis.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MTime.h"
#include "MReportCC.h"
#include "MReportPyrometer.h"
#include "MEventRate.h"

ClassImp(MHWeather);

using namespace std;

void MHWeather::ResetGraph(TGraph &g) const
{
    g.Set(1);
    g.SetPoint(0, 0, 0); // Dummy Point
    g.SetEditable();     // Used as flag: First point? yes/no
}

void MHWeather::InitGraph(TGraph &g) const
{
    ResetGraph(g);
    g.SetMarkerStyle(kFullDotMedium);
}

void MHWeather::AddPoint(TGraph &g, Double_t x, Double_t y) const
{
    if (g.IsEditable())
    {
        g.RemovePoint(0);
        g.SetEditable(kFALSE);
    }

    g.SetPoint(g.GetN(), x, y);
}

// --------------------------------------------------------------------------
//
// Setup histograms 
//
MHWeather::MHWeather(const char *name, const char *title)
: fReportCC(0), fReportPyro(0), fRate(0)
{
    fName  = name  ? name  : "MHWeather";
    fTitle = title ? title : "Graphs for weather data";

    // Init Graphs

    fHumidity.SetNameTitle("Humidity", "Humidity");
    fTemperature.SetNameTitle("Temperature", "Temperature (red=weather station, black=pyrometer, blue=sky)");
    fWindSpeed.SetNameTitle("WindSpeed", "Wind Speed");

    fEventRate.SetNameTitle("EventRate", "Event Rate at CC-REPORT time");

    fCloudiness.SetNameTitle("Cloudiness", "Cloudiness / Humidity");
    fTempAir.SetNameTitle("TempAir", "Air temperature from Pyrometer");
    fTempSky.SetNameTitle("TempSky", "Sky temperature from Pyrometer");

    InitGraph(fHumidity);
    InitGraph(fTemperature);
    InitGraph(fWindSpeed);

    InitGraph(fEventRate);

    InitGraph(fCloudiness);
    InitGraph(fTempAir);
    InitGraph(fTempSky);

    fHumidity.SetMarkerColor(kBlue);
    fTemperature.SetMarkerColor(kRed);
    fTempSky.SetMarkerColor(kBlue);
}

// --------------------------------------------------------------------------
//
// Setup the Binning for the histograms automatically if the correct
// instances of MBinning
//
Bool_t MHWeather::SetupFill(const MParList *plist)
{
    // Remark: This is called twice in the star-eventloop!
    fReportCC = (MReportCC*)plist->FindObject("MReportCC");
    if (!fReportCC)
    {
        *fLog << warn << "MReportCC not found... abort." << endl;
        return kFALSE;
    }
    fRate = (MEventRate*)plist->FindObject("MEventRate");
    if (!fRate)
    {
        *fLog << warn << "MEventRate not found... abort." << endl;
        return kFALSE;
    }

    fReportPyro = (MReportPyrometer*)plist->FindObject("MReportPyrometer");
    if (!fReportPyro)
        *fLog << warn << "MReportPyrometer not found..." << endl;

    // Reset Graphs
    ResetGraph(fHumidity);
    ResetGraph(fTemperature);
    ResetGraph(fWindSpeed);
    ResetGraph(fEventRate);
    ResetGraph(fCloudiness);
    ResetGraph(fTempAir);
    ResetGraph(fTempSky);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histograms with data from a MMuonCalibPar and
// MMuonSearchPar container.
//
Int_t MHWeather::Fill(const MParContainer *par, const Stat_t w)
{
    const MTime *t = dynamic_cast<const MTime*>(par);
    if (!t)
    {
        *fLog << err <<"MHWeather::Fill - ERROR: MTime not given as argument... abort." << endl;
        return kERROR;
    }

    const Double_t tm = t->GetAxisTime();

    AddPoint(fEventRate, tm, fRate->GetRate());

    if (TString(t->GetName())=="MTimeCC")
    {
        AddPoint(fTemperature, tm, fReportCC->GetTemperature());
        AddPoint(fHumidity,    tm, fReportCC->GetHumidity());
        AddPoint(fWindSpeed,   tm, fReportCC->GetWindSpeed());
    }

    if (TString(t->GetName())=="MTimePyrometer" && fReportPyro)
    {
        AddPoint(fCloudiness, tm, fReportPyro->GetCloudiness());
        AddPoint(fTempAir,    tm, fReportPyro->GetTempAir());
        AddPoint(fTempSky,    tm, fReportPyro->GetTempSky()-200);
    }

    return kTRUE;
}

void MHWeather::DrawGraph(TGraph &g, const char *y) const
{
    TH1 *h = g.GetHistogram();
    if (h)
    {
        delete h;
        g.SetHistogram(0);
        h = g.GetHistogram();
    }
    if (h)
    {
        TAxis *axe = h->GetXaxis();
        axe->SetLabelSize(0.033);
        axe->SetTimeFormat("%H:%M:%S %F1995-01-01 00:00:00 GMT");
        axe->SetTimeDisplay(1);
        axe->SetTitle("Time");
        if (y)
            h->SetYTitle(y);
    }
}

// --------------------------------------------------------------------------
//
// Update position of an axis on the right side of the histogram
//
void MHWeather::UpdateRightAxis(TGraph &g) const
{
    TH1 &h = *g.GetHistogram();

    const Double_t max = h.GetMaximum();
    if (max==0)
        return;

    TGaxis *axis = (TGaxis*)gPad->FindObject("RightAxis");
    if (!axis)
        return;

    axis->SetX1(g.GetXaxis()->GetXmax());
    axis->SetX2(g.GetXaxis()->GetXmax());
    axis->SetY1(gPad->GetUymin());
    axis->SetY2(gPad->GetUymax());
    axis->SetWmax(max);
}

// --------------------------------------------------------------------------
//
// Draw an axis on the right side of the histogram
//
void MHWeather::DrawRightAxis(const char *title, Int_t col) const
{
    TGaxis *axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(),
                              gPad->GetUxmax(), gPad->GetUymax(),
                              0, 1, 510, "+L");
    axis->SetBit(TAxis::kRotateTitle);
    axis->SetName("RightAxis");
    axis->SetTitle(title);
    axis->SetTitleOffset(1.2);
    axis->SetTextColor(col);
    axis->SetLabelColor(col);
    axis->SetBit(kCanDelete);
    axis->Draw();
}

// --------------------------------------------------------------------------
//
// This displays the TGraph like you expect it to be (eg. time on the axis)
// It should also make sure, that the displayed time always is UTC and
// not local time...
//
void MHWeather::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad();

    pad->Divide(2,2);

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fTemperature.Draw("AP");
    fTempAir.Draw("P");
    fTempSky.Draw("P");
    DrawRightAxis("T_{sky}-200 [K]", kBlue);

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
     fHumidity.Draw("AP");
    fCloudiness.Draw("P");
    DrawRightAxis("Cloudiness [%]");

    pad->cd(3);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fWindSpeed.Draw("AP");

    pad->cd(4);
    gPad->SetBorderMode(0);
    gPad->SetGridx();
    gPad->SetGridy();
    fEventRate.Draw("AP");
}

void MHWeather::Paint(Option_t *o)
{
    // If this is set to early the plot remains empty in root 5.12/00
    if (fHumidity.GetN()>0)
    {
        fHumidity.SetMinimum(0);
        fHumidity.SetMaximum(100);
    }
    if (fCloudiness.GetN()>0)
    {
        fCloudiness.SetMinimum(0);
        fCloudiness.SetMaximum(100);
    }

    if (fTemperature.GetN()>0)
    {
        fTemperature.SetMinimum(-2.5);
        fTemperature.SetMaximum(27.5);
    }
    if (fTempSky.GetN()>0)
    {
        fTempSky.SetMinimum(-2.5);
        fTempSky.SetMaximum(27.5);
    }
    if (fWindSpeed.GetN()>0)
        fWindSpeed.SetMinimum(0);
    if (fEventRate.GetN()>0)
        fEventRate.SetMinimum(0);

    DrawGraph(fHumidity,    "Humidity [%]");
    DrawGraph(fTemperature, "T [\\circ C]");
    DrawGraph(fWindSpeed,   "km/h");
    DrawGraph(fEventRate,   "f [Hz]");
    DrawGraph(fCloudiness,  "Cloudiness [%]");
    DrawGraph(fTempAir,     "T [\\circ C]");
    DrawGraph(fTempSky,     "T_{sky}-200 [K]");

    TVirtualPad *p = gPad;

    p->cd(1);
    if (gPad)
    {
        fTemperature.GetYaxis()->SetLabelColor(kRed);
        fTemperature.GetYaxis()->SetTitleColor(kRed);
        UpdateRightAxis(fTemperature); // Primary axis
    }

    p->cd(2);
    if (gPad)
    {
        fHumidity.GetYaxis()->SetLabelColor(kBlue);
        fHumidity.GetYaxis()->SetTitleColor(kBlue);
        fHumidity.GetYaxis()->SetTitleOffset(1.2);
        UpdateRightAxis(fHumidity);    // Primary axis
    }
}
