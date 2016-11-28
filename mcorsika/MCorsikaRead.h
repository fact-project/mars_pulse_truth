#ifndef MARS_MCorsikaRead
#define MARS_MCorsikaRead

#include <vector>

#ifndef MARS_MRead
#include "MRead.h"
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

class TList;
//class MZlib;
class MTaskList;

class MCorsikaRunHeader;
class MCorsikaEvtHeader;
class MPhotonEvent;
class MCorsikaFormat;

class MCorsikaRead : public MRead
{
private:
    MCorsikaRunHeader *fRunHeader;  //! run header information container to fill from file
    MCorsikaEvtHeader *fEvtHeader;  //! event header information container to fill from file
    MPhotonEvent      *fEvent;      //! event information

    Int_t           fTelescopeIdx;  // Index of telescope to be extracted
    Int_t           fArrayIdx;      // Index of telescope-array to be extracted
    Bool_t          fForceMode;     // Force mode skipping defect RUNE

    TList    *fFileNames;      // list of file names
    UInt_t    fNumFile;        //! number of next file
    UInt_t    fNumEvents;      //! input stream (file to read from)
    UInt_t    fNumTotalEvents; //! total number of events in all files

    MCorsikaFormat *fInFormat; //! access to input corsika data

    MParList *fParList;        //! tasklist to call ReInit from

    Int_t  fNumTelescopes;     //! Number of telescopes in array
    Int_t  fNumTelescope;      //! Number of telescope currently being read
    TArrayF fTelescopeX;       //! x pos (measured towards north, unit: cm)
    TArrayF fTelescopeY;       //! y pos (measured towards west,  unit: cm)
    TArrayF fTelescopeZ;       //! z pos (from detection level,   unit: cm)
    TArrayF fTelescopeR;       //! Radii of spheres around tel. (unit: cm)

    Int_t   fBlockType;        //! block type of the just read block header
    Int_t   fBlockVersion;     //! block version of the just read block header 
    Int_t   fBlockIdentifier;  //! block identifier of the just read block header
    Int_t   fBlockLength;      //! block length from the just read block header

    Int_t   fReadState;        //! 0: nothing read yet 
                               //  1: RUNH is already read
                               //  2: EVTH is already read
                               //  3: EVTE is already read, run still open
                               //  4: inside of a top level block (1205)
                               //  5: RUNE is already read
                               // 10: raw events are currently read
                               // 15: undefined status
    Int_t   fTopBlockLength;   // remaining length of the current top-level block 1204

    std::vector<Float_t>  fRawEvemtBuffer;     //! buffer of raw event data
    //UInt_t    fInterleave;
    //Bool_t    fForce;


    Int_t  OpenNextFile(Bool_t print=kTRUE);
    Bool_t CalcNumTotalEvents();
    Int_t  ReadTelescopePosition();
    Int_t  ReadNextBlockHeader();

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();


public:
    MCorsikaRead(const char *filename=NULL, const char *name=NULL, const char *title=NULL);
    ~MCorsikaRead();

    void SetForceMode(Bool_t b=kTRUE) { fForceMode=b; }
    void SetTelescopeIdx(Int_t idx)   { fTelescopeIdx = idx; }
    void SetArrayIdx(Int_t idx)       { fArrayIdx = idx; }

    TString GetFullFileName() const;

    Int_t  AddFile(const char *fname, Int_t entries=-1);
    Bool_t Rewind();
    UInt_t GetEntries() { return fNumTotalEvents/*/fInterleave*/; }

    ClassDef(MCorsikaRead, 0)	// Task to read the raw data binary file
};

#endif
