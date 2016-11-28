#ifndef MARS_MCubicSpline
#define MARS_MCubicSpline

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TObjArray
#include "TObjArray.h"
#endif

class MCubicCoeff;

class MCubicSpline : public TObject
{
 private:
    TObjArray *fCoeff; //array of the coefficients

    void Init(const Byte_t *y, const Byte_t *x, Bool_t areAllEq, Int_t n, Double_t begSD, Double_t endSD);

 public:
    MCubicSpline(const Byte_t *y, const Byte_t *x, Bool_t areAllEq, Int_t n, Double_t begSD=0.0, Double_t endSD=0.0);
    MCubicSpline(const Byte_t *y);
    ~MCubicSpline();
    Double_t Eval(Double_t x); //Eval the spline at a point x
    Double_t EvalMax();   //Eval the max
    Double_t EvalMin();   //Eval the min
    Double_t EvalAbMax(); //Eval the abscissa of the max
    Double_t EvalAbMin(); //Eval the abscissa of the min
    Double_t FindVal(Double_t y, Double_t x0, Char_t direction); //Finds the abscissa where the spline reaches y
    
    ClassDef(MCubicSpline, 0)  //Class to contain spline coefficients
};

#endif
