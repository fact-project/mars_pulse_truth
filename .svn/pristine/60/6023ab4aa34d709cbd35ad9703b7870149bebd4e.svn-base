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
//  MSimTrigger
//
// This task takes the pure analog channels and simulates a trigger
// electronics.
//
// In a first step several channels can be summed together by a look-up table
// fRouteAC.
//
// In a second step from these analog channels the output of a discriminator
// is calculated using a threshold and optional a fixed digital signal length.
//
// The signal length of the digital signal emitted by the discriminator
// can either be bound to the time the signal is above the threshold
// defined by fDiscriminatorThreshold if fDigitalSignalLength<0 or set to a
// fixed length (fDigitalSignalLength>0).
//
// With a second look-up table fCoincidenceMap the analog channels are
// checked for coincidences. The coincidence must at least be of the length
// defined by fCoincidenceTime. The earliest coincide is then stored as
// trigger position.
//
// If a minimum multiplicity m is given, m signals above threshold
// in the coincidence patterns are enough to emit a trigger signal.
//
//
// For MAGIC1:
//  - fDigitalSignalLength between 6ns and 12ns
//  - fCoincidenceTime between 0.25ns to 1ns
//
//
//  Input Containers:
//   IntendedPulsePos [MParameterD]
//   MAnalogChannels
//   MRawRunHeader
//
//  Output Containers:
//   TriggerPos [MParameterD]
//   MRawEvtHeader
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimTrigger.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"
#include "MParameters.h"

#include "MLut.h"
#include "MArrayI.h"

#include "MRawEvtHeader.h"
#include "MRawRunHeader.h"

#include "MAnalogSignal.h"
#include "MAnalogChannels.h"
#include "MDigitalSignal.h"

#include "MTriggerPattern.h"

#include "MPedestalCam.h"
#include "MPedestalPix.h"

ClassImp(MSimTrigger);

using namespace std;

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimTrigger::MSimTrigger(const char *name, const char *title)
    : fCamera(0),
      fPulsePos(0),
      fTrigger(0),
      fRunHeader(0),
      fEvtHeader(0),
      fElectronicNoise(0),
      fGain(0),
      fDiscriminatorThreshold(-1),
      fDigitalSignalLength(8),
      fCoincidenceTime(0.5),
      fShiftBaseline(kTRUE),
      fUngainSignal(kTRUE),
      fSimulateElectronics(kTRUE),
      fMinMultiplicity(-1),
      fCableDelay(21),
      fCableDamping(0.)     // default Damping Set to zero, so users, who do not set
                            // the CableDamoing in the ceres.rc do not see a difference.

{
    fName  = name  ? name  : "MSimTrigger";
    fTitle = title ? title : "Task to simulate trigger electronics";
}

// --------------------------------------------------------------------------
//
// Take two TObjArrays with a collection of digital signals.
// Every signal from one array is compared with any from the other array.
// For all signals which overlap and which have an overlap time >gate
// a new digital signal is created storing start time and length of overlap.
// They are collected in a newly allocated TObjArray. A pointer to this array
// is returned.
//
// The user gains owenership of the object, i.e., the user is responsible of
// deleting the memory.
//
TObjArray *MSimTrigger::CalcCoincidence(const TObjArray &arr1, const TObjArray &arr2/*, Float_t gate*/) const
{
    TObjArray *res = new TObjArray;

    if (arr1.GetEntriesFast()==0 || arr2.GetEntriesFast()==0)
        return res;

    TIter Next1(&arr1);
    MDigitalSignal *ttl1 = 0;
    while ((ttl1=static_cast<MDigitalSignal*>(Next1())))
    {
        TIter Next2(&arr2);
        MDigitalSignal *ttl2 = 0;
        while ((ttl2=static_cast<MDigitalSignal*>(Next2())))
        {
            MDigitalSignal *ttl = new MDigitalSignal(*ttl1, *ttl2);
            /*
            if (ttl->GetLength()<=gate)
            {
                delete ttl;
                continue;
            }
            */
            res->Add(ttl);
        }
    }

    res->SetOwner();

    return res;
}

class Edge : public TObject
{
private:
    Double_t fEdge;
    Int_t    fRising;

public:
    Edge(Double_t t, Int_t rising) : fEdge(t), fRising(rising) { }
    Bool_t IsSortable() const { return kTRUE; }
    Int_t Compare(const TObject *o) const { const Edge *e = static_cast<const Edge*>(o); if (e->fEdge<fEdge) return 1; if (e->fEdge>fEdge) return -1; return 0; }

    Int_t IsRising() const { return fRising; }
    Double_t GetEdge() const { return fEdge; }
};

// --------------------------------------------------------------------------
//
// Calculate a multiplicity trigger on the given array(s). The idx-array
// conatins all channels which should be checked for coincidences
// and the ttls array conatins the arrays with the digital signals.
//
// For the windows in which more or euqal than threshold channels have
// a high signal a new MDigitalSignal is created.  newly allocated
// array with a collection of these trigger signals is returned.
//
TObjArray *MSimTrigger::CalcMinMultiplicity(const MArrayI &idx, const TObjArray &ttls, Int_t threshold) const
{
    // Create a new array for the rising and falling edges of the signals
    TObjArray times;
    times.SetOwner();

    // Fill the array with edges from all digital signals of all our channels
    for (UInt_t k=0; k<idx.GetSize(); k++)
    {
        TObjArray *arr = static_cast<TObjArray*>(ttls[idx[k]]);

        TIter Next(arr);
        MDigitalSignal *ttl = 0;
        while ((ttl=static_cast<MDigitalSignal*>(Next())))
        {
            times.Add(new Edge(ttl->GetStart(),  1));
            times.Add(new Edge(ttl->GetEnd(),   -1));
        }
    }

    // Sort them in time
    times.Sort();

    // Start with no channel active
    Int_t lvl = 0;

    TObjArray *res = new TObjArray;
    res->SetOwner();

    // First remove all edges which do not change the status
    // "below threshold" or "above threshold"
    for (int i=0; i<times.GetEntriesFast(); i++)
    {
        // Get i-th edge
        const Edge &e = *static_cast<Edge*>(times.UncheckedAt(i));

        // Claculate what the number of active channels after the edge is
        const Int_t lvl1 = lvl + e.IsRising();

        // Remove edge if number of active channels before and after the
        // edge lower is lower than the threshold or higher than
        // the threshold
        if (lvl+1<threshold || lvl-1>=threshold)
            delete times.RemoveAt(i);

        // keep the (now) "previous" level
        lvl = lvl1<0 ? 0 : lvl1;
    }

    // Remove the empty slots from the array
    times.Compress();

    //
    for (int i=0; i<times.GetEntriesFast()-1; i++)
    {
        // get the current edge
        const Edge &e0 = *static_cast<Edge*>(times.UncheckedAt(i));

        // go ahead if this is a falling edge
        if (e0.IsRising()!=1)
            continue;

        // get the following edge (must be a falling edge now)
        const Edge &e1 = *static_cast<Edge*>(times.UncheckedAt(i+1));

        // calculate the length of the digital signal
        const Double_t len = e1.GetEdge()-e0.GetEdge();

        // Create a digital trigger signal
        MDigitalSignal *ds = new MDigitalSignal(e0.GetEdge(), len);
        //ds->SetIndex(lvl);
        res->Add(ds);
    }

    return res;
}

// --------------------------------------------------------------------------
//
// Check for the necessary parameter containers. Read the luts.
//
Int_t MSimTrigger::PreProcess(MParList *pList)
{
    fTrigger = (MParameterD*)pList->FindCreateObj("MParameterD", "TriggerPos");
    if (!fTrigger)
        return kFALSE;

    fPulsePos = (MParameterD*)pList->FindObject("IntendedPulsePos", "MParameterD");
    if (!fPulsePos)
    {
        *fLog << err << "IntendedPulsePos [MParameterD] not found... aborting." << endl;
        return kFALSE;
    }

    fCamera = (MAnalogChannels*)pList->FindObject("MAnalogChannels");
    if (!fCamera)
    {
        *fLog << err << "MAnalogChannels not found... aborting." << endl;
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

    if (!fSimulateElectronics)
    {
        *fLog << inf << "Simulation of electronics switched off... first photon will trigger." << endl;
        return kTRUE;
    }

    fElectronicNoise = 0;
    if (fShiftBaseline)
    {
        fElectronicNoise = (MPedestalCam*)pList->FindObject("ElectronicNoise", "MPedestalCam");
        if (!fElectronicNoise)
        {
            *fLog << err << "ElectronicNoise [MPedestalCam] not found... aborting." << endl;
            return kFALSE;
        }
        *fLog << inf << "Baseline will be shifted back to 0 for discriminator." << endl;
    }

    fGain = 0;
    if (fUngainSignal)
    {
        fGain = (MPedestalCam*)pList->FindObject("Gain", "MPedestalCam");
        if (!fGain)
        {
            *fLog << err << "Gain [MPedestalCam] not found... aborting." << endl;
            return kFALSE;
        }
        *fLog << inf << "Discriminator will be multiplied by applied gain." << endl;
    }

    fRouteAC.Delete();
    if (!fNameRouteAC.IsNull() && fRouteAC.ReadFile(fNameRouteAC)<0)
        return kFALSE;

    fCoincidenceMap.Delete();
    if (!fNameCoincidenceMap.IsNull() && fCoincidenceMap.ReadFile(fNameCoincidenceMap)<0)
        return kFALSE;

    // ---------------- Consistency checks ----------------------

    if (!fRouteAC.IsEmpty() && !fCoincidenceMap.IsEmpty() &&
        fCoincidenceMap.GetMaxIndex()>fRouteAC.GetNumRows()-1)
    {
        *fLog << err;
        *fLog << "ERROR - AC routing produces " << fRouteAC.GetNumRows() << " analog channels," << endl;
        *fLog << "        but the coincidence map expects at least " << fCoincidenceMap.GetMaxIndex()+1 << " channels." << endl;
        return kERROR;
    }

//    if (fDiscriminatorThreshold<=0)
//    {
//        *fLog << err << "ERROR - Discriminator threshold " << fDiscriminatorThreshold << " invalid." << endl;
//        return kFALSE;
//    }

    if (fElectronicNoise && !fRouteAC.IsEmpty() && !fRouteAC.IsDefaultCol())
    {
        // FIXME: Apply to analog channels when summing
        *fLog << warn << "WARNING - A baseline shift doesn't make sense for sum-channels... reset." << endl;
        fElectronicNoise = 0;
    }

    if (fGain && !fRouteAC.IsEmpty() && !fRouteAC.IsDefaultCol())
    {
        // FIXME: Apply to analog channels when summing
        *fLog << warn << "WARNING - Ungain doesn't make sense for sum-channels... reset." << endl;
        fGain = 0;
    }


    // ---------------- Information output ----------------------

    *fLog << inf;

    if (fRouteAC.IsEmpty())
        *fLog << "Re-routing/summing of analog channels before discriminator switched off." << endl;
    else
        *fLog << "Using " << fNameRouteAC << " for re-routing/summing of analog channels before discriminator." << endl;

    if (fCoincidenceMap.IsEmpty() && fMinMultiplicity<=0)
        *fLog << "No coincidences of digital channels will be checked. Signal-above-threshold trigger applied." << endl;
    else
    {
        *fLog << "Using ";
        if (fCoincidenceMap.IsEmpty())
            *fLog << "the whole camera";
        else
            *fLog << "patterns from " << fNameCoincidenceMap;
        *fLog << " to check for ";
        if (fMinMultiplicity>0)
            *fLog << fMinMultiplicity << " multiplicity." << endl;
        else
            *fLog << "coincidences of the digital channels." << endl;
        }

    *fLog << "Using discriminator threshold of " << fDiscriminatorThreshold << endl;

    *fLog << "Using fCableDelay " << fCableDelay << endl;
    *fLog << "Using fCableDamping " << fCableDamping << endl;

    ////////////////////////////////
    // open some output files for debugging
//    patch_file.open("/home/fact_opr/patch_file.csv", ios_base::out);
//    clipped_file.open("/home/fact_opr/clipped_file.csv", ios_base::out);
//    digital_file.open("/home/fact_opr/digital_file.csv", ios_base::out);
//    ratescan_file.open("/home/fact_opr/ratescan_file.csv", ios_base::out);



    return kTRUE;
}

/*
class MDigitalChannel : public TObjArray
{
private:
    TObjArray fArray;

public:
    MDigitalSignal *GetSignal(UInt_t i) { return static_cast<MDigitalSignal*>(fArray[i]); }

};
*/

#include "MCamEvent.h"
class MTriggerSignal : public MParContainer, public MCamEvent
{
private:
    TObjArray fSignals;

public:
    MTriggerSignal() { fSignals.SetOwner(); }

    void Add(MDigitalSignal *signal) { fSignals.Add(signal); }

    MDigitalSignal *GetSignal(UInt_t i) { return static_cast<MDigitalSignal*>(fSignals[i]); }

    void Sort() { fSignals.Sort(); }

    Int_t GetNumSignals() const { return fSignals.GetEntriesFast(); }

    Float_t GetFirstTrigger() const
    {
        MDigitalSignal *sig = static_cast<MDigitalSignal*>(fSignals[0]);
        return sig ? sig->GetStart() : -1;
    }
    Int_t GetFirstIndex() const
    {
        MDigitalSignal *sig = static_cast<MDigitalSignal*>(fSignals[0]);
        return sig ? sig->GetIndex() : -1;
    }
    Bool_t GetPixelContent(Double_t&, Int_t, const MGeomCam&, Int_t) const
    {
        switch (1)
        {
        case 1: // yes/no
        case 2: // first time
        case 3: // length
        case 4: // n
            break;
        }

        return kTRUE;
    }
    void DrawPixelContent(Int_t) const
    {
    }
};


void MSimTrigger::SetTrigger(Double_t pos, Int_t idx)
{
    // FIXME: Jitter! (Own class?)
    fTrigger->SetVal(pos);
    fTrigger->SetReadyToSave();

    // Flag this event as triggered by the lvl1 trigger (FIXME?)
    fEvtHeader->SetTriggerPattern(pos<0 ? 0 : 4);
    fEvtHeader->SetNumTrigLvl1((UInt_t)idx);
    fEvtHeader->SetReadyToSave();
}

// --------------------------------------------------------------------------
//
Int_t MSimTrigger::Process()
{
    // Invalidate trigger
    //fTrigger->SetVal(-1);
    // Calculate the minimum and maximum time for a valid trigger
    const Double_t freq    = fRunHeader->GetFreqSampling()/1000.;
    const Float_t  nsamp   = fRunHeader->GetNumSamplesHiGain();
    const Float_t  pulspos = fPulsePos->GetVal()/freq;

    // Valid range in units of bins
    const Float_t min = fCamera->GetValidRangeMin()+pulspos;
    const Float_t max = fCamera->GetValidRangeMax()-(nsamp-pulspos);

    if (!fSimulateElectronics)
    {
        SetTrigger(min, -1);
        return kTRUE;
    }

    // ================== Simulate channel bundling ====================

    // FIXME: Before we can bundle the channels we have to make a copy
    //        and simulate clipping

    // Check if routing should be done
    const Bool_t empty = fRouteAC.IsEmpty();

    // If no channels are summed the number of patches stays the same
    const UInt_t npatch = empty ? fCamera->GetNumChannels() : fRouteAC.GetEntriesFast();

    // Use the given analog channels as default out. If channels are
    // summed overwrite with a newly allocated set of analog channels
    MAnalogChannels *patches = fCamera;
    MAnalogChannels *raw_patches = fCamera;
    if (!empty)
    {
        // FIXME: Can we add gain and offset here into a new container?

        patches = new MAnalogChannels(npatch, fCamera->GetNumSamples());
        raw_patches = new MAnalogChannels(npatch, fCamera->GetNumSamples());
        for (UInt_t patch_id=0; patch_id<npatch; patch_id++)
        {
            const MArrayI &row = fRouteAC.GetRow(patch_id);
            for (UInt_t pxl_in_patch=0; pxl_in_patch<row.GetSize(); pxl_in_patch++)
            {
                const UInt_t pixel_id = row[pxl_in_patch];

                // FIXME: Shrinking the mapping table earlier (e.g.
                //        ReInit) would avoid a lot of if's
                if (pixel_id<fCamera->GetNumChannels())
                {
                    (*raw_patches)[patch_id].AddSignal((*fCamera)[pixel_id]);
                    (*patches)[patch_id].AddSignal((*fCamera)[pixel_id]);
                    (*patches)[patch_id].AddSignal((*fCamera)[pixel_id], fCableDelay, fCableDamping);
                }
            }
        }
    }

    // DN: 20140219 Ratescan:
    //
    //
//    for (UInt_t patch_id=0; patch_id<npatch; patch_id++)
//    {
//        MAnalogSignal current_patch = (*raw_patches)[patch_id];
//        float max = current_patch[0];
//        for (UInt_t i=1; i<current_patch.GetSize(); i++)
//        {
//            if (current_patch[i] > max)
//            {
//                max = current_patch[i];
//            }
//        }
//        ratescan_file << max << " ";
//    }
//    ratescan_file << endl;

//    // DN 20131108: DEBUGGING:
//    for (UInt_t patch_id=0; patch_id<npatch; patch_id++)
//    {
//        for (UInt_t slice=0; slice<fCamera->GetNumSamples(); slice++)
//        {
//            patch_file << (*raw_patches)[patch_id][slice] << "\t";
//            clipped_file << (*patches)[patch_id][slice] << "\t";
//        }
//        patch_file << endl;
//        clipped_file << endl;
//    }



    // FIXME: Write patches

    // ================== Simulate discriminators ====================

    TObjArray ttls(npatch);
    ttls.SetOwner();

    for (UInt_t i=0; i<npatch; i++)
    {
        // FIXME: What if the gain was also allpied to the baseline?
        const Double_t offset = fElectronicNoise ? (*fElectronicNoise)[i].GetPedestal() : 0;
        const Double_t gain   = fGain            ? (*fGain)[i].GetPedestal()            : 1;
        ttls.AddAt(
                    (*patches)[i].Discriminate(
                        fDiscriminatorThreshold*gain+offset,                // treshold
                        Double_t(fCableDelay),                              // start
                        Double_t(fCamera->GetNumSamples() - fCableDelay),   // end
                        //fDigitalSignalLength                              // time-over-threshold, or fixed-length?
                        -1                                                  // -1 = time-over-threshold
                        ),
                    i
                    );
    }

    // FIXME: Write TTLs!

    // If analog channels had been newly allocated free memmory
    if (patches!=fCamera)
        delete patches;
    if (raw_patches!=fCamera)
        delete raw_patches;

    // =================== Simulate coincidences ======================

    // If the map is empty we create a one-pixel-coincidence map
    // FIMXE: This could maybe be accelerated if the Clone can be
    //        omitted in the loop
    if (fCoincidenceMap.IsEmpty())
    {
        if (fMinMultiplicity>0)
            fCoincidenceMap.SetDefaultRow(npatch);
        else
            fCoincidenceMap.SetDefaultCol(npatch);
    }

    // Create an array for the individual triggers
    MTriggerSignal triggers;

    Int_t cnt  = 0;
    Int_t rmlo = 0;
    Int_t rmhi = 0;

//    cout << "MSimTrigger::fMinMultiplicity = " << fMinMultiplicity << endl;
//    cout << "MSimTrigger::fCoincidenceTime = " << fCoincidenceTime << endl;
//    cout << "fCoincidenceMap.GetEntries() = " << fCoincidenceMap.GetEntries() << endl;
//    cout << "MSimTrigger::fCableDelay = " << fCableDelay << endl;
//    cout << "MSimTrigger::fCableDamping = " << fCableDamping << endl;
//    cout << "min:" << min << endl;
//    cout << "max:" << max << endl;

    for (int j=0; j<fCoincidenceMap.GetEntries(); j++)
    {
        const MArrayI &idx = fCoincidenceMap.GetRow(j);

        TObjArray *arr = 0;

        if (fMinMultiplicity>0)
        {
            arr = CalcMinMultiplicity(idx, ttls, fMinMultiplicity);
        }
        else
        {
            arr = CalcMinMultiplicity(idx, ttls, idx.GetSize());
            /*
            // Start with a copy of the first coincidence channel
            arr = static_cast<TObjArray*>(ttls[idx[0]]->Clone());
            arr->SetOwner();

            // compare to all other channels in this coincidence patch, one by one
            for (UInt_t k=1; k<idx.GetSize() && arr->GetEntriesFast()>0; k++)
            {
                TObjArray *res = CalcCoincidence(*arr, *static_cast<TObjArray*>(ttls[idx[k]]));//, fCoincidenceTime);

                // Delete the original array and keep the new one
                delete arr;
                arr = res;
            }*/
        }

        // Count the number of totally emitted coincidence signals
        cnt += arr->GetEntriesFast();

        // Remove all signals which are not in the valid digitization range
        // (This is not the digitization window, but the region in which
        //  the analog channels contain usefull data)
        // and which are shorter than the defined coincidence gate.
        TIter Next(arr);
        MDigitalSignal *ttl = 0;
        while ((ttl=static_cast<MDigitalSignal*>(Next())))
        {
            if (ttl->GetLength()<fCoincidenceTime)
            {
                delete arr->Remove(ttl);
                continue;
            }

            if (ttl->GetStart()<min)
            {
                delete arr->Remove(ttl);
                rmlo++;
                continue;
            }
            if (ttl->GetStart()>max)
            {
                delete arr->Remove(ttl);
                rmhi++;
                continue;
            }

            // Set trigger-channel index to keep this information
            //ttl->SetIndex(j);
        }

        // Remove the empty slots
        arr->Compress();

//        cout << "ttls(j=" << j << "):";
//        TObjArray *arr2 = static_cast<TObjArray*>(ttls[j]);
//        TIter Nexty(arr);
//        MDigitalSignal *ttly = 0;
//        while ((ttly=static_cast<MDigitalSignal*>(Nexty())))
//        {
//            cout << "|"<< ttly->GetStart() << ", " << ttly->GetLength();
//        }
//        cout << endl;


        // If we have at least one trigger keep the earliest one.
        // FIXME: The triggers might be ordered in time automatically:
        //        To be checked!
        // FIXME: Simulate trigger dead-time!
        if (arr->GetEntriesFast()>0)
        {
            ttl = static_cast<MDigitalSignal*>(arr->RemoveAt(0));
            // Set trigger-channel index to keep this information
            ttl->SetIndex(j);
            triggers.Add(ttl);
        }

        // delete the allocated space
        delete arr;
    }

    // There are usually not enough entries that it is worth to search
    // for the earliest instead of just sorting and taking the first one
    //  FIXME: This could be improved if checking for IsSortable
    //         is omitted
    triggers.Sort();
    // FIXME: Store triggers! (+ Reversed pixels?)

    SetTrigger(triggers.GetFirstTrigger(), triggers.GetFirstIndex());

    // No trigger issued. Go on.
    if (triggers.GetNumSignals()==0)
    {
        if (rmlo>0 || rmhi>0)
            *fLog << inf2 << GetNumExecutions() << ": " << rmlo << "/" << rmhi << " trigger out of valid range. No trigger raised." << endl;
        return kTRUE;
    }

    // Number of patches which have triggered out of the total number of
    // Coincidence signals emitted. (If the total number is higher than
    // the number of triggers either some triggers had to be removed or
    // or a patch has emitted more than one trigger signal)
    // FIXME: inf2?
    *fLog << inf << GetNumExecutions() << ": ";
    *fLog << setw(3) << triggers.GetNumSignals() << " triggers left out of ";
    *fLog << setw(3) << cnt << " (" << rmlo << "/" << rmhi << " trigger out of valid range), T=" << fTrigger->GetVal();
    *fLog << endl;

    //# Trigger characteristics: gate length (ns), min. overlapping time (ns),
    //# amplitude and FWHM of (gaussian) single phe response for trigger:
    //trigger_prop 3.0 0.25 1.0 2.0

    return kTRUE;
}

Int_t MSimTrigger::PostProcess()
{
//    patch_file.close();
//    clipped_file.close();
//    digital_file.close();
//    ratescan_file.close();
    return kTRUE;
}


// --------------------------------------------------------------------------
//
// FileNameRouteac:         routeac.txt
// FileNameCoincidenceMap:  coincidence.txt
// DiscriminatorTheshold:   3.5
// DigitalSignalLength:     8
// CoincidenceTime:         0.5
// SimulateElectronics:     Yes
//
Int_t MSimTrigger::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "FileNameRouteAC", print))
    {
        rc = kTRUE;
        fNameRouteAC = GetEnvValue(env, prefix, "FileNameRouteAC", fNameRouteAC);
    }

    if (IsEnvDefined(env, prefix, "FileNameCoincidenceMap", print))
    {
        rc = kTRUE;
        fNameCoincidenceMap = GetEnvValue(env, prefix, "FileNameCoincidenceMap", fNameCoincidenceMap);
    }

    if (IsEnvDefined(env, prefix, "DiscriminatorThreshold", print))
    {
        rc = kTRUE;
        fDiscriminatorThreshold = GetEnvValue(env, prefix, "DiscriminatorThreshold", fDiscriminatorThreshold);
    }

    if (IsEnvDefined(env, prefix, "DigitalSignalLength", print))
    {
        rc = kTRUE;
        fDigitalSignalLength = GetEnvValue(env, prefix, "DigitalSignalLength", fDigitalSignalLength);
    }

    if (IsEnvDefined(env, prefix, "CoincidenceTime", print))
    {
        rc = kTRUE;
        fCoincidenceTime = GetEnvValue(env, prefix, "CoincidenceTime", fCoincidenceTime);
    }

    if (IsEnvDefined(env, prefix, "SimulateElectronics", print))
    {
        rc = kTRUE;
        fSimulateElectronics = GetEnvValue(env, prefix, "SimulateElectronics", fSimulateElectronics);
    }

    if (IsEnvDefined(env, prefix, "MinMultiplicity", print))
    {
        rc = kTRUE;
        fMinMultiplicity = GetEnvValue(env, prefix, "MinMultiplicity", fMinMultiplicity);
    }

    if (IsEnvDefined(env, prefix, "CableDelay", print))
    {
        rc = kTRUE;
        fCableDelay = GetEnvValue(env, prefix, "CableDelay", fCableDelay);
    }

    if (IsEnvDefined(env, prefix, "CableDamping", print))
    {
        rc = kTRUE;
        fCableDamping = GetEnvValue(env, prefix, "CableDamping", fCableDamping);
    }



    return rc;
}
