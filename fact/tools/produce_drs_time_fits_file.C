// The FACT collaboration
// Dominik Neise and Sebastian Mueller October 2014
// A callisto like CERN-Root script to process the FACT DRS 
// (Domino-Ring-Sampler) time calibration files. 
//
// The basic script is taken from the October 2014 analysis framework running on
// La Palma, maintained by Daniela Dorner and Thomas Bretz. There it was called
// "callisto_drs_time.C"

// Dominik and Sebastian:
// Our first approach was to use the MWriteFitsFile class but we have not been 
// able to use it.

// MWriteFitsFile fitsfile(outfile, 
//                 MWriteFitsFile::kSingleFile, 
//                 "RECREATE", 
//                 "fitsfile", 
//                 "atitle");
// fitsfile.SetBytesPerSample("Data", 2);
// fitsfile.AddContainer("MDrsCalibrationTime","DANDELION");

#include "MLogManip.h"

int produce_drs_time_fits_file(
    const char *drs_time_file,  //* path to raw drs-time-calib-file
    const char *drs_file,       //* path to drs.fits(.gz) file
    const char *outfile         //* output path like yyyymmdd_rrr.drs.time.fits 
){
    gLog.Separator("produce_drs_time_fits_file");
    gLog << all;
    gLog << "DRS Timing " << drs_time_file << '\n';
    gLog << "DRS 1024   " << drs_file << '\n';
    gLog << endl;

    MParList plist0;

    MTaskList tlist0;
    plist0.AddToList(&tlist0);

    MDrsCalibration drscalib1024;
    if (!drscalib1024.ReadFits(drs_file)){
        gLog << "Error while opening drs amplitude calibration file:" 
            << drs_file << "Aborting" << endl;
        return 52; // DN: Why 52 I don't know.
    }
    plist0.AddToList(&drscalib1024);
    
    MDrsCalibrationTime timecam;
    plist0.AddToList(&timecam);

    MEvtLoop loop0("DetermineTimeCal");
    loop0.SetParList(&plist0);

    factfits drstimeRawFile(drs_time_file);
    const int NumberOfChips = drstimeRawFile.GetInt("NPIX")/9;  // --> 160 
    const int NumberOfCells = drstimeRawFile.GetInt("NROI");    // --> 1024
    drstimeRawFile.close();

    MRawFitsRead read0(drs_time_file);
    tlist0.AddToList(&read0);
    MGeomApply apply0;
    tlist0.AddToList(&apply0);
    MDrsCalibApply drsapply0;
    tlist0.AddToList(&drsapply0);
    MContinue cont0("MRawEvtHeader.GetTriggerID!=33792", "SelectTim");
    tlist0.AddToList(&cont0);
    MFillH fill0("MHDrsCalibrationTime");
    fill0.SetNameTab("DeltaT");
    tlist0.AddToList(&fill0);

    if (!loop0.Eventloop(0)){
        gLog << "Error performing the loop over the drs-time-run:" 
            << drs_time_file << "Aborting" << endl;   
        return 8; // DN: why 8 I don't know.
    }

    ofits drstimeFile(outfile);
    drstimeFile.SetDefaultKeys();
    drstimeFile.AddColumnDouble(  
        NumberOfChips*NumberOfCells,
        "SamplingTimeDeviation", "nominal slices",
        "see following COMMENT."
    );
    drstimeFile.SetInt("NCHIPS", NumberOfChips, "number of drs chips");
    drstimeFile.SetInt("NCELLS", NumberOfCells, "number of cells of each chip");
    drstimeFile.AddComment(
"The SamplingTimeDeviation specifies the deviation of the actual ");
    drstimeFile.AddComment(
"sampling time of a certain cell, from its nominal sampling time.");
    drstimeFile.AddComment(
"It is measured in nominal slices.");
    drstimeFile.AddComment(
"In order to convert the sample index into the actual sampling time");
    drstimeFile.AddComment(
"just do: ");
    drstimeFile.AddComment(
"     cell_index + td(cell_index) - td(stop_cell_index)");
    drstimeFile.AddComment(
"where td(i) refers to the SamplingTimeDeviation of cell i of the ");
    drstimeFile.AddComment(
"current DRS4 chip.");
    drstimeFile.AddComment(
"This gives you the actual sampling time of the cell measured in ");
    drstimeFile.AddComment(
"nominal slices [nsl] with respect to the stop cells sampling time.");
    drstimeFile.AddComment(
"Convert it to ns, simply by multiplication ");
    drstimeFile.AddComment(
"with 0.5 ns/nsl.");
    drstimeFile.AddComment(
"");

    drstimeFile.AddComment(
"In FACT it became common practice to measure un-time-calibrated ");
    drstimeFile.AddComment(
" DRS4 data in 'time slices' or simply 'samples', while ");
    drstimeFile.AddComment(
" time-calibrated DRS4 data usually measured in ns. Since the method,");
    drstimeFile.AddComment(
" that is emplyed to measure the DRS4 time calibration constants has ");
    drstimeFile.AddComment(
" no possibility to assert the actual length of a slice is really half");
    drstimeFile.AddComment(
" a nanosecond, this practice is not advisable.");
    drstimeFile.AddComment(
" I propose instead to call un-time-calibrated data 'samples',");
    drstimeFile.AddComment(
" since this is what they are. If one wants to stress the fact,");
    drstimeFile.AddComment(
" that no drs time calibration has been applied one should refer to ");
    drstimeFile.AddComment(
"'uncalibrated slices'. Since it *should* be common practice to apply");
    drstimeFile.AddComment(
" the drs4 time calibration in all cases, I also propose to use the");
    drstimeFile.AddComment(
" short term of 'slices' or 'sl'. If one wants to stress, that the");
    drstimeFile.AddComment(
" drs4 time calibration has actually been applied to the data,");
    drstimeFile.AddComment(
" the term 'calibrated slices' or 'nominal slices' or short 'nsl'.");

    drstimeFile.WriteTableHeader("DRS_CELL_TIMES");

    // By drs_sampling_time_deviations we refer to the deviations (measured in nominal slices) of
    // the actual sampling time compared to the nominal sampling time of every cell of every chip.
    double *drs_sampling_time_deviations = new double[ NumberOfChips * NumberOfCells ];
    for (int chip = 0; chip < NumberOfChips; chip++){
        for (int cell = 0; cell < NumberOfCells; cell++){
            // Dominik and Sebastian:
            // We ended with using DrsCalibrateTime.Sum() in order to retrieve 
            // the contents DrsCalibrateTime.fStat.
            // First we wanted to access the member fStat of class DrsCalibrateTime
            // which is declared public but it did not work out of the box.
            // drs_sampling_time_deviations[chip*NumberOfCells+cell] = 
            // timecam.fStat[chip*NumberOfCells+cell].first;
            drs_sampling_time_deviations[ chip * NumberOfCells + cell ] = 
                timecam.Sum( chip * NumberOfCells + cell );
        }
    }

    drstimeFile.WriteRow(drs_sampling_time_deviations, sizeof(double) * NumberOfChips * NumberOfCells);
    drstimeFile.close();

    return 0;
}
