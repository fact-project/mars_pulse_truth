#ifndef MARS_MMcRunHeader
#define MARS_MMcRunHeader

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

// -------------------------------------------------------------
//
//  The following data member are in use:
//
//    fCorsikaVersion        MHCollectionArea
//    fReflVersion           MSrcPosCalc
//    fCamVersion            MMcCalibrationUpdate, MReadMarsFile,
//                           MMcPedestalCopy, MMcPedestalNSBAdd
//    fStarField*            MMcBadPixelSet
//    fImpactMax             MHCollectionArea
//    fNumSimulatedShowers   MHCollectionArea
//    [fAllEvtsTriggered]    MHCollectionArea
//    fNumPheFromDNSB        MMcPedestalNSBAdd
//
// -------------------------------------------------------------

class MMcRunHeader : public MParContainer
{
private:
  UInt_t  fNumTrigCond;     // Number of trigger conditions in this file,
  // zero means single condition mode
  
  //-- 0 means: MMcTrig
  //-- 1 means: MMcTrig;1
  //-- 2 means: MMcTrig;1 MMcTrig;2
  
  Byte_t  fAllEvtsTriggered;  // boolean that indicates is all images are 
                              // saved or only the ones that trigger

  Byte_t  fMcEvt;           // McEvt stored or not
  Byte_t  fMcTrig;          // McTrig stored or not
  Byte_t  fMcFadc;          // McFadc stored or not

  Int_t   fNumAnalisedPixels;  // Number of analised pixels
  
  UInt_t  fNumSimulatedShowers; // Number of showers that were simualted
  UInt_t  fNumStoredShowers;    // Number of store showers in this run
  UInt_t  fNumEvents;           // Number of events in this root file
  
  //  Coordinates of the Starfield
  
  Int_t fStarFieldRaH;
  Int_t fStarFieldRaM;
  Int_t fStarFieldRaS;
  Int_t fStarFieldDeD;
  Int_t fStarFieldDeM;
  Int_t fStarFieldDeS;

  Float_t fNumPheFromDNSB;  // Number of phe/ns from diffuse NSB
  
  //  Angular range used in the Corsika showers generation (degrees)
  Float_t fShowerThetaMax; // [deg]
  Float_t fShowerThetaMin; // [deg]
  Float_t fShowerPhiMax; // [deg] See note in class description.
  Float_t fShowerPhiMin; // [deg] See note in class description.

  //  Maximum impact parameter in the input rfl file
  Float_t fImpactMax; // [cm] 

  UShort_t fCorsikaVersion;
  UShort_t fReflVersion;
  UShort_t fCamVersion;

  // ---- Run Header Informations ----
  Float_t fMcRunNumber;     // Run Number
  UInt_t  fProductionSite;  // code to know where the run was generated
  Float_t fDateRunMMCs;     // Date of the MMCs production
  Float_t fDateRunCamera;   // Date, when the Camera program is run.

  Byte_t  fRawEvt;          // RawEvt[Data,Hedaer] stored or not

  Byte_t  fElecNoise;       // Electronic Noise simulated or not
  Byte_t  fStarFieldRotate; // Is the starfield rotation 
                            // switched on (1) or off (0)

  //  Wavelength limits for the Cerenkov photons
  Float_t fCWaveLower;
  Float_t fCWaveUpper;

  //  Observation levels  
  UInt_t    fNumObsLev;
  Float_t   fHeightLev[10]; 

  //  Spectral index
  Float_t   fSlopeSpec;

  // Noise from Optic Links
  Byte_t  fOpticLinksNoise;  // Noise from Optic Links simualted or not.

  // Semiaperture of the cone around the direction of the primary within which the
  // orientation of the telescope axis is scattered (deg). This is used in reflector
  // simulation to re-use each Corsika event more than once.
  Float_t fRandomPointingConeSemiAngle;


public:
  MMcRunHeader(const char *name=NULL, const char *title=NULL);

  void Fill(const Float_t  runnumber,
	    const UInt_t   productionsite,
	    const Float_t  daterunMMCs,
	    const Float_t   daterunCamera,
	    const UInt_t   numtrigcond,
	    const Byte_t   allevts,
	    const Byte_t   mcevt,
	    const Byte_t   mctrig,
	    const Byte_t   mcfadc,
	    const Byte_t   rawevt,
	    const Byte_t   elecnoise, 
	    const Int_t    numanalpixels,
	    const UInt_t   numsim,
	    const UInt_t   numsto,
	    const Byte_t   starfieldrotate,
	    const Int_t    sfRaH,
	    const Int_t    sfRaM,
	    const Int_t    sfRaS,
	    const Int_t    sfDeD,
	    const Int_t    sfDeM,
	    const Int_t    sfDeS,
	    const Float_t  numdnsb,
	    const Float_t  shthetamax,
	    const Float_t  shthetamin,
	    const Float_t  shphimax,
	    const Float_t  shphimin,
	    const Float_t  impactmax,
	    const Float_t  cwavelower,
	    const Float_t  cwaveupper,
	    const Float_t  slopespec,
	    const UInt_t   num0bslev,
	    const Float_t  heightlev[10],
	    const UInt_t   corsika,                
	    const UInt_t   refl,
	    const UInt_t   cam,
            const Byte_t   opticnoise,
            const Float_t  conesmiangle=0
	    );
  
  void     GetStarFieldRa(Int_t *hour, Int_t *minute, Int_t *second) const;
  void     GetStarFieldDec(Int_t *degree, Int_t *minute, Int_t *second) const;

  Float_t  GetNumPheFromDNSB() const      { return fNumPheFromDNSB; }
  UShort_t GetCamVersion() const          { return fCamVersion; }
  UShort_t GetReflVersion() const         { return fReflVersion; }
  UInt_t   GetNumSimulatedShowers() const { return fNumSimulatedShowers; }
  UInt_t   GetCorsikaVersion() const      { return fCorsikaVersion; }
  Bool_t   GetAllEvtsTriggered() const    { return fAllEvtsTriggered ? kTRUE : kFALSE; }

  Float_t  GetShowerThetaMin() const { return fShowerThetaMin; } 
  Float_t  GetShowerThetaMax() const { return fShowerThetaMax; } 

  Float_t  GetShowerPhiMin() const { return fShowerPhiMin; }
  Float_t  GetShowerPhiMax() const { return fShowerPhiMax; }

  Float_t GetImpactMax() const            {return fImpactMax;}

  Bool_t IsCeres() const { return fCamVersion==UShort_t(-1); }

  void SetNumSimulatedShowers(UInt_t n) { fNumSimulatedShowers=n; }
  void SetImpactMax(Float_t im) { fImpactMax=im; }
  void SetCorsikaVersion(UInt_t v) { fCorsikaVersion=v; }

  ClassDef(MMcRunHeader, 7)	// storage container for general run info
};

#endif
