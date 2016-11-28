Int_t CheckVoltage(TString fname, Double_t beg, Double_t end)
{
    fname.ReplaceAll("RATE_SCAN_DATA", "BIAS_CONTROL_VOLTAGE");

    fits file(fname.Data());
    if (!file)
        return -2;

    //cout << fname << endl;

    Double_t time;
    Float_t  Uout[416];
    UShort_t Uref[416];
    memset(Uout, 0, 416*4);
    memset(Uref, 0, 416*2);

    if (!file.SetPtrAddress("Time",  &time))
        return -1;

    if (!file.SetPtrAddress("Uout", Uout) && !file.SetPtrAddress("Uref", Uref))
        return -1;

    //cout << "Search for: " << beg-15773 << " " << end-15773 << endl;

    Int_t rows = 0;
    while (file.GetNextRow())
    {
        if (time<beg)
            continue;

        if (time>end)
            break;

        Int_t cnt = 0;
        for (int i=0; i<320; i++)
            if (Uout[i]>50 || Uref[i]>2900)
                cnt++;

        if (cnt<318)
            return kFALSE;

        rows++;
    }

    return rows>0;
}

Float_t GetOffset(TString fname, Double_t beg, Double_t end)
{
    if (end < 15937)
        fname.ReplaceAll("RATE_SCAN_DATA", "FEEDBACK_DEVIATION");
    else
        fname.ReplaceAll("RATE_SCAN_DATA", "FEEDBACK_CALIBRATED_CURRENTS");

    fits file(fname.Data());

    //cout << fname << endl;

    Double_t time;
    Float_t  delta      = -100;
    Float_t  delta_prev = -100;

    if (!file.SetPtrAddress("Time",  &time))
        return -100;

    if (end < 15937)
    {
        if (!file.SetPtrAddress("DeltaUser", &delta))
            return -100;
    }
    else
    {
        if (!file.SetPtrAddress("U_nom", &delta))
            return -100;
    }

    //cout << "Search for: " << beg-15773 << " " << end-15773 << endl;

    Int_t rows = 0;
    while (file.GetNextRow())
    {
        if (time>beg)
            return delta_prev;

        delta_prev = delta;
    }

    return -100;
}

Float_t GetCurrent(TString fname, Double_t beg, Double_t end)
{
    fname.ReplaceAll("RATE_SCAN_DATA", "FEEDBACK_CALIBRATED_CURRENTS");

    //the next two lines are needed for ISDC and data before 11.3.2013
    //fname.ReplaceAll("RATE_SCAN_DATA", "CALIBRATED_CURRENTS");
    //fname = gSystem->BaseName(fname.Data());
    //fname.Prepend("/scratch_nfs/calibrated_currents/");

    fits file(fname.Data());
    if (!file)
        return -2;

    //cout << fname << endl;

    Double_t time;
    Float_t med, dev;
    if (!file.SetPtrAddress("Time",  &time) ||
        !file.SetPtrAddress("I_med", &med))
        return -2;

    double med_avg  = 0;
    int    cnt  = 0;

    //cout << "Search for: " << beg-15773 << " " << end-15773 << endl;

    while (file.GetNextRow())
    {
//        if (int(time)==15773)
//            cout << time-15773 << endl;

        if (time<beg)
            continue;

        if (time>end)
            break;

        med_avg += med;
        cnt ++;
    }

    return cnt==0 ? -1 : med_avg/cnt;
}

TArrayD GetPointing(TString fname, Double_t beg, Double_t end)
{
    fname.ReplaceAll("RATE_SCAN_DATA", "DRIVE_CONTROL_TRACKING_POSITION");

    fits file(fname.Data());
    if (!file)
        return TArrayD();

    //cout << fname << endl;

    Double_t time;
    Double_t zd, az, ra, dec;
    if (!file.SetPtrAddress("Time", &time) ||
        !file.SetPtrAddress("Az",   &az)   ||
        !file.SetPtrAddress("Zd",   &zd)   ||
        !file.SetPtrAddress("Ra",   &ra)   ||
        !file.SetPtrAddress("Dec",  &dec))
        return TArrayD();

    Int_t n = 0;

    TArrayD rc;
    while (file.GetNextRow())
    {
        if (time<beg)
            continue;

        if (time>end)
            break;

        if (rc.GetSize()==0)
        {
            rc.Set(6);

            rc[0] = zd;
            rc[1] = az;
            rc[2] = zd;
            rc[3] = az;

            rc[4] = ra;
            rc[5] = dec;
            continue;
        }

        if (zd<rc[0]) rc[0] = zd;
        if (zd>rc[2]) rc[2] = zd;

        if (az<rc[1]) rc[1] = az;
        if (az>rc[3]) rc[3] = az;

        rc[4] += ra;
        rc[5] += dec;

        n++;
    }

    if (rc.GetSize()>0)
    {
        rc[4] /= n;
        rc[5] /= n;
    }

    return rc;
}

struct data {
    TString name;
    int offset;
    double time[2];
    UShort_t th[2];
    double rate[2];
    int cnt;
    Long64_t id;

    data(TString fname)
    {
        name = fname;
        offset = -100;
        time[0] = time[1] = -1;
        th[0] = th[1] = 0;
        rate[0] = rate[1] = -1;
        cnt = 0;
        id = -1;
    }
};

int Delete(MSQLMagic &serv, TString fname)
{
    TRegexp rex("20[0-1][0-9]/[01][0-9]/[0-3][0-9]");

    TString date = fname(rex);

    date.ReplaceAll("/", "-");

    MTime beg;
    beg.SetSqlDateTime(date);

    TString query;
    query += "fTimeBegin>'";
    query += beg.GetSqlDateTime();
    query += "' AND ";
    query += "fTimeEnd<'";
    query += MTime(beg.GetMjd()+1).GetSqlDateTime();
    query += "'";

    return serv.Delete("Ratescan", query) ? 1 : 2;
}

int Fill(MSQLMagic &serv, data &d)
{
    if (d.time[1]<0 || d.cnt==0 || d.th[0]==d.th[1])
        return 1;

    Int_t   volt    = CheckVoltage(d.name, d.time[0], d.time[1]);
    Float_t current = GetCurrent(d.name, d.time[0], d.time[1]);
    Float_t ov      = GetOffset(d.name, d.time[0], d.time[1]);
    TArrayD point   = GetPointing(d.name, d.time[0], d.time[1]);

    MTime tbeg(d.time[0]+d.offset);
    MTime tend(d.time[1]+d.offset);

    TString beg = tbeg.GetSqlDateTime();
    TString end = tend.GetSqlDateTime();

    cout << "=== BEG === " << beg  << '\n';

    cout << " ID          " << d.id << endl;

    if (current>=0)
        cout << " CUR         " << current << '\n';

    if (volt>=0)
        cout << " VOLT        " << volt << '\n';

    if (ov>-70)
        cout << " OV          " << ov                      << '\n';

    cout << " TH_beg      " << d.th[0]                  << '\n';
    cout << " TH_end      " << d.th[1]                 << '\n';
    cout << " RATE_beg    " << d.rate[0]                << '\n';
    cout << " RATE_end    " << d.rate[1]               << '\n';
    cout << " COUNT       " << d.cnt                     << '\n';
    if (point.GetSize()>0)
    {
        cout << " ZD/AZ[deg]  " << point[0] << "/" << point[1] << " -- " << point[2] << "/" << point[3] <<'\n';
        cout << " RA/DEC      " << point[4] << "h/" << point[5] << "deg\n";
    }
    cout << "=== END === " << MTime(d.time[1]+d.offset).GetSqlDateTime() << '\n' << endl;

    TString vars;
    if (volt>=0)
        vars += Form("fVoltageIsOn=%d, ", volt);
    if (current>=0)
        vars += Form("fCurrentMedMean=%f, ", current);
    if (ov>=-70)
        vars += Form("fOvervoltage=%f, ", ov);
    if (point.GetSize()>0)
    {
        vars += Form("fZdMin=%f, ", point[0]);
        vars += Form("fZdMax=%f, ", point[2]);
        vars += Form("fAzMin=%f, ", point[1]);
        vars += Form("fAzMax=%f, ", point[3]);
        vars += Form("fRaMean=%f, ", point[4]);
        vars += Form("fDecMean=%f, ", point[5]);
    }
    vars += Form("fNight=%d, ",           MTime(tbeg.GetMjd()-1).GetNightAsInt());
    vars += Form("fRatescanID=%d, ",      d.id);
    vars += Form("fTimeBegin='%s', ",     beg.Data());
    vars += Form("fTimeEnd='%s', ",       end.Data());
    vars += Form("fRateBegin=%f, ",       d.rate[0]);
    vars += Form("fRateEnd=%f, ",         d.rate[1]);
    vars += Form("fThresholdBegin=%d, ",  d.th[0]);
    vars += Form("fThresholdEnd=%d, ",    d.th[1]);
    vars += Form("fNumPoints=%d ",       d.cnt);


    return serv.Insert("Ratescan", vars);
}

int Process(MSQLMagic &serv, TString fname)
{
    data d(fname);

    Int_t cnt = 0;

    fits file(fname.Data());
    if (!file)
        return 2;

    d.offset = file.GetUInt("MJDREF");

    bool old = file.HasColumn("Data0");

    /*  ================== That does not work, I don't know why ===================
    Double_t  time;
    ULong64_t id;
    UInt_t    th;
    Float_t   trig;
    Float_t   ontime;

    if (!file.SetPtrAddress(string("Time"), &time) ||
        !file.SetPtrAddress(string(old ? "Data0" : "Id"), &id) ||
        !file.SetPtrAddress(string(old ? "Data1" : "Threshold"), &th) ||
        !file.SetPtrAddress(string(old ? "Data4" : "TriggerRate"), &trig) ||
        !file.SetPtrAddress(string(old ? "Data3" : "RelOnTime"), &ontime))
        return -1;
        */
    Double_t  *ptime   = file.SetPtrAddress("Time");
    ULong64_t *pid     = file.SetPtrAddress(old ? "Data0" : "Id");
    UInt_t    *pth     = file.SetPtrAddress(old ? "Data1" : "Threshold");
    Float_t   *ptrig   = file.SetPtrAddress(old ? "Data4" : "TriggerRate");
    Float_t   *pontime = file.SetPtrAddress(old ? "Data3" : "RelOnTime");

    if (!ptime || !pid || !pth || !ptrig || !pontime)
        return;

    Double_t  &time   = *ptime;
    ULong64_t &id     = *pid;
    UInt_t    &th     = *pth;
    Float_t   &trig   = *ptrig;
    Float_t   &ontime = *pontime;

    while (file.GetNextRow())
    {
        if (th>3000)
            continue;

        if (id!=d.id)
        {
            if (!Fill(serv, d))
                return 0;

            d.rate[0] = ontime>0 ? trig/ontime : -1;
            d.time[0] = time;
            d.th[0]   = th;
            d.id      = id;

            d.cnt = 0;
        }

        d.rate[1] = ontime>0 ? trig/ontime : -1;
        d.time[1] = time;
        d.th[1]   = th;

        d.cnt++;
    }

    return Fill(serv, d);
}

int fillratescan(TString fname, Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "fillratescan" << endl;
    cout << "------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "File: " << fname << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    if (!Delete(serv, fname))
        return 2;

    //return Process(serv, fname);
    if (!Process(serv, fname))
        return 2;

    return 1;
}

int fillratescan(Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "fillratescan" << endl;
    cout << "------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    MDirIter Next("/fact/aux", "*RATE_SCAN_DATA.fits", 3);

    while (1)
    {
        TString name = Next();
        if (name.IsNull())
            break;

        if (name==".")
            continue;

        cout << "File: " << name << endl;
        if (!Delete(serv, name))
            return 2;
        if (!Process(serv, name))
            return 2;
    }

    return 1;
}
