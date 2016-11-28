/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz  12/2000 <mailto:tbretz@uni-sw.gwdg.de>
!
!   Copyright: MAGIC Software Development, 2000-2001
!
!
\* ======================================================================== */

#include "MLogManip.h"

/*
// ----------------------------------------------------------------
//            Definitions of the manipulator functions
// ----------------------------------------------------------------

ostream& __omanip_debug(ostream& lout, int i)
{
    //
    // get the streambuf of the stream
    // get the pointer to the parent class by casting
    // set the output level of the logging stream
    //
    // Be careful: This manipulator can only be used in MLogging
    // streams - in other streams SetOutputLevel is a call
    // of a non existing function
    //
    // Be careful 2: The change is valid for everything which is
    // in the present buffer. This means it can also affect a
    // part of the stream which is already in the stream but not flushed
    // to the output device. A flush occures either if you tell a stream
    // to flush (flush, endl) or if an buffer overflow occures, the
    // last behaviour could be changed if someone want to have a dynamic
    // buffer.
    //
    MLog *log=(MLog*)lout.rdbuf();
    log->SetOutputLevel(i);
    return lout;
}

ostream& __omanip_device(ostream& lout, int i)
{
    //
    // get the streambuf of the stream
    // get the pointer to the parent class by casting
    // set the output device of the logging stream, more than
    // one output device can be ored together
    //
    // Be careful: This manipulator can only be used in MLogging
    // streams - in other streams SetOutputLevel is a call
    // of a non existing function
    //
    MLog *log=(MLog*)lout.rdbuf();
    log->SetOutputDevice(i);
    return lout;
}

ostream& __omanip_edev(ostream& lout, int i)
{
    //
    // get the streambuf of the stream
    // get the pointer to the parent class by casting
    // Enable an output device of the logging stream, it should
    // be possible to enable more than one output device at the
    // same time by oring them together
    //
    // Be careful: This manipulator can only be used in MLogging
    // streams - in other streams SetOutputLevel is a call
    // of a non existing function
    //
    MLog *log=(MLog*)lout.rdbuf();
    log->EnableOutputDevice((MLog::_flags)i);
    return lout;
}

ostream& __omanip_ddev(ostream& lout, int i)
{
    //
    // get the streambuf of the stream
    // get the pointer to the parent class by casting
    // Disable an output device of the logging stream, it should
    // be possible to disable more than one output device at the
    // same time by oring them together
    //
    // Be careful: This manipulator can only be used in MLogging
    // streams - in other streams SetOutputLevel is a call
    // of a non existing function
    //
    MLog *log=(MLog*)lout.rdbuf();
    log->DisableOutputDevice((MLog::_flags)i);
    return lout;
}

ostream& underline(ostream& lout)
{
    //
    // get the streambuf of the stream
    // get the pointer to the parent class by casting
    // Disable an output device of the logging stream, it should
    // be possible to disable more than one output device at the
    // same time by oring them together
    //
    // Be careful: This manipulator can only be used in MLogging
    // streams - in other streams SetOutputLevel is a call
    // of a non existing function
    //
    MLog *log=(MLog*)lout.rdbuf();
    log->Underline();
    return lout;
}
*/
