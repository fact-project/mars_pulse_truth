#ifndef MARS_MReadFiles
#define MARS_MReadFiles

#ifndef MARS_MRead
#include "MRead.h"
#endif

class izstream;

class MReadFiles : public MRead
{
private:
    TList     fFileNames;  // Array which stores the \0-terminated filenames

    TString   fFileName;   // Currently open file

    UInt_t    fNumFile;    // File number in sequence
    UInt_t    fNumLine;    // Line number in file

    MParList *fParList;    //! Parlist for reinitialization

    virtual Bool_t AnalyzeHeader(MParList &plist)=0;
    virtual Bool_t ReadHeader()=0;
    virtual Int_t  ReadEvent()=0;

protected:
    izstream *fIn;         // the inputfile

    // MReadFiles
    Bool_t ReadLine(TString &line);

    UInt_t GetNumLine() const { return fNumLine; }

    Bool_t OpenNextFile(Int_t num=-1);
    Bool_t CheckFile(TString name) const;

    // MTask
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();
    Int_t  PostProcess();

public:
    MReadFiles(const char *filename=NULL, const char *name=NULL,
               const char *title=NULL);
    ~MReadFiles();

    Int_t AddFile(const char *fname, int i=0);

    Bool_t  Rewind();
    void    Close();
    TString GetFullFileName() const { return fFileName; }

    ClassDef(MReadFiles, 0) // Base class to read a chain of files
};

#endif
