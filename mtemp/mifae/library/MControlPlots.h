#ifndef MARS_MControlPlots
#define MARS_MControlPlots

#ifndef MARS_MTask
#include "MTask.h"
#endif

class TString;
class MIslands;
class MImgIsland;
class MGeomCam;
class MHCamera;

class MControlPlots : public MTask
{
 public:
  enum OnOffMode_t {kOn=0,kOff=1};
  
 private:
  OnOffMode_t  fMode;           // On/Off data mode 
  TString      fFileName;       // name of the ps file
  MGeomCam*    fGeomCam;        // pointer to camera geometry object
  MIslands*    fIslands;        // pointer to the island object
  MHCamera*    fCameraHisto[2]; // pointer to camera histos
  Bool_t       fProduceFile;    // flag to produce the ouput (ps) file
  
  Int_t PreProcess(MParList *plist);
  Int_t Process();
  Int_t PostProcess();

 public:
  MControlPlots(TString filename="",const char* name=NULL, const char* title=NULL);

  virtual ~MControlPlots();

  void Reset() { Clear(); }
  void Clear(const Option_t *o="");

  void SetFilename(TString fname)           {fFileName=fname;}
  void SetMode(OnOffMode_t mode)            {fMode=mode;}
  void SetProduceFile(Bool_t mod=kTRUE)     {fProduceFile=mod;}  

  ClassDef(MControlPlots, 0) // task to produce some control plots
};

#endif

