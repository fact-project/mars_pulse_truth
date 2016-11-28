#ifndef MARS_MReadMarsFile
#define MARS_MReadMarsFile

#ifndef MARS_MReadTree
#include "MReadTree.h"
#endif

class MParList;
class MTaskList;

class MReadMarsFile : public MReadTree
{
private:
    MReadTree *fRun;
    MParList  *fParList;  //! Parlist for reinitialization

    Bool_t Notify();

    Int_t PreProcess(MParList *pList);
  
public:
    MReadMarsFile();
    MReadMarsFile(const char *treename, const char *filename=NULL, const char *name=NULL, const char *title=NULL);
    ~MReadMarsFile();

    static Byte_t IsFileValid(const char *name);

    void SortFiles();

    Int_t AddFile(const char *fname, Int_t entries=-1);

    Bool_t Rewind() { if (fRun) fRun->Rewind(); MReadTree::Rewind(); return kTRUE; }

    ClassDef(MReadMarsFile, 1)	// Reads a tree from file(s) and the information from the 'RunHeader'-tree
};

#endif
