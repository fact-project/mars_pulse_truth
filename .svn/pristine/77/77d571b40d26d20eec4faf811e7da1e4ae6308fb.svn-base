#ifndef MARS_MExtralgoDigitalFilter
#define MARS_MExtralgoDigitalFilter

#ifndef ROOT_TMatrix
#include <TMatrix.h>
#endif

class TH1;
class TH2;
class TH1F;
class TH2F;
class TArrayF;

class MExtralgoDigitalFilter
{
private:
    // Input
    const Float_t *fVal;
    Int_t fNum;

    Float_t const *fWeightsAmp;
    Float_t const *fWeightsTime;
    Float_t const *fPulseShape;

    const TMatrix *fAinv;

    const Int_t fWeightsPerBin; // Number of weights per data bin
    const Int_t fWindowSize;

    // Result
    Float_t fTime;
    Float_t fTimeDev;
    Float_t fSignal;
    Float_t fSignalDev;

    Float_t GetChisq(const Int_t maxp, const Int_t frac, const Float_t sum) const;

    inline Double_t ChiSq(const Double_t sum, const Int_t startv, const Int_t startw=0) const
    {
        //
        // Slide with a window of size windowsize over the sample
        // and multiply the entries with the corresponding weights
        //
        Double_t chisq = 0;

        // Shift the start of the weight to the center of sample 0
        Float_t const *w = fPulseShape + startw;

        const Float_t *beg = fVal+startv;
        for (Float_t const *pex=beg; pex<beg+fWindowSize; pex++)
        {
            const Double_t c = *w - *pex/sum;
            chisq += c*c;
            w += fWeightsPerBin;
        }
        return chisq;
    }

    // Weights: Weights to evaluate
    // Startv: Index of first bin of data
    // startw: Offset on the weights
    inline Double_t Eval(Float_t const *weights, const Int_t startv, const Int_t startw=0) const
    {
        //
        // Slide with a window of size windowsize over the sample
        // and multiply the entries with the corresponding weights
        //
        Double_t sum = 0;

        // Shift the start of the weight to the center of sample 0
        Float_t const *w = weights + startw;

        const Float_t *beg = fVal+startv;
        for (Float_t const *pex=beg; pex<beg+fWindowSize; pex++)
        {
            sum += *w * *pex;
            w += fWeightsPerBin;
        }
        return sum;
    }

    inline void AlignIntoLimits(Int_t &maxp, Int_t &frac) const
    {
        // Align maxp into available range (TO BE CHECKED)
        if (maxp < 0)
        {
            maxp = 0;
            frac = fWeightsPerBin/2-1; // Assume peak at the end of the last slice
        }
        if (maxp > fNum-fWindowSize)
        {
            maxp = fNum-fWindowSize;
            frac = -fWeightsPerBin/2; // Assume peak at the beginning of the first slice
        }
    }

    Int_t AlignExtractionWindow(Int_t &maxp, Int_t &frac, const Double_t ampsum);
    void  AlignExtractionWindow(Int_t &maxp, Int_t &frac)
    {
        const Double_t amp = Eval(fWeightsAmp, maxp, frac);
        if (amp!=0)
            AlignExtractionWindow(maxp, frac, amp);
    }

public:
    MExtralgoDigitalFilter(Int_t res, Int_t windowsize, Float_t *wa, Float_t *wt, Float_t *ps=0, TMatrix *ainv=0)
        : fVal(0), fNum(0), fWeightsAmp(wa+res/2), fWeightsTime(wt+res/2),
        fPulseShape(ps), fAinv(ainv), fWeightsPerBin(res), fWindowSize(windowsize),
        fTime(0), fTimeDev(-1), fSignal(0), fSignalDev(-1)
    {
    }

    void SetData(Int_t n, Float_t const *val) { fNum=n; fVal=val; }

    Float_t GetTime() const          { return fTime; }
    Float_t GetSignal() const        { return fSignal; }

    Float_t GetTimeDev() const       { return fTimeDev; }
    Float_t GetSignalDev() const     { return fSignalDev; }

    void GetSignal(Float_t &sig, Float_t &dsig) const { sig=fSignal; dsig=fSignalDev; }
    void GetTime(Float_t &sig, Float_t &dsig) const   { sig=fTime; dsig=fTimeDev; }

    Float_t ExtractNoise() const;
    void Extract(Int_t maxpos=-1);

    static Int_t CalculateWeights(TH1 &shape, const TH2 &autocorr, TArrayF &wa, TArrayF &wt, Int_t wpb=-1);
    static Int_t CalculateWeights2(TH1 &shape, const TH2 &autocorr, TArrayF &wa, TArrayF &wt, Int_t wpb=-1);
};


#endif
