#include "MMcTrig.hxx"

#include <iostream>


//==========
// MMcTrig
//    
// This class handles and contains the MonteCarlo information
// with which the events have been generated
// This information exists for each event.
//
// This the second version of this output class. Old root files, which have
// a previous version of this class, are still compatibles and can be used.
// But of course, you can no try to get infromatino in these old files about
// the new data members.
//
// The fPixelsFirst array has been increased from 273 (which was the trigger 
// zone) to 577 (which is the full Camera)
//
////////////////////////////////////////////////////////////// 


ClassImp(MMcTrig);

using namespace std;


MMcTrig::MMcTrig(const char *name, const char *title) {
  //
  //  default constructor
  //  set all values to zero

  
  fName  = name  ? name  : "MMcTrig";
  fTitle = title ? title : "Trigger info from Monte Carlo";

    
  Int_t i,j;

  fNumFirstLevel  = 0 ;

  for(i=0;i<TOTAL_TRIGGER_TIME/LEVEL1_DEAD_TIME+1;i++){
    fTimeFirst[i]    = -99.9;
    for(j=0;j<CAMERA_PIXELS/8+1;j++){
      fPixelsFirst[j][i]   = 0;
    }
  }

  fNumSecondLevel = 0 ; 
  for(i=0;i<TOTAL_TRIGGER_TIME/LEVEL2_DEAD_TIME+1;i++){
    fFirstToSecond[i]=0;
  }
}

MMcTrig::~MMcTrig() {
  //
  //  default destructor
  //
}

void MMcTrig::SetTime(Float_t t, Int_t i)
{
    if (i>TOTAL_TRIGGER_TIME/LEVEL1_DEAD_TIME+1  || i<1)
    {
        cout << "fNumFirstLevel out of range. Time will be -99" << endl;
        return;
    }

    fTimeFirst[i-1]=t;
}

void MMcTrig::Clear(Option_t *opt) {
  //
  //  
  //  reset all values to zero
  Int_t i,j;

  fNumFirstLevel  = 0 ;
  for(i=0;i<TOTAL_TRIGGER_TIME/LEVEL1_DEAD_TIME+1;i++){
    fTimeFirst[i]    = -99.9;
    for(j=0;j<CAMERA_PIXELS/8+1;j++){
      fPixelsFirst[j][i]   = 0;
    }
  }

  fNumSecondLevel = 0 ; 
  for(i=0;i<TOTAL_TRIGGER_TIME/LEVEL2_DEAD_TIME+1;i++){
    fFirstToSecond[i]=0;
  }

  //  cout << "MMcTrig::Clear() " << endl ; 
}

void MMcTrig::Print(Option_t *option) const
{
    //
    //  print out the data member on screen
    //
    cout << "Monte Carlo Trigger output:" << endl;
    cout << " First Level Trigger in this Event: " << fNumFirstLevel << endl;
    if (!TString(option).Contains("short"))
    {
        cout << " Times of first Level Trigger in this Event: ";
        for (int i=0; i<fNumFirstLevel; i++)
            cout << fTimeFirst[i] << " ";
        cout << endl;

        cout << " Pixels of first  Level Trigger in this Event: ";
        for (int i=0; i<fNumFirstLevel; i++)
            for(int j=0; j<CAMERA_PIXELS/8+1; j++)
                cout << (int)fPixelsFirst[j][i] << " ";
        cout << endl;
    }

    cout << " Second Level Trigger in this Event: " << fNumSecondLevel << endl;
    cout << endl;
}

Byte_t MMcTrig::IsPixelFired(Int_t npix, Int_t nfirstlevel){
  //======================================================================
  //
  //  It returns >1 if the pixel npix was fired when the nfirstlevel
  //  first level trigger happened, 0 if not.
  //
  
  const Int_t  body=npix/8;
  const Byte_t reminder= 1<<(npix%8);

  return reminder&fPixelsFirst[body][nfirstlevel];
  /*
  Byte_t ret=0;
  Byte_t reminder;
  Int_t body;

  body=npix/8;
  reminder=(Byte_t)(pow(2,npix%8));
  ret=reminder&fPixelsFirst[body][nfirstlevel];
  return(ret);
  */
}
