#ifndef MARS_MHBadPixels
#define MARS_MHBadPixels

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TH2
#include <TH2.h>
#endif

class MPedPhotCam;
class MPointingPos;
class MParList;
class MGeomCam;

class MHBadPixels : public MH
{
private:
    MGeomCam      *fCam;      //!
    MPedPhotCam   *fPedPhot;  //!
    MPointingPos  *fPointPos; //!

    TString  fNamePedPhotCam; //! name of the 'MPedPhotCam' container
    MBinning *fBinsTheta;     //!
    MBinning *fBinsPix;       //!

    TH2D          *fBadId; // 2D-histogram : pixel Id vs. Theta
    TH2D          *fBadN;  // 2D-histogram : no.of bad pixels vs. Theta

public:
    MHBadPixels(const char *name=NULL, const char *title=NULL);
    ~MHBadPixels();

    void SetNamePedPhotCam(const char *name)  { fNamePedPhotCam = name; }

    const TH2D *GetBadId()       { return fBadId; }
    const TH2D *GetBadId() const { return fBadId; }

    const TH2D *GetBadN()       { return fBadN; }
    const TH2D *GetBadN() const { return fBadN; }

    TH2 *GetBadIdByName(const TString name) { return fBadId; }
    TH2 *GetBadNByName(const TString name)  { return fBadN; }

    void Draw(Option_t* option = "");

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);

    ClassDef(MHBadPixels, 1)  // Histogram of bad pixel Id vs. Theta
};

#endif


