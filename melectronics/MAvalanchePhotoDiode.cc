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
//  APD
//
// All times in this class are relative times. Therefor the unit for the
// time is not intrinsically fixed. In fact the dead-time and recovery-
// time given in the constructor must have the same units. This is what
// defines the unit of the times given in the function and the unit of
// rates given.
// For example, if recovery and dead time are given in nanoseconds the
// all times must be in nanoseconds and rates are given per nanosecond,
// i.e. GHz.
//
//  Hamamatsu 30x30 cells:  APD(30, 0.2, 3, 35)
//  Hamamatsu 60x60 cells:  APD(60, 0.2, 3, 8.75)
//
//
// The implementation of afterpulsing is based on
//    A.Du, F.Retiere
//    After-pulsing and cross-talk in multi-pixel photon counters
//    NIM A, Volume 596, Issue 3, p. 396-401
//
//
// Example:
//
//    APD apd(ncells, crosstalk, deadtime, recovery);
//    apd.SetAfterpulseProb(0.14, 0.11);
//
//    while (1)
//    {
//        // Make the chip "empty" from the influence of external photons
//        // It also sets fTime to 0.
//        apd.Init(freq); // freq of external noise (eg. nsb)
//
//        // Now call this function for each external photon you have. The
//        // times are relative to the the time you get by apd.GetTime()
//        // set automatically after the call to apd.Init().
//        for (int i=0; i<nphot; i++)
//            apd.HitRandomCellRelative(dt);
//
//        [...]
//
//        // Process and produce afterpulses until a time, wrt to GetTime(),
//        // given by the end of your simulated window. Note that this
//        // does not produce noise. This must have been properly simulated
//        // up to this time already.
//        apd.IncreaseTime(dtend);
//
//        // Now you can excess the afterpulses by
//        TIter Next(&a->GetListOfAfterpulses());
//        Afterpulse *ap = 0;
//        while ((ap=static_cast<Afterpulse*>(Next())))
//        {
//           if (apd.GetTime()>=dtend)
//              continue;
//
//           cout << "Amplitude:    " << ap->GetAmplitude() << endl;
//           cout << "Arrival Time: " << ap->GetTime()      << endl;
//        }
//     }
//
//
//////////////////////////////////////////////////////////////////////////////
#include "MAvalanchePhotoDiode.h"

#include <TRandom.h>

#include "MMath.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(APD);

using namespace std;

/*
class MyProfile : public TProfile2D
{
public:
    void AddBinEntry(Int_t cell) { fBinEntries.fArray[cell]++; }
};
*/

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
//    n     is the number od cells in x or y. The APD is assumed to
//          be square.
//    prob  is the crosstalk probability, i.e., the probability that a
//          photon which produced an avalanche will create another
//          photon in a neighboring cell
//    dt    is the deadtime, i.e., the time in which the APD cell will show
//          no response to a photon after a hit
//    rt    is the recovering tims, i.e. the exponential (e^(-dt/rt))
//          with which the cell is recovering after being dead
//
//    prob, dt and ar can be set to 0 to switch the effect off.
//    0 is also the dfeault for all three.
//
APD::APD(Int_t n, Float_t prob, Float_t dt, Float_t rt)
    : fHist("APD", "", n, 0.5, n+0.5, n, 0.5, n+0.5),
    fCrosstalkProb(prob), fDeadTime(dt), fRecoveryTime(rt),
    fTime(-1)
{
    fHist.SetDirectory(0);

    fAfterpulses.SetOwner();

    fAfterpulseProb[0] = 0;
    fAfterpulseProb[1] = 0;

    fAfterpulseTau[0] = 15;
    fAfterpulseTau[1] = 85;
}

// --------------------------------------------------------------------------
//
// This is the time a chips needs after an external signal to relax to
// a "virgin" state, i.e. without no influence of the external pulse
// above the given threshold.
//
// It takes into account the dead time of the cell, the relaxation time
// and the two afterpulse distributions. However, in most cases the
// afterpulse distribution will dominate (except they are switched off by
// a zero probability).
//
// FIXME: Maybe the calculation of the relaxation time could be optimized?
//
Float_t APD::GetRelaxationTime(Float_t threshold) const
{
    // Calculate time until the probability of one of these
    // events falls below the threshold probability.
    const Double_t rt0 = - TMath::Log(threshold)*fRecoveryTime;
    const Double_t rt1 = fAfterpulseProb[0]>0 ? -TMath::Log(threshold/fAfterpulseProb[0])*fAfterpulseTau[0] : 0;
    const Double_t rt2 = fAfterpulseProb[1]>0 ? -TMath::Log(threshold/fAfterpulseProb[1])*fAfterpulseTau[1] : 0;

    // Probability not between t and inf, but between t and t+dt
    // -tau * log ( p / ( 1 - exp(- dt/tau) ) ) = t

    return fDeadTime + TMath::Max(rt0, TMath::Max(rt1, rt2));
}

// --------------------------------------------------------------------------
//
// This is the recursive implementation of a hit. If a photon hits a cell
// at x and y (must be a valid cell!) at time t, at first we check if the
// cell is still dead. If it is not dead we calculate the signal height
// from the recovery time. Now we check with the crosstalk probability
// whether another photon is created. If another photon is created we
// calculate randomly which of the four neighbor cells are hit.
// If the cell is outside the APD the photon is ignored. As many
// new photons are created until our random number is below the crosstak-
// probability.
//
// For each photon the possible afterpulses of two distributions are
// created and added to the list of afterpulses. This is done by calling
// GenerateAfterpulse for the two afterpulse-distributions.
//
// The total height of the signal (in units of photons) is returned.
// Note, that this can be a fractional number.
//
// This function looks a bit fancy accessing the histogram and works around
// a few histogram functions. This is a speed optimization which works
// around a lot of sanity checks which are obsolete in our case.
//
// The default time is 0.
//
Float_t APD::HitCellImp(Int_t x, Int_t y, Float_t t)
{
    //        if (x<1 || x>fHist.GetNbinsX() ||
    //            y<1 || y>fHist.GetNbinsY())
    //            return 0;
#ifdef DEBUG
    cout << "Hit: " << t << endl;
#endif

    // Number of the x/y cell in the one dimensional array
    // const Int_t cell = fHist.GetBin(x, y);
    const Int_t cell = x + (fHist.GetNbinsX()+2)*y;

    // Getting a reference to the float is the fastes way to
    // access the bin-contents in fArray
    Float_t &cont = fHist.GetArray()[cell];

    // Calculate the time since the last breakdown
    // const Double_t dt = t-fHist.GetBinContent(x, y)-fDeadTime; //
    const Float_t dt = t-cont-fDeadTime;

    // Photons within the dead time are just ignored
    if (/*hx.GetBinContent(x,y)>0 &&*/ dt<=0)
    {
#ifdef DEBUG
        cout << "Dead: " << t << " " << cont << " " << dt << endl;
#endif
        return 0;
    }
    // The signal height (in units of one photon) produced after dead time
    // depends on the recovery of the cell - described by an exponential.
    const Float_t weight = fRecoveryTime<=0 ? 1. : 1-TMath::Exp(-dt/fRecoveryTime);

    // Now we know the charge in the cell and we can generate
    // the afterpulses with both time-constants
    GenerateAfterpulse(cell, 0, weight, t);
    GenerateAfterpulse(cell, 1, weight, t);

    // The probability that the cell emits a photon causing crosstalk
    // scales as the signal height.
    const Float_t prob = weight*fCrosstalkProb;

    // Set the contents to the time of the last breakdown (now)
    cont = t; // fHist.SetBinContent(x, y, t)

    // Counter for the numbers of produced photons
    Float_t n = weight;

    // Get random number of emitted and possible converted crosstalk photons
    const UInt_t rndm = gRandom->Poisson(prob);

    for (UInt_t i=0; i<rndm; i++)
    {
        // Get a random neighbor which is hit.
        switch (gRandom->Integer(4))
        {
        case 0: if (x<fHist.GetNbinsX()) n += HitCellImp(x+1, y, t); break;
        case 1: if (x>1)                 n += HitCellImp(x-1, y, t); break;
        case 2: if (y<fHist.GetNbinsY()) n += HitCellImp(x, y+1, t); break;
        case 3: if (y>1)                 n += HitCellImp(x, y-1, t); break;
        }
    }

    return n;
}

// --------------------------------------------------------------------------
//
// Check if x and y is a valid cell. If not return 0, otherwise
// HitCelImp(x, y, t). HitCellImp generates Crosstalk and Afterpulses.
//
// The default time is 0.
//
Float_t APD::HitCell(Int_t x, Int_t y, Float_t t)
{
    if (x<1 || x>fHist.GetNbinsX() ||
        y<1 || y>fHist.GetNbinsY())
        return 0;

    return HitCellImp(x, y, t);
}

// --------------------------------------------------------------------------
//
// Determine randomly (uniformly) a cell which was hit. Return
// HitCellImp for this cell and the given time. HitCellImp
// generates Crosstalk and Afterpulses
//
// The default time is 0.
//
// If you want t w.r.t. fTime use HitRandomCellRelative istead.
//
Float_t APD::HitRandomCell(Float_t t)
{
    const UInt_t nx  = fHist.GetNbinsX();
    const UInt_t ny  = fHist.GetNbinsY();

    const UInt_t idx = gRandom->Integer(nx*ny);

    const UInt_t x   = idx%nx;
    const UInt_t y   = idx/nx;

    return HitCellImp(x+1, y+1, t);
}

// --------------------------------------------------------------------------
//
// Sets all cells with a contents which is well before the time t such that
// the chip is "virgin". Therefore all cells are set to a time which
// is twice the deadtime before the given time and 1000 times the recovery
// time.
//
// The afterpulse list is deleted.
//
// If deadtime and recovery time are 0 then t-1 is set.
//
// Sets fTime to t
//
// The default time is 0.
//
void APD::FillEmpty(Float_t t)
{
    const Int_t n = (fHist.GetNbinsX()+2)*(fHist.GetNbinsY()+2);

    const Double_t tm = fDeadTime<=0 && fRecoveryTime<=0 ? t-1 : t-2*fDeadTime-1000*fRecoveryTime;

    for (int i=0; i<n; i++)
        fHist.GetArray()[i] = tm;

    fHist.SetEntries(1);

    fAfterpulses.Delete();

    fTime = t;
}

// --------------------------------------------------------------------------
//
// First call FillEmpty for the given time t. Then fill each cell by
// by calling HitCellImp with time t-gRandom->Exp(n/rate) with n being
// the total number of cells. This the time at which the cell was last hit.
//
// Sets fTime to t
//
// If the argument t is omitted it defaults to 0.
//
// Since after calling this function the chip should reflect the
// status at the new time fTime=t, all afterpulses are processed
// until this time. However, the produced random pulses might have produced
// new new afterpulses.
//
// All afterpulses before the new timestamp are deleted.
//
// WARNING: Note that this might not correctly reproduce afterpulses
//          produced by earlier pulese.
//
void APD::FillRandom(Float_t rate, Float_t t)
{
    FillEmpty(t);

    // If the rate is 0, we don't need to initiatize the cells, because there
    // won't be any hitted cells.
    if (rate > 0.)
    {

        const Int_t nx = fHist.GetNbinsX();
        const Int_t ny = fHist.GetNbinsY();

        const Double_t f = (nx*ny)/rate;

        // FIXME: Dead time is not taken into account,
        //        possible earlier afterpulses are not produced.

        for (int x=1; x<=nx; x++)
            for (int y=1; y<=ny; y++)
                HitCellImp(x, y, t-MMath::RndmExp(f));

    }

    // Deleting of the afterpulses before fHist.GetMinimum() won't
    // speed things because we have to loop over them once in any case

    ProcessAfterpulses(fHist.GetMinimum(), t);
    DeleteAfterpulses(t);

    fTime = t;
}

// --------------------------------------------------------------------------
//
// Shift all times including fTime to dt (ie. add -dt to all times)
// This allows to set a user-defined T0 or shift T0 to fTime=0.
//
// However, T0<0 is not allowed (dt cannot be greater than fTime)
//
void APD::ShiftTime(Double_t dt)
{
    if (dt>fTime)
    {
        gLog << warn << "APD::ShiftTime: WARNING - requested time shift results in fTime<0... ignored." << endl;
        return;
    }

    // If reset was requested shift all times by end backwards
    // so that fTime is now 0
    const Int_t n = (fHist.GetNbinsX()+2)*(fHist.GetNbinsY()+2);
    for (int i=0; i<n; i++)
        fHist.GetArray()[i] -= dt;

    fTime -= dt;
}

// --------------------------------------------------------------------------
//
// Evolve the chip from fTime to fTime+dt if it with a given frequency
// freq. Returns the total signal "recorded".
//
// fTime is set to the fTime+dt.
//
// If you want to evolve over a default relaxation time (relax the chip
// from a signal) use Relax instead.
//
// Since after calling this function the chip should reflect the
// status at the new time fTime=fTime+dt, all afterpulses are processed
// until this time. However, evolving the chip until this time might
// have produced new afterpulses.
//
// All afterpulses before the new timestamp are deleted.
//
Float_t APD::Evolve(Double_t freq, Double_t dt)
{
    const Double_t end = fTime+dt;

    Float_t hits = 0;
 
    if (freq>0)
    {
        const Double_t avglen = 1./freq;

        Double_t time = fTime;
        while (1)
        {
            const Double_t deltat = MMath::RndmExp(avglen);
            if (time+deltat>end)
                break;

            time += deltat;

            hits += HitRandomCell(time);
        }
    }

    // Deleting of the afterpulses before fTime won't speed things
    // because we have to loop over them once in any case

    ProcessAfterpulses(fTime, dt);
    DeleteAfterpulses(end);

    fTime = end;

    return hits;
}

// --------------------------------------------------------------------------
//
// Retunrs the number of cells which have a time t<=fDeadTime, i.e. which are
// dead.
// The default time is 0.
//
// Note that if you want to get a correct measure of teh number of dead cells
// at the time t, this function will only produce a valid count if the
// afterpulses have been processed up to this time.
//
Int_t APD::CountDeadCells(Float_t t) const
{
    const Int_t nx = fHist.GetNbinsX();
    const Int_t ny = fHist.GetNbinsY();

    Int_t n=0;
    for (int x=1; x<=nx; x++)
        for (int y=1; y<=ny; y++)
            if ((t-fHist.GetBinContent(x, y))<=fDeadTime)
                n++;

    return n;
}

// --------------------------------------------------------------------------
//
// Returs the number of cells which have a time t<=fDeadTime+fRecoveryTime.
// The default time is 0.
//
// Note that if you want to get a correct measure of teh number of dead cells
// at the time t, this function will only produce a valid count if the
// afterpulses have been processed up to this time.
//
Int_t APD::CountRecoveringCells(Float_t t) const
{
    const Int_t nx = fHist.GetNbinsX();
    const Int_t ny = fHist.GetNbinsY();

    Int_t n=0;
    for (int x=1; x<=nx; x++)
        for (int y=1; y<=ny; y++)
        {
            Float_t dt = t-fHist.GetBinContent(x, y);
            if (dt>fDeadTime && dt<=fDeadTime+fRecoveryTime)
                n++;
        }
    return n;
}

// --------------------------------------------------------------------------
//
// Generate an afterpulse originating from the given cell and a pulse with
// charge. The afterpulse distribution to use is specified by
// the index. The "current" time to which the afterpulse delay refers must
// be given by t.
//
// A generated Afterpulse is added to the list of afterpulses
//
void APD::GenerateAfterpulse(UInt_t cell, Int_t idx, Double_t charge, Double_t t)
{
    // The cell had a single avalanche with signal height weight.
    // This cell now can produce an afterpulse photon/avalanche.
    const Double_t p = gRandom->Uniform();

    // It's probability scales with the charge of the pulse
    if (p>charge*fAfterpulseProb[idx])
        return;

    // Afterpulses come with a well defined time-constant
    // after the normal pulse
    const Double_t dt = MMath::RndmExp(fAfterpulseTau[idx]);

    fAfterpulses.Add(new Afterpulse(cell, t+dt));

#ifdef DEBUG
    cout << "Add : " << t << " + " << dt << " = " << t+dt << endl;
#endif
}

// --------------------------------------------------------------------------
//
// Process afterpulses between time and time+dt. All afterpulses in the list
// before t=time are ignored. All afterpulses between t=time and
// t=time+dt are processed through HitCellImp. Afterpulses after and
// equal t=time+dt are skipped.
//
// Since the afterpulse list is a sorted list newly generated afterpulses
// are always inserted into the list behind the current entry. Consequently,
// afterpulses generated by afterpulses will also be processed correctly.
//
// Afterpulses with zero amplitude are deleted from the list. All other after
// pulses remain in the list for later evaluation.
//
void APD::ProcessAfterpulses(Float_t time, Float_t dt)
{
#ifdef DEBUG
    cout << "Process afterpulses from " << time << " to " << time+dt << endl;
#endif

    const Float_t end = time+dt;

    TObjLink *lnk = fAfterpulses.FirstLink();
    while (lnk)
    {
        Afterpulse &ap = *static_cast<Afterpulse*>(lnk->GetObject());

        // Skip afterpulses which have been processed already
        // or which we do not have to process anymore
        if (ap.GetTime()<time || ap.GetAmplitude()>0)
        {
            lnk = lnk->Next();
            continue;
        }

        // No afterpulses left in correct time window
        if (ap.GetTime()>=end)
           break;

        // Process afterpulse through HitCellImp
        const Float_t ampl = ap.Process(*this);

        // Step to the next entry already, the current one might get deleted
        lnk = lnk->Next();

        if (ampl!=0)
            continue;

#ifdef DEBUG
        cout << "Del : " << ap.GetTime() << " (" << ampl << ")" << endl;
#endif

        // The afterpulse "took place" within the dead time of the
        // pixel ==> No afterpulse, no crosstalk.
        delete fAfterpulses.Remove(&ap);
    }
}

// --------------------------------------------------------------------------
//
// Delete all afterpulses before and equal to t=time from the list
//
void APD::DeleteAfterpulses(Float_t time)
{
    TIter Next(&fAfterpulses);

    Afterpulse *ap = 0;

    while ((ap = static_cast<Afterpulse*>(Next())))
    {
        if (ap->GetTime()>=time)
            break;

        delete fAfterpulses.Remove(ap);
    }
}
