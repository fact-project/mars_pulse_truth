#ifndef MARS_MImgCleanTGB
#define MARS_MImgCleanTGB

#ifndef MARS_MGTask
#include "MGTask.h"
#endif

class MGeomCam;
class MSigmabar;
class MCerPhotPix;
class MCerPhotEvt;
class MPedPhotCam;

class MGGroupFrame;

class MImgCleanTGB : public MGTask
{
public:
    typedef enum {
        kStandard,
        kDemocratic
    } CleaningMethod_t;

private:
    const MGeomCam    *fCam;  //!
          MCerPhotEvt *fEvt;  //!
          MSigmabar   *fSgb;  //!
          MPedPhotCam *fPed;  //!

    CleaningMethod_t fCleaningMethod;

    Float_t fCleanLvl1;
    Float_t fCleanLvl2;

    UShort_t fCleanRings;

    Float_t fInnerNoise;      //!

    void CreateGuiElements(MGGroupFrame *f);
    void StreamPrimitive(ostream &out) const;

    Int_t CleanStep3b(MCerPhotPix &pix);
    void  CleanStep3(Int_t num1, Int_t num2);
    void  CleanStep3(Byte_t *nb, Int_t num1, Int_t num2);

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MImgCleanTGB(const Float_t lvl1=3.0, const Float_t lvl2=2.5,
              const char *name=NULL, const char *title=NULL);
    void Print(Option_t *o="") const;

    Float_t  GetCleanLvl1() const { return fCleanLvl1; }
    Float_t  GetCleanLvl2() const { return fCleanLvl2; }
    UShort_t GetCleanRings() const { return fCleanRings;}

    void SetCleanRings(UShort_t r) { if(r==0) r=1; fCleanRings=r; }
    void SetMethod(CleaningMethod_t m) { fCleaningMethod = m; }

    Bool_t ProcessMessage(Int_t msg, Int_t submsg, Long_t param1, Long_t param2);

    ClassDef(MImgCleanTGB, 2)    // task doing the image cleaning
}; 

#endif
