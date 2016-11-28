#ifndef MARS_MArray
#define MARS_MArray

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MArray                                                                  //
//                                                                         //
// Abstract array base class for TObject derived Arrays                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

class MArray : public TObject
{
protected:
    UInt_t fN; // Number of array elements

public:
   MArray()                              { fN = 0; }
   MArray(UInt_t n)                      { fN = n; }
   MArray(const MArray &a) : TObject()   { fN = a.fN; }
   virtual ~MArray()                     { fN = 0; }

   MArray &operator=(const MArray &rhs)  { fN = rhs.fN; return *this; }

   UInt_t       GetSize() const          { return fN; }
   virtual void Set(UInt_t n) = 0;
   
   ClassDef(MArray, 1)  //Abstract array base class for TObject derived Arrays
};

#endif
