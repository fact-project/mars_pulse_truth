// Be aware that this macro just inserts and not updates the rows

void GetObsTimes(Bool_t dummy=kTRUE, Int_t start=55866 /*1.11.2011*/, Int_t stop=58848 /*31.12.2019*/)
{
    MSQLMagic serv("sql.rc");
    Bool_t con = serv.IsConnected();
    if (con)
    {
        cout << "Connected to " << serv.GetName() << endl;
        cout << endl;
    }
    else
    {
        cout << "Could not connect to " << serv.GetName() << endl;
        cout << endl;
        return;
    }


    MTime time(-1);
    MTime time2, time3, time4, time5;
    MTime time6, time7, time8, time9;
    Double_t darknight=0;
    Int_t night=0;
    TString query;
    for (int i=start; i<=stop; i++)
    {
        //GetSolarRst returns the information for the next day
        //  as JD seems to be rounded to the next value in all 
        //  cases in the libnova function ln_get_solar_rst
        //to get the correct values the following would be needed
        //Nova::RstTime t1= Nova::GetSolarRst(time.GetJD()-1, -12);
        //Nova::RstTime t2= Nova::GetSolarRst(time.GetJD(), -12);
        //time2.SetJD(t1.set);
        //time3.SetJD(t2.rise);
        //however, as time.GetNightAsInt() returns the date of the
        //  sunrise (MAGIC naming convention), we use:
        time.SetMjd(i);
        Nova::RstTime t1= Nova::GetSolarRst(time.GetJD(), -12);
        Nova::RstTime t2= Nova::GetSolarRst(time.GetJD()+1, -12);
        Nova::RstTime t3= Nova::GetSolarRst(time.GetJD(), -18);
        Nova::RstTime t4= Nova::GetSolarRst(time.GetJD()+1, -18);
        Nova::RstTime t5= Nova::GetLunarRst(time.GetJD());
        Nova::RstTime t6= Nova::GetLunarRst(time.GetJD()+1);

        /*
        cout << time.GetNightAsInt()
//            << " " << MTime(t1.set).GetSqlDateTime()
//            << " " << MTime(t2.rise).GetSqlDateTime()
//            << " s" << MTime(t3.set).GetSqlDateTime()
//            << " r" << MTime(t4.rise).GetSqlDateTime()
            << " s" << MTime(t5.set).GetSqlDateTime()
            << " r" << MTime(t5.rise).GetSqlDateTime()
            << " s" << MTime(t6.set).GetSqlDateTime()
            << " r" << MTime(t6.rise).GetSqlDateTime()
            << endl;
        */

        // astronomical twilight (-12 deg)
        time2.SetJD(t1.set);
        time3.SetJD(t2.rise);
        // dark night (-18 deg)
        time4.SetJD(t3.set);
        time5.SetJD(t4.rise);
        // setting rise (time6) and set (time7) of moon
        if (t5.set<t5.rise)
        {
            time6.SetJD(t5.rise);
            time7.SetJD(t6.set);
        }
        else//set>rise
        {
            if (t5.set>t3.set)
                time7.SetJD(t5.set);
            else
                time7.SetJD(t6.set);

            time6.SetJD(t6.rise);
        }
        // setting times for dark night
        // sunset after moonrise
        if (time4>time6)
            time8=time7;
        else
        {
            if (time4<time7 && time5>time7)
                time8=time7;
            else
                time8=time4;
        }
        // moonset before sunrise
        if (time7<time5)
            time9=time5;
        else
        {
            // sunrise before moonrise
            if (time5<time6)
                time9=time5;
            else
                time9=time6;
        }
        // number of dark hours
        darknight=time9<time8 ? 0 : (time9.GetMjd()-time8.GetMjd())*24;

        /*
        cout << time.GetNightAsInt() << " "
            << " ==> r" << time6.GetSqlDateTime()
            << " s" << time7.GetSqlDateTime()
            << " ==> start" << time8.GetSqlDateTime()
            << " stop" << time9.GetSqlDateTime()
            << " dark: " << darknight << "h "
            << endl;
        */

        query=Form("INSERT ObservationTimes SET fNight=%d, "
                   "fStartObservation='%s', fStopObservation='%s', "
                   "fStartDarkTime='%s', fStopDarkTime='%s', "
                   "fStartDarkNight='%s', fStopDarkNight='%s', "
                   "fMoonRise='%s', fMoonSet='%s', "
                   "fNumDarkHours=%f, fMjd=%d",
                   time.GetNightAsInt(),
                   time2.GetSqlDateTime().Data(), time3.GetSqlDateTime().Data(),// astronomical twilight
                   time4.GetSqlDateTime().Data(), time5.GetSqlDateTime().Data(),// darktime
                   time8.GetSqlDateTime().Data(), time9.GetSqlDateTime().Data(),// darknight
                   time6.GetSqlDateTime().Data(), time7.GetSqlDateTime().Data(),// moon
                   darknight, i); //dark hours

        cout << "QUERY: " << query << endl;

        if (dummy)
            continue;
        TSQLResult *res = serv.Query(query);
        if (!res)
        {
            cout << "problem to insert. " << endl;
            return;
        }
    }
    return;

}
