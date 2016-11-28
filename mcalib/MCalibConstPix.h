#ifndef MARS_MCalibConstPix
#define MARS_MCalibConstPix

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MCalibConstPix : public MParContainer
{
private:
    Float_t fCalibConst;        // conversion factor (modified after each interlaced cal. update)
    Float_t fCalibFFactor;      // global F-Factor   (modified after each interlaced cal. update)

public:
    MCalibConstPix();

    // TObject
    void Clear(Option_t *o="")
    {
        fCalibConst   = -1.;
        fCalibFFactor = -1.;
    }

    void Copy(TObject &object) const
    {
        MCalibConstPix &pix =  (MCalibConstPix&)object;
        pix.fCalibConst   = fCalibConst;
        pix.fCalibFFactor = fCalibFFactor;
    }

    // Getters
    Float_t GetCalibConst()   const { return fCalibConst;   }
    Float_t GetCalibFFactor() const { return fCalibFFactor; }

    // Setters
    void SetCalibConst  ( const Float_t f )  { fCalibConst   = f; }
    void SetCalibFFactor( const Float_t f )  { fCalibFFactor = f; }

    ClassDef(MCalibConstPix, 1) // Temporay Storage Calibraion Constant of one pixel
};

#endif
