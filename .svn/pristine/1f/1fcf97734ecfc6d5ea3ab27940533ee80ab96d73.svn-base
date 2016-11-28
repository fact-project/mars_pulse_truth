#ifndef MARS_MHSimulatedAnnealing
#define MARS_MHSimulatedAnnealing
///////////////////////////////////////////////////////////////////////////////
//
//  MHSimulatedAnnealing
//
///////////////////////////////////////////////////////////////////////////////
#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TH2
#include <TH2.h>
#endif

#ifndef ROOT_TMatrix
#include <TMatrix.h>
#endif

#ifndef ROOT_TVector
#include <TVector.h>
#endif


class MHSimulatedAnnealing : public MH
{
private:
    UShort_t fDim;             // The dimension of the whole thing
    UShort_t fMoves;           // The total number of moves

    TObjArray *fTimeEvolution; //-> Display the time evolution of the simplex in TH1D's

    TH2F     fBestEver;        // The best values ever found during search
    TH1F     fBestFuncEval;    // The best function values ever found during search

public:

    MHSimulatedAnnealing(UShort_t moves = 0,UShort_t ndim = 0, 
	                 const char *name=NULL, const char *title=NULL);
    ~MHSimulatedAnnealing();

    void InitFullSimplex();
    Bool_t StoreFullSimplex(const TMatrix &p, const UShort_t move);
    Bool_t StoreBestValueEver(const TVector &y, const Float_t yb, const UShort_t move);
    
    Bool_t ChangeTitle(const UShort_t index, const char* title);
    void ChangeFuncTitle(const char* title);    
    
    TObjArray *GetTimeEvolution() const   { return fTimeEvolution; }
    const TH2F &GetBestEver()     const   { return fBestEver; }
    const TH1F &GetBestFuncEval() const   { return fBestFuncEval; }
    
    void Draw(Option_t *opt=NULL);
    TObject *DrawClone(Option_t *opt=NULL) const;
    
    ClassDef(MHSimulatedAnnealing,1) // Storage Histogram Container for Cuteval Results
};
    
#endif
