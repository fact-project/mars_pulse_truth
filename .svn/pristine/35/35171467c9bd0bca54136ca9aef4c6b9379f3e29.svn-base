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
#include "MExtractFACT.h"
#include "MFillH.h"
#include "MDrsCalibApply.h"
#include "MGeomApply.h"
#include "MContinue.h"
#include "MRawFitsRead.h"
#include "MReadMarsFile.h"
#include "MEvtLoop.h"
#include "MParList.h"
#include "MStatusDisplay.h"
#include "MDrsCalibrationTime.h"
#include "MH3.h"
#include "MGeomCamFACT.h"
#include "MCalibrateFact.h"
#include "MParameters.h"
#include "MWriteAsciiFile.h"

#endif

using namespace std;

/* Maybe you wanna use this macro like this:
 * 
 * 0.) ---- call root ----
 *  root -b
 * 
 * 1.) ---- compile the stuff ----
 *  .L fact/analysis/callisto_buildable_no_sequence_file.C++
 *  <read a lot of warnings>
 * 
 * 2.) ---- you can call it then ---- 
 *      Therefore you need to specify all the paths ... see below.
 *   
 * When you wanna call the stuff directly from the bash make sure to 
 * escape the bracets and quotes correctly.
 * 
 * your can do:
 *  root -b -q callisto_buildable_no_sequence_file.C++'("path1","path2",...)'
 * or:
 *  root -b -q callisto_buildable_no_sequence_file.C++(\"path1\",\"$HOME\",...)
 * using bash enviroment variables like $HOME is not possible in the upper variant.
 */

int callisto(const TString drsfile="test300samples2.drs.fits.gz",
             const TString pedfile="00000001.001_P_MonteCarlo000_Events.fits",
             const TString datfile="00000003.387_D_MonteCarlo010_Events.fits",
             TString outpath = "",
             TString displayfile = "", TString displaytitle = "")
{

    // ======================================================

    if (displaytitle.IsNull())
        displaytitle = gSystem->BaseName(datfile);

    FileStat_t fstat;
    int rc = gSystem->GetPathInfo(outpath, fstat);
    bool isdir = !rc || R_ISDIR(fstat.fMode);

    TString filename = datfile + "_callisto.root";
    filename.Replace(0, filename.Last('/')+1, "");
    const char *buf = gSystem->ConcatFileName(outpath, filename);
    TString outfile = buf;
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

    // ======================================================

    // true:  Display correctly mapped pixels in the camera displays
    //        but the value-vs-index plot is in software/spiral indices
    // false: Display pixels in hardware/linear indices,
    //        but the order is the camera display is distorted
    bool usemap = true;

    // map file to use (get that from La Palma!)
    const char *pmap = usemap ? "/home/isdc/toscanos/FACT/Mars_svn/resources/FACTmap111030.txt" : NULL;

    // ------------------------------------------------------

    MStatusDisplay *d = new MStatusDisplay;

    MBadPixelsCam badpixels;
    badpixels.InitSize(1440);
    badpixels[ 424].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    badpixels[ 583].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    badpixels[ 830].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    badpixels[ 923].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    badpixels[1208].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    badpixels[1399].SetUnsuitable(MBadPixelsPix::kUnsuitable);
    //  Twin pixel
    //     113
    //     115
    //     354
    //     423
    //    1195
    //    1393

    // ------------------------------------------------------

    // Calib: 51 / 90 / 197 (20% TH)
    // Data:  52 / 64 / 104 (20% TH)

    // Extraction range in slices. It will always(!) contain the full range
    // of integration
    const int first_slice =  25; //  10ns
    const int last_slice  = 225; // 125ns

    Long_t max  =    0;  // All
    Long_t max3 =  max;  // Pedestal Rndm
    Long_t max4 =  max;  // Pedestal Ext

    // ======================================================

    //double scale = 0.1;
    double scale = 0.1024;

    // ======================================================

    if (pmap && gSystem->AccessPathName(pmap, kFileExists))
    {
        gLog << err << "ERROR - Cannot access mapping file '" << pmap << "'" << endl;
        return 1;
    }

    gLog.Separator("Callisto");
    gLog << all;
    gLog << "Data File:     " << datfile << '\n';
    gLog << "DRS calib 300: " << drsfile << endl;;

    MDrsCalibration drscalib300;
    if (!drscalib300.ReadFits(drsfile.Data())) {
        gLog << err << "ERROR - Cannot access drscallib300 file '" << drsfile << "'" << endl;
        return 5;
    }
    gLog << all;
    gLog << "Pedestal file: " << pedfile << '\n';
    gLog << "Output file:   " << outfile << '\n';
    gLog << "Display file:  " << displayfile << '\n';
    gLog << "Display title: " << displaytitle << endl;

    // ------------------------------------------------------
/*
    MStatusArray arrt, arrp;

    TFile ft(lp_template);
    if (arrt.Read()<=0)
    {
        gLog << err << "ERROR - Reading LP template from " << lp_template << endl;
        return 100;
    }

    MHCamera *lpref = (MHCamera*)arrt.FindObjectInCanvas("ExtCalSig;avg", "MHCamera", "Cam");
    if (!lpref)
    {
        gLog << err << "ERROR - LP Template not found in " << lp_template << endl;
        return 101;
    }
    lpref->SetDirectory(0);

    MHCamera *gain = (MHCamera*)arrt.FindObjectInCanvas("gain", "MHCamera", "Gain");
    if (!gain)
    {
        gLog << err << "ERROR - Gain not found in " << lp_template << endl;
        return 101;
    }
    gain->SetDirectory(0);

    TFile fp(pulse_template);
    if (arrp.Read()<=0)
    {
        gLog << err << "ERROR - Reading Pulse template from " << pulse_template << endl;
        return 102;
    }

    TH1F *hpulse = (TH1F*)arrp.FindObjectInCanvas("hPixelEdgeMean0_0", "TH1F", "cgpPixelPulses0");
    if (!hpulse)
    {
        gLog << err << "ERROR - Pulse Template not found in " << pulse_template << endl;
        return 103;
    }
    hpulse->SetDirectory(0);
    */
    // ======================================================

    MDrsCalibrationTime timecam;

    // Plot the trigger pattern rates vs. run-number
    MH3 hrate("MRawRunHeader.GetFileID", "MRawEvtHeader.GetTriggerID&0xff00");
    hrate.SetWeight("1./TMath::Max(MRawRunHeader.GetRunLength,1)");
    hrate.SetName("Rate");
    hrate.SetTitle("Event rate [Hz];File Id;Trigger Type;");
    hrate.InitLabels(MH3::kLabelsXY);
    hrate.DefineLabelY(    0, "Data"); // What if TriggerID==0 already???
    hrate.DefineLabelY(0x100, "Cal");
    hrate.DefineLabelY(0x400, "Ped");
    // hrate.DefaultLabelY("ERROR");
    gStyle->SetOptFit(kTRUE);


    // ========================= Result ==================================
/*
    //~ Double_t avgS = evt1f.GetHist()->GetMean();
    //~ Double_t medS = evt1f.GetHist()->GetMedian();
    //~ Double_t rmsS = evt1f.GetHist()->GetRMS();
    //~ Double_t maxS = evt1f.GetHist()->GetMaximum();

    MArrayF der1(hpulse->GetNbinsX());
    MArrayF der2(hpulse->GetNbinsX());

    MExtralgoSpline spline(hpulse->GetArray()+1, hpulse->GetNbinsX(),
                           der1.GetArray(), der2.GetArray());
    spline.SetRiseFallTime(rise_time_dat, fall_time_dat);
    spline.SetExtractionType(type);
    spline.SetHeightTm(heighttm);

    spline.Extract(hpulse->GetMaximumBin()-1);

    // The pulser signal is most probably around 400mV/9.5mV
    // IntegraFixed 2/48 corresponds to roughly 215mV*50slices
    Double_t scale = 1./spline.GetSignal();

    MArrayD calib(1440);
    for (int i=0; i<1440; i++)
        calib[i] =1.;

    gROOT->SetSelectedPad(0);
    d->AddTab("PulseTemp");
    gPad->SetGrid();
    hpulse->SetNameTitle("Pulse", "Single p.e. pulse template");
    hpulse->SetDirectory(0);
    hpulse->SetLineColor(kBlack);
    hpulse->DrawCopy();

    TAxis *ax = hpulse->GetXaxis();

    Double_t w = hpulse->GetBinWidth(1);
    Double_t T = w*(spline.GetTime()+0.5)       +ax->GetXmin();
    //~ Double_t H = w*(hpulse->GetMaximumBin()+0.5)+ax->GetXmin();

    TLine line;
    line.SetLineColor(kRed);
    line.DrawLine(T-rise_time_dat*w, spline.GetHeight(),
                  T+fall_time_dat*w, spline.GetHeight());
    line.DrawLine(T, spline.GetHeight()/4, T, 3*spline.GetHeight()/4);
    line.DrawLine(T-rise_time_dat*w, 0,
                  T-rise_time_dat*w, spline.GetHeight());
    line.DrawLine(T+fall_time_dat*w, 0,
                  T+fall_time_dat*w, spline.GetHeight());

    TGraph gg;
    for (int ix=1; ix<=hpulse->GetNbinsX(); ix++)
        for (int i=0; i<10; i++)
        {
            Double_t x = hpulse->GetBinLowEdge(ix)+i*hpulse->GetBinWidth(ix)/10.;
            gg.SetPoint(gg.GetN(), x+w/2, spline.EvalAt(ix-1+i/10.));
        }

    gg.SetLineColor(kBlue);
    gg.SetMarkerColor(kBlue);
    gg.SetMarkerStyle(kFullDotMedium);
    gg.DrawClone("L");

    gROOT->SetSelectedPad(0);
    d->AddTab("CalConst");
    MGeomCamFACT fact;
    MHCamera hcalco(fact);
    hcalco.SetName("CalConst");
    hcalco.SetTitle(Form("Relative calibration constant [%.0f/pe]", 1./scale));
    hcalco.SetCamContent(calib);
    hcalco.SetAllUsed();
    //hcalco.Scale(scale);
    hcalco.DrawCopy();
*/
    // ======================================================

    gLog << endl;
    gLog.Separator("Extracting random pedestal");

    MTaskList tlist3;

    MParList plist3;
    plist3.AddToList(&tlist3);
    plist3.AddToList(&drscalib300);
    plist3.AddToList(&badpixels);
    plist3.AddToList(&timecam);

    MEvtLoop loop3("DetermineRndmPed");
    loop3.SetDisplay(d);
    loop3.SetParList(&plist3);

    // ------------------ Setup the tasks ---------------

    MRawFitsRead read3;
    read3.LoadMap(pmap);
    read3.AddFile(pedfile);

    MFillH fill3a(&hrate);

    MContinue cont3("(MRawEvtHeader.GetTriggerID&0xff00)!=0x400", "SelectPed");

    MGeomApply apply3;

    MDrsCalibApply drsapply3;

    MFilterData filterdata5;

    //---

    MExtractFACT extractor3;
    extractor3.SetRange(first_slice, last_slice);
    extractor3.SetNoiseCalculation(kTRUE);

    MCalibrateFact conv3;
    conv3.SetScale(scale);
    //conv3.SetCalibConst(calib);

    MBadPixelsTreat treat3;
    treat3.SetProcessPedestalRun(kFALSE);
    treat3.SetProcessPedestalEvt(kFALSE);
    treat3.SetProcessTimes(kFALSE);

    MHCamEvent evt3b(0, "PedRdm","Interpolated random pedestal;;Signal [~phe]");
    //evt2b.SetErrorSpread(kFALSE);

    MFillH fill3b(&evt3b, "MSignalCam", "FillPedRdm");
    fill3b.SetDrawOption("gaus");

    // ------------------ Setup eventloop and run analysis ---------------

    tlist3.AddToList(&read3);
    tlist3.AddToList(&apply3);
    tlist3.AddToList(&drsapply3);
    tlist3.AddToList(&cont3);
    tlist3.AddToList(&filterdata3);
    tlist3.AddToList(&extractor3);
//    tlist3.AddToList(&fill3a);
    tlist3.AddToList(&conv3);
    tlist3.AddToList(&treat3);
    tlist3.AddToList(&fill3b);

    if (!loop3.Eventloop(max3))
        return 14;

    if (!loop3.GetDisplay())
        return 15;

    // ======================================================

    gLog << endl;
    gLog.Separator("Extracting pedestal");

    MTaskList tlist4;

    MParList plist4;
    plist4.AddToList(&tlist4);
    plist4.AddToList(&drscalib300);
    plist4.AddToList(&badpixels);
    plist4.AddToList(&timecam);

    MEvtLoop loop4("DetermineExtractedPed");
    loop4.SetDisplay(d);
    loop4.SetParList(&plist4);

    // ------------------ Setup the tasks ---------------

    MRawFitsRead read4;
    read4.LoadMap(pmap);
    read4.AddFile(pedfile);

    MContinue cont4("(MRawEvtHeader.GetTriggerID&0xff00)!=0x400", "SelectPed");

    MGeomApply apply4;

    MDrsCalibApply drsapply4;

    MFilterData filterdata4;

    MExtractFACT extractor4;
    extractor4.SetRange(first_slice, last_slice);
    extractor4.SetNoiseCalculation(kFALSE);

    MCalibrateFact conv4;
    conv4.SetScale(scale);
    //conv4.SetCalibConst(calib);

    MBadPixelsTreat treat4;
    treat4.SetProcessPedestalRun(kFALSE);
    treat4.SetProcessPedestalEvt(kFALSE);

    MHCamEvent evt4b(0, "PedExt","Interpolated extracted pedestal;;Signal [~phe]");
    //evt4b.SetErrorSpread(kFALSE);

    MFillH fill4b(&evt4b, "MSignalCam", "FillPedExt");
    fill4b.SetDrawOption("gaus");

    // ------------------ Setup eventloop and run analysis ---------------

    tlist4.AddToList(&read4);
    tlist4.AddToList(&apply4);
    tlist4.AddToList(&drsapply4);
    tlist4.AddToList(&cont4);
    tlist4.AddToList(&filterdata4);
    tlist4.AddToList(&extractor4);
    tlist4.AddToList(&conv4);
    tlist4.AddToList(&treat4);
    tlist4.AddToList(&fill4b);

    if (!loop4.Eventloop(max4))
        return 15;

    if (!loop4.GetDisplay())
        return 16;

    // ===================================================================

    gLog << endl;
    gLog.Separator("Extracting and calibration data");

    MTaskList tlist5;

    MParList plist5;
    plist5.AddToList(&tlist5);
    plist5.AddToList(&drscalib300);
    plist5.AddToList(&badpixels);
    plist5.AddToList(&timecam);

    MEvtLoop loop5("CalibratingData");
    loop5.SetDisplay(d);
    loop5.SetParList(&plist5);

    // ------------------ Setup the tasks ---------------

    MRawFitsRead read5a;
    MReadMarsFile read5b("Events");
    read5a.LoadMap(pmap);
    read5a.AddFile(datfile);
    read5b.DisableAutoScheme();
    read5b.AddFile(datfile);

    MRead &read5 = datfile.EndsWith(".root") ? static_cast<MRead&>(read5b) : static_cast<MRead&>(read5a);

    MFillH fill5a(&hrate);

    MGeomApply apply5;

    MDrsCalibApply drsapply5;

    MTreatSaturation treatsat5;

    MFilterData filterdata5;

    MFDataPhrase filterdat("(MRawEvtHeader.GetTriggerID&0xff00)==0",     "SelectDat");
    MFDataPhrase filtercal("(MRawEvtHeader.GetTriggerID&0xff00)==0x100", "SelectCal");
    MFDataPhrase filterped("(MRawEvtHeader.GetTriggerID&0xff00)==0x400", "SelectPed");
    MFDataPhrase filterncl("(MRawEvtHeader.GetTriggerID&0xff00)!=0x100", "SelectNonCal");

    //MContinue cont4("MRawEvtHeader.GetTriggerID!=4", "SelectData");

    // ---

    MExtractFACT extractor5dat;
    extractor5dat.SetRange(first_slice, last_slice);
    extractor5dat.SetNoiseCalculation(kFALSE);

    MExtractFACT extractor5cal;
    extractor5cal.SetRange(first_slice, last_slice);
    extractor5cal.SetNoiseCalculation(kFALSE);

    MExtractFACT extractor5tm("ExtractTM");
    extractor5tm.SetRange(last_slice, 294);
    extractor5tm.SetNoiseCalculation(kFALSE);
    extractor5tm.SetNameSignalCam("TimeMarkerAmplitude");
    extractor5tm.SetNameTimeCam("TimeMarkerTime");

    extractor5dat.SetFilter(&filterncl);
    extractor5cal.SetFilter(&filtercal);
    //extractor4tm.SetFilter(&filtercal);

    // ---
    MCalibrateFact conv5;
    conv5.SetScale(scale);
    //conv5.SetCalibConst(calib);

    MCalibrateDrsTimes calctm5;
    calctm5.SetNameUncalibrated("UncalibratedTimes");

    MCalibrateDrsTimes calctm5tm("CalibrateTimeMarker");
    calctm5tm.SetNameArrivalTime("TimeMarkerTime");
    calctm5tm.SetNameUncalibrated("UncalTimeMarker");
    calctm5tm.SetNameCalibrated("TimeMarker");
    calctm5tm.SetTimeMarker();
    //calctm4tm.SetFilter(&filtercal);

    MBadPixelsTreat treat5;
    treat5.SetProcessPedestalRun(kFALSE);
    treat5.SetProcessPedestalEvt(kFALSE);

    MHCamEvent evt5b(0, "ExtSig",   "Extracted signal;;S [mV·sl]");
    MHCamEvent evt5c(0, "CalSig",   "Calibrated and interpolated signal;;S [~phe]");
    MHCamEvent evt5d(4, "ExtSigTm", "Extracted time;;T [sl]");
    MHCamEvent evt5e(6, "CalSigTm", "Calibrated and interpolated time;;T [ns]");

    MFillH fill5b(&evt5b, "MExtractedSignalCam", "FillExtSig");
    MFillH fill5c(&evt5c, "MSignalCam",          "FillCalSig");
    MFillH fill5d(&evt5d, "MArrivalTimeCam",     "FillExtTm");
    MFillH fill5e(&evt5e, "MSignalCam",          "FillCalTm");

    fill5c.SetDrawOption("gaus");
    fill5d.SetDrawOption("gaus");
    fill5e.SetDrawOption("gaus");

    /*
    fill4b.SetFilter(&filterdat);
    fill4c.SetFilter(&filterdat);
    fill4d.SetFilter(&filterdat);
    fill4e.SetFilter(&filterdat);
    */

    //MFSoftwareTrigger swtrig;
    //MContinue contsw(&swtrig, "FilterSwTrigger", "Software trigger");
    //contsw.SetInverted();

    // The second rule is for the case reading raw-files!
    
    MWriteRootFile write5(outfile, "RECREATE", "Calibrated Data", 2);
    write5.AddContainer("MRawRunHeader",       "RunHeaders");
    write5.AddContainer("MGeomCam",            "RunHeaders");
    write5.AddContainer("MMcCorsikaRunHeader", "RunHeaders", kFALSE);
    write5.AddContainer("MCorsikaRunHeader",   "RunHeaders", kFALSE);
    write5.AddContainer("MMcRunHeader",        "RunHeaders", kFALSE);

    // Common events
    write5.AddContainer("MCorsikaEvtHeader",   "Events", kFALSE);
    write5.AddContainer("MMcEvt",              "Events", kFALSE);
    write5.AddContainer("IncidentAngle",       "Events", kFALSE);
    write5.AddContainer("MPointingPos",        "Events", kFALSE);
    write5.AddContainer("MSignalCam",          "Events");
    write5.AddContainer("MTime",               "Events", kFALSE);
    write5.AddContainer("MRawEvtHeader",       "Events");
    //write.AddContainer("MTriggerPattern", "Events");
    
    // ------------------ Setup histograms and fill tasks ----------------

    MContinue test;
    test.SetFilter(&filterncl);

    MTaskList tlist5tm;
    tlist5tm.AddToList(&extractor5tm);
    tlist5tm.AddToList(&calctm5tm);
    tlist5tm.SetFilter(&filtercal);

    MTaskList tlist5dat;
    tlist5dat.AddToList(&fill5b);
    tlist5dat.AddToList(&fill5c);
    tlist5dat.AddToList(&fill5d);
    tlist5dat.AddToList(&fill5e);
    tlist5dat.SetFilter(&filterdat);

    tlist5.AddToList(&read5);
    tlist5.AddToList(&apply5);
    tlist5.AddToList(&drsapply5);
    tlist5.AddToList(&filterncl);
    //tlist5.AddToList(&test);
    tlist5.AddToList(&filterdat);
    tlist5.AddToList(&filtercal);
    tlist5.AddToList(&filterped);
    tlist5.AddToList(&fill5a);
    tlist5.AddToList(&treatsat5);
    tlist5.AddToList(&filterdata5);
    tlist5.AddToList(&extractor5dat);
    tlist5.AddToList(&extractor5cal);
    tlist5.AddToList(&calctm5);
    tlist5.AddToList(&tlist5tm);
    tlist5.AddToList(&conv5);
    tlist5.AddToList(&treat5);
    tlist5.AddToList(&tlist5dat);
    tlist5.AddToList(&write5);

    if (!loop5.Eventloop(max4))
        return 18;

    if (!loop5.GetDisplay())
        return 19;

    d->SetTitle(displaytitle, kFALSE);
    d->SaveAs(displayfile);

    return 0;
}
