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
!   Author(s): Harald Kornmayer 1/2001
!
!   Copyright: Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MHexagon
//
// Class Version 2:
//  - added fPhi
//
//////////////////////////////////////////////////////////////////////////////

#include "MHexagon.h"

#include <fstream>
#include <iostream>

#include <TVector2.h>       // TVector2
#include <TVirtualPad.h>    // gPad
#include <TOrdCollection.h> // TOrdCollection

#include "MLog.h"
#include "MLogManip.h"

#include "MMath.h"

ClassImp(MHexagon);

using namespace std;

const Double_t MHexagon::fgCos60 = 0.5;        // TMath::Cos(60/TMath::RadToDeg());
const Double_t MHexagon::fgSin60 = sqrt(3.)/2; // TMath::Sin(60/TMath::RadToDeg());

const Double_t MHexagon::fgDx[6] = { fgCos60,   0.,          -fgCos60,  -fgCos60,   0.,            fgCos60 };
const Double_t MHexagon::fgDy[6] = { fgSin60/3, fgSin60*2/3, fgSin60/3, -fgSin60/3, -fgSin60*2/3, -fgSin60/3 };

// ------------------------------------------------------------------------
//
//   default constructor for MHexagon
//
MHexagon::MHexagon()
{
}

// ------------------------------------------------------------------------
//
//    normal constructor for MHexagon
//
MHexagon::MHexagon(Float_t x, Float_t y, Float_t d, Float_t phi)
: TAttLine(1, 1, 1), TAttFill(0, 1001), fX(x), fY(y), fD(d), fPhi(phi)
{
}

// ------------------------------------------------------------------------
//
//    copy constructor for MHexagon
//
MHexagon::MHexagon(const MHexagon &hexagon) : TObject(hexagon), TAttLine(hexagon), TAttFill(hexagon)
{
    fX   = hexagon.fX;
    fY   = hexagon.fY;
    fD   = hexagon.fD;
    fPhi = hexagon.fPhi;
} 

// ------------------------------------------------------------------------
//
//     copy this hexagon to hexagon
//
void MHexagon::Copy(TObject &obj)
#if ROOT_VERSION_CODE > ROOT_VERSION(3,04,01)
const
#endif
{
    MHexagon &hex = (MHexagon&) obj;

    TObject::Copy(obj);
    TAttLine::Copy(hex);
    TAttFill::Copy(hex);

    hex.fX   = fX;
    hex.fY   = fY;
    hex.fD   = fD;
    hex.fPhi = fPhi;
}

// ------------------------------------------------------------------------
//
// compute the distance of a point (px,py) to the Hexagon
// this functions needed for graphical primitives, that
// means without this function you are not able to interact
// with the graphical primitive with the mouse!!!
//
// All calcutations are running in pixel coordinates
//
Int_t MHexagon::DistancetoPrimitive(Int_t px, Int_t py, Float_t conv)
{
    //FIXME: Rotation phi missing!

    //
    //  compute the distance of the Point to the center of the Hexagon
    //
    const Int_t pxhex = gPad->XtoAbsPixel(fX*conv);
    const Int_t pyhex = gPad->YtoAbsPixel(fY*conv);

    //const Double_t x = TMath::Abs(px-pxhex);
    //const Double_t y = TMath::Abs(py-pyhex);

    TVector2 v(TMath::Abs(px-pxhex), TMath::Abs(py-pyhex));
    // FIXME: fPhi or -fPhi?
    v = v.Rotate(-fPhi);             // FIXME: Replace with a precalculates sin/cos vector

    const Double_t x = TMath::Abs(v.X());
    const Double_t y = TMath::Abs(v.Y());

    const Double_t disthex = TMath::Sqrt(x*x + y*y);

    if (disthex==0)
        return 0;

    const Double_t cosa = x / disthex;
    const Double_t sina = y / disthex;

    //
    // comput the distance of hexagon center to pixel border
    //
    const Double_t dx = fD/2 * cosa;
    const Double_t dy = fD/2 * sina;

    const Int_t pxborder = gPad->XtoAbsPixel((fX + dx)*conv);
    const Int_t pyborder = gPad->YtoAbsPixel((fY + dy)*conv);

    const Double_t bx = pxhex-pxborder;
    const Double_t by = pyhex-pyborder;

    const Double_t distborder = TMath::Sqrt(bx*bx + by*by);

    //
    //  compute the distance from the border of Pixel
    //  here in the first implementation is just circle inside
    //
    return distborder < disthex ? (int)((disthex-distborder)/conv+1) : 0;
}
/*
// ------------------------------------------------------------------------
//
// compute the distance of a point (px,py) to the Hexagon center in world
// coordinates. Return -1 if inside.
//
Float_t MHexagon::DistanceToPrimitive(Float_t px, Float_t py) const
{
    //FIXME: Rotation phi missing!

    //
    //  compute the distance of the Point to the center of the Hexagon
    //
    //const Double_t dx = px-fX;
    //const Double_t dy = py-fY;

    TVector2 v(px-fX, py-fY);
    // FIXME: fPhi or -fPhi?
    v = v.Rotate(-fPhi);             // FIXME: Replace with a precalculates sin/cos vector

    const Double_t dx = v.X();
    const Double_t dy = v.Y();

    const Double_t disthex = TMath::Sqrt(dx*dx + dy*dy);

    //
    // Now check if point is outside of hexagon; just check x coordinate
    // in three coordinate systems: the default one, in which two sides of
    // the hexagon are paralel to the y axis (see camera displays) and two 
    // more, rotated with respect to that one by +- 60 degrees.
    //

    if (TMath::Abs(dx) > fD/2.)
      return disthex;

    const Double_t dx2 = dx*fgCos60 + dy*fgSin60;

    if  (TMath::Abs(dx2) > fD/2.)
      return disthex;

    const Double_t dx3 = dx*fgCos60 - dy*fgSin60;

    if  (TMath::Abs(dx3) > fD/2.)
      return disthex;

    return -1;
}
*/
/*
Float_t MHexagon::DistanceToPrimitive(Float_t px, Float_t py)
{
    //
    //  compute the distance of the Point to the center of the Hexagon
    //
    const Double_t dx = px-fX;
    const Double_t dy = py-fY;

    const Double_t disthex = TMath::Sqrt(dx*dx + dy*dy);

    //
    //  compute the distance from the border of Pixel
    //  here in the first implementation is just circle outside
    //
    return fD*0.5772 < disthex ? disthex-fD*0.5772 : -1;
    //
    // FIXME: this approximate method results in some photons being 
    // assigned to two or even three different pixels.
    //
}
*/

// ------------------------------------------------------------------------
//
//  Draw this ellipse with new coordinate
//
void MHexagon::DrawHexagon(Float_t x, Float_t y, Float_t d, Float_t phi)
{ 
    MHexagon *newhexagon = new MHexagon(x, y, d, phi);

    TAttLine::Copy(*newhexagon);
    TAttFill::Copy(*newhexagon);

    newhexagon->SetBit(kCanDelete);
    newhexagon->AppendPad();
}

/*
// ------------------------------------------------------------------------
//
//  This is the first test of implementing a clickable interface
//  for one pixel
//
void MHexagon::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
    switch (event)
    {
    case kButton1Down:
        cout << endl << "kButton1Down" << endl;
        SetFillColor(2);
        gPad->Modified();
        break;

    case kButton1Double:
        SetFillColor(0);
        gPad->Modified();
        break;
        //  case kMouseEnter:
        //     printf ("\n Mouse inside object \n" ) ;
        //     break;
    }
}
*/

// ------------------------------------------------------------------------
//
//  list this hexagon with its attributes
//
void MHexagon::ls(const Option_t *) const
{
    TROOT::IndentLevel();
    Print();
}

// ------------------------------------------------------------------------
//
//  paint this hexagon with its attribute
//
void MHexagon::Paint(Option_t *)
{
    PaintHexagon(fX, fY, fD, fPhi);
}

// ------------------------------------------------------------------------
//
//  draw this hexagon with the coordinates
//
void MHexagon::PaintHexagon(Float_t inX, Float_t inY, Float_t inD, Float_t phi)
{ 
    //
    //  calculate the positions of the pixel corners
    //
    Double_t x[7], y[7];
    for (Int_t i=0; i<7; i++)
    {
        TVector2 v(fgDx[i%6], fgDy[i%6]);

        v = v.Rotate(phi); // FIXME: Replace with a precalculates sin/cos vector

        x[i] = inX + v.X()*inD;
        y[i] = inY + v.Y()*inD;
    }

    TAttLine::Modify();    // Change line attributes only if neccessary
    TAttFill::Modify();    // Change fill attributes only if neccessary

    //
    //   paint the pixel
    //
    if (GetFillColor())
        gPad->PaintFillArea(6, x, y);

    if (GetLineStyle())
        gPad->PaintPolyLine(7, x, y);
}

// ------------------------------------------------------------------------
//
//  print/dump this hexagon with its attributes
//
void MHexagon::Print(Option_t *) const
{
    cout << "MHexagon: ";
    cout << "x=" << fX << "mm y=" << fY << "mm r=" << fD << "mm phi=" << TMath::RadToDeg() << "deg" << endl;

    cout << " Line:";
    cout << " Color=" << GetLineColor() << ",";
    cout << " Style=" << GetLineStyle() << ",";
    cout << " Width=" << GetLineWidth() << endl;
    cout << " Fill:";
    cout << " Color=" << GetFillColor() << ",";
    cout << " Style=" << GetFillStyle() << endl;
}

// ------------------------------------------------------------------------
//
// Save primitive as a C++ statement(s) on output stream out
//
void MHexagon::SavePrimitive(ostream &out, Option_t *)
{
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,00)
    if (gROOT->ClassSaved(Class()))
       out << "   ";
    else
       out << "   MHexagon *";

    out << "hexagon = new MHexagon(" << fX << "," << fY << "," << fD << "," << fPhi << ");" << endl;

    SaveFillAttributes(out, "hexagon");
    SaveLineAttributes(out, "hexagon");

    out << "   hexagon->Draw();" << endl;
#endif
}

void MHexagon::SavePrimitive(ofstream &out, Option_t *)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,12,00)
    if (gROOT->ClassSaved(Class()))
       out << "   ";
    else
       out << "   MHexagon *";

    out << "hexagon = new MHexagon(" << fX << "," << fY << "," << fD << "," << fPhi << ");" << endl;

    SaveFillAttributes(out, "hexagon");
    SaveLineAttributes(out, "hexagon");

    out << "   hexagon->Draw();" << endl;
#else
    MHexagon::SavePrimitive(static_cast<ostream&>(out), "");
#endif
}
