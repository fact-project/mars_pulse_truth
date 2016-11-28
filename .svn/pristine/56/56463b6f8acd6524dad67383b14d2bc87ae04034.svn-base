#ifndef MARS_MInputStreamID
#define MARS_MInputStreamID

///////////////////////////////////////////////////////////////////////
//                                                                   //
// MInpuStreamID                                                     //
//                                                                   //
// Assigns an ID to tasks or a task list                             //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

// -----------------------------------------------------------------------

class MInputStreamID : public MParContainer
{
private:
    TString fStreamId;	//! which type of task should be processed?

public:
    MInputStreamID(const char *name=NULL, const char *title=NULL);

    Bool_t HasStreamId() const { return fStreamId.CompareTo("all", TString::kIgnoreCase)!=0; }

    const TString &GetStreamId() const { return fStreamId; }
    void SetStreamId(const char *t)    { fStreamId = t; }

    Bool_t operator==(MInputStreamID &id) const
    {
        return fStreamId == id.fStreamId;
    }

    Bool_t operator!=(MInputStreamID &id) const
    {
        return fStreamId != id.fStreamId;
    }

    ClassDef(MInputStreamID, 0)	//Assigns an Id to tasks or a task list
};

// ---------------------------------------------------------------------

#endif
