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
!   Author(s): Thomas Bretz, 01/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MBinning
//
// This is a MParCOntainer storing a binning for a histogram. Doing this
// you are able to distribute a single binning to several histograms
// in your parameter list.
//
// In some classes the title of the container is used to set the
// axis-title of the corresponding axis in your histogram.
//
// For all the features supported see the function descriptions in
//  MBinning and MH
//
//////////////////////////////////////////////////////////////////////////////
#include "MBinning.h"

#include <ctype.h>      // tolower
#include <fstream>

#include <TH1.h>        // InheritsFrom
#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MH.h"

ClassImp(MBinning);

using namespace std;

const TString MBinning::gsDefName  = "MBinning";
const TString MBinning::gsDefTitle = "Container describing the binning of an axis";

// --------------------------------------------------------------------------
//
// Default Constructor. It sets name and title only. Typically you won't
// need to change this.
//
MBinning::MBinning(const char *name, const char *title)
{
    //
    //   set the name and title of this object
    //
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    SetEdges(10, 0, 1);
    fType = kIsDefault;
}

// --------------------------------------------------------------------------
//
// Copy Constructor. If necessary give also name and title.
//
MBinning::MBinning(const MBinning &bins, const char *name, const char *title)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    SetEdges(bins);
}

// --------------------------------------------------------------------------
//
// Instantiate MBinning with nbins number of bins between lo (lower edge)
// and hi (upper edge), name name and title title.
//
MBinning::MBinning(Int_t nbins, Axis_t lo, Axis_t hi, const char *name, const char *opt, const char *title)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    SetEdges(nbins, lo, hi, opt);
}

// --------------------------------------------------------------------------
//
// Initialize Binning from an axis of a TH1. If no title given,
// a title combined from the axis titles and the TH1 title is
// used.
//
MBinning::MBinning(const TH1 &h, const Char_t axis, const char *name, const char *title)
{
    fName  = name  ? name : gsDefName.Data();
    fTitle = title ? Form("%s;%s;%s;%s", h.GetTitle(),
                          h.GetXaxis()->GetTitle(),
                          h.GetYaxis()->GetTitle(),
                          h.GetZaxis()->GetTitle()) : gsDefTitle.Data();

    SetEdges(h, axis);
}

// --------------------------------------------------------------------------
//
// Initialize Binning from TAxis.
//
MBinning::MBinning(const TAxis &axis, const char *name, const char *title)
{
    fName  = name  ? name : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    SetEdges(axis);
}

// --------------------------------------------------------------------------
//
// Initialize Binning from TArrayD.
//
MBinning::MBinning(const TArrayD &axis, const char *name, const char *title)
{
    fName  = name  ? name : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();

    SetEdges(axis);
}

// --------------------------------------------------------------------------
//
// Search in the parameter list for the binning with name "name". If found,
// set the edges and title accordingly. Default is name of object.
// return kTRUE if object found, kFALSE otherwise.
//
Bool_t MBinning::SetEdges(const MParList &list, const char *name)
{
    MBinning *bins = (MBinning*)list.FindObject(name ? name : fName.Data(), "MBinning");
    if (!bins)
        return kFALSE;

    SetEdges(*bins);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Setup the edges stored in MBinning from the TAxis axe
//
void MBinning::SetEdges(const TAxis &axe)
{
    const TArrayD &arr = *axe.GetXbins();
    if (arr.GetSize()>0)
    {
        SetEdges(arr);
        return;
    }

    SetEdges(axe.GetNbins(), axe.GetXmin(), axe.GetXmax());
}

// --------------------------------------------------------------------------
//
// Add a new upper edge to the edges stored in MBinning. The new upper
// edge must be greater than the current greatest. Using this you can
// enhance a histogram bin-by-bin, eg:
//   TH1F h("", "", 2, 0, 1);
//   MBinning b;
//   b.SetEdges(h);
//   b.AddEdge(2);
//   b.Apply(h);
//   b.AddEdge(3);
//   b.Apply(h);
//   [...]
//
void MBinning::AddEdge(Axis_t up)
{
    const UInt_t n = fEdges.GetSize();

    if (up<=fEdges[n-1])
    {
        *fLog << warn << dbginf << "WARNING - New upper edge not greater than old upper edge... ignored." << endl;
        return;
    }

    fEdges.Set(n+1);
    fEdges[n] = up;

    fType = kIsUserArray;
}

// --------------------------------------------------------------------------
//
// Removes the first edge
//
void MBinning::RemoveFirstEdge()
{
    const Int_t n = fEdges.GetSize();
    for (int i=0; i<n-1; i++)
        fEdges[i] = fEdges[i+1];
    fEdges.Set(n-1);
}

// --------------------------------------------------------------------------
//
// Removes the last edge
//
void MBinning::RemoveLastEdge()
{
    fEdges.Set(fEdges.GetSize()-1);
}

// --------------------------------------------------------------------------
//
// Set the edges in MBinning from a histogram-axis. Which axis is
// specified by axis ('x', 'y', 'z')
//
void MBinning::SetEdges(const TH1 &h, const Char_t axis)
{
    switch (tolower(axis))
    {
    case 'x':
        SetEdges(*h.GetXaxis());
        return;
    case 'y':
        SetEdges(*h.GetYaxis());
        return;
    case 'z':
        SetEdges(*h.GetZaxis());
        return;
    default:
        *fLog << warn << "MBinning::SetEdges: Axis '" << axis << "' unknown... using x." << endl;
        SetEdges(*h.GetXaxis());
    }
}

// --------------------------------------------------------------------------
//
// Specify the number of bins <nbins> (not the number of edges), the
// lowest <lo> and highest <up> Edge (of your histogram)
//
void MBinning::SetEdgesLin(Int_t nbins, Axis_t lo, Axis_t up)
{
    if (nbins<=0)
    {
        *fLog << warn << "WARNING - Number of bins cannot be <= 0... reset to 1." << endl;
        nbins = 1;
    }

    if (up<lo)
    {
        *fLog << warn << "WARNING - Upper edge must be greater than lower edge... exchanging." << endl;

        const Axis_t dummy(lo);
        lo = up;
        up = dummy;
    }

    const Double_t binsize = nbins<=0 ? 0 : (up-lo)/nbins;
    fEdges.Set(nbins+1);
    for (int i=0; i<=nbins; i++)
        fEdges[i] = binsize*i + lo;

    fType = kIsLinear;
}

// --------------------------------------------------------------------------
//
// Set edged from text. With the following structure:
//
//     n lo hi [type [title]]
//
//  n:  number of bins
//  lo: lowest edge
//  hi: highest edge
//  type: "lin" <default>, "log", "cos", "asin" (without quotationmarks)
//  title: Whatever the title might be
//
// For example:
//   SetEdgesRaw("12 0 1 lin This is the title");
//
Bool_t MBinning::SetEdgesRaw(const char *txt)
{
    Int_t   nbins  = 0;
    Float_t loedge = 0;
    Float_t upedge = 0;
    Int_t   len    = 0;
    if (3!=sscanf(txt, " %d %f %f %n", &nbins, &loedge, &upedge, &len))
    {
        *fLog << warn << GetDescriptor() << "::SetEdges: Not enough arguments... ignored." << endl;
        return kFALSE;
    }

    if (loedge>=upedge)
    {
        *fLog << warn << GetDescriptor() << "::SetEdges: Lowest edge >= highest edge... ignored." << endl;
        return kFALSE;
    }

    TString str(txt);
    str.Remove(0, len);
    str = str.Strip(TString::kBoth);

    TString typ(str);
    Ssiz_t pos = str.First(' ');
    if (pos>=0)
    {
        typ = str(0, pos);
        str.Remove(0, pos);
        str = str.Strip(TString::kBoth);
        if (typ!=(TString)"lin" && typ!=(TString)"log" && typ!=(TString)"cos" && typ!=(TString)"asin")
        {
            *fLog << warn << GetDescriptor() << "::SetEdges: Type " << typ << " unknown... ignored." << endl;
            return kFALSE;
        }
    }

    SetEdges(nbins, loedge, upedge, typ.Data());

    if (!str.IsNull())
        fTitle = str;

    return kTRUE;
}
/*
// --------------------------------------------------------------------------
//
// Set edged from text. With the following structure:
//
//     n= lo= hi= type= title="my title"
//
//  n:  number of bins
//  lo: lowest edge
//  hi: highest edge
//  type: "lin" <default>, "log", "cos" (without quotationmarks)
//  title: Whatever the title might be
//
// For example:
//   SetEdgesRaw("12 0 1 lin This is the title");
//
Bool_t MBinning::SetEdgesRaw(const char *txt)
{
    Int_t   nbins  = 0;
    Float_t loedge = 0;
    Float_t upedge = 0;
    Int_t   len    = 0;
    if (3!=sscanf(txt, " %d %f %f %n", &nbins, &loedge, &upedge, &len))
    {
        *fLog << warn << GetDescriptor() << "::SetEdges: Not enough arguments... ignored." << endl;
        return kFALSE;
    }

    if (loedge>=upedge)
    {
        *fLog << warn << GetDescriptor() << "::SetEdges: Lowest edge >= highest edge... ignored." << endl;
        return kFALSE;
    }

    TString str(txt);
    str.Remove(0, len);
    str = str.Strip(TString::kBoth);

    TString typ;
    Ssiz_t pos = str.First(' ');
    if (pos>=0)
    {
        typ = str(0, pos);
        if (typ!=(TString)"lin" && typ!=(TString)"log" && typ!=(TString)"cos")
        {
            *fLog << warn << GetDescriptor() << "::SetEdges: Type " << typ << " unknown... ignored." << endl;
            return kFALSE;
        }
    }

    SetEdges(nbins, loedge, upedge, typ.Data());

    str = str.Strip(TString::kBoth);

    if (!str.IsNull())
        fTitle = str;

    return kTRUE;
}
*/
// --------------------------------------------------------------------------
//
// Calls SetEdgesLog if opt contains "log"
// Calls SetEdgesCos if opt contains "cos"
// Calls SetEdges in all other cases
//
void MBinning::SetEdges(const Int_t nbins, const Axis_t lo, Axis_t up, const char *opt)
{
    const TString o(opt);
    if (o.Contains("log", TString::kIgnoreCase))
    {
        SetEdgesLog(nbins, lo, up);
        return;
    }
    if (o.Contains("asin", TString::kIgnoreCase))
    {
        SetEdgesASin(nbins, lo, up);
        return;
    }
    if (o.Contains("cos", TString::kIgnoreCase))
    {
        SetEdgesCos(nbins, lo, up);
        return;
    }
    SetEdges(nbins, lo, up);
}

// --------------------------------------------------------------------------
//
// Specify the number of bins <nbins> (not the number of edges), the
// lowest <lo> and highest <up> Edge (of your histogram)
//
void MBinning::SetEdgesLog(Int_t nbins, Axis_t lo, Axis_t up)
{
    // if (lo==0) ...
    if (nbins<=0)
    {
        *fLog << warn << "WARNING - Number of bins cannot be <= 0... reset to 1." << endl;
        nbins = 1;
    }

    if (up<lo)
    {
        *fLog << warn << "WARNING - Upper edge must be greater than lower edge... exchanging." << endl;

        const Axis_t dummy(lo);
        lo = up;
        up = dummy;
    }

    const Double_t binsize = log10(up/lo)/nbins;
    fEdges.Set(nbins+1);
    for (int i=0; i<=nbins; i++)
        fEdges[i] = pow(10, binsize*i) * lo;

    fType = kIsLogarithmic;
}

// --------------------------------------------------------------------------
//
// Specify the number of bins <nbins> (not the number of edges), the
// lowest [deg] <lo> and highest [deg] <up> Edge (of your histogram)
//
void MBinning::SetEdgesCos(Int_t nbins, Axis_t lo, Axis_t up)
{
    if (nbins<=0)
    {
        *fLog << warn << "WARNING - Number of bins cannot be <= 0... reset to 1." << endl;
        nbins = 1;
    }

    if (up<lo)
    {
        *fLog << warn << "WARNING - Upper edge must be greater than lower edge... exchanging." << endl;

        const Axis_t dummy(lo);
        lo = up;
        up = dummy;
    }

    // if (lo==0) ...
    const Axis_t ld = lo/kRad2Deg;
    const Axis_t ud = up/kRad2Deg;

    const Double_t cld = ld<0 ? cos(ld)-1 : 1-cos(ld);
    const Double_t cud = ud<0 ? cos(ud)-1 : 1-cos(ud);

    SetEdgesASin(nbins, cld, cud);
}

// --------------------------------------------------------------------------
//
// Specify the number of bins <nbins> (not the number of edges), the
// lowest [deg] <lo> and highest [deg] <up> Edge (of your histogram)
//
void MBinning::SetEdgesASin(Int_t nbins, Axis_t lo, Axis_t up)
{
    if (nbins<=0)
    {
        *fLog << warn << "WARNING - Number of bins cannot be <= 0... reset to 1." << endl;
        nbins = 1;
    }

    if (up<lo)
    {
        *fLog << warn << "WARNING - Upper edge must be greater than lower edge... exchanging." << endl;

        const Axis_t dummy(lo);
        lo = up;
        up = dummy;
    }

    const Double_t binsize = nbins<=0 ? 0 : (up-lo)/nbins;
    fEdges.Set(nbins+1);
    for (int i=0; i<=nbins; i++)
    {
        const Double_t a = binsize*i + lo;
        fEdges[i] = a<0 ? -acos(1+a)*kRad2Deg : acos(1-a)*kRad2Deg;
    }

    fType = kIsCosinic;
}

// --------------------------------------------------------------------------
//
// Apply this binning to the given histogram.
// (By definition this works only for 1D-histograms. For 2D- and 3D-
//  histograms use MH::SetBinning directly)
//
void MBinning::Apply(TH1 &h) const
{
    if (h.InheritsFrom("TH2") || h.InheritsFrom("TH3"))
    {
        *fLog << warn << "MBinning::Apply: '" << h.GetName() << "' is not a basic TH1 object... no binning applied." << endl;
        return;
    }

    MH::SetBinning(h, *this);
}

// --------------------------------------------------------------------------
//
// Print binning.
//
void MBinning::Print(Option_t *o) const
{
    *fLog << all;
    *fLog << GetDescriptor() << ": nbins=" << GetNumBins() << " [";
    *fLog << GetEdgeLo() << ", " << GetEdgeHi() << "] <";
    switch (fType)
    {
    case kIsDefault:     *fLog << "default"; break;
    case kIsLinear:      *fLog << "linear"; break;
    case kIsLogarithmic: *fLog << "logarithmic"; break;
    case kIsCosinic:     *fLog << "consinic"; break;
    case kIsUserArray:   *fLog << "user-array"; break;
    }
    *fLog << ">";

    if (fTitle!=gsDefTitle)
        *fLog << " title=" << fTitle;

    *fLog << endl;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MBinning::StreamPrimitive(ostream &out) const
{
    out << "   MBinning " << GetUniqueName();
    if (fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << "(\"" << fName << "\"";
        if (fTitle!=gsDefTitle)
            out << ", \"" << fTitle << "\"";
        out <<")";
    }
    out << ";" << endl;

    if (IsDefault())
        return;

    if (IsLinear() || IsLogarithmic() || IsCosinic())
    {
        out << "   " << GetUniqueName() << ".SetEdges";
        if (IsLogarithmic())
            out << "Log";
        if (IsCosinic())
            out << "Cos";
        out << "(" << GetNumBins() << ", " << GetEdgeLo() << ", " << GetEdgeHi() << ");" << endl;
        return;
    }

    out << "   {" << endl;
    out << "      TArrayD dummy;" << endl;
    for (int i=0; i<GetNumEdges(); i++)
        out << "      dummy[" << i << "]=" << GetEdges()[i] << ";" << endl;
    out << "      " << GetUniqueName() << ".SetEdges(dummy);" << endl;
    out << "   }" << endl;
}

// --------------------------------------------------------------------------
//
// Allows reading a binning from resource files. The structure is as follows
//
Int_t MBinning::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;

    UInt_t  nbins  = GetNumBins();
    Float_t edgelo = GetEdgeLo();
    Float_t edgehi = GetEdgeHi();
    TString type;
    if (IsEnvDefined(env, prefix, "NumBins", print))
    {
        nbins = GetEnvValue(env, prefix, "NumBins", GetNumBins());
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "EdgeLo", print))
    {
        edgelo = GetEnvValue(env, prefix, "EdgeLo", GetEdgeLo());
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "EdgeHi", print))
    {
        edgehi = GetEnvValue(env, prefix, "EdgeHi", GetEdgeHi());
        rc = kTRUE;
    }
    if (rc==kTRUE && (type==kIsUserArray || type==kIsDefault))
        type = kIsLinear;

    if (IsEnvDefined(env, prefix, "Type", print))
    {
        type = GetEnvValue(env, prefix, "Type", "lin");
        if (type!=(TString)"lin" && type!=(TString)"log" && type!=(TString)"cos" && type!=(TString)"acos")
        {
            *fLog << warn << GetDescriptor() << "::ReadEnv - WARNING: Type is not lin, log nor cos... assuming lin." << endl;
            type = "lin";
        }
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "Edges", print))
    {
        if (rc==kTRUE)
            *fLog << warn << GetDescriptor() << "::ReadEnv - WARNING: 'Edges' found... ignoring any 'NumBins', 'EdgeLo' and 'EdgeHi'" << endl;

        const TString etype = GetEnvValue(env, prefix, "Edges", "");
        //type = kIsUserArray;
        /* MISSING */
        rc = kTRUE;
        *fLog << err << " SORRY USER ARRAY NOT YET IMPLEMENTED" << endl;
        return kERROR;
    }

    const Bool_t raw = IsEnvDefined(env, prefix, "Raw", print);
    //const Bool_t fullbins    = IsEnvDefined(env, prefix, "Binning", print);
    if (!raw && /*!fullbins &&*/ rc==kTRUE)
        SetEdges(nbins, edgelo, edgehi, type.Data());

    if (rc==kTRUE)
        *fLog << warn << GetDescriptor() << "::ReadEnv - WARNING: 'Binning' found... ignoring any 'NumBins', 'EdgeLo', 'EdgeHi' and 'Edges'" << endl;

    if (IsEnvDefined(env, prefix, "Title", print))
    {
        fTitle = GetEnvValue(env, prefix, "Title", gsDefTitle.Data());
        rc = kTRUE;
    }

    if (raw)
    {
        const TString txt = GetEnvValue(env, prefix, "Raw", "");
        if (!SetEdgesRaw(txt.Data()))
            return kERROR;
    }
/*
    if (fullbins)
    {
        TString txt = GetEnvValue(env, prefix, "Binning", "");
        SetEdgesRaw(txt.Data());
    }
   */

    return rc;
}
