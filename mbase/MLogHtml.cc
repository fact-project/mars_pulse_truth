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
!   Author(s): Thomas Bretz, 1/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */


//////////////////////////////////////////////////////////////////////////////
//
// MLogHtml
//
//////////////////////////////////////////////////////////////////////////////
#include "MLogHtml.h"

#include <string.h>  // necessary for Fedora core 2 with kernel 2.6.9-1.667 #1 and gcc 3.4.2
#include <errno.h>   // necessary for Fedora core 2 with kernel 2.6.9-1.667 #1 and gcc 3.4.2

#include <fstream>  // ofstream
#include <iostream> // cout

#include "MTime.h"

ClassImp(MLogHtml);

using namespace std;

MLogHtml::MLogHtml(const char *name) : fUnderline(0), fColor(-1)
{
    fOut = new ofstream(name);
    if (!*fOut)
    {
        delete fOut;
        fOut = NULL;

        cerr << "Cannot open file " << name << ": " << strerror(errno) << endl;
        return;
    }

    // switch off buffering
    fOut->rdbuf()->pubsetbuf(0,0);

    MTime time;
    time.Now();

    *fOut << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">" << endl;
    *fOut << endl;
    *fOut << "<html>" << endl;
    *fOut << "<head>" << endl;
    *fOut << "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">" << endl;
    *fOut << "    <meta name=\"Author\" content=\"Mars-MLogHtml" << MARSVER << "\">" << endl;
    *fOut << "    <title>MARS - Logging, created " << time << "</title>" << endl;
    *fOut << "</head>" << endl;
    *fOut << endl;
    *fOut << "<pre>" << endl;
    //*fOut << "<body background=\"background.gif" text="#000000" bgcolor="#000099" link="#1122FF" vlink="#8888FF" alink="#FF0000">
}

MLogHtml::~MLogHtml()
{
    if (!fOut)
        return;

    *fOut << "</font>" << endl;
    *fOut << "</pre>" << endl;
    *fOut << endl;
    *fOut << "</html>" << endl;

    delete fOut;
}

void MLogHtml::Underline()
{
    if (!fOut)
        return;

    *fOut << "<u>";
    fUnderline = kTRUE;
}

void MLogHtml::SetColor(Int_t col)
{
    if (!fOut)
        return;

    if (fColor>0 && fColor!=col)
        *fOut << "</font>";

    if (fColor==col)
        return;

    switch (col)
    {
    case 0:  break;
    case 1:  *fOut << "<font color='maroon'>";  break;  // err
    case 2:  *fOut << "<font color='#FF6600'>"; break;  // warn (olive?)
    case 3:                                             // inf
    case 4:                                             // inf2
    case 5:  *fOut << "<font color='green'>";   break;  // inf3
    default: *fOut << "<font color='navy'>";    break;  // all others (dbg)
    }

    fColor=col;
}

void MLogHtml::WriteBuffer(const char *str, int len)
{
    if (!fOut)
    {
        cout.write(str, len);
        return;
    }

    TString txt(str, len);

    txt.ReplaceAll(">", "&gt;");
    txt.ReplaceAll("<", "&lt;");

    fOut->write(txt.Data(), txt.Length());
    if (fUnderline)
    {
        *fOut << "</u>";
        fUnderline = kFALSE;
    }
}
