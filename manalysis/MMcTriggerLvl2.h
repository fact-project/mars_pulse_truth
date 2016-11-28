#ifndef MARS_MMcTriggerLvl2
#define MARS_MMcTriggerLvl2

#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MMcTrig;
class MMcEvt;
class MGeomCamMagic;
class MGeomCam;
class MGeomPix;

class MMcTriggerLvl2 : public MParContainer, public MCamEvent
{
 private:
  
  // Global trigger variables related to the geometry and trigger structures are here defined.  
  // FIXME!  this is a temporary solution: these variables should be defined in a GeomTrg class    

  static const Int_t fNumPixCell = 36; // Number of pixels in one cell

  static const Int_t gsNCells = 19;   
  static const Int_t gsNTrigPixels = 397;   
  static const Int_t gsNPixInCell = 36;  
  static const Int_t gsNLutInCell = 3;  
  static const Int_t gsNPixInLut = 12; 
  Int_t fPixels[gsNPixInCell][gsNCells];  
  
  // Array with flag for triggered pixels. 1st idx:pixels in trigger cell; 2nd idx:trigger cell number      

  Int_t fFiredPixel[gsNTrigPixels]; // Array with flag for triggered pixel. idx: pixel id    
  static const Int_t gsPixelsInCell[gsNPixInCell][gsNCells];   
  static const Int_t gsPixelsInLut[gsNLutInCell][gsNPixInLut];     

  Int_t fMaxCell;  // Cell with maximum number of fired pixels

  Int_t fLutPseudoSize;   // number of compact pixels in one lut   
  Int_t fPseudoSize;   // Multiplicity of the cluster identified by the L2T    
  Int_t fSizeBiggerCell; // Number of fired pixel in bigger cell    
  Int_t fCompactNN;    //Number of NN pixels that define a compact pixel    
  Int_t fCompactPixel[gsNTrigPixels]; //Array with flag for compact pixels    
  Int_t fCluster_pix[gsNTrigPixels]; //Array with pixel in cluster   
  Double_t fEnergy;  // Energy of the shower    
  Int_t fTriggerPattern; // x-NN compact trigger pattern;    
  Int_t fCellPseudoSize; // number of compact pixels in one cell    
  //  Int_t fCellCompactPixels[fNumPixCell]; // Array with compact pixels in cell

  MMcTrig *fMcTrig;   
  MGeomCam *fGeomCam;      

     
  Int_t CalcBiggerFiredCell();   
  Int_t CalcBiggerLutPseudoSize();   
  void CalcPseudoSize();   
  Int_t CalcCellPseudoSize();   
  Int_t CalcBiggerCellPseudoSize();    
  Int_t GetCellCompactPixel(int cell, MGeomCam *fCam);

  void SetNewCamera(MGeomCam *geom) {fGeomCam = geom;}   

 public:     

  MMcTriggerLvl2(const char* name = NULL, const char* title = NULL);   
  ~MMcTriggerLvl2();      

  virtual void Calc();    
  virtual void Print(Option_t *opt="") const;    

  void SetLv1(MMcTrig *trig = NULL);    
  void SetPixelFired(Int_t pixel, Int_t fired=1);     
  void SetCompactNN(Int_t neighpix)     {fCompactNN=neighpix;}    

  Int_t GetPseudoSize() const     {return fPseudoSize;}   
  Int_t GetLutPseudoSize() const  {return fLutPseudoSize;}   
  Int_t GetSizeBiggerCell() const {return fSizeBiggerCell;}   
  Int_t GetCompactNN() const      {return fCompactNN;}   
  Int_t GetCellPseudoSize() const {return fCellPseudoSize;}
  Int_t GetMaxCell() const        {return (fMaxCell+1);} // Returns
                     // cell with maximum number of compact pixels
  Int_t GetCellNumberFired(int cell);   
  Int_t GetLutCompactPixel(int cell, int lut); 
  void CalcCompactPixels(MGeomCam *fCam);
  void CalcTriggerPattern(MGeomCam *fCam);
    
  Int_t GetTriggerPattern() const {return fTriggerPattern;}    

  void GetEnergy(MMcEvt *fMcEvt = NULL);     
  Double_t GetEnergy() const    {return fEnergy;}      

  Bool_t IsPixelInCell(Int_t pixel, Int_t cell);    
  Bool_t IsPixelInTrigger(Int_t pixel) const;    

  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
  void DrawPixelContent(Int_t num) const;


  ClassDef(MMcTriggerLvl2,0) // Container for 2nd Level Trigger selection parameters 
    };  
    
#endif      
    
