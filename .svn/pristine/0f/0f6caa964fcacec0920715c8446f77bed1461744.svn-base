#ifndef MARS_MBinning
#define MARS_MBinning

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class TH1;
class TAxis;

class MParList;

class MBinning : public MParContainer
{
public:
    static const TString gsDefName;
    static const TString gsDefTitle;

private:

    TArrayD fEdges;

    Byte_t  fType;

    void StreamPrimitive(std::ostream &out) const;

    enum {
        kIsDefault,
        kIsLinear,
        kIsLogarithmic,
        kIsCosinic,
        kIsUserArray
    };

public:
    MBinning(const char *name=NULL, const char *title=NULL);
    MBinning(Int_t nbins, Axis_t lo, Axis_t hi, const char *name=0, const char *opt="", const char *title=NULL);
    MBinning(const MBinning &bins, const char *name=NULL, const char *title=NULL);
    MBinning(const TH1 &h, const Char_t axis='x', const char *name=0, const char *title=0);
    MBinning(const TAxis &a, const char *name=0, const char *title=0);
    MBinning(const TArrayD &a, const char *name=0, const char *title=0);

    void Copy(TObject &named) const
    {
        MBinning &bins = (MBinning&)named;
        bins.SetEdges(*this);
    }

    void SetEdges(const TArrayD &arr)
    {
        fEdges = arr;
        fType  = kIsUserArray;
    }

    Bool_t SetEdges(const MParList &list, const char *name=0);
    Bool_t SetEdgesRaw(const char *txt);
    //Bool_t SetEdges(const char *txt);
    void SetEdges(const TAxis &axe);
    void SetEdges(const MBinning &bins) { SetEdges(bins.fEdges); fType = bins.fType; fTitle = bins.fTitle; }
    void SetEdges(const TH1 &h, const Char_t axis='x');
    void SetEdges(Int_t nbins, const Axis_t lo, const Axis_t up) { SetEdgesLin(nbins, lo, up); }
    void SetEdges(const Int_t nbins, const Axis_t lo, Axis_t up, const char *opt);
    void SetEdgesLin(Int_t nbins, Axis_t lo, Axis_t up);
    void SetEdgesLog(Int_t nbins, Axis_t lo, Axis_t up);
    void SetEdgesCos(Int_t nbins, Axis_t lo, Axis_t up);
    void SetEdgesASin(Int_t nbins, Axis_t lo, Axis_t up);

    Int_t FindLoEdge(Double_t val) const
    {
        if (val<GetEdgeLo() || val>=GetEdgeHi())
            return -1;

	for (int i=1; i<fEdges.GetSize(); i++)
        {
            if (fEdges[i] >= val)
                return i-1;
        }
        return -1;
    }
    Int_t FindHiEdge(Double_t val) const
    {
        const Int_t i = FindLoEdge(val);
        return i<0 ? -1 : i+1;
    }

    Double_t GetEdgeLo() const { return fEdges[0]; }
    Double_t GetEdgeHi() const { return fEdges[fEdges.GetSize()-1]; }

    Int_t GetNumEdges() const  { return fEdges.GetSize(); }
    Int_t GetNumBins() const   { return fEdges.GetSize()-1; }

    const Double_t *GetEdges() const { return fEdges.GetArray(); }
    const TArrayD &GetEdgesD() const { return fEdges; }

    Double_t operator[](Int_t i) const { return fEdges[i]; }

    void AddEdge(Axis_t up);
    void RemoveFirstEdge();
    void RemoveLastEdge();

    Bool_t IsLinear() const      { return fType==kIsLinear; }
    Bool_t IsLogarithmic() const { return fType==kIsLogarithmic; }
    Bool_t IsCosinic() const     { return fType==kIsCosinic; }
    Bool_t IsDefault() const     { return fType==kIsDefault; }
    Bool_t IsUserArray() const   { return fType==kIsUserArray; }

    Bool_t HasTitle() const      { return gsDefTitle!=fTitle; }

    void Apply(TH1 &) const;

    void Print(Option_t *o="") const; //*MENU*

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    ClassDef(MBinning, 1) //Container to store the binning of a histogram
};

#endif
