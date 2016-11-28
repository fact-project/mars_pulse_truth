#ifndef MARS_MIter
#define MARS_MIter

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MIter                                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TIterator
#include <TIterator.h>
#endif

class MParContainer;

class MIter
{
private:
   TIterator *fIterator;         //collection iterator

protected:
   MIter() : fIterator(0) { }

public:
   MIter(const TCollection *col, Bool_t dir = kIterForward)
        : fIterator(col ? col->MakeIterator(dir) : 0) { }
   MIter(TIterator *it) : fIterator(it) { }
   MIter(const MIter &iter);
   MIter &operator=(const MIter &rhs);
   virtual            ~MIter() { SafeDelete(fIterator) }
   MParContainer     *operator()() { return (MParContainer*)(fIterator ? fIterator->Next() : 0); }
   MParContainer     *Next() { return (MParContainer*)(fIterator ? fIterator->Next() : 0); }
   const TCollection *GetCollection() const { return fIterator ? fIterator->GetCollection() : 0; }
   Option_t          *GetOption() const { return fIterator ? fIterator->GetOption() : ""; }
   void               Reset() { if (fIterator) fIterator->Reset(); }

   ClassDef(MIter,0)  //Iterator wrapper
};

#endif
