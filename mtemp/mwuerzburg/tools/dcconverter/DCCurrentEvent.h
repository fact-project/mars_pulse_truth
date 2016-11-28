// DCCurrentEvent.h: Schnittstelle für die Klasse DCCurrentEvent.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCCURRENTEVENT_INCLUDED)
#define DCCURRENTEVENT_INCLUDED

#include <ctime>
#include <vector>
#include <string>
#include <istream>
#include <ostream>

class DCCurrentEvent
{
    friend std::istream& operator >> ( std::istream& in, DCCurrentEvent& event );
    friend std::ostream& operator << ( std::ostream& out, const DCCurrentEvent& event );

private:
    int	       		m_dcError1;
    int	       		m_dcError2;
    int	       		m_dcHour;
    int	       		m_dcMin;
    int	       		m_dcSec;
    int	       		m_dcMSec;
    time_t     		m_dcTime;
    std::string       m_dcCurr;
    std::vector<int>	m_dcValues;

public:
	DCCurrentEvent();
	virtual ~DCCurrentEvent();
};

std::istream& operator >> ( std::istream& in, DCCurrentEvent& event );
std::ostream& operator << ( std::ostream& out, const DCCurrentEvent& event );

#endif // !defined(DCCURRENTEVENT_INCLUDED)
