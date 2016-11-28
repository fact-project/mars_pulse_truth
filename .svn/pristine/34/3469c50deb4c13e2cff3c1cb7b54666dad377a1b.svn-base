#ifndef MARS_MHMcCollectionArea
#define MARS_MHMcCollectionArea

#ifndef MARS_MH
#include "MH.h"
#endif

#include "MBinning.h"
#include "TF1.h"

class TH1D;
class TH2D;
class TH3D;

//
// Version 2:
//
// Now all histograms are saved with the object. Added members
// fImpactMax, fImpactBins, fMinEvents
//

class MHMcCollectionArea : public MH
{
private:
  TH3D *fHistAll;        // all simulated showers
  TH3D *fHistSel;        // the selected showers
  TH2D *fHistCol;        // the collection area in fine bins
  TH2D *fHistColCoarse;  // the coll. area in coarse bins 

  Int_t   fImpactBins;   // number of bins in i.p. for histograms
  Float_t fImpactMax;    // [m] Maximum impact parameter for histograms

  Int_t   fMinEvents;    
  // minimum number of events in each of the "fine bins" of fHistAll 
  // so that the bin is used in the averaging for the coarse bins. 

  void Calc(TH3D &hsel, TH3D &hall);

public:
  MHMcCollectionArea(const char *name=NULL, const char *title=NULL);
  ~MHMcCollectionArea();

  void FillAll(Double_t energy, Double_t radius, Double_t theta);
  void FillSel(Double_t energy, Double_t radius, Double_t theta);

  const TH2D *GetHist()       const { return fHistCol; }
  const TH2D *GetHistCoarse() const { return fHistColCoarse; }
  const TH3D *GetHistAll()    const { return fHistAll; }
  const TH3D *GetHistSel()    const { return fHistSel; }

  void SetBinnings(const MBinning &binsEnergy, const MBinning &binsTheta);
  void SetCoarseBinnings(const MBinning &binsEnergy, const MBinning &binsTheta);

  void Draw(Option_t *option="");

  void Calc(TF1 *spectrum);

  ClassDef(MHMcCollectionArea, 2)  // Data Container to store Collection Area
    };

#endif
