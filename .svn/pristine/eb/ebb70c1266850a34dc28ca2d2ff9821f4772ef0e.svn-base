#ifndef MARS_MRawFitsRead
#define MARS_MRawFitsRead

#ifndef MARS_MRawFileRead
#include "MRawFileRead.h"
#endif

#include <vector>

class fits;
class MRawBoardsFACT;

class MRawFitsRead : public MRawFileRead
{
private:
    std::vector<UInt_t>   fPCTime;   //! Buffer
    std::vector<UShort_t> fPixelMap; //! 
    UInt_t fNumBoards;               //!

    MRawBoardsFACT *fRawBoards;

    Int_t         PreProcess(MParList *pList);
    std::istream *OpenFile(const char *filename);
    Bool_t        ReadRunHeader(std::istream &fin);
    Bool_t        InitReadData(std::istream &fin);
    Bool_t        ReadEvent(std::istream &fin);
    void          SkipEvent(std::istream &fin);

public:
    MRawFitsRead(const char *filename=NULL, const char *name=NULL, const char *title=NULL);

    static Bool_t IsFits(const char *name);

    Bool_t LoadMap(const char *name);

    const fits *GetFitsFile() const;

    const std::vector<UInt_t> &GetPCTime() const { return fPCTime; }

    ClassDef(MRawFitsRead, 0)	// Task to read the raw data binary file
};

#endif
