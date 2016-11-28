#ifndef MARS_MCalibrationIntensityCam
#define MARS_MCalibrationIntensityCam

#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

#ifndef MARS_MArrayD
#include "MArrayD.h"
#endif

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

#ifndef ROOT_TOrdCollection
#include <TOrdCollection.h>
#endif

class MCalibrationPix;
class MBadPixelsPix;
class MGeomCam;
class MHCalibrationCam;

class MCalibrationIntensityCam : public MParContainer, public MCamEvent
{
protected:  
  TOrdCollection *fCams;   // Array of MCalibrationCams, one per pulse colour and intensity
  TOrdCollection *fHists;  // Array of MHCalibrationCam's, containing the mean charges per area histograms

  virtual void Add(const UInt_t from, const UInt_t to);
  virtual void InitSize( const UInt_t n );
  
public:
  MCalibrationIntensityCam(const char *name=NULL, const char *title=NULL);
  ~MCalibrationIntensityCam();
  
  void  Clear ( Option_t *o="" );
  void  Copy(TObject& object) const;  
  
  void AddToList(const char* name, const MGeomCam &geom);
  void AddHist(const MHCalibrationCam *cam);

  Int_t CountNumEntries(const MCalibrationCam::PulserColor_t col) const;

  const Int_t GetSize() const  { return fCams->GetSize();  }

  const Int_t            GetAverageAreas     ()            const   { return GetCam()->GetAverageAreas(); }
        MCalibrationPix &GetAverageArea      ( UInt_t i )          { return GetCam()->GetAverageArea(i);  }
  const MCalibrationPix &GetAverageArea      ( UInt_t i )  const   { return GetCam()->GetAverageArea(i);  }
        MBadPixelsPix   &GetAverageBadArea   ( UInt_t i )          { return GetCam()->GetAverageBadArea(i); }  
  const MBadPixelsPix   &GetAverageBadArea   ( UInt_t i )  const   { return GetCam()->GetAverageBadArea(i); }
  const Int_t            GetAverageSectors   ()            const   { return GetCam()->GetAverageSectors();  }
        MCalibrationPix &GetAverageSector    ( UInt_t i )          { return GetCam()->GetAverageSector(i);  }
  const MCalibrationPix &GetAverageSector    ( UInt_t i )  const   { return GetCam()->GetAverageSector(i);  }
        MBadPixelsPix   &GetAverageBadSector ( UInt_t i )          { return GetCam()->GetAverageBadSector(i); }
  const MBadPixelsPix   &GetAverageBadSector ( UInt_t i )  const   { return GetCam()->GetAverageBadSector(i); }

  MCalibrationCam *GetCam              ( Int_t i/*=-1*/)       {
      return static_cast<MCalibrationCam*>(i==-1 ? fCams->Last() : fCams->At(i)); }
  const MCalibrationCam *GetCam              ( Int_t i/*=-1*/) const {
      return static_cast<MCalibrationCam*>(i==-1 ? fCams->Last() : fCams->At(i)); }

  MCalibrationCam *GetCam() { return static_cast<MCalibrationCam*>(fCams->Last()); }
  const MCalibrationCam *GetCam() const { return static_cast<MCalibrationCam*>(fCams->Last()); }

//        MCalibrationCam *GetCam              ( const char *name );
//  const MCalibrationCam *GetCam              ( const char *name ) const;

  // The "DeleteOldCam" function must not delete the first entry in
  // the array because it is a special cam from the MParList. (see above)
  void DeleteOldCam(MCalibrationCam *cam) { /* if (fCams->IndexOf(cam)>0) delete fCams->Remove(cam);*/ }

        MHCalibrationCam *GetHist            ( Int_t i=-1);      
  const MHCalibrationCam *GetHist            ( Int_t i=-1) const;

        MHCalibrationCam *GetHist            ( const char *name );      
  const MHCalibrationCam *GetHist            ( const char *name ) const;

        MCalibrationPix &operator[]          ( UInt_t i );
  const MCalibrationPix &operator[]          ( UInt_t i )  const;
 
  const Float_t GetNumHiGainFADCSlices ( const Int_t aidx=0 ) const { return GetCam()->GetNumHiGainFADCSlices(aidx); }
  const Float_t GetNumLoGainFADCSlices ( const Int_t aidx=0 ) const { return GetCam()->GetNumLoGainFADCSlices(aidx); }
  const Int_t   GetNumUnsuitable       ( const Int_t aidx=-1) const { return GetCam()->GetNumUnsuitable(aidx);       }
  const Int_t   GetNumUnreliable       ( const Int_t aidx=-1) const { return GetCam()->GetNumUnreliable(aidx);       }
  
  // Inits
  void  Init   ( const MGeomCam &geom );
  
   // Prints
  void   Print(Option_t *o="")         const;

  // Setters
  void  SetNumHiGainFADCSlices( const Float_t f, const Int_t aidx=0) { GetCam()->SetNumHiGainFADCSlices(f,aidx); }
  void  SetNumLoGainFADCSlices( const Float_t f, const Int_t aidx=0) { GetCam()->SetNumLoGainFADCSlices(f,aidx); }
  void  SetNumUnsuitable      ( const UInt_t i,  const Int_t aidx  ) { GetCam()->SetNumUnsuitable(i,aidx);       }
  void  SetNumUnreliable      ( const UInt_t i,  const Int_t aidx  ) { GetCam()->SetNumUnreliable(i,aidx);       }
  void  SetPulserColor        ( const MCalibrationCam::PulserColor_t col=MCalibrationCam::kCT1) {
                                                                       GetCam()->SetPulserColor(col);            }
  void  SetCam( MCalibrationCam *cam, const Int_t i)                 {  fCams->PutAt(cam,i);  }
  
  // MCamEvent
  Bool_t GetPixelContent( Double_t &val, Int_t idx, const MGeomCam &cam,Int_t type=0) const 
	{ return GetCam()->GetPixelContent(val,idx,cam,type);  }
  void DrawPixelContent( Int_t num) const;

  ClassDef(MCalibrationIntensityCam, 3) // Base Container Intensity Calibration Results
};

#endif
