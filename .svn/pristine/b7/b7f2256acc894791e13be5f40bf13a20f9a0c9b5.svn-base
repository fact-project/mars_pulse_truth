#ifndef MARS_MImgIsland
#define MARS_MImgIsland

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif


class MImgIsland : public MParContainer
{
 private:

  Int_t   fPixNum;
  Float_t fSigToNoise;
  Float_t fTimeSpread;
  Float_t fDist;
  Float_t fDistL;
  Float_t fDistW;
  Float_t fDistS;
  
  Float_t fWidth;
  Float_t fLength;
  Float_t fSizeIsl;
  Float_t fMeanX;
  Float_t fMeanY;
 
  Float_t fAlpha;


  TArrayI fPixList;
  TArrayF fPeakPulse;
  
public:

  MImgIsland(const char *name=NULL, const char *title=NULL);
  ~MImgIsland();

  Int_t   GetPixNum()      { return fPixNum; }
  Float_t GetSigToNoise()  { return fSigToNoise; }
  Float_t GetTimeSpread()  { return fTimeSpread; }
  Float_t GetDist()        { return fDist; }  
  Float_t GetDistL()       { return fDistL; }
  Float_t GetDistW()       { return fDistW; }
  Float_t GetDistS()       { return fDistS; }

  //hillas parameters
  Float_t GetSizeIsl()     { return fSizeIsl; }
  Float_t GetMeanX()       { return fMeanX; }
  Float_t GetMeanY()       { return fMeanY; }
  Float_t GetWidth()       { return fWidth; }
  Float_t GetLength()      { return fLength; }

  // hillas src parameters
  Float_t GetAlpha()       { return fAlpha; }
  
  void    InitSize(Int_t i);
  UInt_t  GetSize() const { return fPixList.GetSize(); }

  Int_t    GetPixList(const Int_t i = 0)   const { return fPixList.At(i); };
  Float_t   GetPeakPulse(const Int_t i = 0) const { return fPeakPulse.At(i); };
 
  void Reset();

  void SetPixNum    (Int_t   i)   { fPixNum = i; }
  void SetSigToNoise(Float_t val) { fSigToNoise = val; }
  void SetTimeSpread(Float_t val) { fTimeSpread = val; }
  void SetDist      (Float_t val) { fDist = val; } 
  void SetDistL     (Float_t val) { fDistL = val; }
  void SetDistW     (Float_t val) { fDistW = val; }
  void SetDistS     (Float_t val) { fDistS = val; } 

  //hillas parameters
  void SetSizeIsl   (Float_t val) { fSizeIsl = val; }
  void SetMeanX     (Float_t val) { fMeanX = val; }
  void SetMeanY     (Float_t val) { fMeanY = val; }
  void SetWidth     (Float_t val) { fWidth = val; }
  void SetLength    (Float_t val) { fLength = val; }
  
  // hillas src parameters
  void SetAlpha     (Float_t val) { fAlpha = val; }
 
  void SetPixList( const Int_t i, const Int_t id);
  void SetPeakPulse( const Int_t i, const Float_t time);

  //  void Paint(Option_t *opt=NULL);
  void Print(Option_t *opt=NULL) const;  

  ClassDef(MImgIsland, 2) // Container that holds the island information

};

#endif
