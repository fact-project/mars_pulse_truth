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
!   Author(s): Thomas Bretz 10/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004-2006
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// starvisday.C
//
// displays the star visibility of several stars for one day
//
// See the code for ducumentation and setup
//
///////////////////////////////////////////////////////////////////////////

void zoom()
{
    TH1 *h = (TH1*)gPad->FindObject("frame");
    if (!h)
        return;

    Int_t mjd = TMath::FloorNint(h->GetXaxis()->GetXmin());

    h->GetXaxis()->SetRangeUser(mjd+0.33*3600*24, mjd+0.85*3600*24);
    h->GetYaxis()->SetRangeUser(35, 90);

    gPad->Modified();
    gPad->Update();
}


void starvisday()
{
    // Date (UTC) for which to get the visibility plot
    MTime time(-1); // -1 means NOW

    // Setup a different time as you like
    // time.Set(2006, 12, 20);

    // Current observatory (see class reference of MObservatory)
    const MObservatory obs(MObservatory::kMagic1);

    // The class contaning the catalog
    MAstroCatalog stars;
    stars.SetObservatory(obs);
    stars.SetTime(time);

    // Read the stars from a star catalog. The catalogs can be downloaded
    // from the Mars web page. For more information see class reference.
    stars.ReadXephem("magic_favorites.edb");

    // Mark the stars you would like to display (see the names in the catalog)
    stars.MarkObject("CrabNebula");
    stars.MarkObject("1ES1218+304");
    stars.MarkObject("1ES1426+428");
    stars.MarkObject("Mrk421");
    // stars.MarkObject("Mrk501");
    // stars.MarkObject("1ES1959+650");
    // stars.MarkObject("1ES2344+514");
    stars.MarkObject("M87");

    // --------------------------------------------------------------------------
    //
    //  Start producing the nice plot
    //

    // open and setup a new canvas
    TCanvas *c = new TCanvas;
    c->SetBorderMode(0);
    c->SetFillColor(kWhite);
    c->SetFrameBorderMode(0);
    c->SetFrameFillStyle(0);

    c->SetGridx();
    c->SetGridy();

    // Setup the TGraph which is drawn
    TGraph g;
    g.SetTitle(Form("Visibility at %s, %s (MOON=%.1f%%)",
                    obs.GetObservatoryName().Data(),
                    time.GetStringFmt("%A %e.%B %Y").Data(),
                    MAstro::GetMoonPhase(time.GetMjd())*100)
              );
    g.SetLineWidth(2);

    // ===================
    // Set graph name to object name
    g.SetName("Moon");

    // produce visibility curve
    stars.GetVisibilityCurve(g, "Moon");

    // Draw Frame
    TH1F *hist = g.GetHistogram();
    hist->SetName("frame");
    hist->DrawCopy();

    // Get x-range
    TAxis *axex = g.GetXaxis();
    Float_t mn = axex->GetBinLowEdge(axex->GetFirst());
    Float_t mx = axex->GetBinUpEdge(axex->GetLast());

    // Draw the twilight and dark time
    TBox box;

    for (int i=3; i>=0; i--)
    {
        double set = obs.GetSunRiseSet(time.GetMjd()-1, -6*i)[1];
        double ris = obs.GetSunRiseSet(time.GetMjd(),   -6*i)[0];

        box.SetFillStyle(3001);
        box.SetFillColor(kGray+3-i);
        box.DrawBox(mn, 5, MTime(set).GetAxisTime(), 90);
        box.DrawBox(MTime(ris).GetAxisTime(), 5, mx, 90);
        box.DrawBox(MTime(set).GetAxisTime(), 15+(i+1)*10,
                    MTime(ris).GetAxisTime(),  5)+i*10;
    }

    // Draw TGraph for Moon
    g.SetFillStyle(3003);
    g.SetFillColor(kRed+1);
    g.SetLineColor(kRed+1);
    g.SetLineWidth(1);
    g.SetLineStyle(kDotted);
    g.DrawClone("bc")->SetBit(kCanDelete);
    g.SetLineWidth(2);

    // ===================

    // Some helper to iterate these three colors
    Int_t col[] = { kBlack, kBlue, kRed+1, kMagenta+1, kGreen+2 };

    // Loop over all stars in the catalog
    int k=0;
    TIter Next(stars.GetList());
    TObject *o=0;
    while ((o=Next()))
    {
        // Check if the star was previously marked
        if (!o->TestBit(1<<14))
            continue;

        // Set graph name to object name
        g.SetName(o->GetName());

        // produce visibility curve
        stars.GetVisibilityCurve(g);

        // Setup color and style of TGraph
        g.SetLineColor(col[k%5]);
        g.SetLineStyle(1+k/3);

        // Draw TGraph
        g.DrawClone("c")->SetBit(kCanDelete);

        // Search for culmination
        Long64_t max = TMath::LocMax(g.GetN(), g.GetY());

        // Setup description
        TText txt;
        txt.SetTextFont(102);
        txt.SetTextSize(0.03);
        txt.SetTextColor(g.GetLineColor());

        // draw name at culmination
        txt.DrawText(g.GetX()[max]-1.0*3600, g.GetY()[max]+1, o->GetName());

        k++;
    }

    c->Modified();
    c->Update();

    TButton *but = new TButton("zoom", "zoom()", 0.88, 0.95, 0.99, 0.99);
    but->Draw();
}
