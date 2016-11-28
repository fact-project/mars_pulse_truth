#ifndef MARS_MMcFadcHeader
#define MARS_MMcFadcHeader

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

#include "MFadcDefine.h"

class MMcFadcHeader : public MParContainer, public MCamEvent {
 private:

  Float_t  fFadcShape   ;   // a number that indicate the shape type of 
                            // the signal   
  Float_t  fFadcShapeOuter; // a number that indicate the shape type of 
                            // the signal   
                            // = 0 --> a gaussian  
                            // = 1 --> from Pulpo set-up  
 
  // NOTE : meaning of fAmplFadc, fAmplFadcOuter changed in camera 0.7, 
  // 30/03/2004: before it was amplitude of (gaussian) pulse, now is 
  // integral of pulse (which may be gaussian or not).

  Float_t  fAmplFadc    ;   // the integral of single phe response [counts]
  Float_t  fFwhmFadc    ;   // the width of the signal in nsec
  Float_t  fAmplFadcOuter;  // the integral of single phe response [counts], 
                            // outer pixels
  Float_t  fFwhmFadcOuter;  // the width of the signal in nsec, outer

  Float_t  fPedesMean[MFADC_CHANNELS]    ;  //  The mean value for the pedestal
                                            //  of each pixel (channel)
  Float_t  fPedesSigmaHigh[MFADC_CHANNELS]    ; //  The sigma for the pedestal
                                                //  of each pixel (channel)
  Float_t  fPedesSigmaLow[MFADC_CHANNELS]    ;  //  The sigma for the pedestal
                                                //  of each pixel (channel)

  // In camera 0.7, the meaning of fPedesSigmaHigh, fPedesSigmaLow changed:
  // before it was the rms of the single FADC slice. Now we calculate the 
  // RMS of the distribution of the sum of 14 FADC slices. The value we set 
  // as fPedesSigmaHigh/Low is that RMS divided by sqrt(14). It can be seen
  // that the fluctuations of the integrated pedestal, when adding n slices 
  // to obtain the pixel signal, with n>~6, is more or less well 
  // approximated by sqrt(n)*RMS(sum_14)slices)/sqrt(14).


  Float_t  fElecNoise[MFADC_CHANNELS]   ;  //  The rms value in the pedestal 
                                           //  due to the electronics for
                                           //  each pixel (channel)
  Float_t  fDigitalNoise[MFADC_CHANNELS];  //  The rms value in the pedestal 
                                           //  due to the digital for
                                           //  each pixel (channel)
  Float_t fLow2HighGain;    // low gain factor 

  TString fElecNoiseFileName; 
  // Name of the file from which the electronic noise used in the 
  // simulation has been read in, if the input card option "fadc_noise_from_file"
  // of the camera simulation has been chosen.

  Bool_t  fGainFluctuations;
  // kTRUE if PMT gain fluctuations were simulated for the signal 
  // (=> default in camera simulation)

  Bool_t  fNoiseGainFluctuations;
  // kTRUE if PMT gain fluctuations were simulated for the NSB noise 
  // (=> default in StarResponse program)


 public:
  MMcFadcHeader(const char *name=NULL, const char *title=NULL);

  void Print(Option_t *opt=NULL) const;
  
  void SetShape(Float_t shape){
    fFadcShape=shape;
  }

  void SetShapeOuter(Float_t shape){
    fFadcShapeOuter=shape;
  }

  void SetLow2High(Float_t l2h){
    fLow2HighGain=l2h;
  }

  void SetAmplitud(Float_t amp, Float_t ampout=0.0){
    fAmplFadc=amp;
    fAmplFadcOuter=ampout;
 }

  void SetFwhm(Float_t fwhm, Float_t fwhmout=0.0){
    fFwhmFadc=fwhm;
    fFwhmFadcOuter=fwhmout;
  }

  void SetPedestal(Float_t *mean, Int_t dim){
    for (Int_t i=0;i<dim;i++)
      fPedesMean[i]=mean[i];
  }

  void SetPedestalSigma(Float_t *sigmalo, Float_t *sigmahi, Int_t dim){
    for (Int_t i=0;i<dim;i++){
      fPedesSigmaLow[i]=sigmalo[i];
      fPedesSigmaHigh[i]=sigmahi[i];
    }
  }

  void SetElecNoise(Float_t *sigmae, Float_t *sigmad, Int_t dim){
    for (Int_t i=0;i<dim;i++){
      fElecNoise[i]=sigmae[i];
      fDigitalNoise[i]=sigmad[i];
    }
  }
  
  void SetGainFluctuations(Bool_t x) { fGainFluctuations = x; }
  void SetNoiseGainFluctuations(Bool_t x) { fNoiseGainFluctuations = x; }

  Float_t GetPedestal(UInt_t i) const    { return fPedesMean[i]; }
  Float_t GetPedestalRmsHigh(UInt_t i) const { return fPedesSigmaHigh[i]; }
  Float_t GetPedestalRmsLow(UInt_t i) const { return fPedesSigmaLow[i]; }
  Float_t GetElecNoise(UInt_t i) const { return fElecNoise[i]; }
  Float_t GetDigitalNoise(UInt_t i) const { return fElecNoise[i]; }
  Float_t GetAmplitud() const { return fAmplFadc; }
  Float_t GetAmplitudOuter() const { return fAmplFadcOuter; }
  Float_t GetLow2HighGain() const { return fLow2HighGain; }

  UInt_t GetNumPixel() const { return MFADC_CHANNELS; }

  // MCamEvent
  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
  void   DrawPixelContent(Int_t num) const { }

  ClassDef(MMcFadcHeader, 10)  //Stores Montecarlo Information describing the FADC behaviour
};

#endif
