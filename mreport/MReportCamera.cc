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
!   Author(s): Thomas Bretz, 11/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniel Mazin, 04/2005 <mailto:mazin@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MReportCamera
//
// This is the class interpreting and storing the CAMERA-REPORT information.
//
// Most of the information is redirected to the classes MCamera* and stored
// there.
//
// Version 2:
// ----------
//   - fStstusDC
//
//////////////////////////////////////////////////////////////////////////////
#include "MReportCamera.h"

#include "MLogManip.h"

#include "MAstro.h"
#include "MParList.h"

#include "MCameraCalibration.h"
#include "MCameraCooling.h"
#include "MCameraDC.h"
#include "MCameraHV.h"
#include "MCameraLV.h"
#include "MCameraAUX.h"
#include "MCameraActiveLoad.h"
#include "MCameraCentralPix.h"
#include "MCameraLids.h"

ClassImp(MReportCamera);

using namespace std;

// --------------------------------------------------------------------------
//
// Default construtor. Initialize identifier to "CAMERA-REPORT"
//
MReportCamera::MReportCamera() : MReport("CAMERA-REPORT")
{
    fName = "MReportCamera";
    fTitle = "Class for CAMERA-REPORT information";
}

// --------------------------------------------------------------------------
//
// FindCreate the following objects:
//  - MCameraCooling
//  - MCameraLids
//  - MCameraAUX
//  - MCameraHV
//  - MCameraLV
//  - MCameraCalibration
//
Bool_t MReportCamera::SetupReading(MParList &plist)
{
    fCooling = (MCameraCooling*)plist.FindCreateObj("MCameraCooling");
    if (!fCooling)
        return kFALSE;

    fLids = (MCameraLids*)plist.FindCreateObj("MCameraLids");
    if (!fLids)
        return kFALSE;

    fAUX = (MCameraAUX*)plist.FindCreateObj("MCameraAUX");
    if (!fAUX)
        return kFALSE;

    fHV = (MCameraHV*)plist.FindCreateObj("MCameraHV");
    if (!fHV)
        return kFALSE;

    fDC = (MCameraDC*)plist.FindCreateObj("MCameraDC");
    if (!fDC)
        return kFALSE;

    fLV = (MCameraLV*)plist.FindCreateObj("MCameraLV");
    if (!fLV)
        return kFALSE;

    fCalibration = (MCameraCalibration*)plist.FindCreateObj("MCameraCalibration");
    if (!fCalibration)
        return kFALSE;

    fActiveLoad = (MCameraActiveLoad*)plist.FindCreateObj("MCameraActiveLoad");
    if (!fActiveLoad)
        return kFALSE;

    fCentralPix = (MCameraCentralPix*)plist.FindCreateObj("MCameraCentralPix");
    if (!fCentralPix)
        return kFALSE;

    return MReport::SetupReading(plist);
}

// --------------------------------------------------------------------------
//
// Interprete the DC* part of the report
//
Bool_t MReportCamera::InterpreteDC(TString &str)
{
    if (!CheckTag(str, "DC "))
        return kFALSE;

    return fDC->Interprete(str);
}

// --------------------------------------------------------------------------
//
// Interprete the HV* part of the report
//
Bool_t MReportCamera::InterpreteHV(TString &str)
{
    if (!CheckTag(str, "HV "))
        return kFALSE;

    const char *pos = str.Data();
    const char *end = str.Data()+577*3;

    Int_t i=0;
    while (pos<end)
    {
        const Char_t hex[4] = { pos[0], pos[1], pos[2], 0 };
        pos += 3;

        const Int_t n=sscanf(hex, "%3hx", &fHV->fHV[i++]);
        if (n==1)
            continue;

        *fLog << warn << "WARNING - Reading hexadecimal HV information." << endl;
        return kFALSE;
    }

    str.Remove(0, end-str.Data()); // Remove DC currents
    str=str.Strip(TString::kLeading);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the COOL* part of the report
//
Bool_t MReportCamera::InterpreteCOOL(TString &str)
{
    if (!CheckTag(str, "COOL "))
        return kFALSE;

    Int_t len;

    Int_t wall, opt, center, water;
    Short_t hwall, hcenter, hip, lop, pump, ref, valv, res, fans;
    const Int_t n=sscanf(str.Data(), "%d %d %d %d %hu %hu %hu %hu %hu %hu %hu %hu %hu %n",
                         &wall, &opt, &center, &water, &hwall, &hcenter,
                         &hip,  &lop, &pump, &ref, &valv, &res, &fans, &len);
    if (n!=13)
    {
        *fLog << warn << "WARNING - Reading information of 'COOL' section." << endl;
        return kFALSE;
    }

    fCooling->fTempWall            = 0.1*wall;
    fCooling->fTempOptLink         = 0.1*opt;
    fCooling->fTempCenter          = 0.1*center;
    fCooling->fTempWater           = 0.1*water;
    fCooling->fHumWall             = (Byte_t)hwall;
    fCooling->fHumCenter           = (Byte_t)hcenter;
    fCooling->fStatusPressureHi    = (Bool_t)hip;
    fCooling->fStatusPressureLo    = (Bool_t)lop;
    fCooling->fStatusPump          = (Bool_t)pump;
    fCooling->fStatusRefrigrerator = (Bool_t)ref;
    fCooling->fStatusValve         = (Bool_t)valv;
    fCooling->fStatusResistor      = (Bool_t)res;
    fCooling->fStatusFans          = (Bool_t)fans;

    str.Remove(0, len);
    str=str.Strip(TString::kLeading);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the LID* part of the report
//
Bool_t MReportCamera::InterpreteLID(TString &str)
{
    if (!CheckTag(str, "LID "))
        return kFALSE;

    Int_t len;
    Short_t limao, limac, limbo, limbc;
    Short_t slimao, slimac, slimbo, slimbc;
    Short_t slida, slidb, mlida, mlidb;
    const Int_t n=sscanf(str.Data(), "%hu %hu %hu %hu %hu %hu %hu %hu %hu %hu %hu %hu %n",
                         &limao, &limac, &limbo, &limbc,
                         &slimao, &slimac, &slimbo, &slimbc,
                         &slida, &slidb, &mlida, &mlidb,
                         &len);
    if (n!=12)
    {
        *fLog << warn << "WARNING - Reading information of 'LID' section." << endl;
        return kFALSE;
    }

    fLids->fLidA.fLimitOpen       = (Bool_t)limao;
    fLids->fLidA.fLimitClose      = (Bool_t)limac;
    fLids->fLidA.fSafetyLimitOpen = (Bool_t)slimao;
    fLids->fLidA.fSafetyLimitClose= (Bool_t)slimac;
    fLids->fLidA.fStatusLid       = (Byte_t)slida;
    fLids->fLidA.fStatusMotor     = (Byte_t)mlida;

    fLids->fLidB.fLimitOpen       = (Bool_t)limbo;
    fLids->fLidB.fLimitClose      = (Bool_t)limbc;
    fLids->fLidB.fSafetyLimitOpen = (Bool_t)slimbo;
    fLids->fLidB.fSafetyLimitClose= (Bool_t)slimbc;
    fLids->fLidB.fStatusLid       = (Byte_t)slidb;
    fLids->fLidB.fStatusMotor     = (Byte_t)mlidb;

    str.Remove(0, len);
    str=str.Strip(TString::kLeading);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the HVPS* part of the report
//
Bool_t MReportCamera::InterpreteHVPS(TString &str)
{
    if (!CheckTag(str, "HVPS "))
        return kFALSE;

    Int_t len;
    Short_t c1, c2;
    const Int_t n=sscanf(str.Data(), "%hd %hd %hd %hd %n",
                         &fHV->fVoltageA, &fHV->fVoltageB, &c1, &c2, &len);
    if (n!=4)
    {
        *fLog << warn << "WARNING - Reading information of 'HVPS' section." << endl;
        return kFALSE;
    }

    fHV->fCurrentA = (Byte_t)c1;
    fHV->fCurrentB = (Byte_t)c2;

    str.Remove(0, len);
    str=str.Strip(TString::kLeading);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the LV* part of the report
//
Bool_t MReportCamera::InterpreteLV(TString &str)
{
    if (!CheckTag(str, "LV "))
        return kFALSE;

    Int_t len;
    Short_t vap5, vap12, van12, vbp5, vbp12, vbn12;
    Short_t valp12, vblp12, cap5, cap12, can12, cbp5, cbp12;
    Short_t cbn12, calp12, cblp12, lvps, temp, hum;
    const Int_t n=sscanf(str.Data(), "%hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %n",
                         &vap5, &vap12, &van12, &vbp5, &vbp12, &vbn12,
                         &valp12, &vblp12, &cap5, &cap12, &can12, &cbp5, &cbp12,
                         &cbn12, &calp12, &cblp12, &lvps, &temp, &hum, &len);
    if (n!=19)
    {
        *fLog << warn << "WARNING - Reading information of 'LV' section." << endl;
        return kFALSE;
    }

    fLV->fRequestPowerSupply = (Bool_t)lvps;
    fLV->fTemp = 0.1*temp;
    fLV->fHumidity = (Byte_t)hum;

    fLV->fPowerSupplyA.fVoltagePos5V         = 0.01*vap5;
    fLV->fPowerSupplyA.fVoltagePos12V        = 0.01*vap12;
    fLV->fPowerSupplyA.fVoltageNeg12V        = 0.01*van12;
    fLV->fPowerSupplyA.fVoltageOptLinkPos12V = 0.01*valp12;
    fLV->fPowerSupplyA.fCurrentPos5V         = 0.001*cap5;
    fLV->fPowerSupplyA.fCurrentPos12V        = 0.001*cap12;
    fLV->fPowerSupplyA.fCurrentNeg12V        = 0.001*can12;
    fLV->fPowerSupplyA.fCurrentOptLinkPos12V = 0.001*calp12;

    fLV->fPowerSupplyB.fVoltagePos5V         = 0.01*vbp5;
    fLV->fPowerSupplyB.fVoltagePos12V        = 0.01*vbp12;
    fLV->fPowerSupplyB.fVoltageNeg12V        = 0.01*vbn12;
    fLV->fPowerSupplyB.fVoltageOptLinkPos12V = 0.01*vblp12;
    fLV->fPowerSupplyB.fCurrentPos5V         = 0.001*cbp5;
    fLV->fPowerSupplyB.fCurrentPos12V        = 0.001*cbp12;
    fLV->fPowerSupplyB.fCurrentNeg12V        = 0.001*cbn12;
    fLV->fPowerSupplyB.fCurrentOptLinkPos12V = 0.001*cblp12;

    str.Remove(0, len);
    str=str.Strip(TString::kLeading);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the AUX* part of the report
//
Bool_t MReportCamera::InterpreteAUX(TString &str)
{
    if (!CheckTag(str, "AUX "))
        return kFALSE;

    Int_t len;
    Short_t led, fan;
    const Int_t n=sscanf(str.Data(), "%hd %hd %n", &led, &fan, &len);
    if (n!=2)
    {
        *fLog << warn << "WARNING - Reading information of 'AUX' section." << endl;
        return kFALSE;
    }

    fAUX->fRequestCaosLEDs=(Bool_t)led;
    fAUX->fRequestFansFADC=(Bool_t)fan;

    str.Remove(0, len);
    str=str.Strip(TString::kLeading);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the CAL* part of the report
//
Bool_t MReportCamera::InterpreteCAL(TString &str)
{
    if (!CheckTag(str, "CAL "))
        return kFALSE;

    Int_t len;
    Short_t hv, lv, cont, pin;

    const Int_t n=sscanf(str.Data(), "%hd %hd %hd %hd %n", &hv, &lv, &cont, &pin, &len);
    if (n!=4)
    {
        *fLog << warn << "WARNING - Reading information of 'CAL' section." << endl;
        return kFALSE;
    }

    fCalibration->fRequestHiVoltage = (Bool_t)hv;
    fCalibration->fRequestLoVoltage = (Bool_t)lv;
    fCalibration->fRequestContLight = (Bool_t)cont;
    fCalibration->fRequestPinDiode  = (Bool_t)pin;

    str.Remove(0, len);
    str=str.Strip(TString::kBoth);
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the HOT* part of the report
//
Bool_t MReportCamera::InterpreteHOT(TString &str)
{
    if (!CheckTag(str, "HOT "))
        return kFALSE;

    Int_t len;
    Int_t hot;

    const Int_t n=sscanf(str.Data(), "%d %n", &hot, &len);
    if (n!=1)
    {
        *fLog << warn << "WARNING - Reading information of 'HOT' section." << endl;
        return kFALSE;
    }

    str.Remove(0, len);
    str=str.Strip(TString::kBoth);

    return kTRUE;
}


// --------------------------------------------------------------------------
//
// Interprete the Active Load REPORT part
//
Bool_t MReportCamera::InterpreteActiveLoad(TString &str)
{
    if (!CheckTag(str, "ACTLOAD "))
        return kFALSE;

    Int_t len;
    Short_t v360a, i360a, v360b, i360b, v175a, i175a, v175b, i175b;
    Int_t n=sscanf(str.Data(), " %hd %hd %hd %hd %hd %hd %hd %hd %n",
                   &v360a, &i360a, &v360b, &i360b, &v175a, &i175a, &v175b, &i175b, &len);
    if (n!=8)
    {
        *fLog << warn << "WARNING - Reading information of 'ACTLOAD' section." << endl;
        return kFALSE;
    }

    fActiveLoad->fVoltage360A = (float)v360a*0.1;
    fActiveLoad->fIntens360A  = (float)i360a*0.01;
    fActiveLoad->fVoltage360B = (float)v360b*0.1;
    fActiveLoad->fIntens360B  = (float)i360b*0.01;
    fActiveLoad->fVoltage175A = (float)v175a*0.1;
    fActiveLoad->fIntens175A  = (float)i175a*0.01;
    fActiveLoad->fVoltage175B = (float)v175b*0.1;
    fActiveLoad->fIntens175B  = (float)i175b*0.01;

    str.Remove(0, len);
    str=str.Strip(TString::kBoth);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the Central Pixel part
//
Bool_t MReportCamera::InterpreteCentralPix(TString &str, Int_t ver)
{
    if (!CheckTag(str, "CPIX "))
        return kFALSE;

    Int_t len;
    Short_t status;

    Int_t n=sscanf(str.Data(), " %hd %n", &status, &len);
    if (n!=1)
    {
        *fLog << warn << "WARNING - Reading information of 'CPIX' section." << endl;
            return kFALSE;
    }

    if (ver>=200812140)
    {
        Int_t len2;
        Int_t dc;
        n=sscanf(str.Data()+len, " %d %n", &dc, &len2);
        if (n!=1)
        {
            *fLog << warn << "WARNING - Reading information of 'CPIX' section." << endl;
            return kFALSE;
        }

        fCentralPix->fDC = dc;

        len += len2;
    }

    fCentralPix->fStatus = (Bool_t)status;

    str.Remove(0, len);
    str=str.Strip(TString::kBoth);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the CHTEMP part
//
Bool_t MReportCamera::InterpreteCHTEMP(TString &str)
{
    if (!CheckTag(str, "CHTEMP "))
        return kFALSE;

    Int_t len, temp1, temp2, temp3;
    Int_t n=sscanf(str.Data(), " %d %d %d %n", &temp1, &temp2, &temp3, &len);
    if (n!=3)
    {
        *fLog << warn << "WARNING - Reading information of 'CHTEMP' section." << endl;
        return kFALSE;
    }

    fAUX->fTempCountingHouse1 = temp1*0.01;
    fAUX->fTempCountingHouse2 = temp2*0.01;
    fAUX->fTempCountingHouse3 = temp3*0.01;

    str.Remove(0, len);
    str=str.Strip(TString::kBoth);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the HVFIL part
//
Bool_t MReportCamera::InterpreteHVFIL(TString &str)
{
    if (!CheckTag(str, "HVFIL "))
        return kFALSE;

    str=str.Strip(TString::kBoth);

    if (str.BeginsWith("PSSEN "))
    {
        fHV->fFileName = "";
        return kTRUE;
    }

    const Ssiz_t pos = str.First(' ');
    /*
    if (pos<0)
    {
        *fLog << warn << "WARNING - Reading information of 'HVFIL' section." << endl;
        return kFALSE;
    }
    */

    fHV->fFileName = pos<0 ? (TString)"" : str(0, pos);
    if (pos<0)
        return kTRUE;

    str.Remove(0, pos);
    str=str.Strip(TString::kBoth);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// ps: Camera cabinet power supply
// v1: PS sensor v1
// v2: PS sensor v2
//
Bool_t MReportCamera::InterpretePSSEN(TString &str)
{
    if (!CheckTag(str, "PSSEN "))
        return kCONTINUE;

    Int_t len;
    Int_t ps, v1, v2;

    const Int_t n=sscanf(str.Data(), "%d %d %d %n", &ps, &v1, &v2, &len);
    if (n!=3)
    {
        *fLog << warn << "WARNING - Reading information of 'PSSEN' section." << endl;
        return kFALSE;
    }

    str.Remove(0, len);
    str=str.Strip(TString::kBoth);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// liq: Liquid inside camera
//
Bool_t MReportCamera::InterpreteLIQ(TString &str)
{
    if (!CheckTag(str, "LIQ "))
        return kFALSE;

    Int_t len;
    Int_t liq;

    const Int_t n=sscanf(str.Data(), "%d %n", &liq, &len);
    if (n!=1)
    {
        *fLog << warn << "WARNING - Reading information of 'LIQ' section." << endl;
        return kFALSE;
    }

    str.Remove(0, len);
    str=str.Strip(TString::kBoth);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the CAMERA-REPORT part
//
Bool_t MReportCamera::InterpreteCamera(TString &str, Int_t ver)
{
    //
    // I have tried to do it with pure pointer arithmentics, but most of the time is spent
    // to do the sscanf. So we gain less than 5% not using TString like it is done here.
    Int_t len1=0;
    Short_t cal, stat, hvps, lid, lv, cool, hv, dc, led, fan, can, io, clv;
    Int_t n;

    n=sscanf(str.Data(), " %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %n",
             &cal, &stat, &hvps, &lid, &lv, &cool, &hv,
             &dc, &led, &fan, &can, &io, &clv, &len1);
    if (n!=13)
    {
        *fLog << warn << "WARNING - Cannot interprete status' of subsystems." << endl;
        return kFALSE;
    }

    fHV->fStatus                   = (Byte_t)hvps;
    fLids->fStatus                 = (Byte_t)lid;
    fLV->fStatus                   = (Byte_t)lv;
    fCooling->fStatus              = (Byte_t)cool;
    fHV->fStatusRamping            = (Byte_t)hv;
    fAUX->fStatusCaosLEDs          = (Bool_t)led;
    fAUX->fStatusFansFADC          = (Bool_t)fan;
    fCalibration->fStatus          = (Bool_t)cal;
    fCalibration->fStatusCANbus    = (Bool_t)can;
    fCalibration->fStatusIO        = (Bool_t)io;
    fCalibration->fStatusLoVoltage = (Bool_t)clv;
    fStatus                        = (Byte_t)stat;
    fDC->fStatus                   = (Byte_t)dc;
    fActiveLoad->fStatus           = 0xff;

    Int_t len2=0;
    if (ver > 200504130)
    {
        Short_t actl;
        n=sscanf(str.Data()+len1, " %hd %n", &actl, &len2);
        if (n!=1)
        {
            *fLog << warn << "WARNING - Cannot interprete status of active load." << endl;
            return kFALSE;
        }
        fActiveLoad->fStatus = (Byte_t)actl;
    }
    str.Remove(0, len1+len2);
    str=str.Strip(TString::kLeading);

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Interprete the body of the CAMERA-REPORT string
//
Int_t MReportCamera::InterpreteBody(TString &str, Int_t ver)
{
    if (!InterpreteCamera(str, ver))
        return kCONTINUE;

    if (!InterpreteDC(str))
        return kCONTINUE;

    if (!InterpreteHV(str))
        return kCONTINUE;

    if (!InterpreteCOOL(str))
        return kCONTINUE;

    if (!InterpreteLID(str))
        return kCONTINUE;

    if (!InterpreteHVPS(str))
        return kCONTINUE;

    if (!InterpreteLV(str))
        return kCONTINUE;

    if (!InterpreteAUX(str))
        return kCONTINUE;

    if (!InterpreteCAL(str))
        return kCONTINUE;

    if (ver >= 200407070)
    {
        if (!InterpreteHOT(str))
            return kCONTINUE;
    }

    if (ver > 200504130)
    {
         if (!InterpreteActiveLoad(str))
            return kCONTINUE;
         if (!InterpreteCentralPix(str, ver))
            return kCONTINUE;
    }

    if (ver >= 200507190)
    {
        if (!InterpreteCHTEMP(str))
            return kCONTINUE;
        if (!InterpreteHVFIL(str))
            return kCONTINUE;
    }

    if (ver >= 200812140)
    {
        if (!InterpretePSSEN(str))
            return kCONTINUE;
        if (!InterpreteLIQ(str))
            return kCONTINUE;
    }

    if (str!="OVER")
    {
        *fLog << warn << "WARNING - 'OVER' tag not found... remaining: " << str << endl;
        return kCONTINUE;
    }

    return kTRUE;
}
