#ifndef MARS_MMcCorsikaRunHeader
#define MARS_MMcCorsikaRunHeader
///////////////////////////////////////////////////////////////////////
//                                                                   //
// MMcCorsikaRunHeader                                               //
//                                                                   //
///////////////////////////////////////////////////////////////////////
#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

class MGeomCorsikaCT;

class MMcCorsikaRunHeader : public MParContainer
{
private:
    // Header copied directly from CORSIKA header, skiping dummy values.
    // The propouse of this container is being able to cmopare with
    // Corsika run Header
    // See CORSIKA manual for explanations
    Float_t fRunNumber;
    Float_t fDate;
    Float_t fCorsikaVersion;
    Float_t fNumObsLev;
    Float_t fHeightLev[10];
    Float_t fSlopeSpec;  // Slope of primaries' energy spectrum
    Float_t fELowLim;
    Float_t fEUppLim;    // Limits of energy range for generation
    Float_t fEGS4flag;
    Float_t fNKGflag;
    Float_t fEcutoffh;
    Float_t fEcutoffm;
    Float_t fEcutoffe;
    Float_t fEcutoffg;

    // Physical constants and interaction flags (see CORSIKA manual):
    Float_t fC[50];
    Float_t fCKA[40];
    Float_t fCETA[5];
    Float_t fCSTRBA[11];
    Float_t fAATM[5];
    Float_t fBATM[5];
    Float_t fCATM[5];
    Float_t fNFL[4];

    // (degrees) Inner and outer angles in Corsika's VIEWCONE option
    Float_t fViewconeAngles[2]; 
                                  

    Float_t fWobbleMode;     // Indicates wobble mode with which
                             //  reflector has been run 
    Float_t fAtmosphericModel; // Indicates atmospheric model used in
                               // absorption simulation. 0 = no atmosphere,
                               // 1 = atm_90percent, 2 = atm_isothermal,
                               // 3 = atm_corsika.
    UInt_t fNumCT;
    TObjArray fTelescopes;

public:
    MMcCorsikaRunHeader(const char *name=NULL, const char *title=NULL,
			int NumCT=1);


    void Fill(const Float_t  runnumber,
              const Float_t  date,
              const Float_t  vers,
              const Float_t  fNumObsLev,
              const Float_t  height[10],
              const Float_t  slope,
              const Float_t  elow,
              const Float_t  eupp,
              const Float_t  egs4,
              const Float_t  nkg,
              const Float_t  eh,
              const Float_t  em,
              const Float_t  ee,
              const Float_t  eg,
              const Float_t  c[50],
              const Float_t  cka[40],
              const Float_t  ceta[5],
              const Float_t  cstrba[11],
              const Float_t  aatm[5],
              const Float_t  batm[5],
              const Float_t  catm[5],
              const Float_t  nfl[4],
	      const Float_t  viewcone[2],
	      const Float_t  wobble,
	      const Float_t  atmospher
             );

    Float_t GetELowLim() const { return fELowLim; }
    Float_t GetEUppLim() const { return fEUppLim; }
    Float_t GetSlopeSpec() const { return fSlopeSpec; }
    Float_t GetWobbleMode() const { return fWobbleMode; }
    Float_t GetCorsikaVersion() const { return fCorsikaVersion; }
    Float_t GetViewconeAngleInner() const { return fViewconeAngles[0]; }
    Float_t GetViewconeAngleOuter() const { return fViewconeAngles[1]; }
    Float_t GetAtmosphericModel() const { return fAtmosphericModel; }
    Bool_t HasViewCone() const { return fViewconeAngles[1]>0; }

    Int_t GetNumCT() const { return fNumCT; }

    void FillCT(Float_t ctx, Float_t cty, Float_t ctz,
		Float_t cttheta, Float_t ctphi,
		Float_t ctdiam, Float_t ctfocal,
		Int_t CTnum);

    void SetSpectrum(Float_t slope, Float_t emin, Float_t emax)
    {
        fSlopeSpec=slope; fELowLim=emin; fEUppLim=emax;
    }
    void SetViewCone(Float_t inner, Float_t outer)
    {
        fViewconeAngles[0] = inner;
        fViewconeAngles[1] = outer;
    }


    MGeomCorsikaCT &operator[](Int_t i) const;

    virtual void Print(Option_t *opt=NULL) const;

    ClassDef(MMcCorsikaRunHeader, 3) // storage container for corsika setup information
};
#endif



