#ifndef MARS_MReflector
#define MARS_MReflector

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

class MQuaternion;
class MMirror;

class MReflector : public MParContainer
{
private:
    //Simple Array               // 5.1s
    TObjArray fMirrors;          // 6.1s   (Pointer)
    //TObjArray fMirrors;        // 6.1s   (GetObjectRef)
    //TObjArray fMirrors;        // 8.3s   (Next)
    //TObjArray fMirrors;        // 10.1s  (UncheckedAt)
    //TList fMirrors;            // 10.7s
    //TOrdCollection fMirrors;   // 23.4s

    Double_t fMaxR;

    void InitMaxR();

    // Helper for I/O
    MMirror *EvalTokens(TObjArray &arr, Double_t defpsf) const;

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MReflector(const char *name=NULL, const char *title=NULL);

    const MMirror **GetFirstPtr() const;
    const UInt_t GetNumMirrors() const;

    const MMirror *GetMirror(UInt_t idx) const { return idx>=GetNumMirrors()?0:*(GetFirstPtr()+idx); }

    Bool_t ReadFile(TString fname, Double_t defpsf=-1);
    Bool_t WriteFile(TString fname) const;

    Double_t GetMaxR() const { return fMaxR; }
    Double_t GetA() const;

    virtual Bool_t CanHit(const MQuaternion &p) const;

    Int_t ExecuteReflector(MQuaternion &p, MQuaternion &u) const;

    void SetSigmaPSF(Double_t psf);

    // TObject
    void Paint(Option_t *o);
    void Print(Option_t *o) const;

    ClassDef(MReflector, 1) // Parameter container storing a collection of several mirrors (reflector)
};
    
#endif
