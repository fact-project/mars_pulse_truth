void contemp(const char *fname, double beg=0, double end=100000)
{
    fits file(fname);

    //file.PrintColumns();
    //file.PrintKeys();

    Double_t time;
    Float_t temp;
    file.SetPtrAddress("Time", &time);
    file.SetPtrAddress("T",    &temp);

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

        if (time<beg)
        {
            last = temp;
            diff = beg-time;
            continue;
        }

        avg += temp;
        rms += temp*temp;
        cnt ++;
    }

    if (cnt==0)
    {
        if (diff<5./24/3600)
            return;

        cout << "result " << last << " 0 " << endl;
        return;
    }

    avg /= cnt;
    rms /= cnt;

    rms = rms>avg*avg ? 0 : sqrt(rms-avg*avg);

    cout << "result " << avg << " " << rms << endl;
}
