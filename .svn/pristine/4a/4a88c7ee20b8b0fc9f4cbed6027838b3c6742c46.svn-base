#ifndef MARS_MHImagePar
#define MARS_MHImagePar

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif

class MHillas;

class MHImagePar : public MH
{
private:
    TH1F fHistSatHi;          // Number of pixels with sat hi-gain
    TH1F fHistSatLo;          // Number of pixels with sat lo-gain

    TH1F fHistIslands;        // Number of islands per event
    TH1F fHistSizeSubIslands; // Size of sub islands
    TH1F fHistSizeMainIsland; // Size of main island

    TH1F fHistNumSP;          // Number of single core pixels
    TH1F fHistSizeSP;         // size of single core pixels

    Short_t fNumSatPixelsHG; // number of pixels with saturating hi-gains
    Short_t fNumSatPixelsLG; // number of pixels with saturating lo-gains

public:
    MHImagePar(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    TH1 *GetHistByName(const TString name) const;
    TObject *FindObject(const TObject *obj) const { return 0; }
    TObject *FindObject(const char *name) const
    {
        return (TObject*)GetHistByName(name);
    }

    TH1F &GetHistSatHi()   { return fHistSatHi; }
    TH1F &GetHistSatLo()   { return fHistSatLo; }

    TH1F &GetHistIslands() { return fHistIslands; }

    void Paint(Option_t *opt="");
    void Draw(Option_t *opt="");

    ClassDef(MHImagePar, 1) // Histograms of image parameters
};

#endif
