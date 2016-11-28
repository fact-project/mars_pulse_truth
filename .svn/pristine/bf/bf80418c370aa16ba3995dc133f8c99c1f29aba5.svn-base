#ifndef MARS_MArrayD
#define MARS_MArrayD

#ifndef MARS_MArray
#include "MArray.h"
#endif

#include <string.h>

class TArrayD;
class MArrayD : public MArray
{
private:
    Double_t *fArray; //[fN] Array of fN chars

public:

    MArrayD()
    {
        fN     = 0;
        fArray = NULL;
    }

    MArrayD(UInt_t n)
    {
        fN     = 0;
        fArray = NULL;
        Set(n);
    }

    MArrayD(UInt_t n, Double_t *array)
    {
        // Create TArrayC object and initialize it with values of array.
        fN     = 0;
        fArray = NULL;
        Set(n, array);
    }

    MArrayD(const MArrayD &array) : MArray()
    {
        // Copy constructor.
        fArray = NULL;
        Set(array.fN, array.fArray);
    }

    MArrayD &operator=(const MArrayD &rhs)
    {
        // TArrayC assignment operator.
        if (this != &rhs)
            Set(rhs.fN, rhs.fArray);
        return *this;
    }

    virtual ~MArrayD()
    {
        // Delete TArrayC object.
        delete [] fArray;
        fArray = NULL;
    }

    void Adopt(UInt_t n, Double_t *array)
    {
        // Adopt array arr into TArrayC, i.e. don't copy arr but use it directly
        // in TArrayC. User may not delete arr, TArrayC dtor will do it.
        if (fArray)
            delete [] fArray;

        fN     = n;
        fArray = array;
    }

    void AddAt(Double_t c, UInt_t i)
    {
        // Add char c at position i. Check for out of bounds.
        fArray[i] = c;
    }

    Double_t     At(UInt_t i) const
    {
        return fArray[i];
    }

    Double_t    *GetArray() const
    {
        return fArray;
    }

    void Reset()
    {
        memset(fArray, 0, fN*sizeof(Double_t));
    }

    void Reset(Double_t v)
    {
        for (Double_t *d=fArray; d<fArray+fN; d++)
            *d = v;
    }

    void Set(UInt_t n)
    {
        // Set size of this array to n chars.
        // A new array is created, the old contents copied to the new array,
        // then the old array is deleted.

        if (n==fN)
            return;

        Double_t *temp = fArray;
        if (n == 0)
            fArray = NULL;
        else
        {
            fArray = new Double_t[n];
            if (n < fN)
                memcpy(fArray, temp, n*sizeof(Double_t));
            else
            {
                memcpy(fArray, temp, fN*sizeof(Double_t));
                memset(&fArray[fN], 0, (n-fN)*sizeof(Double_t));
            }
        }

        if (fN)
            delete [] temp;

        fN = n;
    }

    void Set(UInt_t n, Double_t *array)
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
            fArray = new Double_t[fN];

        memcpy(fArray, array, n*sizeof(Double_t));
    }

    Double_t &operator[](UInt_t i)
    {
        return fArray[i];
    }
    const Double_t &operator[](UInt_t i) const
    {
        return fArray[i];
    }

    Double_t GetSum() const
    {
        const Double_t *end = fArray+fN;

        Double_t sum = 0;
        for (Double_t *ptr = fArray; ptr<end; ptr++)
            sum += *ptr;
        return sum;
    }

    static void  StripZeros(TArrayD &arr);
    void  StripZeros();

    ClassDef(MArrayD, 1)  //Array of Double_t
};

#endif
