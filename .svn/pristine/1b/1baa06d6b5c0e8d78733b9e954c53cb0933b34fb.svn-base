void drstemp82(fits &file, double beg, double end)
{
    UInt_t offset = file.GetUInt("MJDREF");
    if (beg < 30000)
        beg+=offset;
    if (end < 30000)
        end+=offset;

    double time;
    file.SetPtrAddress("Time", &time);

    float *temp = file.SetPtrAddress("temp");

    double avgmin = 0;
    double avgmax = 0;
    double rmsmin = 0;
    double rmsmax = 0;

    int cnt = 0;

    double lastavgmin = -1;
    double lastavgmax = -1;
    double lastrmsmin = -1;
    double lastrmsmax = -1;

    double diff  = -1;

    while (file.GetNextRow())
    {
        time += offset;

        if (time>end)
            break;

        double mn = 0;
        double mx = 0;
        double rn = 0;
        double rx = 0;
        for (int i=1; i<=40; i++)
        {
            mn += temp[i];
            rn += temp[i]*temp[i];

            mx += temp[i+41];
            rx += temp[i+41]*temp[i+41];
        }
        mn /= 40;
        mx /= 40;
        rn /= 40;
        rx /= 40;

        if (time<beg)
        {
            lastavgmin  = mn;
            lastavgmax  = mx;
            lastrmsmin  = sqrt(rn-mn*mn);
            lastrmsmax  = sqrt(rx-mx*mx);
            diff  = beg-time;
            continue;
        }

        avgmin += mn;
        avgmax += mx;

        rmsmin += sqrt(rn-mn*mn);
        rmsmax += sqrt(rx-mx*mx);

        cnt ++;
    }

    if (cnt==0)
    {
        if (diff<5./24/3600)
            return;

        cout << "result " << lastavgmin << " " << lastavgmax << " " << lastrmsmin << " " << lastrmsmax << endl;
        return;
    }

    avgmin /= cnt;
    avgmax /= cnt;
    rmsmin /= cnt;
    rmsmax /= cnt;

    cout << "result " << avgmin << " " << avgmax << " " << rmsmin << " " << rmsmax << endl;
}

void drstemp160(fits &file, double beg, double end)
{
    UInt_t offset = file.GetUInt("MJDREF");
    if (beg < 30000)
        beg+=offset;
    if (end < 30000)
        end+=offset;

    double time;
    file.SetPtrAddress("Time", &time);

    float *temp = file.SetPtrAddress("temp");

    double avg = 0;
    double rms = 0;

    int cnt = 0;

    double lastavg = -1;
    double lastrms = -1;

    double diff  = -1;

    while (file.GetNextRow())
    {
        time += offset;

        if (time>end)
            break;

        double mn = 0;
        double rn = 0;
        for (int i=0; i<=160; i++)
        {
            mn += temp[i];
            rn += temp[i]*temp[i];
        }
        mn /= 160;
        rn /= 160;

        if (time<beg)
        {
            lastavg = mn;
            lastrms = sqrt(rn-mn*mn);
            diff    = beg-time;
            continue;
        }

        avg += mn;
        rms += sqrt(rn-mn*mn);

        cnt ++;
    }

    if (cnt==0)
    {
        if (diff<5./24/3600)
            return;

        cout << "result " << lastavg << " " << lastavg << " " << lastrms << " " << lastrms << endl;
        return;
    }

    avg /= cnt;
    rms /= cnt;

    cout << "result " << avg << " " << avg << " " << rms << " " << rms << endl;
}

void drstemp(const char *fname, double beg=0, double end=100000)
{
    fits file(fname);

    //file.PrintColumns();
    //file.PrintKeys();

    float temp[160];
//    if (file.SetPtrAddress("temp", temp, 82))
//    {
//        drstemp82(file, beg, end);
//        return;
//    }

    file.SetPtrAddress("temp", temp, 160);
    drstemp160(file, beg, end);
}
