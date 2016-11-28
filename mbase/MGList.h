#ifndef MARS_MGList
#define MARS_MGList

#ifndef ROOT_TList
#include <TList.h>
#endif

class TGWidget;
class TGPicture;

class MGList : public TList
{
private:
    TGWidget *GetWidget(TObject *obj) const;
    Bool_t    IsExisting(TObject *obj) const;

    void      AddPicture(TGPicture *pic, const char *name);
    void      DeletePictures();

public:
    MGList();
    ~MGList();

    void Add(TObject *obj);
    void Add(TObject *obj, Option_t *opt);

    void Delete(Option_t *option="");

    const TGPicture *GetPicture(const char *name);
    const TGPicture *GetPicture(const char *name, Int_t width, Int_t height);

    TObject *FindWidget(Int_t id) const;

    ClassDef(MGList, 0)
};

#endif
