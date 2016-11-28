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
!   Author(s): Thomas Bretz 12/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MCalendar
//
//
// Resource files
// --------------
//
// A calendar sheet is bets setup via a resource file with the
// following contants:
//
//  The day shown in the first column. (Sun=0, Mon=1, ...)
//     FirstDay: 0
//
//  The Number of blocks the calendar sheet is made of:
//     NumBlocks: 2
//
//  Set the position a graphic of the moon. The alignment is
//  coded as described in TAttText (0=off):
//     Moon.Graf.Align: 11
//
//  Set the position a text describing the moon phase. The alignment is
//  coded as described in TAttText (0=off):
//     Moon.Text.Align: 11
//
//  Set the text type: 0=MoonPhase, 1=MoonPeriod, 2=MagicPeriod
//     Moon.Text.Type:  0
//
//  Determin whether the text should be continous or only at
//  new moon (only valid if both alignments identical)
//     Moon.Text.Continous: Yes
//
//  Radius of the moon graphics in units of the pave height:
//     Moon.Graf.Radius: 0.08
//
//  Horizontal and vertical margin in units of the pave height
//  between the pave border and the text:
//     MarginX: 0.05
//
//  Space between the cells in units of the cell height:
//     CellSpace: 0.01
//
//  Space between the blocks in units of the cell height:
//     BlockSpace: 0.1
//
//  Local to determin the language to be used. For more details see
//  for example MTime::GetStringFmt or strftime (eg. de_DE, es_ES, it_IT)
//  The empty default is the system default (eg as if you call "date")
//     Language:
//
//  The file name of a file containing the setup for holidays and birthdays:
//     HolidayFile:
//
//  The setup of the pave in your calendar is done by the style defined
//  by a pave. For more details see TPave or MEnv::GetEnvPave:
//     Date.{PaveStyle}
//
//  The contents of each pave in your calendar.
//      Contents: Date Day Week Holiday Birthday
//
//  There are NO predefined content-strings. All the names given here
//  are completely artificial. You could also call them just A and B.
//  If you setup them correctly they might still behave like Date and Day.
//  This also means that you can define as many of them as you want.
//
//  You can setup the contents by their names individually (eg. Holiday).
//  The possible Style is defined by a TLatex object. For more details
//  see TLatex or MEnv::GetAttText.
//
//      To setup the position in the pave use:
//          Holiday.TextAlign: center
//          Holiday.TextAlign: center top
//          Holiday.TextAlign: right center
//          Holiday.TextAlign: right top
//
//      The text style can be setup by:
//          Holiday.TextSize:  0.13
//          Holiday.TextColor: Blue
//
//      To setup a day-dependant string, in addition to the three resources
//      above, a format string as defined in MTime::GetStringFmt or strftime.
//          Holiday.Format: %a
//
//  Contents for which a format was setup are compiled by using
//  MTime::GetStringFmt.
//  Contents without a format set (by eg. Holiday.Format) are searched in
//  the holiday resource file:
//
//      For the 1.1. and 25.12. the setup could be:
//          Holiday.01/01: New year
//          Holiday.12/25: Christmas
//
//      Holidays with respect to Easter are given as
//          Holiday.-1: Easter Saturday
//          Holiday.0: Easter
//          Holiday.1: Easter Monday
//
//  In addition to this you can overwrite the default for the active
//  and/or inactivedays by:
//     Date.Active.{PaveStyle}
//     Holiday.Active.TextColor: Blue
//     Holiday.Inactive.TextColor: Gray1
//
//  To define a special setup for only one day (Sun=0, Mon=1, ...) use:
//     Date.Active.0.{PaveStyle}
//     Holiday.Active.0.TextColor: Red
//     Holiday.Inactive.0.TextColor: White
//
//  To get a resource file describing the default layout draw a calendar
//  sheet and choose "SaveResourceFile" from the context menu. By using
//  the context menu you can change the layout and also save it to a
//  resource file. Read such a resource file using ReadResourceFile.
//
//
// Context Menu
// ------------
//
//  void SetDate(UInt_t Month, Int_t Year=-1)
//    Used to set new year and month. Use Year=-1 to keep the year
//
//  void SetLocal(const char *Locale="")
//    Set new Localization. For more details see MTime::GetStringFmt or
//    strftime
//
//  void SetFirstDay(Int_t FirstDay)
//    Define the WeekDay in the first column (Son=0, Mon=1, ...)
//
//  void SetLayout(Byte_t NumberOfBlocks, Double_t MarginX, Double_t MarginY, Double_t CellSpace, Double_t BlockSpace)
//    change the Layout. For more details see section "Resource Files"
//
//  void ResetLayout(num)
//    Remove everything from the layout. num is the number of default blocks
//    (corresponding to NumBlocks). A value smaller or equal 0 will leave
//    the number of block unchanged (which is the default from the
//    context menu)
//
//  void ResetHolidays()
//    remove all holidays.
//
//  void SetDefaultLayout(num)
//    set the layout to the default layout. num is the number of displayed
//    blocks (corresponding to NumBlocks). A value smaller or equal 0 will
//    leave the number of block unchanged (which is the default from the
//    context menu)
//
//  void SetDefaultHolidays()
//    set the default holidays
//
//  void ReadResourceFile(const char *FileName=0)
//    read layout from a resource file. (also the holidays are reread)
//
//  void ReadHolidayFile(const char *FileName=0)
//    read holidays from a file
//
//  void SaveResourceFile(const char *FileName=0)
//    save current layout to a file
//
//  void SaveHolidayFile(const char *FileName=0)
//    save current holidays to a file
//
//  void Add(const char *Name, Int_t Align, const char *Format, Float_t FontSize=0.3, Int_t FontColor=1); //*MENU
//    add contents to the pave (for more details see section
//    "resource files")
//
//  void SetDay(const char *Format="%a", Float_t FontSize=0.3, Int_t Color=kBlack)
//  void SetDate(const char *Format="%e", Float_t FontSize=0.3, Int_t Color=kBlack)  { Remove(GetAlign()); Add("Date", GetAlign(), Format, FontSize, Color); } //*MENU*
//    set the current position in the pave to a format defined by Format.
//    with FontSize and FontColor. For more details see MTime::GetStringFmt
//    or the man-page of strftime. The name of the corrsponding resources
//    is set to "Day" or "Date".
//
//  void SetHoliday(Float_t FontSize=0.13, Int_t Color=kBlue)
//  void SetBirthday(Float_t FontSize=0.13, Int_t Color=kBlue)
//  void SetEaster(Float_t FontSize=0.13, Int_t Color=kBlue)
//    set the current position in the pave to a holiday with name
//    "Holiday", "Birthday". The holidays are read from the
//    holiday resource file. For more details see section "Resource files"
//
//  void AddHoliday(const char *text)
//  void AddBirthday(const char *text)
//  void AddEaster(const char *text)
//    Add a holiday ("Holiday.mm/dd", "Birthday.mm/dd" or "Holiday.%d")
//    for the current date (for easter the offset to easter is used instead)
//    to the holiday resources.
//
//  void Remove()
//    Remove the contents from the edge you are pointing to.
//
// void PrintEnv()
//    Print the current resources.
//
// void SetEnv(const char *VariableName, const char *Value)
//    Set a new resource.
//
// void RemoveEnv(const char *VariableName)
//    Remove a resource and all subsequent resources
//
//  void SetMoonGraf(Int_t Align, Float_t Radius)
//  void SetMoonText(Int_t Align, Int_t Type, Int_t Continous)
//    change the setup of the moon. For more details see the
//    section "Resource Files"
//
//
// Format
// ------
//
// For convinience here is a copy from MTime of the most important
// format string to be used in the "Format" resource, GetStringFmt
// or DrawDate:
//
//  %a  The abbreviated weekday name according to the current locale.
//  %A  The full weekday name according to the current locale.
//  %b  The abbreviated month name according to the current locale.
//  %B  The full month name according to the current locale.
//  %d  The day of the month as a decimal number (range  01 to 31).
//  %e  Like %d, the day of the month as a decimal number,
//      but a leading zero is replaced by a space.
//  %H  The hour as a decimal number using a 24-hour clock (range 00 to 23)
//  %k  The hour (24-hour clock) as a decimal number (range 0 to 23);
//      single digits are preceded by a blank.
//  %m  The month as a decimal number (range 01 to 12).
//  %x  The preferred date representation for the current locale w/o time.
//  %y  The year as a decimal number without a century (range 00 to 99).
//  %Y  The year as a decimal number including the century.

/////////////////////////////////////////////////////////////////////////////
#include "MCalendar.h"

#include <TStyle.h>
#include <TArrow.h>
#include <TLatex.h>
#include <TMarker.h>
#include <TArrayD.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TEllipse.h>
#include <TASImage.h>
#include <TPaveText.h>
#include <THashList.h>
#include <TObjArray.h>

#include "MEnv.h"
#include "MTime.h"

ClassImp(MCalendar);

// ****************+ ROOT Bugs  *****************
// fCornerRadius not handled in TPave::Copy
// TEllipse::Paint(values) doesn't work at all
// TEnv::SetValue crashes if created with default constructor
// TEnv::Save doesn't take filename as an argument
// TEnv::fRcName no Getter
// DrawClone doesn't work for a canvas/pad containing a TASImage
// TPave::GetCornerRadius not const
// TPabe::GetBorderRadius not const
// TEllipse is transparent if FillColor=kWhite
// TLatex::PaintLatex also sets data members to arguments in
//  contradiction to all other graf-classes.
// TEnv cannot be streamed from/to a file (means: cannot be cloned!)
// Double_t TEnv::GetValue  Typo: dobule
// Support for TTF fonts in font directory in TText?
// TASImage::DrawText doesn't search TTFonts in root ttfont-path
// TTF::SetTextFont ttffont not deleted (see gSystem->Which)
// TASImage doesn't support transparency?

using namespace std;

//---------------------------------------------------------------------------
//
// Convert Umlauts and other exotic characters to TLatex readable characters
//
void MCalendar::Convert2Latex(TString &str)
{
    str.ReplaceAll("\xe4", "\\ddot{a}");
    str.ReplaceAll("\xf6", "\\ddot{o}");
    str.ReplaceAll("\xfc", "\\ddot{u}");
    str.ReplaceAll("\xc4", "\\ddot{A}");
    str.ReplaceAll("\xd6", "\\ddot{O}");
    str.ReplaceAll("\xdc", "\\ddot{U}");
    str.ReplaceAll("\xdf", "\\beta");
    str.ReplaceAll("\xe1", "\\acute{a}");
    str.ReplaceAll("\xe9", "\\acute{e}");
    str.ReplaceAll("\xec", "\\grave{i}");

    str.ReplaceAll("\xc3\xa4", "\\ddot{a}");
    str.ReplaceAll("\xc3\xb6", "\\ddot{o}");
    str.ReplaceAll("\xc3\xbc", "\\ddot{u}");
    str.ReplaceAll("\xc3\x84", "\\ddot{A}");
    str.ReplaceAll("\xc3\x96", "\\ddot{O}");
    str.ReplaceAll("\xc3\x9c", "\\ddot{U}");
    str.ReplaceAll("\xc3\x9f", "\\beta");
    str.ReplaceAll("\xc3\xa1", "\\acute{a}");
    str.ReplaceAll("\xc3\xa9", "\\acute{e}");
    str.ReplaceAll("\xc3\xac", "\\grave{i}");
}

void MCalendar::ConvertUTF8(TString &str, Bool_t fwd)
{
    if (fwd)
    {
        str.ReplaceAll("\xe4", "\xc3\xa4");
        str.ReplaceAll("\xf6", "\xc3\xb6");
        str.ReplaceAll("\xfc", "\xc3\xbc");
        str.ReplaceAll("\xc4", "\xc3\x84");
        str.ReplaceAll("\xd6", "\xc3\x96");
        str.ReplaceAll("\xdc", "\xc3\x9c");
        str.ReplaceAll("\xdf", "\xc3\x9f");
        str.ReplaceAll("\xe1", "\xc3\xa1");
        str.ReplaceAll("\xe9", "\xc3\xa9");
        str.ReplaceAll("\xec", "\xc3\xac");
    }
    else
    {
        str.ReplaceAll("\xc3\xa4", "\xe4");
        str.ReplaceAll("\xc3\xb6", "\xf6");
        str.ReplaceAll("\xc3\xbc", "\xfc");
        str.ReplaceAll("\xc3\x84", "\xc4");
        str.ReplaceAll("\xc3\x96", "\xd6");
        str.ReplaceAll("\xc3\x9c", "\xdc");
        str.ReplaceAll("\xc3\x9f", "\xdf");
        str.ReplaceAll("\xc3\xa1", "\xe1");
        str.ReplaceAll("\xc3\xa9", "\xe9");
        str.ReplaceAll("\xc3\xac", "\xec");
    }
}


//---------------------------------------------------------------------------
//
// TEnv doesn't have a streamer so we have to make sure that the
// resources get corretly cloned.
//
TObject *MCalendar::Clone(const char *newname) const
{
    MCalendar *cal = (MCalendar*)TObject::Clone(newname);

    if (cal->fEnv)
        delete cal->fEnv;
    if (cal->fEnvHolidays)
        delete cal->fEnvHolidays;

    cal->fEnv         = (MEnv*)fEnv->Clone();
    cal->fEnvHolidays = (MEnv*)fEnvHolidays->Clone();

    return cal;
}

//---------------------------------------------------------------------------
//
// return the StringFmt (see MTime::GetStringFmt or strftime)
// corresponding to the Format fmt (Default="%B", LongMonthName).
// For more complicated formatings day, hour, minutes and sec can
// be given.
//
TString MCalendar::GetStringFmt(const char *fmt, Int_t day, Int_t h, Int_t m, Int_t s) const
{
    MTime t;
    t.Set(fYear, fMonth, day, h, m, s);
    return t.GetStringFmt(fmt, GetLanguage());
}

//---------------------------------------------------------------------------
//
// Get the Pave setup for the given condition from the resources
//
void MCalendar::GetPave(TPave &pave, Bool_t active, Int_t n)
{
    // ----- Default ----
    TPave def;
    def.SetName(pave.GetName());
    def.SetLineStyle(kSolid);
    def.SetLineColor(kBlack);
    def.SetLineWidth(0);
    def.SetFillColor(18);
    def.SetFillStyle(1001);
    def.SetCornerRadius(0);
    def.SetBorderSize(1);

    fEnv->GetAttPave("", def);

    def.Copy(pave);

    pave.SetCornerRadius(def.GetCornerRadius());
    pave.SetLineColor(active ? kBlack : 15);

    const char *fmt1 = active ? "Active"    : "Inactive";
    const char *fmt2 = active ? "Active.%d" : "Inactive.%d";

    fEnv->GetAttributes(     fmt1,     &pave);
    fEnv->GetAttributes(Form(fmt2, n), &pave);
}

//---------------------------------------------------------------------------
//
// Get the Moon setup for the given condition from the resources
//
void MCalendar::GetMoon(TAttText &text, TAttFill &fill, Bool_t active, Int_t n)
{
    TAttText txt(11, 0, gStyle->GetTextColor(), gStyle->GetTextFont(), 0.13);
    TAttFill fil(0, 1001);

    fEnv->GetAttText("Moon", txt);
    fEnv->GetAttFill("Moon", fil);

    txt.Copy(text);
    fil.Copy(fill);

    text.SetTextColor(active ? kRed   : 15);
    fill.SetFillColor(active ? kBlack : 15);

    const char *fmt1 = active ? "Moon.Active"    : "Moon.Inactive";
    const char *fmt2 = active ? "Moon.Active.%d" : "Moon.Inactive.%d";

    fEnv->GetAttText(fmt1, text);
    fEnv->GetAttFill(fmt1, fill);

    fEnv->GetAttText(Form(fmt2, n), text);
    fEnv->GetAttFill(Form(fmt2, n), fill);
}

//---------------------------------------------------------------------------
//
// Get the Text setup for the given condition from the resources
//
void MCalendar::GetLatex(TLatex &latex, Bool_t active, Int_t n)
{
    const TString fmt = fEnv->GetValue(Form("%s.Format", latex.GetName()), "");
    latex.SetTitle(fmt);

    TAttText(0, 0, kBlack, gStyle->GetTextFont(), 0.3).Copy(latex);
    fEnv->GetAttText(latex.GetName(), latex);

    if (!active)
        latex.SetTextColor(15);

    const char *fmt1 = active ? "%s.Active"    : "%s.Inactive";
    const char *fmt2 = active ? "%s.Active.%d" : "%s.Inactive.%d";

    fEnv->GetAttText(Form(fmt1, latex.GetName()),    latex);
    fEnv->GetAttText(Form(fmt2, latex.GetName(), n), latex);
}

//---------------------------------------------------------------------------
//
// Add a text. The name determins the identifier for which the resources
// are searched, align the position in the pave (the convention is the same
// as in TAttText). If a format fmt is given the date corresponding to the
// pave is converted to a string using MTime::GetStringFmt. See MTime
// for more details. If the format is empty the Holiday resources are
// searched for an entry with the name. size defines the default height
// of the text in units of the pave-height and color the default color in
// case of active days.
//  eg.
//         Add("Day", 13, "%a", 0.3, kRed);
// would produce a short WeekDay-name (%a) at the top left edge with
// with a default height of 0.3*pave-height in Red. You can change
// the behaviour from the resource file by:
//         Day.{TextAttribute}
//         Day.Active.{TextAttribute}
//         Day.Inctive.{TextAttribute}
//         Day.Active.i.{TextAttribute}
//         Day.Inctive.i.{TextAttribute}
// for more details about TextAttribute see MEnv::GetAttText and TAttText.
// i is a placeholder for the WeekDay number (starting with sunday=0)
//
void MCalendar::Add(const char *name, Int_t align, const char *fmt, Float_t size, Int_t col)
{
    TAttText att(align, 0, col, gStyle->GetTextFont(), size);
    Add(name, fmt, att);
}

void MCalendar::Add(const char *name, const char *fmt, const TAttText &att)
{
    fEnv->SetAttText(name, att);
    fEnv->SetValue(Form("%s.Format", name), fmt);

    const TString cont = Form(" %s ", fEnv->GetValue("Contents", ""));
    const TString form = Form(" %s ", name);
    if (!cont.Contains(form))
        fEnv->SetValue("Contents", Form("%s %s", cont.Strip(TString::kBoth).Data(), name));
}

//---------------------------------------------------------------------------
//
// Save the current layout to a resource file. If no filename is given
// the filename of the current open resource file is used.
//
void MCalendar::SaveResourceFile(const char *fname)
{
    const TString name = fname ? fname : fEnv->GetName();

    gSystem->Unlink(name);

    MEnv env(name);

    env.SetValue("FirstDay",             fFirstDay);
    env.SetValue("NumBlocks",            fNumBlocks);

    env.SetValue("Orientation",          fOrientation);
    env.SetValue("Rotation",             fRotation);

    env.SetValue("Moon.Graf.Align",      fMoonAlignGraf);
    env.SetValue("Moon.Text.Align",      fMoonAlignText);
    env.SetValue("Moon.Text.Type",       fMoonTextType);
    env.SetValue("Moon.Text.Continous",  fMoonTextCont);
    env.SetValue("Moon.Graf.Radius",     fMoonRadius);

    env.SetValue("MarginX",              fMarginX);
    env.SetValue("MarginY",              fMarginY);
    env.SetValue("CellSpace",            fCellSpace);
    env.SetValue("BlockSpace",           fBlockSpace);

    env.SetValue("Language",             GetLanguage());

    env.SetValue("HolidayFile",          fEnvHolidays->GetName());

    env.AddEnv(*fEnv);
    env.Save();
}

//---------------------------------------------------------------------------
//
// Save the current holidays to a resource file. If no filename is given
// the filename of the current open holiday file is used.
//
void MCalendar::SaveHolidayFile(const char *fname)
{
    const TString name = fname ? fname : fEnvHolidays->GetName();

    gSystem->Unlink(name);

    MEnv env(name);
    env.AddEnv(*fEnvHolidays);
    env.Save();
}

//---------------------------------------------------------------------------
//
// The Layout will be resetted and than restored from the specified
// resource file. If no filename is given the filename of the current
// open resource file is used.
//
void MCalendar::ReadResourceFile(const char *fname)
{
    const TString name = fname ? fname : fEnv->GetName();

    ResetLayout(2);

    delete fEnv;
    fEnv = new MEnv(name);

    fFirstDay      = fEnv->GetValue("FirstDay",             fFirstDay);
    fNumBlocks     = fEnv->GetValue("NumBlocks",            fNumBlocks);

    fOrientation   = fEnv->GetValue("Orientation",          fOrientation);
    fRotation      = fEnv->GetValue("Rotation",             fOrientation);

    fMoonAlignGraf = fEnv->GetValue("Moon.Graf.Align",      fMoonAlignGraf);
    fMoonAlignText = fEnv->GetValue("Moon.Text.Align",      fMoonAlignText);
    fMoonTextType  = fEnv->GetValue("Moon.Text.Type",       fMoonTextType);
    fMoonTextCont  = fEnv->GetValue("Moon.Text.Continous",  fMoonTextCont);
    fMoonRadius    = fEnv->GetValue("Moon.Graf.Radius",     fMoonRadius);

    fMarginX       = fEnv->GetValue("MarginX",              fMarginX);
    fMarginY       = fEnv->GetValue("MarginY",              fMarginY);
    fCellSpace     = fEnv->GetValue("CellSpace",            fCellSpace);
    fBlockSpace    = fEnv->GetValue("BlockSpace",           fBlockSpace);

    SetLocal(fEnv->GetValue("Language", ""));
    ReadHolidayFile();
}

//---------------------------------------------------------------------------
//
// The Holidays will be resetted and than restored from the specified
// holiday file. If no filename is given the filename of the current
// open holiday file is used.
//
void MCalendar::ReadHolidayFile(const char *fname)
{
    const TString name = fname ? fname : fEnvHolidays->GetName();

    if (fEnvHolidays)
        delete fEnvHolidays;
    fEnvHolidays = new MEnv(name);
}

//---------------------------------------------------------------------------
//
// Reset the layout (remove everything)
//
// num is the number of default blocks (corresponding to NumBlocks).
// A value smaller or equal 0 will leave the number of block unchanged
// (which is the default)
//
void MCalendar::ResetLayout(Int_t num)
{
    fOrientation   = kFALSE;
    fRotation      = kFALSE;

    fFirstDay      = 1;

    if (num>0)
        fNumBlocks = num;

    fMoonAlignGraf = 0;
    fMoonAlignText = 0;
    fMoonTextType  = 0;
    fMoonTextCont  = kFALSE;
    fMoonRadius    = 0.08;

    fMarginX       = 0.05;
    fMarginY       = 0.05;

    fCellSpace     = 0.01;
    fBlockSpace    = 0.1;

    if (fEnv)
        delete fEnv;

    fEnv = new MEnv("/dev/null");
}

//---------------------------------------------------------------------------
//
// Reset the holidays (remove everything)
//
void MCalendar::ResetHolidays()
{
    if (fEnvHolidays)
        delete fEnvHolidays;

    fEnvHolidays = new MEnv("/dev/null");
}

//---------------------------------------------------------------------------
//
// Set the holidays to the default holidays
//
void MCalendar::SetDefaultHolidays()
{
    ResetHolidays();

    fEnvHolidays->SetValue("Holiday.0",     "Easter");
    fEnvHolidays->SetValue("Holiday.01/01", "New Year");
    fEnvHolidays->SetValue("Holiday.12/25", "Christmas");
    fEnvHolidays->SetValue("Holiday.12/26", "Christmas");
}

//---------------------------------------------------------------------------
//
// Set the layout to the default layout
//
// num is the number of default blocks (corresponding to NumBlocks).
// A value smaller or equal 0 will leave the number of block unchanged
// (which is the default)
//
void MCalendar::SetDefaultLayout(Int_t num)
{
    ResetLayout(num);

    fMoonAlignGraf = 11;
    fMoonAlignText = 11;

    fEnv->SetValue("Contents", "Day Date Week Holiday Birthday");

    fEnv->SetValue("Day.TextAlign",           "top left");
    fEnv->SetValue("Day.Format",              "%e");

    fEnv->SetValue("Date.TextAlign",          "bottom right");
    fEnv->SetValue("Date.Format",             "%a");

    fEnv->SetValue("Week.TextAlign",          "top right");
    fEnv->SetValue("Week.Format",             "KW%V");
    fEnv->SetValue("Week.TextSize",           0.1);
    fEnv->SetValue("Week.Restriction",        1);
    fEnv->SetValue("Week.Active.TextColor",   "Grey8");

    fEnv->SetValue("Holiday.TextAlign",       "center");
    fEnv->SetValue("Holiday.TextSize",        0.13);
    fEnv->SetValue("Holiday.TextColor",       kBlue);

    fEnv->SetValue("Birthday.TextAlign",      "center");
    fEnv->SetValue("Birthday.TextSize",       0.13);
    fEnv->SetValue("Birthday.TextColor",      kBlue);

    //    fEnv->SetValue("Easter.TextAlign",        "center");
    //    fEnv->SetValue("Easter.TextSize",         0.13);
    //    fEnv->SetValue("Easter.TextColor",        kBlue);

    fEnv->SetValue("Date.Active.0.TextColor", kRed);

    fEnv->SetValue("Date.Active.TextFont",     22);
    fEnv->SetValue("Day.Active.TextFont",      22);
    fEnv->SetValue("Date.Inactive.TextFont",  132);
    fEnv->SetValue("Day.Inactive.TextFont",   132);

    // fEnv->SetValue("Date.Inactive.LineStyle", kDashed);
    // fEnv->SetValue("Date.Inactive.FillColor", kWhite);
}

//---------------------------------------------------------------------------
//
// Create a calendar piece for the given year and month. If a filename
// is given the corresponding resource file is read. If no month and/or
// no year is given the current date is used instead.
//
MCalendar::MCalendar(UShort_t y, Byte_t m, const char *fname)
   : fYear(y==0?MTime(-1).Year():y), fMonth(m==0?MTime(-1).Month():m)
{
    fEnv        =0;
    fEnvHolidays=0;

    //fUpdate = kFALSE;

    SetDefaultLayout(2);
    SetDefaultHolidays();

    if (fname)
        ReadResourceFile(fname);
}

//---------------------------------------------------------------------------
//
// Delete the two resource files
//
MCalendar::~MCalendar()
{
    delete fEnv;
    delete fEnvHolidays;
}

//---------------------------------------------------------------------------
//
// Take the pave edges and the margin and calculate the new x and y
// coordinates from it for the given alignment (as in TAttText),
//
TArrayD MCalendar::ConvertAlign(Int_t align, Double_t x[2], Double_t y[2], Double_t m[2]) const
{
    align = Rotate(align);

    TArrayD p(2);
    switch (align/10)
    {
    case 1: p[0] = x[0]+m[0];     break;
    case 2: p[0] = (x[0]+x[1])/2; break;
    case 3: p[0] = x[1]-m[0];     break;
    }
    switch (align%10)
    {
    case 1: p[1] = y[0]+m[1];     break;
    case 2: p[1] = (y[0]+y[1])/2; break;
    case 3: p[1] = y[1]-m[1];     break;
    }
    return p;
}

//---------------------------------------------------------------------------
//
// Rotate the aligment if the orientation is changed.
//
Int_t MCalendar::Rotate(Int_t align, Bool_t fwd) const
{
    if (fOrientation==fRotation)
        return align;

    static const Int_t b[4] = { 11, 31, 33, 13 };
    static const Int_t a[4] = { 11, 13, 33, 31 };

    const Int_t *c = fwd ? a :b;

    for (int i=0; i<4; i++)
        if (align==c[i])
            return c[(i+1)%4];

    return align;
}

//---------------------------------------------------------------------------
//
// Paint the text str at the position indirectly given by alignement, x and
// y (see ConvertAlign for more details) with the attributes from TAttText.
//
void MCalendar::PaintLatex(TAttText &att, Int_t align, Double_t x[2], Double_t y[2], Double_t ratio[2], /*Double_t height,*/ TString str)
{
    TLatex tex;
    att.Copy(tex);

    Double_t m[2] = {
        fMarginX*ratio[0],
        fMarginY*ratio[1] 
    };

    const TArrayD p = ConvertAlign(align, x, y, m);
     /*
    tex.SetText(p[0], p[1], str);
    tex.SetTextAngle(0);
    tex.SetTextSize(att.GetTextSize()*height);

    while (1)
    {
        Double_t w = tex.GetXsize();
        Double_t h = tex.GetYsize();
        if (w<width-2*m[0] && h<height-2*m[1])
            break;

        tex.SetTextSize(tex.GetTextSize()*0.99);
    }

    tex.Paint();
    */

    const Double_t scale = fOrientation            ? x[1]-x[0] : y[1]-y[0];
    const Double_t phi   = fOrientation!=fRotation ? -90 : 0;

    Convert2Latex(str);

    tex.SetTextAlign(align);
    tex.PaintLatex(p[0], p[1], phi, att.GetTextSize()*scale, str);
}

//---------------------------------------------------------------------------
//
// Paint a clock as a symbol for summer-/wintertime
//
void MCalendar::PaintClock(Double_t x[2], Double_t y[2], Double_t r[2], Int_t fill, char dir)
{
    Double_t k[2] = { (x[0]+x[1])/2, (y[0]+y[1])/2 };

    TEllipse e(k[0], k[1], r[0]*2.5, r[1]*2.5, 30, 360, 0);
    e.SetLineColor(kBlack);
    e.SetLineStyle(kSolid);
    e.SetFillColor(fill);
    e.Paint();

    TMarker m;
    m.SetMarkerStyle(kFullDotSmall);
    for (int i=2; i<12; i++)
        m.PaintMarker(k[0]+r[0]*2*cos(TMath::TwoPi()*i/12),
                      k[1]+r[1]*2*sin(TMath::TwoPi()*i/12));

    TArrow a(k[0]+r[0]*3, k[1], k[0]+r[0]*2.7, k[1]+r[1]*1.5, r[1]/2);
    a.Paint(dir=='-'?"->":"<-");
}

//---------------------------------------------------------------------------
//
// Check the holiday resources for a holiday matching the the resource name:
//   name.mm/dd
// while name is the name of the TObject, mm and dd are month and day
// of the MTime.
//
TString MCalendar::GetHoliday(const TObject &o, const MTime &tm)
{
    const Int_t easter = (Int_t)MTime::GetEaster(fYear).GetMjd();

    const TString fmt = o.GetTitle();

    if (!fmt.IsNull())
        return tm.GetStringFmt(fmt, GetLanguage());

    TString env(o.GetName());

    const TString post1 = Form(".%d", (Int_t)tm.GetMjd()-easter);
    const TString post2 = Form(".%02d/%02d", tm.Month(), tm.Day());

    TString rc;
    for (int i=0; i<7; i++)
    {
        MTime tx(tm.GetMjd()+i);
        const TString post3 = Form(".%02d/%02d-%d", tx.Month(), tx.Day(), (7-tm.WeekDay())%7);
        rc = fEnvHolidays->GetValue(env+post3, rc);
    }

    rc = fEnvHolidays->GetValue(env+post1, rc);
    rc = fEnvHolidays->GetValue(env+post2, rc);
/*
    if (fLanguage=="de_DE")
    {
        MTime t;
        t.Set(fYear, 5, 14);
        t.SetMjd(t.GetMjd()-t.WeekDay());

        if ((Int_t)t.GetMjd() == easter + 49)
            t.SetMjd(t.GetMjd()-7);

        if ((Int_t)tm.GetMjd()==(Int_t)t.GetMjd())
            return "Muttertag";
    }

  */
    return rc;
}

void MCalendar::Paint(Option_t *opt)
{
    /*
    if (fUpdate)
    {
        fEnv->SetAttLine(fActive?"Date.Active":"Date.Inactive", *this);
        fUpdate = kFALSE;
    }
    */

    // How should this be done?
    //fOrientation = gPad->PixeltoX(1)>-gPad->PixeltoY(1);

    MTime t;
    t.Set(fYear, fMonth, 1);

    Int_t n = t.WeekDay();

    Int_t diff = (7-fFirstDay)%7;
    n += diff;
    n %= 7;

    t.SetMjd(t.GetMjd()-n);
    n = 0;

    for (; n<99; n++)
    {
        const MTime tm(t.GetMjd()+n);

        const Bool_t active = tm.Month()==(UInt_t)fMonth;

        Double_t x[2], y[2], ratio[2];
        const Bool_t rc =  GetBox(n, x, y, ratio);

        if (!active && rc)
            break;

        // Get part of string for this day
        const Int_t day = (n+7-diff)%7;

        TPave pave;
        pave.SetName("Date");
        GetPave(pave, active, day);

        // ---------- Paint Border and fill area ----------
        pave.SetX1NDC(x[0]);
        pave.SetY1NDC(y[0]);
        pave.SetX2NDC(x[1]);
        pave.SetY2NDC(y[1]);
        pave.Paint(pave.GetOption());

        Double_t r[2] = {
            fMoonRadius*ratio[0],
            fMoonRadius*ratio[1]
        };

        // ---------- Paint Text ----------
        const TString contents = fEnv->GetValue("Contents", "");
        TObjArray *arr = contents.Tokenize(" ");

        TIter Next(arr);
        TObject *o=0;
        while ((o=Next()))
        {
            TLatex latex;
            latex.SetName(o->GetName());

            const TString res = fEnv->GetValue(Form("%s.Restriction", o->GetName()), "0123456");
            if (res.First('0'+tm.WeekDay())<0)
                continue;

            GetLatex(latex, active, day);

            const TString text = GetHoliday(latex, tm);

            if (text=="T+" || text=="T-")
            {
                PaintClock(x, y, r, pave.GetFillColor(), text[1]);
                continue;
            }

            if (!text.IsNull())
                PaintLatex(latex, latex.GetTextAlign(), x, y, ratio, text);
        }

        delete arr;

        // ---------- Paint Moon Phase -----------
        //Double_t r[2] = {
        //    fMoonRadius*ratio[0],
        //    fMoonRadius*ratio[1]
        //};
        Double_t m[2] = {
            fMarginX*ratio[0] + r[0],
            fMarginY*ratio[1] + r[1]
        };

        TAttText text;
        TAttFill fill;
        GetMoon(text, fill, active, day);

        // Shift if grafic and moon should be displayed at the same
        // location, the text is a discrete number and it is
        // switched to continous.
        const Bool_t  rot  = fOrientation!=fRotation;
        const Float_t phi  = rot ? -90 : 0;
        const Int_t   dx   = rot;
        const Int_t   dy   = (rot+1)%2;
        const Bool_t  cont = fMoonTextType==0 || fMoonTextCont;
        if (fMoonAlignGraf)
        {
            TArrayD p = ConvertAlign(fMoonAlignGraf, x, y, m);

            if (fMoonAlignGraf==fMoonAlignText && cont && fMoonAlignGraf==22)
                p[dy] += rot ? -m[dy]/2 : m[dy]/2;

            TEllipse e(p[0], p[1], r[0], r[1], -90+phi, 90+phi, 0);
            fill.Copy(e);
            e.SetLineStyle(0);
            e.Paint();
            if (tm.GetMoonPhase()<0.5)
            {
                e.SetLineStyle(kSolid);
                e.SetLineColor(pave.GetFillColor());
                e.SetFillColor(pave.GetFillColor());
            }
            else
            {
                e.SetPhimin(90+phi);  // bug in PaintEllipse!
                e.SetPhimax(270+phi); // bug in PaintEllipse!
            }
            const Double_t R  = tm.GetMoonPhase()<0.5 ? 1-tm.GetMoonPhase() : tm.GetMoonPhase();
            const Double_t r0 = 1.0-TMath::Sqrt(2.0-2.0*R);
            if (rot)
                e.SetR2(r[1]*r0);  // bug in PaintEllipse!
            else
                e.SetR1(r[0]*r0);  // bug in PaintEllipse!
            e.Paint();
        }

        if (fMoonAlignGraf==fMoonAlignText && !cont && tm.GetMoonPhase()>0.004)
            continue;

        // ---------- Paint Moon Text -----------
        if (fMoonAlignText)
        {
            if (fMoonAlignGraf!=fMoonAlignText || (fMoonAlignGraf==fMoonAlignText && !cont))
                m[dx] -= r[dx];

            TArrayD p = ConvertAlign(fMoonAlignText, x, y, m);

            if (fMoonAlignGraf==fMoonAlignText && cont)
            {
                if (rot)
                {
                    m[dx] = -m[dx];
                    m[dy] = -m[dy];
                }

                switch (fMoonAlignGraf/10)
                {
                case 1: p[dx] += m[dx];                                          break;
                case 2: p[dy] += fMoonAlignGraf%10==1 ? m[dy]*1.25 : -m[dy]*1.25; break;
                case 3: p[dx] -= m[dx];                                          break;
                }
            }

            TString num;
            switch (fMoonTextType)
            {
            case 0:// kMoonPhase:
                num = Form("%d%%", TMath::Nint(tm.GetMoonPhase()*100));
                break;
            case 1://kMoonPeriod:
                num = Form("%d", (Int_t)tm.GetMoonPeriod());
                break;
            case 2://kMagicPeriod:
                num = Form("%d", tm.GetMagicPeriod());
                break;
            }

            const Double_t scale = fOrientation ? x[1]-x[0] : y[1]-y[0];

            TLatex tex;
            text.Copy(tex);
            tex.SetTextAlign(10*(fMoonAlignText/10)+2);  // 12
            tex.PaintLatex(p[0], p[1], phi, text.GetTextSize()*scale, num);
        }
    }

    /*
     TASImage img;
     img.FromPad(gPad);
     img.Flip(90);
     img.Paint();
     */
}

Bool_t MCalendar::GetBox(Int_t n, Double_t x[2], Double_t y[2], Double_t *ratio)
{
    //const Int_t   maxrows = 6; //28/(7*fNumBlocks) + 1;
    const Float_t addrows = 0;

    Int_t maxrows;
    switch(fNumBlocks)
    {
    case 0:
    case 1: maxrows=6; break;
    case 2: maxrows=3; break;
    case 3: maxrows=2; break;
    case 4: maxrows=2; break;
    default: maxrows=1; break;
    }

    const Float_t ratio0  = -gPad->PixeltoX(1)/gPad->PixeltoY(1);

    // Cellspace
    const Float_t  celly  = fCellSpace;
    const Float_t  cellx  = fOrientation ? fCellSpace/ratio0 : fCellSpace*ratio0;

    // Blockspace
    //Float_t blockspacey = fBlockSpace;
    const Float_t  blockx = fOrientation ? fBlockSpace/ratio0 : fBlockSpace*ratio0;

    const Float_t  w      = (0.99-cellx*(7*fNumBlocks-1)-(fNumBlocks-1)*blockx)/(fNumBlocks*7);
    const Float_t  h      = (0.99-celly*(maxrows-1)/*-blockspacey*/)/(maxrows+addrows);


    const Int_t    ix     = n%(7*fNumBlocks);
    const Int_t    iy     = n/(7*fNumBlocks);

    const Int_t    col    = ix/7;

    const Double_t p0     = 0.005 - col*(cellx - blockx);

    TArrayD a(2), b(2);

    a[0] =  p0   + ix*(cellx + w);
    a[1] =  p0   + ix*(cellx + w) + w;

    b[0] =  0.995 - iy*(celly + h) - h;
    b[1] =  0.995 - iy*(celly + h);

    if (ratio)
    {
        ratio[0] = ratio0>1 ? h : h*ratio0;
        ratio[1] = ratio0>1 ? h/ratio0 : h;
    }

    if (fOrientation)
    {
        x[0] = b[0];
        x[1] = b[1];
        y[0] = a[0];
        y[1] = a[1];
    }
    else
    {
        x[0] = a[0];
        x[1] = a[1];
        y[0] = b[0];
        y[1] = b[1];
    }

    if (fOrientation)
    {
        TArrayD d(2,y);
        y[0] = 1-d[1];
        y[1] = 1-d[0];
    }

    if (fRotation)
    {
        TArrayD d(2,x);
        x[0] = 1-d[1];
        x[1] = 1-d[0];
    }

    return ix==0 && iy>0;
}

//---------------------------------------------------------------------------
//
// Determine the absolute number of the day from the event (mouse)
// position.
//
Int_t MCalendar::GetN(Double_t x[2], Double_t y[2])
{
    if (!gPad)
        return 0;

    const Double_t px = gPad->AbsPixeltoX(gPad->GetEventX());
    const Double_t py = gPad->AbsPixeltoY(gPad->GetEventY());

    int n=0;
    for (n=0; n<99; n++)
    {
        GetBox(n, x, y);
        if (x[0]<px && px<x[1] && y[0]<py && py<y[1])
           break;
    }

    return n==99 ? -1 : n;
}

//---------------------------------------------------------------------------
//
// Determine the date of the day from the event (mouse) position.
//
MTime MCalendar::GetDate()
{
    const Int_t n = GetN();
    if (n<0)
        return MTime();

    MTime t;
    t.Set(fYear, fMonth, 1);

    Int_t k = t.WeekDay();

    k += (7-fFirstDay)%7;
    k %= 7;

    return MTime(t.GetMjd()-k+n);
}

//---------------------------------------------------------------------------
//
// Determin the alignment of the event (mouse) position.
//
Int_t MCalendar::GetAlign()
{
    Double_t x[2], y[2];
    if (GetN(x, y)<0)
        return 0;

    const Double_t px = gPad->AbsPixeltoX(gPad->GetEventX());
    const Double_t py = gPad->AbsPixeltoY(gPad->GetEventY());

    const Int_t alignx = (Int_t)(3*(px-x[0])/(x[1]-x[0]))+1;
    const Int_t aligny = (Int_t)(3*(py-y[0])/(y[1]-y[0]))+1;

    return Derotate(alignx*10+aligny);
}

//---------------------------------------------------------------------------
//
// Remove the text entry at the event (mouse) position.
//
void MCalendar::Remove(Int_t align)
{
    TObjArray *arr = TString(fEnv->GetValue("Contents", "")).Tokenize(" ");

    TIter Next(arr);
    TObject *o=0;
    while ((o=Next()))
    {
        TAttText att;
        fEnv->GetAttText(o->GetName(), att);

        if (att.GetTextAlign()!=align)
            continue;

        TString cont = Form(" %s ", fEnv->GetValue("Contents", ""));
        cont.ReplaceAll(Form(" %s ", o->GetName()), " ");
        fEnv->SetValue("Contents", cont.Strip(TString::kBoth).Data());

        RemoveEnv(o->GetName());
    }

    delete arr;
}

Int_t MCalendar::DistancetoPrimitive(Int_t x, Int_t y)
{
    const Double_t px = gPad->AbsPixeltoX(x);
    const Double_t py = gPad->AbsPixeltoY(y);

    return px>0.01 && px<0.99 && py>0.01 && py<0.99 ? 0 : 99999;
}

//---------------------------------------------------------------------------
//
// Add a holiday named "text" at the event (mouse) position.
//
void MCalendar::AddHoliday(const char *text)
{
    MTime t = GetDate();
    if (!t)
        return;

    const TString str = Form("Holiday.%02d/%02d", t.Month(), t.Day());

    fEnvHolidays->SetValue(str, text);
}

//---------------------------------------------------------------------------
//
// Add a birthday named "text" at the event (mouse) position.
//
void MCalendar::AddBirthday(const char *text)
{
    MTime t = GetDate();
    if (!t)
        return;

    const TString str = Form("Birthday.%02d/%02d", t.Month(), t.Day());

    fEnvHolidays->SetValue(str, text);
}

//---------------------------------------------------------------------------
//
// Add a easterday named "text" at the event (mouse) position. The special
// thing is that the holiday is relative to easter and will shift with
// easter for other years.
//
void MCalendar::AddEaster(const char *text)
{
    MTime t = GetDate();
    if (!t)
        return;

    const Int_t easter = (Int_t)MTime::GetEaster(fYear).GetMjd();

    const TString str = Form("Holiday.%d", (Int_t)t.GetMjd()-easter);

    fEnvHolidays->SetValue(str, text);
}

//---------------------------------------------------------------------------
//
// Print the current resources.
//
void MCalendar::PrintEnv() const
{
    fEnv->Print();
}

//---------------------------------------------------------------------------
//
// Set a new resource.
//
void MCalendar::SetEnv(const char *VariableName, const char *Value) const
{
    fEnv->SetValue(VariableName, Value);
}

//---------------------------------------------------------------------------
//
// Remove a resource and all subsequent resources
//
void MCalendar::RemoveEnv(const char *name) const
{
    const TString n1 = name;
    const TString n2 = Form("%s.", name);

    TCollection *table = fEnv->GetTable();

    TIter Next(table);
    TObject *o=0;
    while ((o=Next()))
        if (n1==o->GetName() || TString(o->GetName()).BeginsWith(n2))
            delete table->Remove(o);
}

//---------------------------------------------------------------------------
//
// Create a pad with the given coordinates, Draw a clone of the image
// into it and resize the pad to the image properties such that it is
// centered in the original area.
//
// DrawImage cd()'s to the new pad.
//
TASImage *MCalendar::DrawImage(const TASImage &img, Float_t x1, Float_t y1, Float_t x2, Float_t y2, Bool_t rot)
{
    TPad *pad=new TPad("Img", "Image", x1, y1, x2, y2);
    pad->SetBorderMode(0);
    pad->SetFillColor(gPad?gPad->GetFillColor():kWhite);
    pad->SetBit(kCanDelete);
    pad->SetTopMargin(0);
    pad->SetRightMargin(0);
    pad->SetLeftMargin(0);
    pad->SetBottomMargin(0);
    pad->Draw();

    gROOT->SetSelectedPad(0);

    pad->cd();

    TASImage *clone = (TASImage*)img.DrawClone();
    clone->SetBit(kCanDelete);

    pad->Modified();
    pad->Update();

    const Int_t wi = clone->GetScaledWidth();
    const Int_t hi = clone->GetScaledHeight();

    TCanvas *c = pad->GetCanvas();

    const Float_t x = (x1+x2)/2;
    const Float_t y = (y1+y2)/2;

    Float_t w = TMath::Abs(x2-x1)/2;
    Float_t h = TMath::Abs(y2-y1)/2;

    const Float_t ri = TMath::Abs(c->PixeltoX(wi)/c->PixeltoY(hi));
    const Float_t rp = TMath::Abs(w/h);

    if (ri>rp)
        h = w/ri;
    else
        w = h*ri;

    pad->SetPad(x-w, y-h, x+w, y+h);
    pad->SetFixedAspectRatio();

    return clone;
}

//---------------------------------------------------------------------------
//
// Small helper for GetImage: Allocate a new TASImage. If it is valid
// return the pointer, if not delete the object and return NULL.
//
TASImage *MCalendar::ReturnFile(const char *file) const
{
    TASImage *img = new TASImage(file);
    if (img->IsValid())
        return img;

    delete img;
    return NULL;
}

//---------------------------------------------------------------------------
//
// Get a TASImage from a TEnv (if no TEnv is given as argument the
// default resource file os used). The TEnv is searched for the following
// tags in the following order:
//
//   %B:    The month name as defined by the local (try GetStringFmt("%B"))
//   %b:    The abbreviation of the month name as defined by the local (try GetStringFmt("%b"))
//   %02d:  The number of the month with leading zeros.
//
// If no tag could be found or the image read is invalid NULL is returned.
// Otherwise a newly allocated TASImage is returned (the used is responsible
// of deleting it)
//
TASImage *MCalendar::GetImage(TEnv *env, const char *pwd)
{
    if (!env)
        env = fEnv;

    TString fStrMonth  = GetStringFmt("%B");
    TString fStrMonth3 = GetStringFmt("%b");

    ConvertUTF8(fStrMonth3, kFALSE);
    ConvertUTF8(fStrMonth,  kFALSE);

    const TString path(pwd);

    TString file = env->GetValue(fStrMonth, "");
    if (!file.Strip(TString::kBoth).IsNull())
        return ReturnFile(path+file);

    file = env->GetValue(fStrMonth3, "");
    if (!file.Strip(TString::kBoth).IsNull())
        return ReturnFile(path+file);

    file = env->GetValue(Form("%02d", fMonth), "");
    if (!file.Strip(TString::kBoth).IsNull())
        return ReturnFile(path+file);

    return NULL;
}

//---------------------------------------------------------------------------
//
// Draw a Latex (after conversion with Convert2Latex, to support Umlauts,
// etc) text text at the coordinates x and y with the attributes att.
//
// To use a date format string (see GetStringFmt()) instead of a plain
// text use DrawDate instead.
//
void MCalendar::DrawLatex(Float_t x, Float_t y, const char *text, const TAttText &att)
{
    TString str(text);
    Convert2Latex(str);

    TLatex *latex = new TLatex(x, y, text);
    att.Copy(*latex);
    latex->SetBit(kCanDelete);
    latex->Draw();
}

const char *MCalendar::GetTTFontPath(const char *file) const
{
    const TString p1 = gEnv->GetValue("Root.TTFontPath", "");
    const TString p2 = "/usr/X11R6/lib/X11/fonts/truetype";
   // const TString p3 = path;

    TString n(file);

    if (n.First('.')<0 && n.First('/')<0)
        n.Append(".ttf");

    return gSystem->Which(p1+":"+p2/*+":"+p3*/, n, kReadPermission);
}

//---------------------------------------------------------------------------
//
// Draw a Text text into your image. For more details see TASImage::DrawText.
//
// This is a wrapper to simplify access to TrueType Fonts.
//
// You can just skip the path and the extension, eg "comic" is enough for
// the TTF "comic.ttf".
//
// The search path is "*.*.Root.TTFontPath" from gEnv (.rootrc),
// "/usr/X11R6/lib/X11/fonts/truetype" and path.
//
// For available fonts see eg. $ROOTSYS/fonts
//
void MCalendar::DrawText(TASImage &img, Int_t x, Int_t y, const char *txt,
                         Int_t size, const char *color, const char *font,
                         TImage::EText3DType type, const char *path)
{
    const char *file = GetTTFontPath(font);
    if (!file)
        return;

    img.DrawText(x, y, txt, size, color, file, type);

    delete file;
}

#define __CINT__
#include <TTF.h>
TASImage *MCalendar::DrawTTF(Float_t x1, Float_t x2, Float_t y1, Float_t y2,
                             const char *text, Float_t sz, const char *font)
{
    const char *file = GetTTFontPath(font);
    if (!file)
        return NULL;

    const UInt_t size = TMath::Nint(sz*941);

    delete file;

    const Double_t x = (x1+x2)/2;
    const Double_t y = (y1+y2)/2;

    TASImage img;
    DrawDate(img, 0, 0, text, size, "#000000", font);

    const Double_t w = gPad->PixeltoX(img.GetWidth())/2;
    const Double_t h = gPad->PixeltoY(img.GetHeight())/2;

    return DrawImage(img, x-w, y-h, x+w, y+h);
}

/*
void MCalendar::Update()
{
    cout << "Update" << endl;
    fUpdate=kTRUE;
}

void MCalendar::Selected(TVirtualPad *pad,TObject *o,Int_t event)
{
    if (event==kButton1Down && pad)
    {
        pad->cd();
        fAlign = GetAlign();
        fActive = GetDate().Month()==fMonth;
        fEnv->GetAttLine(fActive?"Date.Active":"Date.Inactive", *this);
        fEnv->GetAttLine(Form(fActive?"Date.Active.%d":"Date.Inactive.%d", fDay), *this);
    }

    cout << "SetModel " << gPad << " " << pad << " " << o << " " << event << endl;
    //Picked(fSelectedPad, fSelected, fEvent);  // emit signal

}

void MCalendar::SetAttributes()
{
    if (!gPad)
        return;

    TAttLine::SetLineAttributes();

    cout << "gPad " << gPad << endl;

    fAlign  = GetAlign();
    fActive = GetDate().Month()==fMonth;
    fDay    = GetDate().WeekDay();

    fEnv->GetAttLine(fActive?"Date.Active":"Date.Inactive", *this);
    fEnv->GetAttLine(Form(fActive?"Date.Active.%d":"Date.Inactive.%d", fDay), *this);

    TQObject::Connect("TCanvas", "Selected(TVirtualPad*,TObject*,Int_t)",
                      "MCalendar", this, "Selected(TVirtualPad*,TObject*,Int_t)");

    TQObject::Connect("TGedPatternSelect", "PatternSelected(Style_t)", "MCalendar", this, "Update()");
    TQObject::Connect("TGColorSelect",     "ColorSelected(Pixel_t)",   "MCalendar", this, "Update()");
    TQObject::Connect("TGListBox",         "Selected(Int_t)",          "MCalendar", this, "Update()");
}
*/

