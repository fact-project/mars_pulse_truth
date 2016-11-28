/*
#ifndef HAVE_ZLIB
#define HAVE_ZLIB
#endif

#include <TMath.h>

#include "fits.h"
#include "ofits.h"

#include "MArrayF.h"
#include "MMath.h"
*/

using namespace std;

bool Read(const char *fname, int *npix)
{
    std::ifstream fin(fname);

    int l = 0;

    while (1)
    {
        if (l>1439)
            break;

        TString line;
        line.ReadLine(fin);
        if (!fin)
            break;

        line = line.Strip(TString::kBoth);
        if (line[0]=='#')
            continue;

        TObjArray *arr = line.Tokenize(' ');
        int cbpx  = atoi(((*arr)[1])->GetName());
        int board = atoi(((*arr)[6])->GetName());
        int ch    = atoi(((*arr)[7])->GetName());
        delete arr;

        int hv = board*32+ch;
        if (hv>=416)
            return false;

        int n = cbpx%10>3 ? 5 : 4;
        if (npix[hv]>0 && npix[hv]!=n)
            return false;

        npix[hv] = n;
        l++;
    }

    return l==1440;
}

int calibrate(UInt_t night, TString path="/fact/aux")
{
    int npix[416];
    memset(npix, 0, sizeof(int)*416);
    if (!Read("/gpfs/scratch/fact/FACTmap111030.txt", npix))
        return 42;

    UInt_t d =  night%100;
    UInt_t m = (night/100)%100;
    UInt_t y =  night/10000;

    path += Form("/%04d/%02d/%02d/%d", y, m, d, night);
    TString path2 = Form("/gpfs/scratch/fact/calibrated_currents/%d", night);

    TString filec = path + ".FEEDBACK_CALIBRATION.fits";
    TString fileu = path + ".BIAS_CONTROL_VOLTAGE.fits";
    TString filei = path + ".BIAS_CONTROL_CURRENT.fits";
    TString fileo = path2 + ".CALIBRATED_CURRENTS.fits";

    cout << filec << endl;
    cout << fileu << endl;
    cout << filei << endl;
    cout << fileo << endl;

    fits fitsc1(filec.Data());
    fits fitsc2(filec.Data());
    if (!fitsc2)
    {
        cout << "ERROR - Open " << filec << " failed." << endl;
        return 1;
    }

    fits fitsu1(fileu.Data());
    fits fitsu2(fileu.Data());
    if (!fitsu2)
    {
        cout << "ERROR - Open " << fileu << " failed." << endl;
        return 2;
    }

    fits fitsi1(filei.Data());
    fits fitsi2(filei.Data());
    if (!fitsi2)
    {
        cout << "ERROR - Open " << filei << " failed." << endl;
        return 3;
    }

    ofits fout(fileo.Data());
    if (!fout)
    {
        cout << "ERROR - Open " << fileo << " failed." << endl;
        return 4;
    }

    //const MTime now;
    fout.SetStr(  "TELESCOP", "FACT",               "Telescope that acquired this data");
    fout.SetStr(  "PACKAGE",  "MARS",               "Package name");
    fout.SetStr(  "VERSION",  "1.0",                "Package description");
    fout.SetFloat("EXTREL",   1.0,                  "Release Number");
    //fout.SetStr(  "COMPILED", __DATE__" "__TIME__,  "Compile time");
    fout.SetStr(  "ORIGIN",   "FACT",               "Institution that wrote the file");
    //fout.SetStr(  "DATE",     now.GetStringFmt("%Y-%m-%dT%H:%M:%S").Data(), "File creation date");
    //fout.SetInt(  "NIGHT",    now.GetNightAsInt(),  "Night as int");
    fout.SetStr(  "TIMESYS",  "UTC",                "Time system");
    fout.SetStr(  "TIMEUNIT", "d",                  "Time given in days w.r.t. to MJDREF");
    fout.SetInt(  "MJDREF",   40587,                "MJD to UNIX time (seconds since 1970/1/1)");

    fout.AddColumnDouble("Time",  "MJD", "Event time: MJD - MJDREF (= Unix Time)");
    fout.AddColumnDouble("T_cal", "MJD", "Calibration time: MJD - MJDREF (= Unix Time)");
    fout.AddColumnInt(   "N",     "",    "Number of valid values");
    fout.AddColumnFloat( "I_avg", "uA",  "Average calibrated current (320 channels)");
    fout.AddColumnFloat( "I_rms", "uA",  "RMS of calibrated current (320 channels)");
    fout.AddColumnFloat( "I_med", "uA",  "Median of calibrated currents (320 channels)");
    fout.AddColumnFloat( "I_dev", "uA",  "Deviation of calibrated currents (320 channels)");
    fout.AddColumnFloat(416, "I", "uA", "My comment");

    fout.WriteTableHeader("CALIBRATED");

    Double_t timec, timeu, timei;
    Double_t nextc, nextu, nexti;

    if (!fitsc1.SetPtrAddress("Time", &timec))
    {
        cout << "ERROR couldn't get pointer to 1st Time for R" << endl;
        return 2;
    }
    if (!fitsu1.SetPtrAddress("Time", &timeu))
    {
        cout << "ERROR couldn't get pointer to 1st Time for U" << endl;
        return 2;
    }
    if (!fitsi1.SetPtrAddress("Time", &timei))
    {
        cout << "ERROR couldn't get pointer to 1st Time for I" << endl;
        return 2;
    }

    if (!fitsc2.SetPtrAddress("Time", &nextc))
    {
        cout << "ERROR couldn't get pointer to 2nd Time for R" << endl;
        return 2;
    }
    if (!fitsu2.SetPtrAddress("Time", &nextu))
    {
        cout << "ERROR couldn't get pointer to 2nd Time for U" << endl;
        return 2;
    }
    if (!fitsi2.SetPtrAddress("Time", &nexti))
    {
        cout << "ERROR couldn't get pointer to 2nd Time for I" << endl;
        return 2;
    }

    Float_t  R[416];
    Float_t  U[416];
    UShort_t I[416];

    if (!fitsc1.SetPtrAddress("R",    R))
    {
        cout << "ERROR couldn't get pointer to R" << endl;
        return 2;
    }
    if (!fitsu1.SetPtrAddress("Uout", U))
    {
        cout << "ERROR couldn't get pointer to U" << endl;
        return 2;
    }
    if (!fitsi1.SetPtrAddress("I",    I))
    {
        cout << "ERROR couldn't get pointer to I" << endl;
        return 2;
    }

    fitsc2.GetNextRow();
    fitsu2.GetNextRow();
    fitsi2.GetNextRow();

    if (!fitsc1.GetNextRow())
        return 0;
    if (!fitsc2.GetNextRow())
        nextc = 1e50;

    while (1)
    {
        // First the next two rows (voltage, current)
        // with identical time stamp
        if (nextu<nextc && nextu<nexti)
        {
            if (!fitsu1.GetNextRow())
                break;
            fitsu2.GetNextRow();
            continue;
        }

        if (nexti<nextc && nexti<nextu)
        {
            if (!fitsi1.GetNextRow())
                break;
            fitsi2.GetNextRow();
            continue;
        }

        // At this point we have found currents and voltages
        // with the same time-stamp
        if (nexti!=nextu)
            break;

        // Now read the next row, voltages and currenst, which
        // should be evaluated
        if (!fitsu1.GetNextRow() || !fitsi1.GetNextRow())
            break;

        // Read also one event in advance to get the next time-stamps
        fitsu2.GetNextRow();
        fitsi2.GetNextRow();

        // Read new calibration events as long as the difference
        // in time to the next calibration evenet is smaller
        // than to the current one. In this way always the
        // closest one is used.
        while (fabs(nextc-nextu)<fabs(timec-timeu))
        {
            // If there is no next one, make sure that
            // the the condition is never met again.
            if (!fitsc1.GetNextRow())
            {
                nextc=1e50;
                break;
            }
            fitsc2.GetNextRow();
        }

        // Calibrate the data (subtract offset)
        MArrayF rc(9+416);
        MArrayF stat(416);

        Float_t *cur = rc.GetArray()+9;

        UInt_t cnt = 0;
        for (int i=0; i<416; i++)
        {
            if (R[i]<=0)
                continue;

            // Convert dac counts to uA
            // Measued current minus leakage current (bias crate calibration)
            cur[i] = I[i] * 5000./4096 - U[i]/R[i]*1e6;

            if (npix[i]>0)
            {
                cur[i] /= npix[i];
                stat[cnt++] = cur[i];
            }
        }

        memcpy(rc.GetArray(),   &timeu, sizeof(Double_t));
        memcpy(rc.GetArray()+2, &timec, sizeof(Double_t));
        memcpy(rc.GetArray()+4, &cnt,   sizeof(UInt_t));

        Double_t med;
        rc[5] = TMath::Mean(cnt, stat.GetArray());
        rc[6] = TMath::RMS(cnt, stat.GetArray());
        rc[8] = MMath::MedianDev(cnt, stat.GetArray(), med);
        rc[7] = med;

        fout.WriteRow(rc.GetArray(), rc.GetSize()*sizeof(Float_t));
    }

    return 0;
}
