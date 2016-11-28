/* This macro is defined as a class for debugging (with CInt) reasons

To use it at the root prompt type:

root [0] .L spline.C
root [1] TestSpline::sp()
*/
/* Example of a spline. You can use it as Test. If you think there are some
   bugs in the MCubicSpline class please mail to: raducci@fisica.uniud.it */

class TestSpline
{
public:
    void sp();
};

void TestSpline::sp()
{
gROOT -> Reset();

//Here are defined the points. X goes from 0 to 14 (as the fadc slices...)
//Y are arbitrary values

/* User Change */
const Byte_t y[]={0x0F,0x10,0x2F,0x7F,0xAA,0x6C,0x14,0x13,0x15,0x18,0x21,0x12,0x11,0x14,0x13};
/* End user Change */
const Byte_t x[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E};

/*This cast is needed only to show graphically the output. Don' t needed if you
  use the spline to calc the arrival times       */

Int_t *newX = new Int_t[15];
Int_t *newY = new Int_t[15];

for (Int_t i = 0; i < 15; i++)
{
    newX[i] = (Int_t) x[i];
    newY[i] = (Int_t) y[i];
}

//Canvas to display output
TCanvas *c = new TCanvas ("c1","c1",800,600);

//Graph containting only the points (displayed as stars)
TGraph *g1 = new TGraph(15,newX,newY);

g1 -> Draw("A*");

//Spline constructor(specialized for 15 slices using Bytes as values. There exist another constructor.
MCubicSpline *s = new MCubicSpline(y);

//*spline and *ab are two arrays containing some values evaluated from the spline
Double_t *spline = new Double_t[139];
Double_t *ab     = new Double_t[139];
Double_t step = 0.0;

for (Int_t i = 0; i < 139; i++)
{
    spline[i] = s->Eval(step);
    ab[i] = step;
    step += 0.1;
}

//Graph of the sline. The points calculated are joined with a red line. If the stars lie
//on the red line, then the Spline class is working properly
TGraph *g2 = new TGraph(139,ab,spline);

g2 -> SetLineColor(2);
g2 -> Draw("C");

//Maximum and minimum evaluation
Double_t *mm   = new Double_t[2];
Double_t *abmm = new Double_t[2];

mm[0] = s->EvalMin();
mm[1] = s->EvalMax();
abmm[0] = s->EvalAbMin();
abmm[1] = s->EvalAbMax();

//Display the max and the min using two black squares. If they lie on the max and min
//of the red line, then the Spline class is working properly

TGraph *g3 = new TGraph(2,abmm,mm);

g3 -> SetMarkerStyle(21);
g3 -> Draw("P");

//Test of the Cardan formula. Find the point(abval) where the Spline value is val
Double_t val = 82.5;
Double_t abval = s->FindVal(val, abmm[1], 'l');

//Display this point. Again, if it lies on the red line, then we are right.
//It's a black triangle

TGraph *g4 = new TGraph(1,&abval,&val);

g4 -> SetMarkerStyle(22);
g4 -> Draw("P");

//Free memory
s->~MCubicSpline();
delete [] newX;
delete [] newY;
delete [] spline;
delete [] ab;
delete [] mm;
delete [] abmm;

} 
