#include "MTruePhotonsPerPixelCont.h"
ClassImp(MTruePhotonsPerPixelCont);

MTruePhotonsPerPixelCont::MTruePhotonsPerPixelCont(const char *name, const char *title)
{
    cherenkov_photons_weight = new MArrayF(1440);
    cherenkov_photons_number = new MArrayF(1440);
    muon_cherenkov_photons_weight = new MArrayF(1440);
    muon_cherenkov_photons_number = new MArrayF(1440);
    noise_photons_weight = new MArrayF(1440);
    cherenkov_arrival_time_mean = new MArrayF(1440);
    cherenkov_arrival_time_variance = new MArrayF(1440);
    cherenkov_arrival_time_min = new MArrayF(1440);
    cherenkov_arrival_time_max = new MArrayF(1440);
}

MTruePhotonsPerPixelCont::~MTruePhotonsPerPixelCont(){
    delete cherenkov_photons_weight;
    delete cherenkov_photons_number;
    delete muon_cherenkov_photons_weight;
    delete muon_cherenkov_photons_number;
    delete noise_photons_weight;
    delete cherenkov_arrival_time_mean;
    delete cherenkov_arrival_time_variance;
    delete cherenkov_arrival_time_min;
    delete cherenkov_arrival_time_max;
}
