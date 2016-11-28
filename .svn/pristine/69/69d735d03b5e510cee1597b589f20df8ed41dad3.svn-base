#ifndef MARS_MAstroCamera
#define MARS_MAstroCamera

#ifndef MARS_MAstroCatalog
#include "MAstroCatalog.h"
#endif

class TClonesArray;
class TList;

class MTime;
class MGeomCam;
class MGeomMirror;
class MObservatory;

class MAstroCamera : public MAstroCatalog
{
private:
    MGeomCam     *fGeom;
    TClonesArray *fMirrors;

    MGeomMirror  *fMirror0;     //!

    Int_t  ConvertToPad(const TVector3 &w, TVector2 &v) const;
    void   AddPrimitives(TString o);
    void   SetRangePad(Option_t *o) { }
    void   ExecuteEvent(Int_t event, Int_t mp1, Int_t mp2);

public:
    MAstroCamera();
    ~MAstroCamera();

    void SetMirrors(TClonesArray &arr);
    void SetMirrors(const char *fname);
    void SetGeom(const MGeomCam &cam);

    //void FillStarList(TList *list);

    void GetDiffZdAz(Double_t x, Double_t y, Double_t &dzd, Double_t &daz);

    ClassDef(MAstroCamera, 1) // Display class to display stars on the camera
};

#endif
