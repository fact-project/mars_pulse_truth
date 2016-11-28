void ontime(const char *fname, double beg=0, double end=100000)
{
    //open the file
    fits file(fname);

    //file.PrintColumns();
    //file.PrintKeys();

    //allocate variables to read data into
    Double_t time;
    Float_t OnTime, ElTime, TriggerRate;
    file.SetPtrAddress("Time",  &time);
    file.SetPtrAddress("OnTime", &OnTime);
    file.SetPtrAddress("ElapsedTime", &ElTime);
    file.SetPtrAddress("TriggerRate", &TriggerRate);

    //some file MJDREF offset fun. Before a given day, the MJDREF was not subtracted correctly.
    //Also, see the logbook for more details (there are also some broken files)
    UInt_t offset = file.GetUInt("MJDREF");
    if (beg < 30000)
        beg+=offset;
    if (end < 30000)
        end+=offset;

    //variables to calculate stuff with
    double  SumOnTime           = 0.;   //Sum up the OnTime_i for later effective on calculation
    double  SumElTime           = 0.;   //sum up ElTime_i for later effective on calculation

    double  SumOnEl             = 0.;   //Sum of the OnTime_i / ElapsedTime_i
    double  SumOnEl2            = 0.;   //Sum of the (OnTime_i / ElapsedTime_i)^2,
                                        //both values needed for variance of the effective on timeseries

    double  EffectiveOn         = 0.;   //this will be the one effective on value
    double  EffectiveOnRMS      = 0.;   //this will be variance(effective on)
    int     counter             = 0;    //cnt the number of non zero ElTime datapoints

    double  TriggerTimeOver100  = 0.;   //this will be the time  in sec the Trigger
    double  TriggerTimeOver125  = 0.;   //is above 100Hz,125Hz,...
    double  TriggerTimeOver150  = 0.;
    double  TriggerTimeOver175  = 0.;

    double  SumRate             = 0.;   //for calculating the sqrt(var) of the trigger rate
    double  SumRate2            = 0.;   //same
    double  TriggerRateRms      = 0.;
    int     ratecounter         = 0;

    //loop over all rows in slow-file
    while (file.GetNextRow())
    {
        //add the offset, as the data is saved with the offset subtracted
        time += offset;

        //if the end of the run is reached, break
        if (time>end)
            break;

        //if the data is before the beginning of the run, continue
        if (time<beg)
        {
            continue;
        }

        //sum the data for the mean effective on
        SumOnTime   += OnTime;
        SumElTime   += ElTime;

        //sum the data for the variance of the effective on timeseries, check for zero elapsed time
        if (ElTime>0){
            SumOnEl     += OnTime/ElTime;
            SumOnEl2    += OnTime/ElTime*OnTime/ElTime;
            counter++;
            //cout << "OnTime:" << OnTime << " # ElTime:" << ElTime << " # OnTime/ElTime:" << OnTime/ElTime << " # (OnTime/ElTime)^2:" << OnTime/ElTime*OnTime/ElTime << " # Counter:" << counter << endl;
        }

        //to calculate the sqrt(var) of the rate
        SumRate     +=TriggerRate;
        SumRate2    +=TriggerRate*TriggerRate;
        ratecounter +=1;

        //sum the time the triggerate is over 100Hz, 125Hz,...
        if (TriggerRate>100.){
            TriggerTimeOver100 += ElTime;

            if (TriggerRate>125.){
                TriggerTimeOver125 += ElTime;

                if (TriggerRate>1500.){
                    TriggerTimeOver150 += ElTime;

                    if (TriggerRate>175.){
                        TriggerTimeOver175 += ElTime;

                    }
                }
            }
        }

    }

    //cout << "SumOnEl2:" << SumOnEl2 << " # SumOnEl:" << SumOnEl << " # (SumOnEl2/counter-(SumOnEl/counter*SumOnEl/counter)): " << (SumOnEl2/counter-(SumOnEl/counter*SumOnEl/counter)) << endl;

    //calculate the return values, watch out for traps when doing math operations!
    EffectiveOn         = SumElTime>0 ? SumOnTime/SumElTime : 0.;
    EffectiveOnRMS      = counter>0 ? (SumOnEl2/counter-
                                       (SumOnEl/counter*SumOnEl/counter)) : 0.;
    EffectiveOnRMS      = EffectiveOnRMS>0 ? sqrt(EffectiveOnRMS) : 0;
    TriggerRateRms      = ratecounter>0? (SumRate2/ratecounter-
                                          (SumRate/ratecounter*SumRate/ratecounter)) : 0.;
    TriggerRateRms      = TriggerRateRms>0 ? sqrt(TriggerRateRms) : 0;

    //check for crap data, which happenes sometimes...
    if (EffectiveOn>1.) EffectiveOn = 1.;
    if (EffectiveOnRMS>0.5) EffectiveOnRMS = 0.5;
    if (TriggerRateRms>1000) TriggerRateRms = 0.;

    //return fun. The style from daniela (grep the output) requires an non scientific notation,
    //i.e. withour exponential+ therefore set the precision to ~double with fixed width after decimal point
    cout << setiosflags(ios::fixed) << setprecision(12)
         << "result "
         << EffectiveOn         << " "
         << EffectiveOnRMS      << " "
         << SumOnTime           << " "
         << TriggerTimeOver100  << " "
         << TriggerTimeOver125  << " "
         << TriggerTimeOver150  << " "
         << TriggerTimeOver175  << " "
         << TriggerRateRms  << " "
         << endl;
}
