void dust(const char *fname, double beg=0, double end=100000)
{
    fits file(fname);

    //file.PrintColumns();
    //file.PrintKeys();

    Double_t time;
    Float_t dust;
    file.SetPtrAddress("Time", &time);
    file.SetPtrAddress("Dust", &dust);

    UInt_t offset = file.GetUInt("MJDREF");
    if (beg < 30000)
        beg+=offset;
    if (end < 30000)
        end+=offset;

    TGraph g;
    Double_t timeprev;
    Float_t dustprev;
    Int_t counter=1;
    Int_t day=TMath::Nint((beg+end)/2);
    while (file.GetNextRow())
    {
        // handle doubl values
        if (time==timeprev && dust==dustprev)
            continue;

        //exclude day
        if (time+offset<day-0.25)
            continue;
        if (time+offset>day+0.35)
            break;

        //handle values where time gets stuck
        if (time-timeprev<1/12.*counter)
        {
            g.SetPoint(g.GetN(), timeprev+1/12.*counter+offset, dust);
            counter++;
            continue;
        }
        else
        {
            g.SetPoint(g.GetN(), time+offset, dust);
            counter=1;
        }

        timeprev=time;
        dustprev=dust;
    }
    cout << "result " << MSpline3(g).Eval((beg+end)/2) << endl;
}
