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
!   Author(s): Thomas Bretz, 2/2007 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

void mirrordelay()
{
    TF1 fpar("Parab",  "x*x/(4*[0])");
    TF1 fsph("Sphere", "-sqrt(4*[0]*[0] - x*x) + 2*[0]");

    Double_t F = 4.92; // Fokallaenge = 1/4a
    Double_t D = 4.5;  // Mirror Diameter
    Double_t x = 1.85; // incident point for photon
                                     
    fsph.SetRange(-D*0.55, D*0.55);
    fpar.SetRange(-D*0.55, D*0.55);

    fpar.SetParameter(0, F);
    fsph.SetParameter(0, F);

    fpar.SetMaximum(F*1.05);

    TCanvas *c = new TCanvas;
    c->SetBorderMode(0);
    c->SetFrameBorderMode(0);
    c->SetFillColor(kWhite);

    TF1 *f = (TF1*)fpar.DrawClone();
    fsph.DrawClone("same");

    f->GetXaxis()->SetTitle("x [m]");
    f->GetYaxis()->SetTitle("y [m]");
    f->GetXaxis()->CenterTitle();

    TArrow l;
    l.SetLineColor(kBlue);
    l.DrawArrow(-x, F*1.05, -x, fsph.Eval(-x), 0.02);
    l.DrawArrow( x, F*1.05,  x, fsph.Eval( x), 0.02);
    l.SetLineColor(kRed);
    l.DrawArrow(-x, fsph.Eval(-x), -x, fpar.Eval(-x), 0.02);
    l.DrawArrow( x, fsph.Eval(-x),  x, fpar.Eval( x), 0.02);
    l.DrawArrow(-x, fpar.Eval(-x), 0, F, 0.02);
    l.DrawArrow(-x, fsph.Eval(-x), 0, F, 0.02);
    l.DrawArrow( x, fpar.Eval( x), 0, F, 0.02);
    l.DrawArrow( x, fsph.Eval( x), 0, F, 0.02);

    TMarker m;
    m.SetMarkerStyle(kStar);
    m.DrawMarker(0, F);

    // Plot run-time different versus distance

    c = new TCanvas;
    c->SetBorderMode(0);
    c->SetFrameBorderMode(0);
    c->SetFillColor(kWhite);
    c->SetGridx();
    c->SetGridy();
    c->SetRightMargin(0.01);

    TLegend leg(0.40, 0.6, 0.60, 0.85, "Focal length");//, "NDC");

    for (int i=-2; i<3; i++)
    {
        // Make sure to set the parameters BEFORE reusing the function!
        fsph.SetParameter(0, F+i*0.10*F);
        fpar.SetParameter(0, F+i*0.10*F);

        TF1 fdel1("Diff1", "Sphere-Parab");
        TF1 fdel2("Diff2", "TMath::Hypot(x-0, Parab-[0])-TMath::Hypot(x-0, Sphere-[0])");
        fdel2.SetParameter(0, F+i*0.10*F);

        TF1 fdel("Diff", "(Diff1+Diff2)/3e8*1e9");
        fdel.SetRange(-D*0.55, D*0.55);
        fdel.SetLineWidth(2);
        fdel.SetParameter(0, F+i*0.10*F);

        fdel.SetLineColor(kBlack+(abs(i)==2?3:abs(i)));
        if (i<0)
            fdel.SetLineStyle(2);
        if (i==0)
            fdel.SetLineStyle(kSolid);
        if (i>0)
            fdel.SetLineStyle(7);

        f = (TF1*)fdel.DrawClone(i==-2?"":"same");

        f->GetHistogram()->SetTitle("Arrival time delay for a spherical mirror");

        f->GetXaxis()->SetTitle("r [m]");
        f->GetYaxis()->SetTitle("Delay  \\Delta t [ns]");
        f->GetXaxis()->CenterTitle();
        f->GetYaxis()->CenterTitle();
        f->GetYaxis()->SetTitleOffset(1.3);

        leg.AddEntry(f, Form("F=%.1fm", F+i*0.10*F));
    }

    leg.DrawClone();
}
