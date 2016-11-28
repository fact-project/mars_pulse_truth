#ifndef MARS_MHNewImagePar2
#define MARS_MHNewImagePar2

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif

class MGeomCam;
class MHillas;

class MHNewImagePar2 : public MH
{
private:
    MGeomCam *fGeom;    //! conversion mm to deg

    TH1F fHistBorder1;  //
    TH1F fHistBorder2;  //

public:
    MHNewImagePar2(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    void Draw(Option_t *opt="");

    ClassDef(MHNewImagePar2, 2) // Histograms of new image parameters
};

#endif


