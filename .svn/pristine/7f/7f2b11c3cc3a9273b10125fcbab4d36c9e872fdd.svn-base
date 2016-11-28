#ifndef MARS_MRawRunHeader
#define MARS_MRawRunHeader
///////////////////////////////////////////////////////////////////////
//                                                                   //
// MRunHeader                                                        //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#ifndef MARS_MTime
#include "MTime.h"
#endif

class TBuffer;
class MArrayS;

class MRawRunHeader : public MParContainer
{
public:
    //
    // enum for the Run Type. Monte Carlo Runs have
    // to have a value greater than 255 (>0xff)
    //
    enum {
        kRTData        = 0x0000,
        kRTPedestal    = 0x0001,
        kRTCalibration = 0x0002,
        kRTDominoCal   = 0x0003,
        kRTLinearity   = 0x0004,
        kRTPointRun    = 0x0007,
        kRTMonteCarlo  = 0x0100,
        kRTFact        = 0xfac7,
        kRTNone        = 0xffff
    };

    //
    // Magic number to detect the magic file type
    //
    static const UShort_t kMagicNumber;
    static const Byte_t   kMaxFormatVersion;

private:
    /* ---- Run Header Informations ---- */
    UShort_t  fMagicNumber;          // File type identifier

    UInt_t    fHeaderSizeRun;        // Size of run header
    UInt_t    fHeaderSizeEvt;        // Size of evt header
    UInt_t    fHeaderSizeCrate;      // Size of crate header

    UShort_t  fFormatVersion;        // File format version
    UShort_t  fSoftVersion;          // DAQ software version
    UShort_t  fFadcType;             // FADC type (1=Siegen, 2=MUX)
    UShort_t  fCameraVersion;        // Camera Version (1=MAGIC I)
    UShort_t  fTelescopeNumber;      // Telescope number (1=Magic I)
    UShort_t  fRunType;              // Run Type
    UInt_t    fRunNumber;            // Run number
    UInt_t    fFileNumber;           // File number
    Char_t    fProjectName[101];     // Project name
    Char_t    fSourceName[81];       // Source name
    Char_t    fObservationMode[61];  // observation mode
    Char_t    fSourceEpochChar[4];   // epoch char of the source
    UShort_t  fSourceEpochDate;      // epoch date of the source
    UShort_t  fNumCrates;            // number of electronic boards
    UShort_t  fNumPixInCrate;        // number of pixels in crate
    UShort_t  fNumSamplesLoGain;     // number of logain samples stored
    UShort_t  fNumSamplesHiGain;     // number of higain samples stored
    UShort_t  fNumBytesPerSample;    // number of bytes per sample
    Bool_t    fIsSigned;
    UInt_t    fNumEvents;            // number of events stored
    UInt_t    fNumEventsRead;        // number of events read by the electronics
    UShort_t  fSamplingFrequency;    // Sampling Frequency [MHz]
    Byte_t    fFadcResolution;       // number of significant bits
    MTime     fRunStart;             // time of run start
    MTime     fRunStop;              // time of run stop
    MArrayS  *fPixAssignment;        //-> pixel assignment table

    Bool_t SwapAssignment(Short_t id0, Short_t id1);
    void   FixRunNumbers();
    Bool_t FixAssignment();
    Bool_t Fixes();
    Bool_t IsConsistent() const;

    Bool_t ReadEvtOld(std::istream& fin);

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MRawRunHeader(const char *name=NULL, const char *title=NULL);
    MRawRunHeader(const MRawRunHeader &h);
    ~MRawRunHeader();

    // This is to be used in the MC chain only!
/*
    void SetMagicNumber(UShort_t a)       { fMagicNumber=a; }
    void SetFormatVersion(UShort_t a)     { fFormatVersion=a; }
    void SetSoftVersion(UShort_t a)       { fSoftVersion=a; }
    void SetRunType(UShort_t a)           { fRunType=a; }
    void SetRunNumber(UInt_t a)           { fRunNumber=a; }
    void SetNumEvents(UInt_t a)           { fNumEvents=a; }
    void SetNumSamples(UShort_t low, UShort_t high)
    {
        fNumSamplesLoGain=low;
        fNumSamplesHiGain=high;
    }
    void SetNumCrates(UShort_t a)         { fNumCrates=a; }
    void SetNumPixInCrate(UShort_t a)     { fNumPixInCrate=a; }
*/
    void InitFadcType(UShort_t type);
    void InitCamera(UShort_t type, UShort_t pix=0);
    void InitPixels(UShort_t pix) { InitCamera((UShort_t)-1, pix); }
    void InitFact(UShort_t num, UShort_t pix, UShort_t samples, UShort_t *map=0);
    void SetRunType(UShort_t type) { fRunType=type; }
    void SetRunInfo(UShort_t tel, UInt_t run, UInt_t file=0);
    void SetFileNumber(UInt_t file) { fFileNumber=file; }
    void SetSourceInfo(const TString src, char epoch='J', UShort_t date=2000);
    void SetRunTime(const MTime &start, const MTime &end);
    void SetRunTimeMjd(Float_t start, Float_t stop) { fRunStart.SetMjd(start); fRunStop.SetMjd(stop); }
    void SetObservation(const TString mode, const TString proj);
    void SetNumEvents(UInt_t num);
    void SetValidMagicNumber() { fMagicNumber=kMagicNumber; }
    void SetFormat(UShort_t file, UShort_t soft) { fFormatVersion=file; fSoftVersion=soft; }

    // This is to get the numbers...
    UShort_t GetMagicNumber() const       { return fMagicNumber; }
    UInt_t   GetHeaderSizeEvt() const     { return fHeaderSizeEvt; }
    UInt_t   GetHeaderSizeCrate() const   { return fHeaderSizeCrate; }
    UShort_t GetFormatVersion() const     { return fFormatVersion; }
    UShort_t GetSoftVersion() const       { return fSoftVersion; }
    UShort_t GetCameraVersion() const     { return fCameraVersion; }
    UInt_t   GetRunNumber() const         { return fRunNumber; }
    UInt_t   GetFileNumber() const        { return fFileNumber; }
    UInt_t   GetTypeID() const            { return (fRunNumber/1000000)%100; }
    UInt_t   GetFileID() const            { return fRunNumber>1000000?(fRunNumber%1000000)*1000+(fFileNumber%1000):fRunNumber; }
    TString  GetStringID() const;
    UShort_t GetTelescopeNumber() const   { return fTelescopeNumber; }
    UShort_t GetRunType() const           { return fRunType; }
    const Char_t *GetRunTypeStr() const;
          Char_t  GetRunTypeChar() const;
    const Char_t *GetProjectName() const  { return fProjectName; }
    const Char_t *GetSourceName() const   { return fSourceName; }
    const Char_t *GetSourceEpocheChar() const { return fSourceEpochChar; }
    const Char_t *GetObservationMode() const  { return fObservationMode; }
    UShort_t GetSourceEpocheDate() const  { return fSourceEpochDate; }
    UShort_t GetNumCrates() const         { return fNumCrates; }
    UShort_t GetNumPixInCrate() const     { return fNumPixInCrate; }
    UShort_t GetNumSamplesLoGain() const  { return fNumSamplesLoGain; }
    UShort_t GetNumSamplesHiGain() const  { return fNumSamplesHiGain; }
    UInt_t   GetNumSamples() const        { return fNumSamplesHiGain+fNumSamplesLoGain; }
    UShort_t GetNumBytesPerSample() const { return fNumBytesPerSample; }
    Bool_t   IsSigned() const             { return fIsSigned; }
    UInt_t   GetNumEvents() const         { return fNumEvents; }
    UInt_t   GetNumEventsRead() const     { return fNumEventsRead; }
    UShort_t GetFreqSampling() const      { return fSamplingFrequency; }
    const MTime &GetRunStart() const      { return fRunStart; }
    const MTime &GetRunEnd() const        { return fRunStop; }
    Double_t GetRunLength() const         { return !fRunStart || !fRunStop ? 0 : fRunStop-fRunStart; }
    Short_t GetPixAssignment(UShort_t i) const;
    UShort_t GetMaxPixId() const;
    UShort_t GetMinPixId() const;
    UShort_t GetNumConnectedPixels() const;
    UShort_t GetNumNormalPixels() const;
    UShort_t GetNumSpecialPixels() const;
    UInt_t   GetScale() const { switch (fNumBytesPerSample) { case 1: return 0x1; case 2: return 0x100; case 4: return 0x1000000; } return 0; }
    UInt_t   GetMax() const { switch (fNumBytesPerSample) { case 1: return 0xff; case 2: return 0xffff; case 4: return 0xffffffff; } return 0; }

    const MArrayS &GetPixAssignment() const { return *fPixAssignment; }

    UInt_t GetNumSamplesPerCrate() const
    {
        return fNumPixInCrate*(fNumSamplesLoGain+fNumSamplesHiGain);
    }

    UShort_t GetNumPixel() const;
    Int_t GetNumTotalBytes() const;

    Bool_t IsValid() const { return fMagicNumber==0xc0c0 || fMagicNumber==0xc0c1; }
    Bool_t IsMonteCarloRun() const { return fRunType>0x00ff; }
    Bool_t IsDataRun() const        { return (fRunType&0xff)==kRTData; }
    Bool_t IsPedestalRun() const    { return (fRunType&0xff)==kRTPedestal; }
    Bool_t IsCalibrationRun() const { return (fRunType&0xff)==kRTCalibration; }
    Bool_t IsPointRun() const       { return (fRunType&0xff)==kRTPointRun; }

    Bool_t IsValidRun() const { return fRunType!=kRTNone; }

    Bool_t IsConsistent(const MRawRunHeader &h) const;

    void Print(Option_t *t=NULL) const;

    Bool_t ReadEvt(std::istream& fin);
    //Bool_t WriteEvt(ostream& fout) const;

    ClassDef(MRawRunHeader, 12)	// storage container for general info
};
#endif
