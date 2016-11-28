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
!   Author(s): Thomas Bretz    12/2000 <mailto:tbretz@uni-sw.gwdg.de>
!   Author(s): Rudolf Bock     10/2001 <mailto:Rudolf.Bock@cern.ch>
!
!   Copyright: MAGIC Software Development, 2000-2002
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MSrcPosCam
//
// Storage Container to hold the current position of the source (or
// anti/false source) in the camera plain
//
//////////////////////////////////////////////////////////////////////////////
#include "MSrcPosCam.h"

#include "fits.h"

#include <TMarker.h>
#include <TVector2.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MSrcPosCam);

using namespace std;

static const TString gsDefName  = "MSrcPosCam";
static const TString gsDefTitle = "Virtual source position in the camera";

// --------------------------------------------------------------------------
//
// Default constructor.
//
MSrcPosCam::MSrcPosCam(const char *name, const char *title) : fX(0), fY(0)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
// Copy constructor, set default name and title
//
MSrcPosCam::MSrcPosCam(const MSrcPosCam &p) : fX(p.fX), fY(p.fY)
{
    fName  = gsDefName.Data();
    fTitle = gsDefTitle.Data();
}

// -----------------------------------------------------------------------
//
// Print contents
//
void MSrcPosCam::Print(Option_t *) const
{
    *fLog << all;
    *fLog << "Source position in the camera plain (" << GetName() << ")" << endl;
    *fLog << " - x [mm] = " << fX << endl;
    *fLog << " - y [mm] = " << fY << endl;
}

// -----------------------------------------------------------------------
//
// Paint contents
//
void MSrcPosCam::Paint(Option_t *)
{
    TMarker m;
    m.SetMarkerStyle(kStar);
    m.SetMarkerColor(kBlack);
    m.DrawMarker(fX, fY);
}

// -----------------------------------------------------------------------
//
// Set fX to v.X() and fY to v.Y()
//
void MSrcPosCam::SetXY(const TVector2 &v)
{
    fX = v.X();
    fY = v.Y();
}

// -----------------------------------------------------------------------
//
// Add v.X() to fX and v.Y() to fY
//
void MSrcPosCam::Add(const TVector2 &v)
{
    fX+=v.X();
    fY+=v.Y();
}

// -----------------------------------------------------------------------
//
//    return TMath::Hypot(fX, fY);
//
Float_t MSrcPosCam::GetDist() const
{
    return TMath::Hypot(fX, fY);
}

// -----------------------------------------------------------------------
//
// Get TVector2(fX, fY)
//
TVector2 MSrcPosCam::GetXY() const
{
    return TVector2(fX, fY);
}

/*
// -----------------------------------------------------------------------
//
// overloaded MParContainer to read MSrcPosCam from an ascii file
//
void MSrcPosCam::AsciiRead(ifstream &fin)
{
    fin >> fX;
    fin >> fY;
}

// -----------------------------------------------------------------------
//
// overloaded MParContainer to write MSrcPosCam to an ascii file
//
void MSrcPosCam::AsciiWrite(ofstream &fout) const
{
    fout << fX << " " << fY;
}
*/

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MSrcPosCam::StreamPrimitive(ostream &out) const
{
    out << "   MSrcPosCam " << GetUniqueName();
    if (fName!=gsDefName)
    {
        out << "(\"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\"";
        out <<")";
    }
    out << ";" << endl;

    out << "   " << GetUniqueName() << ".SetXY(" << fX << ", " << fY << ");" << endl;
}

Bool_t MSrcPosCam::SetupFits(fits &fin)
{
    if (!fin.SetRefAddress(Form("%s.fX", fName.Data()), fX)) return kFALSE;
    if (!fin.SetRefAddress(Form("%s.fY", fName.Data()), fY)) return kFALSE;

    return kTRUE;
}
