#ifndef MARS_MAvalanchePhotoDiode
#define MARS_MAvalanchePhotoDiode

#ifndef ROOT_TH2
#include <TH2.h>
#endif

#ifndef ROOT_TSortedList
#include <TSortedList.h>
#endif

class APD : public TObject  // FIXME: Derive from TH2?
{
    friend class Afterpulse;

private:
    TH2F fHist;

    TSortedList fAfterpulses;   //! List of produced afterpulses

    Float_t fCrosstalkProb;     // Probability that a converted photon creates another one in a neighboring cell
    Float_t fDeadTime;          // Deadtime of a single cell after a hit
    Float_t fRecoveryTime;      // Recoverytime after Deadtime (1-exp(-t/fRecoveryTime)
    Float_t fAfterpulseProb[2]; // Afterpulse probabilities
    Float_t fAfterpulseTau[2];  // Afterpulse time constants

    Float_t fTime;              // A user settable time of the system

    // The implementation of the cell behaviour (crosstalk and afterpulses)
    Float_t HitCellImp(Int_t x, Int_t y, Float_t t=0);

    // Processing of afterpulses
    void GenerateAfterpulse(UInt_t cell, Int_t idx, Double_t charge, Double_t t);
    void ProcessAfterpulses(Float_t time, Float_t dt);
    void DeleteAfterpulses(Float_t time);

public:
    APD(Int_t n, Float_t prob=0, Float_t dt=0, Float_t rt=0);

    // --- Setter and Getter ----

    // Set the afterpulse probability and time-constant of distribution 1 and 2
    void SetAfterpulse1(Double_t p, Double_t tau) { fAfterpulseProb[0]=p; fAfterpulseTau[0]=tau; }
    void SetAfterpulse2(Double_t p, Double_t tau) { fAfterpulseProb[1]=p; fAfterpulseTau[1]=tau; }

    // Set the afterpulse probability for distribution 1 and 2
    void SetAfterpulseProb(Double_t p1, Double_t p2) { fAfterpulseProb[0]=p1; fAfterpulseProb[1]=p2; }

    // Getter functions
    Float_t GetCellContent(Int_t x, Int_t y) const { return fHist.GetBinContent(x, y); }
    Int_t   GetNumCellsX() const { return fHist.GetNbinsX(); }

    Float_t GetCrosstalkProb() const { return fCrosstalkProb; }
    Float_t GetDeadTime() const { return fDeadTime; }
    Float_t GetRecoveryTime() const { return fRecoveryTime; }
    Float_t GetTime() const { return fTime; }

    Float_t GetRelaxationTime(Float_t threshold=0.001) const;

    Float_t GetLastHit() const { return fHist.GetMaximum(); }

    TSortedList &GetListOfAfterpulses() { return fAfterpulses; }

    // Functions for easy production of statistics about the cells
    Int_t CountDeadCells(Float_t t=0) const;
    Int_t CountRecoveringCells(Float_t t=0) const;

    // --- Lower level user interface ---

    // Implementation to hit a specified or random cell
    Float_t HitCell(Int_t x, Int_t y, Float_t t=0);
    Float_t HitRandomCell(Float_t t=0);

    // Functions to produce virgin chips or just effected by constant rates
    void FillEmpty(Float_t t=0);
    void FillRandom(Float_t rate, Float_t t=0);

    // Produce random pulses with the given rate over a time dt.
    // Processes afterpulses until the new time and deletes previous
    // afterpulses.
    Float_t Evolve(Double_t freq, Double_t dt);

    // Delete Afterpulses before fTime. This might be wanted after
    // a call to Evolve or Relax to maintain memeory usage.
    void DeleteAfterpulses() { DeleteAfterpulses(fTime); }

    // --- High level user interface ---

    // This fills a G-APD with a rough estimated state at a given time
    // T=0. It then evolves the time over the ralaxation time. If the
    // chip is not virgin (i.e. fTime<0) the random filling is omitted
    void Init(Float_t rate) { if (fTime<0) FillRandom(rate); Relax(rate); ShiftTime(); }

    // Shifts all times including fTime by dt backwards (adds -dt)
    // This is convenient because you can set the current time (fTime) to 0
    void ShiftTime(Double_t dt);
    void ShiftTime() { ShiftTime(fTime); }

    // Functions producing photons hitting cells. It is meant to add
    // many photons with an arrival time t after fTime. The photons
    // must be sorted in time first to ensure proper treatment of the
    // afterpulses.
    Float_t HitRandomCellRelative(Float_t t=0) { ProcessAfterpulses(fTime, t); return HitRandomCell(fTime+t); }

    // Produce random pulses with a given frequency until the influence
    // of the effects of the G-APD (relaxation time, afterpulses) are
    // below the given threshold. (Calls Evolve())
    // FIXME: Maybe the calculation of the relaxation time could be optimized?
    Float_t Relax(Double_t freq, Float_t threshold=0.001) { return Evolve(freq, GetRelaxationTime(threshold)); }

    // Issue afterpulses until fTime+dt and set fTime to fTime+dt
    // This is needed to create all afterpulses from external pulses
    // and afterpulses until the time fTime+dt. This makes mainly
    // the list of afterpulses complete until fTime+dt
    void IncreaseTime(Float_t dt) { ProcessAfterpulses(fTime, dt); fTime += dt; }

    // TObject
    void Draw(Option_t *o="") { fHist.Draw(o); }
    void DrawCopy(Option_t *o="") { fHist.DrawCopy(o); }

    ClassDef(APD, 1) // An object representing a Geigermode APD
};

class Afterpulse : public TObject
{
private:
    UInt_t  fCellIndex;  // Index of G-APD cell the afterpulse belongs to

    Float_t fTime;       // Time at which the afterpulse avalanch broke through
    Float_t fAmplitude;  // Amplitude (crosstalk!) the pulse produced

    Int_t Compare(const TObject *obj) const
    {
        return static_cast<const Afterpulse*>(obj)->fTime>fTime ? -1 : 1;
    }

    Bool_t IsSortable() const { return kTRUE; }

public:
    Afterpulse(UInt_t idx, Float_t t) : fCellIndex(idx), fTime(t), fAmplitude(0) { }

    UInt_t GetCellIndex() const { return fCellIndex; }

    Float_t GetTime() const { return fTime; }
    Float_t GetAmplitude() const { return fAmplitude; }

    Float_t Process(APD &apd)
    {
        // Do not process afterpulses twice (e.g. HitRelative + IncreaseTime)
        // This should not happen anyway
        //        if (fAmplitude>0)
        //            return fAmplitude;

        const UInt_t nx  = apd.GetNumCellsX()+2;

        const UInt_t x = fCellIndex%nx;
        const UInt_t y = fCellIndex/nx;

        fAmplitude = apd.HitCellImp(x, y, fTime);

        return fAmplitude;
    }
    ClassDef(Afterpulse, 1) // An Afterpulse object
};

#endif
