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
//  MAstroCatalog
//  =============
//
//  THIS IMPLEMENTATION IS PRELIMINARY AND WILL BE MERGED WITH
//  SOME PARTS OF THE DRIVE SOFTWARE SOON!
//
//
//  Catalogs:
//  ---------
//
//  To be able to use this class you need a catalog file suppored by
//  MAstroCatalog.
//  Catalog files can be found at
//  http://magic.astro.uni-wuerzburg.de/mars/catalogs.html
//  You must copy the file into the directory from which you start your macro
//  or give an abolute path loading the catalog.
//
//
//  Usage:
//  ------
//
//  To display a starfield you must have a supported catalog, then do:
//
//    MTime time;
//    // Time for which to get the picture
//    time.Set(2004, 2, 28, 20, 14, 7);
//    // Current observatory
//    MObservatory magic1;
//    // Right Ascension [h] and declination [deg] of source
//    // Currently 'perfect' pointing is assumed
//    const Double_t ra  = MAstro::Hms2Rad(5, 34, 31.9);
//    const Double_t dec = MAstro::Dms2Rad(22, 0, 52.0);
//    MAstroCatalog stars;
//    // Magnitude up to which the stars are loaded from the catalog
//    stars.SetLimMag(6);
//    // Radius of FOV around the source position to load the stars
//    stars.SetRadiusFOV(3);
//    // Source position
//    stars.SetRaDec(ra, dec);
//    // Catalog to load (here: Bright Star Catalog V5)
//    stars.ReadBSC("bsc5.dat");
//    // Obersavatory and time to also get local coordinate information
//    stars.SetObservatory(magic1);
//    stars.SetTime(time);
//    // Enable interactive GUI
//    stars.SetGuiActive();
//    //Clone the catalog due to the validity range of the instance
//    TObject *o = stars.Clone();
//    o->SetBit(kCanDelete);
//    o->Draw();
//
//  If no time and/or Obervatory location is given no local coordinate
//  information is displayed.
//
//
//  Coordinate Transformation:
//  -------------------------
//  The conversion from sky coordinates to local coordinates is done using
//  MAstroSky2Local which does a simple rotation of the coordinate system.
//  This is inaccurate in the order of 30arcsec due to ignorance of all
//  astrometrical corrections (nutation, precission, abberation, ...)
//
//
//  GUI:
//  ----
//    * If the gui is interactive you can use the cursor keys to change
//      the position you are looking at and with plus/minus you
//      can (un)zoom the FOV (Field Of View)
//    * The displayed values mean the following:
//        + alpha: Right Ascension
//        + delta: Declination
//        + theta: zenith distance / zenith angle
//        + phi:   azimuth angle
//        + rho:   angle of rotation sky-coordinate system vs local-
//                 coordinate system
//        + time of display
//    * Move the mouse on top of the grid points or the stars to get
//      more setailed information.
//    * Enable the event-info in a canvas to see the current
//      ObjectInfo=tooltip-text
//    * call SetNoToolTips to supress the tooltips
//    * the blue lines are the local coordinat system
//    * the red lines are sky coordinate system
//
//
//  ToDo:
//  -----
//   - replace MVetcor3 by a more convinient class. Maybe use TExMap, too.
//   - change tooltips to multi-line tools tips as soon as root
//     supports them
//   - a derived class is missing which supports all astrometrical
//     correction (base on slalib and useable in Cosy)
//   - Implement a general loader for heasarc catlogs, see
//     http://heasarc.gsfc.nasa.gov/W3Browse/star-catalog/
//
//  Class Version 2:
//    + MAttLine fAttLineSky;    // Line Style and color for sky coordinates
//    + MAttLine fAttLineLocal;  // Line Style and color for local coordinates
//    + added new base class TAttMarker
//
//////////////////////////////////////////////////////////////////////////////
#include "MAstroCatalog.h"

#include <errno.h>      // strerror
#include <stdlib.h>     // ati, atof
#include <limits.h>     // INT_MAX (Suse 7.3/gcc 2.95)

#include <fstream>

#include <KeySymbols.h> // kKey_*

#include <TLine.h>      // TLine
#include <TMarker.h>    // TMarker
#include <TCanvas.h>    // TCanvas
#include <TArrayI.h>    // TArrayI
#include <TGToolTip.h>  // TGToolTip
#include <TPaveText.h>  // TPaveText

#include <TH1.h>        // TH1F
#include <TGraph.h>     // TGraph

#include "MLog.h"
#include "MLogManip.h"

#include "izstream.h"   // izstream <ifstream>

#include "MTime.h"
#include "MString.h"
#include "MAstro.h"
#include "MAstroSky2Local.h"
#include "MObservatory.h"

#undef DEBUG
//#define DEBUG

#ifdef DEBUG
#include <TStopwatch.h>
#endif

ClassImp(MAttLine);
ClassImp(MAstroCatalog);

using namespace std;

// Datacenter default path for catalogs
const TString MAstroCatalog::kDefaultPath="/magic/datacenter/setup/";

// --------------------------------------------------------------------------
//
//  Default Constructor. Set Default values:
//   fLimMag    = 99
//   fRadiusFOV = 90
//
MAstroCatalog::MAstroCatalog() : fLimMag(99), fRadiusFOV(90), fToolTip(0), fObservatory(0), fTime(0)
{
    fList.SetOwner();
    fMapG.SetOwner();

    fToolTip = gROOT->IsBatch() || !gClient ? 0 : new TGToolTip(0, "", 0);

    fAttLineSky.SetLineStyle(kDashDotted);
    fAttLineLocal.SetLineStyle(kDashDotted);

    fAttLineSky.SetLineColor(kRed);
    fAttLineLocal.SetLineColor(kBlue);

    SetMarkerColor(kBlack);
    SetMarkerStyle(kCircle);
}

// --------------------------------------------------------------------------
//
// Destructor. Delete fTime, fObservatory. Disconnect signal. delete tooltip.
// Delete Map with gui primitives
//
MAstroCatalog::~MAstroCatalog()
{
    // First disconnect the EventInfo...
    // FIXME: There must be an easier way!
    TIter Next(gROOT->GetListOfCanvases());
    TCanvas *c;
    while ((c=(TCanvas*)Next()))
        c->Disconnect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", this,
                      "EventInfo(Int_t,Int_t,Int_t,TObject*)");

    // Now delete the data members
    if (fTime)
        delete fTime;
    if (fObservatory)
        delete fObservatory;

    if (fToolTip)
    {
        fToolTip->Hide();
        delete fToolTip;
    }
}

// --------------------------------------------------------------------------
//
// Set Radius of FOV using the pixsize [arcsec/pix], width
// and height [pixel] of image
//
void MAstroCatalog::SetRadiusFOV(Double_t pixsize, Double_t w, Double_t h)
{
    // pixsize [arcsec/pixel]
    // w       [pixel]
    // h       [pixel]
    const Double_t scale = TMath::Hypot(w, h)/2;
    SetRadiusFOV(scale*pixsize/3600);
} 

// --------------------------------------------------------------------------
//
// Snippet to for reading catalog files.
//
TString MAstroCatalog::FindToken(TString &line, Char_t tok)
{
    Ssiz_t token = line.First(tok);
    if (token<0)
    {
        const TString copy(line);
        line = "";
        return copy;
    }

    const TString res = line(0, token);
    line.Remove(0, token+1);
    return res;
}

// --------------------------------------------------------------------------
//
// return int correspoding to TString
//
Int_t MAstroCatalog::atoi(const TString &s)
{
    return const_cast<TString&>(s).Atoi();
}

// --------------------------------------------------------------------------
//
// return float correspoding to TString
//
Float_t MAstroCatalog::atof(const TString &s)
{
    return const_cast<TString&>(s).Atof();
}

// --------------------------------------------------------------------------
//
// Read from a xephem catalog, set bit kHasChahanged.
// Already read data is not deleted. To delete the stored data call
// Delete().
//
Int_t MAstroCatalog::ReadXephem(TString catalog)
{
    gLog << inf << "Reading Xephem catalog: " << catalog << endl;

    gSystem->ExpandPathName(catalog);
    if (gSystem->AccessPathName(catalog, kReadPermission))
    {
        gLog << inf2 << "Searching Xephem catalog " << catalog << " in " << kDefaultPath << endl;
        catalog.Prepend(kDefaultPath);
    }

    izstream fin(catalog);
    if (!fin)
    {
        gLog << err << "Cannot open catalog file " << catalog << ": ";
        gLog << strerror(errno) << endl;
        return 0;
    }

    Int_t add=0;
    Int_t cnt=0;
    Int_t pos=0;

    Double_t maxmag=0;

    while (1)
    {
        TString row;
        row.ReadLine(fin);
        if (!fin)
            break;

        pos++;

        if (row[0]=='#')
            continue;

        TString line(row);

        TString name  = FindToken(line);
        TString dummy = FindToken(line);
        TString r     = FindToken(line);
        TString d     = FindToken(line);
        TString m     = FindToken(line);
        TString epoch = FindToken(line);

        if (name.IsNull() || r.IsNull() || d.IsNull() || m.IsNull() || epoch.IsNull())
        {
            gLog << warn << "Invalid Entry Line #" << pos << ": " << row << endl;
            continue;
        }

        cnt++;

        const Double_t mag = atof(m);

        maxmag = TMath::Max(maxmag, mag);

        if (mag>fLimMag)
            continue;

        if (epoch.Atoi()!=2000)
        {
            gLog << warn << "Epoch != 2000... skipped." << endl;
            continue;
        }

        Double_t ra0, dec0;
        MAstro::Coordinate2Angle(r, ra0);
        MAstro::Coordinate2Angle(d, dec0);

        ra0  *= TMath::Pi()/12;
        dec0 *= TMath::Pi()/180;

        if (AddObject(ra0, dec0, mag, name))
            add++;
    }
    gLog << inf << "Read " << add << " out of " << cnt << " (Total max mag=" << maxmag << ")" << endl;

    return add;
}

// --------------------------------------------------------------------------
//
// Read from a NGC2000 catalog. set bit kHasChanged
// Already read data is not deleted. To delete the stored data call
// Delete().
//
Int_t MAstroCatalog::ReadNGC2000(TString catalog)
{
    gLog << inf << "Reading NGC2000 catalog: " << catalog << endl;

    gSystem->ExpandPathName(catalog);
    if (gSystem->AccessPathName(catalog, kReadPermission))
    {
        gLog << inf2 << "Searching NGC2000 catalog " << catalog << " in " << kDefaultPath << endl;
        catalog.Prepend(kDefaultPath);
    }

    izstream fin(catalog);
    if (!fin)
    {
        gLog << err << "Cannot open catalog file " << catalog << ": ";
        gLog << strerror(errno) << endl;
        return 0;
    }

    Int_t add=0;
    Int_t cnt=0;
    Int_t n  =0;

    Double_t maxmag=0;

    while (1)
    {
        TString row;
        row.ReadLine(fin);
        if (!fin)
            break;

        cnt++;

        const Int_t   rah  = atoi(row(13, 2));
        const Float_t ram  = atof(row(16, 4));
        const Char_t  decs = row(22);
        const Int_t   decd = atoi(row(23, 2));
        const Int_t   decm = atoi(row(26, 2));
        const TString m    = row(43, 4);

        if (m.Strip().IsNull())
            continue;

        n++;

        const Double_t mag = atof(m);

        maxmag = TMath::Max(maxmag, mag);

        if (mag>fLimMag)
            continue;

        const Double_t ra  = MAstro::Hms2Rad(rah,  (int)ram, fmod(ram, 1)*60);
        const Double_t dec = MAstro::Dms2Rad(decd, decm, 0, decs);

        if (AddObject(ra, dec, mag, row(0,8)))
            add++;
    }

    gLog << inf << "Read " << add << " out of " << n << " (Total max mag=" << maxmag << ")" << endl;

    return add;
}

// --------------------------------------------------------------------------
//
// Read from a Bright Star Catalog catalog. set bit kHasChanged
// Already read data is not deleted. To delete the stored data call
// Delete().
//
Int_t MAstroCatalog::ReadBSC(TString catalog)
{
    gLog << inf << "Reading Bright Star Catalog (BSC5) catalog: " << catalog << endl;

    gSystem->ExpandPathName(catalog);
    if (gSystem->AccessPathName(catalog, kReadPermission))
    {
        gLog << inf2 << "Searching Bright Star catalog " << catalog << " in " << kDefaultPath << endl;
        catalog.Prepend(kDefaultPath);
    }

    izstream fin(catalog);
    if (!fin)
    {
        gLog << err << "Cannot open catalog file " << catalog << ": ";
        gLog << strerror(errno) << endl;
        return 0;
    }

    Int_t add=0;
    Int_t cnt=0;
    Int_t n  =0;

    Double_t maxmag=0;

    while (1)
    {
        TString row;
        row.ReadLine(fin);
        if (!fin)
            break;

        cnt++;

        const Int_t   rah    = atoi(row(75, 2));
        const Int_t   ram    = atoi(row(77, 2));
        const Float_t ras    = atof(row(79, 4));
        const Char_t  decsgn = row(83);
        const Int_t   decd   = atoi(row(84, 2));
        const Int_t   decm   = atoi(row(86, 2));
        const Int_t   decs   = atoi(row(88, 2));
        const TString m      = row(102, 5);

        if (m.Strip().IsNull())
            continue;

        n++;

        const Double_t mag = atof(m.Data());

        maxmag = TMath::Max(maxmag, mag);

        if (mag>fLimMag)
            continue;

        const Double_t ra  = MAstro::Hms2Rad(rah, ram, ras);
        const Double_t dec = MAstro::Dms2Rad(decd, decm, decs, decsgn);

        if (AddObject(ra, dec, mag, row(4,9)))
            add++;
    }

    gLog << inf << "Read " << add << " out of " << n << " (Total max mag=" << maxmag << ")" << endl;

    return add;
}

// --------------------------------------------------------------------------
//
// Read from a ascii heasarc ppm catalog. set bit kHasChanged
// Already read data is not deleted. To delete the stored data call
// Delete().
// If the second argument is given all survived stars are written
// to a file outname. This files will contain an apropriate compressed
// file format. You can read such files again using ReadCompressed.
//
// FIXME: A General loader for heasarc catlogs is missing, see
//        http://heasarc.gsfc.nasa.gov/W3Browse/star-catalog/
//
Int_t MAstroCatalog::ReadHeasarcPPM(TString catalog, TString outname)
{
    gLog << inf << "Reading Heasarc PPM catalog: " << catalog << endl;

    gSystem->ExpandPathName(catalog);
    if (gSystem->AccessPathName(catalog, kReadPermission))
    {
        gLog << inf2 << "Searching Heasarc PPM catalog " << catalog << " in " << kDefaultPath << endl;
        catalog.Prepend(kDefaultPath);
    }

    izstream fin(catalog);
    if (!fin)
    {
        gLog << err << "Cannot open catalog file " << catalog << ": ";
        gLog << strerror(errno) << endl;
        return 0;
    }

    ofstream *fout = outname.IsNull() ? 0 : new ofstream(outname);
    if (fout && !*fout)
    {
        gLog << warn << "Cannot open output file " << outname << ": ";
        gLog << strerror(errno) << endl;
        delete fout;
        fout = 0;
    }

    Int_t add=0;
    Int_t cnt=0;
    Int_t n  =0;

    Double_t maxmag=0;

    while (1)
    {
        TString row;
        row.ReadLine(fin);
        if (!fin)
            break;

        cnt++;

        if (!row.BeginsWith("PPM "))
            continue;

        const TString name = row(0, row.First('|'));
        row = row(row.First('|')+1, row.Length());
        row = row(row.First('|')+1, row.Length());

        const TString vmag = row(0, row.First('|'));

        n++;
        const Double_t mag = atof(vmag.Data());
        maxmag = TMath::Max(maxmag, mag);
        if (mag>fLimMag)
            continue;

        row = row(row.First('|')+1, row.Length());
        row = row(row.First('|')+1, row.Length());

        row = row(row.First('|')+1, row.Length());
        row = row(row.First('|')+1, row.Length());

        row = row(row.First('|')+1, row.Length());
        row = row(row.First('|')+1, row.Length());

        const TString ra = row(0, row.First('|'));
        row = row(row.First('|')+1, row.Length());
        const TString de = row(0, row.First('|'));
        row = row(row.First('|')+1, row.Length());

        Char_t sgn;
        Int_t d, m;
        Float_t s;
        if (sscanf(ra.Data(), "%d %d %f", &d, &m, &s)!=3)
        {
            // gLog << "Error loading entry in line " << i << endl;
            continue;
        }
        const Double_t ra0 = MAstro::Hms2Rad(d, m, s);

        if (sscanf(de.Data(), "%c%d %d %f", &sgn, &d, &m, &s)!=4)
        {
            // gLog << "Error loading entry in line " << i << endl;
            continue;
        }
        const Double_t de0 = MAstro::Dms2Rad(d, m, s, sgn);

        if (!AddObject(ra0, de0, mag, name))
            continue;

        add++;

        if (fout)
            ((MVector3*)fList.Last())->WriteBinary(*fout);
    }

    gLog << inf << "Read " << add << " out of " << n << " (Total max mag=" << maxmag << ")" << endl;

    return add;
}

// --------------------------------------------------------------------------
//
// Read from a MAstroCatalog compressed catalog. set bit kHasChanged
// Already read data is not deleted. To delete the stored data call
// Delete().
//
Int_t MAstroCatalog::ReadCompressed(TString catalog)
{
    SetBit(kHasChanged);

    gLog << inf << "Reading MAstroCatalog compressed catalog: " << catalog << endl;

    gSystem->ExpandPathName(catalog);
    if (gSystem->AccessPathName(catalog, kReadPermission))
    {
        gLog << inf2 << "Searching MAstroCatalog comressed catalog " << catalog << " in " << kDefaultPath << endl;
        catalog.Prepend(kDefaultPath);
    }

    izstream fin(catalog);
    if (!fin)
    {
        gLog << err << "Cannot open catalog file " << catalog << ": ";
        gLog << strerror(errno) << endl;
        return 0;
    }

    Int_t add=0;
    Int_t cnt=0;
    Int_t n  =0;

    Double_t maxmag=0;

    MVector3 entry;

    while (1)
    {
        cnt++;

        entry.ReadBinary(fin);
        if (!fin)
            break;

        n++;

        const Double_t mag = entry.Magnitude();
        maxmag = TMath::Max(maxmag, mag);
        if (mag>fLimMag)
            continue;

        if (entry.Angle(fRaDec)*TMath::RadToDeg()>fRadiusFOV)
            continue;

        fList.Add(entry.Clone());
        add++;
    }

    gLog << inf << "Read " << add << " out of " << n << " (Total max mag=" << maxmag << ")" << endl;

    return add;
}

// --------------------------------------------------------------------------
//
// Add an object to the star catalog manually. Return true if the object
// was added and false otherwise (criteria is the FOV)
//
Bool_t MAstroCatalog::AddObject(Float_t ra, Float_t dec, Float_t mag, TString name)
{
    MVector3 *star = new MVector3;
    star->SetRaDec(ra, dec, mag);
    star->SetName(name);

    if (star->Angle(fRaDec)*TMath::RadToDeg()>fRadiusFOV)
    {
        delete star;
        return 0;
    }

    SetBit(kHasChanged);
    fList.AddLast(star);
    return 1;
}

// --------------------------------------------------------------------------
//
// Get the visibility curve (altitude vs time) for the current time
// and observatory for the catalog entry with name name.
// If name==0 the name of the TGraph is taken instead.
// The day is divided into as many points as the graph has
// points. If the graph has no points the default is 96.
//
void MAstroCatalog::GetVisibilityCurve(TGraph &g, const char *name) const
{
    if (!fTime || !fObservatory)
    {
        g.Set(0);
        return;
    }

    const Bool_t ismoon = TString(name).CompareTo("moon", TString::kIgnoreCase)==0;

    MVector3 *star = ismoon ? 0 : static_cast<MVector3*>(FindObject(name ? name : g.GetName()));
    if (!star && !ismoon)
        return;

    const Double_t mjd = TMath::Floor(fTime->GetMjd());
    const Double_t lng = fObservatory->GetLongitudeDeg()/360;

    if (g.GetN()==0)
        g.Set(96);

    for (int i=0; i<g.GetN(); i++)
    {
        const Double_t offset = (Double_t)i/g.GetN() - 0.5;

        const MTime tm(mjd-lng+offset);

        MVector3 v = star ? *star : MVector3();
        if (!star)
        {
            Double_t ra, dec;
            MAstro::GetMoonRaDec(tm.GetMjd(), ra, dec);
            v = MVector3(ra, dec);
        }

        v *= MAstroSky2Local(tm.GetGmst(), *fObservatory);

        g.SetPoint(i, tm.GetAxisTime(), 90-v.Theta()*TMath::RadToDeg());
    }

    TH1   &h = *g.GetHistogram();
    TAxis &x = *h.GetXaxis();
    TAxis &y = *h.GetYaxis();

    y.SetTitle("Altitude [\\circ]");
    y.CenterTitle();

    x.SetTitle("UTC");
    x.CenterTitle();
    x.SetTimeFormat("%H:%M %F1995-01-01 00:00:00 GMT");
    x.SetTimeDisplay(1);
    x.SetLabelSize(0.033);

    const Double_t atm = MTime(mjd).GetAxisTime();

    x.SetRangeUser(atm-(0.5+lng)*24*60*60+15*60, atm+(0.5-lng)*24*60*60-15*60);

    g.SetMinimum(5);
    g.SetMaximum(90);
}

// --------------------------------------------------------------------------
//
// Set Range of pad. If something has changed create and draw new primitives.
// Paint all gui primitives.
//
void MAstroCatalog::Paint(Option_t *o)
{
    if (!fRaDec.IsValid())
        return;

    SetRangePad(o);

    // In the case MAstroCatalog has been loaded from a file
    // kHasChanged is not set, but fMapG.GetSize() is ==0
    if (TestBit(kHasChanged) || fMapG.GetSize()==0)
        DrawPrimitives(o);

    fMapG.Paint();
}

// --------------------------------------------------------------------------
//
// Set Range of pad if pad available. If something has changed create
// and draw new primitives. Paint all gui primitives to the Drawable with
// Id id. This can be used to be able to
//
/*
void MAstroCatalog::PaintImg(Int_t id, Option_t *o)
{
    if (gPad)
        SetRangePad(o);

    if (TestBit(kHasChanged))
    {
        if (id>0)
            gPad=0;
        DrawPrimitives(o);
    }

    fMapG.Paint(id, fRadiusFOV);
}
*/

// --------------------------------------------------------------------------
//
// Set Range of pad. If something has changed create and draw new primitives.
// Paint all gui primitives.
//
// Because in some kind of multi-threaded environments gPad doesn't stay
// the same in a single thread (because it might be changed in the same
// thread inside a gui updating timer for example) we have to secure the
// usage of gPad with a bit. This is also not multi-thread safe against
// calling this function, but the function should work well in multi-
// threaded environments. Never call this function from different threads
// simultaneously.
//
void MAstroCatalog::PaintImg(unsigned char *buf, int w, int h, Option_t *o)
{
    if (!o)
        o = "local mirrorx yellow * =";

    if (TestBit(kHasChanged))
    {
        SetBit(kDrawingImage);
        DrawPrimitives(o);
        ResetBit(kDrawingImage);
    }

    fMapG.Paint(buf, w, h, fRadiusFOV);
}

// --------------------------------------------------------------------------
//
// Draw a black marker at the position of the star. Create a corresponding
// tooltip with the coordinates.
// x, y: Pad Coordinates to draw star
// v: Sky position (Ra/Dec) of the star
// col: Color of marker (<0 mean transparent)
// txt: additional tooltip text
// resize: means resize the marker according to the magnitude
//
void MAstroCatalog::DrawStar(Double_t x, Double_t y, const TVector3 &v, Int_t col, const char *txt, Bool_t resize)
{
    const Double_t ra  = v.Phi()*TMath::RadToDeg()/15;
    const Double_t dec = (TMath::Pi()/2-v.Theta())*TMath::RadToDeg();

    const Double_t mag = -2.5*log10(v.Mag());

    TString str(v.GetName());
    if (!str.IsNull())
        str += ":  ";
    str += MString::Format("Ra=%.2fh  Dec=%.1fd  Mag=%.1f", ra, dec, mag);
    if (txt)
    {
        str += "  (";
        str += txt;
        str += ")";
    }

    // draw star on the camera display
    TMarker *tip=new TMarker(x, y, kDot);
    tip->SetMarkerColor(col);

    TAttMarker::Copy(*tip);

    fMapG.Add(tip, new TString(str));

    if (resize)
        tip->SetMarkerSize((10 - (mag>1 ? mag : 1))/15);
}

// --------------------------------------------------------------------------
//
// Set pad as modified.
//
void MAstroCatalog::Update(Bool_t upd)
{
    SetBit(kHasChanged);
    if (gPad && TestBit(kMustCleanup))
    {
        gPad->Modified();
        if (upd)
            gPad->Update();
    }
}

// --------------------------------------------------------------------------
//
// Set the observation time. Necessary to use local coordinate
// system. The MTime object is cloned.
//
void MAstroCatalog::SetTime(const MTime &time)
{
    if (fTime)
        delete fTime;
    fTime=(MTime*)time.Clone();
}

// --------------------------------------------------------------------------
//
// Set the observatory location. Necessary to use local coordinate
// system. The MObservatory object is cloned.
//
void MAstroCatalog::SetObservatory(const MObservatory &obs)
{
    if (fObservatory)
        delete fObservatory;
    fObservatory=new MObservatory;
    obs.Copy(*fObservatory);
}

// --------------------------------------------------------------------------
//
// Convert the vector to pad coordinates. After conversion
// the x- coordinate of the vector must be the x coordinate
// of the pad - the same for y. If the coordinate is inside
// the current draw area return kTRUE, otherwise kFALSE.
// If it is an invalid coordinate return kERROR
//
Int_t MAstroCatalog::ConvertToPad(const TVector3 &w0, TVector2 &v) const
{
    TVector3 w(w0);

    // Stretch such, that the Z-component is alwas the same. Now
    // X and Y contains the intersection point between the star-light
    // and the plain of a virtual plain screen (ccd...)
    if (TestBit(kPlainScreen))
        w *= 1./w(2);

    w *= TMath::RadToDeg(); // FIXME: *conversion factor?
    v.Set(TestBit(kMirrorX) ? -w(0) : w(0),
          TestBit(kMirrorY) ? -w(1) : w(1));

    v=v.Rotate(fAngle*TMath::DegToRad());

    if (w(2)<0)
        return kERROR;

    if (TestBit(kDrawingImage) || !gPad)
        return v.Mod2()<fRadiusFOV*fRadiusFOV;

    return v.X()>gPad->GetX1() && v.Y()>gPad->GetY1() &&
           v.X()<gPad->GetX2() && v.Y()<gPad->GetY2();
}

// --------------------------------------------------------------------------
//
// Convert theta/phi coordinates of v by TRotation into new coordinate
// system and convert the coordinated to pad by ConvertToPad.
// The result is retunred in v.
//
Int_t MAstroCatalog::Convert(const TRotation &rot, TVector2 &v) const
{
    MVector3 w;
    w.SetMagThetaPhi(1, v.Y(), v.X());
    w *= rot;

    return ConvertToPad(w, v);
}

// --------------------------------------------------------------------------
//
// Draw a line from v to v+(dx,dy) using Convert/ConvertToPad to get the
// corresponding pad coordinates.
//
Bool_t MAstroCatalog::DrawLine(const TVector2 &v, Int_t dx, Int_t dy, const TRotation &rot, Int_t type)
{
    const TVector2 add(dx*TMath::DegToRad(), dy*TMath::DegToRad());

    // Define all lines in the same direction
    const TVector2 va(dy==1?v:v+add);
    const TVector2 vb(dy==1?v+add:v);

    TVector2 v0(va);
    TVector2 v1(vb);

    const Int_t rc0 = Convert(rot, v0);
    const Int_t rc1 = Convert(rot, v1);

    // Both are kFALSE or both are kERROR
    if ((rc0|rc1)==kFALSE || (rc0&rc1)==kERROR)
        return kFALSE;

    TLine *line = new TLine(v0.X(), v0.Y(), v1.X(), v1.Y());
    if (type==1)
        dynamic_cast<TAttLine&>(fAttLineSky).Copy(dynamic_cast<TAttLine&>(*line));
    else
        dynamic_cast<TAttLine&>(fAttLineLocal).Copy(dynamic_cast<TAttLine&>(*line));
    fMapG.Add(line);

    if (dx!=0)
        return kTRUE;

    const TVector2 deg = va*TMath::RadToDeg();

    const TString txt = type==1 ?
        MString::Format("Ra=%.2fh  Dec=%.1fd", fmod(deg.X()/15+48, 24),  fmod(90-deg.Y()+270,180)-90) :
        MString::Format("Zd=%.1fd  Az=%.1fd",  fmod(deg.Y()+270,180)-90, fmod(deg.X()+720, 360));

    TMarker *tip=new TMarker(v0.X(), v0.Y(), kDot);
    tip->SetMarkerColor(kWhite+type*2);
    fMapG.Add(tip, new TString(txt));

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Use "local" draw option to align the display to the local
// coordinate system instead of the sky coordinate system.
// dx, dy are arrays storing recuresively all touched points
// stepx, stepy are the step-size of the current grid.
//
void MAstroCatalog::Draw(const TVector2 &v0, const TRotation &rot, TArrayI &dx, TArrayI &dy, Int_t stepx, Int_t stepy, Int_t type)
{
    // Calculate the end point
    const TVector2 v1 = v0 + TVector2(dx[0]*TMath::DegToRad(), dy[0]*TMath::DegToRad());

    // Check whether the point has already been touched.
    Int_t idx[] = {1, 1, 1, 1};

    Int_t dirs[4][2] = { {0, stepy}, {stepx, 0}, {0, -stepy}, {-stepx, 0} };

    // Check for ambiguities.
    for (int i=0; i<dx.GetSize(); i++)
    {
        for (int j=0; j<4; j++)
        {
            const Bool_t rcx0 = (dx[i]+720)%360==(dx[0]+dirs[j][0]+720)%360;
            const Bool_t rcy0 = (dy[i]+360)%180==(dy[0]+dirs[j][1]+360)%180;
            if (rcx0&&rcy0)
                idx[j] = 0;
        }
    }

    // Enhance size of array by 1, copy current
    // position as last entry
    dx.Set(dx.GetSize()+1);
    dy.Set(dy.GetSize()+1);

    dx[dx.GetSize()-1] = dx[0];
    dy[dy.GetSize()-1] = dy[0];

    // Store current positon
    const Int_t d[2] = { dx[0], dy[0] };

    for (int i=0; i<4; i++)
        if (idx[i])
        {
            // Calculate new position
            dx[0] = d[0]+dirs[i][0];
            dy[0] = d[1]+dirs[i][1];

            // Draw corresponding line and iterate through grid
            if (DrawLine(v1, dirs[i][0], dirs[i][1], rot, type))
                Draw(v0, rot, dx, dy, stepx, stepy, type);

            dx[0]=d[0];
            dy[0]=d[1];
        }
}

// --------------------------------------------------------------------------
//
// Draw a grid recursively around the point v0 (either Ra/Dec or Zd/Az)
// The points in the grid are converted by a TRotation and CovertToPad
// to pad coordinates. The type arguemnts is neccessary to create the
// correct tooltip (Ra/Dec, Zd/Az) at the grid-points.
// From the pointing position the step-size of teh gris is caluclated.
//
void MAstroCatalog::DrawGrid(const TVector3 &v0, const TRotation &rot, Int_t type)
{
    TArrayI dx(1);
    TArrayI dy(1);

    // align to 1deg boundary
    TVector2 v(v0.Phi()*TMath::RadToDeg(), v0.Theta()*TMath::RadToDeg());
    v.Set((Float_t)TMath::Nint(v.X()), (Float_t)TMath::Nint(v.Y()));

    // calculate stepsizes based on visible FOV
    Int_t stepx = 1;

    if (v.Y()<fRadiusFOV || v.Y()>180-fRadiusFOV)
        stepx=36;
    else
    {
        // This is a rough estimate how many degrees are visible
        const Float_t m = log(fRadiusFOV/180.)/log(90./(fRadiusFOV+1)+1);
        const Float_t t = log(180.)-m*log(fRadiusFOV);
        const Float_t f = m*log(90-fabs(90-v.Y()))+t;
        const Int_t nx = (Int_t)(exp(f)+0.5);
        stepx = nx<4 ? 1 : nx/4;
        if (stepx>36)
            stepx=36;
    }

    const Int_t ny = (Int_t)(fRadiusFOV+1);
    Int_t stepy = ny<4 ? 1 : ny/4;

    // align stepsizes to be devisor or 180 and 90
    while (180%stepx)
        stepx++;
    while (90%stepy)
        stepy++;

    // align to step-size boundary (search for the nearest one)
    Int_t dv = 1;
    while ((int)(v.X())%stepx)
    {
        v.Set(v.X()+dv, v.Y());
        dv = -TMath::Sign(TMath::Abs(dv)+1, dv);
    }

    dv = 1;
    while ((int)(v.Y())%stepy)
    {
        v.Set(v.X(), v.Y()+dv);
        dv = -TMath::Sign(TMath::Abs(dv)+1, dv);
    }

    // draw...
    v *= TMath::DegToRad();

    Draw(v, rot, dx, dy, stepx, stepy, type);
}

// --------------------------------------------------------------------------
//
// Get a rotation matrix which aligns the pointing position
// to the center of the x,y plain
//
TRotation MAstroCatalog::AlignCoordinates(const TVector3 &v) const
{
    TRotation trans;
    trans.RotateZ(-v.Phi());
    trans.RotateY(-v.Theta());
    trans.RotateZ(-TMath::Pi()/2);
    return trans;
}

// --------------------------------------------------------------------------
//
// Return the rotation matrix which converts either sky or
// local coordinates to coordinates which pole is the current
// pointing direction.
//
TRotation MAstroCatalog::GetGrid(Bool_t local)
{
    const Bool_t enable = fTime && fObservatory;

    // If sky coordinate view is requested get rotation matrix and
    // draw corresponding sky-grid and if possible local grid
    if (!local)
    {
        const TRotation trans(AlignCoordinates(fRaDec));

        DrawGrid(fRaDec, trans, 1);

        if (enable)
        {
            const MAstroSky2Local rot(*fTime, *fObservatory);
            DrawGrid(rot*fRaDec, trans*rot.Inverse(), 2);
        }

        // Return the correct rotation matrix
        return trans;
    }

    // If local coordinate view is requested get rotation matrix and
    // draw corresponding sky-grid and if possible local grid
    if (local && enable)
    {
        const MAstroSky2Local rot(*fTime, *fObservatory);

        const TRotation trans(AlignCoordinates(rot*fRaDec));

        DrawGrid(fRaDec,     trans*rot, 1);
        DrawGrid(rot*fRaDec, trans,     2);

        // Return the correct rotation matrix
        return trans*rot;
    }

    return TRotation();
}

// --------------------------------------------------------------------------
//
// Create the title for the pad.
//
TString MAstroCatalog::GetPadTitle() const
{
    const Double_t ra  = fRaDec.Phi()*TMath::RadToDeg();
    const Double_t dec = (TMath::Pi()/2-fRaDec.Theta())*TMath::RadToDeg();

    TString txt;
    txt += MString::Format("\\alpha=%.2fh ",      fmod(ra/15+48, 24));
    txt += MString::Format("\\delta=%.1f\\circ ", fmod(dec+270,180)-90);
    txt += MString::Format("/ FOV=%.1f\\circ",    fRadiusFOV);

    if (!fTime || !fObservatory)
        return txt;

    const MAstroSky2Local rot(*fTime, *fObservatory);
    const TVector3 loc = rot*fRaDec;

    const Double_t rho = rot.RotationAngle(fRaDec.Phi(), TMath::Pi()/2-fRaDec.Theta());

    const Double_t zd = TMath::RadToDeg()*loc.Theta();
    const Double_t az = TMath::RadToDeg()*loc.Phi();

    txt.Prepend("#splitline{");
    txt += MString::Format("  \\theta=%.1f\\circ ", fmod(zd+270,180)-90);
    txt += MString::Format("\\phi=%.1f\\circ ",     fmod(az+720, 360));
    txt += MString::Format(" / \\rho=%.1f\\circ",   rho*TMath::RadToDeg());
    txt += "}{<";
    txt += fTime->GetSqlDateTime();
    txt += ">}";
    return txt;
}

// --------------------------------------------------------------------------
//
// To overlay the catalog make sure, that in any case you are using
// the 'same' option.
//
// If you want to overlay this on top of any picture which is created
// by derotation of the camera plain you have to use the 'mirror' option
// the compensate the mirroring of the image in the camera plain.
//
// If you have already compensated this by x=-x and y=-y when creating
// the histogram you can simply overlay the catalog.
//
// To overlay the catalog on a 2D histogram the histogram must have
// units of degrees (which are plain, like you directly convert the
// camera units by multiplication to degrees)
//
// To be 100% exact you must use the option 'plain' which assumes a plain
// screen. This is not necessary for the MAGIC-camera because the
// difference between both is less than 1e-3.
//
// You should always be aware of the fact, that the shown stars and the
// displayed grid is the ideal case, like a reflection on a virtual
// perfectly aligned central mirror. In reality the star-positions are
// smeared to the edge of the camera the more the distance to the center
// is, such that the center of gravity of the light distribution might
// be more far away from the center than the display shows.
//
// If you want the stars to be displayed as circles with a size
// showing their magnitude use "*" as an option.
//
// Use 'white'  to display white instead of black stars
// Use 'yellow' to display white instead of black stars
//
//
void MAstroCatalog::AddPrimitives(TString o)
{
    const Bool_t same   = o.Contains("same",    TString::kIgnoreCase);
    const Bool_t local  = o.Contains("local",   TString::kIgnoreCase);
    const Bool_t mirx   = o.Contains("mirrorx", TString::kIgnoreCase);
    const Bool_t miry   = o.Contains("mirrory", TString::kIgnoreCase);
    const Bool_t mirror = o.Contains("mirror",  TString::kIgnoreCase) && !mirx && !miry;
    const Bool_t size   = o.Contains("*",       TString::kIgnoreCase);
    const Bool_t white  = o.Contains("white",   TString::kIgnoreCase);
    const Bool_t yellow = o.Contains("yellow",  TString::kIgnoreCase) && !white;
    const Bool_t rot180 = o.Contains("180",     TString::kIgnoreCase);
    const Bool_t rot270 = o.Contains("270",     TString::kIgnoreCase);
    const Bool_t rot90  = o.Contains("90",      TString::kIgnoreCase);

    if (white)
        SetMarkerColor(kWhite);

    fAngle = 0;
    if (rot90)
        fAngle=90;
    if (rot180)
        fAngle=180;
    if (rot270)
        fAngle=270;

    // X is vice versa, because ra is defined anti-clockwise
    mirx || mirror ? ResetBit(kMirrorX) : SetBit(kMirrorX);
    miry || mirror ? SetBit(kMirrorY) : ResetBit(kMirrorY);

    const TRotation rot(GetGrid(local));

    TIter Next(&fList);
    MVector3 *v=0;
    while ((v=(MVector3*)Next()))
    {
        if (v->Magnitude()>fLimMag)
            continue;

        TVector2 s(v->Phi(), v->Theta());
        if (Convert(rot, s)==kTRUE)
            DrawStar(s.X(), s.Y(), *v, yellow?kYellow:(white?kWhite:kBlack), 0, size);
    }

    if (!same && !TestBit(kDrawingImage) && gPad)
    {
        TPaveText *pv = new TPaveText(0.01, 0.90, 0.63, 0.99, "brNDC");
        pv->AddText(GetPadTitle());
        fMapG.Add(pv);
    }

    TMarker *mk=new TMarker(0, 0, kMultiply);
    mk->SetMarkerColor(white||yellow?kWhite:kBlack);
    mk->SetMarkerSize(1.5);
    fMapG.Add(mk);
}

// --------------------------------------------------------------------------
//
// Do nothing if 'same' option given.
// Otherwise set pad-range such that x- and y- coordinates have the same
// step-size
//
void MAstroCatalog::SetRangePad(Option_t *o)
{
    if (TString(o).Contains("same", TString::kIgnoreCase))
        return;

    const Double_t edge = fRadiusFOV/TMath::Sqrt(2.);
    //gPad->Range(-edge, -edge, edge, edge);

    const Float_t w = gPad->GetWw();
    const Float_t h = gPad->GetWh();

    if (w<h)
        gPad->Range(-edge, -edge*h/w, edge, edge*h/w);
    else
        gPad->Range(-edge*w/h, -edge, edge*w/h, edge);
}

// --------------------------------------------------------------------------
//
// Bends some pointers into the right direction...
// Calls TAttLine::SetLineAttributes and connects some signals
// to the gui to recreate the gui elements if something has changed.
//
void MAstroCatalog::SetLineAttributes(MAttLine &att)
{
    if (!gPad)
        return;

    gPad->SetSelected(&att);
    gROOT->SetSelectedPrimitive(&att);

    att.SetLineAttributes();

    TQObject::Connect("TGColorSelect", "ColorSelected(Pixel_t)", "MAstroCatalog", this, "ForceUpdate()");
    TQObject::Connect("TGListBox",     "Selected(Int_t)",        "MAstroCatalog", this, "ForceUpdate()");
}

// --------------------------------------------------------------------------
//
// Calls TAttMarker::SetMarkerAttributes and connects some signals
// to the gui to recreate the gui elements if something has changed.
//
void MAstroCatalog::SetMarkerAttributes()
{
    if (!gPad)
        return;

    TAttMarker::SetMarkerAttributes();

    // Make sure that if something is changed the gui elements
    // are recreated
    TQObject::Connect("TGedMarkerSelect", "MarkerSelected(Style_t)", "MAstroCatalog", this, "ForceUpdate()");
    TQObject::Connect("TGColorSelect",    "ColorSelected(Pixel_t)",  "MAstroCatalog", this, "ForceUpdate()");
    TQObject::Connect("TGListBox",        "Selected(Int_t)",         "MAstroCatalog", this, "ForceUpdate()");
}

void MAstroCatalog::DrawPrimitives(Option_t *o)
{
    fMapG.Delete();

    if (!TestBit(kDrawingImage) && gPad)
        SetRangePad(o);

#ifdef DEBUG
    TStopwatch clk;
    clk.Start();
#endif
    AddPrimitives(o);
#ifdef DEBUG
    clk.Stop();
    clk.Print();
#endif

    // Append to a possible second pad
    if (!TestBit(kDrawingImage) && gPad && !gPad->GetListOfPrimitives()->FindObject(this))
        AppendPad(o);

    ResetBit(kHasChanged);
}

// --------------------------------------------------------------------------
//
// Append "this" to current pad
// set bit kHasChanged to recreate all gui elements
// Connect signal
//
void MAstroCatalog::Draw(Option_t *o)
{
    // Append to first pad
    AppendPad(o);

    // If contents have not previously changed make sure that
    // all primitives are recreated.
    SetBit(kHasChanged);

    // Connect all TCanvas::ProcessedEvent to this->EventInfo
    // This means, that after TCanvas has processed an event
    // EventInfo of this class is called, see TCanvas::HandleInput
    gPad->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
                               "MAstroCatalog", this,
                               "EventInfo(Int_t,Int_t,Int_t,TObject*)");
}

// --------------------------------------------------------------------------
//
// This function was connected to all created canvases. It is used
// to redirect GetObjectInfo into our own status bar.
//
// The 'connection' is done in Draw. It seems that 'connected'
// functions must be public.
//
void MAstroCatalog::EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected)
{
    TCanvas *c = (TCanvas*)gTQSender;

    gPad = c ? c->GetSelectedPad() : NULL;
    if (!gPad)
        return;


    // Try to find a corresponding object with kCannotPick set and
    // an available TString (for a tool tip)
    TString str;
    if (!selected || selected==this)
        selected =  fMapG.PickObject(px, py, str);

    if (!selected)
        return;

    // Handle some gui events
    switch (event)
    {
    case kMouseMotion:
        if (fToolTip && !fToolTip->IsMapped() && !str.IsNull())
            ShowToolTip(px, py, str);
        break;

    case kMouseLeave:
        if (fToolTip && fToolTip->IsMapped())
            fToolTip->Hide();
        break;

    case kKeyPress:
        ExecuteEvent(kKeyPress, px, py);
        break;
    }
}

// --------------------------------------------------------------------------
//
// Handle keyboard events.
//
void MAstroCatalog::ExecuteEventKbd(Int_t /*keycode*/, Int_t keysym)
{
    Double_t dra =0;
    Double_t ddec=0;

    switch (keysym)
    {
    case kKey_Left:
        dra = -TMath::DegToRad();
        break;
    case kKey_Right:
        dra = +TMath::DegToRad();
        break;
    case kKey_Up:
        ddec = +TMath::DegToRad();
        break;
    case kKey_Down:
        ddec = -TMath::DegToRad();
        break;
    case kKey_Plus:
        SetRadiusFOV(fRadiusFOV+1);
        break;
    case kKey_Minus:
        SetRadiusFOV(fRadiusFOV-1);
        break;

    default:
        return;
    }

    const Double_t r = fRaDec.Phi();
    const Double_t d = TMath::Pi()/2-fRaDec.Theta();

    SetRaDec(r+dra, d+ddec);

    gPad->Update();
}

// ------------------------------------------------------------------------
//
// Execute a gui event on the camera
//
void MAstroCatalog::ExecuteEvent(Int_t event, Int_t mp1, Int_t mp2)
{
    if (!TestBit(kGuiActive))
        return;

    if (event==kKeyPress)
        ExecuteEventKbd(mp1, mp2);
}

// --------------------------------------------------------------------------
//
// Displays a tooltip
//
void MAstroCatalog::ShowToolTip(Int_t px, Int_t py, const char *txt)
{
    if (TestBit(kNoToolTips))
        return;

    Int_t x=0;
    Int_t y=0;

    const Window_t id1 = gVirtualX->GetWindowID(gPad->GetCanvasID());
    const Window_t id2 = fToolTip->GetParent()->GetId();

    Window_t id3;
    gVirtualX->TranslateCoordinates(id1, id2, px, py, x, y, id3);

    // Show tool tip
    fToolTip->SetText(txt);
    fToolTip->Show(x+4, y+4);
}

// --------------------------------------------------------------------------
//
// Calculate distance to primitive by checking all gui elements
//
Int_t MAstroCatalog::DistancetoPrimitive(Int_t px, Int_t py)
{
    return fMapG.DistancetoPrimitive(px, py);
}

// ------------------------------------------------------------------------
//
// Returns string containing info about the object at position (px,py).
// Returned string will be re-used (lock in MT environment).
//
char *MAstroCatalog::GetObjectInfo(Int_t px, Int_t py) const
{
    return fMapG.GetObjectInfo(px, py);
}
