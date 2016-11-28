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
!   Author(s): Daniela Dorner, 08/2004 <mailto:dorner@astro.uni-wuerzburg.de>
!   Author(s): Thomas Bretz, 08/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// filldotrun.C
// ============
//
// This macro is used in the datacenter to automatically fill the run-database
// with the information stored in the .run-files written by the central
// control.
//
// To following Arehucas versions are Currently supported:
//   040505-0, 040514-0,
//   040518-0, 040727-0,
//   041113-0, 041209-0, 041221-0
//   050224-0, 050317-0, 050322-0, 050401-0, 050413-0, 050415-0, 050714-0,
//   050719-0, 050829-0, 051025-0,
//   060330-0, 060401-0, 060808-0
//   070416-0,
//   080220-0, 080519-0, 080912-0, 081204-0, 081214-0,
//   090203-0, 090221-0, 090522-0, 090525-0, 090616-0, 090625-0, 090702-0,
//   090706-0, 090731-0
//
// Usage:
//    .x filldotrun.C+("/data/MAGIC/Period019/ccdata", kTRUE)
//
// While the first argument is the directory in which all subdirectories where
// searches for CC_*.run files. All these files were analysed and the run
// info will be put into the DB, eg:
//  "/magic/subsystemdata/cc"                  would do it for all data
//  "/magic/subsystemdata/cc/2005"                  for one year
//  "/magic/subsystemdata/cc/2005/11"               for one month
//  "/magic/subsystemdata/cc/2005/11/11"            for a single day
//  "/magic/subsystemdata/cc/2005/11/11/file.run"   for a single file
//
// The second argument is the 'dummy-mode'. If it is kTRUE dummy-mode is
// switched on and nothing will be written into the database. Instead
// informations about the subtables are displayed. This is usefull for tests
// when adding a new arehucas version support. If it is kFALSE the information
// are written into the subtables and the runs info is written into the
// rundatabase.
//
// In the automatic case it makes sense to check the logfiles to make sure
// that everything is fine...
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
//
// Remark: Running it from the commandline looks like this:
//   root -q -l -b filldotrun.C+\(\"path\"\,kFALSE\) 2>&1 | tee filldotrun.log
//
// Returns 0 in case of failure and 1 in case of success.
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <fstream>

#include <TMath.h>
#include <TRegexp.h>

#include "MTime.h"
#include "MDirIter.h"
#include "MSQLMagic.h"

using namespace std;

Int_t insert(MSQLMagic &serv, Bool_t dummy, TString filename)
{
    ifstream fin(filename);
    if (!fin)
    {
        cout << "Could not open file " << filename << endl;
        return -1;
    }

    TString strng;
    TObjArray *array = new TObjArray();
    Int_t check=0;

    strng.ReadLine(fin);
    if (strng!=TString("[CC Plain Run Summary File]"))
    {
        cout << filename << ": No Plain Run Summary File" << endl;
        cout << "First Line: " << strng << endl;
        cout << endl;
        return -1;
    }

    strng.ReadLine(fin);
    TRegexp reg("[0-9][0-9][0-9][0-9][0-9][0-9]-[0-9]");
    TString arehucas = strng(reg);
    arehucas.Prepend("20");
    arehucas.ReplaceAll("-", "");

    Int_t version = atoi(arehucas.Data());
    if (version!=200405050 && version!=200405140 && version!=200405180 &&
        version!=200407270 && version!=200411130 && version!=200412090 &&
        version!=200412210 &&
        version!=200502240 && version!=200503170 && version!=200503220 &&
        version!=200504010 && version!=200504130 && version!=200504150 &&
        version!=200507140 && version!=200507190 && version!=200508290 &&
        version!=200510250 &&
        version!=200603300 && version!=200604010 && version!=200608080 &&
        version!=200704160 &&
        version!=200802200 && version!=200805190 && version!=200809120 &&
        version!=200812040 && version!=200812140 &&
        version!=200902030 && version!=200902210 && version!=200905220 &&
	version!=200905250 && version!=200906160 && version!=200906250 &&
	version!=200907020 && version!=200907060 && version!=200907310)
    {
        cout << filename << ": File Version unknown - please update the macro!" << endl;
        cout << "Second Line: " << strng << endl;
        cout << endl;
        return -1;
    }

    Int_t telcheck=1;
    TString tcheck;
    if (version >= 200805190)
    {
	strng.ReadLine(fin);
        if (!strng.BeginsWith("Telescope M"))
        {
            cout << "WARNING - Line 3 doesn't start with 'Telescope M'." << endl;
            cout << strng << endl;
        }
	telcheck = atoi(strng.Data()+11);

        if (telcheck != 1 && telcheck != 2)
        {
            cout << filename << ": Telescope declaration wrong!" << endl;
            cout << "Third Line: " << strng << endl;
            cout << endl;
            return -1;
        }
    }

    if (version >= 200411130)
    {
        strng.ReadLine(fin);
        if (strng[0]!='#')
        {
            cout << "WARNING - '#' expected." << endl;
            cout << strng << endl;
        }
    }

    cout << " * V" << version << " " << endl;

    Int_t cnt=0;
    while (1)
    {
        // ===== Check for number of columns in file =====
        // different arehucas versions provide a different number of columns:
        // 200405050 - 200405140: 18 columns
        // 200405180 - 200407270: 35 columns
        // 200411130 - 200510250: 43 columns
        // 200603300 - 200802200: 52 columns
        // 200805190 - 200809120: 54 columns
        // 200812040 - 200906160: 55 columns
        // >= 200906250:          61 columns
        //
	strng.ReadLine(fin);
	//fill an array with the values separated by ' '. Advantage: In contrast to ReadToDelim(' ') objects 'between'
	//two whitespaces are not written to the array. In case of a missing column the error is detected by comparison
	//with the number of default columns. In case of an 'accidental' double(triple)-whitespace the columns are treated
	//in the right way, only a warning is released to edit the file, if necessary.
	array = strng.Tokenize(" ");
	check = array->GetEntries();

        if (strng.Contains("  ") || strng. Contains("   "))
	{
	    cout << "WARNING - Multiple whitespaces found, please check the file:" << endl;
	    cout << filename << endl;
	}

        /*
	for (int n=0; n< array->GetEntries(); n++)
	{
	    cout << (*array)[n]->GetName() << endl;
	}
        */
        //check the number of columns for the different versions, if there is an empty line (check=0), do nothing
        if (check != 0)
        {
            if ((version <= 200405140 && check != 18) ||
                (version >  200405140 && version <= 200407270 && check != 35) ||
                (version >  200407270 && version <= 200510250 && check != 43) ||
                (version >  200510250 && version <= 200802200 && check != 52) ||
                (version >  200802200 && version <= 200809120 && check != 54) ||
                (version >  200809120 && version <= 200906160 && check != 55) ||
                (version >  200906160 && ((telcheck == 1 && check != 61) || (telcheck == 2 && check != 63))))
            {
		cout << "ERROR - Number of columns (" << check << ") doesn't match number of required columns, please check the file:" << endl;
                cout << filename << endl;
		delete array;
                continue;
            }
	}

	//when reaching the end of the file...
	if (!fin)
            break;

        // count variable for the columns
	Int_t i = 0;

        // ========== Col 1: Telescope Number =========
        Int_t telnumber = 1; // FIXME: "NULL"?
        if (version >=200805190)
        {
            strng = (*array)[i++]->GetName();

	    if (strng[0]!='M')
            {
                cout << "WARNING - First character is not an M." << endl;
                cout << strng << endl;
                delete array;
                continue;
            }
	    telnumber = atoi(strng.Data()+1);

        }

        // ========== Col 2: Run Number =========
        //Reading the line
        //and converting some strings to ints/floats
	strng = (*array)[i++]->GetName();

        Int_t runnumber = atoi(strng.Data());

        //runnumber=0 means no valid dataset
        //-> continue
        if (runnumber == 0)
        {
	    delete array;
            cout << "WARNING - Runnumber == 0" << endl;
            continue;
	}

	//cout << runnumber << " ";

        // ========== Col 3: Subrun Number ========= starting with version 200805190
        Int_t filenumber = 0; // FIXME: "NULL"?
        if (version >=200805190)
        {
	    strng = (*array)[i++]->GetName();
            filenumber = atoi(strng.Data());
        }

        TString where = Form("fTelescopeNumber=%d AND fFileNumber=%d",
                             telnumber, filenumber);
        if (serv.ExistStr("fRunNumber", "RunData", Form("%d", runnumber), where))
        {
            // FIXME: Maybe we can implement a switch to update mode?
            cout << "WARNING - Entry M" << telnumber << ":" << runnumber << "/" << filenumber << " already existing... skipped." << endl;
            delete array;
            continue;
	}


        // ========== Col 4: Run Type =========
	strng = (*array)[i++]->GetName();
        if (strng.Contains("???"))
            strng="n/a";

        Int_t runtype = serv.QueryKeyOfName("RunType", strng, kFALSE);
        if (runtype<0)
        {
            cout << "ERROR - RunType " << strng << " not available." << endl;
            delete array;
            continue;
	}


        // ========== Col 5,6: Start Time =========
        TString startdate, starttime;
	startdate = (*array)[i++]->GetName();
	starttime = (*array)[i++]->GetName();
        //cout << startdate << " " << starttime << " ";

        // ========== Col 7,8: Stop Time =========
	TString stopdate, stoptime;
	stopdate = (*array)[i++]->GetName();
	stoptime = (*array)[i++]->GetName();
        //cout << stopdate << " " << stoptime << " ";

        if (startdate.Contains("???"))
            startdate="0000-00-00";
        if (starttime.Contains("???"))
            starttime="00:00:00";
        if (stopdate.Contains("???"))
            stopdate="0000-00-00";
        if (stoptime.Contains("???"))
            stoptime="00:00:00";

        // ========== Col 9: Source Name =========
	strng = (*array)[i++]->GetName();
        if (strng.Contains("???"))
            strng="Unavailable";

        Int_t sourcekey = serv.QueryKeyOfName("Source", strng.Data());
        if (sourcekey<0)
        {
            delete array;
            continue;
        }

	//cout << sourcekey << " ";

        // ========== Col 10,11: Local source position =========
	strng = (*array)[i++]->GetName();
        Float_t zd = atof(strng.Data());

	strng = (*array)[i++]->GetName();
	Float_t az = atof(strng.Data());

        //cout << zd << " " << az << " ";

        // ========== Col 12: Number of Events =========
	strng = (*array)[i++]->GetName();
	Int_t evtno = atoi(strng.Data());

        //cout << evtno << " ";

        // ========== Col 13: Project Name =========
	strng = (*array)[i++]->GetName();
        if (strng.Contains("???"))
            strng="Unavailable";

        Int_t projkey = serv.QueryKeyOfName("Project", strng);
        if (projkey<0)
        {
            delete array;
            continue;
        }
	//cout << projkey << " ";

        // ========== Col 14: Trigger Table Name =========
        // starting from version 200411130: Col 14,15: Trigger Table Name =========
	strng = (*array)[i++]->GetName();
        if (strng.Contains("???"))
            strng="n/a";

        Int_t l1triggerkey=1;
        Int_t l2triggerkey=1;
        if (version >=200411130)
        {
            l1triggerkey = serv.QueryKeyOfName("L1TriggerTable", strng);
            if (l1triggerkey<0)
            {
                delete array;
                continue;
            }

	    strng = (*array)[i++]->GetName();
            if (strng.Contains("???"))
                strng="n/a";

            l2triggerkey = serv.QueryKeyOfName("L2TriggerTable", strng);
            if (l2triggerkey<0)
            {
                delete array;
                continue;
	    }
        }
        else
        {
            Int_t c=0;

            if (strng.Contains(":"))
                c=1;

            if (strng.Contains("L1_") && !(strng.Contains(":")))
                c=2;

            if (strng.Contains("n/a"))
                c=3;

            switch (c)
            {
            case 0:
                {
                    l2triggerkey = serv.QueryKeyOfName("L2TriggerTable", strng);
                    if (l2triggerkey<0)
                    {
                        delete array;
                        continue;
                    }

                    strng="n/a";
                    l1triggerkey = 1;

                    break;
                }
            case 1:
                {
                    TString L1TT, L2TT;
                    L2TT=strng(7,12);
                    L1TT=strng(0,6);

                    l1triggerkey = serv.QueryKeyOfName("L1TriggerTable", L1TT);
                    if (l1triggerkey<0)
                    {
                        delete array;
                        continue;
                    }

                    l2triggerkey = serv.QueryKeyOfName("L2TriggerTable", L2TT);
                    if (l2triggerkey<0)
                    {
                        delete array;
                        continue;
                    }

                    break;
                }
            case 2:
                {
                    l1triggerkey = serv.QueryKeyOfName("L1TriggerTable", strng);
                    if (l1triggerkey<0)
                    {
                        delete array;
                        continue;
                    }

                    strng="n/a";
                    l2triggerkey = 1;

                    break;
                }
            case 3:
                {
                    l1triggerkey = 1;
                    l2triggerkey = 1;
                    break;
                }
            default:
                {
                    cout << "WARNING: neither L1 nor L2 Trigger table - please check what is happening." << strng << endl;
                    break;
                }
            }
        }

        // ========== Col 16-18: TrigRate, L2 UnPresc Rate, L2 Presc Rate ==========
	strng = (*array)[i++]->GetName();
        Float_t trigrate = atof(strng.Data());

	strng = (*array)[i++]->GetName();
        Float_t l2uprate = atof(strng.Data());

	strng = (*array)[i++]->GetName();
        Float_t l2prrate = atof(strng.Data());

        // ========== Col 19,20: DaqRate, Storage Rate ==========
	strng = (*array)[i++]->GetName();
        Float_t daqrate = atof(strng.Data());

	strng = (*array)[i++]->GetName();
        Float_t storerate = atof(strng.Data());

        // ========== Col 21: HV table =========
	strng = (*array)[i++]->GetName();
	if (version==200405050 || version==200405140)
	{
	    delete array;
	    continue;
	}
        if (strng.Contains("???"))
            strng="n/a";

        Int_t hvkey = serv.QueryKeyOfName("HvSettings", strng);
        if (hvkey<0)
        {
            delete array;
            continue;
        }

        if (version==200405180 || version==200407270)
	{
	    delete array;
	    continue;
	}

        Int_t testflagkey=1;
        Int_t lightcondkey=1;
        Int_t dttablekey=1;
        Int_t triggerdelaytablekey=1;
        Int_t calibrationscriptkey=1;
        if (version>=200411130)
        {
            // ========== Col 22-38: DC and HV-values, mjd =========
            for (int n=0 ; n<17 ; n++)
            {
                i++;
            }

            // ========== Col 39: test-flag =========
	    strng = (*array)[i++]->GetName();
            if (strng.Contains("???"))
                strng="n/a";

            testflagkey = serv.QueryKeyOfName("TestFlag", strng);
            if (testflagkey<0)
            {
                delete array;
                continue;
            }

            // ========== Col 40: light conditions =========
	    strng = (*array)[i++]->GetName();
            if (strng.Contains("???"))
                strng="n/a";

            lightcondkey = serv.QueryKeyOfName("LightConditions", strng);
            if (lightcondkey<0)
            {
                delete array;
                continue;
            }

            // ========== Col 41: discriminator threshold table =========
	    strng = (*array)[i++]->GetName();
            if (strng.Contains("???"))
                strng="n/a";

            dttablekey = serv.QueryKeyOfName("DiscriminatorThresholdTable", strng);
            if (dttablekey<0)
            {
                delete array;
                continue;
            }

            // ========== Col 42: trigger delay table =========
	    strng = (*array)[i++]->GetName();
            if (strng.Contains("???"))
                strng="n/a";

            triggerdelaytablekey = serv.QueryKeyOfName("TriggerDelayTable", strng);
            if (triggerdelaytablekey<0)
            {
                delete array;
                continue;
            }

            // ========== Col 43,44: Telescope RA and Dec sent to drive =========
            i++;
            i++;

            // ========== Col 45: Calibration Script =========
	    strng = (*array)[i++]->GetName();
            if (version>=200411130 && version<=200510250)
	    {
		delete array;
		continue;
	    }
            if (strng.Contains("???"))
                strng="n/a";

            calibrationscriptkey = serv.QueryKeyOfName("CalibrationScript", strng);
            if (calibrationscriptkey<0)
            {
                delete array;
                continue;
            }

        }

        Int_t observationmodekey=1;
        if (version>=200603300)
        {
            // ========== Col 46: Observation Mode =========
	    strng = (*array)[i++]->GetName();
            if (strng.Contains("???"))
                strng="n/a";

            observationmodekey = serv.QueryKeyOfName("ObservationMode", strng);
            if (observationmodekey<0)
            {
                delete array;
                continue;
            }

            // ========== Col 47-54: Source RA and Dec, DT's and IPR =========
            for (int n=0 ; n<7 ; n++)
            {
                i++;
            }
	    strng = (*array)[i++]->GetName();
	    if (version<=200809120)
	    {
		delete array;
		continue;
	    }
        }

        Int_t sumtriggerflagkey=1;
        if (version>=200812040)
        {
            // ========= Col 55: SumTrigger flag =========
	    strng = (*array)[i++]->GetName();
            if (version<=200906160)
	    {
		delete array;
		continue;
	    }
	    if (strng.Contains("???"))
                strng="n/a";

            sumtriggerflagkey = serv.QueryKeyOfName("SumTriggerFlag", strng);
            if (sumtriggerflagkey<0)
            {
                delete array;
                continue;
            }
        }

        Int_t l3triggerkey=1;
        Int_t cyclekey=1;
        Int_t pikey=1;
        Int_t workinggroupkey=1;
	Int_t proposalkey=1;
        Float_t l3trigrate=0;
        TString wheelpos1 = "NULL";
        TString wheelpos2 = "NULL";
        if (version>=200906250)
        {
            // for MAGIC 2: additional columns wheel_pos1 and ~2

            if (telnumber == 2)
            {
		strng = (*array)[i++]->GetName();
                wheelpos1 = strng.Data();
		strng = (*array)[i++]->GetName();
                wheelpos2 = strng.Data();
            }

	    // ========= Col 56: L3 trigger table =========
	    strng = (*array)[i++]->GetName();
            if (strng.Contains("???") || strng.Contains("na") || strng.Contains("Unknown"))
                strng="n/a";

            l3triggerkey = serv.QueryKeyOfName("L3TriggerTable", strng);
            if (l3triggerkey<0)
            {
                delete array;
                continue;
            }

            // ========= Col 57: L3 trigger rate =========
	    strng = (*array)[i++]->GetName();
            l3trigrate = atof(strng.Data());

            // ========= Col 58: Cycle =========
	    strng = (*array)[i++]->GetName();
            if (strng.Contains("???") || strng.Contains("na"))
                strng="n/a";

            cyclekey = serv.QueryKeyOfName("Cycle", strng);
            if (cyclekey<0)
            {
                delete array;
                continue;
            }

            // ========= Col 59: PI =========
	    strng = (*array)[i++]->GetName();
            if (strng.Contains("???") || strng.Contains("na"))
                strng="n/a";

            pikey = serv.QueryKeyOfName("PI", strng);
            if (pikey<0)
            {
                delete array;
                continue;
            }

            // ========= Col 60: Working group =========
	    strng = (*array)[i++]->GetName();
            if (strng.Contains("???") || strng.Contains("na"))
                strng="n/a";

            workinggroupkey = serv.QueryKeyOfName("WorkingGroup", strng);
            if (workinggroupkey<0)
            {
                delete array;
                continue;
            }

            // ========= Col 61: Proposal =========
	    strng = (*array)[i++]->GetName();
            if (strng.Contains("???") || strng.Contains("na"))
                strng="n/a";

            proposalkey = serv.QueryKeyOfName("Proposal", strng);
            if (proposalkey<0)
            {
                delete array;
                continue;
            }
        }
	delete array;


        // ================================================================
        // ========== Data read from file now access the database =========
        // ================================================================

        //assemble the query that is needed to insert the values of this run
        TString query;
        query += Form("fTelescopeNumber=%d, ", telnumber);
        query += Form("fRunNumber=%d, ",       runnumber);
        query += Form("fFileNumber=%d, ",      filenumber);
        query += Form("fRunTypeKEY=%d, ",      runtype);
        query += Form("fProjectKEY=%d, ",      projkey);
        query += Form("fSourceKEY=%d, ",       sourcekey);
        query += Form("fNumEvents=%d, ",       evtno);
        query += Form("fRunStart=\"%s %s\", ", startdate.Data(), starttime.Data());
        query += Form("fRunStop=\"%s %s\", ",  stopdate.Data(),  stoptime.Data());
        query += Form("fL1TriggerTableKEY=%d, ",    l1triggerkey);
        query += Form("fL2TriggerTableKEY=%d, ",    l2triggerkey);
        query += Form("fTestFlagKEY=%d, ",          testflagkey);
        query += Form("fCalibrationScriptKEY=%d, ", calibrationscriptkey);
        query += Form("fTriggerDelayTableKEY=%d, ", triggerdelaytablekey);
        query += Form("fDiscriminatorThresholdTableKEY=%d, ", dttablekey);
        query += Form("fLightConditionsKEY=%d, ",   lightcondkey);
        query += Form("fHvSettingsKEY=%d, ",        hvkey);
        query += Form("fObservationModeKEY=%d, ",   observationmodekey);
        query += Form("fSumTriggerFlagKEY=%d, ",    sumtriggerflagkey);
        query += Form("fL3TriggerTableKEY=%d, ",    l3triggerkey);
        query += Form("fCycleKEY=%d, ",             cyclekey);
        query += Form("fPIKEY=%d, ",                pikey);
        query += Form("fWorkingGroupKEY=%d, ",      workinggroupkey);
        query += Form("fProposalKEY=%d, ",          proposalkey);
        if (!TMath::IsNaN(zd) && TMath::Finite(zd))
            query += Form("fZenithDistance=%d, ", TMath::Nint(zd));
        if (!TMath::IsNaN(az) && TMath::Finite(az))
            query += Form("fAzimuth=%d, ", TMath::Nint(az));
        if (!TMath::IsNaN(storerate) && TMath::Finite(storerate))
            query += Form("fDaqStoreRate=%d, ", TMath::Nint(storerate));
        if (!TMath::IsNaN(daqrate) && TMath::Finite(daqrate))
            query += Form("fDaqTriggerRate=%d, ", TMath::Nint(daqrate));
        if (!TMath::IsNaN(trigrate) && TMath::Finite(trigrate))
            query += Form("fMeanTriggerRate=%d, ", TMath::Nint(trigrate));
        if (!TMath::IsNaN(l2prrate) && TMath::Finite(l2prrate))
            query += Form("fL2RatePresc=%d, ", TMath::Nint(l2prrate));
        if (!TMath::IsNaN(l2uprate) && TMath::Finite(l2uprate))
            query += Form("fL2RateUnpresc=%d, ", TMath::Nint(l2uprate));
        if (!TMath::IsNaN(l3trigrate) && TMath::Finite(l3trigrate))
            query += Form("fL3TriggerRate=%d, ", TMath::Nint(l3trigrate));
	query += Form("fWheelPos1=%s, ", wheelpos1.Data());
	query += Form("fWheelPos2=%s, ", wheelpos2.Data());
        query += "fMagicNumberKEY=1, fExcludedFDAKEY=1";

        cnt++;

        //send query, add dataset to DB
        if (serv.Insert("RunData", query)==kFALSE)
            return -1;

        TString query2=Form("fTelescopeNumber=%d, fRunNumber=%d, fFileNumber=%d, "
                            "fPriority=%d, fTimingCorrection='1970-01-01 00:00:00', fCompmux='1970-01-01 00:00:00'",
                            telnumber, runnumber, filenumber, runnumber);
        if (testflagkey==3)
            query2+=" , fDataCheckDone='1970-01-01 00:00:00'";

        //create entry in table RunProcessStatus for this runnumber
        if (serv.Insert("RunProcessStatus", query2)==kFALSE)
            return -1;
    }

    return cnt;
}

// This tool will work from Period017 (2004_05_17) on...
int filldotrun(const TString path="/home/lapalma/transfer/ccdata", Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    cout << "filldotrun" << endl;
    cout << "----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "Search Path: " << path << endl;
    cout << endl;

    serv.SetIsDummy(dummy);

    if (path.EndsWith(".run"))
    {
        cout << path(TRegexp("CC_.*.run", kFALSE)) << flush;
        Int_t n = insert(serv, dummy, path);
        cout << " <" << n << "> " << (dummy?"DUMMY":"") << endl;

        return n<0 ? 2 : 1;
    }

    MDirIter Next(path, "CC_*.run", -1);
    while (1)
    {
        TString name = Next();
        if (name.IsNull())
            break;

        cout << " * " << name(TRegexp("CC_.*.run", kFALSE)) << endl;
        Int_t n = insert(serv, dummy, name);
        cout << "   <" << n << "> " << (dummy?"DUMMY":"") << endl;

        if (n<0)
            return 2;
    }

    return 1;
}
