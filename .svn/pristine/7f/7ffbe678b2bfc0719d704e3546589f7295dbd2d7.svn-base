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

#ifndef _defines_h_
#define _defines_h_ "__defines_h__"

#define cIDSTRING(x,y,z) "@(#) GAMMAS "#x" version "#y", date "#z"."
#define CAT(x,y) #x#y
#define XCAT(x,y) CAT(x,y)

#define Boolean int

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/*************** DEFINES ******************************************/

/* General defines */

#define cHELLOSTRING(x,y) "This is preproc.\nVersion "#x", date "#y"\n\n"
#define PREPROCVERSIONSTRING(x) "V"#x  

#define dDEG_PER_HOUR 15.
#define dRAD_PER_DEG 0.0174532925199 /* = 3.14159265359/180. */
#define dSECS_PER_DAY 86400.
#define SQUARE(x) ((x) * (x))

/* defines for histograms and ntuple creation */

#define HMAX 2000000 /* memory reserved for pawc common block 
		     * used by hbook */
#define DAB -999 /* if a histogramm parameter is to be Defined 
		  * At Booking enter -999 */
#define iADCHISTID 1000 /* id of the first histogram for ADC 
			 * count spectra */
#define cADCHISTTITLE "Pixel signals before calibration;pixel \
number;ADC counts"
#define iADCHIST_BINS 127 /* This is a good guess. bparametersok()
			   * will look at parameters for the bins 
			   * in X direction and adjust the other,
			   * similar histograms acoordingly. */
#define fADCHIST_LOLIMX 0.5
#define fADCHIST_HILIMX 127.5 /* For both of them, see above. */



#define iPHEHISTID 2000 /* id of the first histogram for photo 
			 *electron spectra */
#define cPHEHISTTITLE "Pixel signals after calibration;pixel \
number;photo electrons"
#define iPHEHIST_BINS 127 /* This is a good guess. bparametersok()
			   * will look at parameters for the bins 
			   * in X direction and adjust the other,
			   * similar histograms acoordingly. */
#define fPHEHIST_LOLIMX 0.5
#define fPHEHIST_HILIMX 127.5 /* For both of them, see above. */

#define iPEDONHISTID 12000 /* id of the first histogram for the 
			    *signals taken in pedestalruns for ON */
#define iPEDOFHISTID 11000 /* id of the first histogram for the 
			    *signals taken in pedestalruns for OF */
#define cPEDONHISTTITLE "Pixel signals from ON pedestal runs;\
pixel number;ADC Counts"
#define cPEDOFHISTTITLE "Pixel signals from OF pedestal runs;\
pixel number;ADC Counts"
#define iPEDONMEANHISTID 14000 /* id of the first histogram for the 
				*signals taken in pedestalruns for ON */
#define iPEDOFMEANHISTID 13000 /* id of the first histogram for the 
				*signals taken in pedestalruns for OF */
#define cPEDONMEANHISTTITLE "Pedestal Means from ON pedestal runs;\
Event Index;ADC Counts"
#define cPEDOFMEANHISTTITLE "Pedestal Means from OF pedestal runs;\
Event Index;ADC Counts"
#define iPEDONJITTHISTID 16000 /* id of the first histogram for the 
				*signals taken in pedestalruns for ON */
#define iPEDOFJITTHISTID 15000 /* id of the first histogram for the 
				*signals taken in pedestalruns for OF */
#define cPEDONJITTHISTTITLE "Deviation from Pedestal Means from \
ON pedestal runs;Pixel Number;ADC Counts"
#define cPEDOFJITTHISTTITLE "Deviation from Pedestal Means from \
OF pedestal runs;Pixel Number;ADC Counts"
#define iCALONHISTID 18000 /* id of the first histogram for the 
			    *signals taken in laserruns for ON */
#define iCALOFHISTID 17000 /* id of the first histogram for the 
			    *signals taken in plaserruns for OF */
#define cCALONHISTTITLE "Pixel signals from ON laser runs;\
pixel number;ADC Counts"
#define cCALOFHISTTITLE "Pixel signals from OF laser runs;\
Pixel Number;ADC Counts"
#define iCALONMEANHISTID 20000 /* id of the first histogram for the 
				*signals taken in laserruns for ON */
#define iCALOFMEANHISTID 19000 /* id of the first histogram for the 
			    *signals taken in plaserruns for OF */
#define cCALONMEANHISTTITLE "Laser Means from ON laser runs;\
Event Index;ADC Counts"
#define cCALOFMEANHISTTITLE "Laser Means from OF laser runs;\
Event Index;ADC Counts"
#define iCALONJITTHISTID 22000 /* id of the first histogram for the 
				*signals taken in laserruns for ON */
#define iCALOFJITTHISTID 21000 /* id of the first histogram for the 
			    *signals taken in plaserruns for OF */
#define cCALONJITTHISTTITLE "Deviation from Laser Means from \
ON laser runs;Pixel Number;ADC counts"
#define cCALOFJITTHISTTITLE "Deviation from Laser Means from \
OF laser runs;Pixel Number;ADC counts"
#define iZENONHISTID 200 /* id of the histogram for the zenith 
			  * angle distribution before filter ON */
#define iZENOFHISTID 100 /* id of the histogram for the zenith 
			  * angle distribution before filter OF */
#define cZENONHISTTITLE "zenith angle distribution before filter \
ON;zenith angle;number of events ON"
#define cZENOFHISTTITLE "zenith angle distribution before filter \
OFF;zenith angle;number of events OFF"
#define iZENHISTNUMBINS 90
#define fZENHISTLOLIM 0.
#define fZENHISTHILIM 90.
/* defines for noise pickup correction of pedestal (added D.K.)
 */
#define iCORRPEDHISTID 400 /* id of histogram for pedestal correction values
			      (noise pickup) */
#define cCORRPEDHISTTIT "Pickup Correction;ADC counts;Nr. of Events"
#define iCPEDFWHMHISTID 410 /* id of the histogram for FWHM of pedestal
			       correction values (noise pickup) */
#define cCPEDFWHMHISTTIT "FWHM of Pickup Correction;ADC counts;Nr. of Events"
#define fMIN_CORRPEDHIST -20.
#define fMAX_CORRPEDHIST 20.
#define iCORRPEDHIST_BINS 80
#define fMIN_CPEDFWHMHIST 0.
#define fMAX_CPEDFWHMHIST 20
#define iCPEDFWHMHIST_BINS 40
#define iBIAS 50             /* is used to make the pedestals peak at 'iBIAS' */
#define dPEDCORRSIZELIMIT 500. /* maximum event SIZE for which noise-pickup is
				  corrected */
/* end: defines for noise pickup correction of pedestal
 */

/* MKA May 8 2000: Version > 0.4 */
#define cHISTOFILENAME ".hbook" 
#define cCALNTFILENAME ".nt.hbook"
#define cOUTDIRNAME "preproc_out/"
#define cPROJECTNAME "check"
#define iCALIBNTUPLE_ID  3
#define iRECL 1024 /* the record length of the calibration ntuple */ 

#define iPARTNERSEARCHDEPTH 20 /* Default search depth if not specified. */
/* various parameters */

#ifdef ALPHA
#define iMAXNUMLASERRUNS 4 /* maximum number of laser runs 
			    * recorded in one file 
			    * (90 degrees * 4 minutes/degree / 20 minutes) */
#endif
#ifdef LINUX
#define iMAXNUMLASERRUNS 6 /* maximum number of laser runs 
			    * recorded in one file 
			    * (90 degrees * 4 minutes/degree / 20 minutes) */
#endif
#define iMAXNUMLASEREVENTS 250 /* maximum number of laser shots
				* recorded for one laser run */
#ifdef ALPHA
#define iMAXNUMPEDRUNS 4 /* maximum number of pedestal runs 
			   * recorded in one file */
#endif
#ifdef LINUX
#define iMAXNUMPEDRUNS 26 /* maximum number of pedestal runs 
			   * recorded in one file */
#endif
#define iMAXNUMPEDEVENTS 500 /* maximum number of random triggers 
			      * recorded for one pedestal run */
#define iMAXNUMRUNEVENTS 40000 /* maximum number of events recorded 
				* in one run data file */
#define iNUMRMSFITXBINS 100 /* Bins in the (vector)histogram for the 
			     * fit of the Pedestal RMS */
#define iRMSFITXSTRETCH 50 /* strectch of the (vector)histogram for the 
			     * fit of the Pedestal RMS */
/* Attention: for iNUMRMSFITXBINS and iRMSFITXSTRETCH the following
 * requirement must be fulfilled:
 * A width of 1 must be achieveble as an integer multiple of the
 * binwidth, i.e. 5 or 6 times (iRMSFITXSTRETCH/iNUMRMSFITXBINS) must
 * make up 1. */

#define iRMSFITXOFFSET 2 /* half a bin offset to the left: Start a bin 
			 * with center at 0 at 1./2. binwidth earlier */

#define iMAXFILENAMELENGTH 400
/* general limit on the length of filename strings (including path) */ 
#define iMAXNUMDIRENTRIES 1028 /* MKA: Nonsens to limit this; we 
				* rewrite this in the future. 
				* Make a chained list and allocate
				* memory for it dynamically. Free
				* this memory afterwards. 
				* Put on to-do-list Nov 6 1999 */
/* the maximum number of filenames expected in a directory to 
 * be read by ireaddir() */
#define cDIRFILENAME "preproc.dir" 
/* intermediate file used by ireaddir() */
#define iMAXNUMCALFILEOFFSET 400 /* That is the number of Runs, which 
				  * is searched back to find a TelCal 
				  * file for analysis. */

#define iMAXNUMPEDFILEOFFSET 400 /* That is the number of Runs, which 
				  * is searched back to find a TelPed 
				  * file for analysis. */ 

#define iMAXNUMTELLOGFILEOFFSET 400 /* That is the number of Runs, which 
				     * is searched back to find a TelLog 
				     * file for analysis. */

/* keywords, numbers of arguments, values and ranges of values 
   which define the syntax in the parameters file
   
   in order to introduce a new input parameter, you have to 
   a) increase iNUMKEYWORDS
   b) #define the keyword (cKEYWORDXX), the format for the 
      arguments (cKEYWORDXXARG)
      the number of arguments (iKEYWORDXXNUMARG), 
      the minimum and maximum value 
      (iMIN..., iMAX...) for numerical arguments
   c) for enum-type arguments look at keywords 10 or 11 to see 
      what to #define
      (iKEYWORDXXARGYNUMVAL = the number of values the arguments 
      no. Y of keyword no. XX can take, 
      cKEYWORDXXARGYVALS  = enumeration of the different values 
      (strings) the argument can have
   d) put entries into the initialization of the arrays ckeyword, 
      ckeywordarg, ikeywordnumarg, fargvalmin, fargvalmax, and 
      for enum-type arguments declare a new array keywordXXvalues 
      as for keywords 10 and 11
   e) include the new parameter into the structure inputpars, 
      and, if you wish to, in outputpars also.
   f) add the corresponding case to the switch(k) statement in main()
   g) modify bparametersok(), bprintinputpars() and 
      vsetinputparsunfilled()
   h) program the transfer of the parameter from inputpars to 
      outputpars if necessary
      .... to be continued  */


#define iNUMKEYWORDS 46  /* the number of different kewords 
			  * defined for use in the
                          * parameters file */

#define cKEYWORD01                "iTELESCOPE"
/* keyword no. 1, this keyword must have this number, don't change it */
#define cKEYWORD01ARG             "%d"
#define iKEYWORD01NUMARG 1 /* number of arguments */
#define iMINTELESCOPEID 1 /* minimum value */
#define iMAXTELESCOPEID 2 /* maximum value */

#define cKEYWORD02                "iRUNNUM"
/* keyword no. 2, this keyword must have this number, don't change it */
#define cKEYWORD02ARG             "%d"
#define iKEYWORD02NUMARG 1
#define iMINRUNNUM 0 /* minimum value */
#define iMAXRUNNUM 99999 /* maximum value */

#define cKEYWORD03                "inumpixels"
/* keyword no. 3 */
#define cKEYWORD03ARG             "%d"
#define iKEYWORD03NUMARG 1
#define iMINNUMPIX 37 /* minimum value for the number of camera 
		       * pixels (excl. muon shield) */
/* #define iMAXNUMPIX ..  maximum value defined in structures.h */

#define cKEYWORD04                "fpixdiameter_deg"
/* keyword no. 4 */
#define cKEYWORD04ARG             "%f"
#define iKEYWORD04NUMARG 1
#define fMINPIXDIA_DEG 0.1 /* minimum value */
#define fMAXPIXDIA_DEG 1.0 /* maximum value */

#define cKEYWORD05                "ise1zeropos"
/* keyword no. 5 */
#define cKEYWORD05ARG             "%d"
#define iKEYWORD05NUMARG 1
#define iMINSE1POS 0 /* minimum value */
#define iMAXSE1POS 50000 /* maximum value */

#define cKEYWORD06                "ise2zeropos"
/* keyword no. 6 */
#define cKEYWORD06ARG             "%d"
#define iKEYWORD06NUMARG 1
#define iMINSE2POS 0 /* minimum value */
#define iMAXSE2POS 50000 /* maximum value */

#define cKEYWORD07               "idontusepix"
/* keyword no. 7 */
#define cKEYWORD07ARG             "%d"
#define iKEYWORD07NUMARG 1 
#define iMINPIXNUM 1 /* in the parameters file counting of the 
		      * pixels starts at 1 */
/* maximum value of this parameter is iMAXNUMPIX */
/* negative parameters in the range -iMINPIXNUM to -iMAXNUMPIX 
 * are also allowed: */
/* they switch the pixel back on */

#define cKEYWORD08               "isum_thresh_phot"
/* keyword no. 8 */
#define cKEYWORD08ARG             "%d"
#define iKEYWORD08NUMARG 1
#define iMINSUMTHRESH_PHOT 0 /* minimum value */
#define iMAXSUMTHRESH_PHOT 65536 /* maximum value */

#define cKEYWORD09              "i2out_thresh_phot"
/* keyword no. 9 */
#define cKEYWORD09ARG             "%d"
#define iKEYWORD09NUMARG 1
#define iMIN2OUTTHRESH_PHOT 0 /* minimum value */
#define iMAX2OUTTHRESH_PHOT 65536 /* maximum value */

#define cKEYWORD10               "usecalfile"
/* keyword no. 10 */
#define cKEYWORD10ARG             "%s %d %d"
#define iKEYWORD10NUMARG 3 /* first arg: search direction, 
			    * second arg: end of search or
			    * search depth in case of 
			    * pingpong method */
#define iKEYWORD10ARG1NUMVAL 5
#define cKEYWORD10ARG1VALS "match", "before", "after", "pingpong", "none"
/* minimum and maximum value of the second argument are 
 * identical with iMINRUNNUM and iMAXRUNNUM */

#define cKEYWORD11               "usepedfile"
/* keyword no. 11 */
#define cKEYWORD11ARG             "%s %d %d"
#define iKEYWORD11NUMARG 3 /* first arg: search direction,
			    * second arg: end of search or
			    * search depth in case of 
			    * pingpong method */
#define iKEYWORD11ARG1NUMVAL 5
#define cKEYWORD11ARG1VALS "match", "before", "after", "pingpong", "none"
/* minimum and maximum value of the second argument are 
 * identical with iMINRUNNUM and iMAXRUNNUM */

#define cKEYWORD12                "inummuonpixels"
/* keyword no. 12 */
#define cKEYWORD12ARG             "%d"
#define iKEYWORD12NUMARG 1
#define iMINNUMMUONPIX 0 /* minimum value */
/* #define iMAXNUMMUONPIX ..  maximum value defined in structures.h */

#define cKEYWORD13                "inumcointdcs"
/* keyword no. 13 */
#define cKEYWORD13ARG             "%d"
#define iKEYWORD13NUMARG 1
#define iMINNUMCOINTDCS 0 /* minimum value */
/* #define iMAXNUMCOINTDCS ..  maximum value defined in structures.h */

#define cKEYWORD14                "fphotoel_per_adccnt"
/* keyword no. 14 */
#define cKEYWORD14ARG             "%s %f"
#define iKEYWORD14NUMARG 2
#define fMINPHOTOEL_PER_ADCCNT 0.1 /* minimum value for the 
				    * second argument */
#define fMAXPHOTOEL_PER_ADCCNT 999.0 /* maximum value for the 
				      * second argument */
/* as first argument either a single integer number or a 
 * range may be given (format: e.g. 10-20) minimum and 
 * maximum value of the first argument are identical with 
 * iMINPIXNUM and the number of pixels in the camera */

#define cKEYWORD15               "bmuon_suppression"       
/* keyword no. 15 */
#define cKEYWORD15ARG             "%s"
#define iKEYWORD15NUMARG 1 /* this keyword is a switch */
#define iKEYWORD15ARG1NUMVAL 2
#define cKEYWORD15ARG1VALS "off", "on" /* the order of this 
					* enumeration is essential 
					* to make off equivalent 
					* to FALSE */

#define cKEYWORD16                "ftolerated_pointerror_deg"
/* keyword no. 16 */
#define cKEYWORD16ARG             "%f"
#define iKEYWORD16NUMARG 1
#define fMINTOLERATED_POINTERROR_DEG 0. /* minimum value for 
					 * the maximum tolerated 
					 * pointing error in the 
					 * position check in 
					 * iprocessdata */
#define fMAXTOLERATED_POINTERROR_DEG 180. /* maximum value */

#define cKEYWORD17                "iratecalc_numevents_odd"
/* keyword no. 17 */
#define cKEYWORD17ARG             "%d"
#define iKEYWORD17NUMARG 1
#define iMINRATECALC_NUMEVENTS_ODD 3 /* minimum value for the 
				      * number of events used 
				      * in the event rate 
				      * calculation */
#define iMAXRATECALC_NUMEVENTS_ODD 999 /* maximum value */

#define cKEYWORD18                "fdegrees_per_step1"
/* keyword no. 18 */
#define cKEYWORD18ARG             "%f"
#define iKEYWORD18NUMARG 1
#define fMINDEGREES_PER_STEP 0.0001 /* minimum value for the 
				     * resolution of shaft encoder 1 */
#define fMAXDEGREES_PER_STEP 0.1 /* maximum value */

#define cKEYWORD19                "fdegrees_per_step2"
/* keyword no. 19 */
#define cKEYWORD19ARG             "%f"
#define iKEYWORD19NUMARG 1
/* minimum and maximum values as for keyword 18 */

#define cKEYWORD20               "se1_is"
/* keyword no. 20 */
#define cKEYWORD20ARG             "%s"
#define iKEYWORD20NUMARG 1 
#define iKEYWORD20ARG1NUMVAL 4
#define cKEYWORD20ARG1VALS "RA", "DEC", "ALT", "AZ"

#define cKEYWORD21                "flongitude_deg"
/* keyword no. 21 */
#define cKEYWORD21ARG             "%f"
#define iKEYWORD21NUMARG 1
#define fMIN_LONG_DEG 0. /* minimum value for the longitude 
			  * of the telescope position */
#define fMAX_LONG_DEG 359.99999 /* maximum value (the 
				 * longitude is counted 
				 * positive towards West */

#define cKEYWORD22                "flatitude_deg"
/* keyword no. 22 */
#define cKEYWORD22ARG             "%f"
#define iKEYWORD22NUMARG 1
#define fMIN_LAT_DEG  -90.0 /* minimum value for the latitude 
			     * of the telescope position */
#define fMAX_LAT_DEG +90.0 /* maximum value */

#define cKEYWORD23                "irubminusutc_usecs"
/* keyword no. 23 */
#define cKEYWORD23ARG             "%d"
#define iKEYWORD23NUMARG 1
#define iMINRUBMINUSUTC_USECS -60E6 /* minimum value for the 
				     * time difference between 
				     * rubidium clock and UTC*/
#define iMAXRUBMINUSUTC_USECS 60e6 /* maximum value */

#define cKEYWORD24                "iaz_rev_track_corr"
/* keyword no. 24 */
#define cKEYWORD24ARG             "%d"
#define iKEYWORD24NUMARG 1
#define iMINAZ_REV_TRACK_CORR -1000 /* minimum value for the 
				     * correction of the az 
				     * shaft encoder values 
				     * in reverse tracking mode
				     * see Calc_corr in TIJARAFE */
#define iMAXAZ_REV_TRACK_CORR 1000 /* maximum value */

#define cKEYWORD25                "ialt_rev_track_corr"
/* keyword no. 25 */
#define cKEYWORD25ARG             "%d"
#define iKEYWORD25NUMARG 1
#define iMINALT_REV_TRACK_CORR -1000 /* minimum value for the 
				      * correction of the alt 
				      * shaft encoder values 
				      * in reverse tracking mode
				      * see Calc_corr in TIJARAFE */
#define iMAXALT_REV_TRACK_CORR 1000 /* maximum value */

#define cKEYWORD26                "fbendingcorr"
/* keyword no. 26 */
#define cKEYWORD26ARG             "%f"
#define iKEYWORD26NUMARG 1
#define fMINBENDINGCORR  -0.5 /* minimum value for the bending 
			       * correction factor for the alt 
			       * shaft encoder values
			       * see Calc_corr in TIJARAFE */
#define fMAXBENDINGCORR 0.5 /* maximum value */

#define cKEYWORD27                "fextinction"
/* keyword no. 27 */
#define cKEYWORD27ARG             "%f"
#define iKEYWORD27NUMARG 1
#define fMINEXTINCTION  0. /* minimum value for the 
			    * atmospheric extinction */
#define fMAXEXTINCTION  1. /* maximum value */

#define cKEYWORD28                "dsource_ra_hours"
/* keyword no. 28 */
#define cKEYWORD28ARG             "%lf"
#define iKEYWORD28NUMARG 1
#define fMINRA_HOURS 0.0   /* minimum value for the right 
			    * ascension of the assumed source */
#define fMAXRA_HOURS 23.99999999 /* maximum value */

#define cKEYWORD29                "dsource_dec_deg"
/* keyword no. 29 */
#define cKEYWORD29ARG             "%lf"
#define iKEYWORD29NUMARG 1
#define fMINDEC_DEG -90.0 /* minimum value for the 
			   * declination of the assumed source */
#define fMAXDEC_DEG 90.0        /* maximum value */

#define cKEYWORD30                "imuoncut_thresh_adccnt"
/* keyword no. 30 */
#define cKEYWORD30ARG             "%s %d"
#define iKEYWORD30NUMARG 2
#define iMINMUONCUT_THRESH_ADCCNT 0 /* minimum value for the 
				     * second argument */
#define iMAXMUONCUT_THRESH_ADCCNT 65536 /* maximum value for 
					 * the second argument */
/* as first argument either a single integer number or a 
 * range may be given (format: e.g. 3-5) minimum and maximum 
 * value of the first argument are the number of pixels in 
 * the camera +1 and the number of pixels in the camera + 
 * the number of muonpixels in the camera */

#define cKEYWORD31                "fxpointcorr_deg"
/* keyword no. 31 */
#define cKEYWORD31ARG             "%f"
#define iKEYWORD31NUMARG 1
#define fMINRAPOINTERROR_DEG -10. /* minimum value for the 
				   * pointing error correction 
				   * along camera x axis as known 
				   * from e.g. a pointrun */
#define fMAXRAPOINTERROR_DEG +10. /* maximum value */

#define cKEYWORD32                "fypointcorr_deg"
/* keyword no. 32 */
#define cKEYWORD32ARG             "%f"
#define iKEYWORD32NUMARG 1
#define fMINDECPOINTERROR_DEG -10. /* minimum value for the 
				    * pointing error correction 
				    * along camera y axis as known 
				    * from e.g. a pointrun */
#define fMAXDECPOINTERROR_DEG +10. /* maximum value */

#define cKEYWORD33                "fcamera_align_angle_deg"
/* keyword no. 33 */
#define cKEYWORD33ARG             "%f"
#define iKEYWORD33NUMARG 1
#define fMINCAMALIGNANGLE_DEG -180. /* minimum value for the 
				     * angle between the camera 
				     * y-axis and the meridian 
				     * when a culminating object
				     * is observed (defined 
				     * counter-clockwise looking 
				     * at the sky) */
#define fMAXCAMALIGNANGLE_DEG 180. /* maximum value */

#define cKEYWORD34               "bhistograms"
/* keyword no. 34 */
#define cKEYWORD34ARG             "%s"
#define iKEYWORD34NUMARG 1 /* this keyword is a threefold switch */
#define iKEYWORD34ARG1NUMVAL 3
#define cKEYWORD34ARG1VALS "off", "on", "new" /* the order of this 
					       * enumeration is 
					       * essential to make 
					       * off equivalent to FALSE */

#define cKEYWORD35                "iadc_histopars"
/* keyword no. 35 */
#define cKEYWORD35ARG             "%d %f %f"
#define iKEYWORD35NUMARG 3
#define iMINBINS 3
#define iMAXBINS 300
#define fMIN_ADCHIST_LOLIM -50.5
#define fMAX_ADCHIST_LOLIM 1000.
#define fMIN_ADCHIST_HILIM 0.5
#define fMAX_ADCHIST_HILIM 65535.5

#define cKEYWORD36                "iphe_histopars"
/* keyword no. 36 */
#define cKEYWORD36ARG             "%d %f %f"
#define iKEYWORD36NUMARG 3
#define fMIN_PHEHIST_LOLIM -10000.
#define fMAX_PHEHIST_LOLIM 1000.
#define fMIN_PHEHIST_HILIM -1000.
#define fMAX_PHEHIST_HILIM 10000.

#define cKEYWORD37                "idontusepix_in_trig"
/* keyword no. 37 */
#define cKEYWORD37ARG             "%s"
#define iKEYWORD37NUMARG 1    /* in the parameters file 
			       * counting of the pixels starts at 1
                              * minimum value is iMINPIXNUM 
                              * maximum value of this parameter 
			      * is iMAXNUMPIX the pixel can only be 
			      * taken back into trigger by resetting all
                              * parameters */

#define cKEYWORD38                "fpadlevel_phot_pix"
/* keyword no. 38 */
#define cKEYWORD38ARG             "%s %f"
#define iKEYWORD38NUMARG 2
#define fMINPADLEVEL_PHOT  0. /* minimum value for the software 
			       * padding level for each pixel */
#define fMAXPADLEVEL_PHOT  100. /* maximum value */

#define cKEYWORD39                "frandomseeds"
/* keyword no. 39 */
#define cKEYWORD39ARG             "%f %f "
#define iKEYWORD39NUMARG 2
#define fMINSEED 1. /* minimum value for the maximum tolerated 
		     * pointing error in the position check in 
		     * iprocessmcdata */
#define fMAXSEED 1.e10 /* maximum value */

#define cKEYWORD40                "iswap_pixels"
/* keyword no. 40 */
#define cKEYWORD40ARG             "%d %d " /* the number of the 
					    * two pixels for which 
					    * the numbering should 
					    * be swapped (in 
					    * ireadrunfile, 
					    * -pedfile, -calfile) */
#define iKEYWORD40NUMARG 2
/* the minimum value for this argument is iMINPIXNUM, the maximum 
 * is iMAXNUMPIX */

#define cKEYWORD41                "%"
/* keyword no. 41 */
#define cKEYWORD41ARG             "\n" /* using this keyword, 
					* comments can be added 
					* to the parameter file */
#define iKEYWORD41NUMARG 0

#define cKEYWORD42           "errortolerance"
/* keyword no. 42 */
#define cKEYWORD42ARG        "%s"
#define iKEYWORD42NUMARG 1
#define iKEYWORD42ARG1NUMVAL 3
#define cKEYWORD42ARG1VALS "cautious", "goodphysics", "tank"
/* cautious: exits on any reason 
 * normal: the usual way: exit only, when the physics would 
 *         be affected
 * tank: exits only when all runs are processed. Otherwise
 *       it goes through everything, even through the pain of 
 *       a coredump. */

#define cKEYWORD43           "montecarlo"
/* keyword no. 43 */
#define cKEYWORD43ARG        "%s"
#define iKEYWORD43NUMARG 1
#define iKEYWORD43ARG1NUMVAL 3
#define cKEYWORD43ARG1VALS "all", "flag", "noflag"
/* Whenever this one is specified, there shell apply certain 
 * limitations: No pedestal and calibration files shall be read in. 
 * Maybe more.... 
 * Arguments:
 * all: all events which survive the filter are written to the 
 *      events NTuple. 
 * flag: When Dorota's triggerflag is set to 1 for a particular
 *       event, it shall be written to the output. All others shall
 *       just be disregarded.
 * noflag: Opposite of 'flag': only events with triggerflag = 0 shall
 *         be treated further. */

#define cKEYWORD44           "mcfile"
/* keyword no. 44 */
#define cKEYWORD44ARG        "%s"
#define iKEYWORD44NUMARG 1

#define cKEYWORD45           "fmean_nsb_pe_pix"
/* keyword no. 45 */
#define cKEYWORD45ARG             "%s %f"
#define iKEYWORD45NUMARG 2
#define fMIN_NSB_PE_PIX  0.
#define fMAX_NSB_PE_PIX  2000000000. /* some ridiculous number...*/

#define cKEYWORD46           "fitmypedrms"
/* keyword no. 46 */
/* Setting this keyword in the parameterfile shall trigger the 
 * pedestal MEAN and SIGMA=sqrt(MEAN) to be fitted from pickup
 * corrected pedestal data and not calculated from the pickup 
 * corrected ADC values, as it was done up to ~November 2000. */

/* Macintosh variable formats */
#define lMACSIZEOFSHORT  2L
#define lMACSIZEOFLONG   4L
#define lMACSIZEOFDOUBLE 8L

/* Time Offsets */
#define iMACTIMEOFFSET 2082844800 /* offset to be subtracted 
				   * from the Macintosh time 
				   * read from the various data 
				   * files to get DECSTATION/ALPHA 
				   * time */

/* TIJARAFE filename conventions */

#define cCALFILENAME_TEMPLATE "Te%1sCal_*%s_%2s_*"
#define cLOGFILENAME_TEMPLATE "Te%1sLog_*%s_%2s_*"
#define cPEDFILENAME_TEMPLATE "Te%1sPed_*%s_%2s_???????_*"
#define cRUNFILENAME_TEMPLATE "Te%1sRun_*%s_%2s_*"
#define cPOIFILENAME_TEMPLATE "Te%1sPoi_*%s_%2s_*"
#define cCTIDPOSITION_TIJ "2" /* element of the filename character 
			   * array which contains the telescope id
			   * a length of 1 character is assumed */
#define cRUNNUMPOSITION_TIJ "7" /* element of the filename character 
			     * array which contains the run number */
#define cRUNNUMLENGTH_TIJ "4" /* number of characters in the run number */
#define cRUNTYPEPOSITION_TIJ "12" /* element of the filename character 
			       * array which contains the ON or OF 
			       * string a length of 2 characters is 
			       * assumed */
#define iTIJ_DUMMYLENGTH 20

/* ROBERTO filename conventions */

#define cCTIDPOSITION_ROB "2" /* element of the filename character 
			       * array which contains the telescope id
			       * a length of 1 character is assumed */
#define cRUNNUMPOSITION_ROB "7" /* element of the filename character 
				 * array which contains the run number */
#define cRUNNUMLENGTH_ROB "5" /* number of characters in the run number */
#define cRUNTYPEPOSITION_ROB "13" /* element of the filename character 
				   * array which contains the ON or OF 
				   * string a length of 2 characters is 
				   * assumed */
#define iROB_DUMMYLENGTH 26

/* Razmick's Method's F Factor for the PMTs: */
/* #define FIXED_F_VALUE 1.09 */
/* #define FIXED_F_VALUE 1.15 */
#define FIXED_F_VALUE 1.075

/* Monte Carlo defines, that _MUST_ be exactly the same as in
 * zorp's defines.h. */
#define iMAXNUMCTS 1
#define iNUMENERGYBINS 19
#define iNUMIMPACTBINS 60
#define iNUMZENANGLEBINS 24
#define iMAXNUMNFLSHIFTS 1
#define fMAXIMPACTPAR 600.

/* exit codes */

#define iERRORINPROCESSDATA 100
#define iERRORINMAKEFILENAME 101
#define iERRORCT2TRACKMODE 102
#define iERRORGAINZERO 103

/*************** GLOBAL VARIABLES *********************************/

#define FILEHANDLING_VERSION 4.2

#endif

#ifdef _defines_h_

extern char G_cfilehandlingident[];
extern char G_filehandhfilehident[];

extern char G_cfatalerrortext[22];
extern Boolean G_bhbook_is_init;
extern float G_fpreprocversion;
extern char G_preprocident[];
extern char G_preprocstrucident[];
extern char G_preprocfilehident[];

extern char *calnt_var_list[22];
extern char cprojectname[40];

extern HBOOK_FILE calnt;
extern HBOOK_FILE calhb;
extern int G_oferfloweventID;
extern int G_oferfloweventoffset;
extern float pixcoords[271][2]; /* For the 2-dim camera pictures of the 
				 * overflow events */
#endif
