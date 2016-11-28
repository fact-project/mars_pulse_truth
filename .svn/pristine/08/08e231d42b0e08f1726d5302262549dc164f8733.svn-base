void extralgospline()
{
    Int_t n = 40;

    TArrayF x(n);
    TArrayF y(n);
    for (int i=0; i<n; i++)
    {
        x[i] = i;
        y[i] = gRandom->Uniform(10)-5;

        y[n/2-2] = 50;
        y[n/2-1] = 100;
        y[n/2]   = 110;
        y[n/2+1] = 70;
        y[n/2+2] = 30;

    }

    TArrayF d1(n);
    TArrayF d2(n);

    // Do some handling if maxpos is last slice!
    MExtralgoSpline s(y.GetArray(), n, d1.GetArray(), d2.GetArray());

    s.SetHeightTm(0.5);
    s.SetRiseFallTime(0.5, 0.5);

    Int_t maxpos = TMath::LocMax(n, y.GetArray());

    Float_t time, dtime, sum, dsum, mtime, max;
    s.SetExtractionType(MExtralgoSpline::kIntegralRel);
    s.Extract(0, maxpos);
    s.GetTime(time, dtime);
    s.GetSignal(sum, dsum);

    s.SetExtractionType(MExtralgoSpline::kAmplitude);
    s.Extract(0, maxpos);
    s.GetTime(mtime, dtime);
    s.GetSignal(max, dsum);

    TGraph gr1(n, x.GetArray(), y.GetArray());

    TGraph gr2;
    for (int i=0; i<n*10; i++)
        gr2.SetPoint(gr2.GetN(), 0.1*i, s.EvalAt(0.1*i));

    gr2.SetMarkerColor(kBlue);
    gr2.SetMarkerStyle(kFullDotMedium);
    gr1.SetMarkerStyle(kFullDotMedium);

    gr1.DrawClone("AP*");
    gr2.DrawClone("PL");

    TMarker m;
    m.SetMarkerStyle(kFullDotMedium);
    m.SetMarkerColor(kRed);
    m.DrawMarker(x[maxpos], y[maxpos]);

    TLine line;
    line.SetLineColor(kRed);
    line.DrawLine(mtime, max-5, mtime, max+5);
    line.DrawLine(mtime-0.5, max, mtime+0.5, max);
    line.SetLineStyle(kDashed);
    line.DrawLine(time, 0, time, 110);

    gPad->SetGridx();
    gPad->SetGridy();
}
