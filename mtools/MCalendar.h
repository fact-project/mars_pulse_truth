#ifndef MARS_MCalendar
#define MARS_MCalendar

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#ifndef ROOT_TString
#include <TString.h>
#endif
#ifndef ROOT_TImage
#include <TImage.h>
#endif
#ifndef ROOT_Gtypes
#include <Gtypes.h>
#endif

/*
#include <TAttLine.h>
#include <TAttFill.h>
#include <TAttText.h>
*/

class TEnv;
class TPave;
class TLatex;
class TAttText;
class TAttFill;
class TArrayD;
class TASImage;

class MEnv;
class MTime;

class MCalendar : public TObject//, public TAttLine, public TAttText, public TAttFill
{
private:
    /*
    enum {
        kMoonPhase,
        kMoonPeriod,
        kMagicPeriod
    };*/

    /*
    Int_t  fAlign;
    Bool_t fActive;
    Bool_t fUpdate;
    Int_t  fDay;
    */
    MEnv    *fEnv;
    MEnv    *fEnvHolidays;

    Bool_t   fOrientation;
    Bool_t   fRotation;

    Int_t    fYear;
    Int_t    fMonth;
    Int_t    fFirstDay;
    Int_t    fNumBlocks;

    TString  fLanguage;

    Double_t fMarginX;
    Double_t fMarginY;
    Double_t fCellSpace;
    Double_t fBlockSpace;

    Int_t    fMoonAlignGraf;
    Int_t    fMoonAlignText;
    Int_t    fMoonTextType;
    Int_t    fMoonTextCont;
    Float_t  fMoonRadius;

    // Resource file evaluation
    void    GetPave(TPave &pave, Bool_t active, Int_t n);
    void    GetMoon(TAttText &text, TAttFill &fill, Bool_t active, Int_t n);
    void    GetLatex(TLatex &latex, Bool_t active, Int_t n);
    TString GetHoliday(const TObject &o, const MTime &tm);

    // Helper function for Paint
    TArrayD ConvertAlign(Int_t align, Double_t x[2], Double_t y[2], Double_t m[2]) const;
    Bool_t  GetBox(Int_t n, Double_t x[2], Double_t y[2], Double_t *ratio=0);

    // Helper function for GetImage
    TASImage  *ReturnFile(const char *file) const;
    const char *GetTTFontPath(const char *file) const;

    // Orientation
    Int_t Rotate(Int_t align, Bool_t fwd=kTRUE) const;
    Int_t Derotate(Int_t align) const { return Rotate(align, kFALSE); }

    // Menu interaction
    Int_t   GetN(Double_t x[2], Double_t y[2]);
    Int_t   GetN() { Double_t x[2], y[2]; return GetN(x,y); }
    MTime   GetDate();
    Int_t   GetAlign();

    // Paint text
    void PaintLatex(TAttText &att, Int_t align, Double_t x[2], Double_t y[2], Double_t ratio[2], /*Double_t height,*/ TString str);
    void PaintClock(Double_t x[2], Double_t y[2], Double_t r[2], Int_t fill, char dir);

    // GUI interactions
    Int_t DistancetoPrimitive(Int_t px, Int_t py);

public:
    MCalendar(UShort_t y=0, Byte_t m=0, const char *fname=0);
    ~MCalendar();

    TObject    *Clone(const char *newname="") const;

    // Getter
    const char *GetName() const { return Form("%04d-%02d", fYear, fMonth); }
    const char *GetLanguage() const { return fLanguage; }
    Int_t       GetYear() const { return fYear; }
    Int_t       GetMonth() const { return fMonth; }
    Int_t       GetFirstDay() const { return fFirstDay; }
    Int_t       GetNumBlocks() const { return fNumBlocks; }
    Double_t    GetMarginX() const { return fMarginX; }
    Double_t    GetMarginY() const { return fMarginY; }
    Double_t    GetCellSpace() const { return fCellSpace; }
    Double_t    GetBlockSpace() const { return fBlockSpace; }
    Int_t       GetMoonAlignGraf() const { return fMoonAlignGraf; }
    Int_t       GetMoonAlignText() const { return fMoonAlignText; }
    Int_t       GetMoonTextType() const { return fMoonTextType; }
    Int_t       GetMoonTextCont() const { return fMoonTextCont; }
    Float_t     GetMoonRadius() const { return fMoonRadius; }
    TString     GetStringFmt(const char *fmt="%B", Int_t day=1, Int_t h=12, Int_t m=0, Int_t s=0) const;
    Bool_t      IsRotated() const { return fRotation; }
    Bool_t      IsVertical() const { return fOrientation; }


    // Setter - Menu
    void SetDate(UInt_t Month, Int_t Year=-1) { fMonth=Month; if (Year>0) fYear=Year; } // *MENU* *ARGS={Month=>fMonth,Year=>fYear}
    void SetLocal(const char *Locale="") { fLanguage=Locale; } // *MENU* *ARGS={Locale=>fLanguage}
    void SetFirstDay(Int_t FirstDay) { fFirstDay=FirstDay%7; } // *MENU* *ARGS={FirstDay=>fFirstDay}
    void SetLayout(Byte_t NumberOfBlocks, Double_t MarginX, Double_t MarginY, Double_t CellSpace, Double_t BlockSpace) { fNumBlocks=NumberOfBlocks, fMarginX=MarginX; fMarginY=MarginY; fCellSpace=CellSpace; fBlockSpace=BlockSpace; } //*MENU* *ARGS={NumberOfBlocks=>fNumBlocks,MarginX=>fMarginX,MarginY=>fMarginY,CellSpace=>fCellSpace,BlockSpace=>fBlockSpace}

    void ResetLayout(Int_t num);
    void ResetLayout() { ResetLayout(-1); } //*MENU
    void ResetHolidays(); //*MENU

    void SetDefaultLayout(Int_t num);
    void SetDefaultLayout() { SetDefaultLayout(-1); } //*MENU
    void SetDefaultHolidays(); //*MENU

    void ReadResourceFile(const char *FileName=0); //*MENU
    void ReadHolidayFile(const char *FileName=0);  //*MENU
    void SaveResourceFile(const char *FileName=0); //*MENU
    void SaveHolidayFile(const char *FileName=0);  //*MENU

    void Add(const char *Name, Int_t Align, const char *Format, Float_t FontSize=0.3, Int_t FontColor=1); //*MENU
    void Add(const char *name, const char *fmt, const TAttText &att);

    void SetDay(const char *Format="%a", Float_t FontSize=0.3, Int_t Color=kBlack)  { Remove(GetAlign()); Add("Day", GetAlign(), Format, FontSize, Color); } //*MENU*
    void SetDate(const char *Format="%e", Float_t FontSize=0.3, Int_t Color=kBlack)  { Remove(GetAlign()); Add("Date", GetAlign(), Format, FontSize, Color); } //*MENU*
    void SetHoliday(Float_t FontSize=0.13, Int_t Color=kBlue)   { Remove(GetAlign()); Add("Holiday", GetAlign(), "", FontSize, Color); }  //*MENU*
    void SetBirthday(Float_t FontSize=0.13, Int_t Color=kBlue)  { Remove(GetAlign()); Add("Birthday", GetAlign(), "", FontSize, Color); } //*MENU*
    void SetEaster(Float_t FontSize=0.13, Int_t Color=kBlue)    { Remove(GetAlign()); Add("Easter", GetAlign(), "", FontSize, Color); }   //*MENU*

    void AddHoliday(const char *text);    //*MENU
    void AddBirthday(const char *text);   //*MENU
    void AddEaster(const char *text);     //*MENU

    void Remove() { Remove(GetAlign()); } //*MENU*
    void Remove(Int_t align);

    //    void ToggleOrientation() { fOrientation = !fOrientation; } //*MENU
    void SetRotated(Bool_t b=kTRUE) { fRotation = b; } // *TOGGLE* *GETTER=IsRotated
    void SetVertical(Bool_t b=kTRUE) { fOrientation = b; } // *TOGGLE* *GETTER=IsVertical

    void PrintEnv() const; //*MENU
    void SetEnv(const char *VariableName, const char *Value) const;//*MENU
    void RemoveEnv(const char *VariableName) const;//*MENU

    void SetMoonGraf(Int_t Align, Float_t Radius) { fMoonAlignGraf=Align; fMoonRadius=Radius; } //*MENU* *ARGS={Align=>fMoonAlignGraf,Radius=>fMoonRadius}
    void SetMoonText(Int_t Align, Int_t Type, Int_t Continous) { fMoonAlignText=Align; fMoonTextType=Type; fMoonTextCont=Continous; } //*MENU* *ARGS={Align=>fMoonAlignText,Type=>fMoonTextType,Continous=>fMoonTextCont}

    // Calendar sheet interface
    static TASImage *DrawImage(const TASImage &img, Float_t x1, Float_t y1, Float_t x2, Float_t y2, Bool_t rot=kFALSE);
    TASImage *GetImage(TEnv *env=NULL, const char *path="");
    void      DrawLatex(Float_t x, Float_t y, const char *text, const TAttText &att);
    void      DrawDate(Float_t x, Float_t y, const char *text, const TAttText &att)
    {
        DrawLatex(x, y, GetStringFmt(text), att);
    }
    void DrawText(TASImage &img, Int_t x, Int_t y, const char *txt,
                  Int_t size=50, const char *color = 0,
                  const char *font = "fixed", TImage::EText3DType type = TImage::kPlain, const char *path=0);
    void DrawDate(TASImage &img, Int_t x, Int_t y, const char *txt,
                  Int_t size=50, const char *color = 0,
                  const char *font = "fixed", TImage::EText3DType type = TImage::kPlain, const char *path=0)
    {
        DrawText(img, x, y, GetStringFmt(txt), size, color, font, type, path);
    }
    TASImage *DrawTTF(Float_t x1, Float_t x2, Float_t y1, Float_t y2,
                      const char *text, Float_t size, const char *font);
    TASImage *DrawTTFDate(Float_t x1, Float_t x2, Float_t y1, Float_t y2,
                          const char *text, Float_t size, const char *font)
    {
        return DrawTTF(x1, y1, x2, y2, GetStringFmt(text), size, font);
    }
    //Bool_t DrawImage(TEnv &env, Float_t x1, Float_t y1, Float_t x2, Float_t y2) const;

    // Convertions
    static void Convert2Latex(TString &str);
    static void ConvertUTF8(TString &str, Bool_t fwd=kTRUE);

    // TObject
    void Paint(Option_t *o="");

    /*
     void Update();
     void SetLineAttributes() {}
     void SetFillAttributes() {}
     void SetTextAttributes() {}
     void SetTextAngle(Float_t) {}
     void Selected(TVirtualPad*,TObject*,Int_t);
     */
     //void SetAttributes(); //*MENU

    ClassDef(MCalendar, 1)
};

#endif
