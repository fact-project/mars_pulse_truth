#ifndef MARS_MMcEvt
#define MARS_MMcEvt

#ifndef MARS_MMcEvtBasic
#include "MMcEvtBasic.h"
#endif


class MMcEvt : public MMcEvtBasic
{
private:
    UInt_t  fEvtNumber;          // [fits: name=EventNum ; unit=uint32]
    Float_t fThick0;             // [g/cm2]
    Float_t fFirstTarget;        // []
    Float_t fZFirstInteraction;  // [cm]

    Float_t fCoreD;              // [cm] Core d pos
    Float_t fCoreX;              // [cm] Core x pos
    Float_t fCoreY;              // [cm] Core y pos

    // Up to here, the info from the CORSIKA event header.

    // Time of first and last photon:
    Float_t fTimeFirst;          // [ns]
    Float_t fTimeLast;           // [ns]

    // 6 parameters and chi2 of the NKG fit to the longitudinal
    // particle distribution. See CORSIKA manual for explanation,
    // section 4.42 "Longitudinal shower development":
    //
    Float_t fLongiNmax;          // [particles]
    Float_t fLongit0;            // [g/cm2]
    Float_t fLongitmax;          // [g/cm2]
    Float_t fLongia;             // [g/cm2]
    Float_t fLongib;             // []
    Float_t fLongic;             // [cm2/g]
    Float_t fLongichi2;

    UInt_t fPhotIni;             // [ph] Initial number of photons
    UInt_t fPassPhotAtm;         // [ph] Passed atmosphere
    UInt_t fPassPhotRef;         // [ph] Passed reflector(reflectivity + effective area)
    UInt_t fPassPhotCone;        // [ph]  Within any valid pixel, before plexiglas
    UInt_t fPhotElfromShower;    // [phe] Passed qe, coming from the shower
    UInt_t fPhotElinCamera;      // [phe] usPhotElfromShower + mean of phe from NSB

    // Now follow the fraction of photons reaching the camera produced by
    // electrons, muons and other particles respectively:

    Float_t  fElecCphFraction;
    Float_t  fMuonCphFraction;
    Float_t  fOtherCphFraction;

    Float_t  fFadcTimeJitter;
    
    Int_t    fEventReuse; // Number running from 0 to N-1, being N the number
                          // of times a Corsika event has been reused, by
                          // orienting the telescope in different ways or by
                          // setting it at a different location on the ground.

public:
    MMcEvt();
    MMcEvt(UInt_t, ParticleId_t, Float_t, Float_t, Float_t,
           Float_t, Float_t, Float_t, Float_t, Float_t, Float_t,
           Float_t, Float_t, Float_t, Float_t, Float_t, Float_t,
           Float_t, Float_t, Float_t, Float_t, Float_t, Float_t,
           UInt_t, UInt_t, UInt_t, UInt_t, UInt_t, UInt_t,
           Float_t, Float_t, Float_t, Float_t, Int_t ireuse=0) ;

    // Getter
    UInt_t  GetEvtNumber() const { return fEvtNumber; }  //Get Event Number

    Float_t GetCoreX() const { return fCoreX; }          //Get Core x pos
    Float_t GetCoreY() const { return fCoreY; }          //Get Core y pos

    UInt_t  GetPhotIni() const { return fPhotIni; }           //Get Initial photons
    UInt_t  GetPassPhotAtm() const { return fPassPhotAtm;}    //Get Passed atmosphere
    UInt_t  GetPassPhotRef() const { return fPassPhotRef; }   //Get Passed reflector
    UInt_t  GetPassPhotCone() const { return fPassPhotCone; } //Get Passed glas
    UInt_t  GetPhotElfromShower() const { return fPhotElfromShower; }   //Get Passed qe from shower
    UInt_t  GetPhotElinCamera() const { return fPhotElinCamera; }       //Get Passed qe total
    Float_t GetZFirstInteraction() const { return fZFirstInteraction; }

    Float_t GetOtherCphFraction() const { return fOtherCphFraction; }

    Float_t GetLongiNmax() const { return fLongiNmax; }
    Float_t GetLongia()    const { return fLongia; }
    Float_t GetLongib()    const { return fLongib; }
    Float_t GetLongic()    const { return fLongic; }
    Float_t GetLongichi2() const { return fLongichi2; }
    Float_t GetLongit0()   const { return fLongit0; }
    Float_t GetLongitmax() const { return fLongitmax; }

    Float_t GetFadcTimeJitter() const { return fFadcTimeJitter; }

    Float_t GetMuonCphFraction() const { return fMuonCphFraction; }

    TString GetDescription(const TString &s="") const;

    // Setter
    void SetCoreD(Float_t CoreD) { fCoreD=CoreD; }                //Set Core d pos
    void SetCoreX(Float_t CoreX) { fCoreX=CoreX; }                //Set Core x pos
    void SetCoreY(Float_t CoreY) { fCoreY=CoreY; }                //Set Core y pos

    void SetEvtNumber(UInt_t n) { fEvtNumber=n; }
    void SetEventReuse(UInt_t n) { fEventReuse=n; }
    void SetPhotElfromShower(UInt_t n) { fPhotElfromShower=n; }

    void Fill( UInt_t, ParticleId_t, Float_t, Float_t, Float_t,
               Float_t, Float_t, Float_t, Float_t, Float_t, Float_t,
               Float_t, Float_t, Float_t, Float_t, Float_t, Float_t,
               Float_t, Float_t, Float_t, Float_t, Float_t, Float_t,
               UInt_t, UInt_t, UInt_t, UInt_t, UInt_t, UInt_t,
               Float_t, Float_t, Float_t, Float_t, Int_t ireuse=0);

    // MParContainer
    Bool_t SetupFits(fits &fin);

    // TObject
    void Print(Option_t *opt=NULL) const;
    void Clear(Option_t *opt=NULL);

    ClassDef(MMcEvt, 7)  //Stores Montecarlo Information of one event (eg. the energy)
};

#endif
