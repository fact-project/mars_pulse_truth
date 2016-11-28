#ifndef MARS_MStarCam
#define MARS_MStarCam

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MHCamera
#include "MHCamera.h"
#endif

class TList;

class MGeomCam;
class MStarPos;

class MStarCam : public MParContainer
{
private:

  TList  *fStars;  //-> FIXME: Change TClonesArray away from a pointer?

  // BaseLine DCs info
  Float_t fInnerPedestalDC;         //[ua]
  Float_t fOuterPedestalDC;         //[ua]

  Float_t fInnerPedestalRMSDC;      //[ua]
  Float_t fOuterPedestalRMSDC;      //[ua]

  MHCamera fDisplay;

 public:

  MStarCam(const char *name=NULL, const char *title=NULL);
  ~MStarCam();

  MStarPos &operator[] (Int_t i);
  const MStarPos &operator[] (Int_t i) const;

  TList *GetList() const { return fStars; }
  UInt_t GetNumStars() const { return fStars->GetSize(); }

  //Getters

  Float_t GetInnerPedestalDC() {return fInnerPedestalDC;}
  Float_t GetOuterPedestalDC() {return fOuterPedestalDC;}
  Float_t GetInnerPedestalRMSDC() { return fInnerPedestalRMSDC;}
  Float_t GetOuterPedestalRMSDC() { return fOuterPedestalRMSDC;}

  MHCamera& GetDisplay() { return fDisplay; }

    //Setters
  void SetInnerPedestalDC(Float_t ped) {fInnerPedestalDC = ped;}
  void SetOuterPedestalDC(Float_t ped) {fOuterPedestalDC = ped;}
  void SetInnerPedestalRMSDC(Float_t rms){fInnerPedestalRMSDC = rms;}
  void SetOuterPedestalRMSDC(Float_t rms){fOuterPedestalRMSDC = rms;}

  void Paint(Option_t *o=NULL);
  void Print(Option_t *o=NULL) const;

  ClassDef(MStarCam, 1)	// Storage Container for star positions in the camera
};

#endif
