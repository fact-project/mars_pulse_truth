#ifndef MARS_MIslands
#define MARS_MIslands

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MHCamera
#include "MHCamera.h"
#endif

class TList;

class MImgIsland;

class MIslands : public MParContainer
{
 private:

  TList*  fIslands;  //-> FIXME: Change TClonesArray away from a pointer?

  //MHCamera fDisplay;
  
  Int_t fIslNum; 
  Float_t fAlphaW;

 public:

  MIslands(const char *name=NULL, const char *title=NULL);
  ~MIslands();

  MImgIsland &operator[] (Int_t i);
  const MImgIsland &operator[] (Int_t i) const;

  TList* GetList() const { return fIslands; }
  UInt_t GetIslNum() const { return fIslands->GetSize(); } //number of islands
  Float_t GetAlphaW() const { return fAlphaW; }     //alpha weighted

  void SetIslNum       (Int_t   i)   { fIslNum = i; }
  void SetAlphaW(Float_t val) { fAlphaW = val; }
  //  MHCamera& GetDisplay() { return fDisplay; }

  //  void Paint(Option_t *o=NULL);
  void Print(Option_t *o=NULL) const;

  ClassDef(MIslands, 2)	// Storage Container for islands
};

#endif
