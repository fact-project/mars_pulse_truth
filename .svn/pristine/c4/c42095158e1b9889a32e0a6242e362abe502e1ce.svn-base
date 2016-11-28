#ifndef MARS_MArrivalTimePix
#define MARS_MArrivalTimePix

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MArrivalTimePix : public MParContainer
{
private:
  Float_t fArrivalTimeHiGain;      // mean value of the ArrivalTime
  Float_t fArrivalTimeHiGainError; // error of the mean value of the ArrivalTime
  Float_t fArrivalTimeLoGain;      // mean value of the ArrivalTime
  Float_t fArrivalTimeLoGainError; // error of the mean value of the ArrivalTime

  Byte_t fNumHiGainSaturated;      // Number of first hi-gain slice which has saturated (could be negative if already the first slice saturates)
  Byte_t fNumLoGainSaturated;      // Number of first lo-gain slices which have saturated

public:
    MArrivalTimePix(const char* name=NULL, const char* title=NULL);

    void Clear(Option_t *o="");
    void Print(Option_t *o="") const;
    void Copy(TObject &obj) const
    {
        MArrivalTimePix &pix = static_cast<MArrivalTimePix&>(obj);

        pix.fArrivalTimeHiGain      = fArrivalTimeHiGain;
        pix.fArrivalTimeHiGainError = fArrivalTimeHiGainError;
        pix.fArrivalTimeLoGain      = fArrivalTimeLoGain;
        pix.fArrivalTimeLoGainError = fArrivalTimeLoGainError;

        pix.fNumHiGainSaturated     = fNumHiGainSaturated;
        pix.fNumLoGainSaturated     = fNumLoGainSaturated;
    }

    // Setter
    void SetArrivalTime(Float_t sig, Float_t sigerr=0);
    void SetArrivalTime(Float_t sighi, Float_t sighierr,Float_t siglo, Float_t sigloerr);
    void SetGainSaturation(Byte_t higain, Byte_t logain=0);

    // Getter
    Float_t GetArrivalTimeHiGain()      const { return fArrivalTimeHiGain; }
    Float_t GetArrivalTimeHiGainError() const { return fArrivalTimeHiGainError; }

    Float_t GetArrivalTimeLoGain()      const { return fArrivalTimeLoGain; }
    Float_t GetArrivalTimeLoGainError() const { return fArrivalTimeLoGainError; }

    Float_t GetArrivalTime()            const;
    Bool_t  IsArrivalTimeValid()        const;

    Byte_t GetNumHiGainSaturated()      const { return fNumHiGainSaturated; }
    Byte_t GetNumLoGainSaturated()      const { return fNumLoGainSaturated; }

    Bool_t IsHiGainSaturated()          const { return fNumHiGainSaturated>0; }
    Bool_t IsLoGainSaturated()          const { return fNumLoGainSaturated>0; }

    Bool_t IsLoGainValid()              const { return fArrivalTimeLoGainError>=0; }
    Bool_t IsHiGainValid()              const { return fArrivalTimeHiGainError>=0; }

    ClassDef(MArrivalTimePix, 1)	// Storage Container for ArrivalTime information of one pixel
};

#endif
