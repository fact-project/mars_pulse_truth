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
!   Author(s): Thomas Bretz, 1/2004 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Javier Rico,  4/2004 <mailto:jrico@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2008
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MRunIter
//
//  Use this to iterate over run-files giving only the run-number.
//
//  You need the specify the run-file root-directory (eg /data/MAGIC).
//
/////////////////////////////////////////////////////////////////////////////
#include "MRunIter.h"

#include <iostream>

#include <TMath.h>
#include <TSystem.h>

ClassImp(MRunIter);

using namespace std;

void MRunIter::SortRuns()
{
    const int n = GetNumRuns();

    TArrayI idx(n);
    TMath::Sort(n, fRuns.GetArray(), idx.GetArray(), kFALSE);

    for (int i=0; i<n; i++)
        idx[i] = fRuns[idx[i]];

    fRuns = idx;
}

Int_t MRunIter::AddRun(UInt_t run, const char *path)
{
    TString p(path);

    if (p.IsNull())
        p = fPath;

    if (p.IsNull())
        p = ".";

    // R. DeLosReyes and T. Bretz
    // Changes to read the DAQ numbering format. Changes takes place
    // between runs 35487 and 00035488 (2004_08_30)
    const char *fmt;
    if(fIsStandardFile)
      {
        fmt = "%05d-%s";
        fIsRawFile = kFALSE;
      }
    else
      fmt = run>35487 ? "*_%08d_*_%s" : "*_%05d_*_%s";

    MDirIter NextR;
    NextR.AddDirectory(p, Form(fmt, run,fIsRawFile?"*.raw":"*.root"), -1);

    const TString name(NextR());
    if (name.IsNull())
        return 0;

    AddRunNumber(run);

    return AddDirectory(gSystem->DirName(name), gSystem->BaseName(name), -1);
}

// --------------------------------------------------------------------------
//
// Add runs specified in a character chain with the format:
// run1,run2-run3,run4-run5,...
// e.g  if runrange="100,105-107,110-112,115" 
// runs 100,105,106,107,110,111,112 and 115 are included in the iterator list
//
Int_t MRunIter::AddRuns(const char* runrange, const char* path)
{ 
    const TString chcopy(runrange);
  
    Ssiz_t last=0;
    Int_t  lowrun=-1;
    UInt_t totdir=0;
  
    // loop over the elements of the character chain 
    for (Int_t i=0;i<chcopy.Length();i++)
    {
        // look for a digit, a '-' or a ',' 
        const char c=chcopy[i];
        if (! ((c>='0' && c<='9') || c=='-' || c==','))
            return totdir;
        
        // if '-' is found, save the previous number as initial run
        if (c=='-' && lowrun<0 && i>last)
        {
            const TSubString chrun = chcopy(last,i-last);
            lowrun=atoi(chrun.Data());	  	  
            last=i+1;
            continue;
        }
        // if ',' or the end of the string are found, save the previous run or run range
        if (c==',' && i>last)
        {
            const TSubString chrun = chcopy(last,i-last);
            const Int_t up=atoi(chrun.Data());
            if(lowrun>=0 && lowrun<=up)
                totdir+=AddRuns(lowrun,up,path);
            else if(lowrun<0)
                totdir+=AddRun(up,path);
            
            lowrun=-1;
            last=i+1;
            continue;
        }
  
        // if find two continous separators exit
        if ((c=='-' && i==last) || (c==',' && i==last))
            return totdir;
    }
  
    // save last run range
    const TSubString chrun = chcopy(last,chcopy.Length()-last);
    const Int_t upprun=atoi(chrun.Data());
    if(lowrun>=0 && lowrun<=upprun)
    {
        totdir+=AddRuns(lowrun,upprun,path);
        return totdir;
    }
  
    if(lowrun<0)
        totdir+=AddRun(upprun,path);
  
    return totdir;
}
