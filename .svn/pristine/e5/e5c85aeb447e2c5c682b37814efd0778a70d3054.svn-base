#ifndef MARS_MFMagicCuts
#define MARS_MFMagicCuts

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class MParList;

class MGeomCam;
class MHillas;
class MHillasSrc;
class MHillasExt;
class MNewImagePar;
class MParameterD;
class MPointingPos;
class MHMatrix;

class MFMagicCuts : public MFilter
{
public:
    // Possible kind of theta cuts
    enum ThetaCut_t {
        kNone  =BIT(0),
        kOn    =BIT(1),
        kOff   =BIT(2),
        kWobble=kOn|kOff
    };
    // Possible kind of hadronness cuts
    enum HadronnessCut_t {
        kNoCut     =BIT(0),
        kArea      =BIT(1),
        kHadronness=BIT(2),
        kAreaLin   =BIT(3),
        kAll       =kArea|kHadronness
    };

private:
    // Elements for mapping. kLastElement must not be used and must be
    // the last on in the list. It is used as counter for fMap.
    enum {
        kESize, kEAlpha, kEAlphaAnti, kEArea, kEDist, kEDistAnti,
        kEM3Long, kEM3LongAnti, kEWdivL, kELeakage, kESlope,
        kESlopeAnti, kEHadronness, kESign, kEDisp, kEGhostbuster,
        kLastElement
    };

    MGeomCam       *fGeom;              //! Conversion factor from mm to deg
    MHillas        *fHil;               //! Pointer to MHillas container
    MHillasSrc     *fHilSrc;            //! Pointer to MHillasSrc container
    MHillasSrc     *fHilAnti;           //! Pointer to MHillasSrc container called MHillasSrcAnti
    MHillasExt     *fHilExt;            //! Pointer to MHillasExt container
    MNewImagePar   *fNewImgPar;         //! Pointer to MHillasExt container
    MParameterD    *fThetaSq;           //! Pointer to MParameterD container called ThetaSq
    MParameterD    *fDisp;              //! Pointer to MParameterD container called Disp
    MParameterD    *fGhostbuster;       //! Pointer to MParameterD container called Ghostbuster
    MParameterD    *fHadronness;        //! Pointer to MParameterD container called Hadronness

    Bool_t          fResult;            //! Result of the filter evaluation

    Int_t           fMap[kLastElement]; //! Mapping table for fast optimization
    MHMatrix       *fMatrix;            //! Matrix thorugh which the mapped elements are accessed

    TArrayD         fVariables;         // Coefficients of cuts

    ThetaCut_t      fThetaCut;          // Which kind of theta cut should be evaluated
    HadronnessCut_t fHadronnessCut;     // Which kind of hadronness cut should be evaluated
    Bool_t          fCalcDisp;          // Should we use Disp from the parameterlist?
    Bool_t          fCalcGhostbuster;   // Should we use Ghostbuster from the parameterlist?

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();

    // MFilter
    Bool_t IsExpressionTrue() const { return fResult; }

    // MFMagicCuts
    Double_t GetDisp(Double_t slope, Double_t lgsize) const;
    Bool_t   IsGhost(Double_t m3long, Double_t slope, Double_t dist) const;
    Double_t GetVal(Int_t i) const;
    TString  GetParam(Int_t i) const;
    Double_t GetThetaSq(Double_t p, Double_t d, Double_t a) const;

public:
    MFMagicCuts(const char *name=NULL, const char *title=NULL);

    // Getter
    Double_t GetThetaSqCut() const;

    // Setter
    void   SetThetaCut(ThetaCut_t c) { fThetaCut=c; }
    void   SetHadronnessCut(HadronnessCut_t c) { fHadronnessCut=c; }
    void   SetCalcDisp(Bool_t b=kTRUE) { fCalcDisp=b; }
    void   SetCalcGhostbuster(Bool_t b=kTRUE) { fCalcGhostbuster=b; }

    // MFMagicCuts
    void   InitMapping(MHMatrix *mat);
    void   StopMapping() { InitMapping(NULL); }

    Bool_t CoefficentsRead(const char *fname);
    Bool_t CoefficentsWrite(const char *fname) const;

    // MParContainer
    void   SetVariables(const TArrayD &arr);

    Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // TObject
    void   Print(Option_t *o="") const;

    ClassDef(MFMagicCuts, 2) // A filter to evaluate the MagicCuts
};

#endif
