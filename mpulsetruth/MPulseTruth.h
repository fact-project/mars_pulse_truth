#ifndef MARS_SimulationTruth
#define MARS_SimulationTruth

#include <vector>
#include <string>

namespace SimulationTruth {

    struct Pulse {
        float amplitude;
        float slice;
        int origin; 

        Pulse();
        std::string get_json()const;

        // CHERENKOV_PHOTON >= 0 
        static const int MARS_DEFAULT;
        static const int NIGHT_SKY;
        static const int ELECTRONICS_ARTIFICIAL;
    };

    struct Event {
        unsigned int event_number;
        std::vector<std::vector<Pulse>> pulses_in_pixels;
        std::string get_json()const;
    };
}// SimulationTruth
#endif