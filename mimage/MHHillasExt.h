#ifndef MARS_MHHillasExt
#define MARS_MHHillasExt

#ifndef ROOT_TH2
#include <TH2.h>
#endif
#ifndef MARS_MH
#include "MH.h"
#endif

class MGeomCam;
class MHillas;
class MHillasExt;

class MHHillasExt : public MH
{
private:
    MGeomCam   *fGeom;      //! conversion mm to deg
    MHillas    *fHillas;    //! Pointer to the MHillas container
    MHillasExt *fHillasExt; //! Pointer to the MHillasExt container

    TH1F fHAsym;            // [mm] fDist minus dist: center of ellipse, highest pixel
    TH1F fHM3Long;          // [mm] 3rd moment (e-weighted) along major axis
    TH1F fHM3Trans;         // [mm] 3rd moment (e-weighted) along minor axis
    TH2F fHSlopeL;          //

    TH1F fHTimeSpread;      // [ns] Spread (rms) of arrival time around mean
    TH1F fHTimeSpreadW;     // [ns] Weighted spread (rms) of arrival time around weighted mean
    TH1F fHSlopeSpread;     // [ns] Spread (rms) of arrival time around slope
    TH1F fHSlopeSpreadW;    // [ns] Weighted spread (rms) of arrival time around slope

    TString fHilName;

public:
    MHHillasExt(const char *name=NULL, const char *title=NULL);

    void SetHillasName(const char *name) { fHilName = name; }

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    TH1 *GetHistByName(const TString name) const;
    TObject *FindObject(const TObject *obj) const { return 0; }
    TObject *FindObject(const char *name) const
    {
        return (TObject*)GetHistByName(name);
    }

    void Draw(Option_t *opt=NULL);

    ClassDef(MHHillasExt, 4) // Container which holds histograms for the extended hillas parameters
};

#endif
