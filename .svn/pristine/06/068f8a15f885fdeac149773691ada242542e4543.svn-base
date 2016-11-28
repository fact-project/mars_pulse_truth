#ifndef MARS_MFEventSelector2
#define MARS_MFEventSelector2

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MFEventSelector2                                                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MFilter
#include "MFilter.h"
#endif
#ifndef ROOT_TArrayL
#include <TArrayL.h>
#endif
#ifndef MARS_MDataChain
#include "MDataChain.h"
#endif

class TH1;
class TCanvas;

class MH3;
class MRead;

class MFEventSelector2 : public MFilter
{
private:
    static const TString gsDefName;
    static const TString gsDefTitle;

    MH3       *fHistOrig; // original distribution of the events
    MH3       *fHistNom;  // nominal distribution
    MH3       *fHistRes;  // resulting distribution
    MDataChain fDataX;    // data rule for the x-axis
    MDataChain fDataY;    // data rule for the y-axis
    MDataChain fDataZ;    // data rule for the z-axis
    Long_t     fNumMax;   // Maximum number of selected events
    TArrayL    fIs;       //! array storing the original distribution
    TArrayL    fNom;      //! array storing the nominal distribution
    TCanvas   *fCanvas;   //! canvas for online display

    TString    fFilterName; // name of the MFilter object to be used when determining the original distribution

    Bool_t fResult;
    Bool_t fHistIsProbability;
    Bool_t fUseOrigDist;    // flag indicating that in the selection the original distribution is not changed

    Int_t  fCounter[3];

    TH1   &InitHistogram(MH3* &hist);
    Bool_t ReadDistribution(MRead &read, MFilter *filter);
    void   PrepareHistograms();
    Bool_t PreProcessData(MParList *parlist);
    Bool_t Select(Int_t bin);
    Bool_t SelectProb(Int_t bin) const;

    Int_t PreProcess(MParList *parlist);
    Int_t Process();
    Int_t PostProcess();

public:
    MFEventSelector2(MH3 &hist, const char *name=NULL, const char *title=NULL);
    ~MFEventSelector2();

    MH3 *GetHistOrig() { return fHistOrig; }

    void SetNumMax(Long_t max=-1) { fNumMax = max; }
    void SetHistIsProbability(Bool_t b=kTRUE) { fHistIsProbability=b; }
    Bool_t IsExpressionTrue() const { return fResult; }

    ClassDef(MFEventSelector2, 0) // FIXME!
};

#endif
