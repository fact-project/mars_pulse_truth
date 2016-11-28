#ifndef MARS_MLogPlugin
#define MARS_MLogPlugin

#ifndef ROOT_TObject
#include <TObject.h>
#endif

class MLogPlugin : public TObject
{
public:
    virtual void SetColor(int) { }
    virtual void Underline() { }
    virtual void WriteBuffer(const char *str, int len) = 0;

    ClassDef(MLogPlugin, 0) // Base for a logger plugin
};

#endif
