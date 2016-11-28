#ifndef MARS_MGainFluctuationPix_H
#define MARS_MGainFluctuationPix_H

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MGainFluctuationPix : public MParContainer
{
private:

    Int_t    fPixId;     // the pixel Id

    Float_t  fGain;      // The relative gain repect to nominal one

public:
    MGainFluctuationPix(Int_t pix=-1, Float_t gain=0);

    Int_t   GetPixId() const            { return fPixId;   }
    Float_t GetGain() const             { return fGain;    }
    void    SetGain(Float_t g)          { fGain    = g; }

    void    Print(Option_t *opt = NULL) const;

    ClassDef(MGainFluctuationPix, 1)  // class containing information about the Cerenkov Photons in a pixel
};

#endif
