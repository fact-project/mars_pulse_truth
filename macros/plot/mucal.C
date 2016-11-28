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
!   Author(s): Thomas Bretz, 02/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// mucal.C
// =======
//
// Use this macro to plot the calibration correction factors versus period.
//
/////////////////////////////////////////////////////////////////////////////
void mucal()
{
    TEnv env("resources/calibration.rc");

    TGraph g;
    for (int i=0; i<100; i++)
    {
        TString s;
        s += i;

        Double_t f = env.GetValue(s, -1.0);
        if (f>0)
            g.SetPoint(g.GetN(), i, f);
    }

    TH1   *h    = g.GetHistogram();
    TAxis *axex = h->GetXaxis();
    TAxis *axey = h->GetYaxis();

    h->SetXTitle("Observation Period");
    h->SetYTitle("Calibration correction factor (S_{D}/S_{MC})");
    axey->CenterTitle();
    axex->CenterTitle();

    axey->SetLabelSize(0.06);
    axey->SetTitleSize(0.05);
    axex->SetLabelSize(0.06);
    axex->SetTitleSize(0.06);
    axey->SetTitleOffset(0.65);

    g.SetMarkerStyle(kFullDotLarge);

    TCanvas *c1 = new TCanvas;
    c1->Divide(1,2);
    c1->cd(1);

    gPad->SetGridx();
    gPad->SetGridy();

    gPad->SetTopMargin(0.01);
    gPad->SetRightMargin(0.01);
    gPad->SetLeftMargin(0.07);
    gPad->SetBottomMargin(0.13);

    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetFillColor(kWhite);

    Double_t min = 0.71;
    Double_t max = 1.29;

    g.SetMinimum(min);
    g.SetMaximum(max);
    g.DrawClone("AP");

    TLine l;
    l.SetLineStyle(kDashed);
    l.SetLineWidth(1);
    l.DrawLine(axex->GetXmin(), 1, axex->GetXmax(), 1);

    l.SetLineColor(kBlue);
    l.SetLineStyle(kDashed);
    for (int i=2000; i<2020; i++)
    {
        Double_t period = MAstro::GetMagicNewYear(i);
        if (period>axex->GetXmin() && period<axex->GetXmax())
            l.DrawLine(period, min, period, max);
    }
}
