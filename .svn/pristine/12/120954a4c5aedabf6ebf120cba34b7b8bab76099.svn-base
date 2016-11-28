#ifndef MARS_MTriggerDefine
#define MARS_MTriggerDefine
//
//
//      In this file are the fundamental definitions for the class MCTrigger
//
// Number of pixels in the trigger region (used by camera simulation,
// see camera.cxx.
//
#define TRIGGER_PIXELS_1      397
#define TRIGGER_PIXELS_2      397
#define TRIGGER_PIXELS_3      1657
#define TRIGGER_PIXELS_4      547  // For MGeomCamMagic1183, PRELIMINARY!
#define TRIGGER_PIXELS_5      397
#define TRIGGER_PIXELS_6      1657
#define TRIGGER_PIXELS_8      126
#define TRIGGER_PIXELS_9      126
//
//      This is the number of Pixels contributing to the TRIGGER logic
//      for each of the already implemented geometries.
//      All Pixels-Id above that value don't do an trigger stuff. 
//      Actually, not all of them are involved in TRIGGER logic, only the
//      ones of them that are also in some TRIGGER_CELL 
//   (MagicSoft/Simulation/Detector/include-MTrigger/TABLE_PIXELS_IN_CELLS)
//
#define TOTAL_TRIGGER_TIME    160 
//
//      This values defines the total range in that we try to find
//      a trigger. 
//
#define LEVEL1_DEAD_TIME    50
//
//      Dead time of the detector after one first level trigger happens.
//
#define LEVEL2_DEAD_TIME    300
//
//      Dead time of the detector after second level trigger fires
//
#define TRIG_SLICES_PER_NSEC         4
//      
//      Each nano second is divided into the number of this values slices. 
//      So you can get the total number of timeslices for one Pixel by 
//      ( TOTAL_TRIGGER_TIME * SLICES_PER_NSEC ). 
//      In the current settings this are 1000 slices
//
#define TRIGGER_TIME_SLICES     (TOTAL_TRIGGER_TIME*TRIG_SLICES_PER_NSEC) 
//
//
//
//
//       ------>>>   SETTINGS for the RESPONSE FUNCTION
// 
#define RESPONSE_SLICES_TRIG        40
//
//       This is the dimension of the array containing the standard response Signal 
//       for 1 Photoelectron, as seen at the input of the discriminators (for the 
//       trigger simulation). Each bin corresponds to 1./TRIG_SLICES_PER_NSEC nanoseconds,
//       so by default it is 0.25 ns and hence the total range is 10 ns. This should be 
//       enough for any reasonable single photoelectron response (should be much narrower 
//       than 10 ns!)
//

//       The Response function
//       These values are discussed with Eckart. We start from this point. 
//
#define RESPONSE_FWHM          2. 

#define RESPONSE_AMPLITUDE     1. 
//
//       This are the Standard values of the response function for
//       1 photo electron. ( 1 means 1 mV per photoelectron ) 
//
//
//       -------->>> SETTINGS for the DISKRIMINATORS
//
//
#define CHANNEL_THRESHOLD      2.5 
//
//       This is the diskriminator threshold for each individual channel
//       First we set the value to 2 unit of the RESPONSE_AMPLITUDE 
//
#define TRIGGER_GATE           3. 
// 
//       Here we set the width of the digital signal we get if the signal
//       passes the diskriminator
//
//
#define TRIGGER_OVERLAPING           0.25 
// 
//       Here we set the required overlaping time among pixels
//       to be in coincidence.
//
//
//      --------->>>> SETTINGS for the TRIGGER logic
//
//
#define TRIGGER_CELLS          19  
//
//       Number of trigger cells that cover the trigger zone
//
#define TRIGGER_MULTI          4.  
//
//       We get a Level Zero Trigger, if we have a least TRIGGER_MULTI
//       channels with a diskrimiator signal at the same time 
//
#define TRIGGER_GEOM           0
//
//      This defines the geometry required for a trigger. There exists 
//      different meaning for this behaviour: 
//         0 means a pixel with trigger_multi-1 neighbours
//         1 means trigger_multi neighbours
//         2 means trigger_multi closed neighbours
//
#endif
