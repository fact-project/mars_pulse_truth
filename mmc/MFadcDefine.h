//
//  --> Sampling frequency of the FADC, in number of slices
//  per nsec, for the original 300 MHz FADCs of MAGIC. WARNING:
//  DO NOT CHANGE this number to set a different sampling frequency
//  of the signals! USE instead the "fadc_GHz" command in the camera
//  input card (see instructions)
//
#define FADC_SLICES_PER_NSEC 0.3
//
//
//  --> The amount of FADC slice written to the raw format.
#define FADC_SLICES  15
//
//  --> Number of bins per FADC slice that we will use for the analog 
//      signal simulation. In March 2005, raised this value from 5 to 500 
//      so that we can properly simulate the time jitter of the PMTs (which is
//      by default 200 ps)
#define SUBBINS     500 
//
//
// --> Default values of the single photoelectron response going to the FADC,
//     in the case the gaussian shape is chosen.
//
#define MFADC_RESPONSE_FWHM       5.0
#define MFADC_RESPONSE_INTEGRAL   4.0

//
// -->  The maximum number of FADC channels
//      Most likely it will be always equal to CAMERA_PIXELS 
#define MFADC_CHANNELS  3800
//
//  --> Ratio of high to low gain:
#define HIGH2LOWGAIN 10.
//
//
//  --> The amount of ns before trigger that would be shown from the ADC
//      history in order to show also the start of the pulse before the
//      the trigger time.-> JUST FOR DISPLAY purposes! (see MFadc::Scan)
#define TIME_BEFORE_TRIGGER    10.  
// 
//
