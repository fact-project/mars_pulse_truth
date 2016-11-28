/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz, 08/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniela Dorner, 08/2004 <mailto:dorner@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// buildsequenceentries.C
// ======================
//
// to group the runs of one night into sequences, this marco:
//  - reads the runinformation of one night from the database
//  - group the runs into sets of following runs with the same conditions
//  - groups the runs in this sets to sequences such that each run belongs
//    to the nearest (in time) calibration run
//  - check if the runs with the same runtype have the same calibration script
//    and the same trigger tables
//  if sequence is okay:
//  - check if in the range of the runnumbers of this sequence other sequences
//    exist in the database
//  if there are no sequences, insert the new sequence, else:
//  - delete overlaping sequences
//  if there's only one sequence in the same runnumber range:
//  - check if the new and the old sequence are identical
//    if they are identical, do nothing, if not, delete the old sequence and
//    insert the new one
//
// remark: deleting sequences includes the following steps:
// - delete entries from the tables Sequences, SequenceProcessStatus,
//   Calibration and Star
// - updating the sequence number (fSequenceFirst) in the table RunData
// - remove the Sequence File, the calibrated data and the image files from
//   the disk
//
// the macro can be executed either for all nights or for one single night
// .x buildsequenceentries.C+( "datapath", "sequpath", Bool_t dummy=kTRUE)
// .x buildsequenceentries.C+( "night", "datapath", "sequpath")
//
// the Bool_t dummy:
//  kTRUE:  dummy-mode, i.e. nothing is inserted into the database, but the
//          commands, that would be executed are returned
//  kFALSE: the information is inserted into the database and the files of
//          removed sequences is deleted
// be careful with this option - for tests use always kTRUE
//
// TString datapath, TString sequpath:
//  datapath: path, where the processed data is stored in the datacenter
//  sequpath: path, where the sequence files are stored in the datacenter
// the datapath (standard: /magic/data/) and the sequencepath (standard:
// /magic/sequences) have to be given, that the sequence file, the
// calibrated data and the star files can be removed, when an old sequence
// has to be removed from the database
//
// If nothing failes 1 is returned. In the case of an error 2 and if
// there's no connection to the database 0 is returned.
// This is needed for the scripts that execute the macro.
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <fstream>

#include <map>
#include <utility>

#include <errno.h>

#include <TSQLRow.h>
#include <TSQLResult.h>

#include <TEnv.h>
#include <TMap.h>
#include <TMath.h>
#include <TExMap.h>
#include <TArrayI.h>
#include <TArrayL.h>
#include <TArrayD.h>
#include <TPRegexp.h>
#include <TSystem.h>
#include <TObjString.h>
#include <TObjArray.h>

#include "MTime.h"
#include "MSequence.h"

#include "MSQLMagic.h"

using namespace std;

    /*static*/ ULong_t GetId(const TString str)
    {
        const Ssiz_t dot = str.First('.');

        const UInt_t run = str.Atoi();
        const UInt_t sub = dot<0 ? 0 : atoi(str.Data()+dot+1);

        return ULong_t(run)*1000+sub;
    }


class Rule : public TObject
{
private:
    TPRegexp fRegexp;

    ULong_t fMin;
    ULong_t fMax;

public:
    Rule(TObjArray &arr) :
        fRegexp(arr.GetEntries()>0?Form("^%s", arr[0]->GetName()):""),
            fMin(0), fMax((ULong_t)-1)
    {
        if (arr.GetEntries()>1)
            fMin = GetId(arr[1]->GetName());
        if (arr.GetEntries()>2)
            fMax = GetId(arr[2]->GetName());
        cout << "----> " << fMin << " " << fMax << endl;
    }

    Ssiz_t Match(const TString &str, Int_t idx, Long_t run=-1)
    {
        if (!IsValid(run))
            return 0;

        TString mods;
        TArrayI pos;
        fRegexp.Match(str.Data()+idx, mods, 0, str.Length()*10, &pos);

        return pos.GetSize()<2 ? 0 : pos[1]-pos[0];
    }

    Bool_t IsValid(ULong_t run) const { return run>=fMin && run<=fMax; }
    ClassDef(Rule, 0)
};
ClassImp(Rule);

class CheckMatch : public TObject
{
private:
    TPRegexp fRunType1;
    TPRegexp fRunType2;

    TPRegexp fRegexp1;
    TPRegexp fRegexp2;

    ULong_t fMin;
    ULong_t fMax;

    void Init(const TObjArray &arr, Int_t idx=0)
    {
        const Int_t n = arr.GetEntries();

        const Bool_t isminus = n>idx && TString(arr[idx]->GetName())=="-";

        for (int i=0; i<n-idx; i++)
        {
            //cout << arr[i+idx]->GetName() << " ";

            TString str(arr[i+idx]->GetName());
            if (str=="*")
                str = ".*";

            switch (isminus && i>1 ? i+1 : i)
            {
            case 0: fRunType1 = TPRegexp(Form(isminus?"-":"^%s$", str.Data())); break;
            case 1: fRunType2 = TPRegexp(Form("^%s$", str.Data())); break;
            case 2: fRegexp1  = TPRegexp(Form("^%s$", str.Data())); break;
            case 3: fRegexp2  = TPRegexp(Form("^%s$", str.Data())); break;
            case 4: fMin      = GetId(str);               break;
            case 5: fMax      = GetId(str);               break;
            }
        }
    }

public:
    CheckMatch() : fRunType1(""), fRunType2(""), fRegexp1(""), fRegexp2("") {}

    CheckMatch(const TString &txt) : fRunType1(""), fRunType2(""), fRegexp1(""), fRegexp2(""), fMin(0), fMax((ULong_t)-1)
    {
        TObjArray *arr = txt.Tokenize(" ");
        Init(*arr);
        delete arr;
    }

    CheckMatch(TObjArray &arr, Int_t idx=0) : fRunType1(""), fRunType2(""), fRegexp1(""), fRegexp2(""), fMin(0), fMax((ULong_t)-1)
    {
        Init(arr, idx);
    }

    CheckMatch(const char *from, const char *to, Long_t min=0, Long_t max=-1)
        : fRunType1(".*"), fRunType2(".*"), fRegexp1(Form("^%s$", from)), fRegexp2(Form("^%s$", to)), fMin(min), fMax(max)
    {
    }

    Int_t Matches(const TString &rt1, const TString &rt2, const TString &lc1, const TString &lc2, ULong_t run=0)
    {
        if (run>0)
        {
            cout << " - - > " << run << endl;
            if (run<fMin || run>fMax)
                return kFALSE;
        }

        const TString test("X-"); // FIXME:STUPID!

        //if (test.Index(fRunType2)==1)
         //   return -(!rt1(fRunType1).IsNull() && !lc1(fRegexp1).IsNull());

        if (test.Index(fRunType1,0)==1)
            return -(!rt2(fRunType2).IsNull() && !lc2(fRegexp2).IsNull());

        return !rt1(fRunType1).IsNull() && !rt2(fRunType2).IsNull() && !lc1(fRegexp1).IsNull() && !lc2(fRegexp2).IsNull();
    }
    ClassDef(CheckMatch,0)
};
ClassImp(CheckMatch);

class CheckList : public TList
{
public:
    CheckList() { SetOwner(); }
    Int_t Matches(const TString &rt1, const TString &rt2, const TString &lc1, const TString &lc2, Long_t run=-1) const
    {
        TIter Next(this);

        CheckMatch *check = 0;

        while ((check=(CheckMatch*)Next()))
        {
            const Int_t rc = check->Matches(rt1, rt2, lc1, lc2, run);
            if (rc)
                return rc;
        }

        return kFALSE;
    }
    ClassDef(CheckList,0)
};
ClassImp(CheckList);

class SequenceBuild : public MSQLMagic
{
private:
    TString fPathRawData;
    TString fPathSequences;

    Int_t fTelescopeNumber;

    TMap  fMap;
    TList fListRegexp;

    Int_t CheckTransition(TSQLResult &res, const TString *keys, TSQLRow &row, Int_t i)
    {
        // Get the name of the column from result table
        const TString key = res.GetFieldName(i);

        // Get the list with the allowed attributed from the map
        CheckList *list = dynamic_cast<CheckList*>(fMap.GetValue(key));
        if (!list)
            return kFALSE;

        // Check whether the current run (row[0]) with the current attribute
        // (row[i]) and the current run-type (row[1]) matches the attribute
        // of the last run (keys[i] with run-type row[i])
        return list->Matches(keys[1], row[1], keys[i], row[i], atol(row[0]));
    }

    Bool_t InsertSequence(std::pair<Long_t,Long_t> drs, Long_t from, Long_t to, const TString &cond)
    {
        //cout << "     - Inserting Sequence into database." << endl;

        TString query;
        query  = "SELECT fNight, STR_TO_DATE(fNight,'%Y%m%d') ";
        query += " FROM RunInfo WHERE ";
        query += cond;
        query += Form(" AND fRunID=%ld", from);

        TSQLResult *res = Query(query);
        if (!res)
            return kFALSE;

        if (res->GetFieldCount()!=2 || res->GetRowCount()!=1)
        {
            cout << "ERROR - Wrong result from query: " << query << endl;
            return kFALSE;
        }

        TSQLRow *row = res->Next();

        UInt_t  run = atoi((*row)[0]);
        TString day = (*row)[1];

        delete res;

        query  = "SELECT fRunID, fRunTypeKEY, fHasDrsFile, fDrsStep ";
        query += " FROM RunInfo WHERE ";
        query += cond;
        query += Form(" AND fRunID BETWEEN %ld AND %ld ORDER BY fRunID", from, to);

        if (!(res = Query(query)))
            return kFALSE;

        if (res->GetFieldCount()!=4)
        {
            cout << "ERROR - Wrong result from query: " << query << endl;
            return kFALSE;
        }

        MSequence seq("sequence.def", "", (run%1000000)*1000+from);
        seq.SetNight(day);

        if (drs.first!=from)
        {
            seq.SetDrsSequence(drs.first);
            if (drs.second!=-1)
                seq.AddFile(drs.second, -1, MSequence::kDrsFile);
        }

        while ((row=res->Next()))
        {
            UInt_t num = atol((*row)[0]);
            UInt_t key = atol((*row)[1]);

            switch (key)
            {
            case 1: seq.AddFile(num, -1, MSequence::kDat);    break;
            case 2: seq.AddFile(num, -1, MSequence::kPed);    break;
            case 3: seq.AddFile(num, -1, MSequence::kDrsRun); break;
            case 4: seq.AddFile(num, -1, MSequence::kCal);    break;
            case 5: seq.AddFile(num, -1, MSequence::kDat);    break;
            case 6: seq.AddFile(num, -1, MSequence::kCal);    break;
            }

            if (atoi((*row)[2])==1)
                if (!(*row)[3] || atoi((*row)[3])==2)
                    seq.AddFile(num, -1, MSequence::kDrsFile);
        }

        const TString path =
            Form("%s/%04d/%02d/%02d", fPathSequences.Data(),
                 run/10000, (run/100)%100, run%100);

        const TString fname =
            Form("%s/%d_%03ld.%s", path.Data(), run, from,
                 drs.first==from?"drs.seq":"seq");

        if (IsDummy())
        {
            cout << "----- " << fname << " -----" << endl;
            seq.Print();
            cout << "^^^^^ " << fname << " ^^^^^" << endl;
        }
        else
        {
            if (gSystem->AccessPathName(path, kFileExists))
                if (gSystem->mkdir(path, kTRUE)<0)
                    cout << "ERROR - Could not create " << path << endl;

            if (!gSystem->AccessPathName(path, kFileExists))
                seq.WriteFile(fname);
        }

        delete res;

        if (drs.first==from)
            return kTRUE;

        // ========== Request number of events ==========

        // Can be replaced by

        const TString runtime =
            "SUM(TIME_TO_SEC(TIMEDIFF(fRunStop,fRunStart))), ";

        const TString where = Form("(fRunID BETWEEN %ld AND %ld) AND fExcludedFDAKEY=1 AND %s",
                                   from, to, cond.Data());

        query  = "SELECT SUM(fNumEvents), ";
        query += runtime;
        query += " MIN(fZenithDistanceMin), MAX(fZenithDistanceMax), ";
        query += " MIN(fAzimuthMin), MAX(fAzimuthMax), ";
        query += " MIN(fRunStart), MAX(fRunStop) ";
        query += " FROM RunInfo WHERE ";
        query += where;
        query += " AND fRunTypeKEY=1";// GROUP BY fNight";

        res = Query(query);
        if (!res)
            return kFALSE;

        row = res->Next();
        if (!row || res->GetFieldCount()!=8)
        {
            cout << "ERROR - Wrong result from query: " << query << endl;
            return kFALSE;
        }

        const TString nevts = (*row)[0];
        const TString secs  = (*row)[1];
        const TString zdmin = (*row)[2];
        const TString zdmax = (*row)[3];
        const TString azmin = (*row)[4];
        const TString azmax = (*row)[5];
        const TString start = (*row)[6];
        const TString stop  = (*row)[7];

        delete res;
/*
        const TString elts = GetELTSource(where);
        if (elts.IsNull())
            return kFALSE;

        const TString eltp = GetELTProject(where);
        if (eltp.IsNull())
            return kFALSE;

        // ========== Request data of sequence ==========
        query  = "SELECT ";

        query += elts;
        query += ", ";
        query += eltp;

        query += ", fL1TriggerTableKEY, fL2TriggerTableKEY,"
            " fHvSettingsKEY, fDiscriminatorThresholdTableKEY,"
            " fTriggerDelayTableKEY, fObservationModeKEY, fSumTriggerFlagKEY "
            " FROM RunData WHERE fRunTypeKEY=2 AND ";
        query += where;
        query += " LIMIT 1";

        res = Query(query);
        if (!res)
            return kFALSE;

        row = res->Next();
        if (!row || res->GetFieldCount()!=9)
        {
            cout << "ERROR - No result from query: " << query << endl;
            return kFALSE;
        }
        */

        const TString set1 = Form("fSequenceID=%ld", from);
        const TString set2 = set1 + Form(", fNight=%d", run);
        const TString set3 = set2 + Form(", fStartTime=Now(), fStopTime=Now(), fProcessingSiteKEY=4, fAvailable=Now() ");

        TString query1;
        query1 += set2+",";
        query1 += Form(" fNumEvents=%s,",         nevts.Data());
        query1 += Form(" fSequenceDuration=%s,",  secs.Data());
        query1 += Form(" fSequenceStart=\"%s\",", start.Data());
        query1 += Form(" fSequenceStop=\"%s\",",  stop.Data());
        query1 += Form(" fZenithDistanceMin=%s,", zdmin.IsNull() ? "NULL" : zdmin.Data());
        query1 += Form(" fZenithDistanceMax=%s,", zdmax.IsNull() ? "NULL" : zdmax.Data());
        query1 += Form(" fAzimuthMin=%s,",        azmin.IsNull() ? "NULL" : azmin.Data());
        query1 += Form(" fAzimuthMax=%s",         azmax.IsNull() ? "NULL" : azmax.Data());

        if (!Insert("SequenceInfo", query1))
        {
            cout << "ERROR - Could not insert Sequence into Sequences." << endl;
            return kFALSE;
        }

        if (!Insert("SequenceFileAvailISDCStatus", set3))
        {
            cout << "ERROR - Could not insert Sequence into SequenceFileAvailISDCStatus." << endl;
            return kFALSE;
        }

        if (!Update("RunInfo", set1, where))
        {
            cout << "ERROR - Could not update RunData." << endl;
            return kFALSE;
        }

        return kTRUE;
    }

    Bool_t DeleteSequence(Int_t sequ, UInt_t night)
    {
        if (fPathRawData.IsNull() || fPathSequences.IsNull())
        {
            cout << "       + Deletion " << sequ << " skipped due to missing path." << endl;
            return kTRUE;
        }

        //queries to delete information from the database
        const TString cond(Form("fSequenceID=%d AND fNight=%d", sequ, night));

        const TString path(Form("%04d/%02d/%02d", night/10000, (night/100)%100, night%100));

        //commands to delete files from the disk
        const TString cmd0(Form("rm -f %s/%s/%08d_%03d*seq", fPathSequences.Data(), path.Data(), night, sequ));

//        const TString cmd1(Form("rm -rf %s/callisto/%04d/%08d/", fPathRawData.Data(),   sequ/10000, sequ));
//        const TString cmd2(Form("rm -rf %s/star/%04d/%08d/",     fPathRawData.Data(),   sequ/10000, sequ));

        if (!Delete("SequenceInfo", cond))
            return 2;

        if (!Update("RunInfo", "fSequenceID=NULL", cond))
            return 2;

        if (IsDummy())
        {
            cout << "       + would execute: " << cmd0 << endl;
//            cout << "       + would execute: " << cmd1 << endl;
//            cout << "       + would execute: " << cmd2 << endl;
            return kTRUE;
        }

        cout << "       + will execute: " << cmd0 << endl;
//        cout << "       + will execute: " << cmd1 << endl;
//        cout << "       + will execute: " << cmd2 << endl;

        gSystem->Exec(cmd0);
//        gSystem->Exec(cmd1);
//        gSystem->Exec(cmd2);

        return kTRUE;
    }

    Int_t CheckSequence(Long_t runstart, Long_t runstop, const TString &cond)
    {
        const char *fmt1 = "SELECT fRunID FROM RunInfo WHERE";
        const char *fmt2 = "AND fExcludedFDAKEY=1 ORDER BY fRunID";

        const TString query1 = Form("%s fSequenceID=%ld AND %s %s",            fmt1, runstart, cond.Data(), fmt2);
        const TString query2 = Form("%s fRunID BETWEEN %ld AND %ld AND %s %s", fmt1, runstart, runstop, cond.Data(), fmt2);

        TSQLResult *res1 = Query(query1);
        if (!res1)
            return 2;

        TSQLResult *res2 = Query(query2);
        if (!res2)
        {
            delete res1;
            return 2;
        }

        while (1)
        {
            TSQLRow *row1 = res1->Next();
            TSQLRow *row2 = res2->Next();

            if (!row1 && !row2)
                return kTRUE;

            if (!row1 || !row2)
                return kFALSE;

            if (atol((*row1)[0])!=atol((*row2)[0]))
                return kFALSE;
        }

        return kFALSE;
    }

    Int_t CreateSequence(std::pair<Long_t,Long_t> &drs, Long_t runstart, Long_t runstop, const TString &cond)
    {
        cout << "   * Creating Sequence " << runstart << "-" << runstop << " (drs=" << drs.first << "/" << drs.second << "):" << endl;

        //----------------------------------------------------------------------------

        TString query;
        query  = "SELECT fRunID FROM RunInfo WHERE fExcludedFDAKEY=1 AND ";
        query += cond;
        query += " AND fRunTypeKEY=4 AND fRunID BETWEEN ";
        query += Form("%ld AND %ld", runstart, runstop);

        TSQLResult *res = Query(query);
        if (!res)
            return kFALSE;

        // Check if this is a DRS calibration (contains a G run)
        if (res->GetRowCount()>0 && res->GetFieldCount()==1)
        {
            delete res;

            // Get DRS file with pedestal (roi<1024)
            query  = "SELECT MAX(fRunID) FROM RunInfo WHERE ";
            query += cond;
            query += " AND fHasDrsFile=1 AND (fDrsStep IS NULL OR fDrsStep=2) AND fRunTypeKEY=2";
            query += " AND fRunID BETWEEN ";
            query += Form("%ld AND %ld", runstart, runstop);

            res = Query(query);
            if (!res)
                return kFALSE;

            if (res->GetRowCount()==0 || res->GetFieldCount()!=1)
                return kFALSE;

            TSQLRow *row = res->Next();

            drs.first  = runstart;
            drs.second = (*row)[0] ? atol((*row)[0]) : -1;
        }

        delete res;

        //----------------------------------------------------------------------------

        query  = "SELECT fSequenceID, fNight FROM RunInfo WHERE ";
        query += cond;
        query += " AND fRunID BETWEEN ";
        query += Form("%ld AND %ld", runstart, runstop);
        query += " AND fSequenceID>0";
        query += " GROUP BY fSequenceID";

        res = Query(query);
        if (!res)
            return -2;

        const Int_t cnt = res->GetRowCount();

        Int_t rc = kTRUE;
        if (cnt==1)
        {
            TSQLRow *row=res->Next();
            const Int_t check = CheckSequence(runstart, runstop, cond);
            if (check==kTRUE)
            {
                cout << "     - Identical sequence already existing." << endl;
                delete res;
                return -1; // kTRUE
            }
            if (check==2)
                rc=2;
            else
            {
                cout << "     - Deleting quasi-identical sequence " << atoi((*row)[0]) << endl;
                if (DeleteSequence(atoi((*row)[0]), atoi((*row)[1]))==2)
                    rc = 2;
            }
        }
        else
        {
            TSQLRow *row=0;
            while ((row=res->Next()))
            {
                cout << "     - Deleting overlapping sequence " << atoi((*row)[0]) << endl;
                if (DeleteSequence(atoi((*row)[0]), atoi((*row)[1]))==2)
                    rc = 2;
            }
        }

        delete res;

        if (rc==2)
            return -2;

        if (!InsertSequence(drs, runstart, runstop, cond))
            return -2;

        return kTRUE;
    }

    Bool_t ReadResources(const char *fname)
    {
        // Check for the section header
        TPRegexp regexp("^\\[(RunTypes|Transition|Regexp):?[0-9 ]*\\]$");
        // Check if section header contains a number
        TPRegexp regnum("[0-9]");
        // Check if section header contains the telescope number
        TPRegexp regtel(Form("[^0-9]0*%d[^0-9]", fTelescopeNumber));

        ifstream fin(fname);
        if (!fin)
        {
            cout << "Cannot open file " << fname << ": ";
            cout << strerror(errno) << endl;
            return kFALSE;
        }

        Int_t section = 0;

        TString key;
        while (1)
        {
            TString txt;
            txt.ReadLine(fin);
            if (!fin)
                break;

            txt = txt.Strip(TString::kBoth);

            if (txt[0]=='#' || txt.IsNull())
                continue;

            if (txt[0]=='[')
            {
                TString sec = txt(regexp);
                if (!sec.IsNull())
                {
                    // Skip sections with the wrong telescope number
                    // (If section contains numbers but not the tel num)
                    if (!sec(regnum).IsNull() && sec(regtel).IsNull())
                        continue;

                    // Check which section we are in
                    if (sec.BeginsWith("[RunTypes"))
                        section = 1;
                    if (sec.BeginsWith("[Transition"))
                        section = 2;
                    if (sec.BeginsWith("[Regexp"))
                        section = 3;

                    continue;
                }

                if (section!=3)
                {
                    cout << "WARNING - Line starts with [ but we are not in the Regexp section." << endl;
                    cout << txt << endl;
                    continue;
                }
            }


            TObjArray *arr = txt.Tokenize(" ");

            if (arr->GetEntries()>0)
                switch (section)
                {
                case 1:
                    // Create a llist of run-types
                    // drs-time T
                    break;
                case 2:
                    {
                        TString key = arr->At(0)->GetName();
                        key.Prepend("f");
                        key.Append("KEY");

                        CheckList *list = dynamic_cast<CheckList*>(fMap.GetValue(key));
                        if (!list)
                        {
                            //cout << key << endl;
                            list = new CheckList;
                            fMap.Add(new TObjString(key), list);
                        }

                        if (arr->GetEntries()>1)
                        {
                            //cout << key << " ";
                            list->Add(new CheckMatch(*arr, 1));
                        }
                    }
                    break;
                case 3:
                    fListRegexp.Add(new Rule(*arr));
                    break;
                }

            delete arr;
        }

        return kTRUE;
    }
/*
    TString GetELT(const char *col, TSQLResult *res, TList &regexp)
    {
        TObjArray names; // array with old names (including regexp)

        // Add to array and expand the array if necessary
        TSQLRow *row=0;
        while ((row=res->Next()))
            names.AddAtAndExpand(new TObjString((*row)[1]), atoi((*row)[0]));

        // Now a LUT is build which converts the keys for
        // the names including the regexp into keys for
        // the names excluding the regexp
        TString elt(Form("ELT(RunData.f%sKEY+1", col));

        // loop over all entries in the list
        const Int_t n = names.GetSize();
        for (int i=0; i<n; i++)
        {
            // For all entries which are not in the list
            // write an undefined value into the LUT
            TObject *o = names.UncheckedAt(i);
            if (!o)
            {
                elt += ",0";
                continue;
            }

            // Remove the regexp from the string which includes it
            TString name = o->GetName();

            TIter NextR(&regexp);
            TObject *obj=0;
            while ((obj=NextR()))
            {
                TPRegexp reg(obj->GetName());
                const Ssiz_t pos = name.Index(reg, 0);
                if (pos>0)
                {
                    name.Remove(pos);
                    name += "-W";
                    break;
                }
            }

            // Check if such a Key exists, if not insert it
            const Int_t key = QueryKeyOfName(col, name);

            // RESOLVE return code!!!
            //if (key<0)
            //     return "";

            // add index to the LUT
            elt += Form(",%d", key);
        }

        // close LUT expression
        //    elt += ") AS Elt";
        //    elt += col;

        elt += ") AS f";
        elt += col;
        elt += "KEY";

        // return result
        return elt;
    }

    TString GetELT(const char *col, TSQLResult *res, TString regexp)
    {
        TList list;
        list.SetOwner();
        list.Add(new TObjString(regexp));
        return GetELT(col, res, list);
    }

    TString GetELTQuery(const char *col, const char *cond) const
    {
        return Form("SELECT RunData.f%sKEY, f%sName FROM RunData "
                    "LEFT JOIN %s USING (f%sKEY) WHERE %s GROUP BY f%sName",
                    col, col, col, col, cond, col);
    }

    TString GetELTSource(const char *cond)
    {
        //query all sources observed in this night
        TSQLResult *resx = Query(GetELTQuery("Source", cond));
        if (!resx)
            return "";

        // In the case there is only a single source
        // do not replace the source key by the ELT
        if (resx->GetRowCount()==1)
            return "fSourceKEY";

        TString elts = GetELT("Source", resx, "\\-?W[1-9][ abc]?$");
        delete resx;

        return elts;
    }

    TString GetELTProject(const char *cond)
    {
        //query all project names observed in this night
        TSQLResult *resx = Query(GetELTQuery("Project", cond));
        if (!resx)
            return "";

        // In the case there is only a single project
        // do not replace the project key by the ELT
        if (resx->GetRowCount()==1)
            return "fProjectKEY";

        TList regexp;
        regexp.Add(new TObjString("\\-?W[1-9][abc]?$"));
        regexp.Add(new TObjString("\\-W[0-9]\\.[0-9][0-9]\\+[0-9][0-9][0-9]$"));

        TString eltp2 = GetELT("Project", resx, regexp);
        delete resx;
        regexp.Delete();

        return eltp2;
    }
*/
    Bool_t HasAtLeastOne(TString src, TString chk) const
    {
        src.ToLower();
        chk.ToLower();

        for (int i=0; i<chk.Length(); i++)
            if (src.First(chk[i])<0)
                return kFALSE;

        return kTRUE;
    }

    TString PrepareString(TSQLResult &res, TArrayL &runs, TArrayL &nights)
    {
        // Number of result rows
        const Int_t rows = res.GetRowCount();

        runs.Set(rows);       // initialize size of array for run numbers
        nights.Set(rows);       // initialize size of array for nights

        TArrayD start(rows);  // instantiate array for start times
        TArrayD stop(rows);   // instantiate array for stop times

        TString str;          // result string

        Int_t idx=0;
        TSQLRow *row=0;
        while ((row=res.Next()))
        {
            runs[idx] = atol((*row)[0]);          // run number
            nights[idx] = atol((*row)[4]);          // night

            const TString tstart = ((*row)[2]);   // start time
            const TString tstop  = ((*row)[3]);   // stop time

            start[idx] = MTime(tstart).GetMjd();  // convert to double
            stop[idx]  = MTime(tstop).GetMjd();   // convert to double

            // This is a workaround for broken start-times
            if (tstart=="0000-00-00 00:00:00")
                start[idx] = stop[idx];

            // Add a run-type character for this run to the string
            str += RunType((*row)[1]);

            // Increase index
            idx++;
        }
/*
        // Now the P- and D- runs are classified by the time-distance
        // to the next or previous C-Run
        Double_t lastc = -1;
        Double_t nextc = -1;
        for (int i=0; i<str.Length(); i++)
        {
            if (str[i]=='C')
            {
                // Remember stop time of C-Run as time of last C-run
                lastc = stop[i];

                // Calculate start time of next C-Run
                const TString residual = str(i+1, str.Length());
                const Int_t pos = residual.First('C');

                // No C-Run found anymore. Finished...
                if (pos<0)
                    break;

                // Remember start  time of C-Run as time of next C-run
                nextc = start[i+1+pos];
                continue;
            }

            // Check whether the identifying character should
            // be converted to lower case
            if (start[i]-lastc>nextc-stop[i])
                str[i] = tolower(str[i]);
        }
*/
        //cout << str << endl;
        return str;
    }

    void PrintResidual(Long_t runstart, Long_t runstop, TString residual, const char *descr)
    {
        residual.ToLower();

        // Count number of unsequences "characters"
        const Int_t numd = residual.CountChar('d');
        const Int_t nump = residual.CountChar('p');
        const Int_t numb = residual.CountChar('b');
        const Int_t numg = residual.CountChar('g');
        const Int_t numt = residual.CountChar('t');
        const Int_t numc = residual.CountChar('c');

        // Print some information to the output steram
        if (numd+nump+numb+numg+numt+numc==0)
            return;

        cout << "   ! " << runstart << "-" << runstop << " [" << setw(3) << 100*residual.Length()/(runstop-runstart+1) << "%]: " << descr << " not sequenced:  P=" << nump << "  C=" << numc << "  D=" << numd;
        if (numd>0)
            cout << "  (DATA!)";
/*
        if (nump==0 || numc==0 || numd==0)
            cout << "  Missing";
        if (numd==0)
            cout << " D";
        if (nump==0)
            cout << " P";
        if (numc==0)
            cout << " C";
*/
        cout << endl;
    }

    Int_t SplitBlock(Long_t runstart, Long_t runstop, const TString &cond)
    {
        // Request data necessary to split block into sequences
        /*const*/
        TString query =
            "SELECT fRunID, fRunTypeKEY, fRunStart, fRunStop, fNight*1000 "
            " FROM RunInfo WHERE fExcludedFDAKEY=1 AND ";
        query += Form("fRunID BETWEEN %ld AND %ld AND "
                      /*" (fRunTypeKEY BETWEEN 2 AND 4)"*/, runstart, runstop);

        query += cond;
        query += " ORDER BY fRunID";


        // Send query
        TSQLResult *res = Query(query);
        if (!res)
            return 2;

        // Get String containing the sequence of P-,C- and D-Runs
        // and an array with the corresponding run-numbers
        TArrayL runs;
        TArrayL nights;
        const TString str = PrepareString(*res, runs, nights);

        delete res;

        // Check if the prepared string at least contains one run of each type
        // ??????????????????????????????????
        /*
        if (!HasAtLeastOne(str, "PCD"))
        {
            PrintResidual(runstart, runstop, str, "Block");
            return kTRUE;
        }*/
        // ??????????????????????????????????

        // Start the sequence building
        // ---------------------------

        Int_t   pos = 0;   // Position in the string
        std::map<Long_t, Long_t>  map;       // Map for resulting sequences
        TString residual;  // Unsequences "characters"

        // Step through the string one character by one
        cout << "   ";
        while (pos<str.Length())
        {
            Bool_t found = kFALSE;

            // Loop over the predefined regular expressions
            TIter Next(&fListRegexp);
            Rule *obj = 0;
            while ((obj=(Rule*)Next()))
            {
                // Check if regular expressions matchs
                const Ssiz_t len = obj->Match(str, pos, nights[pos]);
                if (len>0)
                {
                    // In case of match get the substring which
                    // defines the sequence and print it
                    TString sub = str(pos, len);
                    sub.ToUpper();
                    cout << runs[pos]<<":"<< sub << "|";

                    // Add the first and last run of the sequence to the map
                    //map.Add(runs[pos], runs[pos+len-1]);
                    map[runs[pos]] = runs[pos+len-1];

                    // A sequence was found...
                    found = kTRUE;

                    // step forward in the string by the length of the sequence
                    pos += len;
                    break;
                }
            }

            // If a sequence was found go on...
            if (found)
                continue;

            // print unsequenced characters
            cout << (char)tolower(str[pos]);

            // Count the number of "characters" not sequenced
            residual += str[pos];

            // step one character forward
            pos++;
        }
        cout << endl;

        PrintResidual(runstart, runstop, residual, "Runs ");

        // Create all sequences which were previously found
        std::pair<Long_t,Long_t> drs(-1, -1);
        for (std::map<Long_t,Long_t>::const_iterator it=map.begin();
             it!=map.end(); it++)
            if (CreateSequence(drs, it->first, it->second, cond)==2)
                return 2;
        /*
        Long64_t first, last;
        TExMapIter iter(&map);
        while (iter.Next(first, last))
            if (CreateSequence(first, last, cond)==2)
                return 2;
         */
        return kTRUE;
    }

    Char_t RunType(Int_t n) const
    {
        switch (n)
        {
        case  1: return 'D';
        case  2: return 'P'; // Ped1024
        case  3: return 'B'; // Ped 300
        case  4: return 'G'; // drs-gain
        case  5: return 'T'; // drs-time
        case  6: return 'C'; // light-pulser-ext
        case 11: return 'M'; // ped-and-lp-ext
        case 12: return 'U'; // drs-time-upshifted
        }
        return '-';
    }
    Char_t RunType(const char *str) const
    {
        return RunType(atoi(str));
    }

    Int_t BuildBlocks(TSQLResult *res, TExMap &blocks, const TString &cond)
    {
        // col key content
        // -----------------------------
        //  0   -  runnumber
        //  1   0  RunTypeKEY
        //  2   1  source
        //  3   2  project
        //  .   .
        //  .   .
        //  .   .  from transition.txt
        //

        //build blocks of runs, which have the same values
        //for each block the first and the last run are stored in a TExMap
        //the values are checked with the help of an array of TStrings
        Long_t runstart = -1;
        Long_t runstop  = -1;

        const UInt_t ncols = res->GetFieldCount();

        TString keys[ncols]; // Index 0 is not used (Index 1: RunType)

        // Loop over runs
        TSQLRow *row=0;
        while ((row=res->Next()))
        {
            // This is the runnumber of the first run in the new block
            if (runstart<0)
                runstart=atol((*row)[0]);

            // Check which transitions might be allowed for this run and
            // which are not. Check also which attributes should be ignored
            // for this run. The information about it is stored in the map.
            Int_t rc[ncols];
            for (UInt_t i=2; i<ncols; i++)
                rc[i] = CheckTransition(*res, keys, *row, i);

            for (UInt_t i=2; i<ncols; i++)
            {
                switch (rc[i])
                {
                case kTRUE:    // Transition allowed, switch to new attribute
                    keys[i] = (*row)[i];
                    continue;

                case -1:       // Ignore attribute, continue with next one
                    //if (keys[i] == (*row)[i])
                    //    break; // Only ignore it if it is really different
                    continue;

                case kFALSE:   // No decision (nothing matching found in map)
                    break;     //  go on checking

                }

                // If past-attribute is not yet initialized, do it now.
                if (keys[i].IsNull())
                    keys[i] = (*row)[i];

                // Check whether key has changed for this run
                // (this condition is never true for the first run)
                // This condition in only reached if keys[i] was initialized
                if (keys[i] == (*row)[i])
                    continue;

                // Found one block with unique keys, fill values into TExMap
                // (except if this is the first run processed)
                cout << endl;
                cout << " - Identical conditions from " << runstart << " to " << runstop << endl;
                blocks.Add((ULong_t)blocks.GetSize(), runstart, runstop);
                if (SplitBlock(runstart, runstop, cond)==2)
                    return 2;
                cout << " - Transition from " << RunType(keys[1]) << ":";
                cout << QueryNameOfKey(res->GetFieldName(i), keys[i]) << " <" << keys[i] << "> to " << RunType((*row)[1]) << ":";
                cout << QueryNameOfKey(res->GetFieldName(i), (*row)[i]) << " <" << (*row)[i] << ">";
                cout << " in " << res->GetFieldName(i) << " of " << (*row)[0] << endl;

                // This is already the first run of the new block
                runstart=atol((*row)[0]);

                // These are the keys corresponding to the first run
                // in the new block. All following runs should have
                // identical keys. Do not set the attribute if for
                // this attribute the transition check evaluated
                // to "ignore".
                for (UInt_t j=2; j<ncols; j++)
                    keys[j] = rc[j]==-1 ? "" : (*row)[j];

                break;
            }

            keys[1] = (*row)[1]; // Remember run type for next run

            // This is the new runnumber of the last run in this block
            runstop=atol((*row)[0]);
        }

        // If runstart==-1 a possible block would contain a single run...
        if (runstart>0 && runstart!=runstop)
        {
            cout << " - Identical conditions from " << runstart << " to " << runstop << " (last run)" << endl;
            //fill values into TExMap (last value)
            blocks.Add((ULong_t)blocks.GetSize(), runstart, runstop);
            if (SplitBlock(runstart, runstop, cond)==2)
                return 2;
        }
        cout << "Done." << endl << endl;

        return kTRUE;
    }

public:
    SequenceBuild(Int_t tel=1, const char *rc="sql.rc") : MSQLMagic(rc), fTelescopeNumber(tel)
    {
        fListRegexp.SetOwner();

        // FIXME: THIS IS NOT YET HANDLED
        if (ReadResources("fact/resources/sequences.rc"))
            return;
    }

    SequenceBuild(TEnv &env, Int_t tel=1) : MSQLMagic(env), fTelescopeNumber(tel)
    {
        fListRegexp.SetOwner();

        // FIXME: THIS IS NOT YET HANDLED
        if (ReadResources("fact/resources/sequences.rc"))
            return;
    }
    ~SequenceBuild()
    {
        fMap.DeleteAll();
    }

    void SetPathRawData(const char *path) { fPathRawData=path; }
    void SetPathSequences(const char *path) { fPathSequences=path; }

    int Build(TString day)
    {
        cout << endl;
        cout << "Night of sunset at " << day << ":" << endl;

        const TString cond = Form("fNight=%s ", day.Data());

        //    Form("fRunNumber=%s "
        //         "AND fExcludedFDAKEY=1 AND fRunTypeKEY BETWEEN 2 AND 4 "
        //         "AND fTelescopeNumber=%d ",
        //         day.Data(), fTelescopeNumber);
/*
        //query all sources observed in this night
        const TString elts = GetELTSource(cond);
        if (elts.IsNull())
            return 2;

        //query all project names observed in this night
        const TString eltp2 = GetELTProject(cond);
        if (elts.IsNull())
            return 2;
*/
        // Setup query to get all values from the database,
        // that are relevant for building sequences
        TString query("SELECT fRunID, fRunTypeKEY ");
/*
        query += elts;
        query += ", ";
        query += eltp2;
*/
        // Now add all entries from the transition table to the query
        TIter NextPair(&fMap);
        TObject *mapkey = 0;
        while ((mapkey=(TPair*)NextPair()))
            if (!query.Contains(mapkey->GetName()))
            {
                query += ", ";
                query += mapkey->GetName();
            }
        query += " FROM RunInfo WHERE fExcludedFDAKEY=1 AND ";
        query += cond;
        query += " ORDER BY fRunID";

        TSQLResult *res = Query(query);
        if (!res)
            return 2;

        TExMap blocks;
        const Int_t rc = BuildBlocks(res, blocks, cond);
        delete res;

        return rc;
    }

    Int_t Build()
    {
        //get all dates from the database
        TSQLResult *res = Query("SELECT fNight FROM RunInfo GROUP BY fNight ORDER BY fNight DESC");
        if (!res)
            return 2;

        //execute buildsequenceentries for all dates
        TSQLRow *row=0;
        while ((row=res->Next()))
            Build((*row)[0]);

        delete res;

        return 1;
    }
    ClassDef(SequenceBuild, 0)
};

ClassImp(SequenceBuild);

int buildseqentries(TString day, TString datapath, TString sequpath, Int_t tel=1, Bool_t dummy=kTRUE)
{
    SequenceBuild serv(tel, "sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "buildseqentries" << endl;
    cout << "---------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    if (!datapath.IsNull())
        cout << "DataPath:  " << datapath << endl;
    if (!sequpath.IsNull())
        cout << "SeqPath:   " << sequpath << endl;
    cout << "Day:       " << day      << endl;
    cout << "Telescope: " << tel      << endl;
    cout << endl;

    serv.SetIsDummy(dummy);
    serv.SetPathRawData(datapath);
    serv.SetPathSequences(sequpath);
    return serv.Build(day);
}

//
// Build Sequences for all Nights
//
int buildseqentries(TString datapath, TString sequpath, Int_t tel=1, Bool_t dummy=kTRUE)
{
    SequenceBuild serv(tel, "sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "buildseqentries" << endl;
    cout << "---------------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "DataPath:  " << datapath << endl;
    cout << "SeqPath:   " << sequpath << endl;
    cout << "Telescope: " << tel      << endl;
    cout << endl;

    serv.SetIsDummy(dummy);
    serv.SetPathRawData(datapath);
    serv.SetPathSequences(sequpath);
    return serv.Build();
}

int buildseqentries(Int_t tel=1, Bool_t dummy=kTRUE)
{
    return buildseqentries("", "", tel, dummy);
}

int buildseqentries(TString day, Int_t tel=1, Bool_t dummy=kTRUE)
{
    return buildseqentries(day, "", "", tel, dummy);
}
