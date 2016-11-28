#ifndef MARS_MPulse
#define MARS_MPulse

#include <string>

class MPulse {
public:
    float amplitude;
    float slice;
    int origin;

    MPulse();
    std::string get_json()const;
};
#endif
