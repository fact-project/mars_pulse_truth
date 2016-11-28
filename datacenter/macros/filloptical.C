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
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// filloptical.C
// =============
//
// This macro is used to read the files from KVA containing the results from
// the optical observations.
// Remark: Running it from the commandline looks like this:
//   root -q -l -b filloptical.C+\(\"filename\"\,kFALSE\) 
//
// Make sure, that database and password are corretly set.
//
// Returns 1 in case of success.
// Returns 2 in case of invalid file or line in file.
// Returns 3 in case of a missing object name in the database.
// Returns 4 in case of a telescope line with more or less than 2 arguments
// Returns 5 in case of a timestamp line with more or less than 4 arguments
// Returns 6 in case of a object line with more or less than 10 arguments
//
///////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <fstream>

#include <TVector3.h>

#include <TRegexp.h>

#include <TSQLRow.h>
#include <TSQLResult.h>

#include "MDirIter.h"
#include "MSQLServer.h"
#include "MSQLMagic.h"

#include "MTime.h"
#include "MObservatory.h"
#include "MAstroSky2Local.h"

using namespace std;

//
// insert the entries from this optical data file into the db
//
int process(MSQLMagic &serv, TString fname)
{
    ifstream fin(fname);
    if (!fin)
    {
        cout << "Could not open file " << fname << endl;
        return 0;
    }

    TRegexp regexp("^20[0-9][0-9]-[0-1][0-9]-[0-3][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9].*$", kFALSE);

    Int_t numexp=0;
    Int_t numstars=0;

    TString timestamp;
    TString exposure;
    TString fitsfile;
    TString object;
    TString skylevel;
    TString fwhm;
    TString ellipticity;
    TString posangle;
    TString aperturer;
    TString mag;
    TString magerr;
    TString status;
    TString query;
    TString select;
    TString telescope;
    TString telname;
    TString ccd;
    TString filterband;
    TString filter;
    TString band;
    Double_t ra=0;
    Double_t dec=0;
    Double_t zd=0;
    MTime t;
    MObservatory obs(MObservatory::kMagic1);
    TVector3 v;

    if (fname(TRegexp("20[0-9][0-9]_[0-1][0-9]_[0-3][0-9]_.*_[RV][_]?[12]?[.]instr", kFALSE)).IsNull())
    {
        cout << "Found not valid file: " << fname << endl;
        return 2;
    }

    while (1)
    {
        TString line;
        line.ReadLine(fin);
        if (!fin)
        {
            cout << numstars << " objects inserted for this exposure. " << endl;
            break;
        }

        if (line.IsNull())
            continue;

        TObjArray *arr = line.Tokenize("  ");

        if (line=="KVA_Celestron_ST8  KVA_Celestron_R" ||
            line=="KVA_Celestron_ST8  KVA_Celestron_V" ||
            line=="Tuorla_ST1001E  Tuorla_R")
        {
            if (arr->GetEntries()!=2)
            {
                cout << "WARNING: Telescopeline with less or more than 2 arguments found in file " <<
                    fname(TRegexp("20[0-9][0-9]_[0-1][0-9]_[0-3][0-9]_.*_[RV][_]?[12]?[.]instr", kFALSE)) << endl;
                cout << "Line: " << line << endl;
                return 4;
            }
            telescope=(*arr)[0]->GetName();
            telname=telescope(0,telescope.First('_'));
            ccd=telescope(telescope.Last('_')+1, telescope.Length());
            filterband=(*arr)[1]->GetName();
            filter=filterband(0,filterband.Last('_'));
            band=filterband(filterband.Last('_')+1, filterband.Length());
            continue;
        }

        if (!line(regexp).IsNull())
        {
            if (arr->GetEntries()!=4)
            {
                cout << "WARNING: Timestampline with less or more than 4 arguments found in file " <<
                    fname(TRegexp("20[0-9][0-9]_[0-1][0-9]_[0-3][0-9]_.*_[RV][_]?[12]?[.]instr", kFALSE)) << endl;
                cout << "Line: " << line << endl;
                return 5;
            }
            numexp+=1;
            if (numstars)
                cout << numstars << " objects inserted for this exposure. " << endl;
            numstars=0;
            timestamp =Form("%s %s", (*arr)[0]->GetName(),(*arr)[1]->GetName());

            exposure   = (*arr)[2]->GetName();
            fitsfile = (*arr)[3]->GetName();
            t.SetSqlDateTime(timestamp.Data());
            continue;
        }
        else
        {
            if (arr->GetEntries()!=10)
            {
                cout << "WARNING: Objectline with less or more than 10 arguments found in file " <<
                    fname(TRegexp("20[0-9][0-9]_[0-1][0-9]_[0-3][0-9]_.*_[RV][_]?[12]?[.]instr", kFALSE)) << endl;
                cout << "Line: " << line << endl;
                return 6;
            }

            //calculation of zd if ra and dec are available
            if (numstars==0)
            {
                select="SELECT fRightAscension, fDeclination FROM Object WHERE ";
                select+=Form("fObjectName='%s/BL'", (*arr)[0]->GetName());

                TSQLResult *res = serv.Query(select);
                if (!res)
                    return 2;

                TSQLRow *row=res->Next();
                if (!row)
                {
                    cout << "Couldn't get ZD - Query failed: " << select << endl;
                    cout << "Position of the object is missing in the DB." << endl;
                    return 3;
                }
                else
                {
                    ra =atof((*row)[0]);
                    dec=atof((*row)[1]);
                    v.SetMagThetaPhi(1, TMath::Pi()/2-(dec*TMath::DegToRad()), ra*TMath::DegToRad()*15);
                    v *= MAstroSky2Local(t, obs);
                    zd = v.Theta()*TMath::RadToDeg();
                }
                delete res;
            }

            object      = Form("%s/%s", (*arr)[0]->GetName(),(*arr)[1]->GetName());
            skylevel    = (*arr)[2]->GetName();
            fwhm        = (*arr)[3]->GetName();
            ellipticity = (*arr)[4]->GetName();
            posangle    = (*arr)[5]->GetName();
            aperturer   = (*arr)[6]->GetName();
            mag         = (*arr)[7]->GetName();
            magerr      = (*arr)[8]->GetName();
            status      = (*arr)[9]->GetName();
            //make sure that no nonsense values enter the db
            if (skylevel.Contains("-"))
                skylevel = "NULL";
            if (ellipticity.Contains("-"))
                ellipticity = "NULL";
            if (fwhm.Contains("-") || !fwhm.IsFloat())
                fwhm     = "NULL";
            if (posangle.EndsWith("-"))
                posangle     = "NULL";
            if (!mag.IsFloat())
                mag      = "NULL";
            if (!magerr.IsFloat())
                magerr   = "NULL";
            numstars+=1;
        }
        delete arr;

        if (numstars!=0)
        {
            /*
            cout << numexp << "th exposure: star # " << numstars << endl;
            cout << " timestamp  : " << timestamp   << endl;
            cout << " exposure   : " << exposure    << endl;
            cout << " fitsfile   : " << fitsfile    << endl;
            cout << " object     : " << object      << endl;
            cout << " skylevel   : " << skylevel    << endl;
            cout << " fwhm       : " << fwhm        << endl;
            cout << " ellipticity: " << ellipticity << endl;
            cout << " posangle   : " << posangle    << endl;
            cout << " aperturer  : " << aperturer   << endl;
            cout << " mag        : " << mag << " +/- " << magerr << endl;
            cout << " status     : " << status      << endl << endl;
            */

            Int_t statuskey   = serv.QueryKeyOfName("Status", status.Data());
            Int_t objectkey   = serv.QueryKeyOfName("Object", object.Data());
            Int_t fitsfilekey = serv.QueryKeyOfName("FitsFile", fitsfile.Data());
            Int_t telkey      = serv.QueryKeyOfName("Telescope", telname.Data());
            Int_t ccdkey      = serv.QueryKeyOfName("CCD", ccd.Data());
            Int_t bandkey     = serv.QueryKeyOfName("Band", band.Data());
            Int_t filterkey   = serv.QueryKeyOfName("Filter", filter.Data());

            query=Form("fTimeStamp='%s', fExposure=%s, fFitsFileKEY=%d, "
                       "fObjectKEY=%d, fSkyLevel=%s, fFWHM=%s, "
                       "fEllipticity=%s, fPosAngle=%s, fApertureRadius=%s, "
                       "fInstrumentalMag=%s, fInstrumentalMagErr=%s, "
                       "fStatusKEY=%d, fCCDKEY=%d, fFilterKEY=%d, "
                       "fTelescopeKEY=%d, fBandKEY=%d, fZenithDistance=%.1f ",
                       timestamp.Data(), exposure.Data(), fitsfilekey,
                       objectkey, skylevel.Data(), fwhm.Data(),
                       ellipticity.Data(), posangle.Data(), aperturer.Data(),
                       mag.Data(), magerr.Data(), statuskey, ccdkey,
                       filterkey, telkey, bandkey, zd);

            /*
            if (ra==0 || dec==0 || zd==0)
                query+="NULL";
            else
                query+=Form("%.1f", zd);
            */
            if (serv.Insert("OpticalData", query)==kFALSE)
                return 2;
        }
    }

    cout << fname(TRegexp("20[0-9][0-9]_[0-1][0-9]_[0-3][0-9]_.*_[RV][_]?[12]?[.]instr", kFALSE))
        << ": " << setw(2) << numexp << " exposures." << endl << endl;

    return 1;
}

// --------------------------------------------------------------------------
//
// loop over all files in this path
//
int filloptical(TString path, Bool_t dummy=kTRUE)
{
    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    serv.SetIsDummy(dummy);

    cout << endl;
    cout << "filloptical" << endl;
    cout << "-----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "Search Path: " << path << endl;
    cout << endl;

    //get all runbook files in path
    if (path.EndsWith(".instr"))
        return process(serv, path);

    //fill entries for each optical data file
    MDirIter Next(path, "20[0-9][0-9]_[0-1][0-9]_[0-3][0-9]_*_R[_]?[12]?[.]instr", -1);
    while (1)
    {
        TString name = Next();
        if (name.IsNull())
            break;

        if (!process(serv, name))
            return 0;
    }

    return 1;
}
