

void currents(const char *fname, double beg=0, double end=100000)
{
    fits file(fname);

    //file.PrintColumns();
    //file.PrintKeys();

    Double_t time;
    Double_t jd;

    Float_t med, dev;
    file.SetPtrAddress("Time",  &time);
    file.SetPtrAddress("I_med", &med);
    file.SetPtrAddress("I_dev", &dev);

    UInt_t offset = file.GetUInt("MJDREF");
    if (beg < 30000)
        beg+=offset;
    if (end < 30000)
        end+=offset;

    double med_beg  = 0;
    double med_end  = 0;
    double med_avg  = 0;
    double med_rms  = 0;
    double dev_avg  = 0;
    double dev_rms  = 0;
    int    cnt_beg  = 0;
    int    cnt_end  = 0;

    double med_last = -1;
    double dev_last = -1;
    double diff = -1;

    double calc_dev_avg  = 0;
    double calc_dev_rel  = 0;

    TGraph g;

    while (file.GetNextRow())
    {
        time += offset;
        jd    = time + 2400000.5;

        if (time>end)
            break;

        if (time<beg)
        {
            med_last = med;
            dev_last = dev;
            diff = beg-time;
            continue;
        }

        med_avg += med;
        med_rms += med*med;
        dev_avg += dev;
        dev_rms += dev*dev;

        if (time<beg+15./24/3600)
        {
            med_beg += med;
            cnt_beg++;
        }
        if (time>end-15./24/3600)
        {
            med_end += med;
            cnt_end++;
        }

        //calculate the moon
        Nova::EquPosn moon = Nova::GetLunarEquCoords(jd, 0.01);
        // get local position of moon
        Nova::HrzPosn hrzm = Nova::GetHrzFromEqu(moon, jd);

        //calculate the light condition
        double disk = Nova::GetLunarDisk(jd);

        // Current prediction
        //double cang = sin(angle);
        const double calt = sin(hrzm.alt*TMath::DegToRad());

        //                                                                           semi-major axis
        const double lc = /*cang==0 ? 0 :*/ calt*pow(disk, 2.2)*pow(Nova::GetLunarEarthDist(jd)/384400, -2);///sqrt(cang);
        //const double lc = cang==0 ? -1 : calt*pow(disk, 2.2)*pos(dist, -2);
        const double Ical = lc>0 ? 4+103*lc : 4;

        g.SetPoint(g.GetN(), time, med);

        calc_dev_avg += med - Ical;
        calc_dev_rel += med/Ical;

    }

    if (g.GetN()==0)
    {
        if (diff<5./24/3600)
            return;

        // return the last report before the run
        //   (if not older than 5 minutes)
        cout << "result " << med_last << " 0 " << dev_last << " 0 0 0 0" << endl;
        return;
    }

    Double_t a0, a1;
    Int_t ifail;
    g.LeastSquareLinearFit(0, a0, a1, ifail);

    double fluct_abs = 0;
    double fluct_rel = 0;
    if (ifail==0)
    {
        for (int i=0; i<g.GetN(); i++)
            g.GetY()[i] -= a0+a1*g.GetX()[i];
        fluct_abs = g.GetRMS(2);

        for (int i=0; i<g.GetN(); i++)
            g.GetY()[i] /= a0+a1*g.GetX()[i];
        fluct_rel = g.GetRMS(2);
    }

    med_beg = cnt_beg>0 ? med_beg/cnt_beg : -1;
    med_end = cnt_end>0 ? med_end/cnt_end : -1;

    med_avg /= g.GetN();
    med_rms /= g.GetN();
    med_rms = sqrt(med_rms-med_avg*med_avg);

    dev_avg /= g.GetN();
    dev_rms /= g.GetN();
    dev_rms = sqrt(dev_rms-dev_avg*dev_avg);
    calc_dev_avg /= g.GetN();

    calc_dev_rel -= g.GetN();
    calc_dev_rel /= g.GetN();


    cout << "result " << med_avg << " " << med_rms << " " << dev_avg << " " << dev_rms << " " << med_beg << " " << med_end << " " << calc_dev_avg << " " << calc_dev_rel << " " << fluct_abs << " " << fluct_rel << endl;
}
