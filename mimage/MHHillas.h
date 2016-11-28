#ifndef MARS_MHHillas
#define MARS_MHHillas

#ifndef MARS_MH
#include "MH.h"
#endif

class TH1F;
class TH2F;
class MHillas;
class MGeomCam;

class MHHillas : public MH
{
private:
    MGeomCam *fGeomCam; //! Camera geometry for plots (for the moment this is a feature for a loop only!)

    TH1F *fLength;  //-> Length
    TH1F *fWidth;   //-> Width

    TH1F *fDistC;   //-> Distance to Camera Center
    TH1F *fDelta;   //-> Angle between Length axis and x-axis

    TH1F *fSize;    //-> Sum of used pixels
    TH2F *fCenter;  //-> Center

    void Paint(Option_t *opt="");

public:
    MHHillas(const char *name=NULL, const char *title=NULL);
    ~MHHillas();

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    TH1 *GetHistByName(const TString name) const;
    TObject *FindObject(const TObject *obj) const { return 0; }
    TObject *FindObject(const char *name) const
    {
        return (TObject*)GetHistByName(name);
    }

    TH1F *GetHistLength() { return fLength; }
    TH1F *GetHistWidth()  { return fWidth; }

    TH1F *GetHistDistC()  { return fDistC; }
    TH1F *GetHistDelta()  { return fDelta; }

    TH1F *GetHistSize()   { return fSize; }
    TH2F *GetHistCenter() { return fCenter; }

    void Draw(Option_t *opt=NULL);

    //Int_t DistancetoPrimitive(Int_t px, Int_t py) { return 0; }

    ClassDef(MHHillas, 2) // Container which holds histograms for the source independent image parameters
};

#endif
