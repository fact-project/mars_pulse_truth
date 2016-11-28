#ifndef STUFF_H
#define STUFF_H

#include "MParContainer.h"
#include "MArrayF.h"

/*
 * 
- sum of heights of all cherenkov photons and their crosstalk pulses
- number of all cherenkov photons and their crosstalk pulses
- sum of heights of all cherenkov photons and their crosstalk pulses caused by muons
- number of all cherenkov photons and their crosstalk pulses caused by muons
- sum of heights of all other pulses (NSB, DC,Afterpulses) and their crosstalk pulses
- Mean of the arrivaltimes of the cherenkov photons
- Rms of the arrivaltimes of the cherenkov photons
- Median of the arrivaltimes of the cherenkov photons
- Min of the arrivaltimes of the cherenkov photons
- Max of the arrivaltimes of the cherenkov photons 
* */

class MTruePhotonsPerPixelCont : public MParContainer
{
public:
    MArrayF *cherenkov_photons_weight;          // {fits: name=McCherPhotWeight}
    MArrayF *cherenkov_photons_number;          // {fits: name=McCherPhotNumber}
    MArrayF *muon_cherenkov_photons_weight;     // {fits: name=McMuonCherPhotWeight}
    MArrayF *muon_cherenkov_photons_number;     // {fits: name=McMuonCherPhotNumber}
    MArrayF *noise_photons_weight;              // {fits: name=McNoisePhotWeight}
    MArrayF *cherenkov_arrival_time_mean;       // {fits: name=McCherArrTimeMean}
    MArrayF *cherenkov_arrival_time_variance;   // {fits: name=McCherArrTimeVar}
    MArrayF *cherenkov_arrival_time_min;        // {fits: name=McCherArrTimeMin}
    MArrayF *cherenkov_arrival_time_max;        // {fits: name=McCherArrTimeMax}

    
    MTruePhotonsPerPixelCont(const char *name="MTruePhotonsPerPixelCont", const char *title="MTruePhotonsPerPixelCont");
    ~MTruePhotonsPerPixelCont();
    
    ClassDef(MTruePhotonsPerPixelCont, 1) // MTruePhotonsPerPixelCont
};

#endif
