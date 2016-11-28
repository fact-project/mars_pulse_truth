void ctrldev(const char *fname, double beg=0, double end=100000)
{
    fits file(fname);

    //file.PrintColumns();
    //file.PrintKeys();

    Double_t time;
    Double_t zd, dzd, daz;
    file.SetPtrAddress("Time", &time);
    file.SetPtrAddress("Zd",   &zd);
    file.SetPtrAddress("dZd",  &dzd);
    file.SetPtrAddress("dAz",  &daz);

    UInt_t offset = file.GetUInt("MJDREF");
    if (beg < 30000)
        beg+=offset;
    if (end < 30000)
        end+=offset;

    double avg  = 0;
    double rms  = 0;
    int    cnt  = 0;

    double last = -1;
    double diff = -1;

    while (file.GetNextRow())
    {
        time += offset;

        if  (time>end)
            break;

        // Calculate absolute misspointing from error in zd and error in az
        Double_t dev = MAstro::GetDevAbs(zd, dzd, daz)*60;

        if (time<beg)
        {
            last = dev;
            diff = beg-time;
            continue;
        }

        avg += dev;
        rms += dev*dev;
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

    rms = sqrt(rms-avg*avg);

    cout << "result " << avg << " " << rms << endl;
}
