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
!   Author(s): Thomas Bretz, 11/2002 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCT1ReadPreProc
//
// Reads a output file of the CT1 preproc.
//
// Implements usage of a selector (see MRead) Use such a filter to skip
// events before reading! But never use a filter which needs read data
// as input...
//
//  Input Containers:
//   -/-
//
//  Output Containers:
//    MCerPhotEvt     the data container for all data.
//    MPedPhotCam     CT1 pedestals
//    MMcEvt          monte carlo data container for MC files
//    MMcTrig         mc data container for trigger information
//    MSrcPosCam      source position in the camera
//    MBadPixelsCam   Array holding blind pixels
//
/////////////////////////////////////////////////////////////////////////////
#include "MCT1ReadPreProc.h"

#include <errno.h>
#include <fstream>

#include <TList.h>
#include <TSystem.h>
#include <TRandom3.h>

#define LINUX
#define HISTO void
#define HBOOK_FILE int
#include "defines.h"
#include "structures.h"

#include "MLog.h"
#include "MLogManip.h"

#include "MTime.h"
#include "MFilter.h"

#include "MParList.h"
#include "MCerPhotEvt.h"

#include "MPedPhotPix.h"
#include "MPedPhotCam.h"

#include "MGeomCam.h"
#include "MSrcPosCam.h"
#include "MBadPixelsCam.h"
#include "MBadPixelsPix.h"

#include "MRawRunHeader.h"
#include "MTaskList.h"

#include "MMcEvt.hxx"
#include "MMcTrig.hxx"
#include "MBinning.h"

#include "MParameters.h"

ClassImp(MCT1ReadPreProc);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. Creates an array which stores the file names of
// the files which should be read. If a filename is given it is added
// to the list.
//
MCT1ReadPreProc::MCT1ReadPreProc(const char *fname, const char *name,
                                 const char *title) : fIn(NULL), fEntries(0)
{
    fName  = name  ? name  : "MRead";
    fTitle = title ? title : "Reads a CT1 preproc data file";

    //
    // remember file name for opening the file in the preprocessor
    //
    fFileNames = new TList;
    fFileNames->SetOwner();

    if (fname)
        AddFile(fname);
}

// --------------------------------------------------------------------------
//
// Delete the filename list and the input stream if one exists.
//
MCT1ReadPreProc::~MCT1ReadPreProc()
{
    delete fFileNames;
    if (fIn)
        delete fIn;
}

// --------------------------------------------------------------------------
//
// Add this file as the last entry in the chain
//
Int_t MCT1ReadPreProc::AddFile(const char *txt, int)
{
    const char *name = gSystem->ExpandPathName(txt);

    TString fname(name);
    delete [] name;

    if (!CheckHeader(fname))
    {
        *fLog << warn << "WARNING - Problem reading header... ignored." << endl;
        return 0;
    }

    const Int_t n = GetNumEvents(fname);
    if (n==0)
    {
        *fLog << warn << "WARNING - File contains no data... ignored." << endl;
        return 0;
    }

    fEntries += n;

    *fLog << inf << "File " << txt << " contains " << n << " events (Total=" << fEntries << ")" << endl;

    fFileNames->AddLast(new TNamed(txt, ""));
    return 1;
}

// --------------------------------------------------------------------------
//
// Print data from the header to the screen and analyse the header data,
// means store and copy the needed data into Mars structures and
// data members
//
void MCT1ReadPreProc::ProcessRunHeader(const struct outputpars &outpars)
{
    if (outpars.inumpixels != iMAXNUMPIX)
        *fLog << warn << "WARNING! File doesn't contain " << iMAXNUMPIX << " Pixels... maybe corrupt." << endl;

    fNumEventsInRun = 0;

    //
    // ------------------- Output some stuff -----------------------
    //

    // int     itelescope;       // number of the CT which took the data
    *fLog << inf << "Telescope: CT" << outpars.itelescope;

    // float   flongitude_deg;   // longitude (counted positive towards West) of CT position */
    // float   flatitude_deg;    // latitude (counted positive towards North) of CT position */
    *fLog << " located @ Longitude=" << outpars.flongitude_deg;
    *fLog << "deg  Latitude=" << outpars.flatitude_deg << "deg" << endl;

    // int     irunnum;          // run number (from parameters file)
    fRawRunHeader->SetRunNumber(outpars.irunnum);
    fRawRunHeader->SetReadyToSave();

    // enum    onoroff {NEITHER_ON_NOR_OFF, OFF_SOURCE, ON_SOURCE} eruntype; // runtype
    *fLog << "Run:       #" << outpars.irunnum << "  (";
    switch (outpars.eruntype)
    {
    case NEITHER_ON_NOR_OFF: *fLog << "unknown";    break;
    case OFF_SOURCE:         *fLog << "off-source"; break;
    case ON_SOURCE:          *fLog << "on-source";  break;
    default:                 *fLog << (int)outpars.eruntype; break;
    }
    *fLog << ", ";
    switch (outpars.etrackmode)
    {
    case NORMAL:  *fLog << "normal tracking";  break;
    case REVERSE: *fLog << "reverse tracking"; break;
    case DUNNO:   *fLog << "unknown tracking"; break;
    default:      *fLog << (int)outpars.etrackmode; break;
    }
    *fLog << ")" << endl;

    //double  dsourcera_hours;  // right ascension of observed source in hours
    //double  dsourcedec_deg;   // declination of observed source in degrees
    *fLog << "Source:    RA=" << outpars.dsourcera_hours << "h  DEC=";
    *fLog << outpars.dsourcedec_deg << "deg" << endl;

    //int     inummuonpixels;   // number of pixels in the muon shield
    //int     inumcointdcs;     // number of coincidence tdcs recorded in the runfile
    //float   fpixdiameter_deg; // smallest pixel diameter (degrees) (from parameters file) */

    // enum    axes {RA, DEC, ALT, AZ} ese1_is; // name of the axis to which shaft encoder 1 is attached (implies the type of mount)
    *fLog << "Shaftencoder 1 @ ";
    switch (outpars.ese1_is)
    {
    case RA:  *fLog << "RA";  break;
    case DEC: *fLog << "DEC"; break;
    case ALT: *fLog << "ALT"; break;
    case AZ:  *fLog << "AZ";  break;
    default:  *fLog << (int)outpars.ese1_is; break;
    }
    *fLog << endl;

    // int     isezeropos[2];       // zero position of shaftencoders 1 and 2 (from parameters file)
    *fLog << "SE Zero:   SE(1)=" << outpars.isezeropos[0] << "  ";
    *fLog << "SE(2)=" << outpars.isezeropos[1] << endl;

    // int     iaz_rev_track_corr;  // correction for the azimuth shaft encoder (ALT/AZ mount only) in reverse tracking mode
    // int     ialt_rev_track_corr; // correction for the altitude shaft encoder (ALT/AZ mount only) in reverse tracking mode
    *fLog << "Reverse tracking corrections: SE(az)=" << outpars.iaz_rev_track_corr;
    *fLog << "  SE(alt)=" << outpars.ialt_rev_track_corr << endl;

    // float   fbendingcorr;        // bending correction factor (ALT/AZ mount only)
    // float   fextinction;         // atmospheric extinction (typically taken from the Carlsberg Meridian Circle data)
    *fLog << "Bending: Correction factor=" << outpars.fbendingcorr << "  ";
    *fLog << "Extinction=" << outpars.fextinction << endl;

    // Boolean bdontusepix[iMAXNUMPIX]; // bdontusepix is set true if the pixel should not be used in image analysis, otherwise it is true;

    fBlinds->Clear();
    *fLog << "Don't use pixels: ";
    for (int i=0; i<iMAXNUMPIX; i++)
        if (outpars.bdontusepix[i])
        {
            *fLog << i << " ";
            (*fBlinds)[i].SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
        }
    *fLog << endl;

    *fLog << "Exclude pixels: ";
    // Boolean bexcludepix[iMAXNUMPIX];
    for (int i=0; i<iMAXNUMPIX; i++)
        if (outpars.bexcludepix[i])
        {
            *fLog << i << " ";
            (*fBlinds)[i].SetUnsuitable(MBadPixelsPix::kUnsuitableRun);
        }
    *fLog << endl;

    // save blind pixels for all events of this run
    fBlnd.Set(iMAXNUMPIX);
    for (int i=0; i<iMAXNUMPIX; i++)
        fBlnd[i] = (*fBlinds)[i].IsBad() ? 1 : 0;

    fBlinds->SetReadyToSave();

    /* bexcludepix[] is set TRUE (== exclude from pedestal, Laser
     * calibration and the further analysis) when the Mean value
     * of a pixel inside a pedestal Run is larger than 50 or ( || )
     * if the pedestal RMS value of this pixel is larger than 5.0
     * This is reflected in the (new for versions >= 0.4)
     * variable "pixonoff" in the ntuple written by preproc:
     * preproc.nt.hbook
     *
     * When the pixel is excluded by the user it will get a -2 otherwise
     * pixonoff = 0.
     * Additive to this a -1 is added when preproc excludes the pixel
     * for a given Run. So the actual value tells you whether you caught
     * it already by hand or not.
     *
     * A plot of pixonoff may also be handy to tell you the status of your
     * ADC equipment. */

    // float   fphotoel_per_adccnt[iMAXNUMPIX]; // conversion factors for the pixel signals */
    /*
    float padc = outpars.fphotoel_per_adccnt[0];
    *fLog << "Phe/ADC (pixel 0): " << padc << endl;
    for (int i=0; i<iMAXNUMPIX; i++)
        *fLog << outpars.fphotoel_per_adccnt[i] << " ";
    *fLog << endl;
    */
    /*
     --- USEFULL? NEEDED? ---
     int     irubminusutc_usecs;              // difference between rubidium clock and UTC in microseconds
     int     isum_thresh_phot;                // threshold for the total sum of photoelectrons filter
     int     i2out_thresh_phot;               // threshold for the two-pixels-out-of-all software
     int     imuoncut_thresh_adccnt[iMAXNUMMUONPIX]; // thresholds for the muon cut
     Boolean bmuon_suppression;               // "Dolby" noise reduction flag
     float   ftolerated_pointerror_deg;       // maximum tolerated pointing error in the position
     */

    // float fxpointcorr_deg; // pointing correction (to be added along the camera x axis) e.g. from point run */
    // float fypointcorr_deg; // pointing correction (to be added along the camera y axis) e.g. from point run */
    *fLog << "Pointing correction: dx=" << outpars.fxpointcorr_deg << "deg  ";
    *fLog << "dy=" << outpars.fypointcorr_deg << "deg" << endl;

    // FIXME? Is x-y echanged between Mars CT1 geometry and CT1 definition?
    fSrcPos->SetXY(-outpars.fypointcorr_deg/fGeom->GetConvMm2Deg(),
                   -outpars.fxpointcorr_deg/fGeom->GetConvMm2Deg());
    fSrcPos->SetReadyToSave();

    /*
     --- USEFULL? NEEDED? ---
     float   fcamera_align_angle_deg;         // the angle between the camera y-axis and the meridian when a culminating object is observed (defined counter-clockwise looking at the sky)
     int     iratecalc_numevents_odd;         // number of events used in the rate calculation (must be odd)
     int     inumpedfile;                     // number of the pedestal file used
     int     inumpedrun;                      // number of the pedestal run used in the file (starting at 0)
     int     inumcalfile;                     // number of the calibration file used
     int     inumlaserrun;                    // number of the laserrun used in the file (starting at 0)
     int     inumtellogfile;                  // number of the TelLog file to be used
     int     inumtellogrun;                   // number of the tellog entry (Runnumber) used from the log file
     int     imcparticle;                     // CORSIKA-coded Monte Carlo particle type.
    */

    // ----- preprocessing results -----

    // int     istart_mjdate_day;                 // MJD of run start (first event) */
    // int     iend_mjdate_day;                   // MJD of run end (last event) */
    // int     irunduration_secs;                 // difference between start and end time (secs) */
    *fLog << "Run Time: From " << outpars.istart_mjdate_day << " to ";
    *fLog << outpars.iend_mjdate_day << " (MJD),  Duration=";
    *fLog << outpars.irunduration_secs/3600 << "h";
    *fLog << (outpars.irunduration_secs/60)%60 << "m";
    *fLog << outpars.irunduration_secs%60 << "s" << endl;
    fRawRunHeader->SetRunTime(outpars.istart_mjdate_day, outpars.iend_mjdate_day);
    fRawRunHeader->SetReadyToSave();

    /*
     --- USEFULL? NEEDED? ---
     int     iproc_mjdate;                      // MJD of data processing (i.e. creation of this file)
     */

    // int     iproc_evts;                        // number of events processed */
    *fLog << "Number of processed events: " << outpars.iproc_evts << endl;

    // --- USEFULL? NEEDED? ---
    // double  dactual_sourcera_hours;            // for off runs: the false source (that should have been) observed */

    // float   frms_pedsig_phot[iMAXNUMPIX];      // standard deviation of the calibrated signals from the pedestal run */
    fPedest->Init(*fGeom);

    fPedRMS.Set(iMAXNUMPIX);

    *fLog << "PedestalRMS : ";
    for (Int_t i=0; i<iMAXNUMPIX; i++)
    {
        (*fPedest)[i].Set(0, outpars.frms_pedsig_phot[i]);
        *fLog << outpars.frms_pedsig_phot[i] << " "; 
        fPedRMS[i] = outpars.frms_pedsig_phot[i];
    }
    *fLog << endl;

    fPedest->SetReadyToSave();

    // Used to communicate the mean over all pixels
    // pedestal RMS into the Runs NTuple, as it might
    // be used for e.g. quality cuts.
    // float   fpedrms_mean;
    *fLog << "Pedestal RMS: " << outpars.fpedrms_mean << endl;

    // The average current over the active pixels
    // for this run. This is done separately for
    // ON and OF runs.
    //float   fcurrent_mean;

    // enum eERRORTOLERANCE {CAUTIOUS=0, GOODPHYSICS, TANK} eerrortolerance;
    /* 0 == "cautious", exits on any reason (but tells in
     * the .err file,
     * 1 == "goodphysics", exits when physics could be affected
     * by the error,
     * 2 == "tank", never exits except on coredumps and when
     * all files have been processed. Do not use such files for
     * physics analysis!
     *
     * NOTE: the capital letter words are the enums, the small letter
     * words must be used inside the parameter file. */

    // enum eMCTRIGGERFLAG {ALL=0, FLAG, NOFLAG} emctriggerflag;
    /* all: all events which survive the filter are written to the
     *      events NTuple.
     * flag: When Dorota's triggerflag is set to 1 for a particular
     *       event, it shall be written to the output. All others shall
     *       just be disregarded. (Default)
     * noflag: Opposite of 'flag': only events with triggerflag = 0 shall
     *         be treated further. */

    *fLog << "Particle Id #" << outpars.imcparticle << endl;
    *fLog << "Right Ascension: " << outpars.dsourcera_hours << "h" << endl;
    *fLog << "Declination: " << outpars.dsourcedec_deg << "deg" << endl;

    // Next statement commented out because bmontecarlo was set wrongly
    //fIsMcFile = outpars.bmontecarlo==TRUE;
    fIsMcFile = (outpars.dsourcera_hours==0 && outpars.dsourcedec_deg==0 &&
                 outpars.imcparticle != 0);

    if (fIsMcFile != (outpars.bmontecarlo==TRUE))
    {
        *fLog << "File tells you that it is a ";
        *fLog << (outpars.bmontecarlo ? "Monte Carlo" : "Real Data");
        *fLog << " file." << endl;
    }

    *fLog << "File detected as a ";
    *fLog << (fIsMcFile ? "Monte Carlo" : "Real Data");
    *fLog << " file." << endl;
    *fLog << " " << endl;
}

// --------------------------------------------------------------------------
//
// Read CT1 PreProc File Header:
//
Int_t MCT1ReadPreProc::ReadRunHeader()
{
    char cheadertitle[iHEADERTITLELENGTH];
    fIn->read(cheadertitle, iHEADERTITLELENGTH);

    TString s = cheadertitle;
    TString m = cTITLE_TEMPLATE;

    if (!s.BeginsWith(m(0, m.First('%'))))
        return kFALSE;

    *fLog << cheadertitle << flush;

    // cTITLE_TEMPLATE "PREPROC V%f/S%f CT %d RUN %d %d PROCMJD %d\n"
    struct outputpars outpars;

    int dummy;

    Float_t fpreprocversion, structversion;
    sscanf(cheadertitle, cTITLE_TEMPLATE,
           &fpreprocversion,    &structversion,
           &outpars.itelescope, &outpars.irunnum,
           &dummy/*&outpars.eruntype*/, &outpars.iproc_mjdate);

    if (fpreprocversion<0.6)
    {
        *fLog << err << "Sorry, only files from PreProc V0.6 and newer are supported." << endl;
        return kFALSE;
    }

    //
    // This is a stupid way of getting rid of numerical uncertanties when
    // comparing floating point numbers (Argh...)
    //
    TString s1 = Form("%.2f", structversion);
    TString s2 = Form("%.2f", STRUCT_VERSION);

    if (s1 != s2)
    {
        *fLog << warn << "WARNING: Version of C-structures of file (V";
        *fLog << s1 << ") not identical with current structures (V";
        *fLog << s2 << ")" << endl;
    }

    fIn->read((char*)&outpars, sizeof(struct outputpars));

    ProcessRunHeader(outpars);

    //rwagner: ReInit whenever new run commences
    // rc==-1 means: ReInit didn't work out

    MTaskList *tlist = (MTaskList*)fParList->FindCreateObj("MTaskList");
    if (!tlist)
        return -1;

    if (!tlist->ReInit(fParList))
        return -1;

    return kTRUE;
}

Int_t MCT1ReadPreProc::ReadRunFooter()
{
    char cheadertitle[iHEADERTITLELENGTH];
    fIn->read(cheadertitle, iHEADERTITLELENGTH);
    /*
     sscanf(cheadertitle, cEND_EVENTS_TEMPLATE,
     &filterres.ifilter_passed_evts);
     */

    TString s = cheadertitle;
    TString m = cEND_EVENTS_TEMPLATE;
    Int_t p = m.First('%');


    if (!s.BeginsWith(m(0,p)))
    {
        fIn->seekg(-iHEADERTITLELENGTH, ios::cur);
        return 0;
    }

    *fLog << inf << cheadertitle << flush;

    struct filterresults filterres;
    fIn->read((char*)&filterres, sizeof(struct filterresults));
    /*
     int   imax_alt_arcs;            // maximum altitude reached during the run
     int   iaz_at_max_alt_arcs;      // azimuth at the time the max. alt. was reached
     int   itimeaverage_alt_arcs;    // altitude averaged over the runtime
     int   icoord_inconsist_evts;    // number of events with time-coordinate inconsistency in this run
     int   ifilter_passed_evts;      // number of events which passed the filter
     int   imuon_fail_evts;          // number of events rejected as muons (other filters passed)
     int   i2out_fail_evts;          // number of events which failed in the two out of all pixels software trigger
     int   imuon_and_2out_fail_evts; // number of events failing in both muon and 2out filter
     int   isum_fail_evts;           // number of events which failed the sum-of-all-calibrated ADC counts filter
     int   isum_and_muon_fail_evts;  // number of events which failed in both the sum and the muon filter
     int   isum_and_2out_fail_evts;  // number of events which failed in both the sum and the 2out filter
     int   iall_filters_fail_evts;   // number of events which failed in all filters
     float favg_event_rate_hz;       // average rate before filtering
     float fstddev_event_rate_hz;    // standard deviation of the rate before filtering
     */

    if (fNumEventsInRun!=(UInt_t)filterres.ifilter_passed_evts)
    {
        *fLog << err << "ERROR! Number of events in run (" << (UInt_t)filterres.ifilter_passed_evts;
        *fLog << ") doesn't match number of read events (";
        *fLog << fNumEventsInRun << ")" << endl;
        *fLog << "       File corrupted." << endl;
        return -1;
    }

    fNumFilterEvts += fNumEventsInRun;
    fNumRuns++;

    *fLog << inf << "Read " << fNumEventsInRun << " events from run (Total=";
    *fLog << fNumFilterEvts << "/" << fEntries << " [";
    *fLog << 100*fNumFilterEvts/fEntries << "%], Runs=" << fNumRuns << ")";
    *fLog << endl;

    return 1;
}

// --------------------------------------------------------------------------
//
// This opens the next file in the list and deletes its name from the list.
//
Bool_t MCT1ReadPreProc::OpenNextFile()
{
    //
    // open the input stream and check if it is really open (file exists?)
    //
    if (fIn)
        delete fIn;
    fIn = NULL;

    //
    // Check for the existence of a next file to read
    //
    if (fNumFile >= (UInt_t)fFileNames->GetSize())
        return kFALSE;

    TNamed *file = (TNamed*)fFileNames->At(fNumFile);

    //TNamed *file = (TNamed*)fFileNames->GetFirst();
    //if (!file)
    //    return kFALSE;

    //
    // open the file which is the first one in the chain
    //
    const TString name = file->GetName();

    const char *expname = gSystem->ExpandPathName(name);
    const TString fname(expname);
    delete [] expname;

    //
    // Remove this file from the list of pending files
    //
    //fFileNames->Remove(file);

    *fLog << inf << "Open file: '" << name << "'" << endl;

    if (!CheckHeader(fname))
        return kFALSE;

    fNumFile++;

    fIn = new ifstream(fname);
    if (!*fIn)
    {
        *fLog << err << "Cannot open file " << fname << ": ";
        *fLog << strerror(errno) << endl;
        return kFALSE;
    }

    *fLog << inf << "-----------------------------------------------------------------------" << endl;

    switch (ReadRunHeader())
    {
    case kFALSE:
        *fLog << warn << "Unable to read first run header... skipping file." << endl;
        return kFALSE;
    case -1:
        *fLog << warn << "ReInit of Tasklist didn't succeed." << endl;
        return kFALSE;
    default:
        *fLog << "After opening next file: Number of Events #" << fNumEventsInRun << endl;
        return kTRUE;
    }
}

Bool_t MCT1ReadPreProc::CheckHeader(const TString fname) const
{
    ifstream fin(fname);
    if (!fin)
    {
        *fLog << dbginf << err << "ERROR - Cannot open file '" << fname << "'" << endl;
        return kFALSE;
    }

    char cheadertitle[iHEADERTITLELENGTH];
    fin.read(cheadertitle, iHEADERTITLELENGTH);

    Float_t fpreprocversion, structversion;
    Int_t dummyi;

    sscanf(cheadertitle, cTITLE_TEMPLATE,
           &fpreprocversion, &structversion,
           &dummyi, &dummyi, &dummyi, &dummyi);

    if (fpreprocversion < 0.6)
    {
        *fLog << dbginf << err << "ERROR - You must use PreProc V0.6 or higher." << endl;
        return kFALSE;
    }

    if (STRUCT_VERSION > structversion)
    {
        *fLog << warn << "WARNING: Version of C-structures of file (V";
        *fLog << structversion << ") newer than current structures (V";
        *fLog << STRUCT_VERSION << ")" << endl;
    }

    *fLog << "Current structures: " << STRUCT_VERSION << "  ";
    *fLog << "Structures in file: " << structversion << "  ";
    *fLog << "Used preproc version: " << fpreprocversion << endl;

    return kTRUE;
}


Int_t MCT1ReadPreProc::GetNumEvents(const TString fname) const
{
    *fLog << inf << "Scanning file " << fname << " for size" << flush;

    ifstream fin(fname);
    if (!fin)
    {
        *fLog << dbginf << err << "ERROR - Opening file." << endl;
        return 0;
    }

    const TString m(cEND_EVENTS_TEMPLATE);
    const Int_t p = m.First('%');
    const TString test = m(0, p);

    Int_t nevts = 0;
    Int_t nruns = 0;

    while (!fin.eof() && fin.peek()!=EOF)
    {
        fin.seekg(iHEADERTITLELENGTH, ios::cur);
        fin.seekg(sizeof(struct outputpars), ios::cur);

        while (1)
        {
            if (fin.peek()==cEND_EVENTS_TEMPLATE[0])
            {
                char cheadertitle[iHEADERTITLELENGTH];
                fin.read(cheadertitle, iHEADERTITLELENGTH);

                const TString s = cheadertitle;
                if (s.BeginsWith(test))
                {
                    fin.seekg(sizeof(struct filterresults), ios::cur);
                    nruns++;
                    break;
                }

                fin.seekg(-iHEADERTITLELENGTH, ios::cur);
            }

            fin.seekg(sizeof(struct eventrecord), ios::cur);
            if (fin.eof())
                break;

            nevts++;
        }
        *fLog << "." << flush;
    }

    *fLog << "done." << endl;
    *fLog << "Found " << nevts << " events in " << nruns << " runs." << endl;

    return nevts;
}

Bool_t MCT1ReadPreProc::Rewind()
{
    fNumFilterEvts = 0;
    fNumEvents     = 0;
    fNumRuns       = 0;
    fNumFile       = 0;
    if (fIn)
        delete fIn;

    fIn=NULL;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Open the first file in the list. Check for the output containers or create
// them if they don't exist.
//
// Initialize the size of the MPedPhotCam container to 127 pixels (CT1 camera)
//
Int_t MCT1ReadPreProc::PreProcess(MParList *pList)
{
    fParList = pList;

    //
    //  look for the HourAngle container in the plist
    //
    fHourAngle = (MParameterD*)pList->FindCreateObj("MParameterD", "HourAngle");
    if (!fHourAngle)
        return kFALSE;
    fHourAngle->SetTitle("Store the CT1 hour angle [deg]");

    //
    //  look for the ThetaOrig container in the plist
    //
    fThetaOrig = (MParameterD*)pList->FindCreateObj("MParameterD", "ThetaOrig");
    if (!fThetaOrig)
        return kFALSE;
    fThetaOrig->SetTitle("Store the original CT1 zenith angle [rad]");

    //
    //  look for the MCerPhotEvt class in the plist
    //
    fNphot = (MCerPhotEvt*)pList->FindCreateObj("MCerPhotEvt");
    if (!fNphot)
        return kFALSE;

    //
    //  look for the pedestal class in the plist
    //
    fPedest = (MPedPhotCam*)pList->FindCreateObj("MPedPhotCam");
    if (!fPedest)
        return kFALSE;

    //
    //  look for the time class in the plist
    //
    fTime = (MTime*)pList->FindCreateObj("MTime");
    if (!fTime)
        return kFALSE;

    //
    //  look for the pedestal class in the plist
    //
    fBlinds = (MBadPixelsCam*)pList->FindCreateObj("MBadPixelsCam");
    if (!fBlinds)
        return kFALSE;

    //
    //  look for the source position in the camera
    //
    fSrcPos = (MSrcPosCam*)pList->FindCreateObj("MSrcPosCam");
    if (!fSrcPos)
        return kFALSE;

    //
    //  look for the camera geometry
    //
    fGeom = (MGeomCam*)pList->FindCreateObj("MGeomCamCT1", "MGeomCam");
    if (!fGeom)
        return kFALSE;

    //
    //  look for the mc event class
    //
    fMcEvt = (MMcEvt*)pList->FindCreateObj("MMcEvt");
    if (!fMcEvt)
        return kFALSE;

    //
    //  look for the mc trigger class
    //
    fMcTrig = (MMcTrig*)pList->FindCreateObj("MMcTrig");
    if (!fMcTrig)
        return kFALSE;

    //
    //  look for the raw run header class
    //
    fRawRunHeader = (MRawRunHeader*)pList->FindCreateObj("MRawRunHeader");
    if (!fRawRunHeader)
        return kFALSE;

    fBinningT = (MBinning*)pList->FindObject("BinningTheta");

    Rewind();

    fPedest->Init(*fGeom);

    return GetSelector() ? GetSelector()->CallPreProcess(pList) : kTRUE;
}


// --------------------------------------------------------------------------
//
// Smear Theta uniformly in a bin of Theta
//   theta [rad]
//   SmearTheta [rad]
//
Double_t MCT1ReadPreProc::SmearTheta(Double_t theta)
{
    if (!fBinningT)
        return theta;

    const Int_t bin = fBinningT->FindLoEdge(theta * 180/TMath::Pi());
    if (bin<0)
        return theta;

    // smear Theta within the Theta bin
    const Double_t low = fBinningT->GetEdges()[bin];
    const Double_t up  = fBinningT->GetEdges()[bin+1];

    // "up-": Do not allow the upper edge
    return (up - gRandom->Uniform() * (up-low)) * TMath::Pi()/180;
}

// --------------------------------------------------------------------------
//
// Discretize Theta according to the binning in Theta
//   theta [rad]
//   DiscreteTheta [rad]  (= bin center)
//
Double_t MCT1ReadPreProc::DiscreteTheta(Double_t theta)
{
    if (!fBinningT)
        return theta;

    const Int_t bin = fBinningT->FindLoEdge(theta * 180/TMath::Pi());
    if (bin<0)
        return theta;

    const Double_t low = fBinningT->GetEdges()[bin];
    const Double_t up  = fBinningT->GetEdges()[bin+1];

    return 0.5*(up+low) * TMath::Pi()/180;
}

// --------------------------------------------------------------------------
//
// Analyse the event data, means store and copy the needed data into
// Mars structures and data members
//
Bool_t MCT1ReadPreProc::ProcessEvent(const struct eventrecord &event)
{
    /*
    if (fRawRunHeader->GetRunNumber() == 1)
    {
        *fLog << "eventrecord" << endl;
        *fLog << "isecs_since_midday = " << event.isecs_since_midday << endl;
        *fLog << "isecfrac_200ns     = " << event.isecfrac_200ns << endl;
        *fLog << "snot_ok_flags      = " << event.snot_ok_flags << endl;
        *fLog << "ialt_arcs          = " << event.ialt_arcs << endl;
        *fLog << "iaz_arcs           = " << event.iaz_arcs << endl;
        *fLog << "ipreproc_alt_arcs  = " << event.ipreproc_alt_arcs << endl;
        *fLog << "ipreproc_az_arcs   = " << event.ipreproc_az_arcs << endl;
        *fLog << "ifieldrot_arcs     = " << event.ifieldrot_arcs << endl;

        *fLog << "srate_millihz      = " << event.srate_millihz << endl;
        *fLog << "fhourangle         = " << event.fhourangle << endl;
        *fLog << "fmcenergy_tev      = " << event.fmcenergy_tev << endl;
        *fLog << "fmcsize_phel       = " << event.fmcsize_phel << endl;
        *fLog << "imcimpact_m        = " << event.imcimpact_m << endl;
        *fLog << "imcparticle        = " << event.imcparticle << endl;
        *fLog << "imctriggerflag     = " << event.imctriggerflag << endl;
    }
    */

    // reset blind pixels for this event
    fBlinds->Clear();
    for (int i=0; i<iMAXNUMPIX; i++)
        if (fBlnd[i]==1)
            (*fBlinds)[i].SetUnsuitable(MBadPixelsPix::kUnsuitableRun);

    // reset pedestal RMS for this event
    for (Int_t i=0; i<iMAXNUMPIX; i++)
        (*fPedest)[i].Set(0, fPedRMS[i]);

    //  int   isecs_since_midday; // seconds passed since midday before sunset (JD of run start)
    //  int   isecfrac_200ns;     // fractional part of isecs_since_midday
    fTime->SetCT1Time((UInt_t)fRawRunHeader->GetRunStart().GetMjd(), event.isecfrac_200ns, event.isecs_since_midday);
    fTime->SetReadyToSave();

    /*
     --- USEFULL? NEEDED? ---
     short snot_ok_flags;      // the bits in these two bytes are flags for additional information on the event: Everything OK =: all Bits = 0

     // for ALT-AZ mount telescopes: rotation angle of the field of
     // view; this angle is defined mathematically positive looking
     // towards the sky as the angle between the hour circle through
     // the object being tracked and the line through pixel 1 and 2
     int   ifieldrot_arcs;

     // event rate in milli Hertz before filtering calculated by
     // iratecalc_numevents_odd/(time[i+iratecalc_numevents_odd/2] -
     // time[i-iratecalc_numevents_odd/2])
     // For the first and the last iratecalc_numevents_odd/2
     // events the rate is assumed to be constant
     unsigned short srate_millihz;

     // This is the angle between the observation of this event and the
     // culmination point. It is going to be written into the events NTuple.
     float fhourangle;
     */

    //
    // read in the number of cerenkov photons and add the 'new' pixel
    // too the list with it's id, number of photons and error
    // number of photoelectrons measured in each pixel only the
    // actual number of pixels (outputpars.inumpixels) is written out
    // short spixsig_10thphot[iMAXNUMPIX];
    //
    for (Int_t i=0; i<iMAXNUMPIX; i++)
    {
      //*fLog << event.spixsig_10thphot[i] << " ";

      if (event.spixsig_10thphot[i]==0)
            continue;

        fNphot->AddPixel(i, 0.1*event.spixsig_10thphot[i], (*fPedest)[i].GetRms());
    }
    fNphot->FixSize();
    fNphot->SetReadyToSave();

    // int ipreproc_alt_arcs; // "should be" alt according to preproc (arcseconds)
    // int ipreproc_az_arcs;  // "should be" az according to preproc (arcseconds)

    // smear Theta in its Theta bin
    const Double_t theta = TMath::Pi()*(0.5-1./180*event.ialt_arcs/3600);
    fThetaOrig->SetVal(theta);

    // store hour angle
    fHourAngle->SetVal(event.fhourangle);

    fMcEvt->Fill(event.isecs_since_midday,     //0, /*fEvtNum*/
                 fIsMcFile ? event.imcparticle : 0, /*corsika particle type*/
                 fIsMcFile ? event.fmcenergy_tev*1000 : 0,
		 0, /* fThi0 */
		 0, /* fFirTar */
                 0, /* fzFirInt */
		 // 0, /* fThet*/
		 // rwagner: The following should be theta, right? Check with
		 // altitude fill some lines down...
		 0, // altitude (arcseconds)
                 0, /* fPhii */
                 0, /* fCorD */
                 0, /* fCorX */
                 0, /* fCorY */
                 fIsMcFile ? event.imcimpact_m*100 : 0,
                 TMath::Pi()/180*event.iaz_arcs/3600, // azimuth (arcseconds)
		 // fIsMcFile ? SmearTheta(theta) : theta,
                 DiscreteTheta(theta),
                 0, /* fTFirst */
		 0, /* fTLast */
		 0, /* fL_Nmax */
		 0, /* fL_t0 */
		 0, /* fL_tmax */
		 0, /* fL_a */
		 0, /* fL_b */
		 0, /* fL_c */
		 0, /* fL_chi2 */
		 0, /* uiPin */
		 0, /* uiPat */
		 0, /* uiPre */
		 0, /* uiPco */
		 0, /* uiPelS */
                 (int)(fIsMcFile ? event.fmcsize_phel : 0), /* uiPelC, Simulated SIZE */
		 0, /* elec */
		 0, /* muon */
		 0  /* other */
                 );

    fMcTrig->SetFirstLevel(event.imctriggerflag);    // MC data from Dorota get a triggerflag: 1 means triggered, 0 not. */

    fMcTrig->SetReadyToSave();
    fMcEvt->SetReadyToSave();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Because of the file format of the preproc output we have to check at any
// event where in the file stream we are...
//
Bool_t MCT1ReadPreProc::CheckFilePosition()
{
    //
    // Means: If no file is open (first call) try to open the first file
    //
    if (!fIn)
        return kFALSE;

    //
    // Because we can have 0-event runs in the file we loop as often
    // as we don't find a new footer-header combination.
    //
    while (1)
    {
        //
        // If the first character isn't the first of the footer it must be
        // an event
        //
        if (fIn->peek()!=cEND_EVENTS_TEMPLATE[0])
            return kTRUE;

        //
        // Try reading the footer. If this isn't successful...
        // must be an event
        //
        switch (ReadRunFooter())
        {
        case -1:
            return kFALSE;
        case 0:
            return kTRUE;
        }

        *fLog << inf << "Footer found." << endl;

        const char c = fIn->peek();

        //
        // No after reading the footer check if we reached the end of the file
        //
        if (fIn->eof() || c==EOF)
        {
            *fLog << "End of file." << endl;
            return kFALSE;
        }

        //
        // If the eof isn't reached a new header must follow. Check for it.
        //
        if (c!=cTITLE_TEMPLATE[0])
        {
            *fLog << inf << "Error finding new run header in file (possible EOF)... skipping rest of file." << endl;
            return kFALSE;
        }

        *fLog << "-----------------------------------------------------------------------" << endl;


        if (ReadRunHeader() < 0)
        {
            *fLog << warn << "ReInit of Tasklist didn't succeed." << endl;
            return kFALSE;
        }
    }
}

// --------------------------------------------------------------------------
//
// Check for the event number and depending on this number decide if
// pedestals or event data has to be read.
//
// If the end of the file is reached try to open the next in the list. If
// there is now next file stop the eventloop.
//
Int_t MCT1ReadPreProc::Process()
{
    //
    // Check where in the file we are. If neither a new event, nor a
    // footer/header combination is detected go to the next file.
    //
    if (!CheckFilePosition())
        if (!OpenNextFile())
            return kFALSE;

    //
    // Check for a selector. If one is given and returns kFALSE
    // skip this event.
    //
    if (GetSelector())
    {
        //
        // Make sure selector is processed
        //
        if (!GetSelector()->CallProcess())
        {
            *fLog << err << dbginf << "Processing Selector failed." << endl;
            return kFALSE;
        }

        //
        // Skip Event
        //
        if (!GetSelector()->IsConditionTrue())
        {
            fIn->seekg(sizeof(struct eventrecord), ios::cur);

            fNumEvents++;
            fNumEventsInRun++;

            return kCONTINUE;
        }
    }

    // event data to be read from the file
    struct eventrecord event;



    // read the eventrecord from the file
    fIn->read((char*)&event, sizeof(struct eventrecord));

    switch (ProcessEvent(event))
    {
    case kFALSE:
        return kFALSE;
    case kCONTINUE:
        return kCONTINUE;
    }

    fNumEvents++;
    fNumEventsInRun++;

    return kTRUE;
}

Int_t MCT1ReadPreProc::PostProcess()
{
    *fLog << all;
    *fLog << "Number events passed the filter: " << fNumFilterEvts << endl;
    *fLog << "Number of Events read from file: " << fNumEvents << endl;
    *fLog << "Number of Runs read from file:   " << fNumRuns << endl;
    *fLog << "Number of events detected first: " << fEntries << endl;

    if (fNumEvents!=fNumFilterEvts)
    {
        *fLog << warn << "WARNING! Number of events in file doesn't match number of read events..." << endl;
        *fLog << "         File might be corrupt." << endl;
    }

    delete fIn;
    fIn = NULL;

    return GetSelector() ? GetSelector()->CallPostProcess() : kTRUE;
}
