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
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// runbook.C
// =========
//
// This is an example macro how to extract the runbook information of
// a given date from the runbook files.
//
// The argument is a date in regexp format, eg.
//   .x runbook.C("2004-05-1[0-9]")
//
// With an MDirIter you can tell the macro which directories and files
// should be searched.
//
// The output are the runbook entries matching your query.
//
///////////////////////////////////////////////////////////////////////////

void Wrap(TString &str)
{
    // Wrap lines to 80 chars
    while (str.Length()>80)
    {
        TString ss = str(0, 80);
        Int_t l = ss.Last(' ');
        if (l<0)
            break;
        cout << str(0, l) << endl;
        str.Remove(0, l);
    }
    cout << str << endl;
}

void ProcessFile(TString fname, TString date)
{
    // File processed
    cout << fname << endl;

    // Open file
    ifstream fin(fname);

    TString line;

    int i=81;

    TRegexp r0("20[0-2][0-9]-[0-1][0-9]-[0-3][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9]");
    TRegexp r1(date+" [0-2][0-9]:[0-5][0-9]:[0-5][0-9]");

    int lcnt=0;

    int print = 0;

    while (!fin.eof())
    {
        // Read file line by line
        char *txt=new char[i];
        fin.getline(txt, i-1);

        if (lcnt++<4)
            continue;

        if (!fin)
            txt[i-1]=0;

        line += txt;
        delete txt;

        if (line.Length()==0)
        {
            if (print)
                cout << endl;
            continue;
        }

        if (!fin)
        {
            i *= 2;
            fin.clear();
            continue;
        }

        // Check whether line matches regexp
        if (!line(r0).IsNull())
            print = !line(r1).IsNull();

        // Wrap lines to 80 chars
        if (print)
            Wrap(line);
        line = "";
        i=81;
    }
    cout << "DONE." << endl;
}

void runbook(const char *d="20[0-2][0-9]-[0-1][0-9]-[0-3][0-9]")
{
    // Regexp to check for valid date
    TString regexp = "20[0-2][0-9]-[0-1][0-9]-[0-3][0-9]";

    if (regexp!=d)
    {
        TRegexp r(regexp);

        TString date=d;
        if (date(r).IsNull())
        {
            cout << "ERROR - Date has wrong format (2003-05-06)" << endl;
            return;
        }
    }

    // Tell which dierctories and files to search
    MDirIter Next;
    Next.AddDirectory("/home/MAGIC/online_data/runbook", "CC_*.rbk");

    // Loop over files
    TString name;
    while (1)
    {
        name=Next();
        if (name.IsNull())
            break;

        ProcessFile(name, d);
    }
}
