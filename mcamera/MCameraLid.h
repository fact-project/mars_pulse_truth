#ifndef MARS_MCameraLid
#define MARS_MCameraLid

// Derived from MParContainer instead of TObject
// to supress writing fUniqueId and fBits
#ifndef MARS_MParContainer
#include <MParContainer.h>
#endif

class MCameraLid : public MParContainer
{
    friend class MReportCamera;
private:
     Bool_t fLimitOpen;        // 0=not active, 1= active
     Bool_t fLimitClose;       // 0=not active, 1= active

     Bool_t fSafetyLimitOpen;  // 0=not active, 1= active
     Bool_t fSafetyLimitClose; // 0=not active, 1= active

     Byte_t fStatusLid;        // 0=positioning, 1=open, 2=closed
     Byte_t fStatusMotor;      // 0=stopped, 1=opening, 2=closing

public:
    MCameraLid()
    {
        fName  = "MCameraLid";
        fTitle = "Container storing information about a Camera lid";
    }

    Bool_t GetLimitOpen() const        { return fLimitOpen;        }
    Bool_t GetLimitClose() const       { return fLimitClose;       }

    Bool_t GetSafetyLimitOpen() const  { return fSafetyLimitOpen;  }
    Bool_t GetSafetyLimitClose() const { return fSafetyLimitClose; }

    Byte_t GetStatusLid() const        { return fStatusLid;        }
    Byte_t GetStatusMotor() const      { return fStatusMotor;      }

    ClassDef(MCameraLid, 1) // Container storing information about a Camera lid
};

#endif
