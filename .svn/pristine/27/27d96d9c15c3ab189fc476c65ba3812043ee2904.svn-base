/* ======================================================================== *\
!  $Name: not supported by cvs2svn $:$Id: MBadPixelsCam.h,v 1.13 2009-03-01 21:48:13 tbretz Exp $
\* ======================================================================== */
#ifndef MARS_MBadPixelsCam
#define MARS_MBadPixelsCam

#ifndef MARS_MBadPixelsPix
#include "MBadPixelsPix.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

class TArrayC;
class TClonesArray;

class MGeom;

class MBadPixelsCam : public MParContainer, public MCamEvent
{
private:
    TClonesArray *fArray; //-> 

    Short_t GetNumSuitableNeighbors(MBadPixelsPix::UnsuitableType_t type, const MGeom &pix) const;
    Short_t GetNumMaxCluster(MBadPixelsPix::UnsuitableType_t type, TObjArray &list, Int_t idx, Int_t aidx) const;

    void PrintBadPixels( MBadPixelsPix::UncalibratedType_t typ, const char *text) const;
    
public:
    MBadPixelsCam(const char *name=NULL, const char *title=NULL);
    MBadPixelsCam(const MBadPixelsCam &cam);
    ~MBadPixelsCam();

    void Reset();
    void Clear(Option_t *o="");
    void Print(Option_t *o="") const; //*MENU*
    void Copy(TObject &object) const;

    void InitSize(const UInt_t i);
    Int_t GetSize() const;

    MBadPixelsPix &operator[](Int_t i);
    const MBadPixelsPix &operator[](Int_t i) const;

    void Merge(const MBadPixelsCam &cam);

    Short_t GetNumUnsuitable(MBadPixelsPix::UnsuitableType_t type, const MGeomCam *geom, Int_t aidx=-1) const;
    Short_t GetNumUnsuitable(MBadPixelsPix::UnsuitableType_t type) const { return GetNumUnsuitable(type, 0); }
    Short_t GetNumUnsuitable() const;
    Short_t GetNumSuitable(MBadPixelsPix::UnsuitableType_t type, const MGeomCam *geom, Int_t aidx=-1) const;
    Short_t GetNumSuitable(MBadPixelsPix::UnsuitableType_t type) const { return GetNumSuitable(type, 0); }
    Short_t GetNumIsolated(MBadPixelsPix::UnsuitableType_t type, const MGeomCam &geom, Int_t aidx=-1) const;
    Short_t GetNumIsolated(const MGeomCam &geom, Int_t aidx=-1) const { return GetNumIsolated(MBadPixelsPix::kUnsuitableRun, geom, aidx); }
    Short_t GetNumMaxCluster(MBadPixelsPix::UnsuitableType_t type, const MGeomCam &geom, Int_t aidx=-1) const;
    Short_t GetNumMaxCluster(const MGeomCam &geom, Int_t aidx=-1) { return GetNumMaxCluster(MBadPixelsPix::kUnsuitableRun, geom, aidx); }

    TArrayC GetUnsuitable(MBadPixelsPix::UnsuitableType_t typ=MBadPixelsPix::kUnsuitable) const;
    TArrayC GetUncalibrated(MBadPixelsPix::UncalibratedType_t typ) const;

    void   AsciiRead(std::istream &fin);
    Bool_t AsciiWrite(std::ostream &out) const;

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
    void DrawPixelContent(Int_t num) const;

    ClassDef(MBadPixelsCam, 1)	//Storage container to store bad pixel of the camera...
};

#endif

