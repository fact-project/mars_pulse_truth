#ifndef MARS_MHFadcCam
#define MARS_MHFadcCam

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef MARS_MHFadcPix
#include "MHFadcPix.h"
#endif

#ifndef ROOT_TH1
#include <TH1.h>
#endif

class MRawEvtData;

class MHFadcCam : public MH
{
private:
    TObjArray *fArray;	//-> List of Lo/Hi gain Histograms

    //    void FillHi(UInt_t ipix, Byte_t data) { (*this)[ipix]->FillHi(data); }
    //    void FillLo(UInt_t ipix, Byte_t data) { (*this)[ipix]->FillLo(data); }

    Int_t fNumHiGains; //!
    Int_t fNumLoGains; //!

    MHFadcPix::Type_t fType; //!

public:
    MHFadcCam(/*const Int_t n=577,*/ MHFadcPix::Type_t t=MHFadcPix::kValue, const char *name=NULL, const char *title=NULL);
    ~MHFadcCam();

    MHFadcPix &operator[](UInt_t i) { return *(MHFadcPix*)(fArray->At(i)); }
    const MHFadcPix &operator[](UInt_t i) const { return *(MHFadcPix*)(fArray->At(i)); }

    TH1F *GetHistHi(UInt_t i)  { return (*this)[i].GetHistHi(); }
    TH1F *GetHistLo(UInt_t i)  { return (*this)[i].GetHistLo(); }

    const TH1F *GetHistHi(UInt_t i) const { return (*this)[i].GetHistHi(); }
    const TH1F *GetHistLo(UInt_t i) const { return (*this)[i].GetHistLo(); }

    Bool_t SetupFill(const MParList *pList);
    Bool_t Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Fill(const MRawEvtData *par);

    void ResetHistograms();
    void ResetEntry(Int_t i);

    //
    // FIXME! This should be replaced by a Draw(Option_t)-function
    //
    void DrawHi(UInt_t i)      { GetHistHi(i)->Draw(); }
    void DrawLo(UInt_t i)      { GetHistLo(i)->Draw(); }

    Bool_t HasLo(UInt_t i) const { return GetHistLo(i)->GetEntries()>0; }
    Bool_t HasHi(UInt_t i) const { return GetHistHi(i)->GetEntries()>0; }

    Bool_t Exists(UInt_t i) const { return HasLo(i) && HasHi(i); }

    void DrawPix(UInt_t i)     { (*this)[i].Draw(); }

    Int_t GetEntries() const   { return fArray->GetEntries(); }

    TObject *Clone(const char *opt="") const;

    ClassDef(MHFadcCam, 1) // A list of histograms storing the Fadc spektrum of one pixel
};

#endif

