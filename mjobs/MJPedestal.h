#ifndef MARS_MJPedestal
#define MARS_MJPedestal

#ifndef MARS_MJCalib
#include "MJCalib.h"
#endif
#ifndef MARS_MPedestalCam
#include "MPedestalCam.h"
#endif
#ifndef MARS_MHPedestalCam
#include "MHPedestalCam.h"
#endif
#ifndef MARS_MBadPixelsCam
#include "MBadPixelsCam.h"
#endif

class TCanvas;
class MParList;
class MHCamera;
class TH1D;
class MExtractor;
class MExtractPedestal;
class MEvtLoop;

class MJPedestal : public MJCalib
{
private:
    static const TString  fgReferenceFile;    //! default for fReferenceFile ("pedestalref.rc")
    static const TString  fgBadPixelsFile;    //! default for fBadPixelsFile ("badpixels_0_559.rc")

    static const Float_t  fgExtractWinLeft;   //! default for fExtractWinLeft  (now set to: 2.5)
    static const Float_t  fgExtractWinRight;  //! default for fExtractWinRight (now set to: 6.0)

    Axis_t fPedestalMin;                     //! Minimum Axis value for pedestal datacheck display
    Axis_t fPedestalMax;                     //! Maximum Axis value for pedestal datacheck display
    Axis_t fPedRmsMin;                       //! Minimum Axis value for ped. RMS datacheck display
    Axis_t fPedRmsMax;                       //! Maximum Axis value for ped. RMS datacheck display
				             
    Float_t fRefPedClosedLids;               //! Reference line pedestal for closed lids run
    Float_t fRefPedExtraGalactic;            //! Reference line pedestal for extragalactic source
    Float_t fRefPedGalactic;                 //! Reference line pedestal for galactic source

    Float_t fRefPedRmsClosedLidsInner;       //! Ref. line ped. RMS for closed lids run - inner pixels
    Float_t fRefPedRmsExtraGalacticInner;    //! Ref. line ped. RMS for extragalactic source - inner pixels
    Float_t fRefPedRmsGalacticInner;         //! Ref. line ped. RMS for galactic source - inner pixels

    Float_t fRefPedRmsClosedLidsOuter;       //! Ref. line ped. RMS for closed lids run - outer pixels
    Float_t fRefPedRmsExtraGalacticOuter;    //! Ref. line ped. RMS for extragalactic source - outer pixels
    Float_t fRefPedRmsGalacticOuter;         //! Ref. line ped. RMS for galactic source - outer pixels

    Float_t fExtractWinLeft;                 //  Number of FADC slices to extract leftward from mean pulse pos.  
    Float_t fExtractWinRight;                //  Number of FADC slices to extract rightward from mean pulse pos.

    TString fReferenceFile;                  // File name containing the reference values
    TString fBadPixelsFile;                  // File name containing the bad pixels excluded beforehand 

    MExtractor *fExtractor;                  // Signal extractor, used to find the nr. of used FADC slices

    MPedestalCam  fPedestalCamIn;            // Handed over pedestal results
    MPedestalCam  fPedestalCamOut;           // Created pedestal results
    MBadPixelsCam fBadPixels;                // Bad Pixels
    //    MHPedestalCam fPedestalHist;             // Histogramming pedestal Camera

    enum Display_t { kDisplayNone, kDisplayNormal, kDisplayDataCheck }; // Possible Display types
    Display_t fDisplayType;                  // Chosen Display type

    enum Extract_t { kUseData, kUsePedRun }; // Possible flags for the extraction of the pedestal
    Extract_t fExtractType;                  // Chosen extractor type

    enum Extraction { kFundamental, kWithExtractorRndm, kWithExtractor };
    Int_t fExtractionType;                   // Flag if the extractor is used to calculate the pedestals

//    Bool_t fIsUseHists;                      // Switch on histogramming or not
    Bool_t fDeadPixelCheck;                  // Should the dead pixel check be done?

    UInt_t  fMinEvents;                      // Minimum number of events
    UInt_t  fMinPedestals;                   // Minimum number of events
    UInt_t  fMaxPedestals;                   // Maximum number of events for pedestal extraction
    UInt_t  fMinCosmics;                     // Minimum number of events
    UInt_t  fMaxCosmics;                     // Maximum number of events for pulse    extraction

    MExtractor *ReadCalibration();
    Bool_t ReadPedestalCam();
    //Bool_t WriteResult();
    Bool_t WritePulsePos(TObject *obj) const;
    //Bool_t WriteEventloop(MEvtLoop &evtloop) const;

    void   DisplayResult(const MParList &plist);
    void   DisplayReferenceLines(const MHCamera &hist, const Int_t what) const;
    //void   DisplayOutliers(TH1D *hist) const;
    void   FixDataCheckHist(TH1D *hist) const;
    Int_t  PulsePosCheck(const MParList &plist) const;

    Bool_t CheckEnvLocal();

    const char*  GetOutputFileName() const;

    void ReadReferenceFile();

    Bool_t WriteExtractor() const;

public:
    MJPedestal(const char *name=NULL, const char *title=NULL);
    ~MJPedestal();

    MPedestalCam &GetPedestalCam()            { return fPedestalCamOut; }
    const MBadPixelsCam &GetBadPixels() const { return fBadPixels;   }

    //    const MHPedestalCam &GetPedestalHist() const { return fPedestalHist;  }

    const Bool_t IsUseData() const { return fExtractType == kUseData; }

    Int_t Process();

    void SetBadPixels(const MBadPixelsCam &bad) { bad.Copy(fBadPixels); }
    void SetPedestals(const MPedestalCam  &ped) { ped.Copy(fPedestalCamIn); }
    void SetExtractor(MExtractor* ext);
    void SetUseData()                           { fExtractType = kUseData;   }
//    void SetUseHists( const Bool_t b=kTRUE)     { fIsUseHists = b;  }
    void SetDeadPixelCheck(const Bool_t b=kTRUE) { fDeadPixelCheck = b; }
    void SetUsePedRun()                         { fExtractType = kUsePedRun; }
    void SetDataCheckDisplay()                  { fDisplayType = kDisplayDataCheck; }
    void SetNormalDisplay()                     { fDisplayType = kDisplayNormal;    }
    void SetNoDisplay()                         { fDisplayType = kDisplayNone; }

    void SetExtractionFundamental()       { fExtractionType=kFundamental;       /*fPedestalCamOut.SetName("MPedestalFundamental");*/   }
    void SetExtractionWithExtractorRndm() { fExtractionType=kWithExtractorRndm; /*fPedestalCamOut.SetName("MPedestalExtractorRndm");*/ }
    void SetExtractionWithExtractor()     { fExtractionType=kWithExtractor;     /*fPedestalCamOut.SetName("MPedestalExtractor");*/     }

    void SetReferenceFile( const TString ref=fgReferenceFile ) { fReferenceFile = ref; }
    void SetBadPixelsFile( const TString ref=fgBadPixelsFile ) { fBadPixelsFile = ref; }

    void SetExtractWinLeft ( const Float_t f=fgExtractWinLeft  ) { fExtractWinLeft  = f; }
    void SetExtractWinRight( const Float_t f=fgExtractWinRight ) { fExtractWinRight = f; }    

    MExtractor *GetExtractor() const { return fExtractor; }

    ClassDef(MJPedestal, 0) // Tool to create a pedestal file (MPedestalCam)
};

#endif
