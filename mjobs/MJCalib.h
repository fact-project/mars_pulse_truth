#ifndef MARS_MJCalib
#define MARS_MJCalib

#ifndef MARS_MJob
#include "MJob.h"
#endif

class MRunIter;

class MJCalib : public MJob
{
public:

private:

    static const Int_t fgCheckedPixId;  //! Default for fCheckedPixId

    enum  Storage_t                     // Possible devices for calibration
      {
        kNoStorage,
        kHistsStorage
      };                                // Possible flags for the storage of results

    Byte_t fStorage;                    // Bit-field for chosen storage type

    enum  Device_t                                       // Possible devices for calibration
    {
        kUseBlindPixel,
        kUsePINDiode
    };

    Byte_t fDevices;                                     // Bit-field for used devices for calibration

protected:

    Bool_t fIsPixelCheck;               // Check a test pixel?
    Int_t  fCheckedPixId;               // ID of checked pixel

    Bool_t fIsPulsePosCheck;            // Check pulse position?

    Bool_t CheckEnvLocal();

    Bool_t IsNoStorage    () const { return TESTBIT(fStorage,kNoStorage);    }
    Bool_t IsHistsStorage () const { return TESTBIT(fStorage,kHistsStorage); }

public:
    MJCalib();

    Bool_t IsUseBlindPixel() const { return TESTBIT(fDevices,kUseBlindPixel); }
    Bool_t IsUsePINDiode  () const { return TESTBIT(fDevices,kUsePINDiode);   }

    void SetCheckedPixId ( const Int_t  i=fgCheckedPixId ) { fCheckedPixId  = i; }

    void SetNoStorage    ( const Bool_t b=kTRUE ) { b ? SETBIT(fStorage,kNoStorage)    : CLRBIT(fStorage,kNoStorage); }
    void SetHistsStorage ( const Bool_t b=kTRUE ) { b ? SETBIT(fStorage,kHistsStorage) : CLRBIT(fStorage,kHistsStorage); }

    void SetPixelCheck     ( const Bool_t b=kTRUE )  { fIsPixelCheck        = b; }
    void SetPulsePosCheck  ( const Bool_t b=kTRUE )  { fIsPulsePosCheck     = b; }

    void SetUseBlindPixel(const Bool_t b=kTRUE) { b ? SETBIT(fDevices,kUseBlindPixel) : CLRBIT(fDevices,kUseBlindPixel); }
    void SetUsePINDiode(const Bool_t b=kTRUE)   { b ? SETBIT(fDevices,kUsePINDiode) : CLRBIT(fDevices,kUsePINDiode); }

    ClassDef(MJCalib, 0) // Base class for calibration jobs
};

#endif
