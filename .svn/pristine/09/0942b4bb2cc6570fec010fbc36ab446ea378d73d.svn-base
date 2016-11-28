#include <errno.h>
#include <stdlib.h> // strtol (Ubuntu 8.10)

#include <fstream>

#include <TRegexp.h>

#include "MAGIC.h"

#include "MArgs.h"

#include "MLog.h"
#include "MLogManip.h"

using namespace std;

static void StartUpMessage()
{
    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << endl;
    gLog << "showlog --- Mars V" << MARSVER << " compiled on <" << __DATE__ << "> using ROOT v" << ROOT_RELEASE << endl;
    gLog << endl;
}

static void Usage()
{
    gLog << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << "   showlog [options] {filename}|{location number}" << endl;
    gLog << " or" << endl;
    gLog << "   showlog [options] < filename" << endl << endl;
    gLog << " Arguments:" << endl;
    gLog << "   filename                  Input log file" << endl;
    gLog << "   location number           Open file from location callisto, star, etc." << endl;
    gLog << "                             of sequence or dataset number" << endl << endl;
    gLog.Usage();
    gLog << "   --version, -V             Show startup message with version number" << endl;
    gLog << "   -?, -h, --help            This help" << endl;
    gLog << endl;
    gLog << " This program converts colored output  made with ANSI codes" << endl;
    gLog << " (like it is done by MLog)  and redirected into a file back" << endl;
    gLog << " into colored output." << endl << endl;
    gLog << " It cannot be used to get rid of the ANSI codes. To display" << endl;
    gLog << " colored output with less use the option -R, eg." << endl;
    gLog << "   less -R logfile.log" << endl << endl;
}

void RemoveAnsi(TString &s)
{
    static const TRegexp regexp("[][[][0-9]+[m]");

    while (1)
    {
        Ssiz_t len = 0;
        Ssiz_t idx = s.Index(regexp, &len);
        if (idx<0)
            break;

        s.Remove(idx, len);
    }
}

// FIXME: Enhance this tool with a converter to HTML, etc.
//        Add option for 'no-colors'
int main(int argc, char **argv)
{
    if (!MARS::CheckRootVer())
        return 0xff;

    MArgs arg(argc, argv);

    if (arg.HasOnly("-V") || arg.HasOnly("--version"))
    {
        StartUpMessage();
        return 0;
    }

    if (arg.HasOnly("-?") || arg.HasOnly("-h") || arg.HasOnly("--help"))
    {
        Usage();
        return 2;
    }

    arg.RemoveRootArgs();

    const Bool_t kNoColors = arg.HasOnly("--no-colors") || arg.HasOnly("-a");

    gLog.Setup(arg);

    if (arg.GetNumOptions()>0)
    {
        gLog << warn << "WARNING - Unknown command line options..." << endl;
        arg.Print("options");
        gLog << endl;
    }

    //
    // check for the right usage of the program
    //
    if (arg.GetNumArguments()>2)
    {
        Usage();
        return 2;
    }

    //
    // Something special for datacenter access
    //
    TString kInput = arg.GetArgumentStr(0);
    if (arg.GetNumArguments()==2)
    {
        const Int_t num = arg.GetArgumentStr(1).Atoi();
        TString file = "/magic/data/";
        file += kInput;
        file += Form("/%04d/%08d/", num/10000, num);
        file += kInput;
        file += Form("%08d.log", num);

        kInput = file;

        gLog << inf << "Inflated file name: " << kInput << endl;
    }


    // casts necessary for gcc 2.95.3
    istream *in = arg.GetNumArguments()>0 ? (istream*)new ifstream(kInput) : (istream*)&cin;
    if (!*in)
    {
        gLog << err << "Cannot open file " << kInput << ": " << strerror(errno) << endl;
        return 2;
    }

    TString s;
    while (1)
    {
        s.ReadLine(*in);
        if (!*in)
            break;

        if (kNoColors)
            RemoveAnsi(s);
        else
            s.ReplaceAll("", "\033");

        const char *end = s.Data()+s.Length();
        for (const char *c=s.Data(); c<end; c++)
        {
            char *to = NULL;
            Int_t n  = -1;

            if (c+1<end-1 && *c=='\033' && *(c+1)=='[')
                n = strtol(c+2, &to, 10);

            if (to==NULL || *to!='m')
            {
                gLog << *c;
                continue;
            }

            c = to;

            switch (n)
            {
            case 0:
                if (c<end-1)
                    gLog << flush << all;
                continue;
            case 31:
                gLog << flush << err;
                continue;
            case 32:
                gLog << flush << inf;
                continue;
            case 33:
                gLog << flush << warn;
                continue;
            case 34:
                gLog << flush << dbg;
                continue;
            }
            gLog << flush << "\033[" << n << "m";
        }
        gLog << endl << all;
    }

    if (in!=&cin)
        delete in;

    return 1;
}
