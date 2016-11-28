#ifndef MARS_MCT1PointingCorrCalc
#define MARS_MCT1PointingCorrCalc

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MCT1PointingCorrCalc                                                    //
//                                                                         //
// Task to do the pointing correction                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MSrcPosCam;
class MParameterD;


class MCT1PointingCorrCalc : public MTask
{
public:
    typedef enum {
        k421,
        k1959
    } PointedSource_t;

private:

    MSrcPosCam   *fSrcPos;
    TString       fSrcName;
    MParameterD  *fHourAngle;

    PointedSource_t fPoiSource;
    Float_t       fMm2Deg;

    void PointCorr421();
    void PointCorr1959();

public:

    MCT1PointingCorrCalc(const char *srcname="MSrcPosCam",
                         const char *name=NULL, const char *title=NULL);

    Int_t PreProcess(MParList *pList);
    Int_t Process();

    void SetPointedSource(PointedSource_t s) { fPoiSource = s; }
 
    ClassDef(MCT1PointingCorrCalc, 0)   // Task to do the CT1 pointing correction for Mkn421 2001 data or 1ES1959 2002 data
};

#endif











