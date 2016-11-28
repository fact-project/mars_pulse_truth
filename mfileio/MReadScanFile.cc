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
!   Author(s): Thomas Bretz, 7/2008 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MReadFiles
//
/////////////////////////////////////////////////////////////////////////////
#include "MReadScanFile.h"

#include <stdlib.h> // atoi (Ubuntu 8.10)

#include "MLog.h"
#include "MLogManip.h"

#include "MParameters.h"
#include "MParList.h"

ClassImp(MReadScanFile);

using namespace std;

// --------------------------------------------------------------------------
//
// Check if str only contains '*'
//
Bool_t MReadScanFile::IsDelimiter(const TString &str) const
{
    const char *end = str.Data()+str.Length()+1;
    char const *ptr = str.Data();

    while (*ptr++=='*');

    if (ptr==end)
        return kTRUE;

    *fLog << err << "ERROR - Synatx error in line " << GetNumLine() << "... delimiter expected." << endl;
    *fLog << "'" << str << "'" << endl;

    return kFALSE;
}

// --------------------------------------------------------------------------
//
// Read a line, check if it is a delimiter line and check its length.
//
Bool_t MReadScanFile::ReadDelimiter()
{
    TString str;
    if (!ReadLine(str))
        return kFALSE;
    if (!IsDelimiter(str))
        return kFALSE;

    return kTRUE;

/*

    if (fLength<0)
        fLength = str.Length();

    if (fLength==str.Length())
        return kTRUE;

    *fLog << err << "ERROR - Delimiter line " << GetNumLine() << " has wrong length." << endl;
    return kFALSE;
    */
}

// --------------------------------------------------------------------------
//
// Read the header. Do some action if necessary. Leave the stream pointer
// behind the header.
//
Bool_t MReadScanFile::ReadHeader()
{
    if (!ReadDelimiter())
        return kFALSE;

    // FIXME: Check header here
    TString str;
    if (!ReadLine(str))
        return kFALSE;
    cout << str << endl;

    if (!ReadDelimiter())
        return kFALSE;

    return kTRUE;
}

Int_t MReadScanFile::ReadEvent()
{
    // If end-of-file go on reading next event in next file
    TString str;
    if (!ReadLine(str))
        return Process();
/*
    if (fLength!=str.Length())
    {
        *fLog << err << "ERROR - Line " << GetNumLine() << " has wrong length." << endl;
        return kERROR;
    }
 */
    // Line is not properly formatted by TTree::Scan
    if (str[0]!='*')
    {
        *fLog << err << "Line " << GetNumLine() << " doens't start with a '*'" << endl;
        return kERROR;
    }

    // Line is the delimiter at the end of the file
    // FIXME: Could we concatenate files here into one file?!
    if (str[1]=='*')
        return IsDelimiter(str) ? Process() : kERROR;

    // Read and interpete the data
    const char *end = str.Data()+str.Length();
    char const *ptr = str.Data()+1;

    TIter Next (&fList);
    MParameterD *d = 0;

    while (ptr && ptr<end)
    {
        // We have more columns in the file than conatiners
        d = static_cast<MParameterD*>(Next());
        if (!d)
        {
            *fLog << err << "Line " << GetNumLine() << " has too many fields." << endl;
            return kERROR;
        }

        // set interpreted value
        d->SetVal(atof(ptr));

        // go on with the next column
        ptr = strchr(ptr, '*')+1;
    }

    // we didn't find enough columns
    if (ptr<end)
    {
        *fLog << err << "Line " << GetNumLine() << " has too less fields." << endl;
        return kERROR;
    }

    // everything's ok
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Analyzse the header. Starts reading at the beginning of the file.
// Leaves the stream pointer at the end of the header.
// Add needed MParContainers to parlist or check for them.
//
Bool_t MReadScanFile::AnalyzeHeader(MParList &plist)
{
    fList.Clear("nodelete");

    fLength = -1;
    if (!ReadDelimiter())
        return kFALSE;
 
    TString line;
    if (!ReadLine(line))
        return kFALSE;
/*
    if (fLength!=str.Length())
    {
        *fLog << err << "ERROR - Line " << GetNumLine() << " has wrong length." << endl;
        return kFALSE;
    }
 */
 
    TObjArray *arr = line.Tokenize('*');

    TIter Next(arr);
    TObject *o = 0;
    while ((o=Next()))
    {
        TString name = TString(o->GetName()).Strip(TString::kBoth);
        name.ReplaceAll(".", "_");

        MParContainer *p = plist.FindCreateObj("MParameterD", name);
        if (!p)
        {
            delete arr;
            return kFALSE;
        }

        fList.AddLast(p);
    }

    delete arr;

    return ReadDelimiter();
}

Int_t MReadScanFile::PostProcess()
{
    fList.Clear("nodelete");
    return kTRUE;
}

UInt_t MReadScanFile::GetEntries()
{
    *fLog << err << "Unkown number of entries" << endl;
    return 0;
}
