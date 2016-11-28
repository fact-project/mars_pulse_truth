#ifndef MARS_MCameraLids
#define MARS_MCameraLids

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MCameraLid
#include "MCameraLid.h"
#endif

class MCameraLids : public MParContainer
{
    friend class MReportCamera;
private:
    MCameraLid fLidA;
    MCameraLid fLidB;

    Byte_t fStatus; // CaCo monitored lid status, Cam.LID_state [0-9]
public:
    MCameraLids()
    {
        fName  = "MCameraLids";
        fTitle = "Container storing information about the Camera lids";
    }

    Byte_t GetStatus() const { return fStatus; }

    const MCameraLid &GetLidA() const { return fLidA; }
    const MCameraLid &GetLidB() const { return fLidB; }

    ClassDef(MCameraLids, 1) // Container storing information about the Camera lids
};

#endif
