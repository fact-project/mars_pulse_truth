#ifndef MARS_MHVsSize
#define MARS_MHVsSize

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TH2
#include <TH2.h>
#endif

class MGeomCam;
class MHillas;
class MHillasExt;
class MNewImagePar;

class MHVsSize : public MH
{
private:
    MGeomCam     *fGeom;        //! Conversion from mm to deg
    MHillas      *fHillas;      //!
    MHillasExt   *fHillasExt;   //!
    MNewImagePar *fNewImagePar; //!

    TH2F fLength;  // Length
    TH2F fWidth;   // Width

    TH2F fDist;    // Distance to Camera Center
    TH2F fConc1;   // Angle between Length axis and x-axis

    TH2F fM3Long;  // 
    TH2F fArea;    // 

public:
    MHVsSize(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    void Draw(Option_t *opt=NULL);

    ClassDef(MHVsSize, 2) // Container which holds histograms for image parameters vs size
};

#endif
