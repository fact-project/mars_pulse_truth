void lastth(const char *fname, double beg=0)
{
    fits file(fname);

    //file.PrintColumns();
    //file.PrintKeys();

    Double_t time;
    UShort_t th;
    file.SetPtrAddress("Time",      &time);
    file.SetPtrAddress("threshold", &th);

    UInt_t offset = file.GetUInt("MJDREF");
    if (beg<30000)
        beg += offset;

    UShort_t th_last   =  0;
    Double_t time_last = -1;

    while (file.GetNextRow())
    {
        time += offset;

        if (time>beg && fabs(beg-time)>fabs(time_last))
            break;

        th_last   = th;
        time_last = beg-time;
    }

    if (th_last>0)
        cout << "result " << th_last << " " << TMath::Nint(time_last*24*3600) << endl;
}
