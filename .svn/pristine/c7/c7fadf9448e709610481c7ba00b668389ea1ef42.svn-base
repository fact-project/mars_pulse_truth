#ifndef MARS_MStereoCalc
#define MARS_MStereoCalc

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MStereoCalc                                                             //
//                                                                         //
// Task to calculate some shower parameters in stereo mode                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeomCam;
class MHillas;
class MStereoPar;
class MPointingPos;

class MStereoCalc : public MTask
{
    const MGeomCam     *fGeomCam1;     //! CT1: Camera Geometry
    const MHillas      *fHillas1;      //! CT1: Hillas parameters
    const MPointingPos *fPointingPos1; //! CT1: Pointing Direction

    const MGeomCam     *fGeomCam2;     //! CT2: Camera Geometry
    const MHillas      *fHillas2;      //! CT2: Hillas parameters
    const MPointingPos *fPointingPos2; //! CT2: pointing direction

    Int_t fCT1id;   // CT1: Identifier number
    Int_t fCT2id;   // CT2: Identifier number

    Float_t fCT1x;   //! FIXME -> Move to parameter list
    Float_t fCT1y;   //! Position of first telescope
    Float_t fCT2x;   //! FIXME -> Move to parameter list
    Float_t fCT2y;   //! Position of second telescope

    MStereoPar   *fStereoPar;     //! output container to store result
    TString       fStereoParName; // name of the 'MStereoPar' container

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:

    MStereoCalc(const char *name=NULL, const char *title=NULL);

    void SetNameStereoPar(const char *name) { fStereoParName = name; }

    void SetCTids(Int_t i, Int_t j) { fCT1id = i; fCT2id = j; }
    void SetCT1coor(Float_t x, Float_t y) { fCT1x = x; fCT1y = y; } // in m
    void SetCT2coor(Float_t x, Float_t y) { fCT2x = x; fCT2y = y; } // in m

    ClassDef(MStereoCalc, 0) // Task to calculate some shower parameters in stereo mode
};

#endif


