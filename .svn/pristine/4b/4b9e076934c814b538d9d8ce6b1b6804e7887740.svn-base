#ifndef MARS_MLogHtml
#define MARS_MLogHtml

#ifndef MARS_MLogPlugin
#include "MLogPlugin.h"
#endif

class MLogHtml : public MLogPlugin
{
private:
    std::ofstream *fOut;

    Bool_t fUnderline;
    Int_t  fColor;

    enum { kFontOpen=BIT(15) };

public:
    MLogHtml(const char *name);
    MLogHtml() : fOut(0)
    {
    }

    ~MLogHtml();

    void Underline();
    void SetColor(int col);
    void WriteBuffer(const char *str, int len);

    ClassDef(MLogHtml, 0) // Logger Plugin for HTML
};

#endif
