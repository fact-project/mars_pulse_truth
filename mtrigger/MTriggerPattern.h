#ifndef MARS_MTriggerPattern
#define MARS_MTriggerPattern

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MTriggerPattern : public MParContainer
{
    friend class MTriggerPatternDecode;

public:
    enum Pattern_t {
        kTriggerLvl1 = BIT(0), //  1   1: Level 1 from L2 board
        kCalibration = BIT(1), //  2   2: Pulse Trigger
        kTriggerLvl2 = BIT(2), //  4   4: LUT Pseudo Size selection
        kPedestal    = BIT(3), //  8   8: Artificial pedestal event
        kPinDiode    = BIT(4), // 10  16:
        kSumTrigger  = BIT(5), // 20  32: Flag for an event taken with sum trigger
        kUndefined1  = BIT(6), // 40  64: Trigger lvl1 directly from L1 without going through L2
        kUndefined2  = BIT(7)  // 80 128: Undefined? (L3?)
    };

private:
    Byte_t fPrescaled;   // Bit Pattern as defined above
    Byte_t fUnprescaled; // Bit Pattern as defined above

public:
    MTriggerPattern(const char *name=0, const char *title=0);

    void Print(Option_t *o="") const;
    void Copy(TObject &obj) const;

    void Reset() { fPrescaled=0; fUnprescaled=0; }

    Byte_t GetPrescaled() const   { return fPrescaled; }
    Byte_t GetUnprescaled() const { return fUnprescaled; }

    ClassDef(MTriggerPattern, 1) // Container storing the decoded trigger pattern
};

#endif
