#include <iostream>

#include <TString.h>
#include <TFile.h>
#include <TGraph.h>

#include "zfits.h"
#include "PixelMap.h"
#include "Interpolator2D.h"

#include "MMath.h"
#include "MHCamera.h"
#include "MGeomCamFACT.h"

PixelMap pmap;

bool extract_current(UInt_t day, double tstart, double tstop, MHCamera &havg, MHCamera &hrms, TGraph &g)
{
    TString naux = Form("/fact/aux/%4d/%02d/%02d/%8d.FEEDBACK_CALIBRATED_CURRENTS.fits",
                        day/10000, (day/100)%100, day%100, day);

    // =====================================================================

    // Now we read the temperatures of the sensors during the
    // single pe run and average them
    fits faux(naux.Data());
    if (!faux)
    {
        cout << "Could not open " << naux << endl;
        return false;
    }

    Double_t time;
    Float_t  I[416];

    if (!faux.SetPtrAddress("Time",  &time))
        return false;
    if (!faux.SetPtrAddress("I", I))
        return false;

    TArrayD avg(320);
    TArrayD rms(320);
    TArrayI cnt(320);

    while (faux.GetNextRow())
    {
        if (time<tstart || time>tstop)
            continue;

        TArrayD med(320);

        int j=0;
        for (int i=0; i<320; i++)
        {
            if (i!=66 && i!=191 && i!=193 && i!=17 && i!=206)
                med[j++]  = I[i];

            avg[i] += I[i];
            rms[i] += I[i]*I[i];
            cnt[i]++;
        }

        Double_t median;
        MMath::MedianDev(315, med.GetArray(), median);
        g.SetPoint(g.GetN(), time, median);
    }

    if (cnt[0]==0)
        return 2;

    for (int i=0; i<320; i++)
    {
        avg[i] /= cnt[i];
        rms[i] /= cnt[i];
        rms[i] -= avg[i]*avg[i];
        rms[i] = rms[i]<0 ? 0 : sqrt(rms[i]);
    }

    double mn = 0;
    for (int i=0; i<1440; i++)
    {
        mn += avg[pmap.index(i).hv()];
        havg.SetBinContent(i+1, avg[pmap.index(i).hv()]);
        hrms.SetBinContent(i+1, rms[pmap.index(i).hv()]);
    }

    return true;
}

bool extract_dac(UInt_t day, double tstart, double tstop, MHCamera &havg, MHCamera &hrms, TGraph &g)
{
    TString naux = Form("/fact/aux/%4d/%02d/%02d/%8d.BIAS_CONTROL_DAC.fits",
                        day/10000, (day/100)%100, day%100, day);

    // =====================================================================

    // Now we read the temperatures of the sensors during the
    // single pe run and average them
    fits faux(naux.Data());
    if (!faux)
    {
        cout << "Could not open " << naux << endl;
        return false;
    }

    Double_t time;
    UShort_t dac[416];

    if (!faux.SetPtrAddress("Time",  &time))
        return false;
    if (!faux.SetPtrAddress("U", dac))
        return false;

    TArrayD avg(320);
    TArrayD rms(320);
    TArrayI cnt(320);

    while (faux.GetNextRow())
    {
        if (time<tstart || time>tstop)
            continue;

        TArrayD med(320);

        for (int i=0; i<320; i++)
        {
            med[i]  = dac[i];
            avg[i] += dac[i];
            rms[i] += dac[i]*dac[i];
            cnt[i]++;
        }

        Double_t median;
        MMath::MedianDev(320, med.GetArray(), median);
        g.SetPoint(g.GetN(), time, median);
    }

    if (cnt[0]==0)
        return 2;

    for (int i=0; i<320; i++)
    {
        avg[i] /= cnt[i];
        rms[i] /= cnt[i];
        rms[i] -= avg[i]*avg[i];
        rms[i] = rms[i]<0 ? 0 : sqrt(rms[i]);
    }

    for (int i=0; i<1440; i++)
    {
        havg.SetBinContent(i+1, avg[pmap.index(i).hv()]);
        hrms.SetBinContent(i+1, rms[pmap.index(i).hv()]);
    }

    return true;
}

bool extract_temp(UInt_t day, double tstart, double tstop, MHCamera &havg, MHCamera &hrms, TGraph &g)
{
    TString naux = Form("/fact/aux/%4d/%02d/%02d/%8d.FSC_CONTROL_TEMPERATURE.fits",
                        day/10000, (day/100)%100, day%100, day);

    Interpolator2D interpol;

    const auto sens = Interpolator2D::ReadGrid("operation/sensor-pos.txt");
    if (sens.size()!=31)
    {
        cout << "Reading sensor-pos.txt failed: " << interpol.getInputGrid().size() << endl;
        return false;
    }

    // =====================================================================

    // Now we read the temperatures of the sensors during the
    // single pe run and average them
    fits faux(naux.Data());
    if (!faux)
    {
        cout << "Could not open " << naux << endl;
        return false;
    }

    Double_t time;
    Float_t  temp[31];

    if (!faux.SetPtrAddress("Time",  &time))
        return false;
    if (!faux.SetPtrAddress("T_sens", temp))
        return false;

    TArrayD avg(31);
    TArrayD rms(31);
    TArrayI cnt(31);

    while (faux.GetNextRow())
    {
        if (time<tstart || time>tstop)
            continue;

        double mean  = 0;
        int    count = 0;

        for (int i=0; i<31; i++)
        {
            if (temp[i]==0)
                continue;

            double T = temp[i];

            mean += T;
            count++;

            avg[i] += T;
            rms[i] += T*T;
            cnt[i]++;
        }

        g.SetPoint(g.GetN(), time, mean/count);

    }

    vector<double> zavg;
    vector<double> zrms;

    vector<Interpolator2D::vec> pos;
    for (int i=0; i<31; i++)
    {
        if (cnt[i]>0)
        {
            avg[i] /= cnt[i];
            rms[i] /= cnt[i];
            rms[i] -= avg[i]*avg[i];
            rms[i] = rms[i]<0 ? 0 : sqrt(rms[i]);

            pos.push_back(sens[i]);
            zavg.push_back(avg[i]);
            zrms.push_back(rms[i]);
        }
    }

    // ================================================================

    interpol.SetInputGrid(pos);

    if (!interpol.ReadOutputGrid("operation/bias-positions.txt"))
    {
        cout << "Error setting output grid." << endl;
        return false;
    }
    if (interpol.getOutputGrid().size()!=320)
    {
        cout << "Reading bias-positions.txt failed: " << interpol.getOutputGrid().size() << endl;
        return false;
    }

    // ================================================================

    const auto &input = interpol.getInputGrid();

    TGraph sensors(input.size());

    for (unsigned int i=0; i<input.size(); i++)
        sensors.SetPoint(i, input[i].x, input[i].y);

    const vector<double> rc_avg = interpol.Interpolate(zavg);
    const vector<double> rc_rms = interpol.Interpolate(zrms);

    // ================================================================

    for (int i=0; i<1440; i++)
    {
        havg.SetBinContent(i+1, rc_avg[pmap.index(i).hv()]);
        hrms.SetBinContent(i+1, rc_rms[pmap.index(i).hv()]);
    }

    return true;

}

void extract_aux(UInt_t day=20131114, UInt_t run=136, const char *output=0)
{
    if (!pmap.Read("operation/FACTmap111030.txt"))
    {
        cout << "FACTmap111030.txt not found." << endl;
        return;
    }

    // =====================================================================

    TString nraw = Form("/fact/raw/%4d/%02d/%02d/%8d_%03d.fits.fz",
                        day/10000, (day/100)%100, day%100, day, run);


    zfits fraw(nraw.Data());
    if (!fraw)
    {
        cout << "Open raw file failed: " << nraw << endl;
        return;
    }

    double tstart = fraw.GetUInt("TSTARTI") + fraw.GetFloat("TSTARTF");
    double tstop  = fraw.GetUInt("TSTOPI")  + fraw.GetFloat("TSTOPF");

    // =====================================================================

    MGeomCamFACT geom;

    TGraph gcur;
    MHCamera hcur_m(geom);
    MHCamera hcur_r(geom);
    if (!extract_current(day, tstart, tstop, hcur_m, hcur_r, gcur))
        return;

    TGraph gtmp;
    MHCamera htmp_m(geom);
    MHCamera htmp_r(geom);
    if (!extract_temp(day, tstart, tstop, htmp_m, htmp_r, gtmp))
        return;

    TGraph gdac;
    MHCamera hdac_m(geom);
    MHCamera hdac_r(geom);
    if (!extract_dac(day, tstart, tstop, hdac_m, hdac_r, gdac))
        return;

    // ================================================================

    TCanvas *c = new TCanvas;
    c->Divide(3, 3);

    c->cd(1);
    hcur_m.SetName("CurrentAvg");
    hcur_m.SetAllUsed();
    hcur_m.DrawCopy();
    c->cd(4);
    hcur_r.SetName("CurrentRms");
    hcur_r.SetAllUsed();
    hcur_r.DrawCopy();
    c->cd(7);
    gcur.SetName("CurrentTime");
    gcur.SetMarkerStyle(kFullDotMedium);
    gcur.SetMinimum(0);
    gcur.DrawClone("AP");

    c->cd(2);
    htmp_m.SetName("TempAvg");
    htmp_m.SetAllUsed();
    htmp_m.DrawCopy();
    c->cd(5);
    htmp_r.SetName("TempRms");
    htmp_r.SetAllUsed();
    htmp_r.DrawCopy();
    c->cd(8);
    gtmp.SetName("TempTime");
    gtmp.SetMarkerStyle(kFullDotMedium);
    gtmp.SetMinimum(0);
    gtmp.DrawClone("AP");

    c->cd(3);
    hdac_m.SetName("DacAvg");
    hdac_m.SetAllUsed();
    hdac_m.DrawCopy();
    c->cd(6);
    hdac_r.SetName("DacRms");
    hdac_r.SetAllUsed();
    hdac_r.DrawCopy();
    c->cd(9);
    gdac.SetName("DacTime");
    gdac.SetMarkerStyle(kFullDotMedium);
    gdac.SetMinimum(0);
    gdac.DrawClone("AP");

    if (output)
        c->SaveAs(output);
}
