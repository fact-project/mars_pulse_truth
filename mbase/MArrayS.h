#ifndef MARS_MArrayS
#define MARS_MArrayS

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MArrayS                                                                 //
//                                                                         //
// Array of UShort_t                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MArray
#include "MArray.h"
#endif

#include <string.h>

class MArrayS : public MArray
{
private:
    UShort_t *fArray; //[fN] Array of fN chars

public:

    MArrayS()
    {
        fN     = 0;
        fArray = NULL;
    }

    MArrayS(UInt_t n)
    {
        fN     = 0;
        fArray = NULL;
        Set(n);
    }

    MArrayS(UInt_t n, UShort_t *array)
    {
        // Create TArrayC object and initialize it with values of array.
        fN     = 0;
        fArray = NULL;
        Set(n, array);
    }

    MArrayS(const MArrayS &array) : MArray()
    {
        // Copy constructor.
        fArray = NULL;
        Set(array.fN, array.fArray);
    }

    MArrayS &operator=(const MArrayS &rhs)
    {
        // TArrayC assignment operator.
        if (this != &rhs)
            Set(rhs.fN, rhs.fArray);
        return *this;
    }

    virtual ~MArrayS()
    {
        // Delete TArrayC object.
        delete [] fArray;
        fArray = NULL;
    }

    void Adopt(UInt_t n, UShort_t *array)
    {
        // Adopt array arr into TArrayC, i.e. don't copy arr but use it directly
        // in TArrayC. User may not delete arr, TArrayC dtor will do it.
        if (fArray)
            delete [] fArray;

        fN     = n;
        fArray = array;
    }

    void AddAt(UShort_t c, UInt_t i)
    {
        // Add char c at position i. Check for out of bounds.
        fArray[i] = c;
    }

    UShort_t     At(UInt_t i)
    {
        return fArray[i];
    }

    UShort_t    *GetArray() const
    {
        return fArray;
    }

    void Reset()
    {
        memset(fArray, 0, fN*sizeof(UShort_t));
    }

    void Reset(UShort_t v)
    {
        for (UShort_t *s=fArray; s<fArray+fN; s++)
            *s = v;
    }

    void Set(UInt_t n)
    {
        // Set size of this array to n chars.
        // A new array is created, the old contents copied to the new array,
        // then the old array is deleted.

        if (n==fN)
            return;

        UShort_t *temp = fArray;
        if (n == 0)
            fArray = NULL;
        else
        {
            fArray = new UShort_t[n];
            if (n < fN)
                memcpy(fArray, temp, n*sizeof(UShort_t));
            else
            {
                memcpy(fArray, temp, fN*sizeof(UShort_t));
                memset(&fArray[fN], 0, (n-fN)*sizeof(UShort_t));
            }
        }

        if (fN)
            delete [] temp;

        fN = n;
    }

    void Set(UInt_t n, UShort_t *array)
    {
        // Set size of this array to n chars and set the contents.
        if (!array)
            return;

        if (fArray && fN != n)
        {
            delete [] fArray;
            fArray = 0;
        }
        fN = n;

        if (fN == 0)
            return;

        if (!fArray)
            fArray = new UShort_t[fN];

        memcpy(fArray, array, n*sizeof(UShort_t));
    }

    UShort_t &operator[](UInt_t i)
    {
        return fArray[i];
    }
    const UShort_t &operator[](UInt_t i) const
    {
        return fArray[i];
    }

    ClassDef(MArrayS, 1)  //Array of UShort_t
};

#endif
