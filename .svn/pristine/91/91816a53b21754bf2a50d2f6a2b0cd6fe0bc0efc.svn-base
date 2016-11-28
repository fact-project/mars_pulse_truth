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
!   Author(s): Thomas Bretz 08/2010 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2010
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MJTrainCuts
// =========
//
// This class is meant as a tool to understand better what a trained
// random forest is doing in the multi-dimensional phase space.
// Consequently, it can also be used to deduce good one or two dimensional
// cuts from the results by mimicing the behaviour of the random forest.
//
//
// Usage
// -----
//
// The instance is created by its default constructor
//
//    MJTrainCuts opt;
//
// In a first step a random forest must be trained and in a second step its
// performance can be evaluated with an independent test sample. The used
// samples are defined by two MDataSet objects, one for the on-data (e.g.
// gammas) and the other one for the off-data (e.g. protons). SequencesOn
// and SequencesOff are used for testing and training respectively.
//
//    MDataSet seton ("myondata.txt");
//    MDataSet setoff("myoffdata.txt");
//
//    // If you want to use all available events
//    opt.SetDataSetOn(seton);
//    opt.SetDataSetOff(setoff);
//
//    // Try to select 10000 and 30000 events for training and testing resp.
//    // opt.SetDataSetOn(seton, 10000, 30000);
//    // opt.SetDataSetOff(setoff, 10000, 30000);
//
// Note that by using several data set in one file (see MDataSet) you can
// have everything in a single file.
//
// The variables which are used for training are now setup as usual
//
//    Int_t p1 = opt.AddParameter("MHillas.fSize");
//    Int_t p2 = opt.AddParameter("MHillas.GetArea*MGeomCam.fConvMm2Deg^2");
//    Int_t p3 = opt.AddParameter("MHillasSrc.fDist*MGeomCam.fConvMm2Deg");
//
// In addition you can now setup a binning for the display of each train
// parameter as follows (for details see MBinning)
//
//    opt.AddBinning(p1, MBinning(40, 10, 10000, "", "log"));
//    opt.AddBinning(p2, MBinning(50,  0, 0.25));
//    opt.AddBinning(p3, MBinning(50,  0, 2.5));
//
// Since with increasing number of variables the possibly combinations
// increase to fast you have to define which plots you are interested in,
// for example:
//
//    opt.AddHist(p3);      // A 1D plot dist
//    opt.AddHist(p1, p2);  // A 2D plot area vs. size
//    opt.AddHist(p3, p2);  // A 2D plot dist vs. size
//
// Also 3D plots are avaiable but they are most probably difficult to
// interprete.
//
// In addition to this you have the usual user interface, i.e. that
//   - PreCuts
//   - TrainCuts
//   - TestCuts
//   - PreTasks
//   - PostTasks
//   - TestTasks
// are available. For details see MJOptimizeBase
//
// void EnableRegression() / void EnableClassification()
//    Defines whether to use the random forest's regression of
//    classification method. Classification is the default.
//
//
// The produced plots
// ------------------
//
// The tab with the plots filled will always look like this:
//
//    +--------+--------+
//    |1       |2       |
//    +--------+--------|
//    |3       |4       |
//    +--------+        |
//    |5       |        |
//    +--------+--------+
//
// Pad1 and Pad2 contain the weighted event distribution of the test-sample.
//
// Pad2 and Pad5 conatin a profile of the hadronness distribution of the
// test-sample.
//
// Pad4 contains a profile of the hadronness distribution of on-data and
// off-data together of the test-data.
//
// If the profiles for on-data and off-data are identical the displayed
// hadronness is obviously independant of the other (non shown) trainings
// variables. Therefore the difference between the two plots show how much
// the variables are correlated. The same is true if the prfiles in
// pad3 and pad5 don't differe from the profile in pad4.
//
// In the most simple case - the random forest is only trained with the
// variables displayed - all three plots should be identical (apart from the
// difference in the distrubution of the three sets).
//
// The plot in pad4 can now be used to deduce a good classical cut in the
// displayed variables.
//
//
// Example:
// --------
//
//    MJTrainCuts opt;
//    MDataSet seton ("dataset_on.txt");
//    MDataSet setoff("dataset_off.txt");
//    opt.SetDataSetOn(seton);
//    opt.SetDataSetOff(setmix);
//    Int_t p00 = opt.AddParameter("MHillas.fSize");
//    Int_t p01 = opt.AddParameter("MHillas.GetArea*MGeomCam.fConvMm2Deg^2");
//    opt.AddHist(p00, p01);    // Area vs Size
//    MStatusDisplay *d = new MStatusDisplay;
//    opt.SetDisplay(d);
//    opt.Process("rf-cuts.root");
//
//
// Random Numbers:
// ---------------
//   Use:
//         if(gRandom)
//             delete gRandom;
//         gRandom = new TRandom3();
//   in advance to change the random number generator.
//
////////////////////////////////////////////////////////////////////////////
#include "MJTrainCuts.h"

#include <TGraph.h>
#include <TMarker.h>
#include <TCanvas.h>
#include <TPRegexp.h>
#include <TStopwatch.h>

#include "MHMatrix.h"

#include "MLog.h"
#include "MLogManip.h"

// tools
#include "MMath.h"
#include "MBinning.h"
#include "MTFillMatrix.h"
#include "MStatusDisplay.h"

// eventloop
#include "MParList.h"
#include "MTaskList.h"
#include "MEvtLoop.h"

// tasks
#include "MReadMarsFile.h"
#include "MContinue.h"
#include "MFillH.h"
#include "MRanForestCalc.h"

// container
#include "MParameters.h"

// histograms
#include "MHn.h"
#include "MHHadronness.h"

// filter
#include "MFEventSelector.h"
#include "MFilterList.h"

using namespace std;

class HistSet1D : public TObject
{
protected:
    UInt_t fNx;

    virtual void AddHist(MHn &h, const char *rx, const char *, const char *) const
    {
        h.AddHist(rx);
    }
    virtual void AddProf(MHn &h, const char *rx, const char *, const char *) const
    {
        h.AddHist(rx, "MHadronness.fVal", MH3::kProfileSpread);
    }
    virtual void SetupName(MHn &h, const char *name) const
    {
        h.InitName(Form("%s%d;%d", name, fNx, fNx));
    }
    virtual void SetupHist(MHn &h, const char *name, const char *title) const
    {
        SetupName(h, name);

        h.SetAutoRange(kFALSE, kFALSE, kFALSE);
        h.InitTitle(title);
    }

    void CreateHist(MHn &h, const char *rx, const char *ry=0, const char *rz=0) const
    {
        h.SetLayout(MHn::kComplex);
        h.SetBit(MHn::kDoNotReset);

        AddHist(h, rx, ry, rz);
        SetupHist(h, "DistOn", "Distribution of on-data");
        h.SetWeight("Type.fVal");

        AddHist(h, rx, ry, rz);
        SetupHist(h, "DistOff", "Distribution of off-data");
        h.SetWeight("1-Type.fVal");

        AddProf(h, rx, ry, rz);
        SetupHist(h, "HadOn", "Hadronness profile for on-data");
        h.SetWeight("Type.fVal");

        AddProf(h, rx, ry, rz);
        SetupHist(h, "Had", "Hadronness profile for all events");

        AddProf(h, rx, ry, rz);
        SetupHist(h, "HadOff", "Hadronness profile for off-data");
        h.SetWeight("1-Type.fVal");
    }


public:
    HistSet1D(UInt_t nx) : fNx(nx) { }

    virtual MHn *GetHistN(const TList &rules) const
    {
        if (!rules.At(fNx))
            return 0;

        MHn *h = new MHn(Form("%d", fNx));

        CreateHist(*h, rules.At(fNx)->GetName());

        return h;
    }

    virtual Bool_t CheckBinning(const TObjArray &binnings) const
    {
        return binnings.FindObject(Form("Binning%d", fNx));
    }
};

class HistSet2D : public HistSet1D
{
protected:
    UInt_t fNy;

    void AddHist(MHn &h, const char *rx, const char *ry, const char *) const
    {
        h.AddHist(rx, ry);
    }
    void AddProf(MHn &h, const char *rx, const char *ry, const char *) const
    {
        h.AddHist(rx, ry, "MHadronness.fVal", MH3::kProfileSpread);
    }
    void SetupName(MHn &h, const char *name) const
    {
        h.InitName(Form("%s%d:%d;%d;%d", name, fNx, fNy, fNx, fNy));
    }

    void SetupHist(MHn &h, const char *name, const char *title) const
    {
        HistSet1D::SetupHist(h, name, title);
        h.SetDrawOption("colz");
    }

public:
    HistSet2D(UInt_t nx, UInt_t ny) : HistSet1D(nx), fNy(ny) { }

    MHn *GetHistN(const TList &rules) const
    {
        if (!rules.At(fNx) || !rules.At(fNy))
            return 0;

        MHn *h = new MHn(Form("%d:%d", fNx, fNy));

        CreateHist(*h, rules.At(fNx)->GetName(), rules.At(fNy)->GetName());

        return h;
    }
    Bool_t CheckBinning(const TObjArray &binnings) const
    {
        return HistSet1D::CheckBinning(binnings) && binnings.FindObject(Form("Binning%d", fNy));
    }
};


class HistSet3D : public HistSet2D
{
private:
    UInt_t fNz;

    void AddHist(MHn &h, const char *rx, const char *ry, const char *rz) const
    {
        h.AddHist(rx, ry, rz);
    }
    void AddProf(MHn &h, const char *rx, const char *ry, const char *rz) const
    {
        h.AddHist(rx, ry, rz, "MHadronness.fVal");
    }
    void SetupName(MHn &h, const char *name) const
    {
        h.InitName(Form("%s%d:%d:%d;%d;%d;%d", name, fNx, fNy, fNz, fNx, fNy, fNz));
    }

public:
    HistSet3D(UInt_t nx, UInt_t ny, UInt_t nz) : HistSet2D(nx, ny), fNz(nz) { }

    MHn *GetHistN(const TList &rules) const
    {
        if (!rules.At(fNx) || !rules.At(fNy) || !rules.At(fNz))
            return 0;

        MHn *h = new MHn(Form("%d:%d:%d", fNx, fNy, fNz));

        CreateHist(*h,
                   rules.At(fNx)->GetName(),
                   rules.At(fNy)->GetName(),
                   rules.At(fNy)->GetName());

        return h;
    }
    Bool_t CheckBinning(const TObjArray &binnings) const
    {
        return HistSet2D::CheckBinning(binnings) && binnings.FindObject(Form("Binning%d", fNz));
    }
};

// ---------------------------------------------------------------------------------------

void MJTrainCuts::AddHist(UInt_t nx)
{
    fHists.Add(new HistSet1D(nx));
}

void MJTrainCuts::AddHist(UInt_t nx, UInt_t ny)
{
    fHists.Add(new HistSet2D(nx, ny));
}

void MJTrainCuts::AddHist(UInt_t nx, UInt_t ny, UInt_t nz)
{
    fHists.Add(new HistSet3D(nx, ny, nz));
}

void MJTrainCuts::AddBinning(UInt_t n, const MBinning &bins)
{
    const char *name = Form("Binning%d", n);

    TObject *o = fBinnings.FindObject(name);
    if (o)
    {
        delete fBinnings.Remove(o);
        *fLog << warn << "WARNING - Binning for parameter " << n << " (" << name << ") already exists... replaced." << endl;
    }

    // FIXME: Check for existence
    fBinnings.Add(new MBinning(bins, name, bins.GetTitle()));
}

/*
void MJTrainCuts::AddBinning(const MBinning &bins)
{
    // FIXME: Check for existence
    fBinnings.Add(new MBinning(bins, bins.GetName(), bins.GetTitle()));
}
*/

// ---------------------------------------------------------------------------------------

// --------------------------------------------------------------------------
//
void MJTrainCuts::DisplayResult(MH3 &h31, MH3 &h32, Float_t ontime)
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

        gROOT->SetSelectedPad(0);
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

        gROOT->SetSelectedPad(0);
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

// --------------------------------------------------------------------------
//
Bool_t MJTrainCuts::Process(const char *out)
{
    // =========================== Consistency checks ==================================
    if (!fDataSetOn.IsValid())
    {
        *fLog << err << "ERROR - DataSet for on-data invalid!" << endl;
        return kFALSE;
    }
    if (!fDataSetOff.IsValid())
    {
        *fLog << err << "ERROR - DataSet for off-data invalid!" << endl;
        return kFALSE;
    }

    if (fDataSetOn.IsWobbleMode()!=fDataSetOff.IsWobbleMode())
    {
        *fLog << err << "ERROR - On- and Off-DataSet have different observation modes!" << endl;
        return kFALSE;
    }

    if (fDataSetOn.IsMonteCarlo()!=fDataSetOff.IsMonteCarlo())
    {
        *fLog << err << "ERROR - On- and Off-DataSet have different monte carlo modes!" << endl;
        return kFALSE;
    }

    if (!HasWritePermission(out))
        return kFALSE;

    // Check if needed binning exists
    TIter NextH(&fHists);
    TObject *o = 0;
    while ((o=NextH()))
    {
        const HistSet1D *hs = static_cast<HistSet1D*>(o);
        if (hs->CheckBinning(fBinnings))
            continue;

        *fLog << err << "ERROR - Not all needed binnning exist." << endl;
        return kFALSE;
    }

    // =========================== Preparation ==================================

    if (fDisplay)
        fDisplay->SetTitle(out);

    TStopwatch clock;
    clock.Start();

    // ------------------ Setup reading --------------------
    MReadMarsFile read1("Events");
    MReadMarsFile read2("Events");
    MReadMarsFile read3("Events");
    MReadMarsFile  read4("Events");
    read1.DisableAutoScheme();
    read2.DisableAutoScheme();
    read3.DisableAutoScheme();
    read4.DisableAutoScheme();

    // Setup four reading tasks with the on- and off-data of the two datasets
    // Training -- On
    if (!fDataSetOn.AddFilesOn(read1))
        return kFALSE;
    // Testing -- On
    if (!fDataSetOn.AddFilesOff(read4))
        return kFALSE;
    // Training -- Off
    if (!fDataSetOff.AddFilesOn(read3))
        return kFALSE;
    // Testing -- Off
    if (!fDataSetOff.AddFilesOff(read2))
        return kFALSE;

    // ===============================================================================
    // ======================           Training             =========================
    // ===============================================================================

    // ---------------- Setup RF Matrix ----------------
    MHMatrix train("Train");
    train.AddColumns(fRules);
//    if (fEnableWeights[kTrainOn] || fEnableWeights[kTrainOff])
//        train.AddColumn("MWeight.fVal");
    train.AddColumn("MHadronness.fVal");

    // ----------------- Prepare filling Matrix RF ------------------

    // Setup the hadronness container identifying gammas and off-data
    // and setup a container for the weights
    MParameterD had("MHadronness");
    MParameterD wgt("MWeight");
    MParameterD typ("Type");

    // Add them to the parameter list
    MParList plistx;
    plistx.AddToList(this); // take care of fDisplay!
    plistx.AddToList(&had);
    plistx.AddToList(&wgt);
    plistx.AddToList(&typ);

    // Setup the tool class to fill the matrix
    MTFillMatrix fill;
    fill.SetLogStream(fLog);
    fill.SetDisplay(fDisplay);
    fill.AddPreCuts(fPreCuts);
    fill.AddPreCuts(fTrainCuts);

    // ----------------- Fill on data into matrix ------------------

    // Setup the tool class to read the gammas and read them
    fill.SetName("FillOn");
    fill.SetDestMatrix1(&train, fNum[kTrainOn]);
    fill.SetReader(&read1);
//    fill.AddPreTasks(fPreTasksSet[kTrainOn]);
    fill.AddPreTasks(fPreTasks);
//    fill.AddPostTasks(fPostTasksSet[kTrainOn]);
    fill.AddPostTasks(fPostTasks);

    // Set classifier for gammas
    had.SetVal(0);
    wgt.SetVal(1);
    typ.SetVal(0);

    // Fill matrix
    if (!fill.Process(plistx))
        return kFALSE;

    // Check the number or read events
    const Int_t numontrn = train.GetNumRows();
    if (numontrn==0)
    {
        *fLog << err << "ERROR - No on-data events available for training... aborting." << endl;
        return kFALSE;
    }

    // Remove possible post tasks
    fill.ClearPreTasks();
    fill.ClearPostTasks();

    // ----------------- Fill off data into matrix ------------------

    // In case of wobble mode we have to do something special
    // Setup the tool class to read the background and read them
    fill.SetName("FillOff");
    fill.SetDestMatrix1(&train, fNum[kTrainOff]);
    fill.SetReader(&read3);
//    fill.AddPreTasks(fPreTasksSet[kTrainOff]);
    fill.AddPreTasks(fPreTasks);
//    fill.AddPostTasks(fPostTasksSet[kTrainOff]);
    fill.AddPostTasks(fPostTasks);

    // Set classifier for background
    had.SetVal(1);
    wgt.SetVal(1);
    typ.SetVal(1);

    // Fiull matrix
    if (!fill.Process(plistx))
        return kFALSE;

    // Check the number or read events
    const Int_t numofftrn = train.GetNumRows()-numontrn;
    if (numofftrn==0)
    {
        *fLog << err << "ERROR - No off-data available for training... aborting." << endl;
        return kFALSE;
    }

    // ------------------------ Train RF --------------------------

    MRanForestCalc rf("TrainSeparation", fTitle);
    rf.SetNumTrees(fNumTrees);
    rf.SetNdSize(fNdSize);
    rf.SetNumTry(fNumTry);
    rf.SetNumObsoleteVariables(1);
//    rf.SetLastDataColumnHasWeights(fEnableWeights[kTrainOn] || fEnableWeights[kTrainOff]);
    rf.SetDebug(fDebug>1);
    rf.SetDisplay(fDisplay);
    rf.SetLogStream(fLog);
    rf.SetFileName(out);
    rf.SetNameOutput("MHadronness");

    // Train the random forest either by classification or regression
    if (!rf.Train(train, fUseRegression))
        return kFALSE;

    // ----------------- Print result of training ------------------

    // Output information about what was going on so far.
    *fLog << all;
    fLog->Separator("The forest was trained with...");

    *fLog << "Training method:" << endl;
    *fLog << " * " << (fUseRegression?"regression":"classification") << endl;
    /*
    if (fEnableWeights[kTrainOn])
        *fLog << " * weights for on-data" << endl;
    if (fEnableWeights[kTrainOff])
        *fLog << " * weights for off-data" << endl;
    */
    *fLog << endl;
    *fLog << "Events used for training:"   << endl;
    *fLog << " * Gammas:     " << numontrn  << endl;
    *fLog << " * Background: " << numofftrn << endl;
    *fLog << endl;
    *fLog << "Gamma/Background ratio:" << endl;
    *fLog << " * Requested:  " << (float)fNum[kTrainOn]/fNum[kTrainOff] << endl;
    *fLog << " * Result:     " << (float)numontrn/numofftrn << endl;
    *fLog << endl;
    *fLog << "Run-Time: " << Form("%.1f", clock.RealTime()/60) << "min (CPU: ";
    *fLog << Form("%.1f", clock.CpuTime()/60) << "min)" << endl;
    *fLog << endl;
    *fLog << "Output file name: " << out << endl;

    // ===============================================================================
    // ======================           Testing              =========================
    // ===============================================================================
    fLog->Separator("Test");

    clock.Continue();

    // ---------------------- Prepare eventloop off-data ---------------------

    // Setup parlist and tasklist for testing
    MParList  plist;
    MTaskList tlist;
    plist.AddToList(this); // Take care of display
    plist.AddToList(&tlist);

//    MMcEvt mcevt;
//    plist.AddToList(&mcevt);

    plist.AddToList(&wgt);
    plist.AddToList(&typ);

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

    plist.AddToList(&fBinnings);

    MHHadronness hist;

    // ----- Setup tasks -----
    MFillH fillh0(&hist, "", "FillHadronness");
    MFillH fillh1(&h31,  "", "FillHadVsSize");
    MFillH fillh2(&h32,  "", "FillHadVsSize");
    MFillH fillh4(&h40,  "", "FillHadVsEnergy");
    fillh0.SetWeight("MWeight");
    fillh1.SetWeight("MWeight");
    fillh2.SetWeight("MWeight");
    fillh4.SetWeight("MWeight");
    fillh1.SetDrawOption("colz profy");
    fillh2.SetDrawOption("colz profy");
    fillh4.SetDrawOption("colz profy");
    fillh1.SetNameTab("HadSzOff");
    fillh2.SetNameTab("HadSzOn");
    fillh4.SetNameTab("HadEnOn");
    fillh0.SetBit(MFillH::kDoNotDisplay);

    // ----- Setup filter -----
    MFilterList precuts;
    precuts.AddToList(fPreCuts);
    precuts.AddToList(fTestCuts);

    MContinue cont0(&precuts);
    cont0.SetName("PreCuts");
    cont0.SetInverted();

    MFEventSelector sel; // FIXME: USING IT (WITH PROB?) in READ will by much faster!!!
    sel.SetNumSelectEvts(fNum[kTestOff]);

    MContinue contsel(&sel);
    contsel.SetInverted();

    // ----- Setup tasklist -----
    tlist.AddToList(&read2);     // Reading task
    tlist.AddToList(&contsel);   // event selector
//    tlist.AddToList(fPreTasksSet[kTestOff]);
    tlist.AddToList(fPreTasks);  // list of pre tasks
    tlist.AddToList(&cont0);     // list of pre cuts and test cuts
    tlist.AddToList(&rf);        // evaluate random forest
//    tlist.AddToList(fPostTasksSet[kTestOff]);
    tlist.AddToList(fPostTasks); // list of post tasks
    tlist.AddToList(&fillh1);    // Fill HadSzOff

    TList autodel;
    autodel.SetOwner();

    TPRegexp regexp("([0-9]:*)+");

    NextH.Reset();
    while ((o=NextH()))
    {
        HistSet1D *hs = static_cast<HistSet1D*>(o);

        // FIXME: Move to beginning of function
        // Check if needed binning exists
        if (!hs->CheckBinning(fBinnings))
            return kFALSE;

        MHn    *hist = hs->GetHistN(fRules);
        MFillH *fill = new MFillH(hist, "", Form("Fill%s", hist->GetName()));

        fill->SetWeight("MWeight");
        fill->SetDrawOption("colz");
        fill->SetNameTab(hist->GetName());
        fill->SetBit(MFillH::kDoNotDisplay);

        tlist.AddToList(fill);

        autodel.Add(fill);
        autodel.Add(hist);
    }
    tlist.AddToList(&fillh0);      // Fill MHHadronness (not displayed in first loop)
    tlist.AddToList(&fTestTasks);  // list of test tasks

    // Enable Acceleration
    tlist.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    // ---------------------- Run eventloop on background ---------------------
    MEvtLoop loop;
    loop.SetDisplay(fDisplay);
    loop.SetLogStream(fLog);
    loop.SetParList(&plist);
    //if (!SetupEnv(loop))
    //   return kFALSE;

    wgt.SetVal(1);
    typ.SetVal(0);
    if (!loop.Eventloop())
        return kFALSE;

    // ---------------------- Prepare eventloop on-data ---------------------

    sel.SetNumSelectEvts(fNum[kTestOn]);      // set number of target events

    fillh0.ResetBit(MFillH::kDoNotDisplay);   // Switch on display MHHadronness

    TIter NextF(&autodel);
    while ((o=NextF()))
    {
        MFillH *fill = dynamic_cast<MFillH*>(o);
        if (fill)
            fill->ResetBit(MFillH::kDoNotDisplay);
    }

    // Remove PreTasksOff and PostTasksOff from the list
//    tlist.RemoveFromList(fPreTasksSet[kTestOff]);
//    tlist.RemoveFromList(fPostTasksSet[kTestOff]);

    tlist.Replace(&read4);                   // replace reading off-data by on-data

    // Add the PreTasksOn directly after the reading task
//    tlist.AddToListAfter(fPreTasksSet[kTestOn], &c1);

    // Add the PostTasksOn after rf
//    tlist.AddToListAfter(fPostTasksSet[kTestOn], &rf);

    tlist.Replace(&fillh2);                  // Fill HadSzOn instead of HadSzOff
    tlist.AddToListAfter(&fillh4, &fillh0);  // Filling of HadEnOn

    // Enable Acceleration
    tlist.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    // ---------------------- Run eventloop on-data ---------------------

    wgt.SetVal(1);
    typ.SetVal(1);
    if (!loop.Eventloop())
        return kFALSE;

    // ---------------------- Print/Display result ---------------------

    // Show what was going on in the testing
    const Double_t numontst  = h32.GetHist().GetEntries();
    const Double_t numofftst = h31.GetHist().GetEntries();

    *fLog << all;
    fLog->Separator("The forest was tested with...");
    *fLog << "Test method:" << endl;
    *fLog << " * Random Forest: " << out << endl;
    /*
    if (fEnableWeights[kTestOn])
        *fLog << " * weights for on-data" << endl;
    if (fEnableWeights[kTestOff])
        *fLog << " * weights for off-data" << endl;
    */
    *fLog << endl;
    *fLog << "Events used for test:"   << endl;
    *fLog << " * Gammas:     " << numontst   << endl;
    *fLog << " * Background: " << numofftst << endl;
    *fLog << endl;
    *fLog << "Gamma/Background ratio:" << endl;
    *fLog << " * Requested:  " << (float)fNum[kTestOn]/fNum[kTestOff] << endl;
    *fLog << " * Result:     " << (float)numontst/numofftst << endl;
    *fLog << endl;

    // Display the result plots
    DisplayResult(h31, h32, -1);
    //DisplayResult(h31, h32, ontime);

    *fLog << "Total Run-Time: " << Form("%.1f", clock.RealTime()/60) << "min (CPU: ";
    *fLog << Form("%.1f", clock.CpuTime()/60) << "min)" << endl;
    fLog->Separator();

    // ----------------- Write result ------------------

    fDataSetOn.SetName("DataSetOn");
    fDataSetOff.SetName("DataSetOff");

    // Write the display
    TObjArray arr;
    arr.Add(const_cast<MDataSet*>(&fDataSetOn));
    arr.Add(const_cast<MDataSet*>(&fDataSetOff));
    if (fDisplay)
        arr.Add(fDisplay);

    SetPathOut(out);
    return WriteContainer(arr, 0, "UPDATE");
}

