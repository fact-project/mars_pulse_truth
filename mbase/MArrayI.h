#ifndef MARS_MArrayI
#define MARS_MArrayI

#ifndef MARS_MArray
#include "MArray.h"
#endif

#include <string.h>

class MArrayI : public MArray
{
private:
    Int_t *fArray; //[fN] Array of fN chars

public:

    MArrayI()
    {
        fN     = 0;
        fArray = NULL;
    }

    MArrayI(UInt_t n)
    {
        fN     = 0;
        fArray = NULL;
        Set(n);
    }

    MArrayI(UInt_t n, Int_t *array)
    {
        // Create TArrayC object and initialize it with values of array.
        fN     = 0;
        fArray = NULL;
        Set(n, array);
    }

    MArrayI(const MArrayI &array) : MArray()
    {
        // Copy constructor.
        fArray = NULL;
        Set(array.fN, array.fArray);
    }

    MArrayI &operator=(const MArrayI &rhs)
    {
        // TArrayC assignment operator.
        if (this != &rhs)
            Set(rhs.fN, rhs.fArray);
        return *this;
    }

    virtual ~MArrayI()
    {
        // Delete TArrayC object.
        delete [] fArray;
        fArray = NULL;
    }

    void Adopt(UInt_t n, Int_t *array)
    {
        // Adopt array arr into TArrayC, i.e. don't copy arr but use it directly
        // in TArrayC. User may not delete arr, TArrayC dtor will do it.
        if (fArray)
            delete [] fArray;

        fN     = n;
        fArray = array;
    }

    void AddAt(Int_t c, UInt_t i)
    {
        // Add char c at position i. Check for out of bounds.
        fArray[i] = c;
    }

    void Add(Int_t c)
    {
        Set(fN+1);
        fArray[fN-1] = c;
    }

    Int_t Find(Int_t c) const
    {
        for (Int_t *ptr=fArray; ptr<fArray+fN; ptr++)
            if (*ptr==c)
                return ptr-fArray;

        return -1;
    }

    void AddUniq(Int_t c)
    {
        if (Find(c)<0)
            Add(c);
    }

    Int_t     At(UInt_t i) const
    {
        return fArray[i];
    }

    Int_t    *GetArray() const
    {
        return fArray;
    }

    void Reset()
    {
        memset(fArray, 0, fN*sizeof(Int_t));
    }

    void Reset(Int_t v)
    {
        for (Int_t *i=fArray; i<fArray+fN; i++)
            *i = v;
    }

    void Set(UInt_t n)
    {
        // Set size of this array to n chars.
        // A new array is created, the old contents copied to the new array,
        // then the old array is deleted.

        if (n==fN)
            return;

        Int_t *temp = fArray;
        if (n == 0)
            fArray = NULL;
        else
        {
            fArray = new Int_t[n];
            if (n < fN)
                memcpy(fArray, temp, n*sizeof(Int_t));
            else
            {
                memcpy(fArray, temp, fN*sizeof(Int_t));
                memset(&fArray[fN], 0, (n-fN)*sizeof(Int_t));
            }
        }

        if (fN)
            delete [] temp;

        fN = n;
    }

    void Set(UInt_t n, Int_t *array)
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
            fArray = new Int_t[fN];

        memcpy(fArray, array, n*sizeof(Int_t));
    }

    Int_t &operator[](UInt_t i)
    {
        return fArray[i];
    }
    const Int_t &operator[](UInt_t i) const
    {
        return fArray[i];
    }

    void ReSort(Bool_t down=kFALSE);

    ClassDef(MArrayI, 1)  //Array of Int_t
};

#endif
