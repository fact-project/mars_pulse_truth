/* ======================================================================== *\
!  $Name: not supported by cvs2svn $:$Id: MHCamera.h,v 1.69 2009-02-11 12:25:42 tbretz Exp $
\* ======================================================================== */
#ifndef MARS_MHCamera
#define MARS_MHCamera

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif
#ifndef ROOT_TArrayC
#include <TArrayC.h>
#endif
#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif
#ifndef ROOT_MArrayD
#include <MArrayD.h>
#endif
#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif
#ifndef ROOT_TCanvas
#include <TCanvas.h>
#endif

class TPaveStats;
class TProfile;

class MGeomCam;
class MCamEvent;
class MRflEvtData;
class MCerPhotEvt;
class MImgCleanStd;

class MHCamera : public TH1D
{
public:
    enum {
        // DO NOT USE BIT(14) (MStatusArray WORKAROUND!)
        // BIT(15)/BIT(16)/BIT(17) used by TH1
        kProfile            = BIT(18), // FIXME: When changing change max/min!
        kFreezed            = BIT(19),
        kNoLegend           = BIT(20),
        kNoScale            = BIT(21),
        kNoUnused           = BIT(22),
        kErrorMean          = BIT(23)/*,  // NO MORE BITS ALLOWED!
        kSqrtVariance       = BIT(21),
        kSinglePixelProfile = BIT(22)*/
    };
protected:
    MGeomCam      *fGeomCam;     // pointer to camera geometry (y-axis)
    TArrayC        fUsed;        // array containing flags
    TArrayI        fBinEntries;  // number of entries per bin

    TList         *fNotify;      //!

//#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,03)
    Bool_t         fFreezed;     //! Just a dummy!!!! ([Set,Is]Freezed)
//#endif

    Float_t fAbberation;

    void Init();
/*
    Stat_t Profile(Stat_t val) const
    {
        if (!TestBit(kProfile))
            return val;

        const Stat_t n = TH1D::GetEntries();
        return n>0 ? val/n : val;
    }
  */
    Int_t GetColor(Float_t val, Float_t min, Float_t max, Bool_t islog);

    void  PaintIndices(Int_t type);
    void  Update(Bool_t islog, Bool_t isbox, Bool_t iscol, Bool_t issame);
    void  UpdateLegend(Float_t min, Float_t max, Bool_t islog);

    TPaveStats *GetStatisticBox();

    Int_t GetPixelIndex(Int_t px, Int_t py, Float_t conv=1) const;

    void PaintAxisTitle();

    enum {
        kIsUsed = BIT(1)
    };

    void   ResetUsed(Int_t idx)    { CLRBIT(fUsed[idx], kIsUsed); }

    Bool_t FindVal(const TArrayI &arr, Int_t val) const
    {
        const Int_t n = arr.GetSize();
        if (n==0)
            return kTRUE;

        const Int_t *p = arr.GetArray();
        const Int_t *end = p+n;
        while (p<end)
            if (val==*p++)
                return kTRUE;

        return kFALSE;
    }
    Bool_t MatchSector(Int_t idx, const TArrayI &sector, const TArrayI &aidx) const;

    // This is a trick to remove TH1 entries from the context menu
    TH1 *Rebin(Int_t =2, const char * ="", const Double_t * =0) { return this; }
    TH1 *Rebin(Int_t =2, const char * ="") { return this; }
    void DrawPanel() {}

    Int_t Fill(Axis_t x);
    Int_t Fill(const char *, Stat_t) { return -1; }
    void  FillN(Int_t, const Axis_t *, const Double_t *, Int_t =1) {}
    void  FillN(Int_t, const Axis_t *, const Axis_t *,   const Double_t *, Int_t) {}

public:
    MHCamera();
    MHCamera(const MGeomCam &geom, const char *name="", const char *title="");
    ~MHCamera();

    TObject *Clone(const char *newname="") const;

    void SetGeometry(const MGeomCam &geom, const char *name="", const char *title="");
    const MGeomCam* GetGeometry() const { return fGeomCam; }

    Bool_t IsUsed(Int_t idx) const { return TESTBIT(fUsed[idx], kIsUsed); }
    void   SetUsed(Int_t idx)      { SETBIT(fUsed[idx], kIsUsed); }
    void   SetAllUsed()            { fUsed.Reset(BIT(kIsUsed)); }

    UInt_t GetNumUsedSector(const TArrayI &sector, const TArrayI &aidx) const;
    UInt_t GetNumUsed() const { return GetNumUsedSector(TArrayI(), TArrayI()); }
    UInt_t GetNumUsedSector(Int_t sector, Int_t aidx) const
    {
        return GetNumUsedSector(TArrayI(1, &sector), TArrayI(1, &aidx));
    }


    Int_t Fill(Axis_t x, Axis_t y, Stat_t w);
    Int_t Fill(Axis_t x, Stat_t w);

    //void     AddPixContent(Int_t idx) const { AddBinContent(idx+1); }
    //void     AddPixContent(Int_t idx, Stat_t w) const { AddBinContent(idx+1, w); }

    // This is a trick to remove TH1 entries from the context menu
    /*
     void Add(const TH1 *h1, const TH1 *h2, Double_t c1=1, Double_t c2=1) { TH1::Add(h1, h2, c1, c2); }
     void Add(TF1 *h1, Double_t c1=1) { TH1::Add(h1, c1); }
     void Add(const TH1 *h1, Double_t c1=1) { TH1::Add(h1, c1); }
     void Divide(TF1 *f1, Double_t c1=1) { TH1::Divide(f1, c1); }
     void Divide(const TH1 *h1) { TH1::Divide(h1); }
     void Divide(const TH1 *h1, const TH1 *h2, Double_t c1=1, Double_t c2=1, Option_t *option="") { TH1::Divide(h1, h2, c1, c2, option); }
     void Multiply(TF1 *h1, Double_t c1=1) { TH1::Multiply(h1, c1); }
     void Multiply(const TH1 *h1) { TH1::Multiply(h1); }
     void Multiply(const TH1 *h1, const TH1 *h2, Double_t c1=1, Double_t c2=1, Option_t *option="") { TH1::Multiply(h1, h2, c1, c2, option); }
     */

    void FitPanel() { TH1::FitPanel(); }

    virtual Double_t GetPixContent(Int_t idx) const { return GetBinContent(idx+1); }
    virtual void     AddCamDifference(const MCamEvent &evt, const MCamEvent &evt2, Int_t type=0, Stat_t weight=1);
    virtual void     AddCamRatio(const MCamEvent &evt, const MCamEvent &evt2, Int_t type=0, Stat_t weight=1);
    virtual void     AddCamContent(const MCamEvent &evt, Int_t type=0, Stat_t weight=1);
    virtual void     AddCamContent(const MHCamera &evt, Int_t type=0);
    virtual void     AddCamContent(const TArrayD &arr, const TArrayC *used=NULL);
    virtual void     AddCamContent(const MArrayD &arr, const TArrayC *used=NULL);
    virtual void     SetCamContent(const MCamEvent &evt, Int_t type=0)           { Reset(); AddCamContent(evt, type); }
    virtual void     SetCamContent(const MHCamera &cam, Int_t type=0)            { Reset(); AddCamContent(cam, type); }
    virtual void     SetCamContent(const TArrayD &evt, const TArrayC *used=NULL) { Reset(); AddCamContent(evt, used); }
    virtual void     SetCamContent(const MArrayD &evt, const TArrayC *used=NULL) { Reset(); AddCamContent(evt, used); }

    virtual void     SetCamError(const MCamEvent &evt, Int_t type=0);
    virtual void     SetUsed(const TArrayC &arr);

    virtual void     AddMedianShift(const MCamEvent &evt, Int_t type=0, Stat_t weight=1);
    virtual void     AddMeanShift(const MCamEvent &evt, Int_t type=0, Stat_t weight=1);

    virtual void     CntCamContent(const MCamEvent &evt, Double_t threshold, Int_t type=0, Bool_t isabove=kTRUE);
    virtual void     CntCamContent(const MCamEvent &evt, TArrayD threshold, Int_t type=0, Bool_t isabove=kTRUE);
    virtual void     CntCamContent(const TArrayD &evt, Double_t threshold, Bool_t ispos=kTRUE);
    virtual void     CntCamContent(const MCamEvent &event, Int_t type1, const MCamEvent &thresevt, Int_t type2, Double_t threshold, Bool_t isabove);

    virtual void     SetMaxCamContent(const MCamEvent &evt, Int_t type=0);
    virtual void     SetMinCamContent(const MCamEvent &evt, Int_t type=0);

    Stat_t   GetBinContent(Int_t bin) const;
    Stat_t   GetBinContent(Int_t binx, Int_t) const { return GetBinContent(binx); }
    Stat_t   GetBinContent(Int_t binx, Int_t, Int_t) const { return GetBinContent(binx); }
    Stat_t   GetBinError(Int_t bin) const;
    Stat_t   GetBinError(Int_t binx, Int_t) const { return GetBinError(binx); }
    Stat_t   GetBinError(Int_t binx, Int_t, Int_t) const { return GetBinError(binx); }

    Double_t GetMinimum(Bool_t ball) const { return GetMinimumSectors(TArrayI(), TArrayI(), ball); }
    Double_t GetMaximum(Bool_t ball) const { return GetMaximumSectors(TArrayI(), TArrayI(), ball); }

    Double_t GetMinimum(Double_t /*gt*/) const { return GetMinimumSectors(TArrayI(), TArrayI(), kFALSE); } // FIXME: To be done: Minimum greater than
    Double_t GetMaximum(Double_t /*lt*/) const { return GetMaximumSectors(TArrayI(), TArrayI(), kFALSE); } // FIXME: To be done: Maximum lower than

    Double_t GetMinimum() const { return GetMinimum(0.0); } // FIXME: To be done: Minimum greater than
    Double_t GetMaximum() const { return GetMaximum(0.0); } // FIXME: To be done: Maximum lower than

    Double_t GetMinimumSector(Int_t sector, Int_t aidx, Bool_t ball=kFALSE) const
    {
        return GetMinimumSectors(TArrayI(1, &sector), TArrayI(1, &aidx), ball);
    }
    Double_t GetMaximumSector(Int_t sector, Int_t aidx, Bool_t ball=kFALSE) const
    {
        return GetMaximumSectors(TArrayI(1, &sector), TArrayI(1, &aidx), ball);
    }
    Double_t GetMinimumSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t ball=kFALSE) const;
    Double_t GetMaximumSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t ball=kFALSE) const;

    Int_t    GetNumBinsAboveThreshold(Double_t t=0) const;
    Int_t    GetNumBinsBelowThreshold(Double_t t=0) const;

    void     SetLevels(const TArrayF &arr);

    void     FillRandom(const char *fname, Int_t ntimes=5000) { TH1::FillRandom(fname, ntimes); }
    void     FillRandom(TH1 *h, Int_t ntimes=5000) { TH1::FillRandom(h, ntimes); }
    void     FillRandom();

    void     PrintInfo() const { Print(""); } // *MENU*
    void     Reset(Option_t *);
    void     Reset() { Reset(""); } // *MENU*

    void     Print(Option_t *) const;
    void     Paint(Option_t *option="");
    void     Draw(Option_t *option="");
    void     DrawProjection (Int_t fit=0) const;
    void     DrawRadialProfile()           const;
    void     DrawAzimuthProfile()          const;

    void     SavePrimitive(std::ostream &out, Option_t *);
    void     SavePrimitive(std::ofstream &out, Option_t *);
    Int_t    DistancetoPrimitive(Int_t px, Int_t py);
    char    *GetObjectInfo(Int_t px, Int_t py) const;
    void     ExecuteEvent(Int_t event, Int_t px, Int_t py);
    void     SetDrawOption(Option_t *option); //*MENU*

    void     SetPalette(Int_t ncolors, Int_t *colors);

    void     SetPrettyPalette(); // *MENU*
    void     SetDeepBlueSeaPalette(); // *MENU*
    void     SetInvDeepBlueSeaPalette(); // *MENU*

    void     SetFreezed(Bool_t f=kTRUE) { f ? SetBit(kFreezed) : ResetBit(kFreezed); } // *TOGGLE* *GETTER=IsFreezed
    Bool_t   IsFreezed() const { return TestBit(kFreezed); }
    //void  SetOptStat(Int_t os=-1) { fOptStat = os; } // *MENU*

    void     SetErrorSpread(Bool_t f=kTRUE) { f ? ResetBit(kErrorMean) : SetBit(kErrorMean); } // *TOGGLE* *GETTER=IsErrorSpread
    Bool_t   IsErrorSpread() const { return !TestBit(kErrorMean); }

    void     SetAbberation(Float_t f=0.0713) { fAbberation=f; } // *MENU*

    void     SetAutoScale() { fMinimum = fMaximum = -1111; } // *MENU*
    void     SetMinMax(Double_t min=-1111, Double_t max=-1111) { SetMinimum(min); SetMaximum(max); } // *MENU*

    void     AddNotify(TObject *event);

    Stat_t   GetMean(Bool_t ball) const   { return GetMeanSectors(TArrayI(), TArrayI(), ball); }
    Stat_t   GetMedian(Bool_t ball) const { return GetMedianSectors(TArrayI(), TArrayI(), ball); }
    Stat_t   GetRMS(Bool_t ball)  const   { return GetRmsSectors(TArrayI(), TArrayI(), ball); }
    Stat_t   GetDev(Bool_t ball)  const   { return GetDevSectors(TArrayI(), TArrayI(), ball); }

    Stat_t   GetMean(Int_t=0) const   { return GetMeanSectors(TArrayI(), TArrayI(), kFALSE); }
    Stat_t   GetMedian(Int_t=0) const { return GetMedianSectors(TArrayI(), TArrayI(), kFALSE); }
    Stat_t   GetRMS(Int_t=0)  const   { return GetRmsSectors(TArrayI(), TArrayI(), kFALSE); }
    Stat_t   GetDev(Int_t=0)  const   { return GetRmsSectors(TArrayI(), TArrayI(), kFALSE); }

    Stat_t   GetMeanSector(Int_t sector, Int_t aidx, Bool_t ball=kFALSE) const
    {
        return GetMeanSectors(TArrayI(1, &sector), TArrayI(1, &aidx), ball);
    }
    Stat_t   GetMedianSector(Int_t sector, Int_t aidx, Bool_t ball=kFALSE) const
    {
        return GetMedianSectors(TArrayI(1, &sector), TArrayI(1, &aidx), ball);
    }
    Stat_t   GetRmsSector(Int_t sector, Int_t aidx, Bool_t ball=kFALSE) const
    {
        return GetRmsSectors(TArrayI(1, &sector), TArrayI(1, &aidx), ball);
    }
    Stat_t   GetDevSector(Int_t sector, Int_t aidx, Bool_t ball=kFALSE) const
    {
        return GetDevSectors(TArrayI(1, &sector), TArrayI(1, &aidx), ball);
    }

    Stat_t   GetMeanSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t all=kFALSE) const;
    Stat_t   GetMedianSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t all=kFALSE) const;
    Stat_t   GetRmsSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t all=kFALSE) const;
    Stat_t   GetDevSectors(const TArrayI &sector, const TArrayI &aidx, Bool_t all=kFALSE) const;

    UInt_t   GetNumPixels() const;

    TH1D    *Projection(const char *name="_proj", const Int_t nbins=50) const
    {
        return ProjectionS(TArrayI(), TArrayI(), name,nbins);
    }
    TH1D    *ProjectionS(Int_t sector, Int_t aidx, const char *name="_proj", const Int_t nbins=50) const
    {
        return ProjectionS(TArrayI(1, &sector), TArrayI(1, &aidx), name, nbins);
    }
    TH1D    *ProjectionS(const TArrayI &sector, const TArrayI &aidx, const char *name="_proj", const Int_t nbins=50) const;

    TProfile *RadialProfile(const char *name="_rad", Int_t nbins=25) const { return  RadialProfileS(TArrayI(), TArrayI(), name, nbins);}
    TProfile *RadialProfileS(Int_t sector, Int_t aidx, const char *name="_rad", const Int_t nbins=25) const
    {
        return RadialProfileS(TArrayI(1, &sector), TArrayI(1, &aidx), name, nbins);
    }
    TProfile *RadialProfileS(const TArrayI &sector, const TArrayI &aidx, const char *name="_rad", const Int_t nbins=25) const;

    TProfile *AzimuthProfile(const char *name="_az", Int_t nbins=25) const { return  AzimuthProfileA(TArrayI(), name, nbins);  }
    TProfile *AzimuthProfile(Int_t aidx, const char *name="_az", const Int_t nbins=25) const
    {
        return AzimuthProfileA(TArrayI(1, &aidx), name, nbins);
    }
    TProfile *AzimuthProfileA(const TArrayI &aidx, const char *name="_az", const Int_t nbins=25) const;
    
    void CamDraw(TCanvas &c, const Int_t x, const Int_t y, 
                 const Int_t fit, const Int_t rad=0, const Int_t azi=0,
                 TObject *notify=NULL);             
    
    const MGeomCam &GetGeomCam() const { return *fGeomCam; }

    ClassDef(MHCamera, 1) // Displays the magic camera
};

#endif

/* ------------ OK ---------------
 virtual void     Browse(TBrowser *b);
 virtual void     FillRandom(const char *fname, Int_t ntimes=5000);
 virtual void     FillRandom(TH1 *h, Int_t ntimes=5000);

 virtual Int_t    GetQuantiles(Int_t nprobSum, Double_t *q, const Double_t *probSum=0);
 virtual Axis_t   GetRandom();
 virtual void     GetStats(Stat_t *stats) const;
 virtual Stat_t   GetSumOfWeights() const;
 virtual Int_t    GetSumw2N() const {return fSumw2.fN;}
 virtual Stat_t   GetRMS(Int_t axis=1) const;

 virtual Int_t    GetNbinsX() const {return fXaxis.GetNbins();}
 virtual Int_t    GetNbinsY() const {return fYaxis.GetNbins();}
 virtual Int_t    GetNbinsZ() const {return fZaxis.GetNbins();}

 // ------------- to check -------------------

 virtual Double_t ComputeIntegral();
 virtual Int_t    DistancetoPrimitive(Int_t px, Int_t py);
 virtual void     Draw(Option_t *option="");
 virtual TH1     *DrawCopy(Option_t *option="") const;
 virtual TH1     *DrawNormalized(Option_t *option="", Double_t norm=1) const;
 virtual Int_t    BufferEmpty(Bool_t deleteBuffer=kFALSE);
 virtual void     Eval(TF1 *f1, Option_t *option="");
 virtual void     ExecuteEvent(Int_t event, Int_t px, Int_t py);
 virtual void     FillN(Int_t ntimes, const Axis_t *x, const Double_t *w, Int_t stride=1);
 virtual void     FillN(Int_t, const Axis_t *, const Axis_t *, const Double_t *, Int_t) {;}
 virtual Int_t    FindBin(Axis_t x, Axis_t y=0, Axis_t z=0);
 virtual TObject *FindObject(const char *name) const;
 virtual TObject *FindObject(const TObject *obj) const;
 virtual Int_t    Fit(const char *formula ,Option_t *option="" ,Option_t *goption="", Axis_t xmin=0, Axis_t xmax=0); // *MENU*
 virtual Int_t    Fit(TF1 *f1 ,Option_t *option="" ,Option_t *goption="", Axis_t xmin=0, Axis_t xmax=0);
 virtual void     FitPanel(); // *MENU*
 TH1             *GetAsymmetry(TH1* h2, Double_t c2=1, Double_t dc2=0);
 Int_t            GetBufferLength() const {return (Int_t)fBuffer[0];}
 Int_t            GetBufferSize  () const {return fBufferSize;}
 const   Double_t *GetBuffer() const {return fBuffer;}
 static  Int_t    GetDefaultBufferSize();
 virtual Double_t *GetIntegral() {return fIntegral;}

 TList           *GetListOfFunctions() const { return fFunctions; }

 virtual Int_t    GetNdivisions(Option_t *axis="X") const;
 virtual Color_t  GetAxisColor(Option_t *axis="X") const;
 virtual Color_t  GetLabelColor(Option_t *axis="X") const;
 virtual Style_t  GetLabelFont(Option_t *axis="X") const;
 virtual Float_t  GetLabelOffset(Option_t *axis="X") const;
 virtual Float_t  GetLabelSize(Option_t *axis="X") const;
 virtual Float_t  GetTitleOffset(Option_t *axis="X") const;
 virtual Float_t  GetTitleSize(Option_t *axis="X") const;
 virtual Float_t  GetTickLength(Option_t *axis="X") const;
 virtual Float_t  GetBarOffset() const {return Float_t(0.001*Float_t(fBarOffset));}
 virtual Float_t  GetBarWidth() const  {return Float_t(0.001*Float_t(fBarWidth));}
 virtual Int_t    GetContour(Double_t *levels=0);
 virtual Double_t GetContourLevel(Int_t level) const;
 virtual Double_t GetContourLevelPad(Int_t level) const;

 virtual void     GetCenter(Axis_t *center) const {fXaxis.GetCenter(center);}
 TDirectory      *GetDirectory() const {return fDirectory;}
 virtual Stat_t   GetEntries() const;
 virtual TF1     *GetFunction(const char *name) const;
 virtual Int_t    GetDimension() const { return fDimension; }
 virtual void     GetLowEdge(Axis_t *edge) const {fXaxis.GetLowEdge(edge);}
 virtual Double_t GetMaximum() const;
 virtual Int_t    GetMaximumBin() const;
 virtual Int_t    GetMaximumBin(Int_t &locmax, Int_t &locmay, Int_t &locmaz) const;
 virtual Double_t GetMaximumStored() const {return fMaximum;}
 virtual Double_t GetMinimum() const;
 virtual Int_t    GetMinimumBin() const;
 virtual Int_t    GetMinimumBin(Int_t &locmix, Int_t &locmiy, Int_t &locmiz) const;
 virtual Double_t GetMinimumStored() const {return fMinimum;}
 virtual Stat_t   GetMean(Int_t axis=1) const;
 virtual Double_t GetNormFactor() const {return fNormFactor;}
 virtual char    *GetObjectInfo(Int_t px, Int_t py) const;
 Option_t        *GetOption() const {return fOption.Data();}

 TVirtualHistPainter *GetPainter();

 TAxis   *GetXaxis() const;
 TAxis   *GetYaxis() const;
 TAxis   *GetZaxis() const;
 virtual Stat_t   Integral(Option_t *option="") const;
 virtual Stat_t   Integral(Int_t binx1, Int_t binx2, Option_t *option="") const;
 virtual Stat_t   Integral(Int_t, Int_t, Int_t, Int_t, Option_t * ="") const {return 0;}
 virtual Stat_t   Integral(Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Option_t * ="" ) const {return 0;}
 virtual Double_t KolmogorovTest(TH1 *h2, Option_t *option="") const;
 virtual void     LabelsDeflate(Option_t *axis="X");
 virtual void     LabelsInflate(Option_t *axis="X");
 virtual void     LabelsOption(Option_t *option="h", Option_t *axis="X");
 virtual Int_t    Merge(TCollection *list);
 virtual void     Multiply(TF1 *h1, Double_t c1=1);
 virtual void     Multiply(const TH1 *h1);
 virtual void     Multiply(const TH1 *h1, const TH1 *h2, Double_t c1=1, Double_t c2=1, Option_t *option=""); // *MENU*
 virtual void     Paint(Option_t *option="");
 virtual void     Print(Option_t *option="") const;
 virtual void     PutStats(Stat_t *stats);
 virtual TH1     *Rebin(Int_t ngroup=2, const char*newname="");  // *MENU*
 virtual void     RebinAxis(Axis_t x, Option_t *axis="X");
 virtual void     Rebuild(Option_t *option="");
 virtual void     RecursiveRemove(TObject *obj);
 virtual void     Reset(Option_t *option="");
 virtual void     SavePrimitive(ofstream &out, Option_t *option);
 virtual void     Scale(Double_t c1=1);
 virtual void     SetAxisColor(Color_t color=1, Option_t *axis="X");
 virtual void     SetAxisRange(Axis_t xmin, Axis_t xmax, Option_t *axis="X");
 virtual void     SetBarOffset(Float_t offset=0.25) {fBarOffset = Short_t(1000*offset);}
 virtual void     SetBarWidth(Float_t width=0.5) {fBarWidth = Short_t(1000*width);}
 virtual void     SetBinContent(Int_t bin, Stat_t content);
 virtual void     SetBinContent(Int_t binx, Int_t biny, Stat_t content);
 virtual void     SetBinContent(Int_t binx, Int_t biny, Int_t binz, Stat_t content);
 virtual void     SetBinError(Int_t bin, Stat_t error);
 virtual void     SetBinError(Int_t binx, Int_t biny, Stat_t error);
 virtual void     SetBinError(Int_t binx, Int_t biny, Int_t binz, Stat_t error);
 virtual void     SetBins(Int_t nx, Axis_t xmin, Axis_t xmax);
 virtual void     SetBins(Int_t nx, Axis_t xmin, Axis_t xmax, Int_t ny, Axis_t ymin, Axis_t ymax);
 virtual void     SetBins(Int_t nx, Axis_t xmin, Axis_t xmax, Int_t ny, Axis_t ymin, Axis_t ymax,
 Int_t nz, Axis_t zmin, Axis_t zmax);
 virtual void     SetBinsLength(Int_t = -1) { } //refefined in derived classes
 virtual void     SetBuffer(Int_t buffersize, Option_t *option="");
 virtual void     SetCellContent(Int_t binx, Int_t biny, Stat_t content);
 virtual void     SetCellError(Int_t binx, Int_t biny, Stat_t content);
 virtual void     SetContent(const Stat_t *content);
 virtual void     SetContour(Int_t nlevels, const Double_t *levels=0);
 virtual void     SetContourLevel(Int_t level, Double_t value);
 static  void     SetDefaultBufferSize(Int_t buffersize=1000);
 virtual void     SetDirectory(TDirectory *dir);
 virtual void     SetEntries(Stat_t n) {fEntries = n;};
 virtual void     SetError(const Stat_t *error);
 virtual void     SetLabelColor(Color_t color=1, Option_t *axis="X");
 virtual void     SetLabelFont(Style_t font=62, Option_t *axis="X");
 virtual void     SetLabelOffset(Float_t offset=0.005, Option_t *axis="X");
 virtual void     SetLabelSize(Float_t size=0.02, Option_t *axis="X");

 virtual void     SetMaximum(Double_t maximum=-1111); // *MENU*
 virtual void     SetMinimum(Double_t minimum=-1111); // *MENU*
 virtual void     SetName(const char *name); // *MENU*
 virtual void     SetNameTitle(const char *name, const char *title);
 virtual void     SetNdivisions(Int_t n=510, Option_t *axis="X");
 virtual void     SetNormFactor(Double_t factor=1) {fNormFactor = factor;}
 virtual void     SetStats(Bool_t stats=kTRUE);
 virtual void     SetOption(Option_t *option=" ") {fOption = option;}
 virtual void     SetTickLength(Float_t length=0.02, Option_t *axis="X");
 virtual void     SetTitleOffset(Float_t offset=1, Option_t *axis="X");
 virtual void     SetTitleSize(Float_t size=0.02, Option_t *axis="X");
 virtual void     SetTitle(const char *title);
 virtual void     SetXTitle(const char *title) {fXaxis.SetTitle(title);}
 virtual void     SetYTitle(const char *title) {fYaxis.SetTitle(title);}
 virtual void     SetZTitle(const char *title) {fZaxis.SetTitle(title);}
 virtual void     Smooth(Int_t ntimes=1); // *MENU*
 static  void     SmoothArray(Int_t NN, Double_t *XX, Int_t ntimes=1);
 static Double_t  SmoothMedian(Int_t n, Double_t *a);
 virtual void     Sumw2();
 void             UseCurrentStyle();

 ClassDef(TH1,4)  //1-Dim histogram base class
};

*/
