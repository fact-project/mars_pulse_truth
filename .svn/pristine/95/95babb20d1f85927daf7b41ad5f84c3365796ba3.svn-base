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
!   Author(s): Thomas Bretz, 06/2007 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Daniela Dorner, 11/2005 <mailto:dorner@astro.uni-wuerzburg.de>
!   Author(s): Daniel Hoehne, 06/2008 <mailto:hoehne@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// fillcamera.C
// ============
//
// This macro is used to read the camera-output files and fill the values
// into the MC database.
// You have to specify the path where to search for the camera files.
// As default the dummy mode is kTRUE, so nothing will be filled but only
// the mysql orders are printed. To really fill the db, set dummy to kFALSE.
//
// Make sure, that database and password are corretly set in a resource
// file called sql.rc and the resource file is found.
// In order not to confuse the MC processing with the data processing,
// both branches will be processed in two different Mars directories,
// e.g. Mars-2.0 for data and Mars.MC for MCs.
//
// Returns 2 in case of failure and 1 in case of success.
// Returns 0 in case of no connection to the db.
//
//
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>

#include <TEnv.h>
#include <TRegexp.h>
#include <TObjectTable.h>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

//#include <TH1.h>

#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSystem.h>

#include "MSQLServer.h"
#include "MSQLMagic.h"

#include "MMcRunHeader.hxx"
#include "MMcConfigRunHeader.h"
#include "MMcCorsikaRunHeader.h"
#include "MMcFadcHeader.hxx"
#include "MMcEvtBasic.h"
#include "MRawRunHeader.h"
//#include "MRawEvtData.h"
#include <MDirIter.h>

#include <math.h>

using namespace std;

int Process(MSQLMagic &serv, TString fname, Bool_t dummy)
{
    //
    // Read file
    //
    TFile file(fname, "READ");
    if (!file.IsOpen())
    {
        cout << "ERROR - Could not find file " << fname << endl;
        return 2;
    }

    //
    // Get tree RunHeaders from file
    //
    TTree *tree = dynamic_cast<TTree*>(file.Get("RunHeaders"));
    if (!tree)
    {
        cout << "ERROR - Tree RunHeaders not found in file " << fname << endl;
        return 2;
    }

    //
    // Get branch MMcCorsikaRunHeader from tree RunHeaders
    //
/*    TBranch *b1 = tree->GetBranch("MMcCorsikaRunHeader.");
    if (!b1)
    {
        cout << "ERROR - Branch MMcCorsikaRunHeader. not found in file " << fname << endl;
        return 2;
    }
    MMcCorsikaRunHeader *runheader1 = 0;
    b1->SetAddress(&runheader1);
*/
    MMcCorsikaRunHeader *runheader1 = 0;
    tree->SetBranchAddress("MMcCorsikaRunHeader.", &runheader1);

    //
    // Get branch MMcConfigRunHeader from tree RunHeaders
    //
    MMcConfigRunHeader *runheader2 = 0;
    tree->SetBranchAddress("MMcConfigRunHeader.", &runheader2);

    //
    // Get branch MMcRunHeader from tree RunHeaders
    //
    MMcRunHeader *runheader3 = 0;
    tree->SetBranchAddress("MMcRunHeader.", &runheader3);

    //
    // Get branch MMcFadcRunHeader from tree RunHeaders
    //
    MMcFadcHeader *fadcheader = 0;
    tree->SetBranchAddress("MMcFadcHeader.", &fadcheader);

    //
    // Get branch MRawRunHearder from tree RunHeaders
    //
    MRawRunHeader *rawheader = 0;
    tree->SetBranchAddress("MRawRunHeader.", &rawheader);

    if (tree->GetEntry(0)<0)
    {
        cout << "ERROR - No entry found in tree RunHeaders of file: " << fname << endl;
        return 2;
    }

    //
    // Get tree Events from file
    //
    TTree *tree2 = dynamic_cast<TTree*>(file.Get("Events"));
    if (!tree2)
    {
        cout << "ERROR - Tree Events not found in file " << fname << endl;
        return 2;
    }

    //
    // Get branch MMcEvtBasic from tree Events
    //
    MMcEvtBasic *evtbasic = 0;
    tree2->SetBranchAddress("MMcEvtBasic.", &evtbasic);

    if (tree2->GetEntry(0)<0)
    {
        cout << "ERROR - No entry found in tree Events of file: " << fname << endl;
        return 2;
    }


    // Definition of values
    Double_t misptx       = runheader2->GetMissPointingX();
    Double_t mispty       = runheader2->GetMissPointingY();
    Double_t point        = runheader2->GetPointSpread();
    Double_t pointspreadx = runheader2->GetPointSpreadX();
    Double_t addspotsize;
    if (pointspreadx < 0.25)
    {
        addspotsize = 0;
    }
    else
    {
        addspotsize = TMath::Sqrt(pointspreadx*pointspreadx - 0.25);
    }
    Double_t realpsf      = TMath::Sqrt(pointspreadx*pointspreadx + 0.25);
    Double_t pointspready = runheader2->GetPointSpreadY();
    Double_t coneai       = runheader1->GetViewconeAngleInner();
    Double_t coneao       = runheader1->GetViewconeAngleOuter();
    Double_t spectrum     = runheader1->GetSlopeSpec();
    Double_t tmin         = runheader3->GetShowerThetaMin();
    Double_t tmax         = runheader3->GetShowerThetaMax();
    UInt_t   corsvers     = runheader3->GetCorsikaVersion();
    UInt_t   reflvers     = runheader3->GetReflVersion();
    UInt_t   camvers      = runheader3->GetCamVersion();
    UInt_t   numsimshow   = runheader3->GetNumSimulatedShowers();
    UInt_t   numevents    = tree2->GetEntries();

    // some calculations: round the given values
    pointspreadx = TMath::Floor(pointspreadx*100+0.5)/100;
    addspotsize  = TMath::Floor(addspotsize*100+0.5)/100;
    realpsf      = TMath::Floor(realpsf*100+0.5)/100;
    pointspready = TMath::Floor(pointspready*100+0.5)/100;
    point        = TMath::Floor(point*10+0.5)/10;
    coneai       = TMath::Floor(coneai*10+0.5)/10;
    coneao       = TMath::Floor(coneao*10+0.5)/10;
    spectrum     = TMath::Floor(spectrum*10+0.5)/10;

    // Definition of strings
    TString numslices = Form("%5.0i", rawheader->GetNumSamplesHiGain());
    TString elow      = Form("%5.1f", runheader1->GetELowLim());
    TString eupp      = Form("%5.1f", runheader1->GetEUppLim());
    TString slope     = Form("%5.1f", spectrum);
    TString wobble    = Form("%5.0f", runheader1->GetWobbleMode());
    TString corsika1  = Form("%5.0f", runheader1->GetCorsikaVersion());
    TString coneanglei = Form("%5.1f", coneai);
    TString coneangleo = Form("%5.1f", coneao);
    TString atmomodel = Form("%5.1f", runheader1->GetAtmosphericModel());
    TString psf       = Form("%5.1f", point);
    TString psfx      = Form("%5.2f", pointspreadx);
    TString addspot   = Form("%5.2f", addspotsize);
    TString rpsf      = Form("%5.2f", realpsf);
    TString psfy      = Form("%5.2f", pointspready);
    TString psfadd    = Form("%5.2f", TMath::Hypot(runheader2->GetPointSpreadX(), runheader2->GetPointSpread()));
    Int_t mirr = runheader2->GetMirrorFraction();
    TString mirrfrac  = Form("%5.2f", runheader2->GetMirrorFraction());
    if (mirr < 0 )
        mirrfrac = "NULL";
    TString misx      = Form("%5.2f", misptx);
    TString misy      = Form("%5.2f", mispty);
    TString reflector = Form("%5.0i", reflvers);
    TString camera    = Form("%5.0i", camvers);
    Int_t impact = runheader3->GetImpactMax();
    TString imax      = Form("%5.1f", runheader3->GetImpactMax());
    if (impact < 0)
        imax = "NULL";
    TString numphe    = Form("%7.3f", runheader3->GetNumPheFromDNSB());
    TString pmin      = Form("%5.2f", runheader3->GetShowerPhiMin());
    TString pmax      = Form("%5.2f", runheader3->GetShowerPhiMax());
    TString numss     = Form("%7.0i", numsimshow);
    TString thetamin  = Form("%5.2f", tmin);
    TString thetamax  = Form("%5.2f", tmax);
    TString ped       = Form("%5.1f", fadcheader->GetPedestal(1));
    TString low2high  = Form("%5.1f", fadcheader->GetLow2HighGain());
    TString amplfadc  = Form("%5.2f", fadcheader->GetAmplitud());
    TString amplfadco = Form("%5.2f", fadcheader->GetAmplitudOuter());
    TString enoise    = Form("%5.1f", fadcheader->GetElecNoise(1));
    TString dnoise    = Form("%5.1f", fadcheader->GetDigitalNoise(1));
    TString numevt    = Form("%7.0i", numevents);
    TString partid    = Form("%5.0f", evtbasic->GetPartId());
    TString partname  = evtbasic->GetParticleName();

    // get the number of triggers
    UInt_t ntrig = (UInt_t)tree2->Draw("Length$(MRawEvtData.fHiGainPixId.fN)", "MRawEvtData.fHiGainPixId.fN!=0", "goff");
    if (ntrig<0)
    {
        cout << "ERROR - Evaluating triggered events in file: " << fname << endl;
        return 2;
    }
    TString numtrig   = Form("%7.0i", ntrig);
//    TH1I h("myhist", "", 1, -0.5, 0.5);
//    tree2->Draw("MRawEvtData.GetNumPixels()>>myhist", "", "goff");
//    h.SetDirectory(0);
//    TString numtrig   = Form("%7.0i", TMath::Nint(h.GetBinContent(2)));

    UInt_t nsumtrig = (UInt_t)tree2->Draw("Length$(MRawEvtHeader.fTrigPattern[0])","(MRawEvtHeader.fTrigPattern[0]==0xffffdfdf) || (MRawEvtHeader.fTrigPattern[0]==0xffffdede)","goff");
    if (nsumtrig<0)
    {
        cout << "ERROR - Evaluating sumtrigger events in file: " << fname << endl;
        return 2;
    }
    TString numsumtrig   = Form("%7.0i", nsumtrig);
    if (nsumtrig==0)
        numsumtrig = "0";

    // Determine observation mode, fake wobble means On mode with mispointing
    TString wobblemod="Wobble";
    if (atoi(wobble)==0)
        wobblemod = misptx == 0 && mispty == 0 ? "On" : "Fake Wobble";
    if (atoi(coneangleo)>0)
        wobblemod = "Diffuse";

    //get the psf value for the path for linking
    Float_t pointsfuncx = pointspreadx*10;
    pointsfuncx         = TMath::Floor(pointsfuncx*10+0.5)/10;
    Int_t pointsfx      = TMath::Nint(pointsfuncx);

    Float_t aspotsize   = addspotsize*10;
    aspotsize           = TMath::Floor(aspotsize*10+0.5)/10;
    Int_t ass           = TMath::Nint(aspotsize);

    Float_t realpointsf = realpsf*10;
    realpointsf         = TMath::Floor(realpointsf*10+0.5)/10;
    Int_t rpointsf = TMath::Nint(realpointsf);


    // Get zbin
    Float_t zBin=TMath::Nint((1-((TMath::Cos(tmin*TMath::Pi()/180)+TMath::Cos(tmax*TMath::Pi()/180))/2))*100);
    Float_t zBinmin=TMath::Nint((1-(TMath::Cos(tmin*TMath::Pi()/180)))*100);
    Float_t zBinmax=TMath::Nint((1-(TMath::Cos(tmax*TMath::Pi()/180)))*100);
    Int_t zBin2=TMath::Nint(zBin);
    Int_t zBin2Min=TMath::Nint(zBinmin);
    Int_t zBin2Max=TMath::Nint(zBinmax);

    // Definition of keys
    Int_t corsikakey;
    Int_t reflectorkey;
    Int_t camerakey;
    Int_t observationkey;
    Int_t particlekey;
    Int_t atmomodelkey;
    Int_t amplfadckey;
    Int_t psfkey;
    Int_t spotsizekey;
    Int_t viewconeangleokey;
    Int_t spectrumkey;
    Int_t triggerkey;

    // Definition of variables to be filled in the db
    TString vars;

    // Separation of ped/cal and data runs by checking the value pointspread
    // ped/cal runs
    if(point==-1)
    {
        cout << endl;
        cout << "psf value: " << point << " -> MC P or C run." << endl;
        cout << endl;
        cout << "--- From File ---" << endl;
        cout << endl;
        cout << "Energy Range    " << elow << " < E < " << eupp      << endl;
        cout << "SpectralIndex   " << slope     << endl;
        cout << "CorsikaVer      " << corsika1  << endl;
        cout << "ParticleId      " << partid    << endl;
        cout << "ParticleName    "  << partname  << endl;
        cout << "PointSpread     " << psf       << endl;
        cout << "NumSimShowers   " << numsimshow << endl;
        cout << "ImpactMax       " << imax      << endl;
        cout << "NumEvents       " << numevt    << endl;
        cout << "NumTriggers     " << numtrig   << endl;
        cout << "NumSumTriggers  " << numsumtrig   << endl;
        cout << "NumPheFromDNSB  " << numphe    << endl;
        cout << "ViewconeAngleInner  " << coneanglei << endl;
        cout << "ViewconeAngleOuter  " << coneangleo << endl;
        cout << "AtmosphericModel " << atmomodel << endl;
        cout << "Pedestal        " << ped       << endl;
        cout << "Low2HighGain    " << low2high  << endl;
        cout << "AmplitudeFADC   " << amplfadc  << endl;
        cout << "AmplFADCOuter   " << amplfadco << endl;
        cout << "ElecNoise       " << enoise    << endl;
        cout << "DigiNoise       " << dnoise    << endl;
        cout << "Num HiGainSlices " << numslices << endl;
        cout << "PhiMin          " << pmin      << endl;
        cout << "PhiMax          " << pmax      << endl;
        cout << "ThetaMin        " << thetamin  << endl;
        cout << "ThetaMax        " << thetamax  << endl;
        cout << "Zenith range    " << tmin << " to " << tmax << endl;
        cout << "MirrorFraction  " << mirrfrac << endl;
        cout << endl;
        cout << endl;
        cout << "P/C run: there will be fixed values inserted into the db for the following parameters:" << endl;
        cout << "ObservationMode " << wobblemod   << " -> " << "n/a" <<endl;
        cout << "ReflVer         " << reflvers    << " -> " << "n/a" <<endl;
        cout << "CamVer          " << camvers     << " -> " << "n/a" <<endl;
        cout << "PointSpreadX    " << psfx        << " -> " << "0" <<endl;
        cout << "Add. SpotSize   " << addspot     << " -> " << "0" <<endl;
        cout << "Real PSF        " << rpsf        << " -> " << "0" <<endl;
        cout << "PointSpreadY    " << psfy        << " -> " << "0" <<endl;
        cout << "MispointingX    " << misx        << " -> " << "0" <<endl;
        cout << "MispointingY    " << misy        << " -> " << "0" <<endl;
        cout << endl;
        cout << "--- key's from mcdb tables ---" << endl;
        cout << endl;

        corsikakey     = 1;
        reflectorkey   = 1;
        camerakey      = 1;
        observationkey = 1;
        particlekey    = 1;
        atmomodelkey   = serv.QueryKeyOfName("AtmosphericModel", atmomodel.Data());
        amplfadckey    = serv.QueryKeyOfName("AmplFadc", amplfadc.Data());
        viewconeangleokey = serv.QueryKeyOfName("ViewconeAngleO", coneangleo.Data());
        spectrumkey    = 1;
        triggerkey = nsumtrig == 0 ? 2 : 3;
        psfkey         = 1;
        spotsizekey    = 1;

        if (corsvers==0)
        {
            cout << "CorsikaVersion not available" << endl;
        }
        else
        {
            corsikakey = serv.QueryKeyOfName("CorsikaVersion", corsika1.Data());
        }
        if (reflvers==0)
        {
            cout << "ReflectorVersion not available" << endl;
        }
        else
        {
            reflectorkey = serv.QueryKeyOfName("ReflectorVersion", reflector.Data());
        }
        if (camvers==0)
        {
            cout << "CameraVersion not available" << endl;
        }
        else
        {
            camerakey = serv.QueryKeyOfName("CameraVersion", camera.Data());
        }

        cout << "corsikakey:     " << corsikakey << endl;
        cout << "reflectorkey:   " << reflectorkey << endl;
        cout << "camerakey:      " << camerakey << endl;
        cout << "observationkey: " << observationkey << endl;
        cout << "particlekey:    " << particlekey << endl;
        cout << "atmomodelkey:   " << atmomodelkey << endl;
        cout << "amplfadckey:    " << amplfadckey << endl;
        cout << "psfkey:         " << psfkey << endl;
        cout << "spotsizekey:    " << spotsizekey << endl;
        cout << "coneangleokey:  " << viewconeangleokey << endl;
        cout << "spectrumkey:    " << spectrumkey << endl;
        cout << "triggerkey:     " << triggerkey << endl;
        cout << endl;

        // For ped/cal: fixed values for psf and mispointing
        vars = Form("fPointSpreadX=0, fAdditionalSpotSize=0, fRealPointSpread=0, fPointSpreadY=0, fMissPointingX=0, fMissPointingY=0, ");

    }
    // Data runs
    else
    {

        cout << endl;
        cout << "psf value: " << point << " -> MC data run." << endl;
        cout << endl;
        cout << "--- From File ---" << endl;
        cout << endl;
        cout << "Energy Range    " << elow << " < E < " << eupp      << endl;
        cout << "SpectralIndex   " << slope     << endl;
        cout << "ObservationMode " << wobblemod << endl;
        cout << "CorsikaVer      " << corsika1  << endl;
        cout << "ReflVer         " << reflector << endl;
        cout << "CamVer          " << camera    << endl;
        cout << "ParticleId      " << partid    << endl;
        cout << "ParticleName    " << partname  << endl;
        cout << "PointSpread     " << psf       << endl;
        cout << "PointSpreadX    " << pointspreadx << endl;
        cout << "Add. SpotSize   " << addspot   << endl;
        cout << "Real PSF        " << rpsf      << endl;
        cout << "PointSpreadXY   " << psfx      << " /" << psfy << endl;
//        cout << "AdditionPSF     " << psfadd << endl;
        cout << "MispointingXY   " << misx << " /" << misy <<endl;
        cout << "NumSimShowers   " << numss     << endl;
        cout << "ImpactMax       " << imax      << endl;
        cout << "NumEvents       " << numevt    << endl;
        cout << "NumTriggers     " << numtrig   << endl;
        cout << "NumSumTriggers  " << numsumtrig   << endl;
        cout << "NumPheFromDNSB  " << numphe    << endl;
        cout << "ViewconeAngleInner  " << coneanglei << endl;
        cout << "ViewconeAngleOuter  " << coneangleo << endl;
        cout << "AtmosphericModel " << atmomodel << endl;
        cout << "Pedestal        " << ped       << endl;
        cout << "Low2HighGain    " << low2high  << endl;
        cout << "AmplitudeFADC   " << amplfadc  << endl;
        cout << "AmplFADCOuter   " << amplfadco << endl;
        cout << "ElecNoise       " << enoise    << endl;
        cout << "DigiNoise       " << dnoise    << endl;
        cout << "Num HiGainSlices " << numslices << endl;
        cout << "PhiMin          " << pmin      << endl;
        cout << "PhiMax          " << pmax      << endl;
        cout << "ThetaMin        " << thetamin  << endl;
        cout << "ThetaMax        " << thetamax  << endl;
        cout << "Zenith range    " << tmin << " to " << tmax << endl;
        cout << "MirrorFraction  " << mirrfrac << endl;
        cout << endl;
        cout << endl;
        cout << "--- key's from mcdb tables ---" << endl;
        cout << endl;

        corsikakey     = 1;
        reflectorkey   = 1;
        camerakey      = 1;
        observationkey = serv.QueryKeyOfName("ObservationMode", wobblemod.Data());
        particlekey    = serv.QueryKeyOfName("MCParticle", partname.Data());
        atmomodelkey   = serv.QueryKeyOfName("AtmosphericModel", atmomodel.Data());
        amplfadckey    = serv.QueryKeyOfName("AmplFadc", amplfadc.Data());
        viewconeangleokey = serv.QueryKeyOfName("ViewconeAngleO", coneangleo.Data());
        psfkey         = serv.QueryKeyOfName("PSF", rpsf);
        spotsizekey    = serv.QueryKeyOfName("AddSpotSize", addspot);
        spectrumkey    = serv.QueryKeyOfName("Spectrum", slope);
        triggerkey = nsumtrig == 0 ? 2 : 3;


        if (corsvers==0)
        {
            cout << "CorsikaVersion not available" << endl;
        }
        else
        {
            corsikakey = serv.QueryKeyOfName("CorsikaVersion", corsika1.Data());
        }
        if (reflvers==0)
        {
            cout << "ReflectorVersion not available" << endl;
        }
        else
        {
            reflectorkey = serv.QueryKeyOfName("ReflectorVersion", reflector.Data());
        }
        if (camvers==0)
        {
            cout << "CameraVersion not available" << endl;
        }
        else
        {
            camerakey = serv.QueryKeyOfName("CameraVersion", camera.Data());
        }

        cout << "corsikakey:     " << corsikakey << endl;
        cout << "reflectorkey:   " << reflectorkey << endl;
        cout << "camerakey:      " << camerakey << endl;
        cout << "observationkey: " << observationkey << endl;
        cout << "particlekey:    " << particlekey << endl;
        cout << "atmomodelkey:   " << atmomodelkey << endl;
        cout << "amplfadckey:    " << amplfadckey << endl;
        cout << "psfkey:         " << psfkey << endl;
        cout << "spotsizekey:    " << spotsizekey << endl;
        cout << "coneangleokey:  " << viewconeangleokey << endl;
        cout << "spectrumkey:    " << spectrumkey << endl;
        cout << "triggerkey:     " << triggerkey << endl;
        cout << endl;

        // For data runs the values are taken from the file
        vars = Form("fPointSpreadX=%5.2f, fAdditionalSpotSize=%5.2f, fRealPointSpread=%5.2f, fPointSpreadY=%5.2f, fMissPointingX=%s, fMissPointingY=%s, ",
                    pointspreadx, addspotsize, realpsf, pointspready, misx.Data(), misy.Data());

    }

    // Query maximum run number and add 1 to it, if no runnumber is found, start with 1
    TString query(Form("SELECT IF (isnull(MAX(MCRunData.fRunNumber)+1),1,MAX(MCRunData.fRunNumber)+1) FROM MCRunData;"));
    //        TString query(Form("SELECT MCRunData.fRunNumber, IF (isnull(MAX(MCRunData.fRunNumber)+1),1,MAX(MCRunData.fRunNumber)+1), "
    //                           "fFileName FROM MCRunData LEFT JOIN "
    //                           "MCRunProcessStatus ON MCRunData.fRunNumber=MCRunProcessStatus.fRunNumber "
    //                           "where fFileName=\"%s\" group by MCRunData.fRunNumber;", fname.Data()));

    TSQLResult *res = serv.Query(query);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query << endl;
        return 2;
    }

    TSQLRow *row = 0;
    row = res->Next();
    if (!row)
        return 2;
    TString num=(*row)[0];
    Int_t RunNum=atoi(num.Data());

    delete res;
//    delete row;
    // Query run number and filename from MCRunProcessStatus
    TString query2(Form("SELECT fRunNumber, fFileName from MCRunProcessStatus where fFileName=\"%s\"", fname.Data()));
    res = serv.Query(query2);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query2 << endl;
        return 2;
    }

    row = res->Next();
    // If query gives no entry for the filename the file will be inserted into the db
    if (!row)
    {
        cout << "No entry in query result: " << query2 << endl;
        cout << "--------" << endl;
        cout << "RunNum: " << RunNum << endl;
        cout << "File: " << fname << endl;
        cout << endl;

        vars +=
            Form("fELowLim=%s, fEUppLim=%s, fSlopeSpec=%5.2f, "
                 "fImpactMax=%s, fNumSimulatedShowers=%d, fNumEvents=%d, "
                 "fNumPheFromDNSB=%s, fZBin=%d, fZBinMin=%d, fZBinMax=%d, fThetaMin=%s, "
                 "fThetaMax=%s, fPhiMin=%s, fPhiMax=%s, fPointSpread=%s, "
                 "fPedesMean=%s, fLow2HighGain=%s, "
                 "fAmplFadcInner=%s, fAmplFadcOuter=%s, ",
                 elow.Data(), eupp.Data(), spectrum,
                 imax.Data(), numsimshow, numevents,
                 numphe.Data(), zBin2, zBin2Min, zBin2Max, thetamin.Data(),
                 thetamax.Data(), pmin.Data(), pmax.Data(), psf.Data(),
                 ped.Data(), low2high.Data(),
                 amplfadc.Data(), amplfadco.Data());
        vars +=
            Form("fElectricNoise=%s, "
                 "fDigitalNoise=%s, fRunNumber=%d, "
                 "fNumSlices=%s, fCorsikaVersionKEY=%d, "
                 "fReflectorVersionKEY=%d, fCameraVersionKEY=%d, "
                 "fObservationModeKEY=%d, fMCParticleKEY=%d, "
                 "fNumTriggers=%d, fNumSumTriggers=%d, fViewconeAngleInner=%5.2f, fViewconeAngleOuter=%5.2f, "
                 "fAtmosphericModelKEY=%d, fAmplFadcKEY=%d, fAddSpotSizeKEY=%d, "
                 "fPSFKEY=%d, fViewconeAngleOKEY=%d, fSpectrumKEY=%d, fTriggerFlagKEY=%d ",
                 enoise.Data(),
                 dnoise.Data(), RunNum,
                 numslices.Data(), corsikakey,
                 reflectorkey, camerakey,
                 observationkey, particlekey,
                 ntrig, nsumtrig, coneai, coneao,
                 atmomodelkey, amplfadckey, spotsizekey,
                 psfkey, viewconeangleokey, spectrumkey, triggerkey);

        TString vars2 =
            Form("fRunFilled=Now(), fFileName=\"%s\", fRunNumber=%d, fPriority=%d ",
                 fname.Data(), RunNum, RunNum);


        if (!serv.ExistStr("fRunNumber", "MCRunData", Form("%d", RunNum)))
        {
            if (!serv.Insert("MCRunData", vars))
                return 2;
        }
        else
            if (!serv.Update("MCRunData", vars, Form("fRunNumber=%d", RunNum)))
                return 2;

//MarsVersion miteintragen bei Prozessierung

        if (!serv.ExistStr("fRunNumber", "MCRunProcessStatus", Form("%d", RunNum)))
        {
            if (!serv.Insert("MCRunProcessStatus", vars2))
                return 2;
        }
        else
            if (!serv.Update("MCRunProcessStatus", vars2, Form("fRunNumber=%d", RunNum)))
                return 2;
    }
    // If the file is already filled into the db, the values will be updated
    else
    {
        TString run=(*row)[0];
        TString check=(*row)[1];
        if (check==fname)
        {
            RunNum=atoi(run.Data());
            cout << "File already inserted into db, do update " << endl;
            cout << "--------" << endl;
            cout << "RunNum: " << RunNum << endl;
            cout << "File: " << fname << endl;
            cout << endl;

            vars +=
                Form("fELowLim=%s, fEUppLim=%s, fSlopeSpec=%5.2f, "
                     "fImpactMax=%s, fNumSimulatedShowers=%d, fNumEvents=%d, "
                     "fNumPheFromDNSB=%s, fZBin=%d, fZBinMin=%d, fZBinMax=%d, fThetaMin=%s, "
                     "fThetaMax=%s, fPhiMin=%s, fPhiMax=%s, fPointSpread=%s, "
                     "fPedesMean=%s, fLow2HighGain=%s, "
                     "fAmplFadcInner=%s, fAmplFadcOuter=%s, ",
                     elow.Data(), eupp.Data(), spectrum,
                     imax.Data(), numsimshow, numevents,
                     numphe.Data(), zBin2, zBin2Min, zBin2Max, thetamin.Data(),
                     thetamax.Data(), pmin.Data(), pmax.Data(), psf.Data(),
                     ped.Data(), low2high.Data(),
                     amplfadc.Data(), amplfadco.Data());
            vars +=
                Form("fElectricNoise=%s, "
                     "fDigitalNoise=%s, fRunNumber=%d, "
                     "fNumSlices=%s, fCorsikaVersionKEY =%d, "
                     "fReflectorVersionKEY=%d, fCameraVersionKEY=%d, "
                     "fObservationModeKEY=%d, fMCParticleKEY=%d, "
                     "fNumTriggers=%d, fNumSumTriggers=%d, fViewconeAngleInner=%5.2f, fViewconeAngleOuter=%5.2f, "
                     "fAtmosphericModelKEY=%d, fAmplFadcKEY=%d, fAddSpotSizeKEY=%d, "
                     "fPSFKEY=%d, fViewconeAngleOKEY=%d, fSpectrumKEY=%d, fTriggerFlagKEY=%d ",
                     enoise.Data(),
                     dnoise.Data(), RunNum,
                     numslices.Data(), corsikakey,
                     reflectorkey, camerakey,
                     observationkey, particlekey,
                     ntrig, nsumtrig, coneai, coneao,
                     atmomodelkey, amplfadckey, spotsizekey,
                     psfkey, viewconeangleokey, spectrumkey, triggerkey);

            TString vars2 =
                Form("fRunFilled=Now(), fFileName=\"%s\", fRunNumber=%d, fPriority=%d ",
                     fname.Data(), RunNum, RunNum);


            if (!serv.ExistStr("fRunNumber", "MCRunData", Form("%d", RunNum)))
            {
                if (!serv.Insert("MCRunData", vars))
                    return 2;
            }
            else
                if (!serv.Update("MCRunData", vars, Form("fRunNumber=%d", RunNum)))
                    return 2;

            if (!serv.ExistStr("fRunNumber", "MCRunProcessStatus", Form("%d", RunNum)))
            {
                if (!serv.Insert("MCRunProcessStatus", vars2))
                    return 2;
            }
            else
                if (!serv.Update("MCRunProcessStatus", vars2, Form("fRunNumber=%d", RunNum)))
                    return 2;
        }
        //If none of the above options is true, something must be wrong
        else
        {
            cout << "ERROR - Filename in query (" << check << ") and processed filename (" << fname << ") should be the same!" << endl;
            return 2;
        }
    }
    delete res;
//    delete row;

    //link file geht nur fuer data files, cal und ped haben keine psf... muessen aber in alle verzeichnisse verlinkt werden
    //wieder mit if point 0.5/-1.0 else

//    TString query3(Form("SELECT fMCParticleName, fSpectrum, fFileName, fObservationModeKEY from MCRunData "
//                        "left join MCParticle on MCRunData.fMCParticleKEY=MCParticle.fMCParticleKEY "
//                        "left join Spectrum on Spectrum.fSpectrumKEY=MCRunData.fSpectrumKEY "
//                        "left join MCRunProcessStatus on MCRunProcessStatus.fRunNumber=MCRunData.fRunNumber "
//                        "where MCRunData.fRunNumber=%d;", RunNum));
    TString query3(Form("SELECT fMCParticleName, fSpectrum FROM MCRunData "
                        "LEFT JOIN MCParticle USING (fMCParticleKEY) "
//                        "LEFT JOIN ON MCRunData.fMCParticleKEY=MCParticle.fMCParticleKEY "
                        "LEFT JOIN Spectrum USING (fSpectrumKEY) "
//                        "LEFT JOIN Spectrum ON Spectrum.fSpectrumKEY=MCRunData.fSpectrumKEY "
                        "WHERE MCRunData.fRunNumber=%d;", RunNum));
    res = serv.Query(query3);
    if (!res)
    {
        cout << "ERROR - Query failed: " << query3 << endl;
        return 2;
    }

    row = res->Next();
    if (!row)
        return 2;


    // Definition of variables for linking the file
    TString rawpath;
    TString type = "D";  //P, C, D
    TString particle = (*row)[0];    //ParticleName abfragen
    TString spec     = (*row)[1];    //Spectrum.fSpectrum abfragen
    TString obsmode;             //wird ueber obskey praezisiert
    TString trigmode;            //wird ueber triggerkey praezisiert
    TString mkdir;
    TString link;
    TString linkname;
    TString RunNumber=Form("%08d",RunNum);

    delete res;
    rawpath = Form("/magic/montecarlo/rawfiles/19%02d/%02d/%02d", zBin2, amplfadckey, ass);
//    rawpath = Form("/home/hoehne/Analyse/TestMonteCarlo/montecarlo/rawfiles/19%02d/%02d/%02d", zBin2, amplfadckey, ass);
    mkdir = Form("mkdir -p -v %s", rawpath.Data());

    switch (observationkey)
    {
    case 1:
//        obsmode = ""; //evtl für ped u cal verwenden
        cout << "" << endl;
        cout << "Ped or Cal run -> no linking by fillcamera.C" << endl;
//        spec = "";
//        particle="MonteCarlo";
//        TRegexp pedcal("_._");
//        TString peca = fname(pedcal);
//        if (peca="_P_")
//            type="P";
//        if (peca="_C_")
//            type="C";
        break;
        //Problem: zbin und psf
        //19990101_00002_C_MonteCarlo_E.root

    case 2:
        obsmode = "W";
        break;

    case 3:
        obsmode = "";
        break;

    case 4:
        obsmode = "FW";
        break;

    case 5:
        obsmode = "Diff";
        break;

    default:
        cout << "ERROR - ObservationModeKEY wrong value" << endl;
        return 2;
    }

    TRegexp reg("_w.");
    TString add = fname(reg);
//    add = add.Data()+2;
    if (add=="_w+" || add=="_w-")
    {
        if (add=="_w+")
            obsmode += "1";
        else
            obsmode += "2";
    }

    switch(triggerkey)
    {
    case 2:
        obsmode += "";
        break;
    case 3:
        obsmode += "SUM";
        break;
    default:
        cout << "ERROR - TriggerFlagKEY wrong value" << endl;
        return 2;
    }

    if (observationkey!=1)
    {
        link = Form("ln -sv %s %s/19%02d%02d%02d_%s_%s_%s%s%s_E.root", fname.Data(), rawpath.Data(), zBin2, amplfadckey, ass, RunNumber.Data(), type.Data(), particle.Data(), spec.Data(), obsmode.Data());
        linkname = Form("%s/19%02d%02d%02d_%s_%s_%s%s%s_E.root", rawpath.Data(), zBin2, amplfadckey, ass, RunNumber.Data(), type.Data(), particle.Data(), spec.Data(), obsmode.Data());

        if (dummy==kFALSE)
        {
            gSystem->Exec(mkdir);
            gSystem->Exec(link);

            TString vars3 =
                Form("fRunLinked=Now(), fLinkName=\"%s\", fRunNumber=%d ",
                     linkname.Data(), RunNum);

            if (!serv.ExistStr("fRunNumber", "MCRunProcessStatus", Form("%d", RunNum)))
            {
                if (!serv.Insert("MCRunProcessStatus", vars3))
                    return 2;
            }
            else
                if (!serv.Update("MCRunProcessStatus", vars3, Form("fRunNumber=%d", RunNum)))
                    return 2;

        }
        else
        {
            cout << "" << endl;
            cout << "Dummy mode. The following commands would be executed:" << endl;
            cout << mkdir << endl;
            cout << link << endl;
            cout << "" << endl;
        }
    }
    else
    {
        cout << "" << endl;
        cout << "P/C Linking will be done by fillcamera script" << endl;
        cout << "" << endl;
    }

//    delete runheader1;
//    delete runheader2;
//    delete runheader3;
//    delete fadcheader;
//    delete rawheader;
//    delete evtbasic;



    return 1;

}
/*
int fillcamera(TString fname, Bool_t dummy=kTRUE)
{
    TEnv env("sql.rc");

    MSQLMagic serv(env);
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    serv.SetIsDummy(dummy);

    cout << endl;
    cout << "fillcamera" << endl;
    cout << "----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "File: " << fname << endl;
    cout << endl;

    return Process(serv, fname, dummy);
}
*/
int fillcamera(TString path="/magic/montecarlo/camera", Bool_t dummy=kTRUE)
{
//    TEnv env("mcsql.rc");
    TEnv env("sql.rc");

    MSQLMagic serv(env);
//    MSQLMagic serv("sql.rc");
    if (!serv.IsConnected())
    {
        cout << "ERROR - Connection to database failed." << endl;
        return 0;
    }

    serv.SetIsDummy(dummy);

    cout << endl;
    cout << "fillcamera" << endl;
    cout << "----------" << endl;
    cout << endl;
    cout << "Connected to " << serv.GetName() << endl;
    cout << "Search path: " << path << endl;
    cout << endl;


    TString fname;
    TString name;


    MDirIter Next(path,"*.root",-1);

    while(1)
    {
        TString name = Next();
        if (name.IsNull())
            break;
        fname=name;
        cout << endl;
        cout << endl;
        cout << "filename: " << fname << endl;

//        TObject::SetObjectStat(kTRUE);

        if (!Process(serv, name, dummy))
            return 2;
/*
        if (TObject::GetObjectStat())
        {
            TObject::SetObjectStat(kFALSE);
            gObjectTable->Print();
        }
*/
    }

    return 1;
}
