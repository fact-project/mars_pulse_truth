#ifndef MARS_MSrcPosFromStars
#define MARS_MSrcPosFromStars

#ifndef ROOT_TArrayF
#include "TArrayF.h"
#endif

#ifndef ROOT_TMath
#include "TMath.h"
#endif

#ifndef MARS_MSrcPlace
#include "MSrcPlace.h"
#endif

class MStarLocalCam;
class MSrcPosCam;

class MSrcPosFromStars : public MSrcPlace
{
 private:

  MStarLocalCam *fStars;

  UInt_t fNumStars;
  TArrayF fDistances;

  virtual Int_t ComputeNewSrcPosition();
  virtual Int_t PreProcess(MParList *plist);
    
public:    

  MSrcPosFromStars(const char *name=NULL, const char *title=NULL);


// This fuction is to compute the probability using a ring distribution
  Double_t Prob(Double_t d, Double_t dist, Double_t sigma)
    {
      return TMath::Exp(-(d-dist)*(d-dist)/(2*sigma*sigma));///(sigma*TMath::Sqrt(2*3.141592654));
    }
 
  Double_t Dist(Double_t x1, Double_t x2, Double_t y1, Double_t y2)
    {
      return TMath::Sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    }
  
  void SetDistances(TArrayF dist) { fDistances = dist; }
  
  ClassDef(MSrcPosFromStars, 0) // task to calculate the position of the source using the position of stars
};

#endif
