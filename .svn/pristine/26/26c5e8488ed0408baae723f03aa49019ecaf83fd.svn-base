#ifndef MARS_MMcTrig
#define MARS_MMcTrig

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MTriggerDefine
#include "MTriggerDefine.h"
#endif
#ifndef MARS_Mdefine
#include "Mdefine.h"
#endif

class MMcTrig : public MParContainer {
 private:

  Short_t fNumFirstLevel  ;    // Number of First Level Trigger in this Event
  Short_t fNumSecondLevel ;    // Number of Second Level Trigger in this Event

  Float_t fTimeFirst[((Int_t)(TOTAL_TRIGGER_TIME/LEVEL1_DEAD_TIME))+1];
                            // Time when it triggers
  Byte_t fPixelsFirst[((Int_t)(CAMERA_PIXELS/8))+1][((Int_t)(TOTAL_TRIGGER_TIME/LEVEL1_DEAD_TIME))+1];
                            // Pixel which are above threshold when trigger happens
  Short_t fFirstToSecond[((Int_t)(TOTAL_TRIGGER_TIME/LEVEL2_DEAD_TIME))+1];

 public:
  MMcTrig(const char *name=NULL, const char *title=NULL);

  ~MMcTrig(); 

  void Clear(Option_t *opt=NULL);
  
  void Print(Option_t *opt=NULL) const;

  void SetFirstLevel  ( Short_t nTr ) {
    fNumFirstLevel = nTr ; 
  } 
  void SetSecondLevel ( Short_t nTr ) {
    fNumSecondLevel = nTr ; 
  } 

  void SetTime( Float_t t, Int_t i);

  void SetMapPixels(Byte_t *map,Int_t nfirst){
    //  
    //  It sets the map of pixel that are above the trheshold
    //

    int i;

    for(i=0;i<((Int_t)(CAMERA_PIXELS/8))+1;i++){
      fPixelsFirst[i][nfirst]=map[i];
    }
  }

  Int_t GetFirstLevel() const {
    return ( fNumFirstLevel );  
  }

  Int_t GetNumFirstLevel() const {
    return ( fNumFirstLevel );  
  }

  Byte_t IsPixelFired(Int_t npix, Int_t nfirstlevel);

  ClassDef(MMcTrig, 4)  //Stores Montecarlo Information (number of 1st, 2nd level triggers)

};

#endif
