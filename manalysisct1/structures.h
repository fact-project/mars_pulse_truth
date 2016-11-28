/******************************************************************/
/*              GGG   AAA   M   M M   M  AAA   SSS                */
/*             G   G A   A  MM MM MM MM A   A S   S               */
/*             G     A   A  M M M M M M A   A  SS                 */
/*             G  GG AAAAA  M   M M   M AAAAA   SS                */
/*             G   G A   A  M   M M   M A   A S   S               */
/*              GGG  A   A  M   M M   M A   A  SSS                */
/*       Gamma Astronomer's Munich Madrid Analysis Suite          */
/*   An Atmospheric Cherenkov Telescope Data Analysis Software    */
/*  MPI f"ur Physik, M"unchen & Universidad Complutense, Madrid   */
/******************************************************************/

/******************************************************************/
/* This File belongs to the programs                              */
/*                                                                */
/*  P R E P R O C E S S O R   and   I M A G E R                   */
/*                                                                */
/* Purpose: provide the structure definitions common to both      */
/*          programs, especially the output structures of the     */
/*          preprocessor                                          */ 
/*                                                                */
/******************************************************************/

/******************************************************************/
/* @(#) File name structures.h
   @(#) latest SID 0.5  
   @(#) Date of latest delta 26/6/00  
   Author: D. Petry 
   changed by M. Kestel */
/******************************************************************/

#ifndef _structures_h_
#define _structures_h_ "__structures_h__"

#define STRUCT_VERSION 0.6

#define iHEADERTITLELENGTH 60
/* length of the string written at the beginning of the */
/* output for each runfile */
#define cTITLE_TEMPLATE "PREPROC V%f/S%f CT %d RUN %d %d PROCMJD %d\n"

#define cEND_EVENTS_TEMPLATE "EVENTS AFTER FILTER = %d\n"

#define iMAXNUMPIX 127
/* maximum value for the number of camera pixels (excl. muon shield) */
#define iMAXNUMMUONPIX 5 
/* maximum value for the number of pixels in the muon shield */
#define iMAXNUMCOINTDCS 5
/* maximum value for the number of coincidence tdcs */


#define iBADMACTIME_BIT 1
/* snot_ok_flags is ORed with this number to set the bit which 
 * indicates that the mactime measurement didn't work */
#define iBADRUBTIME_BIT 2 
/* snot_ok_flags is ORed with this number to set the bit which 
 * indicates that the Rubidium time measurement didn't work */
#define iBADPOINTING_BIT 4 
/* snot_ok_flags is ORed with this number to set the bit which 
 * indicates that the shaft encoder values were inconsistent with 
 * time OR source coordinates */
#define iMUON_BIT 8
 /* snot_ok_flags is ORed with this number to set the bit which 
  * indicates that the event didn't pass the muon filter 
  * (for test puposes) */

#define i2OUTFAIL_BIT 16 
/* snot_ok_flags is ORed with this number to set the bit which 
 * indicates that the event didn't pass the "two out of all" 
 * software trigger */
#define iSUMFAIL_BIT 32 
/* snot_ok_flags is ORed with this number to set the bit which
 * indicates that the event didn't pass the "sum of all
 * photoelectrons" filter */

#define iPARTICLETYPE_FACTOR 1024 
/* for MC data: the particle type multiplied with this number 
 * is ORed with snot_ok_flags */

enum onoroff {NEITHER_ON_NOR_OFF, OFF_SOURCE, ON_SOURCE};
enum trackmodes {NORMAL, REVERSE, DUNNO};
enum axes {RA, DEC, ALT, AZ};
enum eERRORTOLERANCE {CAUTIOUS=0, GOODPHYSICS, TANK};
enum eMCTRIGGERFLAG {ALL=0, FLAG, NOFLAG};
 
struct outputpars {
  /* preprocessing parameters that are passed to imager.
   * Note: Not all parameters from the parameter file are passed to
   * imager, only those that are repeated in this struct outputpars 
   * here !! That's why these two structs must be the same for both
   * preproc and imager. */

  int     itelescope;
  /* number of the CT which took the data (from parameters file */
  /* crosschecked with run file name) */
  float   flongitude_deg;
  /* longitude (counted positive towards West) of CT position */
  float   flatitude_deg;
  /* latitude (counted positive towards North) of CT position */
  int     irunnum;
  /* run number (from parameters file cross checked with run */
  /* file name) */
  enum onoroff eruntype;
#ifdef LINUX
  int dummy;
#endif
  /* indicates if the */
  /* run is on- or off source (from runfile cross checked with */
  /* file name) */
  double  dsourcera_hours;
  /* right ascension of observed source in hours */
  double  dsourcedec_deg; 
  /* declination of observed source in degrees */
  int     inumpixels; 
  /* number of pixels in the camera (from parameters file cross */
  /* checked with run file, calibration file and pedestal file) */
  int     inummuonpixels;
  /* number of pixels in the muon shield (from parameters file */
  /* cross checked with run file, calibration file and pedestal */
  /* file) */
  int     inumcointdcs;
  /* number of coincidence tdcs recorded in the runfile (from */
  /* parameters file cross checked */
  /* with run file) */
  float   fpixdiameter_deg;
  /* smallest pixel diameter (degrees) (from parameters file) */ 
  enum axes ese1_is;
  /* name of the axis to which shaft encoder 1 is attached */
  /* (implies the type of mount) */
  int     isezeropos[2];
  /* zero position of shaftencoders 1 and 2 (from parameters file) */
  int     iaz_rev_track_corr;
  /* correction for the azimuth shaft encoder (ALT/AZ mount */
  /* only) in reverse tracking mode */
  int     ialt_rev_track_corr;
  /* correction for the altitude shaft encoder (ALT/AZ mount */
  /* only) in reverse tracking mode */
  float   fbendingcorr;
  /* bending correction factor (ALT/AZ mount only) */
  float   fextinction;
  /* atmospheric extinction (typically taken from the Carlsberg */
  /* Meridian Circle data) */
  Boolean bdontusepix[iMAXNUMPIX];
  /* bdontusepix is set true if the pixel should due to whatever
   * reason not be used in image analysis, otherwise it is true; 
   * 
   * bdontusepix is set by the user inside preproc.par and is 
   * untouched by preproc (version >= 0.4). */
  Boolean bexcludepix[iMAXNUMPIX];
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
  float   fphotoel_per_adccnt[iMAXNUMPIX];
  /* conversion factors for the pixel signals */
  int     irubminusutc_usecs;
  /* difference between rubidium clock and UTC in microseconds */
  int     isum_thresh_phot;
  /* threshold for the total sum of photoelectrons filter */
  /* from the parameters file */
  int     i2out_thresh_phot;
  /* threshold for the two-pixels-out-of-all software */
  /* trigger from parameters file */
  int     imuoncut_thresh_adccnt[iMAXNUMMUONPIX]; 
  /* thresholds for the muon cut */
  Boolean bmuon_suppression;
  /* "Dolby" noise reduction flag */
  float   ftolerated_pointerror_deg;
  /* maximum tolerated pointing error in the position 
   * check in iprocessdata */
  float   fxpointcorr_deg;
  /* pointing correction (to be added along the camera
   * x axis) e.g. from point run */ 
  float   fypointcorr_deg;  
  /* pointing correction (to be added along the camera 
   * y axis) e.g. from point run */ 
  float   fcamera_align_angle_deg;
  /* the angle between the camera y-axis and the meridian 
   * when a culminating object is observed (defined 
   * counter-clockwise looking at the sky) */ 
  int     iratecalc_numevents_odd;
  /* number of events used in the rate calculation (must be odd) */
  int     inumpedfile;
  /* number of the pedestal file used */
  int     inumpedrun;
  /* number of the pedestal run used in the file (starting at 0) */
  int     inumcalfile;
  /* number of the calibration file used */
  int     inumlaserrun;
  /* number of the laserrun used in the file (starting at 0) */ 
  int     inumtellogfile;
  /* number of the TelLog file to be used */
  int     inumtellogrun;
  /* number of the tellog entry (Runnumber) used from the log file */
  int imcparticle; /* CORSIKA-coded Monte Carlo particle type.
		    * This is given once per run. */

  /* preprocessing results: */

  int     istart_mjdate_day;
  /* MJD of run start (first event) */
  int     iend_mjdate_day;
  /* MJD of run end (last event) */
  int     irunduration_secs;
  /* difference between start and end time (secs) */
  int     iproc_mjdate; 
  /* MJD of data processing (i.e. creation of this file) */ 
  enum trackmodes etrackmode;
  /* tracking mode (for ALT/AZ CTs) */
  int     iproc_evts;
#ifdef LINUX
  int dummy2;
#endif
  /* number of events processed */
  double  dactual_sourcera_hours;
  /* for off runs: the false source (that should have been) observed */
  float   frms_pedsig_phot[iMAXNUMPIX];
  /* standard deviation of the calibrated signals from the pedestal run */ 
  float   fpedrms_mean; /* Used to communicate the mean over all pixels 
			 * pedestal RMS into the Runs NTuple, as it might 
			 * be used for e.g. quality cuts. */
  float   fcurrent_mean; /* The average current over the active pixels
			  * for this run. This is done separately for
			  * ON and OF runs. */

  enum eERRORTOLERANCE eerrortolerance;
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
  enum eMCTRIGGERFLAG emctriggerflag;
  /* all: all events which survive the filter are written to the 
   *      events NTuple. 
   * flag: When Dorota's triggerflag is set to 1 for a particular
   *       event, it shall be written to the output. All others shall
   *       just be disregarded. (Default)
   * noflag: Opposite of 'flag': only events with triggerflag = 0 shall
   *         be treated further. */
  Boolean bmontecarlo;
  /* if TRUE we have a monte carlo dataset before us, if FALSE 
   * (default value) it is just normal data. */
};

struct filterresults {
  int     imax_alt_arcs;
  /* maximum altitude reached during the run */
  int     iaz_at_max_alt_arcs;
  /* azimuth at the time the max. alt. was reached */
  int     itimeaverage_alt_arcs;
  /* altitude averaged over the runtime */
  int     icoord_inconsist_evts;
  /* number of events with time-coordinate */
  /* inconsistency in this run */
  int     ifilter_passed_evts;
  /* number of events which passed the filter */
  int     imuon_fail_evts;
  /* number of events rejected as muons (other filters passed) */
  int     i2out_fail_evts;
  /* number of events which failed in the two out of all */
  /* pixels software trigger */
  int     imuon_and_2out_fail_evts;
  /* number of events failing in both muon and */
  /* 2out filter */  
  int     isum_fail_evts;
  /* number of events which failed the sum-of-all-calibrated */
  /* ADC counts filter */
  int     isum_and_muon_fail_evts;
  /* number of events which failed in both the sum and */
  /* the muon filter */
  int     isum_and_2out_fail_evts;
  /* number of events which failed in both the sum and */
  /* the 2out filter */
  int     iall_filters_fail_evts;
  /* number of events which failed in all filters */ 
  float   favg_event_rate_hz;
  /* average rate before filtering */
  float   fstddev_event_rate_hz;
  /* standard deviation of the rate before filtering */

};

struct eventrecord {
  int   isecs_since_midday;
  /* seconds passed since midday before sunset (JD of run start) */
  int   isecfrac_200ns;
  /* fractional part of isecs_since_midday */
  short snot_ok_flags;
  /* the bits in these two bytes are flags for additional */
  /* information on the event: Everything OK =: all Bits = 0 */
  int   ialt_arcs;
  /* altitude (arcseconds) */
  int   iaz_arcs;
  /* azimuth (arcseconds) */
  int   ipreproc_alt_arcs;
  /* "should be" alt according to preproc (arcseconds) */
  int   ipreproc_az_arcs;
  /* "should be" az according to preproc (arcseconds) */
  int   ifieldrot_arcs;
  /* for ALT-AZ mount telescopes: rotation angle of the field of 
   * view; this angle is defined mathematically positive looking 
   * towards the sky as the angle between the hour circle through 
   * the object being tracked and the line through pixel 1 and 2 */
  unsigned short srate_millihz;
  /* event rate in milli Hertz before filtering calculated by 
   * iratecalc_numevents_odd/(time[i+iratecalc_numevents_odd/2] - 
   * time[i-iratecalc_numevents_odd/2]) 
   * For the first and the last iratecalc_numevents_odd/2 
   * events the rate is assumed to be constant */
  float fhourangle; /* This is the angle between the observation of this
		     * event and the culmination point. It is going to 
		     * be written into the events NTuple. */
  float fmcenergy_tev; /* Simulated Energy.... dropping through to
			* the Events NTuple. */
  float fmcsize_phel; /* Simulated SIZE.... dropping through to
			* the Events NTuple. */
  int imcimpact_m;
  /* MC data contain the impact parameter, which is given here in 
   * meters. */
  int imcparticle;
  /* MC data know which particle they are.... all in CORSIKA standard. */
  int imctriggerflag;
  /* MC data from Dorota get a triggerflag: 1 means triggered, 0 not. */
  short spixsig_10thphot[iMAXNUMPIX];
  /* number of photoelectrons measured in each pixel only the 
   * actual number of pixels (outputpars.inumpixels) is written out */
};

struct camera { /* camera parameters for imaging */
  int inumpixels;
  int inumrings;
  double dpixdiameter_deg;
  double dxc[iMAXNUMPIX]; 
/* Pixel coordinates in camera coordinate system (x points from
 * pixel 1 to 2). */
  double dyc[iMAXNUMPIX];
  /* The numbering of the pixels in these arrays starts at 0! */
  double dxpointcorr_deg; 
  /* correction of the pixel coordinates; to be added to dxc[] 
   * to get correct value */
  double dypointcorr_deg; 
  /* correction of the pixel coordinates; to be added to dxc[] 
   * to get correct value */
};


/* two structures for better file handling */
struct inputfile {
  char cname[iMAXFILENAMELENGTH];
  /* filename (including path) */
  FILE *pointer;
  /* filepointer */
  char ccannotopentext[161];
  /* Error text printed when file cannot be opened */
  int  icannotopencode; 
  /* Error code for the exit statement when file could not be opened */
  int  iearlyeofcode;
  /* Error code for the exit statement for unexpected EOF */
  int  ierroratclosecode;
  /* Error code for the exit statement for error while trying to close */
};

struct outputfile {
  char cname[iMAXFILENAMELENGTH]; /* filename (including path) */
  FILE *pointer;  /* filepointer */
  char ccannotopentext[161]; 
  /* Error text printed when file cannot be opened */
  int  icannotopencode; 
  /* Error code for the exit statement when file could not be opened */
  int  icannotwritecode; 
  /* Error code for the exit statement for failed fprintf */
  int  ierroratclosecode; 
  /* Error code for the exit statement for error while trying to close */
};

struct P2ALLHISTOS {

  HISTO *p2adchist;
  HISTO *p2pedonhist;
  HISTO *p2pedofhist;
  HISTO *p2pedonmeanhist;
  HISTO *p2pedofmeanhist;
  HISTO *p2pedonjitthist;
  HISTO *p2pedofjitthist;
  HISTO *p2calonhist;
  HISTO *p2calofhist;
  HISTO *p2calonmeanhist;
  HISTO *p2calofmeanhist;
  HISTO *p2calonjitthist;
  HISTO *p2calofjitthist;
  HISTO *p2phehist;
  HISTO *p2zenonhist;
  HISTO *p2zenofhist;
  HISTO *p2corrpedhist;
  HISTO *p2pedfwhmhist;
};

struct CALNT_DAT { 
  /* the structure for an entry in the calibration ntuple */
  float CTrunnum;
  float runtype;
  float ALT_deg; /* the ALT taken from the first event of the run */
  float extinct; /* extinction */
  float RtiSecBF; /* runtime in seconds before filter */
  float pixnum;  /* number of the pixel for which the following 
		  * data is given */
  float pixonoff; /* Quantity telling whether this pixel was switched
		   * off by the user (-= 2.) or by preproc (-= 1.).
		   * If ON, its value = 0. */
  float ped_adcc; /* the standard deviation of the pedestal as 
		   * calculated by TIJARAFE */
  float pedrmsac; /* the standard deviation of the pedestal in
		   * ADC counts as calculated by TIJARAFE */
  float pedrmsPE; /* the calibrated ped RMS in photoelectrons 
		   * which is also given to the imager */
  float las_adcc; /* the mean signal for this pixel from the 
		   * laser run */
  float lasrmsac; /* RMS of the laser events after cleaning from cosmics */
  float conv_fac; /* conversion factor = <Q>/Var(Q) * F^2 */
  float F;        /* The F from the line before */
  float rel_gain; /* the relative gain of the pixel */
  float numtrig;  /* number of events in which the pixel was 
		   * above trigger threshold */
  float num3sig;  /* number of events in which the pixel was 
		   * 3 sigma above its pedestal */ 
  float HV1;      /* Adjustment of HV1 for this run, if available. */
  float HV2;      /* Adjustment of HV2 for this run, if available. */
  float ThrCurr;
  float AvgCurr;  /* the sum of all currents during this run (with the
		   * particular runtype in question) from the TelLog 
		   * file. The currents are averaged over the 2-minute
		   * intervals but summed over all pixels with currents
		   * higher than the threshold current value. This sum
		   * is then stored in the runs ntuple. 
		   * Same for the scaler values --> */
  float AvgScal;
};

struct inputpars {
  Boolean bkeywordgiven[iNUMKEYWORDS+1]; /* if a valid keyword 
					  * is given in the 
					  * parameters file 
					  * the corresponding
					  * element in this 
					  * array is set to true */ 
  int     itelescope;       /* number of the CT which took the data */
  float   flongitude_deg;   /* longitude (counted positive 
			     * towards West) of CT position */
  float   flatitude_deg;    /* latitude (counted positive 
			     * towards North) of CT position */
  int     ifirstrunnum;     /* first run number to which these 
			     * parameters apply*/
  int     ilastrunnum;      /* last run number to which these 
			     * parameters apply, */
                            /* i.e. this run will be processed 
			     * with these parameters */
  int     inumpixels;       /* number of pixels in the camera */
  int     inummuonpixels;   /* number of pixels in the muon shield */
  int     inumcointdcs;     /* number of coincidence tdcs recorded 
			     * in the runfile */
  float   fpixdiameter_deg; /* smallest pixel diameter (degrees) */
  enum axes ese1_is;        /* name of the axis to which shaft
			     * encoder 1 is attached
			     * (implies the type of mount) (the 
			     * type axes is declared in structures.h) */
  float   fdegrees_per_step[2]; /* angular resolution of shaft 
				 * encoders 1 and 2 */
  int     isezeropos[2];    /* zero position of shaftencoders 1 and 
			     * 2 from parameters file */
  int     iaz_rev_track_corr; /* correction for the azimuth shaft 
			       * encoder (ALT/AZ mount only) in 
			       * reverse tracking mode */
  int     ialt_rev_track_corr; /* correction for the altitude 
				* shaft encoder (ALT/AZ mount only) 
				* in reverse tracking mode */
  float   fbendingcorr;     /* bending correction factor 
			     * (ALT/AZ mount only) */
  Boolean bdontusepix[iMAXNUMPIX]; /* bdontusepix is set true 
				    * if the pixel should due 
				    * to whatever reason not be 
				    * used in image analysis, 
				    * otherwise it is false;
				    * this is a copy of the 
				    * input from the parameters file */
  Boolean bdontusepix_in_trig[iMAXNUMPIX]; /* is set true if the 
					    * pixel should due 
					    * to whatever reason not 
					    * be used in the two out 
					    * of all trigger, otherwise 
					    * it is false; this is a 
					    * copy of the input from 
					    * the parameters file */
  float   fphotoel_per_adccnt[iMAXNUMPIX]; /* conversion factors for 
					    * the pixel signals */
  float   fextinction;      /* atmospheric extinction (typically 
			     * taken from the Carlsberg Meridian
			     * Circle data) */
  int     irubminusutc_usecs; /* difference between rubidium clock 
			       * and UTC in microseconds */
  int     isum_thresh_phot; /* threshold for the total sum of 
			     * photoelectrons filter from the 
			     * parameters file */
  int     i2out_thresh_phot; /* threshold for the 
			      * two-pixels-out-of-all software 
			      * trigger from parameters file */
  int     imuoncut_thresh_adccnt[iMAXNUMMUONPIX]; /* thresholds for 
						   * the muon cut */
  Boolean bmuon_suppression;       /* if true, the events which do 
				    * not pass the muon cut are not 
				    * written to the output */
  float   ftolerated_pointerror_deg; /* maximum tolerated pointing 
				      * error in the position check 
				      * in iprocessdata */
  float   fxpointcorr_deg;  /* pointing correction (to be added 
			     * along the camera x axis) e.g. 
			     * from point run */ 
  float   fypointcorr_deg;  /* pointing correction (to be added 
			     * along the camera y axis) e.g. 
			     * from point run */ 
  float   fcamera_align_angle_deg; /* the angle between the camera 
				    * y-axis and the meridian when 
				    * a culminating object is 
				    * observed (defined 
				    * counter-clockwise looking at 
				    * the sky) */ 
  int     iratecalc_numevents_odd; /* number of events used in the 
				    * rate calculation (must be odd) */
  enum pedsearchdirs {MATCH=0, BEFORE, AFTER, PINGPONG, 
		      PEAKFWHM, NONE} epedsearchdir ; 
  /* MATCH = only same number as run file (if ipedendofsearch is 0) or 
     exactly ipedendofsearch;
     BEFORE = search backwards until pedendofsearch; 
     AFTER = search forward until pedendofsearch, 
     PINGPONG = Try to fibnd the matching partner, try run#-1, run#+1,
     run#-2, run#+2 etc. up to the third argument, the partnerfindrange 
     NONE = use none of both. This is only useful for Monte Carlo
     Analysis, for everything else it is highly unrecommended.*/
  int     ipedendofsearch;  /* pedestal file number until which to 
			     * search (see epedsearchdir) */
  int     ipedsearchdepth;  /* depth, until which preproc should
			     * try to find TelPed* partnerfiles */
  enum    pedsearchdirs ecalsearchdir; 
  /* MATCH = only same number as run file (if ipedendofsearch is 0) or 
     exactly ipedendofsearch;
     BEFORE = search backwards until pedendofsearch; 
     AFTER = search forward until pedendofsearch, 
     PINGPONG = Try to fibnd the matching partner, try run#-1, run#+1,
     run#-2, run#+2 etc. up to the third argument, the partnerfindrange 
     NONE = use none of both. This is only useful for Monte Carlo
     Analysis, for everything else it is highly unrecommended.*/
  int     icalendofsearch;  /* calibration file number until which 
			     * to search (see ecalsearchdir)*/
  int     icalsearchdepth;  /* depth, until which preproc should
			     * try to find TelCal* partnerfiles */
  double  dsourcera_hours;  /* right ascension of observed source 
			     * in hours */
  double  dsourcedec_deg;   /* declination of observed source in 
			     * degrees */
  float   fpadlevel_phot_pix[iMAXNUMPIX];   /* if > 0., software 
					     * padding to this 
					     * level is applied 
					     * for the given pixel */
  float   fseed1; /* seed for random generators */
  float   fseed2;

  int iswap_partner_of_pix[iMAXNUMPIX]; /* if >= 0, pixel i is 
					 * swapped with 
					 * iswap_partner_of_pix[i] 
					 * in ireadrunfile, -pedfile, 
					 * -calfile (internal 
					 * numbering starting at 0)*/

  Boolean bnewhistograms; /* set to true if new histograms shall be booked */
  Boolean bhisto_on; /* set to true if histogramming is on */

  enum eERRORTOLERANCE eerrortolerance; 
  /* 0 == "cautious", exits on any reason (but tells in 
   * the .err file,
   * 1 == "normal", exits when physics could be affected
   * by the error,
   * 2 == "tank", never exits except on coredumps and when
   * all files have been processed. Do not use such files for
   * physics analysis! 
   *
   * NOTE: the capital letter words are the enums, the small letter
   * words are to be used inside the parameter file. */
  Boolean bmontecarlo;
  /* if TRUE we have a monte carlo dataset before us, if FALSE 
   * (default value) it is just normal data. */
  enum eMCTRIGGERFLAG emctriggerflag;
  /* all: all events which survive the filter are written to the 
   *      events NTuple. 
   * flag: When Dorota's triggerflag is set to 1 for a particular
   *       event, it shall be written to the output. All others shall
   *       just be disregarded. (Default)
   * noflag: Opposite of 'flag': only events with triggerflag = 0 shall
   *         be treated further. */
  char mcfile[FILENAME_MAX];
  float fmean_nsb_pe_pix[iMAXNUMPIX];
  Boolean bfitmypedrms;
  /* If TRUE, the pedestal MEAN and SIGMA=sqrt(MEAN) is fitted
   * from pickup corrected pedestal data and not calculated from 
   * the pickup corrected ADC values, as it was done up to 
   * ~November 2000. */
};

struct calibrationdata { /* Calibration (Laser Run) Data to be read 
			  * from the Te?Cal files */
  int iid; /* no useful information, is long in the Macintosh format */
  int itelescope; /* number of the CT which took the data */
  int irunnum; /* run number from the file name */
  enum onoroff eruntype; /* indicates if the run is on- or off 
			  * (taken from filename) */
  int inumruns; /* number of laser runs recorded in this file, is 
		 * long in the Macintosh format */
  char cname[iMAXFILENAMELENGTH]; /* copy of the filename used when 
				   * the data was read */
  double dmean_adc[iMAXNUMLASERRUNS][iMAXNUMPIX]; 
  /* means of pixel signals over all shots calculated by TIJARAFE /
   * Roberto: in processdata.c this number is recalculated. */
  double dmean_lasoutput[iMAXNUMLASERRUNS][iMAXNUMLASEREVENTS]; 
  /* This new (April 26 2000) quantity monitors the laser mean
   * output recorded in events, which are not considered cosmics. */
  double dmean_alladc[iMAXNUMPIX]; 
  /* means of pixel signals over all triggers calculated by 
   * preproc. */
  double dmean_laserjitter[iMAXNUMPIX]; 
  /* Here we calc. the deviation from the actual lasergain in a 
   * single laser shot to the laser mean as averaged over the camera */
  double drms_adc[iMAXNUMPEDRUNS][iMAXNUMPIX];
  /* the standard deviations of the pixel signals */
  int inumpixels; /* number of pixels in the camera, short in 
		   * Macintosh format */
  int inumevents[iMAXNUMLASERRUNS]; /* number laser shots, 
				     * short in Macintosh format */
  int ipixsig_adc[iMAXNUMLASERRUNS][iMAXNUMLASEREVENTS][iMAXNUMPIX]; 
  /* the signals obtained from each pixel for each laser shot and 
   * each laser run, unsigned short in the Macintosh format */
};

struct pedestaldata { /* Pedestal Run Data to be read from the 
		       * Te?Ped files */
  int iid; /* no useful information, is long in the Macintosh 
	    * format */
  int itelescope; /* number of the CT which took the data */
  int irunnum; /* run number from the file name */
  enum onoroff eruntype; /* indicates if the run is on- or of 
			  * (taken from filename) */
  int inumruns; /* number of pedestal events recorded in this file, 
		 * is long in the Macintosh format */
  char cname[iMAXFILENAMELENGTH]; /* copy of the filename used 
				   * when the data was read */
  double dmean_adc[iMAXNUMPEDRUNS][iMAXNUMPIX]; 
  /* means of pixel signals over all triggers calculated by 
   * TIJARAFE using a sigma cut of 2 */
  double dmean_alladc[iMAXNUMPIX]; 
  /* means of pixel signals over all triggers calculated by 
   * preproc. */
  double dmean_allPE[iMAXNUMPIX]; 
  /* means of pixel signals over all triggers calculated by 
   * preproc. */
  double dmean_pedlevel[iMAXNUMPEDRUNS][iMAXNUMPEDEVENTS]; 
  /* This new (preproc_0.4) quantity monitors the pedestal mean
   * output recorded in events, which are not considered cosmics. 
   * The calc.ing is done inside processdata.c */
  double dmean_pedlevelPE[iMAXNUMPEDRUNS][iMAXNUMPEDEVENTS]; 
  /* This new (preproc_0.4) quantity monitors the pedestal mean
   * output recorded in events, which are not considered cosmics. 
   * The calc.ing is done inside processdata.c */
  double dmean_pedoffset[iMAXNUMPIX]; 
  /* Mean Offset from the ADC spectrum of a pixel to the mean of the
   * pedestal events. */
  double dmean_pedoffsetPE[iMAXNUMPIX]; 
  /* Mean Offset from the ADC spectrum of a pixel to the mean of the
   * pedestal events. */
  double drms_adc[iMAXNUMPEDRUNS][iMAXNUMPIX];
  /* the standard deviations of the pixel signals used in the 
   * sigma cut mentioned above */
  double drms_alladc[iMAXNUMPIX];
  /* The RMS value of the as-is ADC spectrum for all events inside a 
   * Pedestal file. */
  double drms_allPE[iMAXNUMPIX];
  /* The RMS value of the as-is ADC spectrum for all events inside a 
   * Pedestal file. */
  double drms_nopickup[iMAXNUMPIX];
  /* the standard deviations of the pixel signals' deviation from
   * the events' mean (in ADC counts, of course...)  ---
   * This is the same as the RMS of the pickup corrected pedestals! */
  double drms_nopickupPE[iMAXNUMPIX];
  /* the standard deviations of the pixel signals' deviation from
   * the events' mean (in ADC counts, of course...)  ---
   * This is the same as the RMS of the pickup corrected pedestals! */
  int inumpixels; /* number of pixels in the camera, short in 
		   * Macintosh format */
  int inumevents[iMAXNUMPEDRUNS]; /* number of random triggers, 
				   * short in Macintosh format */
  int ipixsig_adc[iMAXNUMPEDRUNS][iMAXNUMPEDEVENTS][iMAXNUMPIX]; 
  /* the signals obtained from each pixel for each random trigger 
   * and each pedestal event, short in the Macintosh format */
}; 

struct runfiledata { /* Data read from Te?Run files */
  int iformatid; /* Format ID of the data, long in Macintosh format */
  int iversid;   /* Version ID of the data, long in Macintosh format */
  int irevid;    /* Revision ID..., long in Macintosh format */
  int itelescope; /* number of the CT which took the data */
  int irunnum; /* run number from the file name */
  int irunnumx; /* number read from the file, long in Macintosh format */
  int inumevents; /* number of events recorded in this run, long in 
		   * Macintosh format */
  char cname[iMAXFILENAMELENGTH]; /* copy of the filename used when 
				   * the data was read */
  int ieventheaderlength; /* number of bytes in the event header,
			   * long in Macintosh format */
  int ieventheaderoffset; /* unknown meaning, long in Macintosh 
			   * format */
  int isignalslength; /* number of bytes used by the pixel signals, 
		       * long in Macintosh format */
  int isignalsoffset; /* unknown meaning, long in Macintosh format */
  int itdccountslength; /* number of bytes used by the TDC signals, 
			 * long in Macintosh format */
  int itdccountsoffset; /* unknown meaning, long in Macintosh format */
  int iobservtime_mins; /* (planned) observational time (minutes), 
			 * short in Macintosh format */
  enum onoroff eruntype; /* indicates if the run is on- or off-source; 
			  * in the Macintosh format, this is a short
			  * where 1 means off and 2 means on */
  int ileapsecs; /* Leap seconds, introduced into UTC in order to 
		  * keep track of some very small differences 
		  * accumulated through the years. */
  int ihv1;
  int ihv2; /* these two numbers represent the HV adjustment made
	     * by the shiftpersonnel. The last three items are 
	     * only read for Roberto files, but not for Tijarafe 
	     * files, as e.g. the HVs are not written out correctly. */
  char cdummy[40]; /* unknown meaning */
  int imactime_secs[iMAXNUMRUNEVENTS]; 
  /* Macintosh time (time_t mydatum) in seconds since 1970, this is 
   * put together from two Macintosh shorts and converted to the time 
   * format required by SYSTEM */
  unsigned int uigpstime_secs[iMAXNUMRUNEVENTS]; 
  /* GPS time (time_t mydatum) in seconds since 1970, this is 
   * put together from two Macintosh shorts and converted to the time 
   * format required by SYSTEM 
   * This time is new from Roberto and is coming from the PC-GPS card.
   * There is another flag available in the runheader, which gives an
   * overall flag for the run, whether or not the GPS time had been
   * working. It is, however, not a good idea to use this flag, as 
   * the workability of this card can also be dedued from the GPS time
   * itself. This allows to check for failures on a event-by-event 
   * basis, which seems more reasonable than using the global flag. */
  unsigned int uigpstime_usecs[iMAXNUMRUNEVENTS]; 
  /* GPS microseconds -- available from the card, that's why it is 
   * put to the data, here it is.
   * This is put together from two Macintosh shorts and converted
   * to the time format required by SYSTEM */
  unsigned int uirubsecs_secs[iMAXNUMRUNEVENTS]; 
  /* the seconds of the Rubidium clock time this is put together 
   * from two Macintosh shorts and converted to the time format 
   * required by SYSTEM */
  unsigned int uirubsecfrac_200ns[iMAXNUMRUNEVENTS]; 
  /* the fractional part of the Rubidium clock time second, this is 
   * put together from two Macintosh shorts, unit is 200 ns */
  int isepos[2][iMAXNUMRUNEVENTS]; 
  /* positions of shaft encoders 1 and 2, short in Macintosh format */
  float fhourangle; /* This is the angle between the observation of this
		     * event and the culmination point. It is going to 
		     * be written into the events NTuple. */
  int ieventnumber[iMAXNUMRUNEVENTS]; 
/* number of the event, short in Macintosh format */
  int inumpixels; /* number of pixels in the camera, short in 
		   * Macintosh format */
  int inummuonpixels; /* number of pixels in  the muon shield; 
		       * the sum of inumpixels and inummuonpixels is 
		       * part of the event record but should be the 
		       * same for all events, so it is not put into 
		       * an array */
  int inumcointdcs; /* number of coincidence TDCs for which counts 
		     * were recorded, short in Macintosh format
                     * this value is part of the event record but 
		     * should be the same for all events, so it is 
		     * not put into an array */
  int ipixsig_adc[iMAXNUMRUNEVENTS][iMAXNUMPIX]; 
  /* signals from the camera photo multipliers (ADC counts) */
  int imuonpixsig_adc[iMAXNUMRUNEVENTS][iMAXNUMMUONPIX]; 
  /* signals from the muon shield photo multipliers (ADC counts) */
  int itdcsig_cnts[iMAXNUMRUNEVENTS][iMAXNUMCOINTDCS]; 
  /* counts from the coincidence TDCs if itdccountslength > 0 */
};

struct TELLOGDATA {
  int irunnum; /* The runnumber which we are interested in. 
		* Only data for this runnumber are written into 
		* this structure and then copied to the calnt NTuple.
		* 
		* This runnumber is generally the closest match 
		* inside the TelLog file. */
  float pixel_timeaveraged_current[127];
  float pixel_timeaveraged_scaler[127];
};

/************************************************
 * structures in output file                    *
 * (Konopelko files format)                     *
 * (from structures.h file 4.4)                 *
 ************************************************/
struct mcbankheader { /* Once in every file */
  int iparticle_type;  
  /* Primary particle type: 0 = gamma; +-1= e+-; 
   * +-2 = mu+-; +-3 = p+-, n; +-4 = pi+- */
  int inum_cts;
  /* Number of simulated telescopes */
  int inum_nfl_shifts; 
  /* Number of NFL shifts for hadrons (per event) */
  int itrigger_thresh_phot; 
  /* Only images with two pixels above this threshold are 
   * in the MC data file */
  int imin_signal;
  /* "Tail cut" = smallest signal saved */
  int inum_sim_showers; 
  /* Total number of showers simulated for this file */
  float fatmothick_gcm2; 
  /* Atmosphere above the telescope [g/cm2] */
  float fcos_zenangle;
  /* Cosine of the zenith angle */
  float fnfl_shift_radius_deg; 
  /* Maximum angular radius within which a shifted 
   * hadron shower is sampled [degrees] */
  float fenergybin_bound_tev[iNUMENERGYBINS+1]; 
  /* Boundaries for the 14 energy bins [TeV] 
   * (newline after the first 8 numbers) */
  float fzenithanglebin_bound[iNUMZENANGLEBINS+1]; 
  /* Boundaries for the 11 zenith angle bins [degrees] */
  int inum_show_in_bin[iNUMENERGYBINS]; 
  /* Number of simulated (or saved) showers 
   * per energy bin (newline after the first 8 numbers) */
  float fmaxdist_impact_m[iNUMIMPACTBINS]; 
  /* Maximum distance of the impact point to the 
   * central telescope [m] for each energy bin 
   * (newline after the first 8 numbers) */
  int inumpixels_for_ct[iMAXNUMCTS]; 
  /* Number of pixels in the camera of each simulated CT */
  float fpixwidth_deg_ct[iMAXNUMCTS]; 
  /* Pixel width [degrees] for each CT */
  float fmirrsize_m2_ct[iMAXNUMCTS];  
  /* Mirror area [m^2] for each CT  */
  float fmean_nsb_phot_ct[iMAXNUMCTS]; 
  /* Mean signal caused by the NSB in each pixel for each CT 
   * [photoelectrons]. This is the simulation of the NSB, 
   * not the electronic noise */
  float fphes_per_photon_ct[iMAXNUMCTS]; 
  /* Conversion factor photoelectron per photon */
  float frelative_x_ct[iMAXNUMCTS]; 
  /* x position relative to the central CT for each CT */
  float frelative_y_ct[iMAXNUMCTS]; 
  /* y position relative to the central CT for each CT */
}; 
#endif







