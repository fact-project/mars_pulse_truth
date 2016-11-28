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
!   Author(s): Wolfgang Wittek 5/2002 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  MHFlux                                                                 //
//                                                                          //
//  calculates absolute photon fluxes                                       //
//             from the distributions of the estimated energy               //
//                      for the different bins in some variable 'Var'       //
//                      (Var = Theta or time)                               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include "MHFlux.h"

#include <TStyle.h>

#include <TF1.h>
#include <TH2.h>
#include <TProfile.h>

#include <TCanvas.h>

#include "MTime.h"

#include "MBinning.h"
#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MHThetabarTheta.h"
#include "MHEffOnTime.h"
#include "MHGamma.h"

ClassImp(MHFlux);

using namespace std;

MHFlux::MHFlux(const MHGamma &hist, const TString varname, const TString unit)
    : fHOrig(), fHUnfold(), fHFlux()
{
    const TH2D &h2d = *hist.GetProject();

    if (varname.IsNull() || unit.IsNull())
        *fLog << warn << dbginf << "varname or unit not defined" << endl;

    fVarname = varname;
    fUnit    = unit;

    // char txt[100];

    // original distribution of E-est for different bins 
    //                       of the variable (Theta or time)
    // sprintf(txt, "gammas vs. E-est and %s",varname);

    ((TH2D&)h2d).Copy(fHOrig);

    fHOrig.SetName("E_est");
    fHOrig.SetTitle(TString("No.of gammas vs. E-est and ")+fVarname);

    fHOrig.SetDirectory(NULL);
    fHOrig.SetXTitle("E_{est} [GeV]");
    fHOrig.SetYTitle(fVarname+fUnit);
    //fHOrig.Sumw2();

    SetBinning((TH2*)&fHOrig, (TH2*)&h2d);

    fHOrig.Copy(fHUnfold);

    // unfolded distribution of E-unfold for different bins 
    //                       of the variable (Theta or time)
    // sprintf(txt, "gammas vs. E-unfold and %s",varname);
    fHUnfold.SetName("E-unfolded");
    fHUnfold.SetTitle(TString("No.of gammas vs. E-unfold and ")+fVarname);

    fHUnfold.SetDirectory(NULL);
    fHUnfold.SetXTitle("E_{unfold} [GeV]");
    fHUnfold.SetYTitle(fVarname+fUnit);
    //fHUnfold.Sumw2();
    
    SetBinning((TH2*)&fHUnfold, (TH2*)&fHOrig);


    // absolute photon flux vs. E-unfold
    //          for different bins of the variable (Theta or time)
    //
    // sprintf(txt, "gamma flux [1/(s m2 GeV) vs. E-unfold and %s",varname);
    fHFlux.SetName("photon flux");
    fHFlux.SetTitle(TString("Gamma flux [1/(s m^2 GeV) vs. E-unfold and ")+fVarname);

    fHFlux.SetDirectory(NULL);
    fHFlux.SetXTitle("E_{unfold} [GeV]");
    fHFlux.SetYTitle(fVarname+fUnit);
    fHFlux.Sumw2();

    SetBinning((TH2*)&fHFlux, (TH2*)&fHUnfold);
}

// --------------------------------------------------------------------------
//
// Default Constructor. It sets the variable name (Theta or time)
//                      and the units for the variable
// 
MHFlux::MHFlux(const TH2D &h2d, const TString varname, const TString unit)
    : fHOrig(), fHUnfold(), fHFlux()
{
    if (varname.IsNull() || unit.IsNull())
        *fLog << warn << dbginf << "varname or unit not defined" << endl;

    fVarname = varname;
    fUnit    = unit;

    // char txt[100];

    // original distribution of E-est for different bins 
    //                       of the variable (Theta or time)
    // sprintf(txt, "gammas vs. E-est and %s",varname);

    ((TH2D&)h2d).Copy(fHOrig);

    fHOrig.SetName("E_est");
    fHOrig.SetTitle(TString("No.of gammas vs. E-est and ")+fVarname);

    fHOrig.SetDirectory(NULL);
    fHOrig.SetXTitle("E_{est} [GeV]");
    fHOrig.SetYTitle(fVarname+fUnit);
    //fHOrig.Sumw2();

    // copy fHOrig into fHUnfold in case no unfolding is done
    fHOrig.Copy(fHUnfold);

    SetBinning((TH2*)&fHOrig, (TH2*)&h2d);


    // unfolded distribution of E-unfold for different bins 
    //                       of the variable (Theta or time)
    // sprintf(txt, "gammas vs. E-unfold and %s",varname);
    fHUnfold.SetName("E-unfolded");
    fHUnfold.SetTitle(TString("No.of gammas vs. E-unfold and ")+fVarname);

    fHUnfold.SetDirectory(NULL);
    fHUnfold.SetXTitle("E_{unfold} [GeV]");
    fHUnfold.SetYTitle(fVarname+fUnit);
    //fHUnfold.Sumw2();
    
    SetBinning((TH2*)&fHUnfold, (TH2*)&fHOrig);


    // absolute photon flux vs. E-unfold
    //          for different bins of the variable (Theta or time)
    //
    // sprintf(txt, "gamma flux [1/(s m2 GeV) vs. E-unfold and %s",varname);
    fHFlux.SetName("photon flux");
    fHFlux.SetTitle(TString("Gamma flux [1/(s m^{2} GeV)] vs. E-unfold and ")+fVarname);

    fHFlux.SetDirectory(NULL);
    fHFlux.SetXTitle("E_{unfold} [GeV]");
    fHFlux.SetYTitle(fVarname+fUnit);
    fHFlux.Sumw2();

    SetBinning((TH2*)&fHFlux, (TH2*)&fHUnfold);
}

// -------------------------------------------------------------------------
//
// Unfold the distribution in E-est
//
void MHFlux::Unfold()
{
}

void MHFlux::CalcFlux(const MHEffOnTime &teff, const MHThetabarTheta &thetabar,
                      const TH2D *aeff)
{
    CalcFlux(teff.GetHist(), thetabar.GetHist(), aeff);
}

Double_t MHFlux::ParabInterpolLog(const TAxis &axe, Int_t j,
                                  Double_t y[], Double_t Ebar) const
{
    const double t1 = log10(axe.GetBinLowEdge(j-1)) + log10(axe.GetBinUpEdge(j-1));
    const double t2 = log10(axe.GetBinLowEdge(j))   + log10(axe.GetBinUpEdge(j));
    const double t3 = log10(axe.GetBinLowEdge(j+1)) + log10(axe.GetBinUpEdge(j+1));

    const Double_t lebar = log10(Ebar);

    return Parab(t1/2, t2/2, t3/2, y[j-2], y[j-1], y[j], lebar);
}

// --------------------------------------------------------------------
//
//  determine bins for interpolation (k3 is the middle one) in bar.
//  k0 denotes the bin from which the error is copied
//
void MHFlux::FindBins(const TAxis &axe, const Double_t bar, Int_t &k3, Int_t &k0) const
{
    const Int_t n = axe.GetNbins();

    k3 = axe.FindFixBin(bar);
    k0 = k3;

    if (k3<2)
    {
        k3 = 2;
        if (bar<axe.GetBinLowEdge(2))
            k0 = 1;
    }

    if (k3>n-1)
    {
        k3 = n-1;
        if (bar>axe.GetBinLowEdge(n))
            k0 = n;
    }

    if (bar>=axe.GetBinLowEdge(1) && bar<=axe.GetBinUpEdge(n))
        return;

    *fLog << dbginf << "extrapolation: bar = " << bar;
    *fLog << ", min =" << axe.GetBinLowEdge(1);
    *fLog << ", max =" << axe.GetBinUpEdge(n) << endl;
}

Double_t MHFlux::ParabInterpolCos(const TAxis &axe, const TH2D *aeff, Int_t j, Int_t k3, Double_t val) const
{
    const double t1 = cos( axe.GetBinCenter (k3-1) );
    const double t2 = cos( axe.GetBinCenter (k3)   );
    const double t3 = cos( axe.GetBinCenter (k3+1) );

    const double a1 = aeff->GetBinContent(j, k3-1);
    const double a2 = aeff->GetBinContent(j, k3);
    const double a3 = aeff->GetBinContent(j, k3+1);

    return Parab(t1, t2, t3, a1, a2, a3, val);
}

// -------------------------------------------------------------------------
//
// Calculate photon flux by dividing the distribution in Eunf (fHUnfold) by
//                       the width of the energy interval     (deltaE)
//                       the effective ontime                 (*teff)
//                       and the effective collection area    (*aeff)
//
void MHFlux::CalcFlux(const TH1D *teff, const TProfile *thetabar,
                      const TH2D *aeff)
{
    //
    // Note that fHUnfold  has bins in Eunf and Var
    //           *teff     has bins in Var  (the same bins in Var as fHUnfold)
    //           *thetabar has bins in Var  (the same bins in Var as fHUnfold)
    //           *aeff     has bins in Etru and Theta
    //                     (where in general the binning in Etru is different
    //                      from the binning in Eunf)
    // The variable Var may be 'time' or 'Theta'

    const TAxis &axex = *((TH2*)aeff)->GetXaxis();
    const TAxis &axey = *((TH2*)aeff)->GetYaxis();

    if (axex.GetNbins()<3)
    {
        *fLog << err << "ERROR - Number of Energy bins <3 not implemented!" << endl;
        return;
    }

    if (axey.GetNbins()<3)
        *fLog << warn << "WARNING - Less than 3 theta-bins not supported very well!" << endl;

    //
    // calculate effective collection area
    //    for the Eunf and Var bins of the histogram fHUnfold
    //    from the histogram *aeff, which has bins in Etru and Theta
    // the result is the histogram fHAeff
    //
    TH2D fHAeff;
    SetBinning((TH2*)&fHAeff, (TH2*)&fHUnfold);
    fHAeff.Sumw2();

    //
    // ------ start loops ------
    //
    const Int_t nEtru = aeff->GetNbinsX();

    Double_t *aeffbar  = new Double_t[nEtru];
    Double_t *daeffbar = new Double_t[nEtru];

    const Int_t nVar = fHFlux.GetNbinsY();
    for (int n=1; n<=nVar; n++)
    {
        const Double_t tbar = thetabar->GetBinContent(n);
        const Double_t costbar = cos(tbar/kRad2Deg);

        // determine bins for interpolation (k3, k0)
        Int_t kv, ke;
        FindBins(axey, tbar, kv, ke);

        //
        // calculate effective collection area at Theta = Thetabar
        // by quadratic interpolation in cos(Theta);
        // do this for each bin of Etru
        //
        for (int j=1; j<=nEtru; j++)
        {
            if (axey.GetNbins()<3)
            {
                // FIXME: Other interpolation?
                aeffbar[j-1]  = aeff->GetBinContent(j, n);
                daeffbar[j-1] = aeff->GetBinError(j, n);
            }
            else
            {
                aeffbar[j-1]  = ParabInterpolCos(axey, aeff, j, kv, costbar);
                daeffbar[j-1] = aeff->GetBinError(j, ke);
            }
        }

        //
        // calculate effective collection area at (E = Ebar, Theta = Thetabar)
        // by quadratic interpolation in log10(Etru)
        // do this for each bin of Eunf
        //
        CalcEffCol(axex, fHAeff, n, aeffbar, daeffbar);
    }

    delete aeffbar;
    delete daeffbar;

    //
    // now calculate the absolute gamma flux
    //
    CalcAbsGammaFlux(*teff, fHAeff);
}

// --------------------------------------------------------------------
//
//  calculate effective collection area at (E = Ebar, Theta = Thetabar)
//  by quadratic interpolation in log10(Etru)
//  do this for each bin of Eunf
//
void MHFlux::CalcEffCol(const TAxis &axex, TH2D &fHAeff, Int_t n, Double_t aeffbar[], Double_t daeffbar[])
{
    const Int_t nEunf = fHFlux.GetNbinsX();

    const TAxis &unfx = *fHUnfold.GetXaxis();

    for (int m=1; m<=nEunf; m++)
    {
        const Double_t Ebar = GetBinCenterLog(unfx, m);

        Int_t j0, j3;
        FindBins(axex, Ebar, j3, j0);

        const Double_t v = ParabInterpolLog(axex, j3, aeffbar, Ebar);

        fHAeff.SetBinContent(m,n, v);
        fHAeff.SetBinError(m,n, daeffbar[j0-1]);
    }
}

// --------------------------------------------------------------------
//
//  calculate the absolute gamma flux
//
void MHFlux::CalcAbsGammaFlux(const TH1D &teff, const TH2D &fHAeff)
{
    const Int_t nEunf = fHFlux.GetNbinsX();
    const Int_t nVar  = fHFlux.GetNbinsY();

    for (int m=1; m<=nEunf; m++)
    {
        const Double_t DeltaE = fHFlux.GetXaxis()->GetBinWidth(m);

        for (int n=1; n<=nVar; n++)
        {
            const Double_t Ngam  = fHUnfold.GetBinContent(m,n);
            const Double_t Aeff  = fHAeff.GetBinContent(m,n);
            const Double_t Effon = teff.GetBinContent(n);

            const Double_t c1 = fHUnfold.GetBinError(m,n)/Ngam;
            const Double_t c2 = teff.GetBinError(n)      /Effon;
            const Double_t c3 = fHAeff.GetBinError(m,n)  /Aeff;

            const Double_t cont  = Ngam / (DeltaE * Effon * Aeff);
            const Double_t dcont = sqrt(c1*c1 + c2*c2 + c3*c3);

            //
            // Out of Range
            //
            const Bool_t oor = Ngam<=0 || DeltaE<=0 || Effon<=0 || Aeff<=0;

            if (oor)
                *fLog << warn << "MHFlux::CalcAbsGammaFlux(" << m << "," << n << ") ";

            if (Ngam<=0)
                *fLog << " Ngam=0";
            if (DeltaE<=0)
                *fLog << " DeltaE=0";
            if (Effon<=0)
                *fLog << " Effon=0";
            if (Aeff<=0)
                *fLog << " Aeff=0";

            if (oor)
                *fLog << endl;

            fHFlux.SetBinContent(m,n, oor ? 1e-20 : cont);
            fHFlux.SetBinError(m,n,   oor ? 1e-20 : dcont*cont);
        }
    }
}

// --------------------------------------------------------------------
//
// draw the differential photon flux vs. E-unf
// for the individual bins of the variable Var
//
void MHFlux::DrawFluxProjectionX(Option_t *opt) const
{
    const Int_t nVar = fHFlux.GetNbinsY();

    for (int n=1; n<=nVar; n++)
    {
        TString strg0("Flux-");

        TH1D &h = *((TH2D)fHFlux).ProjectionX(strg0+fVarname, n, n, "E");

        TString strg1 = "Photon flux vs. E_{unfold} for ";
        TString strg2 = fVarname+"-bin #";
        strg2 += n;

        new TCanvas(strg2, strg1+strg2);
        gPad->SetLogx();
        gPad->SetLogy();

        TString name = fVarname+"bin_";
        name += n;

        h.SetName(name);
        h.SetTitle(strg1+strg2);
        h.SetXTitle("E_{unfold} [GeV]");
        h.SetYTitle("photons / (s m^{2} GeV)");
        h.GetXaxis()->SetLabelOffset(-0.025);
        h.GetXaxis()->SetTitleOffset(1.1);
        h.GetXaxis()->SetNdivisions(1);
        h.GetYaxis()->SetTitleOffset(1.25);
        h.DrawCopy();
     }
}

void MHFlux::DrawOrigProjectionX(Option_t *opt) const
{
    const Int_t nVar = fHOrig.GetNbinsY();

    for (int n=1; n<=nVar; n++)
    {
        TString strg0 = "Orig-";
        strg0 += fVarname;
        strg0 += "_";
        strg0 += n;

        TH1D &h = *((TH2D)fHOrig).ProjectionX(strg0, n, n, "E");

        TString strg1("No.of photons vs. E-est for ");
        strg1 += fVarname+"-bin ";
        strg1 += n;

        new TCanvas(strg0, strg1);

        gPad->SetLogx();
        gPad->SetLogy();

        h.SetName(strg0);
        h.SetTitle(strg1);
        h.SetXTitle("E_{est} [GeV]");
        h.GetXaxis()->SetLabelOffset(-0.025);
        h.GetXaxis()->SetTitleOffset(1.1);
        h.GetXaxis()->SetNdivisions(1);
        h.SetYTitle("No.of photons");
        h.DrawCopy();
    }
}

// -------------------------------------------------------------------------
//
//  Draw the histograms
//
void MHFlux::Draw(Option_t *opt)
{
    TVirtualPad *pad = gPad ? gPad : MakeDefCanvas(this);
    pad->SetBorderMode(0);

    AppendPad("");

    pad->Divide(2,2);

    pad->cd(1);
    gPad->SetBorderMode(0);
    fHOrig.Draw(opt);

    pad->cd(2);
    gPad->SetBorderMode(0);
    fHUnfold.Draw(opt);

    pad->cd(3);
    gPad->SetBorderMode(0);
    fHFlux.Draw(opt);

    pad->Modified();
    pad->Update();
}

Double_t MHFlux::Parab(Double_t x1, Double_t x2, Double_t x3,
                       Double_t y1, Double_t y2, Double_t y3,
                       Double_t val)
{
    Double_t c0, c1, c2;
    Parab(x1, x2, x3, y1, y2, y3, &c0, &c1, &c2);
    return c0 + c1*val + c2*val*val;
}

// -------------------------------------------------------------------------
//
// Quadratic interpolation
//
// *** calculate the parameters of a parabula 
//                      y = a + b*x + c*x**2 = F(x)
//     such that       yi = F(xi)       for (i=1,3)
//
Bool_t MHFlux::Parab(Double_t x1, Double_t x2, Double_t x3,
                     Double_t y1, Double_t y2, Double_t y3,
                     Double_t *a, Double_t *b, Double_t *c)
{
    const double det =
        + x2*x3*x3 + x1*x2*x2 + x3*x1*x1
        - x2*x1*x1 - x3*x2*x2 - x1*x3*x3;

    if (det==0)
    {
        *a = 0;
        *b = 0;
        *c = 0;
        return kFALSE;
    }

    const double det1 = 1.0/det;

    const double ai11 = x2*x3*x3 - x3*x2*x2;
    const double ai12 = x3*x1*x1 - x1*x3*x3;
    const double ai13 = x1*x2*x2 - x2*x1*x1;

    const double ai21 = x2*x2 - x3*x3;
    const double ai22 = x3*x3 - x1*x1;
    const double ai23 = x1*x1 - x2*x2;

    const double ai31 = x3 - x2;
    const double ai32 = x1 - x3;
    const double ai33 = x2 - x1;

    *a = (ai11*y1 + ai12*y2 + ai13*y3) * det1;
    *b = (ai21*y1 + ai22*y2 + ai23*y3) * det1;
    *c = (ai31*y1 + ai32*y2 + ai33*y3) * det1;

    return kTRUE;
}
