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
!   Author(s): Thomas Bretz, 4/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MJSpectrum
//
// Program to calculate spectrum
//
/////////////////////////////////////////////////////////////////////////////
#include "MJSpectrum.h"

// Root
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TLine.h>
#include <TFile.h>
#include <TGraph.h>
#include <TChain.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TObjArray.h>

// Environment
#include "MLog.h"
#include "MLogManip.h"

#include "MMath.h"
#include "MString.h"
#include "MDirIter.h"

#include "MStatusArray.h"
#include "MStatusDisplay.h"

// Container
#include "MH3.h"
#include "MHn.h"
#include "MEnv.h"
#include "MBinning.h"
#include "MParameters.h"
#include "MDataSet.h"
#include "MMcCorsikaRunHeader.h"

// Spectrum
#include "MAlphaFitter.h"
#include "MHAlpha.h"
#include "MHCollectionArea.h"
#include "MHEnergyEst.h"
#include "MMcSpectrumWeight.h"
#include "MHEffectiveOnTime.h"

// Eventloop
#include "MEvtLoop.h"
#include "MTaskList.h"
#include "MParList.h"

// Tasks/Filter
#include "MReadMarsFile.h"
#include "MEnergyEstimate.h"
#include "MTaskEnv.h"
#include "MFillH.h"
#include "MFDataPhrase.h"
#include "MFDataMember.h"
#include "MContinue.h"
#include "MWriteRootFile.h"

ClassImp(MJSpectrum);

using namespace std;

MJSpectrum::MJSpectrum(const char *name, const char *title)
    : fCutQ(0), fCut0(0),fCut1(0), fCut2(0), fCut3(0), fCutS(0),
    fEstimateEnergy(0), fCalcHadronness(0),  fCalcDisp(0),
    fForceTheta(kFALSE), fForceRunTime(kFALSE), fForceOnTimeFit(kFALSE)
{
    fName  = name  ? name  : "MJSpectrum";
    fTitle = title ? title : "Standard program to calculate spectrum";

    // Make sure that fDisplay is maintained properly
    // (i.e. removed via RecursiveRemove if closed)
    gROOT->GetListOfCleanups()->Add(this);
    SetBit(kMustCleanup);
}

// --------------------------------------------------------------------------
//
// Delete all the fCut* data members and fCalcHadronness/fEstimateEnergy
//
MJSpectrum::~MJSpectrum()
{
    if (fCutQ)
        delete fCutQ;
    if (fCut0)
        delete fCut0;
    if (fCut1)
        delete fCut1;
    if (fCut2)
        delete fCut2;
    if (fCut3)
        delete fCut3;
    if (fCutS)
        delete fCutS;
    if (fEstimateEnergy)
        delete fEstimateEnergy;
    if (fCalcHadronness)
        delete fCalcHadronness;
    if (fCalcDisp)
        delete fCalcDisp;
}

// --------------------------------------------------------------------------
//
// Setup a task estimating the energy. The given task is cloned.
//
void MJSpectrum::SetEnergyEstimator(const MTask *task)
{
    if (fEstimateEnergy)
        delete fEstimateEnergy;
    fEstimateEnergy = task ? (MTask*)task->Clone() : 0;
}

// --------------------------------------------------------------------------
//
// Read a MTask named name into task from the open file. If this task is
// required mustexist can be set. Depending on success kTRUE or kFALSE is
// returned. If the task is a MContinue SetAllowEmpty is called.
// The name of the task is set to name.
//
Bool_t MJSpectrum::ReadTask(MTask* &task, const char *name, Bool_t mustexist) const
{
    // If a task is set delete task
    if (task)
    {
        delete task;
        task = 0;
    }

    // Try to get task from file
    task = (MTask*)gFile->Get(name);
    if (!task)
    {
        if (!mustexist)
            return kTRUE;
        *fLog << err << dbginf << "ERROR - " << name << " doen't exist in file!" << endl;
        return kFALSE;
    }

    // Check if task inherits from MTask
    if (!task->InheritsFrom(MTask::Class()))
    {
        *fLog << err << dbginf << "ERROR - " << name << " read doesn't inherit from MTask!" << endl;
        delete task;
        return kFALSE;
    }

    // Set name of task
    task->SetName(name);

    // SetAllowEmpty for MContinue tasks
    if (dynamic_cast<MContinue*>(task))
        dynamic_cast<MContinue*>(task)->SetAllowEmpty();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print setup used for the MC processing, namely MAlphaFitter ans all fCut*.
//
void MJSpectrum::PrintSetup(const MAlphaFitter &fit) const
{
    fLog->Separator("Alpha Fitter");
    *fLog << all;
    fit.Print("result");

    fLog->Separator("Used Cuts");
    fCutQ->Print();
    fCut0->Print();
    fCut1->Print();
    fCutS->Print();
    fCut2->Print();
    fCut3->Print();
}

// --------------------------------------------------------------------------
//
// Read the first MMcCorsikaRunHeader from the RunHeaders tree in
// the dataset.
// The simulated energy range and spectral slope is initialized from
// there.
// In the following eventloops the forced check in MMcSpectrumWeight
// ensures, that the spectral slope and energy range doesn't change.
//
Bool_t  MJSpectrum::InitWeighting(const MDataSet &set, MMcSpectrumWeight &w) const
{
    fLog->Separator("Initialize energy weighting");

    // Read Resources
    if (!CheckEnv(w))
    {
        *fLog << err << "ERROR - Reading resources for MMcSpectrumWeight failed." << endl;
        return kFALSE;
    }

    w.Print("new");

    return kTRUE;
}

Float_t MJSpectrum::ReadInput(MParList &plist, TH1D &h1, TH1D &h2)
{
    *fLog << inf << "Reading from file: " << fPathIn << endl;

    TFile file(fPathIn, "READ");
    if (!file.IsOpen())
    {
        *fLog << err << dbginf << "ERROR - Could not open file " << fPathIn << endl;
        return -1;
    }

    MStatusArray arr;
    if (arr.Read()<=0)
    {
        *fLog << err << dbginf << "ERROR - MStatusDisplay not found in file... abort." << endl;
        return -1;
    }

    //arr.EnableTH1Workaround();

    TH1D *vstime = (TH1D*)arr.FindObjectInCanvas("Theta",  "TH1D", "OnTime");
    TH1D *size   = (TH1D*)arr.FindObjectInCanvas("Excess", "TH1D", "Hist");
    if (!vstime)
    {
        *fLog << err << dbginf << "ERROR - Theta [TH1D] not found in OnTime-tab... abort." << endl;
        return -1;
    }
    if (!size)
    {
        *fLog << err << dbginf << "ERROR - Excess [TH1D] not found in Hist-tab... abort." << endl;
        return -1;
    }

    vstime->Copy(h1);
    size->Copy(h2);
    h1.SetDirectory(0);
    h2.SetDirectory(0);

    if (fDisplay)
        arr.DisplayIn(*fDisplay, "Hist");

    if (!ReadTask(fCutQ, "CutQ"))
        return -1;
    if (!ReadTask(fCut0, "Cut0"))
        return -1;
    if (!ReadTask(fCut1, "Cut1"))
        return -1;
    if (!ReadTask(fCut2, "Cut2"))
        return -1;
    if (!ReadTask(fCut3, "Cut3"))
        return -1;
    if (!ReadTask(fCalcHadronness, "CalcHadronness", kFALSE))
        return -1;
    if (!ReadTask(fCalcDisp,       "CalcDisp",       kFALSE))
        return -1;

    TObjArray arrread;

    TIter Next(plist);
    TObject *o=0;
    while ((o=Next()))
        if (o->InheritsFrom(MBinning::Class()))
            arrread.Add(o);

    arrread.Add(plist.FindObject("MAlphaFitter"));

    if (!ReadContainer(arrread))
        return -1;

    // We don't have to proceed. We later overwrite the result anyway
    if (fForceOnTimeFit)
        return 0;

    const Double_t ufl = vstime->GetBinContent(0);
    const Double_t ofl = vstime->GetBinContent(vstime->GetNbinsX()+1);
    const Double_t eff = vstime->Integral()+ufl+ofl;
    if (ufl>0)
    {
        if (vstime->GetBinLowEdge(1)<=0)
        {
            *fLog << err << "ERROR - Undeflow bin of OnTime histogram not empty as it ought to be." << endl;
            return -1;
        }
        *fLog << warn << "WARNING - " << Form("%.1f%% (%.0fs)", 100*ufl/eff, ufl) << " of the eff. observation time is in the underflow bin." << endl;
    }
    if (ofl>0)
        *fLog << warn << "WARNING - " << Form("%.1f%% (%.0fs)", 100*ofl/eff, ofl) << " of the eff. observation time is in the overflow bin." << endl;

    if (!fForceRunTime)
        return eff;

    TH1D *time   = (TH1D*)arr.FindObjectInCanvas("ExcessTime", "TH1D", "Hist");
    if (!time)
    {
        *fLog << err;
        *fLog << "ERROR - ExcessTime [TH1D] not found in Hist-tab... abort." << endl;
        *fLog << "        Did you try to process Monte Carlos with --force-runtime?" <<endl;
        return -1;
    }

    const Double_t obs = time->GetXaxis()->GetXmax()-time->GetXaxis()->GetXmin();

    *fLog << inf;
    *fLog << "Total run time: " << obs/60 << "min" << endl;
    *fLog << "Eff. obs. time: " << eff/60 << "min  (" << Form("%.1f%%", 100*eff/obs) << ")" << endl;

    return obs;
}

// --------------------------------------------------------------------------
//
// return maximum value of MMcRunHeader.fImpactMax stored in the RunHeaders
// of the files from the MC dataset
//
Bool_t MJSpectrum::AnalyzeMC(const MDataSet &set, Float_t &impactmax, Float_t &emin/*, Float_t emax*/) const
{
    if (fDisplay)
        fDisplay->SetStatusLine1("Analyzing Monte Carlo headers...");

    // ----- Create chain -----
    *fLog << inf << "Getting files... " << flush;
    TChain chain("RunHeaders");
    if (!set.AddFilesOn(chain))
        return kFALSE;
    *fLog << "done. " << endl;

    *fLog << all;
    *fLog << "Searching for maximum impact... " << flush;
    impactmax = chain.GetMaximum("MMcRunHeader.fImpactMax");
    *fLog << "found " << impactmax/100 << "m" << endl;

    *fLog << "Searching for minimum lower energy limit... " << flush;
    emin = chain.GetMinimum("MMcCorsikaRunHeader.fELowLim");
    *fLog << "found " << emin << "GeV" << endl;

    *fLog << "Searching for maximum lower energy limit... " << flush;
    const Float_t emin2 = chain.GetMaximum("MMcCorsikaRunHeader.fELowLim");
    *fLog << "found " << emin2 << "GeV" << endl;

    // Need a check for the upper energy LIMIT?!?

    if (emin2>emin)
    {
        *fLog << warn;
        *fLog << "WARNING - You are using files with different lower limits for the simulated" << endl;
        *fLog << "          energy, i.e. that the collection area and your correction" << endl;
        *fLog << "          coefficients between " << emin << "GeV and ";
        *fLog << emin2 << "GeV might be wrong." << endl;
    }

/*
    *fLog << "Getting maximum energy... " << flush;
    emax = chain.GetMaximum("MMcCorsikaRunHeader.fEUppLim");
    *fLog << "found " << emax << "GeV" << endl;
 */
    return kTRUE;
}

Bool_t MJSpectrum::ReadOrigMCDistribution(const MDataSet &set, TH1 &h, MMcSpectrumWeight &weight) const
{
    // Some debug output
    *fLog << all << endl;
    fLog->Separator("Compiling original MC distribution");

    Float_t impactmax=0, Emin=0;
    if (!AnalyzeMC(set, impactmax, Emin))
        return kFALSE;

    *fLog << inf << "Getting files... " << flush;
    MDirIter iter;
    if (!set.AddFilesOn(iter))
        return kFALSE;
    *fLog << "done. " << endl;

    const Int_t tot = iter.GetNumEntries();

    // Prepare histogram
    h.Reset();
    h.Sumw2();
    if (h.InheritsFrom(TH2::Class()))
    {
        h.SetNameTitle("ThetaEMC", "Event-Distribution vs Theta and Energy for MC (produced)");
        h.SetXTitle("\\Theta [\\circ]");
        h.SetYTitle("E [GeV]");
        h.SetZTitle(MString::Format("Counts normalized to r=%.0fm", impactmax/100));
    }
    else
    {
        h.SetNameTitle("ThetaMC", "Event-Distribution vs Theta for MC (produced)");
        h.SetXTitle("\\Theta [\\circ]");
        h.SetYTitle(MString::Format("Counts normalized to r=%.0fm", impactmax/100));
    }
    h.SetDirectory(0);

    const TString cont = h.InheritsFrom(TH2::Class()) ?
        "MMcEvtBasic.fEnergy:MMcEvtBasic.fTelescopeTheta*TMath::RadToDeg()>>ThetaEMC" :
        "MMcEvtBasic.fTelescopeTheta*TMath::RadToDeg()>>ThetaMC";

    if (fDisplay)
        fDisplay->SetStatusLine1("Reading OriginalMC distribution...");

    TH1 *hfill = (TH1*)h.Clone(h.InheritsFrom(TH2::Class())?"ThetaEMC":"ThetaMC");
    hfill->SetDirectory(0);

    *fLog << all << "Compiling simulated source spectrum... please stand by, this may take a while." << endl;

    Double_t evts = 0;
    Int_t    num  = 0;

    // Reading this with a eventloop is five times slower :(
    TString fname;
    while (fDisplay)
    {
        if (fDisplay)
            fDisplay->SetProgressBarPosition(Float_t(num++)/tot);

        // Get next filename
        fname = iter.Next();
        if (fname.IsNull())
            break;

        if (fDisplay)
            fDisplay->SetStatusLine2(fname);

        // open file
        TFile file(fname);
        if (file.IsZombie())
        {
            *fLog << err << "ERROR - Couldn't open file " << fname << endl;
            return kFALSE;
        }

        // Get tree OriginalMC
        TTree *t = dynamic_cast<TTree*>(file.Get("OriginalMC"));
        if (!t)
        {
            *fLog << err << "ERROR - File " << fname << " doesn't contain tree OriginalMC." << endl;
            return kFALSE;
        }
        if (t->GetEntries()==0)
        {
            *fLog << warn << "WARNING - OriginalMC of " << fname << " empty." << endl;
            continue;
        }

        // Get tree RunHeaders
        TTree *rh = dynamic_cast<TTree*>(file.Get("RunHeaders"));
        if (!rh)
        {
            *fLog << err << "ERROR - File " << fname << " doesn't contain tree RunHeaders." << endl;
            return kFALSE;
        }
        if (rh->GetEntries()!=1)
        {
            *fLog << err << "ERROR - RunHeaders of " << fname << " doesn't contain exactly one entry." << endl;
            return kFALSE;
        }

        // Get corsika run header
        MMcCorsikaRunHeader *head=0;
        rh->SetBranchAddress("MMcCorsikaRunHeader.", &head);
        rh->GetEntry(0);
        if (!head)
        {
            *fLog << err << "ERROR - Couldn't read MMcCorsikaRunHeader from " << fname << "." << endl;
            return kFALSE;
        }

        // Get the maximum impact parameter of this file. Due to different
        // production areas an additional scale-factor is applied.
        //
        // Because it is assumed that the efficiency outside the production
        // area is nearly zero no additional weight has to be applied to the
        // events after cuts. For the events before cuts it is fair to use
        // weights... maybe filling the residual impact with unweighted
        // events would be better?!? (Not that the weighting might be
        // less correct with low statistics, because it could pronounce
        // a fluctuation)
        const Double_t impact = rh->GetMaximum("MMcRunHeader.fImpactMax");
        const Double_t scale  = impactmax/impact;

        // Propagate the run header to MMcSpectrumWeight
        if (!weight.Set(*head))
        {
            *fLog << err << "ERROR - Initializing MMcSpectrumWeight from " << fname << " failed." << endl;
            return kFALSE;
        }

        // Get the corresponding rule for the weights
        const TString weights(weight.GetFormulaWeights("MMcEvtBasic"));

        // No we found everything... go on reading contents
        *fLog << inf2 << "Reading OriginalMC of " << fname << endl;

        // Fill histogram from tree
        hfill->SetDirectory(&file);
        if (t->Draw(cont, weights, "goff")<0)
        {
            *fLog << err << "ERROR - Reading OriginalMC failed." << endl;
            return kFALSE;
        }
        hfill->SetDirectory(0);

        evts += hfill->GetEntries();

        // ----- Complete incomplete energy ranges -----
        // ----- and apply production area weights -----
        weight.CompleteEnergySpectrum(*hfill, Emin, scale*scale);

        // Add weighted data from file to final histogram
        h.Add(hfill);
    }
    delete hfill;

    *fLog << all << "Total number of events from files in Monte Carlo dataset: " << evts << endl;
    if (fDisplay)
        fDisplay->SetStatusLine2("done.");

    if (!fDisplay || h.GetEntries()>0)
        return kTRUE;

    *fLog << err << "ERROR - Histogram with distribution from OriginalMC empty..." << endl;
    return kFALSE;
}

void MJSpectrum::GetThetaDistribution(TH1D &temp1, TH2D &hist2) const
{
    TH1D &temp2 = *hist2.ProjectionX();

    // Display some stuff
    if (fDisplay)
    {
        TCanvas &c = fDisplay->AddTab("ZdDist");
        c.Divide(2,2);

        // On-Time vs. Theta
        c.cd(1);
        gPad->SetBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        temp1.DrawCopy();

        // Number of MC events (produced) vs Theta
        c.cd(2);
        gPad->SetBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
        temp2.SetName("NVsTheta");
        temp2.DrawCopy("hist");

        c.cd(4);
        gPad->SetBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();

        c.cd(3);
        gPad->SetBorderMode(0);
        gPad->SetGridx();
        gPad->SetGridy();
    }

    // Calculate the Probability
    temp1.Divide(&temp2);
    temp1.Scale(1./temp1.Integral(1, temp1.GetNbinsX()+1));

    // Some cosmetics: Name, Axis, etc.
    temp1.SetName("ProbVsTheta");
    temp1.SetTitle("Probability vs. Zenith Angle to choose MC events");
    temp1.SetYTitle("Probability");
    if (fDisplay)
        temp1.DrawCopy("hist");

    delete &temp2;
}

// --------------------------------------------------------------------------
//
// Display the final theta distribution.
//
Bool_t MJSpectrum::DisplayResult(const TH2D &h2) const
{
    if (!fDisplay || !fDisplay->CdCanvas("ZdDist"))
    {
        *fLog << err << "ERROR - Display or tab ZdDist vanished... abort." << endl;
        return kFALSE;
    }

    TH1D &proj = *h2.ProjectionX();
    proj.SetNameTitle("ThetaFinal", "Final Theta Distribution");
    proj.SetXTitle("\\Theta [\\circ]");
    proj.SetYTitle("Counts");
    proj.SetLineColor(kBlue);
    proj.SetDirectory(0);
    proj.SetBit(kCanDelete);

    TVirtualPad *pad = gPad;

    pad->cd(4);
    proj.DrawCopy();

    pad->cd(1);
    TH1D *theta = (TH1D*)gPad->FindObject("Theta");
    if (!theta)
    {
        *fLog << err << "ERROR - Theta-Histogram vanished... cannot proceed." << endl;
        return kFALSE;
    }

    // Check whether histogram is empty
    if (proj.GetMaximum()==0)
    {
        *fLog << err;
        *fLog << "ERROR - The Zenith Angle distribution of your Monte Carlos doesn't overlap" << endl;
        *fLog << "        with the Zenith Angle distribution of your observation." << endl;
        *fLog << "        Maybe the energy binning is undefined or wrong (from ";
        *fLog << h2.GetYaxis()->GetXmin() << "GeV to " << h2.GetYaxis()->GetXmax() << "GeV)" << endl;
        theta->SetLineColor(kRed);
        return kFALSE;;
    }

    // scale histogram and set new maximum for display
    proj.Scale(theta->GetMaximum()/proj.GetMaximum());
    theta->SetMaximum(1.05*TMath::Max(theta->GetMaximum(), proj.GetMaximum()));

    // draw project
    proj.Draw("same");

    // Compare both histograms
    *fLog << inf << "Comparing theta distributions for data and MCs." << endl;

    const Double_t prob = proj.Chi2Test(theta, "P");
    if (prob==1)
        return kTRUE;

    if (prob>0.99)
    {
        *fLog << inf;
        *fLog << "The Zenith Angle distribution of your Monte Carlos fits well" << endl;
        *fLog << "with the Zenith Angle distribution of your observation." << endl;
        *fLog << "The probability for identical Theta distributions is " << prob << endl;
        return kTRUE;
    }

    if (prob<0.01)
    {
        *fLog << err;
        *fLog << "ERROR - The Zenith Angle distribution of your Monte Carlos does not fit" << endl;
        *fLog << "        with the Zenith Angle distribution of your observation." << endl;
        *fLog << "        The probability for identical Theta distributions is " << prob << endl;
        if (!fForceTheta)
            *fLog << "        To force processing use --force-theta (with care!)" << endl;
        theta->SetLineColor(kRed);
        return fForceTheta;
    }

    *fLog << warn;
    *fLog << "WARNING - The Zenith Angle distribution of your Monte Carlos doesn't fits well" << endl;
    *fLog << "          with the Zenith Angle distribution of your observation." << endl;
    *fLog << "          The probability for identical Theta distributions is " << prob << endl;
    return kTRUE;
}

TString MJSpectrum::GetHAlpha() const
{
    TString cls("MHAlpha");
    if (!fDisplay)
        return cls;

    TCanvas *c = fDisplay->GetCanvas("Hist");
    if (!c)
        return cls;

    TIter Next(c->GetListOfPrimitives());
    TObject *obj=0;
    while ((obj=Next()))
        if (obj->InheritsFrom(MHAlpha::Class()))
            break;

    return obj ? TString(obj->ClassName()) : cls;
}

// --------------------------------------------------------------------------
//
// Fills the excess histogram (vs E-est) from the events stored in the
// ganymed result file and therefor estimates the energy.
//
// The resulting histogram excess-vs-energy ist copied into h2.
//
Bool_t MJSpectrum::Refill(MParList &plist, TH1D &h2)/*const*/
{
    // Now fill the histogram
    *fLog << endl;
    fLog->Separator("Refill Excess");
    *fLog << endl;

    MTaskList tlist;
    plist.AddToList(&tlist);

    MReadTree read("Events");
    read.DisableAutoScheme();
    read.AddFile(fPathIn);
/*
    MTaskEnv taskenv0("CalcDisp");
    taskenv0.SetDefault(fCalcDisp);

    MTaskEnv taskenv1("CalcHadronness");
    taskenv1.SetDefault(fCalcHadronness);
 */
    MEnergyEstimate est;
    MTaskEnv taskenv2("EstimateEnergy");
    taskenv2.SetDefault(fEstimateEnergy ? fEstimateEnergy : &est);

    MContinue *cont = new MContinue("", "CutS");
    cont->SetAllowEmpty();

    if (fCutS)
        delete fCutS;
    fCutS = cont;

    // Try to find the class used to determine the signal!
    const TString cls = GetHAlpha();

    // FIXME: Create HistE and HistEOff to be able to modify it from
    // the resource file.
    MFillH fill1(Form("HistEOff [%s]", cls.Data()), "", "FillHistEOff");
    MFillH fill2(Form("HistE    [%s]", cls.Data()), "", "FillHistE");

    // Fill a new MHEffectiveOnTime. It can either be used to
    // re-calculate the on-time or just for manual cross check
    MFillH fillT("MHEffectiveOnTime", "MTime", "FillOnTime");
    fillT.SetNameTab("OnTime");

    MFDataMember f0("DataType.fVal", '<', 0.5, "FilterOffData");
    MFDataMember f1("DataType.fVal", '>', 0.5, "FilterOnData");

    fill1.SetFilter(&f0);
    fill2.SetFilter(&f1);
    fillT.SetFilter(&f1);

    tlist.AddToList(&read);
    //tlist.AddToList(&taskenv0); // not necessary, stored in file!
    //tlist.AddToList(&taskenv1); // not necessary, stored in file!
    tlist.AddToList(&f1);
    tlist.AddToList(&fillT);
    tlist.AddToList(fCutS);
    tlist.AddToList(&taskenv2);
    tlist.AddToList(&f0);
    tlist.AddToList(&fill1);
    tlist.AddToList(&fill2);

    // by setting it here it is distributed to all consecutive tasks
    tlist.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    MEvtLoop loop("RefillExcess"); // ***** fName *****
    loop.SetParList(&plist);
    loop.SetDisplay(fDisplay);
    loop.SetLogStream(fLog);

    if (!SetupEnv(loop))
        return kFALSE;

    if (!loop.Eventloop())
    {
        *fLog << err << GetDescriptor() << ": Refilling of data failed." << endl;
        return kFALSE;
    }

    if (!loop.GetDisplay())
    {
        *fLog << err << GetDescriptor() << ": Execution stopped by user." << endl;
        return kFALSE;
    }

    const MHAlpha *halpha = (MHAlpha *)plist.FindObject("HistE");
    if (!halpha)
    {
        *fLog << err << GetDescriptor() << ": HistE [MHAlpha] not found... abort." << endl;
        return kFALSE;
    }

    halpha->GetHEnergy().Copy(h2);
    h2.SetDirectory(0);

    return kTRUE;
}

/*
Bool_t MJSpectrum::IntermediateLoop(MParList &plist, MH3 &mh1, TH1D &temp1, const MDataSet &set, MMcSpectrumWeight &weight) const
{
    MTaskList tlist1;
    plist.Replace(&tlist1);

    MReadMarsFile readmc("OriginalMC");
    //readmc.DisableAutoScheme();
    if (!set.AddFilesOn(readmc))
        return kFALSE;

    readmc.EnableBranch("MMcEvtBasic.fTelescopeTheta");
    readmc.EnableBranch("MMcEvtBasic.fEnergy");

    mh1.SetLogy();
    mh1.SetLogz();
    mh1.SetName("ThetaE");

    MFillH fill0(&mh1);
    //fill0.SetDrawOption("projx only");

    MBinning *bins2 = (MBinning*)plist.FindObject("BinningEnergyEst");
    MBinning *bins3 = (MBinning*)plist.FindObject("BinningTheta");
    if (bins2 && bins3)
    {
        bins2->SetName("BinningThetaEY");
        bins3->SetName("BinningThetaEX");
    }
    tlist1.AddToList(&readmc);
    tlist1.AddToList(&weight);

    temp1.SetXTitle("MMcEvtBasic.fTelescopeTheta*kRad2Deg");
    MH3 mh3mc(temp1);

    MFEventSelector2 sel1(mh3mc);
    sel1.SetHistIsProbability();

    fill0.SetFilter(&sel1);

    //if (!fRawMc)
        tlist1.AddToList(&sel1);
    tlist1.AddToList(&fill0);

    // by setting it here it is distributed to all consecutive tasks
    tlist1.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    MEvtLoop loop1("IntermediateLoop"); // ***** fName *****
    loop1.SetParList(&plist);
    loop1.SetLogStream(fLog);
    loop1.SetDisplay(fDisplay);

    if (!SetupEnv(loop1))
        return kFALSE;

    if (!loop1.Eventloop(fMaxEvents))
    {
        *fLog << err << GetDescriptor() << ": Processing of MC-data failed." << endl;
        return kFALSE;
    }

    if (!loop1.GetDisplay())
    {
        *fLog << err << GetDescriptor() << ": Execution stopped by user." << endl;
        return kFALSE;
    }

    if (bins2 && bins3)
    {
        bins2->SetName("BinningEnergyEst");
        bins3->SetName("BinningTheta");
    }

    return kTRUE;
}
*/

TString MJSpectrum::FormFloat(Double_t d)
{
    TString s;
    s += d;
    return s.Strip(TString::kLeading);
}

TString MJSpectrum::FormFlux(const TF1 &f, const char *unit)
{
    Double_t p0 = -f.GetParameter(0);
    Double_t p1 =  f.GetParameter(1);

    Double_t e0 =  f.GetParError(0);
    Double_t e1 =  f.GetParError(1);

    MMath::Format(p0, e0);
    MMath::Format(p1, e1);

    const Int_t    i   = TMath::FloorNint(TMath::Log10(p1));
    const Double_t exp = TMath::Power(10, i);

    TString str = MString::Format("(%s #pm %s)·10^{%d} ",
                                  FormFloat(p1/exp).Data(), FormFloat(e1/exp).Data(), i);

    str += MString::Format("#left(#frac{E}{%s}#right)^{-%s #pm %s}", unit,
                           FormFloat(p0).Data(), FormFloat(e0).Data());

    str += " TeV^{-1} m^{-2} s^{-1}";

    return str;
}

TString MJSpectrum::FormString(const TF1 &f, Byte_t type)
{
    switch (type)
    {
    case 0:
        return FormFlux(f, "500GeV");
    case 1:
        return MString::Format("\\chi^{2}/NDF=%.2f/%d", f.GetChisquare(),f.GetNDF());
    case 2:
        return MString::Format("P=%.0f%%", 100*TMath::Prob(f.GetChisquare(),  f.GetNDF()));
    }
    return "";
}

TArrayD MJSpectrum::FitSpectrum(TH1D &spectrum) const
{
    Axis_t lo, hi;
    MH::GetRangeUser(spectrum, lo, hi);

    TF1 f("f", "[1]*(x/500)^[0]", lo, hi);
    f.SetParameter(0, -3.0);
    f.SetParameter(1, spectrum.GetMaximum());
    f.SetLineColor(kBlue);
    f.SetLineWidth(2);
    spectrum.Fit(&f, "NIR"); // M skipped
    f.DrawCopy("same");

    TString str = FormString(f);

    TLatex tex;
    tex.SetTextSize(0.045);
    tex.SetBit(TLatex::kTextNDC);
    tex.SetTextAlign(31);
    tex.DrawLatex(0.89, 0.935, str);

    str = FormString(f, 1);
    tex.DrawLatex(0.89, 0.83, str);

    str = FormString(f, 2);
    tex.DrawLatex(0.89, 0.735, str);

    TArrayD res(2);
    res[0] = f.GetParameter(0);
    res[1] = f.GetParameter(1);
    return res;
}

// --------------------------------------------------------------------------
//
// Calculate the final spectrum from:
//  - collection area
//  - excess
//  - correction coefficients
//  - ontime
// and display it
//
TArrayD MJSpectrum::DisplaySpectrum(MHCollectionArea &area, TH1D &excess, MHEnergyEst &hest, Double_t ontime) const
{
    // Create copies of the histograms
    TH1D collarea(area.GetHEnergy());
    TH1D spectrum(excess);
    TH1D weights;

    // Get spill-over corrections from energy estimation
    hest.GetWeights(weights);  // E_mc/E_est

    // Print effective on-time
    cout << "Effective On time: " << ontime << "s" << endl;

    // Setup spectrum plot
    spectrum.SetNameTitle("Preliminary", "N/sm^{2} versus Energy (before unfolding)");
    spectrum.SetYTitle("N/sm^{2}");
    spectrum.SetDirectory(NULL);
    spectrum.SetBit(kCanDelete);

    // Divide by collection are and on-time
    spectrum.Scale(1./ontime);
    spectrum.Divide(&collarea);

    // Draw spectrum before applying spill-over corrections
    TCanvas &c1 = fDisplay->AddTab("Spectrum");
    c1.Divide(2,2);
    c1.cd(1);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetLogy();
    gPad->SetGridx();
    gPad->SetGridy();
    collarea.DrawCopy();

    c1.cd(2);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetLogy();
    gPad->SetGridx();
    gPad->SetGridy();
    TH1D *spec=(TH1D*)spectrum.DrawCopy();
    //FitSpectrum(*spec);

    c1.cd(3);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetLogy();
    gPad->SetGridx();
    gPad->SetGridy();
    weights.DrawCopy();

    // Apply spill-over correction (done't take the errors into account)
    // They are supposed to be identical with the errors of the
    // collection area and cancel out.
    //spectrum.Multiply(&weights);
    spectrum.SetNameTitle("Flux", "Spectrum");
    spectrum.SetBit(TH1::kNoStats);

    // Minimum number of excessevents to get 3sigma in 1h
    TF1 sensl("SensLZA", "85*(x/200)^(-0.55)", 100, 6000);
    TF1 sensh("SensHZA", "35*(x/200)^(-0.70)", 100, 1000);
    //sens.SetLineColor(kBlue);
    //sens.DrawClone("Csame");

    c1.cd(4);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetLogy();
    gPad->SetGridx();
    gPad->SetGridy();

    TGraph gsensl;//("Sensitivity LZA", "");
    TGraph gsensh;//("Sensitivity HZA", "");

    const Double_t sqrton = TMath::Sqrt(ontime/3600.);

    for (int i=0; i<excess.GetNbinsX(); i++)
    {
        spectrum.SetBinContent(i+1, spectrum.GetBinContent(i+1)*weights.GetBinContent(i+1));
        spectrum.SetBinError(i+1,   spectrum.GetBinError(i+1)  *weights.GetBinContent(i+1));

        spectrum.SetBinContent(i+1, spectrum.GetBinContent(i+1)/spectrum.GetBinWidth(i+1)*1000);
        spectrum.SetBinError(i+1,   spectrum.GetBinError(i+1)/  spectrum.GetBinWidth(i+1)*1000);

        if (collarea.GetBinContent(i+1)<=0)
            continue;

        const Double_t cen = spectrum.GetBinCenter(i+1);
        gsensl.SetPoint(gsensl.GetN(), cen, sensl.Eval(cen)*sqrton/spectrum.GetBinWidth(i+1)*1000/collarea.GetBinContent(i+1)/ontime);
        gsensh.SetPoint(gsensh.GetN(), cen, sensh.Eval(cen)*sqrton/spectrum.GetBinWidth(i+1)*1000/collarea.GetBinContent(i+1)/ontime);

        cout << cen << "   " << sensl.Eval(cen)*sqrton/spectrum.GetBinWidth(i+1)*1000/collarea.GetBinContent(i+1)/ontime;
        cout <<  "   " << sensh.Eval(cen)*sqrton/spectrum.GetBinWidth(i+1)*1000/collarea.GetBinContent(i+1)/ontime;
        cout << endl;
    }

    spectrum.SetMinimum(1e-12);
    spectrum.SetXTitle("E [GeV]");
    spectrum.SetYTitle("dN/dE [N/TeVsm^{2}]");
    spec = (TH1D*)spectrum.DrawCopy();

    TLatex tex;
    tex.SetTextColor(13);

    TF1 fc("Crab", "6.0e-6*(x/300)^(-2.31-0.26*log10(x/300))", 100, 6000);
    fc.SetLineStyle(9);
    fc.SetLineWidth(2);
    fc.SetLineColor(14);
    fc.DrawCopy("same");

    tex.DrawLatex(1250, fc.Eval(1250), "Crab/\\Gamma=-2.3");

    TF1 fa("PG1553", "1.8e-6*(x/200)^-4.21", 90, 600);
    static_cast<const TAttLine&>(fc).Copy(fa);
    fa.DrawCopy("same");

    tex.SetTextAlign(23);
    tex.DrawLatex(600, fa.Eval(600), "PG1553/\\Gamma=-4.2");

    gROOT->SetSelectedPad(0);

    gsensl.SetLineStyle(5);
    gsensl.SetLineColor(14);
    gsensl.SetLineWidth(2);
    gsensl.DrawClone("C")->SetBit(kCanDelete);

    gsensh.SetLineStyle(5);
    gsensh.SetLineColor(14);
    gsensh.SetLineWidth(2);
    gsensh.DrawClone("C")->SetBit(kCanDelete);

    // Display dN/dE*E^2 for conveinience
    fDisplay->AddTab("Check");
    gPad->SetLogx();
    gPad->SetLogy();
    gPad->SetGrid();

    // Calculate Spectrum * E^2
    for (int i=0; i<spectrum.GetNbinsX(); i++)
    {
        const Double_t e = TMath::Sqrt(spectrum.GetBinLowEdge(i+1)*spectrum.GetBinLowEdge(i+2))*1e-3;

        spectrum.SetBinContent(i+1, spectrum.GetBinContent(i+1)*e*e);
        spectrum.SetBinError(  i+1, spectrum.GetBinError(i+1)  *e*e);
    }

    for (int i=0; i<gsensl.GetN(); i++)
    {
        const Double_t e = gsensl.GetX()[i]*1e-3;

        gsensl.GetY()[i] *= e*e;
        gsensh.GetY()[i] *= e*e;
    }

    spectrum.SetName("FluxStd");
    spectrum.SetMarkerStyle(kFullDotMedium);
    spectrum.SetTitle("Differential flux times E^{2}");
    spectrum.SetYTitle("E^{2}·dN/dE [N·TeV/sm^{2}]");
    spectrum.SetDirectory(0);
    spectrum.DrawCopy();

    TF1 fc2("Crab*E^2", "x^2*Crab*1e-6", 100, 6000);
    static_cast<const TAttLine&>(fc).Copy(fc2);
    fc2.DrawCopy("same");

    TF1 fa2("PG1553*E^2", "x^2*PG1553*1e-6", 100, 6000);
    static_cast<const TAttLine&>(fc).Copy(fa2);
    fa2.DrawCopy("same");

    gsensl.DrawClone("C")->SetBit(kCanDelete);
    gsensh.DrawClone("C")->SetBit(kCanDelete);

    // Fit Spectrum
    c1.cd(4);
    return FitSpectrum(*spec);

/*
    TF1 f("f", "[1]*(x/1e3)^[0]", 10, 3e4);
    f.SetParameter(0, -2.87);
    f.SetParameter(1, 1.9e-6);
    f.SetLineColor(kGreen);
    spectrum.Fit(&f, "NIM", "", 100, 5000);
    f.DrawCopy("same");

    const Double_t p0 = f.GetParameter(0);
    const Double_t p1 = f.GetParameter(1);

    const Double_t e0 = f.GetParError(0);
    const Double_t e1 = f.GetParError(1);

    const Int_t    np  = TMath::Nint(TMath::Floor(TMath::Log10(p1)));
    const Double_t exp = TMath::Power(10, np);

    TString str;
    str += Form("(%.2f#pm%.2f)10^{%d}", p1/exp, e1/exp, np);
    str += Form("(\\frac{E}{TeV})^{%.2f#pm%.2f}", p0, e0);
    str += "\\frac{ph}{TeVm^{2}s}";

    TLatex tex;
    tex.SetTextSize(0.045);
    tex.SetBit(TLatex::kTextNDC);
    tex.DrawLatex(0.45, 0.935, str);

    str = Form("\\chi^{2}/NDF=%.2f", f.GetChisquare()/f.GetNDF());
    tex.DrawLatex(0.70, 0.83, str);

    TArrayD res(2);
    res[0] = f.GetParameter(0);
    res[1] = f.GetParameter(1);

    return res;
  */
}

// --------------------------------------------------------------------------
//
// Scale some image parameter plots using the scale factor and plot them
// together with the corresponding MC histograms.
// Called from DisplaySize
//
Bool_t MJSpectrum::PlotSame(MStatusArray &arr, MParList &plist, const char *name, const char *tab, const char *plot, Double_t scale) const
{
    TString same(name);
    same += "Same";

    TH1 *h1  = (TH1*)arr.FindObjectInCanvas(name, "TH1F", tab);
    TH1 *h2  = (TH1*)arr.FindObjectInCanvas(same, "TH1F", tab);
    if (!h1 || !h2)
        return kFALSE;

    TObject *obj = plist.FindObject(plot);
    if (!obj)
    {
        *fLog << warn << plot << " not in parameter list... skipping." << endl;
        return kFALSE;
    }

    TH1 *h3  = (TH1*)obj->FindObject(name);
    if (!h3)
    {
        *fLog << warn << name << " not found in " << plot << "... skipping." << endl;
        return kFALSE;
    }


    const MAlphaFitter *fit = (MAlphaFitter*)plist.FindObject("MAlphaFitter");
    const Double_t ascale = fit ? fit->GetScaleFactor() : 1;

    gPad->SetBorderMode(0);
    h2->SetLineColor(kBlack);
    h3->SetLineColor(kBlue);
    h2->Add(h1, -ascale);

    //h2->Scale(1./ontime);   //h2->Integral());
    h3->Scale(scale);         //h3->Integral());

    h2->SetMaximum(1.05*TMath::Max(h2->GetMaximum(), h3->GetMaximum()));

    h2 = h2->DrawCopy();
    h3 = h3->DrawCopy("same");

    // Don't do this on the original object!
    h2->SetStats(kFALSE);
    h3->SetStats(kFALSE);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Take a lot of histograms and plot them together in one plot.
// Calls PlotSame
//
Bool_t MJSpectrum::DisplaySize(MParList &plist, Double_t scale) const
{
    *fLog << inf << "Reading from file: " << fPathIn << endl;

    TFile file(fPathIn, "READ");
    if (!file.IsOpen())
    {
        *fLog << err << dbginf << "ERROR - Could not open file " << fPathIn << endl;
        return kFALSE;
    }

    file.cd();
    MStatusArray arr;
    if (arr.Read()<=0)
    {
        *fLog << "MStatusDisplay not found in file... abort." << endl;
        return kFALSE;
    }

    TH1 *excess = (TH1D*)arr.FindObjectInCanvas("Excess", "TH1D", "Hist");
    if (!excess)
        return kFALSE;

    // ------------------- Plot excess versus size ------------------- 

    TCanvas &c = fDisplay->AddTab("Excess");
    c.Divide(3,2);
    c.cd(1);
    gPad->SetBorderMode(0);
    gPad->SetLogx();
    gPad->SetLogy();
    gPad->SetGridx();
    gPad->SetGridy();

    excess->SetTitle("Excess events vs Size (data/black, mc/blue)");
    excess = excess->DrawCopy("E2");
    // Don't do this on the original object!
    excess->SetStats(kFALSE);
    excess->SetMarkerStyle(kFullDotMedium);
    excess->SetFillColor(kBlack);
    excess->SetFillStyle(0);
    excess->SetName("Excess  ");
    excess->SetDirectory(0);

    TObject *o=0;
    if ((o=plist.FindObject("ExcessMC")))
    {
        TH1 *histsel = (TH1F*)o->FindObject("");
        if (histsel)
        {
            if (scale<0)
                scale = excess->Integral()/histsel->Integral();

            histsel->Scale(scale);
            histsel->SetLineColor(kBlue);
            histsel->SetBit(kCanDelete);
            histsel = histsel->DrawCopy("E1 same");
            // Don't do this on the original object!
            histsel->SetStats(kFALSE);
          /*
            fLog->Separator("Kolmogorov Test");
            histsel->KolmogorovTest(excess, "DX");
            fLog->Separator("Chi^2 Test");
            const Double_t p = histsel->Chi2Test(excess, "P");

            TLatex tex;
            tex.SetBit(TLatex::kTextNDC);
            tex.DrawLatex(0.75, 0.93, Form("P(\\chi^{2})=%.0f%%", p*100));
           */
        }
    }

    // -------------- Comparison of Image Parameters --------------
    c.cd(2);
    PlotSame(arr, plist, "Dist",   "HilSrc",  "MHHilSrcMCPost", scale);

    c.cd(3);
    PlotSame(arr, plist, "Length", "PostCut", "MHHillasMCPost", scale);

    c.cd(4);
    PlotSame(arr, plist, "M3l",    "HilExt",  "MHHilExtMCPost", scale);

    c.cd(5);
    PlotSame(arr, plist, "Conc1",  "NewPar",  "MHNewParMCPost", scale);

    c.cd(6);
    PlotSame(arr, plist, "Width",  "PostCut", "MHHillasMCPost", scale);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
void MJSpectrum::DisplayCutEfficiency(const MHCollectionArea &area0, const MHCollectionArea &area1) const
{
    if (!fDisplay)
        return;

    const TH1D &trig = area0.GetHEnergy();
    TH1D &cut = (TH1D&)*area1.GetHEnergy().Clone();

    fDisplay->AddTab("CutEff");

    gPad->SetBorderMode(0);
    gPad->SetFrameBorderMode(0);
    gPad->SetLogx();
    gPad->SetGridx();
    gPad->SetGridy();

    cut.Divide(&trig);
    cut.Scale(100);
    cut.SetNameTitle("CutEff", "Background Supression: Cut efficiency (after star)");
    cut.SetYTitle("\\eta [%]");
    cut.SetDirectory(0);
    cut.SetMinimum(0);
    cut.SetMaximum(100);
    cut.SetBit(kCanDelete);
    cut.Draw();

    TLine line;
    line.SetLineColor(kBlue);
    line.SetLineWidth(2);
    line.SetLineStyle(kDashed);
    line.DrawLine(cut.GetBinLowEdge(1), 50, cut.GetBinLowEdge(cut.GetNbinsX()+1), 50);
}

void MJSpectrum::SetupHistEvtDist(MHn &hist) const
{
    hist.AddHist("MMcEvt.fEnergy");
    hist.InitName("EnergyDist;EnergyEst");
    hist.InitTitle("Unweighted event distribution (Real statistics);E [GeV];Counts;");

    hist.AddHist("MPointingPos.fZd");
    hist.InitName("ZdDist;Theta");
    hist.InitTitle("Unweighted event distribution (Real statistics);Zd [\\circ];Counts;");
}

void MJSpectrum::SetupHistEnergyEst(MHn &hist) const
{
    const char *res = "log10(MEnergyEst.fVal)-log10(MMcEvt.fEnergy)";

    hist.AddHist("MHillas.fSize", res);
    hist.InitName("ResSize;Size;EnergyResidual");
    hist.InitTitle(";S [phe];\\Delta lg E;");
    hist.SetDrawOption("colz profx");

    hist.AddHist("MPointingPos.fZd", res);
    hist.InitName("ResTheta;Theta;EnergyResidual");
    hist.InitTitle(";Zd [\\circ];\\Delta lg E;");
    hist.SetDrawOption("colz profx");

    hist.AddHist("MNewImagePar.fLeakage1", res);
    hist.InitName("ResLeak;Leakage;EnergyResidual");
    hist.InitTitle(";Leak;\\Delta lg E;");
    hist.SetDrawOption("colz profx");

    hist.AddHist("MHillasSrc.fDist*3.37e-3", res);
    hist.InitName("ResDist;Dist;EnergyResidual");
    hist.InitTitle(";D [\\circ];\\Delta lg E;");
    hist.SetDrawOption("colz profx");
}

void MJSpectrum::SetupHistDisp(MHn &hist) const
{
    const char *res = "-Disp.fVal*sign(MHillasSrc.fCosDeltaAlpha)-MHillasSrc.fDist*3.37e-3";

    hist.AddHist("MHillas.fSize", res);
    hist.InitName("ResSize;Size;ResidualDist");
    hist.InitTitle(";S [phe];Disp-Dist [\\circ];");
    hist.SetDrawOption("colz profx");

    hist.AddHist("MPointingPos.fZd", res);
    hist.InitName("ResTheta;Theta;ResidualDist");
    hist.InitTitle(";Zd [\\circ];Disp-Dist [\\circ];");
    hist.SetDrawOption("colz profx");

    hist.AddHist("MNewImagePar.fLeakage1", res);
    hist.InitName("ResLeak;Leakage;ResidualDist");
    hist.InitTitle(";Leak;Disp-Dist [\\circ];");
    hist.SetDrawOption("colz profx");

    hist.AddHist("MHillasExt.fSlopeLong*sign(MHillasSrc.fCosDeltaAlpha)/3.37e-3", res);
    hist.InitName("ResSlope;Slope;ResidualDist");
    hist.InitTitle(";Slope;Disp-Dist [\\circ];");
    hist.SetDrawOption("colz profx");
}

void MJSpectrum::SetupHistEnergyRes(MHn &hist) const
{
    hist.AddHist("MEnergyEst.fVal", "(MMcEvt.fEnergy/MEnergyEst.fVal-1)^2", MH3::kProfile);
    hist.InitName("ResEest;EnergyEst;");
    hist.InitTitle(";E_{est} [GeV];Resolution (E_{mc}/E_{est}-1)^{2};");

    //hist.AddHist("MMcEvt.fEnergy", "(MEnergyEst.fVal/MMcEvt.fEnergy-1)^2", MH3::kProfile);
    //hist.InitName("ResEmc;EnergyEst;");
    //hist.InitTitle(";E_{mc} [GeV];Resolution (E_{est}/E_{mc}-1)^{2};");
    hist.AddHist("MHillas.fSize", "(MMcEvt.fEnergy/MEnergyEst.fVal-1)^2", MH3::kProfile);
    hist.InitName("ResSize;Size;");
    hist.InitTitle(";S [phe];Resolution (E_{mc}/E_{est}-1)^{2};");

    hist.AddHist("MPointingPos.fZd", "(MMcEvt.fEnergy/MEnergyEst.fVal-1)^2", MH3::kProfile);
    hist.InitName("ResTheta;Theta;");
    hist.InitTitle(";\\Theta [\\circ];Resolution (E_{mc}/E_{est}-1)^{2};");

    hist.AddHist("MMcEvt.fImpact/100", "(MMcEvt.fEnergy/MEnergyEst.fVal-1)^2", MH3::kProfile);
    hist.InitName("ResImpact;Impact;");
    hist.InitTitle(";I [m];Resolution (E_{mc}/E_{est}-1)^{2};");
}

// --------------------------------------------------------------------------
//
// Setup write to write:
//     container         tree       optional?
//  --------------     ----------  -----------
//   "MHillas"      to  "Events"
//   "MHillasSrc"   to  "Events"
//   "Hadronness"   to  "Events"       yes
//   "MEnergyEst"   to  "Events"       yes
//   "DataType"     to  "Events"
//
void MJSpectrum::SetupWriter(MWriteRootFile *write/*, const char *name*/) const
{
    if (!write)
        return;

    //write->SetName(name);
    write->AddContainer("MHillas",        "Events");
    write->AddContainer("MHillasSrc",     "Events");
    write->AddContainer("MHillasExt",     "Events");
    //write->AddContainer("MPointingPos",   "Events");
    write->AddContainer("MHillasSrcAnti", "Events", kFALSE);
    write->AddContainer("MImagePar",      "Events", kFALSE);
    write->AddContainer("MNewImagePar",   "Events", kFALSE);
    write->AddContainer("MNewImagePar2",  "Events", kFALSE);
    write->AddContainer("Hadronness",     "Events", kFALSE);
    write->AddContainer("MSrcPosCam",     "Events", kFALSE);
    write->AddContainer("MSrcPosAnti",    "Events", kFALSE);
    write->AddContainer("ThetaSquared",   "Events", kFALSE);
    write->AddContainer("OpticalAxis",    "Events", kFALSE);
    write->AddContainer("Disp",           "Events", kFALSE);
    write->AddContainer("Ghostbuster",    "Events", kFALSE);
    write->AddContainer("MEnergyEst",     "Events", kFALSE);
    write->AddContainer("MTime",          "Events", kFALSE);
    write->AddContainer("MMcEvt",         "Events", kFALSE);
    write->AddContainer("MWeight",        "Events");
    write->AddContainer("DataType",       "Events");
    write->AddContainer("FileId",         "Events");
    write->AddContainer("EvtNumber",      "Events");
}

Bool_t MJSpectrum::Process(const MDataSet &set)
{
    if (!set.IsValid())
    {
        *fLog << err << "ERROR - DataSet invalid!" << endl;
        return kFALSE;
    }

    if (!HasWritePermission(GetPathOut()))
        return kFALSE;

    if (!CheckEnv())
        return kFALSE;

    // --------------------------------------------------------------------------------

    *fLog << inf;
    fLog->Separator(GetDescriptor());
    *fLog << "Compile Monte Carlo sample (dataset " << set.GetBaseName() << ")" << endl;
    *fLog << endl;

    if (fDisplay)
        fDisplay->SetWindowName(fName);

    // Setup everything which is read from the ganymed file
    MBinning bins1("BinningAlpha");
    MBinning bins2("BinningEnergyEst");
    MBinning bins3("BinningTheta");
    MBinning bins4("BinningFalseSource");
    MBinning bins5("BinningWidth");
    MBinning bins6("BinningLength");
    MBinning bins7("BinningDist");
    MBinning bins8("BinningM3Long");
    MBinning bins9("BinningM3Trans");
    MBinning bins0("BinningSlope");
    MBinning binsa("BinningAsym");
    MBinning binsb("BinningConc1");

    MEnv env("", "ganymed.rc");

    MAlphaFitter fit;

    MParList plist;
    plist.AddToList(&bins0);
    plist.AddToList(&bins1);
    plist.AddToList(&bins3);
    plist.AddToList(&bins4);
    plist.AddToList(&bins5);
    plist.AddToList(&bins6);
    plist.AddToList(&bins7);
    plist.AddToList(&bins8);
    plist.AddToList(&bins9);
    plist.AddToList(&binsa);
    plist.AddToList(&binsb);
    plist.AddToList(&fit);

    // Read from the ganymed file
    TH1D htheta, size;
    Float_t ontime = ReadInput(plist, htheta, size);
    if (ontime<0)
    {
        *fLog << err << GetDescriptor() << ": Could not determine effective on time..." << endl;
        return kFALSE;
    }

    // Set Zenith angle binning to binning from the ganymed-file
    bins3.SetEdges(htheta, 'x');

    // Read energy binning from resource file
    if (!CheckEnv(bins2))
    {
        *fLog << err << "ERROR - Reading energy binning from resources failed." << endl;
        return kFALSE;
    }
    plist.AddToList(&bins2); // For later use in MC processing

    // -------------- Fill excess events versus energy ---------------

    TH1D excess;

    if (fForceOnTimeFit)
    {
        // Refill excess histogram to determine the excess events
        // If we use the eff. on-time fit we have to loop over the data first
        // This is not really desired, because if something is wrong with
        // the Monte Carlos the program runs quite long before it fails
        if (!Refill(plist, excess))
            return kFALSE;

        // Print the setup and result of the MAlphaFitter, print used cuts
        PrintSetup(fit);

        // ------------ On user request redo eff. on-time fit ------------
        const MHEffectiveOnTime *htime = (MHEffectiveOnTime*)plist.FindObject("MHEffectiveOnTime");
        if (!htime)
        {
            // This should never happen, bt you never know
            *fLog << err;
            *fLog << "ERROR - Use of new effective on-time fit requested for on-time determination," << endl;
            *fLog << "        but MHEffectiveOnTime not found in parameter list... aborting." << endl;
            return kFALSE;
        }

        const TH1D &h = htime->GetHEffOnTheta();
        /*
        if (!htime->IsConsistent() || h.GetNbinsX()!=htheta.GetNbinsX())
        {
            *fLog << err << "ERROR - Effective on-time from newly filles MHEffectiveOnTime (Tab='OnTime) invalid... aborting." << endl;
            return kFALSE;
        }*/

        *fLog << inf;
        *fLog << "Using eff. on-time from new MHEffectiveOnTime (see also 'OnTime')" << endl;
        *fLog << "   Orig. value: " << ontime << "s" << endl;

        // Copy ontime from newly filled and fitted eff on-time histogram
        ontime = htime->GetEffOnTime();

        *fLog << "   New   value: " << ontime << "s" << endl;

        h.Copy(htheta);           // Copy contents of newly filled hist into on-time vs. theta
        htheta.SetName("Theta");  // Copy overwrites the name needed in DisplayResult
        htheta.SetDirectory(0);   // Remove from global directory added by SetName
    }

    // ---------------------------------------------------------------

    // Initialize weighting to a new spectrum as defined in the resource file
    MMcSpectrumWeight weight;
    if (!InitWeighting(set, weight))
        return kFALSE;

    // Print Theta and energy binning for cross-checks
    *fLog << all << endl;
    bins2.Print();
    bins3.Print();

    // Now we read the MC distribution as produced by corsika
    // vs zenith angle and energy.
    // Weight for the new energy spectrum defined in MMcSpectumWeight
    // are applied.
    // Also correction for different lower energy bounds and
    // different production areas (impact parameters) are applied.
    TH2D hist;
    hist.UseCurrentStyle();
    MH::SetBinning(hist, bins3, bins2);
    if (!ReadOrigMCDistribution(set, hist, weight))
        return kFALSE;

    // Check if user has closed the display
    if (!fDisplay)
        return kTRUE;

    // Display histograms and calculate za-weights into htheta
    GetThetaDistribution(htheta, hist);

    // Give the zenith angle weights to the weighting task
    weight.SetWeightsZd(&htheta);

    // No we apply the the zenith-angle-weights to the corsika produced
    // MC distribution. Unfortunately this must be done manually
    // because we are multiplying column by column
    for (int y=0; y<=hist.GetNbinsY()+1; y++)
        for (int x=0; x<=hist.GetNbinsX()+1; x++)
        {
            hist.SetBinContent(x, y, hist.GetBinContent(x, y)*htheta.GetBinContent(x));
            hist.SetBinError(x, y,   hist.GetBinError(x, y)  *htheta.GetBinContent(x));
        }

    // Display the resulting distribution and check it matches
    // the observation time distribution (this could be false
    // for example if you miss MCs of some zenith angles, which you have
    // data for)
    if (!DisplayResult(hist))
        return kFALSE;

    // Refill excess histogram to determine the excess events
    if (!fForceOnTimeFit)
    {
        if (!Refill(plist, excess))
            return kFALSE;

        // Print the setup and result of the MAlphaFitter, print used cuts
        PrintSetup(fit);
    }

    // ------------------------- Final loop --------------------------

    *fLog << endl;
    fLog->Separator("Calculate efficiencies");
    *fLog << endl;

    MTaskList tlist2;
    plist.AddToList(&tlist2);

    MReadMarsFile read("Events");
    read.DisableAutoScheme();
    if (!set.AddFilesOn(read))
        return kFALSE;

    // Selector to get correct (final) theta-distribution
    //temp1.SetXTitle("MPointingPos.fZd");
    //
    //MH3 mh3(temp1);
    //
    //MFEventSelector2 sel2(mh3);
    //sel2.SetHistIsProbability();
    //
    //MContinue contsel(&sel2);
    //contsel.SetInverted();

    // Get correct source position
    //MSrcPosCalc calc;

    // Calculate corresponding Hillas parameters
    //MHillasCalc hcalc1;
    //MHillasCalc hcalc2("MHillasCalcAnti");
    //hcalc1.SetFlags(MHillasCalc::kCalcHillasSrc);
    //hcalc2.SetFlags(MHillasCalc::kCalcHillasSrc);
    //hcalc2.SetNameHillasSrc("MHillasSrcAnti");
    //hcalc2.SetNameSrcPosCam("MSrcPosAnti");

    // Fill collection area and energy estimator (unfolding)
    // Make sure to use the same binning for MHCollectionArea and MHEnergyEst
    MHCollectionArea area0("TriggerArea");
    MHCollectionArea area1;
    area0.SetHistAll(hist);
    area1.SetHistAll(hist);

    MHEnergyEst      hest;

    MFillH fill30(&area0, "", "FillTriggerArea");
    MFillH fill31(&area1, "", "FillCollectionArea");
    MFillH fill4(&hest,   "", "FillEnergyEst");
    MFillH fill5("MHThreshold", "", "FillThreshold");
    fill30.SetWeight();
    fill31.SetWeight();
    fill4.SetWeight();
    fill5.SetWeight();
    fill30.SetNameTab("TrigArea");
    fill31.SetNameTab("ColArea");
    fill4.SetNameTab("E-Est");
    fill5.SetNameTab("Threshold");

/*
    MH3 henergy("MMcEvt.fEnergy");
    henergy.SetName("EventDist;EnergyEst");
    henergy.SetTitle("Unweighted event distribution (Real statistics);E [GeV];Counts;");
    henergy.Sumw2();
    */

    // ---------------------------------------------------------

    MBinning binsA(50, 10, 100000, "BinningSize",           "log");
    MBinning binsC(50,  0, 0.3,    "BinningLeakage",        "lin");
    MBinning binsB(51, -1, 1,      "BinningEnergyResidual", "lin");
    MBinning binsD(51, -1, 1,      "BinningResidualDist",   "lin");
    MBinning binsI(16,  0, 800,    "BinningImpact",         "lin");

    plist.AddToList(&binsA);
    plist.AddToList(&binsB);
    plist.AddToList(&binsC);
    plist.AddToList(&binsD);
    plist.AddToList(&binsI);

    MHn heest("Energy", "Energy Residual (lg E_{est} - lg E_{mc})");
    SetupHistEnergyEst(heest);

    MHn hdisp("Disp", "Dist residual (Disp-Dist)");
    SetupHistDisp(hdisp);

    MHn henergy("EvtDist");
    SetupHistEvtDist(henergy);

    MHn heres("EnergyRes");
    SetupHistEnergyRes(heres);

    MFillH fill4b(&heest, "", "FillEnergyResidual");
    fill4b.SetWeight();

    MFillH fill4c(&hdisp, "", "FillDispResidual");
    fill4c.SetWeight();

    MFillH fill4d(&heres, "", "FillEnergyResolution");
    fill4d.SetWeight();

    MFDataPhrase fdisp("Disp.fVal*sign(MHillasSrc.fCosDeltaAlpha)<0", "FilterDisp");
    fill4c.SetFilter(&fdisp);

    // ---------------------------------------------------------

    MH3 hsize("MHillas.fSize");
    hsize.SetName("ExcessMC");
    hsize.Sumw2();

    MBinning bins(size, "BinningExcessMC");
    plist.AddToList(&hsize);
    plist.AddToList(&bins);

    // ---------------------------------------------------------

    MFillH fillsp("MHSrcPosCam", "MSrcPosCam", "FillSrcPosCam");
    MFillH fill0a(&henergy, "", "FillEventDist");
    MFillH fill1a("MHHillasMCPre  [MHHillas]",      "MHillas",      "FillHillasPre");
    MFillH fill2a("MHHillasMCPost [MHHillas]",      "MHillas",      "FillHillasPost");
    MFillH fill3a("MHVsSizeMCPost [MHVsSize]",      "MHillasSrc",   "FillVsSizePost");
    MFillH fill4a("MHHilExtMCPost [MHHillasExt]",   "MHillasSrc",   "FillHilExtPost");
    MFillH fill5a("MHHilSrcMCPost [MHHillasSrc]",   "MHillasSrc",   "FillHilSrcPost");
    MFillH fill6a("MHImgParMCPost [MHImagePar]",    "MImagePar",    "FillImgParPost");
    MFillH fill7a("MHNewParMCPost [MHNewImagePar]", "MNewImagePar", "FillNewParPost");
    MFillH fill8a("ExcessMC       [MH3]",           "",             "FillExcessMC");
    fillsp.SetNameTab("SrcPos");
    fill0a.SetNameTab("EvtDist");
    fill1a.SetNameTab("PreCut");
    fill2a.SetNameTab("PostCut");
    fill3a.SetNameTab("VsSize");
    fill4a.SetNameTab("HilExt");
    fill5a.SetNameTab("HilSrc");
    fill6a.SetNameTab("ImgPar");
    fill7a.SetNameTab("NewPar");
    fill8a.SetBit(MFillH::kDoNotDisplay);
    fill1a.SetWeight();
    fill2a.SetWeight();
    fill3a.SetWeight();
    fill4a.SetWeight();
    fill5a.SetWeight();
    fill6a.SetWeight();
    fill7a.SetWeight();
    fill8a.SetWeight();
    fillsp.SetWeight();

    // FIXME: To be done: A task checking the lower 1% after the lower
    // energy limit!

    MTaskEnv taskenv0("CalcDisp");
    taskenv0.SetDefault(fCalcDisp);

    MTaskEnv taskenv1("CalcHadronness");
    taskenv1.SetDefault(fCalcHadronness);

    MEnergyEstimate est;
    MTaskEnv taskenv2("EstimateEnergy");
    taskenv2.SetDefault(fEstimateEnergy ? fEstimateEnergy : &est);

    MWriteRootFile write(GetPathOut());
    SetupWriter(&write);

    MParameterI *par = (MParameterI*)plist.FindCreateObj("MParameterI", "DataType");
    if (!par)
        return kFALSE;
    par->SetVal(2);

    // Not really necessary but for sanity
    TObject *cam = plist.FindObject("MSrcPosCam");
    if (cam)
        cam->Clear();

    tlist2.AddToList(&read);
    // If no weighting should be applied but the events should
    // be thrown away according to the theta distribution
    // it is enabled here
    //if (!fRawMc && fNoThetaWeights)
    //    tlist2.AddToList(&contsel);
    //tlist2.AddToList(&calc);
    //tlist2.AddToList(&hcalc1);
    //tlist2.AddToList(&hcalc2);
    tlist2.AddToList(&weight);
    tlist2.AddToList(&fillsp);
    tlist2.AddToList(&fill1a);
    tlist2.AddToList(&fill30);
    tlist2.AddToList(fCutQ);
    tlist2.AddToList(fCut0);
    tlist2.AddToList(&taskenv0);
    tlist2.AddToList(&taskenv1);
    tlist2.AddToList(&fdisp);
    tlist2.AddToList(&fill4c);
    tlist2.AddToList(fCut1);
    tlist2.AddToList(fCutS);
    tlist2.AddToList(fCut2);
    tlist2.AddToList(fCut3);
    tlist2.AddToList(&taskenv2);
    if (!GetPathOut().IsNull())
        tlist2.AddToList(&write);
    tlist2.AddToList(&fill31);
    tlist2.AddToList(&fill4);
    tlist2.AddToList(&fill4b);
    tlist2.AddToList(&fill4d);
    tlist2.AddToList(&fill5);
    tlist2.AddToList(&fill0a);
    tlist2.AddToList(&fill2a);
    tlist2.AddToList(&fill3a);
    tlist2.AddToList(&fill4a);
    tlist2.AddToList(&fill5a);
    tlist2.AddToList(&fill6a);
    tlist2.AddToList(&fill7a);
    tlist2.AddToList(&fill8a);
    //tlist2.AddToList(&fill9a);

    // by setting it here it is distributed to all consecutive tasks
    tlist2.SetAccelerator(MTask::kAccDontReset|MTask::kAccDontTime);

    MEvtLoop loop2(fName); // ***** fName *****
    loop2.SetParList(&plist);
    loop2.SetDisplay(fDisplay);
    loop2.SetLogStream(fLog);

    if (!SetupEnv(loop2))
        return kFALSE;

    if (!loop2.Eventloop(fMaxEvents))
    {
        *fLog << err << GetDescriptor() << ": Processing of MC-data failed." << endl;
        return kFALSE;
    }

    if (!loop2.GetDisplay())
    {
        *fLog << err << GetDescriptor() << ": Execution stopped by user." << endl;
        return kFALSE;
    }

    fLog->Separator("Energy Estimator");
    if (plist.FindObject("EstimateEnergy"))
        plist.FindObject("EstimateEnergy")->Print();

    fLog->Separator("Spectrum");

    // -------------------------- Spectrum ----------------------------

    // Calculate and display spectrum (N/TeVsm^2 at 1TeV)
    TArrayD res(DisplaySpectrum(area1, excess, hest, ontime));

    // Spectrum fitted (convert res[1] from TeV to GeV)
    TF1 flx("flx", MString::Format("%e*pow(x/500, %f)", res[1]/500, res[0]).Data());

    // Number of events this spectrum would produce per s and m^2
    Double_t n = flx.Integral(weight.GetEnergyMin(), weight.GetEnergyMax());

    // scale with effective collection area to get the event rate (N/s)
    // scale with the effective observation time to absolute observed events
    n *= area1.GetCollectionAreaAbs()*ontime; // N

    // Now calculate the scale factor from the number of events
    // produced and the number of events which should have been
    // observed with our telescope in the time ontime
    const Double_t scale = n/area1.GetEntries();

    // Print normalization constant
    cout << "MC normalization factor:  " << scale << endl;

    // Display cut efficiency
    DisplayCutEfficiency(area0, area1);

    // Overlay normalized plots
    DisplaySize(plist, scale);

    // check if output should be written
    if (!fPathOut.IsNull())
    {
        TNamed ganame("ganymed.root", fPathIn.Data());
        TNamed cmdline("CommandLine", fCommandLine.Data());

        // Write the output
        TObjArray cont;
        cont.Add(&env);                           // ganymed.rc
        cont.Add(const_cast<TEnv*>(GetEnv()));    // sponde.rc
        cont.Add(const_cast<MDataSet*>(&set));    // Dataset
        cont.Add(plist.FindObject("MAlphaFitter"));
        cont.Add(&area0);
        cont.Add(&area1);
        cont.Add(&hest);
        cont.Add(&ganame);
        cont.Add(&cmdline);

        if (fDisplay)
            cont.Add(fDisplay);

        if (!WriteContainer(cont, "", GetPathOut().IsNull()?"RECREATE":"UPDATE"))
        {
            *fLog << err << GetDescriptor() << ": Writing result failed." << endl;
            return kFALSE;
        }
    }

    *fLog << all << GetDescriptor() << ": Done." << endl;
    *fLog << endl << endl;

    return kTRUE;
}
