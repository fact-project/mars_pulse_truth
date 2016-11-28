#include "SimulationTruth.h"
#include <sstream>

namespace SimulationTruth {

    Pulse::Pulse():
        amplitude(0.0), 
        slice(0.0), 
        origin(-1) 
    {}

    string Pulse::get_json()const {
        std::stringstream out;
        out << "{amplitude:" << amplitude << ",";
        out << "slice:" << slice << ",";
        out << "origin:" << origin << "}";
        return out.str();
    }

    // CHERENKOV_PHOTON >= 0 
    const int Pulse::MARS_DEFAULT = -1;
    const int Pulse::NIGHT_SKY = -100;
    const int Pulse::ELECTRONICS_ARTIFICIAL = -200;

    string Event::get_json()const {
        std::stringstream out;
        out << "{";
        out << "event_number:" << event_number << ",";
        out << "pixels:[";
        for(int pixel=0; pixel<pulses_in_pixels.size(); pixel++) {
            out << "[";
            for(int pulse=0; pixel<pulses_in_pixels.at(pixel).size(); pulse++) {
                out << pulses_in_pixels.at(pixel).get_json();
                if(pulse+1 != pixel<pulses_in_pixels.at(pixel).size())
                    out << ",";
            }
            if(pixel+1 != pixel<pulses_in_pixels.size())
                out << ",";
            out << "]";
        }
        out << "]";
        out << "}";
        return out.str();
    }
}// SimulationTruth