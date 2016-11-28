#ifndef MARS_MStarPos
#define MARS_MStarPos

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MStarPos : public MParContainer
{
private:

    //Expected position on camera
    Float_t fMagExp;
    Float_t fXExp;    //[mm]
    Float_t fYExp;    //[mm]

    //Ideal position on camera
    Float_t fMagIdeal;
    Float_t fXIdeal;    //[mm]
    Float_t fYIdeal;    //[mm]

    //Info from calculation
    Float_t fMagCalc;
    Float_t fMaxCalc;            //[uA]
    Float_t fMeanXCalc;          //[mm]
    Float_t fMeanYCalc;          //[mm]
    Float_t fSigmaXCalc;         //[mm]
    Float_t fSigmaYCalc;         //[mm]
    Float_t fCorrXYCalc;          // correlation coefficient
    Float_t fXXErrCalc;           // error matrix of (fMeanXCalc,fMeanYCalc)
    Float_t fXYErrCalc;
    Float_t fYYErrCalc;

    //Info from Gauss fit
    Float_t fMagFit;
    Float_t fMaxFit;             //[uA]
    Float_t fMeanXFit;           //[mm]
    Float_t fMeanYFit;           //[mm]
    Float_t fSigmaXFit;          //[mm]
    Float_t fSigmaYFit;          //[mm]
    Float_t fCorrXYFit;          // correlation coefficient
    Float_t fXXErrFit;           // error matrix of (fMeanXFit,fMeanYFit)
    Float_t fXYErrFit;
    Float_t fYYErrFit;

    Float_t fChiSquareFit;
    Int_t   fNdofFit;


public:

    MStarPos(const char *name=NULL, const char *title=NULL);
    //~MStarPos();

    Float_t GetMagExp() {return fMagExp;}
    Float_t GetXExp() {return fXExp;}
    Float_t GetYExp() {return fYExp;}

    Float_t GetMagIdeal() {return fMagIdeal;}
    Float_t GetXIdeal() {return fXIdeal;}
    Float_t GetYIdeal() {return fYIdeal;}

    Float_t GetMagCalc() {return fMagCalc;}
    Float_t GetMaxCalc() {return fMaxCalc;}
    Float_t GetMeanXCalc() {return fMeanXCalc;}
    Float_t GetMeanYCalc() {return fMeanYCalc;}
    Float_t GetSigmaXCalc() {return fSigmaXCalc;}
    Float_t GetSigmaYCalc() {return fSigmaYCalc;}
    Float_t GetCorrXYCalc()        {return fCorrXYCalc;}
    Float_t GetXXErrCalc()         {return fXXErrCalc;}
    Float_t GetXYErrCalc()         {return fXYErrCalc;}
    Float_t GetYYErrCalc()         {return fYYErrCalc;}

    
    // getters for the Gauss fit
    Float_t GetMagFit()           {return fMagFit;}
    Float_t GetMaxFit()           {return fMaxFit;}
    Float_t GetMeanXFit()         {return fMeanXFit;}
    Float_t GetMeanYFit()         {return fMeanYFit;}
    Float_t GetSigmaXFit()        {return fSigmaXFit;}
    Float_t GetSigmaYFit()        {return fSigmaYFit;}
    Float_t GetCorrXYFit()        {return fCorrXYFit;}
    Float_t GetXXErrFit()         {return fXXErrFit;}
    Float_t GetXYErrFit()         {return fXYErrFit;}
    Float_t GetYYErrFit()         {return fYYErrFit;}
    Float_t GetChiSquareFit()     {return fChiSquareFit;}
    UInt_t  GetNdofFit()          {return fNdofFit;}
    Float_t GetChiSquareNdofFit() {return fChiSquareFit/fNdofFit;}

    // -----
    Float_t GetMeanX() {return fMeanXFit!=0?fMeanXFit:fMeanXCalc;}
    Float_t GetMeanY() {return fMeanYFit!=0?fMeanYFit:fMeanYCalc;}

    Float_t GetSigmaX() {return fSigmaXFit!=0?fSigmaXFit:fSigmaXCalc;}
    Float_t GetSigmaY() {return fSigmaYFit!=0?fSigmaYFit:fSigmaYCalc;}


    void Reset();

    void SetExpValues(Float_t mag, Float_t x, Float_t y);

    void SetIdealValues(Float_t mag, Float_t x, Float_t y);

    void SetCalcValues(Float_t mag,    Float_t max,    Float_t x, Float_t y, 
                       Float_t sigmaX, Float_t sigmaY, Float_t correlation, 
		       Float_t xx,     Float_t xy,     Float_t yy);

    void SetFitValues(Float_t mag,    Float_t max,    Float_t x, Float_t y, 
                      Float_t sigmaX, Float_t sigmaY, Float_t correlation, 
                      Float_t xx,     Float_t xy,     Float_t yy,
                      Float_t chi,   Int_t ndof);

    void Paint(Option_t *opt=NULL);
    void Print(Option_t *opt=NULL) const;

    ClassDef(MStarPos, 1) // Container that holds the star information in the PMT camera
};

#endif


