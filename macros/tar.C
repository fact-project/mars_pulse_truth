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
!   Author(s): Thomas Bretz <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

///////////////////////////////////////////////////////////////////////////
//
// tar.C
// =====
//
// Service script. used to create the tar archive for an release. It makes
// sure that the correct files are included and the name of the archive is
// the name of the current directory (+.tar.gz)
//
///////////////////////////////////////////////////////////////////////////
void tar()
{
    TString dir = gSystem->pwd();

    Int_t slash = dir.Last('/');

    TString name = &dir[slash+1];

    if (!gSystem->AccessPathName("../"+name+".tar"))
    {
        cout << "File '../" << name << ".tar' existing." << endl;
        return;
    }

    if (!gSystem->AccessPathName("../"+name+".tar.gz"))
    {
        cout << "File '../" << name << ".tar.gz' existing." << endl;
        return;
    }

    gSystem->cd("..");

    TString cmd = "tar cvf "+name+".tar --exclude=Makefile.depend --exclude=Root --exclude=Tag --exclude=mtemp "+name+"/.rootrc "+name+"/*";

    cout << "Executing: " << cmd << endl;

    gSystem->Exec(cmd);
    gSystem->Exec("gzip -9 "+name+".tar");

    gSystem->cd(name);
}
