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
! * in supporting documentation. It is provided "as is" without expressed
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz, 03/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2002-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MVector3
//  ========
//
//////////////////////////////////////////////////////////////////////////////
#include "MVector3.h"

#include <iostream>

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MVector3);

using namespace std;

void MVector3::SetThetaPhiMag(Double_t theta, Double_t phi, Double_t mag)
{
    SetMagThetaPhi(TMath::Power(10, -mag/2.5), theta, phi);
}

Double_t MVector3::Magnitude() const
{
    return -2.5*TMath::Log10(Mag());
}

void MVector3::SetRaDec(Double_t ra, Double_t dec, Double_t mag)
{
    fType = kIsRaDec;
    SetThetaPhiMag(TMath::Pi()/2-dec, ra, mag);
}

void MVector3::SetAltAz(Double_t alt, Double_t az, Double_t mag)
{
    fType = kIsAltAz;
    SetThetaPhiMag(TMath::Pi()/2-alt, az, mag);
}

void MVector3::WriteBinary(ostream &out) const
{
    const Double_t t = Theta();
    const Double_t p = Phi();
    const Float_t  m = Mag();
    out.write((char*)&t, 8);
    out.write((char*)&p, 8);
    out.write((char*)&m, 4);
    out << fName << endl;
}

void MVector3::ReadBinary(istream &in)
{
    Double_t t, p;
    Float_t  m;

    in.read((char*)&t, 8);
    in.read((char*)&p, 8);
    in.read((char*)&m, 4);
    fName.ReadLine(in);

    SetMagThetaPhi(m, t, p);
}
