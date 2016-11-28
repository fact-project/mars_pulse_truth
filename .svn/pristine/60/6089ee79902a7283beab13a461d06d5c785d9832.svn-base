#ifndef MARS_MGeomPMT
#define MARS_MGeomPMT

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif
#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MGeomPMT : public MParContainer
{
private:
    Int_t   fPMTId;        // the PMT Id

    TArrayF fWavelength;  // List of wavelength
    TArrayF fQE;          // QE values

public:

    MGeomPMT(Int_t pmt=-1,const char *name=NULL, const char *title=NULL);

    Int_t   GetPMTId() const         { return fPMTId;   }

    void    SetArraySize(Int_t dim) { fWavelength.Set(dim); fQE.Set(dim); }

    void    SetPMTContent(Int_t pmt, const TArrayF &wav, const TArrayF &qe);

    ClassDef(MGeomPMT, 1)  // class containing information about PMTs
};

#endif


