void lidar(const char *fname, double beg=0, double end=100000)
{
    fits file(fname);

    //file.PrintColumns();
    //file.PrintKeys();

    Double_t time;
    Float_t t3, t6, t9, t12;
    file.SetPtrAddress("Time", &time);
    file.SetPtrAddress("T3",  &t3);
    file.SetPtrAddress("T6",  &t6);
    file.SetPtrAddress("T9",  &t9);
    file.SetPtrAddress("T12", &t12);

    UInt_t offset = file.GetUInt("MJDREF");
    if (beg < 30000)
        beg+=offset;
    if (end < 30000)
        end+=offset;

    Double_t lastt;
    Float_t trans[4];

    Double_t avg[4] = {0,0,0,0};
    Int_t cnt = 0;
    while (file.GetNextRow())
    {
        if (time<50000)
            time += 682607./45;

        time += offset;

        if (time<beg)
        {
            trans[0] = t3;
            trans[1] = t6;
            trans[2] = t9;
            trans[3] = t12;
            lastt = time;
            continue;
        }

        if (time>end)
            break;

        avg[0] += t3;
        avg[1] += t6;
        avg[2] += t9;
        avg[3] += t12;
        cnt++;

    }

    if (cnt>0)
    {
        cout << "result " << avg[0]/cnt << " " << avg[1]/cnt << " " << avg[2]/cnt << " " << avg[3]/cnt << endl;
        return;
    }

    if (beg-lastt<time-end && (beg-lastt)*24*3600<10*60)
    {
        cout << "result " << trans[0] << " " << trans[1] << " " << trans[2] << " " << trans[3] << endl;
        return;
    }
    if (beg-lastt>time-end && (time-end)*24*3600<10*60)
    {
        cout << "result " << t3 << " " << t6 << " " << t9 << " " << t12 << endl;
        return;
    }
    cout << "result " << endl;
    return;
}
