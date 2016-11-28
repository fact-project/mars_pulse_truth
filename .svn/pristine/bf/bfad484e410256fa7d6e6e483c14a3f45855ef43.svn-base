#ifndef MARS_MAstroCatalog
#define MARS_MAstroCatalog

#ifndef MARS_MVector3
#include "MVector3.h"
#endif
#ifndef ROOT_TList
#include <TList.h>
#endif
#ifndef ROOT_TAttLine
#include <TAttLine.h>
#endif
#ifndef ROOT_TAttMarker
#include <TAttMarker.h>
#endif
#ifndef MARS_MGMap
#include <MGMap.h>
#endif

class MTime;
class MObservatory;
class TArrayI;
class TGToolTip;
class TGraph;

class MAttLine : public TObject, public TAttLine
{
public:
    MAttLine() { }
    ClassDef(MAttLine, 1) // Helper class to have a TAttLine derving from TObject (for standalone GED editor)
};

class MAstroCatalog : public TObject, public TAttMarker
{
private:
    static const TString kDefaultPath; //! Datacenter default path for catalogs

    Double_t   fLimMag;     // [1]   Limiting Magnitude
    Double_t   fRadiusFOV;  // [deg] Radius of Field of View

    TGToolTip *fToolTip;    //! The tooltip currently displayed
    Double_t   fAngle;      //! Rotation angle of map

    MAttLine fAttLineSky;   // Line Style and color for sky coordinates
    MAttLine fAttLineLocal; // Line Style and color for local coordinates

    void ShowToolTip(Int_t px, Int_t py, const char *txt);
    void SetLineAttributes(MAttLine &att);

    TString FindToken(TString &line, Char_t tok=',');

    Int_t   atoi(const TString &s);
    Float_t atof(const TString &s);

//#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,03)
//    Bool_t fPlainScreen;  //! Just a dummy!!!! ([Set,Is]Freezed)
//    Bool_t fNoToolTips;   //! Just a dummy!!!! ([Set,Is]Freezed)
//#endif

    virtual Int_t ConvertToPad(const TVector3 &w, TVector2 &v) const;
    virtual void  AddPrimitives(TString o);
    virtual void  SetRangePad(Option_t *o);

    Int_t  Convert(const TRotation &rot, TVector2 &v) const;
    void   Draw(const TVector2 &v0, const TRotation &rot, TArrayI &dx, TArrayI &dy, Int_t stepx, Int_t stepy, Int_t type);
    void   DrawPrimitives(Option_t *o);
    Bool_t DrawLine(const TVector2 &v0, Int_t dx, Int_t dy, const TRotation &rot, Int_t type);
    void   DrawGrid(const TVector3 &v0, const TRotation &rot, Int_t type);
    void   Paint(Option_t *o="");
    Int_t  DistancetoPrimitive(Int_t px, Int_t py);

protected:
    enum {
        kMark         = BIT(14), // A mark for the sources in the list
        kHasChanged   = BIT(15), // Display has changed
        kGuiActive    = BIT(16), // GUI is interactive
        kPlainScreen  = BIT(17), // View is a plain screen view
        kMirrorX      = BIT(18), // Mirror display in X
        kMirrorY      = BIT(19), // Mirror display in Y
        kNoToolTips   = BIT(20), // suppress tooltips
        kDrawingImage = BIT(21)  // just drawing into a bitmap
    };

    MGMap    fMapG;              //! A map with all gui primitives and tooltips

    TList    fList;              // List of stars loaded
    MVector3 fRaDec;             // pointing position

    MObservatory *fObservatory;  // Possible obervatory location
    MTime        *fTime;         // Possible observation time

    TRotation AlignCoordinates(const TVector3 &v) const;
    virtual TString GetPadTitle() const;
    TRotation GetGrid(Bool_t local);
    void      DrawStar(Double_t x, Double_t y, const TVector3 &v, Int_t col, const char *txt=0, Bool_t resize=kFALSE);
    void      Update(Bool_t upd=kFALSE);

    void      ExecuteEventKbd(Int_t keycode, Int_t keysym);
    void      ExecuteEvent(Int_t event, Int_t mp1, Int_t mp2);
    char     *GetObjectInfo(Int_t px, Int_t py) const;

public:
    MAstroCatalog();
    ~MAstroCatalog();

    void ForceUpdate() { Update(kTRUE); } // Slot for marker handling

    // Setter to control the sky position and behaviour
    void SetTime(const MTime &time);
    void SetObservatory(const MObservatory &obs);
    void SetLimMag(Double_t mag) { fLimMag=mag; Update(); } // *MENU* *ARGS={mag=>fLimMag}

    void SetRadiusFOV(Double_t pixsize, Double_t w, Double_t h);
    void SetRadiusFOV(Double_t deg)
    {
        //const Double_t max = TestBit(kPlainScreen) ? 90 : 55;
        const Double_t max = TestBit(kPlainScreen) ? 180 : 90;
        if (deg>max)
            deg=max;
        if (deg<1)
            deg=1;

        fRadiusFOV=deg;

        Update();
    } // *MENU* *ARGS={deg=>fRadiusFOV}

    void     SetRaDec(Double_t ra, Double_t dec) { fRaDec.SetRaDec(ra, dec, 1); Update(); }
    void     SetRaDec(const TVector3 &v)         { fRaDec=v; Update(); }
    void     SetGuiActive(Bool_t b=kTRUE)        { b ? SetBit(kGuiActive) : ResetBit(kGuiActive); }

    void     SetPlainScreen(Bool_t b=kTRUE)    { b ? SetBit(kPlainScreen) : ResetBit(kPlainScreen); Update(); } // *TOGGLE* *GETTER=IsPlainScreen
    Bool_t   IsPlainScreen() const             { return TestBit(kPlainScreen); }

    void     SetNoToolTips(Bool_t b=kTRUE)     { b ? SetBit(kNoToolTips) : ResetBit(kNoToolTips); } // *TOGGLE* *GETTER=HasNoToolTips
    Bool_t   HasNoToolTips() const             { return TestBit(kNoToolTips); }

    Double_t GetLimMag() const { return fLimMag; } // Get Limiting Magnitude
    Double_t GetRadiusFOV() const { return fRadiusFOV; } // Get maximum radius of Field Of View

    // Interface to set stars
    Int_t  ReadXephem(TString catalog = "/usr/X11R6/lib/xephem/catalogs/YBS.edb");
    Int_t  ReadNGC2000(TString catalog = "ngc2000.dat");
    Int_t  ReadBSC(TString catalog = "bsc5.dat");
    Int_t  ReadHeasarcPPM(TString catalog = "heasarc_ppm.tdat", TString fout="");
    Int_t  ReadCompressed(TString catalog);
    Bool_t AddObject(Float_t ra, Float_t dec, Float_t mag, TString name="");

    // Interface to get stars
    TList *GetList() { return &fList; } // Return list of stars
    UInt_t GetNumStars() const { return fList.GetEntries(); }
    TObject *FindObject(const char *name) const { return fList.FindObject(name); }
    TObject *FindObject(const TObject *obj) const { return fList.FindObject(obj); }
    void MarkObject(const char *name) const { TObject *o=FindObject(name); if (o) o->SetBit(kMark); }

    void GetVisibilityCurve(TGraph &g, const char *name=0) const;

    // TObject
    void Delete(Option_t * = "") { fList.Delete(); fMapG.Delete(); } // Delete list of stars
    void Print(Option_t * = "") const { fList.Print(); } // Print all stars
    void Draw(Option_t * = "");
    void SetDrawOption(Option_t *option="")
    {
        TObject::SetDrawOption(option);
        Update(kTRUE);
    } //*MENU*

    // Interface to Cosy
    void PaintImg(unsigned char *buf, int w=768, int h=576, Option_t *o=0);

    // Interface to line styles
    void SetLineAttributesLocal() { SetLineAttributes(fAttLineLocal); } //*MENU*
    void SetLineAttributesSky()   { SetLineAttributes(fAttLineSky);   } //*MENU*
    void SetMarkerAttributes();   //*MENU*

    TAttLine &GetAttLineSky() { return fAttLineSky; }
    TAttLine &GetAttLineLocal() { return fAttLineLocal; }

    virtual void EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected=0);

    ClassDef(MAstroCatalog, 2) // Display class for star catalogs
};

#endif
