#include <algorithm>

void threshold(const char *fname, double beg=0, double end=100000)
{
    fits file(fname);

    //file.PrintColumns();
    //file.PrintKeys();

    Double_t time;
    UShort_t th[160];
    file.SetPtrAddress("Time",        &time);
    file.SetPtrAddress("PatchThresh", th);

    UInt_t offset = file.GetUInt("MJDREF");
    if (beg < 30000)
        beg+=offset;
    if (end < 30000)
        end+=offset;

    double average  = 0;
    double med_avg  = 0;
    double med_rms  = 0;
    UShort_t max_tot  = 0;
    int    cnt  = 0;

    double avg_last = -1;
    double med_last = -1;
    UShort_t max_last = -1;
    double diff = -1;

    while (file.GetNextRow())
    {
        time += offset;

        if (time>end)
            break;

        sort(th, th+160);

        Float_t med = (th[80] + th[81]) / 2;
        UShort_t max = th[159];

        Double_t avg = 0;
        for (int i=0; i<160; i++)
            avg += th[i];
        avg /= 160;

        if (time<beg)
        {
            avg_last = avg;
            med_last = med;
            max_last = max;
            diff = beg-time;
            continue;
        }

        med_avg += med;
        med_rms += med*med;
        average += avg;

        if (max>max_tot)
            max_tot = max;

        cnt ++;
    }

    if (cnt==0)
    {
        if (diff<5./24/3600)
            return;

        cout << "result " << med_last << " 0 " << max_last << " " << avg_last << endl;
        return;
    }

    average /= cnt;
    med_avg /= cnt;
    med_rms /= cnt;
    med_rms = sqrt(med_rms-med_avg*med_avg);

    cout << "result " << med_avg << " " << med_rms << " " << max_tot << " " << average << endl;
}
