MStatusDisplay *d = new MStatusDisplay;
TList list[201];

void Fit(TGraph &g, double rate_start=3e6, double rate_kink=100, double rate_end=10)
{

    double begin = -1;
    double end   = -1;

    double kinkx = -1;
    double kinky =  0;

    TGraph gl;
    for (int j=0; j<g.GetN(); j++)
    {
        gl.SetPoint(gl.GetN(), g.GetX()[j], log10(g.GetY()[j]));
        if (g.GetY()[j]<rate_start && begin<0)
            begin = g.GetX()[j];
        if (g.GetY()[j]<rate_end && end<0)
            end = g.GetX()[j];
        if (g.GetY()[j]<rate_kink && kinkx<0)
        {
            kinkx = g.GetX()[j];
            kinky = g.GetY()[j];
        }
    }

    TF1 func2("f2", "log10("
              "  [1]*exp([2]*(x-[0]))"
              "+ [1]*exp([3]*(x-[0]))"
              //"+ [4]*exp([5]*(x-700))"
              ")", begin, end);

    Double_t v00 = g.Eval(kinkx-150, 0, "S");
    Double_t v01 = g.Eval(kinkx- 50, 0, "S");

    Double_t v10 = g.Eval(kinkx+100, 0, "S");
    Double_t v11 = g.Eval(kinkx+200, 0, "S");

    //Double_t v20 = g.Eval(700-50, 0, "S");
    //Double_t v21 = g.Eval(700,    0, "S");
    //Double_t v22 = g.Eval(700+50, 0, "S");

    func2.SetParameter(0,  kinkx);
    func2.SetParameter(1,  kinky);
    func2.SetParameter(2,  log(v01/v00)/100);
    func2.SetParameter(3,  log(v11/v10)/100);

    func2.FixParameter(4,  0);
    func2.FixParameter(5,  0);
    func2.SetLineWidth(1);
    func2.SetLineColor(kBlue);

    gl.Fit(&func2, "N0", "", begin, end);

    TF1 func("f1",
             "[0]*exp([1]*(x-[6])) + "
             "[2]*exp([3]*(x-[6])) + "
             "[4]*exp([5]*(x-700))", begin, end);
    func.FixParameter(0,  func2.GetParameter(1));
    func.FixParameter(1,  func2.GetParameter(2));
    func.FixParameter(2,  func2.GetParameter(1));
    func.FixParameter(3,  func2.GetParameter(3));
    func.FixParameter(4,  func2.GetParameter(4));
    func.FixParameter(5,  func2.GetParameter(5));
    func.FixParameter(6,  func2.GetParameter(0));
    func.SetLineWidth(2);
    func.SetLineColor(kBlue);
    func.DrawClone("same");
    func.SetLineWidth(1);

    func.SetLineStyle(kDashed);
    func.SetRange(0, 1000);

    func.FixParameter(0,  0);
    func.FixParameter(2,  0);
    func.FixParameter(4,  func2.GetParameter(4));
    func.DrawClone("same");

    func.FixParameter(0,  func2.GetParameter(1));
    func.FixParameter(2,  0);
    func.FixParameter(4,  0);
    func.DrawClone("same");

    func.FixParameter(0,  0);
    func.FixParameter(2,  func2.GetParameter(1));
    func.FixParameter(4,  0);
    func.DrawClone("same");


    TLatex txt;
    txt.SetTextColor(kBlue);
    txt.SetTextAlign(33);
    txt.DrawLatex(func2.GetParameter(0), func2.GetParameter(1),
                  Form("%.1fHz @ %.0f", func2.GetParameter(1), func2.GetParameter(0)));

    // [1]*exp([2]*(x-[0])) / exp(1)           = [1]*exp([3]*(x-[0]))
    //     exp([2]*(x-[0])) / exp(1)           =    exp([3]*(x-[0]))
    //     exp([2]*(x-[0])) / exp([3]*(x-[0])) =    exp(1)
    //     exp([2]*(x-[0])  -     [3]*(x-[0])) =    exp(1)
    //         [2]*(x-[0])  -     [3]*(x-[0])  =    1
    //
    // x = 1/([2]-[3]) + [0]


    Double_t th   = 1. / (func2.GetParameter(3)-func2.GetParameter(2)) + func2.GetParameter(0);
    Double_t rate = pow(10, func2.Eval(th));

    txt.SetTextAlign(11);
    txt.DrawLatex(th+10, rate, Form("%.1fHz @ %.0f", rate, th));

    txt.SetTextAlign(12);
    txt.DrawLatex(180, 3e6, Form("%.1f", 1./func2.GetParameter(2)));
    txt.DrawLatex(350,   3, Form("%.0f", 1./func2.GetParameter(3)));

    TLine line;
    line.SetLineStyle(kDashed);
    line.SetLineColor(kBlue);
    line.DrawLine(th, 1e-2, th, 1e8);

}

void plot(const char *tab, const char *title="")
{
    gROOT->SetSelectedPad(0);
    d->AddTab(tab);

    gPad->SetGrid();
    gPad->SetLogy();

    TH1D h0("frame", title, 1000, 0, 1000);
    h0.SetStats(kFALSE);
    h0.SetXTitle("Threshold [dac]");
    h0.SetYTitle("Trigger rate [Hz]");
    h0.SetDirectory(0);
    h0.SetMaximum(1e8);
    h0.SetMinimum(0.01);
    h0.DrawCopy();
}


void same(const char *id, const char *fmt, bool fit=true, bool boards=false, bool patches=false)
{
    for (int i=0; i<160; i++)
    {
        TGraph *g = (TGraph*)list[i+41].FindObject(id);
        if (!g)
        {
            cout << "==W==> " << id << " not found in i=" << i+41 << endl;
            return;
        }

        g->SetMarkerStyle(kFullDotSmall);
        g->SetMarkerColor(kGray+1);
        g->SetLineColor(kGray+1);
        if (patches)
            g->DrawClone(fmt);

        if (g->Eval(400)>10)
            cout << "Patch " << setw(3) << i << ": C" << i/40 << " B" << (i%40)/4 << " P" << (i%40)%4 << ": " << g->Eval(400) << "Hz @ th=400" << endl;
    }

    for (int i=0; i<40; i++)
    {
        TGraph *g = (TGraph*)list[i+1].FindObject(id);
        if (!g)
        {
            cout << "==W==> " << id << " not found in i=" << i+1 << endl;
            return;
        }

        g->SetMarkerStyle(kFullDotSmall);
        g->SetMarkerColor(kGray+2);
        g->SetLineColor(kGray+2);
        if (boards)
            g->DrawClone(fmt);

        if (g->Eval(400)>10)
            cout << "Board " << setw(3) << i << ": C" << i/10 << " B" << i%10 << ":    " << g->Eval(400) << "Hz @ th=400" << endl;
    }


    TGraph *g001 = (TGraph*)list[0].FindObject(id);
    if (!g001)
    {
        cout << "==W==> " << id << " not found in i=0" << endl;
        return;
    }

    g001->SetMarkerStyle(boards||patches?kFullDotMedium:kFullDotSmall);
    g001->DrawClone(fmt);

    if (fit)
        Fit(*g001);
}

void read(const char *filename)
{
    fits fin(filename);
    if (!fin)
        return;

    cout << '\n' << filename << '\n';
    cout << setfill('-') <<setw(strlen(filename)) << '-' << setfill(' ') << endl;

    //fin.PrintColumns();

    ULong64_t start;
    UInt_t    threshold;
    Float_t   time;
    Float_t   ontime;
    Float_t   Rc;
    Float_t   Rb[40];
    Float_t   Rp[160];

    fin.SetPtrAddress("Data0", &start);
    fin.SetPtrAddress("Data1", &threshold);
    fin.SetPtrAddress("Data2", &time);
    fin.SetPtrAddress("Data3", &ontime);
    fin.SetPtrAddress("Data4", &Rc);
    fin.SetPtrAddress("Data5", Rb);
    fin.SetPtrAddress("Data6", Rp);

    ULong_t first = 0;

    TGraph *g[201];

    while (fin.GetNextRow())
    {
        if (first==0 || first!=start)
        {
            first = start;

            MTime t;
            t.SetUnixTime(start);

            TString name = t.GetString();
            name = name(0, 19);

            cout << name << endl;

            for (int i=0; i<201; i++)
            {
                g[i] = new TGraph;
                g[i]->SetMarkerStyle(kFullDotMedium);
                g[i]->SetName(name);
                g[i]->SetTitle(t.GetStringFmt("%H:%M:%S"));

                list[i].Add(g[i]);
            }
        }

        g[0]->SetPoint(g[0]->GetN(), threshold, Rc/ontime);
        for (int i=0; i<40; i++)
            g[i+1]->SetPoint(g[i+1]->GetN(), threshold, Rb[i]);
        for (int i=0; i<160; i++)
            g[i+41]->SetPoint(g[i+41]->GetN(), threshold, Rp[i]);
    }
}

void ratescan()
{
    cout << setprecision(4);

    // Read three files
    read("/loc_data/aux/2012/02/18/20120218.RATE_SCAN_DATA.fits");
    read("/loc_data/aux/2012/02/19/20120219.RATE_SCAN_DATA.fits");
    read("/loc_data/aux/2012/02/21/20120221.RATE_SCAN_DATA.fits");

    // add a new tab
    plot("0.0V", "Temp control 19.02./22.02. (+0V)");

    // Plot a curve, do not fit
    same("19.02.2012 20:21:26", "LP", false);

    // plot a curve, fit and plot boards
    same("22.02.2012 04:19:15", "LP", true, true);

    // raed another file
    read("/loc_data/aux/2012/03/25/20120325.RATE_SCAN_DATA.fits");

    // add another tab
    plot("25/03", "Current control 25.03. (+0V)");

    // plot curve, fit and plot boards and patches
    same("26.03.2012 03:01:14", "LP", true, true, true);

    // plot curve, do not fit, do not draw boards and patches
    same("26.03.2012 03:21:47", "LP", false);

    // write result to output file
    d->SaveAs("/home/tbretz/ratescan.root");
}
