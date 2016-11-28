/* ======================================================================== *\
!
! *
! * This file is part of CheObs, the Modular Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appears in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz,  1/2009 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: CheObs Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReflector
//
//////////////////////////////////////////////////////////////////////////////
#include "MReflector.h"

#include <fstream>
#include <errno.h>

#include <stdlib.h> // atof (Ubuntu 8.10)

#include <TClass.h>
#include <TSystem.h>
#include <TEllipse.h>

#include "MQuaternion.h"
#include "MMirror.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MH.h"

ClassImp(MReflector);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor
//
MReflector::MReflector(const char *name, const char *title)
{
    fName  = name  ? name  : "MReflector";
    fTitle = title ? title : "Parameter container storing a collection of several mirrors (reflector)";

    fMirrors.SetOwner();
}

// --------------------------------------------------------------------------
//
// Set the SigmaPSF of all mirrors currently stored.
//
void MReflector::SetSigmaPSF(Double_t psf)
{
    fMirrors.R__FOR_EACH(MMirror, SetSigmaPSF)(psf);
}

// --------------------------------------------------------------------------
//
// Calculate the maximum radius of th ereflector. This is not meant as
// a precise number but as a rough estimate e.g. to bin a histogram.
//
void MReflector::InitMaxR()
{
    fMaxR = 0;

    TIter Next(&fMirrors);
    MMirror *m = 0;
    while ((m=static_cast<MMirror*>(Next())))
    {
        // Take into account the maximum incident angle 8eg 10deg) and
        // the theta-angle of the mirror and the z-distance.
        const Double_t r = m->GetDist()+1.5*m->GetMaxR();
        if (r > fMaxR)
            fMaxR = r;
    }
}

// --------------------------------------------------------------------------
//
// Return the total Area of all mirrors. Note, that it is recalculated
// with any call.
//
Double_t MReflector::GetA() const
{
    Double_t A = 0;

    TIter Next(&fMirrors);
    MMirror *m = 0;
    while ((m=static_cast<MMirror*>(Next())))
        A += m->GetA();

    return A;
}

// --------------------------------------------------------------------------
//
// Get the pointer to the first mirror. This is a very dangerous way of
// access, but the fastest possible. because it is the most often called
// function in ExecuteReflector we have to have a very fast access.
//
const MMirror **MReflector::GetFirstPtr() const
{
    return (const MMirror**)fMirrors.GetObjectRef(0);
}

// --------------------------------------------------------------------------
//
// Get number of mirrors. There should be no holes in the array!
//
const UInt_t MReflector::GetNumMirrors() const
{
    return fMirrors.GetEntriesFast();
}

// --------------------------------------------------------------------------
//
// Check with a rough estimate whether a photon can hit the reflector.
//
Bool_t MReflector::CanHit(const MQuaternion &p) const
{
    // p is given in the reflectory coordinate frame. This is meant as a
    // fast check without lengthy calculations to omit all photons which
    // cannot hit the reflector at all
    return p.R2()<fMaxR*fMaxR;
}

// --------------------------------------------------------------------------
//
// I/O helper for ReadFile to avoid calling "delete arr" before every return
//
MMirror *MReflector::EvalTokens(TObjArray &arr, Double_t defpsf) const
{
    if (arr.GetEntries()<9)
    {
        *fLog << err << "ERROR - Not enough arguments..." << endl;
        return 0;
    }

    const TVector3 pos(atof(arr[0]->GetName()),
                       atof(arr[1]->GetName()),
                       atof(arr[2]->GetName()));

    const TVector3 norm(atof(arr[3]->GetName()),
                        atof(arr[4]->GetName()),
                        atof(arr[5]->GetName()));

    UInt_t n = 6;

    TString six = arr[n]->GetName();

    Char_t shape = 'S';
    if (!six.IsFloat())
    {
        shape = six[0];
        n++;
    }

    const Double_t F = atof(arr[n++]->GetName());

    const TString val = arr[n++]->GetName();

    const Double_t psf = val.IsFloat() ? val.Atof() : -1;

    n += val.IsFloat() ? 1 : 0;

    TString type = arr[n-1]->GetName();
    type.Prepend("MMirror");

    for (UInt_t i=0; i<n; i++)
        delete arr.RemoveAt(i);
    arr.Compress();

    TString msg;
    TClass *cls = MParContainer::GetClass(type);
    if (!cls)
    {
        *fLog << err << "ERROR - Class " << type << " not in dictionary." << endl;
        return 0;
    }

    if (!cls->InheritsFrom(MMirror::Class()))
    {
        *fLog << err << "ERROR - Cannot create new instance of class " << type << ": " << endl;
        *fLog << "Class doesn't inherit from MMirror." << endl;
        return 0;
    }

    MMirror *m = static_cast<MMirror*>(cls->New());
    if (!m)
    {
        *fLog << err << "ERROR - Cannot create new instance of class " << type << ": " << endl;
        *fLog << " - Class has no default constructor." << endl;
        *fLog << " - An abstract member functions of a base class is not overwritten." << endl;
        return 0;
    }

    m->SetPosition(pos);
    m->SetNorm(norm);
    m->SetShape(shape);
    m->SetFocalLength(F);
    m->SetSigmaPSF(psf>=0 ? psf : defpsf);

    if (m->ReadM(arr)>=0)
        return m;

    *fLog << err << "ERROR - " << type << "::ReadM failed." << endl;

    delete m;
    return 0;
}

// --------------------------------------------------------------------------
//
// Read a reflector setup from a file. This needs improvemtn.
//
// The file structur is like:
//
//     x y z nx ny nz F [psf] Type ...
//
//  x:         x-coordinate of the mirror's center
//  y:         y-coordinate of the mirror's center
//  z:         z-coordinate of the mirror's center
//  nx:        x-component of the normal vecor in the mirror center
//  ny:        y-component of the normal vecor in the mirror center
//  nz:        z-component of the normal vecor in the mirror center
//  [shape]:   S for spherical <default>, P for parabolic
//  F:         Focal distance of a spherical mirror
//  [psf]:     This number is the psf given in the units of x,y,z and
//             defined at the focal distance F. It can be used to overwrite
//             the second argument given in ReadFile for individual mirrors.
//  Type:      A instance of a mirrot of the class Type MMirrorType is created
//             (Type can be, for example, Hex for for MMirrorHex).
//  ...:       Additional arguments as defined in MMirrorType::ReadM
//
//
// Coordinate System:
//  The coordinate system is local in the reflectors frame.
//  It is defined viewing from the back side of the reflector
//  towards the camera. (x "right", y "up", z from reflector to camera)
//  Note, that it is left-handed!
//
Bool_t MReflector::ReadFile(TString fname, Double_t defpsf)
{
    SetTitle(fname);
    fMirrors.Delete();

    gSystem->ExpandPathName(fname);

    ifstream fin(fname);
    if (!fin)
    {
        *fLog << err << "Cannot open file " << fname << ": ";
        *fLog << (errno!=0?strerror(errno):"Insufficient memory for decompression") << endl;
        return kFALSE;
    }

/*
    Int_t idx[964];
    Int_t srt[964];
    for (int i=0; i<964; i++)
        srt[i] = gRandom->Integer(964);

    TMath::Sort(964, srt, idx);
    */

    Int_t cnt = 0;

    while (1)
    {
        TString line;
        line.ReadLine(fin);
        if (!fin)
            break;

        // Count lines
        cnt++;

        // Skip comments
        if (line.BeginsWith("#"))
            continue;

        // Remove leading and trailing whitespaces
        line=line.Strip(TString::kBoth);

        // Skip empty lines
        if (line.IsNull())
            continue;

        // Tokenize line
        TObjArray *arr = line.Tokenize(' ');

        // Evaluate tokens
        MMirror *m = EvalTokens(*arr, defpsf);

        // Delete now obsolete array
        delete arr;

        // Check if a new mirror could be created successfully
        if (!m)
        {
            *fLog << err << "Error in line " << cnt << ": " << line << endl;
            return kFALSE;
        }

        // Add new mirror to array
        fMirrors.Add(m);
    }

    InitMaxR();

    return kTRUE;

//    fMirrors.Sort();
/*
    for (int i=0; i<964; i++)
    {
        MMirror &ref = (MMirror&)*fMirrors[i];

        TArrayD dist(964);
        for (int j=0; j<964; j++)
        {
            const MMirror &mir = (MMirror&)*fMirrors[j];
            dist[j] = (ref-mir).Mod();
        }

        TArrayI idx(964);
        TMath::Sort(964, dist.GetArray(), idx.GetArray(), kFALSE);

        for (int j=0; j<964; j++)
        {
            ref.fNeighbors.Add(fMirrors[idx[j]]);
        }
    }*/
}

// ------------------------------------------------------------------------
//
Bool_t MReflector::WriteFile(TString fname) const
{
    gSystem->ExpandPathName(fname);

    ofstream fout(fname);
    if (!fout)
    {
        *fLog << err << "Cannot open file " << fname << ": ";
        *fLog << (errno!=0?strerror(errno):"Insufficient memory for decompression") << endl;
        return kFALSE;
    }

    TIter Next(&fMirrors);
    MMirror *m = 0;
    while ((m=static_cast<MMirror*>(Next())))
    {
        //  x:         x-coordinate of the mirror's center
        //  y:         y-coordinate of the mirror's center
        //  z:         z-coordinate of the mirror's center
        //  nx:        x-component of the normal vecor in the mirror center
        //  ny:        y-component of the normal vecor in the mirror center
        //  nz:        z-component of the normal vecor in the mirror center
        //  [shape]:   S for spherical <default>, P for parabolic
        //  F:         Focal distance of a spherical mirror
        //  [psf]:     This number is the psf given in the units of x,y,z and
        //             defined at the focal distance F. It can be used to overwrite
        //             the second argument given in ReadFile for individual mirrors.
        //  Type:      A instance of a mirrot of the class Type MMirrorType is created
        //             (Type can be, for example, Hex for for MMirrorHex).
        //  ...:       Additional arguments as defined in MMirrorType::ReadM

        fout << setw(12) << m->X() << " ";
        fout << setw(12) << m->Y() << " ";
        fout << setw(12) << m->Z() << "  ";
        fout << setw(12) << m->Nx() << " ";
        fout << setw(12) << m->Ny() << " ";
        fout << setw(12) << m->Nz() << " ";
        if (m->GetShape()==1)
            fout << "P ";
        fout << m->GetFocalLength() << " ";
        // cout << m->GetSigmaPSF() << " ";
        fout << m->ClassName()+7 << " ";
        m->WriteM(fout);
        fout << endl;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print the collection of mirrors
//
void MReflector::Print(Option_t *o) const
{
    *fLog << all << GetDescriptor() << " (" << GetNumMirrors() << "): " << endl;

    fMirrors.Print(o);
}

// --------------------------------------------------------------------------
//
// Paint the collection of mirrors
//
void MReflector::Paint(Option_t *o)
{
    if (!TString(o).Contains("same", TString::kIgnoreCase))
        MH::SetPadRange(-fMaxR*1.01, -fMaxR*1.01, fMaxR*1.01, fMaxR*1.01);

    fMirrors.Paint(o);

    TEllipse e;
    e.SetFillStyle(0);
    e.SetLineColor(17);
    e.PaintEllipse(0, 0, fMaxR, fMaxR, 0, 360, 0);
}

// --------------------------------------------------------------------------
//
// SigmaPSF: -1
// FileName: reflector.txt
//
// SigmaPSF can be used to set a default for the psf of the mirrors
// read from the file. Note, that this can be overwritten for individual
// mirrors in the file. The SigmaPSF is the sigma of a 1D-Gauss fitted
// to the radial profile of the light distribution.
//
// For details on the file structure see MReflector::ReadFile
//
Int_t MReflector::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;

    Double_t psf = -1;
    if (IsEnvDefined(env, prefix, "SetSigmaPSF", print))
    {
        rc = kTRUE;
        psf = GetEnvValue(env, prefix, "SetSigmaPSF", -1);
    }

    if (IsEnvDefined(env, prefix, "FileName", print))
    {
        rc = kTRUE;
        if (!ReadFile(GetEnvValue(env, prefix, "FileName", ""), psf))
            return kERROR;
    }

    return rc;
}
