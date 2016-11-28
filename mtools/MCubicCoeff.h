#ifndef MARS_MCubicCoeff
#define MARS_MCubicCoeff

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

class MCubicCoeff : public TObject
{
 private:
    Double_t fX;     // abscissa
    Double_t fXNext; // abscissa of the next point
    Double_t fA;     // 3rd order coeff
    Double_t fB;     // 2nd order coeff
    Double_t fC;     // 1st order coeff
    Double_t fY;     // constant term
    Double_t fYNext; // value in the next point
    Double_t fH;     // interval width
    Double_t fMin;   // minimum value
    Double_t fMax;   // maximum value
    Double_t fAbMin; // abscissa of the min
    Double_t fAbMax; // abscissa of the max
 public:
    MCubicCoeff(){}
    MCubicCoeff(Double_t x, Double_t xNext, Double_t y, Double_t yNext,
		Double_t a, Double_t b, Double_t c);
    Double_t GetA()   { return fA; }
    Double_t GetB()   { return fB; }
    Double_t GetC()   { return fC; }
    Double_t GetMin() { return fMin; }
    Double_t GetMax() { return fMax; }
    Double_t GetAbMin() { return fAbMin; }
    Double_t GetAbMax() { return fAbMax; }
    Double_t Eval(Double_t x); //Evaluate the spline at a point x
    Bool_t   EvalMinMax();     //Finds min & max
    Short_t  FindCardanRoot(Double_t y, Double_t *x); //Evaluate the abscissa of the spline given y 
    Bool_t   IsIn(Double_t x);

    ClassDef(MCubicCoeff, 0)  //Class to contain spline coefficients
};

#endif
