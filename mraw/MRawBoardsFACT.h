#ifndef MARS_MRawBoardsFACT
#define MARS_MRawBoardsFACT

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

//gcc 3.2
//class ifstream;
#include <iosfwd>

class MRawBoardsFACT : public MParContainer
{
    friend class MRawFitsRead;

private:
    UInt_t fFadTime[40];  // Crate number the information corresponds to

public:
    MRawBoardsFACT();

    UInt_t GetFadTime(UInt_t i) const { return fFadTime[i]; }

    void Print(Option_t *t=NULL) const;

    ClassDef(MRawBoardsFACT, 1) //Container to store the Raw CRATE DATA
};

#endif
