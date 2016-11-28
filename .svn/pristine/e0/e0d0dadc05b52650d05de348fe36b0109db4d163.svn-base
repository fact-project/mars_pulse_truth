#ifndef MARS_MArrayF
#define MARS_MArrayF

#ifndef MARS_MArray
#include "MArray.h"
#endif

#include <string.h>

class TArrayF;
class MArrayF : public MArray
{
private:
    Float_t *fArray; //[fN] Array of fN chars

public:

    MArrayF()
    {
        fN     = 0;
        fArray = NULL;
    }

    MArrayF(UInt_t n)
    {
        fN     = 0;
        fArray = NULL;
        Set(n);
    }

    MArrayF(UInt_t n, Float_t *array)
    {
        // Create TArrayC object and initialize it with values of array.
        fN     = 0;
        fArray = NULL;
        Set(n, array);
    }

    MArrayF(const MArrayF &array) : MArray()
    {
        // Copy constructor.
        fArray = NULL;
        Set(array.fN, array.fArray);
    }

    MArrayF &operator=(const MArrayF &rhs)
    {
        // TArrayC assignment operator.
        if (this != &rhs)
            Set(rhs.fN, rhs.fArray);
        return *this;
    }

    virtual ~MArrayF()
    {
        // Delete TArrayC object.
        delete [] fArray;
        fArray = NULL;
    }

    void Adopt(UInt_t n, Float_t *array)
    {
        // Adopt array arr into TArrayC, i.e. don't copy arr but use it directly
        // in TArrayC. User may not delete arr, TArrayC dtor will do it.
        if (fArray)
            delete [] fArray;

        fN     = n;
        fArray = array;
    }

    void AddAt(Float_t c, UInt_t i)
    {
        // Add char c at position i. Check for out of bounds.
        fArray[i] = c;
    }

    Float_t     At(UInt_t i) const 
    {
        return fArray[i];
    }

    Float_t    *GetArray() const
    {
        return fArray;
    }

    void Reset()
    {
        memset(fArray, 0, fN*sizeof(Float_t));
    }

    void Reset(Float_t v)
    {
        for (Float_t *f=fArray; f<fArray+fN; f++)
            *f = v;
    }

    void Set(UInt_t n)
    {
        // Set size of this array to n chars.
        // A new array is created, the old contents copied to the new array,
        // then the old array is deleted.

        if (n==fN)
            return;

        Float_t *temp = fArray;
        if (n == 0)
            fArray = NULL;
        else
        {
            fArray = new Float_t[n];
            if (n < fN)
                memcpy(fArray, temp, n*sizeof(Float_t));
            else
            {
                memcpy(fArray, temp, fN*sizeof(Float_t));
                memset(&fArray[fN], 0, (n-fN)*sizeof(Float_t));
            }
        }

        if (fN)
            delete [] temp;

        fN = n;
    }

    void Set(UInt_t n, Float_t *array)
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
            fArray = new Float_t[fN];

        memcpy(fArray, array, n*sizeof(Float_t));
    }

    Float_t &operator[](UInt_t i)
    {
        return fArray[i];
    }
    const Float_t &operator[](UInt_t i) const
    {
        return fArray[i];
    }

    Double_t GetSum() const
    {
        const Float_t *end = fArray+fN;

        Double_t sum = 0;
        for (Float_t *ptr=fArray; ptr<end; ptr++)
            sum += *ptr;

        return sum;
    }

    template<class T>
        void Add(const T *src, UInt_t n, UInt_t beg=0, Float_t scale=1)
    {
        if (!src || beg >= fN)
            return;

        Float_t *ptr = fArray+beg;
        const Float_t *end  = beg+n>fN ? fArray+fN : ptr+n;

        // we treat the case m==1. special, in order to speed up the code a bit
        // since when m==1. the multiplication can be omitted and this should be a bit faster then.
        if (scale==1)
        {
            while (ptr<end)
                *ptr++ += *src++;
        }
        else
        {
            while (ptr<end)
                *ptr++ += (*src++) * scale;
        }
    }

    template<class T>
        void AddClipped(Double_t th, const T *src, UInt_t n, UInt_t beg=0)
    {
        if (!src || beg>=fN)
            return;

        Float_t *ptr = fArray + beg;
        const Float_t *end  = beg+n>fN ? fArray+fN : ptr+n;

        while (ptr<end)
            *ptr++ += TMath::Min(*src++, th);
    }

    static void  StripZeros(TArrayF &arr);
    void  StripZeros();

    ClassDef(MArrayF, 1)  //Array of Float_t
};

#endif
