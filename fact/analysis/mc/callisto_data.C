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

int callisto_data(const TString drsfile="test300samples2.drs.fits.gz",
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
    bool usemap = false;

    // map file to use (get that from La Palma!)
    const char *pmap = usemap ? "FACTmap111030.txt" : NULL;

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
    double scale = 0.1024; // 0.00389429

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
    gLog << "Output file:   " << outfile << '\n';
    gLog << "Display file:  " << displayfile << '\n';
    gLog << "Display title: " << displaytitle << endl;

    // ------------------------------------------------------

    // ======================================================

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

    gLog << endl;
    gLog.Separator("Extracting and calibrating data");

    MTaskList tlist5;

    MParList plist5;
    plist5.AddToList(&tlist5);
    plist5.AddToList(&drscalib300);
    plist5.AddToList(&badpixels);

    MEvtLoop loop5("CalibratingData");
    loop5.SetDisplay(d);
    loop5.SetParList(&plist5);

    // ------------------ Setup the tasks ---------------

    MRawFitsRead read5a;
    MReadMarsFile read5b("Events");
    if (pmap)
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
