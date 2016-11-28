#ifndef MARS_MReadScanFile
#define MARS_MReadScanFile

#ifndef MARS_MReadFiles
#include "MReadFiles.h"
#endif

class MParList;

class MReadScanFile : public MReadFiles
{
private:
    TList fList;

    Int_t fLength;

    Bool_t IsDelimiter(const TString &str) const;
    Bool_t ReadDelimiter();

    Bool_t AnalyzeHeader(MParList &plist);
    Bool_t ReadHeader();
    Int_t  ReadEvent();
    Int_t  PostProcess();

public:
    MReadScanFile(const char *filename=NULL, const char *name=NULL,
                  const char *title=NULL) : MReadFiles(filename, name, title), fLength(-1) { }

    UInt_t GetEntries();

    ClassDef(MReadScanFile, 0) // Reads files written with TTree::Scan
};

#endif
