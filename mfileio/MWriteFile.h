#ifndef MARS_MWriteFile
#define MARS_MWriteFile

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MWriteFile : public MTask
{
protected:
    Bool_t ReInit(MParList *pList);

private:
    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    virtual Bool_t      IsFileOpen() const = 0;
    virtual Bool_t      CheckAndWrite() = 0;
    virtual Bool_t      GetContainer(MParList *pList) = 0;
    virtual const char *GetFileName() const = 0;


    ClassDef(MWriteFile, 0)	// Base class for tasks to write single containers to several output formats
};

#endif
