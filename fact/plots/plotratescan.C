/*void Fit(TGraph &g, double rate_start=3e5, double rate_kink=80, double rate_end=1)
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
        {
            end = g.GetX()[j];
            //if (end<550)
            //    end = 750;
        }
        if (g.GetY()[j]<rate_kink && kinkx<0)
        {
            kinkx = g.GetX()[j];
            kinky = g.GetY()[j];
        }
    }

    if (end==-1)
        end = 600;//g.GetX()[g.GetN()-1];

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

    cout << begin << " -- " << end << " : " << kinkx << endl;

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


    Double_t th       = 1. / (func2.GetParameter(3)-func2.GetParameter(2)) + func2.GetParameter(0);
    Double_t rate     = pow(10, func2.Eval(th));
    Double_t rate_end = pow(10, func2.Eval(end)-0.5);

    txt.SetTextAlign(11);
    txt.DrawLatex(th+10, rate, Form("%.1fHz @ %.0f", rate, th));

    txt.SetTextAlign(12);
    txt.DrawLatex(180, 3e6,      Form("%.1f", 1./func2.GetParameter(2)));
    txt.DrawLatex(end, rate_end, Form("%.0f", 1./func2.GetParameter(3)));

    TLine line;
    line.SetLineStyle(kDashed);
    line.SetLineColor(kBlue);
    line.DrawLine(th, 1e-2, th, 1e8);
}*/

Bool_t plot(MSQLMagic &serv, Long64_t search_id, TGraph &g, TGraph *gb)
{
    TString query;
    query += "SELECT fTimeBegin, fTimeEnd, fVoltageIsOn, fOvervoltage, fCurrentMedMean, fNight, fAzMin, fAzMax, fZdMin, fZdMax ";
    query += "FROM Ratescan WHERE fRatescanID=";
    query += search_id;

    TSQLResult *res = serv.Query(query);
    if (!res)
        return kFALSE;

    if (res->GetRowCount()!=1)
    {
        cout << "ERROR - Row count is not 1." << endl;
        delete res;
        return kTRUE;
    }

    TSQLRow *row = res->Next();

    const char *time_beg = (*row)[0];
    const char *time_end = (*row)[1];
    const char *night    = (*row)[5];
    const char *az_beg = (*row)[6];
    const char *az_end = (*row)[7];
    const char *zd_beg = (*row)[8];
    const char *zd_end = (*row)[9];

    int   voltage_on  = (*row)[2] ? atoi((*row)[2]) :   -1;
    float overvoltage = (*row)[3] ? atof((*row)[3]) : -100;
    float current     = (*row)[4] ? atof((*row)[4]) :   -1;

    delete row;

    gROOT->SetSelectedPad(0);
    TCanvas *c = new TCanvas(time_beg+11, time_beg);

    c->SetGrid();
    c->SetLogy();
    c->SetTopMargin(0.01);
    c->SetRightMargin(0.005);

    TPaveText leg(600, 1e5, 1050, 8e8, "br");
    leg.SetBorderSize(1);
    leg.SetFillColor(kWhite);
    leg.SetTextAlign(12);
    leg.AddText(Form("Ratescan %d ", search_id));
    //leg.AddText("");
    leg.AddText(Form("Begin       %s", time_beg));
    leg.AddText(Form("End          %s", time_end));
    leg.AddText(Form("Az            %s#circ to %s#circ", az_beg, az_end));
    leg.AddText(Form("Zd             %s#circ to %s#circ", zd_beg, zd_end));
    //leg.AddText("");
    if (voltage_on==0)
        leg.AddText("Voltage off");
    else
    {
        if (current>=0)
            leg.AddText(Form("Current     <I_{med}>  =  %.1f #muA", current));
        if (overvoltage>-70)
            leg.AddText(Form("Voltage   #DeltaU  =  %+.2f V", overvoltage));
    }

    TH1D h0("frame", "", 1050, 0, 1050);
    h0.SetDirectory(0);
    h0.SetStats(kFALSE);
    h0.SetXTitle("Threshold [dac counts]");
    h0.SetYTitle("Trigger rate [Hz]");
    h0.GetXaxis()->SetLabelSize(0.05);
    h0.GetXaxis()->SetTitleSize(0.05);
    h0.GetYaxis()->SetLabelSize(0.05);
    h0.GetYaxis()->SetTitleSize(0.05);
    h0.SetMaximum(8e8);
    h0.SetMinimum(0.61);
    h0.DrawCopy();

    leg.DrawClone();

    for (int i=0; i<40; i++)
    {
        gb[i].SetNameTitle(Form("Board%02d", i), "Board trigger rate (time 40)");
        gb[i].DrawClone("P");
    }

    g.SetMarkerStyle(kFullDotMedium);
    g.SetMarkerColor(kBlue);
    g.SetLineColor(kBlue);
    g.SetNameTitle("Camera", "Camera trigger rate");
    g.DrawClone("PL");

    TGraph gr("good_ratescan_edit.txt", "%lg %lg");
    gr.SetLineColor(12);
    gr.DrawClone("L");

    c->Write();

    TString name;
    name += night;
    name += "-ratescan ";
    name += time_beg;

    //c->SaveAs(name+".pdf");
    //c->SaveAs(name+".eps");
    //c->SaveAs("/loc_data/analysis/"+name+".png");
    c->SaveAs(Form("/loc_data/analysis/ratescans/%04d/%02d/%02d/%06d_%d.png", atoi(night)/10000, (atoi(night)/100)%100, atoi(night)%100, atoi(night), search_id));

    delete c;
}

Int_t plotid(MSQLMagic &serv, fits &file, const char *_search_id, Int_t row)
{
    Long64_t search_id = atol(_search_id);

    cout << "   " << search_id << endl;

    UInt_t offset = file.GetUInt("MJDREF");

    bool old = file.HasColumn("Data0");

    Double_t  *ptime   = file.SetPtrAddress("Time");
    UInt_t    *pid     = file.SetPtrAddress(old ? "Data0" : "Id");
    Float_t   *rates   = file.SetPtrAddress(old ? "Data5" : "BoardRate");
    UInt_t    *pth     = file.SetPtrAddress(old ? "Data1" : "Threshold");
    Float_t   *ptrig   = file.SetPtrAddress(old ? "Data4" : "TriggerRate");
    Float_t   *pontime = file.SetPtrAddress(old ? "Data3" : "RelOnTime");

    if (!ptime || !pid || !rates || !pth || !ptrig || !pontime)
        return -1;

    TGraph g;
    TGraph gb[40];

    while (file.GetRow(row++))
    {
        if (pid[0]<search_id)
            continue;

        if (pid[0]!=search_id)
            break;

        g.SetPoint(g.GetN(), pth[0], ptrig[0]/pontime[0]);
        for (int i=0; i<40; i++)
            gb[i].SetPoint(gb[i].GetN(), pth[0], rates[i]*40);
    }

    if (g.GetN()>0)
        plot(serv, search_id, g, gb);

    return row;


}

Bool_t plotratescan(MSQLMagic &serv, const char *_night)
{
    Long64_t night = atol(_night);

    TString fname = Form("/fact/aux/%04d/%02d/%02d/%06d.RATE_SCAN_DATA.fits",
                         night/10000, (night/100)%100, night%100, night);

    cout << "   " << fname << endl;

    fits file(fname.Data());
    if (!file)
    {
        cout << "ERROR - Cannot access " << fname << ": " << gSystem->GetError() << endl;
        return kFALSE;
    }

    TString query;
    query += "SELECT fRatescanID FROM Ratescan WHERE fNight=";
    query += night;
    query += " ORDER BY fRatescanID";
    /*
    query += "SELECT fRatescanID, fTimeBeg, fTimeEnd, fVoltageIsOn, ";
    query += "       fOvervoltage, fCurrentMedMean, fRateBegin, fRateEnd, ";
    query += "       fThresholdBegin, fThresholdEnd, fNumPoints ";
    query += "WHERE fNight=";
    query += night;
     */
    TSQLResult *res = serv.Query(query);
    if (!res)
        return kFALSE;

    if (res->GetRowCount()==0)
    {
        delete res;
        return kTRUE;
    }

    TString oname = Form("%06d-ratescan.root", night);
    //cout << "   " << oname << '\n' << endl;
    TFile rootfile(oname.Data(), "recreate");

    TSQLRow *row = 0;

    Int_t cnt = 0;
    while ((row=res->Next()) && cnt>=0)
    {
        const char *id = (*row)[0];
        cnt = plotid(serv, file, id, cnt);
        delete row;
    }

    delete res;

    cout << endl;

    return cnt>=0;
}

Bool_t plotratescan(const char *night)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotratescan" << endl;
    cout << "------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "Night: " << night << endl;
    cout << endl;

    return plotratescan(serv, night);
}

Bool_t plotratescan()
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "plotratescan" << endl;
    cout << "------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    TSQLResult *res = serv.Query("SELECT fNight FROM Ratescan GROUP BY fNight ORDER BY fNight");
    if (!res)
        return kFALSE;

    TSQLRow *row = 0;
    while ((row=res->Next()))
    {
        const char *night = (*row)[0];
        plotratescan(serv, night);
        delete row;
    }

    delete res;

    return kTRUE;
}
