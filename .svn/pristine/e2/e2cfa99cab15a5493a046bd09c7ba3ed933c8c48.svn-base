#ifndef MARS_MSkyPlot
#define MARS_MSkyPlot

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TH2
#include <TH2.h>
#endif

#ifndef ROOT_TH1
#include <TH1.h>
#endif

#ifndef ROOT_TOrdCollection
#include <TOrdCollection.h>
#endif

class TH2D;
class TH1D;

class MParList;
class MTime;
class MPointingPos;
class MSrcPosCam;
class MReportDrive;
class MObservatory;
class MHillas;
class MHillasExt;
class MHillasSrc;
class MNewImagePar;
class MHadronness;
class MGeomCam;
class TOrdCollection;
class MSkyPlot : public MTask
{
private:

    MGeomCam      *fGeomCam;     //! container to take the event time from
    MTime         *fTime;        //! container to take the event time from
    MPointingPos  *fPointPos;    //! container to take pointing position from
    MReportDrive  *fRepDrive;    //!      
    MSrcPosCam    *fSrcPosCam;   //! container with x and y of the source
    MSrcPosCam    *fPntPosCam;   //! container with x and y of the position MReportDrive.GetRa, MReportDrive.GetDec
    MObservatory  *fObservatory; //! container to take observatory location from
    MHillas       *fHillas;      //!
    MHillasExt    *fHillasExt;   //!
    MHillasSrc    *fHillasSrc;   //!
    MNewImagePar  *fNewImagePar; //!
    MHadronness   *fHadron;      //!

    TOrdCollection *fHistAlpha;  // vector of histograms for alpha

    Float_t        fMm2Deg;      // conversion factor for display in degrees
    Double_t       fGridBinning; // degrees
    Double_t       fGridFineBin; // degrees

//    Float_t fAlphaCut;           // Alpha cut
//    Float_t fBgMean;             // Background mean

//    Float_t fMinDist;            // Min dist
//    Float_t fMaxDist;            // Max dist

//    Float_t fMinLD;              // Minimum distance in percent of dist
//    Float_t fMaxLD;              // Maximum distance in percent of dist

    Int_t fNumalphahist;		// number of histograms for alpha
    Int_t fNumBinsAlpha;
    Float_t fHistAlphaBinWidth;
    Float_t fAlphaLeftEdge;
    Float_t fAlphaRightEdge;
    Float_t fAlphaONMax;	//  [deg] , upper cut for alpha ON region in the alpha plot, [deg], ON region in the alpha plot, maybe 5 deg is better,  NOTE: up to now only values of 5, 10, 15, 20 degrees are possible
    Float_t fAlphaBgLow;	   // lower limit for bg region in the ON alpha plot 
    Float_t fAlphaBgUp;		   // upper limit for bg region in the ON alpha plot
    
    TH2D     fHistSignif;          // sky plot of significance vs. x and y
    TH2D     fHistNexcess;         // sky plot of number of excess events vs. x and y
    TH2D     fHistOn;              // sky plot of events below fAlphaONMax vs. x and y
    TH1D     fHistSignifGaus;      // distribution of significance
    Bool_t   fSetCenter;           // used to set the center of these histograms once
    Bool_t   fUseRF;               // use RF hadronness cut instead of supercuts
    Double_t fRa0;		   //    
    Double_t fDec0;		   //
    Bool_t   fSaveAlphaPlots;	   //
    Bool_t   fSaveSkyPlots;	   //
    Bool_t   fSaveNexPlot;	   //
				   
    Float_t fMinXGrid;		   //  [deg] , left edge of the skyplot
    Float_t fMaxXGrid;		   //  [deg] , right edge of the skyplot
    Float_t fMinYGrid;		   //  [deg] , upper edge of the skyplot
    Float_t fMaxYGrid;		   //  [deg] , lower edge of the skyplot
    Float_t fBinStepGrid;	   //  [deg] , grid size
    Int_t fNumStepsX;		   // number of bins in x 
    Int_t fNumStepsY;		   // number of bins in y


    // some filter cuts:
    Float_t fSizeMin;		// min size in photons
    Float_t fSizeMax;		// max size in photons
    Float_t fLeakMax;		// leakmax  in per cent
    Float_t fMaxDist;          // dist max cut (ever possible)
    Float_t fMinDist;          // dist min cut (ever possible)
    Float_t fHadrCut;          // hadronness cut 

    // coefficients for the cuts:
    Double_t fLengthUp[8], fLengthLo[8], fWidthUp[8], fWidthLo[8], fDistUp[8], fDistLo[8];

    Double_t fRa;
    Double_t fDec;

    TString fAlphaHName;          // name for histogram with alpha plots
    TString fSkyHName;            // name for histogram with sky plots
    TString fNexHName;            // name for canvas with Nex plot
    Float_t fElaTime;             // elapsed time [min]

    TObject *GetCatalog();

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();


public:
    MSkyPlot(const char *name=NULL, const char *title=NULL);
    ~MSkyPlot();

    Double_t CalcLimit(Double_t *a, Double_t ls, Double_t ls2, Double_t dd2);

    TH2D *GetHistSignif    () { return &fHistSignif;  }
    TH2D *GetHistNexcess   () { return &fHistNexcess; }
    TH2D *GetHistOn        () { return &fHistOn;      }
    TH1D *GetHistSignifGaus() { return &fHistSignifGaus; }

    Int_t GetSize()  const { return fHistAlpha->GetSize(); }

    TH1D *GetAlphaPlot( const Int_t i=-1);
    const TH1D *GetAlphaPlot( const Int_t=-1) const;

    void ReadCuts(const TString parSCinit);

    void SaveAlphaPlots(const TString stri2);
    void SaveNexPlot(const TString stri3);
    void SaveSkyPlots(TString stri);

    void SetAlphaCut(Float_t alpha); 
    void SetAlphaBGLimits(Float_t alphalow, Float_t alphalup); 

    void SetMinDist(Float_t dist) { fMinDist = dist; } // Absolute minimum distance
    void SetMaxDist(Float_t dist) { fMaxDist = dist; } // Absolute maximum distance
    void SetSizeMin(Float_t size) { fSizeMin = size; } // Absolute minimum Size
    void SetSizeMax(Float_t size) { fSizeMax = size; } // Absolute maximum Size
    void SetSkyPlot(Float_t xmin, Float_t xmax, Float_t ymin, Float_t ymax, Float_t step);
    void SetHadrCut(Float_t b)    { fHadrCut = b;    }  // hadronness cut

    void SetOutputSkyName(TString outname2)     { fSkyHName = outname2; }
    void SetNotSaveSkyPlots()                   { fSaveSkyPlots = kFALSE; }

    void SetOutputAlphaName(TString outname1)   { fAlphaHName = outname1; }
    void SetNotSaveAlphaPlots()                 { fSaveAlphaPlots = kFALSE; }

    void SetOutputNexName(TString outname3)     { fNexHName = outname3; }
    void SetElapsedTime(Float_t g)              { fElaTime = g; }
    void SetNotSaveNexPlot()                    { fSaveNexPlot = kFALSE; }

    void SetUseRF()                             { fUseRF = kTRUE; }

    ClassDef(MSkyPlot, 1) //2D-histogram in alpha, x and y
};

#endif
