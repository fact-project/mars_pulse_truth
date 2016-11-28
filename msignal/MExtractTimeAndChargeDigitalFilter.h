#ifndef MARS_MExtractTimeAndChargeDigitalFilter
#define MARS_MExtractTimeAndChargeDigitalFilter

#ifndef MARS_MExtractTimeAndCharge
#include "MExtractTimeAndCharge.h"
#endif

#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif

class MCalibrationPattern;

class MExtractTimeAndChargeDigitalFilter : public MExtractTimeAndCharge
{
private:
    static const Byte_t  fgHiGainFirst;             //! Default for fHiGainFirst       (now set to: 0)
    static const Byte_t  fgHiGainLast;              //! Default for fHiGainLast        (now set to:14)
    static const Int_t   fgLoGainFirst;             //! Default for fLoGainFirst       (now set to: 3)
    static const Byte_t  fgLoGainLast;              //! Default for fLoGainLast        (now set to:14)
    static const Int_t   fgBinningResolutionHiGain; //! Default for fBinningResolutionHiGain (now set to: 10)
    static const Int_t   fgBinningResolutionLoGain; //! Default for fBinningResolutionLoGain (now set to: 10)
    static const TString fgNameWeightsFile;         //! "cosmics_weights.dat"
    static const Float_t fgOffsetLoGain;            //! Default for fOffsetLoGain (now set to 1.7)

    MCalibrationPattern  *fCalibPattern;            //! Calibration DM pattern

    Int_t   fBinningResolutionHiGain;               //  Number of weights per bin High-Gain
    Int_t   fBinningResolutionLoGain;               //  Number of weights per bin Low-Gain

    MArrayF fAmpWeightsHiGain;                      //! Amplitude weights High-Gain (from weights file)
    MArrayF fTimeWeightsHiGain;                     //! Time weights High-Gain (from weights file)
    MArrayF fAmpWeightsLoGain;                      //! Amplitude weights Low-Gain (from weights file)
    MArrayF fTimeWeightsLoGain;                     //! Time weights Low-Gain (from weights file)

    MArrayF fPulseHiGain;                           //! Pulse Shape Hi-Gain (for chisq)
    MArrayF fPulseLoGain;                           //! Pulse Shape Lo-Gain (for chisq)

    TString fNameWeightsFile;                       // Name of the weights file
    Bool_t  fAutomaticWeights;                      // Flag whether weight should be determined automatically
    TString fNameWeightsFileSet;                    //! Flag if weights have alreayd been set

    // MExtractTimeAndChargeDigitalFilter
    void    CalcBinningResArrays();
    Int_t   GetAutomaticWeights();
    Bool_t  GetWeights();
    Int_t   ReadWeightsFile(TString filename, TString path="");
    TString CompileWeightFileName(TString path, const TString &name) const;


    // MExtractTimeAndCharge
    Bool_t InitArrays(Int_t n);

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();

protected:
    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MExtractTimeAndChargeDigitalFilter(const char *name=NULL, const char *title=NULL);
    ~MExtractTimeAndChargeDigitalFilter() { }

    void SetNameWeightsFile(TString s="")
    {
        s.ReplaceAll("\015", ""); // This is a fix for TEnv files edited with windows editors
        fNameWeightsFile = s;
        fNameWeightsFileSet="";
    }

    void EnableAutomaticWeights(Bool_t b=kTRUE) { fAutomaticWeights = b; }

    void SetBinningResolution(const Int_t rh=fgBinningResolutionHiGain, const Int_t rl=fgBinningResolutionLoGain)
    {
        fBinningResolutionHiGain = rh;
        fBinningResolutionLoGain = rl;
    }

    void SetWindowSize( Int_t windowh, Int_t windowl);
    const char* GetNameWeightsFile() const  { return fNameWeightsFile.Data(); }

    void Print(Option_t *o="") const; //*MENU*

    void FindTimeAndChargeHiGain2(const Float_t *firstused, Int_t num, Float_t &sum, Float_t &dsum,
                                  Float_t &time, Float_t &dtime,
                                  Byte_t sat, Int_t maxpos) const;

    void FindTimeAndChargeLoGain2(const Float_t *firstused, Int_t num, Float_t &sum,  Float_t &dsum,
                                  Float_t &time, Float_t &dtime,
                                  Byte_t sat, Int_t maxpos) const;

    ClassDef(MExtractTimeAndChargeDigitalFilter, 3)   // Hendrik's digital filter
};

#endif
