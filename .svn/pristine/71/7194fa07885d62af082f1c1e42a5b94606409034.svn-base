#ifndef MARS_MRawEvtHeader
#define MARS_MRawEvtHeader

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

// gcc 3.2
//class ifstream;
#include <iosfwd>

class MTime;
class MArrayB;
class MRawRunHeader;

class MRawEvtHeader : public MParContainer
{
public:
    //
    // Trigger Type (TT)
    /*
    enum {
        kTTEvent       = 0,
        kTTPedestal    = 1,
        kTTCalibration = 2,
        kTTPinDiode    = 3
    };
    */
    enum CLColor_t 
      {
        kCLUV       = BIT(0),
        kCLGreen    = BIT(1),
        kCLAmber    = BIT(2),
        kCLRed      = BIT(3),
        kCLRedAmber   = kCLRed & kCLAmber,
        kCLRedGreen   = kCLRed & kCLGreen,
        kCLRedUV      = kCLRed & kCLUV   ,
        kCLAmberGreen = kCLAmber & kCLGreen,
        kCLAmberUV    = kCLAmber & kCLUV   ,
        kCLGreenUV    = kCLGreen & kCLUV   ,
        kCLRedAmberGreen = kCLRedAmber & kCLGreen,
        kCLRedGreenUV    = kCLRedGreen & kCLUV,
        kCLAmberGreenUV  = kCLAmberGreen & kCLUV,
        kCLAll           = kCLRedAmberGreen & kCLUV
      };
    

private:
    MTime   *fTime;            //! object to store the time in (ReadEvt)

    UInt_t   fDAQEvtNumber;    // Number of Event {fits: name=EventNum ; unit=uint32}

    UInt_t   fNumTrigLvl1;     // Number of 1st level tiggers between 2 events
                               // Used in MSimTrigger for the index of the trigger channel
    UInt_t   fNumTrigLvl2;     // Number of 2nd level tiggers between 2 events
    UInt_t   fTrigPattern[2];  // Trigger configuration

    UShort_t fNumLoGainOn;     // Indicating if no pixel has a neglegible
                               // low gain signal (0), else it is the number
                               // of pixels with lo gain on

    //
    // Informations only needed to read the raw file correctly
    //
    UShort_t fTrigType;        //! Trigger Type of this event
    MArrayB *fPixLoGainOn;     //! Array which tell you which pixels have lo gain on

    Bool_t DecodeTime(const UInt_t abstime[2], UShort_t ver) const;

    Int_t GetNumBytes() const;

    Int_t ReadEvtOld(std::istream& fin, UShort_t ver);

public:
    MRawEvtHeader(const char *name=NULL, const char *title=NULL);
    ~MRawEvtHeader();

    // MRawEvtHeader
    void InitRead(MRawRunHeader *rh, MTime *t);

    // Getter
    UShort_t GetTrigType() const     { return fTrigType; }
    UInt_t   GetNumTrigLvl1() const  { return fNumTrigLvl1; }
    UInt_t   GetNumTrigLvl2() const  { return fNumTrigLvl2; }
    UInt_t   GetDAQEvtNumber() const { return fDAQEvtNumber; }
    
    UInt_t   GetTriggerID() const;
    UInt_t   GetCalibrationPattern() const;
    UInt_t   GetPulserSlotPattern()  const;

    // Setter (ONLY for Monte Carlo purpose)
    void FillHeader(UInt_t, Float_t=0);

    void  SetTriggerPattern( const UInt_t pattern )  {  fTrigPattern[0] = pattern; }
    void  SetCalibrationPattern( const UInt_t pattern )  {  fTrigPattern[1] = pattern; }
    void  SetDAQEvtNumber(const UInt_t n) { fDAQEvtNumber = n; }
    void  SetNumTrigLvl1(UInt_t num) { fNumTrigLvl1 = num; }

    // TObject
    void Clear(Option_t * = NULL);
    void Print(Option_t * = NULL) const;

    // I/O
    Int_t ReadEvt(std::istream& fin, UShort_t ver, UInt_t size);
    void  SkipEvt(std::istream& fin, UShort_t ver);

    ClassDef(MRawEvtHeader, 1) // Parameter Conatiner for raw EVENT HEADER
}; 

#endif
