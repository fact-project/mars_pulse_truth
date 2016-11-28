#ifndef MARS_MHPSFFromStars
#define MARS_MHPSFFromStars

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

#ifndef ROOT_TH1F
#include <TH1F.h>
#endif

#ifndef ROOT_TGraph
#include <TGraph.h>
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

#ifndef ROOT_TProfile
#include <TProfile.h>
#endif

#ifndef ROOT_TProfile2D
#include <TProfile2D.h>
#endif

#ifndef MARS_MHCamera
#include "MHCamera.h"
#endif

#ifndef MARS_MH
#include "MH.h"
#endif

class TObject;
class MCameraDC;
class MStarLocalPos;
class MStarLocalCam;

class MHPSFFromStars : public MH
{
 private:

  MGeomCam* fGeom;
  MCameraDC* fCurr;
  
  TH1F fHistMeanX;
  TH1F fHistMeanY;
  TH1F fHistSigmaMinor;
  TH1F fHistSigmaMajor;

  TGraph *fGraphMeanX;
  TGraph *fGraphMeanY;
  TGraph *fGraphPath;
  TGraph *fGraphSigmaMinor;
  TGraph *fGraphSigmaMajor;

  TArrayF fvsTimeMeanX;
  TArrayF fvsTimeMeanY;
  TArrayF fvsTimeSigmaMinor;
  TArrayF fvsTimeSigmaMajor;

  TArrayF fTime;
  TArrayF fEvent;

  ULong_t fNumEvents;

  MHCamera fCamera;
  TProfile fProjectionX;
  TProfile fProjectionY;
  TProfile2D fProfile;

  Int_t fSelectedStarPos;
  MStarLocalPos* SelectStar(MStarLocalCam* stars);

  public:

  MHPSFFromStars(Int_t starpos=-1, const char *name=NULL, const char *title=NULL);
  ~MHPSFFromStars();

  Bool_t SetupFill(const MParList *pList);
  Bool_t Fill(const MParContainer *par, const Stat_t w=1);
  Bool_t Finalize();

  void Draw(const Option_t*);
/*   TObject* Clone(const char *) const; */

  //Getter
  TProfile& GetProjectionX() { return fProjectionX; }
  TProfile& GetProjectionY() { return fProjectionY; }
  
  ClassDef(MHPSFFromStars, 1) // A list of histograms storing star information from PMT DCs
};

#endif

