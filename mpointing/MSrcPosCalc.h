#ifndef MARS_MSrcPosCalc
#define MARS_MSrcPosCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TVector2
#include <TVector2.h>
#endif

class MObservatory;
class MPointingPos;
class MPointingDev;
class MSrcPosCam;
class MGeomCam;
class MTime;
class MVector3;
class MTaskList;
class MMcEvt;
class MMcRunHeader;
class MMcCorsikaRunHeader;

class MSrcPosCalc : public MTask
{
public:
    enum Mode_t {
        kDefault = 0,   // Set source position to on-position
        kOffData = 1,   // The source position is fixed to (0/0)
        kWobble  = 2,   // The source position is set to the wobble aka. anti-source position depending on the cycle number
        kFixed   = 3    // Set source position to predefined fFixedPos
    };
private:
    enum {
        kIsOwner = BIT(14)
    };

    const MObservatory        *fObservatory;   //! Observatory location
    const MPointingPos        *fPointPos;      //! Present pointing position of the telescope in Zd/Az
    const MPointingDev        *fDeviation;     //! Deviation calculated from starguider data
    const MMcEvt              *fMcEvt;         //! Possible input of shower position from MC
    const MMcRunHeader        *fMcHeader;      //! Monte Carlo run header needed for correct wobble position
    const MMcCorsikaRunHeader *fMcCorsika;     //! Monte Carlo run header needed to determine view cone option
    const MGeomCam            *fGeom;          //! Camera geomety
    const MTime               *fTime;          //! Time of the current event
    const MTaskList           *fCallback;      //! Callback function to get the number of the cycle
    MPointingPos *fSourcePos;     //! Source Postion in sky coordinates
    MSrcPosCam   *fSrcPosCam;     //! Output: Source position in the camera
    MSrcPosCam   *fSrcPosAnti;    //! Output: Anti Source position in the camera

    UShort_t fRunType;            //! Run Type to decide where to get pointing position from

    TVector2 fFixedPos;           //! [deg] Fixed source position

    Int_t fMode;                  // Mode how the source position is calculated

    Int_t fNumRandomOffPositions; // Number of possible random off-sourcr position

    // MSrcPosCalc
    void     SetSrcPos(TVector2 v=TVector2()) const;
    TVector2 Rotate(TVector2 v, Int_t pass, Int_t num) const;
    TString  GetRaDec(const MPointingPos &pos) const;
    Bool_t   GetCoordinate(TString str, Double_t &ret) const;
    void     FreeSourcePos();
    void     CalcResult(const MVector3 &pos0, const MVector3 &pos);
    void     InitFixedPos() const;

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // MTask
    Bool_t ReInit(MParList *pList);
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

public:
    MSrcPosCalc(const char *name=NULL, const char *title=NULL);
    ~MSrcPosCalc() { FreeSourcePos(); }

    // MSrcPosCalc
    void SetSourcePos(MPointingPos *pos) { FreeSourcePos(); fSourcePos = pos; }
    void SetSourcePos(Double_t ra, Double_t dec);
    void SetOwner(Bool_t b=kTRUE) { b ? SetBit(kIsOwner) : ResetBit(kIsOwner); } // Make MSrcPosCalc owner of fSourcePos
    void SetMode(Mode_t m=kDefault) { fMode = m; }
    void SetCallback(MTaskList *list) { fCallback=list; }
    void SetNumRandomOffPositions(Int_t num=0) { fNumRandomOffPositions=num; }

    ClassDef(MSrcPosCalc, 0) // Calculates the source position in the camera
};

#endif
