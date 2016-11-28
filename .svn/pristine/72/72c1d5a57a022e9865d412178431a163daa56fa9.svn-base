#ifndef MARS_MDrsCalibrationTime
#define MARS_MDrsCalibrationTime

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef MARS_DrsCalib
#include "DrsCalib.h"
#endif

class TH1;
class TGraph;

class MDrsCalibrationTime : public MParContainer//, public DrsCalibrateTime
{
    int64_t fNumEntries;

    size_t fNumSamples;
    size_t fNumChannels;

    std::vector<double> fOffsets;
    std::vector<double> fDelays;

public:
    MDrsCalibrationTime(const char *name=0, const char *title=0)
    {
        fName  = name  ? name  : "MDrsCalibrationTime";
        fTitle = title ? title : "";
    }

    void InitSize(uint16_t channels, uint16_t samples)
    {
        fNumSamples  = samples;
        fNumChannels = channels;
    }

    void SetCalibration(const DrsCalibrateTime &cal)
    {
        fNumEntries  = cal.fNumEntries,
        fNumSamples  = cal.fNumSamples,
        fNumChannels = cal.fNumChannels,

        fOffsets.resize(fNumSamples*fNumChannels);

        for (size_t c=0; c<fNumChannels; c++)
            for (size_t s=0; s<fNumSamples; s++)
                fOffsets[c*fNumSamples+s] = cal.Offset(c, s);
    }

    bool SetDelays(const TH1 &cam);
    void SetDelays(const TGraph &g);

    double GetOffset(uint32_t hw, uint32_t spos, float tm) const
    {
        const uint32_t ch = (hw/9)*fNumSamples;
        return fOffsets[ch + fmod(tm+spos, fNumSamples)] - fOffsets[ch + spos];
    }

    double GetDelay(int hw) const
    {
        return fDelays.size()==0 ? 0 : fDelays[hw];
    }

    bool ReadFits(TString fname);
    bool WriteFits(const std::string &fname) const;

    ClassDef(MDrsCalibrationTime, 3) // A list of histograms storing the Fadc spektrum of one pixel
};

#endif

