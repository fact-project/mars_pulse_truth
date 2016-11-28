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
!   Author(s): Thomas Bretz   2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!              Rudy Boeck     2003 <mailto:
!              Wolfgang Wittek2003 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MHMatrix
//
// This is a histogram container which holds a matrix with one column per
// data variable. The data variable can be a complex rule (MDataPhrase).
// Each event for wich Fill is called (by MFillH) is added as a new
// row to the matrix.
//
// For example:
//   MHMatrix m;
//   m.AddColumn("MHillas.fSize");
//   m.AddColumn("MMcEvt.fImpact/100");
//   m.AddColumn("HillasSource.fDist*MGeomCam.fConvMm2Deg");
//   MFillH fillm(&m);
//   taskliost.AddToList(&fillm);
//   [...]
//   m.Print();
//
/////////////////////////////////////////////////////////////////////////////
#include "MHMatrix.h"

#include <fstream>

#include <TMath.h>

#include <TList.h>
#include <TArrayF.h>
#include <TArrayD.h>
#include <TArrayI.h>

#include <TH1.h>
#include <TCanvas.h>
#include <TRandom3.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MFillH.h"
#include "MEvtLoop.h"
#include "MParList.h"
#include "MTaskList.h"
#include "MProgressBar.h"

#include "MData.h"
#include "MDataArray.h"
#include "MFilter.h"

ClassImp(MHMatrix);

using namespace std;

const TString MHMatrix::gsDefName  = "MHMatrix";
const TString MHMatrix::gsDefTitle = "Multidimensional Matrix";

// --------------------------------------------------------------------------
//
//  Default Constructor
//
MHMatrix::MHMatrix(const char *name, const char *title)
    : fNumRows(0), fData(NULL)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
//  Default Constructor
//
MHMatrix::MHMatrix(const TMatrix &m, const char *name, const char *title)
    : fNumRows(m.GetNrows()), fM(m), fData(NULL)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
//  Constructor. Initializes the columns of the matrix with the entries
//  from a MDataArray
//
MHMatrix::MHMatrix(MDataArray *mat, const char *name, const char *title)
    : fNumRows(0), fData(mat)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
//  Constructor. Initializes the columns of the matrix with the entries
//  from a MDataArray
//
MHMatrix::MHMatrix(const TMatrix &m, MDataArray *mat, const char *name, const char *title)
    : fNumRows(m.GetNrows()), fM(m), fData(mat)
{
    fName  = name  ? name  : gsDefName.Data();
    fTitle = title ? title : gsDefTitle.Data();
}

// --------------------------------------------------------------------------
//
//  Destructor. Does not deleted a user given MDataArray, except IsOwner
//  was called.
//
MHMatrix::~MHMatrix()
{
    if (TestBit(kIsOwner) && fData)
        delete fData;
}

// --------------------------------------------------------------------------
//
Bool_t MHMatrix::SetNumRow(Int_t row) 
{
    if (row>=fM.GetNrows() || row<0) return kFALSE;
    fRow = row;
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Add a new column to the matrix. This can only be done before the first
// event (row) was filled into the matrix. For the syntax of the rule
// see MDataPhrase.
// Returns the index of the new column, -1 in case of failure.
// (0, 1, 2, ... for the 1st, 2nd, 3rd, ...)
//
Int_t MHMatrix::AddColumn(const char *rule)
{
    const Int_t idx = fData ? fData->FindRule(rule) : -1;
    if (idx>=0)
        return idx;

    if (IsValid(fM))
    {
        *fLog << warn << "Warning - matrix is already in use. Can't add a new column... skipped." << endl;
        return -1;
    }

    if (TestBit(kIsLocked))
    {
        *fLog << warn << "Warning - matrix is locked. Can't add new column... skipped." << endl;
        return -1;
    }

    if (!fData)
    {
        fData = new MDataArray;
        SetBit(kIsOwner);
    }

    fData->AddEntry(rule);
    return fData->GetNumEntries()-1;
}

// --------------------------------------------------------------------------
//
void MHMatrix::AddColumns(MDataArray *matrix)
{
    if (IsValid(fM))
    {
        *fLog << warn << "Warning - matrix is already in use. Can't add new columns... skipped." << endl;
        return;
    }

    if (TestBit(kIsLocked))
    {
        *fLog << warn << "Warning - matrix is locked. Can't add new columns... skipped." << endl;
        return;
    }

    if (fData)
        *fLog << warn << "Warning - columns already added... replacing." << endl;

    if (fData && TestBit(kIsOwner))
    {
        delete fData;
        ResetBit(kIsOwner);
    }

    fData = matrix;
}

void MHMatrix::AddColumns(const TCollection &list)
{
    TIter Next(&list);
    TObject *obj = 0;
    while ((obj=Next()))
        AddColumn(obj->GetName());
}


// --------------------------------------------------------------------------
//
// Checks whether at least one column is available and PreProcesses all
// data chains.
//
Bool_t MHMatrix::SetupFill(const MParList *plist)
{
    if (!fData)
    {
        *fLog << err << "Error - No Columns initialized... aborting." << endl;
        return kFALSE;
    }

    return fData->PreProcess(plist);
}

// --------------------------------------------------------------------------
//
// If the matrix has not enough rows double the number of available rows.
//
void MHMatrix::AddRow()
{
    fNumRows++;

    if (fM.GetNrows() > fNumRows)
        return;

    if (!IsValid(fM))
    {
        fM.ResizeTo(1, fData->GetNumEntries());
        return;
    }

#if ROOT_VERSION_CODE < ROOT_VERSION(3,05,07)
    TMatrix m(fM);
#endif

    // If we exceed ~50MB of memory we start growing slower
    const Int_t n = fM.GetNrows()>10000000/fM.GetNcols() ? TMath::Nint(fM.GetNrows()*1.2) : fM.GetNrows()*2;
    fM.ResizeTo(n, fData->GetNumEntries());

#if ROOT_VERSION_CODE < ROOT_VERSION(3,05,07)
    TVector vold(fM.GetNcols());
    for (int x=0; x<m.GetNrows(); x++)
        TMatrixRow(fM, x) = vold = TMatrixRow(m, x);
#endif
}

// --------------------------------------------------------------------------
//
// Add the values correspoding to the columns to the new row
//
Int_t MHMatrix::Fill(const MParContainer *par, const Stat_t w)
{
    AddRow();

    for (int col=0; col<fData->GetNumEntries(); col++)
        fM(fNumRows-1, col) = (*fData)(col);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Resize the matrix to a number of rows which corresponds to the number of
// rows which have really been filled with values.
//
Bool_t MHMatrix::Finalize()
{
    //
    // It's not a fatal error so we don't need to stop PostProcessing...
    //
    if (fData->GetNumEntries()==0 || fNumRows<1)
        return kTRUE;

    if (fNumRows != fM.GetNrows())
    {
        TMatrix m(fM);
        CopyCrop(fM, m, fNumRows);
    }

    return kTRUE;
}

/*
// --------------------------------------------------------------------------
//
// Draw clone of histogram. So that the object can be deleted
// and the histogram is still visible in the canvas.
// The cloned object are deleted together with the canvas if the canvas is
// destroyed. If you want to handle destroying the canvas you can get a
// pointer to it from this function
//
TObject *MHMatrix::DrawClone(Option_t *opt) const
{
    TCanvas &c = *MH::MakeDefCanvas(fHist);

    //
    // This is necessary to get the expected bahviour of DrawClone
    //
    gROOT->SetSelectedPad(NULL);

    fHist->DrawCopy(opt);

    TString str(opt);
    if (str.Contains("PROFX", TString::kIgnoreCase) && fDimension==2)
    {
        TProfile *p = ((TH2*)fHist)->ProfileX();
        p->Draw("same");
        p->SetBit(kCanDelete);
    }
    if (str.Contains("PROFY", TString::kIgnoreCase) && fDimension==2)
    {
        TProfile *p = ((TH2*)fHist)->ProfileY();
        p->Draw("same");
        p->SetBit(kCanDelete);
    }

    c.Modified();
    c.Update();

    return &c;
}

// --------------------------------------------------------------------------
//
// Creates a new canvas and draws the histogram into it.
// Be careful: The histogram belongs to this object and won't get deleted
// together with the canvas.
//
void MHMatrix::Draw(Option_t *opt)
{
    if (!gPad)
        MH::MakeDefCanvas(fHist);

    fHist->Draw(opt);

    TString str(opt);
    if (str.Contains("PROFX", TString::kIgnoreCase) && fDimension==2)
    {
        TProfile *p = ((TH2*)fHist)->ProfileX();
        p->Draw("same");
        p->SetBit(kCanDelete);
    }
    if (str.Contains("PROFY", TString::kIgnoreCase) && fDimension==2)
    {
        TProfile *p = ((TH2*)fHist)->ProfileY();
        p->Draw("same");
        p->SetBit(kCanDelete);
    }

    gPad->Modified();
    gPad->Update();
}
*/

// --------------------------------------------------------------------------
//
// Prints the meaning of the columns and the contents of the matrix.
// Becareful, this can take a long time for matrices with many rows.
// Use the option 'size' to print the size of the matrix.
// Use the option 'cols' to print the culumns
// Use the option 'data' to print the contents
//
void MHMatrix::Print(Option_t *o) const
{
    TString str(o);

    *fLog << all << flush;

    if (str.Contains("size", TString::kIgnoreCase))
    {
        *fLog << GetDescriptor() << ": NumColumns=" << fM.GetNcols();
        *fLog << " NumRows=" << fM.GetNrows() << endl;
    }

    if (!fData && str.Contains("cols", TString::kIgnoreCase))
        *fLog << "Sorry, no column information available." << endl;

    if (fData && str.Contains("cols", TString::kIgnoreCase))
    {
        fData->SetName(fName);
        fData->Print();
    }

    if (str.Contains("data", TString::kIgnoreCase))
        fM.Print();
}

// --------------------------------------------------------------------------
//
const TMatrix *MHMatrix::InvertPosDef()
{
    TMatrix m(fM);

    const Int_t rows = m.GetNrows();
    const Int_t cols = m.GetNcols();

    for (int x=0; x<cols; x++)
    {
        Double_t avg = 0;
        for (int y=0; y<rows; y++)
            avg += fM(y, x);

        avg /= rows;

#if ROOT_VERSION_CODE > ROOT_VERSION(5,00,00)
        TMatrixFColumn(m, x) += -avg;
#else
        TMatrixColumn(m, x) += -avg;
#endif
    }

    TMatrix *m2 = new TMatrix(m, TMatrix::kTransposeMult, m);

    Double_t det;
    m2->Invert(&det);
    if (det==0)
    {
        *fLog << err << "ERROR - MHMatrix::InvertPosDef failed (Matrix is singular)." << endl;
        delete m2;
        return NULL;
    }

    // m2->Print();

    return m2;
}

// --------------------------------------------------------------------------
//
// Calculated the distance of vector evt from the reference sample
// represented by the covariance metrix m.
//  - If n<0 the kernel method is applied and
//    -log(sum(epx(-d/h))/n) is returned.
//  - For n>0 the n nearest neighbors are summed and
//    sqrt(sum(d)/n) is returned.
//  - if n==0 all distances are summed
//
Double_t MHMatrix::CalcDist(const TMatrix &m, const TVector &evt, Int_t num) const
{
    if (num==0) // may later be used for another method
    {
        TVector d = evt;
        d *= m;
        return TMath::Sqrt(d*evt);
    }

    const Int_t rows = fM.GetNrows();
    const Int_t cols = fM.GetNcols();

    TArrayD dists(rows);

    //
    // Calculate:  v^T * M * v
    //
    for (int i=0; i<rows; i++)
    {
        TVector col(cols);
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
        col = TMatrixRow(fM, i);
#else
        col = TMatrixFRow_const(fM, i);
#endif
        TVector d = evt;
        d -= col;

        TVector d2 = d;
        d2 *= m;

        dists[i] = d2*d; // square of distance

        //
        // This corrects for numerical uncertanties in cases of very
        // small distances...
        //
        if (dists[i]<0)
            dists[i]=0;
    }

    TArrayI idx(rows);
    TMath::Sort(dists.GetSize(), dists.GetArray(), idx.GetArray(), kFALSE);

    Int_t from = 0;
    Int_t to   = TMath::Abs(num)<rows ? TMath::Abs(num) : rows;
    //
    // This is a zero-suppression for the case a test- and trainings
    // sample is identical. This would result in an unwanted leading
    // zero in the array. To suppress also numerical uncertanties of
    // zero we cut at 1e-5. Due to Rudy this should be enough. If
    // you encounter problems we can also use (eg) 1e-25
    //
    if (dists[idx[0]]<1e-5)
    {
        from++;
        to ++;
        if (to>rows)
            to = rows;
    }

    if (num<0)
    {
        //
        // Kernel function sum (window size h set according to literature)
        //
        const Double_t h    = TMath::Power(rows, -1./(cols+4));
        const Double_t hwin = h*h*2;

        Double_t res = 0;
        for (int i=from; i<to; i++)
            res += TMath::Exp(-dists[idx[i]]/hwin);

        return -TMath::Log(res/(to-from));
    }
    else
    {
        //
        // Nearest Neighbor sum
        //
        Double_t res = 0;
        for (int i=from; i<to; i++)
            res += dists[idx[i]];

        return TMath::Sqrt(res/(to-from));
    }
}

// --------------------------------------------------------------------------
//
// Calls calc dist. In the case of the first call the covariance matrix
// fM2 is calculated.
//  - If n<0 it is divided by (nrows-1)/h while h is the kernel factor.
//
Double_t MHMatrix::CalcDist(const TVector &evt, Int_t num)
{
    if (!IsValid(fM2))
    {
        if (!IsValid(fM))
        {
            *fLog << err << "MHMatrix::CalcDist - ERROR: fM not valid." << endl;
            return -1;
        }

        const TMatrix *m = InvertPosDef();
        if (!m)
            return -1;

        fM2.ResizeTo(*m);
        fM2 = *m;
        fM2 *= fM.GetNrows()-1;
        delete m;
    }

    return CalcDist(fM2, evt, num);
}

// --------------------------------------------------------------------------
//
void MHMatrix::Reassign()
{
    TMatrix m = fM;
    fM.ResizeTo(1,1);
    fM.ResizeTo(m);
    fM = m;
}

// --------------------------------------------------------------------------
//
// Implementation of SavePrimitive. Used to write the call to a constructor
// to a macro. In the original root implementation it is used to write
// gui elements to a macro-file.
//
void MHMatrix::StreamPrimitive(ostream &out) const
{
    Bool_t data = fData && !TestBit(kIsOwner);

    if (data)
    {
        fData->SavePrimitive(out);
        out << endl;
    }

    out << "   MHMatrix " << GetUniqueName();

    if (data || fName!=gsDefName || fTitle!=gsDefTitle)
    {
        out << "(";
        if (data)
            out << "&" << fData->GetUniqueName();
        if (fName!=gsDefName || fTitle!=gsDefTitle)
        {
            if (data)
                out << ", ";
            out << "\"" << fName << "\"";
            if (fTitle!=gsDefTitle)
                out << ", \"" << fTitle << "\"";
        }
    }
    out << ");" << endl;

    if (fData && TestBit(kIsOwner))
        for (int i=0; i<fData->GetNumEntries(); i++)
            out << "   " << GetUniqueName() << ".AddColumn(\"" << (*fData)[i].GetRule() << "\");" << endl;
}

// --------------------------------------------------------------------------
//
const TArrayI MHMatrix::GetIndexOfSortedColumn(Int_t ncol, Bool_t desc) const
{
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
    TMatrixColumn col(fM, ncol);
#else
    TMatrixFColumn_const col(fM, ncol);
#endif

    const Int_t n = fM.GetNrows();

    TArrayF array(n);

    for (int i=0; i<n; i++)
        array[i] = col(i);

    TArrayI idx(n);
    TMath::Sort(n, array.GetArray(), idx.GetArray(), desc);

    return idx;
}

// --------------------------------------------------------------------------
//
void MHMatrix::SortMatrixByColumn(Int_t ncol, Bool_t desc)
{
    TArrayI idx = GetIndexOfSortedColumn(ncol, desc);

    const Int_t n = fM.GetNrows();

#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
    TVector vold(fM.GetNcols());
#endif

    TMatrix m(n, fM.GetNcols());
    for (int i=0; i<n; i++)
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
        TMatrixRow(m, i) = vold = TMatrixRow(fM, idx[i]);
#else
        TMatrixFRow(m, i) = TMatrixFRow_const(fM, idx[i]);
#endif
    fM = m;
}

// --------------------------------------------------------------------------
//
Bool_t MHMatrix::Fill(MParList *plist, MTask *read, MFilter *filter)
{
    //
    // Read data into Matrix
    //
    const Bool_t is = plist->IsOwner();
    plist->SetOwner(kFALSE);

    MTaskList tlist;
    plist->Replace(&tlist);

    MFillH fillh(this);

    tlist.AddToList(read);

    if (filter)
    {
        tlist.AddToList(filter);
        fillh.SetFilter(filter);
    }

    tlist.AddToList(&fillh);

    //MProgressBar bar;
    MEvtLoop evtloop("MHMatrix::Fill-EvtLoop");
    evtloop.SetParList(plist);
    //evtloop.SetProgressBar(&bar);

    if (!evtloop.Eventloop())
        return kFALSE;

    tlist.PrintStatistics();

    plist->Remove(&tlist);
    plist->SetOwner(is);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Return a comma seperated list of all data members used in the matrix.
// This is mainly used in MTask::AddToBranchList
//
TString MHMatrix::GetDataMember() const
{
    return fData ? fData->GetDataMember() : TString("");
}

// --------------------------------------------------------------------------
//
//
void MHMatrix::ReduceNumberOfRows(UInt_t numrows, const TString opt)
{
    UInt_t rows = fM.GetNrows();

    if (rows==numrows)
    {
        *fLog << warn << "Matrix has already the correct number of rows..." << endl;
        return;
    }

    Float_t ratio = (Float_t)numrows/fM.GetNrows();

    if (ratio>=1)
    {
        *fLog << warn << "Matrix cannot be enlarged..." << endl;
        return;
    }

    Double_t sum = 0;

    UInt_t oldrow = 0;
    UInt_t newrow = 0;

#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
    TVector vold(fM.GetNcols());
#endif
    while (oldrow<rows)
    {
        sum += ratio;

        if (newrow<=(unsigned int)sum)
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
            TMatrixRow(fM, newrow++) = vold = TMatrixRow(fM, oldrow);
#else
            TMatrixFRow(fM, newrow++) = TMatrixFRow_const(fM, oldrow);
#endif

        oldrow++;
    }
}

// ------------------------------------------------------------------------
//
//  Used in DefRefMatrix to display the result graphically
//
void MHMatrix::DrawDefRefInfo(const TH1 &hth, const TH1 &hthd, const TH1 &thsh, Int_t refcolumn)
{
    //
    // Fill a histogram with the distribution after raduction
    //
    TH1F hta;
    hta.SetDirectory(NULL);
    hta.SetName("hta");
    hta.SetTitle("Distribution after reduction");
    CopyBinning(hth, hta);

    for (Int_t i=0; i<fM.GetNrows(); i++)
        hta.Fill(fM(i, refcolumn));

    TCanvas *th1 = MakeDefCanvas(this);
    th1->Divide(2,2);

    th1->cd(1);
    hth.DrawCopy();   // real histogram before

    th1->cd(2);
    hta.DrawCopy();   // histogram after

    th1->cd(3);
    hthd.DrawCopy();  // correction factors

    th1->cd(4);
    thsh.DrawCopy();  // target
}

// ------------------------------------------------------------------------
//
//  Resizes th etarget matrix to rows*source.GetNcol() and copies
//  the data from the first (n)rows or the source into the target matrix.
//
void MHMatrix::CopyCrop(TMatrix &target, const TMatrix &source, Int_t rows)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
    TVector v(source.GetNcols());
#endif
    target.ResizeTo(rows, source.GetNcols());
    for (Int_t ir=0; ir<rows; ir++)
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
        TMatrixRow(target, ir) = v = TMatrixRow(source, ir);
#else
        TMatrixFRow(target, ir) = TMatrixFRow_const(source, ir);
#endif
}

// ------------------------------------------------------------------------
//
// Define the reference matrix
//   refcolumn  number of the column (starting at 0) containing the variable,
//              for which a target distribution may be given;
//   thsh       histogram containing the target distribution of the variable
//   nmaxevts   the number of events the reference matrix should have after 
//              the renormalization
//   rest       a TMatrix conatining the resulting (not choosen)
//              columns of the primary matrix. Maybe NULL if you
//              are not interested in this
//
Bool_t MHMatrix::DefRefMatrix(const UInt_t refcolumn, const TH1F &thsh,
                              Int_t nmaxevts, TMatrix *rest)
{
    if (!IsValid(fM))
    {
        *fLog << err << dbginf << "Matrix not initialized" << endl;
        return kFALSE;
    }

    if (thsh.GetMinimum()<0)
    {
        *fLog << err << dbginf << "Renormalization not possible: ";
        *fLog << "Target Distribution has values < 0" << endl;
        return kFALSE;
    }


    if (nmaxevts>fM.GetNrows())
    {
        *fLog << warn << dbginf << "No.requested (" << nmaxevts;
        *fLog << ") > available events (" << fM.GetNrows() << ")... ";
        *fLog << "setting equal." << endl;
        nmaxevts = fM.GetNrows();
    }


    if (nmaxevts<0)
    {
        *fLog << err << dbginf << "Number of requested events < 0" << endl;
        return kFALSE;
    }

    if (nmaxevts==0)
        nmaxevts = fM.GetNrows();

    //
    // refcol is the column number starting at 0; it is >= 0
    //
    // number of the column (count from 0) containing
    // the variable for which the target distribution is given
    //

    //
    // Calculate normalization factors
    //
    //const int    nbins   = thsh.GetNbinsX();
    //const double frombin = thsh.GetBinLowEdge(1);
    //const double tobin   = thsh.GetBinLowEdge(nbins+1);
    //const double dbin    = thsh.GetBinWidth(1);

    const Int_t  nrows   = fM.GetNrows();
    const Int_t  ncols   = fM.GetNcols();

    //
    // set up the real histogram (distribution before)
    //
    //TH1F hth("th", "Distribution before reduction", nbins, frombin, tobin);
    TH1F hth;
    hth.SetNameTitle("th", "Distribution before reduction");
    CopyBinning(thsh, hth);
    hth.SetDirectory(NULL);
    for (Int_t j=0; j<nrows; j++)
        hth.Fill(fM(j, refcolumn));

    //TH1F hthd("thd", "Correction factors", nbins, frombin, tobin);
    TH1F hthd;
    hthd.SetNameTitle("thd", "Correction factors");
    CopyBinning(thsh, hthd);
    hthd.SetDirectory(NULL);
    hthd.Divide(&thsh, &hth, 1, 1);

    if (hthd.GetMaximum() <= 0)
    {
        *fLog << err << dbginf << "Maximum correction factor <= 0... abort." << endl;
        return kFALSE;
    }

    //
    // ===== obtain correction factors (normalization factors)
    //
    hthd.Scale(1/hthd.GetMaximum());

    //
    // get random access
    //
    TArrayI ind(nrows);
    GetRandomArrayI(ind);

    //
    // define  new matrix
    //
    Int_t evtcount1 = -1;
    Int_t evtcount2 =  0;

    TMatrix mnewtmp(nrows, ncols);
    TMatrix mrest(nrows, ncols);

    TArrayF cumulweight(nrows); // keep track for each bin how many events

    //
    // Project values in reference column into [0,1]
    //
    TVector v(fM.GetNrows());
#if ROOT_VERSION_CODE > ROOT_VERSION(5,00,00)
    v = TMatrixFColumn_const(fM, refcolumn);
#else
    v = TMatrixColumn(fM, refcolumn);
#endif
    //v += -frombin;
    //v *= 1/dbin;

    //
    // select events (distribution after renormalization)
    //
    Int_t ir;
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
    TVector vold(fM.GetNcols());
#endif
    for (ir=0; ir<nrows; ir++)
    {
        // const Int_t indref = (Int_t)v(ind[ir]);
        const Int_t indref = hthd.FindBin(v(ind[ir])) - 1;
        cumulweight[indref] += hthd.GetBinContent(indref+1);
        if (cumulweight[indref]<=0.5)
        {
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
            TMatrixRow(mrest, evtcount2++) = vold = TMatrixRow(fM, ind[ir]);
#else
            TMatrixFRow(mrest, evtcount2++) = TMatrixFRow_const(fM, ind[ir]);
#endif
            continue;
        }

        cumulweight[indref] -= 1.;
        if (++evtcount1 >=  nmaxevts)
            break;

#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
        TMatrixRow(mnewtmp, evtcount1) = vold = TMatrixRow(fM, ind[ir]);
#else
        TMatrixFRow(mnewtmp, evtcount1) = TMatrixFRow_const(fM, ind[ir]);
#endif
    }

    for (/*empty*/; ir<nrows; ir++)
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
        TMatrixRow(mrest, evtcount2++) = vold = TMatrixRow(fM, ind[ir]);
#else
        TMatrixFRow(mrest, evtcount2++) = TMatrixFRow_const(fM, ind[ir]);
#endif

    //
    // reduce size
    //
    // matrix fM having the requested distribution
    // and the requested number of rows;
    // this is the matrix to be used in the g/h separation
    //
    CopyCrop(fM, mnewtmp, evtcount1);
    fNumRows = evtcount1;

    if (evtcount1 < nmaxevts)
        *fLog << warn << "Reference sample contains less events (" << evtcount1 << ") than requested (" << nmaxevts << ")" << endl;

    if (TestBit(kEnableGraphicalOutput))
        DrawDefRefInfo(hth, hthd, thsh, refcolumn);

    if (rest)
        CopyCrop(*rest, mrest, evtcount2);

    return kTRUE;
}

// ------------------------------------------------------------------------
//
// Returns a array containing randomly sorted indices
//
void MHMatrix::GetRandomArrayI(TArrayI &ind) const
{
    const Int_t rows = ind.GetSize();

    TArrayF ranx(rows);

    TRandom3 rnd(0);
    for (Int_t i=0; i<rows; i++)
        ranx[i] = rnd.Rndm(i);

    TMath::Sort(rows, ranx.GetArray(), ind.GetArray(), kTRUE);
}

// ------------------------------------------------------------------------
//
// Define the reference matrix
//   nmaxevts   maximum number of events in the reference matrix
//   rest       a TMatrix conatining the resulting (not choosen)
//              columns of the primary matrix. Maybe NULL if you
//              are not interested in this
//
//              the target distribution will be set
//              equal to the real distribution; the events in the reference
//              matrix will then be simply a random selection of the events 
//              in the original matrix.
//
Bool_t MHMatrix::DefRefMatrix(Int_t nmaxevts, TMatrix *rest)
{
    if (!IsValid(fM))
    {
        *fLog << err << dbginf << "Matrix not initialized" << endl;
        return kFALSE;
    }

    if (nmaxevts>fM.GetNrows())
    {
      *fLog << dbginf << "No.of requested events (" << nmaxevts 
            << ") exceeds no.of available events (" << fM.GetNrows() 
            << ")" << endl;
        *fLog << dbginf 
              << "        set no.of requested events = no.of available events" 
              << endl;
        nmaxevts = fM.GetNrows();
    }

    if (nmaxevts<0)
    {
        *fLog << err << dbginf << "Number of requested events < 0" << endl;
        return kFALSE;
    }

    if (nmaxevts==0)
        nmaxevts = fM.GetNrows();

    const Int_t nrows = fM.GetNrows();
    const Int_t ncols = fM.GetNcols();

    //
    // get random access
    //
    TArrayI ind(nrows);
    GetRandomArrayI(ind);

    //
    // define  new matrix
    //
    Int_t evtcount1 = 0;
    Int_t evtcount2 = 0;

    TMatrix mnewtmp(nrows, ncols);
    TMatrix mrest(nrows, ncols);

    //
    // select events (distribution after renormalization)
    //
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
    TVector vold(fM.GetNcols());
#endif
    for (Int_t ir=0; ir<nmaxevts; ir++)
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
        TMatrixRow(mnewtmp, evtcount1++) = vold = TMatrixRow(fM, ind[ir]);
#else
        TMatrixFRow(mnewtmp, evtcount1++) = TMatrixFRow_const(fM, ind[ir]);
#endif

    for (Int_t ir=nmaxevts; ir<nrows; ir++)
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
        TMatrixRow(mrest, evtcount2++) = vold = TMatrixRow(fM, ind[ir]);
#else
        TMatrixFRow(mrest, evtcount2++) = TMatrixFRow_const(fM, ind[ir]);
#endif

    //
    // reduce size
    //
    // matrix fM having the requested distribution
    // and the requested number of rows;
    // this is the matrix to be used in the g/h separation
    //
    CopyCrop(fM, mnewtmp, evtcount1);
    fNumRows = evtcount1;

    if (evtcount1 < nmaxevts)
        *fLog << warn << "The reference sample contains less events (" << evtcount1 << ") than requested (" << nmaxevts << ")" << endl;

    if (!rest)
        return kTRUE;

    CopyCrop(*rest, mrest, evtcount2);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// overload TOject member function read 
// in order to reset the name of the object read
//
Int_t MHMatrix::Read(const char *name)
{
    Int_t ret = TObject::Read(name);
    SetName(name);

    return ret;
}

// --------------------------------------------------------------------------
//
// Read the setup from a TEnv:
//   Column0, Column1, Column2, ..., Column10, ..., Column100, ...
//
// Searching stops if the first key isn't found in the TEnv. Empty
// columns are not allowed
//
// eg.
//     MHMatrix.Column0: cos(MMcEvt.fTelescopeTheta)
//     MHMatrix.Column1: MHillasSrc.fAlpha
//
Int_t MHMatrix::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    if (IsValid(fM))
    {
        *fLog << err << "ERROR - matrix is already in use. Can't add a new column from TEnv... skipped." << endl;
        return kERROR;
    }

    if (TestBit(kIsLocked))
    {
        *fLog << err << "ERROR - matrix is locked. Can't add new column from TEnv... skipped." << endl;
        return kERROR;
    }

    //
    // Search (beginning with 0) all keys
    //
    int i=0;
    while (1)
    {
        TString idx = "Column";
        idx += i;

        // Output if print set to kTRUE
        if (!IsEnvDefined(env, prefix, idx, print))
            break;

        // Try to get the file name
        TString name = GetEnvValue(env, prefix, idx, "");
        if (name.IsNull())
        {
            *fLog << warn << prefix+"."+idx << " empty." << endl;
            continue;
        }

        if (i==0)
        {
            if (fData)
            {
                *fLog << inf << "Removing all existing columns in " << GetDescriptor() << endl;
                fData->Delete();
            }
            else
            {
                fData = new MDataArray;
                SetBit(kIsOwner);
            }
        }

        fData->AddEntry(name);
        i++;
    }

    return i!=0;
}

// --------------------------------------------------------------------------
//
// ShuffleEvents. Shuffles the order of the matrix rows.
// 
//
void MHMatrix::ShuffleRows(UInt_t seed)
{
    TRandom rnd(seed);

    TVector v(fM.GetNcols());
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
    TVector tmp(fM.GetNcols());
#endif
    for (Int_t irow = 0; irow<fNumRows; irow++)
    {
        const Int_t jrow = rnd.Integer(fNumRows);

#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
        v = TMatrixRow(fM, irow);
        TMatrixRow(fM, irow) = tmp = TMatrixRow(fM, jrow);
        TMatrixRow(fM, jrow) = v;
#else
        v = TMatrixFRow_const(fM, irow);
        TMatrixFRow(fM, irow) = TMatrixFRow_const(fM, jrow);
        TMatrixFRow(fM, jrow) = v;
#endif
    }

    *fLog << warn << GetDescriptor() << ": Attention! Matrix rows have been shuffled." << endl;
}

// --------------------------------------------------------------------------
//
// Reduces the number of rows to the given number num by cutting out the
// last rows.
//
void MHMatrix::ReduceRows(UInt_t num)
{
    if ((Int_t)num>=fM.GetNrows())
    {
        *fLog << warn << GetDescriptor() << ": Warning - " << num << " >= rows=" << fM.GetNrows() << endl;
        return;
    }

#if ROOT_VERSION_CODE < ROOT_VERSION(3,05,07)
    const TMatrix m(fM);
#endif
    fM.ResizeTo(num, fM.GetNcols());

#if ROOT_VERSION_CODE < ROOT_VERSION(3,05,07)
    TVector tmp(fM.GetNcols());
    for (UInt_t irow=0; irow<num; irow++)
        TMatrixRow(fM, irow) = tmp = TMatrixRow(m, irow);
#endif
}

// --------------------------------------------------------------------------
//
// Remove rows which contains numbers not fullfilling TMath::Finite
//
Bool_t MHMatrix::RemoveInvalidRows()
{
    TMatrix m(fM);

    const Int_t ncol=fM.GetNcols();

#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
    TVector vold(ncol);
#endif

    int irow=0;

    for (int i=0; i<m.GetNrows(); i++)
    {
#if ROOT_VERSION_CODE > ROOT_VERSION(5,00,00)
        const TMatrixFRow_const &row = TMatrixFRow_const(m, i);
#else
        const TMatrixRow &row = TMatrixRow(m, i);
#endif
        // finite (-> math.h) checks for NaN as well as inf
        int jcol;
        for (jcol=0; jcol<ncol; jcol++)
            if (!TMath::Finite(row(jcol)))
                break;

        if (jcol==ncol)
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
            TMatrixRow(fM, irow++) = vold = row;
#else
            TMatrixFRow(fM, irow++) = row;
#endif
        else
            *fLog << warn << "Warning - MHMatrix::RemoveInvalidRows: row #" << i<< " removed." << endl;
    }

    // Do not use ResizeTo (in older root versions this doesn't save the contents
    ReduceRows(irow);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Returns the row pointed to by fNumRow into TVector v
//
void MHMatrix::GetRow(TVector &v) const
{
    Int_t ncols = fM.GetNcols();

    v.ResizeTo(ncols);

    while (ncols--)
        v(ncols) = fM(fRow, ncols);
}
