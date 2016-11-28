#include <iostream>

#include <TCanvas.h>
#include <TString.h>
#include <TTree.h>
#include <TFile.h>
#include <TSystem.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TPRegexp.h>

#include "MSQLMagic.h"
#include "MAlphaFitter.h"
#include "MHThetaSq.h"
#include "MStatusArray.h"


using namespace std;

int process(MSQLMagic &serv, TString gfile, TString sfile, TString night2, TString runid, TString table, Bool_t pernight, Int_t source)
{
    TString where;
    TString vars;
    //the variables to save into the db
    Int_t fNumEvtsAfterCleaning = 0;
    Int_t fNumEvtsAfterQualCuts = 0;
    Int_t fNumEvtsAfterBgCuts   = 0;
    Float_t fNumBgEvts          = 0;
    Float_t fNumSigEvts         = 0;
    Float_t fNumExcEvts         = 0;
    Float_t fNumIslandsMean     = 0;
    Float_t fOnTimeAfterCuts    = 0;

    if (!pernight)
    {
        //check star file status
        if (gSystem->AccessPathName(sfile))
        {
            cout << "ERROR - file " << sfile << " does not exist." << endl;
            return 2;
        }

        //try to open the star file
        TFile star_file(sfile, "READ");
        if (!star_file.IsOpen())
        {
            cout << "ERROR - Could not open file " << sfile << endl;
            return 2;
        }

        //Get the events tree
        TTree *star_tree = (TTree *)star_file.Get("Events");
        if (!star_tree)
        {
            cout << "ERROR - Could not read tree " << sfile << endl;
            return 2;
        }

        //the number of events after cleaning
        fNumEvtsAfterCleaning = star_tree->GetEntries();

        //get the mean number of islands in the _I.root file on runbasis
        star_tree->Draw("MImagePar.fNumIslands>>IslandHisto","","");
        TH1F *HistJo = (TH1F*)gDirectory->Get("IslandHisto");
        if (!HistJo)
        {
            cout << "ERROR - Reading of IslandHisto failed " << HistJo << endl;
            return 2;
        }
        fNumIslandsMean = HistJo->GetMean();
    }

    //check ganymed file status
    if (gSystem->AccessPathName(gfile))
    {
        cout << "ERROR - file " << gfile << " does not exist." << endl;
        return 2;
    }

    //try to open the ganymed file
    TFile ganymed_file(gfile, "READ");
    if (!ganymed_file.IsOpen())
    {
        cout << "ERROR - Could not open file " << gfile << endl;
        return 2;
    }

    //Get the status display contents
    MStatusArray arr;
    if (arr.Read()<=0)
    {
        cout << "ERROR - could not read MStatusArray, file " << gfile << endl;
        return 2;
    }

    TH1D *vstime = (TH1D*)arr.FindObjectInCanvas("OnTime",  "TH1D", "OnTime");
    if (!vstime)
    {
        cout << "WARNING - Reading of Theta failed." << endl;
        return 2;
    }
    fOnTimeAfterCuts = vstime->Integral();

    //Get number of events after quality cuts
    //from the number of entries in a histogram
    TH1F *precut =
        (TH1F*)arr.FindCanvas("PreCut")->FindObject("SizeSame");
    if (!precut)
    {
        cout << "WARNING - Reading of PreCut canvas failed." << endl;
        return 2;
    }
    fNumEvtsAfterQualCuts = precut->GetEntries();

    //Get number of events after background rejection cuts
    //from the number of entries in a histogram
    TH1F *postcut =
        (TH1F*)arr.FindCanvas("PostCut")->FindObject("SizeSame");
    if (!postcut)
    {
        cout << "WARNING - Reading of PostCut canvas failed." << endl;
        return 2;
    }
    fNumEvtsAfterBgCuts = postcut->GetEntries();

    //Get signal, bg and excess events
    MHThetaSq *halphaon = (MHThetaSq*)arr.FindObjectInCanvas("MHThetaSq", "Hist");
    if (!halphaon)
    {
        cout << "WARNING - Reading of MHThetaSq failed, file " << gfile << endl;
        return 2;
    }
    const MAlphaFitter &fit = halphaon->GetAlphaFitter();
    if (!&fit)
    {
        cout << "WARNING - Reading of MAlphaFitter failed, file " << gfile << endl;
        return 2;
    }

    //check if the excess events number is small and close to 0
    //due to the computer precision error when subtracting sig and bg events
    fNumBgEvts  = fit.GetEventsBackground();
    fNumSigEvts = fit.GetEventsSignal();
    fNumExcEvts = fit.GetEventsExcess();
    fNumExcEvts = fabs(fNumExcEvts)<1e-5 ? 0 : fNumExcEvts;

    /*
     cout<< fNumEvtsAfterCleaning<< "    "
     << fNumEvtsAfterQualCuts<< "    "
     << fNumEvtsAfterBgCuts  << "    "
     << fNumBgEvts           << "    "
     << fNumSigEvts          << "    "
     << fNumExcEvts          << endl;
     */

    //inserting or updating the information in the database
    if (!pernight)
        vars = Form("fRunID=%s, fNight=%s,", runid.Data(), night2.Data());
    else
        vars = Form("fNight=%s, fSourceKey=%d, ", night2.Data(), source);
    vars += Form(" fNumEvtsAfterQualCuts=%d, "
                 " fNumEvtsAfterBgCuts=%d, "
                 " fNumBgEvts=%6.1f, "
                 " fNumSigEvts=%6.1f, "
                 " fNumExcEvts=%6.1f, "
                 " fOnTimeAfterCuts=%7.2f ",
                 fNumEvtsAfterQualCuts,
                 fNumEvtsAfterBgCuts,
                 fNumBgEvts,
                 fNumSigEvts,
                 fNumExcEvts,
                 fOnTimeAfterCuts
                );
    if (!pernight)
        vars += Form(", fNumIslandsMean=%6.2f, fNumEvtsAfterCleaning=%d ",
                     fNumIslandsMean,
                     fNumEvtsAfterCleaning
                    );


    if (pernight)
        where = Form("fNight=%s AND fSourceKey=%d", night2.Data(), source);
    else
        where = Form("fRunID=%s AND fNight=%s", runid.Data(), night2.Data());

    cout << "vars: " << vars << endl;
    cout << "where: " << where << endl;

    if (!serv.InsertUpdate(table, vars, where))
    {
        cout << "ERROR - insert/update to DB failed  (vars:" << vars << " where: " << where << ")." << endl;
        return 2;
    }
    return 1;
}


int numevents(Int_t night, TString inpath="/loc_data/analysis/", TString table= "AnalysisResultsRunLP", Bool_t dummy=kTRUE, Bool_t pernight=kFALSE, Int_t source=0)
{

    //connect to mysql server
    //MSQLServer serv("sql.rc");
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }
    serv.SetIsDummy(dummy);

    //select runs, where star finished, format the list nicely
    TString query;
    query = " SELECT CONCAT(fNight, '_', LPAD(fRunID, 3, 0)), SUBSTRING(fNight, 1,4), SUBSTRING(fNight, 5,2), SUBSTRING(fNight, 7,2), ";
    query += " fNight, fRunID FROM RunInfo";

    //and only data runs
    query+=" WHERE fRunTypeKEY=1 ";
    query+=Form(" AND fNight=%d",night);
    //query+=" AND fRunID=128 ";
    if (pernight)
        query+=" GROUP BY fNight";

    cout << "Q: " << query << endl;

    //post the query
    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "ERROR - no result from query " << query << endl;
        return 2;
    }

    //allocate variables to use in the loop
    TString night2;
    TString runid;
    TString run;
    TString year;
    TString month;
    TString day;
    TString ganymed_fname;
    TString star_fname;
    Int_t rc=0;

    //loop over the data files
    TSQLRow *row=0;

    //cout the data names
    cout<<"#evts_after_cleaning" << "    "
        <<"evts_after_qual-cuts" << "    "
        <<"evts_after_bg-cuts" << "    "
        <<"mean_evts_from_off_regions"<< "    "
        <<"evts_from_on_region"  << "    "
        <<"calculated_excess_evts_in_file" << "    " << endl;

    while ((row=res->Next()))
    {
        //use the results from the query
        night2=(*row)[4];
        runid=(*row)[5];
        run=(*row)[0];
        year=(*row)[1];
        month=(*row)[2];
        day=(*row)[3];

        // to be fixed: path for nightly ganymeds
        if (!pernight)
            ganymed_fname=inpath+"/ganymed_run/"+year+"/"+month+"/"+day+"/"+run+"-analysis.root";
        else
            if (!source)
                cout << "ERROR - source " << source << " not valid. " << endl;
            else
                ganymed_fname=Form("%s/ganymed_night/%d/%s-analysis.root", inpath.Data(), source, night2.Data());
        cout << "gf => " << ganymed_fname << endl;

        if (!pernight)
        {
            star_fname=inpath+"/star/"+year+"/"+month+"/"+day+"/"+run+"_I.root";
            cout << "sf => " << star_fname << endl;
        }
        rc=process(serv, ganymed_fname, star_fname, night2, runid, table, pernight, source);
        if (rc>1)
            return rc;

    }


    return 1;
}

int numevents(TString ganymedfile, TString starfile, TString table, Bool_t dummy=kTRUE)
{

    //connect to mysql server
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }
    serv.SetIsDummy(dummy);
    //get night and run from file name
    TString basename=gSystem->BaseName(ganymedfile);
    TPRegexp regexp("20[0-9][0-9][0-2][0-9][0-3][0-9]_[0-9][0-9][0-9]");
    TString run = basename(regexp);
    TPRegexp regexp2("20[0-9][0-9][0-2][0-9][0-3][0-9]");
    TPRegexp regexp3("_[0-9][0-9][0-9]");
    TPRegexp regexp4("[0-9][0-9][0-9]");
    TString night = run(regexp2);
    TString runnum = run(regexp3);
    TString runid = runnum(regexp4);

    return process(serv, ganymedfile, starfile, night, runid, table, false, 0);
}

