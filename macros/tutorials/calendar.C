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
!   Author(s): Thomas Bretz 8/2006 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004-2006
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// calendar.C
//
// example of how to use MCalendar to draw your calendar
//
// For more details of the usage and the resources resources please
// refer to the class reference of MCalendar and the descriptions
// of its member functions.
//
// To write an output file check the end of the loop.
//
// For more information on the setup see also
//   macros/tutorials/calendar.rc
//   macros/tutorials/holidays.rc
//
///////////////////////////////////////////////////////////////////////////

void calendar()
{
    // 2, 10, 13
    gStyle->SetTextFont(6*10+2);

    // Define the path where your resource files are located
    TString rcpath  = "macros/tutorials/";

    // Define the path where your images are
    TString imgpath = "./";

    // Create the object containing your setup
    MEnv env(rcpath+"calendar.rc");

    // Create the display to show your calendar
    MStatusDisplay *d = new MStatusDisplay(1426, 1050);

    // Define the first and last month which you want to make a
    // piece of
    Int_t first=1;
    Int_t last=12;

    // Loop over all months
    for (Int_t month=first; month<=last; month++)
    {
        // Create an object do draw your calendar
        MCalendar *cal=new MCalendar;
        cal->SetBit(kCanDelete);

        // Set the month and year you want to draw it for
        cal->SetDate(month, 2006);

        // Read resource file defining the layout.
        // You could also skip this line. In this case a nice
        // default layout is used instead
        cal->ReadResourceFile(rcpath+"calendar.rc");

        // Read file containing the holidays.
        // You could also skip this file. In this case
        // only New Year, Easter and Christmas are shown.
        cal->ReadHolidayFile(rcpath+"holidays.rc");

        // Get the image corresponding to the month
        TASImage *img = cal->GetImage(&env, imgpath);
        if (!img)
        {
            delete cal;
            continue;
        }

        // Check whether the layout should be horizontal or vertical
        Bool_t hor = img->GetWidth()>img->GetHeight();
        Int_t background = kWhite;

        // Define layout of day-table for this month
        if (hor)
            cal->SetLayout(2, 0.05, 0.1, 0.015, 0.03);
        else
            cal->SetLayout(1, 0.05, 0.05, 0.005, 0);

        // Get the name of the month
        TString strmonth = cal->GetStringFmt("%b");
        // convert Umlauts to "StatusDisplay-charset"
        MCalendar::ConvertUTF8(strmonth, kFALSE);

        // Add a new tab for this month
        TCanvas &c = d->AddTab(strmonth);
        // Set background color for the tab
        c.SetFillColor(background);

        // conver month name to latex format
        MCalendar::Convert2Latex(strmonth);

        // Create the main pad
        TPad *pad2;
        if (hor)
            pad2=new TPad("Dat", "Data", 0, 0.01, 1, 0.3);
        else
            pad2=new TPad("Dat", "Data", 0.5, 0.19, 1, 0.81);
        pad2->SetBorderMode(0);
        pad2->SetFillColor(background);
        pad2->SetBit(kCanDelete);

        // Draw the image to your month-tab
        c.cd();

        if (hor)
            cal->DrawImage(*img, 0, 0.305, 0.628, 1, kTRUE);
        else
            cal->DrawImage(*img, 0, 0, 0.5, 0.99, kTRUE);

        gPad->SetFixedAspectRatio();
        delete img;

        // Draw the date to your month-tab
        Double_t x1, y1, x2, y2;
        gPad->GetPadPar(x1, y1, x2, y2);

        c.cd();

        // "comic" is the ttf-font name. You find other ones typically
        // in $ROOTSYS/fonts or /usr/X11R6/lib/X11/fonts/truetype
        if (hor)
            cal->DrawTTFDate(0.64, 0.83, 1, 1, "%B %Y", 0.072, "comic");
        else
            cal->DrawTTFDate(0.5, 0.83, 1, 1, "%B %Y", 0.085, "comic");


        c.cd();

        // Get description and draw description
        TString txt = env.GetValue(Form("%02d.Text", month), "");
        if (!txt.IsNull())
        {
            MCalendar::Convert2Latex(txt);

            TAttText att1(11, 0, kBlack, 42, 0.023);
            if (hor)
                cal->DrawDate(0.63, 0.31, txt, att1);
            else
                cal->DrawDate(0.5, 0.01, txt, att1);

        }

        // Now make sure that everything is displayed as expected...
        pad2->Draw();

        gROOT->SetSelectedPad(0);
        pad2->cd();
        cal->Draw();

        pad2->Modified();
        pad2->Update();

        c.Update();

        // Uncomment this lines to write a postscript (or pdf) file
        //TString psfile = "calendar.ps";
        //if (month==first) psfile+="(";
        //if (month==last)  psfile+=")";
        //c.Print(psfile, "portrait");
    }
}
