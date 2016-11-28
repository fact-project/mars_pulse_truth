#ifndef MARS_MRawCrateArray
#define MARS_MRawCrateArray
///////////////////////////////////////////////////////////////////////
//                                                                   //
// MRunHeader                                                        //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class TClonesArray;
class MRawCrateData;

class MRawCrateArray : public MParContainer
{
private:
    TClonesArray *fArray;

public:
    MRawCrateArray(const char *name=NULL, const char *title=NULL);
    ~MRawCrateArray();

    void Clear(Option_t *opt=NULL);
    void Print(Option_t *t=NULL) const;

    void SetSize(Int_t i);
    Int_t GetSize() const;
    MRawCrateData *GetEntry(Int_t i);

    MRawCrateData* &operator[](Int_t i);

    TClonesArray **GetArray();

    ClassDef(MRawCrateArray, 1)	// Mapping container for the MRawCrateData TClonesArray
};

#endif
