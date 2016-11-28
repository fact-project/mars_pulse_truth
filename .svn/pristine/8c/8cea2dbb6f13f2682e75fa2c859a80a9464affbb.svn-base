#ifndef MARS_MWriteAsciiFile
#define MARS_MWriteAsciiFile

#ifndef MARS_MWriteFile
#include "MWriteFile.h"
#endif
#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

class MData;

class MWriteAsciiFile : public MWriteFile
{
private:
    std::ofstream *fOut; //! ascii file

    TString fNameFile;   // name of the ascii file

    TObjArray fList;     // list of rules and containers to be written
    TObjArray fAutoDel;  //! List of object to be deleted in the destructor

    virtual Bool_t CheckAndWrite();
    virtual Bool_t IsFileOpen() const;
    virtual Bool_t GetContainer(MParList *pList);
    virtual const char *GetFileName() const { return fNameFile; }

    void Init(const char *filename, const char *name, const char *title);

public:
    MWriteAsciiFile(const char *filename, const char *contname,
                    const char *name=NULL, const char *title=NULL);
    MWriteAsciiFile(const char *filename, MParContainer *cont=NULL,
                    const char *name=NULL, const char *title=NULL);
    ~MWriteAsciiFile();

    void AddColumn(const TString rule);
    void AddColumn(MParContainer *cont, const TString member="", Double_t scale=1);
    void AddColumns(const TString name);
    void AddColumns(MParContainer *cont);

    ClassDef(MWriteAsciiFile, 0) // Class to write data into an ascii file
};

#endif
