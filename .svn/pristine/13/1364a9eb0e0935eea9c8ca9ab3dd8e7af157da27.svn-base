#ifndef MARS_MImgCleanTime
#define MARS_MImgCleanTime

#include <stdint.h>
#include <list>
#include <vector>

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeom;
class MGeomCam;
class MSignalPix;
class MSignalCam;

struct Island
{
    uint16_t count;
    float size;
    float min;
    float max;

    Island() : count(0) { }
    Island(float sz, float tm) : count(1), size(sz), min(tm), max(tm) { }

    void operator+=(const Island &i)
    {
        count += i.count;
        size += i.size;
        if (i.min<min)
            min = i.min;
        if (i.max>max)
            max = i.max;
    }
};

class MImgCleanTime : public MTask
{
public:
    const MGeomCam   *fCam;  //!
          MSignalCam *fEvt;  //!

    UInt_t  fMinCount;
    Float_t fMinSize;
    Float_t fDeltaT;

    TString fNameSignalCam;  // name of the 'MSignalCam' container

    std::vector<Island> fIslands;
    std::list<std::pair<uint16_t, uint16_t>> fContacts;
    std::vector<uint16_t> fLut;

    Island CalcIsland(MSignalPix &, const MGeom &, const uint16_t &);

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MImgCleanTime(const char *name=NULL, const char *title=NULL);

    //void Print(Option_t *o="") const;

    void SetMinCount(UInt_t sz)  { fMinCount=sz; }
    void SetMinSize(Float_t lvl) { fMinSize=lvl; }
    void SetDeltaT(Float_t dt)   { fDeltaT=dt; }

    void SetNameSignalCam(const char *name) { fNameSignalCam = name; }

    ClassDef(MImgCleanTime, 0) // task doing the image cleaning
}; 

#endif
