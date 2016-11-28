#include <TROOT.h>
#include <TClass.h>
#include <TStyle.h>
#include <TGClient.h>
#include <TRint.h>
#include <TObjectTable.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MArgs.h"

#include "MStatusDisplay.h"

using namespace std;

static void StartUpMessage()
{
    //                1         2         3         4         5
    //       12345678901234567890123456789012345678901234567890
    gLog << endl;
    gLog << "showplot --- Mars V" << MARSVER << " compiled on <" << __DATE__ << "> using ROOT v" << ROOT_RELEASE << endl;
    //gLog << endl;
}

static void Usage()
{
    //                1         2         3         4         5         6         7         8
    //       12345678901234567890123456789012345678901234567890123456789012345678901234567890
    gLog << all << endl;
    gLog << "Sorry the usage is:" << endl;
    gLog << " showplot [options]" << endl;
    gLog << " showplot [options] file1 file2 file3 ..." << endl;
    gLog << " showplot [options] filetype number" << endl << endl;
    gLog << " Arguments:" << endl;
    gLog << "   fil1 file2 ...            Input files containing an MStatusArray of TCanvases" << endl;
    gLog << "   filetype number           Open file of filetype calib, signal, star, etc." << endl;
    gLog << "                             of sequence or dataset number" << endl << endl;
    gLog << " Root Options:" << endl;
    gLog << "   -b                        Batch mode (no graphical output to screen)" << endl<<endl;
    gLog << "   -r                        Start the root interpreter" << endl<<endl;
    gLog << " Options: "<< endl;
    gLog.Usage();
    gLog << endl;
    gLog << " General Output Options: "<< endl;
    gLog << "   --print[=printer]         Print to printer" << endl;
    gLog << "   --save-as-ps[=filename]   Save plots as postscript" << endl;
    gLog << "   --save-as-pdf[=filename]  Save plots as pdf-file" << endl;
    gLog << "   --save-as-svg[=filename]  Save plots as svg-file" << endl;
    gLog << "   --save-as-gif[=filename]  Save plots as gif files" << endl;
    gLog << "   --save-as-jpg[=filename]  Save plots as jpg files" << endl;
    gLog << "   --save-as-xpm[=filename]  Save plots as xpm files" << endl;
    gLog << "   --save-as-png[=filename]  Save plots as png files" << endl;
    gLog << "   --save-as-bmp[=filename]  Save plots as bmp files" << endl;
    gLog << "   --save-as-xml[=filename]  Save plots as xml files" << endl;
    gLog << "   --save-as-tiff[=filename] Save plots as tiff files" << endl;
    gLog << "   --save-as-root[=filename] Save plots as root file" << endl << endl;
    gLog << "   --save-as-csv[=filename]  Save list of plots as csv table" << endl << endl;
    gLog << "   --save-as-C[=filename]    Save plots as root scripts" << endl;
    gLog << "   --tab=num                 Save only tab number num" << endl << endl;
    gLog << " Print only options:" << endl;
    gLog << "   --print-cmd='lpr -P%p %f' Set the printer command" << endl;
    gLog << "   --print-dir=/tmp          Set the printing temp directory" << endl << endl;
    gLog << " Size options:" << endl;
    gLog << "   --display-width=w         Set width of display window to w" << endl;
    gLog << "   --display-height=h        Set height of display window to h" << endl;
    gLog << "   --canvas-width=w          Set width of canvas' contained by display to w" << endl;
    gLog << "   --canvas-height=h         Set height of canvas' contained by display to h" << endl;
    gLog << "   --auto-size               Determin optimum size (not available in batch mode)" << endl;
    gLog << endl;
    gLog << "   --debug-mem               Debug memory usage" << endl;
    gLog << "   --version, -V             Show startup message with version number" << endl;
    gLog << "   -?, -h, --help            This help" << endl;
    gLog << endl;
    gLog << "Description:" << endl;
    gLog << " Use showplot to display a MStatusArray in an MStatusDisplay." << endl;
    gLog << " MStatusArrays are typically written by programs showing data" << endl;
    gLog << " check plots, like callisto." << endl << endl;
    gLog << " Only the last size option given is taken into account." << endl;
    gLog << " Width or height is set according to height or width." << endl << endl;
    gLog << " In batch mode display width and height and auto-size is ignored." << endl << endl;
    gLog << " If multiple files are written (e.g. a while display is written as png-files)" << endl;
    gLog << " you can use %%name%% and %%tab%% in the name, which is then replaced by" << endl;
    gLog << " the tab-name or tab.number respectively. If none of them are given a" << endl;
    gLog << " minus and the tab.number is added in front of the extension." << endl << endl;
    gLog << "Printing:" << endl;
    gLog << " For more details see MStatusDisplay::PrintPS" << endl << endl;
    gLog << "Examples:" << endl;
    gLog << " showplot filename.root" << endl;
    gLog << " showplot -b --null --print --print-cmd='psnup -2 %f' filename.root | lpr" << endl;
    gLog << " showplot -b --print --print-cmd='psbook %f | psnup -2 | lpr' filename.root" << endl;
    gLog << " showplot -b --print --print-cmd='cat %f' filename.root > filename.ps" << endl;
    gLog << " showplot -b --save-as-ps filename.root" << endl;
    gLog << " showplot -b --save-as-gif=tab5.gif --tab=5 filename.root" << endl;
    gLog << " showplot -b --save-as-ps --print=lp2 filename.root" << endl;
    gLog << endl;
}

TString InflatePath(const MArgs &args)
{
    TString kInput = args.GetArgumentStr(0);

    //
    // Something special for datacenter access
    //
    if (args.GetNumArguments()==1)
        return kInput;

    if (args.GetNumArguments()!=2 || !args.GetArgumentStr(1).IsDigit())
        return "";

    //
    // Something special for datacenter access
    //
    const Int_t num = args.GetArgumentInt(1);

    TString file = "/magic/data/";

    kInput.ToLower();
    switch (kInput.Hash())
    {
    case 3438106369U:
    case  764164111U:
        file += "callisto";
        break;
    default:
        file += kInput;
        break;
    }
    file += kInput==(TString)"ganymed" ? Form("/%05d", num/100000) : Form("/%04d", num/10000);
    file += Form("/%08d/", num);
    file += kInput;
    file += Form("%08d.root", num);

    kInput = file;

    gLog << inf << "Inflated file name: " << kInput << endl;

    return kInput;
}

int main(int argc, char **argv)
{
    if (!MARS::CheckRootVer())
        return 0xff;

    MLog::RedirectErrorHandler(MLog::kColor);

    //
    // Evaluate arguments
    //
    MArgs arg(argc, argv);
    gLog.Setup(arg);

    StartUpMessage();

    if (arg.HasOnly("-V") || arg.HasOnly("--version"))
        return 0;

    if (arg.HasOnly("-?") || arg.HasOnly("-h") || arg.HasOnly("--help"))
    {
        Usage();
        return 2;
    }

    const Bool_t kBatch      = arg.HasOnlyAndRemove("-b");
    const Bool_t kRoot       = arg.HasOnlyAndRemove("-r");
    const Bool_t kDebugMem   = arg.HasOnlyAndRemove("--debug-mem");

    const Int_t  kTab        = arg.GetIntAndRemove("--tab=", -1);

    const Bool_t kPrint      = arg.HasOnlyAndRemove("--print")        || arg.Has("--print=");
    const Bool_t kSaveAsPs   = arg.HasOnlyAndRemove("--save-as-ps")   || arg.Has("--save-as-ps=");
    const Bool_t kSaveAsPdf  = arg.HasOnlyAndRemove("--save-as-pdf")  || arg.Has("--save-as-pdf=");
    const Bool_t kSaveAsSvg  = arg.HasOnlyAndRemove("--save-as-svg")  || arg.Has("--save-as-svg=");
    const Bool_t kSaveAsGif  = arg.HasOnlyAndRemove("--save-as-gif")  || arg.Has("--save-as-gif=");
    const Bool_t kSaveAsJpg  = arg.HasOnlyAndRemove("--save-as-jpg")  || arg.Has("--save-as-jpg=");
    const Bool_t kSaveAsXpm  = arg.HasOnlyAndRemove("--save-as-xpm")  || arg.Has("--save-as-xpm=");
    const Bool_t kSaveAsPng  = arg.HasOnlyAndRemove("--save-as-png")  || arg.Has("--save-as-png=");
    const Bool_t kSaveAsBmp  = arg.HasOnlyAndRemove("--save-as-bmp")  || arg.Has("--save-as-bmp=");
    const Bool_t kSaveAsXml  = arg.HasOnlyAndRemove("--save-as-xml")  || arg.Has("--save-as-xml=");
    const Bool_t kSaveAsCsv  = arg.HasOnlyAndRemove("--save-as-csv")  || arg.Has("--save-as-csv=");
    const Bool_t kSaveAsTiff = arg.HasOnlyAndRemove("--save-as-tiff") || arg.Has("--save-as-tiff=");
    const Bool_t kSaveAsRoot = arg.HasOnlyAndRemove("--save-as-root") || arg.Has("--save-as-root=");
    const Bool_t kSaveAsC    = arg.HasOnlyAndRemove("--save-as-C")    || arg.Has("--save-as-C=");

    const Int_t  kCanvasWidth  = arg.GetIntAndRemove("--canvas-width=",   -1);
    const Int_t  kCanvasHeight = arg.GetIntAndRemove("--canvas-height=",  -1);

    const Bool_t kAutoSize = arg.HasOnlyAndRemove("--auto-size");
    Int_t  kDisplayWidth   = arg.GetIntAndRemove("--display-width=",  -1);
    Int_t  kDisplayHeight  = arg.GetIntAndRemove("--display-height=", -1);
    if (kAutoSize)
    {
        kDisplayWidth=0;
        kDisplayHeight=0;
    }

    //
    // Get file name(s)
    //
    const TString kInput = InflatePath(arg);
    const TString kTitle = kInput.IsNull() ? arg.GetArgumentStr(0) + "..."  : kInput;

    //
    // Process filenames
    //
    const TString kNamePrint = arg.GetStringAndRemove("--print=",        kInput);
    const TString kPrintCmd  = arg.GetStringAndRemove("--print-cmd=",    kInput);
    const TString kPrintDir  = arg.GetStringAndRemove("--print-dir=",    kInput);
    const TString kNamePs    = arg.GetStringAndRemove("--save-as-ps=",   kInput);
    const TString kNamePdf   = arg.GetStringAndRemove("--save-as-pdf=",  kInput);
    const TString kNameSvg   = arg.GetStringAndRemove("--save-as-svg=",  kInput);
    const TString kNameGif   = arg.GetStringAndRemove("--save-as-gif=",  kInput);
    const TString kNameJpg   = arg.GetStringAndRemove("--save-as-jpg=",  kInput);
    const TString kNameXpm   = arg.GetStringAndRemove("--save-as-xpm=",  kInput);
    const TString kNamePng   = arg.GetStringAndRemove("--save-as-png=",  kInput);
    const TString kNameBmp   = arg.GetStringAndRemove("--save-as-bmp=",  kInput);
    const TString kNameXml   = arg.GetStringAndRemove("--save-as-xml=",  kInput);
    const TString kNameCsv   = arg.GetStringAndRemove("--save-as-csv=",  kInput);
    const TString kNameTiff  = arg.GetStringAndRemove("--save-as-tiff=", kInput);
    const TString kNameRoot  = arg.GetStringAndRemove("--save-as-root=", kInput);
    const TString kNameC     = arg.GetStringAndRemove("--save-as-C=",    kInput);

    //
    // check for the right usage of the program
    //
    if (arg.GetNumOptions()>0)
    {
        gLog << err << "Unknown command line options..." << endl;
        arg.Print("options");
        gLog << endl;
        return 3;
    }

    if (kDebugMem)
        TObject::SetObjectStat(kTRUE);

    //
    // Initialize root environment
    //
    TApplication *app = kRoot ? new TRint("showplot", &argc, argv) : new TApplication("showplot", &argc, argv);
    if ((!gROOT->IsBatch() && !gClient) || (gROOT->IsBatch() && !kBatch))
    {
        gLog << err << "Bombing... maybe your DISPLAY variable is not set correctly!" << endl;
        return 1;
    }

    gStyle->SetPalette(1, 0);

    //
    // Update frequency by default = 1Hz
    //
    MStatusDisplay *d = new MStatusDisplay(kDisplayWidth, kDisplayHeight);

    // From now on each 'Exit' means: Terminate the application
    d->SetTitle(kTitle);
    d->SetWindowName(kTitle);

    if (kCanvasHeight>0)
        d->SetCanvasHeight(kCanvasHeight);
    if (kCanvasWidth>0)
        d->SetCanvasWidth(kCanvasWidth);

    if (!kInput.IsNull())
        d->Open(kInput);
    else
    {
        for (int i=0; i<arg.GetNumArguments(); i++)
            d->Open(arg.GetArgumentStr(i));
    }

    if (kPrint)
        d->PrintPS(kTab,    kNamePrint, kPrintCmd, kPrintDir);
    if (kSaveAsPs)
        d->SaveAsPS(kTab,   kNamePs);
    if (kSaveAsPdf)
        d->SaveAsPDF(kTab,  kNamePdf);
    if (kSaveAsSvg)
        d->SaveAsSVG(kTab,  kNameSvg);
    if (kSaveAsGif)
        d->SaveAsGIF(kTab,  kNameGif);
    if (kSaveAsJpg)
        d->SaveAsJPG(kTab,  kNameJpg);
    if (kSaveAsXpm)
        d->SaveAsXPM(kTab,  kNameXpm);
    if (kSaveAsPng)
        d->SaveAsPNG(kTab,  kNamePng);
    if (kSaveAsBmp)
        d->SaveAsBMP(kTab,  kNameBmp);
    if (kSaveAsXml)
        d->SaveAsXML(kTab,  kNameXml);
    if (kSaveAsCsv)
        d->SaveAsCSV(kTab,  kNameCsv);
    if (kSaveAsTiff)
        d->SaveAsTIFF(kTab, kNameTiff);
    if (kSaveAsRoot)
        d->SaveAsRoot(kTab, kNameRoot);
    if (kSaveAsC)
        d->SaveAsC(kTab,    kNameC);

    if (arg.GetNumArguments()>0)
    {
        if (d->GetNumTabs())
            gLog << all << d->GetNumTabs() << " tab(s) displayed." << endl;
        else
            gLog << err << "Display empty... closing." << endl;
    }
    gLog << endl;

    if (kBatch || (arg.GetNumArguments()>0 && d->GetNumTabs()==0))
    {
        delete d;
        return 0;
    }

    // From now on each 'Close' means: Terminate the application
    d->SetBit(MStatusDisplay::kExitLoopOnClose);

    // Wait until the user decides to exit the application
    app->Run(kFALSE);
    delete app;

    if (TObject::GetObjectStat())
    {
        TObject::SetObjectStat(kFALSE);
        gObjectTable->Print();
    }

    return 0;
}
