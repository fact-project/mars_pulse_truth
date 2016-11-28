#ifndef MARS_MHFalseSource
#define MARS_MHFalseSource

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TH3
#include <TH3.h>
#endif

class TH2D;

class MParList;
class MTime;
class MSrcPosCam;
class MPointingPos;
class MObservatory;

class MHFalseSource : public MH
{
protected:
    MTime         *fTime;        //! container to take the event time from
    MPointingPos  *fPointPos;    //! container to take pointing position from
    MSrcPosCam    *fSrcPos;      //! container for sopurce position in camera
    MObservatory  *fObservatory; //! conteiner to take observatory location from

    Float_t fMm2Deg;             // conversion factor for display in degrees

private:
    Float_t fAlphaCut;           // Alpha cut
    Float_t fBgMean;             // Background mean

    Float_t fMinDist;            // Min dist
    Float_t fMaxDist;            // Max dist

    Float_t fMinDW;              // Minimum distance in percent of dist
    Float_t fMaxDW;              // Maximum distance in percent of dist

protected:
    TH3D    fHist;               // Alpha vs. x and y

    const TH3D *fHistOff;

    Double_t fRa;
    Double_t fDec;

    TObject *GetCatalog() const;
    void MakeSymmetric(TH1 *h);

private:
    Int_t DistancetoPrimitive(Int_t px, Int_t py);
    void Modified();

    void ProjectAll(TH2D *h);
    void ProjectOff(const TH3D &src, TH2D *h, TH2D *all);
    void ProjectOn(const TH3D &src, TH2D *h, TH2D *all);
    void ProjectOnOff(TH2D *h, TH2D *all);

public:
    MHFalseSource(const char *name=NULL, const char *title=NULL);

    void FitSignificance(Float_t sigint=10, Float_t sigmax=75, Float_t bgmin=45, Float_t bgmax=85, Byte_t polynom=2); //*MENU*
    void FitSignificanceStd() { FitSignificance(); } //*MENU*

    void SetMinDist(Float_t dist) { fMinDist = dist; } // Absolute minimum distance
    void SetMaxDist(Float_t dist) { fMaxDist = dist; } // Absolute maximum distance
    void SetMinDW(Float_t ratio)  { fMinDW = ratio; }  // Minimum ratio between length/dist
    void SetMaxDW(Float_t ratio)  { fMaxDW = ratio; }  // Maximum ratio between length/dist

    void SetAlphaCut(Float_t alpha); //*MENU*
    void SetAlphaPlus5()  { SetAlphaCut(fAlphaCut+5); } //*MENU*
    void SetAlphaMinus5() { SetAlphaCut(fAlphaCut-5); } //*MENU*

    void SetBgMean(Float_t alpha); //*MENU*
    void SetBgMeanPlus5()  { SetBgMean(fBgMean+5); } //*MENU*
    void SetBgMeanMinus5() { SetBgMean(fBgMean-5); } //*MENU*

    virtual void SetOffData(const MHFalseSource &fs) {
        fHistOff  = &fs.fHist;
        fMinDist  =  fs.fMinDist;
        fMaxDist  =  fs.fMaxDist;
        fMaxDW    =  fs.fMaxDW;
        fMinDW    =  fs.fMinDW;
        fAlphaCut =  fs.fAlphaCut;
        fBgMean   =  fs.fBgMean;
    }

    void DrawNicePlot() const; //*MENU*

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MH
    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    // TObject
    void Paint(Option_t *opt="");
    void Draw(Option_t *option="");

    ClassDef(MHFalseSource, 1) //3D-histogram in alpha, x and y
};

#endif
