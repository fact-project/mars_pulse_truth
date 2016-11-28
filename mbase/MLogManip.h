#ifndef MARS_MLogManip
#define MARS_MLogManip

#include <iomanip>

#ifndef MARS_MLog
#include "MLog.h"
#endif

// --------------------- simple manipulators -----------------------

enum MLogManip {
    underline
};

inline std::ostream &operator<<(std::ostream &lout, MLogManip)
{
    MLog *log=dynamic_cast<MLog*>(lout.rdbuf());
    if (log)
        log->Underline();
    return lout;
}

// ----------------------------- debug -----------------------------

struct _Debug { int level; };

const _Debug all  = { 0 }; // use this for output in any case
const _Debug err  = { 1 }; // use this for fatal errors (red)
const _Debug warn = { 2 }; // use this for wrnings (yellow)
const _Debug inf  = { 3 }; // use this for informations (green)
const _Debug inf2 = { 4 }; // use this for informations (green)
const _Debug inf3 = { 5 }; // use this for informations (green)
const _Debug dbg  = { 6 }; // use this for debug messages (blue)

inline _Debug debug(int level)
{
    _Debug d;
    d.level = level;
    return d;
}

inline std::ostream &operator<<(std::ostream &lout, _Debug d)
{
    MLog *log=dynamic_cast<MLog*>(lout.rdbuf());
    if (log)
        log->SetOutputLevel(d.level);
    return lout;
}

// ------------------------------- edev ----------------------------

struct _EnableDev { MLog::_flags dev; };

inline _EnableDev edev(MLog::_flags i)
{
    _EnableDev e;
    e.dev = i;
    return e;
}

inline std::ostream &operator<<(std::ostream &lout, _EnableDev e)
{
    MLog *log=dynamic_cast<MLog*>(lout.rdbuf());
    if (log)
        log->EnableOutputDevice(e.dev);
    return lout;
}

// ------------------------------- sdev ----------------------------

struct _DisableDev { MLog::_flags dev; };

inline _DisableDev ddev(MLog::_flags i)
{
    _DisableDev d;
    d.dev = i;
    return d;
}

inline std::ostream &operator<<(std::ostream &lout, _DisableDev d)
{
    MLog *log=dynamic_cast<MLog*>(lout.rdbuf());
    if (log)
        log->EnableOutputDevice(d.dev);
    return lout;
}

// ------------------------------ Macros ---------------------------

#ifndef __CINT__
#define dbginf __FILE__ << " l." << dec << __LINE__ << ": "
#endif

#endif
