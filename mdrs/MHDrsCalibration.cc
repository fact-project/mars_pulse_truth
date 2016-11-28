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
!   Author(s): Thomas Bretz 2013 <mailto:thomas.bretz@epfl.ch>
!
!   Copyright: MAGIC Software Development, 2000-2013
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////
//
// MHDrsCalibration
//
// This class contains a list of MHFadcPix.
//
///////////////////////////////////////////////////////////////////////
#include "MHDrsCalibration.h"

#include <fstream>
#include <sstream>
#include <limits.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MGeomCam.h"
#include "MBinning.h"

#include "MRawEvtData.h"
#include "MRawRunHeader.h"
#include "MHCamera.h"
#include "MDrsCalibration.h"

#include <TH2.h>
#include <TMath.h>

ClassImp(MHDrsCalibration);

using namespace std;

// --------------------------------------------------------------------------
//
//  default constructor
//  creates an a list of histograms for all pixels and both gain channels
//
MHDrsCalibration::MHDrsCalibration(const char *name, const char *title) : fRun(0),
fEvt(0), fResult(0), fBuffer(1440*1024*6+160*1024*2+4),
fStep(-1), fScale(0), fNumPixels(0), fNumSamples(0)
{
    //
    // set the name and title of this object
    //
    fName  = name  ? name  : "MHDrsCalibration";
    fTitle = title ? title : "Container for ADC spectra histograms";

    for (int i=1024*1440*2+4; i<1024*1440*3+4; i++)
        fBuffer[i] = 2000./4096; // Set mean to 0.5
}

// --------------------------------------------------------------------------
//
// To setup the object we get the number of pixels from a MGeomCam object
// in the Parameter list.
//
Bool_t MHDrsCalibration::SetupFill(const MParList *pList)
{
    fRun = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRun)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    switch (++fStep)
    {
    case 0:
        SetTitle("DRS average baseline (physical pipeline);;ADC signal [mV];");
        break;
    case 1:
        SetTitle("DRS average gain (baseline subtracted, physical pipeline);;ADC signal [mV];");
        break;
    case 2:
        SetTitle("DRS average trigger offset (baseline subtracted, logical pipeline);;ADC signal [mV];");
        break;
    }

    if (!MHCamEvent::SetupFill(pList))
        return kFALSE;

    fSum->ResetBit(MHCamera::kProfile);

    fData.Reset();

    fScale = 1;

    *fLog << warn << "WARNING --- oulier detection needed!" << endl;

    return kTRUE;
}

Bool_t MHDrsCalibration::ReInit(MParList *pList)
{
/*    fRun = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRun)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }*/

    fEvt = (MRawEvtData*)pList->FindObject("MRawEvtData");
    if (!fEvt)
    {
        *fLog << err << "MRawEvtData not found... aborting." << endl;
        return kFALSE;
    }

    if (!fEvt->HasStartCells())
    {
        *fLog << err << "MRawEvtData has no start cells stored... aborting." << endl;
        return kFALSE;
    }

    // FIXME: check consistency...?

    if (fEvt->GetNumPixels()==0 || fEvt->GetNumSamples()==0)
    {
        *fLog << err << "MRawEvtData contains either no pixels or no samples... aborting." << endl;
        return kFALSE;
    }

    if (fStep>=3)
    {
        *fLog << err << "DRS calibration already finished." << endl;
        return kFALSE;
    }


    fData.InitSize(fEvt->GetNumPixels(), fEvt->GetNumSamples());

    fResult = (MDrsCalibration*)pList->FindCreateObj("MDrsCalibration");
    if (!fResult)
        return kFALSE;

    if (fStep==1 || fStep==2)
        fScale = fResult->fNumOffset;

    fNumPixels  = fEvt->GetNumPixels();
    fNumSamples = fEvt->GetNumSamples();

    if (fStep<2 && fNumSamples!=1024)
    {
        *fLog << err << "Only 1024 samples are supported for step " << fStep << "... file has " << fNumSamples << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
Int_t MHDrsCalibration::Fill(const MParContainer *par, const Stat_t w)
{

    if (fStep==0)
    {
        fData.AddRel(reinterpret_cast<int16_t*>(fEvt->GetSamples()),
                     reinterpret_cast<int16_t*>(fEvt->GetStartCells()));
    }
    if (fStep==1)
    {
        fData.AddRel(reinterpret_cast<int16_t*>(fEvt->GetSamples()),
                     reinterpret_cast<int16_t*>(fEvt->GetStartCells()),
                     fResult->fOffset.data(),
                     fResult->fNumOffset);
    }
    if (fStep==2)
    {
        fData.AddAbs(reinterpret_cast<int16_t*>(fEvt->GetSamples()),
                     reinterpret_cast<int16_t*>(fEvt->GetStartCells()),
                     fResult->fOffset.data(),
                     fResult->fNumOffset);
    }

    /*
    if (fStep==3)
    {
        fData.AddAbs(reinterpret_cast<int16_t*>(fEvt->GetSamples()),
                     reinterpret_cast<int16_t*>(fEvt->GetStartCells()),
                     fResult->fOffset.data(),
                     fResult->fNumOffset);
    }*/

    return kTRUE;
}

void MHDrsCalibration::InitHistogram()
{
    if (!fEvt)
        return;

    // Be carefull: the contents of fData are not cloned!
    pair<vector<double>,vector<double> > p = fData.GetSampleStats();

    vector<double> &mean = p.first;
    vector<double> &rms  = p.second;

    if (mean.size()==0)
        return;

    fSum->Reset();

    for (size_t ch=0; ch<fNumPixels; ch++)
    {
        double m = 0;
        double r = 0;
        for (size_t i=0; i<fNumSamples; i++)
        {
            m += mean[ch*fNumSamples+i];
            r += rms [ch*fNumSamples+i];
        }

        fSum->AddBinContent(fEvt->GetPixelIds()[ch]+1, m);
        fSum->SetBinError(fEvt->GetPixelIds()[ch]+1,   r);
    }

    fSum->Scale(1./fNumSamples/fScale*2000/4096);  // 0.5mV/ADC count
    fSum->SetEntries(fData.GetNumEntries());
    fSum->SetAllUsed();
}

template<typename T>
Bool_t MHDrsCalibration::CopyData(vector<T> &dest) const
{
    const vector<int64_t> &sum = fData.GetSum();
    dest.resize(sum.size());

    for (size_t ch=0; ch<fNumPixels; ch++)
        for (size_t i=0; i<fNumSamples; i++)
        {
            if (sum[ch*fNumSamples+i]>UINT_MAX)
            {
                *fLog << err << "ERROR - Sum over slices exceeded maximum allowed range." << endl;
                return kFALSE;
            }
            dest[ch*fNumSamples+i] = sum[ch*fNumSamples+i];
        }

    return kTRUE;
}

Bool_t MHDrsCalibration::Finalize()
{
    if (!fResult)
        return kTRUE;

    fResult->fRoi   = fNumSamples;
    fResult->fStep  = fStep;
    fResult->fNumTm = 0;

    // ==========================================================================
    if (fStep==0)
    {
        fResult->fNumOffset = fData.GetNumEntries();
        if (!CopyData(fResult->fOffset))
            return kFALSE;

        //--------------------------------------------
        for (int i=0; i<1024*1440; i++)
            fResult->fGain[i] = 4096*fData.GetNumEntries();

        // Scale ADC data from 12bit to 2000mV
        fData.GetSampleStats(fBuffer.data()+4, 2000./4096);
        reinterpret_cast<uint32_t*>(fBuffer.data())[1] = fRun->GetFileNumber();
    }

    // ==========================================================================
    if (fStep==1)
    {
        fResult->fNumGain = fData.GetNumEntries();//*fResult->fNumCellOffset;
        if (!CopyData(fResult->fGain))
            return kFALSE;

        //---------------------------------------------
        // DAC:  0..2.5V == 0..65535            2500*50000   625*50000  625*3125
        // V-mV: 1000                           ----------   ---------  --------
        //fNumGain *= 2500*50000;                  65536       16384      1024
        //for (int i=0; i<1024*1440; i++)
        //    fGain[i] *= 65536;
        fResult->fNumGain *= 1953125;
        for (unsigned int i=0; i<fNumPixels*fNumSamples; i++)
            fResult->fGain[i] *= 1024;

        // Scale ADC data from 12bit to 2000mV
        fData.GetSampleStats(fBuffer.data()+1024*1440*2+4, 2000./4096/fResult->fNumOffset);//0.5);
        reinterpret_cast<uint32_t*>(fBuffer.data())[2] = fRun->GetFileNumber();
    }

    // ==========================================================================
    if (fStep==2)
    {
        fResult->fNumTrgOff = fData.GetNumEntries();//*fResult->fNumCellOffset;
        if (!CopyData(fResult->fTrgOff))
            return kFALSE;

        //--------------------------------------------
        // Scale ADC data from 12bit to 2000mV
        fData.GetSampleStats(fBuffer.data()+1024*1440*4+4, 2000./4096/fResult->fNumOffset);//0.5);
        reinterpret_cast<uint32_t*>(fBuffer.data())[0] = fNumSamples;
        reinterpret_cast<uint32_t*>(fBuffer.data())[3] = fRun->GetFileNumber();
    }

    // --------------------------------------

    InitHistogram();

    if (fOutputPath.IsNull())
        return kTRUE;

    const std::string fname(Form("%s/%d_%03d.drs.fits", fOutputPath.Data(),
                                 fRun->GetRunNumber(), fRun->GetFileNumber()));

    const Bool_t exists = !gSystem->AccessPathName(fname.c_str(), kFileExists);
    if (exists)
    {
        *fLog << err << "File '" << fname << "' already exists." << endl;
        return kFALSE;
    }

    const std::string msg = fResult->WriteFitsImp(fname, fBuffer);

    if (msg.empty())
    {
        *fLog << inf << "Wrote DRS calibration file " << fname << endl;
        return kTRUE;
    }

    *fLog << err << "Error writing to " << fname << ": " << msg << endl;
    return kFALSE;
}

void MHDrsCalibration::Paint(Option_t *o)
{
    InitHistogram();
    MHCamEvent::Paint(o);
}

Short_t MHDrsCalibration::GetNumUnderflows(float lvl) const
{
    if (!fResult)
        return -1;

    short cnt = 0;

    for (int drs=0; drs<160; drs++)
    {
        const int p = drs*9*1024;

        double avg = 0;
        for (int i=0; i<1024*9; i++)
            avg += fResult->fOffset[p+i];

        avg /= 1024*9*fResult->fNumOffset;

        if (avg/1000<lvl)
            cnt++;
    }

    return cnt;

}
