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
!   Author(s): Thomas Bretz, 12/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportCC
//
// This is the class interpreting and storing the CC-REPORT information.
//
// From here maily weather station data is decoded such as
// temperature, humidity, wind-speed and solar radiation
//
// Class Version 2:
// ----------------
//  +  Float_t fUPSStatus; // arbitrary units (still not properly defined)
//  +  Float_t fDifRubGPS; // [us] Difference between the Rubidium clock time and the time provided by the GPS receiver
//
// Class Version 3:
// ----------------
//  * Moved weather data to new base class
//
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportCC.h"

#include "fits.h"

#include "MLogManip.h"

#include "MParList.h"

#include "MReportRec.h"

#include "MCameraTH.h"
#include "MCameraTD.h"
#include "MCameraRecTemp.h"

ClassImp(MReportWeather);
ClassImp(MReportCC);

using namespace std;

// --------------------------------------------------------------------------
//
// Default construtor. Initialize identifier to "CC-REPORT" Report
// is expected to have no 'subsystem' time.
//
MReportCC::MReportCC() : MReportWeather("CC-REPORT")
{
    fName  = "MReportCC";
    fTitle = "Class for CC-REPORT information";
}

// --------------------------------------------------------------------------
//
// FindCreate the following objects:
//  - MCameraTH
//
Bool_t MReportCC::SetupReading(MParList &plist)
{
    fRecRep = (MReportRec*)plist.FindCreateObj("MReportRec");
    if (!fRecRep)
        return kFALSE;


    fTH = (MCameraTH*)plist.FindCreateObj("MCameraTH");
    if (!fTH)
        return kFALSE;

    fTD = (MCameraTD*)plist.FindCreateObj("MCameraTD");
    if (!fTD)
        return kFALSE;

    fRecTemp = (MCameraRecTemp*)plist.FindCreateObj("MCameraRecTemp");
    if (!fRecTemp)
        return kFALSE;


    return MReport::SetupReading(plist);
}

// --------------------------------------------------------------------------
//
// Interprete the body of the CC-REPORT string
//
Bool_t MReportCC::InterpreteCC(TString &str, Int_t ver)
{
    const Int_t skip = ver<200407270 ? 30 : 31;

    // Remove the 30/31 tokens of the subsystem status
    //  table 12.1 p59
    for (int i=0; i<skip; i++)
        str.Remove(0, str.First(' ')+1);

    Int_t len;
    const Int_t n=sscanf(str.Data(),
                         "%*f %*f %*f %*f %f %f %f %f %f %f %n",
                         &fTemperature, &fSolarRadiation, &fWindSpeed,
                         &fHumidity, &fUPSStatus, &fDifRubGPS, &len);
    if (n!=6)
    {
        *fLog << warn << "WARNING - Wrong number of arguments (should be 6)." << endl;
        return kFALSE;
    }

    str.Remove(0, len);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete SCHEDULE section of the CC-REPORT string
//
Bool_t MReportCC::InterpreteSchedule(TString &str)
{
    if (!CheckTag(str, "SCHEDULE "))
        return kFALSE;

    str = str.Strip(TString::kBoth);

    // [Sourcename] sourcecategory
    const Ssiz_t pos1 = str.First(' ');
    if (pos1<0)
    {
        *fLog << warn << "WARNING - Wrong number of arguments (should be 1 or 2)." << endl;
        return kFALSE;
    }

    const TString str1 = str(0, pos1);

    str.Remove(0, pos1);
    str = str.Strip(TString::kBoth);

    if (!str1.IsDigit())
    {
        const Ssiz_t pos2 = str.First(' ');
        if (pos2<0)
        {
            *fLog << warn << "WARNING - Wrong number of arguments (should be 1 or 2)." << endl;
            return kFALSE;
        }

        TString str2 = str(0, pos2);

        str.Remove(0, pos2);

        // Remove a leading minus. The negative numbers are error codes introduced
        // by Arehucas. It's only meant for the GRB monitor to change the priority
        // of its alerts.
        if (str2[0]=='-')
            str2.Remove(0, 1);

        if (!str2.IsDigit())
        {
            *fLog << warn << "WARNING - Wrong type of second argument (obs. category): " << str2 << endl;
            return kFALSE;
        }
    }

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Interprete the part of the CC-Report with the MII Subsystem status'
//
Bool_t MReportCC::InterpreteStatusM2(TString &str)
{
    // Status of: DAQ, DominoCalibration, Drive, Starguider, CaCo,
    //            CaCo2 LID, CaCo sentinel, CaCo LV, CaCo2 HV, AMC, L2T,
    //            Domino, Readout, REC, DT, Readout cooling, calib
    //            %05.2f %05.2f Zd [deg], Az [deg]

    // Remove the 18 tokens of the MAGIC II subsystem status
    for (int i=0; i<19; i++)
        str.Remove(0, str.First(' ')+1);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the body of the CC-REPORT string
//
Int_t MReportCC::InterpreteBody(TString &str, Int_t ver)
{
    if (ver<200404070)
    {
        *fLog << err << "ERROR - MReportCC::InterpreteBody not prepared for ";
        *fLog << " report-files with version<200404070" << endl;
        return kFALSE;
    }

    if (!InterpreteCC(str, ver))
        return kCONTINUE;

    if (ver>=200809030)
        if (!InterpreteSchedule(str))
            return kCONTINUE;

    if (ver>=200902030)
        if (!InterpreteStatusM2(str))
            return kCONTINUE;

    if (ver<200805190)
    {
        fRecRep->InterpreteRec(str, ver, *fTH, *fTD, *fRecTemp);
        Copy(*fRecRep);
        fRecRep->SetReadyToSave();
    }

    if (str.Strip(TString::kBoth)!=(TString)"OVER")
    {
        *fLog << warn << "WARNING - 'OVER' tag not found." << endl;
        return kCONTINUE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print contents of report
//
void MReportWeather::Print(Option_t *opt) const
{
    *fLog << all << GetDescriptor() << ":  Status=" << (int)GetState();
    *fLog << "   Hum=" << Form("%3.0f", fHumidity);
    *fLog << "%  Temp=" << Form("%+3.0f", fTemperature);
    *fLog << "°C  Wind=" << Form("%3.0f", fWindSpeed);
    *fLog << "km/h  SolarRad=" << Form("%4.0f", fSolarRadiation) << "W/m^2" << endl;
}

Bool_t MReportWeather::SetupReadingFits(fits &file)
{
    return
        file.SetRefAddress("T", fTemperature) &&
        file.SetRefAddress("H", fHumidity)    &&
        file.SetRefAddress("v", fWindSpeed);
}
