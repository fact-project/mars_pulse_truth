#include <iostream>
#include <iomanip>
#include <fstream>

#include <TFile.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TPRegexp.h>

#include "MSQLMagic.h"
#include "MTime.h"
#include "MMath.h"
#include "MDirIter.h"

using namespace std;

void PrintRemarks(ofstream &fout, TString sourcename, TString firstnight, TString lastnight)
{
    MTime now(-1);

    fout << "# " << endl;
    fout << "# Please cite the FACT design paper and the QLA webpage when using these data." << endl;
    fout << "#   FACT design paper: H. Anderhub et al. JINST 8 (2013) P6008 " << endl;
    fout << "#                http://iopscience.iop.org/1748-0221/8/06/P06008 " << endl;
    fout << "#   QLA webpage: http://www.fact-project.org/monitoring" << endl;
    fout << "# If you intent to use the data, please let us know for reference. " << endl;
    fout << "# " << endl;
    fout << "# Remarks:" << endl;
    fout << "#  * These are the results of a fast quick look analyis " << endl;
    fout << "#    on site, i.e. they are preliminary. " << endl;
    fout << "#  * The quick look analysis includes all data, " << endl;
    fout << "#    i.e. no data selection done." << endl;
    fout << "#  * The given values are not fluxes but excess rates " << endl;
    fout << "#    (number of excess events per effective ontime), " << endl;
    fout << "#    i.e. there is a dependence on trigger threshold and " << endl;
    fout << "#    zenith distance of the observation (with the current " << endl;
    fout << "#    analysis for zenith distance > 40 degree and trigger " << endl;
    fout << "#    threshold > 500 DAC counts)." << endl;
//    fout << "#  * The data are provided with 20 min binning and nightly binning." << endl;
    fout << "#  * Nights with less than 20 minutes of data are neglected. " << endl;
    fout << "#  * The QLA results are not reprocessed when a new software " << endl;
    fout << "#    version is introduced. " << endl;
    fout << "#  * In case, you need further details about the data or a" << endl;
    fout << "#    different binning, please do not hesitate to contact us." << endl;
    fout << "#  * The QLA contains all data since 12.12.2012. " << endl;
    fout << "#    For older data, please contact us. " << endl;
    fout << "# " << endl;
    fout << "# Contact: Daniela Dorner dorner@astro.uni-wuerzburg.de " << endl;
    fout << "# " << endl;
    fout << "# This file was created at " << now.GetString() << endl;
    fout << "# Source: " << sourcename << endl;
    fout << "# Timerange: " << firstnight << " - " << lastnight << endl;
    fout << "# " << endl;
    fout << "# start(mjd) stop(mjd) excess-rate(evts/h) error-excess-rate " << endl;

    return;
}

int DataFromQLA(Int_t sourcekey=1, Int_t nightmin=2011115, Int_t nightmax=20161231)
{
    MSQLServer serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }
    Bool_t dch=kFALSE;

    TString query=Form("SELECT fSourceName FROM Source WHERE fSourceKey=%d", sourcekey);
    TSQLResult *res1 = serv.Query(query);
    if (!res1)
        return 1;
    TSQLRow *row1=res1->Next();
    TString sourcename=(*row1)[0];
    delete res1;
    sourcename.ReplaceAll(" ", "_");

    // datacheck
    TString datacheck=" ";
    //remove data with wrong settings
    datacheck+=" AND fNight>20120420 AND NOT fNight IN (20120406,20120410,20120503) AND";//data with different bias voltage
    datacheck+=" NOT fNight BETWEEN 20121206 AND 20130110"; // broken bias channel
    //datacheck+=" AND NOT (fNight=20120608 AND fRunID=65) "; // something went wrong with tracking?
    // 24.6. new coefficients
    TString zdparam=" pow(0.753833 * cos(Radians(fZenithDistanceMean)), 7.647435) * exp(-5.753686*pow(Radians(fZenithDistanceMean),2.089609))";
    TString thparam=" pow((if(isnull(fThresholdMinSet),fThresholdMedian,fThresholdMinSet)-329.4203),2) * (-0.0000002044803) ";
    TString param=" (fNumEvtsAfterBgCuts/5-fNumSigEvts)/fOnTimeAfterCuts - "+zdparam+" - "+thparam+" ";
    datacheck+=" AND -0.085 < ("+param+") ";
    datacheck+=" AND 0.25 > ("+param+") ";

    TString select =" SELECT Min(fNight), Max(fNight), Sum(fOnTimeAfterCuts) as bla ";
    TString fromjoinwhere=" FROM AnalysisResultsRunLP ";
    fromjoinwhere+=" LEFT JOIN RunInfo USING(fNight, fRunID) ";
    fromjoinwhere+=Form(" WHERE fSourceKey=%d", sourcekey);
    fromjoinwhere+=Form(" AND fNight BETWEEN %d AND %d ", nightmin, nightmax);
    fromjoinwhere+=" AND fOnTimeAfterCuts < 1000 "; //exclude runs with wrong/too high ontime
    fromjoinwhere+=" AND fOnTimeAfterCuts > 10 "; //exclude runs with wrong/too small ontime
    fromjoinwhere+=" AND NOT ISNULL(fNumExcEvts) ";// only where excess was extracted
    if (dch)
        fromjoinwhere+=datacheck;
    query=select+fromjoinwhere;
    query+=" GROUP BY fSourceKey ";
    query+=" HAVING bla>20*60 ";
    cout << "Q: " << query << endl;
    TSQLResult *res2 = serv.Query(query);
    if (!res2)
        return 1;
    TSQLRow *row2=res2->Next();
    TString firstnight=(*row2)[0];
    TString lastnight=(*row2)[1];
    delete res2;

    TString filename_nightly="FACT_QuickLookAnalysisResults_NightlyBinning_"+sourcename+".txt";
    TString filename_min="FACT_QuickLookAnalysisResults_20MinuteBinning_"+sourcename+".txt";

    ofstream nightly(filename_nightly);
    ofstream min(filename_min);
    if (!nightly)
    {
        cout << "ERROR - cannot write " << filename_nightly << endl;
        return 2;
    }
    if (!min)
    {
        cout << "ERROR - cannot write " << filename_min << endl;
        return 2;
    }
    PrintRemarks(nightly, sourcename, firstnight, lastnight);
    PrintRemarks(min, sourcename, firstnight, lastnight);

    // query data from AnalysisResultsRunLP
    select =" SELECT Sum(fOnTimeAfterCuts) as bla, Sum(fNumExcEvts), Sum(fNumBgEvts), Sum(fNumSigEvts), ";
    select+=" Min(fRunStart), Max(fRunStop), fNight, fRunID ";
    query=select+fromjoinwhere;
    query+=" GROUP BY fNight ";
    query+=" HAVING bla>20*60 ";
    query+=" ORDER BY fNight ";
    //cout << "Q: " << query << endl;

    //variables for calculations and graphs
    TString excevts, bgevts, sigevts, ontime, night;
    Float_t excevtssum=0;
    Float_t bgevtssum=0;
    Float_t sigevtssum=0;
    Float_t ontimesum=0;
    Float_t excerr=0;
    MTime start;
    MTime stop;
    Double_t mjdstart=0;
    Double_t mjdstop=0;
    //Double_t mjd=0;
    //Double_t mjddiff=0;
    Int_t counter=0;

    TSQLResult *res3 = serv.Query(query);
    if (!res3)
        return 1;
    TSQLRow *row3=0;
    while ((row3=res3->Next()))
    {
        ontime=(*row3)[0];
        excevts=(*row3)[1];
        bgevts=(*row3)[2];
        sigevts=(*row3)[3];
        start.SetSqlDateTime((*row3)[4]);
        stop.SetSqlDateTime((*row3)[5]);
        night=(*row3)[6];

        mjdstart = start.GetMjd();
        mjdstop = stop.GetMjd();
        //mjd = mjdstart+(mjdstop-mjdstart)/2.;

        //significance = MMath::SignificanceLiMaSigned(sigevts.Atof(), bgevts.Atof()*5, 0.2);
        excerr = MMath::ErrorExc(sigevts.Atof(), bgevts.Atof()*5, 0.2)/ontime.Atof()*3600.;
        nightly.precision(18);
        //fout << (*row3)[6] << ": " << start.GetMjd() << " " << stop.GetMjd() << " " << excevts.Atof()/ontime.Atof() << " " << excerr << endl;
        nightly << start.GetMjd() << " " << stop.GetMjd() << " " << excevts.Atof()/ontime.Atof()*3600 << " " << excerr << endl;
        query="SELECT fOnTimeAfterCuts, fNumExcEvts, fNumBgEvts, fNumSigEvts, ";
        query+=" fRunStart, fRunStop "+fromjoinwhere;
        query+=" AND fNight="+night;
        //cout << "Q: " << query << endl;
        TSQLResult *res4 = serv.Query(query);
        if (!res4)
            return 1;
        TSQLRow *row4=0;
        counter=0;
        while ((row4=res4->Next()))
        {
            ontime=(*row4)[0];
            excevts=(*row4)[1];
            bgevts=(*row4)[2];
            sigevts=(*row4)[3];
            if(counter==0)
                start.SetSqlDateTime((*row4)[4]);

            if (ontimesum+ontime.Atoi()>20*60)
            {
                excerr = MMath::ErrorExc(sigevtssum, bgevtssum*5, 0.2)/ontimesum*3600.;
                min.precision(18);
                min << start.GetMjd() << " " << stop.GetMjd() << " " << excevtssum/ontimesum*3600 << " " << excerr << endl;
                start.SetSqlDateTime((*row4)[4]);
                ontimesum=0;
                excevtssum=0;
                bgevtssum=0;
                sigevtssum=0;
            }
            counter++;
            stop.SetSqlDateTime((*row4)[5]);
            ontimesum+=ontime.Atoi();
            excevtssum+=excevts.Atof();
            bgevtssum+=bgevts.Atof();
            sigevtssum+=sigevts.Atof();
        }
    }

    delete res3;
    return 0;

}



