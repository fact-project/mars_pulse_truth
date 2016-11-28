#ifndef MARS_MGeomApply
#define MARS_MGeomApply

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MGeomCam;

class MGeomApply : public MTask
{
private:
    TString fGeomName; // Name of geometry class

    TList *fNamesList;
    TList *fList;

    void ProcessAutomatic(MParList &plist, const MGeomCam &geom) const;
    Bool_t ProcessManual(MParList &plist, const MGeomCam &geom) const;

    Int_t  PreProcess(MParList *plist);
    Bool_t ReInit(MParList *pList);
    void   StreamPrimitive(std::ostream &out) const;

public:
    MGeomApply(const char *name=NULL, const char *title=NULL);
    ~MGeomApply();

    void SetGeometry(TString geom) { fGeomName = geom; }

    void AddCamEvent(TObject *obj);
    void AddCamEvent(const char *name);

    ClassDef(MGeomApply, 0) // Task to apply geometry settings
};
    
#endif

