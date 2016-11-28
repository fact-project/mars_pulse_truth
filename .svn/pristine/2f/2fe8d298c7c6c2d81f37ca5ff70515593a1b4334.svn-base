/* ======================================================================== *\
!  $Name: not supported by cvs2svn $:$Id: MCameraTH.h,v 1.8 2008-06-14 14:19:03 tbretz Exp $
\* ======================================================================== */
#ifndef MARS_MCameraTH
#define MARS_MCameraTH

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_MCamEvent
#include "MCamEvent.h"
#endif

#ifndef ROOT_TArrayC
#include <TArrayC.h>
#endif

class MCameraTD;

class MCameraTH : public MParContainer, public MCamEvent
{
    friend class MReportCC;
    friend class MReportRec;
private:
    TArrayC fTH;      // [au] discriminator thresholds
    Bool_t  fIsValid; // fTH contains valid information

    Bool_t InterpreteTH(TString &str, Int_t ver, MCameraTD &td);

public:
    MCameraTH(Int_t size=577, const char *name=NULL, const char *title=NULL);

    Byte_t operator[](Int_t i) { return fTH[i]; }

    Byte_t GetMin() const;
    Byte_t GetMax() const;

    void Print(Option_t *opt=NULL) const;

    void Invalidate() { fTH.Reset(); fIsValid=kFALSE; }

    void SetValid(Bool_t v=kTRUE) { fIsValid=v; }
    Bool_t IsValid() const { return fIsValid; }

    Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const
    {
        if (idx>=fTH.GetSize())
            return kFALSE;

        // FIXME: This is a stupid workaround to describe the trigger area
        //        and only valid for the MAGIC-I camera!
        if (idx>=397 ||
            (idx>=339 && idx<=345) ||
            (idx>=279 && idx<=283) ||
            (idx>=331 && idx<=334) ||
            (idx>=394 && idx<=396) ||
            (idx>=271 && idx<=273) ||
            (idx>=329 && idx<=330) ||
            (idx>=383 && idx<=389) ||
            (idx>=319 && idx<=323) ||
            (idx>=289 && idx<=293) ||
            (idx>=350 && idx<=356) ||
            (idx>=299 && idx<=303) ||
            (idx>=361 && idx<=367) ||
            (idx>=309 && idx<=313) ||
            (idx>=372 && idx<=378))
            return kFALSE;

        val = fTH[idx];
        return val>0;
    }
    void DrawPixelContent(Int_t num) const
    {
    }

    ClassDef(MCameraTH, 2) // Storage Container for the discriminator thresholds
};

#endif
