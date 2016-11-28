// DCCurrentEvent.cpp: Implementiberung der Klasse DCCurrentEvent.
//
//////////////////////////////////////////////////////////////////////

#ifndef DCCURRENTEVENT_INCLUDED
#include "DCCurrentEvent.h"
#endif
#ifndef GLOBALS_INCLUDED
#include "Globals.h"
#endif

#include <sstream>
#include <istream>
#include <ostream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <vector>

extern int gYear, gMonth, gDay;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

DCCurrentEvent::DCCurrentEvent()
{
    m_dcValues.reserve( Globals::CAMERA_PIXELS );
    
}

DCCurrentEvent::~DCCurrentEvent()
{
}

std::istream& operator >> ( std::istream& in, DCCurrentEvent& event )
{
    using std::string;
    using std::getline;

    // read the starting tag
    string tag;
    in >> tag;
    if ( tag != "DC")
    {
        in.clear( std::ios_base::badbit);
        return in;
    }

    // read error codes
    in >> event.m_dcError1 >> event.m_dcError2;

    // read in the timestamp
    in >> event.m_dcHour >> event.m_dcMin >> event.m_dcSec >> event.m_dcMSec;

    getline( in, event.m_dcCurr );

    return in;
}

std::ostream& operator << ( std::ostream& out, const DCCurrentEvent& event )
{
    using namespace std;

    out << "DC-REPORT ";
    out << setfill('0') << right << resetiosflags(ios_base::showpos);
    out << setw(2) << 0 << ' ';
    out << setw(4) << gYear << ' ';
    out << setw(2) << gMonth << ' ';
    out << setw(2) << gDay << ' ';
    out << setw(2) << event.m_dcHour << ' ';
    out << setw(2) << event.m_dcMin  << ' ';
    out << setw(2) << event.m_dcSec  << ' ';
    out << setw(3) << event.m_dcMSec << ' ';

    out << setfill('0') << internal << setiosflags(ios_base::showpos);
    out << setw(3) << event.m_dcError1 << " ";
    out << setw(3) << event.m_dcError2;
    out << setfill('0') << right << resetiosflags(ios_base::showpos);

    out << event.m_dcCurr;
    return out;
}
