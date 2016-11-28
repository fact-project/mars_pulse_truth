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
!   Author(s): Thomas Bretz, 5/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MHDisp
//
// Create a false source plot using disp.
//
// Currently the use of this class requires to be after MFMagicCuts
// in the tasklist. Switching of the M3L cut in MFMagicCuts is recommended.
//
// Class Version 3:
// ----------------
//  + Double_t fScaleMin;    // [deg] Minimum circle for integration of off-data for scaling
//  + Double_t fScaleMax;    // [deg] Maximum circle for integration of off-data for scaling
//
//////////////////////////////////////////////////////////////////////////////
#include "MHDisp.h"

#include <TStyle.h>
#include <TCanvas.h>

#include <TF1.h>
#include <TF2.h>
#include <TProfile.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MMath.h"
#include "MString.h"
#include "MBinning.h"

#include "MParList.h"
#include "MParameters.h"

#include "MHillas.h"
#include "MSrcPosCam.h"
#include "MPointingPos.h"
#include "MPointingDev.h"

ClassImp(MHDisp);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor
//
MHDisp::MHDisp(const char *name, const char *title)
    : fDisp(0), fDeviation(0), fSrcAnti(0), fHalf(kFALSE), fSmearing(-1),
    fWobble(kFALSE), fScaleMin(0.325), fScaleMax(0.475)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MHDisp";
    fTitle = title ? title : "3D-plot using Disp vs x, y";

    fHist.SetName("Alpha");
    fHist.SetTitle("3D-plot of ThetaSq vs x, y");
    fHist.SetXTitle("dx [\\circ]");
    fHist.SetYTitle("dy [\\circ]");
    fHist.SetZTitle("Eq.cts");

    fHist.SetDirectory(NULL);
    fHistBg.SetDirectory(NULL);
    fHistBg1.SetDirectory(NULL);
    fHistBg2.SetDirectory(NULL);

    fHist.SetBit(TH1::kNoStats);
}

// --------------------------------------------------------------------------
//
// Set binnings (takes BinningFalseSource) and prepare filling of the
// histogram.
//
// Also search for MTime, MObservatory and MPointingPos
//
Bool_t MHDisp::SetupFill(const MParList *plist)
{
    if (!MHFalseSource::SetupFill(plist))
        return kFALSE;

    fDisp = (MParameterD*)plist->FindObject("Disp", "MParameterD");
    if (!fDisp)
    {
        *fLog << err << "Disp [MParameterD] not found... abort." << endl;
        return kFALSE;
    }

    if (fWobble)
    {
        fSrcAnti = (MSrcPosCam*)plist->FindObject("MSrcPosAnti", "MSrcPosCam");
        if (!fSrcAnti)
        {
            *fLog << err << "MSrcPosAnti [MSrcPosCam] not found... abort." << endl;
            return kFALSE;
        }

        *fLog << inf << "Wobble mode initialized. " << endl;
    }

    fDeviation = (MPointingDev*)plist->FindObject("MPointingDev");
    if (!fDeviation)
        *fLog << warn << "MPointingDev not found... ignored." << endl;

    MBinning binsx, binsy;
    binsx.SetEdges(fHist, 'x');
    binsy.SetEdges(fHist, 'y');
    MH::SetBinning(fHistBg, binsx, binsy);

    if (!fHistOff)
    {
        MH::SetBinning(fHistBg1, binsx, binsy);
        MH::SetBinning(fHistBg2, binsx, binsy);
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Fill the histogram. For details see the code or the class description
// 
Int_t MHDisp::Fill(const MParContainer *par, const Stat_t w)
{
    const MHillas *hil = dynamic_cast<const MHillas*>(par);
    if (!hil)
    {
        *fLog << err << "MHDisp::Fill: No container specified!" << endl;
        return kERROR;
    }

    // Get camera rotation angle
    Double_t rho = 0;
    if (fTime && fObservatory && fPointPos)
        rho = fPointPos->RotationAngle(*fObservatory, *fTime, fDeviation);

    // Vector of length disp in direction of shower
    // Move origin of vector to center-of-gravity of shower and derotate
    TVector2 pos1 = hil->GetMean()*fMm2Deg + hil->GetNormAxis()*fDisp->GetVal();

    const TVector2 src = fSrcPos->GetXY()*fMm2Deg;

    Double_t w0 = 1;
    if (fWobble)
    {
        const TVector2 anti = fSrcAnti->GetXY()*fMm2Deg;

        // Skip off-data not in the same half than the source (here: anti-source)
        // Could be replaced by some kind of theta cut...
        if (!fHistOff)
        {
            Double_t r = anti.Mod()>0.2*1.7 ? 0.2*1.7 : anti.Mod();

            // In wobble mode processing the off-data, the anti-source
            // position is our source position. Check if this is a possible
            // gamma. If it is, do not fill it into our off-data histogram
            if ((pos1-anti).Mod()<r)
                return kTRUE;

            // Because afterwards the plots are normalized with the number
            // of entries the  non-overlap  region corresponds to half the
            // contents, the overlap region  to  full contents.  By taking
            // the mean of both distributions  we get the same result than
            // we would have gotten using full  off-events with a slightly
            // increased uncertainty
            // FIXME: The delta stuff could be replaced by a 2*antitheta cut...
            //w0 = delta>25 ? 1 : 2;

            w0 = (pos1+anti).Mod()<r ? 2 : 1;
        }

        // When processing off-data the anti-source is the real source
        const TVector2 srcpos = fHistOff ? src : anti;

        // Define by the source position which histogram to fill
        if (TMath::Abs(srcpos.DeltaPhi(fFormerSrc))*TMath::RadToDeg()>90)
            fHalf = !fHalf;
        fFormerSrc = srcpos;
    }

    // If on-data: Derotate source and P. Translate P to center.
    TVector2 m;
    if (fHistOff)
    {
        // Derotate all position around camera center by -rho
        // Move origin of vector to center-of-gravity of shower and derotate
        pos1=pos1.Rotate(-rho);

        // Shift the source position to 0/0
        if (fSrcPos)
        {
            // m: Position of the camera center in the FS plot
            m = src.Rotate(-rho);
            pos1 -= m;
        }
    }

    // -------------------------------------------------
    //  The following algorithm may look complicated...
    //            It is optimized for speed
    // -------------------------------------------------

    // Define a vector used to calculate rotated x and y component
    const TVector2 rot(TMath::Sin(rho), TMath::Cos(rho));

    // Fold event position with the PSF and fill it into histogram
    const TAxis &axex = *fHist.GetXaxis();
    const TAxis &axey = *fHist.GetYaxis();

    const Int_t nx = axex.GetNbins();
    const Int_t ny = axey.GetNbins();

    // normg: Normalization for Gauss [sqrt(2*pi)*sigma]^2
    const Double_t weight = axex.GetBinWidth(1)*axey.GetBinWidth(1)*w*w0;
    const Double_t psf    = 2*fSmearing*fSmearing;
    const Double_t pi2    = fSmearing * TMath::Pi()/2;
    const Double_t normg  = pi2*pi2 * TMath::Sqrt(TMath::TwoPi()) / weight;
    const Int_t    bz     = fHist.GetZaxis()->FindFixBin(0);

    TH2 &hbg = fHalf ? fHistBg1 : fHistBg2;

    const Bool_t smear = fSmearing>0;
    if (!smear)
    {
        if (!fHistOff)
            hbg.Fill(pos1.X(), pos1.Y(), w*w0);
        else
            fHist.Fill(pos1.X(), pos1.Y(), 0.0, w*w0);
    }

    // To calculate significance map smear with 2*theta-cut and
    // do not normalize gauss, for event map use theta-cut/2 instead
    if (smear || fHistOff)
    {
        for (int x=1; x<=nx; x++)
        {
            const Double_t cx = axex.GetBinCenter(x);
            const Double_t px = cx-pos1.X();

            for (int y=1; y<=ny; y++)
            {
                const Double_t cy = axey.GetBinCenter(y);
                const Double_t sp = Sq(px, cy-pos1.Y());
                if (smear)
                {
                    const Double_t dp = sp/psf;

                    // Values below 1e-3 (>3.5sigma) are not filled into the histogram
                    if (dp<4)
                    {
                        const Double_t rc = TMath::Exp(-dp)/normg;
                        if (!fHistOff)
                            hbg.AddBinContent(hbg.GetBin(x, y), rc);
                        else
                            fHist.AddBinContent(fHist.GetBin(x, y, bz), rc);
                    }
                }

                if (!fHistOff)
                    continue;

                // If we are filling the signal already (fHistOff!=NULL)
                // we also fill the background by projecting the
                // background in the camera into the sky plot.
                const TVector2 v = TVector2(cx+m.X(), cy+m.Y());

                // Speed up further: Xmax-fWobble
                if (v.Mod()>axex.GetXmax()) // Gains 10% speed
                    continue;

                const Int_t    bx = axex.FindFixBin(v^rot);
                const Int_t    by = axey.FindFixBin(v*rot);
                const Double_t bg = fHistOff->GetBinContent(bx, by, bz);

                fHistBg.AddBinContent(fHistBg.GetBin(x, y), bg);
            }
        }
    }

    if (fHistOff)
        fHistBg.SetEntries(fHistBg.GetEntries()+1);

    if (!smear)
        return kTRUE;

    if (!fHistOff)
        hbg.SetEntries(hbg.GetEntries()+1);
    else
        fHist.SetEntries(fHist.GetEntries()+1);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Compile the background in camera coordinates from the two background
// histograms
//
Bool_t MHDisp::Finalize()
{
    if (fHistOff)
        return kTRUE;

    const Int_t bz = fHist.GetZaxis()->FindFixBin(0);

    const Double_t n1 = fHistBg1.GetEntries();
    const Double_t n2 = fHistBg2.GetEntries();

    for (int x=1; x<=fHist.GetNbinsX(); x++)
        for (int y=1; y<=fHist.GetNbinsY(); y++)
        {
            const Int_t bin1 = fHistBg1.GetBin(x, y);

            const Double_t rc =
                (n1==0?0:fHistBg1.GetBinContent(bin1)/n1)+
                (n2==0?0:fHistBg2.GetBinContent(bin1)/n2);

            fHist.SetBinContent(x, y, bz, rc/2);
        }

    fHist.SetEntries(n1+n2);

    // Result corresponds to one smeared background event

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Make sure that if the scale is changed by the context menu all subpads
// are redrawn.
//
void MHDisp::Update()
{
    TVirtualPad *pad = gPad;
    for (int i=1; i<=6; i++)
    {
        if (pad->GetPad(i))
            pad->GetPad(i)->Modified();
    }
}

// --------------------------------------------------------------------------
//
// Return the mean signal in h around (0,0) in a distance between
// 0.325 and 0.475deg
//
Double_t MHDisp::GetOffSignal(TH1 &h) const
{
    const TAxis &axex = *h.GetXaxis();
    const TAxis &axey = *h.GetYaxis();

    Int_t    cnt = 0;
    Double_t sum = 0;
    for (int x=0; x<h.GetNbinsX(); x++)
        for (int y=0; y<h.GetNbinsY(); y++)
        {
            const Double_t d = TMath::Hypot(axex.GetBinCenter(x+1), axey.GetBinCenter(y+1));
            if (d>fScaleMin && d<fScaleMax)
            {
                sum += h.GetBinContent(x+1,y+1);
                cnt++;
            }
        }

    return sum/cnt;
}

// --------------------------------------------------------------------------
//
// Fill h2 with the radial profile of h1 around (x0, y0)
//
void MHDisp::Profile(TH1 &h2, const TH2 &h1, Axis_t x0, Axis_t y0) const
{
    const TAxis &axex = *h1.GetXaxis();
    const TAxis &axey = *h1.GetYaxis();

    h2.Reset();

    for (int x=1; x<=axex.GetNbins(); x++)
        for (int y=1; y<=axey.GetNbins(); y++)
        {
            const Double_t dx = axex.GetBinCenter(x)-x0;
            const Double_t dy = axey.GetBinCenter(y)-y0;

            const Double_t r  = TMath::Hypot(dx, dy);

            h2.Fill(r, h1.GetBinContent(x, y));
        }
}

// --------------------------------------------------------------------------
//
// Remove contents of histogram around a circle.
//
void MHDisp::MakeDot(TH2 &h2) const
{
    const TAxis &axex = *h2.GetXaxis();
    const TAxis &axey = *h2.GetYaxis();

    const Double_t rmax = (fWobble ? axex.GetXmax()/*-0.7*/ : axex.GetXmax()) - axex.GetBinWidth(1);

    for (int x=1; x<=axex.GetNbins(); x++)
        for (int y=1; y<=axey.GetNbins(); y++)
        {
            const Int_t bin = h2.GetBin(x,y);

            const Double_t px = h2.GetBinCenter(x);
            const Double_t py = h2.GetBinCenter(y);

            if (rmax<TMath::Hypot(px, py))
                h2.SetBinContent(bin, 0);
            //else
            //    if (h2.GetBinContent(bin)==0)
            //        h2.SetBinContent(bin, 1e-10);
        }
}

// --------------------------------------------------------------------------
//
// Calculate from signal and background the significance map
//
void MHDisp::MakeSignificance(TH2 &s, const TH2 &h1, const TH2 &h2, const Double_t scale) const
{
    const TAxis &axex = *s.GetXaxis();
    const TAxis &axey = *s.GetYaxis();

    const Int_t nx = axex.GetNbins();
    const Int_t ny = axey.GetNbins();

    const Int_t n = TMath::Nint(0.2/axex.GetBinWidth(1));

    const Double_t sc = h2.GetEntries()/fHistOff->GetEntries();

    for (int x=1; x<=nx; x++)
        for (int y=1; y<=ny; y++)
        {
            Double_t S=0;

            // Only calculate significances for pixels far enough
            // from the border to get all expected pixels.
            if (TMath::Hypot((float)x-0.5*nx, (float)y-0.5*ny)<0.5*axex.GetNbins()-n)
            {
                Double_t sig=0;
                Double_t bg =0;

                // Integral a region of n pixels around x/y
                for (int dx=-n; dx<=n; dx++)
                    for (int dy=-n; dy<=n; dy++)
                    {
                        if (TMath::Hypot((float)dx, (float)dy)>n)
                            continue;

                        const Int_t  bin = s.GetBin(x+dx,y+dy);

                        sig += h1.GetBinContent(bin);
                        bg  += h2.GetBinContent(bin);
                    }

                // Scale such, that the statistical error corresponds to
                // the amount of off-data used to determin the background
                // model, not to the background itself. This gives
                // significances as calculated from the theta-sq plot.
                S = sig>0 ? MMath::SignificanceLiMaSigned(sig, bg*scale/sc, sc) : 0;
            }

            s.SetBinContent(x, y, S);
        }
}

void MHDisp::Profile1D(const char *name, const TH2 &h) const
{
    if (!gPad)
        return;

    TH1D *hrc = dynamic_cast<TH1D*>(gPad->FindObject(name));
    if (!hrc)
        return;

    hrc->Reset();

    //const_cast<TH2&>(h).SetMaximum();
    const Double_t max = h.GetMaximum();

    MBinning(51, -max*1.1, max*1.1).Apply(*hrc);

    for (int x=1; x<=h.GetXaxis()->GetNbins(); x++)
        for (int y=1; y<=h.GetXaxis()->GetNbins(); y++)
        {
            const Int_t   bin  = h.GetBin(x,y);
            const Double_t sig = h.GetBinContent(bin);
            if (sig!=0)
                hrc->Fill(sig);
        }

    gPad->SetLogy(hrc->GetMaximum()>0);

    if (!fHistOff || hrc->GetRMS()<0.1)
        return;

    // ---------- Fix mean ----------
    // TF1 *g = (TF1*)gROOT->GetFunction("gaus");
    // g->FixParameter(1, 0);
    // hrc->Fit("gaus", "BQI");

    hrc->Fit("gaus", "QI");

    TF1 *f = hrc->GetFunction("gaus");
    if (f)
    {
        f->SetLineWidth(1);
        f->SetLineColor(kBlue);
    }
}

void MHDisp::Paint(Option_t *o)
{
    // Compile Background if necessary
    Finalize();

    // Paint result
    TVirtualPad *pad = gPad;

    pad->cd(1);

    // Project on data onto yx-plane
    fHist.GetZaxis()->SetRange(0,9999);
    TH2 *h1=(TH2*)fHist.Project3D("yx_on");

    // Set Glow-palette (PaintSurface doesn't allow more than 99 colors)
    MH::SetPalette(fHistOff?"glowsym":"glow1", 99);
    h1->SetContour(99);

    TH2 *hx=0;

    Double_t scale = 1;
    if (fHistOff)
    {
        // Project off data onto yx-plane and subtract it from on-data
        fHistOff->GetZaxis()->SetRange(0,9999);
        TH2 *h=(TH2*)fHistOff->Project3D("yx_off");

        const Double_t h1off = GetOffSignal(*h1);
        const Double_t hoff  = GetOffSignal(fHistBg);

        scale = hoff==0 ? -1 : -h1off/hoff;

        hx = (TH2*)pad->GetPad(4)->FindObject("Alpha_yx_sig");
        if (hx)
        {
            hx->SetContour(99);
            MakeSignificance(*hx, *h1, fHistBg, TMath::Abs(scale));
            MakeDot(*hx);
            MakeSymmetric(hx);
        }

        h1->Add(&fHistBg, scale);

        MakeDot(*h1);
        MakeSymmetric(h1);

        delete h;
    }

    pad->cd(3);
    TH1 *h2 = (TH1*)gPad->FindObject("RadProf");

    TString opt(o);
    opt.ToLower();

    if (h1 && h2)
    {
        Int_t ix, iy, iz;
        h1->GetMaximumBin(ix, iy, iz);

        const Double_t x0 = h1->GetXaxis()->GetBinCenter(ix);
        const Double_t y0 = h1->GetYaxis()->GetBinCenter(iy);
        //const Double_t w0 = h1->GetXaxis()->GetBinWidth(1);

        Profile(*h2, *h1, 0, 0);
        //Profile(*h2, *h1, x0, y0);

        if (h2->GetEntries()>0)
        {
            // Replace with MAlphaFitter?
            TF1 func("fcn", "gaus + [3]*x*x + [4]");
            func.SetLineWidth(1);
            func.SetLineColor(kBlue);

            func.SetParLimits(2, h2->GetBinWidth(1), 1.0);

            func.SetParameter(0, h2->GetBinContent(1));
            func.FixParameter(1, 0);
            func.SetParameter(2, 0.12);
            if (fHistOff)
                func.FixParameter(3, 0);
            func.SetParameter(4, 0);//h2->GetBinContent(20));
            h2->Fit(&func, "IQ", "", 0, 1.0);

            h2->SetTitle(MString::Format("P=(%.2f\\circ/%.2f\\circ) \\omega=%.2f\\circ f=%.2f",
                                         x0, y0, func.GetParameter(2), TMath::Abs(scale)));
        }
    }

    pad->cd(5);
    if (h1)
        Profile1D("Exc", *h1);

    pad->cd(6);
    if (hx)
        Profile1D("Sig", *hx);
}

void MHDisp::Draw(Option_t *o)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    const Int_t col = pad->GetFillColor();
    pad->SetBorderMode(0);

    AppendPad(o);

    // ----- Pad number 4 -----
    TString name = MString::Format("%s_4", pad->GetName());
    TPad *p = new TPad(name,name, 0.525/*0.5025*/, 0.3355, 0.995, 0.995, col, 0, 0);
    p->SetNumber(4);
    p->Draw();
    p->cd();

    TH1 *h3 = fHist.Project3D("yx_sig");
    h3->SetTitle("Significance Map");
    h3->SetDirectory(NULL);
    h3->SetXTitle(fHist.GetXaxis()->GetTitle());
    h3->SetYTitle(fHist.GetYaxis()->GetTitle());
    h3->SetMinimum(0);
    h3->Draw("colz");
    h3->SetBit(kCanDelete);

    if (fHistOff)
        GetCatalog()->Draw("mirror same *");

    // ----- Pad number 1 -----
    pad->cd();
    name = MString::Format("%s_1", pad->GetName());
    p = new TPad(name,name, 0.005, 0.3355, 0.475/*0.4975*/, 0.995, col, 0, 0);
    p->SetNumber(1);
    p->Draw();
    p->cd();

    h3 = fHist.Project3D("yx_on");
    h3->SetTitle("Distribution of equivalent events");
    h3->SetDirectory(NULL);
    h3->SetXTitle(fHist.GetXaxis()->GetTitle());
    h3->SetYTitle(fHist.GetYaxis()->GetTitle());
    h3->SetMinimum(0);
    h3->Draw("colz");
    h3->SetBit(kCanDelete);

    if (fHistOff)
        GetCatalog()->Draw("mirror same *");

    // ----- Pad number 2 -----
    pad->cd();
    name = MString::Format("%s_2", pad->GetName());
    p = new TPad(name,name, 0.005, 0.005, 0.2485, 0.3315, col, 0, 0);
    p->SetNumber(2);
    p->Draw();
    p->cd();
    h3->Draw("surf3");

    // ----- Pad number 3 -----
    pad->cd();
    name = MString::Format("%s_3", pad->GetName());
    p = new TPad(name,name, 0.2525, 0.005, 0.4985, 0.3315, col, 0, 0);
    p->SetNumber(3);
    p->SetGrid();
    p->Draw();
    p->cd();

    const Double_t maxr = TMath::Hypot(h3->GetXaxis()->GetXmax(), h3->GetYaxis()->GetXmax());
    const Int_t    nbin = (h3->GetNbinsX()+h3->GetNbinsY())/2;
    TProfile *h = new TProfile("RadProf", "Radial Profile", nbin, 0, maxr);
    h->SetDirectory(0);
    //TH1F *h = new TH1F("RadProf", "Radial Profile", nbin, 0, maxr);
    //h->Sumw2();
    h->SetXTitle("\\vartheta [\\circ]");
    h->SetYTitle("<cts>/\\Delta R");
    h->SetBit(kCanDelete);
    h->Draw();

    // ----- Pad number 5 -----
    pad->cd();
    name = MString::Format("%s_5", pad->GetName());
    p = new TPad(name,name, 0.5025, 0.005, 0.7485, 0.3315, col, 0, 0);
    p->SetNumber(5);
    p->SetGrid();
    p->Draw();
    p->cd();

    h3 = new TH1D("Exc", "Distribution of excess events", 10, -1, 1);
    h3->SetDirectory(NULL);
    h3->SetXTitle("N");
    h3->SetYTitle("Counts");
    h3->Draw();
    h3->SetBit(kCanDelete);

    // ----- Pad number 6 -----
    pad->cd();
    name = MString::Format("%s_6", pad->GetName());
    p = new TPad(name,name, 0.7525, 0.005, 0.9995, 0.3315, col, 0, 0);
    p->SetNumber(6);
    p->SetGrid();
    p->Draw();
    p->cd();

    h3 = new TH1D("Sig", "Distribution of significances", 10, -1, 1);
    h3->SetDirectory(NULL);
    h3->SetXTitle("N");
    h3->SetYTitle("Counts");
    h3->Draw();
    h3->SetBit(kCanDelete);
}

// --------------------------------------------------------------------------
//
// The following resources are available:
//
//    MHDisp.Smearing: 0.1
//    MHDisp.Wobble:   on/off
//
Int_t MHDisp::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Int_t rc = MHFalseSource::ReadEnv(env, prefix, print);
    if (rc==kERROR)
        return kERROR;

    if (IsEnvDefined(env, prefix, "Smearing", print))
    {
        rc = kTRUE;
        SetSmearing(GetEnvValue(env, prefix, "Smearing", fSmearing));
    }
    if (IsEnvDefined(env, prefix, "Wobble", print))
    {
        rc = kTRUE;
        SetWobble(GetEnvValue(env, prefix, "Wobble", fWobble));
    }
    if (IsEnvDefined(env, prefix, "ScaleMin", print))
    {
        rc = kTRUE;
        SetScaleMin(GetEnvValue(env, prefix, "ScaleMin", fScaleMin));
    }
    if (IsEnvDefined(env, prefix, "ScaleMax", print))
    {
        rc = kTRUE;
        SetScaleMax(GetEnvValue(env, prefix, "ScaleMax", fScaleMax));
    }

    return rc;
}

