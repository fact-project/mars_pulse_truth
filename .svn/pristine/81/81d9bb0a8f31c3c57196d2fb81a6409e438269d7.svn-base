void camtemp(const char *fname, double beg=0, double end=100000)
{
    fits file(fname);

    //file.PrintColumns();
    //file.PrintKeys();

    Double_t time;
    Float_t temp[31];
    file.SetPtrAddress("Time", &time);
    file.SetPtrAddress("T_sens", temp);

    UInt_t offset = file.GetUInt("MJDREF");
    if (beg < 30000)
        beg+=offset;
    if (end < 30000)
        end+=offset;

    double avg    = 0;
    double avgrms = 0;
    double rms    = 0;
    int    cnt    = 0;

    double last  = -1;
    double lastr = -1;
    double diff  = -1;

    while (file.GetNextRow())
    {
        time += offset;

        if (time>end)
            break;

        double m = 0;
        double r = 0;
        int    n = 0;
        for (int i=0; i<31; i++)
        {
            if (temp[i]!=0)
            {
                m += temp[i];
                r += temp[i]*temp[i];
                n++;
            }
        }
        m /= n;
        r /= n;

        if (time<beg)
        {
            last  = m;
            lastr = sqrt(r-m*m);
            diff  = beg-time;
            continue;
        }

        avgrms += sqrt(r-m*m);

        avg += m;
        rms += m*m;
        cnt ++;
    }

    if (cnt==0)
    {
        if (diff<5./24/3600)
            return;

        cout << "result " << last << " 0 " << lastr << endl;
        return;
    }

    avg /= cnt;
    rms /= cnt;

    avgrms /= cnt;

    rms = sqrt(rms-avg*avg);

    cout << "result " << avg << " " << rms << " " << avgrms << endl;
}
