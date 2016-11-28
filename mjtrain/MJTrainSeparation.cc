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
!   Author(s): Thomas Bretz 11/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MJTrainSeparation
//
////////////////////////////////////////////////////////////////////////////
#include "MJTrainSeparation.h"

#include <TF1.h>
#include <TH2.h>
#include <TChain.h>
#include <TGraph.h>
#include <TMarker.h>
#include <TCanvas.h>
#include <TStopwatch.h>
#include <TVirtualPad.h>

#include "MHMatrix.h"

#include "MLog.h"
#include "MLogManip.h"

// tools
#include "MMath.h"
#include "MDataSet.h"
#include "MTFillMatrix.h"
#include "MStatusDisplay.h"

// eventloop
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

// tasks
#include "MReadMarsFile.h"
#include "MReadReports.h"
#include "MContinue.h"
#include "MFillH.h"
#include "MSrcPosRndm.h"
#include "MHillasCalc.h"
#include "MRanForestCalc.h"
#include "MParameterCalc.h"

// container
#include "MMcEvt.hxx"
#include "MParameters.h"

// histograms
#include "MBinning.h"
#include "MH3.h"
#include "MHHadronness.h"

// filter
#include "MF.h"
#include "MFEventSelector.h"
#include "MFilterList.h"

// wobble
#include "MPointingPos.h"
#include "MPointingDevCalc.h"
#include "../mastro/MObservatory.h"
#include "MSrcPosCalc.h"
#include "MSrcPosCorrect.h"
#include "../mimage/MHillasCalc.h"


ClassImp(MJTrainSeparation);

using namespace std;

void MJTrainSeparation::DisplayResult(MH3 &h31, MH3 &h32, Float_t ontime)
{
    TH2D &g = (TH2D&)h32.GetHist();
    TH2D &h = (TH2D&)h31.GetHist();

    h.SetMarkerColor(kRed);
    g.SetMarkerColor(kBlue);

    TH2D res1(g);
    TH2D res2(g);

    h.SetTitle("Hadronness-Distribution vs. Size");
    res1.SetTitle("Significance Li/Ma");
    res1.SetXTitle("Size [phe]");
    res1.SetYTitle("Hadronness");
    res2.SetTitle("Significance-Distribution");
    res2.SetXTitle("Size-Cut [phe]");
    res2.SetYTitle("Hadronness-Cut");
    res1.SetContour(50);
    res2.SetContour(50);

    const Int_t nx = h.GetNbinsX();
    const Int_t ny = h.GetNbinsY();

    gROOT->SetSelectedPad(NULL);


    Double_t Stot = 0;
    Double_t Btot = 0;

    Double_t max2 = -1;

    TGraph gr1;
    TGraph gr2;
    for (int x=nx-1; x>=0; x--)
    {
        TH1 *hx = h.ProjectionY("H_py", x+1, x+1);
        TH1 *gx = g.ProjectionY("G_py", x+1, x+1);

        Double_t S = 0;
        Double_t B = 0;

        Double_t max1 = -1;
        Int_t maxy1 = 0;
        Int_t maxy2 = 0;
        for (int y=ny-1; y>=0; y--)
        {
            const Float_t s = gx->Integral(1, y+1);
            const Float_t b = hx->Integral(1, y+1);
            const Float_t sig1 = MMath::SignificanceLiMa(s+b, b);
            const Float_t sig2 = MMath::SignificanceLiMa(s+Stot+b+Btot, b+Btot)*TMath::Log10(s+Stot+1);
            if (sig1>max1)
            {
                maxy1 = y;
                max1 = sig1;
            }
            if (sig2>max2)
            {
                maxy2 = y;
                max2 = sig2;

                S=s;
                B=b;
            }

            res1.SetBinContent(x+1, y+1, sig1);
        }

        Stot += S;
        Btot += B;

        gr1.SetPoint(x, h.GetXaxis()->GetBinCenter(x+1), h.GetYaxis()->GetBinCenter(maxy1+1));
        gr2.SetPoint(x, h.GetXaxis()->GetBinCenter(x+1), h.GetYaxis()->GetBinCenter(maxy2+1));

        delete hx;
        delete gx;
    }

    //cout << "--> " << MMath::SignificanceLiMa(Stot+Btot, Btot) << " ";
    //cout << Stot << " " << Btot << endl;


    Int_t mx1=0;
    Int_t my1=0;
    Int_t mx2=0;
    Int_t my2=0;
    Int_t s1=0;
    Int_t b1=0;
    Int_t s2=0;
    Int_t b2=0;
    Double_t sig1=-1;
    Double_t sig2=-1;
    for (int x=0; x<nx; x++)
    {
        TH1 *hx = h.ProjectionY("H_py", x+1);
        TH1 *gx = g.ProjectionY("G_py", x+1);
        for (int y=0; y<ny; y++)
        {
            const Float_t s = gx->Integral(1, y+1);
            const Float_t b = hx->Integral(1, y+1);
            const Float_t sig = MMath::SignificanceLiMa(s+b, b);
            res2.SetBinContent(x+1, y+1, sig);

            // Search for top-rightmost maximum
            if (sig>=sig1)
            {
                mx1=x+1;
                my1=y+1;
                s1 = TMath::Nint(s);
                b1 = TMath::Nint(b);
                sig1=sig;
            }
            if (TMath::Log10(s)*sig>=sig2)
            {
                mx2=x+1;
                my2=y+1;
                s2 = TMath::Nint(s);
                b2 = TMath::Nint(b);
                sig2=TMath::Log10(s)*sig;
            }
        }
        delete hx;
        delete gx;
    }

    TGraph gr3;
    TGraph gr4;
    gr4.SetTitle("Significance Li/Ma vs. Hadronness-cut");

    TH1 *hx = h.ProjectionY("H_py");
    TH1 *gx = g.ProjectionY("G_py");
    for (int y=0; y<ny; y++)
    {
        const Float_t s = gx->Integral(1, y+1);
        const Float_t b = hx->Integral(1, y+1);
        const Float_t sg1 = MMath::SignificanceLiMa(s+b, b);
        const Float_t sg2 = s<1 ? 0 : MMath::SignificanceLiMa(s+b, b)*TMath::Log10(s);

        gr3.SetPoint(y, h.GetYaxis()->GetBinLowEdge(y+2), sg1);
        gr4.SetPoint(y, h.GetYaxis()->GetBinLowEdge(y+2), sg2);
    }
    delete hx;
    delete gx;

    if (fDisplay)
    {
        TCanvas &c = fDisplay->AddTab("OptCut");
        c.SetBorderMode(0);
        c.Divide(2,2);

        c.cd(1);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetLogx();
        gPad->SetGridx();
        gPad->SetGridy();
        h.DrawCopy();
        g.DrawCopy("same");
        gr1.SetMarkerStyle(kFullDotMedium);
        gr1.DrawClone("LP")->SetBit(kCanDelete);
        gr2.SetLineColor(kBlue);
        gr2.SetMarkerStyle(kFullDotMedium);
        gr2.DrawClone("LP")->SetBit(kCanDelete);

        c.cd(3);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        gr4.SetMinimum(0);
        gr4.SetMarkerStyle(kFullDotMedium);
        gr4.DrawClone("ALP")->SetBit(kCanDelete);
        gr3.SetLineColor(kBlue);
        gr3.SetMarkerStyle(kFullDotMedium);
        gr3.DrawClone("LP")->SetBit(kCanDelete);

        c.cd(2);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetLogx();
        gPad->SetGridx();
        gPad->SetGridy();
        gPad->AddExec("color", "gStyle->SetPalette(1, 0);");
        res1.SetMaximum(7);
        res1.DrawCopy("colz");

        c.cd(4);
        gPad->SetBorderMode(0);
        gPad->SetFrameBorderMode(0);
        gPad->SetLogx();
        gPad->SetGridx();
        gPad->SetGridy();
        gPad->AddExec("color", "gStyle->SetPalette(1, 0);");
        res2.SetMaximum(res2.GetMaximum()*1.05);
        res2.DrawCopy("colz");

        //    Int_t mx, my, mz;
        //    res2.GetMaximumBin(mx, my, mz);

        TMarker m;
        m.SetMarkerStyle(kStar);
        m.DrawMarker(res2.GetXaxis()->GetBinCenter(mx1), res2.GetYaxis()->GetBinCenter(my1));
        m.SetMarkerStyle(kPlus);
        m.DrawMarker(res2.GetXaxis()->GetBinCenter(mx2), res2.GetYaxis()->GetBinCenter(my2));
    }

    if (ontime>0)
        *fLog << all << "Observation Time:     " << TMath::Nint(ontime/60) << "min" << endl;
    *fLog << "Maximum Significance: " << Form("%.1f", sig1);
    if (ontime>0)
        *fLog << Form(" [%.1f/sqrt(h)]", sig1/TMath::Sqrt(ontime/3600));
    *fLog << endl;

    *fLog << "Significance:         S=" << Form("%.1f", sig1) << " E=" << s1 << " B=" << b1 << " h<";
    *fLog << Form("%.2f", res2.GetYaxis()->GetBinCenter(my1)) << " s>";
    *fLog << Form("%3d", TMath::Nint(res2.GetXaxis()->GetBinCenter(mx1))) << endl;
    *fLog << "Significance*LogE:    S=" << Form("%.1f", sig2/TMath::Log10(s2)) << " E=" << s2 << " B=" << b2 << " h<";
    *fLog << Form("%.2f", res2.GetYaxis()->GetBinCenter(my2)) << " s>";
    *fLog << Form("%3d", TMath::Nint(res2.GetXaxis()->GetBinCenter(mx2))) << endl;
    *fLog << endl;
}

/*
Bool_t  MJSpectrum::InitWeighting(const MDataSet &set, MMcSpectrumWeight &w) const
{
    fLog->Separator("Initialize energy weighting");

    if (!CheckEnv(w))
    {
        *fLog << err << "ERROR - Reading resources for MMcSpectrumWeight failed." << endl;
        return kFALSE;
    }

    TChain chain("RunHeaders");
    set.AddFilesOn(chain);

    MMcCorsikaRunHeader *h=0;
    chain.SetBranchAddress("MMcCorsikaRunHeader.", &h);
    chain.GetEntry(1);

    if (!h)
    {
        *fLog << err << "ERROR - Couldn't read MMcCorsikaRunHeader from DataSet." << endl;
        return kFALSE;
    }

    if (!w.Set(*h))
    {
        *fLog << err << "ERROR - Initializing MMcSpectrumWeight failed." << endl;
        return kFALSE;
    }

    w.Print();
    return kTRUE;
}

Bool_t MJSpectrum::ReadOrigMCDistribution(const MDataSet &set, TH1 &h, MMcSpectrumWeight &weight) const
{
    // Some debug output
    fLog->Separator("Compiling original MC distribution");

    weight.SetNameMcEvt("MMcEvtBasic");
    const TString w(weight.GetFormulaWeights());
    weight.SetNameMcEvt();

    *fLog << inf << "Using weights: " << w << endl;
    *fLog << "Please stand by, this may take a while..." << flush;

    if (fDisplay)
        fDisplay->SetStatusLine1("Compiling MC distribution...");

    // Create chain
    TChain chain("OriginalMC");
    set.AddFilesOn(chain);

    // Prepare histogram
    h.Reset();

    // Fill histogram from chain
    h.SetDirectory(gROOT);
    if (h.InheritsFrom(TH2::Class()))
    {
        h.SetNameTitle("ThetaEMC", "Event-Distribution vs Theta and Energy for MC (produced)");
        h.SetXTitle("\\Theta [\\circ]");
        h.SetYTitle("E [GeV]");
        h.SetZTitle("Counts");
        chain.Draw("MMcEvtBasic.fEnergy:MMcEvtBasic.fTelescopeTheta*TMath::RadToDeg()>>ThetaEMC", w, "goff");
    }
    else
    {
        h.SetNameTitle("ThetaMC", "Event-Distribution vs Theta for MC (produced)");
        h.SetXTitle("\\Theta [\\circ]");
        h.SetYTitle("Counts");
        chain.Draw("MMcEvtBasic.fTelescopeTheta*TMath::RadToDeg()>>ThetaMC", w, "goff");
    }
    h.SetDirectory(0);

    *fLog << "done." << endl;
    if (fDisplay)
        fDisplay->SetStatusLine2("done.");

    if (h.GetEntries()>0)
        return kTRUE;

    *fLog << err << "ERROR - Histogram with original MC distribution empty..." << endl;

    return h.GetEntries()>0;
}
*/

Bool_t MJTrainSeparation::GetEventsProduced(MDataSet &set, Double_t &num, Double_t &min, Double_t &max) const
{
    TChain chain("OriginalMC");
    if (!set.AddFilesOn(chain))
        return kFALSE;

    min = chain.GetMinimum("MMcEvtBasic.fEnergy");
    max = chain.GetMaximum("MMcEvtBasic.fEnergy");

    num = chain.GetEntries();

    if (num<100)
        *fLog << err << "ERROR - Less than 100 entries in OriginalMC-Tree of MC-Train-Data found." << endl;

    return num>=100;
}

Double_t MJTrainSeparation::GetDataRate(MDataSet &set, Double_t &num) const
{
    TChain chain1("Events");
    if (!set.AddFilesOff(chain1))
        return kFALSE;

    num = chain1.GetEntries();
    if (num<100)
    {
        *fLog << err << "ERROR - Less than 100 entries in Events-Tree of Train-Data found." << endl;
        return -1;
    }

    TChain chain("EffectiveOnTime");
    if (!set.AddFilesOff(chain))
        return kFALSE;

    chain.Draw("MEffectiveOnTime.fVal", "MEffectiveOnTime.fVal", "goff");

    TH1 *h = dynamic_cast<TH1*>(gROOT->FindObject("htemp"));
    if (!h)
    {
        *fLog << err << "ERROR - Weird things are happening (htemp not found)!" << endl;
        return -1;
    }

    const Double_t ontime = h->Integral();
    delete h;

    if (ontime<1)
    {
        *fLog << err << "ERROR - Less than 1s of effective observation time found in Train-Data." << endl;
        return -1;
    }

    return num/ontime;
}

Double_t MJTrainSeparation::GetNumMC(MDataSet &set) const
{
    TChain chain1("Events");
    if (!set.AddFilesOn(chain1))
        return kFALSE;

    const Double_t num = chain1.GetEntries();
    if (num<100)
    {
        *fLog << err << "ERROR - Less than 100 entries in Events-Tree of Train-Data found." << endl;
        return -1;
    }

    return num;
}

Float_t MJTrainSeparation::AutoTrain(MDataSet &set, Type_t  typon, Type_t typof, Float_t flux)
{
    Double_t num, min, max;
    if (!GetEventsProduced(set, num, min, max))
        return -1;

    *fLog << inf << "Using build-in radius of 300m to calculate collection area!" << endl;

    // Target spectrum
    TF1 flx("Flux", "[0]/1000*(x/1000)^(-2.6)", min, max);
    flx.SetParameter(0, flux);

    // Number n0 of events this spectrum would produce per s and m^2
    const Double_t n0 = flx.Integral(min, max);    //[#]

    // Area produced in MC
    const Double_t A = TMath::Pi()*300*300;        //[m²]

    // Rate R of events this spectrum would produce per s
    const Double_t R = n0*A;                       //[Hz]

    *fLog << "Source Spectrum: " << flux << " * (E/TeV)^(-2.6) * TeV*m^2*s" << endl;

    *fLog << "Gamma rate from the source inside the MC production area: " << R << "Hz" << endl;

    // Number N of events produced (in trainings sample)
    const Double_t N = num;                        //[#]

    *fLog << "Events produced by MC inside the production area:         " << TMath::Nint(num) << endl;

    // This correponds to an observation time T [s]
    const Double_t T = N/R;                        //[s]

    *fLog << "Total time produced by the Monte Carlo:                   " << T << "s" << endl;

    // With an average data rate after star of
    Double_t data=0;
    const Double_t r = GetDataRate(set, data); //[Hz]
    Double_t ontime = data/r;

    *fLog << "Events measured per second effective on time:             " << r << "Hz" << endl;
    *fLog << "Total effective on time:                                  " << ontime  << "s" << endl;

    const Double_t ratio = T/ontime;
    *fLog << "Ratio of Monte Carlo to data observation time:            " << ratio << endl;

    // 3570.5/43440.2 = 0.082


    // this yields a number of n events to be read for training
    const Double_t n = r*T;                        //[#]

    *fLog << "Events to be read from the data sample:                   " << TMath::Nint(n) << endl;
    *fLog << "Events available in data sample:                          " << data << endl;

    if (r<0)
        return -1;

    Double_t nummc = GetNumMC(set);

    *fLog << "Events available in MC sample:                            " << nummc << endl;

//    *fLog << "MC read probability:                                      " << data/n << endl;

    // more data requested than available => Scale down num MC events
    Double_t on, off;
    if (data<n)
    {
        on  = TMath::Nint(nummc*data/n);
        off = TMath::Nint(data);
        *fLog << warn;
        *fLog << "Not enough data events available... scaling MC to data by " << data/n << endl;
        *fLog << inf;
    }
    else
    {
        on  = TMath::Nint(nummc);
        off = TMath::Nint(n);
    }

    if (fNum[typon]>0 && fNum[typon]<on)
    {
        fNum[typof] = TMath::Nint(off*fNum[typon]/on);
        ontime *= fNum[typon]/on;
        *fLog << warn << "Less MC events requested... scaling data to MC by " << fNum[typon]/on << endl;
    }
    else
    {
        fNum[typon] = TMath::Nint(on);
        fNum[typof] = TMath::Nint(off);
    }

    *fLog << inf;
    *fLog << "Target number of MC events:   " << fNum[typon] << endl;
    *fLog << "Target number of data events: " << fNum[typof] << endl;

    /*
     An event rate dependent selection?
     ----------------------------------
     Total average data rate:      R
     Goal number of events:        N
     Number of data events:        N0
     Rate assigned to single evt:  r

     Selection probability: N/N0 * r/R

     f := N/N0 * r

     MF f("f * MEventRate.fRate < rand");
     */

    return ontime;
}

Bool_t MJTrainSeparation::Train(const char *out)
{
    if (fDisplay)
        fDisplay->SetTitle(out);

    if (!fDataSetTrain.IsValid())
    {
        *fLog << err << "ERROR - DataSet for training invalid!" << endl;
        return kFALSE;
    }
    if (!fDataSetTest.IsValid())
    {
        *fLog << err << "ERROR - DataSet for testing invalid!" << endl;
        return kFALSE;
    }

    if (fDataSetTrain.IsWobbleMode()!=fDataSetTest.IsWobbleMode())
    {
        *fLog << err << "ERROR - Train- and Test-DataSet have different observation modes!" << endl;
        return kFALSE;
    }

    if (!HasWritePermission(out))
        return kFALSE;

        TStopwatch clock;
    clock.Start();

    // ----------------------- Auto Train? ----------------------

    Float_t ontime = -1;
    if (fAutoTrain)
    {
        fLog->Separator("Auto-Training -- Train-Data");
        if (AutoTrain(fDataSetTrain, kTrainOn, kTrainOff, fFluxTrain)<0)
            return kFALSE;
        fLog->Separator("Auto-Training -- Test-Data");
        ontime = AutoTrain(fDataSetTest, kTestOn, kTestOff, fFluxTest);
        if (ontime<0)
            return kFALSE;
    }

    // --------------------- Setup files --------------------
    MReadReports  read1;//("Events");
    MReadMarsFile read2("Events");
    MReadMarsFile read3("Events");
    MReadReports  read4;//("Events");
    //read1.DisableAutoScheme();
    read2.DisableAutoScheme();
    read3.DisableAutoScheme();
    //read4.DisableAutoScheme();

    read1.AddTree("Events", "MTime.", MReadReports::kMaster);
    read4.AddTree("Events", "MTime.", MReadReports::kMaster);
    read1.AddTree("Drive",            MReadReports::kRequired);
    read4.AddTree("Drive",            MReadReports::kRequired);
    read1.AddTree("Starguider",       MReadReports::kRequired);
    read4.AddTree("Starguider",       MReadReports::kRequired);

    // Setup four reading tasks with the on- and off-data of the two datasets
    if (!fDataSetTrain.AddFilesOn(read1))
        return kFALSE;
    const Bool_t setrc1 = fDataSetTrain.IsWobbleMode() ?
        fDataSetTrain.AddFilesOn(read3) : fDataSetTrain.AddFilesOff(read3);
    const Bool_t setrc2 = fDataSetTest.IsWobbleMode() ?
        fDataSetTest.AddFilesOn(read2) : fDataSetTest.AddFilesOff(read2);
    if (!setrc1  || !setrc2)
        return kFALSE;
    if (!fDataSetTest.AddFilesOn(read4))
        return kFALSE;

    // ----------------------- Setup RF Matrix ----------------------
    MHMatrix train("Train");
    train.AddColumns(fRules);
    if (fEnableWeights[kTrainOn] || fEnableWeights[kTrainOff])
        train.AddColumn("MWeight.fVal");
    train.AddColumn("MHadronness.fVal");

    // ----------------------- Fill Matrix RF ----------------------

    // Setup the hadronness container identifying gammas and off-data
    // and setup a container for the weights
    MParameterD had("MHadronness");
    MParameterD wgt("MWeight");

    // Add them to the parameter list
    MParList plistx;
    plistx.AddToList(this); // take care of fDisplay!
    plistx.AddToList(&had);
    plistx.AddToList(&wgt);

    // Setup the tool class to fill the matrix
    MTFillMatrix fill;
    fill.SetLogStream(fLog);
    fill.SetDisplay(fDisplay);
    fill.AddPreCuts(fPreCuts);
    fill.AddPreCuts(fTrainCuts);

    // Set classifier for gammas
    had.SetVal(0);
    wgt.SetVal(1);

    // How to get source position from off- and on-data?
    MPointingPos source("MSourcePos");
    MObservatory     obs;
    MSrcPosCalc      scalc;
    MSrcPosCorrect   scor;
    MHillasCalc      hcalcw;
    MHillasCalc      hcalcw2;
    MPointingDevCalc devcalc;
    scalc.SetMode(MSrcPosCalc::kDefault); // kWobble for off-source
    hcalcw.SetFlags(MHillasCalc::kCalcHillasSrc);
    hcalcw2.SetFlags(MHillasCalc::kCalcHillasSrc);
    hcalcw2.SetNameHillasSrc("MHillasSrcAnti");
    hcalcw2.SetNameSrcPosCam("MSrcPosAnti");
    if (fDataSetTrain.IsWobbleMode())
    {
        // *******************************************************************
        // Possible source position (eg. Wobble Mode)
        if (fDataSetTrain.HasSource())
        {
            if (!fDataSetTrain.GetSourcePos(source))
                return -1;
            *fLog << all;
            source.Print("RaDec");
        }
        else
            *fLog << all << "No source position applied..." << endl;

        // La Palma Magic1
        plistx.AddToList(&obs);
        plistx.AddToList(&source);

        TList tlist2;
        tlist2.Add(&scalc);
        tlist2.Add(&scor);
        tlist2.Add(&hcalcw);
        tlist2.Add(&hcalcw2);

        devcalc.SetStreamId("Starguider");
        tlist2.Add(&devcalc);

        fill.AddPreTasks(tlist2);
        // *******************************************************************
    }

    // Setup the tool class to read the gammas and read them
    fill.SetName("FillGammas");
    fill.SetDestMatrix1(&train, fNum[kTrainOn]);
    fill.SetReader(&read1);
    fill.AddPreTasks(fPreTasksSet[kTrainOn]);
    fill.AddPreTasks(fPreTasks);
    fill.AddPostTasks(fPostTasksSet[kTrainOn]);
    fill.AddPostTasks(fPostTasks);
    if (!fill.Process(plistx))
        return kFALSE;

    // Check the number or read events
    const Int_t numgammastrn = train.GetNumRows();
    if (numgammastrn==0)
    {
        *fLog << err << "ERROR - No gammas available for training... aborting." << endl;
        return kFALSE;
    }

    // Remove possible post tasks
    fill.ClearPreTasks();
    fill.ClearPostTasks();

    // Set classifier for background
    had.SetVal(1);
    wgt.SetVal(1);

    // In case of wobble mode we have to do something special
    MSrcPosRndm srcrndm;
    srcrndm.SetDistOfSource(0.4);

    MHillasCalc hcalc;
    MHillasCalc hcalc2;
    hcalc.SetFlags(MHillasCalc::kCalcHillasSrc);
    hcalc2.SetFlags(MHillasCalc::kCalcHillasSrc);
    hcalc2.SetNameHillasSrc("MHillasSrcAnti");
    hcalc2.SetNameSrcPosCam("MSrcPosAnti");

    if (fDataSetTrain.IsWobbleMode())
    {
        scalc.SetMode(MSrcPosCalc::kWobble); // kWobble for off-source
        fPreTasksSet[kTrainOff].AddFirst(&hcalc2);
        fPreTasksSet[kTrainOff].AddFirst(&hcalc);
        //fPreTasksSet[kTrainOff].AddFirst(&srcrndm);
        fPreTasksSet[kTrainOff].AddFirst(&scor);
        fPreTasksSet[kTrainOff].AddFirst(&scalc);
    }

    // Setup the tool class to read the background and read them
    fill.SetName("FillBackground");
    fill.SetDestMatrix1(&train, fNum[kTrainOff]);
    fill.SetReader(&read3);
    fill.AddPreTasks(fPreTasksSet[kTrainOff]);
    fill.AddPreTasks(fPreTasks);
    fill.AddPostTasks(fPostTasksSet[kTrainOff]);
    fill.AddPostTasks(fPostTasks);
    if (!fill.Process(plistx))
        return kFALSE;

    // Check the number or read events
    const Int_t numbackgrndtrn = train.GetNumRows()-numgammastrn;
    if (numbackgrndtrn==0)
    {
        *fLog << err << "ERROR - No background available for training... aborting." << endl;
        return kFALSE;
    }

    // ------------------------ Train RF --------------------------

    MRanForestCalc rf("TrainSeparation", fTitle);
    rf.SetNumTrees(fNumTrees);
    rf.SetNdSize(fNdSize);
    rf.SetNumTry(fNumTry);
    rf.SetNumObsoleteVariables(1);
    rf.SetLastDataColumnHasWeights(fEnableWeights[kTrainOn] || fEnableWeights[kTrainOff]);
    rf.SetDebug(fDebug>1);
    rf.SetDisplay(fDisplay);
    rf.SetLogStream(fLog);
    rf.SetFileName(out);
    rf.SetNameOutput("MHadronness");

    // Train the random forest either by classification or regression
    if (fUseRegression)
    {
        if (!rf.TrainRegression(train)) // regression
            return kFALSE;
    }
    else
    {
        if (!rf.TrainSingleRF(train))   // classification
            return kFALSE;
    }

    // Output information about what was going on so far.
    *fLog << all;
    fLog->Separator("The forest was trained with...");

    *fLog << "Training method:" << endl;
    *fLog << " * " << (fUseRegression?"regression":"classification") << endl;
    if (fEnableWeights[kTrainOn])
        *fLog << " * weights for on-data" << endl;
    if (fEnableWeights[kTrainOff])
        *fLog << " * weights for off-data" << endl;
    if (fDataSetTrain.IsWobbleMode())
        *fLog << " * random source position in a distance of 0.4°" << endl;
    *fLog << endl;
    *fLog << "Events used for training:"   << endl;
    *fLog << " * Gammas:     " << numgammastrn   << endl;
    *fLog << " * Background: " << numbackgrndtrn << endl;
    *fLog << endl;
    *fLog << "Gamma/Background ratio:" << endl;
    *fLog << " * Requested:  " << (float)fNum[kTrainOn]/fNum[kTrainOff] << endl;
    *fLog << " * Result:     " << (float)numgammastrn/numbackgrndtrn << endl;
    *fLog << endl;
    *fLog << "Run-Time: " << Form("%.1f", clock.RealTime()/60) << "min (CPU: ";
    *fLog << Form("%.1f", clock.CpuTime()/60) << "min)" << endl;
    *fLog << endl;
    *fLog << "Output file name: " << out << endl;

    // Chekc if testing is requested
    if (!fDataSetTest.IsValid())
        return kTRUE;

    // --------------------- Display result ----------------------
    fLog->Separator("Test");

    clock.Continue();

    // Setup parlist and tasklist for testing
    MParList  plist;
    MTaskList tlist;
    plist.AddToList(this); // Take care of display
    plist.AddToList(&tlist);

    MMcEvt mcevt;
    plist.AddToList(&mcevt);

    plist.AddToList(&wgt);

    // ----- Setup histograms -----
    MBinning binsy(50, 0 , 1,      "BinningMH3Y", "lin");
    MBinning binsx(40, 10, 100000, "BinningMH3X", "log");

    plist.AddToList(&binsx);
    plist.AddToList(&binsy);

    MH3 h31("MHillas.fSize",  "MHadronness.fVal");
    MH3 h32("MHillas.fSize",  "MHadronness.fVal");
    MH3 h40("MMcEvt.fEnergy", "MHadronness.fVal");
    h31.SetTitle("Background probability vs. Size:Size [phe]:Hadronness h");
    h32.SetTitle("Background probability vs. Size:Size [phe]:Hadronness h");
    h40.SetTitle("Background probability vs. Energy:Energy [GeV]:Hadronness h");

    MHHadronness hist;

    // ----- Setup tasks -----
    MFillH fillh0(&hist, "", "FillHadronness");
    MFillH fillh1(&h31);
    MFillH fillh2(&h32);
    MFillH fillh4(&h40);
    fillh0.SetWeight("MWeight");
    fillh1.SetWeight("MWeight");
    fillh2.SetWeight("MWeight");
    fillh4.SetWeight("MWeight");
    fillh1.SetDrawOption("colz profy");
    fillh2.SetDrawOption("colz profy");
    fillh4.SetDrawOption("colz profy");
    fillh1.SetNameTab("Background");
    fillh2.SetNameTab("GammasH");
    fillh4.SetNameTab("GammasE");
    fillh0.SetBit(MFillH::kDoNotDisplay);

    // ----- Setup filter -----
    MFilterList precuts;
    precuts.AddToList(fPreCuts);
    precuts.AddToList(fTestCuts);

    MContinue c0(&precuts);
    c0.SetName("PreCuts");
    c0.SetInverted();

    MFEventSelector sel; // FIXME: USING IT (WITH PROB?) in READ will by much faster!!!
    sel.SetNumSelectEvts(fNum[kTestOff]);

    MContinue c1(&sel);
    c1.SetInverted();

    // ----- Setup tasklist -----
    tlist.AddToList(&read2);
    if (fDataSetTest.IsWobbleMode())
    {
        tlist.AddToList(&srcrndm);
        tlist.AddToList(&hcalc);
    }
    tlist.AddToList(&c1);
    tlist.AddToList(fPreTasksSet[kTestOff]);
    tlist.AddToList(fPreTasks);
    tlist.AddToList(&c0);
    tlist.AddToList(&rf);
    tlist.AddToList(fPostTasksSet[kTestOff]);
    tlist.AddToList(fPostTasks);
    tlist.AddToList(&fillh0);
    tlist.AddToList(&fillh1);

    // Enable Acceleration
    tlist.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    // ----- Run eventloop on background -----
    MEvtLoop loop;
    loop.SetDisplay(fDisplay);
    loop.SetLogStream(fLog);
    loop.SetParList(&plist);
    //if (!SetupEnv(loop))
    //   return kFALSE;

    wgt.SetVal(1);
    if (!loop.Eventloop())
        return kFALSE;
 /*
    if (!loop.GetDisplay())
    {
        gLog << warn << "Display closed by user... execution aborted." << endl << endl;
        return kFALSE;
    }
   */
    // ----- Setup and run eventloop on gammas -----
    sel.SetNumSelectEvts(fNum[kTestOn]);
    fillh0.ResetBit(MFillH::kDoNotDisplay);

    // Remove PreTasksOff and PostTasksOff from the list
    tlist.RemoveFromList(fPreTasksSet[kTestOff]);
    tlist.RemoveFromList(fPostTasksSet[kTestOff]);

    // replace the reading task by a new one
    tlist.Replace(&read4);

    if (fDataSetTest.IsWobbleMode())
    {
        // *******************************************************************
        // Possible source position (eg. Wobble Mode)
        if (fDataSetTest.HasSource())
        {
            if (!fDataSetTest.GetSourcePos(source))
                return -1;
            *fLog << all;
            source.Print("RaDec");
        }
        else
            *fLog << all << "No source position applied..." << endl;

        // La Palma Magic1
        plist.AddToList(&obs);
        plist.AddToList(&source);

        // How to get source position from off- and on-data?
        tlist.AddToListAfter(&scalc,  &read4);
        tlist.AddToListAfter(&scor,   &scalc);
        tlist.AddToListAfter(&hcalcw, &scor);

        tlist.AddToList(&devcalc, "Starguider");
        // *******************************************************************
    }

    // Add the PreTasksOn directly after the reading task
    tlist.AddToListAfter(fPreTasksSet[kTestOn], &c1);

    // Add the PostTasksOn after rf
    tlist.AddToListAfter(fPostTasksSet[kTestOn], &rf);

    // Replace fillh1 by fillh2
    tlist.Replace(&fillh2);

    // Add fillh4 after the new fillh2
    tlist.AddToListAfter(&fillh4, &fillh2);

    // Enable Acceleration
    tlist.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    wgt.SetVal(1);
    if (!loop.Eventloop())
        return kFALSE;

    // Show what was going on in the testing
    const Double_t numgammastst   = h32.GetHist().GetEntries();
    const Double_t numbackgrndtst = h31.GetHist().GetEntries();

    *fLog << all;
    fLog->Separator("The forest was tested with...");
    *fLog << "Test method:" << endl;
    *fLog << " * Random Forest: " << out << endl;
    if (fEnableWeights[kTestOn])
        *fLog << " * weights for on-data" << endl;
    if (fEnableWeights[kTestOff])
        *fLog << " * weights for off-data" << endl;
    if (fDataSetTrain.IsWobbleMode())
        *fLog << " * random source position in a distance of 0.4°" << endl;
    *fLog << endl;
    *fLog << "Events used for test:"   << endl;
    *fLog << " * Gammas:     " << numgammastst   << endl;
    *fLog << " * Background: " << numbackgrndtst << endl;
    *fLog << endl;
    *fLog << "Gamma/Background ratio:" << endl;
    *fLog << " * Requested:  " << (float)fNum[kTestOn]/fNum[kTestOff] << endl;
    *fLog << " * Result:     " << (float)numgammastst/numbackgrndtst << endl;
    *fLog << endl;

    // Display the result plots
    DisplayResult(h31, h32, ontime);

    *fLog << "Total Run-Time: " << Form("%.1f", clock.RealTime()/60) << "min (CPU: ";
    *fLog << Form("%.1f", clock.CpuTime()/60) << "min)" << endl;
    fLog->Separator();

    fDataSetTrain.SetName("DataSetTrain");
    fDataSetTest.SetName("DataSetTest");

    // Write the display
    TObjArray arr;
    arr.Add(const_cast<MDataSet*>(&fDataSetTrain));
    arr.Add(const_cast<MDataSet*>(&fDataSetTest));
    if (fDisplay)
        arr.Add(fDisplay);

    SetPathOut(out);
    return WriteContainer(arr, 0, "UPDATE");
}
