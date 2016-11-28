#ifndef MARS_MCalibrationPatternDecode
#define MARS_MCalibrationPatternDecode

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MRawEvtHeader;
class MRawRunHeader;
class MCalibrationPattern;

class MCalibrationPatternDecode : public MTask
{
private:

    enum PulserColorCode_t 
      {
        kSlot1Green     = BIT(0),
        kSlot2Green     = BIT(1),
        kSlot3Blue      = BIT(2),
        kSlot4UV        = BIT(3),
        kSlot5UV        = BIT(4),
        kSlot6Blue      = BIT(5),
        kSlot7Blue      = BIT(6),
        kSlot8Blue      = BIT(7),
        kSlot9AttBlue   = BIT(8),
        kSlot10Blue     = BIT(9),
        kSlot11Blue     = BIT(10),
        kSlot12UV       = BIT(11),
        kSlot13UV       = BIT(12),
        kSlot14Blue     = BIT(13),
        kSlot15Green    = BIT(14),
        kSlot16AttGreen = BIT(15),
//        kCT1Pulser      = BIT(16),
        kAnyGreen       = kSlot1Green  | kSlot2Green | kSlot15Green | kSlot16AttGreen,
        kAnyUV          = kSlot4UV     | kSlot5UV    | kSlot12UV    | kSlot13UV,
        kAnyBlue        = kSlot3Blue   | kSlot6Blue  | kSlot7Blue   | kSlot8Blue 
                        | kSlot9AttBlue| kSlot10Blue | kSlot11Blue  | kSlot14Blue, 
        kGreenAndBlue   = kAnyGreen & kAnyBlue,
        kBlueAndUV      = kAnyBlue  & kAnyUV,
        kGreenAndUV     = kAnyGreen & kAnyUV,
        kIFAEPulser     = kAnyGreen | kAnyBlue | kAnyUV/*,
        kAny            = kAnyGreen | kAnyBlue | kAnyUV | kCT1Pulser*/
      };

    MRawRunHeader   *fRunHeader;
    MRawEvtHeader   *fEvtHeader;
    MCalibrationPattern *fPattern;

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:

    MCalibrationPatternDecode(const char *name=0, const char *title=0);

    ClassDef(MCalibrationPatternDecode, 1) // Task to decode the Trigger Pattern
};

#endif
