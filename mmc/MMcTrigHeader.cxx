#include "MMcTrigHeader.hxx"

#include <iostream>


//==========
// MMcTrigHeader
//    
// This class contains the MonteCarlo information
// of the trigger simulation for the current run.
// The information is saved only once, whatever the
// number of events is
//
// This is the second version of this output class. Old root files, which have
// a previous version of this class, are still compatibles and can be used.
// But of course, you can no try to get infromatino in these old files about
// the new data members.
//
// The following data member have been introduced in this second version 
// and they do not exist in the previous one:
//
//   Float_t  fElecNoiseTrig;  The width of the gaussian noise is that times
//                             the amplitude of the single phe response 
//                             for the trigger 
//
// Version 4
//   Added data members fGainFluctuations and fNoiseGainFluctuations
//
//
/////////////////////////

ClassImp(MMcTrigHeader);

using namespace std;


MMcTrigHeader::MMcTrigHeader() {
  //
  //  default constructor
  //  set all values to zero

  Int_t i;

  fTopology     = -1 ;
  fMultiplicity = -1 ;
  for(i=0;i<CAMERA_PIXELS;i++){
    fThreshold[i] = -1.0;
  }
  
  fTrigPattern[0]=0;
  fTrigPattern[1]=0;

  fTrigShape=0.0; 
  fAmplTrig=RESPONSE_AMPLITUDE;
  fFwhmTrig=RESPONSE_FWHM;
  fOverlapingTime=TRIGGER_OVERLAPING;
  fGateLeng=TRIGGER_GATE ;
}

void MMcTrigHeader::Print(Option_t *opt) const {
  //
  //  print out the data member on screen
  //
  cout << endl;
  cout << "Monte Carlo Trigger output:" << endl;
  cout << " XSTopology Trigger in this run: "   << fTopology << endl;
  cout << " Multiplicity Trigger in this run: " << fMultiplicity << endl;
  cout << " Trigger Pattern in this run: ";
  cout << fTrigPattern[0] << ", " << fTrigPattern[1] << endl;
  cout << endl; 
}
