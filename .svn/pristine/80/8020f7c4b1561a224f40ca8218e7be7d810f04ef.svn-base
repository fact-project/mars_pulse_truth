#ifndef MARS_MTopology
#define MARS_MTopology

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MGeomCam;
class MCerPhotEvt;

class MTopology : public MParContainer
{
private:

    Int_t fDistance; // Evaluated sum of distances between 'used' pixels after image cleaning [mm]
    Int_t fUsed;     // Number of pixels contained in an image

    Int_t fN_Pixels;

public:

    MTopology(const char *name=NULL, const char *title=NULL);

    void Reset();

    Int_t Calc(const MGeomCam &geom, const MCerPhotEvt &evt);
    
    void Print(Option_t *opt=NULL) const;
    
    Int_t GetDistance()             { return fDistance; } 
    Int_t GetUsedPixels()           { return fUsed; }

    void SetDistance(Int_t Dist)    { fDistance=Dist; } 
    void SetUsedPixels(Int_t Used)  { fUsed=Used; }

    ClassDef(MTopology, 1) // Container to hold Topology related parameters
};

#endif
