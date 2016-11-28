#ifndef MARS_MSourcePosfromStarPos
#define MARS_MSourcePosfromStarPos

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MSourcePosfromStarPos                                                   //
//                                                                         //
// Task to calculate the position of the source in the camera from
//      the position of a known star in the camera
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

#ifndef ROOT_TMatrixD
#include <TMatrixD.h>
#endif

class TList;
class MRawRunHeader;
class MPointingPos;
class MMcEvt;
class MGeomCam;
class MSrcPosCam;
class MObservatory;


class MSourcePosfromStarPos : public MTask
{
 private:
    const MRawRunHeader *fRun;      //!
    const MGeomCam      *fGeomCam;  //! Camera Geometry used to calculate Hillas
    MMcEvt              *fMcEvt;       //!
    MPointingPos        *fPointPos;       //!
    MSrcPosCam          *fSrcPos;         //!
    MObservatory        *fObservatory;    //!

    ifstream    *fIn;             // input file
    TList       *fFileNames;      // array which contains the \0-terminated file names

    Double_t fMm2Deg;
    Double_t fDistCameraReflector;

    Int_t   fRuns;                // current number of runs
    Int_t   fSize;                // final   number of runs
    Int_t   fStars;               // number of stars
    Int_t   fStarsRead;           // number of (x,y) pairs to be read


    Double_t fDecSource;  // sky coordinates of source
    Double_t fRaSource;
    Double_t fxSource;    // estimated position of source in camera
    Double_t fySource;
    Double_t fdxSource;
    Double_t fdySource;

    TArrayI fRunNr;       // list of run numbers
    TArrayD fThetaTel;    // for each run : theta, phi, ...
    TArrayD fPhiTel;
    TArrayD fdThetaTel;
    TArrayD fdPhiTel;

    TArrayD  fDecStar;   // sky coordinates of stars
    TArrayD  fRaStar;
    TMatrixD fxStar;     // for each run : positions of stars in camera (from DC currents)
    TMatrixD fyStar;
    TMatrixD fdxStar;
    TMatrixD fdyStar;

    Double_t fxSourceold;  
    Double_t fySourceold;
    Double_t fThetaradold;
    Double_t fPhiradold;

    Bool_t OpenNextFile();
    void   ReadData();
    void   FixSize();

    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();
    Int_t  PostProcess();

public:
    MSourcePosfromStarPos(const char *name=NULL, const char *title=NULL);
    ~MSourcePosfromStarPos();

    void SetSourceAndStarPosition(
	 TString  nameSource,
	 Double_t decSourceDeg, Double_t decSourceMin, Double_t decSourceSec, 
         Double_t raSourceHour, Double_t raSourceMin,  Double_t raSourceSec,
	 TString  nameStar,
	 Double_t decStarDeg,   Double_t decStarMin,   Double_t decStarSec, 
         Double_t raStarHour,   Double_t raStarMin,    Double_t raStarSec  );

    Int_t  AddFile(const char *fname, Int_t dummy=-1);

    void AddStar(
	 TString  nameStar,
	 Double_t decStarDeg,   Double_t decStarMin,   Double_t decStarSec, 
         Double_t raStarHour,   Double_t raStarMin,    Double_t raStarSec  );

    void SourcefromStar(Double_t &, 
      TArrayD  &, TArrayD  &, Double_t &, Double_t &, Double_t &, Double_t &, 
      TArrayD  &, TArrayD  &, TArrayD  &, TArrayD  &, Double_t &, Double_t &, 
      Double_t &, Double_t & );

    ClassDef(MSourcePosfromStarPos, 0) // Task to calculate the source position from a star position
};

#endif








