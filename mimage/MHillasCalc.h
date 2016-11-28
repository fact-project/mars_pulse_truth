#ifndef MARS_MHillasCalc
#define MARS_MHillasCalc

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MHillasCalkc                                                            //
//                                                                         //
// Task to calculate Hillas Parameters                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif
#ifndef ROOT_TArrayL
#include <TArrayL.h>
#endif

class MGeomCam;
class MSignalCam;
class MHillas;
class MHillasExt;
class MHillasSrc;
class MImagePar;
class MNewImagePar;
class MNewImagePar2;
class MConcentration;
class MSrcPosCam;

class MHillasCalc : public MTask
{
    static const TString gsDefName;         // default name
    static const TString gsDefTitle;        // default title

    static const TString gsNameHillas;      // default name of the 'MHillas' container
    static const TString gsNameHillasExt;   // default name of the 'MHillasExt' container
    static const TString gsNameNewImagePar; // default name of the 'MNewImagePar' container
    static const TString gsNameNewImagePar2; // default name of the 'MNewImagePar' container
    static const TString gsNameConc;        // default name of the 'MConcentration' container
    static const TString gsNameImagePar;    // default name of the 'MImagePar' container
    static const TString gsNameHillasSrc;   // default name of the 'MHillasSrc' container
    static const TString gsNameSrcPosCam;   // default name of the 'MSrcPosCam' container

    const MGeomCam      *fGeomCam;          //! Camera Geometry used to calculate Hillas
    const MSignalCam    *fCerPhotEvt;       //! Cerenkov Photon Event used for calculation

    MHillas             *fHillas;           //! output container to store result
    MHillasExt          *fHillasExt;        //! output container to store result
    MHillasSrc          *fHillasSrc;        //! output container to store result
    MImagePar           *fImagePar;         //! output container to store result
    MNewImagePar        *fNewImgPar;        //! output container to store result
    MNewImagePar2       *fNewImgPar2;       //! output container to store result
    MConcentration      *fConc;             //! output container to store result

    TString              fNameHillas;       // name of the 'MHillas' container
    TString              fNameHillasExt;    // name of the 'MHillasExt' container
    TString              fNameHillasSrc;    // name of the 'MHillasSrc' container
    TString              fNameSrcPosCam;    // name of the 'MSrcPosCam' container
    TString              fNameConc;         // name of the 'MConcentration' container
    TString              fNameImagePar;     // name of the 'MImagePar' container
    TString              fNameNewImagePar;  // name of the 'MNewImagePar' container
    TString              fNameNewImagePar2; // name of the 'MNewImagePar' container

    TArrayL              fErrors;           //! Error counter. Do we have to change to Double?

    Int_t                fFlags;            // Flags defining the behaviour of MHillasCalc
    Short_t              fIdxIsland;        // Number of island to use for calculation

    // MParContainer
    void StreamPrimitive(std::ostream &out) const;

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

public:
    // Constructor
    MHillasCalc(const char *name=NULL, const char *title=NULL);

    // Flags
    enum CalcCont_t {
        kCalcHillas       = BIT(0),
        kCalcHillasExt    = BIT(1),
        kCalcHillasSrc    = BIT(2),
        kCalcNewImagePar  = BIT(3),
        kCalcNewImagePar2 = BIT(4),
        kCalcConc         = BIT(5),
        kCalcImagePar     = BIT(6)
    };

    // Setup flags
    void SetFlags(Int_t f) { fFlags  =  f; }
    void Enable(Int_t f)   { fFlags |=  f; }
    void Disable(Int_t f)  { fFlags &= ~f; }

    Bool_t TestFlag(CalcCont_t i) const { return fFlags&i; }
    Bool_t TestFlags(Int_t i) const     { return fFlags&i; }

    // Setup container names
    void SetNameHillas(const char *name)      { fNameHillas      = name; }
    void SetNameHillasExt(const char *name)   { fNameHillasExt   = name; }
    void SetNameHillasSrc(const char *name)   { fNameHillasSrc   = name; }
    void SetNameNewImagePar(const char *name) { fNameNewImagePar = name; }
    void SetNameNewImagePar2(const char *name){ fNameNewImagePar2 = name; }
    void SetNameConc(const char *name)        { fNameConc        = name; }
    void SetNameImagePar(const char *name)    { fNameImagePar    = name; }
    void SetNameSrcPosCam(const char *name)   { fNameSrcPosCam   = name; }

    // Setup island number
    void SetIdxIsland(Short_t idx) { fIdxIsland = idx; }

    // TObject
    void Print(Option_t *o="") const;

    ClassDef(MHillasCalc, 0) // Task to calculate Hillas and other image parameters
};

#endif
