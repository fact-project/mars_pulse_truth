#ifndef MARS_MSimulatedAnnealing
#define MARS_MSimulatedAnnealing

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TMatrix
#include <TMatrix.h>
#endif

#ifndef ROOT_TVector
#include <TVector.h>
#endif

class MHSimulatedAnnealing;
class TRandom;

class MSimulatedAnnealing : public TObject
{
private:

    static const Float_t gsYtryStr;  // Fixed high value to keep the simplex inside the borders
    static const Float_t gsYtryCon;  // Fixed high value to keep the simplex inside the borders
    static const Int_t   gsMaxDim;   // Fixed maximum number of dimensions
    static const Int_t   gsMaxStep;  // Fixed maximum number of loops with temperature=0

    MHSimulatedAnnealing *fResult;   //! The histogram output container

    TRandom *fRandom;                // The random number generator -> random numbers between 0 and 1
  
    Real_t   fTolerance;             // The convergence break condition
  
    UShort_t fNdim;                  // The number of parameters 
    UShort_t fMpts;                  // The number of simplex points (=fNdim+1)
  
    UShort_t fNumberOfMoves;         // The total number of moves (== CPU time) 

    Real_t   fStartTemperature;      // The start temperature -> will slowly get decreased to 0

    Bool_t   fFullStorage;           // kTRUE -> the whole simplex gets stored in MHSimlutedAnnealing
    Bool_t   fInit;                  // kTRUE -> initialization was succesful

    TMatrix fP;                      // The (ndim+1,ndim) matrix containing the simplex 

    TVector fPsum;                   // The sum of each point of the simplex
    
    TVector fP0;                     // The boundary conditions on the weak side
    TVector fP1;                     // The boundary conditions on the strong side
    TVector fY;                      // The array containing the function evaluation results

    Real_t  fYb;                     // The best function evaluation value ever found
    Real_t  fYconv;                  // The function evaluation value at the convergence point
    
    TVector fPb;                     // The parameters belonging to fYb
    TVector fPconv;                  // The parameters belonging to fYconv
    
    Int_t Amebsa(Int_t iter,
                 const Real_t temp); // The function deciding if the simplex has to get reflected, expanded or contracted

    Real_t   Amotsa(const Float_t  fac,
                    const UShort_t ihi,
                    Real_t &yhi,
                    const Real_t temp); // The function reflecting, expanding and contracting the simplex: fac=-1 -> reflection, fac=0.5 -> contraction, fac=2.0 -> expansion

    void     GetPsum();              
    
protected:
  
    virtual Float_t FunctionToMinimize(const TVector &arr); // The optimization function  

public:
    enum BorderFlag_t { kENoBorder, kEStrictBorder, kEContractBorder };
    enum Verbosity_t  { kEDefault, kEVerbose, kEDebug };

private:
    BorderFlag_t fBorder;         
    Verbosity_t  fVerbose;        

public:
    MSimulatedAnnealing();
    virtual ~MSimulatedAnnealing();

    void ModifyTolerance(Float_t tol)          { fTolerance = tol;  }
    void ModifyBorderFlag(BorderFlag_t border) { fBorder    = border; }
    
    Bool_t Initialize(const TMatrix &p,  const TVector &y,
                      const TVector &p0, const TVector &p1);
  
    void SetNumberOfMoves(UShort_t moves)      { fNumberOfMoves    = moves;  }
    void SetStartTemperature(Float_t temp)     { fStartTemperature = temp;   }
    void SetFullStorage()                      { fFullStorage      = kTRUE;  }  
    void SetVerbosityLevel(Verbosity_t level)  { fVerbose          = level;  }
    void SetRandom(TRandom *rand)              { fRandom           = rand;   }

    const TVector &GetPb()    const            { return fPb;    }
    Float_t GetYb()           const            { return fYb;    }

    const TVector &GetPconv() const            { return fPconv; }    
    Float_t GetYconv()        const            { return fYconv; }  
  
  
    MHSimulatedAnnealing *GetResult()          { return fResult; }
  
    Bool_t RunMinimization();
  
    ClassDef(MSimulatedAnnealing,1)  // Class to perform a Simulated Annealing Minimization
};
    
#endif
