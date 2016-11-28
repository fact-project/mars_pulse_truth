#include "MPulse.h"
#include <sstream>

MPulse::MPulse():
    amplitude(0.0), 
    slice(0.0), 
    origin(-1) 
{}

std::string MPulse::get_json()const {
    std::stringstream out;
    out << "{amplitude:" << amplitude << ",";
    out << "slice:" << slice << ",";
    out << "origin:" << origin << "}";
    return out.str();
}
