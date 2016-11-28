#include <algorithm>
#include <functional>

Bool_t Contains(TArrayD **vec, Double_t t0, Double_t range=0)
{
    TArrayD *arr0 = vec[0];
    TArrayD *arr1 = vec[1];
    TArrayD *arr2 = vec[2];

    for (int i=0; i<arr0->GetSize(); i++)
    {
        Double_t start = (*arr1)[i];
        Double_t stop  = (*arr2)[i];

        if (stop>start+305./24/3600)
            stop = start+305./24/3600;

        if (t0>start-range && t0<stop+range)
        //{
        //    if (fmod(t0, 1)>4./24 && fmod(t0,1)<4.1/24)
        //        cout << t0-start << " " <<t0 << " " << stop-t0 << " " << start-15779 << " " << stop-15779 << " " << (*arr0)[i] << endl;
            return kTRUE;
        //}
    }

    return arr0->GetSize()==0;
}

Int_t PlotThresholds(TArrayD **vec, TString fname)
{
    fname += ".RATE_CONTROL_THRESHOLD.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    UShort_t th;

    if (!file.SetPtrAddress("Time", &time))
        return -1;

    if (!file.SetPtrAddress("threshold", &th))
        return -1;

    TGraph g;
    g.SetName("Threshold");

    while (file.GetNextRow())
    {
        if (Contains(vec, time, 10./(24*3600)))
            g.SetPoint(g.GetN(), time*24*3600, th);
    }

    g.SetMinimum(281);
    g.SetMarkerStyle(kFullDotMedium);
    g.GetXaxis()->SetTimeDisplay(true);
    g.GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
    g.GetXaxis()->SetLabelSize(0.12);
    g.GetYaxis()->SetLabelSize(0.1);
    g.GetYaxis()->SetTitle("THRESHOLD");
    g.GetYaxis()->SetTitleOffset(0.2);
    g.GetYaxis()->SetTitleSize(0.1);
    g.DrawClone("AP");

    return 0;
}

#include <vector>
#include <pair>

vector<pair<double, Nova::EquPosn>> vecp;

Nova::EquPosn FindPointing(Double_t time)
{
    for (int i=0; i<vecp.size(); i++)
        if (time<vecp[i].first)
        {
            if (i==0)
                return Nova::EquPosn();
            else
                return vecp[i-1].second;
        }

    return vecp[vecp.size()-1].second;
}

Float_t Prediction(Double_t time)
{
    Double_t jd = time + 40587 + 2400000.5;

    // Sun properties  	 	 
    Nova::EquPosn  sun  = Nova::GetSolarEquCoords(jd);  	 	 
    Nova::ZdAzPosn hrzs = Nova::GetHrzFromEqu(sun, jd);  	 	 

    // Get source position
    Nova::EquPosn pos = FindPointing(time);


    // Moon properties 
    Nova::EquPosn moon = Nova::GetLunarEquCoords(jd, 0.01); 
    Nova::HrzPosn hrzm = Nova::GetHrzFromEqu(moon, jd); 
    double        disk = Nova::GetLunarDisk(jd); 

    // Derived moon properties 
    double angle = Nova::GetAngularSeparation(moon, pos); 
    double edist = Nova::GetLunarEarthDist(jd)/384400; 

    // Current prediction 
    double sin_malt  = hrzm.alt<0 ? 0 : sin(hrzm.alt*TMath::DegToRad()); 
    double cos_mdist = cos(angle*TMath::DegToRad()); 
    double sin_szd   = sin(hrzs.zd*TMath::DegToRad()); 

    double c0 = pow(disk,      2.63); 
    double c1 = pow(sin_malt,  0.60); 
    double c2 = pow(edist,    -2.00); 
    double c3 = exp(0.67*cos_mdist*cos_mdist*cos_mdist*cos_mdist); 
    double c4 = exp(-97.8+105.8*sin_szd*sin_szd); 

    double cur = 6.2 + 95.7*c0*c1*c2*c3 + c4; 

    return cur; 
}

Int_t ReadSources(TString fname)
{
    fname += ".DRIVE_CONTROL_SOURCE_POSITION.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    Double_t time, ra, dec;
    if (!file.SetPtrAddress("Time",  &time))
        return -1;
    if (!file.SetPtrAddress("Ra_cmd", &ra))
        return -1;
    if (!file.SetPtrAddress("Dec_cmd", &dec))
        return -1;

    while (file.GetNextRow())
    {
        Nova::EquPosn p;
        p.ra  = ra*15;
        p.dec = dec;

        vecp.push_back(make_pair(time, p));
    }

    return 0;
}

Int_t PlotCurrent(TArrayD **vec, TString fname)
{
    Int_t rc = ReadSources(fname);
    if (rc<0)
        return rc;

    fname += ".FEEDBACK_CALIBRATED_CURRENTS.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    Float_t  Imed;
    Float_t  Idev;
    Float_t  I[416];

    if (!file.SetPtrAddress("Time",  &time))
        return -1;

    if (!file.SetPtrAddress("I_med", &Imed))
        return -1;

    if (!file.SetPtrAddress("I_dev", &Idev))
        return -1;

    if (!file.SetPtrAddress("I", I))
        return -1;

    TGraph g1;
    TGraph g2;
    TGraph g3;
    TGraph g4;
    TGraph g5;
    g1.SetName("CurrentsMed");
    g2.SetName("Prediction");
    g3.SetName("CurrentsDev");
    g4.SetName("CurrentsMax-4");
    g5.SetName("CurrentsMax");

    while (file.GetNextRow())
        if (Contains(vec, time))
        {
            // crazy pixels
            I[66]  = 0;
            I[191] = 0;
            I[193] = 0;

            sort(I, I+320);

            g1.SetPoint(g1.GetN(), time*24*3600, Imed);
            g2.SetPoint(g2.GetN(), time*24*3600, Prediction(time));
            g3.SetPoint(g3.GetN(), time*24*3600, Imed+Idev);
            g4.SetPoint(g4.GetN(), time*24*3600, I[315]);
            g5.SetPoint(g5.GetN(), time*24*3600, I[319]);
        }

    g1.SetMinimum(0);
    g1.SetMaximum(149);

    g1.SetMarkerStyle(kFullDotMedium);
    g1.GetXaxis()->SetTimeDisplay(true);
    g1.GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
    g1.GetXaxis()->SetLabelSize(0.12);
    g1.GetYaxis()->SetLabelSize(0.1);
    g1.GetYaxis()->SetTitle("CURRENT");
    g1.GetYaxis()->SetTitleOffset(0.2);
    g1.GetYaxis()->SetTitleSize(0.1);
    g1.DrawClone("AP");

    g5.SetMarkerColor(kGray);
    g5.DrawClone("P");

    g4.SetMarkerColor(kGray+1);
    g4.DrawClone("P");

    g3.SetMarkerColor(kGray+2);
    g3.DrawClone("P");

    g2.SetMarkerColor(kBlue);
    g2.SetMarkerStyle(kFullDotMedium);
    g2.DrawClone("P");

    g1.DrawClone("P");

    return 0;
}

Int_t PlotRate(TArrayD **vec, TString fname)
{
    fname += ".FTM_CONTROL_TRIGGER_RATES.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    Float_t  rate;
    Float_t  ontime, elapsed;

    if (!file.SetPtrAddress("Time",  &time))
        return -1;

    if (!file.SetPtrAddress("TriggerRate", &rate))
        return -1;
    if (!file.SetPtrAddress("OnTime", &ontime))
        return -1;
    if (!file.SetPtrAddress("ElapsedTime", &elapsed))
        return -1;

    TGraph g1, g2;
    g1.SetName("TriggerRate");
    g2.SetName("RelOnTime");

    while (file.GetNextRow())
        if (Contains(vec, time))
        {
            if (rate>=0)
            {
                g1.SetPoint(g1.GetN(), time*24*3600, rate);
                g2.SetPoint(g2.GetN(), time*24*3600, ontime/elapsed);
            }
        }

    g1.SetMinimum(0);
    g1.SetMaximum(269);
    g1.SetMarkerStyle(kFullDotMedium);
    g1.GetXaxis()->SetTimeDisplay(true);
    g1.GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
    g1.GetXaxis()->SetLabelSize(0.12);
    g1.GetYaxis()->SetLabelSize(0.1);
    g1.GetYaxis()->SetTitle("TRIGGER RATE");
    g1.GetYaxis()->SetTitleOffset(0.2);
    g1.GetYaxis()->SetTitleSize(0.1);
    g1.DrawClone("AP");

    gROOT->SetSelectedPad(0);
    gPad->GetCanvas()->cd(4);

    gPad->SetGrid();
    gPad->SetTopMargin(0);
    gPad->SetBottomMargin(0);
    gPad->SetRightMargin(0.001);
    gPad->SetLeftMargin(0.04);

    g2.SetMinimum(0);
    g2.SetMaximum(1);
    g2.SetMarkerStyle(kFullDotMedium);
    g2.GetXaxis()->SetTimeDisplay(true);
    g2.GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
    g2.GetXaxis()->SetLabelSize(0.12);
    g2.GetYaxis()->SetLabelSize(0.1);
    g2.GetYaxis()->SetTitle("RELATIVE ON TIME");
    g2.GetYaxis()->SetTitleOffset(0.2);
    g2.GetYaxis()->SetTitleSize(0.1);
    g2.DrawClone("AP");

    return 0;
}

void PlotRateQC(UInt_t night, MSQLServer &serv)
{
    TString query = 
        "LEFT JOIN AnalysisResultsRunLP USING(fNight, fRunID) "
        "WHERE fRunTypeKey=1 AND NOT ISNULL (AnalysisResultsRunLP.fNumEvtsAfterCleaning) AND fNight=";
    query += night;

    TTree *t = serv.GetTree("RunInfo", query);
    if (!t)
        return;

    int save = gErrorIgnoreLevel;
    gErrorIgnoreLevel = kFatal;

    gROOT->SetSelectedPad(0);
    gPad->GetCanvas()->cd(3);

    t->Draw("AnalysisResultsRunLP.fNumEvtsAfterCleaning/AnalysisResultsRunLP.fOnTimeAfterCuts:(RunInfo.fRunStart+RunInfo.fRunStop)/2+9131*24*3600", "", "same");
    TGraph *g = (TGraph*)gPad->GetPrimitive("Graph");
    if (g)
    {
        g->SetName("CleaningRate");
        g->SetMarkerColor(kRed);
        g->SetMarkerStyle(29);//kFullDotMedium);
    }

    t->Draw("AnalysisResultsRunLP.fNumEvtsAfterQualCuts/AnalysisResultsRunLP.fOnTimeAfterCuts:(RunInfo.fRunStart+RunInfo.fRunStop)/2+9131*24*3600", "", "same");
    g = (TGraph*)gPad->GetPrimitive("Graph");
    if (g)
    {
        g->SetName("RateAfterQC");
        g->SetMarkerColor(kBlue);
        g->SetMarkerStyle(29);//kFullDotMedium);
    }

    gErrorIgnoreLevel = save;
}

Int_t PlotSqm(TArrayD **vec, TString fname)
{
    fname += ".SQM_CONTROL_DATA.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    Float_t mag; // magnitude

    if (!file.SetPtrAddress("Time",  &time))
        return -1;
    if (!file.SetPtrAddress("Mag",  &mag))
        return -1;

    //const int marker_style = kFullDotMedium;
    const int marker_style = kDot;

    TGraph g1;
    g1.SetName("SQM");


    bool found_first_time = false;
    while (file.GetNextRow())
        if (Contains(vec, time))
        {
            g1.SetPoint(g1.GetN(), time*24*3600, mag);
        }


    g1.SetMinimum(19.0);
    g1.SetMaximum(21.5);
    g1.SetMarkerColor(kBlack);
    g1.SetMarkerStyle(marker_style);
    g1.GetXaxis()->SetTimeDisplay(true);
    g1.GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
    g1.GetXaxis()->SetLabelSize(0.12);
    g1.GetYaxis()->SetLabelSize(0.1);
    g1.GetYaxis()->SetTitle("SQM [mag]");
    g1.GetYaxis()->SetTitleOffset(0.2);
    g1.GetYaxis()->SetTitleSize(0.1);
    g1.DrawClone("AP");

    return 0;
}

Int_t PlotSqmLinear(TArrayD **vec, TString fname)
{
    fname += ".SQM_CONTROL_DATA.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    Float_t mag; // magnitude

    if (!file.SetPtrAddress("Time",  &time))
        return -1;
    if (!file.SetPtrAddress("Mag",  &mag))
        return -1;

    //const int marker_style = kFullDotMedium;
    const int marker_style = kDot;

    TGraph g1;
    g1.SetName("SQM");


    bool found_first_time = false;
    while (file.GetNextRow())
        if (Contains(vec, time))
        {
            Double_t mag_double = 4.4 * pow(10, 20) * pow( 10, mag*(-0.4));
            g1.SetPoint(g1.GetN(), time*24*3600, mag_double);
        }


    g1.SetMinimum(1.e12);
    g1.SetMaximum(5.e12);
    g1.SetMarkerColor(kBlack);
    g1.SetMarkerStyle(marker_style);
    g1.GetXaxis()->SetTimeDisplay(true);
    g1.GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
    g1.GetXaxis()->SetLabelSize(0.12);
    g1.GetYaxis()->SetLabelSize(0.1);
    g1.GetYaxis()->SetTitle("SQM lin [phot./(s sr m^2)]");
    g1.GetYaxis()->SetTitleOffset(0.2);
    g1.GetYaxis()->SetTitleSize(0.1);
    g1.DrawClone("AP");

    return 0;
}

Int_t PlotHumidity(TArrayD **vec, TString fname)
{
    fname += ".FSC_CONTROL_HUMIDITY.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    Float_t H[4];

    if (!file.SetPtrAddress("Time",  &time))
        return -1;
    if (!file.SetPtrAddress("H",  H))
        return -1;

    //const int marker_style = kFullDotMedium;
    const int marker_style = kDot;

    TGraph g1;
    TGraph g2;
    TGraph g3;
    TGraph g4;
    //TGraph g5;
    g1.SetName("H0");
    g2.SetName("H1");
    g3.SetName("H2");
    g4.SetName("H3");
    //g5.SetName("PFmini");


    Double_t first_time, last_time;
    bool found_first_time = false;
    while (file.GetNextRow())
        if (Contains(vec, time))
        {
            if (!found_first_time){
              first_time = time*24*3600;
              found_first_time = true;
            }            
            g1.SetPoint(g1.GetN(), time*24*3600, H[0]);
            g2.SetPoint(g2.GetN(), time*24*3600, H[1]);
            g3.SetPoint(g3.GetN(), time*24*3600, H[2]);
            g4.SetPoint(g4.GetN(), time*24*3600, H[3]);
            //g5.SetPoint(g5.GetN(), time*24*3600, I[319]);
            last_time = time*24*3600;
        }


    g1.SetMinimum(10);
    g1.SetMaximum(80);
    g1.SetMarkerColor(kAzure);
    g1.SetMarkerStyle(marker_style);
    g1.GetXaxis()->SetTimeDisplay(true);
    g1.GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
    g1.GetXaxis()->SetLabelSize(0.12);
    g1.GetYaxis()->SetLabelSize(0.1);
    g1.GetYaxis()->SetTitle("HUMITIDY");
    g1.GetYaxis()->SetTitleOffset(0.2);
    g1.GetYaxis()->SetTitleSize(0.1);
    g1.DrawClone("AP");

    g2.SetMarkerColor(kAzure+1);
    g2.SetMarkerStyle(marker_style);
    g2.DrawClone("P");
    
    g3.SetMarkerColor(kAzure+3);
    g3.SetMarkerStyle(marker_style);
    g3.DrawClone("P");
    
    g4.SetMarkerColor(kAzure+6);
    g4.SetMarkerStyle(marker_style);
    g4.DrawClone("P");
    
    //g5.SetMarkerColor(kAzure+1);
    //g5.SetMarkerStyle(kFullDotMedium);
    //g5.DrawClone("P");
    
    g1.DrawClone("P");

    TLine l1(first_time-600, 40, last_time+600, 40);
    l1.SetLineColor(kOrange);
    l1.DrawClone();
    TText t1(first_time-600, 41, "Please, note in logbook");
    t1.SetTextSize(0.1);
    t1.DrawClone();

    
    TLine l2(first_time-600, 55, last_time+600, 55);
    l2.SetLineColor(kRed);
    l2.DrawClone();
    TText t2(first_time-600, 56, "Please, report to fact-online");
    t2.SetTextSize(0.1);
    t2.DrawClone();


    return 0;
}

Int_t PlotHumidity2(TArrayD **vec, TString fname)
{
    fname += ".PFMINI_CONTROL_DATA.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    Float_t H;

    if (!file.SetPtrAddress("Time",  &time))
        return -1;
    if (!file.SetPtrAddress("Humidity", &H))
        return -1;

    const int marker_style = kFullDotMedium;
    //const int marker_style = kDot;

    TGraph g1;
    g1.SetName("PFmini");


    while (file.GetNextRow())
        if (Contains(vec, time))
        {
            g1.SetPoint(g1.GetN(), time*24*3600, H);
        }
    
    g1.SetMarkerStyle(marker_style);
    g1.SetMarkerColor(kGreen);
    g1.DrawClone("P");
    return 0;
}

Int_t PlotPointing(TArrayD **vec, TString fname)
{
    fname += ".DRIVE_CONTROL_POINTING_POSITION.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    Double_t zd;

    if (!file.SetPtrAddress("Time",  &time))
        return -1;
    if (!file.SetPtrAddress("Zd", &zd))
        return -1;

    TGraph g;
    g.SetName("Zd");

    while (file.GetNextRow())
        if (Contains(vec, time))
            g.SetPoint(g.GetN(), time*24*3600, 90-zd);

    g.SetMinimum(1);
    g.SetMaximum(90);
    g.SetMarkerStyle(kFullDotMedium);
    g.GetXaxis()->SetTimeDisplay(true);
    g.GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
    g.GetXaxis()->SetLabelSize(0.12);
    g.GetYaxis()->SetLabelSize(0.1);
    g.GetYaxis()->SetTitle("ELEVATION");
    g.GetYaxis()->SetTitleOffset(0.2);
    g.GetYaxis()->SetTitleSize(0.1);
    g.DrawClone("AP");

    return 0;
}

Int_t PlotTemperature1(TArrayD **vec, TString fname)
{
    fname += ".TEMPERATURE_DATA.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    Float_t  temp;

    if (!file.SetPtrAddress("Time",  &time))
        return -1;
    if (!file.SetPtrAddress("T", &temp))
        return -1;

    TGraph g;
    g.SetName("ContainerTemp");

    while (file.GetNextRow())
        if (Contains(vec, time))
            g.SetPoint(g.GetN(), time*24*3600, temp);

    g.SetMinimum(-5);
    g.SetMaximum(49);
    g.SetMarkerStyle(kFullDotMedium);
    g.SetMarkerColor(kRed);
    g.GetXaxis()->SetTimeDisplay(true);
    g.GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
    g.GetXaxis()->SetLabelSize(0.1);
    g.GetYaxis()->SetLabelSize(0.1);
    g.GetYaxis()->SetTitle("TEMPERATURE");
    g.GetYaxis()->SetTitleOffset(0.2);
    g.GetYaxis()->SetTitleSize(0.1);
    g.DrawClone("AP");

    return 0;
}

Int_t PlotTemperature2(TArrayD **vec, TString fname)
{
    fname += ".MAGIC_WEATHER_DATA.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    Float_t  temp;

    if (!file.SetPtrAddress("Time",  &time))
        return -1;
    if (!file.SetPtrAddress("T", &temp))
        return -1;

    TGraph g;
    g.SetName("OutsideTemp");

    while (file.GetNextRow())
        if (Contains(vec, time))
            g.SetPoint(g.GetN(), time*24*3600, temp);

    g.SetMarkerStyle(kFullDotMedium);
    g.SetMarkerColor(kBlue);
    g.DrawClone("P");

    return 0;
}

Int_t PlotTemperature3(TArrayD **vec, TString fname)
{
    fname += ".FSC_CONTROL_TEMPERATURE.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    Float_t  temp[31];

    if (!file.SetPtrAddress("Time",  &time))
        return -1;
    if (!file.SetPtrAddress("T_sens", temp))
        return -1;

    TGraph g, g1, g2;
    g.SetName("SensorTempAvg");
    g1.SetName("SensorTempMin");
    g2.SetName("SensorTempMax");

    while (file.GetNextRow())
        if (Contains(vec, time))
        {
            float min =  100;
            float max = -100;
            double avg = 0;
            int num = 0;
            for (int i=0; i<31; i++)
                if (temp[i]!=0)
                {
                    avg += temp[i];
                    num++;

                    min = TMath::Min(min, temp[i]);
                    max = TMath::Max(max, temp[i]);
                }

            g.SetPoint(g.GetN(), time*24*3600, avg/num);
            g1.SetPoint(g1.GetN(), time*24*3600, min);
            g2.SetPoint(g2.GetN(), time*24*3600, max);
        }

    g.SetMarkerStyle(kFullDotMedium);
    g.DrawClone("P");

    /*
     g1.SetLineWidth(1);
     g1.DrawClone("L");

     g2.SetLineWidth(1);
     g2.DrawClone("L");
    */
    return 0;
}

Int_t PlotTemperature4(TArrayD **vec, TString fname)
{
    fname += ".FAD_CONTROL_TEMPERATURE.fits";

    fits file(fname.Data());
    if (!file)
    {
        cerr << fname << ": " << gSystem->GetError() << endl;
        return -2;
    }

    //cout << fname << endl;

    Double_t time;
    Float_t  temp[160];

    if (!file.SetPtrAddress("Time",  &time))
        return -1;
//    if (!file.SetPtrAddress("Data1", temp) &&
//        !file.SetPtrAddress("temp", temp))
    if (!file.SetPtrAddress("temp", temp))
        return -1;

    Int_t num = file.GetN("temp")==0 ? file.GetN("Data1") : file.GetN("temp");
    Int_t beg = num==82 ? 2 : 0;

    TGraphErrors g1;
    TGraph g2,g3;

    g1.SetName("FadTempAvg");
    g2.SetName("FadTempMin");
    g3.SetName("FadTempMax");

    while (file.GetNextRow())
        if (Contains(vec, time))
        {
            double avg = 0;
            double rms = 0;
            float min =  100;
            float max = -100;
            for (int i=beg; i<num; i++)
            {
                avg += temp[i];
                rms += temp[i]*temp[i];

                min = TMath::Min(min, temp[i]);
                max = TMath::Max(max, temp[i]);
            }

            avg /= num-beg;
            rms /= num-beg;

            g1.SetPoint(g1.GetN(), time*24*3600, avg);
            g1.SetPointError(g1.GetN()-1, 0, sqrt(rms-avg*avg));

            g2.SetPoint(g2.GetN(), time*24*3600, min);
            g3.SetPoint(g3.GetN(), time*24*3600, max);
        }

    g1.SetLineColor(kGreen);
    g1.DrawClone("[]");

    g2.SetLineColor(kGreen);
    g2.SetLineWidth(1);
    g2.DrawClone("L");

    g3.SetLineColor(kGreen);
    g3.SetLineWidth(1);
    g3.DrawClone("L");

    return 0;
}

int quality(UInt_t y=0, UInt_t m=0, UInt_t d=0, const char *outpath="quality")
{
    // To get correct dates in the histogram you have to add
    // the MJDREF offset (should be 40587) and 9131.

    if (y==0)
    {
        UInt_t nt = MTime(MTime(-1).GetMjd()-1.5).GetNightAsInt();
        y =  nt/10000;
        m = (nt/100)%100;
        d =  nt%100;

        cout << y << "/" << m << "/" << d << endl;
    }

    TString fname=Form("/fact/aux/%04d/%02d/%02d/%04d%02d%02d", y, m, d, y, m, d);

    UInt_t night = MTime(y, m, d, 0).GetNightAsInt();

    MSQLMagic serv("sql.rc");
    Bool_t con = serv.IsConnected();

    cout << "quality" << endl;
    cout << "-------" << endl;
    cout << endl;
    if (con)
    {
        cout << "Connected to " << serv.GetName() << endl;
        cout << endl;
    }
    cout << "Night: " << Form("%04d-%02d-%02d", y, m, d) << endl;
    cout << endl;

    TArrayD run, beg, end;

    TArrayD *runs[3] = { &run, &beg, &end };

    if (con)
    {
        TString query;
        query += "SELECT fRunID, fRunStart, fRunStop FROM RunInfo";
        query += " WHERE fNight=";
        query += night;
        query += " AND fRunTypeKey=1 ORDER BY fRunStart";

        TSQLResult *res = serv.Query(query);
        if (!res)
            return 1;

        run.Set(res->GetRowCount());
        beg.Set(res->GetRowCount());
        end.Set(res->GetRowCount());

        Int_t n = 0;

        TSQLRow *row = 0;
        while ((row=res->Next()))
        {
            run[n] = atoi((*row)[0]);
            beg[n] = MTime((*row)[1]).GetMjd()-40587;
            end[n] = MTime((*row)[2]).GetMjd()-40587;
            n++;
            delete row;
        }

        delete res;

        if (n==0)
            cout << "WARNING - No data runs in db, displaying all data." << endl;
        else
            cout << "Num: " << n << "\n" << endl;
    }

    //check if the sqm was already installed on the telescope                                                                                                       
    TCanvas *c = NULL;
    TString datestring = Form("%04d%02d%02d", y, m, d);
    if( datestring.Atoi() > 20140723 ) {
      TCanvas *c = new TCanvas("quality", Form("Quality %04d/%02d/%02d", y, m, d), 1280, 1280);
      c->Divide(1, 8, 1e-5, 1e-5);
    }else{
      TCanvas *c = new TCanvas("quality", Form("Quality %04d/%02d/%02d", y, m, d), 1280, 1120);
      c->Divide(1, 7, 1e-5, 1e-5);
    }

    gROOT->SetSelectedPad(0);

    c->cd(1);
    gPad->SetGrid();
    gPad->SetTopMargin(0);
    gPad->SetRightMargin(0.001);
    gPad->SetLeftMargin(0.04);
    gPad->SetBottomMargin(0);
    cout << PlotThresholds(runs, fname) << endl;

    gROOT->SetSelectedPad(0);
    c->cd(2);
    gPad->SetGrid();
    gPad->SetTopMargin(0);
    gPad->SetRightMargin(0.001);
    gPad->SetLeftMargin(0.04);
    gPad->SetBottomMargin(0);
    cout << PlotCurrent(runs, fname) << endl;

    gROOT->SetSelectedPad(0);
    c->cd(3);
    gPad->SetGrid();
    gPad->SetTopMargin(0);
    gPad->SetBottomMargin(0);
    gPad->SetRightMargin(0.001);
    gPad->SetLeftMargin(0.04);
    cout << PlotRate(runs, fname) << endl;
    cout << PlotRateQC(night, serv) << endl;

    gROOT->SetSelectedPad(0);
    c->cd(5);
    gPad->SetGrid();
    gPad->SetTopMargin(0);
    gPad->SetBottomMargin(0);
    gPad->SetRightMargin(0.001);
    gPad->SetLeftMargin(0.04);
    cout << PlotPointing(runs, fname) << endl;

    gROOT->SetSelectedPad(0);
    c->cd(6);
    gPad->SetGrid();
    gPad->SetTopMargin(0);
    gPad->SetRightMargin(0.001);
    gPad->SetLeftMargin(0.04);
    gPad->SetBottomMargin(0);
    cout << PlotTemperature1(runs, fname) << endl;
    cout << PlotTemperature2(runs, fname) << endl;
    cout << PlotTemperature3(runs, fname) << endl;
    cout << PlotTemperature4(runs, fname) << endl;

    gROOT->SetSelectedPad(0);
    c->cd(7);
    gPad->SetGrid();
    gPad->SetTopMargin(0);
    gPad->SetBottomMargin(0);
    gPad->SetRightMargin(0.001);
    gPad->SetLeftMargin(0.04);
    cout << PlotHumidity(runs, fname) << endl;
    cout << PlotHumidity2(runs, fname) << endl;

    //check if the sqm was already installed
    if( datestring.Atoi() > 20140723 ) {
      gROOT->SetSelectedPad(0);
      c->cd(7);
      gPad->SetGrid();
      gPad->SetTopMargin(0);
      gPad->SetBottomMargin(0);
      gPad->SetRightMargin(0.001);
      gPad->SetLeftMargin(0.04);
      cout << PlotHumidity(runs, fname) << endl;
      cout << PlotHumidity2(runs, fname) << endl;

      gROOT->SetSelectedPad(0);
      c->cd(8);
      gPad->SetGrid();
      gPad->SetTopMargin(0);
      gPad->SetRightMargin(0.001);
      gPad->SetLeftMargin(0.04);
      cout << PlotSqm(runs, fname) << endl;
    }else{

      gROOT->SetSelectedPad(0);
      c->cd(7);
      gPad->SetGrid();
      gPad->SetTopMargin(0);
      gPad->SetRightMargin(0.001);
      gPad->SetLeftMargin(0.04);
      cout << PlotHumidity(runs, fname) << endl;
      cout << PlotHumidity2(runs, fname) << endl;
    }

    c->SaveAs(Form("%s/%04d%02d%02d.png", outpath, y, m, d));

    return 0;
}
