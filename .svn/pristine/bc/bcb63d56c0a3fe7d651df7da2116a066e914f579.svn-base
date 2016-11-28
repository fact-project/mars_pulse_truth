#include <iostream>

#include <TString.h>
#include <TFile.h>
#include <TGraph.h>

#include "zfits.h"
#include "PixelMap.h"
#include "Interpolator2D.h"

#include "MHCamera.h"
#include "MGeomCamFACT.h"

int extract_temp(UInt_t day=20130902, UInt_t run=146, const char *output=0)
{
    TString nraw = Form("/fact/raw/%4d/%02d/%02d/%8d_%03d.fits.fz",
                        day/10000, (day/100)%100, day%100, day, run);

    TString naux = Form("/fact/aux/%4d/%02d/%02d/%8d.FSC_CONTROL_TEMPERATURE.fits",
                        day/10000, (day/100)%100, day%100, day);

    PixelMap pmap;
    if (!pmap.Read("FACTmap111030.txt"))
    {
        cout << "FACTmap111030.txt not found." << endl;
        return 1;
    }

    Interpolator2D interpol;

    const auto sens = Interpolator2D::ReadGrid("operation/sensor-pos.txt");
    if (sens.size()!=31)
    {
        cout << "Reading sensor-pos.txt failed: " << interpol.getInputGrid().size() << endl;
        return 2;
    }

    // =====================================================================

    zfits fraw(nraw.Data());
    if (!fraw)
    {
        cout << "Open raw file failed: " << nraw << endl;
        return 1;
    }

    double tstart = fraw.GetUInt("TSTARTI") + fraw.GetFloat("TSTARTF");
    double tstop  = fraw.GetUInt("TSTOPI")  + fraw.GetFloat("TSTOPF");

    // =====================================================================

    // Now we read the temperatures of the sensors during the
    // single pe run and average them
    fits faux(naux.Data());
    if (!faux)
    {
        cout << "Could not open " << naux << endl;
        return 1;
    }

    Double_t time;
    Float_t  temp[31];

    if (!faux.SetPtrAddress("Time",  &time))
        return -1;
    if (!faux.SetPtrAddress("T_sens", temp))
        return -1;

    TArrayD avg(31);
    TArrayD rms(31);
    TArrayI cnt(31);

    while (faux.GetNextRow())
    {
        if (time<tstart || time>tstop)
            continue;

        for (int i=0; i<31; i++)
        {
            if (temp[i]==0)
                continue;

            double T = temp[i];

            avg[i] += T;
            rms[i] += T*T;
            cnt[i]++;
        }
    }

    vector<double> z;

    vector<Interpolator2D::vec> pos;
    for (int i=0; i<31; i++)
    {
        if (cnt[i]>0)
        {
            avg[i] /= cnt[i];
            rms[i] /= cnt[i];
            rms[i] = sqrt(rms[i]-avg[i]*avg[i]);

            pos.push_back(sens[i]);
            z.push_back(avg[i]);
        }
    }

    // ================================================================

    interpol.SetInputGrid(pos);

    if (!interpol.ReadOutputGrid("operation/bias-positions.txt"))
    {
        cout << "Error setting output grid." << endl;
        return 3;
    }
    if (interpol.getOutputGrid().size()!=320)
    {
        cout << "Reading bias-positions.txt failed: " << interpol.getOutputGrid().size() << endl;
        return 3;
    }

    // ================================================================

    const auto &input = interpol.getInputGrid();

    TGraph sensors(input.size());

    for (unsigned int i=0; i<input.size(); i++)
        sensors.SetPoint(i, input[i].x, input[i].y);

    const vector<double> rc = interpol.Interpolate(z);

    // ================================================================

    TCanvas *c = new TCanvas;
    c->SetName("Temp");

    MGeomCamFACT geom;

    MHCamera h1(geom);
    h1.SetName("Temp");
    for (int i=0; i<1440; i++)
        h1.SetBinContent(i+1, rc[pmap.index(i).hv()]);

    h1.SetAllUsed();
    h1.DrawCopy();

    sensors.SetName("Sensors");
    sensors.SetMarkerColor(kWhite);
    sensors.DrawClone("*");

    if (output)
        c->SaveAs(output);

    return 0;
}
