#ifndef MARS_MHNewImagePar
#define MARS_MHNewImagePar

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif

class MGeomCam;
class MHillas;

class MHNewImagePar : public MH
{
private:
    MGeomCam *fGeom;     //! Conversion from mm to deg

    TH1F fHistLeakage1;  //
    TH1F fHistLeakage2;  //

    TH1F fHistUsedPix;   // Number of used pixels
    TH1F fHistCorePix;   // Number of core pixels

    TH1F fHistUsedArea;  // Area of used pixels
    TH1F fHistCoreArea;  // Area of core pixels

    TH1F fHistConc;      // [ratio] concentration ratio: sum of the two highest pixels / fSize
    TH1F fHistConc1;     // [ratio] concentration ratio: sum of the highest pixel / fSize
    TH1F fHistConcCOG;   // [ratio] concentration of the three pixels next to COG
    TH1F fHistConcCore;  // [ratio] concentration of signals inside or touching the ellipse

public:
    MHNewImagePar(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    TH1 *GetHistByName(const TString name) const;
    TObject *FindObject(const TObject *obj) const { return 0; }
    TObject *FindObject(const char *name) const
    {
        return (TObject*)GetHistByName(name);
    }

    TH1F &GetHistLeakage1()  { return fHistLeakage1; }
    TH1F &GetHistLeakage2()  { return fHistLeakage2; }

    TH1F &GetHistUsedPix()   { return fHistUsedPix; }
    TH1F &GetHistCorePix()   { return fHistCorePix; }

    TH1F &GetHistUsedArea()  { return fHistUsedArea; }
    TH1F &GetHistCoreArea()  { return fHistCoreArea; }

    TH1F &GetHistConc()      { return fHistConc; }
    TH1F &GetHistConc1()     { return fHistConc1; }
    TH1F &GetHistConcCOG()   { return fHistConcCOG; }
    TH1F &GetHistConcCore()  { return fHistConcCore; }

    void Draw(Option_t *opt="");
    void Paint(Option_t *opt="");

    ClassDef(MHNewImagePar, 3) // Histograms of new image parameters
};

#endif


