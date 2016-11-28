void camhum(const char *fname, double beg=0, double end=100000)
{
    fits file(fname);

    //file.PrintColumns();
    //file.PrintKeys();

    Double_t time;
    Float_t hum[4];
    file.SetPtrAddress("Time", &time);
    file.SetPtrAddress("H",     hum);

    UInt_t offset = file.GetUInt("MJDREF");
    if (beg < 30000)
        beg+=offset;
    if (end < 30000)
        end+=offset;

    double avg    = 0;
    int    cnt    = 0;

    double last  = -1;
    double diff  = -1;

    while (file.GetNextRow())
    {
        time += offset;

        if (time>end)
            break;

        int    n = 0;
        double h = 0;
        for (int i=0; i<4; i++)
            if (hum[i]>0)
            {
                h += hum[i];
                n++;
            }

        if (n==0)
            continue;

        if (time<beg)
        {
            last = h/n;
            diff = beg-time;
            continue;
        }

        avg += h/n;
        cnt ++;
    }

    if (cnt==0)
    {
        if (diff<5./24/3600)
            return;

        cout << "result " << last << endl;
        return;
    }

    cout << "result " << avg/cnt << endl;
}
