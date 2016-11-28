#ifndef MARS_MHSrcPosCam
#define MARS_MHSrcPosCam

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef MARS_MTime
#include "MTime.h"
#endif

#ifndef ROOT_TH2
#include <TH2.h>
#endif

#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif

class MGeomCam;
class MParList;
class MParameterD;
class MPointingPos;

class MHSrcPosCam : public MH
{
private:
    TH2D          fHist;           // Histogram of observation time vs source position

    MTime         fTimeLastEffOn;  //! Last time stamp of effective on time
    MTime        *fTimeEffOn;      //! Current effective on time
    MParameterD  *fEffOnTime;      //! Effective on time
    MPointingPos *fSourcePos;      //! Pointing position of the telescope
    MGeomCam     *fGeom;           //! Conversion from mm to deg

    TClonesArray  fPositions;      //! Buffer to store source positions
    Int_t         fNum;            //! Position in array

    Float_t       fWobbleOffset;

public:
    MHSrcPosCam(Float_t wobble=0.4, const char *name=NULL, const char *title=NULL);

    // MH
    Bool_t SetupFill(const MParList *pl);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    // MHSrcPosCam
    const TH2D &GetHist() const { return fHist; }
          TH2D &GetHist()       { return fHist; }

    // TObject
    void Paint(Option_t *option="");
    void Draw(Option_t *option="");

    ClassDef(MHSrcPosCam, 1) // Histogram for source position distribution
};

#endif
