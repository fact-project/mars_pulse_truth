#ifndef MARS_MBadPixelsIntensityCam
#define MARS_MBadPixelsIntensityCam

#ifndef MARS_MBadPixelsPix
#include "MBadPixelsPix.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif
#ifndef MARS_MBadPixelsCam
#include "MBadPixelsCam.h"
#endif
#ifndef MARS_MGeomCamMagic
#include "MGeomCamMagic.h"
#endif

#ifndef ROOT_TOrdCollection
#include <TOrdCollection.h>
#endif

class TGraph;
class MHCamera;

class MBadPixelsIntensityCam : public MParContainer, public MCamEvent
{
private:

  TOrdCollection *fCams;        //-> Array of MBadPixelsCams, one per pulse colour and intensity

  void Add(const UInt_t from, const UInt_t to);
  
public:

  MBadPixelsIntensityCam(const char *name=NULL, const char *title=NULL);
  ~MBadPixelsIntensityCam();
  
  void  Clear ( Option_t *o="" );
  void  Print ( Option_t *o="" ) const;
  void  Copy  ( TObject& object) const;  
  
  void AddToList( const char* name, const MGeomCam &geom );

  // Getters
  Int_t GetSize() const;

        MBadPixelsCam *GetCam     ( Int_t i=-1);
  const MBadPixelsCam *GetCam     ( Int_t i=-1) const;
  /*
        MBadPixelsCam *GetCam     ( const char *name );
  const MBadPixelsCam *GetCam     ( const char *name ) const;
  */

  // The "DeleteOldCam" function must not delete the first entry in
  // the array because it is a special cam from the MParList. (see above)
  void DeleteOldCam(MBadPixelsCam *cam) {/* if (fCams->IndexOf(cam)>0) delete fCams->Remove(cam);*/ }

        MBadPixelsPix &operator[] ( Int_t i );
  const MBadPixelsPix &operator[] ( Int_t i )  const;
 
  Short_t GetNumUnsuitable(MBadPixelsPix::UnsuitableType_t type,const MGeomCam *geom,Int_t aidx=-1) 
	const { return GetCam()->GetNumUnsuitable(type,geom,aidx); }
  Short_t GetNumUnsuitable(MBadPixelsPix::UnsuitableType_t type) 
	const { return GetCam()->GetNumUnsuitable(type); }
  Short_t GetNumIsolated(MBadPixelsPix::UnsuitableType_t type,const MGeomCam &geom,Int_t aidx=-1) 
	const { return GetCam()->GetNumIsolated(type,geom,aidx); }
  Short_t GetNumIsolated(const MGeomCam &geom,Int_t aidx=-1) 
	const { return GetCam()->GetNumIsolated(geom, aidx); }
  Short_t GetNumMaxCluster(MBadPixelsPix::UnsuitableType_t type,const MGeomCam &geom,Int_t aidx=-1) 
	const { return GetCam()->GetNumMaxCluster(type,geom,aidx); }
  Short_t GetNumMaxCluster(const MGeomCam &geom,Int_t aidx=-1)
        { return GetCam()->GetNumMaxCluster(geom, aidx); }
  
  void   AsciiRead(istream &fin) { GetCam()->AsciiRead(fin); }
  Bool_t AsciiWrite(ostream &out) const { return GetCam()->AsciiWrite(out); }
  
  // Inits
  void Init   ( const MGeomCam &geom );
  void InitSize( const UInt_t i );

  Bool_t GetPixelContent ( Double_t &val, Int_t idx, const MGeomCam &cam,Int_t type=0) const;
  void   DrawPixelContent( Int_t num) const;

  TGraph *GetUncalibratedPerAreaVsTime( const MBadPixelsPix::UncalibratedType_t typ,
                                        const Int_t aidx=-1, const MGeomCam &geom=MGeomCamMagic());
  TGraph *GetUnsuitablePerAreaVsTime( const MBadPixelsPix::UnsuitableType_t typ,
                                      const Int_t aidx=-1, const MGeomCam &geom=MGeomCamMagic());

  MHCamera *GetUnsuitableSpectrum( const MBadPixelsPix::UnsuitableType_t typ, const MGeomCam &geom=MGeomCamMagic());

  MHCamera *GetUncalibratedSpectrum( const MBadPixelsPix::UncalibratedType_t typ, const MGeomCam &geom=MGeomCamMagic());

  void DrawUncalibratedPerAreaVsTime( const MBadPixelsPix::UncalibratedType_t typ,
                                        const Int_t aidx=-1, const MGeomCam &geom=MGeomCamMagic()); // *MENU*
  void DrawUnsuitablePerAreaVsTime( const MBadPixelsPix::UnsuitableType_t typ,
                                      const Int_t aidx=-1, const MGeomCam &geom=MGeomCamMagic()); // *MENU*
  


  ClassDef(MBadPixelsIntensityCam, 1) // Base Container Intensity BadPixels Results
};

#endif
