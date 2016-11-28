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
!   Author(s): Thomas Bretz  2013 <mailto:tbretz@physik.rwth-aachen.de>
!
!   Copyright: MAGIC Software Development, 2000-2015
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////
//
// MHDrsCalibration
//
///////////////////////////////////////////////////////////////////////
#include "MDrsCalibrationTime.h"

#include <TH1.h>
#include <TGraph.h>

ClassImp(MDrsCalibrationTime);

using namespace std;

bool MDrsCalibrationTime::SetDelays(const TH1 &cam)
{
    if (cam.GetDimension()!=1)
        return false;

    fDelays.assign(cam.GetNbinsX(), 0);

    for (int i=0; i<cam.GetNbinsX(); i++)
        fDelays[i] = cam.GetBinContent(i+1);

    return true;
}

void MDrsCalibrationTime::SetDelays(const TGraph &g)
{
    fDelays.assign(g.GetY(), g.GetY()+g.GetN());
}

bool MDrsCalibrationTime::ReadFits(TString fname)
{
    gSystem->ExpandPathName(fname);

    try
    {
        fits file(fname.Data());
        if (!file)
            throw runtime_error(strerror(errno));

        if (file.GetStr("TELESCOP")!="FACT")
        {
            std::ostringstream msg;
            msg << "Not a valid FACT file (TELESCOP not FACT in header)";
            throw runtime_error(msg.str());
        }

        if (file.GetNumRows()!=1)
            throw runtime_error("Number of rows in table is not 1.");

        fNumSamples  = file.GetUInt("NROI");
        fNumChannels = file.GetUInt("NCH");
        fNumEntries  = file.GetUInt("NBTIME");

        const double *d = reinterpret_cast<double*>(file.SetPtrAddress("CellOffset"));
        if (!file.GetNextRow())
            throw runtime_error("Read error.");

        fOffsets.assign(d, d+fNumSamples*fNumChannels),
        fDelays.resize(0);
    }
    catch (const exception &e)
    {
        *fLog << err << "Error reading from " << fname << ": " << e.what() << endl;
        return false;
    }

    *fLog << inf << "Read DRS calibration file " << fname << endl;
    return true;
}

bool MDrsCalibrationTime::WriteFits(const string &fname) const
{
    const Bool_t exists = !gSystem->AccessPathName(fname.c_str(), kFileExists);
    if (exists)
    {
        *fLog << err << "File '" << fname << "' already exists." << endl;
        return false;
    }

    try
    {
        ofits file(fname.c_str());
        if (!file)
            throw runtime_error(strerror(errno));

        file.SetDefaultKeys();
        file.AddColumnDouble(fNumSamples*fNumChannels, "CellOffset", "samples", "Integral cell offset");

        file.SetInt("ADCRANGE", 2000,    "Dynamic range of the ADC in mV");
        file.SetInt("DACRANGE", 2500,    "Dynamic range of the DAC in mV");
        file.SetInt("ADC",      12,      "Resolution of ADC in bits");
        file.SetInt("DAC",      16,      "Resolution of DAC in bits");
        file.SetInt("NPIX",     1440,    "Number of channels in the camera");
        file.SetInt("NTM",      0,       "Number of time marker channels");
        file.SetInt("NROI",     fNumSamples,  "Region of interest");
        file.SetInt("NCH",      fNumChannels, "Number of chips");
        file.SetInt("NBTIME",   fNumEntries,  "Num of entries for time calibration");

        file.WriteTableHeader("DrsCellTimes");
        //file.SetInt("NIGHT", night, "Night as int");

        /*
        file.SetStr("DATE-OBS", fDateObs, "First event of whole DRS calibration");
        file.SetStr("DATE-END", fDateEnd, "Last event of whole DRS calibration");
        file.SetStr("RUN-BEG", fDateRunBeg[0], "First event of run 0");
        file.SetStr("RUN-END", fDateRunEnd[0], "Last event of run 0");
        */

        if (!file.WriteRow(fOffsets.data(), fOffsets.size()*sizeof(double)))
            throw runtime_error("Write error.");
    }
    catch (const exception &e)
    {
        *fLog << err << "Error writing to " << fname << ": " << e.what() << endl;
        return false;
    }

    *fLog << inf << "Wrote DRS calibration file " << fname << endl;
    return true;
}
