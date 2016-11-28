#ifndef MARS_MDispCalc
#define MARS_MDispCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

class MSrcPosCam;
class MHillas;
class MHillasExt;
class MNewImagePar;
class MMcEvt;
class MPointingPos;
class MImageParDisp;
class MDispParameters;
class MHMatrix;
class MParList;

class MDispCalc : public MTask
{
private:

    MSrcPosCam      *fSrcPos;
    MHillas         *fHil;
    MHillasExt      *fHilExt;
    MNewImagePar    *fNewPar;
    MMcEvt          *fMcEvt;
    MPointingPos    *fPointing;        
    
    MImageParDisp   *fImageParDisp;    //! output container for Disp value
    MDispParameters *fDispParameters;  //!  container for Disp parameters

    TString  fImageParDispName;    // name of container to store Disp
    TString  fDispParametersName;  // name of container for Disp parameters

    Double_t fLogSize0;            // Variables introduced in the Disp expression
    Double_t fLength0;             // to shift the minimization around the mean
    Double_t fWidth0;              // values of the variables, so it makes easier
    Double_t fConc0;               // to MINUIT to converge. Default values are set
    Double_t fLeakage10;           // in the constructor (to their standard mean 
    Double_t fLeakage20;           // distribution values) but can be changed with
                                   // the corresponding set function.

    Double_t fMm2Deg;              // conversion factor from mm to deg

    MHMatrix *fMatrix;             // matrix defined to have as much columns as 
                                   // variables wanted to parameterize Disp,
                                   // and as much rows as events used for the 
                                   // optimization (filled at MFindDisp)

    TArrayI  fMap;                 // array storing the matrix mapping column
                                   // numbers corresponding to each variable
                                   // added to fMatrix


    Int_t PreProcess(MParList *pList);
    Int_t Process();

    Double_t GetVal(Int_t i) const;        // get value of variable entered in the
                                           // matrix at column fMap[i]


public:

    MDispCalc(const char *imagepardispname = "MImageParDisp",
	      const char *dispname         = "MDisp",
              const char *name=NULL, const char *title=NULL);
    ~MDispCalc();

    void InitMapping(MHMatrix *mat);       // define the matrix of variables
                                           // needed for the Disp parameterization 

    Double_t Calc(TArrayD &imagevar);      // calculate Disp with a given expression

    void SetLogSize0(Double_t newmeanval)  { fLogSize0  = newmeanval; }
    void SetWidth0(Double_t newmeanval)    { fWidth0    = newmeanval; }
    void SetLength0(Double_t newmeanval)   { fLength0   = newmeanval; }
    void SetConc0(Double_t newmeanval)     { fConc0     = newmeanval; }
    void SetLeakage10(Double_t newmeanval) { fLeakage10 = newmeanval; }
    void SetLeakage20(Double_t newmeanval) { fLeakage20 = newmeanval; }

    MDispParameters *GetDispParameters()   { return fDispParameters; }

    ClassDef(MDispCalc, 0) // Task to evaluate Disp
};

#endif












