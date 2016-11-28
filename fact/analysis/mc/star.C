#include <sstream>
#include <iostream>

#include "MLog.h"
#include "MLogManip.h"

#if !defined(__CINT__) || defined(__MAKECINT__)

#include "TH1F.h"
#include "TFile.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TLine.h"

#include "../mcore/DrsCalib.h"
#include "MDrsCalibration.h"
//#include "MLogManip.h"
#include "MExtralgoSpline.h"
#include "MSequence.h"
#include "MStatusArray.h"
#include "MHCamera.h"
#include "MJob.h"
#include "MWriteRootFile.h"
#include "MHCamera.h"
#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"
#include "MDirIter.h"
#include "MTaskList.h"
#include "MFDataPhrase.h"
#include "MArrayF.h"
#include "MBadPixelsTreat.h"
#include "MCalibrateDrsTimes.h"
#include "MHSectorVsTime.h"
#include "MHCamEvent.h"
#include "MExtractTimeAndChargeSpline.h"
#include "MFillH.h"
#include "MDrsCalibApply.h"
#include "MGeomApply.h"
#include "MContinue.h"
#include "MRawFitsRead.h"
#include "MEvtLoop.h"
#include "MParList.h"
#include "MStatusDisplay.h"
#include "MDrsCalibrationTime.h"
#include "MH3.h"
#include "MGeomCamFACT.h"
#include "MCalibrateFact.h"
#include "MParameters.h"
#include "MWriteAsciiFile.h"
#include "MFMagicCuts.h"

#include "MMuonSetup.h"
#include "MReadMarsFile.h"
#include "MHillasCalc.h"
#include "MHn.h"
#include "MMuonSearchParCalc.h"
#include "MMuonCalibParCalc.h"
#include "MBinning.h"
#include "MImgCleanStd.h"

#endif

using namespace std;

int star(const TString callistofile, TString outfile,
         TString displayfile, TString displaytitle,
         const TString txtfile,
         Double_t lvl1=4.0, Double_t lvl2=2.5, Double_t deltat = 17.5);


int star(const TString infile = "callisto.root", TString outfile = "",
         Double_t lvl1=4.0,  Double_t lvl2=2.5, Double_t deltat = 17.5)
{
    return star(infile, outfile, "", "", "", lvl1, lvl2, deltat);
}

int star(const TString callistofile, TString outfile,
         TString displayfile,  TString displaytitle,
         const TString txtfile,
         Double_t lvl1, Double_t lvl2, Double_t deltat)
{

    //Double_t lvl1=7.8, 
    //Double_t lvl2=3.9,
    //    lvl1 = 2.5;
    //    lvl2 = 0.5;
    // ------------------------------------------------------

    if (displaytitle.IsNull())
        displaytitle = gSystem->BaseName(callistofile);

    FileStat_t fstat;
    int rc = gSystem->GetPathInfo(outfile, fstat);
    bool isdir = !rc || R_ISDIR(fstat.fMode);

    char *buf = gSystem->ConcatFileName(outfile, "star.root");
    outfile = buf;
    delete [] buf;

    if (displayfile.IsNull())
    {
        displayfile = outfile;
        displayfile.Insert(displayfile.Last('.'), "-display");
    }
    else
    {
        if (isdir && gSystem->DirName(displayfile)==TString("."))
        {
            buf = gSystem->ConcatFileName(outfile, displayfile);
            displayfile = buf;
            delete [] buf;
        }
    }

    // ------------------------------------------------------

    gLog.Separator("Star");
    gLog << all << "Calculate image parameters of sequence ";
    gLog << endl;
    gLog << "Callisto file: " << callistofile << endl;
    gLog << "Output file:   " << outfile << endl;
    gLog << "Display file:  " << displayfile << endl;
    gLog << "Display title: " << displaytitle << endl;
    gLog << endl;
    gLog << "lvl1: "   << lvl1 <<endl;
    gLog << "lvl2: "   << lvl2 <<endl;
    gLog << "deltat: " << deltat <<endl;
    
    
    gLog << endl;

    // ------------------------------------------------------

    gLog.Separator();

    // --------------------------------------------------------

    MBinning bins1(  36,    -90,    90, "BinningAlpha");
    MBinning bins3(  67, -0.005, 0.665, "BinningTheta", "asin");
    MBinning bins4(  25,      0,   2.5, "BinningDist");
    MBinning binsM1(100,      0,     5, "BinningMuonRadius");
    MBinning binsM2( 60,      0,   0.3, "BinningMuonDeviation");
    MBinning binsM3(150,      0,    60, "BinningMuonTime");
    MBinning binsM4(300,      0,    30, "BinningMuonTimeRms");
    MBinning binsM5( 20,      0,   360, "BinningMuonArcPhi");
    MBinning binsM6( 50,      0,   0.5, "BinningMuonArcWidth");
    MBinning binsM7( 30,      5,  5000, "BinningMuonSize", "log");
    MBinning binsM8(100,      0,     5, "BinningMuonRelTimeSigma");
    MBinning binsM9(100,      0,     5, "BinningRadius");

    // ---------------------------------------------------------

    // Filter to start muon analysis
    //MFDataPhrase fmuon1("MHillas.fSize>150 && MNewImagePar.fConcCOG<0.1", "MuonPreCut");
    // Filter to calculate further muon parameters
//    MFDataPhrase fmuon2("(MMuonSearchPar.fRadius*MGeomCam.fConvMm2Deg>0.6) && (MMuonSearchPar.fRadius*MGeomCam.fConvMm2Deg<1.35) &&"
//                        "(MMuonSearchPar.fDeviation*MGeomCam.fConvMm2Deg<0.152)", "MuonSearchCut");
    MFDataPhrase fmuon2("MMuonSearchPar.fDeviation*MGeomCam.fConvMm2Deg>0.015 &&"
                        "MMuonSearchPar.fDeviation*MGeomCam.fConvMm2Deg>(MMuonSearchPar.fRadius*MGeomCam.fConvMm2Deg-0.1)*0.15/0.4",
                        "MuonSearchCut");

    // Filter to fill the MHMuonPar
    MFDataPhrase fmuon3("MMuonCalibPar.fArcPhi>190 && MMuonCalibPar.fRelTimeSigma<3.2",
                        "MuonFinalCut");
    // Filter to write Muons to Muon tree
    //MFDataMember fmuon4("MMuonCalibPar.fArcPhi", '>', -0.5, "MuonWriteCut");

    // ---------------------------------------------------------

    MStatusDisplay *d = new MStatusDisplay;

    MTaskList tlist;

    MParList plist2;
    plist2.AddToList(&tlist);
    plist2.AddToList(&bins1);
    plist2.AddToList(&bins3);
    plist2.AddToList(&bins4);
    plist2.AddToList(&binsM1);
    plist2.AddToList(&binsM2);
    plist2.AddToList(&binsM3);
    plist2.AddToList(&binsM4);
    plist2.AddToList(&binsM5);
    plist2.AddToList(&binsM6);
    plist2.AddToList(&binsM7);
    plist2.AddToList(&binsM8);
    plist2.AddToList(&binsM9);

    MMuonSetup muonsetup;
    plist2.AddToList(&muonsetup);

    MEvtLoop loop;
    loop.SetDisplay(d);
    loop.SetParList(&plist2);

    MReadMarsFile read("Events");
    read.DisableAutoScheme();
    read.AddFile(callistofile);

    MContinue cont("MRawEvtHeader.GetTriggerID!=4", "SelectData");

    MGeomApply apply;

    MImgCleanStd clean(lvl1, lvl2);
    clean.SetMethod(MImgCleanStd::kAbsolute);
    clean.SetTimeLvl1(deltat);//17.5/*1.3*/);
    clean.SetTimeLvl2(deltat);//17.5/*1.3*/);
    clean.SetPostCleanType(3);

    MHillasCalc hcalc;
    hcalc.Disable(MHillasCalc::kCalcConc);

    MH3 hrate("MRawRunHeader.GetFileID"/*, "MRawEvtHeader.GetTriggerID"*/);
    hrate.SetWeight("1./TMath::Max(MRawRunHeader.GetRunLength,1)");
    hrate.SetName("Rate");
    hrate.SetTitle("Event rate after cleaning;File Id;Event Rate [Hz];");
    hrate.InitLabels(MH3::kLabelsX);
    hrate.GetHist().SetMinimum(0);

    MFillH fillR(&hrate, "", "FillRate");

    // ------------------ Setup histograms and fill tasks ----------------
    MHCamEvent evtC1(0, "Cleaned",  "Average signal after Cleaning;;S [phe]");
    MHCamEvent evtC2(0, "UsedPix",  "Fraction of Events in which Pixels are used;;Fraction");
    MHCamEvent evtC3(8, "PulsePos", "Pulse Position of signal after cleaning;;T [ns]");
    evtC1.SetErrorSpread(kFALSE);
    evtC2.SetErrorSpread(kFALSE);
    evtC2.SetThreshold(0);

    MFillH fillC1(&evtC1, "MSignalCam", "FillSignalCam");
    MFillH fillC2(&evtC2, "MSignalCam", "FillCntUsedPixels");
    MFillH fillC3(&evtC3, "MSignalCam", "FillPulsePos");

    MFillH fillD1("MHHillas",      "MHillas",      "FillHillas");
    MFillH fillD2("MHHillasExt",   "",             "FillHillasExt");
    MFillH fillD3("MHHillasSrc",   "MHillasSrc",   "FillHillasSrc");
    MFillH fillD4("MHImagePar",    "MImagePar",    "FillImagePar");
    MFillH fillD5("MHNewImagePar", "MNewImagePar", "FillNewImagePar");
    MFillH fillD6("MHVsSize",      "MHillasSrc",   "FillVsSize");

    MH3 halpha("MHillasSrc.fDist*MGeomCam.fConvMm2Deg", "fabs(MHillasSrc.fAlpha)");
    halpha.SetName("AvsD;Dist;Alpha");

    MFillH filla(&halpha);
    filla.SetNameTab("AvsD");
    filla.SetDrawOption("colz");

    // ----------------------- Muon Analysis ----------------------
    //writem.SetFilter(&fmuon4);

    MHn mhn1;
    mhn1.AddHist("MMuonSearchPar.fRadius*MGeomCam.fConvMm2Deg");
    mhn1.InitName("MuonRadius");
    mhn1.InitTitle("MuonRadius");

    mhn1.AddHist("MMuonSearchPar.fDeviation*MGeomCam.fConvMm2Deg");
    mhn1.InitName("MuonDeviation");
    mhn1.InitTitle("MuonDeviation");

    mhn1.AddHist("MMuonSearchPar.fTime");
    mhn1.InitName("MuonTime");
    mhn1.InitTitle("MuonTime");

    mhn1.AddHist("MMuonSearchPar.fTimeRms");
    mhn1.InitName("MuonTimeRms");
    mhn1.InitTitle("MuonTimeRms");

    MHn mhn3;
    mhn3.AddHist("MMuonSearchPar.fRadius*MGeomCam.fConvMm2Deg","MMuonSearchPar.fDeviation*MGeomCam.fConvMm2Deg");
    mhn3.InitName("MuonRadius;MuonRadius;MuonDeviation");
    mhn3.InitTitle("MuonRadius");
    mhn3.SetDrawOption("colz");

    MHn mhn2;
    mhn2.AddHist("MMuonCalibPar.fArcPhi");
    mhn2.InitName("MuonArcPhi");
    mhn2.InitTitle("MuonArcPhi");

    mhn2.AddHist("MMuonCalibPar.fArcWidth");
    mhn2.InitName("MuonArcWidth");
    mhn2.InitTitle("MuonArcWidth");

    mhn2.AddHist("MMuonCalibPar.fMuonSize");
    mhn2.InitName("MuonSize");
    mhn2.InitTitle("MuonSize");

    mhn2.AddHist("MMuonCalibPar.fRelTimeSigma");
    mhn2.InitName("MuonRelTimeSigma");
    mhn2.InitTitle("MuonRelTimeSigma");

    MHn mhn4;
    mhn4.AddHist("MMuonCalibPar.fArcPhi","MMuonCalibPar.fArcWidth");
    mhn4.InitName("MuonArcPhi;MuonArcPhi;MuonArcWidth");
    mhn4.InitTitle("MuonArcPhi");
    mhn4.SetDrawOption("colz");

    MFillH fillmhn1(&mhn1, "", "FillMuonSearchPar");
    MFillH fillmhn2(&mhn2, "", "FillMuonCalibPar");
    MFillH fillmhn3(&mhn3, "", "FillMuonDevVsRadius");
    MFillH fillmhn4(&mhn4, "", "FillMuonWidthVsPhi");
    fillmhn1.SetNameTab("MuonS");
    fillmhn3.SetNameTab("DevVsRad");
    fillmhn2.SetNameTab("MuonC");
    fillmhn4.SetNameTab("WidthVsPhi");
    fillmhn2.SetFilter(&fmuon2);
    fillmhn4.SetFilter(&fmuon2);

    MMuonSearchParCalc muscalc;
    //muscalc.SetFilter(&fmuon1);

    MMuonCalibParCalc mcalc;
    mcalc.SetFilter(&fmuon2);

    MFillH fillmuon("MHSingleMuon", "", "FillMuon");
    MFillH fillmpar("MHMuonPar",    "", "FillMuonPar");
    fillmuon.SetFilter(&fmuon2);
    fillmpar.SetFilter(&fmuon3);
    fillmuon.SetBit(MFillH::kDoNotDisplay);

    // ---------------------------------------------------------------

    MWriteAsciiFile writeascii(txtfile, "MHillas");
    writeascii.AddColumns("MHillasExt");
    writeascii.AddColumns("MHillasSrc");


    MWriteRootFile write(outfile, "RECREATE", "Image parameters", 2);
    write.AddContainer("MTime",               "Events", kFALSE);
    write.AddContainer("MHillas",             "Events");
    write.AddContainer("MHillasExt",          "Events");
    write.AddContainer("MHillasSrc",          "Events");
    write.AddContainer("MImagePar",           "Events");
    write.AddContainer("MNewImagePar",        "Events");
    write.AddContainer("MRawEvtHeader",       "Events");
    write.AddContainer("ThetaSquared",        "Events");
    write.AddContainer("Disp",                "Events");
    write.AddContainer("MCorsikaEvtHeader",   "Events", kFALSE);
    write.AddContainer("MMcEvt",              "Events", kFALSE);
    write.AddContainer("IncidentAngle",       "Events", kFALSE);
    write.AddContainer("MPointingPos",        "Events", kFALSE);

    write.AddContainer("MRawRunHeader",       "RunHeaders");
    write.AddContainer("MGeomCam",            "RunHeaders");
    write.AddContainer("MMcCorsikaRunHeader", "RunHeaders", kFALSE);
    write.AddContainer("MCorsikaRunHeader",   "RunHeaders", kFALSE);
    write.AddContainer("MMcRunHeader",        "RunHeaders", kFALSE);

    MFDataPhrase fmuonhn("MMuonCalibPar.fRelTimeSigma>=0",
                         "MuonHistCut");
    fillmhn2.SetFilter(&fmuonhn);

    TArrayD param(12);
    // Parametrization of Disp
    param[0]  =  1.15136;     // constant
    param[8]  =  0.0681437;   // slope
    param[9]  =  2.62932;     // leak
    param[10] =  1.51279;     // size-offset
    param[11] =  0.0507821;   // size-slope
    // Parametrization for sign of disp (m3long, slope)
    param[5]  = -0.07;
    param[6]  =  7.2;
    param[7]  =  0.5;
    // ThetaSq-Cut
    param[1]  =  0.11;  // 0.215
    // Area-Cut
    param[2]  =  0.215468;
    param[3]  =  5.63973;
    param[4]  =  0.0836169;
    MFMagicCuts cuts;
    cuts.SetHadronnessCut(MFMagicCuts::kNoCut);
    cuts.SetVariables(param);


    tlist.AddToList(&read);
    tlist.AddToList(&cont);
    tlist.AddToList(&apply);
    tlist.AddToList(&clean);
    tlist.AddToList(&hcalc);
    tlist.AddToList(&cuts);
    tlist.AddToList(&fillC1);
    tlist.AddToList(&fillC2);
    tlist.AddToList(&fillC3);
    tlist.AddToList(&fillR);
    tlist.AddToList(&fillD1);
    tlist.AddToList(&fillD2);
    tlist.AddToList(&fillD3);
    tlist.AddToList(&fillD4);
    tlist.AddToList(&fillD5);
    tlist.AddToList(&fillD6);
    tlist.AddToList(&filla);
    //tlist.AddToList(&fmuon1);
    tlist.AddToList(&muscalc);
    tlist.AddToList(&fillmhn1);
    tlist.AddToList(&fillmhn3);
    tlist.AddToList(&fmuon2);
    tlist.AddToList(&fillmuon);
    tlist.AddToList(&mcalc);
    tlist.AddToList(&fmuonhn);
    tlist.AddToList(&fillmhn2);
    tlist.AddToList(&fillmhn4);
    tlist.AddToList(&fmuon3);
    tlist.AddToList(&fillmpar);
    //tlist.AddToList(&fmuon4);
    //tlist.AddToList(&writem);
    tlist.AddToList(&write);
    if (!txtfile.IsNull())
        tlist.AddToList(&writeascii);

    if (!loop.Eventloop())
        return 3;

    if (!loop.GetDisplay())
        return 4;

    d->SetTitle(displaytitle, kFALSE);
    d->SaveAs(displayfile);

    return 0;
}

