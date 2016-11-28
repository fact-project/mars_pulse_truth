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
!   Author(s): Thomas Bretz  07/2001 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2010
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  MH                                                                      //
//                                                                          //
//  This is a base tasks for mars histograms. It defines a common interface //
//  for filling the histograms with events (MH::Fill) which is used by a    //
//  common 'filler' And a SetupFill member function which may be used       //
//  by MFillH. The idea is:                                                 //
//   1) If your Histogram can become filled by one single container         //
//      (like MHHillas) you overload MH::Fill and it gets called with       //
//      a pointer to the container with which it should be filled.          //
//                                                                          //
//   2) You histogram needs several containers to get filled. Than you      //
//      have to overload MH::SetupFill and get the necessary objects from   //
//      the parameter list. Use this objects in Fill to fill your           //
//      histogram.                                                          //
//                                                                          //
//  If you want to create your own histogram class the new class must be    //
//  derived from MH (instead of the base MParContainer) and you must        //
//  the fill function of MH. This is the function which is called to fill   //
//  the histogram(s) by the data of a corresponding parameter container.    //
//                                                                          //
//  Remark: the static member function (eg MakeDefCanvas) can be called     //
//          from everywhere using: MH::MakeDefCanvas(...)                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include "MH.h"

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TColor.h>
#include <TMath.h>
#include <TClass.h>
#include <TStyle.h>       // TStyle::GetScreenFactor
#include <TGaxis.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TBaseClass.h>
#include <THashList.h>
#if ROOT_VERSION_CODE > ROOT_VERSION(3,04,01)
#include <THLimitsFinder.h>
#endif
#include <TProfile2D.h>  // root > 5.18

#include "MLog.h"
#include "MLogManip.h"

#include "MString.h"

#include "MParList.h"
#include "MParContainer.h"

#include "MBinning.h"

#include "MArrayD.h"
#include "MArrayF.h"

ClassImp(MH);

using namespace std;

// --------------------------------------------------------------------------
//
// Default Constructor. It sets name and title only. Typically you won't
// need to change this.
//
MH::MH(const char *name, const char *title)
    : fSerialNumber(0), fNumExecutions(0)

{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : "MH";
    fTitle = title ? title : "Base class for Mars histograms";
}

// --------------------------------------------------------------------------
//
// If you want to use the automatic filling of your derived class you
// must overload this function. If it is not overloaded you cannot use
// FillH with this class. The argument is a pointer to a container
// in your paremeter list which is specified in the MFillH constructor.
// If you are not going to use it you should at least add
//   Bool_t MH::Fill(const MParContainer *) { return kTRUE; }
// to your class definition.
//
Int_t MH::Fill(const MParContainer *par, const Stat_t w)
{
    *fLog << warn << GetDescriptor() << ": Fill not overloaded! Can't be used!" << endl;
    return kERROR;
}

// --------------------------------------------------------------------------
//
// This virtual function is ment as a generalized interface to retrieve
// a pointer to a root histogram from the MH-derived class.
//
TH1 *MH::GetHistByName(const TString name) const
{
    *fLog << warn << GetDescriptor() << ": GetHistByName not overloaded! Can't be used!" << endl;
    return NULL;
}

// --------------------------------------------------------------------------
//
// This is a function which should replace the creation of default
// canvases like root does. Because this is inconvinient in some aspects.
// need to change this.
// You can specify a name for the default canvas and a title. Also
// width and height can be given.
// MakeDefCanvas looks for a canvas with the given name. If now name is
// given the DefCanvasName of root is used. If no such canvas is existing
// it is created and returned. If such a canvas already exists a new canvas
// with a name plus anumber is created (the number is calculated by the
// number of all existing canvases plus one)
//
// Normally the canvas size is scaled with gStyle->GetScreenFactor() so
// that on all screens it looks like the same part of the screen.
// To suppress this scaling use usescreenfactor=kFALSE. In this case
// you specify directly the size of the embedded pad.
//
TCanvas *MH::MakeDefCanvas(TString name, const char *title,
                           UInt_t w, UInt_t h, Bool_t usescreenfactor)
{
    const TList *list = (TList*)gROOT->GetListOfCanvases();

    if (name.IsNull())
        name = gROOT->GetDefCanvasName();

    if (list->FindObject(name))
        name += MString::Format(" <%d>", list->GetSize()+1);

    if (!usescreenfactor)
    {
        const Float_t cx = gStyle->GetScreenFactor();
        w += 4;
        h += 28;
        w = (int)(w/cx+1);
        h = (int)(h/cx+1);
    }

    return new TCanvas(name, title, w, h);
}

// --------------------------------------------------------------------------
//
// This function works like MakeDefCanvas(name, title, w, h) but name
// and title are retrieved from the given TObject.
//
// Normally the canvas size is scaled with gStyle->GetScreenFactor() so
// that on all screens it looks like the same part of the screen.
// To suppress this scaling use usescreenfactor=kFALSE. In this case
// you specify directly the size of the embedded pad.
//
TCanvas *MH::MakeDefCanvas(const TObject *obj,
                           UInt_t w, UInt_t h, Bool_t usescreenfactor)
{
    if (!usescreenfactor)
    {
        const Float_t cx = gStyle->GetScreenFactor();
        w += 4;
        h += 28;
        h = (int)(h/cx+1);
        w = (int)(w/cx+1);
    }

    return MakeDefCanvas(obj->GetName(), obj->GetTitle(), w, h);
}

// --------------------------------------------------------------------------
//
// Search in gPad for all objects with the name name and remove all of them
// (TList::Remove)
//
void MH::RemoveFromPad(const char *name)
{
    if (!gPad)
        return;

    TList *list = gPad->GetListOfPrimitives();
    if (!list)
        return;

    TObject *obj = 0;
    while ((obj = gPad->FindObject(name)))
        list->Remove(obj);
}

// --------------------------------------------------------------------------
//
// If labels are set for this axis the correct MBinning corresponding
// to the existing label range is returned (this is necessary to
// maintain the correct number of bins in the histogram)
// otherwise the given binning is returned.
//
MBinning MH::GetBinningForLabels(TAxis &x, const MBinning *bins)
{
    if (!x.GetLabels())
        return *bins;

    const Int_t n = TMath::Max(x.GetLabels()->GetEntries(), 1);
    return MBinning(n, 0, n);
}

// --------------------------------------------------------------------------
//
// If Labels are set this function deletes the fXbins Array from
// the axis (which makes the axis a variable bin-size axis)
// and sets the Nbins, Xmin and Xmax according to the number of labels.
//
void MH::RestoreBinningForLabels(TAxis &x)
{
    if (!x.GetLabels())
        return;

    const Int_t n = TMath::Max(x.GetLabels()->GetEntries(), 1);
    x.Set(n, 0, n);

    const_cast<TArrayD*>(x.GetXbins())->Set(0);
}

// --------------------------------------------------------------------------
//
// Applies a given binning to a 1D-histogram. In case the axis has labels
// (e.g. GetXaxis()->GetLabels()) the binning is set according to the
// labels.
//
void MH::SetBinning(TH1 &h, const MBinning &binsx)
{
    //
    // Another strange behaviour: TAxis::Set deletes the axis title!
    //
    TAxis &x = *h.GetXaxis();

#if ROOT_VERSION_CODE < ROOT_VERSION(3,03,03)
    TString xtitle = x.GetTitle();
#endif

#if ROOT_VERSION_CODE < ROOT_VERSION(5,12,00)
    // All this is reset by TAxis::Set
    const TAttAxis att(x);
    const Bool_t   tm(x.GetTimeDisplay());
    const TString  tf(x.GetTimeFormat());

    //
    // This is a necessary workaround if one wants to set
    // non-equidistant bins after the initialization
    // TH1D::fNcells must be set correctly.
    //
    h.SetBins(binsx.GetNumBins(), 0, 1);

    //
    // Set the binning of the current histogram to the binning
    // in one of the two given histograms
    //
    x.Set(binsx.GetNumBins(), binsx.GetEdges());

    // All this is reset by TAxis::Set
    att.Copy(x);
    x.SetTimeDisplay(tm);
    x.SetTimeFormat(tf);
#else
    if (!x.GetLabels())
        h.SetBins(binsx.GetNumBins(), binsx.GetEdges());
#endif


#if ROOT_VERSION_CODE < ROOT_VERSION(3,03,03)
    x.SetTitle(xtitle);
#endif
}

// --------------------------------------------------------------------------
//
// Applies given binnings to the two axis of a 2D-histogram.
// In case the axis has labels (e.g. GetXaxis()->GetLabels())
// the binning is set according to the labels.
//
void MH::SetBinning(TH2 &h, const MBinning &binsx, const MBinning &binsy)
{
    TAxis &x = *h.GetXaxis();
    TAxis &y = *h.GetYaxis();

    const MBinning bx(GetBinningForLabels(x, &binsx));
    const MBinning by(GetBinningForLabels(y, &binsy));

    //
    // Another strange behaviour: TAxis::Set deletes the axis title!
    //
#if ROOT_VERSION_CODE < ROOT_VERSION(3,03,03)
    TString xtitle = x.GetTitle();
    TString ytitle = y.GetTitle();
#endif

#if ROOT_VERSION_CODE < ROOT_VERSION(5,12,00)
    // All this is reset by TAxis::Set
    const TAttAxis attx(x);
    const TAttAxis atty(y);
    const Bool_t   tmx(x.GetTimeDisplay());
    const Bool_t   tmy(y.GetTimeDisplay());
    const TString  tfx(x.GetTimeFormat());
    const TString  tfy(y.GetTimeFormat());

    //
    // This is a necessary workaround if one wants to set
    // non-equidistant bins after the initialization
    // TH1D::fNcells must be set correctly.
    //
    h.SetBins(bx.GetNumBins(), 0, 1,
              by.GetNumBins(), 0, 1);

    //
    // Set the binning of the current histogram to the binning
    // in one of the two given histograms
    //
    x.Set(bx.GetNumBins(), bx.GetEdges());
    y.Set(by.GetNumBins(), by.GetEdges());

    // All this is reset by TAxis::Set
    attx.Copy(x);
    atty.Copy(y);
    x.SetTimeDisplay(tmx);
    y.SetTimeDisplay(tmy);
    x.SetTimeFormat(tfx);
    y.SetTimeFormat(tfy);
#else
    if (h.InheritsFrom(TProfile2D::Class()))
    {
        h.SetBins(bx.GetNumBins(), 0, 1,
                  by.GetNumBins(), 0, 1);

        h.SetBinsLength();

        x.Set(bx.GetNumBins(), bx.GetEdges());
        y.Set(by.GetNumBins(), by.GetEdges());
    }
    else
        h.SetBins(bx.GetNumBins(), bx.GetEdges(),
                  by.GetNumBins(), by.GetEdges());
#endif

    RestoreBinningForLabels(x);
    RestoreBinningForLabels(y);

#if ROOT_VERSION_CODE < ROOT_VERSION(3,03,03)
    x.SetTitle(xtitle);
    y.SetTitle(ytitle);
#endif
}

// --------------------------------------------------------------------------
//
// Applies given binnings to the three axis of a 3D-histogram
// In case the axis has labels (e.g. GetXaxis()->GetLabels())
// the binning is set according to the labels.
//
void MH::SetBinning(TH3 &h, const MBinning &binsx, const MBinning &binsy, const MBinning &binsz)
{
    //
    // Another strange behaviour: TAxis::Set deletes the axis title!
    //
    TAxis &x = *h.GetXaxis();
    TAxis &y = *h.GetYaxis();
    TAxis &z = *h.GetZaxis();

    const MBinning bx(GetBinningForLabels(x, &binsx));
    const MBinning by(GetBinningForLabels(y, &binsy));
    const MBinning bz(GetBinningForLabels(z, &binsz));

#if ROOT_VERSION_CODE < ROOT_VERSION(3,03,03)
    TString xtitle = x.GetTitle();
    TString ytitle = y.GetTitle();
    TString ztitle = z.GetTitle();
#endif

#if ROOT_VERSION_CODE < ROOT_VERSION(5,12,00)
    // All this is reset by TAxis::Set
    const TAttAxis attx(x);
    const TAttAxis atty(y);
    const TAttAxis attz(z);
    const Bool_t   tmx(x.GetTimeDisplay());
    const Bool_t   tmy(y.GetTimeDisplay());
    const Bool_t   tmz(z.GetTimeDisplay());
    const TString  tfx(x.GetTimeFormat());
    const TString  tfy(y.GetTimeFormat());
    const TString  tfz(z.GetTimeFormat());
#endif

    //
    // This is a necessary workaround if one wants to set
    // non-equidistant bins after the initialization
    // TH1D::fNcells must be set correctly.
    //
    h.SetBins(bx.GetNumBins(), 0, 1,
              by.GetNumBins(), 0, 1,
              bz.GetNumBins(), 0, 1);

    //
    // Set the binning of the current histogram to the binning
    // in one of the two given histograms
    //
    x.Set(bx.GetNumBins(), bx.GetEdges());
    y.Set(by.GetNumBins(), by.GetEdges());
    z.Set(bz.GetNumBins(), bz.GetEdges());

    RestoreBinningForLabels(x);
    RestoreBinningForLabels(y);
    RestoreBinningForLabels(z);

#if ROOT_VERSION_CODE < ROOT_VERSION(5,12,00)
    // All this is reset by TAxis::Set
    attx.Copy(x);
    atty.Copy(y);
    attz.Copy(z);
    x.SetTimeDisplay(tmx);
    y.SetTimeDisplay(tmy);
    z.SetTimeDisplay(tmz);
    x.SetTimeFormat(tfx);
    y.SetTimeFormat(tfy);
    z.SetTimeFormat(tfz);
#endif

#if ROOT_VERSION_CODE < ROOT_VERSION(3,03,03)
    x.SetTitle(xtitle);
    y.SetTitle(ytitle);
    z.SetTitle(ztitle);
#endif
}

// --------------------------------------------------------------------------
//
// Applies given binning (the n+1 edges)  to the axis of a 1D-histogram
//
void MH::SetBinning(TH1 &h, const TArrayD &binsx)
{
    MBinning bx;
    bx.SetEdges(binsx);
    SetBinning(h, bx);
}

// --------------------------------------------------------------------------
//
// Applies given binning (the n+1 edges) to the two axis of a
// 2D-histogram
//
void MH::SetBinning(TH2 &h, const TArrayD &binsx, const TArrayD &binsy)
{
    MBinning bx;
    MBinning by;
    bx.SetEdges(binsx);
    by.SetEdges(binsy);
    SetBinning(h, bx, by);
}

// --------------------------------------------------------------------------
//
// Applies given binning (the n+1 edges) to the three axis of a
// 3D-histogram
//
void MH::SetBinning(TH3 &h, const TArrayD &binsx, const TArrayD &binsy, const TArrayD &binsz)
{
    MBinning bx;
    MBinning by;
    MBinning bz;
    bx.SetEdges(binsx);
    by.SetEdges(binsy);
    bz.SetEdges(binsz);
    SetBinning(h, bx, by, bz);
}

// --------------------------------------------------------------------------
//
// Applies the binning of a TAxis (eg from a root histogram) to the axis
// of a 1D-histogram
//
void MH::SetBinning(TH1 &h, const TAxis &binsx)
{
    const Int_t nx = binsx.GetNbins();

    TArrayD bx(nx+1);
    for (int i=0; i<nx; i++) bx[i] = binsx.GetBinLowEdge(i+1);
    bx[nx] = binsx.GetXmax();

    SetBinning(h, bx);
}

// --------------------------------------------------------------------------
//
// Applies the binnings of the TAxis' (eg from a root histogram) to the
// two axis' of a 2D-histogram
//
void MH::SetBinning(TH2 &h, const TAxis &binsx, const TAxis &binsy)
{
    const Int_t nx = binsx.GetNbins();
    const Int_t ny = binsy.GetNbins();

    TArrayD bx(nx+1);
    TArrayD by(ny+1);
    for (int i=0; i<nx; i++) bx[i] = binsx.GetBinLowEdge(i+1);
    for (int i=0; i<ny; i++) by[i] = binsy.GetBinLowEdge(i+1);
    bx[nx] = binsx.GetXmax();
    by[ny] = binsy.GetXmax();

    SetBinning(h, bx, by);
}

// --------------------------------------------------------------------------
//
// Applies the binnings of the TAxis' (eg from a root histogram) to the
// three axis' of a 3D-histogram
//
void MH::SetBinning(TH3 &h, const TAxis &binsx, const TAxis &binsy, const TAxis &binsz)
{
    const Int_t nx = binsx.GetNbins();
    const Int_t ny = binsy.GetNbins();
    const Int_t nz = binsz.GetNbins();

    TArrayD bx(nx+1);
    TArrayD by(ny+1);
    TArrayD bz(nz+1);
    for (int i=0; i<nx; i++) bx[i] = binsx.GetBinLowEdge(i+1);
    for (int i=0; i<ny; i++) by[i] = binsy.GetBinLowEdge(i+1);
    for (int i=0; i<nz; i++) bz[i] = binsz.GetBinLowEdge(i+1);
    bx[nx] = binsx.GetXmax();
    by[ny] = binsy.GetXmax();
    bz[nz] = binsz.GetXmax();

    SetBinning(h, bx, by, bz);
}

// --------------------------------------------------------------------------
//
// Applies the binnings of one root-histogram x to another one h
// Both histograms must be of the same type: TH1, TH2 or TH3
//
void MH::CopyBinning(const TH1 &x, TH1 &h)
{
    if (h.InheritsFrom(TH3::Class()) && x.InheritsFrom(TH3::Class()))
    {
        SetBinning(static_cast<TH3&>(h), *x.GetXaxis(), *x.GetYaxis(), *x.GetZaxis());
        return;
    }
    if (h.InheritsFrom(TH3::Class()) || x.InheritsFrom(TH3::Class()))
        return;
    if (h.InheritsFrom(TH2::Class()) && x.InheritsFrom(TH2::Class()))
    {
        SetBinning(static_cast<TH2&>(h), *x.GetXaxis(), *x.GetYaxis());
        return;
    }
    if (h.InheritsFrom(TH2::Class()) || x.InheritsFrom(TH2::Class()))
        return;
    if (h.InheritsFrom(TH1::Class()) && x.InheritsFrom(TH1::Class()))
    {
        SetBinning(h, *x.GetXaxis());
        return;
    }
}

void MH::RemoveFirstBin(TH1 &h)
{
    if (h.InheritsFrom(TH2::Class()) || h.InheritsFrom(TH3::Class()))
        return;

    const Bool_t haserr = h.GetSumw2N()>0;

    const Int_t n0 = h.GetNbinsX();
    if (n0<2)
        return;

    TArrayD val(n0-1);
    TArrayD er(haserr ? n0-1 : 0);
    for (int i=1; i<n0; i++)
    {
        val[i-1] = h.GetBinContent(i+1);
        if (haserr)
            er[i-1] = h.GetBinError(i+1);
    }

    MBinning bins;
    bins.SetEdges(h, 'x');
    bins.RemoveFirstEdge();
    bins.Apply(h);

    h.Reset();

    for (int i=1; i<n0; i++)
    {
        h.SetBinContent(i, val[i-1]);
        if (haserr)
            h.SetBinError(i, er[i-1]);
    }
}

// --------------------------------------------------------------------------
//
// Multiplies all entries in a TArrayD by a float f
//
void MH::ScaleArray(TArrayD &bins, Double_t f)
{
    for (int i=0; i<bins.GetSize(); i++)
        bins[i] *= f;
}

// --------------------------------------------------------------------------
//
// Scales the binning of a TAxis by a float f
//
TArrayD MH::ScaleAxis(TAxis &axe, Double_t f)
{
    TArrayD arr(axe.GetNbins()+1);

    for (int i=1; i<=axe.GetNbins()+1; i++)
        arr[i-1] = axe.GetBinLowEdge(i);

    ScaleArray(arr, f);

    return arr;
}

// --------------------------------------------------------------------------
//
// Scales the binning of one, two or three axis of a histogram by a float f
//
void MH::ScaleAxis(TH1 &h, Double_t fx, Double_t fy, Double_t fz)
{
    if (h.InheritsFrom(TH3::Class()))
    {
        SetBinning(static_cast<TH3&>(h),
                   ScaleAxis(*h.GetXaxis(), fx),
                   ScaleAxis(*h.GetYaxis(), fy),
                   ScaleAxis(*h.GetZaxis(), fz));
        return;
    }

    if (h.InheritsFrom(TH2::Class()))
    {
        SetBinning(static_cast<TH2&>(h),
                   ScaleAxis(*h.GetXaxis(), fx),
                   ScaleAxis(*h.GetYaxis(), fy));
        return;
    }

    SetBinning(h, ScaleAxis(*h.GetXaxis(), fx));
}

// --------------------------------------------------------------------------
//
// Tries to find a MBinning container with the name "Binning"+name
// in the given parameter list. If it was found it is applied to the
// given histogram. This is only valid for 1D-histograms.
// If the binning is found, but it IsDefault() kTRUE is returned, but
// no binning is applied.
//
Bool_t MH::ApplyBinning(const MParList &plist, const TString name, TH1 &h)
{
    if (h.InheritsFrom(TH2::Class()) || h.InheritsFrom(TH3::Class()))
    {
        gLog << warn << "MH::ApplyBinning: '" << h.GetName() << "' is not a basic TH1 object... no binning applied." << endl;
        return kFALSE;
    }

    const MBinning *bins = (MBinning*)plist.FindObject("Binning"+name);
    if (!bins)
    {
        gLog << inf << "Object 'Binning" << name << "' [MBinning] not found... no binning applied." << endl;
        return kFALSE;
    }

    if (bins->IsDefault())
    {
        gLog << inf << "Object 'Binning" << name << "' [MBinning] is default... binning unchanged." << endl;
        return kTRUE;
    }

    SetBinning(h, *bins);
    return kTRUE;
}

Bool_t MH::ApplyBinning(const MParList &plist, const TString x, const TString y, TH2 &h)
{
    const MBinning *binsx = (MBinning*)plist.FindObject("Binning"+x);
    const MBinning *binsy = (MBinning*)plist.FindObject("Binning"+y);

    if (!binsx && !binsy)
    {
        gLog << inf << "Neither 'Binning" << x << "' nor 'Binning" << y;
        gLog << "' [MBinning] found... no binning applied." << endl;
        return kFALSE;
    }

    if (!binsx)
        gLog << inf << "Object 'Binning" << x << "' [MBinning] not found... binning unchanged." << endl;
    if (!binsy)
        gLog << inf << "Object 'Binning" << y << "' [MBinning] not found... binning unchanged." << endl;

    if (binsx && binsx->IsDefault())
    {
        gLog << inf << "Object 'Binning" << x << "' [MBinning] is default... binning unchanged." << endl;
        binsx = 0;
    }

    if (binsy && binsy->IsDefault())
    {
        gLog << inf << "Object 'Binning" << y << "' [MBinning] is default... binning unchanged." << endl;
        binsy = 0;
    }

    MBinning binsxx, binsyy;
    binsxx.SetEdges(h, 'x');
    binsyy.SetEdges(h, 'y');

    SetBinning(h, binsx?*binsx:binsxx, binsy?*binsy:binsyy);

    return kTRUE;
}

Bool_t MH::ApplyBinning(const MParList &plist, const TString x, const TString y, const TString z, TH3 &h)
{
    const MBinning *binsx = (MBinning*)plist.FindObject("Binning"+x);
    const MBinning *binsy = (MBinning*)plist.FindObject("Binning"+y);
    const MBinning *binsz = (MBinning*)plist.FindObject("Binning"+z);

    if (!binsx && !binsy && !binsz)
    {
        gLog << inf << "Neither 'Binning" << x << "', 'Binning" << y;
        gLog << "' nor 'Binning" << z << "' [MBinning] found... ";
        gLog << "no binning applied." << endl;
        return kFALSE;
    }

    if (!binsx)
        gLog << inf << "Object 'Binning" << x << "' [MBinning] not found... binning unchanged." << endl;
    if (!binsy)
        gLog << inf << "Object 'Binning" << y << "' [MBinning] not found... binning unchanged." << endl;
    if (!binsz)
        gLog << inf << "Object 'Binning" << z << "' [MBinning] not found... binning unchanged." << endl;

    if (binsx && binsx->IsDefault())
    {
        gLog << inf << "Object 'Binning" << x << "' [MBinning] is default... binning unchanged." << endl;
        binsx = 0;
    }

    if (binsy && binsy->IsDefault())
    {
        gLog << inf << "Object 'Binning" << y << "' [MBinning] is default... binning unchanged." << endl;
        binsy = 0;
    }

    if (binsz && binsz->IsDefault())
    {
        gLog << inf << "Object 'Binning" << z << "' [MBinning] is default... binning unchanged." << endl;
        binsy = 0;
    }

    MBinning binsxx, binsyy, binszz;
    binsxx.SetEdges(h, 'x');
    binsyy.SetEdges(h, 'y');
    binszz.SetEdges(h, 'z');

    SetBinning(h, binsx?*binsx:binsxx, binsy?*binsy:binsyy, binsz?*binsz:binszz);

    return kTRUE;
}

void MH::FindGoodLimits(Int_t nbins, Int_t &newbins, Double_t &xmin, Double_t &xmax, Bool_t isInteger)
{
#if ROOT_VERSION_CODE > ROOT_VERSION(3,04,01)
    THLimitsFinder::OptimizeLimits(nbins, newbins, xmin, xmax, isInteger);
#else
//*-*-*-*-*-*-*-*-*Find reasonable bin values*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*              ==========================

    Double_t dx = 0.1*(xmax-xmin);
    Double_t umin = xmin - dx;
    Double_t umax = xmax + dx;

    if (umin < 0 && xmin >= 0)
        umin = 0;

    if (umax > 0 && xmax <= 0)
        umax = 0;

    Double_t binlow  =0;
    Double_t binhigh =0;
    Double_t binwidth=0;

    TGaxis::Optimize(umin, umax, nbins, binlow, binhigh, nbins, binwidth, "");

    if (binwidth <= 0 || binwidth > 1.e+39)
    {
        xmin = -1;
        xmax = 1;
    }
    else
    {
        xmin = binlow;
        xmax = binhigh;
    }

    if (isInteger)
    {
        Int_t ixmin = (Int_t)xmin;
        Int_t ixmax = (Int_t)xmax;
        Double_t dxmin = (Double_t)ixmin;
        Double_t dxmax = (Double_t)ixmax;

        xmin = xmin<0 && xmin!=dxmin ? dxmin - 1 : dxmin;
        xmax = xmax>0 && xmax!=dxmax ? dxmax + 1 : dxmax;

        if (xmin>=xmax)
            xmax = xmin+1;

        Int_t bw = 1 + (Int_t)((xmax-xmin)/nbins);

        nbins = (Int_t)((xmax-xmin)/bw);

        if (xmin+nbins*bw < xmax)
        {
            nbins++;
            xmax = xmin +nbins*bw;
        }
    }

    newbins = nbins;
#endif
}

// --------------------------------------------------------------------------
//
//  Returns the lowest entry in a histogram which is greater than gt (eg >0)
//
Double_t MH::GetMinimumGT(const TH1 &h, Double_t gt)
{
    Double_t min = FLT_MAX;

    const Int_t nx = h.GetXaxis()->GetNbins();
    const Int_t ny = h.GetYaxis()->GetNbins();
    const Int_t nz = h.GetZaxis()->GetNbins();

    for (int iz=1; iz<=nz; iz++)
        for (int iy=1; iy<=ny; iy++)
            for (int ix=1; ix<=nx; ix++)
            {
                const Double_t v = h.GetBinContent(h.GetBin(ix, iy, iz));
                if (gt<v && v<min)
                    min = v;
            }
    return min;
}

// --------------------------------------------------------------------------
//
//  Returns the bin center in a logarithmic scale. If the given bin
//  number is <1 it is set to 1. If it is =GetNbins() it is set to
//  GetNbins()
//
Double_t MH::GetBinCenterLog(const TAxis &axe, Int_t nbin)
{
    if (nbin>axe.GetNbins())
        nbin = axe.GetNbins();

    if (nbin<1)
        nbin = 1;

    const Double_t lo = axe.GetBinLowEdge(nbin);
    const Double_t hi = axe.GetBinUpEdge(nbin);

    const Double_t val = log10(lo) + log10(hi);

    return pow(10, val/2);
}

// --------------------------------------------------------------------------
//
// Draws a copy of the two given histograms. Uses title as the pad title.
// Also layout the two statistic boxes and a legend.
//
void MH::DrawSameCopy(const TH1 &hist1, const TH1 &hist2, const TString title)
{
    //
    // Draw first histogram
    //
    TH1 *h1 = hist1.DrawCopy();
    gPad->SetBorderMode(0);
    gPad->Update();

    // FIXME: Also align max/min with set Maximum/Minimum
    const Double_t maxbin1 = hist1.GetBinContent(hist1.GetMaximumBin());
    const Double_t maxbin2 = hist2.GetBinContent(hist2.GetMaximumBin());
    const Double_t minbin1 = hist1.GetBinContent(hist1.GetMinimumBin());
    const Double_t minbin2 = hist2.GetBinContent(hist2.GetMinimumBin());

    const Double_t max = TMath::Max(maxbin1, maxbin2);
    const Double_t min = TMath::Min(minbin1, minbin2);

    h1->SetMaximum(max>0?max*1.05:max*0.95);
    h1->SetMinimum(max>0?min*0.95:min*1.05);

    TPaveText *t = (TPaveText*)gPad->FindObject("title");
    if (t)
    {
        t->SetName((TString)"MHTitle");     // rename object
        t->Clear();                         // clear old lines
        t->AddText((TString)" "+title+" "); // add the new title
        t->SetBit(kCanDelete);              // make sure object is deleted

        //
        // FIXME: This is a stupid workaround to hide the redrawn
        // (see THistPainter::PaintTitle) title
        //
        gPad->Modified();  // indicates a change
        gPad->Update();    // recreates the original title
        t->Pop();          // bring our title on top
    }

    //
    // Rename first statistics box
    //
    TPaveStats *s1 = dynamic_cast<TPaveStats*>(gPad->FindObject("stats"));
    if (!s1)
        s1 = dynamic_cast<TPaveStats*>(hist1.GetListOfFunctions()->FindObject("stats"));
    else
        s1->SetName((TString)"Stat"+hist1.GetTitle());

    if (s1 && s1->GetX2NDC()>0.95)
    {
        const Double_t x1 = s1->GetX1NDC()-0.01;
        s1->SetX1NDC(x1-(s1->GetX2NDC()-s1->GetX1NDC()));
        s1->SetX2NDC(x1);
    }

    //
    // Draw second histogram
    //
    TH1 *h2 = hist2.DrawCopy("sames");
    gPad->Update();

    //
    // Draw Legend
    //
    TPaveStats *s2 = dynamic_cast<TPaveStats*>(gPad->FindObject("stats"));
    if (!s2)
        s2 = dynamic_cast<TPaveStats*>(hist2.GetListOfFunctions()->FindObject("stats"));

    if (s2)
    {
        TLegend &l = *new TLegend(s2->GetX1NDC(),
                                  s2->GetY1NDC()-0.015-(s2->GetY2NDC()-s2->GetY1NDC())/2,
                                  s2->GetX2NDC(),
                                  s2->GetY1NDC()-0.01
                                 );
        l.AddEntry(h1, h1->GetTitle());
        l.AddEntry(h2, h2->GetTitle());
        l.SetTextSize(s2->GetTextSize());
        l.SetTextFont(s2->GetTextFont());
        l.SetBorderSize(s2->GetBorderSize());
        l.SetBit(kCanDelete);
        l.Draw();
    }
}

// --------------------------------------------------------------------------
//
// Draws the two given histograms. Uses title as the pad title.
// Also layout the two statistic boxes and a legend.
//
void MH::DrawSame(TH1 &hist1, TH1 &hist2, const TString title, Bool_t same)
{
    //
    // Draw first histogram
    //
    hist1.Draw(same?"same":"");
    gPad->SetBorderMode(0);
    gPad->Update();

    if (hist1.GetEntries()>0 && hist2.GetEntries()>0)
    {
        const Double_t maxbin1 = hist1.GetBinContent(hist1.GetMaximumBin());
        const Double_t maxbin2 = hist2.GetBinContent(hist2.GetMaximumBin());
        const Double_t minbin1 = hist1.GetBinContent(hist1.GetMinimumBin());
        const Double_t minbin2 = hist2.GetBinContent(hist2.GetMinimumBin());

        const Double_t max = TMath::Max(maxbin1, maxbin2);
        const Double_t min = TMath::Min(minbin1, minbin2);

        if (max!=min)
        {
            hist1.SetMaximum(max>0?max*1.05:max*0.95);
            hist1.SetMinimum(max>0?min*0.95:min*1.05);
        }
    }

    TPaveText *t = (TPaveText*)gPad->FindObject("title");
    if (t)
    {
        t->SetName((TString)"MHTitle");     // rename object
        t->Clear();                         // clear old lines
        t->AddText((TString)" "+title+" "); // add the new title
        t->SetBit(kCanDelete);              // make sure object is deleted

        //
        // FIXME: This is a stupid workaround to hide the redrawn
        // (see THistPainter::PaintTitle) title
        //
        gPad->Modified();  // indicates a change
        gPad->Update();    // recreates the original title
        t->Pop();          // bring our title on top
    }

    //
    // Rename first statistics box
    //
    // Where to get the TPaveStats depends on the root version
    TPaveStats *s1 = dynamic_cast<TPaveStats*>(gPad->FindObject("stats"));
    if (!s1)
        s1 = dynamic_cast<TPaveStats*>(hist1.GetListOfFunctions()->FindObject("stats"));
    else
        s1->SetName((TString)"Stat"+hist1.GetTitle());

    if (s1 && s1->GetX2NDC()>0.95)
    {
        const Double_t x1 = s1->GetX1NDC()-0.01;
        s1->SetX1NDC(x1-(s1->GetX2NDC()-s1->GetX1NDC()));
        s1->SetX2NDC(x1);
    }

    //
    // Draw second histogram
    //
    hist2.Draw("sames");
    gPad->Update();

    //
    // Draw Legend
    //
    // Where to get the TPaveStats depends on the root version
    TPaveStats *s2 = dynamic_cast<TPaveStats*>(gPad->FindObject("stats"));
    if (!s2)
        s2 = dynamic_cast<TPaveStats*>(hist2.GetListOfFunctions()->FindObject("stats"));

    if (s2)
    {
        TLegend &l = *new TLegend(s2->GetX1NDC(),
                                  s2->GetY1NDC()-0.015-(s2->GetY2NDC()-s2->GetY1NDC())/2,
                                  s2->GetX2NDC(),
                                  s2->GetY1NDC()-0.01
                                 );
        l.AddEntry(&hist1, hist1.GetTitle());
        l.AddEntry(&hist2, hist2.GetTitle());
        l.SetTextSize(s2->GetTextSize());
        l.SetTextFont(s2->GetTextFont());
        l.SetBorderSize(s2->GetBorderSize());
        l.SetBit(kCanDelete);
        l.Draw();
    }
}

// --------------------------------------------------------------------------
//
// If the opt string contains 'nonew' or gPad is not given NULL is returned.
// Otherwise the present gPad is returned.
//
TVirtualPad *MH::GetNewPad(TString &opt)
{
    opt.ToLower();

    if (!opt.Contains("nonew"))
        return NULL;

    opt.ReplaceAll("nonew", "");

    return gPad;
}

// --------------------------------------------------------------------------
//
// Encapsulate the TObject::Clone such, that a cloned TH1 (or derived)
// object is not added to the current directory, when cloned.
//
TObject *MH::Clone(const char *name) const
{
    const Bool_t store = TH1::AddDirectoryStatus();

    TH1::AddDirectory(kFALSE);
    TObject *o = MParContainer::Clone(name);
    TH1::AddDirectory(store);

    return o;
}

// --------------------------------------------------------------------------
//
// If the opt string contains 'nonew' or gPad is not given a new canvas
// with size w/h is created. Otherwise the object is cloned and drawn
// to the present pad. The kCanDelete bit is set for the clone.
//
TObject *MH::DrawClone(Option_t *opt, Int_t w, Int_t h) const
{
    TString option(opt);

    TVirtualPad *p = GetNewPad(option);
    if (!p)
        p = MakeDefCanvas(this, w, h);
    else
        if (!option.Contains("same", TString::kIgnoreCase))
            p->Clear();

    gROOT->SetSelectedPad(NULL);

    TObject *o = MParContainer::DrawClone(option);
    o->SetBit(kCanDelete);
    return o;
}

// --------------------------------------------------------------------------
//
// Check whether a class inheriting from MH overwrites the Draw function
//
Bool_t MH::OverwritesDraw(TClass *cls) const
{
    if (!cls)
        cls = IsA();

    //
    // Check whether we reached the base class MTask
    //
    if (TString(cls->GetName())=="MH")
        return kFALSE;

    //
    // Check whether the class cls overwrites Draw
    //
    if (cls->GetMethodAny("Draw"))
        return kTRUE;

    //
    // If the class itself doesn't overload it check all it's base classes
    //
    TBaseClass *base=NULL;
    TIter NextBase(cls->GetListOfBases());
    while ((base=(TBaseClass*)NextBase()))
    {
        if (OverwritesDraw(base->GetClassPointer()))
            return kTRUE;
    }

    return kFALSE;
}

// --------------------------------------------------------------------------
//
//  Cuts the bins containing only zeros at the edges.
//
//  A new number of bins can be defined with nbins != 0
//        In the case of nbins == 0, no rebinning will take place
//
//  Returns the new (real) number of bins
//
Int_t MH::StripZeros(TH1 &h, Int_t nbins)
{
    TAxis &axe = *h.GetXaxis();

    const Int_t min1   = axe.GetFirst();
    const Int_t max1   = axe.GetLast();
    const Int_t range1 = max1-min1;

    //
    // Check for useless zeros
    //
    if (range1 == 0)
        return 0;

    Int_t min2 = 0;
    for (int i=min1; i<=max1; i++)
        if (h.GetBinContent(i) != 0)
        {
            min2 = i;
            break;
        }

    //
    // If the histogram consists of zeros only
    //
    if (min2 == max1)
        return 0;

    Int_t max2 = 0;
    for (int i=max1; i>=min2; i--)
        if (h.GetBinContent(i) != 0)
        {
            max2 = i;
            break;
        }

    //
    // Appying TAxis->SetRange before ReBin does not work ...
    // But this workaround helps quite fine
    //
    Axis_t min = h.GetBinLowEdge(min2);
    Axis_t max = h.GetBinLowEdge(max2)+h.GetBinWidth(max2);

    Int_t nbins2 = max2-min2;
    //
    // Check for rebinning
    //
    if (nbins > 0)
      {
        const Int_t ngroup = (Int_t)(nbins2*h.GetNbinsX()/nbins/(max1-min1));
        if (ngroup > 1)
          {
            h.Rebin(ngroup);
            nbins2 /= ngroup;
          }
      }
    
    Int_t newbins = 0;
    FindGoodLimits(nbins2, newbins, min, max, kFALSE);
    axe.SetRangeUser(min,max);
    return axe.GetLast()-axe.GetFirst();
}

// --------------------------------------------------------------------------
//
// In contradiction to TPad::FindObject this function searches recursively
// in a pad for an object. gPad is the default.
//
TObject *MH::FindObjectInPad(const char *name, TVirtualPad *pad)
{
    if (!pad)
        pad = gPad;

    if (!pad)
        return NULL;

    TObject *o;

    TIter Next(pad->GetListOfPrimitives());
    while ((o=Next()))
    {
        if (!strcmp(o->GetName(), name))
            return o;

        if (o->InheritsFrom("TPad"))
            if ((o = FindObjectInPad(name, (TVirtualPad*)o)))
                return o;
    }
    return NULL;
}

// --------------------------------------------------------------------------
//
// 
//
TH1I* MH::ProjectArray(const TArrayF &array, Int_t nbins, 
                       const char* name, const char* title)
{
    const Int_t size = array.GetSize();

    TH1I *h1=0;

    //check if histogram with identical name exist
    TObject *h1obj = gROOT->FindObject(name);
    if (h1obj && h1obj->InheritsFrom("TH1I"))
    {
        h1 = (TH1I*)h1obj;
        h1->Reset();
    }

    Double_t min = size>0 ? array[0] : 0;
    Double_t max = size>0 ? array[0] : 1;

    // first loop over array to find the min and max
    for (Int_t i=1; i<size;i++)
    {
        max = TMath::Max((Double_t)array[i], max);
        min = TMath::Min((Double_t)array[i], min);
    }

    Int_t newbins = 0;
    FindGoodLimits(nbins, newbins, min, max, kFALSE);

    if (!h1)
    {
        h1 = new TH1I(name, title, nbins, min, max);
        h1->SetXTitle("");
        h1->SetYTitle("Counts");
        h1->SetDirectory(NULL);
    }

    // Second loop to fill the histogram
    for (Int_t i=0;i<size;i++)
        h1->Fill(array[i]);

    return h1;
}

// --------------------------------------------------------------------------
//
// 
//
TH1I* MH::ProjectArray(const TArrayD &array, Int_t nbins, const char* name, const char* title)
{
    const Int_t size = array.GetSize();

    Double_t min = size>0 ? array[0] : 0;
    Double_t max = size>0 ? array[0] : 1;

    TH1I *h1=0;

    //check if histogram with identical name exist
    TObject *h1obj = gROOT->FindObject(name);
    if (h1obj && h1obj->InheritsFrom("TH1I"))
    {
        h1 = (TH1I*)h1obj;
        h1->Reset();
    }

    // first loop over array to find the min and max
    for (Int_t i=1; i<size;i++)
    {
        max = TMath::Max(array[i], max);
        min = TMath::Min(array[i], min);
    }

    Int_t newbins = 0;
    FindGoodLimits(nbins, newbins, min, max, kFALSE);

    if (!h1)
    {
      h1 = new TH1I(name, title, newbins, min, max);
      h1->SetXTitle("");
      h1->SetYTitle("Counts");
      h1->SetDirectory(NULL);
    }
    
    // Second loop to fill the histogram
    for (Int_t i=0;i<size;i++)
        h1->Fill(array[i]);

    return h1;
}

// --------------------------------------------------------------------------
//
// 
//
TH1I* MH::ProjectArray(const MArrayF &array, Int_t nbins, 
                       const char* name, const char* title)
{
    return ProjectArray(TArrayF(array.GetSize(),array.GetArray()), nbins, name, title);
}

// --------------------------------------------------------------------------
//
// 
//
TH1I* MH::ProjectArray(const MArrayD &array, Int_t nbins, const char* name, const char* title)
{
    return ProjectArray(TArrayD(array.GetSize(),array.GetArray()), nbins, name, title);
}

// --------------------------------------------------------------------------
//
// This is a workaround for rrot-version <5.13/04 to get correct
// binomial errors even if weights have been used, do:
//    h->Divide(h1, h2, 5, 2, "b");
//    MH::SetBinomialErrors(*h, *h1, *h2, 5, 2);
//
// see http://root.cern.ch/phpBB2/viewtopic.php?p=14818
// see http://savannah.cern.ch/bugs/?20722
//
void MH::SetBinomialErrors(TH1 &hres, const TH1 &h1, const TH1 &h2, Double_t c1, Double_t c2)
{
    for (Int_t binx=0; binx<=hres.GetNbinsX()+1; binx++)
    {
        const Double_t b1 = h1.GetBinContent(binx);
        const Double_t b2 = h2.GetBinContent(binx);
        const Double_t e1 = h1.GetBinError(binx);
        const Double_t e2 = h2.GetBinError(binx);

        //const Double_t w  = c2*b2 ? (c1*b1)/(c2*b2) : 0;
        //const Double_t rc = ((1-2*w)*e1*e1+w*w*e2*e2)/(b2*b2);

        if (b2==0)
        {
            hres.SetBinError(binx, 0);
            continue;
        }

        const Double_t c = c2==0 ? 1 : c1/c2;
        const Double_t u = b2==0 ? 0 : b1/b2;

        const Double_t rc = c*((1-2*u)*e1*e1+u*u*e2*e2)/(b2*b2);

        hres.SetBinError(binx, TMath::Sqrt(TMath::Abs(rc)));
    }
}

// --------------------------------------------------------------------------
//
// Return the first and last bin of the histogram which is not 0
//
void MH::GetRange(const TH1 &h, Int_t &lo, Int_t &hi)
{
    lo = 0;
    hi = 1;

    for (int i=1; i<=h.GetNbinsX(); i++)
    {
        if (lo==0 && h.GetBinContent(i)>0)
            lo = i;

        if (h.GetBinContent(i)>0)
            hi = i;
    }
}

// --------------------------------------------------------------------------
//
// Return the lower edge of the first and the upper edge of the last bin
// of the histogram which is not 0
//
void MH::GetRangeUser(const TH1 &h, Axis_t &lo, Axis_t &hi)
{
    Int_t f, l;
    GetRange(h, f, l);

    lo = h.GetBinLowEdge(f);
    hi = h.GetBinLowEdge(l+1);
}

// --------------------------------------------------------------------------
//
// Return the first and last column (x-bin) of the histogram which is not 0.
// Therefor a proper projection is produced if the argument is a TH2.
//
// TH3 are not yet supported
//
void MH::GetRangeX(const TH1 &hist, Int_t &lo, Int_t &hi)
{
    if (hist.InheritsFrom(TH3::Class()))
        return;

    if (hist.InheritsFrom(TH2::Class()))
    {
        TH1 *pro = static_cast<const TH2&>(hist).ProjectionX();
        GetRange(*pro, lo, hi);
        delete pro;
        return;
    }

    GetRange(hist, lo, hi);
}

// --------------------------------------------------------------------------
//
// Return the first and last row (y-bin) of the histogram which is not 0.
// Therefor a proper projection is produced if the argument is a TH2.
//
// TH3 are not yet supported
//
void MH::GetRangeY(const TH1 &hist, Int_t &lo, Int_t &hi)
{
    if (hist.InheritsFrom(TH3::Class()))
        return;

    if (hist.InheritsFrom(TH2::Class()))
    {
        TH1 *pro = static_cast<const TH2&>(hist).ProjectionY();
        GetRange(*pro, lo, hi);
        delete pro;
    }
}

// --------------------------------------------------------------------------
//
// Return the lower edge of the first and the upper edge of the last bin
// of the histogram h returned by GetRangeX
//
void MH::GetRangeUserX(const TH1 &h, Axis_t &lo, Axis_t &hi)
{
    Int_t f, l;
    GetRangeX(h, f, l);

    lo = h.GetXaxis()->GetBinLowEdge(f);
    hi = h.GetXaxis()->GetBinLowEdge(l+1);
}

// --------------------------------------------------------------------------
//
// Return the lower edge of the first and the upper edge of the last bin
// of the histogram h returned by GetRangeY
//
void MH::GetRangeUserY(const TH1 &h, Axis_t &lo, Axis_t &hi)
{
    Int_t f, l;
    GetRangeY(h, f, l);

    lo = h.GetYaxis()->GetBinLowEdge(f);
    hi = h.GetYaxis()->GetBinLowEdge(l+1);
}

// --------------------------------------------------------------------------
//
// See MTask::PrintSkipped
//
void MH::PrintSkipped(UInt_t n, const char *str)
{
    *fLog << " " << setw(7) << n;
    if (GetNumExecutions()>0)
    {
        *fLog << " (" << MString::Format("%5.1f", 100.*n/GetNumExecutions());
        *fLog << "%)" << str << endl;
    }
    //*fLog << " Evts skipped: " << str << endl;
}

#ifdef CreateGradientColorTable
#error CreateGradientColorTable already defined
#endif

#if ROOT_VERSION_CODE < ROOT_VERSION(5,18,00)
#define CreateGradientColorTable gStyle->CreateGradientColorTable
#else
#define CreateGradientColorTable TColor::CreateGradientColorTable
#endif

// --------------------------------------------------------------------------
//
// Calls gStyle->SetPalette. Allowed palettes are:
//  pretty
//  deepblue:  darkblue -> lightblue
//  lightblue: black -> blue -> white
//  greyscale: black -> white
//  glow1:     black -> darkred -> orange -> yellow -> white
//  glow2:
//  glowsym:   lightblue -> blue -> black -> darkred -> orange -> yellow -> white
//  redish:    darkred -> lightred
//  bluish:    darkblue -> lightblue
//  small1:
//
// If the palette name contains 'inv' the order of the colors is inverted.
//
// The second argument determines the number of colors for the palette.
// The default is 50. 'pretty' always has 50 colors.
//
// (Remark: Drawing 3D object like TH2D with surf3 allows a maximum
//          of 99 colors)
//
void MH::SetPalette(TString paletteName, Int_t ncol)
{
    Bool_t found=kFALSE;

    paletteName.ToLower();

    const Bool_t inverse = paletteName.Contains("inv");

    if (paletteName.Contains("pretty"))
    {
        gStyle->SetPalette(1, 0);
        ncol=50;
        found=kTRUE;
    }

    if (paletteName.Contains("deepblue"))
    {
        Double_t s[5] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
        Double_t r[5] = { 0.00, 0.09, 0.18, 0.09, 0.00 };
        Double_t g[5] = { 0.01, 0.02, 0.39, 0.68, 0.97 };
        Double_t b[5] = { 0.17, 0.39, 0.62, 0.79, 0.97 };
        CreateGradientColorTable(5, s, r, g, b, ncol);
        found=kTRUE;
    }

    if (paletteName.Contains("lightblue"))
    {
        Double_t s[5] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
        Double_t r[5] = { 0.00, 0.09, 0.18, 0.09, 0.00 };
        Double_t g[5] = { 0.00, 0.02, 0.40, 0.70, 1.00 };
        Double_t b[5] = { 0.00, 0.27, 0.51, 0.81, 1.00 };
        CreateGradientColorTable(5, s, r, g, b, ncol);
        found=kTRUE;
    }

    if (paletteName.Contains("greyscale"))
    {
        double s[2] = {0.00, 1.00};
        double r[2] = {0.00, 1.00};
        double g[2] = {0.00, 1.00};
        double b[2] = {0.00, 1.00};
        CreateGradientColorTable(2, s, r, g, b, ncol);
        found=kTRUE;
    }

    if (paletteName.Contains("glow1"))
    {
        double s[5] = {0., 0.10, 0.45, 0.75, 1.00};
        double r[5] = {0., 0.35, 0.85, 1.00, 1.00};
        double g[5] = {0., 0.10, 0.20, 0.73, 1.00};
        double b[5] = {0., 0.03, 0.06, 0.00, 1.00};
        CreateGradientColorTable(5, s, r, g, b, ncol);
        found=kTRUE;
    }

    if (paletteName.Contains("glow2"))
    {
        double s[4] = {0.00, 0.50, 0.75, 1.00};
        double r[4] = {0.24, 0.67, 1.00, 1.00};
        double g[4] = {0.03, 0.04, 0.80, 1.00};
        double b[4] = {0.03, 0.04, 0.00, 1.00};
        CreateGradientColorTable(4, s, r, g, b, ncol);
        found=kTRUE;
    }

    if (paletteName.Contains("glowsym"))
    {
        double s[8] = {0.00, 0.17, 0.39, 0.50, 0.55, 0.72, 0.88, 1.00};
        double r[8] = {0.09, 0.18, 0.09, 0.00, 0.35, 0.85, 1.00, 1.00};
        double g[8] = {0.70, 0.40, 0.02, 0.00, 0.10, 0.20, 0.73, 1.00};
        double b[8] = {0.81, 0.51, 0.27, 0.00, 0.03, 0.06, 0.00, 1.00};
        CreateGradientColorTable(8, s, r, g, b, ncol);
        found=kTRUE;
    }/*
    if (paletteName.Contains("glows2"))
    {
        double s[10] = {0.00, 0.17, 0.35, 0.50, 0.65, 0.73, 0.77, 0.85, 0.92, 1.00};
        double r[10] = {0.09, 0.18, 0.09, 0.00, 0.00, 0.20, 0.55, 0.85, 1.00, 1.00};
        double g[10] = {0.81, 0.51, 0.27, 0.00, 0.00, 0.05, 0.10, 0.20, 0.73, 1.00};
        double b[10] = {0.70, 0.40, 0.02, 0.00, 0.27, 0.40, 0.35, 0.16, 0.03, 1.00};
        gStyle->CreateGradientColorTable(10, s, r, g, b, ncol);
        found=kTRUE;
    }*/

    if (paletteName.Contains("redish"))
    {
        double s[3] = {0., 0.5, 1.};
        double r[3] = {0., 1.0, 1.};
        double g[3] = {0., 0.0, 1.};
        double b[3] = {0., 0.0, 1.};
        CreateGradientColorTable(3, s, r, g, b, ncol);
        found=kTRUE;
    }

    if (paletteName.Contains("bluish"))
    {
        double s[3] = {0., 0.5, 1.};
        double r[3] = {0., 0.0, 1.};
        double g[3] = {0., 0.0, 1.};
        double b[3] = {0., 1.0, 1.};
        CreateGradientColorTable(3, s, r, g, b, ncol);
        found=kTRUE;
    }

    if (paletteName.Contains("small1"))
    {
        double s[4] = {0.00, 0.50, 0.95, 1.};
        double r[4] = {0.04, 0.28, 0.98, 1.};
        double g[4] = {0.28, 0.93, 0.03, 1.};
        double b[4] = {0.79, 0.11, 0.03, 1.};
        CreateGradientColorTable(4, s, r, g, b, ncol);
        found=kTRUE;
    }
    if (paletteName.Contains("pepe"))
    {
        double s[5] = {0.0, 0.6, 0.7, 0.9, 1.0 };
        double r[5] = {0.1, 0.1, 1.0, 1.0, 1.0 };
        double g[5] = {0.1, 0.1, 0.0, 1.0, 1.0 };
        double b[5] = {0.2, 0.7, 0.0, 0.3, 0.9 };
        CreateGradientColorTable(5, s, r, g, b, ncol);
        found=kTRUE;
    }
    if (paletteName.Contains("temp"))
    {
        Double_t s[7] = { 0.00, 0.30, 0.45, 0.75, 0.88, 0.95, 1.00 };
        Double_t r[7] = { 0.00, 0.00, 0.30, 0.60, 1.00, 1.00, 1.00 };
        Double_t g[7] = { 0.00, 0.00, 0.00, 0.00, 0.20, 1.00, 1.00 };
        Double_t b[7] = { 0.10, 0.60, 0.30, 0.00, 0.20, 0.00, 1.00 };
        CreateGradientColorTable(7, s, r, g, b, ncol);
        found=kTRUE;
    }

    if (inverse)
    {
        TArrayI c(ncol);
        for (int i=0; i<ncol; i++)
            c[ncol-i-1] = gStyle->GetColorPalette(i);
        gStyle->SetPalette(ncol, c.GetArray());
    }

    if (!found)
        gLog << warn << "MH::SetPalette: Palette " << paletteName << " unknown... ignored." << endl;
}

// --------------------------------------------------------------------------
//
// Unfortunately in TH1::GetObjectInfo the buffer is just 64 characters
// which is sometimes to small. This is just a copy of the code but the
// buffer has been increased to 128 which should fairly be enough.
//
//  Necessary for root <= 5.22/00
//
char *MH::GetObjectInfoH(Int_t px, Int_t py, const TH1 &h)
{
   const TH1 *fH = &h;
   const TAxis *fXaxis = h.GetXaxis();
   const TAxis *fYaxis = h.GetYaxis();

   //   Redefines TObject::GetObjectInfo.
   //   Displays the histogram info (bin number, contents, integral up to bin
   //   corresponding to cursor position px,py

   if (!gPad) return (char*)"";

   static char info[128];
   Double_t x  = gPad->PadtoX(gPad->AbsPixeltoX(px));
   Double_t y  = gPad->PadtoY(gPad->AbsPixeltoY(py));
   Double_t x1 = gPad->PadtoX(gPad->AbsPixeltoX(px+1));
   const char *drawOption = fH->GetDrawOption();
   Double_t xmin, xmax, uxmin,uxmax;
   Double_t ymin, ymax, uymin,uymax;
   if (fH->GetDimension() == 2) {
      if (gPad->GetView() || strncmp(drawOption,"cont",4) == 0
                          || strncmp(drawOption,"CONT",4) == 0) {
         uxmin=gPad->GetUxmin();
         uxmax=gPad->GetUxmax();
         xmin = fXaxis->GetBinLowEdge(fXaxis->GetFirst());
         xmax = fXaxis->GetBinUpEdge(fXaxis->GetLast());
         x = xmin +(xmax-xmin)*(x-uxmin)/(uxmax-uxmin);
         uymin=gPad->GetUymin();
         uymax=gPad->GetUymax();
         ymin = fYaxis->GetBinLowEdge(fYaxis->GetFirst());
         ymax = fYaxis->GetBinUpEdge(fYaxis->GetLast());
         y = ymin +(ymax-ymin)*(y-uymin)/(uymax-uymin);
      }
   }
   Int_t binx,biny,binmin,binx1;
   if (gPad->IsVertical()) {
      binx   = fXaxis->FindFixBin(x);
      binmin = fXaxis->GetFirst();
      binx1  = fXaxis->FindFixBin(x1);
      // special case if more than 1 bin in x per pixel
      if (binx1-binx>1 && fH->GetDimension() == 1) {
         Double_t binval=fH->GetBinContent(binx);
         Int_t binnear=binx;
         for (Int_t ibin=binx+1; ibin<binx1; ibin++) {
            Double_t binvaltmp = fH->GetBinContent(ibin);
            if (TMath::Abs(y-binvaltmp) < TMath::Abs(y-binval)) {
               binval=binvaltmp;
               binnear=ibin;
            }
         }
         binx = binnear;
      }
   } else {
      x1 = gPad->PadtoY(gPad->AbsPixeltoY(py+1));
      binx   = fXaxis->FindFixBin(y);
      binmin = fXaxis->GetFirst();
      binx1  = fXaxis->FindFixBin(x1);
      // special case if more than 1 bin in x per pixel
      if (binx1-binx>1 && fH->GetDimension() == 1) {
         Double_t binval=fH->GetBinContent(binx);
         Int_t binnear=binx;
         for (Int_t ibin=binx+1; ibin<binx1; ibin++) {
            Double_t binvaltmp = fH->GetBinContent(ibin);
            if (TMath::Abs(x-binvaltmp) < TMath::Abs(x-binval)) {
               binval=binvaltmp;
               binnear=ibin;
            }
         }
         binx = binnear;
      }
   }
   if (fH->GetDimension() == 1) {
      Double_t integ = 0;
      for (Int_t bin=binmin;bin<=binx;bin++) {integ += fH->GetBinContent(bin);}
      sprintf(info,"(x=%g, y=%g, binx=%d, binc=%g, Sum=%g)",x,y,binx,fH->GetBinContent(binx),integ);
   } else {
      biny = fYaxis->FindFixBin(y);
      sprintf(info,"(x=%g, y=%g, binx=%d, biny=%d, binc=%g)",x,y,binx,biny,fH->GetCellContent(binx,biny));
   }
   return info;
}

// --------------------------------------------------------------------------
//
// Unfortunately in TProfile::GetObjectInfo the buffer is just 64 characters
// which is sometimes to small. This is just a copy of the code but the
// buffer has been increased to 128 which should fairly be enough.
//
//  Necessary for root <= 5.22/00
//
char *MH::GetObjectInfoP(Int_t px, Int_t py, const TProfile &p)
{
    if (!gPad) return (char*)"";
    static char info[128];
    Double_t x  = gPad->PadtoX(gPad->AbsPixeltoX(px));
    Double_t y  = gPad->PadtoY(gPad->AbsPixeltoY(py));
    Int_t binx   = p.GetXaxis()->FindFixBin(x);
    sprintf(info,"(x=%g, y=%g, binx=%d, binc=%g, bine=%g, binn=%d)", x, y, binx, p.GetBinContent(binx), p.GetBinError(binx), (Int_t)p.GetBinEntries(binx));
    return info;
}

// --------------------------------------------------------------------------
//
// Unfortunately TH1::GetObjectInfo and TProfile::GetObjectInfo can
// result in buffer ovwerflows therefor we have to re-implement these
// function by our own.
//
//  Necessary for root <= 5.22/00
//
char *MH::GetObjectInfo(Int_t px, Int_t py, const TObject &o)
{
    if (!o.InheritsFrom(TH1::Class()))
        return o.GetObjectInfo(px, py);

    if (o.InheritsFrom(TProfile::Class()))
        return GetObjectInfoP(px, py, static_cast<const TProfile&>(o));

    if (o.InheritsFrom("MHCamera"))
        return o.GetObjectInfo(px, py);

    if (o.InheritsFrom(TH1::Class()))
        return GetObjectInfoH(px, py, static_cast<const TH1&>(o));

    return const_cast<char*>("MH::GetObjectInfo: unknown class.");
}

// --------------------------------------------------------------------------
//
// Set the pad-range such that at the smallest width it is still
// two times max and that the displayed "pixels" (i.e. its coordinate
// system) have the width to height ratio of aspect.
//
void MH::SetPadRange(Float_t max, Float_t aspect)
{
    if (!gPad)
        return;

    const Float_t w = gPad->GetWw();
    const Float_t h = gPad->GetWh();

    if (w>aspect*h)
    {
        const Double_t dx = ((w/h-aspect)/2+1)*max;
        gPad->Range(-dx, -max, dx, max);
    }
    else
    {
        const Double_t dy = ((h/w-1./aspect)/2+1)*max;
        gPad->Range(-max, -dy, max, dy);
    }
}

// --------------------------------------------------------------------------
//
// Set the range of a pad in a way that the coordinates fit into the pad
// without abberation.
//
void MH::SetPadRange(Float_t x0, Float_t y0, Float_t x1, Float_t y1)
{
    if (!gPad)
        return;

    const Float_t w = x1-x0;                              // Width  in user coordinates
    const Float_t h = y1-y0;                              // Hieght in user coordinates

    const Float_t ww = gPad->GetWw()*gPad->GetAbsWNDC();  // Width  of pad in pixels
    const Float_t hh = gPad->GetWh()*gPad->GetAbsHNDC();  // Height of pad in pixels

    if (ww/hh > w/h)
    {
        const Double_t dx = (ww/hh-w/h)/2*h;

        gPad->Range(x0-dx, y0, x1+dx, y1);
    }
    else
    {
        const Double_t dy = (hh/ww-h/w)/2*w;

        gPad->Range(x0, y0-dy, x1, y1+dy);
    }
}
