void sun()
{
    TGraph gr, gs;

    MTime t(MTime(-1).Year(), 01, 01, 00);

    const double jd = t.GetJD();
    const double at = t.GetAxisTime();

    for (int d=0; d<365; d++)
    {
        Nova::RstTime sun_day = Nova::GetSolarRst(jd+d);

        gr.SetPoint(gr.GetN(), at+d*24*3600, (fmod(sun_day.rise, 1)-0.5)*24*3600+at);
        gs.SetPoint(gs.GetN(), at+d*24*3600, (fmod(sun_day.set,  1)+0.5)*24*3600+at);
    }

    TH1S h("", "", 12*60, at+2*24*3600, at+365*24*3600);
    h.SetStats(kFALSE);
    h.SetMinimum(at);
    h.SetMaximum(at+24*3600);

    h.GetXaxis()->CenterLabels();
    h.GetXaxis()->CenterTitle();
    h.GetXaxis()->SetNdivisions(412, kFALSE);  // 4 weeks per month, 12 months per year
    h.GetXaxis()->SetTitle("Month");
    h.GetXaxis()->SetTimeDisplay(true);
    h.GetXaxis()->SetTimeFormat("%m %F1995-01-01 00:00:00 GMT");

    h.GetYaxis()->SetTitle("UTC");
    h.GetYaxis()->SetNdivisions(412, kFALSE);  // 12 'hours' ticks every 15min.
    h.GetYaxis()->SetTitleOffset(1.4);
    h.GetYaxis()->SetTimeDisplay(true);
    h.GetYaxis()->SetTimeFormat("%Hh %F1995-01-01 00:00:00 GMT");

    h.DrawCopy();

    gPad->SetGrid();

    gr.DrawClone("P");
    gs.DrawClone("P");
}
