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
//  MSimReadout
//
// Task to convert the analog channels into a digital signal. This should
// simulate the conversion and saturation bahaviour of the FADC/readout
// system.
//
// You can give a conversion factor from the unitx of your analog signal
// to the units of your adc. This is a fixed factor because it is just
// a matter of what the meaning of an adc count is, nothing which could
// jitter or is a real part of the electronics. Such effects should
// be simulated somewhere else.
//
//
//  Input Containers:
//   MGeomCam
//   MAnalogChannels
//   TriggerPos [MParameterD]
//   IntendedPulsePos [MParameterD]
//   MRawRunHeader
//
//  Output Containers:
//   MRawEvtData
//   MRawEvtHeader
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimReadout.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MArrayI.h"

#include "MParList.h"
#include "MParameters.h"

#include "MGeomCam.h"

#include "MRawRunHeader.h"
#include "MRawEvtHeader.h"
#include "MRawEvtData.h"

#include "MAnalogSignal.h"
#include "MAnalogChannels.h"

ClassImp(MSimReadout);

using namespace std;


// ------------------------------------------------------------------------
//
// Default constructor
//
MSimReadout::MSimReadout(const char* name, const char *title)
    : fRunHeader(0), fEvtHeader(0), fCamera(0), fPulsePos(0), fTrigger(0), fData(0),
    fConversionFactor(1)
{
    fName  = name  ? name  : "MSimReadout";
    fTitle = title ? title : "Task to simulate the analog readout (FADCs)";
}

// ------------------------------------------------------------------------
//
// Look for the needed parameter containers.
// Initialize MRawEvtData from MRawEvtRunHeader.
//
Int_t MSimReadout::PreProcess(MParList *pList)
{
    fCamera = (MAnalogChannels*)pList->FindObject("MAnalogChannels");
    if (!fCamera)
    {
        *fLog << err << "MAnalogChannels not found... aborting." << endl;
        return kFALSE;
    }

    fTrigger = (MParameterD*)pList->FindObject("TriggerPos", "MParameterD");
    if (!fTrigger)
    {
        *fLog << err << "TriggerPos [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }

    fPulsePos = (MParameterD*)pList->FindObject("IntendedPulsePos", "MParameterD");
    if (!fPulsePos)
    {
        *fLog << err << "IntendedPulsePos [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }

    fRunHeader = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
    if (!fRunHeader)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fEvtHeader = (MRawEvtHeader*)pList->FindCreateObj("MRawEvtHeader");
    if (!fEvtHeader)
        return kFALSE;

    fData = (MRawEvtData*)pList->FindCreateObj("MRawEvtData");
    if (!fData)
        return kFALSE;

    return kTRUE;
}

Bool_t MSimReadout::ReInit(MParList *plist)
{
    MGeomCam *cam = (MGeomCam*)plist->FindObject("MGeomCam");
    if (!cam)
    {
        *fLog << err << "MGeomCam not found... aborting." << endl;
        return kFALSE;
    }

    fRunHeader->InitPixels(cam->GetNumPixels());

    fData->InitRead(fRunHeader);
    fData->ResetPixels();
    fData->InitStartCells();
    fData->SetIndices();

    return kTRUE;
}

// ------------------------------------------------------------------------
//
// Convert (digitize) the analog channels into digital (FADC) data.
//
Int_t MSimReadout::Process()
{
    // Sanity checks
    if (fData->GetNumLoGainSamples()>0)
    {
        *fLog << err << "ERROR - MSimReadout: Lo-gains not implemented yet." << endl;
        return kERROR;
    }

    // Make sure that we have not more analog channels than pixels
    // FIXME: Is this really necessary?
    if (fCamera->GetNumChannels()>fData->GetNumPixels())
    {
        *fLog << err;
        *fLog << "ERROR - Number of analog channels " << fCamera->GetNumChannels();
        *fLog << " exceeds number of pixels " << fData->GetNumPixels() << endl;
        return kERROR;
    }

    if (fTrigger->GetVal()<0)
    {
        *fLog << err << "ERROR - MSimReadout: MSimReadout executed for an event which has no trigger." << endl;
        return kERROR;
    }

    // Get the intended pulse position and convert it to slices
    const Float_t pulpos = fPulsePos->GetVal()*fRunHeader->GetFreqSampling()/1000.;

    // Get trigger position and correct for intended pulse position
    const Int_t trig = TMath::CeilNint(fTrigger->GetVal()-pulpos);

    // Check if the position is valid
    if (trig<0)
    {
        *fLog << err;
        *fLog << "ERROR - Trigger position before analog signal." << endl;
        *fLog << "        Trigger:  " << fTrigger->GetVal() << endl;
        *fLog << "        PulsePos: " << pulpos << endl;
        return kERROR;
    }

    // Get Number of samples in analog channels
    const Int_t nsamp = fCamera->GetNumSamples();

    // Get number of samples to be digitized
    const Int_t nslices = fData->GetNumSamples();

    // Check if the whole requested signal can be digitized
    if (trig+nslices>nsamp)
    {
        *fLog << err << "ERROR - Trigger position beyond valid analog signal range." << endl;
        *fLog << "        Trigger:    " << fTrigger->GetVal() << endl;
        *fLog << "        PulsePos:   " << pulpos << endl;
        *fLog << "        SamplesIn:  " << nsamp << endl;
        *fLog << "        SamplesOut: " << nslices << endl;
        return kERROR;
    }

    const Float_t offset    = 0;//128;
    // FTemme: Don't need this anymore:
//    const UInt_t  max       = fData->GetMax();
//    const UInt_t  min       = fData->GetMin();


    // FIXME: Take this into account
//    const UInt_t scale      = 16;
//    const UInt_t resolution = 12;

    // Digitize into a buffer
    MArrayI buffer(nslices*fData->GetNumPixels());

    // Loop over all channels/pixels
    for (UInt_t i=0; i<fCamera->GetNumChannels(); i++)
    {
        // Get i-th canalog hannel
        const MAnalogSignal &sig = (*fCamera)[i];

        // Digitize all slices
        for (Int_t j=0; j<nslices; j++)
        {

            Float_t slice = 0;
            if (j+trig >= (Int_t)sig.GetSize())
            {
                // DN: This, IMHO can never happen, since the check in line 205
                // already took care for this.
                // DN: But I don't understand why Thomas did this?
                //      We need to add noise at least ?!
                slice = offset;
            }
            else
            {
                // normal case

                // Why do we add 'offset' when it is a hardcoded zero?
                // And why do we multiply, while this value *may* be changed
                // by a user, but it should not? Why do we care?
                // Because we can?
                slice = sig[j+trig] * fConversionFactor + offset;
            }


            // Saturation in FACT is done as follows:
            // If the digitized signal is larger than an upper limit 'max'
            // the ADC does set a special bit! The overflow bit ...
            // So while we say we have a 12bit ADC ... in fact we sometimes
            // also use a 13th bit ...
            // but this does not increase our resolution by a factor of 2!

            // There are different binary formats for signed integers,
            // however the 'Two's complement' format
            // http://en.wikipedia.org/wiki/Two%27s_complement
            // is increadibly common, and this is also what is used by FACTs ADCs.

            // A normal 12bit (two's complement formatted) signed integer
            // goes from -2048 to +2047 and is coded like this:
            // from -2048 = 0x800 = 1000.0000.0000
            // to   +2047 = 0x7FF = 0111.1111.1111
            //
            // But on a normal PC we store these 12 bit numbers in a space, that
            // was designed for 16bit numbers. This is no problem for the positive numbers
            // 12bit: 0x7FF = 0111.1111.1111 --> 16bit: 0x07FF = 0000.0111.1111.1111
            // This bit combination is always understood as +2047 .. no problem!
            // But the negative numbers:
            // 12bit: 0x800 = 1000.0000.0000 --> 16bit: 0x0800 = 0000.1000.0000.0000
            // This *would* normally be understood as +2048, because we need to
            // 'enlarge' the 'sign bit'
            // so our largest negative number written into a 16bit storage space
            // should look like this:
            // 0xF800 = 1111.1000.0000.0000  --> -2048

            // The enlargement of the sign bit is autotically done on the FACT FAD
            // board already before the data is send to any PC, because we can do it
            // damn fast on that board, and a PC would need to touch every incoming
            // data word again....
            // But now since we have enlarged the 12th bit ... the sign bit into
            // the space of bits 13,14,15 and 16 ... where did the overflow-bit go?
            //
            //  Well .. we still have plenty of bit-combinations, which are normally
            // forbidden for a 12bit ADC, and these we can use to encode both,
            // the positive and negative overflow.
            // we decided to do this:
            // positive overflow
            // 0000.1000.0000.0000 --> interpreted by a PC as +2048 and thus out of 12 bit range!
            // negative underflow
            // 1111.0111.1111.1111 --> interpreted by a PC as -2049 and thus out of 12 bit range!

            // we will simulate exactly the same behaviour here!

            // max and min can be set, by the user currently ..
            // but I don't see why this should be possible.
            Int_t digitized_value = TMath::Nint(slice);
            if (digitized_value > 2047) // positive overflow
                buffer[nslices*i + j] = 0x0800; // <-- +2048
            else if (digitized_value < -2048)
                buffer[nslices*i + j] = 0xF7FF; // <-- -2049
            else
                buffer[nslices*i + j] = digitized_value;
        }
    }

    // Set samples as raw-data
    fData->Set(buffer);
    fData->SetReadyToSave();

    // Set the trigger/daq event number
    fEvtHeader->SetDAQEvtNumber(GetNumExecutions());
    fEvtHeader->SetReadyToSave();

    // FIMXE: This will never be stored correctly :(
    fRunHeader->SetNumEvents(fRunHeader->GetNumEvents()+1);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read the parameters from the resource file.
//
//  ConversionFactor: 1
//
Int_t MSimReadout::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "ConversionFactor", print))
    {
        rc = kTRUE;
        fConversionFactor = GetEnvValue(env, prefix, "ConversionFactor", fConversionFactor);
    }

    return rc;
}
