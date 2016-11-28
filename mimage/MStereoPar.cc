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
!   Author(s): Abelardo Moralejo 11/2003 <mailto:moralejo@pd.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MStereoPar
//
// Storage Container for shower parameters estimated using the information
// from two telescopes (presently for MC studies)
//
// 
/////////////////////////////////////////////////////////////////////////////
#include "MStereoPar.h"

#include <fstream>

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MHillas.h"
#include "MGeomCam.h"
#include "MPointingPos.h"

ClassImp(MStereoPar);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MStereoPar::MStereoPar(const char *name, const char *title)
{
    fName  = name  ? name  : "MStereoPar";
    fTitle = title ? title : "Stereo image parameters";
}

// --------------------------------------------------------------------------
//
void MStereoPar::Reset()
{
    fCoreX = 0.;
    fCoreY = 0.;
    fSourceX = 0.;
    fSourceY = 0.;
}

// --------------------------------------------------------------------------
//
// Transformation of coordinates, from a point on the camera x, y , to
// the directon cosines of the corresponding direction, in the system of
// coordinates in which X-axis is North, Y-axis is west, and Z-axis 
// points to the zenith. The transformation has been taken from TDAS 01-05,
// although the final system of coordinates is not the same defined there,
// but the one defined in Corsika (for convenience). 
//
// rc is the distance from the reflector center to the camera. CTphi and 
// CTtheta indicate the telescope orientation. The angle CTphi is the 
// azimuth of the vector going along the telescope axis from the camera 
// towards the reflector, measured from the North direction anticlockwise 
// ( being West: phi=pi/2, South: phi=pi, East: phi=3pi/2 )
//
// rc and x,y must be given in the same units!
//
TVector3 MStereoPar::CamToDir(const MGeomCam &geom, const MPointingPos &pos, Float_t x, Float_t y) const
{
    const Double_t rc      = geom.GetCameraDist()*1e3;

    const Double_t CTphi   = pos.GetAzRad();
    const Double_t CTtheta = pos.GetZdRad();

    //
    // We convert phi to the convention defined in TDAS 01-05
    //
    const Double_t sinphi   = sin(TMath::TwoPi()-CTphi);
    const Double_t cosphi   = cos(CTphi);

    const Double_t costheta = cos(CTtheta);
    const Double_t sintheta = sin(CTtheta);

    const Double_t xc = x/rc;
    const Double_t yc = y/rc;

    const Double_t norm = 1/sqrt(1+xc*xc+yc*yc);

    const Double_t xref = xc * norm;
    const Double_t yref = yc * norm;
    const Double_t zref = -1 * norm;

    const Double_t cosx =  xref * sinphi + yref * costheta*cosphi - zref * sintheta*cosphi;
    const Double_t cosy = -xref * cosphi + yref * costheta*sinphi - zref * sintheta*sinphi;
    const Double_t cosz =                  yref * sintheta        + zref * costheta;

    //  Now change from system A of TDAS 01-05 to Corsika system:
    return TVector3(cosx, -cosy, -cosz);
}

TVector3 MStereoPar::CamToDir(const MGeomCam &geom, const MPointingPos &pos, const TVector2 &p) const
{
    return CamToDir(geom, pos, p.X(), p.Y());
}

void MStereoPar::CalcCT(const MHillas &h, const MPointingPos &p, const MGeomCam &g, TVector2 &cv1, TVector2 &cv2) const
{
    //
    // Get the direction corresponding to the c.o.g. of the image on 
    // the camera.
    //
    // ct1_a, Direction from ct1 to the shower c.o.g.
    //
    const TVector3 a = CamToDir(g, p, h.GetMeanX(), h.GetMeanY());

    //
    // Now we get another (arbitrary) point along the image long axis,
    // fMeanX + cosdelta, fMeanY + sindelta, and calculate the direction 
    // to which it corresponds.
    //
    const TVector3 c = CamToDir(g, p, h.GetMeanX()+h.GetCosDelta(), h.GetMeanY()+h.GetSinDelta());

    //
    // The vectorial product of the latter two vectors is a vector 
    // perpendicular to the plane which contains the shower axis and 
    // passes through the telescope center (center of reflector). 
    // The vectorial product of that vector and (0,0,1) is a vector on
    // the horizontal plane pointing from the telescope center to the 
    // shower core position on the z=0 plane (ground).
    //
    const Double_t coreVersorX = a.Z()*c.X() - a.X()*c.Z();
    const Double_t coreVersorY = a.Z()*c.Y() - a.X()*c.Z();

    //
    // Now we calculate again the versor, now assuming that the source 
    // direction is paralel to the telescope axis (camera position 0,0)  
    // This increases the precision of the core determination if the showers
    // actually come from that direction (like for gammas from a point source)

    const TVector3 b = CamToDir(g, p, 0, 0);

    const Double_t coreVersorX_best = a.Z()*b.X() - a.X()*b.Z();
    const Double_t coreVersorY_best = a.Z()*b.Y() - a.Y()*b.Z();

    cv1.Set(coreVersorX,      coreVersorY);
    cv2.Set(coreVersorX_best, coreVersorY_best);
}

TVector2 MStereoPar::VersorToCore(const TVector2 &v1, const TVector2 &v2, const TVector2 &p1, const TVector2 &p2) const
{
    const TVector2 dp = p1 - p2;

    //
    // Estimate core position:
    //
    const Double_t t =
        (dp.X() - v2.X()/v2.Y()*dp.Y())/(v2.X()/v2.Y()*v1.Y() - v1.X());

    // Core will have the same units as p1/p2
    return p1 + v1*t;
}

Double_t MStereoPar::CalcImpact(const TVector2 &w, const TVector2 &v, const TVector2 &p) const
{
    const TVector2 d = v-p;

    const Double_t f = d*w;

    return TMath::Sqrt( d.Mod2() - f*f );
}

Double_t MStereoPar::CalcImpact(const TVector3 &v, const TVector2 &p) const
{
    const TVector2 w = v.XYvector();
    return CalcImpact(w, w, p);
}

Double_t MStereoPar::CalcImpact(const TVector2 &core, const TVector2 &p, const MPointingPos &point) const
{
    const TVector2 pt(-sin(point.GetZdRad()) * cos(point.GetAzRad()),
                      -sin(point.GetZdRad()) * sin(point.GetAzRad()));

    return CalcImpact(pt, core, p);
}

// --------------------------------------------------------------------------
//
//  Calculation of shower parameters
//
void MStereoPar::Calc(const MHillas &hillas1, const MPointingPos &pos1, const MGeomCam &geom1, const Float_t ct1_x, const Float_t ct1_y,
                      const MHillas &hillas2, const MPointingPos &pos2, const MGeomCam &geom2, const Float_t ct2_x, const Float_t ct2_y)
{
    TVector2 coreVersor1, coreVersor1_best;
    CalcCT(hillas1, pos1, geom1, coreVersor1, coreVersor1_best);

    TVector2 coreVersor2, coreVersor2_best;
    CalcCT(hillas2, pos2, geom2, coreVersor2, coreVersor2_best);

    const TVector2 p1(ct1_x, ct1_y);
    const TVector2 p2(ct2_x, ct2_y);

    // Estimate core position:
    const TVector2 core = VersorToCore(coreVersor1, coreVersor2, p1, p2);

    // Now the estimated core position assuming the source is
    // located in the center of the camera
    const TVector2 core2 = VersorToCore(coreVersor1_best, coreVersor2_best, p1, p2);

    // Copy results to data members
    //
    // Be careful, the coordinates in MMcEvt.fCoreX,fCoreY are actually 
    // those of the vector going *from the shower core to the telescope*.
    // Ours are those of the vector which goes from telescope 1 to the 
    // core estimated core.
    //
    fCoreX  = core.X();
    fCoreY  = core.Y();

    fCoreX2 = core2.X();
    fCoreY2 = core2.Y();

    // Now estimate the source location on the camera by intersecting 
    // major axis of the ellipses. This assumes both telescopes are 
    // pointing paralel! We introduce the camera scale to account for
    // the use of telescopes with different focal distances. 
    //
    const TVector2 v1(hillas1.GetSinDelta(), hillas1.GetCosDelta());
    const TVector2 v2(hillas2.GetSinDelta(), hillas2.GetCosDelta());

    const TVector2 h1 = hillas1.GetMean()*geom1.GetConvMm2Deg();
    const TVector2 h2 = hillas2.GetMean()*geom2.GetConvMm2Deg();

    const TVector2 src = VersorToCore(v1, v2, h1, h2);

    // Reconstructed source positon
    fSourceX = src.X();
    fSourceY = src.Y();

    // Squared angular distance from reconstr. src pos to camera center.
    fTheta2 = src.Mod2();

    // Get the direction corresponding to the intersection of axes
    const TVector3 srcdir = CamToDir(geom1, pos1, src/geom1.GetConvMm2Deg());

    fCT1Impact = CalcImpact(srcdir, p1);
    fCT2Impact = CalcImpact(srcdir, p2);

    // Now calculate i.p. assuming source is point-like and placed in
    // the center of the camera.
    fCT1Impact2 = CalcImpact(core2, p1, pos1);
    fCT2Impact2 = CalcImpact(core2, p2, pos2);

    SetReadyToSave();
} 
