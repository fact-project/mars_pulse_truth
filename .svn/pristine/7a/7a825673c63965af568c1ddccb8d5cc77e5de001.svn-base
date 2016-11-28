#ifndef MARS_MArrayB
#define MARS_MArrayB

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MArrayB                                                                 //
//                                                                         //
// Array of Byte_t                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MArray
#include "MArray.h"
#endif

#include <string.h>

class MArrayB : public MArray
{
private:
    Byte_t *fArray; //[fN] Array of fN chars

public:

    MArrayB()
    {
        fN     = 0;
        fArray = NULL;
    }

    MArrayB(UInt_t n)
    {
        fN     = 0;
        fArray = NULL;

        Set(n);
    }

    MArrayB(UInt_t n, Byte_t *array)
    {
        // Create TArrayC object and initialize it with values of array.
        fN     = 0;
        fArray = NULL;

        Set(n, array);
    }

    MArrayB(const MArrayB &array) : MArray()
    {
        // Copy constructor.
        fArray = NULL;
        Set(array.fN, array.fArray);
    }

    MArrayB &operator=(const MArrayB &rhs)
    {
        // TArrayC assignment operator.
        if (this != &rhs)
            Set(rhs.fN, rhs.fArray);
        return *this;
    }

    virtual ~MArrayB()
    {
        // Delete TArrayC object.
        delete [] fArray;
        fArray = NULL;
    }

    void Adopt(UInt_t n, Byte_t *array)
    {
        // Adopt array arr into TArrayC, i.e. don't copy arr but use it directly
        // in TArrayC. User may not delete arr, TArrayC dtor will do it.
        if (fArray)
            delete [] fArray;

        fN     = n;
        fArray = array;
    }

    void AddAt(Byte_t c, UInt_t i)
    {
        // Add char c at position i. Check for out of bounds.
        fArray[i] = c;
    }

    void AddAt(Byte_t *array, UInt_t i, UInt_t n)
    {
        // Add char c at position i. Check for out of bounds.
        memcpy(fArray+i, array, n*sizeof(Byte_t));
    }

    Byte_t     At(UInt_t i)
    {
        return fArray[i];
    }

    Byte_t    *GetArray() const
    {
        return fArray;
    }

    void Reset()
    {
        memset(fArray, 0, fN*sizeof(Byte_t));
    }

    void Reset(Byte_t v)
    {
        for (Byte_t *b=fArray; b<fArray+fN; b++)
            *b = v;
    }


    void Set(UInt_t n)
    {
        // Set size of this array to n chars.
        // A new array is created, the old contents copied to the new array,
        // then the old array is deleted.

        if (n==fN)
            return;

        Byte_t *temp = fArray;
        if (n == 0)
            fArray = 0;
        else
        {
            fArray = new Byte_t[n];
            if (n < fN)
                memcpy(fArray,temp, n*sizeof(Byte_t));
            else
            {
                memcpy(fArray,temp,fN*sizeof(Byte_t));
                memset(&fArray[fN],0,(n-fN)*sizeof(Byte_t));
            }
        }

        if (fN)
            delete [] temp;

        fN = n;
    }

    void Set(UInt_t n, Byte_t *array)
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
            fArray = new Byte_t[fN];

        memcpy(fArray,array, n*sizeof(Byte_t));
    }

    Byte_t &operator[](UInt_t i)
    {
        return fArray[i];
    }
    const Byte_t &operator[](UInt_t i) const
    {
        return fArray[i];
    }

    ClassDef(MArrayB, 1)  //Array of Byte_t
};

#endif
