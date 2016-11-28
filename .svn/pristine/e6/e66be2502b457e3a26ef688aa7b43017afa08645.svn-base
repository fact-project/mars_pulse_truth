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
!   Author(s): Thomas Bretz, 5/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// pointing.C
// ==========
//
// This macro is a demonstartion how check plots for a subsystem
// (here the drive) can be made using Mars.
//
// In this case a merpped (root-) cc-report file is read in. The data
// of the Drive branch is extracted using MReadReports and the
// Stream-Id feature of MTaskList (second argument in AddToList).
//
// The output are plots showing (hopefully) the peformance of the system.
//
/////////////////////////////////////////////////////////////////////////////

/*
 class MGraph : public TGraph
{
public:
    MGraph() : TGraph() {}
    MGraph(Int_t n) : TGraph(n) {}
    MGraph(Int_t n, const Int_t *x, const Int_t *y) : TGraph(n, x, y) {}
    MGraph(Int_t n, const Float_t *x, const Float_t *y) : TGraph(n, x, y) {}
    MGraph(Int_t n, const Double_t *x, const Double_t *y) : TGraph(n, x, y) {}
    MGraph(const TVector  &vx, const TVector  &vy) : TGraph(vx, vy) {}
    MGraph(const TVectorD &vx, const TVectorD &vy) : TGraph(vx, vy) {}
    MGraph(const TH1 *h) : TGraph(h) {}
    MGraph(const TF1 *f, Option_t *option="") : TGraph(f, option) {}
    MGraph(const char *filename, const char *format="%lg %lg", Option_t *option="") : TGraph(filename, format, option) {}

    void Paint(Option_t *o="")
    {
        if (!strstr(option,"POL") && !strstr(option,"pol"))
        {
            TGraph::Paint(o);
            return;
        }

        //gPad->Range(-1.15, -1, 1.15, 1);

        //gPad->Modified();
        //gPad->Update();

        TView *view = gPad->GetView();
        if (!view)
        {
            cout << "No View!" << endl;
            return;
        }

        TGraph gr;

        Double_t *zd=g1->GetY();
        Double_t *az=g2->GetY();

        TMarker m;
        m.SetMarkerStyle(kFullDotMedium);
        m.SetMarkerColor(kRed);

        for (int i=0; i<fN; i++)
        {
            const Double_t x = fX[i]/90;
            const Double_t y = (fY[i]/180+1)*TMath::Pi();

            Double_t r0[3] = { y*cos(x), y*sin(x), 0};
            Double_t r1[3];

            //gr.SetPoint(gr.GetN(), r0[0], r0[1]);

            view->WCtoNDC(x, y);

            m->PaintMarker(r1[0], r1[1]);
        }
    }
};*/

void pointing()
{
    MStatusDisplay *d = new MStatusDisplay;
    d->SetLogStream(&gLog, kTRUE);

    //
    // Create a empty Parameter List and an empty Task List
    // The tasklist is identified in the eventloop by its name
    //
    MParList  plist;

    MTaskList tlist;
    plist.AddToList(&tlist);

    //
    // Now setup the tasks and tasklist:
    // ---------------------------------
    //

    // First Task: Read file with image parameters
    // (created with the star.C macro)
    MReadReports read;
    read.AddTree("Drive");
    read.AddFile("data/2004_01_26_report.root");
    read.AddToBranchList("MReportDrive.*");

    MContinue tracking("MReportDrive.fState<3.5");

    // Create a task which fills one histogram with the data
    //MHVsTime h0("MReportDrive.fMjd");
    MHVsTime h1("MReportDrive.fNominalZd");
    MHVsTime h2("MReportDrive.fNominalAz");
    MHVsTime h3("MReportDrive.fState");
    MHVsTime h7("MReportDrive.GetAbsError*60");
    //h0.SetName("Mjd");
    h1.SetName("Zd");
    h2.SetName("Az");
    h3.SetName("State");
    h7.SetName("ControlDeviation");

    MH3 h4("MReportDrive.GetAbsError*60");
    h4.SetName("DeltaH");

    MH3 h5("MReportDrive.fNominalZd","MReportDrive.GetAbsError*60");
    h5.SetName("DeltaHvsZd");

    MBinning bins("BinningDeltaH");
    bins.SetEdges(18, 0, 3.6);

    MBinning bins2("BinningDeltaHvsZdX");
    MBinning bins3("BinningDeltaHvsZdY");
    bins2.SetEdges(90, 0, 90);
    bins3.SetEdges(18, 0, 3.6);

    plist.AddToList(&bins);
    plist.AddToList(&bins2);
    plist.AddToList(&bins3);

    //MFillH fill0(&h0, "MTimeDrive");
    MFillH fill1(&h1, "MTimeDrive");
    MFillH fill2(&h2, "MTimeDrive");
    MFillH fill3(&h3, "MTimeDrive");
    MFillH fill7(&h7, "MTimeDrive");
    MFillH fill4(&h4);
    MFillH fill5(&h5);

    //fill0.SetBit(MFillH::kDoNotDisplay);
    fill1.SetBit(MFillH::kDoNotDisplay);
    fill2.SetBit(MFillH::kDoNotDisplay);
    fill3.SetBit(MFillH::kDoNotDisplay);
    fill4.SetBit(MFillH::kDoNotDisplay);
    fill5.SetBit(MFillH::kDoNotDisplay);
    fill7.SetBit(MFillH::kDoNotDisplay);

    // -----------------------------------------------

    //
    // Setup Task list
    //
    tlist.AddToList(&read);
    tlist.AddToList(&fill3);
    tlist.AddToList(&tracking);
    //tlist.AddToList(&fill0);
    tlist.AddToList(&fill1);
    tlist.AddToList(&fill2);
    tlist.AddToList(&fill4);
    tlist.AddToList(&fill5);
    tlist.AddToList(&fill7);

    gStyle->SetOptStat(0);

    //
    // Create and setup the eventloop
    //
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    //
    // Execute your analysis
    //
    evtloop.SetDisplay(d);
    if (!evtloop.Eventloop())
        return;

    tlist.PrintStatistics();


    TGraph *g1 = h1.GetGraph();
    TGraph *g2 = h2.GetGraph();

    TCanvas &c = d->AddTab("Sky");
    c.cd();

    //Skyplot
    TPad *p = new TPad("", "",0,0.32,0.5,1);
    p->Draw();
    p->cd();

    gPad->SetTheta(-90);
    gPad->SetPhi(90);
    gPad->SetBorderMode(0);
    gStyle->SetOptStat(0);

    TH2F h("pol", "Telescope Tracking Positions on the Sky", 16, 0, 1, 9, 0, 1);
    h.DrawClone("surf1pol");

    gPad->Modified();
    gPad->Update();

    TView *view = gPad->GetView();
    if (!view)
    {
        cout << "No View!" << endl;
        return;
    }

    Double_t *zd=g1->GetY();
    Double_t *az=g2->GetY();

    Double_t old[2] = {0,0};

    for (int i=0; i<g1->GetN(); i++)
    {
        az[i] += 180;
        az[i] *= TMath::Pi()/180;

        Double_t x[3] = { zd[i]*cos(az[i])/90, zd[i]*sin(az[i])/90, 0};
        Double_t y[3];

        view->WCtoNDC(x, y);

        if (old[0]!=0 && old[1]!=1)
        {
            TLine *l = new TLine(y[0], y[1], old[0], old[1]);
            l->SetLineColor(kBlue);
            l->Draw();
        }

        TMarker *m = new TMarker(y[0], y[1], kFullDotMedium);
        m->SetMarkerColor(i==g1->GetN()-1 ? kGreen : kRed);
        m->Draw();

        old[0] = y[0];
        old[1] = y[1];
    }

    c.cd();


/*
    //MJD
    p = new TPad("", "", 0.6, 0.66, 1, 1);
    p->Draw();
    p->cd();

    MHVsTime *hvt=h0.DrawClone("nonew");
    hvt->GetGraph()->SetMarkerStyle(kFullDotSmall);
    hvt->GetGraph()->GetHistogram()->SetXTitle("Time");
    hvt->GetGraph()->GetHistogram()->SetYTitle("");
    hvt->GetGraph()->GetHistogram()->SetTitle("MJD vs. Time");
    hvt->GetGraph()->GetHistogram()->SetStats(0);

    c.cd();
*/


    //Histogram of Control Deviation
    p = new TPad("", "", 0, 0, 0.5, 0.32);
    p->Draw();
    p->cd();

    gPad->SetBorderMode(0);
    //number of entries, mean, rms and number of overflows in the statusbox
    gStyle->SetOptStat("emro"); 
    //gStyle->SetStatFormat(".2g");

    MH3 *mh3 = (MH3*)h4.DrawClone("nonew");

    TAxis *axey = mh3->GetHist()->GetYaxis();
    TAxis *axex = mh3->GetHist()->GetXaxis();
    axey->SetLabelSize(0.05);
    axex->SetLabelSize(0.05);
    axex->SetTitleSize(0.05);
    axex->SetTitleOffset(0.85);

    mh3->GetHist()->SetXTitle("\\Delta [arcmin]");
    mh3->GetHist()->SetYTitle("");
    mh3->GetHist()->SetTitle("Control deviation of the motors");
    mh3->GetHist()->SetStats(1);

    //insert lines for 0.5, 1 and 2 SE
    TLine ln;
    ln.SetLineColor(kGreen);
    ln.DrawLine(0.5*360*60/16384., 0, 0.5*360*60/16384., h4.GetHist()->GetMaximum());
    ln.SetLineColor(kYellow);
    ln.DrawLine(1.0*360*60/16384., 0, 1.0*360*60/16384., h4.GetHist()->GetMaximum());
    ln.SetLineColor(kRed);
    ln.DrawLine(2.0*360*60/16384., 0, 2.0*360*60/16384., h4.GetHist()->GetMaximum());

    c.cd();


    //Status of the Drive System vs Time
    p = new TPad("", "", 0.5, 0.86, 1, 1);
    p->Draw();
    p->cd();
    gPad->SetBorderMode(0);
    p->SetGridx();

    hvt = (MHVsTime*)h3.DrawClone("nonew");
    hvt->GetGraph()->SetMarkerStyle(kFullDotSmall);

    TH1 *hist = hvt->GetGraph()->GetHistogram();
    TAxis *axey = hist->GetYaxis();
    TAxis *axex = hist->GetXaxis();

    hist->SetXTitle("Time");
    hist->SetYTitle("");
    hist->SetTitle("");//Drive Status vs. Time");
    hist->SetStats(0);
    hist->SetMinimum(-0.5);
    hist->SetMaximum(4.5);
    axey->Set(5, -0.5, 4.5);
    axey->SetBinLabel(axey->FindFixBin(0), "Error");
    axey->SetBinLabel(axey->FindFixBin(1), "Stopped");
    axey->SetBinLabel(axey->FindFixBin(3), "Moving");
    axey->SetBinLabel(axey->FindFixBin(4), "Tracking");
    axey->SetLabelSize(0.15);
    axex->SetLabelSize(0.08);
    axex->SetTitleSize(0.09);
    axex->SetTitleOffset(0.45);

    c.cd();


    //Zd vs Time
    p = new TPad("", "", 0.5, 0.59, 1, 0.86);
    p->Draw();
    p->cd();
    gPad->SetBorderMode(0);
    p->SetGridx();

    hvt = (MHVsTime*)h1.DrawClone("nonew");
    hvt->GetGraph()->SetMarkerStyle(kFullDotSmall);

    TH1 *hist = hvt->GetGraph()->GetHistogram();
    if (hvt->GetGraph()->GetN())
    {
        hist->SetXTitle("Time");
        hist->SetYTitle("Zd [\\circ]");
        hist->SetTitle("Zd vs. Time");
        hist->SetStats(0);
    }

    TAxis *axey = hist->GetYaxis();
    TAxis *axex = hist->GetXaxis();
    axey->SetLabelSize(0.05);
    axey->SetTitleSize(0.06);
    axey->SetTitleOffset(0.6);
    axex->SetLabelSize(0.05);
    axex->SetTitleSize(0.06);
    axex->SetTitleOffset(0.85);

    c.cd();


    //Controll Deviation vs Time
    p = new TPad("", "", 0.5, 0.32, 1, 0.59);
    p->Draw();
    p->cd();
    gPad->SetBorderMode(0);
    p->SetGridx();

    hvt = (MHVsTime*)h7.DrawClone("nonew L");
    TH1 *hist = hvt->GetGraph()->GetHistogram();
    hist->SetAxisRange(-0.5, 10.5, "Y");
    hist->SetXTitle("Time");
    hist->SetYTitle("\\Delta [arcmin]");
    hist->SetTitle("Control Deviation vs. Time");

    TAxis *axey = hist->GetYaxis();
    TAxis *axex = hist->GetXaxis();
    axey->SetLabelSize(0.05);
    axey->SetTitleSize(0.06);
    axey->SetTitleOffset(0.5);
    axex->SetLabelSize(0.05);
    axex->SetTitleSize(0.06);
    axex->SetTitleOffset(0.8);

    //insert lines for 0.5, 1 and 2 SE
    TLine ln;
    ln.SetLineColor(kGreen);
    ln.DrawLine(hist->GetXaxis()->GetXmin(), 0.5*360*60/16384., hist->GetXaxis()->GetXmax(), 0.5*360*60/16384.);
    ln.SetLineColor(kYellow);
    ln.DrawLine(hist->GetXaxis()->GetXmin(), 1.0*360*60/16384., hist->GetXaxis()->GetXmax(), 1.0*360*60/16384.);
    ln.SetLineColor(kRed);
    ln.DrawLine(hist->GetXaxis()->GetXmin(), 2.0*360*60/16384., hist->GetXaxis()->GetXmax(), 2.0*360*60/16384.);

    c.cd();


    //Controll Deviation vs Zd
    p = new TPad("", "", 0.5, 0, 1, 0.32);
    p->Draw();
    p->cd();

    gPad->SetBorderMode(0);
    gStyle->SetOptStat(1110);
    gStyle->SetStatFormat(".2g");

    mh3 = (MH3*)h5.DrawClone("nonew");

    TAxis *axey = mh3->GetHist()->GetYaxis();
    TAxis *axex = mh3->GetHist()->GetXaxis();
    axey->SetLabelSize(0.05);
    axey->SetTitleSize(0.05);
    axey->SetTitleOffset(0.7);
    axex->SetLabelSize(0.05);
    axex->SetTitleSize(0.05);
    axex->SetTitleOffset(0.85);

    mh3->GetHist()->SetYTitle("Zd [\\circ]");
    mh3->GetHist()->SetXTitle("\\Delta [arcmin]");
    mh3->GetHist()->SetTitle("Control deviation of the motors");
    mh3->GetHist()->SetStats(1);
    mh3->GetHist()->Draw("box");

    //insert lines for 0.5, 1 and 2 SE
    TLine ln;
    ln.SetLineColor(kGreen);
    ln.DrawLine(0, 0.5*360*60/16384., 90, 0.5*360*60/16384.);
    ln.SetLineColor(kYellow);
    ln.DrawLine(0, 1.0*360*60/16384., 90, 1.0*360*60/16384.);
    ln.SetLineColor(kRed);
    ln.DrawLine(0, 2.0*360*60/16384., 90, 2.0*360*60/16384.);


    //d.SaveAsPS(2, "rep_files/CC_2003_11_27_22_31_59-new.ps");
    //d.SaveAsRoot(2, "~/dev/Mars/rep_files/CC_2003_11_27_22_31_59-new.root");

}
