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
!   Author(s): Thomas Bretz, 8/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Wolfgang Wittek, 1/2002 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MHEffectiveOnTime
//
//  Filling this you will get the effective on-time versus theta and
//  observation time.
//
//  From this histogram the effective on-time is determined by a fit.
//  The result of the fit (see Fit()) and the fit-parameters (like chi^2)
//  are stored in corresponding histograms
//
//  To determin the efective on time a poisson fit is done. For more details
//  please have a look into the source code of FitH() it should be simple
//  to understand. In this function a Delta-T distribution is fitted, while
//  Delta-T is the time between two consecutive events.
//
//  The fit is done for projections of a 2D histogram in Theta and Delta T.
//  So you get the effective on time versus theta.
//
//  To get the effective on-time versus time a histogram is filled with
//  the Delta-T distribution of a number of events set by SetNumEvents().
//  The default is 12000 (roughly 1min at 200Hz)
//
//  For each "time-bin" the histogram is fitted and the resulting effective
//  on-time is stored in the fHTimeEffOn histogram. Each entry in this
//  histogram is the effective observation time between the upper and
//  lower edges of the bins.
//
//  In addition the calculated effective on time is stored in a
//  "MEffectiveOnTime [MParameterDerr]" and the corresponding time-stamp
//  (the upper edge of the bin) "MTimeEffectiveOnTime [MTime]"
//
//  The class takes two binnings from the Parameter list; if these binnings
//  are not available the defaultbinning is used:
//      MBinning("BinningDeltaT"); // Units of seconds
//      MBinning("BinningTheta");  // Units of degrees
//
//
//  Usage:
//  ------
//    MFillH fill("MHEffectiveOnTime", "MTime");
//    tlist.AddToList(&fill);
//
//
//  Input Container:
//    MPointingPos
//    MRawRunHeader
//    MTime
//
//  Output Container:
//    MEffectiveOnTime [MParameterDerr]
//    MTimeEffectiveOnTime [MTime]
//
//
//  Class version 2:
//  ----------------
//   +  UInt_t fFirstBin;
//   +  UInt_t fNumEvents;
//   -  Int_t fNumEvents;
//
//  Class version 3:
//  ----------------
//   +  Double_t fTotalTime;
//
//  Class version 4:
//  ----------------
//   +  Double_t fEffOnTime;    //[s] On time as fitted from the DeltaT distribution
//   +  Double_t fEffOnTimeErr; //[s] On time error as fitted from the DeltaT distribution
//
// ==========================================================================
// Dear Colleagues,
//
// for the case that we are taking calibration events interleaved with
// cosmics events the calculation of the effective observation time has to
// be modified. I have summarized the proposed procedures in the note at the
// end of this message. The formulas have been checked by a simulation.
//
// Comments are welcome.
//
// Regards,  Wolfgang
// --------------------------------------------------------------------------
//                                                       Wolfgang Wittek
//                                                         2 Dec. 2004
//
// Calculation of the effective observation time when cosmics and calibration
// events are taken simultaneously.
// --------------------------------
//
// I. Introduction
// ---------------
// It is planned to take light calibration events (at a certain fixed frequency
// lambda_calib) interlaced with cosmics events. The advantages of this
// procedure are :
//
// - the pedestals, which would be determined from the cosmics, could be
//   used for both the calibration and the cosmics events
//
// - because calibration and cosmics events are taken quasi simultaneously,
//   rapid variations (in the order of a few minutes) of base lines and of the
//   photon/ADC conversion factors could be recognized and taken into account
//
// The effective observation time T_eff is defined as that time range, within
// which the recorded number of events N_cosmics would be obtained under ideal
// conditions (only cosmics, no dead time, no calibration events, ...).
//
// In the absence of calibration events the effective observation time can
// be determined from the distribution of time differences 'dt' between
// successive cosmics events (see first figure in the attached ps file).
// The exponential slope 'lambda' of this distribution is the ideal cosmics
// event rate. If 'N_cosmics' is the total number of recorded cosmics events,
// T_eff is obtained by
//
//    T_eff = N_cosmics / lambda
//
// In the case of a finite dead time 'dead', the distribution (for dt > dead) is
// still exponential with the same slope 'lambda'. 'lambda' should be determined
// in a region of 'dt' which is not affected by the dead time, i.e. at not too
// low 'dt'.
//
//
//
// II. Problems in the presence of calibration events
// --------------------------------------------------
// If calibration events are taken interlaced with cosmics, and if the dead time
// is negligible, the distribution of time differences 'dt' between cosmics can
// be used for calculating the effective observation time, as if the calibration
// events were not present.
//
// In the case of a non-negligible dead time 'dead', however, the distribution of
// time differences between cosmics is distorted, because a cosmics event may be
// lost due to the dead time after a calibration event. Even if the time
// intervals are ignored which contain a calibration event,
//
//
//        ---|---------o--------|--------->  t
//
//        cosmics    calib    cosmics
//
//            <---------------->           <==== time interval to be ignored
//
//
// the distribution of 'dt' is still distorted, because there would be no
// 'dt' with dt > tau_calib = 1/lambda_calib. The distribution would also be
// distorted in the region dt < tau_calib, due to calibration events occuring
// shortly after cosmics events. As a result, the slope of the distribution of
// 'dt' would not reflect the ideal cosmics event rate (see second figure; the
// values assumed in the simulation are lambda = 200 Hz, lambda_calib = 50
// Hz, dead = 0.001 sec, total time = 500 sec, number of generated cosmics
// events = 100 000).
//
//
// Note also that some calibration events will not be recorded due to the dead
// time after a cosmics event.
//
//
// III. Proposed procedures
// ------------------------
//
// A) The ideal event rate 'lambda' may be calculated from the distribution of
// the time difference 'dt_first' between a calibration event and the first
// recorded cosmics event after the calibration event. In the region
//
//     dead < dt_first < tau_calib
//
// the probability distribution of dt_first is given by
//
//     p(dt_first) = c * exp(-lambda*dt_first)
//
// where c is a normalization constant. 'lambda' can be obtained by a simple
// exponential fit to the experimental distribution of dt_first (see third
// figure). The fit range should start well above the average value of the dead
// time 'dead'.
//
//
// B) One may consider those time intervals between recorded cosmics events, which
// are completely contained in the region
//
//    t_calib < t < t_calib + tau_calib
//
// where t_calib is the time of a recorded calibration event.
//
//
//          <--------------- tau_calib ----------->
//
//
//           0   1  2   3 4  5   6  7    8  9   10
//      --|-o---|-|---|--|-|----|--|---|---|-|----o-|---|-|--------->  t
//          ^                                     ^
//          |                                     |
//        t_calib                               t_calib + tau_calib
//
//
// In this example, of the time intervals 0 to 10 only the intervals 1 to 9
// should be retained and plotted. The distribution of the length 'dt' of these
// intervals in the region
//
//     dead < dt < tau_calib
//
// is given by
//
//     p(dt) = c * (tau_calib-dt-dead) * exp(-lambda*dt)
//
// A fit of this expression to the experimental distribution of 'dt' yields
// 'lambda' (see fourth figure). For 'dead' an average value of the dead time
// should be chosen, and the fit range should end well before dt = tau_calib-dead.
//
//
// Method A has the advantage that the p(dt_first) does not depend on 'dead'.
// 'dead' has to be considered when defining the fit range, both in method A and
// in method B. In method B the event statistics is larger leading to a smaller
// fitted error of 'lambda' than method A (see the figures).
//
//
// The effective observation time is again obtained by
//
//    T_eff = N_cosmics / lambda
//
// where N_cosmics is the total number of recorded cosmics events. Note that
// N_cosmics is equal to
//
//    N_cosmics = N_tot - N_calib
//
// where N_tot is the total number of recorded events (including the calibration
// events) and N_calib is the number of recorded calibration events.
//
// Note that if time intervals are discarded for the determination of lambda,
// the corresponding cosmics events need not and should not be discarded.
//
//
// IV. Procedure if the calibration events are taken in bunches
// ------------------------------------------------------------
// In November 2004 the rate of calibration events is not constant. The events
// are taken in 200 Hz bunches every second, such that the rate is 200 Hz for
// 0.25 sec, followed by a gap of 0.75 sec. Then follows the next 200 Hz bunch.
//
// In this case it is proposed to consider for the calculation of 'lambda' only
// the cosmics events within the gaps of 0.75 sec. For these cosmics events one
// of the methods described in III. can be applied.
//
//
// V. Alternative pocedure
// -----------------------
// The effective observation time can also be determined from the total
// observation time and the total dead time. The latter is written out by the DAQ.
// In this case it has to be made sure that the dead time is available in Mars
// when the effective observation time is calculated.
//
//////////////////////////////////////////////////////////////////////////////
#include "MHEffectiveOnTime.h"

#include <TF1.h>
#include <TMinuit.h>
#include <TRandom.h>

#include <TLatex.h>
#include <TGaxis.h>
#include <TCanvas.h>
#include <TPaveStats.h>

#include "MTime.h"
#include "MString.h"
#include "MParameters.h"
#include "MPointingPos.h"
#include "MRawRunHeader.h"

#include "MBinning.h"
#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHEffectiveOnTime);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor. It initializes all histograms.
//
MHEffectiveOnTime::MHEffectiveOnTime(const char *name, const char *title)
    : fPointPos(0), fTime(0), fParam(0), fIsFinalized(kFALSE), 
    fNumEvents(200*60), fFirstBin(3), fTotalTime(-1),
    fEffOnTime(-1), fEffOnTimeErr(0)
    //fNumEvents(2*60), fFirstBin(1)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHEffectiveOnTime";
    fTitle = title ? title : "Histogram to determin effective On-Time vs Time and Zenith Angle";

    // Main histogram
    fH2DeltaT.SetName("DeltaT");
    fH2DeltaT.SetXTitle("\\Delta t [s]");
    fH2DeltaT.SetYTitle("\\Theta [\\circ]");
    fH2DeltaT.SetZTitle("Count");
    fH2DeltaT.UseCurrentStyle();
    fH2DeltaT.SetDirectory(NULL);

    // Main histogram
    fH1DeltaT.SetName("DeltaT");
    fH1DeltaT.SetXTitle("\\Delta t [s]");
    fH1DeltaT.SetYTitle("Counts");
    fH1DeltaT.UseCurrentStyle();
    fH1DeltaT.SetDirectory(NULL);

    // effective on time versus theta
    fHThetaEffOn.SetName("EffOnTheta");
    fHThetaEffOn.SetTitle("Effective On Time T_{eff}");
    fHThetaEffOn.SetXTitle("\\Theta [\\circ]");
    fHThetaEffOn.SetYTitle("T_{eff} [s]");
    fHThetaEffOn.UseCurrentStyle();
    fHThetaEffOn.SetDirectory(NULL);
    fHThetaEffOn.GetYaxis()->SetTitleOffset(1.2);
    fHThetaEffOn.GetYaxis()->SetTitleColor(kBlue);
    fHThetaEffOn.SetLineColor(kBlue);
    //fHEffOn.Sumw2();

    // effective on time versus time
    fHTimeEffOn.SetName("EffOnTime");
    fHTimeEffOn.SetTitle("Effective On Time T_{eff}");
    fHTimeEffOn.SetXTitle("Time");
    fHTimeEffOn.SetYTitle("T_{eff} [s]");
    fHTimeEffOn.UseCurrentStyle();
    fHTimeEffOn.SetDirectory(NULL);
    fHTimeEffOn.GetYaxis()->SetTitleOffset(1.2);
    fHTimeEffOn.GetXaxis()->SetLabelSize(0.033);
    fHTimeEffOn.GetXaxis()->SetTimeFormat("%H:%M:%S %F1995-01-01 00:00:00 GMT");
    fHTimeEffOn.GetXaxis()->SetTimeDisplay(1);
    fHTimeEffOn.GetYaxis()->SetTitleColor(kBlue);
    fHTimeEffOn.SetLineColor(kBlue);

    // chi2 probability
    fHThetaProb.SetName("ProbTheta");
    fHThetaProb.SetTitle("\\chi^{2} Probability of Fit");
    fHThetaProb.SetXTitle("\\Theta [\\circ]");
    fHThetaProb.SetYTitle("p [%]");
    fHThetaProb.UseCurrentStyle();
    fHThetaProb.SetDirectory(NULL);
    fHThetaProb.GetYaxis()->SetTitleOffset(1.2);
    fHThetaProb.SetMaximum(101);
    fHThetaProb.GetYaxis()->SetTitleColor(kBlue);
    fHThetaProb.SetLineColor(kBlue);

    // chi2 probability
    fHTimeProb.SetName("ProbTime");
    fHTimeProb.SetTitle("\\chi^{2} Probability of Fit");
    fHTimeProb.SetXTitle("Time");
    fHTimeProb.SetYTitle("p [%]");
    fHTimeProb.UseCurrentStyle();
    fHTimeProb.SetDirectory(NULL);
    fHTimeProb.GetYaxis()->SetTitleOffset(1.2);
    fHTimeProb.GetXaxis()->SetLabelSize(0.033);
    fHTimeProb.GetXaxis()->SetTimeFormat("%H:%M:%S %F1995-01-01 00:00:00 GMT");
    fHTimeProb.GetXaxis()->SetTimeDisplay(1);
    fHTimeProb.SetMaximum(101);
    fHTimeProb.GetYaxis()->SetTitleColor(kBlue);
    fHTimeProb.SetLineColor(kBlue);

    // lambda versus theta
    fHThetaLambda.SetName("LambdaTheta");
    fHThetaLambda.SetTitle("Slope (Rate) vs Theta");
    fHThetaLambda.SetXTitle("\\Theta [\\circ]");
    fHThetaLambda.SetYTitle("\\lambda [s^{-1}]");
    fHThetaLambda.UseCurrentStyle();
    fHThetaLambda.SetDirectory(NULL);
    fHThetaLambda.SetLineColor(kGreen);

    // lambda versus time
    fHTimeLambda.SetName("LambdaTime");
    fHTimeLambda.SetTitle("Slope (Rate) vs Time");
    fHTimeLambda.SetXTitle("\\Time [\\circ]");
    fHTimeLambda.SetYTitle("\\lambda [s^{-1}]");
    fHTimeLambda.UseCurrentStyle();
    fHTimeLambda.SetDirectory(NULL);
    fHTimeLambda.GetYaxis()->SetTitleOffset(1.2);
    fHTimeLambda.GetXaxis()->SetLabelSize(0.033);
    fHTimeLambda.GetXaxis()->SetTimeFormat("%H:%M:%S %F1995-01-01 00:00:00 GMT");
    fHTimeLambda.GetXaxis()->SetTimeDisplay(1);
    fHTimeLambda.SetLineColor(kGreen);

    // NDoF versus theta
    fHThetaNDF.SetName("NDofTheta");
    fHThetaNDF.SetTitle("Number of Degrees of freedom vs Theta");
    fHThetaNDF.SetXTitle("\\Theta [\\circ]");
    fHThetaNDF.SetYTitle("NDoF [#]");
    fHThetaNDF.UseCurrentStyle();
    fHThetaNDF.SetDirectory(NULL);
    fHThetaNDF.SetLineColor(kGreen);

    // NDoF versus time
    /*
    fHTimeNDF.SetName("NDofTime");
    fHTimeNDF.SetTitle("Number of Degrees of freedom vs Time");
    fHTimeNDF.SetXTitle("Time");
    fHTimeNDF.SetYTitle("NDoF [#]");
    fHTimeNDF.UseCurrentStyle();
    fHTimeNDF.SetDirectory(NULL);
    fHTimeNDF.GetYaxis()->SetTitleOffset(1.2);
    fHTimeNDF.GetXaxis()->SetLabelSize(0.033);
    fHTimeNDF.GetXaxis()->SetTimeFormat("%H:%M:%S %F1995-01-01 00:00:00 GMT");
    fHTimeNDF.GetXaxis()->SetTimeDisplay(1);
    fHTimeNDF.SetLineColor(kBlue);
    */
    // setup binning
    MBinning btheta("BinningTheta");
    btheta.SetEdgesASin(67, -0.005, 0.665);

    MBinning btime("BinningDeltaT");
    btime.SetEdges(50, 0, 0.1);

    MH::SetBinning(fH2DeltaT, btime, btheta);

    btime.Apply(fH1DeltaT);

    btheta.Apply(fHThetaEffOn);
    btheta.Apply(fHThetaLambda);
    btheta.Apply(fHThetaNDF);
    btheta.Apply(fHThetaProb);
    //btheta.Apply(fHChi2);
}

// --------------------------------------------------------------------------
//
// Set the binnings and prepare the filling of the histogram
//
Bool_t MHEffectiveOnTime::SetupFill(const MParList *plist)
{
   fPointPos = (MPointingPos*)plist->FindObject("MPointingPos");
   if (!fPointPos)
   {
       *fLog << err << dbginf << "MPointingPos not found... aborting." << endl;
       return kFALSE;
   }

   // FIXME: Remove const-qualifier from base-class!
   fTime = (MTime*)const_cast<MParList*>(plist)->FindCreateObj("MTime", "MTimeEffectiveOnTime");
   if (!fTime)
       return kFALSE;
   fParam = (MParameterDerr*)const_cast<MParList*>(plist)->FindCreateObj("MParameterDerr", "MEffectiveOnTime");
   if (!fParam)
       return kFALSE;

   const MBinning* binsdtime = (MBinning*)plist->FindObject("BinningDeltaT");
   const MBinning* binstheta = (MBinning*)plist->FindObject("BinningTheta");
   if (binsdtime)
       binsdtime->Apply(fH1DeltaT);
   if (binstheta)
   {
       binstheta->Apply(fHThetaEffOn);
       binstheta->Apply(fHThetaLambda);
       binstheta->Apply(fHThetaNDF);
       binstheta->Apply(fHThetaProb);
       //binstheta->Apply(fHChi2);
   }
   if (binstheta && binsdtime)
       SetBinning(fH2DeltaT, *binsdtime, *binstheta);

   fTotalTime = 0;

   fEffOnTime    = -1;
   fEffOnTimeErr =  0;

   return kTRUE;
}

Bool_t MHEffectiveOnTime::ReInit(MParList *pList)
{
    MRawRunHeader *header = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!header)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fTotalTime += header->GetRunLength();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fit a single Delta-T distribution. See source code for more details
//
Bool_t MHEffectiveOnTime::FitH(TH1D *h, Double_t *res, Bool_t paint) const
{
    // Count also events in under-/overflowbins
    const Double_t Nm = h->Integral(0, h->GetNbinsX()+1);

    // FIXME: Do fit only if contents of bin has changed
    if (Nm<=0 || h->GetBinContent(1)<=0)
        return kFALSE;

    // determine range (yq[0], yq[1]) of time differences
    // where fit should be performed;
    // require a fraction >=xq[0] of all entries to lie below yq[0]
    //     and a fraction <=xq[1] of all entries to lie below yq[1];
    // within the range (yq[0], yq[1]) there must be no empty bin;
    // choose pedestrian approach as long as GetQuantiles is not available
    Double_t xq[2] = { 0.6, 0.95 }; // previously 0.99
    Double_t yq[2];
    h->GetQuantiles(2, yq, xq);

    //
    // Determine a good starting value for the slope
    //
    const TAxis  &axe = *h->GetXaxis();
    const UInt_t ibin = axe.FindFixBin(yq[1]);
    const Double_t x1 = axe.GetBinCenter(ibin<=fFirstBin?fFirstBin+1:ibin);
    const Double_t x0 = axe.GetBinCenter(fFirstBin);
    const Double_t y1 = h->GetBinContent(ibin)>1 ? TMath::Log(h->GetBinContent(ibin)) : 0;
    const Double_t y0 = TMath::Log(h->GetBinContent(fFirstBin));

    // Estimated slope
    const Float_t m = -(y1-y0)/(x1-x0);

    //
    // Setup exponential function for the fit:
    //
    // parameter 0 = rate [Hz]
    // parameter 1 = normalization
    //
    TF1 func("Exp", " exp([1]-[0]*x)");

    func.SetParameter(0, m);       // Hz
    func.SetParameter(1, log(h->GetBinContent(1)));       // Hz

    // We set a limit to make sure that almost empty histograms which
    // are fitted dont't produce hang ups or crashes
    func.SetParLimits(0, 0, 15000); // Hz

    // options : N  do not store the function, do not draw
    //           I  use integral of function in bin rather than value at bin center
    //           R  use the range specified in the function range
    //           Q  quiet mode
    //           L  Use log-likelihood (better for low statistics)
    h->Fit(&func, "NIQEL", "", h->GetBinLowEdge(fFirstBin)/*yq[0]*/, yq[1]);

    const Double_t chi2 = func.GetChisquare();
    const Double_t prob = func.GetProb();
    const Int_t    NDF  = func.GetNDF();

    // was fit successful ?
    const Bool_t ok = prob>0.001; //NDF>0 && chi2<3*NDF;

    if (paint)
    {
        func.SetLineWidth(2);
        func.SetLineColor(ok ? kGreen : kRed);
        func.Paint("same");
    }

    // The effective on time is the "real rate" (slope of the exponential)
    // divided by the total number of events (histogram integral including 
    // under- and overflows)
    const Double_t lambda = func.GetParameter(0);
    const Double_t dldl   = func.GetParError(0)*func.GetParError(0);
    const Double_t teff   = lambda==0 ? 0 : Nm / lambda;
    const Double_t dteff  = lambda==0 ? 0 : teff * TMath::Sqrt(dldl/(lambda*lambda) + 1.0/Nm);
    const Double_t dl     = TMath::Sqrt(dldl);

    // the effective on time is Nm/lambda
    res[0] = teff;
    res[1] = dteff;

    // plot chi2-probability of fit
    res[2] = prob*100;

    // lambda of fit
    res[3] = lambda;
    res[4] = dl;

    // NDoF of fit
    res[5] = NDF;

    // Chi2
    res[6] = chi2;

    return ok;
}

// --------------------------------------------------------------------------
//
// Fit a all bins of the distribution in theta. Store the result in the
// Theta-Histograms
//
void MHEffectiveOnTime::FitThetaBins()
{
    fHThetaEffOn.Reset();
    fHThetaProb.Reset();
    fHThetaLambda.Reset();
    fHThetaNDF.Reset();

    // Use a random name to make sure the object is unique
    const TString name = MString::Format("CalcTheta%d", (UInt_t)gRandom->Uniform(999999999));

    // nbins = number of Theta bins
    const Int_t nbins = fH2DeltaT.GetNbinsY();

    TH1D *h=0;
    for (int i=1; i<=nbins; i++)
    {
        //        TH1D &h = *hist->ProjectionX("Calc-theta", i, i);
        h = fH2DeltaT.ProjectionX(name, i, i, "E");

        Double_t res[7] = {0, 0, 0, 0, 0, 0, 0};
        //if (!FitH(h, res))
        //    continue;
        FitH(h, res);

        if (res[0]==0)
            continue;

        // the effective on time is Nm/lambda
        fHThetaEffOn.SetBinContent(i, res[0]);
        fHThetaEffOn.SetBinError  (i, res[1]);

        // plot chi2-probability of fit
        fHThetaProb.SetBinContent(i, res[2]);

        // plot chi2/NDF of fit
        //fHChi2.SetBinContent(i, res[3]);

        // lambda of fit
        fHThetaLambda.SetBinContent(i, res[3]);
        fHThetaLambda.SetBinError  (i, res[4]);

        // NDoF of fit
        fHThetaNDF.SetBinContent(i, res[5]);

        // Rdead (from fit) is the fraction from real time lost by the dead time
        //fHRdead.SetBinContent(i, Rdead);
        //fHRdead.SetBinError  (i,dRdead);
    }

    // Histogram is reused via gROOT->FindObject()
    // Need to be deleted only once
    if (h)
        delete h;
}

// --------------------------------------------------------------------------
//
// Fit the single-time-bin histogram. Store the result in the
// Time-Histograms
//
void MHEffectiveOnTime::FitTimeBin()
{
    //
    // Fit histogram
    //
    Double_t res[7];
    if (!FitH(&fH1DeltaT, res))
        return;

    // Reset Histogram
    fH1DeltaT.Reset();

    //
    // Prepare Histogram
    //

    // Get number of bins
    const Int_t n = fHTimeEffOn.GetNbinsX();

    // Enhance binning
    MBinning bins;
    bins.SetEdges(fHTimeEffOn, 'x');
    bins.AddEdge(fLastTime.GetAxisTime());
    bins.Apply(fHTimeEffOn);
    bins.Apply(fHTimeProb);
    bins.Apply(fHTimeLambda);
    //bins.Apply(fHTimeNDF);

    //
    // Fill histogram
    //
    fHTimeEffOn.SetBinContent(n, res[0]);
    fHTimeEffOn.SetBinError(n, res[1]);

    fHTimeProb.SetBinContent(n, res[2]);

    fHTimeLambda.SetBinContent(n, res[3]);
    fHTimeLambda.SetBinError(n, res[4]);

    //fHTimeNDF.SetBinContent(n, res[5]);

    //
    // Now prepare output
    //
    fParam->SetVal(res[0], res[1]);
    fParam->SetReadyToSave();

    *fTime = fLastTime;

    // Include the current event
    fTime->Plus1ns();

    *fLog << inf2 << fLastTime << ":  Val=" << res[0] << "  Err=" << res[1] << endl;
}

// --------------------------------------------------------------------------
//
//  Fill the histogram
//
Int_t MHEffectiveOnTime::Fill(const MParContainer *par, const Stat_t w)
{
    const MTime *time = dynamic_cast<const MTime*>(par);
    if (!time)
    {
        *fLog << err << "ERROR - MHEffectiveOnTime::Fill without argument or container doesn't inherit from MTime... abort." << endl;
        return kERROR;
    }

    //
    // If this is the first call we have to initialize the time-histogram
    //
    if (fLastTime==MTime())
    {
        MBinning bins;
        bins.SetEdges(1, time->GetAxisTime()-fNumEvents/200, time->GetAxisTime());
        bins.Apply(fHTimeEffOn);
        bins.Apply(fHTimeProb);
        bins.Apply(fHTimeLambda);
        //bins.Apply(fHTimeNDF);

        fParam->SetVal(0, 0);
        fParam->SetReadyToSave();

        *fTime = *time;

        // Make this 1ns before the first event!
        fTime->Minus1ns();
    }

    //
    // Fill time difference into the histograms
    //
    const Double_t dt = *time-fLastTime;
    fLastTime = *time;

    fH2DeltaT.Fill(dt, fPointPos->GetZd(), w);
    fH1DeltaT.Fill(dt, w);

    //
    // If we reached the event number limit for the time-bins fit the
    // histogram - if it fails try again when 1.6% more events available
    //
    const UInt_t n = (UInt_t)fH1DeltaT.GetEntries();
    if (n>=fNumEvents && n%(fNumEvents/60)==0)
        FitTimeBin();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Fit the theta projections of the 2D histogram and the 1D Delta-T
// distribution
//
Bool_t MHEffectiveOnTime::Finalize()
{
    FitThetaBins();
    FitTimeBin();


    TH1D *h = fH2DeltaT.ProjectionX("FinalizeProjDeltaT", -1, -1, "E");
    Double_t res[7];
    if (FitH(h, res))
    {
        fEffOnTime    = res[0];
        fEffOnTimeErr = res[1];
    }
    delete h;


    fIsFinalized = kTRUE;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Paint the integral and the error on top of the histogram
//
void MHEffectiveOnTime::PaintText(Double_t val, Double_t error, Double_t range) const
{
    TLatex text;
    text.SetBit(TLatex::kTextNDC);
    text.SetTextSize(0.04);

    TString txt = MString::Format("T_{eff} = %.1fs \\pm %.1fs", val, error);
    if (range>0)
        txt += MString::Format("     T_{axe} = %.1fs", range);
    if (fTotalTime>0)
        txt += MString::Format("     T_{max} = %.1fs", fTotalTime);

    text.SetText(0.35, 0.94, txt);
    text.Paint();
}

void MHEffectiveOnTime::PaintText(Double_t *res) const
{
    TLatex text(0.27, 0.94, MString::Format("T_{eff}=%.1fs\\pm%.1fs  \\lambda=%.1f\\pm%.1fHz  p=%.1f%%  \\chi^{2}/%d=%.1f",
                                            res[0], res[1], res[3], res[4], res[2], (int)res[5], res[6]/res[5]));
    text.SetBit(TLatex::kTextNDC);
    text.SetTextSize(0.04);
    text.Paint();
}

void MHEffectiveOnTime::PaintProb(TH1 &h) const
{
    Double_t sum = 0;
    Int_t    n = 0;
    for (int i=0; i<h.GetNbinsX(); i++)
        if (h.GetBinContent(i+1)>0)
        {
            sum += h.GetBinContent(i+1);
            n++;
        }

    if (n==0)
        return;

    TLatex text(0.47, 0.94, MString::Format("\\bar{p} = %.1f%%", sum/n));
    text.SetBit(TLatex::kTextNDC);
    text.SetTextSize(0.04);
    text.Paint();
}

void MHEffectiveOnTime::UpdateRightAxis(TH1 &h)
{
    const Double_t max = h.GetMaximum()*1.1;
    if (max==0)
        return;

    h.SetNormFactor(h.Integral()*gPad->GetUymax()/max);

    TGaxis *axis = (TGaxis*)gPad->FindObject("RightAxis");
    if (!axis)
        return;

    axis->SetX1(gPad->GetUxmax());
    axis->SetX2(gPad->GetUxmax());
    axis->SetY1(gPad->GetUymin());
    axis->SetY2(gPad->GetUymax());
    axis->SetWmax(max);
}

// --------------------------------------------------------------------------
//
//  Prepare painting the histograms
//
void MHEffectiveOnTime::Paint(Option_t *opt)
{
    TH1D *h=0;
    TPaveStats *st=0;

    TString o(opt);
    if (o==(TString)"fit")
    {
        TVirtualPad *pad = gPad;

        for (int x=0; x<2; x++)
            for (int y=0; y<3; y++)
            {
                TVirtualPad *p=gPad->GetPad(x+1)->GetPad(y+1);
                if (!(st = dynamic_cast<TPaveStats*>(p->GetPrimitive("stats"))))
                    continue;

                if (st->GetOptStat()==11)
                    continue;

                const Double_t y1 = st->GetY1NDC();
                const Double_t y2 = st->GetY2NDC();
                const Double_t x1 = st->GetX1NDC();
                const Double_t x2 = st->GetX2NDC();

                st->SetY1NDC((y2-y1)/3+y1);
                st->SetX1NDC((x2-x1)/3+x1);
                st->SetOptStat(11);
            }

        pad->GetPad(1)->cd(1);
        if ((h = (TH1D*)gPad->FindObject("ProjDeltaT"/*fNameProjDeltaT*/)))
        {
            h = fH2DeltaT.ProjectionX("ProjDeltaT"/*fNameProjDeltaT*/, -1, -1, "E");
            if (h->GetEntries()>0)
                gPad->SetLogy();
        }

        pad->GetPad(2)->cd(1);
        if ((h = (TH1D*)gPad->FindObject("ProjTheta"/*fNameProjTheta*/)))
            fH2DeltaT.ProjectionY("ProjTheta"/*fNameProjTheta*/, -1, -1, "E");

        if (!fIsFinalized)
            FitThetaBins();
        return;
    }
    if (o==(TString)"paint")
    {
        if ((h = (TH1D*)gPad->FindObject("ProjDeltaT"/*fNameProjDeltaT*/)))
        {
            Double_t res[7];
            if (FitH(h, res, kTRUE))
                PaintText(res);
        }
        return;
    }

    if (o==(TString)"timendf")
    {
        //    UpdateRightAxis(fHTimeNDF);
        // FIXME: first bin?
        PaintProb(fHTimeProb);
    }

    if (o==(TString)"thetandf")
    {
        UpdateRightAxis(fHThetaNDF);
        // FIXME: first bin?
        PaintProb(fHThetaProb);
    }

    h=0;

    Double_t range=-1;
    if (o==(TString)"theta")
    {
        h = &fHThetaEffOn;
        UpdateRightAxis(fHThetaLambda);
    }
    if (o==(TString)"time")
    {
        h = &fHTimeEffOn;
        UpdateRightAxis(fHTimeLambda);
        range = h->GetXaxis()->GetXmax() - h->GetXaxis()->GetXmin();
    }

    if (!h)
        return;

    Double_t error = 0;
    for (int i=0; i<h->GetXaxis()->GetNbins(); i++)
        error += h->GetBinError(i);

    PaintText(h->Integral(), error, range);
}

void MHEffectiveOnTime::DrawRightAxis(const char *title)
{
    TGaxis *axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(),
                              gPad->GetUxmax(), gPad->GetUymax(),
                              0, 1, 510, "+L");
    axis->SetName("RightAxis");
    axis->SetTitle(title);
    axis->SetTitleOffset(0.9);
    axis->SetTextColor(kGreen);
    axis->CenterTitle();
    axis->SetBit(kCanDelete);
    axis->Draw();
}

// --------------------------------------------------------------------------
//
// Draw the histogram
//
void MHEffectiveOnTime::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("fit");

    pad->Divide(2, 1, 1e-10, 1e-10);

    TH1 *h;

    pad->cd(1);
    gPad->SetBorderMode(0);
    gPad->Divide(1, 3, 1e-10, 1e-10);
    pad->GetPad(1)->cd(1);
    gPad->SetBorderMode(0);
    h = fH2DeltaT.ProjectionX("ProjDeltaT"/*fNameProjDeltaT*/, -1, -1, "E");
    h->SetTitle("Distribution of \\Delta t [s]");
    h->SetXTitle("\\Delta t [s]");
    h->SetYTitle("Counts");
    h->SetDirectory(NULL);
    h->SetMarkerStyle(kFullDotMedium);
    h->SetBit(kCanDelete);
    h->Draw();
    AppendPad("paint");

    pad->GetPad(1)->cd(2);
    gPad->SetBorderMode(0);
    fHTimeProb.Draw();
    AppendPad("timendf");
    //fHTimeNDF.Draw("same");
    //DrawRightAxis("NDF");

    pad->GetPad(1)->cd(3);
    gPad->SetBorderMode(0);
    fHTimeEffOn.Draw();
    AppendPad("time");
    fHTimeLambda.Draw("same");
    DrawRightAxis("\\lambda [s^{-1}]");

    pad->cd(2);
    gPad->SetBorderMode(0);
    gPad->Divide(1, 3, 1e-10, 1e-10);

    pad->GetPad(2)->cd(1);
    gPad->SetBorderMode(0);
    h = fH2DeltaT.ProjectionY("ProjTheta"/*fNameProjTheta*/, -1, -1, "E");
    h->SetTitle("Distribution of  \\Theta [\\circ]");
    h->SetXTitle("\\Theta [\\circ]");
    h->SetYTitle("Counts");
    h->SetDirectory(NULL);
    h->SetMarkerStyle(kFullDotMedium);
    h->SetBit(kCanDelete);
    h->GetYaxis()->SetTitleOffset(1.1);
    h->Draw();

    pad->GetPad(2)->cd(2);
    gPad->SetBorderMode(0);
    fHThetaProb.Draw();
    AppendPad("thetandf");
    fHThetaNDF.Draw("same");
    DrawRightAxis("NDF");

    pad->GetPad(2)->cd(3);
    gPad->SetBorderMode(0);
    fHThetaEffOn.Draw();
    AppendPad("theta");
    fHThetaLambda.Draw("same");
    DrawRightAxis("\\lambda [s^{-1}]");
}

// --------------------------------------------------------------------------
//
// The following resources are available:
//
//    MHEffectiveOnTime.FistBin:   3
//    MHEffectiveOnTime.NumEvents: 12000
//
Int_t MHEffectiveOnTime::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "FirstBin", print))
    {
        rc = kTRUE;
        SetFirstBin(GetEnvValue(env, prefix, "FirstBin", (Int_t)fFirstBin));
    }
    if (IsEnvDefined(env, prefix, "NumEvents", print))
    {
        rc = kTRUE;
        SetNumEvents(GetEnvValue(env, prefix, "NumEvents", (Int_t)fNumEvents));
    }
    return rc;
}
