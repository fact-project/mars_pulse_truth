#ifndef MARS_MagicJam
#define MARS_Magicjam

#ifndef MARS_MHCamera
#include "MHCamera.h"
#endif

// --------------------------------------------------------------------

class MagicJamAbc : public MHCamera
{
public:
    // Remove this items from the context menu
    void  SetLineAttributes() { }
    void  SetFillAttributes() { }
    void  SetMarkerAttributes() { }
    void  SetName(const char *name) { MHCamera::SetName(name); }
    void  SetTitle(const char *name) { MHCamera::SetTitle(name); }
    TH1  *DrawCopy(const Option_t *o="") const { return MHCamera::DrawCopy(o); }
    void  PrintInfo() const {  }
    void  SetDrawOption(Option_t *option="") { MHCamera::SetDrawOption(option); }
    Int_t Fit(const char *formula ,Option_t *option="" ,Option_t *goption="", Axis_t xmin=0, Axis_t xmax=0) { return 0; }
    Int_t Fit(TF1 *f1 ,Option_t *option="" ,Option_t *goption="", Axis_t xmin=0, Axis_t xmax=0) { return 0; }
    void  SetMaximum(Double_t maximum=-1111) { MHCamera::SetMaximum(maximum); }
    void  SetMinimum(Double_t minimum=-1111) { MHCamera::SetMinimum(minimum); }
    void  Add(const TH1 *h1, const TH1 *h2, Double_t c1=1, Double_t c2=1) { }
    void  Add(TF1 *h1, Double_t c1, Option_t *) { }
    void  Add(const TH1 *h1, Double_t c1=1) { }
    void  Divide(TF1 *f1, Double_t c1=1) { }
    void  Divide(const TH1 *h1) { }
    void  Divide(const TH1 *h1, const TH1 *h2, Double_t c1=1, Double_t c2=1, Option_t *option="") { }
    void  Multiply(TF1 *h1, Double_t c1=1) { }
    void  Multiply(const TH1 *h1) { }
    void  Multiply(const TH1 *h1, const TH1 *h2, Double_t c1=1, Double_t c2=1, Option_t *option="") { }
    void  Smooth(int, int, int) { }
    void  Smooth(int, const Option_t *) { }
    void  SetPrettyPalette() {}
    void  SetDeepBlueSeaPalette() {}
    void  SetInvDeepBlueSeaPalette() {}
    void  SetFreezed(Bool_t f=kTRUE) { }
    void  SetAbberation(Float_t f=0.0713) { }
    void  SetAutoScale() { }
    void  SetMinMax(Double_t min=-1111, Double_t max=-1111) { SetMinimum(min); SetMaximum(max); }
    TH1  *ShowBackground(Int_t niter=2, Option_t *option="same") { return 0; }
    Int_t ShowPeaks(Double_t sigma=2, Option_t *option="", Double_t threshold=0.05) { return 0; }
    void  SetErrorSpread(Bool_t f=kTRUE) { }

    // Make the full pad sensible for the context menu
    Int_t DistancetoPrimitive(Int_t px, Int_t py) { return 0; }

    ClassDef(MagicJamAbc, 0) // Helper class for MagicJam
};

// --------------------------------------------------------------------
class TASImage;

class MagicJamEditor : public MagicJamAbc
{
    Int_t fWidth;

    TASImage *fImage;

    void Move(int dx, int dy);
    void InitGeom(int wx, int hy);
    Int_t GetPixelIndexFlt(Float_t px, Float_t py) const;

public:
    MagicJamEditor(const char *name=0);
    ~MagicJamEditor();

    // MagicJamEditor
    void   SetAllUsed() { MHCamera::SetAllUsed(); } //*MENU*
    void   Reset(Option_t *o="") { MHCamera::Reset(o); } //*MENU*
    void   Invert() { for (UInt_t i=0; i<GetNumPixels(); i++) if (IsUsed(i)) ResetUsed(i); else SetUsed(i); } //*MENU*
    void   SetWidthHeight(Int_t dx, Int_t dy); //*MENU*
    Bool_t ShowUnused() const { return TestBit(kNoUnused); }
    void   DoNotDisplayUnused(Bool_t b) { b ? SetBit(kNoUnused) : ResetBit(kNoUnused); } //*TOGGLE* *GETTER=ShowUnused

    void   LoadGame(const char *name="default.jam");//*MENU* *ARGS={name=>fName}
    void   SaveGame(const char *name="default.jam"); //*MENU* *ARGS={name=>fName}

    void   ShowImageAsBackground(const char *name); //*MENU*
    void   RemoveBackgroundImage(); //*MENU*

    // TObject
    void Paint(Option_t *o);
    void Draw(Option_t *o="");
    void EventInfo(Int_t event, Int_t px, Int_t py, TObject *);
    void ExecuteEvent(Int_t event, Int_t px, Int_t py);

    ClassDef(MagicJamEditor, 0) // Editor for the MAGIC Jam games
};

// --------------------------------------------------------------------

class MagicJam : public MagicJamAbc
{
private:
    Int_t fNumPlayers;
    Int_t fNumPlayer;

    MagicJam *fGame; //!

    void FillPix(Int_t idx);
    Int_t CountPlayer(Int_t player) const;
    Int_t CountPlayers() const;

public:
    MagicJam(const char *jam="mtools/jam/default.jam", MagicJam *game=NULL);

    // MagicJam
    void  SetNumPlayers(Byte_t n=2); //*MENU*
    void  LoadGame(const char *name="default.jam"); //*MENU* *ARGS={name=>fName}
    void  Reset(Option_t *o=""); //*MENU*
    void  StartEditor() const; //*MENU*
    void  OpenInEditor() const; //*MENU*
    void  ShowDirectory(const char *dir="mtools/jam"); //*MENU*

    // TObject
    void  ExecuteEvent(Int_t event, Int_t px, Int_t py);
    void  Paint(Option_t *o);
    void  Draw(Option_t *o=0);
    void  RecursiveRemove(TObject *obj);

    ClassDef(MagicJam, 0) // MAGIC-Jam (game)
};

#endif
