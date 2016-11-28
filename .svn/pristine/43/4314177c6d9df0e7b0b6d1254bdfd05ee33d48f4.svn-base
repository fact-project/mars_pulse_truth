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
!   Author(s): Thomas Bretz  4/2007 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MMovieWrite
//
// The intention of this class is to encode movies prepard by the
// MMoviePrepare task.
//
// For writing the movies the images are converted to ppm and piped through
// ppm2y4m to mpeg2enc. The output format is a mpeg2 movie and should
// be within the specifications of mpeg2 for DVD. Its size is 720x480,
// which is a good compromise between resolution and file size. The frame
// rate is fixed to 24fps.
//
// By changing the setup you can control the output:
//
//    NumEvents: To make sure the file size doesn't get too large
//     (300 evts are roughly 80MB with the default seetings) you can set
//     a maximum number of events. If this number of events has been encoded
//     the eventloop is stopped.
//
//    TargetLength: The length (in seconds) each even will be encoded to.
//     For example with the default Target Length of 5s and the fixed frame
//     rate of 24fps each event will be encoded into 24f/s*5s +1f = 121frames
//     equally distributed between the beginning of the first and the end of
//     the last frame.
//
//    Threshold: The default threshold is 2. At 2 times median pedestal rms
//     of the pixles with area index 0 (for MAGIC: inner pixels) the color
//     palette will change from yellow to red and isolated pixels between
//     the median rms and 2 times the median of the rms will be removed from
//     the image. To switch off this behaviour you can set a threshold
//     below one.
//
//    Filename: The output filename of the movie. If it doesn't end with ".mpg"
//     the suffix is added.
//
//  The interpolation of the frames is done using a TSpline3. If the spline
//  would extrapolate due to the shift by the relative time calibration the
//  contents is set to zero. Unsuitable pixels are interpolated frame by
//  frame using the surrounding suitable pixels.
//
//  A few words about file size: MPEG is a motion compensation compression,
//  which means that if a region of a past frame is shown again at the same
//  place or somewhere else this region is referenced instead of encoded again.
//  This means that in our case (almost all frames are identical!) the
//  increase of file size is far from linear with the number of encoded events!
//
//
//  Input:
//   MGeomCam
//   MRawRunHeader
//   MRawEvtHeader
//   MSignalCam
//   MBadPixelsCam
//   MMovieData
//   [MMcEvt]
//
/////////////////////////////////////////////////////////////////////////////
#include "MMovieWrite.h"

#include <errno.h>

#include <TF1.h>
#include <TStyle.h>
#include <TColor.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TASImage.h>
#include <TStopwatch.h>

#include "MString.h"

#include "MParList.h"
#include "MTaskList.h"

#include "MGeomCam.h"
#include "MGeomPix.h"

#include "MMcEvt.hxx"

#include "MH.h"
#include "MHCamera.h"
#include "MMovieData.h"
#include "MSignalCam.h"
#include "MRawEvtHeader.h"
#include "MRawRunHeader.h"
#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"
#include "MCalibrateData.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MMovieWrite);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MMovieWrite::MMovieWrite(const char *name, const char *title)
    : fPipe(0), fTargetLength(5), fThreshold(2), fNumEvents(25000), fFilename("movie.mpg")
{
    fName  = name  ? name  : "MMovieWrite";
    fTitle = title ? title : "Task to encode a movie";
}

// --------------------------------------------------------------------------
//
// Close pipe if still open
//
MMovieWrite::~MMovieWrite()
{
    if (fPipe)
        gSystem->ClosePipe(fPipe);
}

// --------------------------------------------------------------------------
//
// Check the pipe for errors. In case of error print an error message.
// return kFALSE in case of error, kTRUE in case of success.
//
Bool_t MMovieWrite::CheckPipe()
{
    if (!ferror(fPipe))
        return kTRUE;

    *fLog << err << "Error in pipe: " << strerror(errno) << endl;
    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Open pipe for encoding the movie
//
Bool_t MMovieWrite::OpenPipe()
{
    //    name = "ppmtoy4m -B -S 420mpeg2 -v 0 | yuvplay";
    //    name = Form("ppmtoy4m -B -S 420jpeg -v 0 -F %d:%d | yuv2lav -v 0 -o output%03d.avi", TMath::Nint()  TMath::Nint(fTargetLength*1000))
    //    name = "ppmtoy4m -B -F 3:1 -S 420jpeg -v 0 | yuv2lav -v 0 -o output.avi";

    TString name;
    name  = "ppmtoy4m -B -F 24:1 -S 420jpeg -v 0 | ";
    name += "mpeg2enc -v 0 -F 2 -I 0 -M 2 -o ";
    name += fFilename;
    if (!fFilename.EndsWith(".mpg"))
        name += ".mpg";

    const Int_t n = TMath::Nint(fTargetLength*24)+1;

    name += " -f 9 -E 40 -r 0 -K kvcd ";
    name += MString::Format("-g %d -G %d", n, n);

    // For higher resolution add "--no-constraints"

    fPipe = gSystem->OpenPipe(name, "w");
    if (!fPipe)
    {
        *fLog << err;
        *fLog << "Pipe: " << name << endl;
        *fLog << "Couldn't open pipe... aborting." << endl;
        CheckPipe();
        return kFALSE;
    }

    *fLog << inf << "Setup pipe to ppmtoy4m and mpeg2enc to encode " << fFilename << "." << endl;

    return kTRUE;

    //  1: 37M name += "-f 9 -E 40 -H -4 1 -2 1 --dualprime-mpeg2";
    //  2: 42M name += "-f 9";
    //  3: 37M name += "-f 9 -E 40 -4 1 -2 1 --dualprime-mpeg2";
    //  4: 37M name += "-f 9 -E 40 -4 1 -2 1";
    //  5: 37M name += "-f 9 -E 40 -4 4 -2 4";           // 640x400   3 frames/slice
    //  6: 11M name += "-f 3 -E 40 -b 750";              // 640x400   3 frames/slice

    //  7: 28M name += "-f 9 -E 40 -G 50";               // 640x400   3 frames/slice
    //  8: 24M name += "-f 9 -E 40 -G 500";              // 640x400   3 frames/slice

    //  9: 17M name += "-f 9 -E 40 -G 2400";             // 640x400  24 frames/slice
    // 10: 19M name += "-f 9 -E 40 -G 1120";             // 720x480   3 frames/slice
    // 20: 33M name += "-f 9 -E 40 -g 28 -G 28";         // 720x480   3 frames/slice
    //     57M name += "-f 9 -E 40 -g 28 -G 28 -q 4";    // 720x480   3 frames/slice

    //     30M name += "-f 9 -E  40 -g 84 -G 84 -r  0";          // 720x480   3 frames/slice
    //     31M name += "-f 9 -E  40 -g 56 -G 56 -r  0";          // 720x480   3 frames/slice
    //     34M name += "-f 9 -E  40 -g 28 -G 28 -r  0";          // 720x480   3 frames/slice
    // 24: 24M name += "-f 9 -E  40 -g 28 -G 28 -r  0 -K kvcd";  // 720x480   3 frames/slice
    // 25: 24M name += "-f 9 -E -40 -g 28 -G 28 -r  0 -K kvcd";  // 720x480   3 frames/slice
    // 26: 26M name += "-f 9 -E   0 -g 28 -G 28 -r  0 -K kvcd";  // 720x480   3 frames/slice
    //     34M name += "-f 9 -E  40 -g 28 -G 28 -r  2";          // 720x480   3 frames/slice
    //     33M name += "-f 9 -E  40 -g 28 -G 28 -r 32";          // 720x480   3 frames/slice

    // name += "-f 9 -E 40 -g 121 -G 121 -r 0 -K kvcd";  // 720x480 5s  24 frames/slice

    // 11: 56M name += "-f 9 -E 40 -g 217 -G 217";       // 720x480  24 frames/slice
    // 18: 59M name += "-f 9 -E 40 -G 250";              // 720x480  24 frames/slice
    //     62M name += "-f 9 -E 40 -G 184";              // 720x480  24 frames/slice

    // 12: --- name += "-f 9 -E 40 -G 500 -q 31";        // 720x480  3frames/slice
    // 13: 49M name += "-f 9 -E 40 -G 500 -q 4";         // 720x480  3frames/slice
    // 14: 21M name += "-f 9 -E 40 -G 500 -q 4 -b 1500"; // 720x480  3frames/slice

    // 15: 57M name += "-f 9 -E 40 -G 500 --no-constraints"; // 1280 864  3frames/slice

    // 16: >80 name += "-f 9 -E 40 -G 217 --no-constraints -b 3000"; // 1280 864  24frames/slice
    // 17: >50 name += "-f 9 -E 40 -G 682 -b 3000 --no-constraints"; // 1280 864  24frames/slice
}

// --------------------------------------------------------------------------
//
// Search for:
//   - MGeomCam
//   - MRawRunHeader
//   - MRawEvtHeader
//   - MSignalCam
//   - MBadPixelsCam
//   - MMovieData
//
// Open a pipe to write the images to. Can be either a player or
// an encoder.
//
Int_t MMovieWrite::PreProcess(MParList *plist)
{
    fCam = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!fCam)
    {
        *fLog << err << "MGeomCam not found ... aborting." << endl;
        return kFALSE;
    }
    fRun = (MRawRunHeader*)plist->FindObject("MRawRunHeader");
    if (!fRun)
    {
        *fLog << err << "MRawRunHeader not found ... aborting." << endl;
        return kFALSE;
    }
    fHead = (MRawEvtHeader*)plist->FindObject("MRawEvtHeader");
    if (!fHead)
    {
        *fLog << err << "MRawEvtHeader not found ... aborting." << endl;
        return kFALSE;
    }
    fSig = (MSignalCam*)plist->FindObject("MSignalCam");
    if (!fSig)
    {
        *fLog << err << "MSignalCam not found ... aborting." << endl;
        return kFALSE;
    }
    fBad = (MBadPixelsCam*)plist->FindObject("MBadPixelsCam");
    if (!fBad)
    {
        *fLog << err << "MBadPixelsCam not found ... aborting." << endl;
        return kFALSE;
    }
    fIn = (MMovieData*)plist->FindObject("MMovieData");
    if (!fIn)
    {
        *fLog << err << "MMovieData not found... aborting." << endl;
        return kFALSE;
    }

    fMC = (MMcEvt*)plist->FindObject("MMcEvt");

    return OpenPipe();
}

#ifdef USE_TIMING
TStopwatch clockT, clock1, clock2, clock3;
#endif

// --------------------------------------------------------------------------
//
// Close pipe if still open
//
Int_t MMovieWrite::PostProcess()
{
    if (fPipe)
    {
        gSystem->ClosePipe(fPipe);
        fPipe=0;
    }

#ifdef USE_TIMING
    *fLog << all << endl;
    *fLog << "Snap: " << flush;
    clock1.Print();
    *fLog << "Writ: " << flush;
    clock2.Print();
    *fLog << "Prep: " << flush;
    clock3.Print();
    *fLog << "Totl: " << flush;
    clockT.Print();
    *fLog << endl;
#endif

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Produce a 99 color palette made such, that everything below one
// pedestal rms is white, everything up to two pedestal rms is yellow
// and everything above gets colors.
//
Int_t MMovieWrite::SetPalette(Double_t rms, const TH1 &h) const
{
    const Double_t min = h.GetMinimum();
    const Double_t max = h.GetMaximum();

    const Double_t f = (fThreshold*rms-min)/(max-min);
    const Double_t w = 1-f;

    // --- Produce the nice colored palette ---

    //             min      th*rms                 max
    double s[6] = {0.0, f/2,  f,   f+w/4, f+3*w/5, 1.0 };

    double r[6] = {1.0, 1.0,  1.0, 0.85,  0.1,     0.0 };
    double g[6] = {1.0, 1.0,  1.0, 0.15,  0.1,     0.0 };
    double b[6] = {0.9, 0.55, 0.4, 0.15,  0.7,     0.1 };

    TArrayI col(99);

#if ROOT_VERSION_CODE < ROOT_VERSION(5,18,00)
    const Int_t rc = gStyle->CreateGradientColorTable(6, s, r, g, b, col.GetSize());
#else
    const Int_t rc = TColor::CreateGradientColorTable(6, s, r, g, b, col.GetSize());
#endif

    // --- Overwrite the 'underflow' bin with white ---

    for (int i=0; i<col.GetSize(); i++)
        col[i] = gStyle->GetColorPalette(i);

    col[0] = TColor::GetColor(0xff, 0xff, 0xff);

    // --- Set Plette ---

    gStyle->SetPalette(col.GetSize(), col.GetArray());

    return rc;
}

// --------------------------------------------------------------------------
//
// The created colors are not overwritten and must be deleted manually
// because having more than 32768 color in a palette will crash
// gPad->PaintBox
//
void MMovieWrite::DeletePalette(Int_t colidx) const
{
    for (int i=0; i<99; i++)
    {
        TColor *col = gROOT->GetColor(colidx+i);
        if (col)
            delete col;
    }
}

/*
// --------------------------------------------------------------------------
//
// Do a snapshot from the pad via TASImage::FromPad and write the
// image to the pipe.
// return kFALSE in case of error, kTRUE in case of success.
//
Bool_t MMovieWrite::WriteImage(TVirtualPad &pad)
{
    clock1.Start(kFALSE);
    TASImage img;
    img.FromPad(&pad);
    clock1.Stop();

    clock2.Start(kFALSE);
    const Bool_t rc = WriteImage(img);
    clock2.Stop();

    return rc;
}

#include <TVirtualPS.h>
Bool_t MMovieWrite::WriteImage(TVirtualPad &pad)
{
    TVirtualPS *psave = gVirtualPS;

    clock1.Start(kFALSE);
    TImage dump("", 114);
    dump.SetBit(BIT(11));
    pad.Paint();
    TASImage *itmp = (TASImage*)dump.GetStream();
    clock1.Stop();

    clock2.Start(kFALSE);
    const Bool_t rc = WriteImage(*itmp);
    clock2.Stop();

    gVirtualPS = psave;

    return rc;
}
*/

// --------------------------------------------------------------------------
//
// Update the part of the idst image with the contents of the pad.
//
// It is a lot faster not to rerender the parts of the image which don't
// change anyhow, because rerendering the camera is by far the slowest.
//
void MMovieWrite::UpdateImage(TASImage &idst, TVirtualPad &pad)
{
    // Get image from pad
    TASImage isrc;
    isrc.FromPad(&pad);

    // Get position and width of destination- and source-image
    const UInt_t wsrc = isrc.GetWidth();    // width  of image
    const UInt_t hsrc = isrc.GetHeight();   // height of image

    const UInt_t usrc = pad.UtoPixel(1)*4;  // width  of pad (argb)
    //const UInt_t vsrc = pad.VtoPixel(0);    // height of pad

    const UInt_t xsrc = pad.UtoAbsPixel(0); // offset of pad in canvas
    const UInt_t ysrc = pad.VtoAbsPixel(1); // offset of pad in canvas

    const UInt_t wdst = idst.GetWidth();
    //const UInt_t hdst = idst.GetHeight();

    // Update destination image with source image
    const UInt_t size = wsrc*hsrc;

    UInt_t *psrc = isrc.GetArgbArray();
    UInt_t *pdst = idst.GetArgbArray();

    UInt_t *src = psrc + ysrc*wsrc+xsrc;
    UInt_t *dst = pdst + ysrc*wdst+xsrc;

    while (src<psrc+size)
    {
        memcpy(dst, src, usrc);

        src += wsrc;
        dst += wdst;
    }
}

// --------------------------------------------------------------------------
//
// Write the image as ppm (raw/P6) to the pipe.
// return kFALSE in case of error, kTRUE in case of success.
//
Bool_t MMovieWrite::WriteImage(TASImage &img)
{
    // Write image header
    fprintf(fPipe, "P6 %d %d 255\n", img.GetWidth(), img.GetHeight());
    if (!CheckPipe())
        return kFALSE;

    // Write image data (remove alpha channel from argb data)
    UInt_t *argb = img.GetArgbArray();
    for (UInt_t *ptr=argb; ptr<argb+img.GetWidth()*img.GetHeight(); ptr++)
        if (fwrite(ptr, 3, 1, fPipe)!=1)
            break;

    return CheckPipe();
}

// --------------------------------------------------------------------------
//
// Update TASImage with changing parts of the image and write image to pipe.
// return kFALSE in case of error, kTRUE in case of success.
//
Bool_t MMovieWrite::WriteImage(TASImage &img, TVirtualPad &pad)
{
#ifdef USE_TIMING
    clock1.Start(kFALSE);
    UpdateImage(img, pad);
    clock1.Stop();

    clock2.Start(kFALSE);
    const Bool_t rc = WriteImage(img);
    clock2.Stop();

    return rc;
#else
    UpdateImage(img, pad);
    return WriteImage(img);
#endif
}

// --------------------------------------------------------------------------
//
// Do a simple interpolation of the surrounding suitable pixels for all
// unsuitable pixels.
//
void MMovieWrite::TreatBadPixels(TH1 &h) const
{
    const UShort_t entries = fCam->GetNumPixels();

    //
    // Loop over all pixels
    //
    for (UShort_t i=0; i<entries; i++)
    {
        //
        // Check whether pixel with idx i is blind
        //
        if (!(*fBad)[i].IsUnsuitable())
            continue;

        const MGeom &gpix = (*fCam)[i];

        Int_t    num = 0;
        Double_t sum = 0;

        //
        // Loop over all its neighbors
        //
        Int_t n = gpix.GetNumNeighbors();
        while (n--)
        {
            const UShort_t nidx = gpix.GetNeighbor(n);

            //
            // Do not use blind neighbors
            //
            if ((*fBad)[nidx].IsUnsuitable())
                continue;

            sum += h.GetBinContent(nidx+1);
            num++;
        }

        h.SetBinContent(i+1, sum/num);
    }
}

// --------------------------------------------------------------------------
//
// Do a simple interpolation of the surrounding suitable pixels for all
// unsuitable pixels.
//
void MMovieWrite::Clean(TH1 &h, Double_t rms) const
{
    if (fThreshold<1)
        return;

    const UShort_t entries = fCam->GetNumPixels();

    //
    // Loop over all pixels
    //
    for (UShort_t i=0; i<entries; i++)
    {
        Double_t val = h.GetBinContent(i+1);
        if (val<rms || val>fThreshold*rms)
            continue;

        const MGeom &gpix = (*fCam)[i];

        //
        // Loop over all its neighbors
        //
        Int_t n = gpix.GetNumNeighbors();
        while (n)
        {
            const UShort_t nidx = gpix.GetNeighbor(n-1);
            if (h.GetBinContent(nidx+1)>=rms)
                break;
            n--;
        }

        if (n==0)
            h.SetBinContent(i+1, 0);
    }
}

// --------------------------------------------------------------------------
//
Bool_t MMovieWrite::Process(TH1 &h, TVirtualPad &c)
{
    // ---------------- Setup ------------------

    const Float_t freq   = fRun->GetFreqSampling()/1000.; // [GHz] Sampling frequency
    const UInt_t  slices = fIn->GetNumSlices();
    const Float_t len    = slices/freq;                   // [ns]  length of data stream in data-time
    //const Float_t len    = (slices-2)/freq;               // [ns]  length of data stream in data-time

    const Double_t rms = fIn->GetMedianPedestalRms();
    const Double_t max = fIn->GetMax();    // scale the lover limit such
    const Double_t dif = (max-rms)*99/98;  // that everything below rms is
    const Double_t min = max-dif;          // displayed as white

    // If the maximum is equal or less the
    // pedestal rms something must be wrong
    if (dif<=0)
        return kFALSE;

    h.SetMinimum(min);
    h.SetMaximum(max);

    // -----------------------------------------

    // Produce starting image from canvas
    TASImage img;
    img.FromPad(&c);

    // Set new adapted palette for further rendering
    const Int_t colidx = SetPalette(rms, h);

    // Get the pad containing the camera with the movie
    TVirtualPad &pad = *c.GetPad(1)->GetPad(1);

    // Calculate number of frames
    const Int_t numframes = TMath::Nint(fTargetLength*24);

    // Get number of pixels in camera
    const Int_t npix = fCam->GetNumPixels();

    // Loop over all frames+1 (upper edge)
    for (Int_t i=0; i<=numframes; i++)
    {
        // Calculate corresponding time
        const Float_t t = len*i/numframes;// + 0.5/freq;  // Process from slice beg+0.5 to end-1.5

        // Calculate histogram contents by spline interpolation
        for (UShort_t p=0; p<npix; p++)
        {
            const Double_t y = (*fBad)[p].IsUnsuitable() ? 0 : fIn->CheckedEval(p, t);
            h.SetBinContent(p+1, y);
        }

        // Interpolate unsuitable pixels
        TreatBadPixels(h);

        // Clean single pixels
        Clean(h, rms);

        // Set new name to be displayed
        h.SetName(MString::Format("%d: %.2f/%.1fns", i+1, t*freq, t));

        // Update existing image with new data and encode into pipe
        if (!WriteImage(img, pad))
            return kFALSE;
    }

    DeletePalette(colidx);

    cout << setw(3) << GetNumExecutions() << ": " << MString::Format("%6.2f", (float)numframes/(slices-2)) << " f/sl " << slices << " " << numframes+1 << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
Int_t MMovieWrite::Process()
{
#ifdef USE_TIMING
    clockT.Start(kFALSE);

    clock3.Start(kFALSE);
#endif
    // ---------------- Prepare display ------------------

    Bool_t batch = gROOT->IsBatch();
    gROOT->SetBatch();

    TCanvas c;
    //c.Iconify();
    c.SetBorderMode(0);
    c.SetFrameBorderMode(0);
    c.SetFillColor(kWhite);
    //c.SetCanvasSize(640, 400);
    c.SetCanvasSize(720, 480);
    //c.SetCanvasSize(960, 640);
    //c.SetCanvasSize(1024, 688);
    //c.SetCanvasSize(1152, 768);
    //c.SetCanvasSize(1280, 864);

    MH::SetPalette("pretty");

    c.cd();
    TPad p1("Pad2", "", 0.7, 0.66, 0.99, 0.99);
    p1.SetNumber(2);
    p1.SetBorderMode(0);
    p1.SetFrameBorderMode(0);
    p1.SetFillColor(kWhite);
    p1.Draw();
    p1.cd();

    MHCamera hsig(*fCam, "Signal", "Calibrated Signal");
    hsig.SetYTitle("S [phe]");
    hsig.SetCamContent(*fSig, 99);
    hsig.SetMinimum(0);
    //hsig.SetContour(99);
    hsig.Draw("nopal");

    c.cd();
    TPad p2("Pad3", "", 0.7, 0.33, 0.99, 0.65);
    p2.SetNumber(3);
    p2.SetBorderMode(0);
    p2.SetFrameBorderMode(0);
    p2.SetFillColor(kWhite);
    p2.Draw();
    p2.cd();

    MHCamera htime(*fCam, "ArrivalTime", "Calibrated Arrival Time");
    htime.SetYTitle("T [au]");
    htime.SetCamContent(*fSig, 8);
    htime.Draw("nopal");

    c.cd();
    TPad p3("Pad4", "", 0.7, 0.00, 0.99, 0.32);
    p3.SetNumber(4);
    p3.SetBorderMode(0);
    p3.SetFrameBorderMode(0);
    p3.SetFillColor(kWhite);
    p3.Draw();
    p3.cd();
/*
    TH1F htpro("TimeProj", "", slices, 0, len);
    for (UInt_t i=0; i<htime.GetNumPixels(); i++)
        if(htime.IsUsed(i))
            htpro.Fill((htime.GetBinContent(i+1)-first)/freq, hsig.GetBinContent(i+1));
    htpro.SetMinimum(0);
    htpro.SetMaximum(100);
    htpro.SetLineColor(kBlue);
    htpro.Draw();

    TF1 fgaus("f1", "gaus");
    const Double_t m = (htpro.GetMaximumBin()-0.5)*len/slices;
    fgaus.SetParameter(0, htpro.GetMaximum());
    fgaus.SetParameter(1, m);
    fgaus.SetParameter(2, 1.0);
    fgaus.SetParLimits(1, m-3, m+3);
    fgaus.SetParLimits(2, 0, 3);
    fgaus.SetLineWidth(1);
    fgaus.SetLineColor(kMagenta);
    htpro.Fit(&fgaus, "NI", "", m-3, m+3);
    fgaus.Draw("same");

    g.SetMarkerStyle(kFullDotMedium);
    g.Draw("PL");
    //g.SetMinimum(0);
    //g.SetMaximum(100);
    //g.Draw("APL");

    p3.Update();
    p3.cd(1);
    gPad->Update();
  */
    c.cd();
    TPad p0("MainPad", "", 0.01, 0.01, 0.69, 0.99);
    p0.SetNumber(1);
    p0.SetBorderMode(0);
    p0.SetFrameBorderMode(0);
    p0.SetFillColor(kWhite);
    p0.Draw();
    p0.cd();
 /*
    cout << "Max=" << hsig.GetMaximum() << "/" << fIn->GetMax() << "   ";
    cout << hsig.GetMaximum()/fIn->GetMax() << endl;
    Float_t rms0 = fPed->GetAveragedRmsPerArea(*fCam, 0, fBad)[0];
    Float_t rms1 = fPed->GetAveragedRmsPerArea(*fCam, 1, fBad)[0];
    cout << "RMS="<<rms0<<"/"<<rms1<<endl;

    rms0 = GetMedianPedestalRms();

    cout << "MED=" << rms0 << endl;
    */

    TString s = MString::Format("%d:  Evt #", GetNumExecutions()+1);
    s += fHead->GetDAQEvtNumber();
    s += " of ";
    s += "Run #";
    s += fRun->GetRunNumber();
    if (fMC)
        s = fMC->GetDescription(s);

    MHCamera h(*fCam);
    h.SetTitle(s);
    h.SetAllUsed();
    h.SetYTitle("V [au]");
    h.SetContour(99);

    h.Draw("nopal");

    // ---------------- Show data ------------------
    gStyle->SetOptStat(1000000001);
/*
    p0.Modified();
    p1.Modified();
    p2.Modified();

    p0.GetPad(1)->Modified();
    p1.GetPad(1)->Modified();
    p2.GetPad(1)->Modified();

    c.Update();
 */

    // ---------------- Show data ------------------
#ifdef USE_TIMING
    clock3.Stop();
#endif

    // Switch off automatical adding to directory (SetName would do)
    const Bool_t add = TH1::AddDirectoryStatus();
    TH1::AddDirectory(kFALSE);

    const Bool_t rc = Process(h, c);

    // restore previous state
    TH1::AddDirectory(add);

#ifdef USE_TIMING
    clockT.Stop();
#endif

    gROOT->SetBatch(batch);

    if (!rc)
        return kERROR;

    return fNumEvents<=0 || GetNumExecutions()<fNumEvents;
}

// --------------------------------------------------------------------------
//
// Check for corresponding entries in resource file and setup
//
// Example:
//   MMovieWrite.TargetLength: 5 <seconds>
//   MMovieWrite.NumEvents: 500
//   MMovieWrite.Threshold: 2 <rms>
//   MMovieWrite.FileName: movie.mpg
//
Int_t MMovieWrite::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "NumEvents", print))
    {
        fNumEvents = GetEnvValue(env, prefix, "NumEvents", (Int_t)fNumEvents);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "TargetLength", print))
    {
        fTargetLength = GetEnvValue(env, prefix, "TargetLength", fTargetLength);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "Threshold", print))
    {
        fThreshold = GetEnvValue(env, prefix, "Threshold", fThreshold);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "FileName", print))
    {
        fFilename = GetEnvValue(env, prefix, "FileName", fFilename);
        rc = kTRUE;
    }
    return rc;
}
