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
!   Author(s): Thomas Bretz, 12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MRawEvtData
//
//  Storage container to store the raw FADC values.
//
//  MArrayS fHiGainPixId
//  ---------------------
//  Array of Pixel Numbers for their high voltage channel in the order the
//  FADC values are stored in fHiGainFadcSamples
//
//  MArrayB fHiGainFadcSaples
//  -------------------------
//  FADC samples (hi gain) of all pixels
//
//  MArrayS fLoGainPixId
//  --------------------
//  see fHiGainPixId
//
//  MArrayB fLoGainFadcSamples
//  --------------------------
//  see fHiGainFadcSaples
//
//
//   Version 8
//   ------------------
//    + MArrayS *fStartCell
//    + Bool_t   fIsSigned
//
//   Version 7
//   ------------------
//    + UShort_t fNumBytesPerSample;
//
//   Version 6
//   ------------------
//    - The data can now be stoe in a single array keeping he full
//      compatibility
//
//   Version 5 (0.8.5):
//   ------------------
//    - Changed type of ABFlags from TArrayC to MArrayB
//
//   Version 4 (0.8.4):
//   ------------------
//    - Changed derivement from MCamEvent to MParContainer and MCamEvent
//
//   Version 3 (0.8.4):
//   ------------------
//    - Added fABFlags
//
//   Version 2:
//   ----------
//    - Derives from MCamEvent now
//
//   Version 1:
//   ----------
//    - First implementation
//
/////////////////////////////////////////////////////////////////////////////
#include "MRawEvtData.h"

#include <fstream>

#include <TH1.h>
#include <TGraph.h>
#include <TArrayC.h>
#include <TVirtualPad.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MArrayS.h"
#include "MArrayB.h"
#include "MArrayI.h"
#include "MGeomCam.h"

#include "MRawCrateArray.h"
#include "MRawCrateData.h"

#include "MRawRunHeader.h"
#include "MRawEvtPixelIter.h"

ClassImp(MRawEvtData);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. It initializes all arrays with zero size.
//
MRawEvtData::MRawEvtData(const char *name, const char *title)
    : fRunHeader(0), fTriggerType(0), fNumBytesPerSample(1), fIsSigned(false)
{
    fName  = name  ? name  : "MRawEvtData";
    fTitle = title ? title : "Raw Event Data Information";

    InitArrays();
}

// --------------------------------------------------------------------------
//
// Destructor. Deletes all the arrays.
//
MRawEvtData::~MRawEvtData()
{
    DeleteArrays();
}

// --------------------------------------------------------------------------
//
// reset all arrays
//
void MRawEvtData::Clear(Option_t *)
{
    /*
     FIXME:
     Is Reset (set all entries to zero) what you want to do
     or Set(0) (delete the array)
     */
    fHiGainPixId->Reset();
    fLoGainPixId->Reset();
    fHiGainFadcSamples->Reset();
    fLoGainFadcSamples->Reset();
    fABFlags->Reset();
}

// --------------------------------------------------------------------------
//
// return the number of hi gain samples per pixel
//
UShort_t MRawEvtData::GetNumHiGainSamples() const
{
    // If value<0 we are reading old MC files which don't have the
    // value set so far. So we use the old methid to determin the
    // numbers and calculate them from the length of the arrays.
    return fHiGainPixId->GetSize() ? fHiGainFadcSamples->GetSize()/(fHiGainPixId->GetSize()*fNumBytesPerSample) : 0;
}

// --------------------------------------------------------------------------
//
// return the number of lo gain samples per pixel
//
UShort_t MRawEvtData::GetNumLoGainSamples() const
{
    // If value<0 we are reading old MC files which don't have the
    // value set so far. So we use the old methid to determin the
    // numbers and calculate them from the length of the arrays.
    return fLoGainPixId->GetSize() ? fLoGainFadcSamples->GetSize()/(fLoGainPixId->GetSize()*fNumBytesPerSample) : 0;
}

// --------------------------------------------------------------------------
//
// return the number of stored pixel
//
UShort_t MRawEvtData::GetNumPixels() const
{
    return fHiGainPixId ? fHiGainPixId->GetSize() : 0;
}

// --------------------------------------------------------------------------
//
//  Print out the onformation to *fLog.
//  Options:
//     "hex"      Prints the time slices hexadecimal (default)
//     "dec"      Prints the time slices decimal
//
void MRawEvtData::Print(Option_t *opt) const
{
    //
    // print fadc inforation to screen
    // Possible Options:
    // - DEC: Print values decimal instead of hexadecimal (default)
    //
    const UShort_t nHiSamp = GetNumHiGainSamples();
    const UShort_t nLoSamp = GetNumLoGainSamples();

    const UShort_t bps     = GetNumBytesPerSample();

    fLog->unsetf(ios::showbase);

    *fLog << dec << all;
    *fLog << GetDescriptor() << ": " << endl;
    *fLog << GetNumPixels() << " Pixels with " << (Int_t)nHiSamp << "/" << (Int_t)nLoSamp << " samples" << endl;

    TString str(opt);
    Int_t manip = str.Contains("dec", TString::kIgnoreCase);

    MRawEvtPixelIter pixel(const_cast<MRawEvtData*>(this));
    Int_t i = 0;
    while (pixel.Next())
    {
        const UShort_t idx = pixel.GetPixelId();

        *fLog << endl << dec << setfill(' ');
        *fLog << " " << setw(3) << i++ << " - " << setw(3) << idx << " ";

        if (pixel.IsABFlagValid())
            *fLog << "<" << (pixel.HasABFlag()?"B":"A") << "> ";

        *fLog << (manip?dec:hex) << setfill(manip?' ':'0');

        const Byte_t *hi = (Byte_t*)pixel.GetHiGainSamples();
        const Byte_t *lo = (Byte_t*)pixel.GetLoGainSamples();

        for (int j=0; j<nHiSamp*bps; j++)
        {
            *fLog << setw(manip?3:2);
            *fLog << (hi[j]&0xff);
            if (manip)
                *fLog << ' ';
        }

        for (int j=0; j<nLoSamp*bps; j++)
        {
            *fLog << setw(manip?3:2);
            *fLog << ((UShort_t)lo[j]&0xff);
            if (manip)
                *fLog << ' ';
        }
    }
    *fLog << dec << endl;
}

// --------------------------------------------------------------------------
//
// Draw a pixel. A Histogram or Graph is created and it's draw function is
// called.
//  Options:
//     "GRAPH"      A graph is drawn
//     "HIST"       A histogram is drawn
//     <index>      The pixel with the given index is drawn
//
void MRawEvtData::Draw(Option_t *opt)
{
    if (GetNumPixels()==0)
    {
        *fLog << warn << "Sorry, no pixel to draw!" << endl;
        return;
    }

    TString str(opt);
    str.ToLower();

    UInt_t id = 0;

    if (str.BeginsWith("graph"))
        sscanf(str.Data()+5, "%d", &id);
    if (str.BeginsWith("hist"))
        sscanf(str.Data()+4, "%d", &id);

    MRawEvtPixelIter pix(this);
    if (!pix.Jump(id))
    {
        *fLog << warn << dec << "Pixel Idx #" << id << " doesn't exist!" << endl;
        return;
    }

    const void *higains = pix.GetHiGainSamples();
    const void *logains = pix.GetLoGainSamples();

    const Int_t nh = GetNumHiGainSamples();
    const Int_t nl = GetNumLoGainSamples();

    TString name = "Pixel Idx.";
    name += pix.GetPixelId();

    fIsSigned = kTRUE;

    Bool_t same = str.Contains("same");

    cout << "MIN/MAX=" << fNumBytesPerSample << " " << GetMin() << " " << GetMax() << endl;

    if (str.BeginsWith("graph"))
    {
        *fLog << inf << "Drawing Graph: Pixel Idx #" << dec << pix.GetPixelId();
        *fLog << " of " << (int)GetNumPixels() << "Pixels" << endl;

        TGraph *graphhi = new TGraph;

        for (int i=0; i<nh; i++)
            graphhi->SetPoint(graphhi->GetN(), i, GetSample(higains, i));
        for (int i=0; i<nl; i++)
            graphhi->SetPoint(graphhi->GetN(), i+nh, GetSample(logains, i));

        graphhi->SetMaximum(GetMax()+0.5);
        graphhi->SetMinimum(GetMin());

        graphhi->SetBit(kCanDelete);
        graphhi->Draw(same ? "C*" : "AC*");

        TH1F *histhi = graphhi->GetHistogram();
        histhi->SetMinimum(GetMin());
        histhi->SetMaximum(GetMax()+0.5);
 
        histhi->SetXTitle("Time/FADC Slices");
        histhi->SetYTitle("Signal/FADC Units");

        return;
    }

    if (str.BeginsWith("hist"))
    {
        // FIXME: Add Legend
        *fLog << inf << "Drawing Histogram of Pixel with Idx #" << dec << pix.GetPixelId() << " to " << gPad << endl;

        TH1F *histh = new TH1F(name, "FADC Samples", nh+nl, -0.5, nh+nl-.5);
        histh->SetMinimum(GetMin());
        histh->SetMaximum(GetMax()+0.5);
        histh->SetXTitle("Time [FADC Slices]");
        histh->SetYTitle("Signal [FADC Units]");
        histh->SetDirectory(NULL);
        for (int i=0; i<nh; i++)
            histh->Fill(i, GetSample(higains, i));
        for (int i=0; i<nl; i++)
            histh->Fill(i+nl, GetSample(logains, i));
        histh->SetBit(kCanDelete);
        histh->Draw(same ? "same" : "");

        return;
    }

    *fLog << warn << dbginf << "WARNING - You must specify either 'GRAPH' or 'HIST'" << endl;
}

// --------------------------------------------------------------------------
//
//  Deletes all the arrays
//  The flag is for future usage.
//
void MRawEvtData::InitArrays(UShort_t numconnected, UShort_t maxid)
{
    // fRunHeader should not be set only in the constructor!
    const Int_t numhi  = fRunHeader ? fRunHeader->GetNumSamplesHiGain()  : 0;
    const Int_t numlo  = fRunHeader ? fRunHeader->GetNumSamplesLoGain()  : 0;

    fNumBytesPerSample = fRunHeader ? fRunHeader->GetNumBytesPerSample() : 1;


    if (fRunHeader == 0)
    {
        fTriggerType = 0x0000;
    }
    else
    {
        switch(fRunHeader->GetRunType())
        {
        case MRawRunHeader::kRTMonteCarlo|MRawRunHeader::kRTData:
            fTriggerType = 0x0004; break;
        case MRawRunHeader::kRTMonteCarlo|MRawRunHeader::kRTPedestal:
            fTriggerType = 0x0400; break;
        case MRawRunHeader::kRTMonteCarlo|MRawRunHeader::kRTCalibration:
            fTriggerType = 0x0164; break;
        default:
            fTriggerType = 0x0000; break;
        }
    }

    fHiGainPixId       = new MArrayS(numconnected);
    fLoGainPixId       = new MArrayS(0);
    fHiGainFadcSamples = new MArrayB(numconnected*(numhi+numlo)*fNumBytesPerSample);
    fLoGainFadcSamples = new MArrayB(0);

    fABFlags           = new MArrayB(maxid==0 ? 0 : maxid/8+1);
    fStartCells        = new MArrayS(0);

    fConnectedPixels   = 0;
}

void MRawEvtData::InitStartCells()
{
    delete fStartCells;
    fStartCells = new MArrayS(fHiGainPixId->GetSize());
}

// --------------------------------------------------------------------------
//
//  Deletes all the arrays
//
void MRawEvtData::DeleteArrays()
{
    delete fHiGainPixId;
    delete fLoGainPixId;
    delete fHiGainFadcSamples;
    delete fLoGainFadcSamples;
    delete fABFlags;
    delete fStartCells;
}

// --------------------------------------------------------------------------
//
// Deletes all arrays describing the pixel Id and Samples in pixels.
// The flag is for future usage.
//
void MRawEvtData::ResetPixels(UShort_t numconnected, UShort_t maxid)
{
    //const int npix = fRunHeader->GetNumCrates()*fRunHeader->GetNumPixInCrate();
    if (fHiGainPixId && fHiGainPixId->GetSize()==numconnected && (UShort_t)fABFlags->GetSize()==(maxid/8+1))
    {
        fConnectedPixels = 0;
        return;
    }

    DeleteArrays();
    InitArrays(numconnected, maxid);
}

void MRawEvtData::ResetPixels()
{
    if (!fRunHeader)
        return;

    // FIXME: Better give NumNormalPixels if numconnected==0 ?

    ResetPixels(fRunHeader->GetNumNormalPixels(), fRunHeader->GetNumNormalPixels()-1);
}

// --------------------------------------------------------------------------
//
//  This is to fill the data of one pixel to the MRawEvtHeader Class.
//  The parameters are the pixelnumber and the FADC_SLICES values of ADCs
//
void MRawEvtData::AddPixel(UShort_t nOfPixel, const TArrayC &data)
{
    const Int_t n = fRunHeader->GetNumSamples();
    if (data.GetSize()!=n)
    {
        *fLog << err << "RawEvtData::AddPixel: Error, number of samples in ";
        *fLog << "TArrayC " << data.GetSize() << " doesn't match current number " << n << endl;
        return;
    }

    //
    // enhance pixel array by one
    //
    fHiGainPixId->Set(fHiGainPixId->GetSize()+1);

    //
    // add the number of the new pixel to the array as last entry
    //
    fHiGainPixId->AddAt(nOfPixel, fHiGainPixId->GetSize()-1);

    //
    // enhance the array by the number of new samples
    //
    fHiGainFadcSamples->Set(fHiGainFadcSamples->GetSize()+n);

    //
    // add the new slices as last entries to array
    //
    fHiGainFadcSamples->AddAt((Byte_t*)data.GetArray(), fHiGainFadcSamples->GetSize()-n, n);
}

// --------------------------------------------------------------------------
//
// Add the contents of the MArrayI to the fHiGainFadcSamples
// One Integer is added to one sample in the array.
//
void MRawEvtData::Set(const MArrayI &data)
{
    fConnectedPixels = fHiGainPixId->GetSize();

    UInt_t n = fConnectedPixels*fRunHeader->GetNumSamplesHiGain();
    if (n!=data.GetSize())
    {
        *fLog << err << "MRawEvtData::Set: Size mismatch." << endl;
        *fLog << " fConnectedPixels="   << fConnectedPixels << endl;
        *fLog << " NumHiGainSamples="   << fRunHeader->GetNumSamplesHiGain() << endl;
        *fLog << " data.GetSize()="     << data.GetSize() << endl;
        return;
    }

    Byte_t *dest = fHiGainFadcSamples->GetArray();

    UInt_t *src  = reinterpret_cast<UInt_t*>(data.GetArray());
    UInt_t *end  = reinterpret_cast<UInt_t*>(data.GetArray()) + n;

    switch (fNumBytesPerSample)
    {
    case 1:
        {
            Byte_t *ptr = reinterpret_cast<Byte_t*>(dest);
            while (src<end)
                *ptr++ = Byte_t(*src++);
        }
        return;

    case 2:
        {
            UShort_t *ptr = reinterpret_cast<UShort_t*>(dest);
            while (src<end)
                *ptr++ = UShort_t(*src++);
        }
        return;

    case 4:
        memcpy(dest, data.GetArray(), n*4);
        return;
    }
}

// --------------------------------------------------------------------------
//
// Set indices according to the given array. The array must have the same
// size as fHiGainPixId
//
void MRawEvtData::SetIndices(const MArrayS &idx)
{
    if (idx.GetSize()!=fHiGainPixId->GetSize())
        return;

    for (UInt_t i=0; i<fHiGainPixId->GetSize(); i++)
        (*fHiGainPixId)[i] = idx[i]-1;
}

// --------------------------------------------------------------------------
//
// Set indices according to the length of the array fHiGainPixId
// from 0 to n-1
//
void MRawEvtData::SetIndices()
{
    for (UInt_t i=0; i<fHiGainPixId->GetSize(); i++)
        (*fHiGainPixId)[i] = i;
}

Byte_t   *MRawEvtData::GetSamples() const    { return fHiGainFadcSamples->GetArray(); }
UShort_t *MRawEvtData::GetStartCells() const { return fStartCells->GetArray(); }
UShort_t *MRawEvtData::GetPixelIds() const   { return fHiGainPixId->GetArray(); }
Bool_t    MRawEvtData::HasStartCells() const { return fStartCells->GetSize()==fHiGainPixId->GetSize(); }

/*
void MRawEvtData::AddPixel(UShort_t nOfPixel, TArrayC *data, Bool_t lflag)
{
    MArrayS *arrpix = lflag ? fLoGainPixId       : fHiGainPixId;
    MArrayB *arrsam = lflag ? fLoGainFadcSamples : fHiGainFadcSamples;

    //
    // check whether we got the right number of new samples
    // if there are no samples already stored: this is the new number of samples
    //
    const UShort_t ns    = data->GetSize();
    const UShort_t nSamp = lflag ? GetNumLoGainSamples() : GetNumHiGainSamples();
    if (nSamp && ns!=nSamp)
    {
        *fLog << err << "RawEvtData::AddPixel: Error, number of samples in ";
        *fLog << "TArrayC " << ns << " doesn't match current number " << nSamp << endl;
        return;
    }

    //
    // enhance pixel array by one
    //
    arrpix->Set(arrpix->GetSize()+1);

    //
    // add the number of the new pixel to the array as last entry
    //
    arrpix->AddAt(nOfPixel, arrpix->GetSize()-1);

    //
    // enhance the array by the number of new samples
    //
    arrsam->Set(arrsam->GetSize()+ns);

    //
    // add the new slices as last entries to array
    //
    arrsam->AddAt((Byte_t*)data->GetArray(), arrsam->GetSize()-ns, ns);
}
*/

void MRawEvtData::ReadPixel(istream &fin, Int_t npix)
{
    // number of samples
    const UInt_t ns = fRunHeader->GetNumSamples();

    // bytes per sample
    const Int_t bps = fRunHeader->GetNumBytesPerSample();

    // Number of bytes
    const Int_t nb = ns*bps;

    // position in higain array
    Byte_t *pos = fHiGainFadcSamples->GetArray() + fConnectedPixels*nb;

    // Set pixel index
    fHiGainPixId->AddAt(npix, fConnectedPixels++);

    // Read data for one pixel
    fin.read((char*)pos, nb);
}

void MRawEvtData::SetABFlag(Int_t npix, Bool_t ab)
{
    if (ab)
        SETBIT((*fABFlags)[npix/8], npix%8);
    else
        CLRBIT((*fABFlags)[npix/8], npix%8);
}

// --------------------------------------------------------------------------
//
// Fills members with information from a magic binary file.
//   WARNING: you have to use Init() before you can do this
//
/*
void MRawEvtData::ReadEvt(istream &fin, Int_t posinarray)
{

    const UShort_t npic = fRunHeader->GetNumPixInCrate();

    const UShort_t npos = npic*posinarray;

    //const Byte_t ab = fCrateArray->GetEntry(posinarray)->GetABFlags();

    for (int i=npos; i<npic+npos; i++)
    {
        // calc the spiral hardware pixel number
        const UShort_t ipos = i;

        // Get Hardware Id
        const Short_t hwid = fRunHeader->GetPixAssignment(ipos);

        // Check whether the pixel is connected or not
        if (hwid==0)
        {
            const UShort_t n = fRunHeader->GetNumSamplesLoGain()+fRunHeader->GetNumSamplesHiGain();
            fin.seekg(n, ios::cur);
            return;
        }

        // -1 converts the hardware pixel Id into the software pixel index
        const Int_t npix = (Int_t)hwid-1;

        const Byte_t ab = fCrateArray->GetEntry(posinarray)->GetABFlags();
        AddPixel(fin, npix, TESTBIT(ab, i-npos));
    }
}
*/

// --------------------------------------------------------------------------
//
// Return the size in bytes of one event data block
//
Int_t MRawEvtData::GetNumBytes() const
{
    const UShort_t nlo  = fRunHeader->GetNumSamplesLoGain();
    const UShort_t nhi  = fRunHeader->GetNumSamplesHiGain();
    const UShort_t npic = fRunHeader->GetNumPixInCrate();
    const UShort_t nbps = fRunHeader->GetNumBytesPerSample();

    return (nhi+nlo)*npic*nbps;
}

// --------------------------------------------------------------------------
//
// Make sure, that you skip the whole event. This function only skips a part
// of the event - see MRawRead::SkipEvent
//
void MRawEvtData::SkipEvt(istream &fin)
{
    fin.seekg(GetNumBytes(), ios::cur);
}

Bool_t MRawEvtData::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
    *fLog << warn << "WARNING - The use of MRawEvtData::GetPixelContent is deprecated!" << endl;

    /*
    MRawEvtPixelIter Next(const_cast<MRawEvtData*>(this));
    if (!Next.Jump(idx))
        return kFALSE;

    switch (type)
    {
    case 0:
        val = Next.GetSumHiGainSamples()-(float)GetNumHiGainSamples()*fHiGainFadcSamples->GetArray()[0];
        val*= cam.GetPixRatio(idx);
        break;
    case 1:
        val = Next.GetMaxHiGainSample();
        break;
    case 2:
        val = Next.GetMaxLoGainSample();
        break;
    case 3:
        val = Next.GetIdxMaxHiGainSample();
        break;
    case 4:
        val = Next.GetIdxMaxLoGainSample();
        break;
    case 5:
        val = Next.GetIdxMaxHiLoGainSample();
        return val >= 0;
    }
*/
    return kTRUE;
}

void MRawEvtData::Copy(TObject &named)
#if ROOT_VERSION_CODE > ROOT_VERSION(3,04,01)
const
#endif
{
    MRawEvtData &evt = (MRawEvtData &)named;

    *evt.fHiGainPixId = *fHiGainPixId;
    *evt.fLoGainPixId = *fLoGainPixId;

    *evt.fHiGainFadcSamples = *fHiGainFadcSamples;
    *evt.fLoGainFadcSamples = *fLoGainFadcSamples;

    *evt.fABFlags = *fABFlags;

    evt.fConnectedPixels = fConnectedPixels;

    evt.fNumBytesPerSample = fNumBytesPerSample;
}
