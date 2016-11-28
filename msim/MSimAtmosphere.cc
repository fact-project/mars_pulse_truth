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
//  MSimAtmosphere
//
//  Task to calculate wavelength or incident angle dependent absorption
//
//  Input Containers:
//   MPhotonEvent
//   MCorsikaRunHeader
//
//  Output Containers:
//   MPhotonEvent
//
//////////////////////////////////////////////////////////////////////////////
#include "MSimAtmosphere.h"

#include <fstream>

#include <TGraph.h>
#include <TRandom.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MCorsikaRunHeader.h"
#include "MPhotonEvent.h"
#include "MPhotonData.h"

ClassImp(MSimAtmosphere);

using namespace std;

// ==========================================================================
//
// January 2002, A. Moralejo: We now precalculate the slant paths for the
// aerosol and Ozone vertical profiles, and then do an interpolation in
// wavelength for every photon to get the optical depths. The parameters
// used, defined below, have been taken from "Atmospheric Optics", by
// L. Elterman and R.B. Toolin, chapter 7 of the "Handbook of geophysics
// and Space environments". (S.L. Valley, editor). McGraw-Hill, NY 1965.
// 
// WARNING: the Mie scattering and the Ozone absorption are implemented
// to work only with photons produced at a height a.s.l larger than the
// observation level. So this is not expected to work well for simulating
// the telescope pointing at theta > 90 deg (for instance for neutrino
// studies. Rayleigh scattering works even for light coming from below.
// 
// Fixed bugs (of small influence) in Mie absorption implementation: there
// were errors in the optical depths table, as well as a confusion:
// height a.s.l. was used as if it was height above the telescope level.
// The latter error was also present in the Ozone aborption implementation.
// 
// On the other hand, now we have the tables AE_ABI and OZ_ABI with optical
// depths between sea level and a height h (before it was between 2km a.s.l
// and a height h). So that we can simulate also in the future a different
// observation level.
// 
// AM: WARNING: IF VERY LARGE zenith angle simulations are to be done (say
// above 85 degrees, for neutrino primaries or any other purpose) this code
// will have to be adapted accordingly and checked, since in principle it has
// been written and tested to simulate the absorption of Cherenkov photons
// arriving at the telescope from above.
// 
// AM: WARNING 2: not to be used for wavelengths outside the range 250-700 nm
// 
// January 2003, Abelardo Moralejo: found error in Ozone absorption treatment.
// At large zenith angles, the air mass used was the one calculated for
// Rayleigh scattering, but since the Ozone distribution is rather different
// from the global distribution of air molecules, this is not a good
// approximation. Now I have left in this code only the Rayleigh scattering,
// and moved to atm.c the Mie scattering and Ozone absorption calculated in
// a better way.
// 
// A. Moralejo, January 2003: added some parameters for Mie scattering
// and Ozone absorption derived from the clear standard atmosphere model
// in "Atmospheric Optics", by L. Elterman and R.B. Toolin, chapter 7 of
// the "Handbook of geophysics and Space environments". S.L. Valley,
// editor. McGraw-Hill, NY 1965.
// 
// AM, Jan 2003: Changed the meaning of the argument height: now it is the
// true height above sea level at which a photon has been emitted, before
// it was the height given by Corsika, measured in the vertical of the
// observer and not in the vertical of the emitting particle.
//
//
// MAGIC-Winter and MAGIC-Summer by M. Haffke,
// parametrizing profiles obtained with MSIS:
// http://uap-www.nrl.navy.mil/models_web/msis/msis_home.htm
//
//
// The MAGIC-Winter and MAGIC-Summer parametrisations reproduce the MSIS
// profiles for the 3 atmospheric layers from 0 up to 40 km height. Beyond
// that height, it was not possible to achieve a good fit, but the amount
// of residual atmosphere is so small that light absorption would be
// negligible anyway. Showers develop well below 40 km.
//
//
// The mass overburden is given by T = AATM + BATM * exp(-h/CATM)
// The last layer of the US standard atmosphere (in which T varies
// linearly with h) is above 100 km and has not been included here
// because it should not matter.
//
class MAtmRayleigh
{
private:
    static const Double_t fgMeanFreePath; // [g/cm^2] Mean free path for scattering Rayleigh XR

    Double_t fR;         // [cm] Earth radius to be used

    Double_t fHeight[5]; // Layer boundaries (atmospheric layer)

    // Parameters of the different atmospheres. We use the same parametrization
    // shape as in Corsika atmospheric models (see Corsika manual, appendix D).
    // The values here can be/are obtained from the Corsika output
    //Float_t  fAtmA[4];   // The index refers to the atmospheric layer (starting from sea level and going upwards)
    Float_t  fAtmB[4];   // The index refers to the atmospheric layer (starting from sea level and going upwards)
    Float_t  fAtmC[4];   // The index refers to the atmospheric layer (starting from sea level and going upwards)

    Double_t fRho[5];    // Precalculated integrals for rayleigh scatterning

    // --------------------------------------------------------------------------
    //
    // Precalcalculate the integrals from the observer level to the next
    // atmpsheric layer below including the lower boundary. Thus a
    // correct calculation is reduced to the calculation of the upper
    // boundary.
    //
    // fRho[0] = B0;
    // fRho[1] = B0-A0 + B1;
    // fRho[2] = B0-A0 + B1-A1 + B2;
    // fRho[3] = B0-A0 + B1-A1 + B2+A2 + B3;
    // fRho[4] = B0-A0 + B1-A1 + B2+A2 + B3 - A3;
    //
    void PreCalcRho()
    {
        // Limits (height in cm) of the four layers in which
        // atmosphere is parametrized.
        // This is a stupid trick giving 0 for the integrals below
        // the observer

        // FIXME: Could be replaced by 0, AtmLay[0]-fAtmLay[3]

        const Double_t h[5] =
        {
            fObsLevel,                     // fObsLevel,                   //   0km
            TMath::Max(fObsLevel, 7.75e5), // TMath::Max(fObsLevel, 4e5),  //   4km
             16.5e5,                       //  10e5,                       //  10km
             50.0e5,                       //  40e5,                       //  40km
            105.0e5,                       // 100e5                        // 100km
        };

        memcpy(fHeight, h, sizeof(Double_t)*5);

        fRho[0] = 0;
        for (int i=0; i<4; i++)
        {
            const Double_t b = fAtmB[i];
            const Double_t c = fAtmC[i];

            const Double_t h1 = h[i+1];
            const Double_t h0 = h[i];

            const Double_t B = b*TMath::Exp(-h0/c);
            const Double_t A = b*TMath::Exp(-h1/c);

            // Calculate rho for the i-th layer from the lower
            // to the higher layer boundary.
            // If height is within the layer only calculate up to height.
            fRho[i]  += B;
            fRho[i+1] = fRho[i] - A;
        }
    }

protected:
    Double_t fObsLevel; // [cm] observation level a.s.l.

public:
    // Init an atmosphere from the data stored in MCorsikaRunHeader
    MAtmRayleigh(const MCorsikaRunHeader &h)
    {
        Init(h);
    }

    // Defualt constructor
    MAtmRayleigh() : fObsLevel(-1) { }

    // Check if the ovservation level has been correctly initialized
    // Used as a marker for correct initialization
    Bool_t IsValid() const { return fObsLevel>=0; }

    // Get the Earth radius to be used
    Double_t R() const { return fR; }

    // Init an atmosphere from the data stored in MCorsikaRunHeader
    // This initialized fObsLevel, fR, fAtmB and fAtmC and
    // PreCalcRho
    void Init(const MCorsikaRunHeader &h)
    {
        // Observation level above earth radius
        fObsLevel = h.GetObsLevel();

        // Use earth radius as defined in Corsika
        fR = h.EarthRadius();

        //memcpy(fAtmA, (Float_t*)h.GetAtmosphericCoeffA(), sizeof(Float_t)*4);
        memcpy(fAtmB, (Float_t*)h.GetAtmosphericCoeffB(), sizeof(Float_t)*4);
        memcpy(fAtmC, (Float_t*)h.GetAtmosphericCoeffC(), sizeof(Float_t)*4);

        PreCalcRho();
    }

    // Return the vertical thickness between the observer and height.
    // Therefor the integral of the layers below (including the lower
    // boudary) Have been precalculated by PreCalcRho
    Double_t GetVerticalThickness(Double_t height) const
    {
        // FIXME: We could store the start point above obs-level
        //        (Does this really gain anything?)
        Int_t i=0;
        while (i<4 && height>fHeight[i+1])
            i++;

        const Double_t b = fAtmB[i];
        const Double_t c = fAtmC[i];

        return fRho[i] - b*TMath::Exp(-height/c);
    }

    /*
     // The "orginal" code for the vertical thickness
    Double_t GetVerticalThickness(Double_t obslev, Double_t height) const
    {
        // This is a C++-version of the original code from attenu.c

        // Limits (height in cm) of the four layers in which atmosphere is parametrized:
        const double lahg[5] =
        {
            obslev,
            TMath::Max(obslev, 4e5),
            1.0e6,
            4.0e6,
            1.0e7
        };

        Double_t Rho_Tot = 0.0;
        for (int i=0; i<4; i++)
        {
            const Double_t b = fAtmB[i];
            const Double_t c = fAtmC[i];

            const Double_t h0 = TMath::Min(height, lahg[i+1]);
            const Double_t h1 =                    lahg[i];

            // Calculate rho for the i-th layer from the lower
            // to the higher layer boundary.
            // If height is within the layer only calculate up to height.
            Rho_Tot += b*(exp(-h1/c) - exp(-h0/c));

            if (lahg[i+1] > height)
                break;
        }

        return Rho_Tot;
    }
    */
    Double_t CalcTransmission(Double_t height, Double_t wavelength, Double_t sin2) const
    {
        // sin2: sin(theta)^2
        // height is the true height a.s.l.

        // LARGE ZENITH ANGLE FACTOR (AIR MASS FACTOR):
        // Air mass factor "airmass" calculated using a one-exponential
        // density profile for the atmosphere,
        //
        //     rho = rho_0 exp(-height/hscale) with hscale = 7.4 km
        //
        // The air mass factor is defined as I(theta)/I(0), the ratio of
        // the optical paths I (in g/cm2) traversed between two given
        // heights, at theta and at 0 deg z.a.

        const Double_t H = height-fObsLevel;
        const Double_t h = 2*H;

        // Scale-height (cm) for Exponential density profile
        const Double_t hscale = 7.4e5;
        const Double_t f      = 2*hscale;

        // Precalc R*cos(theta)^2 (FIXME: Is ph.GetCosW2 more precise?)
        const Double_t Rcos2 = fR * (1-sin2); // cos2 = 1 - sin2

        const Double_t x1 = TMath::Sqrt((Rcos2      ) / f);
        const Double_t x2 = TMath::Sqrt((Rcos2 + 2*h) / f);
        const Double_t x3 = TMath::Sqrt((fR         ) / f);
        const Double_t x4 = TMath::Sqrt((fR    + 2*h) / f);

        // Return a -1 transmittance in the case the photon comes
        // exactly from the observation level, because in that case the
        // "air mass factor" would become infinity and the calculation
        // is not valid!
        if (fabs(x3-x4) < 1.e-10)
            return  -1.;

        const Double_t e12 = erfc(x1) - erfc(x2);
        const Double_t e34 = erfc(x3) - erfc(x4);

        const Double_t airmass = TMath::Exp(-fR*sin2 / f) * e12/e34;

        // Calculate the traversed "vertical thickness" of air using the
        // US Standard atmosphere:
        const Double_t Rho_tot = GetVerticalThickness(/*fObsLevel,*/ height);

        // We now convert from "vertical thickness" to "slanted thickness"
        // traversed by the photon on its way to the telescope, simply
        // multiplying by the air mass factor m:
        const Double_t Rho_Fi = airmass * Rho_tot;

        // Finally we calculate the transmission coefficient for the Rayleigh
        // scattering:
        // AM Dec 2002, introduced ABS below to account (in the future) for
        // possible photons coming from below the observation level.

        const Double_t wl = 400./wavelength;

        // Mean free path for scattering Rayleigh XR (g/cm^2)
        return TMath::Exp(-TMath::Abs(Rho_Fi/fgMeanFreePath)*wl*wl*wl*wl);
    }
};

// ==========================================================================

class MAtmosphere : public MAtmRayleigh
{
private:
    static const Double_t STEPTHETA; // aprox. 1 degree

    // Aerosol number density for 31 heights a.s.l., from 0 to 30 km,
    // in 1 km steps (units: cm^-3)
    static const Double_t aero_n[31];

    // Ozone concentration for 51 heights a.s.l., from 0 to 50 km,
    // in 1 km steps (units: cm/km)
    static const Double_t oz_conc[51];

    // aerosol_path contains the path integrals for the aerosol number
    // density (relative to the number density at sea level) between the
    // observation level and a height h for different zenith angles. The
    // first index indicate height above sea level in units of 100m, the
    // second is the zenith angle in degrees.
    float aerosol_path[301][90];

    // ozone_path contains the path integrals for the ozone concentration
    // between the observation level and a height h for different zenith
    // angles. The first index indicate height above sea level in units
    // of 100m, the second is the zenith angle in degrees.
    float ozone_path[501][90];

    // Interpolate the graph at wavelength
    Double_t GetBeta(Double_t wavelength, const TGraph &g) const
    {
        // FIXME: This might not be the fastest because range
        // checks are done for each call!
        return g.GetN()==0 ? 0 : g.Eval(wavelength)*1e-5; // from km^-1 to cm^-1
/*
        // Linear interpolation
        // (FIXME: Is it faster to be replaced with a binary search?)
        // (       This might be faster because we have more photons
        //         with smaller wavelengths)
        //int index;
        //for (index = 1; index <g.GetN()-1; index++)
        //    if (wavelength < g.GetX()[index])
        //        break;
        const Int_t index = TMath::BinarySearch(g.GetN(), g.GetX(), wavelength)+1;

        const Double_t t0 = g.GetY()[index-1];
        const Double_t t1 = g.GetY()[index];

        const Double_t w0 = g.GetX()[index-1];
        const Double_t w1 = g.GetX()[index];

        const Double_t beta0 = t0+(t1-t0)*(wavelength-w0)/(w1-w0);

        return beta0 * 1e-5; // from km^-1 to cm^-1
        */
    }


    //MSpline3 *fAbsCoeffOzone;
    //MSpline3 *fAbsCoeffAerosols;

    TGraph *fAbsCoeffOzone;
    TGraph *fAbsCoeffAerosols;

public:
    MAtmosphere(const MCorsikaRunHeader &h) : fAbsCoeffOzone(0), fAbsCoeffAerosols(0)
    {
        Init(h);//, "ozone.txt", "aerosols.txt");
    }

    MAtmosphere(const char *name1=0, const char *name2=0) : fAbsCoeffOzone(0), fAbsCoeffAerosols(0)
    {
        if (name1)
            InitOzone(name1);
        if (name2)
            InitAerosols(name2);
    }

    ~MAtmosphere()
    {
        if (fAbsCoeffOzone)
            delete fAbsCoeffOzone;
        if (fAbsCoeffAerosols)
            delete fAbsCoeffAerosols;
    }

    Float_t GetWavelengthMin() const { return fAbsCoeffOzone && fAbsCoeffAerosols ? TMath::Max(fAbsCoeffOzone->GetX()[0], fAbsCoeffAerosols->GetX()[0]) : -1; }
    Float_t GetWavelengthMax() const { return fAbsCoeffOzone && fAbsCoeffAerosols ? TMath::Min(fAbsCoeffOzone->GetX()[fAbsCoeffOzone->GetN()-1], fAbsCoeffAerosols->GetX()[fAbsCoeffAerosols->GetN()-1]) : -1; }

    Bool_t HasValidOzone() const   { return fAbsCoeffOzone    && fAbsCoeffOzone->GetN()>0; }
    Bool_t HasValidAerosol() const { return fAbsCoeffAerosols && fAbsCoeffAerosols->GetN()>0; }

    Bool_t IsAllValid() const { return IsValid() && HasValidOzone() && HasValidAerosol(); }

    void PreCalcOzone()
    {
        // It follows a precalculation of the slant path integrals we need
        // for the estimate of the Mie scattering and Ozone absorption:
        Double_t dh = 1.e3;
        const Double_t STEPTHETA = 1.74533e-2; // aprox. 1 degree

        // Ozone absorption
        for (Int_t j = 0; j < 90; j++)
        {
            const Double_t theta = j * STEPTHETA;  
            const Double_t sin2  = sin(theta)*sin(theta);
            const Double_t H     = R()+fObsLevel;

            Double_t path_slant = 0;
            for (Double_t h = fObsLevel; h <= 50e5; h += dh)
            {
                // h is the true height vertical above ground
                if (fmod(h,1e4) == 0)
                    ozone_path[(int)(h/1e4)][j] = path_slant;

                const Double_t km  = h/1e5;
                const Int_t    i   = TMath::FloorNint(km);
                const Double_t l   = R()+h;

                const Double_t L   = TMath::Sqrt(l*l - H*H * sin2);
                const Double_t f   = dh * l / L;

                // Linear interpolation at h/1e5
                Double_t interpol = oz_conc[i] + fmod(km, 1) * (oz_conc[i+1]-oz_conc[i]);

                path_slant += f * interpol;
            }
        }
    }

    void PreCalcAerosol()
    {
        // It follows a precalculation of the slant path integrals we need
        // for the estimate of the Mie scattering and Ozone absorption:
        Double_t dh = 1.e3;
        const Double_t STEPTHETA = 1.74533e-2; // aprox. 1 degree

        /* Mie (aerosol): */
        for (Int_t j = 0; j < 90; j++)
        {
            const Double_t theta = j * STEPTHETA;  
            const Double_t sin2  = sin(theta)*sin(theta);
            const Double_t H     = R()+fObsLevel;

            Double_t path_slant = 0;
            for (Double_t h = fObsLevel; h <= 30e5; h += dh)
            {
                // h is the true height vertical above ground
                if (fmod(h,1e4) == 0)
                    aerosol_path[(int)(h/1e4)][j] = path_slant;

                const Double_t km  = h/1e5;
                const Int_t    i   = TMath::FloorNint(km);
                const Double_t l   = R()+h;

                const Double_t L   = TMath::Sqrt(l*l - H*H * sin2);
                const Double_t f   = dh * l / L;

                // Linear interpolation at h/1e5
                Double_t interpol = aero_n[i] + fmod(km, 1)*(aero_n[i+1]-aero_n[i]);

                path_slant += f * interpol/aero_n[0];    // aero_n [km^-1]
            }
        }
    }

    Bool_t InitOzone(const TString name="")
    {
        if (!name.IsNull())
        {
            if (fAbsCoeffOzone)
                delete fAbsCoeffOzone;

            fAbsCoeffOzone = new TGraph(name);
            fAbsCoeffOzone->Sort();
        }

        if (!HasValidAerosol())
            return kFALSE;

        if (IsValid())
            PreCalcOzone();

        return kTRUE;
    }

    Bool_t InitAerosols(const TString name="")
    {
        if (!name.IsNull())
        {
            if (fAbsCoeffAerosols)
                delete fAbsCoeffAerosols;

            fAbsCoeffAerosols = new TGraph(name);
            fAbsCoeffAerosols->Sort();
        }

        if (!HasValidAerosol())
            return kFALSE;

        if (IsValid())
            PreCalcAerosol();

        return kTRUE;
    }

    void Init(const MCorsikaRunHeader &h, const char *name1=0, const char *name2=0)
    {
        MAtmRayleigh::Init(h);

        InitOzone(name1);
        InitAerosols(name2);
    }
/*
    Double_t GetOz(Double_t height, Double_t theta) const
    {
        // Distance between two points D = 1km /cos(theta)
        // Density along y within this km:   f =  (x[i+1]-x[i])/1km * dy
        // Integral of this density  f =  (x[i+1]-x[i])/1km * (y[i+1]-y[i])
        // f(h) = int [ (c1-c0)/1km*(h-h0)*dh + c0 ] dh
        //      = (c-co)*(h-h0)

        Double_t rc = 0;
        int i;
        for (i=0; i<49; i++)
            if (i>=2 && i+1<height/1e5)    // cm -> km
                rc += oz_conc[i] * 1e5/cos(theta);

        rc -= oz_conc[2]*0.2*1e5/cos(theta);
        rc += oz_conc[i+1]*fmod(height/1e5,1)*1e5/cos(theta);

        return rc;
    }
    */

    Double_t CalcOzoneAbsorption(Double_t h, Double_t wavelength, Double_t theta) const
    {
        if (!fAbsCoeffOzone)
            return 1;

        //******* Ozone absorption *******
        if (h > 50.e5)
            h = 50.e5;

        // Vigroux Ozone absorption coefficient a.s.l. through interpolation:
        //const float oz_vigroux[15]= {1.06e2, 1.01e1, 8.98e-1, 6.40e-2, 1.80e-3, 0, 0, 3.50e-3, 3.45e-2, 9.20e-2, 1.32e-1, 6.20e-2, 2.30e-2, 1.00e-2, 0.00};
        //const Double_t beta0 = getbeta(wavelength, oz_vigroux);
        const Double_t beta0 = GetBeta(wavelength, *fAbsCoeffOzone);

        // Now use the pre-calculated values of the path integral
        // for h and theta
        const UInt_t H = TMath::Nint(h/1e4);
        const UInt_t T = TMath::Min(89, TMath::Nint(theta/STEPTHETA));

        const Double_t path = ozone_path[H][T];

        return TMath::Exp(-beta0*path);
    }

    Double_t CalcAerosolAbsorption(Double_t h, Double_t wavelength, Double_t theta) const
    {
        if (!fAbsCoeffAerosols)
            return 1;

        //******* Mie (aerosol) *******
        if (h > 30.e5)
            h = 30.e5;

        //const float aero_betap[15] = {0.27, 0.26, 0.25, 0.24, 0.24, 0.23, 0.20, 0.180, 0.167, 0.158, 0.150, 0.142, 0.135, 0.127, 0.120};
        //const Double_t beta0 = getbeta(wavelength, aero_betap);
        const Double_t beta0 = GetBeta(wavelength, *fAbsCoeffAerosols);

        // Now use the pre-calculated values of the path integral
        // for h and theta
        const UInt_t H = TMath::Nint(h/1e4);
        const UInt_t T = TMath::Min(89, TMath::Nint(theta/STEPTHETA));


        const Double_t path = aerosol_path[H][T];

        return TMath::Exp(-beta0*path);
    }

    Double_t GetTransmission(const MPhotonData &ph) const
    {
        const Double_t wavelength = ph.GetWavelength();
        const Double_t height     = ph.GetProductionHeight();

        // Reduce the necessary number of floating point operations
        // by storing the intermediate results
        const Double_t sin2  = ph.GetSinW2();
        const Double_t cost  = TMath::Sqrt(1-sin2);
        const Double_t theta = TMath::ACos(cost);

        // Path from production height to obslevel
        const Double_t z = height-fObsLevel;

        // Distance of emission point to incident point on ground
        const Double_t d = z/cost;

        // Avoid problems if photon is very close to telescope:
        if (TMath::Abs(d)<1)
            return 1;

        // Earth radius plus observation height (distance of telescope
        // from earth center)
        const Double_t H = R() + fObsLevel;

        // We calculate h, the true height a.s.l.
        // of the photon emission point in cm
        const Double_t h = TMath::Sqrt(H*H + d*d + 2*H*z) - R();

        //**** Rayleigh scattering: *****
        const Double_t T_Ray = CalcTransmission(h, wavelength, sin2);
        if (T_Ray<0)
            return 0;

        //****** Ozone absorption: ******
        const Double_t T_Oz  = CalcOzoneAbsorption(h, wavelength, theta);

        //******** Mie (aerosol) ********
        const Double_t T_Mie = CalcAerosolAbsorption(h, wavelength, theta);

        // FIXME: What if I wanna display these values?

        // Calculate final transmission coefficient
        return T_Ray * T_Oz * T_Mie;
    }
};

const Double_t MAtmosphere::STEPTHETA = 1.74533e-2; // aprox. 1 degree

const Double_t MAtmRayleigh::fgMeanFreePath = 2970;

const Double_t MAtmosphere::aero_n[31] = {200, 87, 38, 16, 7.2, 3.1, 1.1, 0.4, 0.14, 5.0e-2, 2.6e-2, 2.3e-2, 2.1e-2, 2.3e-2, 2.5e-2, 4.1e-2, 6.7e-2, 7.3e-2, 8.0e-2, 9.0e-2, 8.6e-2, 8.2e-2, 8.0e-2, 7.6e-2, 5.2e-2, 3.6e-2, 2.5e-2, 2.4e-2, 2.2e-2, 2.0e-2, 1.9e-2};

const Double_t MAtmosphere::oz_conc[51]={0.3556603E-02, 0.3264150E-02, 0.2933961E-02, 0.2499999E-02, 0.2264150E-02, 0.2207546E-02, 0.2160377E-02, 0.2226414E-02, 0.2283018E-02, 0.2811320E-02, 0.3499999E-02, 0.4603772E-02, 0.6207545E-02, 0.8452828E-02, 0.9528299E-02, 0.9905657E-02, 0.1028302E-01, 0.1113207E-01, 0.1216981E-01, 0.1424528E-01, 0.1641509E-01, 0.1839622E-01, 0.1971697E-01, 0.1981131E-01, 0.1933962E-01, 0.1801886E-01, 0.1632075E-01, 0.1405660E-01, 0.1226415E-01, 0.1066037E-01, 0.9028300E-02, 0.7933960E-02, 0.6830187E-02, 0.5820753E-02, 0.4830188E-02, 0.4311319E-02, 0.3613206E-02, 0.3018867E-02, 0.2528301E-02, 0.2169811E-02, 0.1858490E-02, 0.1518867E-02, 0.1188679E-02, 0.9301884E-03, 0.7443394E-03, 0.5764149E-03, 0.4462263E-03, 0.3528301E-03, 0.2792452E-03, 0.2226415E-03, 0.1858490E-03};

// ==========================================================================

// --------------------------------------------------------------------------
//
//  Default Constructor.
//
MSimAtmosphere::MSimAtmosphere(const char* name, const char *title)
    : fRunHeader(0), fEvt(0), fAtmosphere(0),
    fFileAerosols("resmc/atmosphere-aerosols.txt"),
    fFileOzone("resmc/atmosphere-ozone.txt"),
    fForce(kFALSE)
{
    fName  = name  ? name  : "MSimAtmosphere";
    fTitle = title ? title : "Simulate the wavelength and height-dependant atmpsheric absorption";

    fAtmosphere = new MAtmosphere;
}

// --------------------------------------------------------------------------
//
//  Calls Clear()
//
MSimAtmosphere::~MSimAtmosphere()
{
    delete fAtmosphere;
}

// --------------------------------------------------------------------------
//
// Search for the needed parameter containers. Read spline from file
// calling ReadFile();
//
Int_t MSimAtmosphere::PreProcess(MParList *pList)
{
    fEvt = (MPhotonEvent*)pList->FindObject("MPhotonEvent");
    if (!fEvt)
    {
        *fLog << err << "MPhotonEvent not found... aborting." << endl;
        return kFALSE;
    }


    return kTRUE;
}

// --------------------------------------------------------------------------
//
Bool_t MSimAtmosphere::ReInit(MParList *pList)
{
    fRunHeader = (MCorsikaRunHeader*)pList->FindObject("MCorsikaRunHeader");
    if (!fRunHeader)
    {
        *fLog << err << "MCorsikaRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    if (fRunHeader->Has(MCorsikaRunHeader::kCeffic) && !fForce)
    {
        *fLog << inf << "CEFFIC enabled... task will be skipped." << endl;
        return kTRUE;
    }

    //if (fRunHeader->Has(MCorsikaRunHeader::kRefraction))
    //    *fLog << inf << "Atmospheric refraction already applied in Corsika... skipping our own." << endl;

    // FIXME: Check wavelength range

    /*
    if (h->GetWavelengthMin()<fSpline->GetXmin())
        *fLog << warn << "WARNING - Lower bound of wavelength bandwidth exceeds lower bound of spline." << endl;

    if (h->GetWavelengthMax()>fSpline->GetXmax())
        *fLog << warn << "WARNING - Upper bound of wavelength bandwidth exceeds upper bound of spline." << endl;
    */

    fAtmosphere->Init(*fRunHeader, fFileOzone, fFileAerosols);

    if (!fAtmosphere->IsAllValid())
    {
        *fLog << err << "ERROR - Something with the atmoshere's initialization went wrong!" << endl;
        return kFALSE;
    }

    if (fRunHeader->GetWavelengthMin()<fAtmosphere->GetWavelengthMin())
        *fLog << warn << "WARNING - Lower bound of wavelength bandwidth exceeds valid range of atmosphere." << endl;

    if (fRunHeader->GetWavelengthMax()>fAtmosphere->GetWavelengthMax())
        *fLog << warn << "WARNING - Lower bound of wavelength bandwidth exceeds  valid range of atmosphere." << endl;

    if (!fRunHeader->Has(MCorsikaRunHeader::kAtmext))
        *fLog << warn << "WARNING - ATMEXT option not used for Corsika data." << endl;

    if (!fRunHeader->Has(MCorsikaRunHeader::kRefraction))
        *fLog << warn << "WARNING - Refraction calculation disabled for Corsika data." << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
Int_t MSimAtmosphere::Process()
{
    // Skip the task if the CEFFIC option has been enabled and
    // its excution has not been forced by the user
    if (!fForce && fRunHeader->Has(MCorsikaRunHeader::kCeffic))
        return kTRUE;

    // Get the number of photons in the list
    const Int_t num = fEvt->GetNumPhotons();

    // FIMXE: Add checks for
    //         * upgoing particles
    //         * Can we take the full length until the camera into account?

    // Counter for number of total and final events
    Int_t cnt = 0;
    for (Int_t i=0; i<num; i++)
    {
        // Get i-th photon from the list
        const MPhotonData &ph = (*fEvt)[i];

        // Get atmospheric transmission for this photon
        const Double_t eff = fAtmosphere->GetTransmission(ph);

        // Get a random value between 0 and 1 to determine whether the photon will survive
        // gRandom->Rndm() = [0;1[
        if (gRandom->Rndm()>=eff)
            continue;

        // Copy the surviving events bakc in the list
        (*fEvt)[cnt++] = ph;
    }

    // Now we shrink the array to the number of new entries.
    fEvt->Shrink(cnt);

    return kTRUE;
}

/*
 Int_t MSimWavelength::Process()
 {
    // Get the number of photons in the list
    const Int_t num = fEvt->GetNumPhotons();

    // FIMXE: Add checks for
    //         * upgoing particles
    //         * wavelength range
    //         * check if corsika atmosphere is switched on
    //         * Can we take the full length until the camera into account?

    // Counter for number of total and final events
    Int_t cnt = 0;
    for (Int_t i=0; i<num; i++)
    {
        // Get i-th photon from the list
        MPhotonData &ph = (*fEvt)[i];

        const Double_t min = fRunHeader->GetWavelengthMin(); // WAVLGL
        const Double_t max = fRunHeader->GetWavelengthMax(); // WAVLGU
        const Double_t f   = (max-min)/max;

        // WAVELENGTH = 1. / (1/min - RD(1)/(min*max/(max-min)))


        ph.SetWavelength(TMath::Nint(min / (1. - gRandom->Rndm()*f)));
    }

    return kTRUE;
 }
 */

// --------------------------------------------------------------------------
//
// FileAerosols: resmc/atmosphere-aerosols.txt
// FileOzone:    resmc/atmosphere-ozone.txt
//
Int_t MSimAtmosphere::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;

    if (IsEnvDefined(env, prefix, "FileAerosols", print))
    {
        rc = kTRUE;
        fFileAerosols = GetEnvValue(env, prefix, "FileAerosols", fFileAerosols);
    }

    if (IsEnvDefined(env, prefix, "FileOzone", print))
    {
        rc = kTRUE;
        fFileOzone = GetEnvValue(env, prefix, "FileOzone", fFileOzone);
    }

    if (IsEnvDefined(env, prefix, "Force", print))
    {
        rc = kTRUE;
        fForce = GetEnvValue(env, prefix, "Force", fForce);
    }

    return rc;
}
