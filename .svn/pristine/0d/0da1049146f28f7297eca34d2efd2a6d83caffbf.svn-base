#ifndef MARS_MMcTrigHeader
#define MARS_MMcTrigHeader

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MTriggerDefine
#include "MTriggerDefine.h"
#endif
#ifndef MARS_Mdefine
#include "Mdefine.h"
#endif

class MMcTrigHeader : public MParContainer{
 private:

  Short_t fTopology     ;      // Topology of the trigger
                                   // 0 = N-1 neighbours of one pixel
                                   // 1 = N neighbours
                                   // 2 = N closed pack
  Short_t fMultiplicity ;      // Pixel multiplicity of trigger configuration
  Float_t fThreshold[CAMERA_PIXELS] ; // Threshold for trigger
  UInt_t fTrigPattern[2];     // Used to inddentify with 
                              // RawEvt::Trigger Pattern identification  
  Float_t  fTrigShape   ;   // a number that indicate the shape type of 
                            // the signal   
                            // = 0 --> a gaussian  
  Float_t  fAmplTrig    ;   // the amplitude of the trigger in mV
  Float_t  fFwhmTrig    ;   // the width of the signal in nsec
  Float_t  fOverlapingTime;// Minimum coincidence time
  Float_t  fGateLeng;       // the length of the digital signal if analog 
                            // signal is above threshold
  Float_t  fElecNoiseTrig;  // The width of the gaussian noise is that times
                            // the amplitude of the single phe response 
                            // for the trigger

  Bool_t  fGainFluctuations;
  // kTRUE if PMT gain fluctuations were simulated for the signal 
  // (=> default in camera simulation)

  Bool_t  fNoiseGainFluctuations;
  // kTRUE if PMT gain fluctuations were simulated for the NSB noise 
  // (=> default in StarResponse program)


 public:
  MMcTrigHeader() ;

  void Print(Option_t *opt=NULL) const;
  
  void SetTopology(Short_t nTop) {
    fTopology=nTop;
  }

  void SetMultiplicity(Short_t nMul) {
    fMultiplicity=nMul;
  }

  void SetThreshold(Float_t fthr[]){
    int i;
    for(i=0;i<CAMERA_PIXELS;i++){
      fThreshold[i]=fthr[i];
    }
  }

  void SetTrigPattern (UInt_t upi, UInt_t loi){

    if (upi==0 && loi==0) {
      fTrigPattern[0]= (UInt_t) fThreshold[0];
      fTrigPattern[1]= (UInt_t) (100*fMultiplicity+fTopology);
    }
    else {
      fTrigPattern[0]=upi;
      fTrigPattern[1]=loi;
    }
  }

  void SetShape(Float_t shape){
    fTrigShape=shape;
  }

  void SetAmplitud(Float_t amp){
    fAmplTrig=amp;
 }

  void SetFwhm(Float_t fwhm){
    fFwhmTrig=fwhm;
  }

  void SetOverlap(Float_t overlap){
    fOverlapingTime=overlap;
  }

  void SetGate(Float_t gate){
    fGateLeng=gate;
  }

  void SetElecNoise( Float_t elecnoise){
    fElecNoiseTrig = elecnoise;
  }

  void SetGainFluctuations(Bool_t x) { fGainFluctuations = x; }
  void SetNoiseGainFluctuations(Bool_t x) { fNoiseGainFluctuations = x; }

  Short_t GetMultiplicity() { return fMultiplicity; }

  Float_t GetMeanThreshold()
  {
    int i;
    Float_t sum = 0., trigpix = 0.;
    for(i=0;i<CAMERA_PIXELS;i++)
    {
      if (fThreshold[i] < 1.e6 && fThreshold[i] > 0.)
      {
        sum += fThreshold[i];
        trigpix += 1.;
      }
    }

    return (sum / trigpix);
  }


  ClassDef(MMcTrigHeader, 5)  //Stores Montecarlo Information which describes the used trigger

};

#endif
