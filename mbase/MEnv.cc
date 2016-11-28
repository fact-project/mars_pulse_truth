/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz  2/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2009
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MEnv
//
// It is a slightly changed version of TEnv. It logs all resources which are
// touched, so that you can print all untouched resources by
// PrintUntouched()
//
// A new special resource is available. With the resource "Include"
// you can include resources from other files, for example
//
//   Include: file1.rc file2.rc
//
// Including can be done recursively. Resources from the included files
// have lower priority. This allows to write a resource file with your
// default resources which then can be included in other files overwriting
// some of the resources.
//
// If given paths are not absolute there base is always th elocation of
// the including file.
//
// Class Version 1:
// ----------------
//  + TString fName
//
//////////////////////////////////////////////////////////////////////////////
#include "MEnv.h"

#include <fstream>
#include <exception>

#include <Gtypes.h>
#include <TObjString.h>
#include <TObjArray.h>

#include <TPave.h>
#include <TAttText.h>
#include <TAttMarker.h>
#include <THashList.h>   // needed since root v5.10/00 (TEnv::GetTable)

#include "MLog.h"
#include "MLogManip.h"

#include "MArgs.h"
#include "MString.h"

ClassImp(MEnv);

using namespace std;

//---------------------------------------------------------------------------
//
// (Default) constructor. If the given file cannot be accessed SetRcName("")
// is called which can then be checked by IsValid()
//
// The second argument is the default name. Usually as name of the MEnv
// the base name of the file is returned. By giving a default name
// you can overwrite this behavious. This is useful for I/O
// because it allows to use Read and Write without argument.
//
MEnv::MEnv(const char *name, const char *defname) : TEnv(name), fName(defname)
{
    fChecked.SetOwner();

    // If TEnv::TEnv has not set fRcName
    if (!IsValid())
        return;

    // ExpandPathName (not done by TEnv::TEnv) and read again
    TString fname(name);
    gSystem->ExpandPathName(fname);

    // Is file accessible
    if (gSystem->AccessPathName(fname, kFileExists))
        fname = "";

    SetRcName(fname);
    
    if (!IsTEnvCompliant(name)){
        throw "This file can't be correctly parsed by TEnv";
        return;
    }

    // No file found
    if (fname.IsNull())
        return;

    // File has been already processed, but ReadInclude is part of a
    // derived function, i.e. not yet executed.
    if (GetEntries()>0 || fname==name)
    {
        if (ReadInclude()<0)
            SetRcName("");
        return;
    }

    // File not yet processed. Reread file.
    if (ReadFile(fname, kEnvLocal)<0)
        SetRcName("");
}

//---------------------------------------------------------------------------
//
// Process an Include directive and read the corresponding contents
//
Int_t MEnv::ReadInclude()
{
    // Check for "Include" resource
    const TString incl = GetValue("Include", "");
    if (incl.IsNull())
        return 0;

    const char *dir = gSystem->DirName(GetRcName());

    // Tokenize the array into single files divided by a whitespace
    TObjArray *arr = incl.Tokenize(" ");

    // We have to rebuild the Include array from scratch to get the
    // correct sorting for a possible rereading.
    SetValue("Include", "");

    // FIXME: Make sure that recursions don't crash the system!

    for (int i=0; i<arr->GetEntries(); i++)
    {
        // Get file name to include
        TString fenv = (*arr)[i]->GetName();

        // If the is not anabsolute path we prepend the dir-name
        // of the including file. This allows that includes
        // do not necessarily need absolute paths and paths are always
        // relative to the location of th eincluding file.
        if (!gSystem->IsAbsoluteFileName(fenv))
        {
            fenv.Prepend("/");
            fenv.Prepend(dir);
        }

        // Read included file and check if its valid
        const MEnv env(fenv);
        if (!env.IsValid())
        {
            delete arr;
            return -1;
        }

        // Add file name before its childs
        SetValue("+Include", fenv);

        // If it is valid add entries from include without overwriting,
        // i.e. the included resources have lower priority
        AddEnv(env, kFALSE);

        // Get a possible child include from env
        const TString incl2 = const_cast<MEnv&>(env).GetValue("Include", "");
        if (!incl2.IsNull())
            SetValue("+Include", incl2);
    }

    delete arr;

    // Get final compiled resource
    TString inc = GetValue("Include", "");

    // Remove obsolete whitespaces for convinience
    inc.ReplaceAll("  ", " ");
    inc = inc.Strip(TString::kBoth);

    // Set final resource, now as kEnvLocal (previously set as kEnvChanged)
    SetValue("Include", inc, kEnvLocal);

    // FIXME: Remove douplets in the correct order

    return 0;
}

//---------------------------------------------------------------------------
//
// Check if the input file is compatible to the way TEnv parses a resource file.
// 
// Returns true in case everything is file and
//         false in case there is a problem with the file.
//
// Background:
//      This check has been introduced as a workaround for a ROOT feature.
//      TEnv will not see the last line in a text file, when this last line 
//      does not end with a \n.
//
//      In addition we check for the occurence of \r, since some editors would
//      place \r instead of \n (unlike windows that uses \n\r), which would
//      also result in problems.
//      FIXME: So currently we also complain about "\n\r" and "\r\n" as well as 
//      "\r  \t   \n" or "\n      \t\t\t\r", which would in fact not be a problem at 
//      all.
Bool_t MEnv::IsTEnvCompliant(const char *fname)
{
    ifstream ifs(fname);
    bool printable_char_since_last_lf = false;
    char c;
    while (ifs.good()){
        ifs.get(c);

        if (c==13){
            gLog << err << "Menv::IsTEnvCompliant - file:" << fname 
                << " contains \\r this might lead to parsing errors. "
                <<"Please exchange \\r by \\n everywhere in that file." 
                << endl;
            return false;
        }
        if (c=='\n'){
            printable_char_since_last_lf = false;
        }
        if (isgraph(c)){
            printable_char_since_last_lf = true;
        }
    }
    if (printable_char_since_last_lf){
        gLog << err << "Menv::IsTEnvCompliant - file:" << fname 
            << " must have \\n at the end of the file. "
            <<"Please make sure this is the case." 
            << endl;
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------
//
// Read and parse the resource file for a certain level.
// Returns -1 on case of error, 0 in case of success.
//
// Check for an include directive
//
Int_t MEnv::ReadFile(const char *fname, EEnvLevel level)
{
    // First read the file via TEnv
    if (TEnv::ReadFile(fname, level)<0)
        return -1;

    return ReadInclude();
}

// --------------------------------------------------------------------------
//
// Read an object from the current directory. If no name is given
// the name of this object is used. 
//
Int_t MEnv::Read(const char *name)
{
    const Int_t rc = TEnv::Read(name?name:(const char*)fName);
    //if (name)
    //    SetName(name);
    return rc;
}

//---------------------------------------------------------------------------
//
// Make sure that the name used for writing doesn't contain a full path
//
const char *MEnv::GetName() const
{
    if (!fName.IsNull())
        return fName;

    const char *pos = strrchr(GetRcName(), '/');
    return pos>0 ? pos+1 : GetRcName();
}

//---------------------------------------------------------------------------
//
// Return the total number of entries in the table
//
Int_t MEnv::GetEntries() const
{
    if (!GetTable())
        return -1;

    return GetTable()->GetEntries();
}

//---------------------------------------------------------------------------
//
// Compile str+post and make sure that in between there is a unique dot.
//
TString MEnv::Compile(TString str, const char *post) const
{
    if (!str.IsNull() && !str.EndsWith("."))
        str += ".";

    str += post;

    return str;
}

//---------------------------------------------------------------------------
//
// Get the value from the table and remember the value as checked 
//
Int_t MEnv::GetValue(const char *name, Int_t dflt)
{
    if (!fChecked.FindObject(name))
        fChecked.Add(new TObjString(name));
    return TEnv::GetValue(name, dflt);
}

//---------------------------------------------------------------------------
//
// Get the value from the table and remember the value as checked 
//
Double_t MEnv::GetValue(const char *name, Double_t dflt)
{
    if (!fChecked.FindObject(name))
        fChecked.Add(new TObjString(name));
    return TEnv::GetValue(name, dflt);
}

//---------------------------------------------------------------------------
//
// Get the value from the table and remember the value as checked 
//
const char *MEnv::GetValue(const char *name, const char *dflt)
{
    if (!fChecked.FindObject(name))
        fChecked.Add(new TObjString(name));
    return TEnv::GetValue(name, dflt);
}

//---------------------------------------------------------------------------
//
// TEnv doen't have a streamer --> cannot be cloned
//   --> we have to clone it ourself
//
TObject *MEnv::Clone(const char *) const
{
    MEnv *env = new MEnv("/dev/null");
    env->SetRcName(GetRcName());
    env->AddEnv(*this);
    return env;
}

//---------------------------------------------------------------------------
//
// Interprete fill style: Hollow, Solid, Hatch, 0%-100%
// If no text style is detected the value is converted to an integer.
//
Int_t MEnv::GetFillStyle(const char *name, Int_t dftl)
{
    TString str = GetValue(name, "");
    str = str.Strip(TString::kBoth);
    if (str.IsNull())
        return dftl;

    str.ToLower();

    switch (str.Hash())
    {
    case 2374867578U: return 0;    // hollow
    case  764279305U: return 1001; // solid
    case 1854683492U: return 2001; // hatch
    }

    return str.EndsWith("%") ? 4000+str.Atoi() : str.Atoi();
}

//---------------------------------------------------------------------------
//
// Interprete line style: Solid, Dashed, Dotted, DashDotted
// If no line style is detected the value is converted to an integer.
//
Int_t MEnv::GetLineStyle(const char *name, Int_t dftl)
{
    TString str = GetValue(name, "");
    str = str.Strip(TString::kBoth);
    if (str.IsNull())
        return dftl;

    str.ToLower();

    switch (str.Hash())
    {
    case  764279305U: return kSolid;
    case  241979881U: return kDashed;
    case 2391642602U: return kDotted;
    case 1124931659U: return kDashDotted;
    }

    return str.Atoi();
}

//---------------------------------------------------------------------------
//
// Interprete alignment: Top, Right, Left, Bottom, Center, tr, cc, bl, ...
// If no text align is detected the value is converted to an integer.
//
//  eg.
//    Top Right
//    Bottom Center
//    Center
//    tr
//    br
//    cr
//
Int_t MEnv::GetAlign(const char *name, Int_t dftl)
{
    TString str = GetValue(name, "");
    str = str.Strip(TString::kBoth);
    if (str.IsNull())
        return dftl;

    str.ToLower();

    switch (str.Hash())
    {
    case 29746: return 33; // tr
    case 25379: return 22; // cc
    case 25132: return 11; // bl

    case 25388: return 12; // cl
    case 25394: return 32; // cr

    case 29731: return 23; // tc
    case 25123: return 32; // bc

    case 29740: return 13; // tl
    case 25138: return 13; // br
    }

    Int_t align = 0;
    if (str.Contains("right", TString::kIgnoreCase))
        align += 3;
    if (str.Contains("left", TString::kIgnoreCase))
        align += 1;
    if (str.Contains("bottom", TString::kIgnoreCase))
        align += 10;
    if (str.Contains("top", TString::kIgnoreCase))
        align += 30;

    if (str.Contains("center", TString::kIgnoreCase))
    {
        if (align==0)
            return 22;
        if (align/10==0)
            return align+20;
        if (align%10==0)
            return align+2;
    }

    return align>0 ? align : str.Atoi();
}

//---------------------------------------------------------------------------
//
// Interprete color: Black, White, Red, Green, Blue, Yellow, Magenta,
// Cyan, Gray1-5, Grey1-5.
// If no text color is detected the value is converted to an integer.
//
//  eg.
//    Red
//    Light Red
//    Dark Red
//
Int_t MEnv::GetColor(const char *name, Int_t dftl)
{
    TString str = GetValue(name, "");

    str = str.Strip(TString::kBoth);
    if (str.IsNull())
        return dftl;

    str.ToLower();

    Int_t offset=0;
    if (str.Contains("dark"))
    {
        str.ReplaceAll("dark", "");
        str = str.Strip(TString::kBoth);
        offset = 100;
    }
    if (str.Contains("light"))
    {
        str.ReplaceAll("light", "");
        str = str.Strip(TString::kBoth);
        offset = 150;
    }

    switch (str.Hash())
    {
    case 2368543371U: return kWhite+offset;
    case 1814927399U: return kBlack+offset;
    case    7496964U: return kRed+offset;
    case 2897107074U: return kGreen+offset;
    case 1702194402U: return kBlue+offset;
    case 2374817882U: return kYellow+offset;
    case 2894218701U: return kMagenta+offset;
    case 1851881955U: return kCyan+offset;
    case  749623518U: return 19; // grey1
    case  749623517U: return 18; // grey2
    case  749623516U: return 17; // grey3
    case  749623515U: return 16; // grey4
    case  749623514U: return 15; // grey5
    case  749623513U: return 14; // grey6
    case  749623512U: return 13; // grey7
    case  749623511U: return 12; // grey8
    case  741234910U: return 19; // gray1
    case  741234909U: return 18; // gray2
    case  741234908U: return 17; // gray3
    case  741234907U: return 16; // gray4
    case  741234906U: return 15; // gray5
    case  741234905U: return 14; // gray6
    case  741234904U: return 13; // gray7
    case  741234903U: return 12; // gray8
    }
    return str.Atoi();
}

//---------------------------------------------------------------------------
//
// As possible convert the color col into a text string which can be
// interpreted by GetColor before setting the resource value
//
void MEnv::SetColor(const char *name, Int_t col)
{
    TString val;

    if (col>99 && col<101+kCyan)
    {
        val = "Dark ";
        col -= 100;
    }
    if (col>150 && col<151+kCyan)
    {
        val = "Light ";
        col -= 150;
    }

    switch (col)
    {
    case kWhite:   val += "White";   break;
    case kBlack:   val += "Black";   break;
    case kRed:     val += "Red";     break;
    case kGreen:   val += "Green";   break;
    case kBlue:    val += "Blue";    break;
    case kYellow:  val += "Yellow";  break;
    case kMagenta: val += "Magenta"; break;
    case kCyan:    val += "Cyan";    break;
    case 19:       val += "Grey1";   break;
    case 18:       val += "Grey2";   break;
    case 17:       val += "Grey3";   break;
    case 16:       val += "Grey4";   break;
    case 15:       val += "Grey5";   break;
    case 14:       val += "Grey6";   break;
    case 13:       val += "Grey7";   break;
    case 12:       val += "Grey8";   break;
    }

    if (val.IsNull())
        val += col;

    SetValue(name, val);
}

//---------------------------------------------------------------------------
//
// As possible convert the alignment align into a text string which can be
// interpreted by GetAlign before setting the resource value
//
void MEnv::SetAlign(const char *name, Int_t align)
{
    TString val;
    if (align==22)
    {
        SetValue(name, "Center");
        return;
    }

    switch (align%10)
    {
    case 1: val += "Left";   break;
    case 2: val += "Center"; break;
    case 3: val += "Right";  break;
    }

    switch (align/10)
    {
    case 1: val += "Bottom"; break;
    case 2: val += "Center"; break;
    case 3: val += "Top";    break;
    }

    SetValue(name, val);
}

//---------------------------------------------------------------------------
//
// As possible convert the fill style style into a text string which can be
// interpreted by GetFillStyle before setting the resource value
//
void MEnv::SetFillStyle(const char *name, Int_t style)
{
    TString val;

    if (style>3999 && style<4101)
        val = MString::Format("%d%%", style-4000);

    switch (style)
    {
    case 0:    val = "Hollow"; break;
    case 1001: val = "Solid";  break;
    case 2001: val = "Hatch";  break;
    }

    if (val.IsNull())
        val += style;

    SetValue(name, val);
}

//---------------------------------------------------------------------------
//
// As possible convert the line style style into a text string which can be
// interpreted by GetLineStyle before setting the resource value
//
void MEnv::SetLineStyle(const char *name, Int_t style)
{
    TString val;
    switch (style)
    {
    case kSolid:      val = "Solid";       break;
    case kDashed:     val = "Dashed";      break;
    case kDotted:     val = "Dotted";      break;
    case kDashDotted: val = "DashDotted";  break;
    }

    if (val.IsNull())
        val += style;

    SetValue(name, val);
}

//---------------------------------------------------------------------------
//
// As possible convert the marker style style into a text string which can be
// interpreted by GetLineStyle before setting the resource value
//
void MEnv::SetMarkerStyle(const char *name, Int_t style)
{
    TString val;
    switch (style)
    {
    case kDot:              val = "dot";
    case kPlus:             val = "plus";
    case kCircle:           val = "circle";
    case kMultiply:         val = "multiply";
    case kFullDotSmall:     val = "fulldotsmall";
    case kFullDotMedium:    val = "fulldotmedium";
    case kFullDotLarge:     val = "fulldotlarge";
//    case kOpenTriangleDown: val = "opentriangledown";
//    case kFullCross:        val = "fullcross";
    case kFullCircle:       val = "fullcircle";
    case kFullSquare:       val = "fullsquare";
    case kFullTriangleDown: val = "fulltriangledown";
    case kOpenCircle:       val = "opencircle";
    case kOpenSquare:       val = "opensquare";
    case kOpenTriangleUp:   val = "opentriangleup";
    case kOpenDiamond:      val = "opendiamond";
    case kOpenCross:        val = "opencross";
    case kFullStar:         val = "fullstar";
    case kOpenStar:         val = "openstar";
    }

    if (val.IsNull())
        val += style;

    SetValue(name, val);
}

//---------------------------------------------------------------------------
//
// Get the attributed from a TAttLine (if dftl is given use it as default)
//   name.LineColor  <see also GetColor>
//   name.LineStyle
//   name.LineWidth
// For more details on the meaning see TAttLine
//
void MEnv::GetAttLine(const char *name, TAttLine &line, TAttLine *dftl)
{
    const TString color = Compile(name, "LineColor");
    const TString style = Compile(name, "LineStyle");
    const TString width = Compile(name, "LineWidth");

    if (!dftl)
        dftl = &line;

    const Color_t col = GetColor(color, dftl->GetLineColor());
    const Style_t sty = GetLineStyle(style, dftl->GetLineStyle());
    const Style_t wid = GetValue(width, dftl->GetLineWidth());

    line.SetLineColor(col);
    line.SetLineStyle(sty);
    line.SetLineWidth(wid);
}

//---------------------------------------------------------------------------
//
// Get the attributed from a TAttText (if dftl is given use it as default)
//   name.TextColor  <see also GetColor>
//   name.TextAlign  <see also GetAlign>
//   name.TextAngle
//   name.TextFont
//   name.TextSize
// For more details on the meaning see TAttText
//
void MEnv::GetAttText(const char *name, TAttText &text, TAttText *dftl)
{
   const TString color = Compile(name, "TextColor");
   const TString align = Compile(name, "TextAlign");
   const TString angle = Compile(name, "TextAngle");
   const TString font  = Compile(name, "TextFont");
   const TString size  = Compile(name, "TextSize");

    if (!dftl)
        dftl = &text;

    const Color_t col = GetColor(color, dftl->GetTextColor());
    const Short_t ali = GetAlign(align, dftl->GetTextAlign());
    const Float_t ang = GetValue(angle, dftl->GetTextAngle());
    const Font_t  fon = GetValue(font,  dftl->GetTextFont());
    const Float_t siz = GetValue(size,  dftl->GetTextSize());

    text.SetTextColor(col);
    text.SetTextAlign(ali);
    text.SetTextAngle(ang);
    text.SetTextFont(fon);
    text.SetTextSize(siz);
}

//---------------------------------------------------------------------------
//
// Get the attributed from a TAttFill (if dftl is given use it as default)
//   name.FillColor  <see also GetColor>
//   name.FillStyle  <see also GetFillStyle>
// For more details on the meaning see TAttFill
//
void MEnv::GetAttFill(const char *name, TAttFill &fill, TAttFill *dftl)
{
    const TString color = Compile(name, "FillColor");
    const TString style = Compile(name, "FillStyle");

    if (!dftl)
        dftl = &fill;

    const Color_t col = GetColor(color, dftl->GetFillColor());
    const Style_t sty = GetFillStyle(style, dftl->GetFillStyle());
 
    fill.SetFillColor(col);
    fill.SetFillStyle(sty);
}

//---------------------------------------------------------------------------
//
// Get the attributed from a TAttMarker (if dftl is given use it as default)
//   name.MarkerColor  <see also GetColor>
//   name.MarkerStyle
//   name.MarkerSize
// For more details on the meaning see TAttMarker
//
void MEnv::GetAttMarker(const char *name, TAttMarker &marker, TAttMarker *dftl)
{
    const TString color = Compile(name, "MarkerColor");
    const TString style = Compile(name, "MarkerStyle");
    const TString size  = Compile(name, "MarkerSize");

    if (!dftl)
        dftl = &marker;

    const Color_t col = GetColor(color, dftl->GetMarkerColor());
    const Style_t sty = GetValue(style, dftl->GetMarkerStyle());
    const Size_t  siz = GetValue(size,  dftl->GetMarkerSize());

    marker.SetMarkerColor(col);
    marker.SetMarkerStyle(sty);
    marker.SetMarkerSize(siz);
}

//---------------------------------------------------------------------------
//
// Get the attributed from a TPave (if dftl is given use it as default)
//   name.CornerRadius
//   name.BorderSize
//   name.Option
// Also all resources from TAttLine and TAttFill are supported.
//
// For your conveinience: If the CornerRadius is greater than 0 "arc" is
// added to the options. If it is equal or less than 0 "arc" is removed
// from the options.
//
// For more details on the meaning see TPave
//
void MEnv::GetAttPave(const char *str, TPave &pave, TPave *dftl)
{
    const TString post(str);

    TString name(pave.GetName());
    if (!name.IsNull() && name!=pave.ClassName())
        name = Compile(name, post);

    GetAttLine(name, pave, dftl);
    GetAttFill(name, pave, dftl);

    const TString corner = Compile(name, "CornerRadius");
    const TString border = Compile(name, "BorderSize");
    const TString option = Compile(name, "Option");

    if (!dftl)
        dftl = &pave;

    const Double_t cor = GetValue(corner, dftl->GetCornerRadius());
    const Int_t    bor = GetValue(border, dftl->GetBorderSize());

    pave.SetCornerRadius(cor);
    pave.SetBorderSize(bor);

    TString  opt = GetValue(option, dftl->GetOption());
    opt.ToLower();

    const Bool_t has = pave.GetCornerRadius()>0;

    if (has && !opt.Contains("arc"))
        opt += "arc";

    if (!has && opt.Contains("arc"))
        opt.ReplaceAll("arc", "");

    pave.SetOption(opt);

}

//---------------------------------------------------------------------------
//
// Get the attributed for the TObject obj. Use dftl for default attributes
// if given.
//
// There is support for:
//    TPave      <see GetAttPave>
//    TAttLine   <see GetAttLine>
//    TAttText   <see GetAttText>
//    TAttFill   <see GetAttFill>
//    TAttMarker <see GetAttMarker>
//
void MEnv::GetAttributes(const char *name, TObject *obj, TObject *dftl)
{
    //TAttAxis         *line = dynamic_cast<TAttAxis*>(obj);
    //TAtt3D           *line = dynamic_cast<TAtt3D*>(obj);
    //TAttCanvas       *line = dynamic_cast<TAttCanvas*>(obj);
    //TAttFillCanvas   *line = dynamic_cast<TAttFillEitor*>(obj);
    //TAttLineCanvas   *line = dynamic_cast<TAttLineCanvas*>(obj);
    //TAttLineEditor   *line = dynamic_cast<TAttLineEditor*>(obj);
    //TAttMarkerCanvas *line = dynamic_cast<TAttMarkerCanvas*>(obj);
    //TAttMarkerEditor *line = dynamic_cast<TAttMarkerEditor*>(obj);
    //TAttPad          *line = dynamic_cast<TAttPad*>(obj);
    //TAttParticle     *line = dynamic_cast<TAttParticle*>(obj);
    //TAttTextCanvas   *line = dynamic_cast<TAttTextCanvas*>(obj);
    //TAttTextEditor   *line = dynamic_cast<TAttTextEditor*>(obj);

    TPave      *pave = dynamic_cast<TPave*>(obj);
    TAttLine   *line = dynamic_cast<TAttLine*>(obj);
    TAttText   *text = dynamic_cast<TAttText*>(obj);
    TAttFill   *fill = dynamic_cast<TAttFill*>(obj);
    TAttMarker *mark = dynamic_cast<TAttMarker*>(obj);

    if (pave)
    {
        GetAttPave(name, *pave, dynamic_cast<TPave*>(dftl));
        return;
    }

    if (line)
        GetAttLine(name, *line, dynamic_cast<TAttLine*>(dftl));
    if (text)
        GetAttText(name, *text, dynamic_cast<TAttText*>(dftl));
    if (fill)
        GetAttFill(name, *fill, dynamic_cast<TAttFill*>(dftl));
    if (mark)
        GetAttMarker(name, *mark, dynamic_cast<TAttMarker*>(dftl));
}

//---------------------------------------------------------------------------
//
// Set the resources from a TAttLine:
//   name.LineColor  <see also SetColor>
//   name.LineStyle
//   name.LineWidth
//
void MEnv::SetAttLine(const char *name, const TAttLine &line)
{
    const TString color = Compile(name, "LineColor");
    const TString style = Compile(name, "LineStyle");
    const TString width = Compile(name, "LineWidth");

    SetColor(color, line.GetLineColor());
    SetLineStyle(style, line.GetLineStyle());
    SetValue(width, line.GetLineWidth());
}

//---------------------------------------------------------------------------
//
// Set the resources from a TAttText:
//   name.TextColor  <see also SetColor>
//   name.TextAlign  <see also SetAlign>
//   name.TextAngle
//   name.TextFont
//   name.TextSize
//
void MEnv::SetAttText(const char *name, const TAttText &text)
{
    const TString color = Compile(name, "TextColor");
    const TString align = Compile(name, "TextAlign");
    const TString angle = Compile(name, "TextAngle");
    const TString font  = Compile(name, "TextFont");
    const TString size  = Compile(name, "TextSize");

    SetColor(color, text.GetTextColor());
    SetAlign(align, text.GetTextAlign());
    SetValue(angle, text.GetTextAngle());
    SetValue(font,  text.GetTextFont());
    SetValue(size,  text.GetTextSize());
}

//---------------------------------------------------------------------------
//
// Set the resources from a TAttFill:
//   name.FillColor  <see also SetColor>
//   name.FillStyle  <see also SetFillStyle>
//
void MEnv::SetAttFill(const char *name, const TAttFill &fill)
{
    const TString color = Compile(name, "FillColor");
    const TString style = Compile(name, "FillStyle");

    SetColor(color, fill.GetFillColor());
    SetFillStyle(style, fill.GetFillStyle());
}

//---------------------------------------------------------------------------
//
// Set the resources from a TAttMarker:
//   name.MarkerColor  <see also SetColor>
//   name.MarkerStyle
//   name.MarkerSize
//
void MEnv::SetAttMarker(const char *name, const TAttMarker &marker)
{
    const TString color = Compile(name, "MarkerColor");
    const TString style = Compile(name, "MarkerStyle");
    const TString size  = Compile(name, "MarkerSize");

    SetColor(color, marker.GetMarkerColor());
    SetMarkerStyle(style, marker.GetMarkerStyle());
    SetValue(size,  marker.GetMarkerSize());
}

//---------------------------------------------------------------------------
//
// Set the resources from a TPave:
//   name.CornerRadius
//   name.BorderSize
//   name.Option
// Also all resources from TAttLine and TAttFill are supported.
//
void MEnv::SetAttPave(const char *str, const TPave &pave)
{
    const TString name(str);

    SetAttLine(name, pave);
    SetAttFill(name, pave);

    const TString corner = Compile(name, "CornerRadius");
    const TString border = Compile(name, "BorderSize");
    const TString option = Compile(name, "Option");

    SetValue(corner, const_cast<TPave&>(pave).GetCornerRadius());
    SetValue(border, const_cast<TPave&>(pave).GetBorderSize());
    SetValue(option, pave.GetOption());
}

//---------------------------------------------------------------------------
//
// Set the attributed for the TObject obj.
//
// There is support for:
//    TPave      <see SetAttPave>
//    TAttLine   <see SetAttLine>
//    TAttText   <see SetAttText>
//    TAttFill   <see SetAttFill>
//    TAttMarker <see SetAttMarker>
//
void MEnv::SetAttributes(const char *name, const TObject *obj)
{
    const TPave      *pave = dynamic_cast<const TPave*>(obj);
    const TAttLine   *line = dynamic_cast<const TAttLine*>(obj);
    const TAttText   *text = dynamic_cast<const TAttText*>(obj);
    const TAttFill   *fill = dynamic_cast<const TAttFill*>(obj);
    const TAttMarker *mark = dynamic_cast<const TAttMarker*>(obj);

    if (pave)
    {
        SetAttPave(name, *pave);
        return;
    }

    if (line)
        SetAttLine(name, *line);
    if (text)
        SetAttText(name, *text);
    if (fill)
        SetAttFill(name, *fill);
    if (mark)
        SetAttMarker(name, *mark);
}

//---------------------------------------------------------------------------
//
// Add all values from TEnv env the this MEnv. To not overwrite existing
// values set overwrite to kFALSE
//
void MEnv::AddEnv(const TEnv &env, Bool_t overwrite)
{
    if (!GetTable() || !env.GetTable())
        return;

    TIter Next(env.GetTable());

    TEnvRec *er;
    while ((er = (TEnvRec*)Next()))
    {
        if (overwrite || !Defined(er->GetName()))
            SetValue(er->GetName(), er->GetValue(), er->GetLevel(), er->GetType());
    }
}

//---------------------------------------------------------------------------
//
// Check MArgs for all options "--rc=" and remove them. Options should be
// given like
//
//    program --rc=Option1:Test1 --rc=Option2.SubOption:Test2
//
// If all resources could be interpeted corrctly kTRUE is returned. If
// there were problems kFALSE is returned.
//
Bool_t MEnv::TakeEnv(MArgs &arg, Bool_t print, Bool_t overwrite)
{
    if (!GetTable())
    {
        gLog << err << "ERROR - MEnv not yet initialized." << endl;
        return kFALSE;
    }

    Bool_t ret = kTRUE;
    while (1)
    {
        const TString rc = arg.GetStringAndRemove("--rc=");
        if (rc.IsNull())
            break;

        const Ssiz_t pos = rc.First(':');
        if (pos<0)
        {
            gLog << warn << "WARNING - Resource '" << rc << "' doesn't contain a colon... ignored." << endl;
            ret=kFALSE;
            continue;
        }
        if (pos==0)
        {
            gLog << warn << "WARNING - Resource '" << rc << "' doesn't contain a name... ignored." << endl;
            ret=kFALSE;
            continue;
        }
        if (pos==rc.Length()-1)
        {
            gLog << warn << "WARNING - Resource '" << rc << "' empty... ignored." << endl;
            ret=kFALSE;
            continue;
        }

        const TString name = rc(0,     pos);
        const TString val  = rc(pos+1, rc.Length());

        if (print)
            gLog << all << "Command line resource '" << name << "' with value '" << val << "'...";

        const Bool_t exists = Defined(name);
        if (!exists)
        {
            SetValue(name, val, kEnvLocal);
            if (print)
                gLog << "set." << endl;
            continue;
        }

        if (overwrite)
        {
            SetValue(name, "");
            SetValue(name, val, kEnvLocal);
            if (print)
                gLog << "changed." << endl;
            continue;
        }

        if (print)
            gLog << "skipped/existing." << endl;
    }
    return ret;
}

//---------------------------------------------------------------------------
//
// Add name and full path to output
//
void MEnv::PrintEnv(EEnvLevel level) const
{
    if (!TString(GetRcName()).IsNull())
    {
        cout << "# Path: " << GetRcName() << endl;
        cout << "# File: " << gSystem->BaseName(GetRcName()) << endl;
    }
    if (!fName.IsNull())
        cout << "# Name: " << fName << endl;

    TEnv::PrintEnv(level);
}

//---------------------------------------------------------------------------
//
// Print resources which have never been touched (for debugging)
//
void MEnv::PrintUntouched() const
{
    int i=0;
    gLog << inf << flush;

    TString sep = "Untouched Resources in ";
    sep += GetRcName();
    gLog.Separator(sep);
    TIter Next(GetTable());
    TObject *o=0;

    while ((o=Next()))
        if (!fChecked.FindObject(o->GetName()))
        {
            gLog << warn << " - Resource " << o->GetName() << " untouched" << endl;
            i++;
        }
    if (i==0)
        gLog << inf << "None." << endl;
    else
        gLog << inf << i << " resources have not been touched." << endl;
}

//---------------------------------------------------------------------------
//
// Return number of resources which have not been touched.
//
Int_t MEnv::GetNumUntouched() const
{
    int i=0;
    TIter Next(GetTable());
    TObject *o=0;
    while ((o=Next()))
        if (!fChecked.FindObject(o->GetName()))
            i++;
    return i;
}
