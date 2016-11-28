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
!   Author(s): Thomas Bretz, 04/2007 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MMovieData
//
/////////////////////////////////////////////////////////////////////////////
#include "MMovieData.h"

#include <TSpline.h>

ClassImp(MMovieData);

using namespace std;

class MSpline3 : public TSpline3
{
public:
   MSpline3(const char *title,
            const TGraph *g, const char *opt=0,
            Double_t valbeg=0, Double_t valend=0)
        : TSpline3(title, g, opt, valbeg, valend)
   {
   }

   Double_t GetXmin() const { return fXmin; }     // Minimum value of abscissa
   Double_t GetXmax() const { return fXmax; }     // Maximum value of abscissa
};

// --------------------------------------------------------------------------
//
// Creates a MSignalPix object for each pixel in the event
//
MMovieData::MMovieData(const char *name, const char *title)
{
    fName  = name  ? name  : "MMovieData";
    fTitle = title ? title : "Storage container for movie data";

    fSplines.SetOwner();
}

// --------------------------------------------------------------------------
//
// Reset, deletes all stored splines
//
void MMovieData::Reset()
{
    fSplines.Delete();
}

// --------------------------------------------------------------------------
//
// Add a new TSpline3 from a TGraph
//
void MMovieData::Add(const TGraph &g)
{
    TSpline *sp = new MSpline3(g.GetName(), &g, "b2 e2");
    fSplines.Add(sp);
}

// --------------------------------------------------------------------------
//
// Check whether tm would extra- or interpolate. kTRUE means tm will
// be interpolated.
//
Bool_t MMovieData::CheckRange(Int_t idx, Double_t tm) const
{
    const MSpline3 &sp = (*this)[idx];
    return tm>=sp.GetXmin() && tm<=sp.GetXmax();
}

// --------------------------------------------------------------------------
//
// Return the value of the idx-th spline at x=tm
//
Double_t MMovieData::Eval(Int_t idx, Double_t tm) const
{
    return (*this)[idx].Eval(tm);
}
